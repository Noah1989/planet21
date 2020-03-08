typedef enum
{
  MAT_SPACE,
  MAT_LAND_DAY,
  MAT_LAND_NIGHT,
  MAT_SEA_DAY,
  MAT_SEA_NIGHT,
  NUM_MATS,
  MAT_MIXED = NUM_MATS
} Material;

typedef enum
{
  PAL_TEXT_0,
  PAL_TEXT_1,
  PAL_TEXT_2,
  PAL_TEXT_3,
  PAL_TEXT_BORDER,
  PAL_FREE,
  NUM_PALS
} Palette;

void load_materials(Color colors[NUM_MATS], Palette base_pal, Color *player_col);
