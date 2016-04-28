#include "filters.h"
#include "segmentation.h" 

// Greyscale
void filter_greyscale(SDL_Surface *img) {
    Uint32 pxl;
    Uint8 r, g, b;
    for (int w = 0; w < img->w; w++) {
        for (int h = 0; h < img->h; h++) {
            pxl = getpixel(img, w, h);
            SDL_GetRGB(pxl, img->format, &r, &g, &b);
            r = 0.3 * r + 0.59 * g + 0.11 * b;
            pxl = SDL_MapRGB(img->format, r, r, r);
            putpixel(img, w, h, pxl);
        }
    }
}

// Black and White
void filter_blackwhite(SDL_Surface *img) {
    Uint32 pxl;
    Uint8 r;
    for (int w = 0; w < img->w; w++) {
        for (int h = 0; h < img->h; h++) {
            pxl = getpixel(img, w, h);
            SDL_GetRGB(pxl, img->format, &r, &r, &r);
            r = r > 255 / 2 ? 255 : 0;
            pxl = SDL_MapRGB(img->format, r, r, r);
            putpixel(img, w, h, pxl);
        }
    }
}

// Noise
void filter_noise_pxl(struct matrix *img, int w, int h) {
  w -= 1;
  h -= 1;
  int  wm = w + 3, hm = h + 3, i = 0;
  for(; w < wm; w++) {
    for(; h < hm; h++) {
      if(img->data[w * img->width + h] == 1) // Choose w and h CAREFULLY
        i++;
    }
  }
  img->data[(w - 1) * img->width + (h - 1)] = i >= 4 ? 1 : 0;
}

void filter_noise(struct matrix *img) {
  for(int i = 1; i < img->width - 1; i++) {
    for(int j = 1; j < img->height - 1; j++) {
      filter_noise_pxl(img, i, j);
    }
  }
}

