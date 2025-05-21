#include <iostream>
#include <string>
#include <sstream>
#include <iomanip> 
#include <math.h>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <algorithm>
#include <windows.h>
#include <chrono>
#include <thread>

// FTXUI headers
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

using namespace ftxui;
using namespace std;

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

bool admin_mode = false;
string ID = "";
string Name = "";

void login();
void menu_student();  
void course_history();
void timetable();
void course_registration(); 
void change_password();
void logout();
void menu_admin();
void manage_courses();
void manage_student_records();
void account_find(string account);

void Name_modify(string s) {
    int length = s.length();
    for (int i = 0; i < length; i++) {
        if (s[i] == '_') {
            s[i] = ' ';
        }
    }
    Name = s;
}

bool pass_check(string student_acc, string password) {
    ifstream student("student.txt");
    string fileID, filePassword, fileusername;
    // Check student accounts
    while (student >> fileID >> filePassword >> fileusername) {
        if (fileID == student_acc) { 
            if (filePassword == password) {
                Name = fileusername;
                Name_modify(Name);
                student.close();
                return true;
            } else {
                student.close();
            }
        }
    }
        
    // Check admin accounts
    ifstream admin("admin.txt"); 
    while (admin >> fileID >> filePassword >> fileusername) {
        if (fileID == student_acc) { 
            if (filePassword == password) {
                admin_mode = true;
                Name = fileusername;
                Name_modify(Name);
                admin.close();
                return true;
            } else {
                admin.close();
                return false; 
            }
        }
    }

    admin.close();
    return false; 
}


void logout() {
    system("cls"); 
    ID = "";
    Name = "";
    login();
}

void login() {
    system("cls"); 
    auto screen = ScreenInteractive::TerminalOutput();
    
    string input_id;
    string input_password;
    InputOption password_option;
    password_option.password = true;
    
    Component id_input = Input(&input_id, "Enter student ID");
    Component password_input = Input(&input_password, "Enter password", password_option);
    
    auto title = text("Course Registration System") | bold | color(Color::Blue);
    
    string error_message;
    bool should_quit = false;
    bool login_success = false;
    

    Component login_button = Button("Login", [&] {
        if (input_id.empty()) {
            should_quit = true;
            screen.ExitLoopClosure()();
        } else if (pass_check(input_id, input_password)) {
            ID = input_id;
            login_success = true;
            screen.ExitLoopClosure()();
        } else {
            error_message = "Invalid ID or password!";
        }
    });


    Component quit_button = Button("Quit", [&] {
        should_quit = true;
        screen.ExitLoopClosure()();
    });
    

    auto button_container = Container::Horizontal({
        login_button,
        quit_button
    });


    auto main_container = Container::Vertical({
        id_input,
        password_input,
        button_container
    });


    auto renderer = Renderer(main_container, [&] {
        return vbox({
            title | hcenter,
            separator(),
            text("Please login to continue") | hcenter,
            separator(),
            hbox(text(" ID: "), id_input->Render()) | hcenter,
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


    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            should_quit = true;
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);


    if (login_success && !should_quit) {
        if (admin_mode == false) {
            menu_student();
        } else {
            menu_admin();
        }
    }
}

void menu_student() {
    system("cls");
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    bool next_page = false;
    int next_page_index = -1;
    
    auto title = text("Course Registration System") | bold | color(Color::Blue);
    
    auto student_info = vbox({
        hbox({
            text("Student ID: ") | color(Color::GrayDark),
            text(ID) | color(Color::Blue)
        }),
        hbox({
            text("Student Name: ") | color(Color::GrayDark),
            text(Name) | color(Color::Blue)
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
        system("cls"); 
        switch (next_page_index) {
            case 0: course_history(); break;
            case 1: timetable(); break;
            case 2: course_registration(); break;
            case 3: change_password(); break;
            case 4: {
                system("cls");
                ID = "";
                Name = "";
                login();
                return; 
            }
        }
    }
}

void menu_admin() {
    system("cls");
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    bool next_page = false;
    int next_page_index = -1;
    
    auto title = text("Course Registration System") | bold | color(Color::Blue);
    
    auto student_info = vbox({
        hbox({
            text("Teacher ID: ") | color(Color::GrayDark),
            text(ID) | color(Color::Blue)
        }),
        hbox({
            text("Teacher Name: ") | color(Color::GrayDark),
            text(Name) | color(Color::Blue)
        })
    });

    vector<string> menu_entries = {
        "Management student records",
        "Manage courses",
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
        system("cls"); 
        switch(next_page_index) {
            case 0: manage_student_records();   break;
            case 1: manage_courses();           break;
            case 2: change_password();          break;
            case 3: { 
                admin_mode = false; 
                ID.clear(); Name.clear();
                login();
            } break;
        }

    }
}

void course_history() {
    system("cls");  
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
    system("cls");
    menu_student();
}

void timetable() {
    system("cls");
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
    system("cls");
    menu_student();
}

void course_registration() {
    system("cls");
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
    system("cls");
    menu_student();
}

void manage_student_records() {
    system("cls");
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    
    auto title = text("Manage Student Records") | bold | color(Color::Blue);
    
    auto container = Container::Vertical({});

    auto renderer = Renderer(container, [&] {
        return vbox({
            title | hcenter,
            separator(),
            text("No Management available.") | hcenter,
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
    system("cls");
    menu_admin();
}

void manage_courses() {
    system("cls");
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    
    auto title = text("manage courses") | bold | color(Color::Blue);
    
    auto container = Container::Vertical({});

    auto renderer = Renderer(container, [&] {
        return vbox({
            title | hcenter,
            separator(),
            text("No management available.") | hcenter,
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
    system("cls");
    menu_admin();
}

bool check_password(string oldpassword, string newpassword, string verification) {
    string fileID, filePassword, fileusername;
    vector<string> records; // To hold all records

    if (admin_mode) {
        ifstream admin("admin.txt");
        bool userFound = false;

        while (admin >> fileID >> filePassword >> fileusername) {
            if (fileID == ID && filePassword == oldpassword && newpassword == verification) {
                filePassword = verification; // Update password
                userFound = true;
            }
            records.push_back(fileID + " " + filePassword + " " + fileusername); // Store record
        }

        admin.close();

        if (userFound) {
            ofstream adminOut("admin.txt");
            for (const auto& record : records) {
                adminOut << record << endl; // Write all records back
            }
            return true;
        } 
    } else {
        ifstream student("student.txt");
        bool userFound = false;

        while (student >> fileID >> filePassword >> fileusername) {
            if (fileID == ID && filePassword == oldpassword && newpassword == verification) {
                filePassword = verification; // Update password
                userFound = true;
            }
            records.push_back(fileID + " " + filePassword + " " + fileusername); // Store record
        }

        student.close();

        if (userFound) {
            ofstream studentOut("student.txt");
            for (const auto& record : records) {
                studentOut << record << endl; // Write all records back
            }
            return true;
        }
    }

    return false; // If no changes were made
}

void change_password() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string old_pw, new_pw, verify_pw;
  InputOption opt; opt.password = true;
  auto old_input = Input(&old_pw,    "Old password", opt);
  auto new_input = Input(&new_pw,    "New password", opt);
  auto ver_input = Input(&verify_pw,"Verify new password",   opt);
  string error_message;
  bool password_changed = false;
  auto submit_button = Button("Submit", [&] {
    if (old_pw.empty() || new_pw.empty() || verify_pw.empty()) {
      error_message = "All fields are required.";
      return;
    }
    if (new_pw != verify_pw) {
      error_message = "New & verify do not match.";
      return;
    }
    if (!check_password(old_pw, new_pw, verify_pw)) {
      error_message = "Bad old password or file I/O error.";
      return;
    }
    password_changed = true;
    error_message = "Password changed successfully! System will turn you back in 1 seconds.";
    screen.ExitLoopClosure()();
  });
  auto cancel_button = Button("Cancel", [&] {
    screen.ExitLoopClosure()();
  });
  auto button_container = Container::Horizontal({
    submit_button,
    cancel_button
  });
  auto main_container = Container::Vertical({
    old_input,
    new_input,
    ver_input,
    button_container
  });
  auto renderer = Renderer(main_container, [&] {
    return vbox({
      text("Change Password") | bold | color(Color::Blue) | hcenter,
      separator(),
      hbox(text("Old password:  "), old_input->Render()) | hcenter,
      hbox(text("New password:  "), new_input->Render()) | hcenter,
      hbox(text("Verify new pw: "), ver_input->Render()) | hcenter,
      separator(),
      hbox({
        submit_button->Render(),
        text(" "),
        cancel_button->Render()
      }) | hcenter,
      filler(),
      text(error_message) | color(Color::Red) | hcenter,
      text("Press Q to quit") | color(Color::GrayDark) | hcenter
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
  system("cls");
  if (admin_mode)
    menu_admin();
  else
    menu_student();
}

void menu_admin();

int main () {
    login();
}