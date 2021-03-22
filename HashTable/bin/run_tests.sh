#!/bin/bash
exec=""
# Run Hash-table...
for test in tests/bin/*; do
    if [ -f ${test} ] ; then
        test_name=`echo $test | cut -d "/" -f3`
        corr_output="tests/corrOutput/corr-$test_name"
        if [[ ${test} = "tests/bin/HashTable" ]]; then
            exec="${test} ./tests/input/BigJunkHtml.txt < ./tests/input/BigJunkHtml.txt"
        elif [[ ${test} = "tests/bin/Top10" ]]; then
            exec="${test} ./tests/input/BigJunkHtml.txt"
        else
            exec="${test}"
        fi
        echo "Running $test..."
        echo "Execute: ./$exec"
        #if eval "$exec" | diff -y - "$corr_output"; then
        if eval "$exec" | diff -q - "$corr_output"; then
                echo "Test $test passed!"
            else
                echo "Test $test failed..."
                eval "$exec" > "tests/wrongOutput/wrong-$test_name"
        fi
        echo
    fi # end  if [ -f ${test} ]
done

#./HashBlob BigJunkHtml.txt < BigJunkHtml.txt > HashBlob.testout.txt
#diff HashTable.correct.txt HashBlob.testout.txt
#./HashBlob BigJunkHtml.txt HashBlob.testout.bin < /dev/null
#./HashFile HashBlob.testout.bin < BigJunkHtml.txt > HashFile.testout.txt
#diff HashTable.correct.txt HashFile.testout.txt