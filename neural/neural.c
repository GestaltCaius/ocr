# include <assert.h>
# include <stdio.h>
# include <stdlib.h>
# include <math.h>

struct neuron{
    double *w; // tableau de poids
    size_t nw; // nbs de poids
    double b; // bias
    double lder; // dernière valeur des dériver (error_der * sig_der)
    double lout; // dernière sortie
};

struct network{
    size_t *L; // tailles des layers
    size_t nL; // nombre de layers
    struct neuron **n; // tableau de neurone (L[i] * nl);
};

struct try{
    double *in; //essai entrée
    double *res; // essai resultat attendu
};

// inits fonctions
struct neuron init_neuron(size_t nw, int ent)
{
    struct neuron new;
    new.nw = nw;
    new.w = calloc(nw,sizeof(size_t));

    // set random weight;
    if(ent)
    {
        for(size_t i = 0; i < nw; i++)
        {
            new.w[i] = 0;
        }
        new.b = 0;
    }
    else
    {
        for(size_t i = 0; i < nw; i++)
        {
            new.w[i] = (((double)rand() / (double)RAND_MAX) - 0.5) * 4 ;
        }

        // set random bias
        new.b = (((double)rand() / (double)RAND_MAX) - 0.5) * 4;
    }
    // set last vals to 0
    new.lder = 0;
    new.lout = 0;

    return new;
}


void free_network_neurons(struct network* net)
{
    for(int i = 0; i < net->nL; i++)
    {
        for(size_t j = 0; j < net->L[i]; j++)
            free(net->n[i][j].w);
        free(net->n[i]);
    }
    free(net->n);
}

struct network init_network(size_t *L, size_t nL)
{
    struct network new;
    new.L = L;
    new.nL = nL;

    // create neurons
    new.n = calloc(nL, sizeof(struct neuron*));
    for(size_t i = 0; i < nL; i++)
    {   
        new.n[i] = calloc(L[i],sizeof(struct neuron));
        for(size_t j = 0; j < L[i]; j++)
        {
            if(i == 0)
            {
                new.n[i][j] = init_neuron(0,1);
            }
            else
            {
                new.n[i][j] = init_neuron(L[i - 1],0);
            }
        }
    }
    return new;
}

struct try* init_try_xor()
{
    struct try *trys = calloc(4,sizeof(struct try));
    
    double input[4][2] = { {0,0} , {0,1} , {1,0} , {1,1} };
    double output[4] = {   0 ,     1 ,     1 ,     0     };

    for(int i = 0; i < 4; i++)
    {
        (trys[i]).in = calloc(2,sizeof(double));
        (trys[i]).in[0] = input[i][0];
        (trys[i]).in[1] = input[i][1];

        (trys[i]).res = calloc(1,sizeof(double));
        (trys[i]).res[0] = output[i];
    }

    return trys;
}

void free_trys(struct try* tries, size_t number)
{
    for(size_t i = 0; i < number; i++)
    {
        free(tries[i].in);
        free(tries[i].res);
    }
    free(tries);
}

double* get_out(struct network net)
{
    double *out = calloc(net.L[net.nL - 1], sizeof(double));
    for(size_t i = 0; i < net.L[net.nL - 1]; i++)
    {
        out[i] = net.n[net.nL - 1][i].lout;
    }
    return out;
}

//sigmoid

double sigmoid(double z)
{
    return 1.0/(1.0+exp(-z));
}

//dérivée sigmoid
double psigmoid(double z)
{
    double sig = sigmoid(z);
    return sig*(1.0-sig);
}

double dif_error(double y, double ti)
{
    return 0.5*(y-ti)*(y-ti);
}


double dot_product(struct neuron* a, double* b,size_t n)
{
    double result = 0;
    for(size_t i = 0; i < n; i++)
        result += a[i].lout*b[i];
    return result;
}


void feedforward(struct network *n, double *in)
{
    double zj;
    for(size_t i = 0; i < (*n).nL; i++)
    {
        for(size_t j = 0; j < (*n).L[i]; j++)
        {
            if(i == 0)
            {
                (*n).n[i][j].lout = in[j];
            }
            else
            {   
                // zf = x[] . w[] + b
                zj = dot_product((*n).n[i - 1],(*n).n[i][j].w,(*n).L[i - 1]) + (*n).n[i][j].b;
                (*n).n[i][j].lout = sigmoid(zj);
            }
        }
    }
}

void backpropa(struct network *n, double eta, struct try in)
{
    double der = 0;
    for(size_t i = (*n).nL - 1; i > 0; i--)
    {
        for(size_t j = 0; j < (*n).L[i]; j++)
        {
            if(i == (*n).nL - 1) // out layer
            {
                // w_k -= eta * (out_(i)(j) - res_voulu) * sig'((out_(i-1)[] . w_i[]) + b_i) * out_(i-1)(k)
                (*n).n[i][j].lder = ( (*n).n[i][j].lout - in.res[j] );
                (*n).n[i][j].lder *= psigmoid(dot_product((*n).n[i-1],(*n).n[i][j].w,(*n).L[i-1]) + (*n).n[i][j].b);
                for(size_t k = 0; k < (*n).L[i-1]; k++)
                {

                    der =  (*n).n[i][j].lder * (*n).n[i-1][k].lout;
                    (*n).n[i][j].w[k] -= eta * der;
                }
                // bias
             //   printf("\n der1: %f\n",der); 
                (*n).n[i][j].b -= eta * (*n).n[i][j].lder;
            }
            else //hiden layer
            {
                // w_k -= eta * sum( lder_(i+1)(j) * w_(i+1)(j) ) * sig'((out_(i-1)[] . w_i[]) + b_i) * out_(i - 1)(k)
                (*n).n[i][j].lder = 0;
                
                for(size_t l = 0; l < (*n).L[i + 1]; l++)
                {   
                    (*n).n[i][j].lder += (*n).n[i + 1][l].lder * (*n).n[i + 1][l].w[j];
                }
                (*n).n[i][j].lder *= psigmoid(dot_product((*n).n[i-1],(*n).n[i][j].w,(*n).L[i-1]) + (*n).n[i][j].b);
                for(size_t k = 0; k < (*n).L[i - 1]; k++)
                {
                    (*n).n[i][j].w[k] -= eta * (*n).n[i][j].lder * (*n).n[i-1][k].lout;
                }
               (*n).n[i][j].b -= eta * (*n).n[i][j].lder; 
                
            }
        }
    }
}

// random sort array
void shuffle_ex(struct try *array,size_t n)
{
    if(n > 1)
    {
        size_t i;
        for(i = 0; i < n; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            struct try t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}



void train(struct network *net, struct try *tr, size_t nbval, size_t nite, size_t display)
{
    double *result;
    for(size_t i = 0; i <= nite; i++)
    {
        shuffle_ex(tr,nbval);
        if(display > 0 && i % display == 0)
            printf("%zu:\n",i);
        for(size_t j = 0; j < nbval; j++)
        {
            feedforward(net, tr[j].in);
            if(display > 0 && i % display == 0)
            {
                result = get_out(*net);
                
                printf("\n\tIN:\n\t|");
                fflush(stdout);
                for(size_t k = 0; k < (*net).L[0]; k++)
                    printf(" %f |",tr[j].in[k]);

                printf("\n\tOUT:\n\t|");
                for(size_t k = 0; k < (*net).L[(*net).nL - 1]; k++)
                    printf(" %f |",result[k]);
                
                printf("\n\tRES:\n\t|");
                for(size_t k = 0; k < (*net).L[(*net).nL - 1]; k++)
                    printf(" %f |",tr[j].res[k]);
           
                printf("\n\tERROR:\n\t|");
                for(size_t k = 0; k < (*net).L[(*net).nL - 1]; k++)
                    printf(" %f |",dif_error(result[k],tr[j].res[k]));
            printf("\n");
                free(result);
            }
            backpropa(net,0.01,tr[j]);
        }

        if(display > 0 && i % display == 0)
            printf("\n\n");

    }
}


int main(){

size_t L[] = {2, 3 , 1};
struct network net = init_network(L,3);
struct try *tr = init_try_xor();

train(&net, tr, 4, 1000000, 100000);

printf("end");
fflush(stdout);

free_trys(tr, 4);
free_network_neurons(&net);

return 0;

}
