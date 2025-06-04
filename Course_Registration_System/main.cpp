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

bool admin_mode = false;
string ID = "";
string Name = "";

void new_password(string ID, string password, string Name);
void register_page();
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
string check_pass(string pass);
void loading_screen();
void loading();

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
    string line;
    while (getline(student, line)) {
        istringstream iss(line);
        string fileID, filePassword, fileusername;
        iss >> fileID >> filePassword >> fileusername;
        if (fileID == student_acc) { 
            if (filePassword == password) {
                Name = fileusername;
                Name_modify(Name);
                student.close();
                return true;
            }
        }
    }
    student.close();
        
    ifstream admin("admin.txt"); 
    while (getline(admin, line)) {
        istringstream iss(line);
        string fileID, filePassword, fileusername;
        iss >> fileID >> filePassword >> fileusername;
        if (fileID == student_acc) { 
            if (filePassword == password) {
                admin_mode = true;
                Name = fileusername;
                Name_modify(Name);
                admin.close();
                return true;
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

void loading_screen() {
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    int frame_count = 0;
    float progress = 0.0f;
    
    vector<string> tasks = {
        "Loading user data...",
        "Initializing system...",
        "Preparing interface...",
        "Checking permissions...",
        "Almost there..."
    };
    
    auto title = text("Loading System") | bold | color(Color::Blue);
    
    auto container = Container::Vertical({});

    auto renderer = Renderer(container, [&] {
        vector<Element> elements;
        elements.push_back(title | hcenter);
        elements.push_back(separator());
        
        const char spinner[] = "⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏";
        int spinner_index = frame_count % 10;
        string spinner_str = string(1, spinner[spinner_index]);
        
        progress = (float)frame_count / 10.0f; 
        if (progress > 1.0f) progress = 1.0f;
        
        // Current task
        int task_index = (progress * tasks.size());
        if (task_index >= tasks.size()) task_index = tasks.size() - 1;
        
        elements.push_back(vbox({
            hbox({
                text(spinner_str) | color(Color::Yellow),
                text(" ") | bold,
                text(tasks[task_index]) | color(Color::GrayDark)
            }) | hcenter,
            text("") | hcenter,
            gauge(progress) | color(Color::Green) | hcenter,
            text("") | hcenter,
            text(to_string((int)(progress * 100)) + "%") | color(Color::Blue) | hcenter
        }));
        
        elements.push_back(filler());
        elements.push_back(text("Please wait while we prepare your workspace...") | color(Color::GrayDark) | hcenter);
        
        frame_count++;
        if (frame_count >= 10) {
            screen.ExitLoopClosure()();
        }
        
        return vbox(elements) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            should_quit = true;
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    auto animation_loop = [&]() {
        while (frame_count < 10) { 
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            screen.PostEvent(Event::Custom);
        }
    };

    std::thread animation_thread(animation_loop);
    animation_thread.detach();

    screen.Loop(event_handler);
    if (admin_mode == true) {
        menu_admin();
    } else {
        menu_student();
    }
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
            error_message = "All fields are required.";
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
    
    Component register_button = Button("Register", [&] {
        should_quit = true;
        screen.ExitLoopClosure()();
        register_page();
    });

    auto button_container = Container::Horizontal({
        login_button,
        quit_button,
        register_button
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
                text(" "),
                register_button->Render(),
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
        system("cls");
        loading_screen();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (admin_mode == false) {
            menu_student();
        } else {
            menu_admin();
        }
    }
}

void register_page() {
    system("cls");
    auto screen = ScreenInteractive::TerminalOutput();
    string ID, new_pw, verify_pw, Name;
    InputOption opt; opt.password = true;
    InputOption name_opt; 
    auto old_input = Input(&ID,"ID", opt);
    auto name_input = Input(&Name,"Name", name_opt);
    auto new_input = Input(&new_pw,"password", opt);
    auto ver_input = Input(&verify_pw,"Verify password", opt);
    string error_message;
    bool password_changed = false;
    auto submit_button = Button("Submit", [&] {
        if (ID.empty() || new_pw.empty() || verify_pw.empty() || Name.empty()) {
            error_message = "All fields are required.";
            return;
        } else if (new_pw != verify_pw) {
            error_message = "Passwords do not match!";
            return;
        } else {
            if (check_pass(new_pw) != "ok") {
                error_message = check_pass(new_pw);
                return;
            } else {
                new_password(ID, new_pw, Name);
            }
        }
        password_changed = true;
        error_message = "Account registered successfully! System will turn you back in 1 seconds.";
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
        name_input,
        new_input,
        ver_input,
        button_container
    });
    auto renderer = Renderer(main_container, [&] {
        return vbox({
            text("Register") | bold | color(Color::Blue) | hcenter,
            separator(),
            hbox(text("ID:  "), old_input->Render()) | hcenter,
            hbox(text("Name:  "), name_input->Render()) | hcenter,
            hbox(text("password:  "), new_input->Render()) | hcenter,
            hbox(text("Verify password: "), ver_input->Render()) | hcenter,
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
    loading();
    login();
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
            case 0: loading(); course_history(); break;
            case 1: loading(); timetable(); break;
            case 2: loading(); course_registration(); break;
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
            case 0: loading(); manage_student_records();   break;
            case 1: loading(); manage_courses();           break;
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
    loading();
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
    loading();
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
    loading();
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

bool check_password(string oldpassword, string newpassword) {
    string fileID, filePassword, fileusername;
    vector<string> records;

    if (admin_mode) {
        ifstream admin("admin.txt");
        bool userFound = false;

        while (admin >> fileID >> filePassword >> fileusername) {
            if (fileID == ID && filePassword == oldpassword) {
                filePassword = newpassword;
                userFound = true;
            }
            records.push_back(fileID + " " + filePassword + " " + fileusername);
        }

        admin.close();

        if (userFound) {
            ofstream adminOut("admin.txt");
            for (const auto& record : records) {
                adminOut << record << endl;
            }
            return true;
        } 
    } else {
        ifstream student("student.txt");
        bool userFound = false;

        while (student >> fileID >> filePassword >> fileusername) {
            if (fileID == ID && filePassword == oldpassword) {
                filePassword = newpassword;
                userFound = true;
            }
            records.push_back(fileID + " " + filePassword + " " + fileusername);
        }

        student.close();

        if (userFound) {
            ofstream studentOut("student.txt");
            for (const auto& record : records) {
                studentOut << record << endl;
            }
            return true;
        }
    }

    return false;
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
    if (!check_password(old_pw, new_pw)) {
      error_message = "Bad password.";
      return;
    }
    if (check_pass(new_pw) != "ok") {
      error_message = check_pass(new_pw);
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
      hbox(text("Verify new password: "), ver_input->Render()) | hcenter,
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
  
  if (password_changed) {
    system("cls");
    loading();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  
  system("cls");
  if (admin_mode) {
    menu_admin();
  } else {
    menu_student();
  }
}

void new_password(string ID, string password, string Name) {
    vector<string> records;
    string line;
    ifstream student("student.txt");
    while (getline(student, line)) {
        records.push_back(line);
    }
    student.close();
    for (int i = 0; i < Name.size(); i++) {
        if (Name[i] == ' ') {
            Name[i] = '_';
        }
    }
    string new_record = ID + " " + password + " " + Name;
    records.push_back(new_record);
    ofstream studentOut("student.txt");
    for (const auto& record : records) {
        studentOut << record << endl;
    }
    studentOut.close();
}

string check_pass (string pass) {
    if (pass.size() < 8) {
        return "Your password should consist at least 8 characters long";
    } else {
        int countlarge = 0; int countsmall = 0; int countspecial = 0;
        for (int i = 0 ; i < pass.size(); i++) {
            if (pass[i] >= 'a' && pass[i] <= 'z') {
                countsmall ++;
            }
            if (pass[i] >= 'A' && pass[i] <= 'Z') {
                countlarge ++;
            }
            if (pass[i] == '_' || pass[i] == '-' || pass[i] == '!' || pass[i] == '"' || pass[i] == '@' || pass[i] == '#' || pass[i] == '$' || pass[i] == '%' || pass[i] == '^' || pass[i] == '&' || pass[i] == '*' || pass[i] == '(' || pass[i] == ')') {
                countspecial ++;
            }
        }
        if (countlarge == 0 || countsmall == 0) {
            return "Your password should consist both uppercase and lowercase letter";
        } else if (countspecial == 0) {
            return "Your password should consist special latter";
        } else {
            return "ok";
        }
    }
}

void loading() {
    system("cls");
    auto screen = ScreenInteractive::TerminalOutput();
    int frame = 0;
    
    auto container = Container::Vertical({});
    
    auto renderer = Renderer(container, [&] {
        return vbox({
            text("Loading...") | bold | color(Color::Blue) | hcenter,
            separator(),
            hbox({
                spinner(1, frame) | color(Color::Yellow),
                text(" ") | bold,
                text("Processing") | color(Color::GrayDark)
            }) | hcenter,
            filler()
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Custom) {
            frame++;
            if (frame >= 10) {
                screen.ExitLoopClosure()();
            }
            return true;
        }
        return false;
    });

    auto animation_loop = [&]() {
        while (frame < 10) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            screen.PostEvent(Event::Custom);
        }
    };

    std::thread animation_thread(animation_loop);
    animation_thread.detach();

    screen.Loop(event_handler);
    return;
}

int main () {
    login();
}
