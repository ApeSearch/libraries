CC=g++ -g3 -std=c++17 -Wall -pedantic -Wconversion -Wextra -Wreorder -fno-builtin

#LDFLAGS:=-I/usr/local/opt/openssl/include/ -L/usr/local/opt/openssl/lib -lssl -lcrypto

## Define certain variables based on system
# -I likely means (Include) -L likely means (Library)
ifeq ($(shell uname -s | tr A-Z a-z), darwin)
	FLAGS:=MACOS
endif
ifeq ($(shell uname -s | tr A-Z a-z), linux)
	FLAGS:=LINUX
endif


SRC=src
SOURCES=$(wildcard ${SRC}/*.cpp)
OBJS=${SOURCES:.cpp=.o}

all: test

TESTDIR=tests
UNITTESTDIR=unitTests
EXECDIR=tests/bin
OUTPUT=tests/output
STDEXECDIR=tests/std_bin

# Rule for compiling all files found in tests/ into executables using apesearch::string first then std::string
# (I compiled them separately for testing)
# creates output directories if non-existent
TEST_SRC:=$(basename $(wildcard ${TESTDIR}/*.cpp))
$(TEST_SRC): %: %.cpp ${OBJS}
	@mkdir -p ${EXECDIR}
	@mkdir -p ${STDEXECDIR}
	@mkdir -p ${OUTPUT}
	${CC} -Dtesting -D${FLAGS} -o  ${EXECDIR}/$(notdir $@) $^ -pthread
	${CC} -D STD -Dtesting -D${FLAGS} ${LDFLAGS} -lz -o ${STDEXECDIR}/$(notdir $@) $^ -pthread

test: ${TEST_SRC}
	make unit_test

${UNITTESTDIR}/unit_test_framework.o:
	${CC} -c ${UNITTESTDIR}/dependencies/unit_test_framework.cpp -o ${UNITTESTDIR}/$(notdir $@)

UNIT_TEST_SRC:=$(basename $(wildcard ${UNITTESTDIR}/*.cpp))
$(UNIT_TEST_SRC): %: %.cpp ${UNITTESTDIR}/unit_test_framework.o ${OBJS}
	${CC} -D${FLAGS} -o ${EXECDIR}/$(notdir $@) $^ -pthread
	${CC} -D ${FLAGS} -DSTD -o ${STDEXECDIR}/$(notdir $@) $^ -pthread

unit_test: ${UNIT_TEST_SRC}


run_test: test
	@echo
	@./bin/run_tests.sh

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $< -o $@
%.o: %.cc
	${CC} -c $<

clean:
	rm -rf ${OBJS} ${EXECDIR}/* ${STDEXECDIR}/* ${OUTPUT}/* ${UNITTESTDIR}/unit_test_framework.o ${STDEXECDIR}/*