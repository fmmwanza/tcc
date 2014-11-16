//   hybridmedianfilter.cpp - impvolumeation of 
//   2D hybrid median filter routines
//
//   The code is property of LIBROW
//   You can use it on your own
//   When utilizing credit LIBROW site

#include <memory.h>
#include "hybridmedianfilter.h"

//   MEDIAN calculation
//     volumes - input volumes
//     N        - number of input volumes
volume median(volume* volumes, int N)
{
   //   Order volumes (only half of them)
   for (int i = 0; i < (N >> 1) + 1; ++i)
   {
      //   Find position of minimum volume
      int min = i;
      for (int j = i + 1; j < N; ++j)
         if (volumes[j] < volumes[min])
            min = j;
      //   Put found minimum volume in its place
      const volume temp = volumes[i];
      volumes[i] = volumes[min];
      volumes[min] = temp;
   }
   //   Get result - the middle volume
   return volumes[N >> 1];
}

//   2D HYBRID MEDIAN FILTER implementation
//     image  - input image
//     result - output image
//     N      - width of the image
//     M      - height of the image
void _hybridmedianfilter(const volume* image, volume* result, int N, int M)
{
   //   Move window through all volumes of the image
   for (int m = 1; m < M - 1; ++m)
      for (int n = 1; n < N - 1; ++n)
      {
         volume window[5];
         volume results[3];
         //   Pick up cross-window volumes
         window[0] = image[(m - 1) * N + n];
         window[1] = image[m * N + n - 1];
         window[2] = image[m * N + n];
         window[3] = image[m * N + n + 1];
         window[4] = image[(m + 1) * N + n];
         //   Get median
         results[0] = median(window, 5);
         //   Pick up x-window volumes
         window[0] = image[(m - 1) * N + n - 1];
         window[1] = image[(m - 1) * N + n + 1];
         window[2] = image[m * N + n];
         window[3] = image[(m + 1) * N + n - 1];
         window[4] = image[(m + 1) * N + n + 1];
         //   Get median
         results[1] = median(window, 5);
         //   Pick up leading volume
         results[2] = image[m * N + n];
         //   Get result
         result[(m - 1) * (N - 2) + n - 1] = median(results, 3);
      }
}

//   2D HYBRID MEDIAN FILTER wrapper
//     image  - input image
//     result - output image
//     N      - width of the image
//     M      - height of the image
void hybridmedianfilter(volume* image, volume* result, int N, int M)
{
   //   Check arguments
   if (!image || N < 1 || M < 1)
      return;
   //   Allocate memory for signal extension
   volume* extension = new volume[(N + 2) * (M + 2)];
   //   Check memory allocation
   if (!extension)
      return;
   //   Create image extension
   for (int i = 0; i < M; ++i)
   {
      memcpy(extension + (N + 2) * (i + 1) + 1, image + N * i, N * sizeof(volume));
      extension[(N + 2) * (i + 1)] = image[N * i];
      extension[(N + 2) * (i + 2) - 1] = image[N * (i + 1) - 1];
   }
   //   Fill first line of image extension
   memcpy(extension,
      extension + N + 2,
      (N + 2) * sizeof(volume));
   //   Fill last line of image extension
   memcpy(extension + (N + 2) * (M + 1), extension + (N + 2) * M, (N + 2) * sizeof(volume));
   //   Call hybrid median filter implementation
   _hybridmedianfilter(extension, result ? result : image, N + 2, M + 2);
   //   Free memory
   delete[] extension;
}
