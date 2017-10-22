#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
//Included in managePNG.h
//#include "RayPolygon.h"
#include "managePNG.h"
#include "coords.c" //generated from icosahedron.py, etc

//https://cocalc.com/projects/b0966444-8a50-4f2a-a0a7-5d5e6e261ea5/files/2017-06-21-221142.sagews
void   vectorCross          ( Vector a,            Vector b,            Vector *c                            ) ;
double vectorDot            ( Vector a,            Vector b                                                  ) ;
void   pointSub             ( Point a,             Point b,             Vector *c                            ) ;
double findT                ( Point *verts,        struct Polygon poly, struct Ray ray,  int * i_s, Vector N ) ;
void   printVector          ( Vector v,            char* name                                                ) ;
int    maxN                 ( Vector N                                                                       ) ;
int    abs                  ( int                                                                            ) ;
void   normalize            ( Vector                                                                         ) ;
void   distPoints           ( Point *a,            Point *b                                                  ) ;
void   project              ( Point *p,            Vector N                                                  ) ;
void   newCoord             ( struct Polygon poly, Point *P,            Vector N                             ) ;
int    findCLeftI           ( double C[][2],       double CLeftI[][3]                                        ) ;
int    checkThatLeftInverse ( double C[][2],       double CleftI[][3]                                        ) ;
void   changeBasis          ( Point *a,            double CLeftI[][3]                                        ) ;
void   findMinMax           ( double X,            double Y,            double *minMaxX, double *minMaxY     ) ;
double findMaxZValueCoord   () ;
void*  generateOutputImage  ( void* threadStructVoid                                                         ) ;
void   runThreadSession(int startIndex, int threads_per_session, struct thread_in *threadStructs);
void   printAndExit         ( const char* str                                                                ) ;
void   pthread_printAndExit         ( const char* str                                                                ) ;

//Important global vars to change functionality:
int v            = 1; //How verbose

int hasColor     = 1;//0 for b/w (input pgm is only 1/0, 1 for color (input ppm has 9 chars p/ RRRGGGBBB)

int numSteps     = 2000; //Used for outline of image, 0 removes, greater then number, the darker the outline

int threads_per_session = 12;

int main(int argc, char **argv){
    printf("**Starting stero.c**\n");
    int yimg, ximg;

    struct pngFile readPngFile;


    if(argc >1){
        v = strtol(argv[1], NULL, 10);
        printf("new v: %i\n", v);
    }
    //sed '2,$s/\s//g' simple.bpm |tr -d "\n" > test.smaller.pbm
    //and then go in and and new lines to the header
    char inFileName[] = "cropped.png";
    if(read_png_file(inFileName, &readPngFile)!=0){
        printAndExit("Problem reading png file...exiting");
        //TODO check RGBA
    }
    yimg = readPngFile.height;
    ximg = readPngFile.width;
    printf("ximg: %i, yimg: %i\n", ximg, yimg);

    int mult = ximg/6;
    //cat coords.c |awk -F, '{print $3}'|sort
    double maxZValue = 0;
    maxZValue = findMaxZValueCoord();
    if(!v)printf("Max ZValue: %f\n", maxZValue);

    //Can change 0->1 so top of light isn't right in middle, untested
    Point endPoint = {ximg/2, yimg/2, ((maxZValue)*mult)};
    if(!v)printVector(endPoint, "endPoint");
    //Point endPoint   = {ximg/2, yimg/2, 2000};

    for(int i = 0; i<(numFaces*(numSides+1));i++){
        if(i%(numSides+1) == 0){
            if(!v)printf("faceIDs: %f, %f, %f\n", faces[i][0], faces[i][1], faces[i][2]);
        }else{
            faces[i][0]*=mult;
            faces[i][1]*=mult;
            faces[i][2]*=mult;

            faces[i][0]+=ximg/2;
            faces[i][1]+=yimg/2;
            faces[i][2]+=0.5; //so off ground
            if(!v)printVector(faces[i], "face");
        }
    }
    //int *outArr = malloc(sizeof(int)*sizeOfPaperX*sizeOfPaperY);
    struct thread_in *threadStructs = NULL;
    threadStructs = malloc(sizeof(struct thread_in)*numFaces);
    if(threadStructs == NULL){
        printAndExit("Couldn't malloc threadStructs");
    }
    for(int i = 0; i<numFaces;i++){
        threadStructs[i].imgArr     = readPngFile.row_pointers;
        threadStructs[i].faceIndex  = i;
        threadStructs[i].endPoint   = &endPoint;
        threadStructs[i].ximg       = ximg;
        threadStructs[i].yimg       = yimg;
        threadStructs[i].color_type = readPngFile.color_type;
        threadStructs[i].bit_depth  = readPngFile.bit_depth;
    }

    for(int tSession = 0; tSession<numFaces;tSession+=threads_per_session){
        if(tSession + threads_per_session >= numFaces){
            threads_per_session = numFaces - tSession;
            printf("%i\n", numFaces - tSession);
        }
        runThreadSession(tSession, threads_per_session, threadStructs);
    }
    //Since numFaces and threads_per_session are ints, int division floors correctly
    /*
    int tLeftOverStart = threads_per_session*(numFaces/threads_per_session);
    runThreadSession(tLeftOverStart, (numFaces - tLeftOverStart), threadStructs);
    */
    printf("Done all threads\n");
    for(int y=0; y<readPngFile.height; y++){
        free(readPngFile.row_pointers[y]);
    }
    free(readPngFile.row_pointers);

    free(threadStructs);
    return 0;
}

void runThreadSession(int startIndex, int threads_per_session, struct thread_in *threadStructs){
    int ret;
    printf("Starting tsession %i - %i\n", startIndex, startIndex+threads_per_session);
    for(int i = startIndex; i<(startIndex+threads_per_session);i++){
        printf("Creating thread %i\n", i);
        ret = pthread_create(&(threadStructs[i].TID),  NULL, generateOutputImage, &(threadStructs[i]));
        if(ret != 0){
            printf("Couldn't create thread %i", i);
            //Still fflush's so useful
            printAndExit("Couldn't create thread");
        }
    }


    for(int i = startIndex; i<(startIndex+threads_per_session);i++){
        printf("Joining thread %i\n", i);
        //NEEDS TO BE LONG, FORCED CASTED SO IF INT, OVERFLOWS!
        long tret;
        int error_code;
        error_code = pthread_join(threadStructs[i].TID, (void **)&tret);
        if(error_code != 0){
            printAndExit("Problem joining thread\n");
        }
        //Error in tret
        printf("     %02d    RETURNED : %ld\n", i, tret);
        if(tret!=0){
            printAndExit("Not 0...exiting\n");
        }
    }
}


void*   generateOutputImage   ( void* threadStructVoid){
    printf("Got to generateOutput\n");
    png_bytep * imgArr  = ((struct thread_in*) threadStructVoid)->imgArr;
    int faceIndex       = ((struct thread_in*) threadStructVoid)->faceIndex;
    Point *endPoint     = ((struct thread_in*) threadStructVoid)->endPoint;
    int ximg            = ((struct thread_in*) threadStructVoid)->ximg;
    int yimg            = ((struct thread_in*) threadStructVoid)->yimg;
    png_byte bit_depth  = ((struct thread_in*) threadStructVoid)->bit_depth;
    png_byte color_type = ((struct thread_in*) threadStructVoid)->color_type;

    printf("ximg: %i, yimg: %i\n", ximg, yimg);
    struct outlineCoord *outlineArr = malloc(sizeof(struct outlineCoord)*(numSides)*numSteps);
    if(outlineArr == NULL){
        pthread_printAndExit("COULDN'T MALLOC\n");
    }
    Vector N;
    double minMaxX[2];
    double minMaxY[2];
    char fileName[20];
    char faceNumber[4];

    struct Polygon poly; 
    poly.n           = numSides;
    poly.interpolate = false;
    poly.coordIds    = faces[faceIndex*4];
    poly.V           = &faces[faceIndex*4+1];
    Point *verts     = poly.V;

    snprintf(faceNumber, 4, "%d", faceIndex);
    strcpy(fileName, "out/");
    strcat(fileName, faceNumber);
    if(hasColor == 0){
        strcat(fileName, ".pgm");
    }else{
        strcat(fileName, ".png");
    }
    printf("Face %02d/%d Start\n", faceIndex, numFaces-1);

    int firstTime = 1; //for findingMinMax, if first run through loop
    //can't be i && j == 0 bc the value at [i][j] might not be == 1
    //Need duplicate to do minMax & scale calculations on (allows to not have negative coordinate values)
    //TODO could just do the minMax for the surrounding shape, if is less than that, won't intersect



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
    C[0][0] = v0v1[0];
    C[1][0] = v0v1[1];
    C[2][0] = v0v1[2];

    C[0][1] = secondAxis[0];
    C[1][1] = secondAxis[1];
    C[2][1] = secondAxis[2];

    //Both runs findCLeftI and checks that is equal to 0
    if(findCLeftI(C, CLeftI) != 0){
        pthread_printAndExit("Couldn't find CLeftI");
    }



    Point lightPoint = {(*endPoint)[0], (*endPoint)[1], (*endPoint)[2]};
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
    for(int i = 0; i<(numSides); i++){
        a[0] = verts[i][0];
        a[1] = verts[i][1];
        a[2] = verts[i][2];

        b[0] = verts[(i+1)%numSides][0];
        b[1] = verts[(i+1)%numSides][1];
        b[2] = verts[(i+1)%numSides][2];

        if(!v)printf("\ngoing between %i and %i\n", i, (i+1)%numSides);
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
            //outlineArr[outLineIndex].coordID = 1;
        }
    }
    int minWidth[2] = {ceil(minMaxX[1] - minMaxX[0])+10,
                       ceil(minMaxY[1] - minMaxY[0])+10};
    if(!v)printf("ximg: %i, yimg: %i\n",              ximg,        yimg);
    if(!v)printf("   minWidthX: %i, minWidthY: %i\n", minWidth[0], minWidth[1]);
    if(!v)printf("   minX: %f, minY: %f\n",           minMaxX[0],  minMaxY[0]);
    if(!v)printf("   maxX: %f, maxY: %f\n",           minMaxX[1],  minMaxY[1]);
    if(minWidth[0] > ximg){
        pthread_printAndExit("WILL OVERFLOW IN X DIREACTION\n");
    }
    if(minWidth[1] > yimg){
        pthread_printAndExit("WILL OVERFLOW IN Y DIREACTION\n");
    }

    int **outArr = NULL;
    //Add a 10 bucket buffer for round overflow
    outArr = malloc(sizeof(int*)*minWidth[1]);
    if(outArr == NULL){
        pthread_printAndExit("Couldn't malloc outer of 2D array\n");
    }
    for(int r = 0; r<minWidth[1];r++){
        //Add a 10 bucket buffer for round overflow
       outArr[r] = malloc(sizeof(int)*minWidth[0]);
       if(outArr[r] == NULL){
           pthread_printAndExit("Couldn't malloc inner of 2D array\n");
       }
    }
    for(int i = 0; i<minWidth[1];i++){
        for(int j = 0; j<minWidth[0];j++){
            if(hasColor == 0){
                //In pgm, maxValue(=2 in this case) is white
                outArr[i][j] = 2;
            }else{
                //In ppm, white = 255 255 255 = 255255255when I compress together
                //Could be just 0 but the compiler will fix and this reminds me
                outArr[i][j] = 255255255;
            }
        }
    }
    for(int i = 0; i<(numSides*numSteps);i++){
        double newX, newY; //X and Y which are min/maxed so > 0
        newX =  round(outlineArr[i].X-minMaxX[0])+1;
        newY =  round(outlineArr[i].Y-minMaxY[0])+1;
        if( newX>=minWidth[0]||newX<0||
            newY>=minWidth[1]||newY<0){
            //If this happens, is not going to show whole thing
            printf("outline extends past outArr! %i\n", faceIndex );
            pthread_printAndExit("");
        }
        /* If want to debug which face is on (and what polarity), set coordID
         * above to be something like ((i*1000+9)+(i+1)%poly.n) to give xxx9yyy
         * for the verts) and then make below = outlineArr[i].coordID;
         */
        outArr[(int)(newY)][(int)(newX)] = 0; // = black
    }
    double finalX, finalY;
    int   color = 9999; //from charColor with atoi
    png_byte *pixel = NULL;
    for(int i = 0; i<yimg;i++){
        for(int j = 0; j<ximg;j++){
            int X = 0;
            int Y = 0;
            if(hasColor == 1){
                //Makes RRRGGGBBB
                //printf("%i, %i\n", i, j);
                pixel = &(imgArr[i][j*3]);
                //RGBA
                //From makeImg, should only be RGB not RGBA. Pretty sure will be 8 bit TOOD
                color = pixel[0]*1000000+
                        pixel[1]*1000+
                        pixel[2];
                if(!v){
                    printf("color: %i\n", color);
                }
            }
            if((hasColor == 1 && color != 255255255)){
                //Is white so skip
                ray.O[0] = j;//x
                ray.O[1] = i;//y
                ray.O[2] = 0;//z

                pointSub(ray.O, *endPoint, &ray.D);
                if(!v)printf("rayDlengthB: %f\n", (ray.D[0]*ray.D[0]+ray.D[1]*ray.D[1]+ray.D[2]*ray.D[2]));
                normalize(ray.D);
                if(!v)printf("rayDlengthA: %f\n", (ray.D[0]*ray.D[0]+ray.D[1]*ray.D[1]+ray.D[2]*ray.D[2]));
                if(!v){
                    printVector(ray.O, "O  ");
                    printVector(*endPoint, "End");
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
                /*
                   if(v==4){
                   fprintf(writeFile, "%f,%f,%f\n", ray.P[0], ray.P[1], ray.P[2] );
                   }
                   */
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
            finalX = round(X-minMaxX[0])+1;
            finalY = round(Y-minMaxY[0])+1;
            if(!v)printf("FINAL COORD: (%f, %f)\n", finalX, finalY);
            if(!(finalX==0 && finalY==0)){
                if(finalX>=0 && finalX< minWidth[0]  && finalY>=0 && finalY<minWidth[1]){
                    //outArr[(int)(ray.P[1])*(maxOutSide)+(int)(ray.P[0])] = 1;
                    /* if(!v) printf("coords [%i, %i]\n", (int)(round(ray.P[1]+sizeOfPaperY*.5)),(int)round(ray.P[0]+sizeOfPaperX*.5)); */
                    /* outArr[(int)round(ray.P[1]+maxOutSide*.5)*(maxOutSide)+(int)round(ray.P[0]+maxOutSide*.5)] = 0; */

                    //outArr[(int)(round(ray.P[1]*1.5+sizeOfPaperY*.5+200)*(sizeOfPaperX)+(int)round(ray.P[0]*2+sizeOfPaperX*.5-200))] = 0;
                    //outArr[(int)(round((ray.P[1]+200)*1.5+sizeOfPaperY*.5)*(sizeOfPaperX)+(int)round(ray.P[0]+sizeOfPaperX*.5))] = 0;
                    //outArr[(int)(round(ray.P[1]*3.5-150)*(sizeOfPaperX)+(int)round(ray.P[0]*3.5-400))] = 0;
                    if(hasColor == 0){
                        outArr[(int)(finalY)][(int)(finalX)] = 23;
                    }else{
                        outArr[(int)(finalY)][(int)(finalX)] = color;
                    }
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
        if(write_png_file(fileName, outArr, minWidth[0],
                    minWidth[1],  bit_depth,
                    color_type)!=0){
            pthread_printAndExit("Problem writing png file");
        }
    }

    for(int r = 0; r<minWidth[1];r++){
        free(outArr[r]);
    }
    free(outArr);
    free(outlineArr);
    printf("     %02d    Finished\n", faceIndex);
    pthread_exit(NULL);
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

int findCLeftI(double C[][2], double CLeftI[][3]){
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
                return 1; //0 is a good value, 1 is bad
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
        //Probably don't want to waste on checking inverse
        return checkThatLeftInverse(C, CLeftI);
    }
    return 0;
}

int checkThatLeftInverse(double C[][2], double CLeftI[][3]){
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
        return 1; //1 is good, 0 is bad
    }else{
        if(!v)printf("Valid LEFT INVERSE\n");
        return 0;
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

void printAndExit(const char* str){
    printf("%s", str);
    fflush(stdout);
    exit(1);
}
void pthread_printAndExit(const char* str){
    printf("%s", str);
    fflush(stderr);
    fflush(stdout);
    pthread_exit((void *)2);
}
