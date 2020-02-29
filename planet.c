#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "gpu.h"

#include "noise.h"
#include "fastmath.h"

#include "material.h"
#include "planet.h"

typedef struct
{
  unsigned char n;
  unsigned int seed;
  char name[10];
  unsigned char size;
  unsigned char level;
  Color color1;
  Color color2;
  signed char ax;
  signed char ay;
  signed char az;
  Palette pal;
} Planet;
Planet planet;

const char *vowels = "aeiouy";
void generate_name()
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
    planet.name[i] = c;
  }
  planet.name[i] = 0;
}

#define MAX_SIZE_TILES 9
#define MAX_SIZE (MAX_SIZE_TILES*GTILE_SIZE)
#define CENTER_X  (CENTER_X_TILE*GTILE_SIZE)
#define CENTER_Y  (CENTER_Y_TILE*GTILE_SIZE)
unsigned char CENTER_X_TILE = 29;
unsigned char CENTER_Y_TILE = 10;
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
void setpixel(signed char rx, signed char ry, signed char rz)
{
  unsigned int x = CENTER_X + rx;
  unsigned int y = CENTER_Y + ry;
  unsigned char tx = x/GTILE_SIZE;
  unsigned char ty = y/GTILE_SIZE;
  unsigned char px = x%GTILE_SIZE;
  unsigned char py = y%GTILE_SIZE;
  unsigned char mat, pal, pat, old;
  unsigned char i;
  unsigned char s = 12-log2(planet.size);
  bool sea, day;

  if (rz == -1)
  {
    mat = MAT_SPACE;
  }
  else
  {
    sea = noise(
      ((long)planet.seed<<16)+(rx<<s),
      ((long)planet.seed<<12)+(ry<<s),
      ((long)planet.seed<< 8)+(rz<<s)
    ) < (planet.level << 4);
    day = (
      planet.ax*rx +
      planet.ay*ry +
      planet.az*rz
    ) >= 0;
    mat = sea
     ? (day ? MAT_SEA_DAY  : MAT_SEA_NIGHT )
     : (day ? MAT_LAND_DAY : MAT_LAND_NIGHT);
  }

  gsetpos(tx, ty);
  pal = GCOL;
  pat = GNAM;

  if (pal == planet.pal+mat)
  {
    return;
  }

  if (pal == PAL_FREE)
  {
    GCOL = planet.pal+mat;
    return;
  }

  if (pal != planet.pal+MAT_MIXED)
  {
    old = GCOL-planet.pal;
    GCOL = planet.pal+MAT_MIXED;
    GNAM = pat = free_pattern--;
    gaddr(pat*GPAT_SIZE);
    old |= old<<4;
    for (i = 0; i < GPAT_SIZE; i++)
    {
      GPAT_INC = old;
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
void setpixel8(signed char x, signed char y, signed char z)
{
  setpixel( x  ,  y  , z);
  setpixel( y  ,  x  , z);
  setpixel(-x-1,  y  , z);
  setpixel(-y-1,  x  , z);
  setpixel( x  , -y-1, z);
  setpixel( y  , -x-1, z);
  setpixel(-x-1, -y-1, z);
  setpixel(-y-1, -x-1, z);
}

void render_line(unsigned char x, unsigned char y, signed int f)
{
  unsigned char s, sx, sy;

  unsigned char r = y+1;
  unsigned char z   = 0;
  unsigned char dfz = 0;
  unsigned char dfy = 2*r;

  /* fill outer space */
  s = GTILE_SIZE-(y%GTILE_SIZE);
  while (--s)
  {
    setpixel8(x, y+s, -1);
  }

  /* fill space in corners */
  if (x+1 >= y)
  {
    sx = s = GTILE_SIZE-(x%GTILE_SIZE);
    while (--sx)
    {
      sy = s;
      while (--sy)
      {
        setpixel8(x+sx, x+sy, -1);
      }
    }
  }

  while (y >= x)
  {
    while (f>=0)
    {
      setpixel8(x, y, z);
      if (y==0)
      {
        return;
      }
      y--;
      dfy -= 2;
      f -= dfy;
    }
    z++;
    dfz += 2;
    f += dfz+1;
  }
}

void draw_planet()
{
  unsigned char r   = planet.size;
  unsigned char dfx = 0;
  unsigned char dfy = 2*r;
  signed   int  f   = 2-r;
  unsigned char x   = 0;
  unsigned char y   = r-1;

  planet.pal = NUM_PALS + (NUM_MATS+1)*planet.n;
  Color colors[NUM_MATS] = {
    { .rgbi = 0x00 },
    { .rgbi = planet.color1.rgbi|0x03 },
    { .rgbi = planet.color1.rgbi&0xFC },
    { .rgbi = planet.color2.rgbi|0x03 },
    { .rgbi = planet.color2.rgbi&0xFC }
  };
  load_materials(colors, planet.pal);

  if(planet.n ==0 )
  {
    free_pattern = 255;
  }

  while (y >= x)
  {
    render_line(x, y, f);

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

  //printf("\n%d free\n", free_pattern-127);

}

void make_planet(unsigned char n, unsigned int seed)
{
  clear_panet_view();
  srand(seed);

planet.n = n;
  planet.seed = seed;
  planet.size = 1+RANDOM(MAX_SIZE);
  planet.level = RANDOM(100);
  planet.color1.rgbi = 1+RANDOM(254);
  planet.color2.rgbi = 1+RANDOM(254);
  planet.ax = -15+RANDOM(31);
  planet.ay = -15+RANDOM(31);
  planet.az = -15+RANDOM(31);
  generate_name();

  printf("Planet %u: %s\n", planet.seed, planet.name);
  printf("Topography: %u / %u\n", planet.size, planet.level);
  printf("Inclination: %d / %d / %d\n", planet.ax, planet.ay, planet.az);

  draw_planet();
}
