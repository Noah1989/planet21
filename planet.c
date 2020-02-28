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
  MAT_SPACE,
  MAT_LAND,
  MAT_SEA,
  NUM_MATS
} Material;
Color mat_colors[NUM_MATS] =
{
  {{.r=0, .g=0, .b=0, .i=0}},
  {{.r=3, .g=2, .b=1, .i=1}},
  {{.r=0, .g=1, .b=3, .i=1}}
};
typedef enum
{
  PAL_SPACE = MAT_SPACE,
  PAL_LAND  = MAT_LAND,
  PAL_SEA   = MAT_SEA,
  PAL_FREE  = NUM_MATS,
  PAL_MIXED,
  PAL_TEXT_TOP,
  PAL_TEXT_BOT,
  NUM_PALS
} Palette;

void load_materials()
{
  unsigned char rgbi;
  for (Material mat = MAT_SPACE; mat < NUM_MATS; mat++)
  {
    rgbi = mat_colors[mat].rgbi;
    gaddr(mat*GPAL_SIZE);
    GPAL = rgbi;
    gaddr(PAL_MIXED*GPAL_SIZE + mat);
    GPAL = rgbi+1;
  }
}

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
      GCOL = PAL_FREE;
    }
  }
}

unsigned char free_pattern;
void setpixel(signed char rx, signed char ry, Material mat)
{
  unsigned int x = CENTER_X + rx;
  unsigned int y = CENTER_Y + ry;
  unsigned char tx = x/GTILE_SIZE;
  unsigned char ty = y/GTILE_SIZE;
  unsigned char px = x%GTILE_SIZE;
  unsigned char py = y%GTILE_SIZE;
  unsigned char pal;
  unsigned char pat;
  unsigned char old;
  unsigned char i;

  gsetpos(tx, ty);
  pal = GCOL;
  pat = GNAM;

  if (pal == mat)
  {
    return;
  }

  if (pal == PAL_FREE)
  {
    GCOL = mat;
    return;
  }

  if (pal != PAL_MIXED)
  {
    old = GCOL;
    GCOL = PAL_MIXED;
    GNAM = pat = free_pattern--;
    gaddr(pat*GPAT_SIZE);
    for (i = 0; i < GPAT_SIZE; i++)
    {
      GPAT_INC = (old << 4) | old;
    }
  }

  gaddr(pat*GPAT_SIZE + (py>>1)*GTILE_SIZE + px);

  if (py&0x01)
  {
    GPAT = GPAT&0x0F | (mat << 4);
  }
  else
  {
    GPAT = GPAT&0xF0 | mat;
  }
}
void setpixel8(unsigned char rx, unsigned char ry, Material mat)
{
  setpixel( rx  ,  ry  , mat);
  setpixel( ry  ,  rx  , mat);
  setpixel(-rx-1,  ry  , mat);
  setpixel(-ry-1,  rx  , mat);
  setpixel( rx  , -ry-1, mat);
  setpixel( ry  , -rx-1, mat);
  setpixel(-rx-1, -ry-1, mat);
  setpixel(-ry-1, -rx-1, mat);
}

void render_line(unsigned char x, unsigned char y)
{
  unsigned char s, sx, sy;

  s = GTILE_SIZE-(y%GTILE_SIZE);
  while (--s)
  {
    setpixel8(x, y+s, MAT_SPACE);
  }

  if (x+1 >= y)
  {
    sx = s = GTILE_SIZE-(x%GTILE_SIZE);
    while (--sx)
    {
      sy = s;
      while (--sy)
      {
        setpixel8(x+sx, x+sy, MAT_SPACE);
      }
    }
  }

  do
  {
    if(x+y>42)
    setpixel8(x, y, MAT_LAND);
    else
    setpixel8(x, y, MAT_SEA);
  } while (y--);

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


unsigned char TEXT_PALETTE_TOP = PAL_TEXT_TOP;
unsigned char TEXT_PALETTE_BOT = PAL_TEXT_BOT;
Color text_bg  = {{.r=0, .g=0, .b=0, .i=0}};
Color text_fg  = {{.r=1, .g=3, .b=0, .i=1}};

int main()
{
  unsigned char key;
  unsigned int seed;

  set_graphics_mode();
  load_text_palettes(&text_bg, &text_fg);
  load_materials();

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
