# include "resize.h"


double* resize_table(struct coords c, double table[], int x, int y)
{
    double ox = c.h2 - c.h1;
    double oy = c.h2 - c.h1;
    double modi = x / ox;
    double modj = y / oy;

    double *new_t = calloc(x*y,sizeof(double));
    for(int i = 0; i < x; i++)
    {
        for(int j = 0; j <= y; j++)
        {
            new_t[i*x+j] = table[(int)(i*modi*ox+j*modj)];
        }
    }

    return new_t;
}
