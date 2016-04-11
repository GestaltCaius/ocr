# include <stdlib.h>
# include <stdio.h>
# include <SDL/SDL.h>
# include <SDL/SDL_image.h>

# include "pixel_operations.h"
# include "vector.h"
# include "loadimage.h"

// img to matrix of 0's and 1's
// 1 = black, 0 = white
struct matrix {
    double *data;
    int width, height;
};

struct matrix* img_to_matrix(SDL_Surface *img)
{
    Uint32 pxl;
    Uint8 r;
    struct matrix* A = malloc(sizeof(struct matrix));
    A -> data = malloc(sizeof(double) * (img->w * img->h));
    A -> width = img -> w;
    A -> height = img -> h;
    for (int h = 0; h < img->h; h++)
    {
        for (int w = 0; w < img->w; w++)
        {
            pxl = getpixel(img, w, h);
            SDL_GetRGB(pxl, img->format, &r, &r, &r);
            A->data[h * img->w + w] = r == 255 ? 0 : 1;
//	    printf("%d ", (int)(A->data[h * img->w + w]));
        }
//	printf("\n");
    }
    return A;
}

void draw_square(SDL_Surface *img, struct coords square)
{
    Uint32 pxl;
    pxl = SDL_MapRGB(img->format, 0, 255, 0);
    for(int i = square.h1; i <= square.h2; i++)
    {
	putpixel(img, square.w1, i, pxl);
	putpixel(img, square.w2, i, pxl);
    }
    for(int i = square.w1 + 1; i < square.w2 - 1; i++)
    {
	putpixel(img, i, square.h1, pxl);
	putpixel(img, i, square.h2, pxl);
    }
}

void display_segmentation(SDL_Surface *img, struct vector *V)
{
    size_t i = 0;
    while(i < V->size)
    {
	draw_square(img, V->data[i]);
	i++;
    }
}

int line_is_empty(struct matrix *img, int line)
{   
    int i = 0;
    for(; i < img -> width && img -> data[line * img->width+ i] == 0; i++){ }
    return(i == img -> width);
}

int column_is_empty(struct matrix *img, int x, int y1, int y2)
{
    int i = y1;
    for(; i < y2 && img->data[i * img->width + x] == 0;i++) {}
    return i == y2;
}

int small_line_is_empty(struct matrix *img, int y, int w1, int w2)
{
    int i = w1;
    for(; i < w2 && img->data[y * img->width + i] == 0; i++) { }
    return i == w2;
}


struct vector* img_to_lines(struct matrix *img)
{
    struct vector *lines = vector_make((img -> height) * (img -> width));
    int status = 0; // not on a line
    int h = 0;
    for(; h < img -> height ; h++)
    {
	if(status)
	{
	    struct coords line;
	    line. w1 = 1;
	    line.w2 = 1;
	    line.h1 = h - 1;
	    for(; h < img -> height && !line_is_empty(img, h); h++) { }
	    line.h2 = h - 1;
	    vector_push_back(lines, line);
	    status = 0;
	}
	else
	{
	    for(; h < img -> height && line_is_empty(img, h); h++) { }
	    status = 1;
	}
    }
    return lines;
}


struct vector *lines_to_char(struct matrix *img, struct vector *lines)
{
    struct vector *imgs = vector_make((img -> height) * (img -> width));
    //printf("// begin\n");
    //fflush(stdout);
    for(size_t k = 0; k < lines->size; k++)
    {
    //printf("// %zu | %zu: \n", k, lines->size);
    //fflush(stdout);
    struct coords actual_coords;
    actual_coords.w1 = 0;
    actual_coords.w2 = 0;
    actual_coords.h1 = lines->data[k].h1;
    actual_coords.h2 = lines->data[k].h2;
    int recording = 0;

    for(int i = 0; i < img->width; i++)
    {
        //printf("// %d | %d - %d\n", i, img->width, img->height);
        if(!recording)
        {
            if(!column_is_empty(img,i,lines->data[k].h1,lines->data[k].h2))
            {
                recording = 1;
                actual_coords.w1 = i;
                actual_coords.w2 = i;
            }
        }
        else
        {
            if(!column_is_empty(img,i,lines->data[k].h1,lines->data[k].h2))
            {
                actual_coords.w2++;
            }
            else
            {
                vector_push_back(imgs, actual_coords);
                recording = 0;
            }

        }
    }
    }
    return imgs;
}

struct vector *resize_char(struct matrix *img, struct vector *chars)
{
    for(size_t i = 0; i < chars->size; i++)
    {
        while(small_line_is_empty(img, chars->data[i].h1, chars->data[i].w1, chars->data[i].w2))
        {
            chars->data[i].h1++;
        }
        
        while(small_line_is_empty(img, chars->data[i].h2, chars->data[i].w1, chars->data[i].w2))
        {
            chars->data[i].h2--;
        }
    }
    return chars;

}

/*struct vector* lines_to_char(struct matrix *img, struct vector* lines)
{
    return NULL;
}*/

/*int column_empty(struct matrix *img, int line)
{
    return 0;
}*/

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

