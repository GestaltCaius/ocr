#ifndef NEURAL_H_INCLUED
#define NEURAL_H_INCLUED

struct neuron {
    double *w;   // tableau de poids
    size_t nw;   // nbs de poids
    double b;    // bias
    double lder; // dernière valeur des dériver (error_der * sig_der)
    double lout; // dernière sortie
};

struct network {
    size_t *L;         // tailles des layers
    size_t nL;         // nombre de layers
    struct neuron **n; // tableau de neurone (L[i] * nl);
};

#endif
