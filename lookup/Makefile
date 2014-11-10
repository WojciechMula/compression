CC=g++
FLAGS=-std=c++11 -Wall -Wpedantic -Werror -O2
INCLUDE=-I../bitstream
DEPS=common.hpp ../bitstream/bitstream.hpp ../bitstream/file_buffers.hpp

.SUFFIXES:

all: compress decompress

compress: compress.cpp compressor.hpp $(DEPS)
	$(CC) $(FLAGS) $(INCLUDE) compress.cpp -o compress

decompress: decompress.cpp decompressor.hpp $(DEPS)
	$(CC) $(FLAGS) $(INCLUDE) decompress.cpp -o decompress

clean:
	rm -f compress
	rm -f decompress
	rm -f *.out
