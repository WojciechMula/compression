#include "bitstream.hpp"
#include "common.hpp"
#include <unordered_map>
#include <map>
#include <cassert>


namespace ngramcompression {

    class decompressor {

        typedef std::unordered_map<uint32_t, ngram_type> map_type;

    public:
        decompressor() {}

        void decompress(bitstream::reader& reader, bitstream::writer& writer) {

            const std::size_t bits = reader.get16();

            auto map = load_grams(reader);

            while (true) {
                const auto index = reader.get(bits);
                if (index < 256) {
                    writer.emit8(index);
                } else {
                    const auto item = map.find(index);
                    assert(item != map.end());

                    const auto gram = (*item).second;
                    switch (gram.count) {
                        case 2: writer.emit16(gram.bytes); break;
                        case 3: writer.emit24(gram.bytes); break;
                        case 4: writer.emit32(gram.bytes); break;
                    }
                }
            }
        }


        map_type load_grams(bitstream::reader& reader) {

            map_type map;

            const std::size_t count2 = reader.get16();
            const std::size_t count3 = reader.get16();
            const std::size_t count4 = reader.get16();

            std::size_t code = 256;

            for (std::size_t i=0; i < count2; i++) {
                const uint32_t tmp = reader.get16();

                map[code++] = ngram_type(tmp, 2);
            }

            for (std::size_t i=0; i < count3; i++) {
                const uint32_t tmp = reader.get24();

                map[code++] = ngram_type(tmp, 3);
            }

            for (std::size_t i=0; i < count4; i++) {
                const uint32_t tmp = reader.get32();

                map[code++] = ngram_type(tmp, 4);
            }

            return map;
        }

    };


} // namespace ngramcompression

