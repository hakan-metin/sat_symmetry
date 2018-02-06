// Copyright 2017 Hakan Metin - LIP6

#include "cosy/ClauseInjector.h"

namespace cosy {

ClauseInjector::ClauseInjector() {
    _injectors.resize(std::numeric_limits<Type>::max());

}

ClauseInjector::~ClauseInjector() {
}


void ClauseInjector::addClause(Type type, BooleanVariable cause,
                               std::vector<Literal>&& literals) {
    _injectors[type].addClause(cause, std::move(literals));
}

void ClauseInjector::removeClause(Type type, BooleanVariable cause) {
    _injectors[type].removeClause(cause);
}

bool ClauseInjector::hasClause(Type type, BooleanVariable cause) const {
    return _injectors[type].hasClause(cause);
}

std::vector<Literal>
ClauseInjector::getClause(Type type, BooleanVariable cause) {
    return _injectors[type].getClause(cause);
}

void ClauseInjector::removeClause(BooleanVariable cause) {
    for (Injector& injector : _injectors)
        injector.removeClause(cause);
}





// bool ClauseInjector::addConflictClause(BooleanVariable cause,
//                                        const std::vector<Literal>& literals) {
//     _conflicts.push_back(ClauseInfos(cause, literals));

//     return true;
// }

// bool ClauseInjector::isConflictClause(BooleanVariable cause) const {
//     if (_conflicts.empty())
//         return false;
//     return _conflicts.back().cause == cause;
// }

// std::vector<Literal> ClauseInjector::conflictClause() {
//     DCHECK_GT(_conflicts.size(), 0);
//     std::vector<Literal> clause(std::move(_conflicts.back().literals));
//     _conflicts.pop_back();
//     _stats.conflicts.increment();
//     return clause;
// }

// void ClauseInjector::removeConflictClause(BooleanVariable cause) {
//     while (_conflicts.size() > 0) {
//         if (_conflicts.back().cause != cause)
//             return;
//         _conflicts.pop_back();
//     }
// }

// void ClauseInjector::addUnitClause(const Literal& literal) {
//     _units.insert(literal);
// }
// bool ClauseInjector::isUnitClause() const {
//     return _units.size() > 0;
// }
// Literal ClauseInjector::unitClause() {
//     Literal literal = *(_units.begin());
//     _units.erase(_units.begin());
//     _stats.units.increment();

//     return literal;
// }


// bool ClauseInjector::addAssertiveClause(BooleanVariable cause,
//                                         const std::vector<Literal>& literals) {
//     LOG(INFO) << literals.size() << cause;
//     return false;
// }


}  // namespace cosy

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
