#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>
#include <string.h>

#include <cosy/BlissSymmetryFinder.h>
#include <cosy/CNFModel.h>
#include <cosy/CNFReader.h>
#include <cosy/Group.h>
#include <cosy/Orbits.h>

int main(int argc, char **argv) {
    std::unique_ptr<cosy::SymmetryFinder> symmetry_finder = nullptr;
    cosy::CNFReader cnf_reader;
    cosy::CNFModel cnf_model;
    cosy::Group group;
    cosy::Orbits orbits;

    std::string cnf_filename;
    bool success;

    if (argc != 2) {
        std::cerr << "Usage: cnf_file" << std::endl;
        return 1;
    }

    cnf_filename = argv[1];

    std::cout << "# CNF Stats" << std::endl;
    std::cout << "path=" << cnf_filename << std::endl;
    std::cout << "file=" << basename(cnf_filename.c_str()) << std::endl;
    std::string command = "echo md5sum=$(md5sum \"" + cnf_filename +
        "\" | cut -d ' ' -f1)";
    std::system(command.c_str());

    success = cnf_reader.load(cnf_filename, &cnf_model);
    if (!success) {
        std::cout << "is_well_formed=0" << std::endl;
    } else {
        std::cout << "is_well_formed=1" << std::endl;
    }

    std::cout << "num_vars=" << cnf_model.numberOfVariables() << std::endl;
    std::cout << "num_clauses=" << cnf_model.numberOfClauses() << std::endl;
    std::cout << "num_unary_clauses=" << cnf_model.numberOfUnaryClauses()
              << std::endl;
    std::cout << "num_binary_clauses=" << cnf_model.numberOfBinaryClauses()
              << std::endl;
    std::cout << "num_ternary_clauses=" << cnf_model.numberOfTernaryClauses()
              << std::endl;
    std::cout << "num_large_clauses=" << cnf_model.numberOfLargeClauses()
              << std::endl;

    std::cout << std::endl;

    symmetry_finder = std::unique_ptr<cosy::BlissSymmetryFinder>
        (new cosy::BlissSymmetryFinder(cnf_model));
    symmetry_finder->findAutomorphism(&group);

    std::cout << "# Symmetry Stats" << std::endl;
    std::cout << "num_generators=" << group.numberOfPermutations() << std::endl;
    std::cout << "percent_sym_vars=" <<
        std::ceil((float)group.numberOfSymmetricVariables() /
                  cnf_model.numberOfVariables() * 100) << std::endl;
    std::cout << "is_only_involution=" << group.isOnlyInvolution() << std::endl;
    std::cout << "is_inverting_perm=" <<
        (group.numberOfInverting() != 0) << std::endl;
    orbits.assign(group.permutations());
    std::cout << "num_orbits=" << orbits.numberOfOrbits() << std::endl;

    std::cout << std::endl;

    return 0;
}
