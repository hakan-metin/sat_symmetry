// Copyright 2017 Hakan Metin - LIP6

#ifndef INCLUDE_COSY_TRAIL_H_
#define INCLUDE_COSY_TRAIL_H_

#include <unordered_map>
#include <vector>

#include "cosy/Assignment.h"
#include "cosy/Bitset.h"
#include "cosy/Literal.h"
#include "cosy/Stats.h"

namespace cosy {

using Reason = std::vector<Literal>;
const Reason kNoReason = Reason();

class Trail {
 public:
    Trail() {}
    ~Trail() {}

    void resize(unsigned int n);
    void enqueue(Literal literal, unsigned int level,
                 const Reason& reason, bool isReasonSymmetric, bool isDecision);
    Literal dequeue();

    bool isDecision(Literal literal) const {
        return _decisions.IsSet(literal.variable());
    }

    unsigned int level(Literal literal) const {
        return _levels.at(literal.variable());
    }

    const Reason& reason(Literal literal) const {
        return _reasons.at(literal.variable());
    }

    bool isReasonSymmetric(Literal literal) const {
        return _reasonsAreSymmetrics.at(literal.variable());
    }

    const Assignment& assignment() const { return _assignment; }

    void printStats() const { _stats.print(true); }

 private:
    std::vector<Literal> _trail;
    Assignment _assignment;
    Bitset64<BooleanVariable> _decisions;
    std::unordered_map<BooleanVariable, int> _levels;
    std::unordered_map<BooleanVariable, Reason> _reasons;
    std::unordered_map<BooleanVariable, bool> _reasonsAreSymmetrics;

    struct Stats : public StatsGroup {
        Stats() : StatsGroup(" Trail "),
                  total_time("Trail total time", this),
                  enqueue_time(" |- enqueue time", this),
                  dequeue_time(" |- dequeue time", this),
                  levels("Level Distribution", this)
        {}
        TimeDistribution total_time;
        TimeDistribution enqueue_time;
        TimeDistribution dequeue_time;
        IntegerDistribution levels;
    };
    Stats _stats;
};


inline void Trail::resize(unsigned int n) {
    _assignment.resize(n);
    _decisions.Resize(BooleanVariable(n));
}

inline void Trail::enqueue(Literal literal, unsigned int level,
                           const Reason& reason, bool isReasonSymmetric,
                           bool isDecision) {
    ScopedTimeDistributionUpdater time(&_stats.total_time);
    time.alsoUpdate(&_stats.enqueue_time);

    const BooleanVariable variable = literal.variable();

    // In level 0 cannot be decision or symmetric reason
    CHECK(level != 0 || !isDecision);
    /* CHECK(level != 0 || !isReasonSymmetric); */

    _trail.push_back(literal);
    _assignment.assignFromTrueLiteral(literal);
    _decisions.Set(variable, isDecision);
    _levels[variable] = level;
    _reasons[variable] = reason;
    _reasonsAreSymmetrics[variable] = isReasonSymmetric;
    _stats.levels.add(level);
}

inline Literal Trail::dequeue() {
    ScopedTimeDistributionUpdater time(&_stats.total_time);
    time.alsoUpdate(&_stats.dequeue_time);

    const Literal literal = _trail.back();
    const BooleanVariable variable = literal.variable();

    _trail.pop_back();
    _assignment.unassignLiteral(literal);
    _decisions.Clear(variable);
    _levels.erase(_levels.find(variable));
    _reasons.erase(_reasons.find(variable));
    _reasonsAreSymmetrics.erase(_reasonsAreSymmetrics.find(variable));

    return literal;
}


}  // namespace cosy
#endif  // INCLUDE_COSY_TRAIL_H_
