// Copyright 2017 Hakan Metin - LIP6

#include "cosy/SPFSManager.h"

namespace cosy {

SPFSManager::SPFSManager(const Group& group, const Assignment& assignment) :
    _group(group),
    _assignment(assignment) {
    for (const std::unique_ptr<Permutation>& perm : _group.permutations()) {
        std::unique_ptr<SPFSStatus> status
            (new SPFSStatus(*perm, _assignment));
        _statuses.emplace_back(status.release());
    }
}

SPFSManager::~SPFSManager() {
}


void SPFSManager::updateNotify(const Literal& literal,
                               ClauseInjector *injector) {
    IF_STATS_ENABLED({
            ScopedTimeDistributionUpdater time(&_stats.total_time);
            time.alsoUpdate(&_stats.notify_time);
        });
}

void SPFSManager::updateCancel(const Literal& literal) {
    IF_STATS_ENABLED({
            ScopedTimeDistributionUpdater time(&_stats.total_time);
            time.alsoUpdate(&_stats.cancel_time);
        });
}

}  // namespace cosy
