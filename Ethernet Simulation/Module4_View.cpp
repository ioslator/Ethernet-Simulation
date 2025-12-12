#include "Module4_View.h"
#include <cstdio>
//负责 GUI 界面绘制，展示拓扑结构和高亮 MST

// 构造函数：初始化视角
NetworkView::NetworkView() {
    scale = 1.0;
    offsetX = 300;
    offsetY = 50;
}

void NetworkView::initWindow() {
    initgraph(1200, 700); // 把窗口开大一点，留出菜单空间
    setbkmode(TRANSPARENT);
}

void NetworkView::closeWindow() {
    closegraph();
}

int NetworkView::toScreenX(int worldX) { return (int)((worldX + offsetX) * scale); }
int NetworkView::toScreenY(int worldY) { return (int)((worldY + offsetY) * scale); }

void NetworkView::zoomIn() { scale *= 1.1; }
void NetworkView::zoomOut() { scale /= 1.1; }
void NetworkView::moveMap(int dx, int dy) { offsetX += dx; offsetY += dy; }

int NetworkView::checkNodeClick(const NetworkGraph& graph, int mouseX, int mouseY) {
    double clickRadius = 25 * scale;
    for (int i = 0; i < graph.nodeCount; ++i) {
        int sx = toScreenX(graph.computers[i].x);
        int sy = toScreenY(graph.computers[i].y);
        int dx = mouseX - sx;
        int dy = mouseY - sy;
        if (dx * dx + dy * dy <= clickRadius * clickRadius) return i;
    }
    return -1;
}

// --- 核心修改：绘制 GUI 菜单面板 ---
void NetworkView::drawGUI(string currentStatus, string extraMsg) {
    // 1. 绘制左侧半透明黑色背景板 (模拟侧边栏)
    setfillcolor(RGB(50, 50, 50)); // 深灰色
    setlinecolor(RGB(50, 50, 50));
    fillrectangle(0, 0, 260, 700); // 左侧 260 像素宽的区域

    // 2. 绘制标题
    settextcolor(WHITE); // 白色字
    settextstyle(24, 0, _T("黑体"));
    outtextxy(20, 20, _T("以太网布网系统"));
    settextstyle(16, 0, _T("宋体"));
    outtextxy(20, 50, _T("Course Design v3.0"));

    // 3. 绘制分隔线
    setlinecolor(RGB(100, 100, 100));
    line(20, 70, 240, 70);

    // 4. 绘制功能菜单 (这就是你要的 1-5 详细说明)
    int startY = 90;
    int stepY = 35;
    settextstyle(18, 0, _T("黑体"));

    // 辅助宏：简化代码
#define DRAW_MENU_ITEM(key, desc) \
        settextcolor(RGB(255, 200, 0)); /* 黄色按键 */ \
        outtextxy(20, startY, _T(key)); \
        settextcolor(WHITE);            /* 白色描述 */ \
        outtextxy(60, startY, _T(desc)); \
        startY += stepY;

    DRAW_MENU_ITEM("[1]", "Prim 算法演示");
    DRAW_MENU_ITEM("[2]", "Kruskal 算法演示");
    DRAW_MENU_ITEM("[3]", "网络回路检测");
    DRAW_MENU_ITEM("[4]", "查询节点详情");
    DRAW_MENU_ITEM("[5]", "重新加载文件");
    startY += 10;
    DRAW_MENU_ITEM("[ESC]", "退出系统");

    // 5. 绘制操作指南
    startY += 30;
    setlinecolor(RGB(100, 100, 100));
    line(20, startY, 240, startY);
    startY += 20;

    settextcolor(RGB(0, 255, 255)); // 青色标题
    outtextxy(20, startY, _T("交互操作:"));
    startY += 25;
    settextcolor(RGB(200, 200, 200)); // 灰色文字
    settextstyle(16, 0, _T("宋体"));

    // --- 修改点：删除了平移提示，修改了缩放提示 ---
    outtextxy(20, startY, _T("缩放: 鼠标滚轮")); // 只保留滚轮
    startY += 20;
    outtextxy(20, startY, _T("详情: 鼠标左键点击节点"));

    // 6. 绘制底部状态栏
    setfillcolor(RGB(30, 30, 30));
    solidrectangle(0, 660, 1200, 700); // 底部黑色条

    settextcolor(WHITE);
    settextstyle(20, 0, _T("黑体"));
    string statusText = "当前状态: " + currentStatus;

    // 转换 string 到 wstring
    int len = MultiByteToWideChar(CP_ACP, 0, statusText.c_str(), -1, NULL, 0);
    wchar_t* wStatus = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, statusText.c_str(), -1, wStatus, len);
    outtextxy(20, 670, wStatus);
    delete[] wStatus;

    // 7. 如果有额外的大红字提示（比如步骤信息），画在屏幕中间偏上
    if (!extraMsg.empty()) {
        settextcolor(RED);
        settextstyle(32, 0, _T("楷体")); // 大号字体

        int len2 = MultiByteToWideChar(CP_ACP, 0, extraMsg.c_str(), -1, NULL, 0);
        wchar_t* wMsg = new wchar_t[len2];
        MultiByteToWideChar(CP_ACP, 0, extraMsg.c_str(), -1, wMsg, len2);

        // 居中显示
        int textW = textwidth(wMsg);
        outtextxy(260 + (940 - textW) / 2, 50, wMsg);
        delete[] wMsg;
    }
}

void NetworkView::drawTopology(const NetworkGraph& graph, const vector<Edge>& mstEdges, string currentStatus, string extraMsg) {
    setbkcolor(WHITE); // 图形区域背景色
    cleardevice();

    // 1. 先画图 (注意：这里不需要改太多，只要确保不被UI挡住)
    // 建议把图画在稍微靠右的位置，所以我们在 toScreenX 里加一个全局偏移，或者让用户自己移
    // 这里我们保持原样，用户可以用 WASD 移动图避开菜单

    // --- 画线 ---
    settextstyle((int)(16 * scale), 0, _T("黑体"));
    for (const auto& edge : graph.allEdges) {
        bool isMst = false;
        for (const auto& me : mstEdges) {
            if ((me.u == edge.u && me.v == edge.v) || (me.u == edge.v && me.v == edge.u)) {
                isMst = true; break;
            }
        }
        int thickness = isMst ? (int)(5 * scale) : max(1, (int)(2 * scale));
        if (isMst) { setlinecolor(RED); setlinestyle(PS_SOLID, thickness); }
        else { setlinecolor(0xDDDDDD); setlinestyle(PS_SOLID, thickness); }

        int x1 = toScreenX(graph.computers[edge.u].x);
        int y1 = toScreenY(graph.computers[edge.u].y);
        int x2 = toScreenX(graph.computers[edge.v].x);
        int y2 = toScreenY(graph.computers[edge.v].y);
        line(x1, y1, x2, y2);

        TCHAR wbuffer[10];
        _stprintf_s(wbuffer, _T("%d"), edge.w);
        settextcolor(RGB(100, 100, 100));
        outtextxy((x1 + x2) / 2, (y1 + y2) / 2, wbuffer);
    }

    // --- 画点 ---
    int r = (int)(35 * scale);

    setlinestyle(PS_SOLID, max(1, (int)(2 * scale)));

    for (int i = 0; i < graph.nodeCount; ++i) {
        int sx = toScreenX(graph.computers[i].x);
        int sy = toScreenY(graph.computers[i].y);

        // 画圆圈
        setfillcolor(0x00AAFF); // 鲜艳的蓝色
        setlinecolor(BLACK);    // 黑色边框
        fillellipse(sx - r, sy - r, sx + r, sy + r);

        // 准备文字
        TCHAR nameBuffer[50];
        size_t converted = 0;
        mbstowcs_s(&converted, nameBuffer, 50, graph.computers[i].name.c_str(), _TRUNCATE);

        // 修改点2：精确计算文字宽高，实现绝对居中
        int textW = textwidth(nameBuffer);
        int textH = textheight(nameBuffer);

        // 设置文字颜色
        settextcolor(WHITE);
        setbkmode(TRANSPARENT); // 确保文字背景透明

        // 在圆心位置减去文字宽高的一半，确保文字中心对准圆心
        outtextxy(sx - textW / 2, sy - textH / 2, nameBuffer);
    }
    // 2. 最后画 GUI，确保 UI 覆盖在图层上面
    drawGUI(currentStatus, extraMsg);
}

void NetworkView::showMessage(string msg, string title) {
    // 保持不变
    int lenM = MultiByteToWideChar(CP_ACP, 0, msg.c_str(), -1, NULL, 0);
    wchar_t* wM = new wchar_t[lenM];
    MultiByteToWideChar(CP_ACP, 0, msg.c_str(), -1, wM, lenM);
    int lenT = MultiByteToWideChar(CP_ACP, 0, title.c_str(), -1, NULL, 0);
    wchar_t* wT = new wchar_t[lenT];
    MultiByteToWideChar(CP_ACP, 0, title.c_str(), -1, wT, lenT);
    MessageBox(GetHWnd(), wM, wT, MB_OK);
    delete[] wM; delete[] wT;
}