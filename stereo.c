#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "RayPolygon.h"
#include "coords.c" //generated from icosahedron.py, etc

//https://cocalc.com/projects/b0966444-8a50-4f2a-a0a7-5d5e6e261ea5/files/2017-06-21-221142.sagews
void   vectorCross           ( Vector a, Vector b, Vector *c                                         );
double vectorDot             ( Vector a, Vector b                                                    );
void   pointSub              ( Point a, Point b, Vector *c                                           );
double findT                 ( Point *verts, struct Polygon poly, struct Ray ray, int * i_s, Vector N);
void   printVector           ( Vector v, char* name                                                  );
int    maxN                  ( Vector N                                                              );
int    abs                   ( int                                                                   );
void   normalize             ( Vector                                                                );
void   distPoints            ( Point *a, Point *b                                                    );
void   project               ( Point *p, Vector N                                                    );
void   newCoord              ( struct Polygon poly, Point *P, Vector N                               );
void   findCLeftI            ( double C[][2], double CLeftI[][3]                                     );
void   checkThatLeftInverse  ( double C[][2], double CleftI[][3]                                     );
void   changeBasis           ( Point *a, double CLeftI[][3]                                          );
void   findMinMax            ( double X, double Y, double *minMaxX, double *minMaxY                  );
double findMaxZValueCoord ( );
void   generateOutputImage   ( char * imgArr, int **outArr, struct outlineCoord *outlineArr,
                               struct Polygon poly, Point endPoint, int ximg, int yimg, FILE *writeFile );

int v = 1;
int sizeOfPaperY = 5000;
int sizeOfPaperX = 5000;
int numSteps = 400;
int main(int argc, char **argv){
    printf("**Starting stero.c**");
    FILE *fp;
    FILE *writeFile;
    char magicNumStr[2];
    int yimg, ximg;
    int tmpF;


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
    /*
    writeFile = fopen("out/out.pgm", "w");
    if(writeFile == NULL){
        printf("couldn't write to file");
    }
    */
    tmpF = fscanf(fp, "%s", magicNumStr);
    if(!v)printf("tmpF: %i\n", tmpF);
    if (feof(fp) ||tmpF != 1){
        printf("Unexpected error in input data.");
        exit(1);
    }
    //Make sure is bmp
    /* printf("%s\n", magicNumStr); */
    if(strcmp(magicNumStr, "P1")!=0){
        printf("Needs to be pbm\n");
        exit(2);
    }

    //Get x and y width
    tmpF = fscanf(fp, "%i %i\n",&yimg, &ximg );
    printf("tmpF: %i\n", tmpF);
    if (feof(fp)||tmpF != 2){
        printf("Unexpected error in input data.");
        exit(1);
    }
    char *imgArr = malloc(sizeof(int)*yimg*ximg);
    if(!imgArr){
        printf("COULDN'T MALLOC\n");
        printf("%s: %p\n", "imgArr", imgArr);
        exit(1);
    }
    //Read in all the image at once!
    tmpF = fread(imgArr, sizeof(int)*(ximg), (yimg), fp);
    printf("tmpF: %i %i, %i, feof(fp): %i\n", tmpF,ximg, yimg, feof(fp));
    if(!feof(fp) ||tmpF!=yimg/4){
        //want it be the end of file
        printf("Unexpected error in input data.");
        exit(1);
    }
    if(!v)printf("--\n");
    int mult = 100;
    //cat coords.c |awk -F, '{print $3}'|sort
    double maxZValue = 0;
    maxZValue = findMaxZValueCoord();
    printf("Max ZValue: %f\n", maxZValue);

    //Can change 0->1 so top of light isn't right in middle, untested
    Point endPoint = {ximg/2, yimg/2, ((maxZValue+0)*mult)};
    //Point endPoint   = {ximg/2, yimg/2, 2000};
    struct Polygon poly; 
    poly.n           = 3;
    poly.interpolate = false;

    for(int i = 0; i<(numFaces*(poly.n+1));i++){
        if(i%(numSides+1) == 0){
            printf("faceIDs: %f, %f, %f\n", faces[i][0], faces[i][1], faces[i][2]);
        }else{
            faces[i][0]*=mult;
            faces[i][1]*=mult;
            faces[i][2]*=mult;

            faces[i][0]+=ximg/2;
            faces[i][1]+=yimg/2;
            faces[i][2]+=0.001; //so off ground
            printVector(faces[i], "face");
        }
    }

    //int *outArr = malloc(sizeof(int)*sizeOfPaperX*sizeOfPaperY);
    int **outArr = malloc(sizeof(int*)*sizeOfPaperY);
    if(outArr == NULL){
        printf("Couldn't malloc outer of 2D array\n");
        exit(1);
    }
    for(int r = 0; r<sizeOfPaperY;r++){
       outArr[r] = malloc(sizeof(int)*sizeOfPaperX);
       if(outArr[r] == NULL){
           printf("Couldn't malloc inner of 2D array\n");
           exit(1);
       }
    }
    struct outlineCoord *outlineArr = malloc(sizeof(struct outlineCoord)*(poly.n)*numSteps);
    if(outlineArr == NULL){
        printf("COULDN'T MALLOC\n");
        printf("%s: %p\n",  "outlineArr", outlineArr);
        fflush(stdout);
        exit(999);
    }

    char fileName[20];
    char faceNumber[4];
    for(int i = 0; i<numFaces;i++){
        poly.coordIds = faces[i*4];
        poly.V = &faces[i*4+1];

        snprintf(faceNumber, 4, "%d", i);
        strcpy(fileName, "out/");
        strcat(fileName, faceNumber);
        strcat(fileName, ".pgm");
        writeFile = fopen(fileName, "w");
        if(writeFile == NULL){
            printf("couldn't write to file");
        }
        printf("Face %02d/%d\n", i, numFaces-1);
        generateOutputImage(imgArr, outArr, outlineArr, poly, endPoint, ximg, yimg, writeFile);
        fclose(writeFile);
        printf("----\n");
    }

    free(imgArr);
    for(int r = 0; r<sizeOfPaperY;r++){
        free(outArr[r]);
    }
    free(outArr);
    free(outlineArr);
    fclose(fp);
    return 0;
}


void generateOutputImage(char * imgArr, int **outArr, struct outlineCoord
        *outlineArr, struct Polygon poly, Point endPoint, int ximg, int yimg,
        FILE *writeFile){

    Vector N;
    double minMaxX[2];
    double minMaxY[2];
    Point *verts = poly.V;

    int firstTime = 1; //for findingMinMax, if first run through loop
    //can't be i && j == 0 bc the value at [i][j] might not be == 1
    //Need duplicate to do minMax & scale calculations on (allows to not have negative coordinate values)
    //TODO could just do the minMax for the surrounding shape, if is less than that, won't intersect
    for(int i = 0; i<sizeOfPaperY;i++){
        for(int j = 0; j<sizeOfPaperX;j++){
            outArr[i][j] = 2;
        }
    }



    double t;
    bool intersects;
    int i_s[2];
    struct Ray ray;

    Vector v0v1, v0v2, secondAxis;
    pointSub(verts[1], verts[0], &v0v1);
    pointSub(verts[2], verts[0], &v0v2);

    vectorCross(v0v1, v0v2, &N);

    normalize(v0v1);
    normalize(N);

    vectorCross(v0v1, N, &secondAxis);
    normalize(secondAxis);
    if(!v){
        printVector(secondAxis, "secondAxisNorm");
        printVector(secondAxis, "secondAxis");
        printVector(v0v1, "v0v1");
        printVector(v0v1, "v0v1Norm");
        printVector(N, "N");
        printVector(N, "NNorm");
    }

    double C[3][2];
    double CLeftI[3][3];
    C[0][0] =v0v1[0]; 
    C[1][0] =v0v1[1]; 
    C[2][0] =v0v1[2]; 

    C[0][1] =secondAxis[0]; 
    C[1][1] =secondAxis[1]; 
    C[2][1] =secondAxis[2]; 

    findCLeftI(C, CLeftI);



    Point lightPoint = {endPoint[0], endPoint[1], endPoint[2]};
    if(!v)printVector(lightPoint, "lightPointBef");
    changeBasis(&lightPoint, CLeftI);
    if(!v)printVector(lightPoint, "lightPointAft");
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

    //The points need to be in the circular order
    Point a, b;
    Vector m;
    for(int i = 0; i<(poly.n); i++){
        a[0] = verts[i][0];
        a[1] = verts[i][1];
        a[2] = verts[i][2];

        b[0] = verts[(i+1)%poly.n][0];
        b[1] = verts[(i+1)%poly.n][1];
        b[2] = verts[(i+1)%poly.n][2];

        if(!v)printf("\ngoing between %i and %i\n", i, (i+1)%poly.n);
        if(!v){
            printVector(a, "vertA");
            printVector(b, "vertB");
        }

        changeBasis(&a, CLeftI);
        changeBasis(&b, CLeftI);

        if(!v){
            printVector(a, "vertA");
            printVector(b, "vertB");
        }

        pointSub(a, b, &m);
        if(!v)printVector(m, "m");

        for(int t = 0; t<numSteps;t++){
            double newX = a[0]+m[0]*((float)t/numSteps);
            double newY = a[1]+m[1]*((float)t/numSteps);

            if(firstTime == 1){
                //set minMax with init values
                minMaxX[0] = newX;
                minMaxX[1] = newX;
                minMaxY[0] = newY;
                minMaxY[1] = newY;
                firstTime = 0;
            }
            int outLineIndex = numSteps*i+t;
            findMinMax(newX, newY, minMaxX, minMaxY);
            outlineArr[outLineIndex].X       = newX;
            outlineArr[outLineIndex].Y       = newY;
            outlineArr[outLineIndex].coordID = 1;
        }
    }
    printf("minX: %f, minY: %f\n", minMaxX[0], minMaxY[0]);
    if(minMaxX[1] - minMaxX[0] > sizeOfPaperX){
        printf("WILL OVERFLOW IN X DIREACTION\n");
    }
    if(minMaxY[1] - minMaxY[0] > sizeOfPaperY){
        printf("WILL OVERFLOW IN Y DIREACTION\n");
    }

    for(int i = 0; i<(poly.n*numSteps);i++){
        double newX, newY; //X and Y which are min/maxed so > 0
        newX =  round(outlineArr[i].X-minMaxX[0]);
        newY =  round(outlineArr[i].Y-minMaxY[0]);
        if( newX>=sizeOfPaperX||newX<0||
            newY>=sizeOfPaperY||newY<0){
            //If this happens, is not going to show whole thing
            printf("outline extends past outArr!\n");
            exit(1);
            continue;
        }
        /* If want to debug which face is on (and what polarity), set coordID
         * above to be something like ((i*1000+9)+(i+1)%poly.n) to give xxx9yyy
         * for the verts) and then make below = outlineArr[i].coordID;
         */
        outArr[(int)(newY)][(int)(newX)] = 0; // = black
    }
    for(int i = 0; i<yimg;i++){
        for(int j = 0; j<ximg;j++){
            int X = 0;
            int Y = 0;
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
                    printVector(ray.O, "O  ");
                    printVector(endPoint, "End");
                    printVector(ray.D, "D  ");
                }

                t = findT(verts, poly, ray, i_s, N);
                if(!v)printf("t: %f\n", t);
                if(t<=0){
                    //either error or intersection behind origin, reject
                    if(!v)printf("t<=0\n"); 
                    continue;
                } 
                if(!v)printf("i_1: %i, i_2:%i\n", i_s[0], i_s[1]); 

                intersects = intersect(&poly, &ray, t, i_s[0], i_s[1], v);

                if(!v){ printVector(ray.P, "rayPB");}
                if(v==4){
                    fprintf(writeFile, "%f,%f,%f\n", ray.P[0], ray.P[1], ray.P[2] ); 
                }
                //distPoints(&(verts[0]), &ray.P);
                //newCoord(poly, &ray.P, N);
                //TODO move to only if intersects
                changeBasis(&ray.P, CLeftI);
                if(!v) printVector(ray.P, "rayPA");
                if(!v) printf(".5intersects? %d\n", intersects);
                if(!v) printVector(ray.P, "rayPAfterAdd");
                if(intersects == 1){
                    if(!v)printf("=");
                    X = ray.P[0];
                    Y = ray.P[1];
                }else{
                    //Already x=y=0. there will be an closed spot at 0,0 but, otherwise, need to store ximg*yimg*sizeof(char OR int) extra bytes to say if intersects
                    if(!v)printf("-");
                }
            }
            double finalX, finalY;
            finalX = X;
            finalY = Y;

            finalX = finalX-minMaxX[0];
            finalY = finalY-minMaxY[0];
            if(v==5)printf("FINAL COORD: (%f, %f)\n", finalX, finalY);
            if(!(finalX==0 && finalY==0)){
                if(finalX>=0 && finalX< sizeOfPaperX  && finalY>=0 && finalY<sizeOfPaperY){
                    //outArr[(int)(ray.P[1])*(maxOutSide)+(int)(ray.P[0])] = 1;
                    /* if(!v) printf("coords [%i, %i]\n", (int)(round(ray.P[1]+sizeOfPaperY*.5)),(int)round(ray.P[0]+sizeOfPaperX*.5)); */
                    /* outArr[(int)round(ray.P[1]+maxOutSide*.5)*(maxOutSide)+(int)round(ray.P[0]+maxOutSide*.5)] = 0; */

                    //outArr[(int)(round(ray.P[1]*1.5+sizeOfPaperY*.5+200)*(sizeOfPaperX)+(int)round(ray.P[0]*2+sizeOfPaperX*.5-200))] = 0;
                    //outArr[(int)(round((ray.P[1]+200)*1.5+sizeOfPaperY*.5)*(sizeOfPaperX)+(int)round(ray.P[0]+sizeOfPaperX*.5))] = 0;
                    //outArr[(int)(round(ray.P[1]*3.5-150)*(sizeOfPaperX)+(int)round(ray.P[0]*3.5-400))] = 0;
                    outArr[(int)(finalY)][(int)(finalX)] = 0;
                    if(!v)printf("@");
                }else{
                    if(!v)printf("Doesn't fit coord\n");
                    if(!v)printf("-");
                    //outArr[(int)(round(ray.P[1]+sizeOfPaperY*.5))*(sizeOfPaperX)+(int)round(ray.P[0]+sizeOfPaperX*.5)] = 1; 
                }
            }
        }
        //wanted for the "-"/"@" quick show of how many points are hit/missed
        if(!v)printf("---\n"); 
    }
    if(v!=4){
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
                fprintf(writeFile, "%i ", outArr[j][i]);
            }
            fprintf(writeFile, "\n");
        }
    }
}

double findT(Point *verts, struct Polygon poly, struct Ray ray, int *i_s, Vector N){
    double d, t, N_dot_D;
    int maxNi;
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

    maxNi = maxN(N);
    if(!v){
        printf("maxNi: %d\n", maxNi);
        printf("N_S: %f, %f, %f\n", N[0], N[1], N[2]);
    }
    switch(maxNi){
        case 0:
            i_s[0] = 1;
            i_s[1] = 2;
            break;
        case 1:
            i_s[0] = 0;
            i_s[1] = 2;
            break;
        case 2:
            i_s[0] = 0;
            i_s[1] = 1;
            break;
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
        return 0;
    }else if(absN1 >= absN2 && absN1>=absN2){
        return 1;
    }else{
        return 2;
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

void findCLeftI(double C[][2], double CLeftI[][3]){
    //https://math.stackexchange.com/questions/79301/left-inverses-for-matrix
    double det;
    double eps = 0.01;
    det = C[0][0]*C[2][1]-C[0][1]*C[2][0];
    if(!v)printf("det02: %f\n", det);
    if(fabs(0-det) > eps){
        det = 1/det;
        CLeftI[0][0] =    C[2][1]*det;
        CLeftI[0][2] = -1*C[0][1]*det;
        CLeftI[1][0] = -1*C[2][0]*det;
        CLeftI[1][2] =    C[0][0]*det;

        CLeftI[0][1] = 0;
        CLeftI[1][1] = 0;
    }else{
        det = C[0][0]*C[1][1]-C[0][1]*C[1][0];
        if(!v)printf("det02 was 0 so det01 is %f\n", det);
        if(fabs(0-det) > eps){
            det = 1/det;
            CLeftI[0][0] =    C[1][1]*det;
            CLeftI[0][1] = -1*C[0][1]*det;
            CLeftI[1][0] = -1*C[1][0]*det;
            CLeftI[1][1] =    C[0][0]*det;

            CLeftI[0][2] = 0;
            CLeftI[1][2] = 0;
        }else{

            det = C[1][0]*C[2][1]-C[1][1]*C[2][0];
            if(!v)printf("det01 was 0 so det12 is %f\n", det);
            if(fabs(0-det) > eps){
                det = 1/det;
                CLeftI[0][1] =    C[2][1]*det;
                CLeftI[0][2] = -1*C[1][1]*det;
                CLeftI[1][1] = -1*C[2][0]*det;
                CLeftI[1][2] =    C[1][0]*det;

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
        printf("  [[%f, %f],\n", C[0][0],C[0][1]);
        printf("  [[%f, %f],\n", C[1][0],C[1][1]);
        printf("  [[%f, %f]]\n", C[2][0],C[2][1]);

        printf("Left Inverse: \n");
        printf("  [[%f, %f, %f],\n", CLeftI[0][0],CLeftI[0][1],CLeftI[0][2]);
        printf("  [[%f, %f, %f]]\n", CLeftI[1][0],CLeftI[1][1],CLeftI[1][2]);
    }
    checkThatLeftInverse(C, CLeftI);
}

void checkThatLeftInverse(double C[][2], double CLeftI[][3]){
    double eps = 0.01;
    double a00, a01, a10, a11;
    a00 = CLeftI[0][0]*C[0][0]+CLeftI[0][1]*C[1][0]+CLeftI[0][2]*C[2][0]+0.0;
    a01 = CLeftI[0][0]*C[0][1]+CLeftI[0][1]*C[1][1]+CLeftI[0][2]*C[2][1]+0.0;
    a10 = CLeftI[1][0]*C[0][0]+CLeftI[1][1]*C[1][0]+CLeftI[1][2]*C[2][0]+0.0;
    a11 = CLeftI[1][0]*C[0][1]+CLeftI[1][1]*C[1][1]+CLeftI[1][2]*C[2][1]+0.0;
    if(!v){
        printf("Left Inverse: \n");
        printf("  [[%f, %f, %f],\n", CLeftI[0][0],CLeftI[0][1],CLeftI[0][2]);
        printf("  [[%f, %f, %f]]\n", CLeftI[1][0],CLeftI[1][1],CLeftI[1][2]);
        printf("checkLeftInverse: \n");
        printf("  [[%a, %a],\n", a00, a01);
        printf("  [[%a, %a]]\n", a10, a11);
    }
    if(!((fabs(a00-1)<eps && fabs(a01-0.0)<eps && fabs(a10-0.0)<eps && fabs(a11-1.0)<eps))){
        printf("LeftC isn't a left inverse\n");
        exit(1);
    }else{
        printf("Valid LEFT INVERSE\n");
    }
}

void changeBasis(Point *a, double CLeftI[][3]){
    double tmpa0;
    if(!v){
        printf("Left Inverse: \n");
        printf("  [[%f, %f, %f],\n", CLeftI[0][0],CLeftI[0][1],CLeftI[0][2]);
        printf("  [[%f, %f, %f]]\n", CLeftI[1][0],CLeftI[1][1],CLeftI[1][2]);
    }
    tmpa0= CLeftI[0][0]*(*a)[0]+CLeftI[0][1]*(*a)[1]+CLeftI[0][2]*(*a)[2];
    (*a)[1] = CLeftI[1][0]*(*a)[0]+CLeftI[1][1]*(*a)[1]+CLeftI[1][2]*(*a)[2];
    (*a)[0] = tmpa0;
    (*a)[2] = 0;
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
double findMaxZValueCoord(){
    float maxZValue;
    float tmpCompareVal;
    //Get the Z (3rd) value of the 1st row with coords
    maxZValue = faces[1][2];
    for(int i = 0; i<numFaces;i++){
        //Want to skip 1st because gives the coord id
        //Want to go <=numFaces because the first line is the faceVertId
        //and then has numSides more
        for(int j = 1; j<=numSides;j++){
            tmpCompareVal = faces[i*(numSides+1)+j][2];
            if(maxZValue < tmpCompareVal){
                maxZValue = tmpCompareVal;
            }
        }
    }
    return maxZValue;
}
