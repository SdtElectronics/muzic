#ifndef SINFL_H_INCLUDED
#define SINFL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define SINFL_PRE_TBL_SIZE 128
#define SINFL_LIT_TBL_SIZE 1334
#define SINFL_OFF_TBL_SIZE 402

struct sinfl {
  const unsigned char *bitptr;
  unsigned long long bitbuf;
  int bitcnt;

  unsigned lits[SINFL_LIT_TBL_SIZE];
  unsigned dsts[SINFL_OFF_TBL_SIZE];
};
int sinflate(void *out, int cap, const void *in, int size);
int zsinflate(void *out, int cap, const void *in, int size);

#ifdef __cplusplus
}
#endif

#ifdef SDEFL_HEADER_ONLY
#define SDEFL_INLINE inline
#include "sinfl.c"
#else
#define SDEFL_INLINE 
#endif /* SDEFL_HEADER_ONLY */

#endif /* SINFL_H_INCLUDED */
