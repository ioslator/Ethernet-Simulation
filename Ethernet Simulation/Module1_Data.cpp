#include "NetworkGraph.h"
#include <iostream>
#include <fstream>
#include <windows.h> // Windows API
#include <commdlg.h> // 公共对话框库
// 实现从真实txt文件读取数据的接口

// 确保链接到系统库
#pragma comment(lib, "comdlg32.lib")

using namespace std;

// 1. 实现文件选择对话框
void NetworkGraph::loadFromFileWithDialog() {
    OPENFILENAMEA ofn;       // 文件对话框结构
    char szFile[260] = { 0 }; // 存放文件名的缓冲区

    // 初始化结构体
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0"; // 过滤器
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    cout << "[提示] 请在弹出的窗口中选择 data.txt 文件...\n";

    // 打开窗口
    if (GetOpenFileNameA(&ofn) == TRUE) {
        string filename = ofn.lpstrFile;
        cout << "[成功] 已选择文件: " << filename << endl;

        // --- 也就是原来的读取逻辑，搬到这里 ---
        ifstream inFile(filename);
        if (!inFile.is_open()) return;

        int n;
        inFile >> n;
        nodeCount = 0;
        allEdges.clear();
        // 清空矩阵
        for (int i = 0; i < MAX_NODES; i++)
            for (int j = 0; j < MAX_NODES; j++)
                adjMatrix[i][j] = (i == j) ? 0 : INF;

        for (int i = 0; i < n; ++i) {
            int id, x, y;
            string name;
            inFile >> id >> name >> x >> y;
            addNode(id, name, x, y);
        }

        int m;
        inFile >> m;
        for (int i = 0; i < m; ++i) {
            int u, v, w, s;
            // 注意：这里多读一个 s (speed)
            inFile >> u >> v >> w >> s;
            addEdge(u, v, w, s);
        }
        inFile.close();
        // -------------------------------------
    }
    else {
        cout << "[取消] 用户取消了文件选择。\n";
    }
}

// 2. 查询节点信息
string NetworkGraph::queryNodeInfo(int id) {
    if (id < 0 || id >= nodeCount) return "查无此设备（ID无效）";

    string info = "【计算机信息】\n";
    info += "ID: " + to_string(computers[id].id) + "\n";
    info += "名称: " + computers[id].name + "\n";
    info += "位置坐标: (" + to_string(computers[id].x) + ", " + to_string(computers[id].y) + ")\n";

    info += "\n【连接网线信息】\n";
    bool found = false;
    for (const auto& edge : allEdges) {
        if (edge.u == id || edge.v == id) {
            found = true;
            int target = (edge.u == id) ? edge.v : edge.u;
            info += " -> 连接到 " + computers[target].name;
            info += " (权值: " + to_string(edge.w) + ", 速率: " + to_string(edge.speed) + "Mbps)\n";
        }
    }
    if (!found) info += "该设备是一个孤岛，没有网线连接。";

    return info;
}