/*
 * Copyright 2022 Young Mei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "uzlib.h"
#include "zlib_comp.h"

void outbits(struct uzlib_comp *out, unsigned long bits, int nbits);

int deflateInit(z_stream* strm, int level){
    if (strm == Z_NULL) return Z_STREAM_ERROR;
    strm->msg = Z_NULL;
    struct uzlib_comp* ustate = (struct uzlib_comp*)malloc(sizeof(struct uzlib_comp));
    if (ustate == Z_NULL) return Z_MEM_ERROR;
    /* uzlib allocates memory for output on demand                            */
    /* so no worry about outbuf; just remember to deallocate it after use     */
    ustate->dict_size = 32768;
    ustate->hash_bits = 12;
    size_t hash_size = sizeof(uzlib_hash_entry_t) * (1 << ustate->hash_bits);
    /* this space is reused in every call to deflate() until deflateEnd()     */
    /* 0-initialization is thereby put in deflate()                           */
    ustate->hash_table = malloc(hash_size);
    ustate->outbuf = NULL;
    strm->state.defl_state = ustate;
    return Z_OK;
}

int deflate(z_stream* strm, int flush){
    struct uzlib_comp* ustate = strm->state.defl_state;

    /* We still have some pending data in the output buffer */
    if(ustate->outbuf != NULL){
        if(strm->avail_out < ustate->outlen){
            memcpy(strm->next_out, ustate->outbuf, strm->avail_out);
            ustate->outbuf  += strm->avail_out;
            ustate->outlen  -= strm->avail_out;
            strm->next_out  += strm->avail_out;
            strm->total_out += strm->avail_out;
            strm->avail_out = 0;
            return Z_BUF_ERROR;
        }

        memcpy(strm->next_out, ustate->outbuf, ustate->outlen);

        ustate->outbuf -= strm->total_out;

        strm->avail_out -= ustate->outlen;
        strm->next_out  += ustate->outlen;
        strm->total_out += ustate->outlen;

        free((void*)ustate->outbuf);
        ustate->outbuf = NULL;
        if(strm->avail_in == 0)return Z_STREAM_END;
    }

    ustate->outlen   = 0;
    ustate->outsize  = 0;
    ustate->outbits  = 0;
    ustate->noutbits = 0;
    ustate->comp_disabled = 0;
    size_t hash_size = sizeof(uzlib_hash_entry_t) * (1 << ustate->hash_bits);
    memset(ustate->hash_table, 0, hash_size);
    
    #ifdef InfZlibStrm
    outbits(ustate, 0x78, 8);
    outbits(ustate, 0x01, 8);
    #endif

    zlib_start_block(ustate);
    uzlib_compress(ustate, strm->next_in, strm->avail_in);
    zlib_finish_block(ustate);
    
    strm->total_in = strm->avail_in;
    /* always deflate all available data                        */
    /* set avail_in to 0 and return Z_STREAM_END to signal that */
    strm->avail_in = 0;

    /* if output buffer doesn't have enough space               */
    if(strm->avail_out < ustate->outlen){
        memcpy(strm->next_out, ustate->outbuf, strm->avail_out);
        ustate->outbuf += strm->avail_out;
        ustate->outlen -= strm->avail_out;
        strm->next_out += strm->avail_out;
        strm->total_out = strm->avail_out;
        strm->avail_out = 0;
        return Z_BUF_ERROR;
    }

    memcpy(strm->next_out, ustate->outbuf, ustate->outlen);
    
    strm->total_out  = ustate->outlen;
    strm->avail_out -= ustate->outlen;
    strm->next_out  += ustate->outlen;

    free((void*)ustate->outbuf);
    ustate->outbuf = NULL;
    return Z_STREAM_END;
}

int deflateEnd(z_stream* strm){
    free(strm->state.defl_state);
    return Z_OK;
}
