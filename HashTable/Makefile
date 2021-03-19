CC=g++ -g3 -std=c++17 -Wall -pedantic -Wconversion -Wextra -Wreorder -fno-builtin

ifeq ($(shell uname -s | tr A-Z a-z), darwin)
	OS:=-DMACOS
endif
ifeq ($(shell uname -s | tr A-Z a-z), linux)
	OS:=-DLINUX
endif

SRC=src
SOURCES=$(wildcard ${SRC}/*.cpp)
OBJS=${SOURCES:.cpp=.o}

UTFWSRC=../unit_test_framework/src
UTFWSOURCES=$(wildcard ${UTFWSRC}/*.cpp)
UTFWOBJS=${UTFWSOURCES:.cpp=.o}

TESTDIR=tests
UNITTESTDIR=unitTests
EXECDIR=tests/bin
OUTPUT=tests/output
STDEXECDIR=tests/std_bin



all: test
TEST_SRC:=$(basename $(wildcard ${TESTDIR}/*.cpp))
$(TEST_SRC): %: %.cpp ${OBJS} ${UTFWOBJS}
	@mkdir -p ${EXECDIR}
	@mkdir -p ${STDEXECDIR}
	@mkdir -p ${OUTPUT}
	@mkdir -p tests/wrongOutput
	${CC} -Dtesting -DLOCAL ${OS} -o  ${EXECDIR}/$(notdir $@) $^ -pthread
	#${CC} -D STD -Dtesting ${LDFLAGS} -lz -o ${STDEXECDIR}/$(notdir $@) $^ -pthread


test: ${TEST_SRC}


run_test: test
	@echo
	@./bin/run_tests.sh


%.o: %.cpp
	${CC} -DLOCAL -c $< -o $@
%.o: %.cc
	${CC} -DLOCAL -c $


Top10: Top10.cpp Common.h Common.cpp TopN.h TopN.cpp HashTable.h 
	g++ Top10.cpp Common.cpp TopN.cpp -o Top10

HashTable: HashTable.cpp Common.h Common.cpp HashTable.h Timer.h
	g++ HashTable.cpp Common.cpp -o HashTable

HashBlob:  HashBlob.cpp HashTable.h HashBlob.h Common.h Common.cpp Timer.h
	g++ HashBlob.cpp Common.cpp -o HashBlob

HashFile:  HashFile.cpp HashTable.h HashBlob.h Common.h Timer.h
	g++ HashFile.cpp Common.cpp -o HashFile


#test: Top10 HashTable HashBlob HashFile BigJunkHtml.txt Top10.correct.txt HashTable.correct.txt
#	./Top10 BigJunkHtml.txt > Top10.testout.txt
#	diff Top10.correct.txt Top10.testout.txt
#	./HashTable BigJunkHtml.txt < BigJunkHtml.txt > HashTable.testout.txt
#	diff HashTable.correct.txt HashTable.testout.txt
#	./HashBlob BigJunkHtml.txt < BigJunkHtml.txt > HashBlob.testout.txt
#	diff HashTable.correct.txt HashBlob.testout.txt
#	./HashBlob BigJunkHtml.txt HashBlob.testout.bin < /dev/null
#	./HashFile HashBlob.testout.bin < BigJunkHtml.txt > HashFile.testout.txt
#	diff HashTable.correct.txt HashFile.testout.txt

clean:
	rm -rf Top10 HashTable HashBlob HashFile *.testout.* ${OBJS} ${EXECDIR} ${OUTPUT}

