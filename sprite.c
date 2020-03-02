#include <stdio.h>
#include <stdbool.h>

#include "util.h"
#include "gpu.h"

unsigned char image[16][16] =
{
  {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

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
  .basepat = 0
};

void load_sprite_colors()
{
  for (size_t i = 0; i < 256; i++)
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

void draw_sprite(unsigned char x, unsigned char y)
{
  unsigned char dx, dy, ix, iy, n, px, py, tmp;

  if (sprite.visible) hide_sprite();

  sprite.tx = x/GTILE_SIZE;
  sprite.ty = y/GTILE_SIZE;
  dx = x%GTILE_SIZE;
  dy = y%GTILE_SIZE;

  gsetpos(sprite.tx, sprite.ty);
  sprite.origpat[0] = GNAM_INC;
  sprite.origpat[1] = GNAM_INC;
  gsetpos(sprite.tx, sprite.ty+1);
  sprite.origpat[2] = GNAM_INC;
  sprite.origpat[3] = GNAM_INC;

  for (iy=0; iy<8; iy++)
  {
    for (ix=0; ix<16; ix++)
    {
      n = 2*(iy/4) + ix/8;
      px = ix%8;
      py = iy%4;
      gaddr(sprite.origpat[n]*GPAT_SIZE + 8U*py + px);
      tmp = GPAT;
      if (image[(2U*iy-dy)%16U][(ix-dx)%16U])
      {
        tmp |= 0x0F;
      }
      if (image[(2U*iy+1U-dy)%16U][(ix-dx)%16U])
      {
        tmp |= 0xF0;
      }
      gaddr((sprite.basepat+n)*GPAT_SIZE + 8U*py + px);
      GPAT = tmp;
    }
  }

  gsetpos(sprite.tx, sprite.ty);
  GNAM_INC = sprite.basepat;
  GNAM_INC = sprite.basepat+1;
  gsetpos(sprite.tx, sprite.ty+1);
  GNAM_INC = sprite.basepat+2;
  GNAM_INC = sprite.basepat+3;

  sprite.visible = true;
}
