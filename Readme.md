# muzic
This library encapsulates a DEFLATE and INFLATE engine to provide a set of compression APIs partially compatible with Zlib. Particularly, it supports Zlib-style streaming operation for both compression and decompression. The objectives of this library are simplicity and lightweight, rather than performance and comprehensiveness, so that it can be integrated on embedded platforms.

## Supported Zlib APIs
Please also refer to the [Zlib Manual](https://www.zlib.net/manual.html) for the complete description of APIs. Note that behaviors of functions in this library are not strictly identical to their Zlib counterparts, and differences are stated below.

### DEFLATE
* `int deflateInit(z_stream* strm, int level)`

    Initializes the internal stream state for compression. Currently, parameter `level` is not used, but preserved for compatibility with Zlib.

* `int deflate(z_stream* strm, int flush)`

    `deflate` compresses as much data as possible, and stops when the input buffer becomes empty or the output buffer becomes full. Currently, parameter `flush` is not used, but preserved for compatibility with Zlib. The behavior of this function is always similar to the Zlib counterpart being fed with the option `Z_FINISH`.

* `int deflateEnd(z_stream* strm)`

    All dynamically allocated data structures for this stream are freed. This function discards any unprocessed input and does not flush any pending output.

### INFLATE
* `int inflateInit(z_stream* strm)`

    Initializes the internal stream state for decompression.

* `int inflate(z_stream* strm, int flush)`

    `inflate` decompresses as much data as possible, and stops when the input buffer becomes empty or the output buffer becomes full. Currently, parameter `flush` is not used, but preserved for compatibility with Zlib. The behavior of this function is always similar to the Zlib counterpart being fed with the option `Z_SYNC_FLUSH`. When macro `MZ_ZLIB_CHECKSUM` is not defined, `inflate` returns `Z_STREAM_END` immediately after the last block is consumed. When macro `MZ_ZLIB_CHECKSUM` is defined, it will further consume a 4-byte adler32 checksum after the last block, and if this block is not presented, it returns `Z_OK` instead of `Z_STREAM_END`. This behavior is similar to the zlib counterpart when `inflateInit()` is called to initialize the stream. See section [Zlib stream checksum]() for more information.

* `int inflateEnd(z_stream* strm)`

    All dynamically allocated data structures for this stream are freed. This function discards any unprocessed input and does not flush any pending output.

### Structures and macros
`z_stream` is the only structure that Zlib exposed to users to interact with DEFLATE and INFLATE APIs. All fields are replicated with their original names, while only the first 7 fields are used, and the rest are dumb variables reserved for compatibility.

Macros representing states and flush modes are preserved. They can be found in `zlib_comp.h`.

## Build and integration
The structure of this library is simple and flat, and no dependency other than libc is required. To use DEFLATE APIs, include `zlib_comp.h` and add `defl_static.c`, `genlz77.c` and `zlib_comp.c` to sources. To use INFLATE APIs, include `zlib_comp.h` and add `inflater.c` to sources.

CMake is also supported. To use this library as a subproject, simply add `muzic` to `target_link_libraries` of targets depend on this library.

## Notes
### Zlib stream header
Zlib pads raw deflate stream with a 2-byte header conforming [RFC-1950](https://www.rfc-editor.org/rfc/rfc1950) by default. To emulate this behavior, the macro `MZ_ZLIB_HEADER` must be enabled in `mz_config.h` (it's enabled by default). Only when `inflateInit2()` and `deflateInit2()` are called with `windowBits` set to negative values, Zlib will generate and accept raw deflate stream without the header, and `MZ_ZLIB_HEADER` can be disabled. Otherwise, Zlib will signal an error when decompressing the stream produced by muzic, and muzic won't be able to decompress stream produced by Zlib if `MZ_ZLIB_HEADER` is disabled.

### Zlib stream checksum
Zlib ends raw deflate stream with a 4-byte checksum conforming [RFC-1950](https://www.rfc-editor.org/rfc/rfc1950) by default. To emulate this behavior, the macro `MZ_ZLIB_CHECKSUM` must be enabled in `mz_config.h` (it's enabled by default).

## Comparison with Zlib
### Binary size
Tested on Linux x86_64 with gcc 11.1.0, CMake option `CMAKE_BUILD_TYPE=Release`
| Binary               | muzic           | Zlib              |
|----------------------|-----------------|-------------------|
| Static library       | 23664B (23.1KB) | 125522B (122.6KB) |
| examples/defl2stdout | 21152B (20.7KB) | 56112B (54.8KB)   |
| examples/infl4stdin  | 25456B (24.9KB) | 47184B (46.1KB)   |


### Memory usage
Tested on Linux x86_64 with gcc 11.1.0, CMake option `CMAKE_BUILD_TYPE=Release`

Profiler: valgrind-3.18.1 with option `--tool=massif --stacks=yes`

| Binary               | muzic (stack/heap/total) | Zlib (stack/heap/total)   |
|----------------------|--------------------------|---------------------------|
| examples/defl2stdout | 3.6KB / 33.1KB / 36.7KB  | 3.6KB / 262.9KB / 266.5KB |
| examples/infl4stdin  | 3.3KB / 22.0KB / 25.3KB  | 3.3KB / 40.0KB / 43.3KB   |

## TODO:
* Support `Z_BLOCK` mode for `inflate()`
* Benchmarks

## Credits
### DEFLATE engine: part of [uzlib](https://github.com/pfalcon/uzlib)
(c) 2014-2020 Paul Sokolovsky

### INFLATE engine: [Inflater](https://github.com/martin-rizzo/Inflater)
Copyright (c) 2020 Martin Rizzo
