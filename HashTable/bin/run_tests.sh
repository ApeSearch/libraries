
./tests/bin/Top10 BigJunkHtml.txt > Top10.testout.txt
diff Top10.correct.txt Top10.testout.txt
./HashTable BigJunkHtml.txt < BigJunkHtml.txt > HashTable.testout.txt
diff HashTable.correct.txt HashTable.testout.txt
./HashBlob BigJunkHtml.txt < BigJunkHtml.txt > HashBlob.testout.txt
diff HashTable.correct.txt HashBlob.testout.txt
./HashBlob BigJunkHtml.txt HashBlob.testout.bin < /dev/null
./HashFile HashBlob.testout.bin < BigJunkHtml.txt > HashFile.testout.txt
diff HashTable.correct.txt HashFile.testout.txt