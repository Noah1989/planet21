#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "util.h"
#include "gpu.h"
#include "keys.h"

#include "text.h"

#define SCROLLX -2
#define SCROLLY -2
const union
{
  struct
  {
    char scrollx_l : 8;
    char scrolly_l : 8;
    char scrollx_u : 2;
    bool zoomx     : 1;
    bool           : 1;
    char scrolly_u : 2;
    bool zoomy     : 1;
    bool textmode  : 1;
  } values;
  char bytes[3];
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
  PAL_TEXT_BOT
} Palette;
void load_palettes()
{
  char text_bg = RGBI_TEXT_BG.rgbi;
  char text_fg = RGBI_TEXT_FG.rgbi;
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
char TEXT_PALETTE_TOP = PAL_TEXT_TOP;
char TEXT_PALETTE_BOT = PAL_TEXT_BOT;

typedef struct
{
  unsigned int id;
  char name[10];
  char size;
} Planet;

void generate_name(Planet *planet)
{
  char name_len = 2+rand()%8;
  bool vowel = rand()%2;
  for (char i = 0; i < name_len; i++)
  {
    if (vowel)
    {
      planet->name[i] = "aeiouy"[rand()%6];
    }
    else
    {
      planet->name[i] = 'a' + rand()%26;
    }
    if (i==0)
    {
      planet->name[i] += 'A'-'a';
    }
    vowel = !vowel;
  }
  planet->name[name_len] = 0;
}

#define MAX_SIZE_TILES 9
#define CENTER_X_TILE 29
#define CENTER_Y_TILE 15
#define MAX_SIZE (MAX_SIZE_TILES*GTILE_SIZE)
#define CENTER_X  (CENTER_X_TILE*GTILE_SIZE)
#define CENTER_Y  (CENTER_Y_TILE*GTILE_SIZE)
void clear_panet_view()
{
  for (char y = CENTER_Y_TILE-MAX_SIZE_TILES; y < CENTER_Y_TILE+MAX_SIZE_TILES; y++)
  {
    for (char x = CENTER_X_TILE-MAX_SIZE_TILES; x < CENTER_X_TILE+MAX_SIZE_TILES; x++)
    {
      gsetpos(x, y);
      GNAM = ' ';
      GCOL = 0;
    }
  }
}

char free_pattern;
void set_pixel(unsigned int x, unsigned int y)
{
  char tx = x/GTILE_SIZE;
  char ty = y/GTILE_SIZE;
  char px = x%GTILE_SIZE;
  char py = y%GTILE_SIZE;
  char pattern;

  gsetpos(tx, ty);
  GCOL = 0x1F;
  pattern = GNAM;
  if (pattern == ' ')
  {
    GNAM = pattern = free_pattern++;
    gaddr(pattern*GPAT_SIZE);
    for (int i = 0; i < GPAT_SIZE; i++) {
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

void draw_planet(Planet *planet)
{
  int r = planet->size;
  int x, y;
  int f = 2-r;
  int dfx = 0;
  int dfy = -2*r;

  free_pattern = 128;

  x = 0;
  y = r-1;
  while (y>=x)
  {
    for (int i=0; i<=y; i++)
    {
      set_pixel(CENTER_X+x, CENTER_Y+i);
    }

    if (f>=0)
    {
      for (int i=0; i<=x; i++)
      {
        set_pixel(CENTER_X+y, CENTER_Y+i);
      }
      y--;
      dfy += 2;
      f += dfy;
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
  planet.size = 1+rand()%MAX_SIZE;
  generate_name(&planet);

  printf("Planet %u\n", planet.id);
  printf("Name: %s\n", planet.name);
  printf("Size: %u\n", planet.size);

  draw_planet(&planet);
}

int main()
{
  char key;
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
