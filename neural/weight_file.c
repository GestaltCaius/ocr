#include <stdio.h>

#include "weight_file.h"

int save_network_to_file(struct network* net, char* filename)
{
    FILE *f;
    
    if(!(f = fopen(filename, "w")))
        return -1;

    fprintf(f, "%zu\n", net->nL);

    for(size_t l = 0; l < net->nL; l++)
        fprintf(f, "%zu ", net->L[l]);

    fprintf(f, "\n");

    //Write the weight (then bias) of the output layer, then of the hidden one
    for(size_t l = net->nL - 1; l > 0; l--)
    {
        for(size_t n = 0; n < net->L[l]; n++)
        {
            for(size_t w = 0; w < net->n[l][n].nw; w++)
                fprintf(f, "%a ", net->n[l][n].w[w]);
            fprintf(f, "%a\n", net->n[l][n].b);
        }
    }

    fclose(f);

    return 0;
}

struct network* load_network_from_file(char* filename)
{
    return NULL;
}
