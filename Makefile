CC=g++ -g3 -std=c++11 -Wall -pedantic -Wconversion -Wextra -Wreorder -fno-builtin

SOURCES=$(wildcard *.cpp)
OBJS=${SOURCES:.cpp=.o}

all: test

TESTDIR=tests
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
	${CC} -o ${EXECDIR}/$(notdir $@) $^ 
	${CC} -D STD -o ${STDEXECDIR}/$(notdir $@) $^ 

test: ${TEST_SRC}

run_test: test
	@echo
	@./bin/run_tests.sh

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${OBJS} ${EXECDIR}/* ${STDEXECDIR}/* ${OUTPUT}/*