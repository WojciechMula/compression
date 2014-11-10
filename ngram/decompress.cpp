#include "file_buffers.hpp"
#include "decompressor.hpp"


void decompress(FILE* input, FILE* output) {

    ngramcompression::decompressor comp;

    file_reader reader(input);
    file_writer writer(output);

    try {
        comp.decompress(reader, writer);
    } catch (stop&) {
        // ok
    }
}


int main(int argc, char* argv[]) {

    FILE* output = fopen("d.out", "wb");

    for (int i=1; i < argc; i++) {

        FILE* input = fopen(argv[i], "rb");
        if (input == nullptr) {
            std::fprintf(stderr, "can't open '%s'\n", argv[i]);
        } else {
            puts(argv[i]);
            decompress(input, output);

            fclose(input);
        }
    }

    fclose(output);
}


