void make_planet(unsigned char n, unsigned int seed);

extern unsigned char CENTER_X_TILE;
extern unsigned char CENTER_Y_TILE;

typedef struct
{
  unsigned char n;
  unsigned int seed;
  char name[10];
  unsigned char size;
  signed char level;
  Color color1;
  Color color2;
  signed char ax;
  signed char ay;
  signed char az;
  Palette pal;
} Planet;
extern Planet planet;
