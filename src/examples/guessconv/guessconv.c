/*
 * Copyright (c) 2010 William Pitcock <nenolod@atheme.org>.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the authors nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libguess.h>
#include <iconv.h>
#include <errno.h>
#include <langinfo.h>
#include <locale.h>

static void
print_version(void)
{
    printf("Atheme guessconv, version 1.0.\n");
    printf("Copyright (c) 2010 William Pitcock <nenolod@atheme.org>\n");
    printf("See COPYING in the libguess source tree for license details.\n");
    printf("\nReport bugs to <http://jira.atheme.org/> against the libguess product.\n");

    exit(EXIT_SUCCESS);
}

static void
print_usage(const char *progname)
{
    fprintf(stderr, "%s: usage: %s -l language -i infile -o outfile\n", progname, progname);
    fprintf(stderr, "%s: usage: %s -v\n", progname, progname);

    exit(EXIT_FAILURE);
}

/*
 * read_buffer()
 *
 * Read a file descriptor's contents into a memory buffer.
 * The returned memory buffer must be free()d.
 * insize contains the size of the buffer.
 */
static char *
read_buffer(int fd, size_t *insize)
{
    char *inbuf = NULL, *inptr = NULL;
    size_t maxlen = 0, actlen = 0;

    while (actlen < maxlen)
    {
        ssize_t n = read(fd, inptr, maxlen - actlen);

        if (n == 0)
            break;

        if (n == -1)
            return NULL;

        inptr += n;
        actlen += n;
    }

    if (actlen == maxlen)
    {
        while (1)
        {
            ssize_t n;
            char *new_inbuf;

            new_inbuf = (char *) realloc (inbuf, maxlen + 32768);
            if (new_inbuf == NULL)
                return NULL;

            inbuf = new_inbuf;
            maxlen += 32768;
            inptr = inbuf + actlen;

            do
            {
                n = read (fd, inptr, maxlen - actlen);

                if (n == 0)
                    break;

                if (n == -1)
                    return NULL;

                inptr += n;
                actlen += n;
            } while (actlen < maxlen);

            if (n == 0)
                break;
        }
    }

    *insize = actlen;
    return inbuf;
}

void
write_buffer(iconv_t cd, char *addr, size_t len, FILE *outfile)
{
#define OUTBUF_SIZE     32768
    char outbuf[OUTBUF_SIZE];
    char *outptr;
    size_t outlen;
    size_t n;
    int ret = 0;

    while (len > 0)
    {
        outptr = outbuf;
        outlen = OUTBUF_SIZE;
        n = iconv (cd, &addr, &len, &outptr, &outlen);

        if (n == (size_t) -1)
        {
            ret = 1;
            if (len == 0)
                n = 0;
            else
                errno = E2BIG;
        }

        if (outptr != outbuf)
        {
            ret = fwrite(outbuf, 1, (outptr - outbuf), outfile);
            if (ret <= 0)
                break;
        }

        if (n != -1)
        {
            outptr = outbuf;
            outlen = OUTBUF_SIZE;
            n = iconv (cd, NULL, NULL, &outptr, &outlen);

            if (outptr != outbuf)
            {
                ret = fwrite(outbuf, 1, (outptr - outbuf), outfile);
                if (ret <= 0)
                    break;
            }

            if (n != -1)
                break;
        }

        if (errno != E2BIG)
        {
            fprintf(stderr, "iconv() failed: %s\n", strerror(errno));
            break;
        }
    }
}

int
main(int argc, char **argv)
{
    char r;
    const char *lang = NULL, *enc, *outenc;
    FILE *infile = stdin, *outfile = stdout;
    iconv_t ic;
    char *indata;
    size_t insize = 0;

    if (argc < 2)
        print_usage(argv[0]);

    setlocale(LC_ALL, "");
    outenc = nl_langinfo(CODESET);

    while ((r = getopt(argc, argv, "l:i:o:O:v")) != -1)
    {
        switch(r)
        {
        case 'l':
            lang = optarg;
            break;
        case 'i':
            fprintf(stderr, "Input file: %s\n", optarg);
            if ((infile = fopen(optarg, "r")) == NULL)
            {
                fprintf(stderr, "%s: unable to open '%s' for reading: %s\n", argv[0], optarg, strerror(errno));
                return EXIT_FAILURE;
            }
            break;
        case 'o':
            fprintf(stderr, "Output file: %s\n", optarg);
            if ((outfile = fopen(optarg, "w")) == NULL)
            {
                fprintf(stderr, "%s: unable to open '%s' for writing: %s\n", argv[0], optarg, strerror(errno));
                return EXIT_FAILURE;
            }
            break;
        case 'O':
            outenc = optarg;
            break;
        case 'v':
            print_version();
            break;
        default:
            print_usage(argv[0]);
            break;
        }
    }

    if (lang == NULL)
        print_usage(argv[0]);

    if ((indata = read_buffer(fileno(infile), &insize)) == NULL)
    {
        fprintf(stderr, "%s: error while reading input buffer: %s\n", argv[0], strerror(errno));
        return EXIT_FAILURE;
    }

    enc = libguess_determine_encoding(indata, insize, lang);
    if (enc == NULL)
    {
        fprintf(stderr, "%s: unable to determine encoding of input data for language: %s\n", argv[0], lang);
        free(indata);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "Input encoding: %s\n", enc);
    fprintf(stderr, "Output encoding: %s\n", outenc);    

    ic = iconv_open(outenc, enc);
    write_buffer(ic, indata, insize, outfile);
    iconv_close(ic);

    free(indata);
    return EXIT_SUCCESS;
}
