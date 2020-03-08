#include "stdio.h"

#include "gpu.h"

#include "colors.h"

const Color player_colors[8] = {
  {{.r=0, .g=0, .b=0, .i=1}},
  {{.r=0, .g=0, .b=3, .i=1}},
  {{.r=0, .g=3, .b=0, .i=1}},
  {{.r=0, .g=3, .b=3, .i=1}},
  {{.r=3, .g=0, .b=0, .i=1}},
  {{.r=3, .g=0, .b=2, .i=1}},
  {{.r=3, .g=3, .b=0, .i=1}},
  {{.r=3, .g=3, .b=3, .i=1}},
};


#define DIM(x) ((x)>1 ? (x)-1 : (x))
#define DDIM(x) DIM(DIM(x))
#define AUG(x) ((x)<3 ? (x)+1 : (x))
#define AAUG(x) AUG(AUG(x))

void color_dark(Color *color)
{
  switch (color->comp.i)
  {
    case 2:
    case 3:
      color->comp.i = 1;
    break;

    case 1:
      color->comp.i = 0;
      color->comp.r = DIM(color->comp.r);
      color->comp.g = DIM(color->comp.g);
      color->comp.b = DIM(color->comp.b);
    break;

    case 0:
      color->comp.r = DDIM(color->comp.r);
      color->comp.g = DDIM(color->comp.g);
      color->comp.b = DDIM(color->comp.b);
    break;
  }
}

void color_bright(Color *color)
{
  switch (color->comp.i)
  {
    case 1:
    case 0:
      color->comp.i = 2;
    break;

    case 2:
      color->comp.i = 3;
      color->comp.r = AUG(color->comp.r);
      color->comp.g = AUG(color->comp.g);
      color->comp.b = AUG(color->comp.b);
    break;

    case 3:
      color->comp.r = AAUG(color->comp.r);
      color->comp.g = AAUG(color->comp.g);
      color->comp.b = AAUG(color->comp.b);
    break;
  }
}
