# include <stdlib.h>
# include <stdio.h>
# include <SDL/SDL.h>
# include <SDL/SDL_image.h>

# include "pixel_operations.h"
# include "vector.h"

// BASIC FUNCTIONS

/*
struct list_coords list_empty(void)
{
	struct list_coords *l = malloc(sizeof(struct list_coords));
	l -> next = NULL;
	return l;
}

void list_push_front(struct list_coords *list, struct list_coords *cell)
{
	struct list_coords *elm = malloc(sizeof(stuct list_coords));
	elm =  cell;
	if (list_is_empty(list))
	{
		list -> next = elm;
	}
	else
	{
		elm -> next = list -> next -> next;
		list -> next = elm;
	}
}

struct list_coords* list_pop_front(struct list_coords *list)
{
	if (!list_is_empty(list))
	{
		struct list_coords *cell = malloc(sizeof(struct list_coords));
		struct list_coords *tmp = list -> next;
		cell = list -> next;
		list -> next = list -> next -> next;
		free(tmp);
		return cell;
	}
	else
		return NULL;
}

void print_coords(struct list_coords *list)
{
	while (list -> next != NULL)
	{
		prinf("(%d,%d) ", list -> next -> w, list -> next -> h);
		struct list_coords *tmp = list;
		list = list -> next;
		free(tmp);
	}
	printf("|"); // end of the list
}*/

// CHARACTER SEGMENTATION

// img to matrix of 0's and 1's
<<<<<<< HEAD
// 1 = black, 0 = white
double *img_to_matrix(SDL_Surface *img)
=======

struct matrix {
    double *data;
    int width, height;
};

struct matrix *img_to_matrix(SDL_Surface *img)
>>>>>>> b33d7fa680680d6c3697333536e721039443730a
{
    Uint32 pxl;
    Uint8 r;
    struct matrix *A = malloc(sizeof(struct matrix) * (img->w * img->h));
    A -> width = img -> w;
    A -> height = img -> h;
    for (int w = 0; w < img->w; w++)
    {
        for (int h = 0; h < img->h; h++)
        {
            pxl = getpixel(img, w, h);
            SDL_GetRGB(pxl, img->format, &r, &r, &r);
            A[w * img->w + h] = r == 255 ? 1 : 0;
        }
    }
    return A;
}

<<<<<<< HEAD
// TESTING

void test_charseg(SDL_Surface *img, struct vector *v)
{
    Uint32 pxl = NULL;
    struct coords *c = NULL;
    int w, h;
    for(size_t ci = 0; ci < v->size; i++)
    {
        c = vector_nth(v, i);
        w = c->w2 - c->w1;
        h = c->h2 - c->h1;
        for(int i = 0; i < w; i++)
        {
            pxl = SDL_MapRGB(img->format, 0, 255, 0);
            putpixel(img, c->w1 + i, h, pxl);
        }
        for(int i = 0; i < h; h++)
        {
            pxl = SDL_MapRGB(img->format, 0, 255, 0);
            putpixel(img, c->w1, c->h1 + i, pxl);
        }
    }
}


// LOUP's COMMENTS
=======
struct vector* img_to_lines(double *img)
{
    for(int h = 0;   ;h++)
    {
	return NULL;
    }
}

struct vector* lines_to_char(double *img, struct vector* lines)
{
    return NULL;
}

int line_empty(struct matrix *img, int line)
{
    int i = 0;
    for(; i < matrix -> width && img[] == 0; i++)
    {
	return 0;
    }
    else
	return 1;
}

int column_empty(double *img, int line)
{
    return 0;
}

>>>>>>> b33d7fa680680d6c3697333536e721039443730a

/* 
  Sends back the heights of the beginning (pos_1) and of the end (pos_2) of each line in the given image, 
  which corresponds to the first line with pixels belonging to yet unidentified characters, and the last  
  pixel line before the end of those characters. 
 
  To identify them and save them :  
    - we browse the image from the top to the bottom. 
    - when there is a big enough proportion (to determine) of black pixels in a single line and close to 
      one another, we consider that we're at the beginning of a line. we save the coordinates of the current 
      height in a struct coords (coords -> h = the height, coords -> w is irrelevant). 
    - We do the opposite than before : we browse every pixel line under the recorded one until we find one 
      with proportionnally a lot (to determine) more white pixels than black. We save its location in another 
      struct coords. 
    - We create a struct list_coords with in it the coordinates of the two struct coords previously created  
      (pos_1 = beginning of the line, pos_2 = end of the line). 
    - We find the coordinates of the next line under the one that we've found by repeating the method described 
      above, we save them in another struct list_coords, and we link the new record to the previous one 
      (line_n -> next = line_(n+1)). 
    - When we reach the end of the image, we set the next line under the last line found to NULL, so that 
      we know when we've reached the coordinates of the last character line. 
 
      ABOUT THE PROPORTIONS : 
      - If we consider that a character cannot be composed of a single black pixel (cf the dot of the 'i' in the shell), 
        then an easy way not to take into account potential parasits could be to consider lonely pixels as irrelevant : 
        we do not take them into account. 
      - We could write a function that counts the number of neighbours of every black pixel to determine its importance : 
        for example, if a pixel is part of a "stain" composed by 14 pixels, and if the minimal number of pixels in 
        a potential character is fixed to 10, then the pixel on which we are is eligible to be part of a character and 
        of a characters line ; if we have enough (to determine) pixels like the previous one, then we're on a line. 
 
struct coords *segmentation_to_lines(SDL_SURFACE *img) // Only the h value of the struct coords matters in that one. 
{
    // We assume that all the paragraphs begin at the same width.
    // bxw filter already applied to img
    return NULL;
}
 
  Gives back the list of the coordinates of the corners on the top left and bottom right corners of a square containing a 
  character, for each character a given line.
	
	METHOD :
	- we browse each column into the identified line (starting from the left) until we find a potential character (cf the
	  commentary about the "stains" above).
	- we record the width of the column.
	- we keep browsing the columns until we find one with no black pixels. we save it in another struct coords.
	- to obtain the coordinates of the square in which there is the character, we take as its top left corner a struct coord
	  with the width of the first of the previously recorded positions, and with the height of the beginning of the line. We use the
		width of the second obtained position and the height of the bottom of the line to get the bottom-right corner.
	- we save the two locations in a struct list_coords, representing the square.
	- we link it to the struct list_coords contaning the coordinates of the next square by repeating the same method.
	- when we've reached the last column of the image, we link the struct list_coords containing the coordinates  of the last
	  character to NULL (coord_char_n -> next = NULL).
 
struct list_coords *line_to characters(SDL_SURFACE *img, struct list_coords *line)
{
    return NULL;
}
 
  DEPENDING ON YOUNES AND THEO'S EXPECTATIONS. 
 
  Uses segmentation_to_lines and list_coords to create an ordonate struct list_coords ; the characters are put in it in the same 
  order than they are on the picture, and marks the end of a line by putting a struct list coords with pos_1 and pos_2 fixed on 
  (-1,-1) after the coordinates of the last character of the line. 
 
struct list_coords* segmentation_final_output(SDL_SURFACE *img, struct list_coords *lines)
{
    return NULL;
}*/

