#include "compressor.hpp"
#include "file_buffers.hpp"
#include <cstdio>


void file_compressor(FILE* input, FILE* output, std::size_t block_size) {

    file_writer writer(output);
    lookupcompress::compressor comp(writer);

    auto buffer = std::unique_ptr<char[]>(new char[block_size]);

    while (!std::feof(input)) {
        const auto readed = fread(buffer.get(), 1, block_size, input);
        comp.save(buffer.get(), readed);
    }
}


int main(int argc, char* argv[]) {

    FILE* input;
    FILE* output;

    output = fopen("compressed.out", "wb");

    for (int i=1; i < argc; i++) {

        input = fopen(argv[i], "rb");
        if (input == nullptr) {
            std::fprintf(stderr, "can't open '%s'\n", argv[i]);
        }

        file_compressor(input, output, 1024);
        fclose(input);
    }

    fclose(output);
}

