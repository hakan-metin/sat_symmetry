// Copyright 2017 Hakan Metin - LIP6

#include "cosy/SPFSManager.h"

namespace cosy {

SPFSManager::SPFSManager(const Group& group, const Trail& trail) :
    _group(group),
    _assignment(trail.assignment()),
    _trail(trail) {
    for (const std::unique_ptr<Permutation>& perm : _group.permutations()) {
        std::unique_ptr<SPFSStatus> status
            (new SPFSStatus(*perm, _trail));
        _statuses.emplace_back(status.release());
    }
}

SPFSManager::~SPFSManager() {
}

void SPFSManager::updateNotify(const Literal& literal) {
    ScopedTimeDistributionUpdater time(&(_stats.total_time));
    time.alsoUpdate(&(_stats.notify_time));

    const BooleanVariable variable = literal.variable();
    for (const unsigned int& index : _group.watch(variable)) {
        const std::unique_ptr<SPFSStatus>& status = _statuses[index];
        status->updateNotify(literal);
    }
}

void SPFSManager::updateCancel(const Literal& literal) {
    ScopedTimeDistributionUpdater time(&_stats.total_time);
    time.alsoUpdate(&_stats.cancel_time);

    const BooleanVariable variable = literal.variable();
    for (const unsigned int& index : _group.watch(variable)) {
        const std::unique_ptr<SPFSStatus>& status = _statuses[index];
        status->updateCancel(literal);
    }
}

void SPFSManager::searchSymmetricalClause(ClauseInjector *injector) {
    for (const std::unique_ptr<SPFSStatus>& status : _statuses)
        if (status->isWeaklyActive()) {
            status->generateSPFS(kNoBooleanVariable, injector);
            break;
        }
}

}  // namespace cosy
