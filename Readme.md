# Small Deflate
This repository contains a restructured version of `sdefl` library in [vurtun's mmx libraries collection](https://github.com/vurtun/lib). The copyright and license are the same as the original library.

`sdefl` is a small bare bone lossless compression library in ANSI C (ISO C90)
which implements the ZLIB (RFC 1950) and Deflate (RFC 1951) compressed data format specification standard.
It is mainly tuned to get as much speed and compression ratio from as little code
as needed to keep the implementation as concise as possible.

## Highlights
- Portable single header and source file duo written in ANSI C (ISO C90)
- Dual license with either MIT or public domain
- Small implementation
- Webassembly:
    - Deflate ~3.7 KB (~2.2KB compressed)
    - Inflate ~3.6 KB (~2.2KB compressed)

## Usage:


### Benchmark

| Compressor name         | Compression| Decompress.| Compr. size | Ratio |
| ------------------------| -----------| -----------| ----------- | ----- |
| miniz 1.0 -1            |   122 MB/s |   208 MB/s |    48510028 | 48.51 |
| miniz 1.0 -6            |    27 MB/s |   260 MB/s |    36513697 | 36.51 |
| miniz 1.0 -9            |    23 MB/s |   261 MB/s |    36460101 | 36.46 |
| zlib 1.2.11 -1          |    72 MB/s |   307 MB/s |    42298774 | 42.30 |
| zlib 1.2.11 -6          |    24 MB/s |   313 MB/s |    36548921 | 36.55 |
| zlib 1.2.11 -9          |    20 MB/s |   314 MB/s |    36475792 | 36.48 |
| sdefl 1.0 -0            |   127 MB/s |   355 MB/s |    40004116 | 39.88 |
| sdefl 1.0 -1            |   111 MB/s |   413 MB/s |    38940674 | 38.82 |
| sdefl 1.0 -5            |    45 MB/s |   436 MB/s |    36577183 | 36.46 |
| sdefl 1.0 -7            |    38 MB/s |   432 MB/s |    36523781 | 36.41 |
| libdeflate 1.3 -1       |   147 MB/s |   667 MB/s |    39597378 | 39.60 |
| libdeflate 1.3 -6       |    69 MB/s |   689 MB/s |    36648318 | 36.65 |
| libdeflate 1.3 -9       |    13 MB/s |   672 MB/s |    35197141 | 35.20 |
| libdeflate 1.3 -12      |  8.13 MB/s |   670 MB/s |    35100568 | 35.10 |

### Compression
Results on the [Silesia compression corpus](http://sun.aei.polsl.pl/~sdeor/index.php?page=silesia):

| File    |   Original | `sdefl 0`    | `sdefl 5`  | `sdefl 7`   |
| --------| -----------| -------------| ---------- | ------------|
| dickens | 10.192.446 | 4,260,187    |  3,845,261 |   3,833,657 |
| mozilla | 51.220.480 | 20,774,706   | 19,607,009 |  19,565,867 |
| mr      |  9.970.564 | 3,860,531    |  3,673,460 |   3,665,627 |
| nci     | 33.553.445 | 4,030,283    |  3,094,526 |   3,006,075 |
| ooffice |  6.152.192 | 3,320,063    |  3,186,373 |   3,183,815 |
| osdb    | 10.085.684 | 3,919,646    |  3,649,510 |   3,649,477 |
| reymont |  6.627.202 | 2,263,378    |  1,857,588 |   1,827,237 |
| samba   | 21.606.400 | 6,121,797    |  5,462,670 |   5,450,762 |
| sao     |  7.251.944 | 5,612,421    |  5,485,380 |   5,481,765 |
| webster | 41.458.703 | 13,972,648   | 12,059,432 |  11,991,421 |
| xml     |  5.345.280 | 886,620      |    674,009 |     662,141 |
| x-ray   |  8.474.240 | 6,304,655    |  6,244,779 |   6,244,779 |
