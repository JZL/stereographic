#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "RayPolygon.h"

void vectorCross(Vector a, Vector b, Vector *c);
int vectorDot(Vector a, Vector b);
void pointSub(Point a, Point b, Vector *c);
double findT(Point *verts, struct Polygon poly, struct Ray ray, int * i_s);
void printVector(Vector v);
int maxN(Vector N);
int abs(int);

int main(int argc, char **argv){
    double t;
    bool intersects;
    int i_s[2];
    struct Ray ray = {
        {0,0,0},
        {0.5, 1, 1.25}
    };

    Point verts[3]= {
        {0,1,0},
        {1,1,0},
        {1,1,1}
    };

    struct Polygon poly = {
        3,
        false,
        verts
    };

    t = findT(verts, poly, ray, i_s);
    printf("%f\n", t);
    if(t<=0){
        //either error or intersection behind origin, reject
        return 0;
    } 
    printf("i_1: %i, i_2:%i\n", i_s[0], i_s[1]);

    intersects = intersect(poly, ray, t, i_s[0], i_s[1]);
    printf("%d\n", intersects);
}
double findT(Point *verts, struct Polygon poly, struct Ray ray, int *i_s){
    Vector v0v1, v0v2, N;
    double d, t, N_dot_D;
    int maxOfN;

    pointSub(verts[0], verts[1], &v0v1);
    pointSub(verts[0], verts[2], &v0v2);

    vectorCross(v0v1, v0v2, &N);
    d = vectorDot(verts[0], N);
    d = -1*d;

    printf("d: %f\n", d);
    printf("N: ");
    printVector(N);
    printf("ray.O ");
    printVector(ray.O);
    printf("ray.D ");
    printVector(ray.D);

    N_dot_D = vectorDot(N, ray.D);
    if(N_dot_D == 0){
        return -1;
    }
    t = -1*(d+vectorDot(N, ray.O))/(vectorDot(N, ray.D));

    printf("N_S: %f, %f, %f\n", N[0], N[1], N[2]);
    maxOfN = maxN(N);
    if(N[0] == maxOfN){
        i_s[0] = 1;
        i_s[1] = 2;
    }else if(N[1] == maxOfN){
        i_s[0] = 0;
        i_s[1] = 2;
    }else{
        i_s[0] = 0;
        i_s[1] = 1;
    }

    return t;
}

void vectorCross(Vector a, Vector b, Vector *c){
 /* a = <a1, a2, a3> */
 /* b = <b1, b2, b3> */
 /* axb = <a2b3-a3b2, a3b1-a1b3, a1b2-a2b1> */

    (*c)[0] = a[1]*b[2]-a[2]*b[1];
    (*c)[1] = a[2]*b[0]-a[0]*b[2];
    (*c)[2] = a[0]*b[1]-a[1]*b[0];
}

int vectorDot(Vector a, Vector b){
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

void pointSub(Point a, Point b, Vector *c){
    
    (*c)[0] = b[0]-a[0];
    (*c)[1] = b[1]-a[1];
    (*c)[2] = b[2]-a[2];
}

void printVector(Vector v){
    /* printf("[%f, %f, %f\n %f, %f, %f\n %f, %f, %f]\n", v[0][0], v[1][0], v[2][0], v[0][1], v[1][1], v[2][1], v[0][2], v[1][2], v[2][2]); */
    printf("[%f, %f, %f]\n", v[0], v[1], v[2]);
}

int maxN(Vector N){
    double absN0, absN1, absN2;
    absN0 = abs(N[0]);
    absN1 = abs(N[1]);
    absN2 = abs(N[2]);
    if(absN0 >= absN1 && absN0>=absN2){
        return N[0];
    }else if(absN1 >= absN2 && absN1>=absN2){
        return N[1];
    }else{
        return N[2];
    }
}

int abs(int a){
    if(a < 0){
        return -1*a;
    }else{
        return a;
    }
}
