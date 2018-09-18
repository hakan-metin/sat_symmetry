// Copyright 2017 Hakan Metin - LIP6

#ifndef INCLUDE_COSY_GROUP_H_
#define INCLUDE_COSY_GROUP_H_

#include <memory>
#include <set>
#include <vector>

#include "cosy/Macros.h"
#include "cosy/Permutation.h"

namespace cosy {

class Group {
 public:
    Group();
    ~Group();

    void addPermutation(std::unique_ptr<Permutation>&& permutation);
    struct Iterator;
    Iterator watch(BooleanVariable var) const;

    const std::vector< std::unique_ptr<Permutation> >& permutations() const {
        return _permutations;
    }

    int64 numberOfPermutations() const { return _permutations.size(); }
    int64 numberOfSymmetricVariables() const { return _symmetric.size(); }
    int64 numberOfInverting() const { return _inverting.size(); }

    void debugPrint() const;
    void summarize(unsigned int num_vars) const;

 private:
    std::vector< std::unique_ptr<Permutation> > _permutations;
    std::set<BooleanVariable> _symmetric;
    std::set<BooleanVariable> _inverting;
    std::vector< std::set<int> > _watchers;

    bool isPermutationSpurious(const std::unique_ptr<Permutation>& p) const;
};

struct Group::Iterator {
    typedef int value_type;
    typedef std::set<int>::const_iterator const_iterator;

    Iterator() {}
    Iterator(const std::set<int>::const_iterator& b,
             const std::set<int>::const_iterator& e) :
        _begin(b), _end(e) {}

    std::set<int>::const_iterator begin() const { return _begin; }
    std::set<int>::const_iterator end() const { return _end; }
    const std::set<int>::const_iterator _begin;
    const std::set<int>::const_iterator _end;

    int size() const { return std::distance(_begin, _end); }
};

}  // namespace cosy


#endif  // INCLUDE_COSY_GROUP_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
