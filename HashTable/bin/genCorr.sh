#!/bin/bash

for test in tests/bin/*; do
    test_name=`echo $test | cut -d "/" -f3`
    test_output="corr-${test_name}"
    if [ -f ${test} ] ; then
        echo "Outputting $test to file..."
        ./${test} > tests/CorrOutput/${test_output}
    fi
done

cp tests/corrOutput/HashTable.correct.txt tests/corrOutput/corr-HashTable
cp tests/corrOutput/Top10.correct.txt tests/corrOutput/corr-Top10