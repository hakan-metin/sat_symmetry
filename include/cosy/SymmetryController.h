// Copyright 2017 Hakan Metin - LIP6

#ifndef INCLUDE_COSY_SYMMETRYCONTROLLER_H_
#define INCLUDE_COSY_SYMMETRYCONTROLLER_H_

#include <memory>
#include <utility>
#include <vector>
#include <string>

#include "cosy/CosyManager.h"
#include "cosy/ClauseInjector.h"
#include "cosy/CNFModel.h"
#include "cosy/CNFReader.h"
#include "cosy/Group.h"
#include "cosy/LiteralAdapter.h"
#include "cosy/Logging.h"
#include "cosy/OrderFactory.h"
#include "cosy/Printer.h"
#include "cosy/SaucyReader.h"
#include "cosy/SPFSManager.h"
#include "cosy/SymmetryFinder.h"
#include "cosy/Stats.h"
#include "cosy/Trail.h"

namespace cosy {

template<class T>
class SymmetryController {
 public:
    SymmetryController(const std::string& cnf_filename,
                       const std::string& symmetry_filename,
                       const std::unique_ptr<LiteralAdapter<T>>& adapter);

    SymmetryController(const std::string& cnf_filename,
                       SymmetryFinder::Automorphism tool,
                       const std::unique_ptr<LiteralAdapter<T>>& adapter);

    virtual ~SymmetryController() {}

    void enableCosy(OrderMode vars, ValueMode value);
    void enableSPFS();

    void updateNotify(T literal_s, unsigned int level,
                      std::vector<T> reason_s,  bool isReasonSymmetric,
                      bool isDecision);
    void updateCancel(T literal_s);

    void propagateFinishWithoutConflict();

    bool hasClauseToInject(ClauseInjector::Type type, T literal_s) const;
    std::vector<T> clauseToInject(ClauseInjector::Type type, T literal_s);

    bool hasClauseToInject(ClauseInjector::Type type) const;
    std::vector<T> clauseToInject(ClauseInjector::Type type);

    void printInfo() const;
    void printStats() const;

 private:
    unsigned int _num_vars;
    const std::unique_ptr<LiteralAdapter<T>>& _literal_adapter;
    Group _group;
    CNFModel _cnf_model;
    Trail _trail;
    ClauseInjector _injector;
    std::unique_ptr<CosyManager> _cosy_manager;
    std::unique_ptr<SPFSManager> _spfs_manager;
    std::unique_ptr<SymmetryFinder> _symmetry_finder;

    bool loadCNFProblem(const std::string cnf_filename);

    std::vector<T> adaptVectorFrom(const std::vector<Literal>& literals);
    std::vector<cosy::Literal> adaptVectorTo(const std::vector<T>& literals);
};

// Implementation

template<class T> inline
bool SymmetryController<T>::loadCNFProblem(const std::string cnf_filename) {
    CNFReader cnf_reader;
    bool success;

    success = cnf_reader.load(cnf_filename, &_cnf_model);
    if (!success) {
        LOG(ERROR) << "CNF file " << cnf_filename << " is not well formed.";
        return false;
    }
    _num_vars = _cnf_model.numberOfVariables();
    _trail.resize(_num_vars);

    return true;
}


template<class T>
inline SymmetryController<T>::SymmetryController(
                           const std::string& cnf_filename,
                           const std::string& sym_filename,
                           const std::unique_ptr<LiteralAdapter<T>>& adapter) :
    _literal_adapter(adapter),
    _cosy_manager(nullptr),
    _spfs_manager(nullptr),
    _symmetry_finder(nullptr) {
    bool success;
    SaucyReader sym_reader;

    if (!loadCNFProblem(cnf_filename))
        return;

    success = sym_reader.load(sym_filename, _num_vars, &_group);
    if (!success)
        LOG(ERROR) << "Saucy file " << sym_filename << " is not well formed.";
}

template<class T>
inline SymmetryController<T>::SymmetryController(
                            const std::string& cnf_filename,
                            SymmetryFinder::Automorphism tool,
                            const std::unique_ptr<LiteralAdapter<T>>& adapter) :
    _literal_adapter(adapter),
    _cosy_manager(nullptr),
    _spfs_manager(nullptr),
    _symmetry_finder(nullptr) {
    if (!loadCNFProblem(cnf_filename))
        return;

    _symmetry_finder = std::unique_ptr<SymmetryFinder>
        (SymmetryFinder::create(_cnf_model, tool));

    CHECK_NOTNULL(_symmetry_finder);
    _symmetry_finder->findAutomorphism(&_group);
}


template<class T>
inline void SymmetryController<T>::enableCosy(OrderMode vars, ValueMode value) {
    if (_group.numberOfPermutations() == 0)
        return;

    std::unique_ptr<Order> order
        (OrderFactory::create(vars, value, _cnf_model, _group));
    CHECK_NOTNULL(order);

    _cosy_manager = std::unique_ptr<CosyManager>
        (new CosyManager(_group, _trail));

    _cosy_manager->defineOrder(std::move(order));
    _cosy_manager->generateUnits(&_injector);
}

template<class T>
inline void SymmetryController<T>::enableSPFS() {
    if (_group.numberOfPermutations() == 0)
        return;

    _spfs_manager = std::unique_ptr<SPFSManager>
        (new SPFSManager(_group, _trail));
}

template<class T>
inline void SymmetryController<T>::updateNotify(T literal_s,
                                                unsigned int level,
                                                std::vector<T> reason_s,
                                                bool isReasonSymmetric,
                                                bool isDecision) {
    const cosy::Literal literal_c = _literal_adapter->convertTo(literal_s);
    const Reason& reason_c = adaptVectorTo(reason_s);
    _trail.enqueue(literal_c, level, reason_c, isReasonSymmetric, isDecision);

    if (_spfs_manager)
        _spfs_manager->updateNotify(literal_c);

    if (_cosy_manager)
        _cosy_manager->updateNotify(literal_c);
}

template<class T>
inline void SymmetryController<T>::updateCancel(T literal_s) {
    const cosy::Literal literal_c = _literal_adapter->convertTo(literal_s);

    /* SPFS Must be updated before unassignLiteral */
    if (_spfs_manager)
        _spfs_manager->updateCancel(literal_c);

    Literal literal = _trail.dequeue();
    CHECK_EQ(literal, literal_c);

    if (_cosy_manager)
        _cosy_manager->updateCancel(literal_c);

    _injector.removeClause(literal_c.variable());
}

template<class T>
inline void SymmetryController<T>::propagateFinishWithoutConflict() {
    if (_spfs_manager)
        _spfs_manager->generateClauses(&_injector);

    /* If SPFS already generate a clause, we do nothing */
    const ClauseInjector::Type type = ClauseInjector::Type::SPFS;
    if (_cosy_manager && !_injector.hasClause(type, kNoBooleanVariable))
        _cosy_manager->generateClauses(&_injector);
}


template<class T> inline bool
SymmetryController<T>::hasClauseToInject(ClauseInjector::Type type,
                                         T literal_s) const {
    const cosy::Literal literal_c = _literal_adapter->convertTo(literal_s);
    return _injector.hasClause(type, literal_c.variable());
}


template<class T> inline std::vector<T>
SymmetryController<T>::clauseToInject(ClauseInjector::Type type, T literal_s) {
    cosy::Literal literal_c =  _literal_adapter->convertTo(literal_s);
    std::vector<cosy::Literal> literals_c =
        std::move(_injector.getClause(type, literal_c.variable()));
    std::vector<T> literals_s = adaptVectorFrom(literals_c);
    return literals_s;
}

template<class T> inline bool
SymmetryController<T>::hasClauseToInject(ClauseInjector::Type type) const {
    return _injector.hasClause(type, kNoBooleanVariable);
}

template<class T> inline std::vector<T>
SymmetryController<T>::clauseToInject(ClauseInjector::Type type) {
    std::vector<cosy::Literal> literals_c =
        std::move(_injector.getClause(type, kNoBooleanVariable));
    std::vector<T> literals_s = adaptVectorFrom(literals_c);
    return literals_s;
}


template<class T> inline std::vector<T>
SymmetryController<T>::adaptVectorFrom(const std::vector<Literal>& literals) {
    std::vector<T> adapted;
    for (const Literal& literal : literals)
        adapted.push_back(_literal_adapter->convertFrom(literal));

    return std::move(adapted);
}

template<class T> inline std::vector<cosy::Literal>
SymmetryController<T>::adaptVectorTo(const std::vector<T>& literals) {
    std::vector<cosy::Literal> adapted;
    for (const T& literal : literals)
        adapted.push_back(_literal_adapter->convertTo(literal));

    return std::move(adapted);
}


template<class T> inline void
SymmetryController<T>::printStats() const {
    _trail.printStats();
    Printer::printSection(" Symmetry Stats ");
    _injector.printStats();

    if (_cosy_manager) {
        IF_STATS_ENABLED(_cosy_manager->printStats());
    }
    if (_spfs_manager) {
        IF_STATS_ENABLED(_spfs_manager->printStats());
    }
}

template<class T> inline void
SymmetryController<T>::printInfo() const {
    _cnf_model.summarize();
    Printer::printSection(" Symmetry Information ");
    if (_symmetry_finder)
        _symmetry_finder->printStats();
    _group.summarize(_num_vars);
    if (_cosy_manager)
        _cosy_manager->summarize();
}



}  // namespace cosy

#endif  // INCLUDE_COSY_SYMMETRYCONTROLLER_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
