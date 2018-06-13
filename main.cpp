#include <iostream>
#include <vector>
#include <set>

using namespace std;

const long long inf = 1e18;

struct Edge {
    long long u, v, cap, cost, flow;
    Edge(long long u, long long v, long long cap, long long cost) : u(u), v(v), cap(cap), cost(cost), flow(0) {}
};

void AddEdge(int u, int v, long long cap, long long cost, vector<vector<int>> &G, vector<Edge> &E) {
    Edge e{u, v, cap, cost};
    Edge e_r{v, u, 0, -cost};
    G[u].push_back(E.size());
    G[v].push_back(E.size() + 1);
    E.push_back(e);
    E.push_back(e_r);
}

bool dfs(int v, const int t, vector<char> &used, const vector<vector<int>> &G, const vector<Edge> &E) {
    used[v] = true;
    if (v == t)
        return true;
    for (auto to : G[v])
        if (E[to].flow < E[to].cap && !used[E[to].v] && dfs(E[to].v, t, used, G, E))
            return true;
    return false;
}

void DFS_path(int v, const int t, vector<int> &path, vector<char> &used, const vector<vector<int>> &G, const vector<Edge> &E) {
    used[v] = true;
    if (v == t)
        return;
    for (auto to : G[v])
        if (!used[E[to].v] && E[to].flow > 0) {
            path.push_back(to);
            DFS_path(E[to].v, t, path, used, G, E);
            if (E[path.back()].v == t)
                return;
            path.pop_back();
        }
}

void FordBellman(const int n, vector<long long> &D, vector<int> &pr, const vector<Edge> &E) {
    for (int k = 0; k < n; ++k)
        for (int i = 0; i < E.size(); i += 2)
            if (D[E[i].u] + E[i].cost < D[E[i].v]) {
                D[E[i].v] = D[E[i].u] + E[i].cost;
                pr[E[i].v] = i;
            }
}

void Dijkstra(const int n, const int s, vector<long long> &D, const vector<long long> &pot, vector<int> &pr, const vector<vector<int>> &G, const vector<Edge> &E) {
    D.assign(n, inf);
    D[s] = 0;
    set<pair<long long, int>> Q;
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

void Johnson(const int n, const int k, long long &flow, long long &cost, const int s, const int t, vector<char> used, const vector<vector<int>> &G, vector<Edge> &E) {
    vector<long long> D(n, inf), pot;
    vector<int> pr(n);
    used.assign(n, false);
    D[s] = 0;
    FordBellman(n, D, pr, E);
    pot = D;
    cost = 0;
    for (flow = 0; flow < k && dfs(s, t, used, G, E); ++flow) {
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

void Read(int &n, int&m, int&k, vector<vector<int>> &G, vector<Edge> &E) {
    cin.tie(0);
    ios_base::sync_with_stdio(false);
    cout.precision(15);
    cin >> n >> m >> k;
    ++n;
    G.resize(n);
    for (int i = 0; i < m; ++i) {
        int u, v, t;
        cin >> u >> v >> t;
        AddEdge(u, v, 1, t, G, E);
        AddEdge(v, u, 1, t, G, E);
    }
}

void Write(const int flow, const int cost, const int n, const int k, const int s, const int t, vector<char> used, const vector<vector<int>> &G, vector<Edge> &E) {
    if (flow != k) {
        cout << -1;
        return;
    }
    cout << 1.0 * cost / flow;
    for (int j = flow; j > 0; --j) {
        vector<int> path;
        used.assign(n, false);
        DFS_path(s, t, path, used, G, E);
        cout << endl << path.size();
        for (auto i : path) {
            --E[i].flow;
            ++E[i ^ 1].flow;
            cout << ' ' << (i >> 2) + 1;
        }
    }
}

int main() {
    int n, m, k, s, t;
    long long flow, cost;
    vector<char> used;
    vector<Edge> E;
    vector<vector<int>> G;
    Read(n, m, k, G, E);
    s = 1;
    t = n - 1;
    Johnson(n, k, flow, cost, s, t, used, G, E);
    Write(flow, cost, n, k, s, t, used, G, E);
    return 0;
}
