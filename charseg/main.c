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
        display_image(img);
        filter_greyscale(img);
        display_image(img);
        filter_blackwhite(img);
        display_image(img);
				//struct list_coords output = segmentation_final_output(img, lines_to_characters(segmentation_to_lines(img));
				//print_coords(output);
				//free(output);
				SDL_FreeSurface(img);
        SDL_Quit();
    }
    else
    {
        printf("Missing path");
    }
    return 0;
}
