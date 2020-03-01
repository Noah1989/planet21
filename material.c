#include <stdio.h>

#include "gpu.h"

#include "material.h"

void load_materials(Color colors[NUM_MATS], Palette base)
{
  unsigned char rgbi;
  for (Material mat = 0; mat < NUM_MATS; mat++)
  {
    rgbi = colors[mat].rgbi;
    gaddr((base+mat)*GPAL_SIZE);
    GPAL = -rgbi;
    gaddr((base+MAT_MIXED)*GPAL_SIZE + mat);
    GPAL = rgbi;
  }
}
