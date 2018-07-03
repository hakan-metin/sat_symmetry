#include <iostream>
#include <string>

#include <cosy/BlissSymmetryFinder.h>
#include <cosy/CNFModel.h>
#include <cosy/CNFReader.h>
#include <cosy/Group.h>
#include <cosy/Printer.h>

using cosy::Permutation;
using cosy::Printer;


int main(int argc, char **argv) {
    std::unique_ptr<cosy::SymmetryFinder> symmetry_finder = nullptr;
    cosy::CNFReader cnf_reader;
    cosy::CNFModel cnf_model;
    cosy::Group group;
    std::string cnf_filename;
    bool success;

    if (argc != 2) {
        std::cerr << "Usage: cnf_file" << std::endl;
        return 1;
    }

    cnf_filename = argv[1];

    success = cnf_reader.load(cnf_filename, &cnf_model);
    if (!success) {
        std::cerr << "CNF file " << cnf_filename << " is not well formed." <<
            std::endl;
        return 1;
    }

    symmetry_finder = std::unique_ptr<cosy::BlissSymmetryFinder>
        (new cosy::BlissSymmetryFinder(cnf_model));
    symmetry_finder->findAutomorphism(&group);


    unsigned int num_involutions = 0;
    unsigned int bin_cycles = 0;
    unsigned int large_cycles = 0;

    bool isInvolution;

    for (const std::unique_ptr<Permutation>& perm : group.permutations()) {
        isInvolution = true;
        for (unsigned int c = 0; c < perm->numberOfCycles(); ++c) {
            if (perm->cycle(c).size() == 2) {
                bin_cycles++;
            } else {
                large_cycles++;
                isInvolution = false;
            }
        }

        if (isInvolution)
            num_involutions++;
    }

    Printer::printStat("number of binary cycles", bin_cycles,
                       bin_cycles+large_cycles);

    Printer::printStat("number of large cycles", large_cycles,
                       bin_cycles+large_cycles);
    Printer::printStat("number of involutions", num_involutions,
                       (unsigned int)group.permutations().size());

    return 0;
}
