#include "decompressor.hpp"
#include "file_buffers.hpp"
#include <cstdio>


void file_decompressor(FILE* input, FILE* output) {

    file_reader reader(input);
    lookupcompress::decompressor comp(reader);

    file_writer writer(output);

    try {
        comp.decode(writer);
    } catch (bitstream::reader::end_of_stream&) {
        // ok
    }
}


int main(int argc, char* argv[]) {

    FILE* input;
    FILE* output;

    output = fopen("decompressed.out", "wb");

    for (int i=1; i < argc; i++) {

        input = fopen(argv[i], "rb");
        if (input == nullptr) {
            std::fprintf(stderr, "can't open '%s'\n", argv[i]);
        }

        file_decompressor(input, output);
        fclose(input);
    }

    fclose(output);
}

