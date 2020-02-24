#define __port __sfr
__port __at 0xB0 GSET_0;
__port __at 0xB1 GSET_1;
__port __at 0xB2 GSET_2;
__port __at 0xB3 GADDR_L;
__port __at 0xB4 GADDR_H;
__port __at 0xB8 GNAM;
__port __at 0xB9 GCOL;
__port __at 0xBA GPAT;
__port __at 0xBB GPAL;
__port __at 0xBC GNAM_INC;
__port __at 0xBD GCOL_INC;
__port __at 0xBE GPAT_INC;
__port __at 0xBF GPAL_INC;

#define GPAL_SIZE 16
#define GPAT_SIZE 32

#define GTILE_SIZE 8
#define GTILE_COLS 128
#define GTILE_ROWS 64
#define GTILE_COUNT (GTILE_COLS * GTILE_ROWS)

void gaddr(size_t addr);
void gsetpos(unsigned char x, unsigned char y);

typedef union
{
  struct
  {
    unsigned char i: 2;
    unsigned char b: 2;
    unsigned char g: 2;
    unsigned char r: 2;
  } comp;
  unsigned char rgbi;
} Rgbi;
