#ifndef BITSTREAM_HPP_INCLUDED__
#define BITSTREAM_HPP_INCLUDED__

#include <cstdint>
#include <cstdio>

namespace bitstream {

    class writer {
    protected:
        virtual void save_byte(uint8_t) = 0;

    private:
        std::size_t bit_offset;
        uint8_t buffer;

        static const std::size_t bits_count = 8;
    
    public:
        writer()
            : bit_offset(0)
            , buffer(0) {}

        void emit_bit(bool valu);
        void emit8(uint8_t value);
        void emit16(uint16_t value);
        void emit24(uint32_t value);
        void emit32(uint32_t value);
        void emit(uint32_t value, std::size_t bits);

        void finish();

    private:
        void flush();
    };


    class reader {
    
        std::size_t offset;
        uint8_t     byte;

    protected:
        virtual uint8_t load_byte() = 0;

    public:
        class end_of_stream {};

    public:
        reader() : offset(8) {}

        bool     bit();
        uint8_t  get8();
        uint16_t get16();
        uint32_t get24();
        uint32_t get32();
        uint32_t get(std::size_t k);
    };


    // --------------------------------------------------


    void writer::emit_bit(bool value) {
        if (value) {
            buffer |= (1u << bit_offset);
        }

        bit_offset += 1;
        if (bit_offset == bits_count) {
            save_byte(buffer);
            buffer = 0;
            bit_offset = 0;
        }
    }

    void writer::emit8(uint8_t value) {
        if (bit_offset == 0) {
            save_byte(value);
        } else {
            buffer |= value << bit_offset;
            save_byte(buffer);

            buffer = value >> (8 - bit_offset);
        }
    }

    void writer::emit16(uint16_t value) {
        emit8(value & 0xff);
        emit8(value >> 8);
    }

    void writer::emit24(uint32_t value) {
        emit16(value & 0xffff);
        emit8(value >> 16);
    }

    void writer::emit32(uint32_t value) {
        emit16(value & 0xffff);
        emit16(value >> 16);
    }

    void writer::emit(uint32_t value, std::size_t bits) {

        const auto bits_left = 8 - bit_offset;

        if (bits < bits_left) {
            buffer |= (value << bit_offset) & 0xff;
            bit_offset += bits;

            if (bit_offset == 8) {
                save_byte(buffer);
                buffer = 0;
                bit_offset = 0;
            }

            return;
        }

        if (bits >= bits_left) {
            buffer |= (value << bit_offset) & 0xff;
            save_byte(buffer);
            buffer = 0;

            bits       = bits - bits_left;
            value      = value >> bits_left;
            bit_offset = 0;
        }

        while (bits >= 8) {
            save_byte(value & 0xff);
            value >>= 8;
            bits -= 8;
        }
        
        if (bits > 0) {
            buffer = value;
            bit_offset = bits;
        }
    }

    void writer::finish() {
        if (bit_offset) {
            save_byte(buffer);
        }
    }


    // --------------------------------------------------

    bool reader::bit() {
        if (offset == 8) {
            byte = load_byte();
            offset = 0;
        }

        return (byte & (1 << offset++));
    }


    uint8_t reader::get8() {
        if (offset == 8) {
            return load_byte();
        }
        
        uint8_t result = 0;
        for (int i=0; i < 8; i++) {
            if (bit()) {
                result |= (1 << i);
            }
        }

        return result;
    }


    uint16_t reader::get16() {
        uint16_t lo = get8();
        uint16_t hi = get8();

        return (hi << 8) | lo;
    }


    uint32_t reader::get24() {
        uint32_t lo = get16();
        uint32_t hi = get8();

        return (hi << 16) | lo;
    }


    uint32_t reader::get32() {
        uint32_t lo = get16();
        uint32_t hi = get16();

        return (hi << 16) | lo;
    }


    uint32_t reader::get(std::size_t bits) {

        uint32_t result = 0;

        if (offset == 8) {
            byte = load_byte();
            offset = 0;
        }

        if (bits < 8 - offset) {
            result = (byte >> offset) & ((1u << bits) - 1);
            offset += bits;

            return result;
        }

        result = byte >> offset;
        bits  -= 8 - offset;

        int shift = 8 - offset;

        while (bits >= 8) {
            byte = load_byte();
            result |= uint32_t(byte) << shift;
            shift  += 8;
            bits   -= 8;
        }

        if (bits > 0) {
            byte = load_byte();
            result |= (uint32_t(byte) & ((1u << bits) - 1)) << shift;
            offset = bits;
        } else {
            offset = 8;
        }

        return result;
    }

} // namespace bitstream

#endif
