#include "bitstream.hpp"
#include "common.hpp"
#include <cstdint>


namespace lookupcompress {

    class decompressor {

        bitstream::reader& stream;

    public:
        decompressor(bitstream::reader& stream);

        void decode(bitstream::writer& writer);

    private:
        void decode_block(bitstream::writer& writer);
    };


    decompressor::decompressor(bitstream::reader& stream)
        : stream(stream) {}


    void decompressor::decode(bitstream::writer& writer) {
        while (true) {
            decode_block(writer);
        }
    }


    void decompressor::decode_block(bitstream::writer& writer) {
        const std::size_t size  = stream.get16();

        characters set;

        set.load(stream);

        const auto lookup = set.make_decompress_lookup();
        const auto bits   = lookup.get_bits();
        for (unsigned i=0; i < size; i++) {
            const auto index = stream.get(bits);

            writer.emit8(lookup[index]);
        }

        const auto total = bits * size;
        for (unsigned i=0; i < total % 8; i++) {
            stream.bit();
        }
    }

} // namespace lookupcompress
