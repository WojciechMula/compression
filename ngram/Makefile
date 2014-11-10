.SUFFIXES:

CC=g++
FLAGS=-std=c++11 -Wall -Wextra -Wpedantic -Werror

DEPS=common.hpp bitstream.hpp file_buffers.hpp

all: compress decompress

compress: compress.cpp compressor.hpp $(DEPS)
	$(CC) $(FLAGS) -O2 compress.cpp -o compress

decompress: decompress.cpp decompressor.hpp $(DEPS)
	$(CC) $(FLAGS) -O2 decompress.cpp -o decompress

clean:
	rm -f compress
	rm -f decompress
	rm -f *.out
