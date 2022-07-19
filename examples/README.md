# Examples

## defl2stdout
This example reads byte-stream from stdin and compresses it with DEFLATE. The result is serialized to stdout as space-delimited bytes in hexadecimal representation.
``` sh
echo -n "test string" | ./defl2stdout
# 78 01 2B 49 2D 2E 51 28 2E 29 CA CC 4B 07 00
```

## infl4stdin
This example reads space-delimited byte-stream in hexadecimal representation and decompresses it with INFLATE. The result is printed to stdout.
``` sh
echo -n "78 01 2B 49 2D 2E 51 28 2E 29 CA CC 4B 07 00" | ./infl4stdin
# test string
```

## chain operation for testing
``` sh
echo -n "test string" | ./defl2stdout | ./infl4stdin
```
The output should be identical to the string passed to `echo`.
