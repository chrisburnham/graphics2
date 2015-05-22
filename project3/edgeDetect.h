#ifndef EDGEDETECT_H

#define EDGEDETECT_H

void gaussianBlur3(Image *src, Image *dest);

void gaussianBlur5(Image *src, Image *dest);

Image *gaussianBlur(Image *src, int size);

// apply a gaussian blur before using
Image *edgeDetect(Image *src, int b);

#endif
