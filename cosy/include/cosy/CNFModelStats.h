// Copyright 2017 Hakan Metin


#ifndef INCLUDE_COSY_CNFMODELSTATS_H_
#define INCLUDE_COSY_CNFMODELSTATS_H_

#include <algorithm>
#include <memory>
#include <vector>

#include "cosy/Clause.h"
#include "cosy/Literal.h"
#include "cosy/Logging.h"

namespace cosy {

class CNFModelStats {
 public:
    CNFModelStats();
    ~CNFModelStats();

    void addClause(std::vector<Literal>* literals);

    int64 numberOfVariables()      const { return _num_variables + 1; }
    int64 numberOfClauses()        const { return _num_clauses;       }

    int64 numberOfUnaryClauses()   const { return _num_unary_clauses;   }
    int64 numberOfBinaryClauses()  const { return _num_binary_clauses;  }
    int64 numberOfTernaryClauses() const { return _num_ternary_clauses; }
    int64 numberOfLargeClauses()   const { return _num_large_clauses;   }

    const std::vector<int64>& occurences() const { return _occurences; }

    void summarize() const;
 private:
    int64 _num_variables;
    int64 _num_clauses;

    int64 _num_unary_clauses;
    int64 _num_binary_clauses;
    int64 _num_ternary_clauses;
    int64 _num_large_clauses;

    std::vector<int64> _positive_occurences;
    std::vector<int64> _negative_occurences;
    std::vector<int64> _occurences;

    void compute_occurences(const std::vector<Literal>& literals);
    void compute_sizes(const std::vector<Literal>& literals);

    DISALLOW_COPY_AND_ASSIGN(CNFModelStats);
};
}  // namespace cosy

#endif  // INCLUDE_COSY_CNFMODELSTATS_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
