#include <ncurses/ncurses.h>
#include <vector>
#include <string>

class ConsoleApp {
public:
    ConsoleApp() {
        initscr();              // 初始化 ncurses
        cbreak();               // 禁用行缓冲
        noecho();               // 不回显输入字符
        keypad(stdscr, TRUE);   // 启用功能键
        getmaxyx(stdscr, height, width); // 获取窗口大小
    }

    ~ConsoleApp() {
        endwin();               // 结束 ncurses
    }

    void run() {
        while (true) {
            display();          // 显示界面
            handle_input();     // 处理用户输入
        }
    }

private:
    int height, width;
    int current_tab = 0;      // 当前选中的区域
    std::vector<std::string> logs; // 日志记录
    std::string input_buffer; // 输入缓冲区

    void display() {
        clear();
        getmaxyx(stdscr, height, width);
        
        // 顶部区域显示日志
        int log_height = height - 3; // 留出输入区域
        for (int i = 0; i < log_height && i < logs.size(); ++i) {
            mvprintw(i, 0, logs[logs.size() - 1 - i].c_str()); // 反向显示
        }

        // 输入区域
        mvprintw(log_height, 0, "Input: ");
        mvprintw(log_height + 1, 0, input_buffer.c_str());
        refresh();
    }

    void handle_input() {
        int ch = getch();
        if (current_tab == 0) { // 日志区域
            if (ch == KEY_TAB) {
                current_tab = 1; // 切换到输入区域
            }
        } else { // 输入区域
            if (ch == KEY_BACKSPACE || ch == 127) {
                if (!input_buffer.empty()) {
                    input_buffer.pop_back(); // 删除最后一个字符
                }
            } else if (ch == '\n') {
                logs.push_back(input_buffer); // 添加日志
                input_buffer.clear(); // 清空输入
            } else if (isprint(ch)) {
                input_buffer += ch; // 添加字符到输入缓冲区
            }

            if (ch == KEY_TAB) {
                current_tab = 0; // 切换到日志区域
            }
        }
    }
};

int main() {
    ConsoleApp app;
    app.run();
    return 0;
}