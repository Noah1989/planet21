#include <math.h>

#include "fastmath.h"

#include "noise.h"

const unsigned char permutation[256] =
{
151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
 57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
 74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
 60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
 65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
 52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
 81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180
};
#define p(x) permutation[(x&255)]

const unsigned int flookup[257] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 6, 7, 9, 10, 12, 14, 17, 19, 22,
  25, 29, 32, 36, 40, 45, 49, 54, 60, 65, 71, 77, 84, 91, 98, 105, 113, 121,
  130, 139, 148, 158, 167, 178, 188, 199, 211, 222, 234, 247, 259, 273, 286,
  300, 314, 329, 344, 359, 374, 390, 407, 424, 441, 458, 476, 494, 512, 531,
  550, 570, 589, 609, 630, 651, 672, 693, 715, 737, 759, 782, 805, 828, 851,
  875, 899, 923, 948, 973, 998, 1023, 1049, 1074, 1100, 1127, 1153, 1180, 1207,
  1234, 1261, 1289, 1316, 1344, 1372, 1400, 1429, 1457, 1486, 1515, 1543, 1572,
  1602, 1631, 1660, 1690, 1719, 1749, 1778, 1808, 1838, 1868, 1898, 1928, 1958,
  1988, 2018, 2048, 2077, 2107, 2137, 2167, 2197, 2227, 2257, 2287, 2317, 2346,
  2376, 2405, 2435, 2464, 2493, 2523, 2552, 2580, 2609, 2638, 2666, 2695, 2723,
  2751, 2779, 2806, 2834, 2861, 2888, 2915, 2942, 2968, 2995, 3021, 3046, 3072,
  3097, 3122, 3147, 3172, 3196, 3220, 3244, 3267, 3290, 3313, 3336, 3358, 3380,
  3402, 3423, 3444, 3465, 3486, 3506, 3525, 3545, 3564, 3583, 3601, 3619, 3637,
  3654, 3672, 3688, 3705, 3721, 3736, 3751, 3766, 3781, 3795, 3809, 3822, 3836,
  3848, 3861, 3873, 3884, 3896, 3907, 3917, 3928, 3937, 3947, 3956, 3965, 3974,
  3982, 3990, 3997, 4004, 4011, 4018, 4024, 4030, 4035, 4041, 4046, 4050, 4055,
  4059, 4063, 4066, 4070, 4073, 4076, 4078, 4081, 4083, 4085, 4086, 4088, 4089,
  4091, 4092, 4092, 4093, 4094, 4094, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
  4095
};

unsigned int fade(unsigned int t)
{
  unsigned int t0 = flookup[ t>>4];
  unsigned int t1 = flookup[(t>>4) + 1];
  return t0 + ((t&15)*(t1-t0) >> 4);
}

int lerp(unsigned int t, int a, int b)
{
  return a + (smul16x16(t, b-a)>>12);
}

int grad(unsigned char hash, int x, int y, int z)
{
  switch(hash & 0xF)
  {
    case 0x0: return  x + y;
    case 0x1: return -x + y;
    case 0x2: return  x - y;
    case 0x3: return -x - y;
    case 0x4: return  x + z;
    case 0x5: return -x + z;
    case 0x6: return  x - z;
    case 0x7: return -x - z;
    case 0x8: return  y + z;
    case 0x9: return -y + z;
    case 0xA: return  y - z;
    case 0xB: return -y - z;
    case 0xC: return  y + x;
    case 0xD: return -y + z;
    case 0xE: return  y - x;
    case 0xF: return -y - z;
    default: return 0; // never happens
  }
}

#define N 0x1000
int noise(long lx, long ly, long lz)
{
  unsigned char X = lx>>12 & 255;
  unsigned char Y = ly>>12 & 255;
  unsigned char Z = lz>>12 & 255;
  int x = lx & N-1;
  int y = ly & N-1;
  int z = lz & N-1;
  unsigned int u = fade(x);
  unsigned int v = fade(y);
  unsigned int w = fade(z);
  unsigned char  A = p(X  )+Y;
  unsigned char AA = p(A  )+Z;
  unsigned char AB = p(A+1)+Z;
  unsigned char  B = p(X+1)+Y;
  unsigned char BA = p(B  )+Z;
  unsigned char BB = p(B+1)+Z;

  return lerp(w, lerp(v, lerp(u, grad(p(AA  ), x  , y  , z   ),
                                 grad(p(BA  ), x-N, y  , z   )),
                         lerp(u, grad(p(AB  ), x  , y-N, z   ),
                                 grad(p(BB  ), x-N, y-N, z   ))),
                 lerp(v, lerp(u, grad(p(AA+1), x  , y  , z-N ),
                                 grad(p(BA+1), x-N, y  , z-N )),
                         lerp(u, grad(p(AB+1), x  , y-N, z-N ),
                                 grad(p(BB+1), x-N, y-N, z-N ))));
}
