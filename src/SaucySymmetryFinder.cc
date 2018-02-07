// Copyright 2017 Hakan Metin - LIP6

#include "cosy/SaucySymmetryFinder.h"

#include "saucy/saucy.h"

namespace cosy {

static int
on_automorphism(int n, const int *aut, int k, int *support, void *arg) {
    UNUSED_PARAMETER(k);
    UNUSED_PARAMETER(support);

    SymmetryFinderInfo *info = static_cast<SymmetryFinderInfo*>(arg);
    Group *group = info->group;
    unsigned int num_vars = info->num_vars;
    std::unique_ptr<Permutation> permutation(new Permutation(num_vars));
    LiteralIndex index;
    std::vector<bool> seen(n);

    for (int i = 0; i < n; ++i) {
        if (i == aut[i] || seen[i])
            continue;

        index = LiteralIndex(node2Literal(i, num_vars));
        if (index != kNoLiteralIndex)
            permutation->addToCurrentCycle(Literal(index));

        seen[i] = true;

        for (int j = aut[i]; j != i; j = aut[j]) {
            seen[j] = true;
            index = LiteralIndex(node2Literal(j, num_vars));
            if (index != kNoLiteralIndex)
                permutation->addToCurrentCycle(Literal(index));
        }
        permutation->closeCurrentCycle();
    }
    group->addPermutation(std::move(permutation));
    return 1;  // Always continue to search
}

void SaucySymmetryFinder::findAutomorphism(Group *group) {
    SCOPED_TIME_STAT(&_stats.find_time);

    SymmetryFinderInfo info(group, _num_vars);
    std::vector<int> adj;
    std::vector<int> edj;
    std::vector<int> colors;
    int n = _graph.numberOfNodes();
    int e = _graph.numberOfEdges();

    adj.push_back(0);
    unsigned int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += _graph.degree(i);
        adj.push_back(sum);
    }

    for (int i = 0; i < n; i++)
        for (const unsigned int& x : _graph.neighbour(i))
            edj.push_back(x);

    for (int i = 0; i < n; i++)
        colors.push_back(_graph.color(i));


    // Initialize saucy structure
    struct saucy *s = reinterpret_cast<struct saucy*>(saucy_alloc(n));
    struct saucy_graph  *g = reinterpret_cast<struct saucy_graph*>
        (malloc(sizeof(struct saucy_graph)));

    g->n = n;
    g->e = e;
    g->adj = adj.data();
    g->edg = edj.data();

    struct saucy_stats stats;
    saucy_search(s, g, 0, colors.data(), on_automorphism,
                 static_cast<void*>(&info), &stats);
    free(g);
    saucy_free(s);
}

}  // namespace cosy
