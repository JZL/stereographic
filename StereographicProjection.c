#include <stdlib.h>
#include <string.h>
#include <stdio.h>


struct 3D {
    double x;
    double y;
    double z
}

struct vector{
    
}

struct triangle{
    struct 3D v1;
    struct 3D v2;
    struct 3D v3;
}

struct 3D badoueRayPolygon(struct vector D, struct triangle V);
int P(struct 3D origin, struct vector D, *int is)
struct 3D Vadd(struct 3D, *int is)
int main(int argc, char **argv){

}

struct 3D Vadd(struct 3D m1, *int is){
    struct 3D added;
    added.X = m1.X + ; 
    return struct 3D
}
int P(struct 3D origin, struct vector D, *int is){
    return Vadd(origin, Vmult(D, is));
}
struct 3D badoueRayPolygon(struct vector D, struct triangle V){
    /* 0 = point; */
    /* D = vector; */
    /* P = inte4rserction point; */
    /* V = array[0..2] of polygon vertices; */

   /* P<- O+Dt; */
  /* u0 = P[i1]-V[0][i1]; */ 
  /* v0 = P[i2]-V[0][i2]; */ 

  /* u1 = V[1][i1] - V[0][i1] */
  /* u2 = V[2][i1] - V[0][i1] */
  /* v1 = V[1][i2] - V[0][i2] */
  /* v2 = V[2][i2] - V[0][i2] */
  /* if u1 = 0 */
    /*     then B = u0/u2 */
    /*         if 0<=B<=1 */
    /*                 a = (v0-B*v2)/v1; */
    /*     else B = (v0*u1-u0*v1)/(v2*u1-u2*v1); */
    /*         if 0<=B<=1 */
    /*                 a = (u0 - b*u2)/u1 */
/* return (a>=0 and B>=0 and (a+B)<=1) */


}

int P(struct 3D origin, struct vector D, *int is)
