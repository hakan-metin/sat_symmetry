// Copyright 2017 Hakan Metin - LIP6

#include "cosy/Stats.h"

namespace cosy {

Stat::Stat(const std::string& name) : _name(name) {
}

Stat::Stat(const std::string& name, StatsGroup *group) : _name(name) {
    group->registerStat(this);
}

void StatsGroup::print(bool section /* = false */) const {
    if (section)
        Printer::printSection(_name.c_str());
    for (const Stat* stat : _stats)
        stat->print();
}


}  // namespace cosy
