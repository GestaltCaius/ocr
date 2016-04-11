# include "pixel_operations.h"
# include "filters.h"
# include "loadimage.h"
# include "segmentation.h"
# include <SDL/SDL.h>
# include <SDL/SDL_image.h>

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        init_sdl();
        SDL_Surface *img = load_image(argv[1]);
        //display_image(img);
        filter_greyscale(img);
        //display_image(img);
        filter_blackwhite(img);
        //display_image(img);
	struct matrix *A = img_to_matrix(img);
	//struct coords test;
	//test.h1 = 5;
	//test.h2 = 10;
	//test.w1 = 5;
	//test.w2 = 30;
	//struct vector *V = vector_make(1);
	//vector_push_front(V, test);
	struct vector *V = img_to_lines(A);
	
    for(size_t i = 0; i < V->size; i++)
    {
        printf("%d - %d\n",V->data[i].h1,V->data[i].h2);
    }
    
    
    struct vector *F = lines_to_char(A, V);
    F = resize_char(A,F);
    printf("%zu\n",F->size);
    fflush(stdout);
/*	for(size_t i = 0; i < F->size; i++)
    {
        printf("%zu | %d - %d\n%d - %d\n",i,F->data[i].h1,F->data[i].w1,F->data[i].h2,F->data[i].w2);
    }*/
    
    display_segmentation(img, F);


	display_image(img);
	printf("FIN");
	SDL_FreeSurface(img);
        SDL_Quit();
    }
    else
    {
        printf("Missing path");
    }
    return 0;
}
