#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <list>
#include <argtable2.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Atlas.h"


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
const uint32_t rmask = 0xff000000;
const uint32_t gmask = 0x00ff0000;
const uint32_t bmask = 0x0000ff00;
const uint32_t amask = 0x000000ff;
#else
const uint32_t rmask = 0x000000ff;
const uint32_t gmask = 0x0000ff00;
const uint32_t bmask = 0x00ff0000;
const uint32_t amask = 0xff000000;
#endif


/*
 * Image atlas node
 */
class Image : public Atlas::NodeRect {
public:

  Image(SDL_Surface *surface) : NodeRect(surface->w, surface->h) {
    mSurface = surface;
  }

  SDL_Surface *getSurface() {
    return mSurface;
  }

  /* Used when sorting image list at size */
  static bool compare(Image *img1, Image *img2) {
    if (img1->getWidth() == img2->getWidth()) {
      return (img1->getHeight() > img2->getHeight());
    }
    else {
      return (img1->getWidth() > img2->getWidth());
    }
  }


private:
  SDL_Surface *mSurface;
};


/*
 * Draw a nodes surface to the atlas surface
 */
static void drawNode(int level, Atlas::Node *node, void *param) 
{  
  SDL_Surface *surface = (SDL_Surface *)param;
  SDL_Rect rect;
  Image *image = (Image *)node->getRect();
  rect.x = node->getLeft();
  rect.y = node->getTop();
  rect.w = node->getWidth();
  rect.h = node->getHeight();

  SDL_BlitSurface(image->getSurface(), NULL, surface, &rect);
}



/*
 * Try to fit images in the image list into a rectangle of the given dimension.
 *
 * Returns the atlas tree if successfully fitted all, else NULL.
 */
static Atlas::Node* tryCreate(int w, int h, std::list<Image *> &imageList)
{
  std::list<Image *>::iterator it;
  Atlas::Node *root = new Atlas::Node(0, 0, w, h);

  int i = 0;
  for (it = imageList.begin(); it != imageList.end(); it ++) {
    Image *image = *it;
    char *name = new char[20];
    sprintf(name, "IMG %d", i ++);
    if (root->insert(image) == NULL) {

      printf("Failed to insert image %d (w: %d, h: %d) in "
	     "surface (dimension w: %d, h: %d)\n", i, image->getWidth(), 
	     image->getHeight(), w, h);

      delete root;
      root = NULL;
      break;
    }
  }

  return root;
}


class Dimension {
public:

  Dimension(int w, int h) {
    mWidth = w;
    mHeight = h;
  }

  int mWidth, mHeight;
};



static int cmdLineParse(int argc, char *argv[], 
			std::list<Image *> *imageList, 
			const char *filename)
{
  int err = 0;

  struct arg_lit *help;
  struct arg_file *infile;
  struct arg_str *outname;
  struct arg_end *end;
 
  /* The command line arguments table */
  void *argtable[] ={
    help = arg_lit0("h", "help", "Display this help text."),
    outname = arg_str0("o", "out-format", "name", "Name of atlas to create."),
    infile = arg_filen(NULL, NULL, "file", 1, 1000, "Image files to include in atlas."),
    end = arg_end(20),
  };


  err = arg_parse(argc, argv, argtable);

  if (help->count) {

    /* Help command entered */

    fprintf(stdout, "Usage: %s", argv[0]);
    arg_print_syntax(stdout, argtable, "\n");
    fprintf(stdout, "Options:\n");
    arg_print_glossary_gnu(stdout, argtable);

    err = 1;
  }
  else if (err > 0) {

    /* Error(s) parsing command line */

    arg_print_errors(stdout, end, argv[0]);
    fprintf(stdout, "Usage: %s", argv[0]);
    arg_print_syntax(stdout, argtable, "\n");

    err = -1;
  }
  else {

    /* No errors parsing command line */
    int i;
    for (i = 0; i < infile->count; i ++) {
      SDL_Surface *surface = IMG_Load(infile->filename[i]);
      if (surface) {
	imageList->push_back(new Image(surface));
      }
      else {
	printf("Error loading image %s\n", infile->filename[i]);
	err = -1;
	break;
      }
    }

    if (!err) {
      imageList->sort(Image::compare);
    }
  }

  return err;
}


int main(int argc, char *argv[])
{
  int err = 0;
  unsigned int seed = time(NULL);
  //  seed = 1343398170;
  printf("Generating images with seed %u\n", seed);
  srand(seed);

  std::list<Image *> imageList;
  std::list<Image *>::iterator it;
  std::list<Dimension *> resolutionList;
  std::list<Dimension *>::iterator rit;

  char outname[512];

  err = cmdLineParse(argc, argv, &imageList, outname);

  if (!err) {

    /* 
     * Generate surface resolutions 
     */

    for (int h = 32; h < 8192*2; h *= 2) {
      for (int w = 32; w < 8192*2; w *= 2) {
	resolutionList.push_back(new Dimension(w, h));
      }
    }


    /* 
     * Sum the total number of pixels
     */

    unsigned long long numPixels = 0;
    for (it = imageList.begin(); it != imageList.end(); it ++) {
      Image *image = *it;
      numPixels += image->getWidth() * image->getHeight();
    }


    /*
     * Try to fit all images in the list surfaces with different 
     * resolutions, then choose to use the tree with the least waste 
     * of unused pixels.
     *
     * If there are more than one surface with the same amount of waste
     * we use the one with its height/width ratio closest to 1.0.
     */

    unsigned long long leastWaste = (unsigned long long)-1;
    Atlas::Node *bestRoot = NULL;
    Dimension *bestDimension = NULL;
    double bestRatio = 0.0;
  
    for (rit = resolutionList.begin(); rit != resolutionList.end(); rit ++) {
      Dimension *dim = *rit;
    
      Atlas::Node *root = tryCreate(dim->mWidth, dim->mHeight, imageList);

      if (root) {
      
	/* Got a tree, compare it to the best so far */

	unsigned long long  pixelWaste = (dim->mWidth * dim->mHeight) - numPixels;
	double ratio;
	if (dim->mHeight < dim->mWidth)
	  ratio = (double)dim->mHeight / (double)dim->mWidth;
	else
	  ratio = (double)dim->mWidth / (double)dim->mHeight;

	printf("Surface with dimension %d x %d created (ratio: %f, waste: %llu pixels)\n",
	       dim->mWidth, dim->mHeight, ratio, pixelWaste);

	if (pixelWaste >= 0) {
	  if ((pixelWaste < leastWaste) || (pixelWaste == leastWaste && ratio > bestRatio)) {
	    printf("Surface with dimension %d x %d best so far\n",
		   dim->mWidth, dim->mHeight);
	    
	    leastWaste = pixelWaste;
	    bestRoot = root;
	    bestDimension = dim;
	    bestRatio = ratio;
	  }
	}


	if (bestRoot) {

	  /* 
	   * Create the final image
	   */
	  
	  SDL_Surface *surface = SDL_CreateRGBSurface (0, bestDimension->mWidth, 
						       bestDimension->mHeight, 
						       32,
						       rmask, gmask, bmask, amask);
	  SDL_FillRect(surface, NULL, 0xffffffff);
	  bestRoot->poTraversal(0, drawNode, surface);
	  SDL_SaveBMP(surface, "atlas.bmp");
	}
      }
    }
  }

  return 0;
}

