#include "bitstream.hpp"
#include "common.hpp"
#include <unordered_map>
#include <map>
#include <cassert>


namespace ngramcompression {

    class compressor {

        class gram_collector {

            std::unordered_map<ngram_type, int, ngram_hasher> map;

            class lookup {

                friend class gram_collector;

                typedef std::unordered_map<ngram_type, uint32_t, ngram_hasher> map_type;

                map_type map;

            private:
                lookup(map_type&& map) : map(map) {}
                void load(bitstream::reader& reader);

            public:
                struct lookup_result {
                    int      length; //< value in range 1..4
                    uint32_t code;   //< char or gram code
                };

                lookup_result operator[](char* c) const;
                uint32_t get_code(char c) const;

            public:
                void save(bitstream::writer& writer) const;
            };

        public:
            gram_collector(char* buffer, std::size_t size);

            /**
                Bits must be greater than 8.
            */
            lookup get_lookup(const std::size_t bits);
        };

        const std::size_t bits;

    public:
        compressor(std::size_t bits) : bits(bits) {}

        void compress(bitstream::writer& writer, char* data, std::size_t size) {

            gram_collector gc(data, size);

            const auto lookup = gc.get_lookup(bits);

            writer.emit16(bits);
            lookup.save(writer);

            std::size_t i = 0;
            while (i < size - 3) {
                const auto gram = lookup[data + i];

                writer.emit(gram.code, bits);

                i += gram.length;
            }

            while (i < size) {
                writer.emit(lookup.get_code(data[i]), bits);
                i += 1;
            }
        }

    };


    // --------------------------------------------------


    compressor::gram_collector::gram_collector(char* buffer, std::size_t size) {
        if (size >= 4) {
            for (auto i=0u; i < size - 3; i++) {

                const uint32_t tmp = *reinterpret_cast<uint32_t*>(buffer + i);
                const auto gram4 = ngram_type(tmp, 4);
                const auto gram3 = ngram_type(tmp, 3);
                const auto gram2 = ngram_type(tmp, 2);

                map[gram4] += 1;
                map[gram3] += 1;
                map[gram2] += 1;
            }
        }
    }


    auto compressor::gram_collector::get_lookup(const std::size_t bits) -> lookup {

        const auto max_size = (1u << bits) - (1u << 8);

        std::multimap<int, ngram_type> keys;

        for (const auto& item: map) {
            const auto key   = item.first;
            const auto count = item.second;

            if (keys.size() < max_size) {
                keys.insert({count, key});
            } else {
                auto minimum = keys.begin();
                keys.erase(minimum);
                keys.insert({count, key});
            }
        }

        assert(keys.size() == std::min(max_size, map.size()));

        lookup::map_type gram2code;

        uint32_t code = 256;

        for (const auto& item: keys) {
            const auto gram = item.second;
            if (gram.count == 2) {
                gram2code[gram] = code++;
            }
        }

        for (const auto& item: keys) {
            const auto gram = item.second;
            if (gram.count == 3) {
                gram2code[gram] = code++;
            }
        }

        for (const auto& item: keys) {
            const auto gram = item.second;
            if (gram.count == 4) {
                gram2code[gram] = code++;
            }
        }

        assert(code == 256 + keys.size());

        return lookup(std::move(gram2code));
    }


    auto compressor::gram_collector::lookup::operator[](char* c) const -> lookup_result {

        // WARNING: possible access violation when buffer is located just
        //          before unreadable pages. Caller is responsible for preventing
        //          this error.

        const uint32_t tmp = *reinterpret_cast<uint32_t*>(c);

        for (int size=4; size >= 2; size--) {
            const auto gram = ngram_type(tmp, size);
            const auto item = map.find(gram);
            if (item != map.end()) {
                return {size, (*item).second};
            }
        }

        return {1, get_code(*c)};
    }


    uint32_t compressor::gram_collector::lookup::get_code(char c) const {
        return uint8_t(c);
    }


    void compressor::gram_collector::lookup::save(bitstream::writer& writer) const {

        int count2 = 0;
        int count3 = 0;
        int count4 = 0;

        for (const auto& item: map) {
            const auto& gram = item.first;
            switch (gram.count) {
                case 2: count2 += 1; break;
                case 3: count3 += 1; break;
                case 4: count4 += 1; break;
            }
        }

        writer.emit16(count2);
        writer.emit16(count3);
        writer.emit16(count4);

        std::map<uint32_t, ngram_type> ordered;

        for (const auto& item: map) {
            const auto& gram = item.first;
            const auto  code = item.second;

            assert(ordered.find(code) == ordered.end());
            ordered[code] = gram;
        }

        for (const auto& item: ordered) {
            const auto& gram = item.second;

            if (gram.count == 2) {
                writer.emit16(gram.bytes);
            }
        }

        for (const auto& item: ordered) {
            const auto& gram = item.second;

            if (gram.count == 3) {
                writer.emit24(gram.bytes);
            }
        }

        for (const auto& item: ordered) {
            const auto& gram = item.second;

            if (gram.count == 4) {
                writer.emit32(gram.bytes);
            }
        }
    }

} // namespace ngramcompression

