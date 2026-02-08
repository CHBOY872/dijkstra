#include <algorithm>
#include <vector>
#include <list>
#include <iostream>

struct Vertice;

struct Edge {
    Vertice* v;
    int weight;
    Edge(Vertice* v, int weight) : v(v), weight(weight) {}
};

struct Vertice {
    unsigned int idx;
    unsigned int length;
    bool visited;
    Vertice* prev_v;
    std::list<Edge> edges;
    Vertice(int idx = -1)
        : idx(idx), length(-1), visited(false), prev_v(nullptr) {}
};

class Graph {
    friend class DijkstraSolver;

    std::list<Vertice> vertices;

    Vertice* GetVertice(unsigned int idx)
    {
        for (auto& i : vertices) {
            if (i.idx == idx)
                return &i;
        }
        return nullptr;
    }
public:
    Graph(std::list<Vertice>&& v) : vertices(std::move(v)) {}
    void CreateEdge(unsigned int idx_from, unsigned int idx_to, int weight)
    {
        for (auto& i : vertices) {
            if (i.idx == idx_from) {
                Vertice* v = GetVertice(idx_to);
                if (v)
                    i.edges.push_back(Edge(v, weight));
            }
        }
    }
};

class DijkstraSolver {
    Graph& g;

public:
    DijkstraSolver(Graph& g) : g(g) {}
    std::list<Vertice> Solve(unsigned int from_idx, unsigned int to_idx);
};

std::list<Vertice> DijkstraSolver::Solve(unsigned int from_idx,
                                         unsigned int to_idx)
{
    auto start = g.GetVertice(from_idx);
    if (!start)
        return {};
    start->length = 0;
    std::list<Vertice> res;

    while (std::find_if(g.vertices.begin(), g.vertices.end(),
        [](const Vertice& v) { return !v.visited; }) != g.vertices.end() &&
        start->idx != to_idx) {

        start->visited = true;

        for (auto& i : start->edges) {
            if (i.weight + start->length < i.v->length) {
                i.v->length = i.weight + start->length;
                i.v->prev_v = start;
            }
        }

        auto x = std::min_element(g.vertices.begin(), g.vertices.end(),
            [](const Vertice& a, const Vertice& b) {
                if (a.visited != b.visited) return !a.visited;
                return a.length < b.length;
            });
        if (x == g.vertices.end() || x->visited ||
            x->length == (unsigned int)-1) {
            start = nullptr;
            break;
        }
        start = &(*x);
    }

    if (!start)
        start = g.GetVertice(to_idx);
    
    if (start->length == (unsigned int)-1)
        return {};

    while (start) {
        res.push_front(*start);
        start = start->prev_v;
    }
 
    return res;
}

int main()
{
    std::list<Vertice> vertices;
    int idx_from, idx_to, weight;
    int n;

    std::cout << "Type number of vertices" << std::endl;
    std::cin >> n;
    std::cout << "Type vertices (idx)" << std::endl;
    for (int i = 0; i < n; i++) {
        std::cin >> idx_from;
        vertices.push_back(Vertice(idx_from));
    }

    Graph g(std::move(vertices));

    std::cout << "Type number of edges" << std::endl;
    std::cin >> n;
    std::cout << "Type edges (from idx, to idx, weight)" << std::endl;
    for (int i = 0; i < n; i++) {
        std::cin >> idx_from >> idx_to >> weight;
        g.CreateEdge(idx_from, idx_to, weight);
    }

    DijkstraSolver d(g);

    std::cout << "Type from to vertices which should be solved" << std::endl;
    std::cin >> idx_from >> idx_to;
    auto x = d.Solve(idx_from, idx_to);
    for (auto& i : x)
        std::cout << "IDX: " << i.idx << std::endl;

    return 0;
}
