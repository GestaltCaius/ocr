#include "filters.h"

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


// Noise reduction using convolution techniques

void filter_noise(struct matrix *M)
{
  // Create conv matrix
  struct matrix *conv = malloc(sizeof(struct matrix));
  conv->data = malloc(9 * sizeof(double));
  conv->width = conv->height = 3;
  for(size_t i = 0; i < 9; i++)
  conv->data[i] = i == 4 ? 5 : 1; // all 1's, except for the 5 in the center
  // Let's apply the conv matrix
  for(int i = 1; i < M->width - 1; i++)
  {
    for(int j = 1; j < M->height - 1; j++)
      convolution_apply(M, i, j, conv);
  }
}

// Subfunction that apply the convulation matrix to one elt of our grey matrix 
// DO NOT call it on the edges, or it'll be out of range
void convolution_apply(struct matrix *M, int w, int h, struct matrix *conv)
{
  double sum = 0;
  for(int i = 0; w - 1 + i <= w + 1; i++)
  {
    for(int j = 0; h - 1 + j < h + 1; j++)
      sum += M->data[ (w-1+i) * M->width + (h-1+j)] * conv->data[i * 3 + j];
  }
  M->data[w * M->width + h] = sum / 9;
}


// Contrast filter

void filter_contrast(struct matrix *M)
{
  // Create convolution matrix
  struct matrix *conv = malloc(sizeof(struct matrix));
  conv->width = conv->height = 3;
  conv->data = malloc(9 * sizeof(double));
  for(int i = 0; i < 9; ++i)
  {
    if (i % 2)
      conv->data[i] = -1;
    else
      conv->data[i] = i == 4 ? 5 : 0;
  }
  // Apply filter
  for(int i = 1; i < M->width - 1; ++i)
  {
    for(int j = 1; j < M->height - 1; ++j)
    {
      convolution_apply(M, i, j, conv);
    }
  }
}

// Matrix operations
struct matrix *filter_greyscale_matrix(SDL_Surface *img) {
    Uint32 pxl;
    Uint8 r, g, b;
    struct matrix *M = malloc(sizeof(struct matrix));
    M->data = malloc(img->w * img->h * sizeof(double));
    M->width = img->w;
    M->height = img->h;
    for (int w = 0; w < img->w; w++) {
        for (int h = 0; h < img->h; h++) {
            pxl = getpixel(img, w, h);
            SDL_GetRGB(pxl, img->format, &r, &g, &b);
            r = 0.3 * r + 0.59 * g + 0.11 * b;
            M->data[w * M->width + h] = (double)r;
        }
    }
    return M;
}

/* we use the img gave in filter_greyscale_matrix
 * that way, its elements are already filled (format, height, width) */
void matrix_to_img(struct matrix *M, SDL_Surface *img)
{
  Uint32 pxl;
  Uint8 rgb;
  for (int w = 0; w < img->w; w++)
  {
    for (int h = 0; h < img->h; h++)
    {
      rgb = (Uint8)M->data[w * M->width + h];
      pxl = SDL_MapRGB(img->format, rgb, rgb, rgb);
      putpixel(img, w, h, pxl);
    }
  }
}









/* 
 * ________________________________________________________
 * LOUP'S BS
 * ________________________________________________________
 */

/*
int cluster_rec(struct matrix *img, int w, int h,struct vector *traced,int *nb)
{
    struct vector matrix *P = malloc(sizeof(struct matrix));
    P->data = calloc(sizeof(double) * img->width * img->height);
    for(int)
}

int cluster(struct matrix *img, int w, int h int min)
{
    struct matrix *traced = malloc(sizeof(struct matrix));
    traced->data = calloc(sizeof(double) * w * h);
    for(int h = 0; h < img->height; h++)
    {
	for(int w = 0; w < img->width; w++)
	{
	    if(img[h * img->width + w] && !traced[h * img->width + w]) //black pixel not treated yet
	    {
		int nb = 0;
		struct tuple *stain = malloc(sizeof(struct tuple));
		stain->next == NULL;
		cluster_rec(img, w, h, traced, &nb,);
		if(nb < min) //we whiten the stain if it is too small to be a char
		{
		    for(; tuple->next != NULL; tuple = tuple->next)
		    {
			img[stain->next->height * img->width + stain->next->width] = 0;
		    }
		    
		}
	    }
	}
    }
}

int stain_size(struct matrix *img, int w, int h)
{

}*/
