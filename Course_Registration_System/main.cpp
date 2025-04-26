#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>      // std::setprecision
#include <math.h>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <algorithm>
#include <windows.h>

// FTXUI headers
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

using namespace ftxui;
using namespace std;

string studentID = "";
string studentName = "";

// 前向声明函数
void course_history();
void timetable();
void course_registration(); 
void change_password();
void logout();

int main() {
    auto title = text("Course Registration System") | bold | color(Color::Blue);
    
    // 创建学生信息显示
    auto student_info = vbox({
        hbox({
            text("Student ID: ") | color(Color::GrayDark),
            text(studentID) | color(Color::Blue)
        }),
        hbox({
            text("Student Name: ") | color(Color::GrayDark),
            text(studentName) | color(Color::Blue)
        })
    });
    // 创建菜单选项
    vector<string> menu_entries = {
        "My course history",
        "My timetable",
        "My course registration", 
        "Change password",
        "Logout"
    };
    
    int selected = 0;

    // 创建按钮组件
    auto menu = Menu(&menu_entries, &selected);
    menu = menu | border | color(Color::Blue);

    // 创建屏幕
    auto screen = ScreenInteractive::TerminalOutput();

    // 组合所有元素
    auto container = Container::Vertical({
        Renderer([&] {
            return vbox({
                hbox({
                    title | hcenter,
                }) | flex,
                separator(),
                student_info | hcenter,
                separator(),
                filler(),
                text("Press Q to quit") | color(Color::GrayDark) | hcenter,
            }) | border;
        }),
        menu
    });

    // 添加按键处理
    container |= CatchEvent([&](Event event) {
        // 处理选择事件
        if (event == Event::Return) {
            switch (selected) {
                case 0: course_history(); break;
                case 1: timetable(); break;
                case 2: course_registration(); break;
                case 3: change_password(); break;
                case 4: logout(); break;
            }
            return true;
        }
        // 处理Q键退出
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(container);

    return 0;
}

// 临时的函数实现
void course_history() {}
void timetable() {}
void course_registration() {}
void change_password() {}
void logout() {}