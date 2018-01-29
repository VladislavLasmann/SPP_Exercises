/**
 * Gruppe: 122
 * Konstantin Müller (2327697) 
 * Robin Ferrari 	 (2585277) 
 * Vladislav Lasmann (2593078)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define BLOCK_SIZE  16
#define HEADER_SIZE 122

typedef unsigned char BYTE;

/********* BMP Image functions **************/
typedef struct
{
    int   width;
    int   height;
    float *data;
} BMPImage;

BYTE bmp_info[HEADER_SIZE]; // Reference header


/**
 * Reads a BMP 24bpp file and returns a BMPImage structure.
 * Thanks to https://stackoverflow.com/a/9296467
 */
BMPImage readBMP(char *filename)
{
    BMPImage bitmap = { 0 };
    int      size   = 0;
    BYTE     *data  = NULL;
    FILE     *file  = fopen(filename, "rb");
    
    // Read the header (expected BGR - 24bpp)
    fread(bmp_info, sizeof(BYTE), HEADER_SIZE, file);
    
    // Get the image width / height from the header
    bitmap.width  = *((int *)&bmp_info[18]);
    bitmap.height = *((int *)&bmp_info[22]);
    size          = *((int *)&bmp_info[34]);
    
    printf("THE FUCKING SIZE IS: %d\n", size);
    printf("THE FUCKING width IS: %d\n", bitmap.width);
    printf("THE FUCKING height IS: %d\n", bitmap.height);
    printf("THE FUCKING arrayLength IS: %lu, in bytes: %lu \n", sizeof(*bitmap.data)/sizeof(bitmap.data[0]), sizeof(*bitmap.data) );

    // Read the image data
    data = (BYTE *)malloc(sizeof(BYTE) * size);
    fread(data, sizeof(BYTE), size, file);
    
    // Convert the pixel values to float
    bitmap.data = (float *)malloc(sizeof(float) * size);
    
    for (int i = 0; i < size; i++)
    {
        bitmap.data[i] = (float)data[i];
    }
    
    fclose(file);
    free(data);
    
    return bitmap;
}



int main(int argc, char *argv[])
{
    //check for arguments
    if(argc != 2){
        puts("Usage: sol input_file\n");
        exit(0);
    }
    
    BMPImage bitmap          = { 0 };
    float    *d_bitmap       = { 0 };
    float    *image_out[2]   = { 0 };
    float    *d_image_out[2] = { 0 };
    int      image_size      = 0;
    double   t[2]            = { 0 };

    // Read the input image and update the grid dimension
    bitmap     = readBMP(argv[1]);
    image_size = bitmap.width * bitmap.height;
    
    
    return 0;
}
