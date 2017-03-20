#ifndef __SHRANK_FENWICK_TREE_H__
#define __SHRANK_FENWICK_TREE_H__

#include "fenwick_tree.hpp"

/** TODO: commenti
 * class
 */
class ShrankFenwickTree : public FenwickTree
{
public:
    static constexpr std::size_t LEAF_BITSIZE = 6;

protected:
    std::uint8_t *tree = nullptr;
    const std::size_t size;
    const std::size_t levels;

public:
    /**
     * CompactFenwickTree - Build a FenwickTree given a sequence of increments.
     * @sequence: Sequence of increments.
     * @size: Number of elements stored by the sequence.
     *
     * Running time: O(length)
     */
    ShrankFenwickTree(std::uint64_t sequence[], std::size_t size);

    virtual ~ShrankFenwickTree();

    virtual std::uint64_t get(std::size_t idx) const;

    virtual void set(std::size_t idx, std::uint64_t inc);

    virtual std::size_t find(std::uint64_t val) const;

    virtual std::size_t bit_count() const;
};

#endif // __SHRANK_FENWICK_TREE_H__
