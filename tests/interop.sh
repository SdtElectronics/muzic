#!/bin/bash

assertIdentical(){
    if [[ $1 != $2 ]]; then
        printf "$1 \n and \n $2 mismatch \n"
        exit -1
    fi
}

sample="Lorem ipsum dolor sit amet, consectetur adipiscing elit,\
 sed do eiusmod tempor incididunt ut labore et dolore magna     \
 aliqua. Ut enim ad minim veniam, quis nostrud exercitation     \
 ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis  \
 aute irure dolor in reprehenderit in voluptate velit esse      \
 cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat\
 cupidatat non proident, sunt in culpa qui officia deserunt     \
 mollit anim id est laborum."

deflBins=(muzicDefl muzicDeflFragIn muzicDeflFragOut zlibDefl zlibDeflFragIn zlibDeflFragOut)
inflBins=(muzicInfl muzicInflFragIn muzicInflFragOut zlibInfl zlibInflFragIn)

for defl in ${deflBins[@]}; do
    for infl in ${inflBins[@]}; do
        if [[ ($defl == "zlib"* && $infl == "zlib"*) || ($defl == "muzic"* && $infl == "muzic"*) ]]
        then
            continue
        fi

        echo "Test case $defl to $infl"
        cmd="echo \"${sample}\" | ${TEST_BINARY_PATH}/${defl} 1 1 | ${TEST_BINARY_PATH}/${infl}"
        echo "$cmd"
        result=$(eval "$cmd")
        ret=$?
        if [[ $ret != 0 ]]; then
            echo "FAILED with $ret"
            exit $ret
        fi
        assertIdentical "$sample" "$result"
        printf "PASSED\n\n"
    done
done
