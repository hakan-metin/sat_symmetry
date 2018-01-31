#include <iostream>
#include <string>

#include <cosy/SaucySymmetryFinder.h>
#include <cosy/CNFModel.h>
#include <cosy/CNFReader.h>
#include <cosy/Group.h>

int main(int argc, char **argv) {
    std::unique_ptr<cosy::SymmetryFinder> symmetry_finder = nullptr;
    cosy::CNFReader cnf_reader;
    cosy::CNFModel cnf_model;
    cosy::Group group;
    std::string cnf_filename;
    bool success;

    if (argc < 2) {
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

    symmetry_finder = std::unique_ptr<cosy::SaucySymmetryFinder>
        (new cosy::SaucySymmetryFinder(cnf_model));
    symmetry_finder->findAutomorphism(&group);

    cnf_model.summarize();
    group.summarize(cnf_model.numberOfVariables());

    return 0;
}
