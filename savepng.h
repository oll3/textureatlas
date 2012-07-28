#ifndef _SAVEPNG_H_
#define _SAVEPNG_H_

#include <SDL/SDL.h>

/*
 * png_save()
 * 
 * Save a SDL Surface as a png image file.
 * Returns 0 if successfully saved, else error.
 */
int png_save(SDL_Surface *surf, const char *name);


#endif

