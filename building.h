typedef enum
{
  B_HQ_LAND,
  B_HQ_SEA,
  B_MINE_LAND,
  B_MINE_SEA,
  B_POWER_SOLAR,
  B_POWER_GEOTHERM,
  B_POWER_TIDAL,
  B_POWER_NUCLEAR,
  B_LAUNCH,
  NUM_BUILDINGS
} Building;

void load_building_patterns();
