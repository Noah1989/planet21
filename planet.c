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
  PAL_SPACE,
  PAL_OUTER,
  PAL_INNER
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
void set_pixel(signed char rx, signed char ry)
{
  unsigned int x = CENTER_X + rx;
  unsigned int y = CENTER_Y + ry;
  unsigned char tx = x/GTILE_SIZE;
  unsigned char ty = y/GTILE_SIZE;
  unsigned char px = x%GTILE_SIZE;
  unsigned char py = y%GTILE_SIZE;
  unsigned char pattern;
  unsigned char i;

  gsetpos(tx, ty);
  GCOL = 0x1F;
  pattern = GNAM;
  if (pattern == ' ')
  {
    GNAM = pattern = free_pattern++;
    gaddr(pattern*GPAT_SIZE);
    for (i = 0; i < GPAT_SIZE; i++) {
      GPAT_INC = 0;
    }
  }

  gaddr(pattern*GPAT_SIZE + (py>>1)*GTILE_SIZE + px);

  if (py&0x01)
  {
    GPAT |= 0x10;
  }
  else
  {
    GPAT |= 0x01;
  }

}

void render_line(unsigned char x, unsigned char y)
{
  do
  {
    if(y%8==7)
    {
      gsetpos(((unsigned int)(CENTER_X+x)), 0);
      y-=7;
    }
    else
    {
      set_pixel(x, y);
    }
  }
  while (y--);
}

void draw_planet(Planet *planet)
{
  unsigned char r = planet->size;
  unsigned char x, y;
  unsigned char dfx = 0;
  unsigned char dfy = 2*r;
  signed   char f   = 2-r;

  free_pattern = 128;

  x = 0;
  y = r-1;
  while (y>=x)
  {
    render_line(x, y);

    if (f>=0)
    {
      render_line(y, x);

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
