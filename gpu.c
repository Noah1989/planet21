#include <stdio.h>

#include "util.h"

#include "gpu.h"

void gaddr(size_t addr)
{
  GADDR_L = LOWER(addr);
  GADDR_H = UPPER(addr);
}

void gsetpos(char x, char y)
{
  gaddr((y)*GTILE_COLS + (x));
}
