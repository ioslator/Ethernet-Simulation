#pragma once
#include "Common.h"
//声明核心类，统筹各个成员的功能接口。
class NetworkView;

class NetworkGraph {
public:
    Computer computers[MAX_NODES];
    int adjMatrix[MAX_NODES][MAX_NODES];
    int nodeCount;
    vector<Edge> allEdges;

    NetworkGraph() {
        nodeCount = 0;
        for (int i = 0; i < MAX_NODES; ++i)
            for (int j = 0; j < MAX_NODES; ++j)
                adjMatrix[i][j] = (i == j) ? 0 : INF;
    }

    void loadFromFileWithDialog();
    string queryNodeInfo(int id);
    void addNode(int id, string name, int x, int y);
    void addEdge(int u, int v, int w, int speed);

    // --- 修改点：增加 NetworkView* 参数，用于动画演示 ---
    vector<Edge> runPrim(int startNode, NetworkView* view);
    vector<Edge> runKruskal(NetworkView* view);
    // --------------------------------------------------

    bool detectLoop(string& reportMsg);
};