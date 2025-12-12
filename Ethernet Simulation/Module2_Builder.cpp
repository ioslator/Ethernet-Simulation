#include "NetworkGraph.h"
//负责邻接矩阵的维护、动态添加节点和边

// 实现添加节点的逻辑
void NetworkGraph::addNode(int id, string name, int x, int y) {
    if (id < MAX_NODES) {
        computers[id] = { id, name, x, y };
        if (id >= nodeCount) nodeCount = id + 1;
    }
}

// 修改：增加了 speed 参数
void NetworkGraph::addEdge(int u, int v, int w, int speed) {
    if (u < nodeCount && v < nodeCount) {
        adjMatrix[u][v] = w;
        adjMatrix[v][u] = w;
        // 存入 speed
        allEdges.push_back({ u, v, w, speed });
    }
}