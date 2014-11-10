#ifndef FILE_BUFFERS_HPP_INCLUDED__
#define FILE_BUFFERS_HPP_INCLUDED__

#include "bitstream.hpp"
#include <memory>
#include <cstdio>


class file_reader final: public bitstream::reader {

    static const std::size_t capacity = 1024 * 8;

    FILE* file;
    std::unique_ptr<uint8_t[]> buffer;
    std::size_t offset;
    std::size_t count;

public:
    file_reader(FILE* file)
        : file(file)
        , buffer(std::unique_ptr<uint8_t[]>(new uint8_t[capacity]))
        , offset(capacity)
        , count(capacity) {

        // nop
    }

protected:
    virtual uint8_t load_byte() override {
        if (offset == count) {
            count = fread(buffer.get(), 1, capacity, file);
            if (count == 0) {
                throw end_of_stream();
            }

            offset = 0;
        }

        return buffer[offset++];
    }
};


class file_writer final: public bitstream::writer {

    static const std::size_t capacity = 1024 * 8;

    FILE* file;
    std::unique_ptr<uint8_t[]> buffer;
    std::size_t offset;

public:
    file_writer(FILE* file)
        : file(file)
        , buffer(std::unique_ptr<uint8_t[]>(new uint8_t[capacity]))
        , offset(0) {

        // nop
    }

    ~file_writer() {
        if (offset > 0) {
            fwrite(buffer.get(), offset, 1, file);
        }
    }

protected:
    virtual void save_byte(uint8_t byte) override {
        if (offset == capacity) {
            fwrite(buffer.get(), capacity, 1, file);
            offset = 0;
        }

        buffer[offset++] = byte;
    }
};

#endif

