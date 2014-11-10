#ifndef COMMON_HPP_INCLUDED__
#define COMMON_HPP_INCLUDED__

#include <cassert>
#include <cstdint>


namespace ngramcompression {

    struct ngram_type {
        uint32_t bytes;
        int      count;

        ngram_type() = default;

        ngram_type(uint32_t b, int count) : count(count) {

            assert(count == 2 || count == 3 || count == 4);

            switch (count) {
                case 2:
                    bytes = b & 0x0000ffff;
                    break;
                case 3:
                    bytes = b & 0x00ffffff;
                    break;
            }
        }

        bool operator==(const ngram_type& other) const {
            return count == other.count &&
                   bytes == other.bytes;
        }
    };


    struct ngram_hasher {
        std::size_t operator()(const ngram_type& gram) const {
            return std::hash<uint32_t>()(gram.bytes) ^ std::hash<int>()(gram.count);
        }
    };


} // namespace ngramcompression

#endif
