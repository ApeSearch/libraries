#!/bin/bash

# Run tests using our implementation of string and std::string, then compare the outputs 
# Assuming the tests are valid, std::string should always succeed while the diff will catch any
# errors in our implementation
for test in tests/bin/*; do
    if [ -f ${test} ] ; then
        echo "Running $test..."
        out="$(basename -- $test)"
        ./${test} > tests/output/${out}_out.txt 
        echo
    fi
done

for test in tests/std_bin/*; do
    if [ -f ${test} ] ; then
        echo "Running $test..."
        out="$(basename -- $test)"
        ./${test} > tests/output/${out}_std_out.txt 
        echo
    fi
done

for test in tests/bin/*; do
    if [ -f ${test} ] ; then
        fname="$(basename -- $test)"
        diff tests/output/${fname}_out.txt tests/output/${fname}_std_out.txt 
        echo
    fi
done