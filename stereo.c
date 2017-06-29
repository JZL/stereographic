#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "RayPolygon.h"

//https://cocalc.com/projects/b0966444-8a50-4f2a-a0a7-5d5e6e261ea5/files/2017-06-21-221142.sagews
void vectorCross(Vector a, Vector b, Vector *c);
double vectorDot(Vector a, Vector b);
void pointSub(Point a, Point b, Vector *c);
double findT(Point *verts, struct Polygon poly, struct Ray ray, int * i_s, Vector N);
void printVector(Vector v, char* name);
int maxN(Vector N);
int abs(int);
void normalize(Vector);
void distPoints(Point *a, Point *b);
void project(Point *p, Vector N);
void newCoord(struct Polygon poly, Point *P, Vector N);
void findCLeftI(int C[][2], int CLeftI[][3]);
int v = 0;

int main(int argc, char **argv){
    FILE *fp;
    FILE *writeFile;
    char magicNumStr[2];
    int yimg, ximg;
    Vector N;
    //sed '2,$s/\s//g' simple.bpm |tr -d "\n" > test.smaller.pbm
    //and then go in and and new lines to the header 
    fp = fopen("test.smaller.pbm", "r");
    /* fp = fopen("test.smaller.pbm", "r"); */
    if(fp == NULL){
        printf("Error reading file\n");
        exit(1);
    }
    writeFile = fopen("out.pgm", "w");
    if(writeFile == NULL){
        printf("couldn't write to file");
    }
    fscanf(fp, "%s", magicNumStr);
    /* printf("%s\n", magicNumStr); */
    if(strcmp(magicNumStr, "P1")!=0){
        printf("Needs to be pbm\n");
        exit(2);
    }

    fscanf(fp, "%i %i\n",&yimg, &ximg );
    /* printf("%i %i\n", height, width); */
    char *imgArr = malloc(sizeof(int)*yimg*(ximg));
    fread(imgArr, sizeof(int)*(ximg), (yimg), fp);
    /* printf("%c\n", imgArr[115*(width)+5]); */
    //arr[i*width+j]
    int maxOutSide = 500;
    int *outArr = malloc(sizeof(int)*maxOutSide*maxOutSide);
    for(int i = 0; i<maxOutSide;i++){
        for(int j = 0; j<maxOutSide;j++){
            outArr[j*(maxOutSide)+i] = 0;
        }
    }



    double t;
    bool intersects;
    int i_s[2];
    //CHANGE POLY ATTR BELOW
    Point verts[3]= {
        {ximg, 0, 0},
        {ximg, yimg, 0},
        {ximg, 0, 100},
    };
    Point endPoint = {(int)ximg*5, (int)yimg*5, (int)ximg*.5};
    /*
    Point verts[3]= {
        {,0, 0},
        {200, 0, 300},
        {200, 200, 300},
    };
    */

    struct Polygon poly = {
        3,
        false,
        verts
    };
    struct Ray ray;

    Vector v0v1, v0v2;
    pointSub(verts[0], verts[1], &v0v1);
    pointSub(verts[0], verts[2], &v0v2);

    vectorCross(v0v1, v0v2, &N);

    normalize(v0v1);
    normalize(N);

    int C[3][2];
    int CLeftI[3][3];
    C[0][0] =v0v1[0]; 
    C[1][0] =v0v1[1]; 
    C[2][0] =v0v1[2]; 

    C[0][0] =N[0]; 
    C[1][1] =N[1]; 
    C[2][2] =N[2]; 

    findCLeftI(C, CLeftI);
    




    //everything in pixel width
    /* printf("x: %i, y: %i\n", width, height); */
    for(int i = 0; i<yimg;i++){
        for(int j = 0; j<ximg;j++){
            //todo
            if(imgArr[j*ximg+i] == '1'){
                ray.O[0] = j;//x
                ray.O[1] = i;//y
                ray.O[2] = 0;//z

                /* endPoint[0] = i; */
                /* endPoint[1] = j; */
                /* endPoint[2] = 21; */

                pointSub(ray.O, endPoint, &ray.D);
                if(!v)printf("rayDlengthB: %f\n", (ray.D[0]*ray.D[0]+ray.D[1]*ray.D[1]+ray.D[2]*ray.D[2]));
                normalize(ray.D);
                if(!v)printf("rayDlengthA: %f\n", (ray.D[0]*ray.D[0]+ray.D[1]*ray.D[1]+ray.D[2]*ray.D[2]));
                if(!v){
                    printf("-----\n");
                    printVector(ray.O, "O  ");
                    printVector(endPoint, "End");
                    printVector(ray.D, "D  ");
                }

                t = findT(verts, poly, ray, i_s, N);
                if(!v)printf("t: %f\n", t);
                if(t<=0){
                    //either error or intersection behind origin, reject
                    if(!v){
                        printf("t<=0\n");
                    }
                    continue;
                } 
                /* printf("i_1: %i, i_2:%i\n", i_s[0], i_s[1]); */

                intersects = intersect(&poly, &ray, t, i_s[0], i_s[1], v);

                if(!v) printVector(ray.P, "rayPB");
                //distPoints(&(verts[0]), &ray.P);
                newCoord(poly, &ray.P, N);
                if(v==3)fprintf(writeFile, "%f,%f,%f\n", ray.P[0], ray.P[1], ray.P[2] );
                if(!v) printVector(ray.P, "rayPA");
                if(!v) printf("intersects? %d\n", intersects);
                if(ray.P[0]< maxOutSide && ray.P[0]>=0 && ray.P[1]< maxOutSide && ray.P[1]>=0){
                    if(intersects == 1){
                        //outArr[(int)(ray.P[1])*(maxOutSide)+(int)(ray.P[0])] = 1;
                        outArr[(int)round(ray.P[1])*(maxOutSide)+(int)round(ray.P[0])] = 2;
                    }else{
                        outArr[(int)round(ray.P[1])*(maxOutSide)+(int)round(ray.P[0])] = 1;
                    }
                }
                /* if(intersects == 1){ */
                /*     if(!v){ */
                /*         printVector(ray.P, "P  "); */
                /*     } */
                /*     /1* ray.P[0] = ray.O[0] + ray.D[0] * t; *1/ */
                /*     /1* ray.P[1] = ray.O[1] + ray.D[1] * t; *1/ */
                /*     /1* ray.P[2] = ray.O[2] + ray.D[2] * t; *1/ */

                /*     if(ray.P[0]< maxOutSide && ray.P[0]>=0 && ray.P[1]< maxOutSide && ray.P[1]>=0){ */
                /*         outArr[(int)(ray.P[1])*(maxOutSide)+(int)(ray.P[0])] = 1; */
                /*     }else{ */
                /*         outArr[(int)(ray.P[1])*(maxOutSide)+(int)(ray.P[0])] = 0; */
                /*     } */
                /* }else{ */
                /*     outArr[(int)(ray.P[1])*(maxOutSide)+(int)(ray.P[0])] = 0; */
                /* } */
                if(!v) printf("---\n");
            }
        }
    }
    /* printf("\n\n\n"); */
    fprintf(writeFile, "P2\n%i %i\n2\n", maxOutSide, maxOutSide);
    for(int i = 0; i<maxOutSide;i++){
        for(int j = 0; j<maxOutSide;j++){
            /* printf("i: %i, j: %i, maxOutSide: %i\n", i, j, maxOutSide); */
            //printf("%i ", outArr[i*(maxOutSide)+j]);
            fprintf(writeFile, "%i ", outArr[i*(maxOutSide)+j]);
            /* fflush(stdout); */
        }
        //printf("\n");
    }
    fclose(writeFile);

    free(imgArr);
    free(outArr);
    fclose(fp);

}
double findT(Point *verts, struct Polygon poly, struct Ray ray, int *i_s, Vector N){
    double d, t, N_dot_D;
    int maxOfN;
    d = vectorDot(verts[0], N);
    d = -1*d;

    /* printf("d: %f\n", d); */
    /* printf("N: "); */
    /* printVector(N); */
    /* printf("ray.O "); */
    /* printVector(ray.O); */
    /* printf("ray.D "); */
    /* printVector(ray.D); */

    N_dot_D = vectorDot(N, ray.D);
    if(N_dot_D == 0){
        return -1;
    }
    t = -1*(d+vectorDot(N, ray.O))/(vectorDot(N, ray.D));

    maxOfN = maxN(N);
    if(!v){
        printf("maxOfN: %d\n", maxOfN);
        printf("N_S: %f, %f, %f\n", (N)[0], (N)[1], (N)[2]);
    }
    if((N)[0] == maxOfN){
        if(!v)printf("0 is maxOfN\n");
        i_s[0] = 1;
        i_s[1] = 2;
    }else if((N)[1] == maxOfN){
        if(!v)printf("1 is maxOfN\n");
        i_s[0] = 0;
        i_s[1] = 2;
    }else{
        if(!v)printf("2 is maxOfN\n");
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

double vectorDot(Vector a, Vector b){
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

void pointSub(Point a, Point b, Vector *c){

    (*c)[0] = b[0]-a[0];
    (*c)[1] = b[1]-a[1];
    (*c)[2] = b[2]-a[2];
}


int maxN(Vector N){
    double absN0, absN1, absN2;
    absN0 = fabs(N[0]);
    absN1 = fabs(N[1]);
    absN2 = fabs(N[2]);
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

void normalize(Vector a){
    float w = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    a[0] /=w;
    a[1] /=w;
    a[2] /=w;
}
void distPoints(Point *a, Point *b){
    if(!v){printVector(*a, "a");}
    if(!v){printVector(*b, "b");}
    (*b)[0] = (*b)[0] - (*a)[0]; 
    (*b)[1] = (*b)[1] - (*a)[1]; 
    (*b)[2] = (*b)[2] - (*a)[2]; 
    if(!v){printVector(*a, "a");}
}
void project(Point *P, Vector N){
    //(x, y, z) - (x*nx + y*ny + z*nz) (nx, ny, nz)
    double scalarProj = vectorDot(*P, N);
    if(!v)printf("scalarProj: %f\n", scalarProj);
    if(!v)printVector(N, "N");
    (*P)[0] = (*P)[0]-scalarProj*N[0];
    (*P)[1] = (*P)[1]-scalarProj*N[1];
    (*P)[2] = (*P)[2]-scalarProj*N[2];
}
void newCoord(struct Polygon poly, Point *P, Vector N){
    Vector V0V1, lastAxis;

    pointSub(poly.V[0], poly.V[1], &V0V1);
    normalize(V0V1);

    vectorCross(N, V0V1, &lastAxis);
    normalize(lastAxis);

    (*P)[0] = vectorDot(*P,  V0V1);
    (*P)[1] = vectorDot(*P,  lastAxis);
}

findCLeftI(int C[][2], int CLeftI[][3]){
   double det;
   det = (1/(C[0][0]*C[2][1]-C[0][1]*C[2][0]));
   if(det == 0){
    det = (1/(C[0][0]*C[1][1]-C[0][1]*C[1][0]));
    if(det == 0){
        printf("Can't find det");
        exit(1);
    }
   }
   det = 1/det;
   CLeftI[0][0] = C[2][1]*det;
   CLeftI[0][1] = -1*C[0][1]*det;
   CLeftI[1][0] = -1*C[2][0]*det;
   CLeftI[1][1] = C[0][0]*det;
}
