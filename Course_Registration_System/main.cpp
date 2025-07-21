// This is a course registration system for a university.
// It allows students to register for courses, view their course history, and view their timetable.
// It also allows administrators to manage courses and student records.
// It is built using the FTXUI library for the user interface.
// It is a console application that runs on Windows only.
// I use AI to help me Debug the cmakelist.txt and the main.cpp file (mainly the UI functions since I am not familiar with the FTXUI library)
// Also the explanation of the code is in the main.cpp file write by AI.

// =====================
// main.cpp Structure
// =====================
// 1. Include headers and using namespace
// 2. Global variables and function declarations
// 3. Utility functions (e.g., Name_modify, pass_check, check_pass)
// 4. Authentication and account management (login, register, change password, logout)
// 5. Student menu and features (course history, timetable, course registration)
// 6. Admin menu and features (manage courses, manage student records)
// 7. Account operations (create, edit, delete account)
// 8. Main function (program entry point)
// =====================

// headers
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

// Global variable
bool admin_mode = false;
string ID = "";
string Name = "";

// calling function
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
void account_find(string account);
string check_pass(string pass);
void Add_course();
void Management_student_records();
void Create_account();
void Delete_account();
void Edit_account();

//name modify function (to avoid the seperation between first name and last name)
void Name_modify(string s) {
    int length = s.length();
    for (int i = 0; i < length; i++) {
        if (s[i] == '_') {
            s[i] = ' ';
        }
    }
    Name = s;
}

//password check function
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

//logout function
void logout() {
    system("cls"); 
    ID = "";
    Name = "";
    login();
}

//login function
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
            text("Press esc to quit") | color(Color::GrayDark) | hcenter,
        }) | border;
    });


    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            should_quit = true;
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);


    if (login_success && !should_quit) {
        system("cls");
        if (admin_mode == false) {
            menu_student();
        } else {
            menu_admin();
        }
    }
}

//register function (might be deketed in the future)
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
            text("Press esc to quit") | color(Color::GrayDark) | hcenter
        }) | border;
    });
    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });
    screen.Loop(event_handler);
    system("cls");
    login();
}

//student menu function
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
            text("Press esc to quit") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Return) {
            next_page = true;
            next_page_index = selected;
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Escape) {
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

//admin menu function
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
            text("Press esc to quit") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Return) {
            next_page = true;
            next_page_index = selected;
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Escape) {
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
            case 0: Management_student_records();   break;
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

//student's course history function
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
            text("Press esc to return") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);
    system("cls");
    menu_student();
}

//student's timetable function
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
            text("Press esc to return") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);
    
    system("cls");
    menu_student();
}

//student's course registration function
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
            text("Press esc to return") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);
    system("cls");
    menu_student();
}

//admin's manage courses function
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
            text("Press esc to return") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);
    system("cls");
    menu_admin();
}

//password check function
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

//change password function
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
      text("Press esc to quit") | color(Color::GrayDark) | hcenter
    }) | border;
  });
  auto event_handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Escape) {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });
  screen.Loop(event_handler);
  
  if (password_changed) {
    system("cls");
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  
  system("cls");
  if (admin_mode) {
    menu_admin();
  } else {
    menu_student();
  }
}

//new password function
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

//password check function
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

// Stage two

//add course function
void Add_course() {
    system("cls");
    auto screen = ScreenInteractive::TerminalOutput();
    string CourseID, CourseName, CourseRoom,CourseCredit, CourseDate, CourseTime;
    InputOption opt; 
    auto CourseID_input = Input(&CourseID,"Course ID", opt);
    auto CourseName_input = Input(&CourseName,"Course Name", opt);
    auto CourseRoom_input = Input(&CourseRoom,"Course Room", opt);
    auto CourseCredit_input = Input(&CourseCredit,"Course Credit", opt);
    auto CourseDate_input = Input(&CourseDate,"Course Date", opt);
    auto CourseTime_input = Input(&CourseTime,"Course Time", opt);
    string error_message;
    auto submit_button = Button("Submit", [&] {
        if (CourseID.empty() || CourseName.empty() || CourseRoom.empty() || CourseCredit.empty() || CourseDate.empty() || CourseTime.empty()) {
            error_message = "All fields are required.";
            return;
        } 
        error_message = "Course added successfully! System will turn you back in 1 seconds.";
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
        CourseID_input,
        CourseName_input,
        CourseRoom_input,
        CourseCredit_input,
        CourseDate_input,
        CourseTime_input,
        button_container
    });
    auto renderer = Renderer(main_container, [&] {
        return vbox({
            text("Add Course") | bold | color(Color::Blue) | hcenter,
            separator(),
            hbox(text("Course ID:  "), CourseID_input->Render()) | hcenter,
            hbox(text("Course Name:  "), CourseName_input->Render()) | hcenter,
            hbox(text("Course Room:  "), CourseRoom_input->Render()) | hcenter,
            hbox(text("Course Credit:  "), CourseCredit_input->Render()) | hcenter,
            hbox(text("Course Date:  "), CourseDate_input->Render()) | hcenter,
            hbox(text("Course Time:  "), CourseTime_input->Render()) | hcenter,
            separator(),
            hbox({
                submit_button->Render(),
                text(" "),
                cancel_button->Render()
            }) | hcenter,
            filler(),
            text(error_message) | color(Color::Red) | hcenter,
            text("Press esc to quit") | color(Color::GrayDark) | hcenter
        }) | border;
    });
    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });
    screen.Loop(event_handler);
    system("cls");
    login();
}

//admin's manage student records function
void Management_student_records() {
    system("cls");
    auto screen = ScreenInteractive::TerminalOutput();
    bool should_quit = false;
    bool next_page = false;
    int next_page_index = -1;

    vector<string> entries = {
        "Create an account",
        "Edit an account",
        "Delete an account",
    };
    int selected = 0;
    auto menu = Menu(&entries, &selected);

    auto renderer = Renderer(menu, [&] {
        return vbox({
            text("Management Student Records") | bold | color(Color::Blue) | hcenter,
            separator(),
            menu->Render(),
            filler(),
            text("Press esc to quit") | color(Color::GrayDark) | hcenter,
        }) | border;
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Return) {
            next_page = true;
            next_page_index = selected;
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Escape) {
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
            case 0: Create_account();   break;
            case 1: Edit_account(); break;
            case 2: Delete_account();   break;
        }
    }
    
    system("cls");
    menu_admin();
}


void Create_account() {
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
            text("Create student account") | bold | color(Color::Blue) | hcenter,
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
            text("Press esc to quit") | color(Color::GrayDark) | hcenter
        }) | border;
    });
    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });
    screen.Loop(event_handler);
    system("cls");
    menu_admin();
}

//admin's edit account function
void Edit_account() {
    system("cls");
    auto screen = ScreenInteractive::TerminalOutput();
    string ID, Name, Password;
    string old_Name, old_Password;
    InputOption opt; opt.password = true;
    InputOption name_opt;
    bool found = false;
    string error_message;
    int tab_index = 0;

    auto ID_input = Input(&ID, "Enter student ID", name_opt);
    auto Name_input = Input(&Name, "New name (leave blank to keep)", name_opt);
    auto Password_input = Input(&Password, "New password (leave blank to keep)", opt);

    auto next_button = Button("Next", [&] {
        if (ID.empty()) {
            error_message = "Please enter ID.";
            return;
        }
        ifstream fin("student.txt");
        string fileID, filePassword, fileName;
        found = false;
        while (fin >> fileID >> filePassword >> fileName) {
            if (fileID == ID) {
                old_Name = fileName;
                old_Password = filePassword;
                found = true;
                break;
            }
        }
        fin.close();
        if (!found) {
            error_message = "ID not found.";
            return;
        }
        Name = "";
        Password = "";
        error_message = string("Current name: ") + old_Name + ", password is hidden.";
        tab_index = 1;
        screen.PostEvent(Event::Custom);
    });

    auto submit_button = Button("Submit", [&] {
        if (!found) {
            error_message = "Please enter a valid ID first.";
            return;
        }
        vector<tuple<string, string, string>> records;
        ifstream fin("student.txt");
        string fileID, filePassword, fileName;
        while (fin >> fileID >> filePassword >> fileName) {
            if (fileID == ID) {
                string newName = Name.empty() ? old_Name : Name;
                string newPassword = Password.empty() ? old_Password : Password;
                records.push_back({fileID, newPassword, newName});
            } else {
                records.push_back({fileID, filePassword, fileName});
            }
        }
        fin.close();
        ofstream fout("student.txt");
        for (auto& rec : records) {
            fout << get<0>(rec) << " " << get<1>(rec) << " " << get<2>(rec) << endl;
        }
        fout.close();
        error_message = "Account updated! Returning in 1 second.";
        screen.ExitLoopClosure()();
    });

    auto cancel_button = Button("Cancel", [&] {
        screen.ExitLoopClosure()();
    });

    auto id_container = Container::Vertical({
        ID_input,
        next_button,
        cancel_button
    });

    auto edit_container = Container::Vertical({
        Name_input,
        Password_input,
        submit_button,
        cancel_button
    });

    auto main_container = Container::Tab(
        {id_container, edit_container},
        &tab_index
    );

    auto renderer = Renderer(main_container, [&] {
        if (tab_index == 0) {
            return vbox({
                text("Edit Account - Finding ID") | bold | color(Color::Blue) | hcenter,
                separator(),
                hbox(text("ID:  "), ID_input->Render()) | hcenter,
                hbox(next_button->Render(), text(" "), cancel_button->Render()) | hcenter,
                filler(),
                text(error_message) | color(Color::Red) | hcenter,
                text("Press esc to quit") | color(Color::GrayDark) | hcenter
            }) | border;
        } else {
            return vbox({
                text("Edit Account - Update Info") | bold | color(Color::Blue) | hcenter,
                separator(),
                text(string("Current name: ") + old_Name) | hcenter,
                hbox(text("New name:  "), Name_input->Render()) | hcenter,
                hbox(text("New password:  "), Password_input->Render()) | hcenter,
                hbox(submit_button->Render(), text(" "), cancel_button->Render()) | hcenter,
                filler(),
                text(error_message) | color(Color::Red) | hcenter,
                text("Press esc to quit") | color(Color::GrayDark) | hcenter
            }) | border;
        }
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);
    system("cls");
    menu_admin();
}

//admin's delete account function (to delete the account)
void Delete_account() {
    system("cls");
    auto screen = ScreenInteractive::TerminalOutput();
    string ID;
    InputOption opt;
    auto ID_input = Input(&ID, "Enter student ID", opt);
    string error_message;
    bool found = false;
    int tab_index = 0; // 0: input ID, 1: confirm
    string found_name; // Save the found name
    bool should_return = false;

    auto submit_button = Button("Next", [&] {
        if (ID.empty()) {
            error_message = "Please enter ID.";
            return;
        }
        // Check if ID exists
        ifstream fin("student.txt");
        string fileID, filePassword, fileName;
        found = false;
        while (fin >> fileID >> filePassword >> fileName) {
            if (fileID == ID) {
                found = true;
                found_name = fileName;
                break;
            }
        }
        fin.close();
        if (!found) {
            error_message = "ID not found.";
            return;
        }
        error_message = "Are you sure you want to delete this account?";
        tab_index = 1;
        screen.PostEvent(Event::Custom);
    });

    auto confirm_button = Button("Confirm", [&] {
        // Delete the account
        vector<tuple<string, string, string>> records;
        ifstream fin("student.txt");
        string fileID, filePassword, fileName;
        while (fin >> fileID >> filePassword >> fileName) {
            if (fileID != ID) {
                records.push_back({fileID, filePassword, fileName});
            }
        }
        fin.close();
        ofstream fout("student.txt");
        for (auto& rec : records) {
            fout << get<0>(rec) << " " << get<1>(rec) << " " << get<2>(rec) << endl;
        }
        fout.close();
        error_message = "Account deleted! Returning in 1 second.";
        screen.ExitLoopClosure()();
    });

    auto cancel_button_1 = Button("Cancel", [&] {
        should_return = true;
        screen.ExitLoopClosure()();
    });
    auto cancel_button_2 = Button("Cancel", [&] {
        should_return = true;
        screen.ExitLoopClosure()();
    });

    auto id_container = Container::Vertical({
        ID_input,
        submit_button,
        cancel_button_1
    });

    auto confirm_container = Container::Vertical({
        confirm_button,
        cancel_button_2
    });

    auto main_container = Container::Tab(
        {id_container, confirm_container},
        &tab_index
    );

    auto renderer = Renderer(main_container, [&] {
        if (tab_index == 0) {
            return vbox({
                text("Delete Account - Enter ID") | bold | color(Color::Blue) | hcenter,
                separator(),
                hbox(text("ID:  "), ID_input->Render()) | hcenter,
                hbox(submit_button->Render(), text(" "), cancel_button_1->Render()) | hcenter,
                filler(),
                text(error_message) | color(Color::Red) | hcenter,
                text("Press esc to quit") | color(Color::GrayDark) | hcenter
            }) | border;
        } else {
            return vbox({
                text("Delete Account - Confirm") | bold | color(Color::Blue) | hcenter,
                separator(),
                text("Are you sure you want to delete this account?") | color(Color::Red) | hcenter,
                hbox(text("ID: "), text(ID) | color(Color::Blue)) | hcenter,
                hbox(text("Name: "), text(found_name) | color(Color::Blue)) | hcenter,
                hbox(confirm_button->Render(), text(" "), cancel_button_2->Render()) | hcenter,
                filler(),
                text(error_message) | color(Color::Red) | hcenter,
                text("Press esc to quit") | color(Color::GrayDark) | hcenter
            }) | border;
        }
    });

    auto event_handler = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            should_return = true;
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);
    system("cls");
    if (should_return) return;
    menu_admin();
}

//main function
int main () {
    login();
}
