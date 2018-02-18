// Copyright 2017 Hakan Metin - LIP6

#include "cosy/SPFSStatus.h"

namespace cosy {


SPFSStatus::SPFSStatus(const Permutation &permutation,
                       const Trail &trail) :
    _permutation(permutation),
    _assignment(trail.assignment()),
    _trail(trail),
    _lookup_index(0),
    _reasonOfInactive(kNoLiteralIndex),
    _firstAsymetricLiteral(kNoLiteralIndex),
    _amountForActive(0) {
}

SPFSStatus::~SPFSStatus() {
}


void SPFSStatus::updateNotify(const Literal& literal) {
    DCHECK(!_permutation.isTrivialImage(literal));
    DCHECK(_assignment.literalIsTrue(literal));

    const Literal image = _permutation.imageOf(literal);
    const Literal inverse = _permutation.inverseOf(literal);

    _notified.push_back(literal);

    const bool isInactive = _reasonOfInactive != kNoLiteralIndex;
    if (isInactive)
        return;

    if (_trail.isDecision(inverse)) {
        if (_assignment.literalIsTrue(inverse)) {
            --_amountForActive;
        } else {
            DCHECK(_assignment.literalIsFalse(inverse));
            _reasonOfInactive = literal.index();
        }
    }

    if (_trail.isDecision(literal)) {
        if (!_assignment.literalIsAssigned(image)) {
            ++_amountForActive;
        } else if (_assignment.literalIsFalse(image)) {
            _reasonOfInactive = literal.index();
        }
    }
}

void SPFSStatus::updateCancel(const Literal& literal) {
    DCHECK(!_permutation.isTrivialImage(literal));
    DCHECK(_assignment.literalIsAssigned(literal));
    DCHECK(_notified.size() > 0 && _notified.back() == literal);

    _notified.pop_back();
    _lookup_index = 0;

    const bool isInactive = _reasonOfInactive != kNoLiteralIndex;
    if (isInactive && _reasonOfInactive != literal.index())
        return;

    _reasonOfInactive = kNoLiteralIndex;
    _firstAsymetricLiteral = kNoLiteralIndex;

    const Literal image = _permutation.imageOf(literal);
    const Literal inverse = _permutation.inverseOf(literal);

    if (_trail.isDecision(literal) && !_assignment.literalIsAssigned(image))
        --_amountForActive;

    if (_trail.isDecision(inverse) && _assignment.literalIsTrue(inverse))
        ++_amountForActive;
}

bool SPFSStatus::isWeaklyActive() {
    _firstAsymetricLiteral = getFirstAsymetricLiteral();
    return  _firstAsymetricLiteral != kNoLiteralIndex;
}


LiteralIndex SPFSStatus::getFirstAsymetricLiteral() {
    if (!( _amountForActive == 0 &&  _reasonOfInactive == kNoLiteralIndex))
        return kNoLiteralIndex;

    Literal literal;
    for (; _lookup_index < _notified.size(); ++_lookup_index) {
        literal = _notified[_lookup_index];
        const Literal image = _permutation.imageOf(literal);
        if (!(_trail.isDecision(literal) || _assignment.literalIsTrue(image)))
            break;
    }

    if (_lookup_index == _notified.size())
        return kNoLiteralIndex;

    DCHECK(!_assignment.literalIsTrue(_permutation.imageOf(literal)));
    return literal.index();
}

/* Sort rules:
 * 1) Undef variable first in any order
 * 2) sorted by decreasing level
 * and lexicographic order if same level
 */
struct ClauseLt {
    explicit ClauseLt(const Trail& t) : trail(t) {}
    bool operator()(const Literal& a, const Literal b) {
        const Assignment& assignment = trail.assignment();
        if (assignment.bothLiteralsAreAssigned(a, b)) {
            if (trail.level(a) == trail.level(b))
                return a < b;
            return trail.level(a) > trail.level(b);
        }
        return !assignment.literalIsAssigned(a);
    }
    const Trail& trail;
};

void SPFSStatus::generateSPFS(BooleanVariable cause,
                              ClauseInjector *injector) {
    std::vector<Literal> literals;

    DCHECK(_firstAsymetricLiteral != kNoLiteralIndex);

    const Literal literal = Literal(_firstAsymetricLiteral);
    // const BooleanVariable variable = literal.variable();

    if (_trail.level(literal) == 0) {
        literals.push_back(_permutation.imageOf(literal));
        literals.push_back(literal.negated());
    } else {
        const Reason& reason = _trail.reason(literal);
        CHECK_NE(reason, kNoReason);
        for (const Literal& l : reason) {
            if (_permutation.isTrivialImage(l))
                literals.push_back(l);
            else
                literals.push_back(_permutation.imageOf(l));
        }
        sortClause(literals);
        // std::sort(literals.begin(), literals.end(), ClauseLt(_trail));
    }

    std::string c = "";

    for (auto& l : literals)
        c += std::to_string(l.signedValue()) +
            "[" + (_assignment.literalIsAssigned(l)
                   ? std::to_string(_trail.level(l)) : "") + "]" +
            "(" + (_assignment.literalIsTrue(l) ? "T" :
            _assignment.literalIsFalse(l) ? "F" : "U") + ") " ;

    LOG(INFO) << "CAUSE: " << cause << " ====== " << c;

    injector->addClause(ClauseInjector::Type::SPFS,
                        cause, std::move(literals));
}

void SPFSStatus::sortClause(std::vector<Literal>& clause) {
    int first = 0;
    int second = 1;
    for(int i=1; i<clause.size(); ++i) {
        CHECK(!_assignment.literalIsTrue(clause[i]));
        if (_assignment.literalIsAssigned(clause[first]) &&
            (!_assignment.literalIsAssigned(clause[i]) ||
             _trail.level(clause[first]) < _trail.level(clause[i]))) {
            second = first;
            first = i;
        } else if (_assignment.literalIsAssigned(clause[second]) &&
                   (!_assignment.literalIsAssigned(clause[i]) ||
                    _trail.level(clause[second]) < _trail.level(clause[i]))) {
            second = i;
        }
    }
       if (first != 0) {
        Literal temp = clause[0];
        clause[0] = clause[first];
        clause[first] = temp;
    }
    assert(second != first);
    if (second == 0) {
        second = first;
    }
    if (second != 1) {
        Literal temp = clause[1];
        clause[1] = clause[second];
        clause[second] = temp;
    }
}



}  // namespace cosy
