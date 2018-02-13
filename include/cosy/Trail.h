// Copyright 2017 Hakan Metin - LIP6

#ifndef INCLUDE_COSY_TRAIL_H_
#define INCLUDE_COSY_TRAIL_H_

#include <unordered_map>
#include <vector>

#include "cosy/Assignment.h"
#include "cosy/Bitset.h"
#include "cosy/Literal.h"

namespace cosy {

using Reason = std::vector<Literal>;
const Reason kNoReason = Reason();

class Trail {
 public:
    Trail() {}
    ~Trail() {}

    void resize(unsigned int n);
    void enqueue(Literal literal, unsigned int level,
                 const Reason& reason, bool isDecision);
    Literal dequeue();

    bool isDecision(Literal literal) const {
        return _decisions.IsSet(literal.variable());
    }
    unsigned int level(Literal literal) const;
    const Reason& reason(Literal literal) const;

    const Assignment& assignment() const { return _assignment; }

 private:
    std::vector<Literal> _trail;
    Assignment _assignment;
    Bitset64<BooleanVariable> _decisions;
    std::unordered_map<BooleanVariable, int> _levels;
    std::unordered_map<BooleanVariable, Reason> _reasons;
};


inline void Trail::resize(unsigned int n) {
    _assignment.resize(n);
    _decisions.Resize(BooleanVariable(n));
}

inline void Trail::enqueue(Literal literal, unsigned int level,
                           const Reason& reason, bool isDecision) {
    const BooleanVariable variable = literal.variable();

    _trail.push_back(literal);
    _assignment.assignFromTrueLiteral(literal);
    _decisions.Set(variable, isDecision);
    _levels[variable] = level;
    _reasons[variable] = reason;
}

inline Literal Trail::dequeue() {
    const Literal literal = _trail.back();
    const BooleanVariable variable = literal.variable();

    _trail.pop_back();
    _assignment.unassignLiteral(literal);
    _decisions.Clear(variable);
    _levels.erase(_levels.find(variable));
    _reasons.erase(_reasons.find(variable));

    return literal;
}


}  // namespace cosy
#endif  // INCLUDE_COSY_TRAIL_H_
