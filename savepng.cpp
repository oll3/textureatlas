#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <png.h>
#include <stdlib.h>
#include <string.h>

#include "savepng.h"

static int png_colortype_from_surface(SDL_Surface *surface)
{
  int colortype = PNG_COLOR_MASK_COLOR; /* grayscale not supported */

  if (surface->format->palette)
    colortype |= PNG_COLOR_MASK_PALETTE;
  else if (surface->format->Amask)
    colortype |= PNG_COLOR_MASK_ALPHA;

  return colortype;
}

void png_user_warn(png_structp ctx, png_const_charp str)
{
  (void)ctx;
  fprintf(stderr, "libpng: warning: %s\n", str);
}

void png_user_error(png_structp ctx, png_const_charp str)
{
  (void)ctx;
  fprintf(stderr, "libpng: error: %s\n", str);
}

/*
 * png_save()
 *
 * Save a SDL Surface as a png image file.
 * Returns 0 if successfully saved, else error.
 */
int PNG::save(SDL_Surface *surf, const char *filename)
{
  FILE *fp;
  png_structp png_ptr;
  png_infop info_ptr;
  int i, colortype;
  png_bytep *row_pointers;

  /* Opening output file */
  fp = fopen(filename, "wb");
  if (fp == NULL) {
    perror("fopen error");
    return -1;
  }

  /* Initializing png structures and callbacks */
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, png_user_error,
                                    png_user_warn);
  if (png_ptr == NULL) {
    printf("png_create_write_struct error!\n");
    return -1;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    printf("png_create_info_struct error!\n");
    exit(-1);
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    exit(-1);
  }

  png_init_io(png_ptr, fp);

  colortype = png_colortype_from_surface(surf);
  png_set_IHDR(png_ptr, info_ptr, surf->w, surf->h, 8, colortype,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  /* Writing the image */
  png_write_info(png_ptr, info_ptr);
  png_set_packing(png_ptr);

  row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * surf->h);
  for (i = 0; i < surf->h; i++)
    row_pointers[i] = (png_bytep)(Uint8 *)surf->pixels + i * surf->pitch;
  png_write_image(png_ptr, row_pointers);
  png_write_end(png_ptr, info_ptr);

  /* Cleaning out... */
  free(row_pointers);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);

  return 0;
}
