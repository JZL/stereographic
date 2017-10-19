#include <png.h>
struct pngFile{
    int height;
    int width;
    png_byte color_type;
    png_byte bit_depth;
    png_bytep * row_pointers;
};

int read_png_file(char* file_name,   struct pngFile *outPngFile);
int write_png_file(char* file_name, int **row_pointers, struct pngFile *pngFileParam);
