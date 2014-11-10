#include "common.hpp"
#include <memory>


namespace lookupcompress {

    class compressor {
    
        bitstream::writer& stream;

    public:
        compressor(bitstream::writer& stream) : stream(stream) {}

    public:
        void save(char* bytes, std::size_t count);
    };


    void compressor::save(char* bytes, std::size_t count) {

        if (count == 0) {
            return;
        }

        uint8_t* data = reinterpret_cast<uint8_t*>(bytes);
    
        // 1. create characters set
        characters set;
        for (decltype(count) i=0; i < count; i++) {
            set.set(data[i]);
        }

        // 1. save block size [2 bytes]
        stream.emit16(count);

        // 2. save set
        set.save(stream);

        // 3. save data
        const auto lookup = set.make_compress_lookup();
        const auto bits   = lookup.get_bits();

        for (unsigned i=0; i < count; i++) {
            const uint8_t byte = data[i];

            stream.emit(lookup[byte], bits);
        }

        stream.finish();
    }

} // namespce lookupcompress

