#include <stdio.h>

#include "util.h"
#include "gpu.h"
#include "io.h"

#include "colors.h"
#include "font.h"
#include "text.h"

unsigned char TEXT_PALETTE_BASE;

void load_text_palettes(Color *base)
{
  unsigned char n, m, i, j;

  Color colors[8];
  colors[0].rgbi = 0x00;
  for (size_t i = 1; i < 8; i++) {
    colors[i].rgbi = base->rgbi;
  }
  color_dark(&colors[1]);
  color_dark(&colors[1]);
  color_dark(&colors[2]);
  color_bright(&colors[4]);
  color_bright(&colors[5]);
  color_bright(&colors[5]);
  color_bright(&colors[6]);
  color_bright(&colors[6]);
  color_bright(&colors[6]);
  colors[7].rgbi^=0xFF;

  gaddr(TEXT_PALETTE_BASE * GPAL_SIZE);
  for (n=8, m=1; n > 0; n/=2, m*=2)
  {
    for (i = 0; i < n; i++) {
      for (j = 0; j < m; j++) {
        GPAL_INC = colors[1].rgbi;
      }
      for (j = 0; j < m; j++) {
        GPAL_INC = colors[5].rgbi;
      }
    }
  }
  BLKOUT(&colors[0].rgbi, GPAL_INC, sizeof(colors));
}

unsigned char border_patterns[8][32] =
{
  {
    0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x00, 0x44, 0x33, 0x33, 0x13, 0x13, 0x13, 0x13,
    0x00, 0x44, 0x33, 0x11, 0x61, 0x16, 0x16, 0x16,
    0x00, 0x44, 0x33, 0x11, 0x66, 0x11, 0x11, 0x11
  },
  {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
  },
  {
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00,
    0x13, 0x13, 0x13, 0x13, 0x33, 0x33, 0x22, 0x00,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x33, 0x22, 0x00,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x33, 0x22, 0x00,
  },
  {
    0x00, 0x44, 0x33, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x44, 0x33, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x44, 0x33, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x44, 0x33, 0x11, 0x11, 0x11, 0x11, 0x11
  },
  {
    0x11, 0x11, 0x11, 0x11, 0x11, 0x33, 0x22, 0x00,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x33, 0x22, 0x00,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x33, 0x22, 0x00,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x33, 0x22, 0x00,
  },
  {
    0x00, 0x44, 0x33, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x44, 0x33, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x44, 0x33, 0x33, 0x31, 0x31, 0x31, 0x31,
    0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02
  },
  {
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02
  },
  {
    0x11, 0x11, 0x11, 0x00, 0x11, 0x33, 0x22, 0x00,
    0x01, 0x01, 0x01, 0x10, 0x11, 0x33, 0x22, 0x00,
    0x31, 0x31, 0x31, 0x31, 0x33, 0x33, 0x22, 0x00,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00
  },
};
void load_text_border_patterns()
{
  gaddr(0);
  BLKOUT(border_patterns[0], GPAT_INC, sizeof(border_patterns));
}

void print(unsigned char line, unsigned char column, unsigned char c)
{
#ifdef FONT8X8
  gsetpos(column, line);
  GCOL = TEXT_PALETTE_BASE+c%4;
  GNAM = c/4;
#endif
#ifdef FONT8X16
  gsetpos(column, 2*line+TEXT_ODD);
  GCOL = TEXT_PALETTE_BASE+c%2;
  GNAM = c/2;
  gsetpos(column, 2*line+TEXT_ODD+1);
  GCOL = TEXT_PALETTE_BASE+c%2+2;
  GNAM = c/2;
#endif
}
char read(unsigned char line, unsigned char column) {
#ifdef FONT8X8
  gsetpos(column, line);
  return GNAM*4+GCOL-TEXT_PALETTE_BASE;
#endif
#ifdef FONT8X16
  gsetpos(column, 2*line+TEXT_ODD);
  return GNAM*2+GCOL-TEXT_PALETTE_BASE;
#endif
}

#if __SDCC_VERSION_MAJOR >= 3 && __SDCC_VERSION_MINOR >= 7
int putchar(int c)
#else
void putchar(char c)
#endif
{
  static unsigned char column = TEXT_LEFT;
  static unsigned char line = TEXT_TOP;

  switch (c)
  {
    case '\n':
      column = TEXT_LEFT;
      line++;
      break;

    case '\f':
      for (line = TEXT_TOP; line <= TEXT_BOTTOM; line++)
      {
        for (column = TEXT_LEFT; column <= TEXT_RIGHT; column++)
        {
          print(line, column, ' ');
        }
      }
      line = TEXT_TOP;
      column = TEXT_LEFT;
      break;

    default:
      print(line, column, c);
      column++;
  }

  if (column > TEXT_RIGHT)
  {
    column = TEXT_LEFT;
    line++;
  }
  if (line > TEXT_BOTTOM)
  {
    for (line = TEXT_TOP; line < TEXT_BOTTOM; line++)
    {
      for (column = TEXT_LEFT; column <= TEXT_RIGHT; column++)
      {
        print(line, column, read(line+1, column));
      }
    }
    for (column = TEXT_LEFT; column <= TEXT_RIGHT; column++)
    {
      print(line, column, ' ');
    }
    column = TEXT_LEFT;
  }

#if __SDCC_VERSION_MAJOR >= 3 && __SDCC_VERSION_MINOR >= 7
  return c;
#endif
}

#define PAL_BORDER (TEXT_PALETTE_BASE+4)
void draw_text_border() {
  int i;

  gsetpos(TEXT_LEFT-1, TEXT_TOP*2-1+TEXT_ODD);
  GCOL = PAL_BORDER;
  GNAM_INC = 0;
  for (i = 0; i < TEXT_WIDTH; i++)
  {
    GCOL = PAL_BORDER;
    GNAM_INC = 1;
  }
  GCOL = PAL_BORDER;
  GNAM_INC = 2;

  for (i = 0; i < TEXT_HEIGHT*2; i++)
  {
    gsetpos(TEXT_LEFT-1, TEXT_TOP*2+i+TEXT_ODD);
    GCOL = PAL_BORDER;
    GNAM_INC = 3;
    gsetpos(TEXT_RIGHT+1, TEXT_TOP*2+i+TEXT_ODD);
    GCOL = PAL_BORDER;
    GNAM_INC = 4;
  }

  gsetpos(TEXT_LEFT-1, (TEXT_BOTTOM+1)*2+TEXT_ODD);
  GCOL = PAL_BORDER;
  GNAM_INC = 5;
  for (i = 0; i < TEXT_WIDTH; i++)
  {
    GCOL = PAL_BORDER;
    GNAM_INC = 6;
  }
  GCOL = PAL_BORDER;
  GNAM_INC = 7;

}
