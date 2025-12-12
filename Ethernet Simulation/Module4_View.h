#pragma once
#include "NetworkGraph.h"
#include <graphics.h>
#include <string>

class NetworkView {
private:
    double scale;
    int offsetX, offsetY;

    // --- 新增：画 GUI 界面的辅助函数 ---
    void drawGUI(string currentStatus, string extraMsg);
    // --------------------------------

public:
    NetworkView();
    void initWindow();
    void closeWindow();

    // 增加 status 参数，用于显示当前程序状态
    void drawTopology(const NetworkGraph& graph, const vector<Edge>& mstEdges,
        string currentStatus = "就绪", string extraMsg = "");

    void showMessage(string msg, string title);
    void zoomIn();
    void zoomOut();
    void moveMap(int dx, int dy);
    int checkNodeClick(const NetworkGraph& graph, int mouseX, int mouseY);
    int toScreenX(int worldX);
    int toScreenY(int worldY);
};