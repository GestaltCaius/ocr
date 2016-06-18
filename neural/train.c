
struct try
    *init_try_folder(char *path) {
        FILE *fp = fopen(path, "r");
        
        char *path_tmp, *i_char;
        ssize_t read;


        size_t nb_line = 0;
        size_t nb_char = 26 * 2 + 10;
        struct try
            *trys = calloc(nb_line * nb_char, sizeof(struct try));
        init_sdl();
        size_t *n;
        for(size_t i = 0; i < nb_line; i++)
        {
            char *path_tmp, *i_char;
            ssize_t read;
            read = getline(&path_tmp,&n,fp);
            for(size_t j = 0; j < nb_char; j++)
            {
                sprintf(i_char, "%d", i);
                strcat(path_tmp, i_char);
                strcat(path_tmp, ".png");

                SDL_Surface *img = load_image(str);
                trys[i].in = calloc(img->w * img->h, sizeof(double));
                for(int k = 0; k < img->w; k++)
                {
                    for(int l = 0; l < img->h; l++)
                    {
                        Uint32 pixel = getpixel(img,l,k);
                        Uint8 r = 0, g = 0, b = 0;
                        SDL_GetRGB(pixel, img->format, &r, &g, &b);
                        Uint8 res = r * 0.3 + g * 0.59 + b * 0.11;
                        if (res < 127)
                            trys[i*nb_line + j].in[k * (img->w) + l] = 1;
                        else
                            trys[i*nb_line + j].in[k * (img->w) + l] = 0;
                    }
                }
            }
        }
    }
