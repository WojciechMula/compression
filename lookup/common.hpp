#ifndef COMMON_HPP_INCLUDED__
#define COMMON_HPP_INCLUDED__


#include "bitstream.hpp"
#include <memory>


namespace lookupcompress {

    
    class characters {

        uint8_t bytes[256/8];

        class lookup {
            friend class characters;

            std::size_t bits;
            int         table[256];

        public:
            std::size_t get_bits() const {
                return bits;
            }

            int operator[](std::size_t index) const {
                return table[index];
            }
        };

    public:
        void set(std::size_t index);
        void clear_all();
        lookup make_compress_lookup();
        lookup make_decompress_lookup();

        std::size_t size() const {
            return sizeof(bytes);
        }

    public:
        void save(bitstream::writer& writer) const;
        void load(bitstream::reader& reader);

    private:
        int log2(int n) const;
    };


    // --------------------------------------------------


    void characters::set(std::size_t index) {
        bytes[index/8] |= (1 << index % 8);
    }


    void characters::clear_all() {
        for (auto& byte: bytes) {
            byte = 0;
        }
    }


    void characters::save(bitstream::writer& writer) const {
        for (unsigned i=0; i < sizeof(bytes); i++) {
            writer.emit8(bytes[i]);
        }
    }


    void characters::load(bitstream::reader& reader) {
        for (unsigned i=0; i < sizeof(bytes); i++) {
            bytes[i] = reader.get8();
        }
    }


    characters::lookup characters::make_compress_lookup() {
        lookup result;

        int n = 0;
        int k = 0;
        for (int i=0; i < 256/8; i++) {
            const uint8_t byte = bytes[i];
            for (int j=0; j < 8; j++) {
                if (byte & (1 << j)) {
                    result.table[k++] = n++;
                } else {
                    result.table[k++] = -1;
                }
            }
        }

        result.bits = log2(n);

        return result;
    }


    characters::lookup characters::make_decompress_lookup() {
        lookup result;

        int n = 0;
        int k = 0;
        for (int i=0; i < 256/8; i++) {
            const uint8_t byte = bytes[i];
            for (int j=0; j < 8; j++) {
                if (byte & (1 << j)) {
                    result.table[n++] = k;
                }

                k++;
            }
        }

        result.bits = log2(n);

        return result;
    }


    int characters::log2(int n) const {
        int result = 0;
        int bit = 0;
        while (n) {
            if (n % 2) {
                result = bit;
            }

            n /= 2;
            bit += 1;
        }
        
        return result + 1;
    }

} // namespce lookupcompress

#endif
