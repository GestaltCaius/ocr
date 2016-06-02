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

/*int line_is_empty(struct matrix *img, int line) {
    int i = 0;
    for (; i < img->width && img->data[line * img->width + i] == 0; i++) {
    }
    return (i == img->width);
}*/

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

struct vector *img_to_lines(struct matrix *img, struct vector *blocks) {
    struct vector *lines = vector_make((img->height) * (img->width));
    int status = 0; // not on a line
    int h = 0;
    for(; blocks->size != 0;)
    {
	struct coords current_block;
	vector_pop_front(blocks, &current_block);
	int w1 = current_block.w1, w2 = current_block.w2;

	for (; h < img->height; h++) {
	    if (status) {
		struct coords line;
		line.w1 = 1;
		line.w2 = 1;
		line.h1 = h - 1;
		for (; h < img->height && !line_is_empty(img, h, w1, w2); h++) {
		}
		line.h2 = h - 1;
		vector_push_back(lines, line);
		status = 0;
	    } else {
		for (; h < img->height && line_is_empty(img, h, w1, w2); h++) {
		}
		status = 1;
	    }
	}
    }
    free(blocks);
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

/*size_t block_thresold(struct matrix *img) {
  int i = 0;
  for(; line_is_empty(img, i); i++){}
  int j = i;
  for(; !line_is_empty(img, j); j++){}
  return (size_t)(j - i);
  }*/

//secondary functions used in img_to_blocks() and blocks_detection :
int line_is_empty(struct matrix *img, int h, int w1, int w2)
{
    int i;
    for(i = w1; i <= w2 && img->data[h * img->width + i] == 0; i++){ }
    return i == w2 + 1;
}

int vertical_search(struct matrix *img, struct coords block, int minimal_size)
{
    int size = 0;
    int i;
    for(i = block.w1; size < minimal_size && i < block.w2 - minimal_size; i++)
    {
	if(column_is_empty(img, i, block.h1, block.h2)){ size++; }
	else { size = 0; }
    }
    if(size >= minimal_size)
    {
	for(; column_is_empty(img, i, block.h1, block.h2); size++) { }
	return i - (size / 2);
    }
    return 0;
}

int horizontal_search(struct matrix *img, struct coords block, int minimal_size)
{
    int size = 0;
    int i;
    for(i = block.h1; size < minimal_size && i < block.h2 - minimal_size; i++)
    {
	if(line_is_empty(img, i, block.w1, block.w2)){ size++; }
	else { size = 0; }
    }       
    if(size >= minimal_size)
    {   
	for(; line_is_empty(img, i, block.w1, block.w2); size++) { }
	return i - (size / 2);
    }
    return 0;
}


//the main functions :
struct vector *img_to_blocks(struct matrix *img)
{
    struct coords init;

    //all the text is within the square ((w1,h1),(w2,h2))
    int i = 0;
    for(; i < img->height - 1 && line_is_empty(img, i, 0, img->width - 1); i++){}
    init.h1 = i;
    for(i = img->height - 1; i > 0 && line_is_empty(img, i, 0, img->width - 1); i--){}
    init.h2 = i;
    for(i = 0; i < img->width - 1 && column_is_empty(img, i, init.h1, init.h2); i++){}
    init.w1 = i;
    for(i = img->width - 1; i > 0 && column_is_empty(img, i, init.h1, init.h2); i--){}
    init.w2 = i;

    //we launch the block detection in that original block
    if(init.w1 < init.w2 && init.h1 < init.h2){
	return vertical_rec(img, init);
    }
    else
	return NULL; //invalid image.
}

/*
   struct vector *blocks_detection(struct matrix *img, struct coords *init)
   {
   int minimal_size = 30;
   struct vector *processing_vector = vector_make(1);
   struct vector *blocks = vector_make(0);

//the first block in the processing vector contains all the text :
vector_push_front(processing_vector, *init);

for(; processing_vector->size != 0;)
{
//tells us if the vertical / horizontal split is a success.
int vertical = 1;
int horizontal = 1;

//we try to split verticaly.
//if we succeed, we enqueue the new blocks created. if not, vertical = 0.
//now we split horizontally the first block of the processing vector.
//if we cannot split vertically nor horizontally, we put the block in the output vect.
//we stop when the processing vector is empty.

struct coords current_block;
if(vector_pop_front(processing_vector, &current_block))
{
int vertical_breach = vertical_search(img, current_block, minimal_size);
if(breach)
{
struct coords new_block_1 = current_block;
new_block_1.h2 = breach;
vector_push_back(processing_vector, new_block_1);

struct coords new_block_2 = current_block;
new_block_2.h1 = breach;
vector_push_back(processing_vector, new_block_2);
}
else
vertical = 0;
int horizontal_breach = horizontal_search
(processing_vector, current_block, minimal_size);
if(breach)
{
struct coords new_block_1 = current_block;
new_block_1.w2 = breach;
vector_push_back(processing_vector, new_block_1);

struct coords new_block_2 = current_block;
new_block_2.w1 = breach;
vector_push_back(processing_vector, new_block_2);
}
else
horizontal = 0;

struct coords z1, z2, z3, z4;
z1 = current_block;
z2 = z1, z3 = z1, z4 = z1;

if(vertical != 0 && horizontal != 0)
{
z1.h2 = horizontal_breach;
z1.w2 = vertical_breach;

z2.h1 = horizontal_breach;
z2.w2 = vertical_breach;

z3.h2 = horizontal_breach;
z3.w1 = vertical_breach;

z4.h1 = horizontal_breach;
z4.w1 = vertical_breach;

vector_push_back(processing_vector, z1);
vector_push_back(processing_vector, z2);
vector_push_back(processing_vector, z3);


}
else if(vertical != 0 && horizontal == 0)
{
    z1.
}
else if(horizontal != 0)
{
}
else
vector_push_back(blocks, current_block);

//////////////////////////////////////////////////

}

}
free_vector(processing_vector);
return blocks;
}*/

struct vector *vertical_rec(struct matrix *img, struct coords block)
{
    int i = vertical_search(img, block, 30);
    if(i)
    {
	struct coords block1, block2;
	block1 = block, block2 = block;
	block1.w2 = i;
	block2.w1 = i;
	return vector_merge(horizontal_rec(img, block1), vertical_rec(img, block2));
    }
    else
    {
	return horizontal_rec(img, block);
    }
}
struct vector *horizontal_rec(struct matrix *img, struct coords block)
{
    int i = horizontal_search(img, block, 30);
    if(i)
    {
	struct coords block1, block2;
	block1 = block, block2 = block;
	block1.h2 = i;
	block2.h1 = i;
	return vector_merge(vertical_rec(img, block1), vertical_rec(img, block2));
    }
    else
    {
	struct vector *indivisible_block = vector_make(1);
	indivisible_block->data[0] = block;
	return indivisible_block;
    }
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
