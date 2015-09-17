#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
    printf("Trying to find %d bits crypto numbers", bits);

    

exit:
    if (f != NULL)
        fclose(f);
    if (data != NULL)
        free(data);
    return res;
}
