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
void findT(Point endPoint, double R, struct Ray *ray, double * Ts);
void printVector(Vector v, char* name);
int maxN(Vector N);
int abs(int a);
void normalize(Point a);
void distPoints(Point *a, Point *b);
void project(Point *p, Vector N);
void newCoord(struct Polygon poly, Point *P, Vector N);
void findCLeftI(int C[][2], int CLeftI[][3]);
void checkThatLeftInverse(int C[][2], int CleftI[][3]);
void changeBasis(Point endPoint, Point P, double R);
int v = 1;

int main(int argc, char **argv){
    FILE *fp;
    FILE *writeFile;
    char magicNumStr[2];
    int yimg, ximg;
    if(argc >1){
        v = strtol(argv[1], NULL, 10);
        printf("new v: %i\n", v);
    }
    //sed '2,$s/\s//g' simple.bpm |tr -d "\n" > test.smaller.pbm
    //and then go in and and new lines to the header 
    fp = fopen("in.pbm", "r");
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
    int outY = 1100;
    int outX = 850;

    int sizeOfPaperY = 1100;
    int sizeOfPaperX = 850;
    
    double R = outY/(2*M_PI);
    printf("R: %f\n", R);

    Point endPoint = {ximg/2., yimg/2., outX};
    if(!v)printVector(endPoint, "endPoint");

    int *outArr = malloc(sizeof(int)*sizeOfPaperX*sizeOfPaperY);
    for(int i = 0; i<sizeOfPaperY;i++){
        for(int j = 0; j<sizeOfPaperX;j++){
            outArr[i*sizeOfPaperX+j] = 2;
        }
    }

    /* Point test = {250,(250-R),0}; */
    /* fprintf(writeFile, "%f,%f,%f\n", test[0], test[1], test[2] ); */
    /* changeBasis(endPoint, test, R); */
    /* fprintf(writeFile, "%f,%f\n", test[0], test[1]); */
    /* exit(1); */

    double Ts[2];
    struct Ray ray;
    for(int i = 0; i<yimg;i++){
        for(int j = 0; j<ximg;j++){
            if(imgArr[j*ximg+i] == '1'){
                if(!v)printf("----->\n");
                ray.O[0] = j;//x
                ray.O[1] = i;//y
                ray.O[2] = 0;//z

                pointSub(ray.O, endPoint, &ray.D);
                if(!v)printf("rayDlengthB: %f\n", (ray.D[0]*ray.D[0]+ray.D[1]*ray.D[1]+ray.D[2]*ray.D[2]));
                normalize(ray.D);
                if(!v)printf("rayDlengthA: %f\n", (ray.D[0]*ray.D[0]+ray.D[1]*ray.D[1]+ray.D[2]*ray.D[2]));
                if(!v){
                    printVector(ray.O, "O  ");
                    printVector(endPoint, "End");
                    printVector(ray.D, "D  ");
                }


                findT(endPoint, R, &ray, Ts);
                for(int t = 0; t<2;t++){
                    if(!v)printf("#%i t0: %f, t1: %f\n",t , Ts[0], Ts[1]);
                    if(Ts[t]<=0 || isnan(Ts[t])){
                        if(!v){
                            printf("t<=0\n");
                        }
                        continue;
                    } 
                    ray.P[0] = ray.O[0] + ray.D[0] * Ts[t];
                    ray.P[1] = ray.O[1] + ray.D[1] * Ts[t];
                    ray.P[2] = ray.O[2] + ray.D[2] * Ts[t];

                    if(ray.P[2] < 0 || ray.P[2]> outX){ //outX = endPoint[2]
                        continue;
                    }
                    if(v==3)fprintf(writeFile, "%f,%f,%f\n", ray.P[0], ray.P[1], ray.P[2] );
                    changeBasis(endPoint, ray.P, R);
                    /* if(v==3)fprintf(writeFile, "%f,%f,%f\n", ray.P[0], ray.P[1], ray.P[2] ); */
                    if(!v) printVector(ray.P, "rayPA");

                    if(ray.P[0]>=0 && round(ray.P[0])<(sizeOfPaperX) &&ray.P[1]>=0 && round(ray.P[1])<(sizeOfPaperY)){
                        if(!v){
                            printf("coords [%f, %f]\n", ray.P[0], ray.P[1]);
                            printf("coords [%i, %i]\n", (int)round(ray.P[0]), (int)round(ray.P[1]));
                        }
                        fflush(stdout);
                        outArr[(int)(round(ray.P[1]))*sizeOfPaperX+(int)round(ray.P[0])] = 0;
                    }else{
                        printf("doesn't fit [%f, %f]\n", ray.P[0], ray.P[1]);
                    }
                }
                /* if(!v)printf("<-----\n"); */
            }
        }
    }
    if(v!=3){
        fprintf(writeFile, "P2\n%i %i\n2\n", sizeOfPaperX, sizeOfPaperY);
            for(int j = 0; j<sizeOfPaperY;j++){
        for(int i = 0; i<sizeOfPaperX;i++){
                /* printf("i: %i, j: %i, maxOutSide: %i\n", i, j, maxOutSide); */
                //printf("%i ", outArr[i*(maxOutSide)+j]);
                fprintf(writeFile, "%i ", outArr[j*(sizeOfPaperX)+i]);
            }
            fprintf(writeFile, "\n");
        }
    }
    fclose(writeFile);

    free(imgArr);
    free(outArr);
    fclose(fp);

}
void findT(Point endPoint, double R, struct Ray *ray, double * Ts){
    double A,B,C;


    //fact check w http://www.wolframalpha.com/input/?i=simplify+(O_0%2BD_0*t)%5E2%2B(O_1%2BD_1*t)%5E2-r%5E2+%3D+0
    //center endPoint (and bring rest along) to (0,0,0);
    double x = ray->O[0] - endPoint[0];
    double y = ray->O[1] - endPoint[1];
    
    A = (ray->D[0]*ray->D[0]+ray->D[1]*ray->D[1]);
    B = 2*(x*ray->D[0]+y*ray->D[1]);
    C = x*x+y*y-R*R;

    Ts[0] = (-1*B+sqrt(B*B-4*A*C))/(2*A);    
    Ts[1] = (-1*B-sqrt(B*B-4*A*C))/(2*A);    
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


int abs(int a){
    if(a < 0){
        return -1*a;
    }else{
        return a;
    }
}

void normalize(Point a){
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
void changeBasis(Point endPoint, Point P, double R){
    //https://stackoverflow.com/questions/14066933/direct-way-of-computing-clockwise-angle-between-2-vectors
    double degWanted = 25;
    Vector V1 = {R*cos((degWanted)*(M_PI/180)), R*sin((degWanted)*(M_PI/180)),0};
    if(!v)printVector(P, "P");
    Vector V2 = {P[0]-endPoint[0], P[1] - endPoint[1], 0};
    /* Vector V2 = {P[0], P[1] , 0}; */
    if(!v){
        printVector(V2, "v2");
    }
    normalize(V1);
    normalize(V2);
    if(!v){
        printVector(V2, "v2");
        printVector(V1, "v1");
    }
    /* double angle = acos(vectorDot(V1, V2));  // atan2(y, x) or atan2(sin, cos) */

    double dot = vectorDot(V1, V2);
    double det = (V1[0]*V2[1]) - (V1[1]*V2[0]);

    double angle = atan2(-1*det, -1*dot)+M_PI;


    /* if(!v)printf("angle: %f deg, %f rad\n", angle*(180/M_PI), angle); */
    if(!v)printf("angle: %f \n", angle*(180/M_PI));
    P[0] = P[2];//up and down
    P[1] = angle*R;//around circ, s = theta*R in radians
    P[2] = 0;
}
