// Copyright 2017 Hakan Metin - LIP6

#include "cosy/CosyStatus.h"

namespace cosy {

CosyStatus::CosyStatus(const Permutation &permutation, const Order &order,
                       const Assignment& assignment) :
    _permutation(permutation),
    _order(order),
    _assignment(assignment),
    _lookup_index(0),
    _state(ACTIVE),
    _generated(false),
    _statesp(SP_REQUIRE),
    _countersp(0),
    _desactivesp(kNoLiteralIndex) {
}

CosyStatus::~CosyStatus() {
}

void CosyStatus::addLookupLiteral(const Literal& literal) {
    _lookup_order.push_back(literal);
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


void CosyStatus::updateNotifySP(const Literal& literal) {
    DCHECK_NE(_statesp, SP_INACTIVE);

    const Literal inverse = _permutation.inverseOf(literal);
    const Literal image = _permutation.imageOf(literal);

    _fals.push_back(literal);

    if (_assignment.isDecision(literal)) {
        if (!_assignment.literalIsAssigned(image)) {
            _countersp++;
        } else if (_assignment.literalIsFalse(image)) {
            _desactivesp = literal.index();
            _statesp = SP_INACTIVE;
            return;
        }
    }

    // else {
    //     // can be simplified as !_assignment.literalIsTrue(inverse)
    //     if (!_assignment.literalIsAssigned(inverse)) {
    //         _fals.push_back(literal);
    //     } else if (_assignment.literalIsFalse(inverse)) {
    //         _fals.push_back(literal);  // SP conflict
    //     }
    // }


    if (_assignment.isDecision(inverse)) {
        if (_assignment.literalIsTrue(inverse)) {
            _countersp--;
        } else {
            DCHECK(_assignment.literalIsFalse(inverse));
            _desactivesp = literal.index();
            _statesp = SP_INACTIVE;
            return;
        }
    }
    // else {
    //     while (_fals.size() > 0) {
    //         Literal fal = _fals.front();
    //         Literal img = _permutation.imageOf(fal);
    //         if (_assignment.literalIsTrue(img))
    //             _fals.pop_front();
    //         break;
    //     }
    // }

    if (_countersp == 0)
        _statesp = SP_ACTIVE;
    else
        _statesp = SP_REQUIRE;

}

void CosyStatus::updateCancelSP(const Literal& literal) {
    if (_statesp == SP_INACTIVE && literal.index() != _desactivesp)
        return;

    _desactivesp = kNoLiteralIndex;
    CHECK_GT(_fals.size(), 0);
    if (_fals.back() != literal)
        return;

    _fals.pop_back();

    // // // update FAL
    // if (_fals.size() > 0) {
    //     Literal back = _fals.back();
    //     if (literal == back)
    //         _fals.pop_back();
    // }

    const Literal inverse = _permutation.inverseOf(literal);
    const Literal image = _permutation.imageOf(literal);

    if (_assignment.isDecision(literal) && !_assignment.literalIsAssigned(image))
        _countersp--;
    if (_assignment.isDecision(inverse) && _assignment.literalIsTrue(inverse))
        _countersp++;


    if (_countersp == 0)
        _statesp = SP_ACTIVE;
    else
        _statesp = SP_REQUIRE;
}

bool CosyStatus::registerSP(ClauseInjector *injector) {
    unsigned int i = 0;

    while (i < _fals.size() &&
           (_assignment.isDecision(_fals[i]) ||
            _assignment.literalIsTrue(_permutation.imageOf(_fals[i]))))
        i++;

    if (i == _fals.size()) {
        _statesp = SP_REQUIRE;
        return false;
    }

    std::vector<Literal> propagate_literal = { _fals[i] };
    injector->addClause(ClauseInjector::Type::SP, kNoBooleanVariable,
                        std::move(propagate_literal));
    return true;
}


void CosyStatus::computeSymmetricalClause(const std::vector<Literal> &src,
                                          std::vector<Literal> *dst) {
    for (const Literal& l : src) {
        if (_permutation.isTrivialImage(l))
            dst->push_back(l);
        else
            dst->push_back(_permutation.imageOf(l));
    }
}


void CosyStatus::updateNotify(const Literal& literal) {
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
    updateState();
}

void CosyStatus::updateCancel(const Literal& literal) {
    _state = ACTIVE;
    if (_lookup_infos.empty())
        return;

    const LookupInfo& lookup = _lookup_infos.back();
    if (lookup.variable != literal.variable())
        return;

    _lookup_index = lookup.back_index;
    _lookup_infos.pop_back();
}



void CosyStatus::updateState() {
    Literal element, inverse;

    if (isLookupEnd()) {
        _state = INACTIVE;
        return;
    }

    element = _lookup_order[_lookup_index];
    inverse = _permutation.inverseOf(element);

    const Literal minimal = _order.leq(element, inverse);
    const Literal maximal = minimal == element ? inverse : element;

    // const Literal minimal = element;
    // const Literal maximal = inverse;

    if (_assignment.bothLiteralsAreAssigned(element, inverse)) {
        DCHECK(!_assignment.hasSameAssignmentValue(element, inverse));
        if (_order.isMinimalValue(minimal, _assignment))
            _state = INACTIVE;
        else
            _state = REDUCER;
    } else {
        // force lex leader case
        // F < T : U <- F or T -> U
        // T < F : U <- T or F -> U
        if ((!_assignment.literalIsAssigned(minimal) &&
            _order.isMinimalValue(maximal, _assignment)) ||
            (!_assignment.literalIsAssigned(maximal) &&
             _order.isMaximalValue(minimal, _assignment)))
            _state = FORCE_LEX_LEADER;
        else
            _state = ACTIVE;
    }
}



void CosyStatus::updateStatic() {
    Literal element, image;
    for (; _lookup_index < _lookup_order.size(); ++_lookup_index) {
        element = _lookup_order[_lookup_index];
        image = _permutation.imageOf(element);

        if (!_assignment.hasSameAssignmentValue(element, image))
            break;
        _generated = false;
    }

    updateState();
}

void CosyStatus::generateStaticESBP(ClauseInjector *injector) {
    std::vector<Literal> literals;
    std::unordered_set<Literal> used;
    Literal element, inverse, l, negated;

    updateStatic();
    if (_state == INACTIVE || _generated || isLookupEnd())
        return;

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

    element = _lookup_order[_lookup_index];
    negated = element.negated();

    inverse = _permutation.inverseOf(negated);

    while (inverse != negated) {
        std::vector<Literal> esbp;

        esbp.push_back(element);
        esbp.push_back(inverse);
        for (Literal lit : literals)
            esbp.push_back(lit);
        injector->addClause(ClauseInjector::Type::ESBP, kNoBooleanVariable,
                            std::move(esbp));

        inverse = _permutation.inverseOf(inverse);
    }
    _generated = true;
}


void
CosyStatus::generateESBP(BooleanVariable reason, ClauseInjector *injector) {
    std::vector<Literal> literals;
    std::unordered_set<Literal> used;
    Literal element, inverse, l;

    DCHECK(!isLookupEnd());
    DCHECK_EQ(_state, REDUCER);

    l = _assignment.getFalseLiteralForAssignedVariable(reason);
    if (used.insert(l).second)
        literals.push_back(l);

    for (unsigned int i = 0; i <= _lookup_index; i++) {
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

    DCHECK_GE(literals.size(), 2);
    std::swap(literals[0], literals[1]);

    // LOG(INFO) << "reaseon : " << reason;
    // for (Literal lit : literals)
    //     std::cout << lit.signedValue() << " " << _assignment.debugStringValue(lit)<< " -- ";
    // std::cout << std::endl;

    injector->addClause(ClauseInjector::Type::ESBP, reason,
                        std::move(literals));
}

void CosyStatus::generateForceLexLeaderESBP(ClauseInjector *injector) {
    std::vector<Literal> literals;
    std::unordered_set<Literal> used;
    Literal element, inverse, l, affected, negated;

    DCHECK(!isLookupEnd());

    element = _lookup_order[_lookup_index];
    negated = element.negated();
    inverse = _permutation.inverseOf(negated);

    if (_assignment.literalIsAssigned(element)) {
        DCHECK(!_assignment.literalIsAssigned(inverse));
        literals.push_back(inverse);
        literals.push_back(element);
    } else {
        DCHECK(!_assignment.literalIsAssigned(element));
        literals.push_back(element);
        literals.push_back(inverse);
    }

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


    // LOG(INFO) << debugString();
    // for (Literal lit : literals)
    //     std::cout << lit.signedValue() << " ";
    // std::cout << std::endl;
    // for (Literal lit : literals)
    //     std::cout << _assignment.debugStringValue(lit)<< " -- ";
    // std::cout << std::endl;

    injector->addClause(ClauseInjector::Type::ESBP_FORCING,
                        kNoBooleanVariable, std::move(literals));
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
    return str;
}


}  // namespace cosy
