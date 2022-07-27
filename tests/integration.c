/*
 * Copyright 2022 SdtElectronics
 *
 * SPDX-License-Identifier: Zlib
 */

#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zlib_comp.h"

#define printf_dec_format(x) _Generic((x), \
    int: "%d", \
    unsigned int: "%u", \
    char*: "%p", \
    const char*: "%p", \
    unsigned char*: "%p", \
    const unsigned char*: "%p", \
    default: "%d" \
)

#define print(x) printf(printf_dec_format(x), x)
#define println(x) printf(printf_dec_format(x), x), printf("\n");

#define test_case(name) int test_failed = 0; printf("\n" #name "\n");
#define test_case_end if (test_failed) {printf("FAILED\n");} \
                                  else {printf("PASSED\n"); return test_failed; }
#define assert_equal(a, b) if(a != b) { printf("assertion failed at %s: %d ", __FILE__, __LINE__); \
                                        print(a); printf(" != "); println(b); test_failed = 1; }

const char sample[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
" eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
" quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo conse quat.  "
"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat"
" nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui "
"officia deserunt mollit anim id est laborum.";

z_stream* deflater;
z_stream* inflater;

void setup(){
    deflater = (z_stream*)malloc(sizeof(z_stream));
    assert(deflater != NULL);
    deflater->next_in  = Z_NULL;
    deflater->avail_in = 0;
    deflater->total_in = 0;
    deflater->total_out = 0;
    deflater->zalloc = Z_NULL;
    deflater->zfree  = Z_NULL;
    deflater->opaque = Z_NULL;
    deflateInit(deflater, Z_DEFAULT_COMPRESSION);

    inflater = (z_stream*)malloc(sizeof(z_stream));
    assert(inflater != NULL);
    inflater->next_in  = Z_NULL;
    inflater->avail_in = 0;
    inflater->total_in = 0;
    inflater->total_out = 0;
    inflater->zalloc = Z_NULL;
    inflater->zfree  = Z_NULL;
    inflater->opaque = Z_NULL;
    inflateInit(inflater);
}

void teardown(){
    deflateEnd(deflater);
    free(deflater);
    deflater = NULL;
    inflateEnd(inflater);
    free(inflater);
    inflater = NULL;
}

int test_deflate_finish(){
    test_case(deflate_finish);

    unsigned char defl_buf[1024];
    unsigned char infl_buf[sizeof(sample)];

    deflater->next_in   = (unsigned char*)sample;
    deflater->avail_in  = sizeof(sample);
    deflater->next_out  = defl_buf;
    deflater->avail_out = sizeof(defl_buf);

    int res = deflate(deflater, Z_FINISH);
    assert_equal(res, Z_STREAM_END);
    assert_equal(deflater->next_in , sample + sizeof(sample));
    assert_equal(deflater->avail_in, 0);
    assert_equal(deflater->next_out + deflater->avail_out, defl_buf + sizeof(defl_buf));
    assert_equal(deflater->total_out, deflater->next_out - defl_buf);
    assert_equal(deflater->total_in, sizeof(sample));

    inflater->next_in   = defl_buf;
    inflater->avail_in  = deflater->next_out - defl_buf;
    inflater->next_out  = infl_buf;
    inflater->avail_out = sizeof(infl_buf);
    res = inflate(inflater, Z_SYNC_FLUSH);
    assert_equal(res, Z_STREAM_END);
    assert_equal(inflater->next_in  , deflater->next_out);
    assert_equal(inflater->avail_in , 0);
    assert_equal(inflater->next_out , infl_buf + sizeof(sample));
    assert_equal(inflater->avail_out, 0);
    assert_equal(inflater->total_out, sizeof(sample));
    assert_equal(inflater->total_in, deflater->total_out);

    assert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0);

    test_case_end;
}

int test_deflate_full_flush(){
    test_case(deflate_full_flush);

    unsigned char defl_buf[1024];
    unsigned char infl_buf[sizeof(sample)];

    deflater->next_in   = (unsigned char*)sample;
    deflater->avail_in  = sizeof(sample);
    deflater->next_out  = defl_buf;
    deflater->avail_out = sizeof(defl_buf);

    int res = deflate(deflater, Z_FULL_FLUSH);
    assert_equal(res, Z_OK);
    assert_equal(deflater->next_in , sample + sizeof(sample));
    assert_equal(deflater->avail_in, 0);
    assert_equal(deflater->next_out + deflater->avail_out, defl_buf + sizeof(defl_buf));
    assert_equal(deflater->total_out, deflater->next_out - defl_buf);
    assert_equal(deflater->total_in, sizeof(sample));

    inflater->next_in   = defl_buf;
    inflater->avail_in  = deflater->next_out - defl_buf;
    inflater->next_out  = infl_buf;
    inflater->avail_out = sizeof(infl_buf);
    res = inflate(inflater, Z_SYNC_FLUSH);
    assert_equal(res, Z_OK);
    assert_equal(inflater->next_in  , deflater->next_out);
    assert_equal(inflater->avail_in , 0);
    assert_equal(inflater->next_out , infl_buf + sizeof(sample));
    assert_equal(inflater->avail_out, 0);
    assert_equal(inflater->total_out, sizeof(sample));
    assert_equal(inflater->total_in, deflater->total_out);

    assert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0);

    test_case_end;
}

int test_deflate_full_flush_frag_in(){
    test_case(deflate_full_flush_frag_in);
    unsigned char defl_buf[4096];
    unsigned char infl_buf[sizeof(sample)];

    deflater->next_in   = (unsigned char*)sample;
    deflater->next_out  = defl_buf;
    deflater->avail_out = sizeof(defl_buf);

    unsigned int len = sizeof(sample);
    const unsigned int step = 4;
    while(len > step){
        deflater->avail_in = step;
        int res = deflate(deflater, Z_FULL_FLUSH);
        assert_equal(res, Z_OK);
        len -= step;
    }

    if (len != 0){
        deflater->avail_in = len;
        int res = deflate(deflater, Z_FULL_FLUSH);
        assert_equal(res, Z_OK);
    }
    assert_equal(deflater->next_in , sample + sizeof(sample));
    assert_equal(deflater->avail_in, 0);
    assert_equal(deflater->next_out + deflater->avail_out, defl_buf + sizeof(defl_buf));
    assert_equal(deflater->total_out, deflater->next_out - defl_buf);
    assert_equal(deflater->total_in, sizeof(sample));
    
    inflater->next_in   = defl_buf;
    inflater->avail_in  = deflater->next_out - defl_buf;
    inflater->next_out  = infl_buf;
    inflater->avail_out = sizeof(infl_buf);
    int res = inflate(inflater, Z_SYNC_FLUSH);
    assert_equal(res, Z_OK);
    assert_equal(inflater->next_in  , deflater->next_out);
    assert_equal(inflater->avail_in , 0);
    assert_equal(inflater->next_out , infl_buf + sizeof(sample));
    assert_equal(inflater->avail_out, 0);
    assert_equal(inflater->total_out, sizeof(sample));
    assert_equal(inflater->total_in, deflater->total_out);

    assert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0);

    test_case_end;
}

int test_deflate_finish_frag_out(){
    test_case(deflate_finish_frag_out);

    unsigned char defl_buf[4];

    deflater->next_in   = (unsigned char*)sample;
    deflater->avail_in  = sizeof(sample);
    deflater->next_out  = defl_buf;
    deflater->avail_out = sizeof(defl_buf);
        
    int res = deflate(deflater, Z_FINISH);
    assert_equal(res, Z_BUF_ERROR);

    test_case_end;
}

int test_deflate_full_flush_frag_out(){
    test_case(deflate_full_flush_frag_out);

    unsigned char defl_buf[4096];
    unsigned char infl_buf[sizeof(sample)];

    deflater->next_in   = (unsigned char*)sample;
    deflater->avail_in  = sizeof(sample);
    deflater->next_out  = defl_buf;
    deflater->avail_out = 0;

    const unsigned int step = 4;

    while(deflater->avail_out == 0){
        deflater->avail_out = step;
        int res = deflate(deflater, Z_FULL_FLUSH);
        assert_equal(res, Z_OK);
    }

    assert_equal(deflater->next_in , sample + sizeof(sample));
    assert_equal(deflater->avail_in, 0);
    assert_equal(deflater->total_out, deflater->next_out - defl_buf);
    assert_equal(deflater->total_in, sizeof(sample));
    
    inflater->next_in   = defl_buf;
    inflater->avail_in  = deflater->next_out - defl_buf;
    inflater->next_out  = infl_buf;
    inflater->avail_out = sizeof(infl_buf);
    int res = inflate(inflater, Z_SYNC_FLUSH);
    assert_equal(res, Z_OK);
    assert_equal(inflater->next_in  , deflater->next_out);
    assert_equal(inflater->avail_in , 0);
    assert_equal(inflater->next_out , infl_buf + sizeof(sample));
    assert_equal(inflater->avail_out, 0);
    assert_equal(inflater->total_out, sizeof(sample));
    assert_equal(inflater->total_in, deflater->total_out);

    assert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0);

    test_case_end;
}

int test_inflate_frag_in(){
    test_case(inflate_frag_in);

    unsigned char defl_buf[1024];
    unsigned char infl_buf[sizeof(sample) + 1];

    deflater->next_in   = (unsigned char*)sample;
    deflater->avail_in  = sizeof(sample);
    deflater->next_out  = defl_buf;
    deflater->avail_out = sizeof(defl_buf);

    int res = deflate(deflater, Z_FINISH);
    assert_equal(res, Z_STREAM_END);
    assert_equal(deflater->next_in , sample + sizeof(sample));
    assert_equal(deflater->avail_in, 0);
    assert_equal(deflater->next_out + deflater->avail_out, defl_buf + sizeof(defl_buf));
    assert_equal(deflater->total_out, deflater->next_out - defl_buf);
    assert_equal(deflater->total_in, sizeof(sample));

    inflater->next_in   = defl_buf;
    inflater->next_out  = infl_buf;
    inflater->avail_out = sizeof(infl_buf);

    const unsigned int step = 4;
    res = Z_OK;
    
    unsigned int rest = deflater->total_out;
    while(res == Z_OK){
        inflater->avail_in = step < rest ? step : rest;
        res = inflate(inflater, Z_SYNC_FLUSH);
        rest -= step;
    }

    assert_equal(res, Z_STREAM_END);
    assert_equal(inflater->next_in  , deflater->next_out);
    assert_equal(inflater->avail_in , 0);
    assert_equal(inflater->next_out , infl_buf + sizeof(sample));
    assert_equal(inflater->avail_out, 1);
    assert_equal(inflater->total_out, sizeof(sample));
    assert_equal(inflater->total_in, deflater->total_out);

    assert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0);

    test_case_end;
}

int test_inflate_frag_out(){
    test_case(inflate_frag_out);

    unsigned char defl_buf[1024];
    unsigned char infl_buf[sizeof(sample) + 1];

    deflater->next_in   = (unsigned char*)sample;
    deflater->avail_in  = sizeof(sample);
    deflater->next_out  = defl_buf;
    deflater->avail_out = sizeof(defl_buf);

    int res = deflate(deflater, Z_FINISH);
    assert_equal(res, Z_STREAM_END);
    assert_equal(deflater->next_in , sample + sizeof(sample));
    assert_equal(deflater->avail_in, 0);
    assert_equal(deflater->next_out + deflater->avail_out, defl_buf + sizeof(defl_buf));
    assert_equal(deflater->total_out, deflater->next_out - defl_buf);
    assert_equal(deflater->total_in, sizeof(sample));

    inflater->next_in   = defl_buf;
    inflater->avail_in  = deflater->next_out - defl_buf;
    inflater->next_out  = infl_buf;
    

    const unsigned int step = 4;
    res = Z_OK;
    
    unsigned int rest = sizeof(sample);
    while(res == Z_OK){
        inflater->avail_out = step < rest ? step : rest;
        res = inflate(inflater, Z_SYNC_FLUSH);
        rest -= step;
    }

    assert_equal(res, Z_STREAM_END);
    assert_equal(inflater->next_in  , deflater->next_out);
    assert_equal(inflater->avail_in , 0);
    assert_equal(inflater->next_out , infl_buf + sizeof(sample));
     /* assert_equal(inflater->avail_out, 1); */
    assert_equal(inflater->total_out, sizeof(sample));
    assert_equal(inflater->total_in, deflater->total_out);

    assert_equal(memcmp(infl_buf, sample, sizeof(sample)), 0);

    test_case_end;
}

/* Output large enough to make the internal buffer of inflate rewind */
int test_inflate_buf_rewind(){
    test_case(inflate_buf_rewind);

    unsigned int len = 65536;
    char largeInput[len];

    while(len-- > 0){
        largeInput[len] = '1' + (len & 0b111111);
    }

    unsigned char defl_buf[sizeof(largeInput)];
    unsigned char infl_buf[sizeof(largeInput)];

    deflater->next_in   = (unsigned char*)largeInput;
    deflater->avail_in  = sizeof(largeInput);
    deflater->next_out  = defl_buf;
    deflater->avail_out = sizeof(defl_buf);

    int res = deflate(deflater, Z_FINISH);

    inflater->next_in   = defl_buf;
    inflater->avail_in  = deflater->next_out - defl_buf;
    inflater->next_out  = infl_buf;
    inflater->avail_out = sizeof(infl_buf);
    res = inflate(inflater, Z_SYNC_FLUSH);
    assert_equal(res, Z_STREAM_END);
    assert_equal(inflater->next_in  , deflater->next_out);
    assert_equal(inflater->avail_in , 0);
    assert_equal(inflater->next_out , infl_buf + sizeof(largeInput));
    assert_equal(inflater->avail_out, 0);
    assert_equal(inflater->total_out, sizeof(largeInput));
    assert_equal(inflater->total_in, deflater->total_out);

    assert_equal(memcmp(infl_buf, largeInput, sizeof(largeInput)), 0);

    test_case_end;
}

int main(){
    int res = 0;
    setup(); res += test_deflate_finish(); teardown();
    setup(); res += test_deflate_full_flush(); teardown();
    setup(); res += test_deflate_full_flush_frag_in(); teardown();
    setup(); res += test_deflate_finish_frag_out(); teardown();
    setup(); res += test_deflate_full_flush_frag_out(); teardown();
    setup(); res += test_inflate_frag_in(); teardown();
    setup(); res += test_inflate_frag_out(); teardown();
    setup(); res += test_inflate_buf_rewind(); teardown();
    return res;
}
