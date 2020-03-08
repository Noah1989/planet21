#include <stdio.h>

#include "gpu.h"
#include "colors.h"
#include "io.h"

#include "material.h"

void load_materials(Color mat_colors[NUM_MATS], Palette base_pal, Color *player_col)
{
  Color colors[8];
  for (size_t i = 1; i < 8; i++) {
    colors[i].rgbi = player_col->rgbi;
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

  for (Material mat = 0; mat < NUM_MATS; mat++)
  {
    colors[0].rgbi = mat_colors[mat].rgbi;
    gaddr((base_pal+mat)*GPAL_SIZE);
    BLKOUT(&colors[0].rgbi, GPAL_INC, 8);

    gaddr((base_pal+MAT_MIXED)*GPAL_SIZE + mat);
    GPAL = colors[0].rgbi;
  }
}
