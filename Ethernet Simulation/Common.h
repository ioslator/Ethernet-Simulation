#pragma once
#include <string>
#include <vector>
#include <climits>
#include <algorithm>
//存放所有模块共用的结构体和常量。
using namespace std;

const int MAX_NODES = 100;
const int INF = INT_MAX;

struct Computer {
    int id;
    string name;
    int x, y;
};

struct Edge {
    int u, v;
    int w;
    int speed; // <--- 新增：传输速率

    bool operator<(const Edge& other) const {
        return w < other.w;
    }
};

// DSU 代码保持不变...
struct DSU {
    int parent[MAX_NODES];
    void init(int n) { for (int i = 0; i <= n; i++) parent[i] = i; }
    int find(int x) { if (parent[x] != x) parent[x] = find(parent[x]); return parent[x]; }
    bool unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX != rootY) { parent[rootX] = rootY; return true; }
        return false;
    }
};