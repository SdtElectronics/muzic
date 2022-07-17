/*
 * Copyright 2022 Young Mei
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>

#include <string.h>

#include "uzlib.h"
#include "zlib_comp.h"

int deflateInit(z_streamp strm, int level){
  if (strm == Z_NULL) return Z_STREAM_ERROR;
  strm->msg = Z_NULL;
  // uzlib allocates memory for output on demand
  // so no worry about outbuf; just remember to deallocate it after use
  struct uzlib_comp* ustate = strm->state.defl_state;
  ustate->dict_size = 32768;
  ustate->hash_bits = 12;
  size_t hash_size = sizeof(uzlib_hash_entry_t) * (1 << ustate->hash_bits);
  // this space is reused in every call to deflate() until deflateEnd()
  // 0-initialization is thereby put in deflate()
  ustate->hash_table = malloc(hash_size);
  return Z_OK;
}

int deflate(z_streamp strm, int flush){
  struct uzlib_comp* ustate = strm->state.defl_state;
  ustate->outbuf = NULL;
  ustate->outlen = 0;
  ustate->outsize = 0;
  ustate->outbits = 0;
  ustate->noutbits = 0;
  ustate->comp_disabled = 0;
  size_t hash_size = sizeof(uzlib_hash_entry_t) * (1 << ustate->hash_bits);
  memset(ustate->hash_table, 0, hash_size);

  zlib_start_block(ustate);
  uzlib_compress(ustate, strm->next_in, strm->avail_in);
  zlib_finish_block(ustate);
  
  strm->next_out = ustate->outbuf;
  // output buffer of z_stream is never used as uzlib allocates memory for output
  // To make output buffer flushed by consumer, always set avail_out to 0
  strm->avail_out = 0;
  // always deflate all available data
  // set avail_in to 0 and return Z_STREAM_END to signal that
  strm->avail_in = 0;
  return Z_STREAM_END;
}

int deflateEnd(z_streamp strm){
  free(strm->state.defl_state);
  return Z_OK;
}
