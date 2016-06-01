#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "pixel_operations.h"
#include "vector.h"
#include "loadimage.h"
#include "segmentation.h"

struct matrix *img_to_matrix(SDL_Surface *img) {
    Uint32 pxl;
    Uint8 r;
    struct matrix *A = malloc(sizeof(struct matrix));
    A->data = malloc(sizeof(double) * (img->w * img->h));
    A->width = img->w;
    A->height = img->h;
    for (int h = 0; h < img->h; h++) {
        for (int w = 0; w < img->w; w++) {
            pxl = getpixel(img, w, h);
            SDL_GetRGB(pxl, img->format, &r, &r, &r);
            A->data[h * img->w + w] = r == 255 ? 0 : 1;
        }
    }
    return A;
}

void free_matrix(struct matrix *A) {
    free(A->data);
    free(A);
}

void draw_square(SDL_Surface *img, struct coords square) {
    Uint32 pxl;
    pxl = SDL_MapRGB(img->format, 0, 255, 0);
    for (int i = square.h1; i <= square.h2; i++) {
        putpixel(img, square.w1, i, pxl);
        putpixel(img, square.w2, i, pxl);
    }
    for (int i = square.w1 + 1; i < square.w2 - 1; i++) {
        putpixel(img, i, square.h1, pxl);
        putpixel(img, i, square.h2, pxl);
    }
}

void display_segmentation(SDL_Surface *img, struct vector *V) {
    size_t i = 0;
    while (i < V->size) {
        if (V->data[i].w1 >= 0) {
            draw_square(img, V->data[i]);
        }
        i++;
    }
}

/* LINE DETECTION PART */

int line_is_empty(struct matrix *img, int line) {
    int i = 0;
    for (; i < img->width && img->data[line * img->width + i] == 0; i++) {
    }
    return (i == img->width);
}

int column_is_empty(struct matrix *img, int x, int y1, int y2) {
    int i = y1;
    for (; i < y2 && img->data[i * img->width + x] == 0; i++) {
    }
    return i == y2;
}

int small_line_is_empty(struct matrix *img, int y, int w1, int w2) {
    int i = w1;
    for (; i < w2 && img->data[y * img->width + i] == 0; i++) {
    }
    return i == w2;
}

struct vector *img_to_lines(struct matrix *img) {
    struct vector *lines = vector_make((img->height) * (img->width));
    int status = 0; // not on a line
    int h = 0;
    for (; h < img->height; h++) {
        if (status) {
            struct coords line;
            line.w1 = 1;
            line.w2 = 1;
            line.h1 = h - 1;
            for (; h < img->height && !line_is_empty(img, h); h++) {
            }
            line.h2 = h - 1;
            vector_push_back(lines, line);
            status = 0;
        } else {
            for (; h < img->height && line_is_empty(img, h); h++) {
            }
            status = 1;
        }
    }
    return lines;
}


/* CHAR DETECTION PART */
struct vector *lines_to_char(struct matrix *img, struct vector *lines) {
    struct vector *imgs = vector_make((img->height) * (img->width));
    for (size_t k = 0; k < lines->size; k++) {
        struct coords actual_coords;
        actual_coords.w1 = 0;
        actual_coords.w2 = 0;
        actual_coords.h1 = lines->data[k].h1;
        actual_coords.h2 = lines->data[k].h2;
        int recording = 0;

        for (int i = 0; i < img->width; i++) {
            if (!recording) {
                if (!column_is_empty(img, i, lines->data[k].h1,
                                     lines->data[k].h2)) {
                    recording = 1;
                    actual_coords.w1 = i;
                    actual_coords.w2 = i;
                }
            } else {
                if (!column_is_empty(img, i, lines->data[k].h1,
                                     lines->data[k].h2)) {
                    actual_coords.w2++;
                } else {
                    vector_push_back(imgs, actual_coords);
                    recording = 0;
                }
            }
        }
        actual_coords.w1 = -1;
        vector_push_back(imgs, actual_coords);
    }
    return imgs;
}

/* BLOCKS DETECTION PART */

size_t block_thresold(struct matrix *img) {
  int i = 0;
  for(; line_is_empty(img, i); i++){}
  int j = i;
  for(; !line_is_empty(img, j); j++){}
  return (size_t)(j - i);
}

struct vector *img_to_blocks(struct matrix *img)
{
    struct coords init;
    
    //all the text is within the square ((w1,h1),(w2,h2))
    int i = 0;
    for(; i < img->height - 1 && line_is_empty(img, i); i++){}
    init.h1 = i;
    for(i = img->height - 1; i > 0 && line_is_empty(img, i); i--){}
    init.h2 = i;
    for(i = 0; i < img->width - 1 && column_is_empty(img, i, init.h1, init.h2); i++){}
    init.w1 = i;
    for(i = img->width - 1; i > 0 && column_is_empty(img, i, init.h1, init.h2); i--){}
    init.w2 = i;

    //we launch the block detection in that original block
    if(init.w1 < init.w2 && init.h1 < init.h2){
    	return blocks_detection(img, &init);
    }
    else
	return NULL; //invalid image.
}

struct vector *blocks_detection(struct matrix *img, struct coords *init)
{
    int minimum_size = 30;
    struct vector *processing_vect = vector_make(1);
    struct vector *blocks = vector_make(0);
    
    //the first block in the processing vector contains all the text :
    vector_push_front(processing_vect, *init);
    
    for(; processing_vect->size != 0;)
    {
	//tells us if the vertical / horizontal split is a success.
	int vertical = 1;
	int horizontal = 1;

	//we try to split verticaly.
	//if we succeed, we enqueue the new blocks created. if not, vertical = 0.
	//now we split horizontally the first block of the processing vector.
	//if we cannot split vertically nor horizontally, we put the block in the output vect.
	//we stop when the processing vector is empty.
    }
    free_vector(processing_vect);
    return blocks;
}

/* RESIZE CHAR PART */

struct vector *resize_char(struct matrix *img, struct vector *chars) {
    for (size_t i = 0; i < chars->size; i++) {
        if (chars->data[i].w1 >= 0) {
            while (small_line_is_empty(img, chars->data[i].h1,
                                       chars->data[i].w1, chars->data[i].w2)) {
                chars->data[i].h1++;
            }

            while (small_line_is_empty(img, chars->data[i].h2,
                                       chars->data[i].w1, chars->data[i].w2)) {
                chars->data[i].h2--;
            }
        }
    }
    return chars;
}

double *resize_table(struct coords c, struct matrix *A, int x, int y) {
    int s_w = c.w2 - c.w1;
    int s_h = c.h2 - c.h1;
    if (s_w >= x || s_h >= y)
        return NULL;

    double *new_t = calloc(x * y, sizeof(double));

    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            if (j <= s_w && i <= s_h) {
                new_t[i * x + j] = A->data[(c.h1 + i) * A->width + (c.w1 + j)];
            } else {
                new_t[i * x + j] = 0;
            }
        }
    }

    return new_t;
}
