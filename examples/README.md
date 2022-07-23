# Examples

## defl2stdout
This example reads byte-stream from stdin and compresses it with DEFLATE. The result is serialized to stdout as binary form.
``` sh
echo "test string" | ./defl2stdout | xxd -p
# 78012b492d2e51282e29cacc4be702001f420482
```

## infl4stdin
This example reads deflated data in binary form and decompresses it with INFLATE. The result is printed to stdout.
``` sh
echo "78012b492d2e51282e29cacc4be702001f420482" | xxd -r -p | ./infl4stdin
# test string
```

## chain operation for testing
``` sh
echo "test string" | ./defl2stdout | ./infl4stdin
# test string
```
The output should be identical to the string passed to `echo`.
