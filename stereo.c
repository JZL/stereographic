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
void checkThatLeftInverse(int C[][2], int CleftI[][3]);
void changeBasis(Point *a, int CLeftI[][3]);
int v = 1;
void findMinMax(double X, double Y, double *minMaxX, double *minMaxY);
struct coord2D  {
    double X;
    double Y;
};
int main(int argc, char **argv){
    FILE *fp;
    FILE *writeFile;
    char magicNumStr[2];
    int yimg, ximg;
    int tmpF;

    double minMaxX[2];
    double minMaxY[2];
    int scale=0; //if make 1, will scale to take up (with 1:1 scaling), the output size

    int firstTime = 1; //for findingMinMax, if first run through loop
    //can't be i && j == 0 bc the value at [i][j] might not be == 1

    Vector N;
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
    writeFile = fopen("out/out.pgm", "w");
    if(writeFile == NULL){
        printf("couldn't write to file");
    }
    tmpF = fscanf(fp, "%s", magicNumStr);
    printf("tmpF: %i\n", tmpF);
    if (feof(fp) ||tmpF != 1){
        printf("Unexpected error in input data.");
        exit(EXIT_FAILURE);
    }
    /* printf("%s\n", magicNumStr); */
    if(strcmp(magicNumStr, "P1")!=0){
        printf("Needs to be pbm\n");
        exit(2);
    }

    tmpF = fscanf(fp, "%i %i\n",&yimg, &ximg );
    printf("tmpF: %i\n", tmpF);
    if (feof(fp)||tmpF != 2){
        printf("Unexpected error in input data.");
        exit(EXIT_FAILURE);
    }
    /* printf("%i %i\n", height, width); */
    char *imgArr = malloc(sizeof(int)*yimg*(ximg));
    tmpF = fread(imgArr, sizeof(int)*(ximg), (yimg), fp);
    printf("tmpF: %i %i, %i, feof(fp): %i\n", tmpF,ximg, yimg, feof(fp));
    if(!feof(fp) ||tmpF!=yimg/4){
        //want ti be the end of file
        printf("Unexpected error in input data.");
        exit(EXIT_FAILURE);
    }


    struct coord2D *newCoordArr = malloc(sizeof(struct coord2D)*yimg*ximg);

    /* printf("%c\n", imgArr[115*(width)+5]); */
    //arr[i*width+j]
    int sizeOfPaperY = 500;
    int sizeOfPaperX = sizeOfPaperY;
    int *outArr = malloc(sizeof(int)*sizeOfPaperX*sizeOfPaperY);
    for(int i = 0; i<sizeOfPaperY;i++){
        for(int j = 0; j<sizeOfPaperX;j++){
            outArr[i*sizeOfPaperX+j] = 2;
        }
    }



    double t;
    bool intersects;
    int i_s[2];
    //CHANGE POLY ATTR BELOW
    //NEEDS TO BE IN CIRCULAR ORDER 
    Point verts[3]= { 
        {ximg*1.0, 0, 0}, 
        {ximg*1.0, yimg*0.6, 0}, 
        //{ximg*1.0, yimg*1.0, ximg*1.0}, 
        {ximg*1.0, 0, ximg*1.2} 
    }; 
    /* Point endPoint = {(int)ximg*2, (int)yimg*1, (int)ximg*.5}; */
    /*
       Point verts[4]= {
       {0,0,0},
       {0,0, 125},
       {125,0,0},
       {125,0,125}
       };
     */
    /*
       Point verts[4]= {
       {0,0,20},
       {ximg, 0, 20},
       {ximg, yimg, 20},
       {0, yimg, 20},
       };
     */

    /*
       for(int i =0; i<4;i++){
       verts[i][0]*=2;
       verts[i][1]*=2;
       verts[i][0]+=(ximg/2-(125/2));
       verts[i][1]+=(yimg/2-(125/2));
       }
     */
    //Point endPoint = {(ximg)/2, (ximg)/2, 55525};
    Point endPoint = {(ximg)*1.2, (yimg)*.5, ximg*.55};

    struct Polygon poly = {
        3,
        false,
        verts
    };
    struct Ray ray;

    Vector v0v1, v0v2, secondAxis;
    /*
       pointSub(verts[1], verts[0], &v0v1);
       pointSub(verts[2], verts[0], &v0v2);
     */
    pointSub(verts[1], verts[0], &v0v1);
    pointSub(verts[2], verts[0], &v0v2);

    vectorCross(v0v1, v0v2, &N);

    printVector(v0v1, "v0v1");
    normalize(v0v1);
    printVector(v0v1, "v0v1Norm");
    printVector(N, "N");
    normalize(N);
    printVector(N, "NNorm");

    vectorCross(v0v1, N, &secondAxis);
    printVector(secondAxis, "secondAxis");
    normalize(secondAxis);
    printVector(secondAxis, "secondAxisNorm");

    int C[3][2];
    int CLeftI[3][3];
    C[0][0] =v0v1[0]; 
    C[1][0] =v0v1[1]; 
    C[2][0] =v0v1[2]; 

    C[0][1] =secondAxis[0]; 
    C[1][1] =secondAxis[1]; 
    C[2][1] =secondAxis[2]; 

    findCLeftI(C, CLeftI);



    Point lightPoint = {endPoint[0], endPoint[1], endPoint[2]};
    printVector(lightPoint, "lightPointBef");
    changeBasis(&lightPoint, CLeftI);
    printVector(lightPoint, "lightPointAft");
    /*
    //firsttime is redundant but if delete lightpoint, don't want to forget
    if(firstTime == 1){
    //set minMax with init values
    minMaxX[0] = ray.P[0];
    minMaxX[1] = ray.P[0];
    minMaxY[0] = ray.P[1];
    minMaxY[1] = ray.P[1];
    firstTime = 0;
    }
    findMinMax(lightPoint[0], lightPoint[1], minMaxX, minMaxY);
     */

    for(int i = 0; i<yimg;i++){
        for(int j = 0; j<ximg;j++){
            newCoordArr[i*ximg+j].X = 0;
            newCoordArr[i*ximg+j].Y = 0;
            //todo
            if(imgArr[i*ximg+j] == '1'){
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

                if(!v){ printVector(ray.P, "rayPB");}
                //distPoints(&(verts[0]), &ray.P);
                //newCoord(poly, &ray.P, N);
                changeBasis(&ray.P, CLeftI);
                if(v==3)fprintf(writeFile, "%f,%f,%f\n", ray.P[0], ray.P[1], ray.P[2] );
                if(!v) printVector(ray.P, "rayPA");
                if(!v) printf("intersects? %d\n", intersects);
                if(!v) printVector(ray.P, "rayPAfterAdd");
                if(firstTime == 1){
                    //set minMax with init values
                    minMaxX[0] = ray.P[0];
                    minMaxX[1] = ray.P[0];
                    minMaxY[0] = ray.P[1];
                    minMaxY[1] = ray.P[1];
                    firstTime = 0;
                }
                findMinMax(ray.P[0], ray.P[1], minMaxX, minMaxY);
                if(intersects == 1){
                    newCoordArr[i*ximg+j].X = ray.P[0];
                    newCoordArr[i*ximg+j].Y = ray.P[1];
                }else{
                    //there will be an closed spot at 0,0 but, otherwise, need to store ximg*yimg*sizeof(char OR int) extra bytes to say if intersects
                    newCoordArr[i*ximg+j].X = 0;
                    newCoordArr[i*ximg+j].Y = 0;
                }
            }
        }
    }
    //The points need to be in the circular order
    Point a, b;
    Vector m;
    int numSteps = 400;
    double changeSize = 1;
    struct coord2D *outlineArr = malloc(sizeof(struct coord2D)*(poly.n)*numSteps);
    printf("====================================================================\n");
    for(int i = 0; i<(poly.n); i++){
        a[0] = verts[i][0];
        a[1] = verts[i][1];
        a[2] = verts[i][2];

        b[0] = verts[(i+1)%poly.n][0];
        b[1] = verts[(i+1)%poly.n][1];
        b[2] = verts[(i+1)%poly.n][2];

        printf("\ngoing between %i and %i\n", i, (i+1)%poly.n);
        printVector(a, "vertA");
        printVector(b, "vertB");

        changeBasis(&a, CLeftI);
        changeBasis(&b, CLeftI);

        printVector(a, "vertA");
        printVector(b, "vertB");

        pointSub(a, b, &m);
        printVector(m, "m");

        for(int t = 0; t<numSteps;t++){
            printf("%i,", t);
            fflush(stdout);
            double newX = a[0]+m[0]*((float)t/numSteps);
            double newY = a[1]+m[1]*((float)t/numSteps);

            findMinMax(newX, newY, minMaxX, minMaxY);
            outlineArr[(numSteps*i+t)].X = newX;
            outlineArr[(numSteps*i+t)].Y = newY;
        }
    }
    double changeXSize = (sizeOfPaperX/(minMaxX[1] - minMaxX[0]));
    double changeYSize = (sizeOfPaperY/(minMaxY[1] - minMaxY[0]));
    if(changeXSize< changeYSize){
        changeSize = changeXSize;
    }else{
        changeSize = changeYSize;
    }
    if(scale == 0){
        changeSize = 1;
    }
    printf("\nminMax0: %f, %f", minMaxX[0], minMaxY[0]);
    for(int i = 0; i<(poly.n*numSteps);i++){
        outlineArr[i].X =  (outlineArr[i].X-minMaxX[0])*changeSize;
        outlineArr[i].Y =  (outlineArr[i].Y-minMaxY[0])*changeSize;
        printf("(%f, %f)\n", outlineArr[i].X, outlineArr[i].Y);
        if( outlineArr[i].X>=sizeOfPaperX||outlineArr[i].X<0||
            outlineArr[i].Y>=sizeOfPaperY||outlineArr[i].Y<0){
            printf("breaking");
            continue;
        }
        outArr[((int)(outlineArr[i].Y)*(sizeOfPaperX))+(int)(outlineArr[i].X)] = 1;
    }


    if(minMaxX[1] - minMaxX[0] > sizeOfPaperX){
        printf("WILL OVERFLOW IN X DIREACTION\n");
    }
    if(minMaxY[1] - minMaxY[0] > sizeOfPaperY){
        printf("WILL OVERFLOW IN Y DIREACTION\n");
    }
    lightPoint[0] = (lightPoint[0]-minMaxX[0])*changeSize;
    lightPoint[1] = (lightPoint[1]-minMaxY[0])*changeSize;
    printVector(lightPoint, "lightPointAft");
    for(int i = 0; i<yimg;i++){
        for(int j = 0; j<ximg;j++){
            int X = newCoordArr[i*ximg+j].X;
            int Y = newCoordArr[i*ximg+j].Y;
            if(!(X==0 && Y==0)){
                X = (X-minMaxX[0])*changeSize;
                Y = (Y-minMaxY[0])*changeSize;
                if(X>=0 && X< sizeOfPaperX  && Y>=0 && Y<sizeOfPaperY){
                    //outArr[(int)(ray.P[1])*(maxOutSide)+(int)(ray.P[0])] = 1;
                    /* if(!v) printf("coords [%i, %i]\n", (int)(round(ray.P[1]+sizeOfPaperY*.5)),(int)round(ray.P[0]+sizeOfPaperX*.5)); */
                    /* outArr[(int)round(ray.P[1]+maxOutSide*.5)*(maxOutSide)+(int)round(ray.P[0]+maxOutSide*.5)] = 0; */

                    //outArr[(int)(round(ray.P[1]*1.5+sizeOfPaperY*.5+200)*(sizeOfPaperX)+(int)round(ray.P[0]*2+sizeOfPaperX*.5-200))] = 0;
                    //outArr[(int)(round((ray.P[1]+200)*1.5+sizeOfPaperY*.5)*(sizeOfPaperX)+(int)round(ray.P[0]+sizeOfPaperX*.5))] = 0;
                    //outArr[(int)(round(ray.P[1]*3.5-150)*(sizeOfPaperX)+(int)round(ray.P[0]*3.5-400))] = 0;
                    outArr[(int)(round(Y)*(sizeOfPaperX)+(int)round(X))] = 0;
                }else{
                    //TODO
                    printf("doesn't fit\n");
                    //outArr[(int)(round(ray.P[1]+sizeOfPaperY*.5))*(sizeOfPaperX)+(int)round(ray.P[0]+sizeOfPaperX*.5)] = 1; 
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
    for(int j = -5; j<=5;j++){
        for(int i =-5; i<=5;i++){
            //TODO check if goes off page, shouldn't
            //outArr[(int)(round(lightPoint[1]+j)*(sizeOfPaperX)+(int)round(lightPoint[0]+i))] = 0;
        }
    }
    /* printf("\n\n\n"); */
    if(v!=3){
        fprintf(writeFile, "P2\n%i %i\n2\n", sizeOfPaperX, sizeOfPaperY);
        for(int j = 0; j<sizeOfPaperY;j++){
            for(int i = 0; i<sizeOfPaperX;i++){
                if(i == sizeOfPaperX/2 || j == sizeOfPaperY/2){
                    //TODO
                    //fprintf(writeFile, "1 ");
                }else{
                    /* printf("i: %i, j: %i, maxOutSide: %i\n", i, j, maxOutSide); */
                    //printf("%i ", outArr[i*(maxOutSide)+j]);
                    //fprintf(writeFile, "%i ", outArr[j*(sizeOfPaperX)+i]);
                    /* fflush(stdout); */
                }
                fprintf(writeFile, "%i ", outArr[j*(sizeOfPaperX)+i]);
            }
            fprintf(writeFile, "\n");
        }
    }
    fclose(writeFile);

    free(imgArr);
    free(outArr);
    free(newCoordArr);
    free(outlineArr);
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

void findCLeftI(int C[][2], int CLeftI[][3]){
    //https://math.stackexchange.com/questions/79301/left-inverses-for-matrix
    double det;
    det = C[0][0]*C[2][1]-C[0][1]*C[2][0];
    printf("det02: %f\n", det);
    if(det != 0){
        det = 1/det;
        CLeftI[0][0] = C[2][1]*det;
        CLeftI[0][2] = -1*C[0][1]*det;
        CLeftI[1][0] = -1*C[2][0]*det;
        CLeftI[1][2] = C[0][0]*det;

        CLeftI[0][1] = 0;
        CLeftI[1][1] = 0;
    }else{
        det = C[0][0]*C[1][1]-C[0][1]*C[1][0];
        printf("det02 was 0 so det01 is %f\n", det);
        if(det != 0){
            det = 1/det;
            CLeftI[0][0] = C[1][1]*det;
            CLeftI[0][1] = -1*C[0][1]*det;
            CLeftI[1][0] = -1*C[1][0]*det;
            CLeftI[1][1] = C[0][0]*det;

            CLeftI[0][2] = 0;
            CLeftI[1][2] = 0;
        }else{

            det = C[1][0]*C[2][1]-C[1][1]*C[2][0];
            printf("det01 was 0 so det12 is %f\n", det);
            if(det != 0){
                det = 1/det;
                CLeftI[0][1] = C[2][1]*det;
                CLeftI[0][2] = -1*C[1][1]*det;
                CLeftI[1][1] = -1*C[2][0]*det;
                CLeftI[1][2] = C[1][0]*det;

                CLeftI[0][0] = 0;
                CLeftI[1][0] = 0;
            }else{
                printf("Can't find det");
                exit(1);
            }
        }
    }
    if(!v){
        printf("C: \n");
        printf("  [[%d, %d],\n", C[0][0],C[0][1]);
        printf("  [[%d, %d],\n", C[1][0],C[1][1]);
        printf("  [[%d, %d]]\n", C[2][0],C[2][1]);

        printf("Left Inverse: \n");
        printf("  [[%d, %d, %d],\n", CLeftI[0][0],CLeftI[0][1],CLeftI[0][2]);
        printf("  [[%d, %d, %d]]\n", CLeftI[1][0],CLeftI[1][1],CLeftI[1][2]);
    }
    checkThatLeftInverse(C, CLeftI);
}

void checkThatLeftInverse(int C[][2], int CLeftI[][3]){
    double a00, a01, a10, a11;
    a00 = CLeftI[0][0]*C[0][0]+CLeftI[0][1]*C[1][0]+CLeftI[0][2]*C[2][0];
    a01 = CLeftI[0][0]*C[0][1]+CLeftI[0][1]*C[1][1]+CLeftI[0][2]*C[2][1];
    a10 = CLeftI[1][0]*C[0][0]+CLeftI[1][1]*C[1][0]+CLeftI[1][2]*C[2][0];
    a11 = CLeftI[1][0]*C[0][1]+CLeftI[1][1]*C[1][1]+CLeftI[1][2]*C[2][1];
    printf("Left Inverse: \n");
    printf("  [[%d, %d, %d],\n", CLeftI[0][0],CLeftI[0][1],CLeftI[0][2]);
    printf("  [[%d, %d, %d]]\n", CLeftI[1][0],CLeftI[1][1],CLeftI[1][2]);
    printf("checkLeftInverse: \n");
    printf("  [[%f, %f],\n", a00, a01);
    printf("  [[%f, %f]]\n", a10, a11);
    if(!(a00==1 && a01==0 && a10==0 && a11==1)){
        printf("LeftC isn't a left inverse\n");
        exit(1);
    }else{
        printf("Valid LEFT INVERSE\n");
    }
}

void changeBasis(Point *a, int CLeftI[][3]){
    double tmpa0;
    if(!v){
        printf("Left Inverse: \n");
        printf("  [[%d, %d, %d],\n", CLeftI[0][0],CLeftI[0][1],CLeftI[0][2]);
        printf("  [[%d, %d, %d]]\n", CLeftI[1][0],CLeftI[1][1],CLeftI[1][2]);
    }
    tmpa0= CLeftI[0][0]*(*a)[0]+CLeftI[0][1]*(*a)[1]+CLeftI[0][2]*(*a)[2];
    (*a)[1] = CLeftI[1][0]*(*a)[0]+CLeftI[1][1]*(*a)[1]+CLeftI[1][2]*(*a)[2];
    (*a)[2] = 0;
    (*a)[0] = tmpa0;
}
void findMinMax(double X, double Y, double *minMaxX, double *minMaxY){
    if(X < minMaxX[0]){
        minMaxX[0] = X;
    }
    if(X > minMaxX[1]){
        minMaxX[1] = X;
    }
    if(Y < minMaxY[0]){
        minMaxY[0] = Y;
    }
    if(Y > minMaxY[1]){
        minMaxY[1] = Y;
    }
}
