# include "pixel_operations.h"
# include "filters.h"
# include "loadimage.h"
# include "segmentation.h"
# include "neural.h"
# include <SDL/SDL.h>
# include <SDL/SDL_image.h>

char array_to_char(int *array)
{
    if(array[0] == 1)
        return '0';
    if(array[1] == 1)
        return '1';
    if(array[2] == 1)
        return '2';
    if(array[3] == 1)
        return '3';
    if(array[4] == 1)
        return '4';
    if(array[5] == 1)
        return '5';
    if(array[6] == 1)
        return '6';
    if(array[7] == 1)
        return '7';
    if(array[8] == 1)
        return '8';
    if(array[9] == 1)
        return '9';
    
    return 'e';
}

struct network tr_and_init_network()
{
    size_t L[] = {16*16, 10, 10};
    struct network net = init_network(L,3);
    struct try *tr = init_numbers_0_to_9("./nbs/");
    train(&net, tr, 10, 100000, 10000);
    return net;
}

struct vector* create_vector(struct matrix *A)
{
    struct vector *V = img_to_lines(A);
    struct vector *F = lines_to_char(A, V);
    F = resize_char(A,F);
    return F;
}


void small_ocr(struct network *net, char *fname, struct matrix *A, struct vector *F)
{
    FILE *file;
    if ( (file = fopen(fname, "w")) == NULL) {
        err(3, "Error while creating %s", fname);
    }
    printf(" == BEGIN OCR == \n");
    for(size_t i = 0; i < F->size; i++)
    {
        double *in = resize_table(F->data[i],A, 16, 16);
        feedforward(net,in);
        int *out = get_bin_out(*net);
        char res = array_to_char(out);
        fprintf(file, "%c", res);
        printf("%c",res);
    }
}
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
    
    struct vector *F = create_vector(A); 
    printf("%zu\n",F->size);
    fflush(stdout);
/*	for(size_t i = 0; i < F->size; i++)
    {
        printf("%zu | %d - %d\n%d - %d\n",i,F->data[i].h1,F->data[i].w1,F->data[i].h2,F->data[i].w2);
    }*/
    
    display_segmentation(img, F);

    
	display_image(img);
	
    struct network net = tr_and_init_network();


    small_ocr(&net, "out.txt",A,F);

    printf("\n == FIN == \n");
    fflush(stdout);
	SDL_FreeSurface(img);
        SDL_Quit();
    }
    else
    {
        printf("Missing path");
    }
    return 0;
}
