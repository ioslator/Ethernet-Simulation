#include "NetworkGraph.h"
#include "Module4_View.h"
#include <iostream>
#include <conio.h>
#include <graphics.h>
#include <iomanip> // 用于格式化输出

using namespace std;

// --- 辅助函数：打印漂亮的分割线 ---
void printSeparator() {
    cout << "------------------------------------------------------------" << endl;
}

// --- 辅助函数：打印操作菜单（每次操作完都显示一遍，防遗忘）---
void printMenu() {
    cout << "\n【等待指令】 请选择下一步操作：" << endl;
    cout << "  [1] 演示 Prim 算法" << endl;
    cout << "  [2] 演示 Kruskal 算法" << endl;
    cout << "  [3] 执行 回路检测" << endl;
    cout << "  [4] 查询 节点详情" << endl;
    cout << "  [5] 重新 加载文件" << endl;
    printSeparator();
    cout << "[提示] 请点击图形化窗口(白框)，然后直接按数字键操作" << endl;
}

// --- 辅助函数：打印算法结果 ---
void printTotalWeight(const vector<Edge>& edges, string algoName) {
    int total = 0;
    for (const auto& e : edges) total += e.w;

    cout << "\n>>> " << algoName << " 算法执行报告 <<<" << endl;
    cout << "  * 选中边数: " << edges.size() << endl;
    cout << "  * 总权值:   " << total << endl;
    printSeparator();
}

// --- 核心修改：带日志的询问复原函数 ---
void askToReset(NetworkGraph& graph, NetworkView& view, vector<Edge>& currentMST) {
    cout << "[系统] 演示已完成，正在等待用户在弹窗中确认..." << endl;

    // 弹窗询问 (图形界面)
    int result = MessageBox(GetHWnd(), _T("演示完毕，是否要清除连线？\n\n点击 [是] -> 清空视图\n点击 [否] -> 保留结果"),
        _T("下一步操作"), MB_YESNO | MB_ICONQUESTION);

    if (result == IDYES) {
        cout << "[用户操作]选择了：清除连线 (复原视图)" << endl;
        currentMST.clear();
        view.drawTopology(graph, currentMST, "就绪 - 视图已复原");
    }
    else {
        cout << "[用户操作]选择了：保留连线 (查看结果)" << endl;
        view.drawTopology(graph, currentMST, "完成 - 结果已保留");
    }
}

int main() {
    SetConsoleTitle(L"以太网模拟系统 - 控制台交互终端");

    // --- 1. 启动欢迎界面 ---
    cout << "############################################################" << endl;
    cout << "#              以太网布网及故障检测模拟系统                #" << endl;
    cout << "#              (双屏交互版 - 控制台日志)                   #" << endl;
    cout << "############################################################" << endl;
    cout << "[系统] 初始化中..." << endl;

    NetworkGraph graph;
    NetworkView view;
    vector<Edge> currentMST;

    cout << "[第一步] 请在弹出的窗口中选择 data.txt 数据文件..." << endl;
    graph.loadFromFileWithDialog();

    if (graph.nodeCount == 0) {
        cout << "[错误] 未选择文件或文件为空，程序退出。" << endl;
        return 0;
    }

    cout << "[系统] 数据加载成功！正在启动图形化界面..." << endl;
    view.initWindow();

    // 初始绘制
    view.drawTopology(graph, currentMST, "就绪 - 等待指令");

    cout << "[成功] 系统启动完成！" << endl;

    // 首次打印菜单
    printMenu();

    ExMessage msg;
    while (true) {
        // 使用 peekmessage 获取消息
        if (peekmessage(&msg, EM_MOUSE | EM_KEY)) {

            // --- 鼠标交互部分 ---
            if (msg.message == WM_MOUSEWHEEL) {
                if (msg.wheel > 0) view.zoomIn(); else view.zoomOut();
                view.drawTopology(graph, currentMST, "交互 - 正在缩放地图");
                // 鼠标滚轮太频繁，不打印日志，以免刷屏
            }
            else if (msg.message == WM_LBUTTONDOWN) {
                int clickedId = view.checkNodeClick(graph, msg.x, msg.y);
                if (clickedId != -1) {
                    string info = graph.queryNodeInfo(clickedId);
                    view.showMessage(info, "节点详情");

                    cout << "\n[交互] 用户点击查看了节点: " << graph.computers[clickedId].name << endl;

                }
            }

            // --- 键盘指令部分 ---
            else if (msg.message == WM_KEYDOWN) {
                bool actionTaken = false; // 标记是否执行了主要动作

                switch (msg.vkcode) {
                case VK_ESCAPE:
                    cout << "\n[系统] 用户请求退出，再见！" << endl;
                    goto END_PROGRAM;

                    // [1] Prim 算法
                case '1':
                    cout << "\n[指令] 收到：演示 Prim 算法..." << endl;
                    currentMST.clear();
                    currentMST = graph.runPrim(0, &view); // 运行动画
                    printTotalWeight(currentMST, "Prim"); // 打印结果
                    askToReset(graph, view, currentMST);  // 询问复原
                    actionTaken = true;
                    break;

                    // [2] Kruskal 算法
                case '2':
                    cout << "\n[指令] 收到：演示 Kruskal 算法..." << endl;
                    currentMST.clear();
                    currentMST = graph.runKruskal(&view);
                    printTotalWeight(currentMST, "Kruskal");
                    askToReset(graph, view, currentMST);
                    actionTaken = true;
                    break;

                    // [3] 回路检测
                case '3':
                    cout << "\n[指令] 收到：执行回路检测..." << endl;
                    currentMST.clear();
                    view.drawTopology(graph, currentMST, "运行中 - 正在检测...");
                    {
                        string r;
                        bool hasLoop = graph.detectLoop(r);
                        cout << "\n>>> 检测报告 <<<" << endl;
                        cout << r << endl;
                        printSeparator();
                        view.showMessage(r, "检测结果");
                        view.drawTopology(graph, currentMST, "完成 - 检测完毕");
                    }
                    actionTaken = true;
                    break;

                    // [4] 查询
                case '4':
                    cout << "\n[指令] 收到：查询节点信息..." << endl;
                    {
                        TCHAR inputBuffer[10];
                        InputBox(inputBuffer, 10, _T("请输入ID (例如 0):"), _T("查询"));
                        int id = _ttoi(inputBuffer);
                        string info = graph.queryNodeInfo(id);

                        cout << ">>> 查询结果 (ID: " << id << ") <<<" << endl;
                        cout << info << endl;
                        printSeparator();

                        view.showMessage(info, "查询结果");
                    }
                    actionTaken = true;
                    break;

                    // [5] 重载
                case '5':
                    cout << "\n[指令] 收到：重新加载文件..." << endl;
                    currentMST.clear();
                    view.drawTopology(graph, currentMST, "状态 - 正在选择文件...");
                    graph.loadFromFileWithDialog();
                    if (graph.nodeCount > 0) {
                        cout << "[成功] 新文件已加载。" << endl;
                        view.drawTopology(graph, currentMST, "就绪 - 新文件已加载");
                    }
                    else {
                        cout << "[取消] 未选择文件。" << endl;
                    }
                    actionTaken = true;
                    break;
                }

                // 如果执行了主要动作，再次打印菜单，形成闭环
                if (actionTaken) {
                    printMenu();
                }
            }
        }
        Sleep(10);
    }

END_PROGRAM:
    view.closeWindow();
    return 0;
}