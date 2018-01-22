// Copyright 2017 Hakan Metin - LIP6

#ifndef INCLUDE_COSY_STATS_H_
#define INCLUDE_COSY_STATS_H_

#include <utility>
#include <deque>
#include <vector>
#include <string>

#include "cosy/IntegralTypes.h"
#include "cosy/Macros.h"
#include "cosy/Printer.h"

namespace cosy {

// Forward Declaration
class StatsGroup;

class Stat {
 public:
    Stat(const std::string& name);
    Stat(const std::string& name, StatsGroup *group);
    virtual ~Stat() {}

    std::string name() const { return _name;}
    virtual std::string valueString() const = 0;

    void print() const {
        Printer::printStat(_name, valueString());
    }
 private:
    std::string _name;
};


class StatsGroup {
 public:
    explicit StatsGroup(const std::string& name) : _name(name), _stats() {}
    ~StatsGroup() {}

    void registerStat(Stat *stat) { _stats.push_back(stat); }
    void reset();

    void print(bool section = false ) const;
 private:
    std::string _name;
    std::vector<Stat*> _stats;

    DISALLOW_COPY_AND_ASSIGN(StatsGroup);
};


class CounterStat : public Stat {
 public:
    explicit CounterStat(const std::string& name) : Stat(name), _value(0) {}
    CounterStat(const std::string& name, StatsGroup *group) :
        Stat(name, group), _value(0) {}
    ~CounterStat() {}

    void increment() { _value++; }
    virtual std::string valueString() const { return std::to_string(_value); }
 private:
    int64 _value;
};

}  // namespace cosy

#endif  // INCLUDE_COSY_STATS_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
