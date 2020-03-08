#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "util.h"
#include "gpu.h"
#include "keys.h"
#include "text.h"
#include "font.h"
#include "colors.h"

#include "material.h"
#include "planet.h"
#include "sprite.h"
#include "physics.h"

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

int spx=160, spy=100;

void clear_screen()
{
  size_t i;
  gaddr(64*GPAT_SIZE);
  for (i = 0; i < GPAT_SIZE; i++)
  {
    GPAT_INC = 0;
  }
  gaddr(0);
  for (i = 0; i < GTILE_COUNT; i++)
  {
    GNAM = 64;
    GCOL_INC = PAL_FREE;
  }
}

int main()
{
  unsigned char key;
  unsigned int seed;
  float m1, m2;
  int spx=160, spy=100;
  int runx, runy;

  char p = 0;

  set_graphics_mode();
  clear_screen();

  TEXT_PALETTE_BASE = PAL_TEXT_0;
  load_text_border_patterns();
  load_font();
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
        draw_sprite(--spx-4, spy-4);
        break;
      case KEY_RIGHT:
        draw_sprite(++spx-4, spy-4);
        break;
      case KEY_UP:
        draw_sprite(spx-4, --spy-4);
        break;
      case KEY_DOWN:
        draw_sprite(spx-4, ++spy-4);
        break;

      case KEY_T:
        test(spx, spy);
        break;
      case KEY_R:
        runx=spx;
        runy=spy;
        while(!getkey())
        {
          run(&runx, &runy);
          draw_sprite(runx-4, runy-4);
        }
        draw_sprite(spx-4, spy-4);
        break;

      case KEY_S:
        draw_sprite(spx-4, spy-4);
        break;
      case KEY_H:
        hide_sprite();
        break;

      case KEY_P:
        printf("\f");
        load_text_palettes(&player_colors[p]);
        draw_text_border();
        printf("Player %d\nThis\nis a\nTest\n...", p);
        p = ++p%8;
        break;

      case KEY_N:
        clear_screen();
        draw_text_border();
        putchar('\f');
        CENTER_X_TILE = 10;
        make_planet(0, seed);
        m1 = planet.size;
        printf("\n");
        CENTER_X_TILE = 30;
        make_planet(1, seed+1);
        m2 = planet.size;
        printf("\n");
        init_physics(m1, m2, 10*8, 30*8, CENTER_Y_TILE*8);
        break;

      case KEY_Q:
        return 0;
    }
  }
}
