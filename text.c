#include <stdio.h>

#include "util.h"
#include "gpu.h"

#include "text.h"

const Rgbi RGBI_TEXT_BG = {{.r=0, .g=0, .b=0, .i=0}};
const Rgbi RGBI_TEXT_FG = {{.r=1, .g=3, .b=0, .i=1}};

void print(char line, char column, char c)
{
  gsetpos(column, line*2);
  GCOL = TEXT_PALETTE_TOP;
  GNAM = c;
  gsetpos(column, line*2 + 1);
  GCOL = TEXT_PALETTE_BOT;
  GNAM = c;
}
char read(char line, char column) {
  gsetpos(column, line*2);
  return GNAM;
}

int putchar(int c)
{
  static char column = TEXT_LEFT;
  static char line = TEXT_TOP;

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
    for (column = TEXT_LEFT; column <= TEXT_RIGHT; column++) {
      print(line, column, ' ');
    }
    column = TEXT_LEFT;
  }

  return c;
}