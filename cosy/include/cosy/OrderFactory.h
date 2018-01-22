// Copyright 2017 Hakan Metin - LIP6

#ifndef INCLUDE_COSY_ORDERFACTORY_H_
#define INCLUDE_COSY_ORDERFACTORY_H_

#include "cosy/Order.h"
#include "cosy/CNFModelStats.h"
#include "cosy/Group.h"

namespace cosy {

class OrderFactory {
 public:
    static Order* create(OrderMode vars, ValueMode value,
                         const CNFModelStats& model, const Group& group);
};

inline Order* OrderFactory::create(OrderMode vars, ValueMode value,
                            const CNFModelStats& cnf_model_stats,
                            const Group& group) {

    const unsigned int num_vars = cnf_model_stats.numberOfVariables();
    Order *order = nullptr;
    switch (vars) {
    case INCREASE:
        order = new IncreaseOrder(num_vars, value);
        break;
    case OCCURENCE:
        order = new OccurenceOrder(num_vars, value, cnf_model_stats);
        break;
    case BREAKID:
        order = new BreakIDOrder(num_vars, value, group);
        break;
    case AUTO:
        if (static_cast<double>(group.numberOfInverting()) / num_vars > 0.25)
            order = new BreakIDOrder(num_vars, value, group);
        else
            order = new OccurenceOrder(num_vars, value, cnf_model_stats);
        break;
    default:
        order = nullptr;
    }

    return order;
}

}  // namespace cosy

#endif  // INCLUDE_COSY_ORDERFACTORY_H_
