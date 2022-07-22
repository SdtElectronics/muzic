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

/*
 * Adler-32 algorithm taken from the zlib source, which is
 * Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler
 */

#define A32_BASE 65521
#define A32_NMAX 5552

uint32_t muzic_adler32(const void *data, unsigned int length, uint32_t prev_sum)
{
   const unsigned char *buf = (const unsigned char *)data;

   unsigned int s1 = prev_sum & 0xffff;
   unsigned int s2 = prev_sum >> 16;

   while (length > 0)
   {
      int k = length < A32_NMAX ? length : A32_NMAX;
      int i;

      for (i = k / 16; i; --i, buf += 16)
      {
         s1 += buf[0];  s2 += s1; s1 += buf[1];  s2 += s1;
         s1 += buf[2];  s2 += s1; s1 += buf[3];  s2 += s1;
         s1 += buf[4];  s2 += s1; s1 += buf[5];  s2 += s1;
         s1 += buf[6];  s2 += s1; s1 += buf[7];  s2 += s1;

         s1 += buf[8];  s2 += s1; s1 += buf[9];  s2 += s1;
         s1 += buf[10]; s2 += s1; s1 += buf[11]; s2 += s1;
         s1 += buf[12]; s2 += s1; s1 += buf[13]; s2 += s1;
         s1 += buf[14]; s2 += s1; s1 += buf[15]; s2 += s1;
      }

      for (i = k % 16; i; --i) { s1 += *buf++; s2 += s1; }

      s1 %= A32_BASE;
      s2 %= A32_BASE;

      length -= k;
   }

   return ((uint32_t)s2 << 16) | s1;
}

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
    
    #ifdef MZ_ZLIB_HEADER
    outbits(ustate, 0x78, 8);
    outbits(ustate, 0x01, 8);
    #endif

    #ifdef MZ_ZLIB_CHECKSUM
    uint32_t adler_v = muzic_adler32(strm->next_in, strm->avail_in, 1);
    #endif
    zlib_start_block(ustate);
    uzlib_compress(ustate, strm->next_in, strm->avail_in);
    zlib_finish_block(ustate);

    #ifdef MZ_ZLIB_CHECKSUM
    int newlen = ustate->outlen + 4;
    if (newlen > ustate->outsize) {
        ustate->outsize = newlen;
        ustate->outbuf = (unsigned char*)realloc(ustate->outbuf, newlen);
    }
    ustate->outbuf[ustate->outlen++] = (adler_v >> 24) & 0xFF;
    ustate->outbuf[ustate->outlen++] = (adler_v >> 16) & 0xFF;
    ustate->outbuf[ustate->outlen++] = (adler_v >>  8) & 0xFF;
    ustate->outbuf[ustate->outlen++] = adler_v         & 0xFF;
    #endif

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
