#include <stdio.h>
#include <gmp.h>

#define NRPRIMES 10000

mpz_t fb[NRPRIMES];

void init_factorbase(size_t bits)
{
    int i;
    mpz_init_set_ui(fb[0], 2);

    for(i = 1; i < NRPRIMES; i++) {
        mpz_init(fb[i]);
        mpz_nextprime(fb[i], fb[i-1]);
    }
    return;
}

int trial_div(mpz_t n)
{
    int i;

    for (i = 0; i < NRPRIMES; i++) {
        if(mpz_divisible_p(n, fb[i])) {
            return 0;
        }
    }
    return 1;
}
