#include <stdio.h>
#include <stdbool.h>

#include "util.h"
#include "gpu.h"
#include "io.h"

#include "material.h"

#include "sprite.h"

unsigned char image[11][9] =
{
  { 0,  0,  0,  0,  0,  0,  0,  0,  0},
  { 0,  0,  0, 15, 15, 15,  0,  0,  0},
  { 0,  0,  0,  0, 15,  0,  0,  0,  0},
  { 0,  0,  0,  0, 15,  0,  0,  0,  0},
  {15,  0,  0,  0, 15,  0,  0,  0, 15},
  {15, 15, 15, 15, 15, 15, 15, 15, 15},
  {15,  0,  0,  0, 15,  0,  0,  0, 15},
  { 0,  0,  0,  0, 15,  0,  0,  0,  0},
  { 0,  0,  0,  0, 15,  0,  0,  0,  0},
  { 0,  0,  0, 15, 15, 15,  0,  0,  0},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0}
};

unsigned char buf[4][4][8];

typedef struct
{
  bool visible;
  unsigned char basepat;
  unsigned char tx;
  unsigned char ty;
  unsigned char origpat[4];
} Sprite;

Sprite sprite =
{
  .visible = false,
  .basepat = 65
};

void load_sprite_colors()
{
  for (size_t i = PAL_FREE; i < 256; i++)
  {
    gaddr(i*GPAL_SIZE + 0x0F);
    GPAL = 0xFF;
  }
}

void hide_sprite()
{
  gsetpos(sprite.tx, sprite.ty);
  GNAM_INC = sprite.origpat[0];
  GNAM_INC = sprite.origpat[1];
  gsetpos(sprite.tx, sprite.ty+1);
  GNAM_INC = sprite.origpat[2];
  GNAM_INC = sprite.origpat[3];
  sprite.visible = false;
}

void draw_sprite(int x, int y)
{
  unsigned char ox, oy;
  unsigned char op[4];
  unsigned char n, ix, iy;
    signed char dx, dy;
  unsigned char *ptr;
  unsigned char *pitop;
  unsigned char *pibot;

  ox = sprite.tx;
  oy = sprite.ty;

  sprite.tx = x/GTILE_SIZE;
  sprite.ty = y/GTILE_SIZE;
  dx = x%GTILE_SIZE;
  dy = y%GTILE_SIZE;

  gsetpos(sprite.tx, sprite.ty);
  op[0] = GNAM_INC;
  op[1] = GNAM_INC;
  gsetpos(sprite.tx, sprite.ty+1);
  op[2] = GNAM_INC;
  op[3] = GNAM_INC;

  for (n = 0; n < 4; n++) {
    if (op[n] >= sprite.basepat && op[n] < sprite.basepat+4)
    {
      op[n] = sprite.origpat[op[n]-sprite.basepat];
    }
  }

  ptr = buf[0][0];
  for (n = 0; n < 4; n++)
  {
    gaddr(op[n]*GPAT_SIZE);
    ptr = BLKIN(ptr, GPAT_INC, GPAT_SIZE);
  }

  ptr = &buf[0][dy/2U][dx];
  pitop = image[~dy&1];
  pibot = pitop+9;
  dx = 8-dx;
  dy = 4-(dy/2);
  iy = 5;
  while (1)
  {
    ix = 9;
    while (1)
    {
      *ptr |= (*pitop) | ((*pibot)<<4);
      pitop++;
      pibot++;
    if (--ix == 0) break;
      ptr++;
      if (--dx == 0) ptr+=32-8;
    }
  if (--iy == 0) break;
    dx+=8;
    ptr-=32-8;
    pitop+=9;
    pibot+=9;
    if (--dy == 0) ptr+=32;
  }

  gaddr(sprite.basepat*GPAT_SIZE);
  BLKOUT(buf[0][0], GPAT_INC, GPAT_SIZE*4);

  if (sprite.visible)
  {
    gsetpos(ox, oy);
    GNAM_INC = sprite.origpat[0];
    GNAM_INC = sprite.origpat[1];
    gsetpos(ox, oy+1);
    GNAM_INC = sprite.origpat[2];
    GNAM_INC = sprite.origpat[3];
  }

  gsetpos(sprite.tx, sprite.ty);
  GNAM_INC = sprite.basepat;
  GNAM_INC = sprite.basepat+1;
  gsetpos(sprite.tx, sprite.ty+1);
  GNAM_INC = sprite.basepat+2;
  GNAM_INC = sprite.basepat+3;

  for (n = 0; n < 4; n++) {
    sprite.origpat[n] = op[n];
  }

  sprite.visible = true;
}
