static struct sdefl sdefl;
void *comp = 0;
size_t size = 0;
struct timespec compr_started;
struct timespec compr_ended;
struct timespec decompr_started;
struct timespec decompr_ended;
double compr_mbs = 0;
double decompr_mbs = 0;
void *decomp;
double ratio = 0.0f;
int len = 0, n, lvl;
void *data_ = 0;

FILE* fp = fopen(argv[1], "rb");
if (!fp) exit(2);
fseek(fp, 0, SEEK_END);
size_ = (size_t)ftell(fp);
fseek(fp, 0, SEEK_SET);
data_ = calloc(size_, 1);
fread(data_, 1, (size_t)size_, fp);
fclose(fp);


comp = calloc(size_*2, 1);
decomp = calloc(size_, 1);

#define SDEFL_HEADER_ONLY
#include "sinfl.h"
#include "sdefl.h"
FILE* fp = fopen("/mnt/e/work_CS/CXX/muzic/sinfl.c", "rb");