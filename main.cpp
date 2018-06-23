#include <iostream>
#include <limits>
#include <vector>
#include <set>

static constexpr long long inf = std::numeric_limits<long long>::max() >> 3;

struct Edge {
    long long u, v, cap, cost, flow;
    Edge(long long u, long long v, long long cap, long long cost) : u(u), v(v), cap(cap), cost(cost), flow(0) {}
};

void AddEdge(int u, int v, long long cap, long long cost, std::vector<std::vector<int>> &G, std::vector<Edge> &E) {
    Edge e{u, v, cap, cost};
    Edge e_r{v, u, 0, -cost};
    G[u].push_back(E.size());
    G[v].push_back(E.size() + 1);
    E.push_back(e);
    E.push_back(e_r);
}

bool Dfs(int v, const int t, std::vector<char> &used, const std::vector<std::vector<int>> &G, const std::vector<Edge> &E) {
    used[v] = true;
    if (v == t)
        return true;
    for (auto to : G[v])
        if (E[to].flow < E[to].cap && !used[E[to].v] && Dfs(E[to].v, t, used, G, E))
            return true;
    return false;
}

void DfsPath(int v, const int t, std::vector<int> &path, std::vector<char> &used, const std::vector<std::vector<int>> &G, const std::vector<Edge> &E) {
    used[v] = true;
    if (v == t)
        return;
    for (auto to : G[v])
        if (!used[E[to].v] && E[to].flow > 0) {
            path.push_back(to);
            DfsPath(E[to].v, t, path, used, G, E);
            if (E[path.back()].v == t)
                return;
            path.pop_back();
        }
}

void FordBellman(const int n, std::vector<long long> &D, std::vector<int> &pr, const std::vector<Edge> &E) {
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < E.size(); i += 2)
            if (D[E[i].u] + E[i].cost < D[E[i].v]) {
                D[E[i].v] = D[E[i].u] + E[i].cost;
                pr[E[i].v] = i;
            }
}

void Dijkstra(const int n, const int s, std::vector<long long> &D, const std::vector<long long> &pot, std::vector<int> &pr, const std::vector<std::vector<int>> &G, const std::vector<Edge> &E) {
    D.assign(n, inf);
    D[s] = 0;
    std::set<std::pair<long long, int>> Q;
    Q.insert({s, s});
    while (Q.size() > 0) {
        int v = Q.begin()->second;
        Q.erase(Q.begin());
        for (auto to : G[v])
            if (E[to].flow != E[to].cap && D[v] + E[to].cost + pot[E[to].u] - pot[E[to].v] < D[E[to].v]) {
                Q.erase({D[E[to].v], E[to].v});
                D[E[to].v] = D[v] + E[to].cost + pot[E[to].u] - pot[E[to].v];
                pr[E[to].v] = to;
                Q.insert({D[E[to].v], E[to].v});
            }
    }
}

void Johnson(const int n, const int k, long long &flow, long long &cost, const int s, const int t, std::vector<char> used, const std::vector<std::vector<int>> &G, std::vector<Edge> &E) {
    std::vector<long long> D(n, inf), pot;
    std::vector<int> pr(n);
    used.assign(n, false);
    D[s] = 0;
    FordBellman(n, D, pr, E);
    pot = D;
    cost = 0;
    for (flow = 0; flow < k && Dfs(s, t, used, G, E); ++flow) {
        for (int v = t; v != s; v = E[pr[v]].u) {
            ++E[pr[v]].flow;
            --E[pr[v] ^ 1].flow;
            cost += E[pr[v]].cost;
        }
        Dijkstra(n, s, D, pot, pr, G, E);
        for (int i = 1; i < n; ++i)
            pot[i] += D[i];
        used.assign(n, false);
    }
}

void Read(int &n, int&m, int&k, std::vector<std::vector<int>> &G, std::vector<Edge> &E) {
    std::cin.tie(0);
    std::ios_base::sync_with_stdio(false);
    std::cout.precision(15);
    std::cin >> n >> m >> k;
    ++n;
    G.resize(n);
    for (int i = 0; i < m; ++i) {
        int u, v, t;
        std::cin >> u >> v >> t;
        AddEdge(u, v, 1, t, G, E);
        AddEdge(v, u, 1, t, G, E);
    }
}

void Write(const int flow, const int cost, const int n, const int k, const int s, const int t, std::vector<char> used, const std::vector<std::vector<int>> &G, std::vector<Edge> &E) {
    if (flow != k) {
        std::cout << -1;
        return;
    }
    std::cout << 1.0 * cost / flow;
    for (int j = flow; j > 0; --j) {
        std::vector<int> path;
        used.assign(n, false);
        DfsPath(s, t, path, used, G, E);
        std::cout << std::endl << path.size();
        for (auto i : path) {
            --E[i].flow;
            ++E[i ^ 1].flow;
            std::cout << ' ' << (i >> 2) + 1;
        }
    }
}

int main() {
    int n, m, k, s, t;
    long long flow, cost;
    std::vector<char> used;
    std::vector<Edge> E;
    std::vector<std::vector<int>> G;
    Read(n, m, k, G, E);
    s = 1;
    t = n - 1;
    Johnson(n, k, flow, cost, s, t, used, G, E);
    Write(flow, cost, n, k, s, t, used, G, E);
    return 0;
}
