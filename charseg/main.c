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
	struct coords test;
	test.h1 = 5;
	test.h2 = 10;
	test.w1 = 5;
	test.w2 = 30;
	struct vector *V = vector_make(1);
	vector_push_front(V, test);
	display_segmentation(img, V);
	//test_charseg(img, V);
	display_image(img);
	//struct vector *V = img_to_lines(A);
	//struct vector *F = lines_to_char(A, V);
	//test_charseg(img, F);
	//display_image(img);
	SDL_FreeSurface(img);
        SDL_Quit();
    }
    else
    {
        printf("Missing path");
    }
    return 0;
}
