/*
 * Copyright 2022 SdtElectronics
 *
 * SPDX-License-Identifier: Zlib
 */

#include <stdio.h>
#include <unistd.h>

#include "zlib_comp.h"

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

    int prev_state = deflateInit(&strm, 1);

    if(prev_state != Z_OK) {
        return prev_state;
    } else {
        prev_state = Z_STREAM_END;
    }

    do {
        /* All compressed data generated in the last iteration is consumed */
        if(prev_state == Z_STREAM_END){
            in_bytes = read(STDIN_FILENO, in_buf, sizeof(in_buf));
            if(in_bytes == 0) break;
            if(in_bytes == -1) return -1;
            strm.next_in   = in_buf;
            strm.avail_in  = in_bytes;
        }
        
        strm.next_out  = out_buf;
        strm.avail_out = sizeof(out_buf);

        prev_state = deflate(&strm, Z_SYNC_FLUSH);
        /* printf("total_out: %d\n ", strm.total_out); */
        unsigned char* idx;
        for(idx = out_buf; idx != strm.next_out; ++idx){
            printf("%02X ", *idx);
        }
        printf("\n");
    } while(in_bytes > 0);

    deflateEnd(&strm);

    return Z_OK;
}
