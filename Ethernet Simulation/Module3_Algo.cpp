#include "NetworkGraph.h"
#include "Module4_View.h"
#include <algorithm>
#include <windows.h>
#include <string> // 用于 to_string
//实现 Prim 和 Kruskal 算法

using namespace std;

// --- 修改点：时间拉长到 1.5 秒 (1500毫秒) ---
const int ANIMATION_DELAY = 1500;

// --- Prim 算法 ---
vector<Edge> NetworkGraph::runPrim(int startNode, NetworkView* view) {
    vector<Edge> resultEdges;
    int dist[MAX_NODES];
    bool visited[MAX_NODES] = { false };
    int parent[MAX_NODES];
    int stepCount = 0; // 步骤计数器

    for (int i = 0; i < nodeCount; ++i) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[startNode] = 0;

    for (int i = 0; i < nodeCount; ++i) {
        int u = -1;
        int minVal = INF;
        for (int j = 0; j < nodeCount; ++j) {
            if (!visited[j] && dist[j] < minVal) { minVal = dist[j]; u = j; }
        }
        if (u == -1) break;
        visited[u] = true;

        if (parent[u] != -1) {
            resultEdges.push_back({ parent[u], u, adjMatrix[parent[u]][u] });
            stepCount++; // 步数+1

            if (view != nullptr) {
                // 拼接提示文字： "步骤 1: 连接 PC_A <--> PC_B (权值: 10)"
                string msg = "步骤 " + to_string(stepCount) + ": 连接 " +
                    computers[parent[u]].name + " <--> " + computers[u].name +
                    " (权值: " + to_string(adjMatrix[parent[u]][u]) + ")";

                view->drawTopology(*this, resultEdges, msg); // 传入文字
                Sleep(ANIMATION_DELAY);
            }
        }

        for (int v = 0; v < nodeCount; ++v) {
            if (!visited[v] && adjMatrix[u][v] != INF && adjMatrix[u][v] < dist[v]) {
                dist[v] = adjMatrix[u][v]; parent[v] = u;
            }
        }
    }

    // 动画结束后，刷新一下，去掉步骤文字，或者显示“完成”
    if (view != nullptr) {
        view->drawTopology(*this, resultEdges, "算法演示完成！");
    }

    return resultEdges;
}

// --- Kruskal 算法 ---
vector<Edge> NetworkGraph::runKruskal(NetworkView* view) {
    vector<Edge> resultEdges;
    vector<Edge> sortedEdges = allEdges;
    sort(sortedEdges.begin(), sortedEdges.end());

    DSU dsu;
    dsu.init(nodeCount);
    int stepCount = 0;

    for (const auto& edge : sortedEdges) {
        if (dsu.unite(edge.u, edge.v)) {
            resultEdges.push_back(edge);
            stepCount++;

            if (view != nullptr) {
                // 拼接提示文字
                string msg = "步骤 " + to_string(stepCount) + ": 选中边 " +
                    computers[edge.u].name + " <--> " + computers[edge.v].name +
                    " (权值: " + to_string(edge.w) + ")";

                view->drawTopology(*this, resultEdges, msg); // 传入文字
                Sleep(ANIMATION_DELAY);
            }
        }
    }

    if (view != nullptr) {
        view->drawTopology(*this, resultEdges, "算法演示完成！");
    }

    return resultEdges;
}