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
  PAL_FREE,
  PAL_TEXT_TOP,
  PAL_TEXT_BOT,
  NUM_PALS
} Palette;

void load_materials(Color colors[NUM_MATS], Palette base);
