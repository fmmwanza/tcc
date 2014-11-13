//   medianfilter.h - declarations for 
//   1D and 2D median filter routines
//
//   The code is property of LIBROW
//   You can use it on your own
//   When utilizing credit LIBROW site

#ifndef _MEDIANFILTER_H_
#define _MEDIANFILTER_H_

//   Signal/image element type
typedef unsigned char volume;


//   2D MEDIAN FILTER, window size 3x3
//     image  - input image
//     result - output image, NULL for inplace processing
//     N      - width of the image
//     M      - height of the image
void medianfilter(volume* image, volume* result, int N, int M);
void _medianfilter(const volume* image, volume* result, int N, int M);

#endif
