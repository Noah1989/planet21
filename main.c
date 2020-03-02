#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "util.h"
#include "gpu.h"
#include "keys.h"
#include "text.h"

#include "material.h"
#include "planet.h"
#include "sprite.h"

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

Color text_bg = {{.r=0, .g=0, .b=0, .i=0}};
Color text_fg = {{.r=1, .g=3, .b=0, .i=1}};

unsigned char spx=160, spy=100;

int main()
{
  unsigned char key;
  unsigned int seed;

  set_graphics_mode();

  TEXT_PALETTE_TOP = PAL_TEXT_TOP;
  TEXT_PALETTE_BOT = PAL_TEXT_BOT;
  load_text_palettes(&text_bg, &text_fg);

  load_sprite_colors();

  putchar('\f');
  while (true)
  {
    seed = rand();
    while(!(key = getkey()))
    {
      seed++;
    }

    switch (key)
    {
      case KEY_LEFT:
        draw_sprite(--spx, spy);
        break;
      case KEY_RIGHT:
        draw_sprite(++spx, spy);
        break;
      case KEY_UP:
        draw_sprite(spx, --spy);
        break;
      case KEY_DOWN:
        draw_sprite(spx, ++spy);
        break;

      case KEY_S:
        draw_sprite(spx, spy);
        break;
      case KEY_H:
        hide_sprite();
        break;

      case KEY_N:
        putchar('\f');
        CENTER_X_TILE = 30;
        make_planet(0, seed);
        printf("\n");
        CENTER_X_TILE = 10;
        make_planet(1, seed+1);
        printf("\n");
        break;

      case KEY_Q:
        return 0;
    }
  }
}
