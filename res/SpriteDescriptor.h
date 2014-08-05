#ifndef _SPRITE_DESCRIPTOR_H_
#define _SPRITE_DESCRIPTOR_H_

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SpriteMapDescriptor;

typedef struct SpriteDescriptor {

  /* Offset/Index in map */
  const int offset;

  /* Sprite Name (from file) */
  const char *name;

  /* Sprite Coordinates (in integers) */
  int left, top, right, bottom;

  /* Sprite Size (integers) */
  int width, height;

} SpriteDescriptor;



typedef struct SpriteMapDescriptor {
  
  /* Sprite Map Name */
  const char *name;

  /* Sprite Map Image File Name */
  const char *imageFileName;

  /* Sprite Map size (pixels) */
  const int width, height;  

  /* Number of sprites in sprite map */
  const unsigned int numSprites;

  /* Pointer to array of sprite descriptors */
  const SpriteDescriptor sprites[];

} SpriteMapDescriptor;


/*
  Find a sprite by name from sprite map
*/
static inline const SpriteDescriptor *spritemap_find_sprite(const SpriteMapDescriptor *spriteMap, const char *name)
{
  int i;
  for (i = 0; i < spriteMap->numSprites; i ++) {
    if (strcmp(spriteMap->sprites[i].name, name) == 0)
      return &spriteMap->sprites[i];
  }
  return NULL;
}


/*
  Get a sprite by index from sprite map
*/
static inline const SpriteDescriptor *spritemap_get_sprite(const SpriteMapDescriptor *spriteMap, unsigned int offset)
{
  if (offset >= spriteMap->numSprites)
    return NULL;
  return &spriteMap->sprites[offset];
}


/*
  Get sprite map from sprite descriptor
*/
static inline const struct SpriteMapDescriptor *spritemap_get_map(const SpriteDescriptor *sprite)
{
  return (struct SpriteMapDescriptor *)(((unsigned char*)&sprite[-sprite->offset]) -
					sizeof(struct SpriteMapDescriptor));
}


/*
  Convert from pixel position to a scaled value (0.0 - 1.0) where 0 is pixel 0 and 1.0 is map width
*/
static inline float spritemap_get_fxpos(const SpriteMapDescriptor *spriteMap, int x)
{
  return (((float)x / (float)spriteMap->width));
}


/*
  Convert from pixel position to a scaled value (0.0 - 1.0) where 0 is pixel 0 and 1.0 is map height
*/
static inline float spritemap_get_fypos(const SpriteMapDescriptor *spriteMap, int y)
{
  return (((float)y / (float)spriteMap->height));
}

#ifdef __cplusplus
}
#endif

#endif
