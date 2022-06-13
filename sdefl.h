#ifndef SDEFL_H_INCLUDED
#define SDEFL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define SDEFL_MAX_OFF   (1 << 15)
#define SDEFL_WIN_SIZ   SDEFL_MAX_OFF
#define SDEFL_WIN_MSK   (SDEFL_WIN_SIZ-1)

#define SDEFL_HASH_BITS 15
#define SDEFL_HASH_SIZ  (1 << SDEFL_HASH_BITS)
#define SDEFL_HASH_MSK  (SDEFL_HASH_SIZ-1)

#define SDEFL_MIN_MATCH 4
#define SDEFL_BLK_MAX   (256*1024)
#define SDEFL_SEQ_SIZ   ((SDEFL_BLK_MAX + SDEFL_MIN_MATCH)/SDEFL_MIN_MATCH)

#define SDEFL_SYM_MAX   (288)
#define SDEFL_OFF_MAX   (32)
#define SDEFL_PRE_MAX   (19)

#define SDEFL_LVL_MIN   0
#define SDEFL_LVL_DEF   5
#define SDEFL_LVL_MAX   8

struct sdefl_freq {
  unsigned lit[SDEFL_SYM_MAX];
  unsigned off[SDEFL_OFF_MAX];
};
struct sdefl_code_words {
  unsigned lit[SDEFL_SYM_MAX];
  unsigned off[SDEFL_OFF_MAX];
};
struct sdefl_lens {
  unsigned char lit[SDEFL_SYM_MAX];
  unsigned char off[SDEFL_OFF_MAX];
};
struct sdefl_codes {
  struct sdefl_code_words word;
  struct sdefl_lens len;
};
struct sdefl_seqt {
  int off, len;
};
struct sdefl {
  int bits, bitcnt;
  int tbl[SDEFL_HASH_SIZ];
  int prv[SDEFL_WIN_SIZ];

  int seq_cnt;
  struct sdefl_seqt seq[SDEFL_SEQ_SIZ];
  struct sdefl_freq freq;
  struct sdefl_codes cod;
};
int sdefl_bound(int in_len);
int sdeflate(struct sdefl *s, void *o, const void *i, int n, int lvl);
int zsdeflate(struct sdefl *s, void *o, const void *i, int n, int lvl);

#ifdef __cplusplus
}
#endif

#ifdef SDEFL_HEADER_ONLY
#define SDEFL_INLINE inline
#include "sdefl.c"
#else
#define SDEFL_INLINE 
#endif /* SDEFL_HEADER_ONLY */

#endif /* SDEFL_H_INCLUDED */
