/*
 * I found this file here:
 * http://www.pell.portland.or.us/~orc/Code/bsd/bsd-current/00library/fgetln.c
 * Alas it contained no copyright notice. In the same dir there
 * was the following note though. So please don't sue me for
 * changing things a bit. I also would have sent the updates if I
 * could have found your email. But I didn't.
 */

/*-
 * Copyright (c) 1994
 * The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *       This product includes software developed by the
 *       University of California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its
 *    contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT
 * SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)COPYRIGHT8.1 (Berkeley) 3/16/94
 */

/*
 * fgetln() for linux.   Hacked up for sed, et al.
 *
 * BSD tweaks around with the inside of a FILE*; we can't do that
 * in the land of GNU libc, so we keep a private array of FILE*
 * keys and pointer/size pairs to hold the lines.
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#define MAXFILES	20

typedef struct {
    FILE *key;		/* file descriptor this bfr is associated with */
    char *bfr;		/* buffer for latest fgetln()ed line */
    int buflen;		/* length of the buffer */
} LINEREC;

static LINEREC tbl[MAXFILES] = {{ NULL }};

char*
fgetln(FILE *fd, size_t *len)
{
    int idx;
    int c, lnsize;

    /* we want to do fgetln, eh?  Check to see if all the variables
     * are okay first (don't want to waste space if the file is bad
     * or the len argument is bogus (==zero)
     */
    if (fd == 0 || len == 0) {
	errno = EINVAL;
	return 0;
    }

    /* find an available slot in the table
     */
    for (idx = 0; idx < MAXFILES; idx++)
	if (tbl[idx].key == fd)
	    break;

    if (idx == MAXFILES) {
	for (idx=0; idx < MAXFILES; idx++)
	    if (tbl[idx].key == 0) {
		/* usable file slot */
		break;
	    }

	if (idx == MAXFILES) {
	    errno = ENFILE;
	    return 0;
	}
    }

    /* Initialize the buffer, if needed
     */
    if (tbl[idx].bfr == 0) {
	tbl[idx].bfr = malloc(200);
	if (tbl[idx].bfr == 0)
	    return 0;
	tbl[idx].buflen = 200;
    }
    tbl[idx].key = fd;

    /* we've got a buffer, so lets start writing a line into
     * it.
     */
    for (lnsize = 0; (c = getc(fd)) != EOF; ) {
	tbl[idx].bfr[lnsize++] = c;
	if (lnsize == tbl[idx].buflen) {
	    tbl[idx].bfr = realloc(tbl[idx].bfr, tbl[idx].buflen * 2);
	    if (tbl[idx].bfr == 0)
		return 0;
	    tbl[idx].buflen *= 2;
	}
	if (c == '\n')
	    break;
    }

    /* save out the size, then return to sender
     */
    *len = lnsize;
    return lnsize ? tbl[idx].bfr : 0;
} /* fgetln */
