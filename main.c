#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gmp.h>

#define TRY_LITTLE_ENDIAN 1
#define TRY_BIG_ENDIAN 2
#define TRY_ALL_ENDIAN TRY_LITTLE_ENDIAN|TRY_BIG_ENDIAN

void hexdump (char *info, void *addr, int len)
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    if(len <= 0)
        return;

    if(info != NULL)
        printf("%s\n", info);

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                printf ("  %s\n", buff);

            printf ("  %04x ", i);
        }

        printf (" %02x", pc[i]);

        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    printf ("  %s\n", buff);
}

void number_checks(mpz_t n, size_t bits, size_t offset)
{
    /* Check if the number is not full of zeros */
    if (mpz_sizeinbase(n, 2) > (bits-2) ) {
        /* Is it prime ? */
        if (mpz_probab_prime_p(n, 20) > 0) {
            printf("Prime of %zd bits found at offset 0x%zx : ", mpz_sizeinbase(n, 2), offset);
            mpz_out_str(stdout, 16, n);
            printf("\n");
        }
    }

    return;
}

int find_numbers(uint8_t *data, size_t len, size_t bits, uint8_t align, uint8_t endian_flags)
{
    /* Temporary storage for bignum */
    mpz_t n;
    size_t i;

    mpz_init(n);

    for (i = 0; i <= (len-bits/8); i+= align) {
        /* use mpz_import to convert from raw data*/
        /* Big endian : len bytes, MSB first, MSB first in each byte */
        mpz_import(n, bits/8, 1, 1, 1, 0, data+i);
        number_checks(n, bits, i);
    }
    mpz_clear(n);
    
    return 0;
}

int main(int argc, char *argv[])
{
    uint8_t *data = NULL;
    size_t data_len;
    FILE *f;
    int fno;
    struct stat st;
    int res = 0;
    int opt, bits = 0;


    while ((opt = getopt(argc, argv, "b:")) != -1) {
        switch (opt) {
        case 'b':
            bits = atoi(optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-t bitsize] file\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    f = fopen(argv[optind], "rb"); 
    if (f == NULL) {
        perror("Can't open file\n");
        return 1;
    }
    fno = fileno(f);
    fstat(fno, &st);

    data_len = st.st_size;
    data = (uint8_t *)malloc(data_len);
    if(data == NULL) {
        perror("Couldn't allocate memory for file");
        res = 1;
        goto exit;
    }

    if(fread(data, 1, data_len, f) != (size_t)data_len) {
        printf("couldn't read data file\n");
        res = 1;
        goto exit;
    }
    fclose(f); f = NULL;

    printf("Trying to find %d bits crypto numbers\n", bits);
    find_numbers(data, data_len, bits, 1, 0);

    

exit:
    if (f != NULL)
        fclose(f);
    if (data != NULL)
        free(data);
    return res;
}
