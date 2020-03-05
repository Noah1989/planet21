#include <stdio.h>
#include <math.h>

#include "physics.h"

/* pixel location of center of gravity (x=0) */
float cgx, cgy;
/* planet sizes (radii), squared */
float s1s, s2s;
/* masses of planets, multiplied by gravity constant */
float gm1, gm2;
/* x coordinates of planets relative to COG */
float x1, x2;
/* rotation frequency of the planets around each other */
float omega, omega_twice, omega_squared;

/* position of the particle */
float lx, ly, lz;
/* velocity of the particle */
float vx, vy, vz;
/* acceleration of the particle */
float ax, ay, az;


void init_physics
(
  /* planet sizes */
  int ps1, int ps2,
  /* planet pixel coordinates, px1 < px2 */
  int px1, int px2, int py
)
{
  float r, m1, m2;

  s1s=ps1*ps1;
  s2s=ps2*ps2;

  m1 = s1s*ps1;
  m2 = s2s*ps2;

  gm1 = m1*0.01;
  gm2 = m2*0.01;

  cgx = (m1*px1 + m2*px2)/(m1+m2);
  cgy = py;
  x1 = px1 - cgx;
  x2 = px2 - cgx;

  r = x2 - x1;
  omega_squared = (gm1+gm2)/(r*r*r);
  omega = sqrtf(omega_squared);
  omega_twice = omega*2;
}

void calc_a()
{

  float dx1 = lx-x1;
  float dx2 = lx-x2;

  /* distance to m1, squared */
  float d1s = dx1*dx1 + ly*ly + lz*lz;
  /* distance to m1, cubed */
  float d1c = d1s*sqrtf(d1s);
  /* distance to m1, cubed, inverse */
  float d1ci = 1/d1c;

  /* distance to m2, squared */
  float d2s = dx2*dx2 + ly*ly + lz*lz;
  /* distance to m2, cubed */
  float d2c = d2s*sqrtf(d2s);
  /* distance to m1, cubed, inverse */
  float d2ci = 1/d2c;

  /* impact */
  if ((d1s<s1s) || (d2s<s2s))
  {
    ax=0; ay=0; az=0;
    vx=0; vy=0; vz=0;
    return;
  }

  ax =
    /* centrifugal force */
    omega_squared*lx
    /* coriolis force */
    + omega_twice*vy
    /* gravity towards m1 */
    - gm1*dx1*d1ci
    /* gravity towards m1 */
    - gm2*dx2*d2ci
  ;

  ay =
    /* centrifugal force */
    omega_squared*ly
    /* coriolis force */
    - omega_twice*vx
    /* gravity towards m1 */
    - gm1*ly*d1ci
    /* gravity towards m1 */
    - gm2*ly*d2ci
  ;

  az =
    /* gravity towards m1 */
    - gm1*lz*d1ci
    /* gravity towards m1 */
    - gm2*lz*d2ci
  ;

}

void test(int px, int py)
{
  lx = px-cgx;
  ly = py-cgy;
  lz = 0;
  vx = 0;
  vy = 3;
  vz = 0;
  calc_a();
  printf
  (
    "\n"
    "ax: %d\n"
    "ay: %d\n"
    "az: %d\n"
    ,(int)(ax*1000), (int)(ay*1000), (int)(az*1000)
  );
}

void run(int *px, int *py)
{
  calc_a();
  vx+=ax;
  vy+=ay;
  vz+=az;
  lx+=vx;
  ly+=vy;
  lz+=vz;
  *px=(int)(lx+cgx);
  *py=(int)(ly+cgy);
}
