// Copyright 2017 Hakan Metin - LIP6

#include "cosy/Group.h"
#include "cosy/Printer.h"

namespace cosy {

Group::Group() : _num_augmented_generators(0) {
}

Group::~Group() {
}

void Group::addPermutation(std::unique_ptr<Permutation>&& permutation,
                           bool augment /* = true */) {
    CHECK_NOTNULL(permutation);

    const unsigned int permutation_index = _permutations.size();
    const unsigned int num_cycles = permutation->numberOfCycles();

    if (num_cycles == 0)
        return;

    if (isPermutationSpurious(permutation))
        return;

    unsigned int order = permutation->order();
    if (augment && order > 2) {
        std::unique_ptr<Permutation> p = permutation->mult(order - 1);
        addPermutation(std::move(p), false);
        _num_augmented_generators++;
    }

    if (permutation->size() > _watchers.size())
        _watchers.resize(permutation->size());

    for (unsigned int c = 0; c < num_cycles; ++c) {
        Literal element = permutation->lastElementInCycle(c);

        for (const Literal& image : permutation->cycle(c)) {
            const int index = image.variable().value();
            _watchers[index].push_back(permutation_index);

            const BooleanVariable variable = image.variable();
            _symmetric.insert(variable);

            if (element == image.negated())
                _inverting.insert(variable);

            element = image;
        }
    }

    _permutations.emplace_back(permutation.release());
}

void Group::augmentAll() {
    unsigned int sz = _permutations.size();
    for (unsigned int i=0; i<sz; i++) {
        const std::unique_ptr<Permutation>& permutation = _permutations[i];
            unsigned int order = permutation->order();
            if (order > 2) {
                std::unique_ptr<Permutation> p = permutation->mult(order - 1);
                addPermutation(std::move(p), false);
                _num_augmented_generators++;
            }
    }
}


Group::Iterator Group::watch(BooleanVariable variable) const {
    const int index = variable.value();
    return Iterator(_watchers[index].begin(), _watchers[index].end());
}

bool Group::isPermutationSpurious(const std::unique_ptr<Permutation>& p) const {
    const unsigned int num_cycles = p->numberOfCycles();
    for (unsigned int c = 0; c < num_cycles; ++c) {
        for (const Literal& element : p->cycle(c)) {
            if (p->isTrivialImage(element))
                return true;
            if (p->isTrivialImage(element.negated()))
                return true;
            if (p->imageOf(element).negated() != p->imageOf(element.negated()))
                return true;
        }
    }

    return false;
}

void Group::summarize(unsigned int num_vars) const {
    int64 initial, augmented, total;
    total = numberOfPermutations();
    augmented = numberOfAugmentedPermutations();
    initial = total - augmented;

    Printer::printStat("Number of generators", total);
    Printer::printStat(" |- Number of initial generators", initial);
    Printer::printStat(" |- Number of augmented generators", augmented);
    Printer::printStat("Number of vars in generators",
                       numberOfSymmetricVariables(),
                       static_cast<int64>(num_vars));
    Printer::printStat("Number of inverting", numberOfInverting());
}

void Group::debugPrint() const {
    unsigned int i = 1;
    for (const std::unique_ptr<Permutation>& permutation : _permutations) {
        std::cout << "[" << i++ << "]: ";
        permutation->debugPrint();
    }
}

}  // namespace cosy


/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
