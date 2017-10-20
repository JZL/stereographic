#include <png.h>
#include "RayPolygon.h"

struct thread_in {
    pthread_t TID;
    png_bytep * imgArr;
    struct    outlineCoord *outlineArr;
    int       faceIndex;
    Point     *endPoint;
    int       ximg;
    int       yimg;
    png_byte  color_type;
    png_byte  bit_depth;
};

struct pngFile{
    int width;
    int height;
    png_byte color_type;
    png_byte bit_depth;
    png_bytep * row_pointers;
};

int read_png_file(char* file_name,   struct pngFile *outPngFile);
int write_png_file(char* file_name, int **row_pointers, int width, int height, png_byte bit_depth, png_byte color_type);
