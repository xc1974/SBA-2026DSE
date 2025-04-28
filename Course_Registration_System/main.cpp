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

void menu();  
void course_history();
void timetable();
void course_registration(); 
void change_password();
void logout();

void login() {
    auto screen = ScreenInteractive::TerminalOutput();
    
    std::string input_id;
    std::string input_password;
    InputOption password_option;
    password_option.password = true;
    
    Component id_input = Input(&input_id, "Enter student ID");
    Component password_input = Input(&input_password, "Enter password", password_option);
    
    auto title = text("Course Registration System") | bold | color(Color::Blue);
    
    std::string error_message;
    bool should_quit = false;
    bool login_success = false;
    
    // 创建登录按钮
    Component login_button = Button("Login", [&] {
        if (input_id.empty()) {
            should_quit = true;
            screen.ExitLoopClosure()();
        } else if (input_id == "S1234567" && input_password == "EMIband1A") {
            studentID = input_id;
            studentName = "Test Student";
            login_success = true;
            screen.ExitLoopClosure()();
        } else {
            error_message = "Invalid ID or password!";
        }
    });

    // 创建退出按钮
    Component quit_button = Button("Quit", screen.ExitLoopClosure());
    
    // 创建按钮容器
    auto button_container = Container::Horizontal({
        login_button,
        quit_button
    });

    // 创建主容器
    auto main_container = Container::Vertical({
        id_input,
        password_input,
        button_container
    });

    // 创建渲染器
    auto renderer = Renderer(main_container, [&] {
        return vbox({
            title | hcenter,
            separator(),
            text("Please login to continue") | hcenter,
            separator(),
            hbox(text(" Student ID: "), id_input->Render()) | hcenter,
            hbox(text(" Password:  "), password_input->Render()) | hcenter,
            separator(),
            hbox({
                login_button->Render(),
                text(" "),
                quit_button->Render(),
            }) | hcenter,
            filler(),
            text(error_message) | color(Color::Red) | hcenter,
            text("Press Q to quit") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    // 添加事件处理
    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            should_quit = true;
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);

    // 如果登录成功且不是退出，则进入主菜单
    if (login_success && !should_quit) {
        menu();
    }
}

void menu() {
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    bool next_page = false;
    int next_page_index = -1;
    
    auto title = text("Course Registration System") | bold | color(Color::Blue);
    
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

    vector<string> menu_entries = {
        "My course history",
        "My timetable",
        "My course registration", 
        "Change password",
        "Logout"
    };
    
    int selected = 0;
    auto menu = Menu(&menu_entries, &selected);

    auto main_container = Container::Vertical({
        menu
    });

    auto renderer = Renderer(main_container, [&] {
        return vbox({
            title | hcenter,
            separator(),
            student_info | hcenter,
            separator(),
            menu->Render(),
            filler(),
            text("Press Q to quit") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Return) {
            next_page = true;
            next_page_index = selected;
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            should_quit = true;
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);

    if (next_page) {
        switch (next_page_index) {
            case 0: course_history(); break;
            case 1: timetable(); break;
            case 2: course_registration(); break;
            case 3: change_password(); break;
            case 4: logout(); break;
        }
    }
}

void course_history() {
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    
    auto title = text("Course History") | bold | color(Color::Blue);
    
    auto container = Container::Vertical({});

    auto renderer = Renderer(container, [&] {
        return vbox({
            title | hcenter,
            separator(),
            text("No course history available.") | hcenter,
            filler(),
            text("Press Q to return") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);
    menu();
}

void timetable() {
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    
    auto title = text("Timetable") | bold | color(Color::Blue);
    
    auto container = Container::Vertical({});

    auto renderer = Renderer(container, [&] {
        return vbox({
            title | hcenter,
            separator(),
            text("No timetable available.") | hcenter,
            filler(),
            text("Press Q to return") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);
    menu();
}

void course_registration() {
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    
    auto title = text("Course Registration") | bold | color(Color::Blue);
    
    auto container = Container::Vertical({});

    auto renderer = Renderer(container, [&] {
        return vbox({
            title | hcenter,
            separator(),
            text("Course registration is not open.") | hcenter,
            filler(),
            text("Press Q to return") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);
    menu();
}

void change_password() {
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    
    auto title = text("Change Password") | bold | color(Color::Blue);
    
    auto container = Container::Vertical({});

    auto renderer = Renderer(container, [&] {
        return vbox({
            title | hcenter,
            separator(),
            text("Password change functionality not implemented.") | hcenter,
            filler(),
            text("Press Q to return") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);
    menu();
}

void logout() {
    studentID = "";
    studentName = "";
    login();
}

int main () {
    login();
}