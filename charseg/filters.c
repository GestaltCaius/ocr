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
