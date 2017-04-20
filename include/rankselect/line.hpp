#ifndef __LINE_RANK_SELECT_H__
#define __LINE_RANK_SELECT_H__

#include "rank_select.hpp"

namespace dyn {

    template <template<size_t> class T, size_t WORDS>
    class LineRankSelect {
    private:
        static constexpr size_t LEAF_BITSIZE = 7 + WORDS - 1;
        T<LEAF_BITSIZE> tree;
        DArray<uint64_t> _bitvector;

    public:
        /**
         * LineRankSelect - Build a dynamic rank&select data structure
         * @bitvector: A bitvector of 64-bit words
         * @length: The length (in words) of the bitvector
         *
         * If you pass the ownership of @bitvector it will make a shallow copy
         * (copy the pointer) and if you don't it will make a deep copy (copy
         * the data) of the bitvector. This data structure works correctly as
         * long as you don't touch its underlining bitvector, so it prevents you
         * to do it.
         */
        LineRankSelect(uint64_t bitvector[], size_t length):
            tree(build_fenwick(bitvector, length)),
            _bitvector(DArray<uint64_t>(length))
        {
            std::copy_n(bitvector, length, _bitvector.get());
        }

        LineRankSelect(DArray<uint64_t> bitvector, size_t length):
            tree(build_fenwick(_bitvector.get(), length)),
            _bitvector(std::move(bitvector))
        {
        }

        virtual const uint64_t* bitvector() const
        {
            return _bitvector.get();
        }

        virtual size_t bitvector_size() const
        {
            return _bitvector.size();
        }

        virtual uint64_t rank(size_t pos) const
        {
            // TODO: spostare il controllo sul -1 dentro l'albero?
            size_t idx = pos/(64*WORDS);
            uint64_t value = idx ? tree.get(idx-1) : 0;

            for (size_t i = idx*WORDS; i < pos/64; i++)
                value += popcount(_bitvector[i]);

            return value + popcount(_bitvector[pos/64] & compact_bitmask(pos%64, 0));
        }

        virtual uint64_t rank(size_t from, size_t to) const
        {
            // TODO: ottimizzabile
            return rank(to) - rank(from);
        }

        virtual uint64_t rankZero(size_t pos) const
        {
            return pos - rank(pos);
        }

        virtual uint64_t rankZero(size_t from, size_t to) const
        {
            return (to - from) - rank(from, to);
        }

        virtual size_t select(uint64_t rank) const
        {
            // TODO: spostare il controllo sul -1 dentro l'albero?
            const size_t idx = tree.find(rank) + 1;
            rank -= (idx != 0 ? tree.get(idx-1) : 0);

            for (size_t i = idx*WORDS; i < idx*WORDS+WORDS; i++) {
                const int rank_chunk = popcount(_bitvector[i]);
                if (rank < rank_chunk)
                    return i*64 + select64(_bitvector[i], rank);
                else
                    rank -= rank_chunk;
            }

            return (idx+1)*WORDS*64;
        }

        virtual size_t selectZero(uint64_t rank) const
        {
            // TODO: spostare il controllo sul -1 dentro l'albero?
            const size_t idx = tree.find(rank, true) + 1;
            rank -= 64*idx*WORDS - (idx != 0 ? tree.get(idx-1) : 0);

            for (size_t i = idx*WORDS; i < idx*WORDS+WORDS; i++) {
                const int rank_chunk = popcount(~_bitvector[i]);
                if (rank < rank_chunk)
                    return i*64 + select64(~_bitvector[i], rank);
                else
                    rank -= rank_chunk;
            }

            return (idx+1)*WORDS*64;
        }

        virtual uint64_t update(size_t index, uint64_t word)
        {
            const uint64_t old = _bitvector[index];
            _bitvector[index] = word;
            tree.set(index / WORDS, popcount(word) - popcount(old));

            return old;
        }

        virtual size_t bit_count() const
        {
            return sizeof(LineRankSelect<T, WORDS>)
                + _bitvector.size()*64
                + tree.bit_count() - sizeof(tree);
        }

    private:
        T<LEAF_BITSIZE> build_fenwick(const uint64_t bitvector[], size_t length)
        {
            uint64_t *sequence = new uint64_t[length/WORDS + 1]();
            for (size_t i = 0; i < length; i++)
                sequence[i/WORDS] += popcount(bitvector[i]);

            return T<LEAF_BITSIZE>(sequence, length);
        }
    };

}

#endif // __LINE_RANK_SELECT_H__
