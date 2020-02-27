#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "gpu.h"
#include "keys.h"

#include "text.h"

#define SCROLLX ((int)-2)
#define SCROLLY ((int)-2)
const union
{
  struct
  {
    unsigned char scrollx_l : 8;
    unsigned char scrolly_l : 8;
    unsigned char scrollx_u : 2;
    bool zoomx              : 1;
    bool                    : 1;
    unsigned char scrolly_u : 2;
    bool zoomy              : 1;
    bool textmode           : 1;
  } values;
  unsigned char bytes[3];
} settings =
{
  {
    .scrollx_l = LOWER(SCROLLX),
    .scrollx_u = UPPER(SCROLLX) & 0b11,
    .scrolly_l = LOWER(SCROLLY),
    .scrolly_u = UPPER(SCROLLY) & 0b11,
    .zoomx     = true,
    .zoomy     = true,
    .textmode  = false
  }
};
void set_graphics_mode()
{
  GSET_0 = settings.bytes[0];
  GSET_1 = settings.bytes[1];
  GSET_2 = settings.bytes[2];
}

typedef enum
{
  PAL_TEXT_TOP,
  PAL_TEXT_BOT,
  PAL_SPACE = 0x8F,
  PAL_INNER = 0x2F,
  PAL_OUTER = 0x4F,
  PAL_LAND  = 0x6F,
  PAL_SEA   = 0x1F,
  PAL_SHORE = 0x9E
} Palette;
void load_palettes()
{
  unsigned char text_bg = RGBI_TEXT_BG.rgbi;
  unsigned char text_fg = RGBI_TEXT_FG.rgbi;
  gaddr(PAL_TEXT_TOP * GPAL_SIZE);
  GPAL_INC = text_bg;
  GPAL_INC = text_fg;
  GPAL_INC = text_bg;
  GPAL_INC = text_fg;
  gaddr(PAL_TEXT_BOT * GPAL_SIZE);
  GPAL_INC = text_bg;
  GPAL_INC = text_bg;
  GPAL_INC = text_fg;
  GPAL_INC = text_fg;
}
unsigned char TEXT_PALETTE_TOP = PAL_TEXT_TOP;
unsigned char TEXT_PALETTE_BOT = PAL_TEXT_BOT;

typedef struct
{
  unsigned int id;
  char name[10];
  unsigned char size;
} Planet;

const char *vowels = "aeiouy";
void generate_name(Planet *planet)
{
  unsigned char name_len = 2+RANDOM(8);
  bool vowel = RANDOM(2);
  unsigned char i;
  char c;

  for (i = 0; i < name_len; i++)
  {
    if (vowel)
    {
      c = vowels[RANDOM(6)];
      vowel = false;
    }
    else
    {
      c = 'a' + RANDOM(26);
      vowel = !strchr(vowels, c);
    }
    if (i==0)
    {
      c += 'A'-'a';
    }
    planet->name[i] = c;
  }
  planet->name[i] = 0;
}

#define MAX_SIZE_TILES 9
#define CENTER_X_TILE 29
#define CENTER_Y_TILE 15
#define MAX_SIZE (MAX_SIZE_TILES*GTILE_SIZE)
#define CENTER_X  (CENTER_X_TILE*GTILE_SIZE)
#define CENTER_Y  (CENTER_Y_TILE*GTILE_SIZE)
void clear_panet_view()
{
  unsigned char x, y;
  for (y = CENTER_Y_TILE-MAX_SIZE_TILES; y < CENTER_Y_TILE+MAX_SIZE_TILES; y++)
  {
    for (x = CENTER_X_TILE-MAX_SIZE_TILES; x < CENTER_X_TILE+MAX_SIZE_TILES; x++)
    {
      gsetpos(x, y);
      GNAM = ' ';
      GCOL = PAL_SPACE;
    }
  }
}

unsigned char free_pattern;
void setpixel(signed char rx, signed char ry)
{
  unsigned int x = CENTER_X + rx;
  unsigned int y = CENTER_Y + ry;
  unsigned char tx = x/GTILE_SIZE;
  unsigned char ty = y/GTILE_SIZE;
  unsigned char px = x%GTILE_SIZE;
  unsigned char py = y%GTILE_SIZE;
  unsigned char pat;
  unsigned char i;

  gsetpos(tx, ty);
  pat = GNAM;
  if (pat == ' ')
  {
    GNAM = pat = free_pattern--;
    gaddr(pat*GPAT_SIZE);
    for (i = 0; i < GPAT_SIZE; i++)
    {
      GPAT_INC = 0;
    }
  }

  gaddr(pat*GPAT_SIZE + (py>>1)*GTILE_SIZE + px);

  if (py&0x01)
  {
    GPAT |= 0x10;
  }
  else
  {
    GPAT |= 0x01;
  }
}
void setpixel8(unsigned char rx, unsigned char ry)
{
  setpixel(rx, ry);
  setpixel(ry, rx);
  setpixel(-rx-1, ry);
  setpixel(-ry-1, rx);
  setpixel(rx, -ry-1);
  setpixel(ry, -rx-1);
  setpixel(-rx-1, -ry-1);
  setpixel(-ry-1, -rx-1);
}

Palette getpal(unsigned char rx, unsigned char ry)
{
  unsigned char rtx = rx/GTILE_SIZE;
  unsigned char rty = ry/GTILE_SIZE;
  gsetpos(CENTER_X_TILE+rtx, CENTER_Y_TILE+rty);
  return GCOL;
}

void setpal8(unsigned char rx, unsigned char ry, Palette pal)
{
  unsigned char rtx = rx/GTILE_SIZE;
  unsigned char rty = ry/GTILE_SIZE;
  gsetpos(CENTER_X_TILE+rtx, CENTER_Y_TILE+rty);
  GCOL = pal;
  gsetpos(CENTER_X_TILE+rty, CENTER_Y_TILE+rtx);
  GCOL = pal;
  gsetpos(CENTER_X_TILE-rtx-1, CENTER_Y_TILE+rty);
  GCOL = pal;
  gsetpos(CENTER_X_TILE-rty-1, CENTER_Y_TILE+rtx);
  GCOL = pal;
  gsetpos(CENTER_X_TILE+rtx, CENTER_Y_TILE-rty-1);
  GCOL = pal;
  gsetpos(CENTER_X_TILE+rty, CENTER_Y_TILE-rtx-1);
  GCOL = pal;
  gsetpos(CENTER_X_TILE-rtx-1, CENTER_Y_TILE-rty-1);
  GCOL = pal;
  gsetpos(CENTER_X_TILE-rty-1, CENTER_Y_TILE-rtx-1);
  GCOL = pal;
}

void render_line(unsigned char x, unsigned char y)
{
  unsigned char ix, iy;
  Palette pal;

  do
  {
    pal = getpal(x, y);
    if (pal != PAL_OUTER)
    {
      if (y%GTILE_SIZE == 7)
      {
        setpal8(x,y, PAL_INNER);        
        y-=7;
        continue;
      }
      setpal8(x, y, PAL_OUTER);
      for (ix = x&0xF8; ix < x; ix++)
      {
        for (iy = (y&0xF8)+GTILE_SIZE-1; iy >= (y&0xF8) && iy >= ix; iy--)
        {
          setpixel8(ix, iy);
          if (iy == 0)
          {
            break;
          }
        }
      }
    }
    setpixel8(x, y);
  } while (x , y--);
}

void draw_planet(Planet *planet)
{
  unsigned char r   = planet->size;
  unsigned char dfx = 0;
  unsigned char dfy = 2*r;
  signed   char f   = 2-r;
  unsigned char x   = 0;
  unsigned char y   = r-1;

  free_pattern = 255;

  while (y >= x)
  {
    render_line(x, y);

    if (f>=0)
    {
      if (y==0)
      {
        break;
      }
      y--;
      dfy -= 2;
      f -= dfy;
    }
    x++;
    dfx += 2;
    f += dfx+1;
  }

  printf("\n\%d free\n", free_pattern-127);

}

void make_planet(unsigned int seed)
{
  Planet planet;

  clear_panet_view();
  srand(seed);

  planet.id   = seed;
  planet.size = 1+RANDOM(MAX_SIZE);
  generate_name(&planet);

  printf("Planet %u\n", planet.id);
  printf("Name: %s\n", planet.name);
  printf("Size: %u\n", planet.size);

  draw_planet(&planet);
}

int main()
{
  unsigned char key;
  unsigned int seed;

  set_graphics_mode();
  load_palettes();

  while (true)
  {
    printf("\nN: new / Q: quit\n");

    seed = rand();
    while(!(key = getkey()))
    {
      seed++;
    }

    switch (key)
    {
      case KEY_N:
        putchar('\f');
        make_planet(seed);
        break;
      case KEY_Q:
        return 0;
    }
  }
}
