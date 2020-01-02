#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(void)
{
    fprintf(stderr,
            "Trivial secret sharing under n parties so that all n pieces are needed for reconstruction\n"
            "(see https://en.wikipedia.org/wiki/Secret_sharing#t_=_n).\n\n"
            "usage: ./xor [ out <numoutfiles> | in <in1> <in2> ... ]\n\n"
            "XOR for multiple streams.\n\n"
            "out:\tSplit up the input read from stdin into <numoutfiles> files\n"
            "\twhich XORed together result in the original input file (exactly all are needed).\n"
            "\tThis uses numoutfiles-1 random bytestreams of the same length as the input\n"
            "\tand XORes them together.\n\n"
            "in:\tCombine/XOR each input file together to reconstruct the original file\n"
            "\twhich is then printed to stdout.\n\n\n"
            "Example calls:  Split up some file into five parts out1, out2, out3, out4, out5:\n\n"
            "\techo 'Hello-World' | ./xor out 5\n\n"
            "Reconstruct the original file from the five parts (all five are needed),\n"
            "each has to be specified exactly once, the order doesn't matter because\n"
            "XOR is commutative.\n\n"
            "\t./xor in out*\n" "\t# Hello-World\n");
    exit(1);
}

void error(char *msg)
{
    perror(msg);
    usage();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        usage();

    if (!strcmp(argv[1], "in")) {
        if (argc < 4)
            usage();

        int num_infiles = argc - 2;
        FILE **infiles = malloc(num_infiles * sizeof(*infiles));
        if (!infiles)
            error("malloc");
        for (int i = 0; i < num_infiles; i++) {
            infiles[i] = fopen(argv[i + 2], "rb");
            if (!infiles[i])
                error("opening input file");
        }

        int c;                  // char at current position in one file
        while ((c = getc(infiles[0])) != EOF) {
            char xor = c;       // xor of the characters at the current position in all files
            for (int i = 1; i < num_infiles; i++)
                if ((c = getc(infiles[i])) != EOF)
                    xor ^= (char)c;
            putchar(xor);
        }

        for (int i = 0; i < num_infiles; i++)
            fclose(infiles[i]);
        free(infiles);
    } else if (!strcmp(argv[1], "out")) {
        if (argc < 3)
            usage();

        int num_outfiles = atoi(argv[2]);
        if (num_outfiles < 2)
            usage();

        FILE *rand = fopen("/dev/urandom", "rb");
        if (!rand)
            error("opening /dev/urandom");

        FILE **outfiles = malloc(num_outfiles * sizeof(*outfiles));
        if (!outfiles)
            error("malloc");
        char fname[20];
        for (int i = 0; i < num_outfiles; i++) {
            snprintf(fname, 20, "out%d", i);
            outfiles[i] = fopen(fname, "wb");
            if (!outfiles[i])
                error("opening output file");
        }

        int c_in;
        int c_rand;
        while ((c_in = getchar()) != EOF) {
            char xor = (char)c_in;
            for (int i = 0; i < num_outfiles - 1; i++) {
                c_rand = getc(rand);
                if (EOF == c_rand)
                    error("reading from /dev/urandom");
                if (EOF == putc(c_rand, outfiles[i]))
                    error("writing to output file");
                xor ^= c_rand;
            }
            if (EOF == putc(xor, outfiles[num_outfiles - 1]))
                error("writing to output file");
        }

        for (int i = 0; i < num_outfiles; i++)
            fclose(outfiles[i]);
        free(outfiles);

        fclose(rand);
    } else
        usage();
}
