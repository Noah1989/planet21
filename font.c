#include "stdio.h"

#include "gpu.h"

#include "font.h"

#ifdef FONT8X8
void load_font()
{
  unsigned char rows[8];
  unsigned char i, j, k, l, byte = 0;
  gaddr(' '/4*GPAT_SIZE);
  /* iterate blocks of 4 characters */
  for (i = 0; i < sizeof(font)/sizeof(font[0])/4; i++)
  {
    /* each character contains 4 double-rows of pixels */
    for (j = 0; j < 4; j++)
    {
      /* collect 8 pixel rows from the 4 characters in the block */
      for (k = 0; k < 4; k++)
      {
        rows[k] = font[i*4+k][2*j];
        rows[k+4] = font[i*4+k][2*j+1];
      }
      for (k = 0; k < 8; k++)
      {
        for (l = 0; l < 8; l++) {
          byte >>= 1;
          byte |= rows[l]&0x80;
          rows[l] <<= 1;
        }
        GPAT_INC = byte;
      }
    }
  }
}
#endif

#ifdef FONT8X16
void load_font()
{
  unsigned char rows[8];
  unsigned char i, j, k, l, byte = 0;
  gaddr(' '/2*GPAT_SIZE);
  /* iterate blocks of 2 characters */
  for (i = 0; i < sizeof(font)/sizeof(font[0])/2; i++)
  {
    /* each character contains 4 double-rows of pixels */
    for (j = 0; j < 4; j++)
    {
      /* collect 8 pixel rows from the 2 characters in the block */
      for (k = 0; k < 2; k++)
      {
        rows[k  ] = font[i*2+k][2*j];
        rows[k+2] = font[i*2+k][2*j+8];
        rows[k+4] = font[i*2+k][2*j+1];
        rows[k+6] = font[i*2+k][2*j+9];
      }
      for (k = 0; k < 8; k++)
      {
        for (l = 0; l < 8; l++) {
          byte >>= 1;
          byte |= rows[l]&0x80;
          rows[l] <<= 1;
        }
        GPAT_INC = byte;
      }
    }
  }
}
#endif
