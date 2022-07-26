/*
 * Copyright 2022 SdtElectronics
 *
 * SPDX-License-Identifier: Zlib
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef MZ_EG_USE_ZLIB
#include <zlib.h>
#else
#include "zlib_comp.h"
#endif

/* only integer multiples of 3 are valid values     
   otherwise hexadecimal representation of bytes grouped in 3 characters 
   can be cut in half and produce faulty results                        */
#ifndef MZ_EG_INPUT_BUFFER_SIZE
#define MZ_EG_INPUT_BUFFER_SIZE 9192
#endif

#ifndef MZ_EG_OUTPUT_BUFFER_SIZE
#define MZ_EG_OUTPUT_BUFFER_SIZE ( 4*MZ_EG_INPUT_BUFFER_SIZE )
#endif

#ifdef MZ_EG_HEXIN
/* Decode a hexadecimal string, set *len to length, in[] to the bytes.  This
   decodes liberally, in that hex digits can be adjacent, in which case two in
   a row writes a byte.  Or they can be delimited by any non-hex character,
   where the delimiters are ignored except when a single hex digit is followed
   by a delimiter, where that single digit writes a byte.  The returned data is
   allocated and must eventually be freed.  NULL is returned if out of memory.
   If the length is not needed, then len can be NULL. */
unsigned char *h2b(const char *hex, unsigned *len) {
    unsigned char *in, *re;
    unsigned next, val;

    in = (unsigned char*)malloc((strlen(hex) + 1) >> 1);
    if (in == NULL)
        return NULL;
    next = 0;
    val = 1;
    do {
        if (*hex >= '0' && *hex <= '9')
            val = (val << 4) + *hex - '0';
        else if (*hex >= 'A' && *hex <= 'F')
            val = (val << 4) + *hex - 'A' + 10;
        else if (*hex >= 'a' && *hex <= 'f')
            val = (val << 4) + *hex - 'a' + 10;
        else if (val != 1 && val < 32)  /* one digit followed by delimiter */
            val += 240;                 /* make it look like two digits */
        if (val > 255) {                /* have two digits */
            in[next++] = val & 0xff;    /* save the decoded byte */
            val = 1;                    /* start over */
        }
    } while (*hex++);       /* go through the loop with the terminating null */
    if (len != NULL)
        *len = next;
    re = (unsigned char*)realloc(in, next);
    return re == NULL ? in : re;
}
#endif

int main(int argc, char** argv){
    #ifdef MZ_EG_HEXIN
    assert(MZ_EG_INPUT_BUFFER_SIZE % 3 == 0);
    #endif
    unsigned int in_bytes;
    unsigned char in_buf[MZ_EG_INPUT_BUFFER_SIZE];
    unsigned char out_buf[MZ_EG_OUTPUT_BUFFER_SIZE];
    z_stream strm;

    /* ======= This initialization is not necessary for muzic ====== */
    /* === It's preserved merely for the compatibility with zlib === */
    {    
        strm.next_in  = Z_NULL;
        strm.avail_in = 0;

        strm.zalloc = Z_NULL;
        strm.zfree  = Z_NULL;
        strm.opaque = Z_NULL;
    }

    int prev_state = inflateInit(&strm);

    if(prev_state != Z_OK) return prev_state;

    #ifdef MZ_EG_HEXIN
    unsigned int in_bin_len = 0;
    unsigned char* in_bin;
    #endif

    strm.avail_out = sizeof(out_buf);

    do {
        /* inflate expects more input, read more */
        if(strm.avail_out > 0){ 
            in_bytes = read(STDIN_FILENO, in_buf, sizeof(in_buf) - 1);
            // printf("%d\n ", in_bytes);
            if(in_bytes == 0) break;
            if(in_bytes == -1) return -1;
            #ifdef MZ_EG_HEXIN
            in_buf[in_bytes] = '\0';
            in_bin = h2b(in_buf, &in_bin_len);
            if(in_bin_len == 0) break;
            strm.next_in  = in_bin;
            strm.avail_in = in_bin_len;
            #else
            strm.next_in  = in_buf;
            strm.avail_in = in_bytes;
            #endif
        }

        strm.next_out  = out_buf;
        strm.avail_out = sizeof(out_buf);

        prev_state = inflate(&strm, Z_SYNC_FLUSH);
        /* printf("total_out: %d\n ", strm.total_out); */
        /* Don't free decoded binary if output buffer run out.  */
        /* Still need them in the next iteration                */
        if(strm.avail_in == 0){
            #ifdef MZ_EG_HEXIN
            free(in_bin);
            #endif
        }else if(prev_state == Z_STREAM_END){
            unsigned char* prev_in = strm.next_in;
            unsigned int prev_avail = strm.avail_in;
            inflateInit(&strm);
            strm.next_in = prev_in;
            strm.avail_in = prev_avail;
        }
        
        /* If there is an error in the input, don't print corrupted output */
        if(prev_state != Z_OK && prev_state != Z_BUF_ERROR && prev_state != Z_STREAM_END){ 
            break;
        }

        /* printf("%.*s", strm.next_out - out_buf, out_buf); */
        fwrite(out_buf, sizeof(char), strm.next_out - out_buf, stdout);
        fflush(stdout);
        
    } while(in_bytes > 0);

    inflateEnd(&strm);

    if(prev_state == Z_STREAM_END){
        return 0;
    }else{
        return prev_state;
    }
}
