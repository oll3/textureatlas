#ifndef _SAVEPNG_H_
#define _SAVEPNG_H_

#include <SDL2/SDL.h>

class PNG {

public:
  /*
   * save()
   *
   * Save a SDL Surface as a png image file.
   * Returns 0 if successfully saved, else error.
   */
  static int save(SDL_Surface *surf, const char *filename);
};

#endif
