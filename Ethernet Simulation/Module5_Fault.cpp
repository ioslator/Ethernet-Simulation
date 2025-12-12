#include "NetworkGraph.h"
//使用 DFS 或 并查集检测回路，并提供故障分析

// 实现回路检测逻辑
bool NetworkGraph::detectLoop(string& reportMsg) {
    DSU dsu;
    dsu.init(nodeCount);
    bool hasLoop = false;
    reportMsg = "回路检测报告:\n-----------------\n";

    // 遍历所有边，如果两个点已经在同一个集合里，再连线就会构成回路
    for (const auto& edge : allEdges) {
        if (!dsu.unite(edge.u, edge.v)) {
            hasLoop = true;
            reportMsg += "[警告] 发现回路: 节点 " + computers[edge.u].name +
                " 与 " + computers[edge.v].name + " 之间的连接会导致环路。\n";
        }
    }

    if (!hasLoop) {
        reportMsg += "网络健康，未检测到任何回路。\n";
    }
    else {
        reportMsg += "-----------------\n建议：请使用Prim或Kruskal算法剔除多余连接。";
    }

    return hasLoop;
}