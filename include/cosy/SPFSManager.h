// Copyright 2017 Hakan Metin - LIP6

#ifndef INCLUDE_COSY_SPFSMANAGER_H_
#define INCLUDE_COSY_SPFSMANAGER_H_

#include <memory>
#include <vector>

#include "cosy/Assignment.h"
#include "cosy/Clause.h"
#include "cosy/ClauseInjector.h"
#include "cosy/Group.h"
#include "cosy/Literal.h"
#include "cosy/Logging.h"
#include "cosy/Permutation.h"
#include "cosy/Stats.h"
#include "cosy/SPFSStatus.h"
#include "cosy/Trail.h"

namespace cosy {

class SPFSManager {
 public:
    SPFSManager(const Group& group, const Trail& trail);
    ~SPFSManager();

    void updateNotify(const Literal& literal);
    void updateCancel(const Literal& literal);

    void generateClauses(ClauseInjector *injector);

    void printStats() const { Printer::print(""); _stats.print(); }

 private:
    const Group& _group;
    const Assignment& _assignment;
    const Trail& _trail;

    std::vector< std::unique_ptr<SPFSStatus> > _statuses;

    struct Stats : public StatsGroup {
        Stats() : StatsGroup("SPFS Manager"),
                  total_time("SPFS total time", this),
                  notify_time(" |- notify time", this),
                  cancel_time(" |- cancel time", this)
        {}
        TimeDistribution total_time;
        TimeDistribution notify_time;
        TimeDistribution cancel_time;
    };
    Stats _stats;

    DISALLOW_COPY_AND_ASSIGN(SPFSManager);
};

}  // namespace cosy
#endif  // INCLUDE_COSY_SPFSMANAGER_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
