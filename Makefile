COMPILER = gcc
CCFLAGS = -Wall

.PHONY: all clean rebuild mrproper

all: example ccir stdout-wav-test genwave.o
rebuild: mrproper all

mrproper: clean
	@echo "*** Cleaning all binary executables files..."
	@rm example

clean:
	@echo "*** Removing objects..."
	@rm *.o

# GENWAVE LIBRARY

genwave.o: genwave.h genwave.c 
	@echo "*** Compiling genwave.o"
	@${COMPILER} ${CCFLAGS} -c genwave.c -o genwave.o                                        

# EXAMPLE

example.o: genwave.h example.c 
	@echo "*** Compiling example.o"
	@${COMPILER} ${CCFLAGS} -c example.c -o example.o

example: genwave.o example.o 
	@echo "*** Linking example..."
	@gcc -lm genwave.o example.o -o example

# CCIR

ccir.o: genwave.h ccir.c 
	@echo "*** Compiling ccir.o"
	@${COMPILER} ${CCFLAGS} -c ccir.c -o ccir.o

ccir: genwave.o ccir.o 
	@echo "*** Linking ccir..."
	@gcc -lm genwave.o ccir.o -o ccir

# STDOUT-WAV-TEST

stdout-wav-test.o: genwave.h stdout-wav-test.c 
	@echo "*** Compiling stdout-wav-test.o"
	@${COMPILER} ${CCFLAGS} -c stdout-wav-test.c -o stdout-wav-test.o

stdout-wav-test: genwave.o stdout-wav-test.o
	@echo "*** Linking stdout-wav-test..."
	@gcc -lm genwave.o stdout-wav-test.o -o stdout-wav-test

