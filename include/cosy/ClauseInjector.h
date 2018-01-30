// Copyright 2017 Hakan Metin - LIP6

#ifndef INCLUDE_COSY_CLAUSEINJECTOR_H_
#define INCLUDE_COSY_CLAUSEINJECTOR_H_

#include <deque>
#include <unordered_set>
#include <vector>

#include "cosy/Literal.h"
#include "cosy/Logging.h"
#include "cosy/Stats.h"

namespace cosy {

class ClauseInjector {
 public:
    ClauseInjector();
    ~ClauseInjector();


    bool addConflictClause(BooleanVariable cause,
                           const std::vector<Literal>& literals);
    bool isConflictClause() const { return _conflicts.size() > 0; }
    bool isConflictClause(BooleanVariable cause) const;
    std::vector<Literal> conflictClause();
    void removeConflictClause(BooleanVariable cause);

    void addUnitClause(const Literal& literal);
    bool isUnitClause() const;
    Literal unitClause();

    bool addAssertiveClause(BooleanVariable cause,
                            const std::vector<Literal>& literals);
    void printStats() const { _stats.print(); }

 private:
    struct ClauseInfos {
        ClauseInfos(BooleanVariable c, const std::vector<Literal>& l) :
            cause(c), literals(l) {}

        BooleanVariable cause;
        std::vector<Literal> literals;
    };

    std::deque<ClauseInfos> _conflicts;
    std::unordered_set<Literal> _units;

    struct Stats : StatsGroup {
        Stats() : StatsGroup("Clause Injector"),
                  conflicts("Number of ESBP", this),
                  units("Number of Units", this) {}
        CounterStat conflicts;
        CounterStat units;
    };
    Stats _stats;

    DISALLOW_COPY_AND_ASSIGN(ClauseInjector);
};

}  // namespace cosy

#endif  // INCLUDE_COSY_CLAUSEINJECTOR_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
