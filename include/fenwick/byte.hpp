#ifndef __BYTE_FENWICK_TREE_H__
#define __BYTE_FENWICK_TREE_H__

#include "../common.hpp"
#include "fenwick_tree.hpp"

namespace dyn {

    static constexpr uint64_t BYTE_MASK[] = { 0x0ULL,
                                              0xFFULL,
                                              0xFFFFULL,
                                              0xFFFFFFULL,
                                              0xFFFFFFFFULL,
                                              0xFFFFFFFFFFULL,
                                              0xFFFFFFFFFFFFULL,
                                              0xFFFFFFFFFFFFFFULL,
                                              0xFFFFFFFFFFFFFFFFULL };

   /**
    * class ByteFenwickTree
    * @tree: Byte indexted Fenwick Tree data.
    * @size: Number of elements in the tree.
    * @level: Lookup table, it store the starting bit-position of each level.
    *
    * It's supposed to store * increments up to 63, so a single byte per element
    * is enough up to level 3. * Then, every 8 levels you add a byte per
    * element.
    *
    * The data is stored in a bottom-up level-order manner.
    */
    template<size_t LEAF_BITSIZE>
    class ByteFenwickTree : public FenwickTree
    {
    public:
    protected:
        const size_t size;
        DArray<uint8_t> tree;
        DArray<size_t> level;

    public:
        /**
         * ByteFenwickTree - Build a FenwickTree given a sequence of increments.
         * @sequence: Sequence of increments.
         * @size: Number of elements stored by the sequence.
         *
         * Running time: O(length)
         */
        ByteFenwickTree(uint64_t sequence[], size_t size) :
            size(size),
            level(find_last_set(size) + 1)
        {
            // TODO: "Ma non dovrebbe essere relativamente semplice estendere la formula
            // con la popcount che permette di fare l'interleaving nel caso Shrank per
            // fare interleaving nel caso byte?" (da vedere)

            level[0] = 0;
            for (size_t i = 1; i < level.size(); i++)
                level[i] = ((size + (1<<(i-1))) / (1<<i)) * get_size(i-1) + level[i-1];

            const size_t levels = level.size() - 1;
            tree = DArray<uint8_t>(level[levels] + 3); // +3 to prevent segfault on the last element

            for (size_t l = 0; l < levels; l++) {
                for (size_t node = 1<<l; node <= size; node += 1 << (l+1)) {
                    const size_t curr_byte_pos = level[l] + get_size(l) * (node >> (l+1));
                    auint64_t * const curr_element = reinterpret_cast<auint64_t*>(&tree[curr_byte_pos]);

                    size_t sequence_idx = node-1;
                    uint64_t value = sequence[sequence_idx];
                    for (size_t j = 0; j < l; j++) {
                        sequence_idx >>= 1;
                        const size_t prev_byte_pos = level[j] + get_size(j) * sequence_idx;
                        const auint64_t * const prev_element = reinterpret_cast<auint64_t*>(&tree[prev_byte_pos]);

                        value += *prev_element & BYTE_MASK[get_size(j)];
                    }

                    *curr_element &= ~BYTE_MASK[get_size(l)];
                    *curr_element |= value & BYTE_MASK[get_size(l)];
                }
            }
        }

        virtual uint64_t get(size_t idx) const
        {
            uint64_t sum = 0;

            idx++;
            size_t index = 0ULL;

            do {
                index += mask_last_set(idx ^ index);
                const size_t height = find_first_set(index) - 1;
                const size_t level_idx = index >> (1 + height);
                const size_t elem_size = get_size(height);
                const size_t byte_pos = level[height] + elem_size * level_idx;
                const auint64_t * const compact_element = reinterpret_cast<auint64_t*>(&tree[byte_pos]);

                sum += *compact_element & BYTE_MASK[elem_size];
            } while (idx ^ index);

            return sum;
        }

        virtual void set(size_t idx, int64_t inc)
        {
            for (idx = idx+1; idx <= size; idx += mask_first_set(idx)) {
                const size_t height = find_first_set(idx) - 1;
                const size_t level_idx = idx >> (1 + height);
                const size_t byte_pos = level[height] + get_size(height) * level_idx;
                auint64_t * const compact_element = reinterpret_cast<auint64_t*>(&tree[byte_pos]);

                *compact_element += inc;
            }
        }

        virtual size_t find(uint64_t val, bool complement=false) const
        {
            size_t node = 0, idx = 0;

            for (uint64_t height = level.size() - 2; height != -1ULL; height--) {
                const size_t elem_size = get_size(height);
                const size_t byte_pos = level[height] + elem_size * idx;
                const auint64_t * const compact_element = reinterpret_cast<auint64_t*>(&tree[byte_pos]);

                uint64_t value = 0;
                if (byte_pos >= level[level.size()-1]) value = -1ULL;
                else value = *compact_element & BYTE_MASK[elem_size];

                if (complement)
                    value = (1ULL << (LEAF_BITSIZE + height - 1)) - value;

                idx <<= 1;

                if(val >= value) {
                    idx++;
                    val -= value;
                    node += 1 << height;
                }
            }

            return node <= size ? node-1 : size-1;
        }

        virtual size_t bit_count() const
        {
            return sizeof(ByteFenwickTree<LEAF_BITSIZE>)*8 + level[level.size()-1]*8;
        }

    private:
        static inline size_t get_size(size_t height)
        {
            return (height+LEAF_BITSIZE-1) / 8 + 1;
        }
    };

}

#endif // __BYTE_FENWICK_TREE_H__