#include <stdio.h>

class Edge;

class Vertice {
    struct EdgeList {
        Edge* e;
        EdgeList* next;
    };
    int value;
    int idx;
    EdgeList* list;
    Vertice* prev;
    bool is_visited;

public:
    Vertice(int _idx = -1)
        : value(-1), idx(_idx), list(0), prev(0), is_visited(false) {}
    ~Vertice();

    inline int GetIdx() const { return idx; }
    inline int GetValue() const { return value; }
    inline void SetValue(int _value) { value = _value; }
    void AddEdge(Edge* e);
    inline Vertice* GetPrevVertice() const { return prev; }
    inline void SetPrevVertice(Vertice* _prev) { prev = _prev; }
    inline bool IsVisited() const { return is_visited; }
    void SetVisited(bool _is_visited) { is_visited = _is_visited; }
    void Reset() { value = -1; prev = 0; is_visited = false; }

    class EdgeListIterator {
        EdgeList* l;
    public:
        EdgeListIterator(Vertice* v) : l(v->list) {}
        Edge* Next()
        {
            if (!l)
                return 0;
            Edge* e = l->e;
            l = l->next;
            return e;
        }
    };
};

Vertice::~Vertice()
{
    EdgeList* tmp = list;
    while (list) {
        list = tmp->next;
        delete tmp;
        tmp = list;
    }
}

void Vertice::AddEdge(Edge* e)
{
    EdgeList* tmp = new EdgeList;
    tmp->e = e;
    tmp->next = list;
    list = tmp;
}

class Edge {
    Vertice* to;
    int weight;

public:
    Edge(Vertice* _to = 0, int _weight = -1) : to(_to), weight(_weight) {}
    inline Vertice* GetVertice() { return to; }
    inline int GetWeight() const { return weight; }
};

class DijkstraSolver {
    struct EdgeList {
        Edge* e;
        EdgeList* next;
    };
    struct VerticeList {
        Vertice* v;
        VerticeList* next;
    };

    EdgeList* edges;
    VerticeList* vertices;
    int vertices_size;

    Vertice *from, *to;

    bool solved;
    int visited_count;

public:
    DijkstraSolver()
        : edges(0), vertices(0), vertices_size(0)
        , from(0), to(0), solved(false), visited_count(0) {}
    ~DijkstraSolver();
    void Solve(int from_idx, int to_idx);
    void AddVertice(int idx);
    void AddEdge(int from_idx, int to_idx, int value);
    void LogResult();
    void Clear();

private:
    inline bool IsAllVisited() const
        { return visited_count == vertices_size; }
    Vertice* GetVertice(int idx);
    Vertice* GetMinimumUnvisitedVertice();
};

DijkstraSolver::~DijkstraSolver()
{
    EdgeList* tmp_e = edges;
    VerticeList* tmp_v = vertices;

    while (edges) {
        edges = tmp_e->next;
        delete tmp_e->e;
        delete tmp_e;
        tmp_e = edges;
    }

    while (vertices) {
        vertices = tmp_v->next;
        delete tmp_v->v;
        delete tmp_v;
        tmp_v = vertices;
    }
}

void DijkstraSolver::AddVertice(int idx)
{
    VerticeList* tmp = vertices;

    while (tmp) {
        if (tmp->v->GetIdx() == idx)
            return;
        tmp = tmp->next;
    }

    tmp = new VerticeList;
    tmp->v = new Vertice(idx);
    tmp->next = vertices;
    vertices = tmp;
    vertices_size++;
}

void DijkstraSolver::AddEdge(int from_idx, int to_idx, int value)
{
    VerticeList *tmp_v = vertices;
    Vertice *from = 0, *to = 0;
    Edge* e;
    EdgeList* tmp_e;

    while (tmp_v) {
        if (tmp_v->v->GetIdx() == from_idx)
            from = tmp_v->v;
        if (tmp_v->v->GetIdx() == to_idx)
            to = tmp_v->v;
        if (from && to)
            break;
        tmp_v = tmp_v->next;
    }

    if (!from || !to)
        return;

    e = new Edge(to, value);
    from->AddEdge(e);
    tmp_e = new EdgeList;
    tmp_e->e = e;
    tmp_e->next = edges;
    edges = tmp_e;
}

Vertice* DijkstraSolver::GetVertice(int idx)
{
    VerticeList* tmp = vertices;

    while (tmp) {
        if (tmp->v->GetIdx() == idx)
            return tmp->v;
        tmp = tmp->next;
    }

    return 0;
}

Vertice* DijkstraSolver::GetMinimumUnvisitedVertice()
{
    VerticeList* tmp = vertices;
    Vertice* res = 0;

    while (tmp) {
        if (tmp->v->IsVisited()) {
            tmp = tmp->next;
            continue;
        }

        if (!res && tmp->v->GetValue() != -1) {
            res = tmp->v;
            tmp = tmp->next;
            continue;
        }

        if (tmp->v->GetValue() != -1 && tmp->v->GetValue() < res->GetValue())
            res = tmp->v;

        tmp = tmp->next;
    }

    return res;
}

void DijkstraSolver::Solve(int from_idx, int to_idx)
{
    int tmp_value = 0;
    Vertice* tmp;
    Edge* e;
    from = GetVertice(from_idx);
    to = GetVertice(to_idx);

    if (!from || !to)
        return;

    tmp = from;
    tmp->SetValue(0);

    while (!IsAllVisited() && tmp && tmp != to ) {
        tmp->SetVisited(true);
        visited_count++;
        Vertice::EdgeListIterator it(tmp);

        while ((e = it.Next())) {
            tmp_value = e->GetVertice()->GetValue();

            if (tmp_value == -1 ||
                tmp_value > e->GetWeight() + tmp->GetValue()) {
                e->GetVertice()->SetValue(e->GetWeight() + tmp->GetValue());
                e->GetVertice()->SetPrevVertice(tmp);
            }

        }

        tmp = GetMinimumUnvisitedVertice();
    }

    if (!tmp)
        return;

    solved = true;
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
    VerticeList* tmp = vertices;
    while (tmp) {
        tmp->v->Reset();
        tmp = tmp->next;
    }
}

int main()
{
    DijkstraSolver s;
    int i, n, idx, from, to, value;

    scanf("%d", &n);
    for (i = 0; i < n; i++) {
        scanf("%d", &idx);
        s.AddVertice(idx);
    }

    scanf("%d", &n);
    for (i = 0; i < n; i++) {
        scanf("%d %d %d", &from, &to, &value);
        s.AddEdge(from, to, value);
    }

    scanf("%d %d", &from, &to);

    s.Solve(from, to);
    s.LogResult();
    s.Clear();
    return 0;
}
