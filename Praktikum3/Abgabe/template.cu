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
 #include "timing.h"
 
 #define BLOCK_SIZE  16
 #define HEADER_SIZE 122
 
 typedef unsigned char BYTE;
 
 //#define CUDA_ERROR_CHECK
 
 #define CudaSafeCall( err ) __cudaSafeCall( err, __FILE__, __LINE__ )
 #define CudaCheckError()    __cudaCheckError( __FILE__, __LINE__ )
 
 void showGPUMem();
 
 inline void __cudaSafeCall( cudaError err, const char *file, const int line )
 {
 #ifdef CUDA_ERROR_CHECK
     if ( cudaSuccess != err )
     {
         fprintf( stderr, "cudaSafeCall() failed at %s:%i : %s\n",
                 file, line, cudaGetErrorString( err ) );
         exit( -1 );
     }
 #endif
     
     return;
 }
 
 inline void __cudaCheckError( const char *file, const int line )
 {
 #ifdef CUDA_ERROR_CHECK
     cudaError err = cudaGetLastError();
     if ( cudaSuccess != err )
     {
         fprintf( stderr, "cudaCheckError() failed at %s:%i : %s\n",
                 file, line, cudaGetErrorString( err ) );
     showGPUMem();
         exit( -1 );
     }
     
     // More careful checking. However, this will affect performance.
     // Comment away if needed.
     /*   err = cudaDeviceSynchronize();
      if( cudaSuccess != err )
      {
      fprintf( stderr, "cudaCheckError() with sync failed at %s:%i : %s\n",
      file, line, cudaGetErrorString( err ) );
      exit( -1 );
      }*/
 #endif
     
     return;
 }
 
 void showGPUMem(){
     // show memory usage of GPU
     
     size_t free_byte ;
     
     size_t total_byte ;
     
     cudaError_t cuda_status = cudaMemGetInfo( &free_byte, &total_byte ) ;
     
     if ( cudaSuccess != cuda_status ){
         
         printf("Error: cudaMemGetInfo fails, %s \n", cudaGetErrorString(cuda_status) );
         
         exit(1);
         
     }
     
     
     
     double free_db = (double)free_byte ;
     
     double total_db = (double)total_byte ;
     
     double used_db = total_db - free_db ;
     
     printf("GPU memory usage: used = %f MB, free = %f MB, total = %f MB\n", used_db/1024.0/1024.0, free_db/1024.0/1024.0, total_db/1024.0/1024.0);
 }
 
 
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
 
 
 
 /**
  * Writes a BMP file in grayscale given its image data and a filename.
  */
 void writeBMPGrayscale(int width, int height, float *image, char *filename)
 {
     FILE *file = NULL;
     
     file = fopen(filename, "wb");
     
     // Write the reference header
     fwrite(bmp_info, sizeof(BYTE), HEADER_SIZE, file);
     
     // Unwrap the 8-bit grayscale into a 24bpp (for simplicity)
     for (int h = 0; h < height; h++)
     {
         int row = h * width;
         
         for (int w = 0; w < width; w++)
         {
             BYTE pixel = (BYTE)((image[row + w] > 255.0f) ? 255.0f :
                                 (image[row + w] < 0.0f)   ? 0.0f   :
                                 image[row + w]);
             
             // Repeat the same pixel value for BGR
             fputc(pixel, file);
             fputc(pixel, file);
             fputc(pixel, file);
         }
     }
     
     fclose(file);
 }
 
 /**
 * Releases a given BMPImage.
 */
 void freeBMP(BMPImage bitmap)
 {
     free(bitmap.data);
 }
 
 
 /*********** Gray Scale Filter  *********/
 
 /**
  * Converts a given 24bpp image into 8bpp grayscale using the CPU.
  */
 void grayscale(int width, int height, float *image, float *image_out)
 {
     for (int h = 0; h < height; h++)
     {
         int offset_out = h * width;      // 1 color per pixel
         int offset     = offset_out * 3; // 3 colors per pixel
         
         for (int w = 0; w < width; w++)
         {
             float *pixel = &image[offset + w * 3];
             
             // Convert to grayscale following the "luminance" model
             image_out[offset_out + w] = pixel[0] * 0.0722f + // B
             pixel[1] * 0.7152f + // G
             pixel[2] * 0.2126f;  // R
         }
     }
 }
 
 /**
  * Converts a given 24bpp image into 8bpp grayscale using the GPU.
  */
 __global__
 void cuda_grayscale(int width, int height, float *image, float *image_out)
 {   
     // calculate the global indices
     int absX = blockIdx.x * blockDim.x + threadIdx.x;
     int absY = blockIdx.y * blockDim.y + threadIdx.y;
     // calculate the offset in the memories
     int offset_out = absY * width + absX;   // 1 color per pixel
     int offset_in  = offset_out * 3;        // 3 color per pixel
 
     // Grayscaling from 3 colors per pixel to 1 color per pixel
     image_out[offset_out] = 0.0722f * image[ offset_in] +     // B
                             0.7152f * image[ offset_in + 1] + // G
                             0.2126f * image[ offset_in + 2];  // R
 }
 
 /****************Convolution Filters*****/
 
 
 /**
  * Applies a 3x3 convolution matrix to a pixel using the CPU.
  */
 float applyFilter(float *image, int stride, float *matrix, int filter_dim)
 {
     float pixel = 0.0f;
     
     for (int h = 0; h < filter_dim; h++)
     {
         int offset        = h * stride;
         int offset_kernel = h * filter_dim;
         
         for (int w = 0; w < filter_dim; w++)
         {
             pixel += image[offset + w] * matrix[offset_kernel + w];
         }
     }
     
     return pixel;
 }
 
 /**
  * Task 5
  * Applies a 3x3 convolution matrix to a pixel using the GPU.
  */
 __device__
 float cuda_applyFilter(float *image, int stride, float *matrix, int filter_dim)
 {
     float pixel = 0.0f;
     
     for (int h = 0; h < filter_dim; h++)
     {
         int offset        = h * stride;
         int offset_kernel = h * filter_dim;
         
         for (int w = 0; w < filter_dim; w++)
         {
             pixel += image[offset + w] * matrix[offset_kernel + w];
         }
     }
     
     return pixel;
 }
 
 /**
  * Applies a Gaussian 3x3 filter to a given image using the CPU.
  */
 void gaussian(int width, int height, float *image, float *image_out)
 {
     float gaussian[9] = {   1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
                             2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
                             1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f };
     
     for (int h = 0; h < (height - 2); h++)
     {
         int offset_t = h * width;
         int offset   = (h + 1) * width;
         
         for (int w = 0; w < (width - 2); w++)
         {
             image_out[offset + (w + 1)] = applyFilter(&image[offset_t + w], width, gaussian, 3);
         }
     }
 }
 
 /**
  * Task 6
  * Applies a Gaussian 3x3 filter to a given image using the GPU.
  */
 __global__
 void cuda_gaussian(int width, int height, float *image, float *image_out)
 {
     //(9 pt): implement gaussian filter kernel
     float gaussian[9] = {   1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
                             2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
                             1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f };
     
     // calculate the global indices
     int absX = blockIdx.x * blockDim.x + threadIdx.x;
     int absY = blockIdx.y * blockDim.y + threadIdx.y;
     
     // calculate the offset in the memory
     int offset_t    = absY * width;
     int offset      = (absY + 1) * width;
 
     if( absY < (height - 2) && absX < (width - 2) ){
         image_out[offset + (absX + 1)] = cuda_applyFilter(&image[offset_t + absX], width, gaussian, 3);
     }
 
 }
 
 /**
  * Calculates the gradient of an image using a Sobel filter on the CPU.
  */
 void sobel(int width, int height, float *image, float *image_out)
 {
     float sobel_x[9] = {1.0f,  0.0f, -1.0f,
                         2.0f,  0.0f, -2.0f,
                         1.0f,  0.0f, -1.0f };
     float sobel_y[9] = {1.0f,  2.0f,  1.0f,
                         0.0f,  0.0f,  0.0f,
                         -1.0f, -2.0f, -1.0f };
     
     for (int h = 0; h < (height - 2); h++)
     {
         int offset_t = h * width;
         int offset   = (h + 1) * width;
         
         for (int w = 0; w < (width - 2); w++)
         {
             float gx = applyFilter(&image[offset_t + w], width, sobel_x, 3);
             float gy = applyFilter(&image[offset_t + w], width, sobel_y, 3);
             
             image_out[offset + (w + 1)] = sqrtf(gx * gx + gy * gy);
         }
     }
 }
 
 /**
  * Task 7
  * Calculates the gradient of an image using a Sobel filter on the GPU.
  */
 __global__
 void cuda_sobel(int width, int height, float *image, float *image_out)
 {
     //TODO (9 pt): implement sobel filter kernel
     float sobel_x[9] = {1.0f,  0.0f, -1.0f,
                         2.0f,  0.0f, -2.0f,
                         1.0f,  0.0f, -1.0f };
     float sobel_y[9] = {1.0f,  2.0f,  1.0f,
                         0.0f,  0.0f,  0.0f,
                         -1.0f, -2.0f, -1.0f };
     
     // calculate the global indices
     int absX = blockIdx.x * blockDim.x + threadIdx.x;
     int absY = blockIdx.y * blockDim.y + threadIdx.y;
     
     // calculate the offset in the memory
     int offset_t    = absY * width;
     int offset      = (absY + 1) * width;
 
     if( absY < (height - 2) && absX < (width - 2) ){
         float gx = cuda_applyFilter(&image[offset_t + absX], width, sobel_x, 3);
         float gy = cuda_applyFilter(&image[offset_t + absX], width, sobel_y, 3);
 
         image_out[offset + (absX + 1)] = sqrtf(gx * gx + gy * gy);
     }
     
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
     //dim3     grid(1);
     dim3     block(BLOCK_SIZE, BLOCK_SIZE);
     char     path[255];
     
 
     init_clock_time();
     
     // Read the input image and update the grid dimension
     bitmap     = readBMP(argv[1]);
     image_size = bitmap.width * bitmap.height;
     
     /* Task 1: Calculate the grid dimensions-size
     */
     dim3 grid( ceil((double) bitmap.width / (double) BLOCK_SIZE), ceil( (double) bitmap.height / (double) BLOCK_SIZE) );
     
     printf("Image read (width=%d height=%d).\n", bitmap.width, bitmap.height);
     
     // Allocate the intermediate image buffers for each step
     for (int i = 0; i < 2; i++)
     {
         image_out[i] = (float *)calloc(image_size, sizeof(float));
         
         // Task2:
         //(2 pt): allocate memory on the device
         //(2 pt): intialize allocated memory on device to zero
         cudaMalloc((void**) &d_image_out[i], image_size * sizeof(float) );
         cudaMemcpy( (void*) d_image_out[i], image_out[i], image_size * sizeof(float), cudaMemcpyHostToDevice );
     }
 
     //Task3: copy input image to device
     //(2 pt): Allocate memory on device for input image
     //(2 pt): Copy input image into the device memory
     cudaMalloc( (void**) &d_bitmap, 3 * image_size * sizeof(float) );
     cudaMemcpy( (void*) d_bitmap, bitmap.data, 3 * image_size * sizeof(float), cudaMemcpyHostToDevice );
 
     t[0] = get_clock_time();
     
     // Covert input image to grayscale
     //grayscale(bitmap.width, bitmap.height, bitmap.data, image_out[0]); //serial version
     
     //Task4: (2 pt): Launch cuda_grayscale()
     cuda_grayscale<<<grid, block>>>(bitmap.width, bitmap.height, d_bitmap, d_image_out[0]);
 
     t[1] = get_clock_time();
 
     //(2 pt): transfer image from device to the main memory for saving onto the disk
     cudaMemcpy( image_out[0], d_image_out[0], image_size * sizeof(float), cudaMemcpyDeviceToHost );
     
     sprintf(path, "images/grayscale.bmp");
     writeBMPGrayscale(bitmap.width, bitmap.height, image_out[0], path); //write output file
     printf("Time taken for grayscaling: %8.5f ms\n",t[1] - t[0]);
     
     // Apply a 3x3 Gaussian filter
     t[0] = get_clock_time();
     // Launch the CPU version
     // gaussian(bitmap.width, bitmap.height, image_out[0], image_out[1]);
     
     // Launch the GPU version
     // (2 pt): Launch cuda_gaussian();
     cuda_gaussian<<<grid, block>>>(bitmap.width, bitmap.height, d_image_out[0], d_image_out[1]);
     
     
     t[1] = get_clock_time();
     //(2 pt): transfer image from device to the main memory for saving onto the disk
     cudaMemcpy( image_out[1], d_image_out[1], image_size * sizeof(float), cudaMemcpyDeviceToHost);
 
     // Store the result image with the Gaussian filter applied
     sprintf(path, "images/gaussian.bmp");
     writeBMPGrayscale(bitmap.width, bitmap.height, image_out[1], path); //write output file
     printf("Time taken for Gaussian filtering: %8.5f ms\n",t[1] - t[0]);
     
 
     // Apply a Sobel filter
     t[0] = get_clock_time();
     // Launch the CPU version
     //sobel(bitmap.width, bitmap.height, image_out[1], image_out[0]);
     
     // Launch the GPU version
     // (2 pt): Launch cuda_sobel();
     cuda_sobel<<<grid, block>>>(bitmap.width, bitmap.height, d_image_out[1], d_image_out[0]);
    
     t[1] = get_clock_time();
     //(2 pt): transfer image from device to the main memory for saving onto the disk
     cudaMemcpy( image_out[0], d_image_out[0], image_size * sizeof(float), cudaMemcpyDeviceToHost);
     
     // Store the final result image with the Sobel filter applied
     sprintf(path, "images/sobel.bmp");
     writeBMPGrayscale(bitmap.width, bitmap.height, image_out[0], path); //write output file
     printf("Time taken for Sobel filtering: %8.5f ms\n",t[1] - t[0]);
     
     
     // Task 9
     // Release the allocated memory
     for (int i = 0; i < 2; i++)
     {
         free(image_out[i]);
         cudaFree(d_image_out[i]);
     }
     
     freeBMP(bitmap);
     cudaFree(d_bitmap);
     
     // Got any errors?
     printf("%s\n", cudaGetErrorString(cudaGetLastError()));
     return 0;
 }
 