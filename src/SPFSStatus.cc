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
    const Literal image = _permutation.imageOf(literal);
    const Literal inverse = _permutation.inverseOf(literal);

    if (_trail.isDecision(literal) && !_assignment.literalIsAssigned(image))
        --_amountForActive;

    if (_trail.isDecision(inverse) && _assignment.literalIsTrue(inverse))
        ++_amountForActive;
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

}  // namespace cosy
