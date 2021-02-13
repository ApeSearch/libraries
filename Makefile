CC=g++ -g3 -std=c++17 -Wall -pedantic -Wconversion -Wextra -Wreorder -fno-builtin

SOURCES=$(wildcard *.cpp)
OBJS=${SOURCES:.cpp=.o}

EXECDIR=tests/bin
TESTDIR=tests

all: string_test

# Compile the file server and tag this compilation
string_test: ${OBJS}
	${CC} -o $@ $^

# ld -r -o $@ ${OBJS}

# TEST_SRC:=$(basename $(wildcard ${TESTDIR}/*.cpp))
# $(TEST_SRC): %: %.cpp query
# 	${CC} -D LOCAL -o ${EXECDIR}/$(notdir $@) $^ 

test: ${TEST_SRC} 

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${OBJS} string_test
