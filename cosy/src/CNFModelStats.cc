// Copyright 2017 Hakan Metin - LIP6

#include "cosy/CNFModelStats.h"
#include "cosy/Printer.h"

namespace cosy {

CNFModelStats::CNFModelStats() :
    _num_variables(0),
    _num_clauses(0),
    _num_unary_clauses(0),
    _num_binary_clauses(0),
    _num_ternary_clauses(0),
    _num_large_clauses(0) {
}

CNFModelStats::~CNFModelStats() {
}

void CNFModelStats::addClause(std::vector<Literal>* literals) {
    CHECK_GT(literals->size(), static_cast<unsigned int>(0));

    /* Remove duplicate literals in clause */
    std::sort(literals->begin(), literals->end());
    auto last_element_it = std::unique(literals->begin(), literals->end());
    literals->erase(last_element_it, literals->end());

    BooleanVariable var = literals->back().variable();
    if (var > _num_variables)
        _num_variables = var.value();

    _num_clauses++;

    compute_occurences(*literals);
    compute_sizes(*literals);
}

void CNFModelStats::compute_occurences(const std::vector<Literal>& literals) {
    const int64 num_vars = numberOfVariables();
    for (const Literal& literal : literals) {
        const int64 index = literal.variable().value();

        if (num_vars > index) {
            _positive_occurences.resize(num_vars);
            _negative_occurences.resize(num_vars);
            _occurences.resize(num_vars);
        }

        if (literal.isPositive())
            _positive_occurences[index]++;
        else
            _negative_occurences[index]++;
        _occurences[index]++;
    }
}

void CNFModelStats::compute_sizes(const std::vector<Literal>& literals) {
    switch (literals.size()) {
    case 1:  _num_unary_clauses++;   break;
    case 2:  _num_binary_clauses++;  break;
    case 3:  _num_ternary_clauses++; break;
    default: _num_large_clauses++;   break;
    }
}

void CNFModelStats::summarize() const {
    const int64 ncl = numberOfClauses();

    Printer::printSection(" Instance Informations ");
    Printer::printStat("Number of variables", numberOfVariables());
    Printer::printStat("Number of clauses", ncl);
    Printer::printStat(" |- unary clauses", numberOfUnaryClauses(), ncl);
    Printer::printStat(" |- binary clauses", numberOfBinaryClauses(), ncl);
    Printer::printStat(" |- ternary clauses",numberOfTernaryClauses(), ncl);
    Printer::printStat(" |- large clauses", numberOfLargeClauses(), ncl);
}

}  // namespace cosy

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
