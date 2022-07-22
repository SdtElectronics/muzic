/*
 * Copyright 2022 SdtElectronics
 *
 * SPDX-License-Identifier: Zlib
 */

#include <stdio.h>
#include <unistd.h>

#ifdef MZ_EG_USE_ZLIB
#include <zlib.h>
#else
#include "zlib_comp.h"
#endif

#ifndef MZ_EG_INPUT_BUFFER_SIZE
#define MZ_EG_INPUT_BUFFER_SIZE 256
#endif

#ifndef MZ_EG_OUTPUT_BUFFER_SIZE
#define MZ_EG_OUTPUT_BUFFER_SIZE ( 4*MZ_EG_INPUT_BUFFER_SIZE )
#endif

int main(int argc, char** argv){
    unsigned int in_bytes;
    unsigned char in_buf[MZ_EG_INPUT_BUFFER_SIZE];
    unsigned char out_buf[MZ_EG_OUTPUT_BUFFER_SIZE];
    z_stream strm;

    /* ======= This initialization is not necessary for muzic ====== */
    /* === It's preserved merely for the compatibility with zlib === */
    {
        strm.avail_in = 0;
        strm.zalloc = Z_NULL;
        strm.zfree  = Z_NULL;
        strm.opaque = Z_NULL;
    }

    int defl_level = Z_DEFAULT_COMPRESSION;
    int defl_mode  = Z_FINISH;
    if(argc == 3){
        defl_level = argv[1][0] - '0';
        defl_mode  = argv[2][0] - '0';
    }

    int prev_state = deflateInit(&strm, defl_level);

    if(prev_state != Z_OK) {
        return prev_state;
    } else {
        prev_state = Z_STREAM_END;
    }

    do {
        /* All compressed data generated in the last iteration is consumed */
        /* The Z_OK predicate is preserved for Zlib only. muzic deflate    */
        /* returns either Z_STREAM_END or Z_BUF_ERROR                      */
        if(prev_state == Z_OK || prev_state == Z_STREAM_END){
            in_bytes = read(STDIN_FILENO, in_buf, sizeof(in_buf));
            if(in_bytes == 0) break;
            if(in_bytes == -1) return -1;
            strm.next_in   = in_buf;
            strm.avail_in  = in_bytes;
        }
        
        strm.next_out  = out_buf;
        strm.avail_out = sizeof(out_buf);

        prev_state = deflate(&strm, defl_mode);
        /* printf("total_out: %d\n ", strm.total_out); */

        #ifdef MZ_EG_HEXOUT
        unsigned char* idx;
        for(idx = out_buf; idx != strm.next_out; ++idx){
            printf("%02X ", *idx);
        }
        #else
        fwrite(out_buf, sizeof(char), strm.next_out - out_buf, stdout);
        #endif
        fflush(stdout);
        // fprintf(stderr, "\n");
    } while(in_bytes > 0);

    deflateEnd(&strm);

    return Z_OK;
}
