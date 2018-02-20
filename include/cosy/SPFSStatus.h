// Copyright 2017 Hakan Metin - LIP6

#ifndef INCLUDE_COSY_SPFSSTATUS_H_
#define INCLUDE_COSY_SPFSSTATUS_H_

#include <deque>

#include "cosy/Assignment.h"
#include "cosy/Clause.h"
#include "cosy/ClauseInjector.h"
#include "cosy/Group.h"
#include "cosy/Literal.h"
#include "cosy/Logging.h"
#include "cosy/Permutation.h"
#include "cosy/Stats.h"
#include "cosy/Trail.h"

namespace cosy {

class SPFSStatus {
 public:
    SPFSStatus(const Permutation &permutation, const Trail& trail);
    ~SPFSStatus();

    void updateNotify(const Literal& literal);
    void updateCancel(const Literal& literal);

    bool isWeaklyActive() const;
    LiteralIndex getFirstAsymetricLiteral();
    void generateSPFS(Literal literal, ClauseInjector *injector);

 private:
    const Permutation& _permutation;
    const Assignment& _assignment;
    const Trail& _trail;

    std::deque<Literal> _notified;
    unsigned int _lookup_index;

    LiteralIndex _reasonOfInactive;
    int _amountForActive;


    void sortClause(std::vector<Literal>& clause);

};

}  // namespace cosy

#endif  // INCLUDE_COSY_SPFSSTATUS_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
