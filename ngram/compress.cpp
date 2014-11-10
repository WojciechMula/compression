#include "file_buffers.hpp"
#include "compressor.hpp"


std::size_t file_size(FILE* file) {

    assert(file != nullptr);

    const auto current = ftell(file);

    fseek(file, 0, SEEK_END);

    const auto size = ftell(file);

    fseek(file, current, SEEK_SET);

    return size;
}


void compress(FILE* input, FILE* output, std::size_t bits) {

    const auto size = file_size(input);
    if (size == 0) {
        return;
    }

    auto buffer = std::unique_ptr<char[]>(new char[size]);

    fread(buffer.get(), size, 1, input);

    ngramcompression::compressor comp(bits);
    file_writer writer(output);

    comp.compress(writer, buffer.get(), size);
}


int main(int argc, char* argv[]) {

    FILE* output = fopen("c.out", "wb");

    for (int i=1; i < argc; i++) {

        FILE* input = fopen(argv[i], "rb");
        if (input == nullptr) {
            std::fprintf(stderr, "can't open '%s'\n", argv[i]);
        } else {
            puts(argv[i]);
            compress(input, output, 12);

            fclose(input);
        }
    }

    fclose(output);
}

