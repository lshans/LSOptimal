#ifndef CANNY_H
#define CANNY_H

struct SIZE;
typedef short *LPBYTE;
void image_cut_merge(short **image, int height, int width);
void Canny(LPBYTE pGray, double sigma, int height, int width, block &block_label);

#endif