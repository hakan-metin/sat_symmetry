// Copyright 2017 Hakan Metin - LIP6

#include "cosy/CosyStatus.h"

namespace cosy {

CosyStatus::CosyStatus(const Permutation &permutation, const Order &order,
                       const Assignment& assignment) :
    _permutation(permutation),
    _order(order),
    _assignment(assignment),
    _lookup_index(0),
    _state(ACTIVE) {
    _keep.resize(permutation.numberOfCycles());
}

CosyStatus::~CosyStatus() {
}

void CosyStatus::addLookupLiteral(const Literal& literal) {
    _lookup_order.push_back(literal);

    Literal l = literal;
    if (_order.valueMode() == FALSE_LESS_TRUE)
        l = l.negated();

    unsigned int cycle = _permutation.literalInCycle(l);
    if (!_keep[cycle].empty())
        return;

    // Add minimal
    _keep[cycle].insert(l);

    // Add complement to the negative one
    Literal negated = l.negated();
    Literal image = _permutation.imageOf(negated);
    unsigned neg_cycle = _permutation.literalInCycle(negated);
    while (image != negated) {
        _keep[neg_cycle].insert(image);
        image = _permutation.imageOf(image);
    }
}

void CosyStatus::generateUnitClauseOnInverting(ClauseInjector *injector) {
    if (isLookupEnd())
        return;

    const Literal element = _lookup_order[_lookup_index];
    const Literal inverse = _permutation.inverseOf(element);

    if (element != inverse.negated())
        return;

    BooleanVariable variable = element.variable();
    Literal unit = Literal(variable, _order.valueMode() == TRUE_LESS_FALSE);

    std::vector<Literal> literals = { unit };

    injector->addClause(ClauseInjector::Type::UNITS, kNoBooleanVariable,
                        std::move(literals));
}

bool CosyStatus::lookAhead(const Literal& literal, ClauseInjector *injector) {
    Literal element, inverse;
    unsigned int i;
    CosyState state;

    for (i = _lookup_index; i < _lookup_order.size(); i++) {
        element = _lookup_order[i];
        inverse = _permutation.inverseOf(element);

        const Literal minimal = _order.leq(element, inverse);
        const Literal maximal = minimal == element ? inverse : element;
        DCHECK_NE(minimal, maximal);

        if (!_assignment.literalIsAssigned(maximal) &&
        _order.isMaximalValue(minimal, _assignment))
            continue;

        if (!_assignment.literalIsAssigned(maximal) &&
        _order.isMaximalValue(minimal, _assignment))
            continue;

        break;
    }

    state = stateInIndex(i);
    if (state == REDUCER) {
        _state = REDUCER;
        generateForceLexLeaderESBPAtIndex(i, literal.variable(), injector);
        return true;
    }
    return false;
}

bool CosyStatus::updateNotify(const Literal& literal, ClauseInjector *injector) {
    unsigned int initial = _lookup_index;
    Literal element, inverse;
    const BooleanVariable variable = literal.variable();

    for (; _lookup_index < _lookup_order.size(); ++_lookup_index) {
        element = _lookup_order[_lookup_index];
        inverse = _permutation.inverseOf(element);

        if (!_assignment.hasSameAssignmentValue(element, inverse))
            break;

        if (_lookup_index == initial)
            _lookup_infos.push_back(LookupInfo(variable, _lookup_index));
    }
    if (lookAhead(literal, injector))
        return true;

    updateState();
    if (_state == REDUCER) {
        generateForceLexLeaderESBP(literal.variable(), injector);
        return true;
    }

    return false;
}

void CosyStatus::updateCancel(const Literal& literal) {
    if (_lookup_infos.empty())
        return;

    const LookupInfo& lookup = _lookup_infos.back();
    if (lookup.variable != literal.variable())
        return;

    _lookup_index = lookup.back_index;
    _lookup_infos.pop_back();
}

CosyState CosyStatus::stateInIndex(unsigned int index) {
    if (index >= _lookup_order.size())
        return ACTIVE;

    const Literal element = _lookup_order[_lookup_index];
    const Literal inverse = _permutation.inverseOf(element);

    const Literal minimal = _order.leq(element, inverse);
    const Literal maximal = minimal == element ? inverse : element;

    if (_assignment.bothLiteralsAreAssigned(element, inverse)) {
        DCHECK(!_assignment.hasSameAssignmentValue(element, inverse));
        if (_order.isMinimalValue(minimal, _assignment))
            return INACTIVE;
        else
            return REDUCER;
    } else {
        // force lex leader case
        // F < T : U <- F or T -> U
        // T < F : U <- T or F -> U
        if ((!_assignment.literalIsAssigned(minimal) &&
             _order.isMinimalValue(maximal, _assignment)) ||
            (!_assignment.literalIsAssigned(maximal) &&
             _order.isMaximalValue(minimal, _assignment)))
            return FORCE_LEX_LEADER;
    }

    return ACTIVE;
}


void CosyStatus::updateState() {
    _state = stateInIndex(_lookup_index);
}

void CosyStatus::generateForceLexLeaderESBPAtIndex(unsigned int index,
                                                   BooleanVariable reason,
                                                   ClauseInjector *injector) {
    std::vector<Literal> literals;
    std::unordered_set<Literal> used;
    Literal element, inverse, l;
    BooleanVariable var;

    if (index >= _lookup_order.size())
        return;

    DCHECK_EQ(_state, REDUCER);

    l = _assignment.getFalseLiteralForAssignedVariable(reason);
    if (used.insert(l).second && isInESBP(l))
        literals.push_back(l);

    for (unsigned int i = 0; i <= index; i++) {
        element = _lookup_order[i];
        inverse = _permutation.inverseOf(element);

        // DCHECK(_assignment.bothLiteralsAreAssigned(element, inverse));
        if (_assignment.literalIsAssigned(element)) {
            var = element.variable();
            l = _assignment.getFalseLiteralForAssignedVariable(var);
            if (used.insert(l).second && isInESBP(l))
                literals.push_back(l);
        }

        if (_assignment.literalIsAssigned(inverse)) {
            var = inverse.variable();
            l = _assignment.getFalseLiteralForAssignedVariable(var);
            if (used.insert(l).second && isInESBP(l))
                literals.push_back(l);
        }
    }

    DCHECK_GE(literals.size(), 2);
    std::swap(literals[0], literals[1]);

    injector->addClause(ClauseInjector::Type::ESBP, reason,
                        std::move(literals));
}

void
CosyStatus::generateESBP(BooleanVariable reason, ClauseInjector *injector) {
    generateForceLexLeaderESBPAtIndex(_lookup_index, reason, injector);
}


void CosyStatus::generateForceLexLeaderESBP(BooleanVariable reason,
                                            ClauseInjector *injector) {
    std::vector<Literal> literals;
    std::unordered_set<Literal> used;
    Literal element, inverse, l, affected, undef;

    DCHECK(!isLookupEnd());

    element = _lookup_order[_lookup_index];
    inverse = _permutation.inverseOf(element);

    undef = _assignment.literalIsAssigned(element) ? inverse : element;
    affected = _assignment.literalIsAssigned(inverse) ? inverse : element;

    l = Literal(undef.variable(), _assignment.literalIsTrue(affected));
    if (used.insert(l).second)
        literals.push_back(l);

    l = _assignment.getFalseLiteralForAssignedVariable(reason);
    if (used.insert(l).second)
        literals.push_back(l);

    l = _assignment.getFalseLiteralForAssignedVariable(affected.variable());
    if (used.insert(l).second)
        literals.push_back(l);

    for (unsigned int i = 0; i < _lookup_index; i++) {
        element = _lookup_order[i];
        inverse = _permutation.inverseOf(element);

        DCHECK(_assignment.bothLiteralsAreAssigned(element, inverse));

        l = _assignment.getFalseLiteralForAssignedVariable(element.variable());
        if (used.insert(l).second)
            literals.push_back(l);

        l = _assignment.getFalseLiteralForAssignedVariable(inverse.variable());
        if (used.insert(l).second)
            literals.push_back(l);
    }

    injector->addClause(ClauseInjector::Type::ESBP_FORCING,
                        reason, std::move(literals));
}

std::string CosyStatus::debugString() const {
    Literal element, inverse;
    std::string str;


    if (isLookupEnd())
        return std::string("empty order");

    element = _lookup_order[_lookup_index];
    inverse = _permutation.inverseOf(element);

    for (const Literal& literal : _lookup_order) {
        if (literal == element)
            str += "[" + std::to_string(literal.signedValue()) + "] ";
        else if (literal == inverse)
            str += "(" + std::to_string(literal.signedValue()) + ") ";
        else
            str += std::to_string(literal.signedValue()) + " ";
    }

    // _permutation.debugPrint();
    // int c = 0;

    // for (auto lits : _keep) {
    //     std::cout << c << ":";
    //     for (auto lit : lits) {
    //         std::cout << lit.signedValue() << " ";
    //     }
    //     c++;
    //     std::cout << std::endl;
    // }
    //     std::cout << std::endl;

    return str;
}


 bool CosyStatus::isInESBP(Literal l) const {
    unsigned int cycle = _permutation.literalInCycle(l);
    return _keep[cycle].find(l) != _keep[cycle].end();
}


}  // namespace cosy
