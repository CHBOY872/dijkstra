#include <stdio.h>
#include <queue>
#include <vector>

class Edge;

class Vertice {
    int value;
    int idx;
    std::vector<Edge> edges;
    Vertice* prev;
    bool is_visited;

public:
    Vertice(int _idx = -1)
        : value(-1), idx(_idx), prev(0), is_visited(false) {}
    ~Vertice();
    inline int GetIdx() const { return idx; }
    inline int GetValue() const { return value; }
    inline void SetValue(int _value) { value = _value; }
    void AddEdge(Edge&& e);
    inline Vertice* GetPrevVertice() const { return prev; }
    inline void SetPrevVertice(Vertice* _prev) { prev = _prev; }
    inline bool IsVisited() const { return is_visited; }
    void SetVisited(bool _is_visited) { is_visited = _is_visited; }
    void Reset() { value = -1; prev = 0; is_visited = false; }
    inline std::vector<Edge>& GetEdges() { return edges; }
};

Vertice::~Vertice() {}

void Vertice::AddEdge(Edge&& e)
{
    edges.push_back(std::move(e));
}

class Edge {
    Vertice* to;
    int weight;
public:
    Edge(Vertice* _to = 0, int _weight = -1) : to(_to), weight(_weight) {}
    inline Vertice* GetVertice() { return to; }
    int GetWeight() const { return weight; }
};

class DijkstraSolver {
    struct VerticeList {
        Vertice* v;
        VerticeList* next;
    };

    Vertice **vertices;
    int vertices_size;
    int max_idx;
    Vertice *from, *to;
    bool solved;

public:
    DijkstraSolver(int _vertices_size)
        : vertices(new Vertice*[_vertices_size])
        , vertices_size(_vertices_size), max_idx(-1)
        , from(0), to(0), solved(false)
    {
        int i;
        for (i = 0; i < _vertices_size; i++)
            vertices[i] = 0;
    }
    ~DijkstraSolver();
    void Solve(int from_idx, int to_idx);
    void AddVertice(int idx);
    void AddEdge(int from_idx, int to_idx, int value);
    void LogResult();
    void Clear();

private:
    inline Vertice* GetVertice(int idx);
    Vertice* GetMinimumUnvisitedVertice();
    void Resize(int new_size);
};

DijkstraSolver::~DijkstraSolver()
{
    int i;

    for (i = 0; i <= max_idx; i++) {
        if (vertices[i])
            delete vertices[i];
    }
    delete[] vertices;
}

void DijkstraSolver::Resize(int new_size)
{
    int i;
    Vertice **tmp_vertices = new Vertice*[new_size];

    for (i = 0; i < new_size; i++)
        tmp_vertices[i] = i < vertices_size ? vertices[i] : 0;
    delete[] vertices;
    vertices = tmp_vertices;
    vertices_size = new_size;
}

void DijkstraSolver::AddVertice(int idx)
{
    if (idx >= vertices_size)
        Resize(idx * 2);
    if (vertices[idx])
        return;
    Vertice* tmp = new Vertice(idx);
    vertices[idx] = tmp;
    if (idx > max_idx) max_idx = idx;
}

void DijkstraSolver::AddEdge(int from_idx, int to_idx, int value)
{
    Vertice *from = vertices[from_idx], *to = vertices[to_idx];

    if (!from || !to)
        return;

    from->AddEdge(Edge(to, value));
}

Vertice* DijkstraSolver::GetVertice(int idx)
{
    return vertices[idx] ? vertices[idx] : 0;
}

Vertice* DijkstraSolver::GetMinimumUnvisitedVertice()
{
    int i;
    Vertice* res = 0;

    for (i = 0; i <= max_idx; i++) {
        if (!vertices[i] || vertices[i]->IsVisited())
            continue;
        if (!res && vertices[i]->GetValue() != -1) {
            res = vertices[i];
            continue;
        }
        if (vertices[i]->GetValue() != -1 &&
            vertices[i]->GetValue() < res->GetValue())
            res = vertices[i];
    }
    return res;
}

void DijkstraSolver::Solve(int from_idx, int to_idx)
{
    from = GetVertice(from_idx);
    to   = GetVertice(to_idx);
    if (!from || !to) return;

    using Node = std::pair<int, Vertice*>;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    from->SetValue(0);
    pq.push({0, from});

    while (!pq.empty()) {
        auto [dist, u] = pq.top();
        pq.pop();

        if (!u) continue;
        if (u->IsVisited()) continue;

        if (u->GetValue() != dist) continue;

        u->SetVisited(true);
        if (u == to) { solved = true; return; }

        for (auto &e : u->GetEdges()) {
            Vertice* v = e.GetVertice();
            if (!v || v->IsVisited()) continue;

            int cur = v->GetValue();
            int cand = dist + e.GetWeight();

            if (cur == -1 || cand < cur) {
                v->SetValue(cand);
                v->SetPrevVertice(u);
                pq.push({cand, v});
            }
        }
    }

    solved = false;
}

void DijkstraSolver::LogResult()
{
    if (!solved) {
        fprintf(stderr, "No path\n");
        return;
    }

    VerticeList* tmp_list_result = 0;
    VerticeList* tmp_l;
    Vertice* tmp = to;

    while (tmp) {
        tmp_l = new VerticeList;
        tmp_l->v = tmp;
        tmp_l->next = tmp_list_result;
        tmp_list_result = tmp_l;
        tmp = tmp->GetPrevVertice();
    }

    while (tmp_list_result) {
        printf("%d%s", tmp_list_result->v->GetIdx(),
            tmp_list_result->next ? " -> " : "\n");
        tmp_l = tmp_list_result;
        tmp_list_result = tmp_list_result->next;
        delete tmp_l;
    }
}

void DijkstraSolver::Clear()
{
    int i;
    for (i = 0; i <= max_idx; i++) {
        if (vertices[i])
            vertices[i]->Reset();
    }
    from = 0;
    to = 0;
    solved = false;
}

int main()
{
    DijkstraSolver* s;
    int i, n, idx, from, to, value;
    scanf("%d", &n);

    s = new DijkstraSolver(n+1);
    for (i = 0; i < n; i++) {
        scanf("%d", &idx); s->AddVertice(idx);
    }

    scanf("%d", &n);
    for (i = 0; i < n; i++) {
        scanf("%d %d %d", &from, &to, &value);
        s->AddEdge(from, to, value);
    }

    scanf("%d %d", &from, &to);

    s->Solve(from, to);
    s->LogResult();
    s->Clear();
    delete s;
    return 0;
}
