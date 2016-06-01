#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "pixel_operations.h"
#include "segmentation.h"

// Greyscale
void filter_greyscale(SDL_Surface *img);

// Black and White
void filter_blackwhite(SDL_Surface *img);

// Convolution
void convolution_apply(struct matrix *M, int w, int h, struct matrix *conv);

void filter_noise(struct matrix *M);
