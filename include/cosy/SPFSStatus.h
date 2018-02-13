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
#include "cosy/SPFSStatus.h"

namespace cosy {

class SPFSStatus {
 public:
    SPFSStatus(const Permutation &permutation, const Assignment& assignment);
    ~SPFSStatus();

    void updateNotify(const Literal& literal);
    void updateCancel(const Literal& literal);

 private:
    const Permutation& _permutation;
    const Assignment& _assignment;

    std::deque<Literal> _notified;
    unsigned int _lookup_index;

    LiteralIndex _reasonOfInactive;
    int _amountForActive;
};

}  // namespace cosy

#endif  // INCLUDE_COSY_SPFSSTATUS_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
