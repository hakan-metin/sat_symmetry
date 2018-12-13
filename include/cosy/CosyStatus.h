// Copyright 2017 Hakan Metin - LIP6

#ifndef INCLUDE_COSY_COSYSTATUS_H_
#define INCLUDE_COSY_COSYSTATUS_H_

#include <deque>
#include <vector>
#include <unordered_set>
#include <string>

#include "cosy/Assignment.h"
#include "cosy/ClauseInjector.h"
#include "cosy/Literal.h"
#include "cosy/Order.h"
#include "cosy/Permutation.h"
#include "cosy/Logging.h"

namespace cosy {

enum CosyState {
    INACTIVE,
    ACTIVE,
    REDUCER,
    FORCE_LEX_LEADER,
};

enum StateSP {
    SP_ACTIVE,
    SP_INACTIVE,
    SP_REQUIRE,
};

class CosyStatus {
 public:
    CosyStatus(const Permutation &permutation, const Order &order,
               const Assignment& assignment);
    ~CosyStatus();

    // ESBP

    void addLookupLiteral(const Literal& literal);

    void updateNotify(const Literal& literal);
    void updateCancel(const Literal& literal);
    CosyState state() const { return _state; }

    void generateUnitClauseOnInverting(ClauseInjector *injector);
    void generateESBP(BooleanVariable reason, ClauseInjector *injector);
    void generateStaticESBP(ClauseInjector *injector);
    void generateForceLexLeaderESBP(ClauseInjector *injector);


    // SP

    void updateNotifySP(const Literal& literal);
    void updateCancelSP(const Literal& literal);
    bool registerSP(ClauseInjector *injector);
    void computeSymmetricalClause(const std::vector<Literal> &src,
                                  std::vector<Literal> *dst);


    StateSP stateSP() const { return _statesp; }

    std::string debugString() const;

 private:
    const Permutation& _permutation;
    const Order& _order;
    const Assignment& _assignment;

    unsigned int _lookup_index;
    std::vector<Literal> _lookup_order;

    struct LookupInfo {
        LookupInfo(BooleanVariable v, unsigned int bi) :
            variable(v), back_index(bi) {}
        BooleanVariable variable;
        unsigned int back_index;
    };

    std::deque<LookupInfo> _lookup_infos;
    CosyState _state;
    bool _generated;

    // SP
    StateSP _statesp;
    unsigned int _countersp;
    std::deque<Literal> _fals;  // first asymmetric literals
    LiteralIndex _desactivesp;


    void updateStatic();
    bool isLookupEnd() const { return _lookup_index >= _lookup_order.size(); }
    void updateState();

    DISALLOW_COPY_AND_ASSIGN(CosyStatus);
};

}  // namespace cosy

#endif  // INCLUDE_COSY_COSYSTATUS_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
