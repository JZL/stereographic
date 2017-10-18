/*
#include <stdarg.h>
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 * (http://zarb.org/~gc/html/libpng.html)
 */

//TODO: deal with RGBA val

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PNG_DEBUG 3
#include <png.h>

int x, y;

/*
   int width, height;
   png_byte color_type;
   png_byte bit_depth;

//int number_of_passes;
png_bytep * row_pointers;
 */


struct pngFile{
    int height;
    int width;
    png_byte color_type;
    png_byte bit_depth;
    png_bytep * row_pointers;
};

//Return 0 for success, 1 for error
int read_png_file(char* file_name,   struct pngFile *outPngFile){
    png_structp png_ptr;
    png_infop info_ptr;
    char header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp){
        printf("[read_png_file] File %s could not be opened for reading", file_name);
        return -1;
    }
    if(fread(header, 1, 8, fp)!=8){
        printf("[read_png_file] File %s's header could not be opened for reading", file_name);
        return -1;
    }
    if (png_sig_cmp(header, 0, 8)){
        ("[read_png_file] File %s is not recognized as a PNG file", file_name);
        return -1;
    }


    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr){
        printf("[read_png_file] png_create_read_struct failed");
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr){
        printf("[read_png_file] png_create_info_struct failed");
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))){
        printf("[read_png_file] Error during init_io");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        fclose(fp);
        return -1;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    outPngFile->width = png_get_image_width(png_ptr, info_ptr);
    outPngFile->height = png_get_image_height(png_ptr, info_ptr);
    outPngFile->color_type = png_get_color_type(png_ptr, info_ptr);
    outPngFile->bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    /* Uneeded - TODO: needed for output if want to be identical to input
       number_of_passes = png_set_interlace_handling(png_ptr);
       png_read_update_info(png_ptr, info_ptr);
     */


    /* read file */
    if (setjmp(png_jmpbuf(png_ptr))){
        printf("[read_png_file] Error during read_image");
        return -1;
    }

    outPngFile->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * (outPngFile->height));
    for (y=0; y<(outPngFile->height); y++){
        outPngFile->row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
    }

    //If going to read multiple rows at once, need to deal with interlacing
    png_read_image(png_ptr, outPngFile->row_pointers);

    png_read_end(png_ptr, (png_infop)NULL);

    png_destroy_read_struct(&png_ptr, &info_ptr,
            (png_infopp)NULL);

    fclose(fp);
    return 0;
}


int write_png_file(char* file_name, png_bytep *row_pointers, struct pngFile *pngFileParam){

    png_structp png_ptr;
    png_infop info_ptr;
    //create file
    FILE *fp = fopen(file_name, "wb");
    if (!fp){
        printf("[write_png_file] File %s could not be opened for writing", file_name);
        return -1;
    }


    // initialize stuff
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr){
        printf("[write_png_file] png_create_write_struct failed");
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr){
        printf("[write_png_file] png_create_info_struct failed");
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))){
        printf("[write_png_file] Error during init_io");
        return -1;
    }

    png_init_io(png_ptr, fp);


    // write header
    if (setjmp(png_jmpbuf(png_ptr))){
        printf("[write_png_file] Error during writing header");
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -1;
    }

    //TODO: can set title, author, fun
    png_set_IHDR(
            png_ptr, info_ptr, 
            pngFileParam->width,
            pngFileParam->height,
            pngFileParam->bit_depth,
            pngFileParam->color_type,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);


    // write bytes
    if (setjmp(png_jmpbuf(png_ptr))){
        printf("[write_png_file] Error during writing bytes");
        return -1;
    }

    png_write_image(png_ptr, row_pointers);


    // end write
    if (setjmp(png_jmpbuf(png_ptr))){
        printf("[write_png_file] Error during end of write");
        return -1;
    }

    png_write_end(png_ptr, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}

/*
   void process_file(void)
   {
   if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB){
   printf("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA " "(lacks the alpha channel)");
   return -1;
   }

   if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA){
   printf("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)", PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));
   return -1;
   }

   for (y=0; y<height; y++) {
   png_byte* row = row_pointers[y];
   for (x=0; x<width; x++) {
   png_byte* ptr = &(row[x*4]);
   printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
   x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

// set red value to 0 and green value to the blue one
ptr[0] = 0;
ptr[1] = ptr[2];
}
}
}

 */
int main(int argc, char **argv){
    struct pngFile readPngFile;
    if(read_png_file("libpngTests/test.png", &readPngFile)!=0){
        printf("Error reading png...aborting");
        exit(1);
    }

    for (y=0; y<readPngFile.height; y++) {
        for (x=0; x<readPngFile.width; x++) {
            png_byte* ptr = &(readPngFile.row_pointers[y][x*3]);
            ptr[0] = 255;
            ptr[1] = 255;
            ptr[2] = 255;
            //printf("Pixel at position [ %d - %d ] has RGB values: %d - %d - %d - %d\n", x, y, ptr[0], ptr[1], ptr[2], ptr[3]);
        }
    }

    //Pas in readPng file for height, width, colorDepth, colorType NOT row_pointers
    if(write_png_file("test_out.png", readPngFile.row_pointers, &readPngFile)!=0){
        printf("Error reading png...aborting");
        exit(1);
    }

    for (y=0; y<readPngFile.height; y++){
        free(readPngFile.row_pointers[y]);
    }
    free(readPngFile.row_pointers);
}
