// This is a course registration system for a university.
// It allows students to register for courses, view their course history, and
// view their timetable. It also allows administrators to manage courses and
// student records. It is built using the FTXUI library for the user interface.
// It is a console application that runs on Windows only.
// I use AI to help me Debug the cmakelist.txt and the main.cpp file (mainly the
// UI functions since I am not familiar with the FTXUI library) Also the
// explanation of the code is in the main.cpp file write by AI.

// =====================
// main.cpp Structure
// =====================
// 1. Include headers and using namespace
// 2. Global variables and function declarations
// 3. Utility functions (e.g., Name_modify, pass_check, check_pass)
// 4. Authentication and account management (login, register, change password,
// logout)
// 5. Student menu and features (course history, timetable, course registration)
// 6. Admin menu and features (manage courses, manage student records)
// 7. Account operations (create, edit, delete account)
// 8. Main function (program entry point)
// =====================

#include <math.h>
#include <windows.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

using namespace ftxui;
using namespace std;

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
void Edit_course();
void Delete_course();
void Management_student_records();
void Management_course_records();
void Create_account();
void Delete_account();
void Edit_account();
void startup_animation();
void write_in_course(string CourseID,
                     string CourseName,
                     string CourseRoom,
                     string CourseCredit,
                     string CourseDate,
                     string CourseTime);
void clash_timetable(string CourseID,
                     string CourseName,
                     string CourseRoom,
                     string CourseCredit,
                     string CourseDate,
                     string CourseTime);
void timetable();

// name modify function (to avoid the seperation between first name and last
// name)
void Name_modify(string s) {
  int length = s.length();
  for (int i = 0; i < length; i++) {
    if (s[i] == '_') {
      s[i] = ' ';
    }
  }
  Name = s;
}

// password check function
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

// logout function
void logout() {
  system("cls");
  ID = "";
  Name = "";
  login();
}

// login function
void login() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();

  string input_id;
  string input_password;
  InputOption password_option;
  password_option.password = true;

  Component id_input = Input(&input_id, "Enter student ID");
  Component password_input =
      Input(&input_password, "Enter password", password_option);

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

  auto button_container =
      Container::Horizontal({login_button, quit_button, register_button});

  auto main_container =
      Container::Vertical({id_input, password_input, button_container});

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
           }) |
           border;
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

// register function (might be deketed in the future)
void register_page() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string ID, new_pw, verify_pw, Name;
  InputOption opt;
  opt.password = true;
  InputOption name_opt;
  auto old_input = Input(&ID, "ID", opt);
  auto name_input = Input(&Name, "Name", name_opt);
  auto new_input = Input(&new_pw, "password", opt);
  auto ver_input = Input(&verify_pw, "Verify password", opt);
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
    error_message =
        "Account registered successfully! System will turn you back in 1 "
        "seconds.";
    screen.ExitLoopClosure()();
  });
  auto cancel_button = Button("Cancel", [&] { screen.ExitLoopClosure()(); });
  auto button_container = Container::Horizontal({submit_button, cancel_button});
  auto main_container = Container::Vertical(
      {old_input, name_input, new_input, ver_input, button_container});
  auto renderer = Renderer(main_container, [&] {
    return vbox(
               {text("Register") | bold | color(Color::Blue) | hcenter,
                separator(), hbox(text("ID:  "), old_input->Render()) | hcenter,
                hbox(text("Name:  "), name_input->Render()) | hcenter,
                hbox(text("password:  "), new_input->Render()) | hcenter,
                hbox(text("Verify password: "), ver_input->Render()) | hcenter,
                separator(),
                hbox({submit_button->Render(), text(" "),
                      cancel_button->Render()}) |
                    hcenter,
                filler(),
                text(error_message) |
                    color(password_changed ? Color::Green : Color::Red) |
                    hcenter,
                text("Press esc to quit") | color(Color::GrayDark) | hcenter}) |
           border;
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

// student menu function
void menu_student() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  bool should_quit = false;
  bool next_page = false;
  int next_page_index = -1;

  auto title = text("Course Registration System") | bold | color(Color::Blue);

  auto student_info =
      vbox({hbox({text("Student ID: ") | color(Color::GrayDark),
                  text(ID) | color(Color::Blue)}),
            hbox({text("Student Name: ") | color(Color::GrayDark),
                  text(Name) | color(Color::Blue)})});

  vector<string> menu_entries = {"My course history", "My timetable",
                                 "My course registration", "Change password",
                                 "Logout"};

  int selected = 0;
  MenuOption menu_option;
  menu_option.on_enter = [&] {
    next_page = true;
    next_page_index = selected;
    screen.ExitLoopClosure()();
  };
  auto menu = Menu(&menu_entries, &selected, menu_option);

  auto main_container = Container::Vertical({menu});

  auto renderer = Renderer(main_container, [&] {
    return vbox({
               title | hcenter,
               separator(),
               student_info | hcenter,
               separator(),
               menu->Render(),
               filler(),
               text("Press esc to quit") | color(Color::GrayDark) | hcenter,
           }) |
           border;
  });

  // Double-click detection state
  auto last_click_time = std::chrono::steady_clock::now() - std::chrono::milliseconds(1000);
  int last_click_selected = -1;

  auto event_handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Return) {
      next_page = true;
      next_page_index = selected;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event.is_mouse()) {
      const auto& m = event.mouse();
      if (m.button == Mouse::Left && m.motion == Mouse::Pressed) {
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_click_time).count();
        if (last_click_selected == selected && diff <= 350) {
          next_page = true;
          next_page_index = selected;
          screen.ExitLoopClosure()();
          return true;
        }
        last_click_selected = selected;
        last_click_time = now;
      }
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
      case 0:
        course_history();
        break;
      case 1:
        timetable();
        break;
      case 2:
        course_registration();
        break;
      case 3:
        change_password();
        break;
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

// admin menu function
void menu_admin() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  bool should_quit = false;
  bool next_page = false;
  int next_page_index = -1;

  auto title = text("Course Registration System") | bold | color(Color::Blue);

  auto student_info =
      vbox({hbox({text("Teacher ID: ") | color(Color::GrayDark),
                  text(ID) | color(Color::Blue)}),
            hbox({text("Teacher Name: ") | color(Color::GrayDark),
                  text(Name) | color(Color::Blue)})});

  vector<string> menu_entries = {"Management student records", "Manage courses",
                                 "Change password", "Logout"};

  int selected = 0;
  MenuOption menu_option;
  menu_option.on_enter = [&] {
    next_page = true;
    next_page_index = selected;
    screen.ExitLoopClosure()();
  };
  auto menu = Menu(&menu_entries, &selected, menu_option);

  auto main_container = Container::Vertical({menu});

  auto renderer = Renderer(main_container, [&] {
    return vbox({
               title | hcenter,
               separator(),
               student_info | hcenter,
               separator(),
               menu->Render(),
               filler(),
               text("Press esc to quit") | color(Color::GrayDark) | hcenter,
           }) |
           border;
  });

  auto last_click_time = std::chrono::steady_clock::now() - std::chrono::milliseconds(1000);
  int last_click_selected = -1;

  auto event_handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Return) {
      next_page = true;
      next_page_index = selected;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event.is_mouse()) {
      const auto& m = event.mouse();
      if (m.button == Mouse::Left && m.motion == Mouse::Pressed) {
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_click_time).count();
        if (last_click_selected == selected && diff <= 350) {
          next_page = true;
          next_page_index = selected;
          screen.ExitLoopClosure()();
          return true;
        }
        last_click_selected = selected;
        last_click_time = now;
      }
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
      case 0:
        Management_student_records();
        break;
      case 1:
        startup_animation();
        manage_courses();
        break;
      case 2:
        change_password();
        break;
      case 3: {
        admin_mode = false;
        ID.clear();
        Name.clear();
        login();
      } break;
    }
  }
}

// student's course history function
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
           }) |
           border;
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

// admin's manage courses function
void manage_courses() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  bool should_quit = false;
  bool next_page = false;
  int next_page_index = -1;

  auto title = text("Course Registration System") | bold | color(Color::Blue);

  auto student_info =
      vbox({hbox({text("Student ID: ") | color(Color::GrayDark),
                  text(ID) | color(Color::Blue)}),
            hbox({text("Student Name: ") | color(Color::GrayDark),
                  text(Name) | color(Color::Blue)})});

  vector<string> menu_entries = {"Create a course", "Edit a course",
                                 "Delete a course", "View timetable"};

  int selected = 0;
  MenuOption menu_option;
  menu_option.on_enter = [&] {
    next_page = true;
    next_page_index = selected;
    screen.ExitLoopClosure()();
  };
  auto menu = Menu(&menu_entries, &selected, menu_option);

  auto main_container = Container::Vertical({menu});

  auto renderer = Renderer(main_container, [&] {
    return vbox({
               title | hcenter,
               separator(),
               student_info | hcenter,
               separator(),
               menu->Render(),
               filler(),
               text("Press esc to quit") | color(Color::GrayDark) | hcenter,
           }) |
           border;
  });

  auto last_click_time = std::chrono::steady_clock::now() - std::chrono::milliseconds(1000);
  int last_click_selected = -1;

  auto event_handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Return) {
      next_page = true;
      next_page_index = selected;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event.is_mouse()) {
      const auto& m = event.mouse();
      if (m.button == Mouse::Left && m.motion == Mouse::Pressed) {
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_click_time).count();
        if (last_click_selected == selected && diff <= 350) {
          next_page = true;
          next_page_index = selected;
          screen.ExitLoopClosure()();
          return true;
        }
        last_click_selected = selected;
        last_click_time = now;
      }
    }
    if (event == Event::Escape) {
      should_quit = true;
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(event_handler);

  if (should_quit) {
    system("cls");
    menu_admin();
    return;
  }

  if (next_page) {
    system("cls");
    switch (next_page_index) {
      case 0:
        Add_course();
        break;
      case 1:
        Edit_course();
        break;
      case 2:
        Delete_course();
        break;
      case 3:
        timetable();
        break;
    }
  }
}

// password check function
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

// change password function
void change_password() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string old_pw, new_pw, verify_pw;
  InputOption opt;
  opt.password = true;
  auto old_input = Input(&old_pw, "Old password", opt);
  auto new_input = Input(&new_pw, "New password", opt);
  auto ver_input = Input(&verify_pw, "Verify new password", opt);
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
    error_message =
        "Password changed successfully! System will turn you back in 1 "
        "seconds.";
    screen.ExitLoopClosure()();
  });
  auto cancel_button = Button("Cancel", [&] { screen.ExitLoopClosure()(); });
  auto button_container = Container::Horizontal({submit_button, cancel_button});
  auto main_container =
      Container::Vertical({old_input, new_input, ver_input, button_container});
  auto renderer = Renderer(main_container, [&] {
    return vbox(
               {text("Change Password") | bold | color(Color::Blue) | hcenter,
                separator(),
                hbox(text("Old password:  "), old_input->Render()) | hcenter,
                hbox(text("New password:  "), new_input->Render()) | hcenter,
                hbox(text("Verify new password: "), ver_input->Render()) |
                    hcenter,
                separator(),
                hbox({submit_button->Render(), text(" "),
                      cancel_button->Render()}) |
                    hcenter,
                filler(),
                text(error_message) |
                    color(password_changed ? Color::Green : Color::Red) |
                    hcenter,
                text("Press esc to quit") | color(Color::GrayDark) | hcenter}) |
           border;
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

// new password function
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

// password check function
string check_pass(string pass) {
  if (pass.size() < 8) {
    return "Your password should consist at least 8 characters long";
  } else {
    int countlarge = 0;
    int countsmall = 0;
    int countspecial = 0;
    for (int i = 0; i < pass.size(); i++) {
      if (pass[i] >= 'a' && pass[i] <= 'z') {
        countsmall++;
      }
      if (pass[i] >= 'A' && pass[i] <= 'Z') {
        countlarge++;
      }
      if (pass[i] == '_' || pass[i] == '-' || pass[i] == '!' ||
          pass[i] == '"' || pass[i] == '@' || pass[i] == '#' ||
          pass[i] == '$' || pass[i] == '%' || pass[i] == '^' ||
          pass[i] == '&' || pass[i] == '*' || pass[i] == '(' ||
          pass[i] == ')') {
        countspecial++;
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

void startup_animation() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();

  vector<string> ascii_art = {
      "                                                                        "
      "                                                        ",
      "                                                                        "
      "                                                        ",
      "      +*  :#                               *#                         "
      "+:                            +*-                    +#.         .=.    "
      "   ",
      "  @@@@@@  #@@@@@#@@@@@@@@@@=    ..........+@@=..........             "
      "@@*                           .@@=                   *@@          "
      "%@%-@@:   ",
      "     .@@  #@=   =@*      *@=   :##########@@@@##########=    "
      ".......@@@-.............       ++++++*@@@*++++++++=         *@@@@@@@@@  "
      "  %@%  @@%  ",
      "   @@@@@. @@@@@.-@@@@@@@@@@=      -@@@@@@@@@@@@@@@@@@%      "
      "@@@@@@@@@@@@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@%##@@@        @@@@:   "
      "@@%---=@@%---+-  ",
      "   @# :@- @: .@:-@@******@@=      +@@     :@@.     #@@            +@@.  "
      "                    @@-              :@@      -@@+ "
      "-@@*#@@=@@@@@@@@@@@@= ",
      "   @@%@@- @@%@@:-@*      *@=      +@@%%%%%@@@@%%%%%@@@           #@@    "
      "   =@@              @@=              -@@     .@%     .@@@     *@@@@.    "
      " ",
      "     #@  @@     -@@@@@@@@@@=              :@@.                  @@@     "
      "   =@@              @@@@@@@@@@@@@@@@@@@@       #@@@@ @@@     :@@  @@-   "
      " ",
      "    %@@@@@@@@@@--@*      *@=     .@@@@@@@@@@@@@@@@@@@@-       %@@@-  "
      "-+++++@@@#+++++.       @@-              :@@      :=   +@@+     *@@:   "
      "@@%   ",
      "   @@@:  +@+    -@@@@@@@@@@=              :@@.      @@-     @@@@@@-  "
      "%@@@@@@@@@@@@@@+       @@-              :@@         @@@@      @@@      "
      "@@%  ",
      "  @@@@@@@@@@@@@  .-@@=+@@..    @@@@@@@@@@@@@@@@@@@@@@@@@@.  %*  @@-     "
      "   =@@              @@@@@@@@@@@@@@@@@@@@      #@@@+     :@@@:        "
      "@@* ",
      "    @@   =@=      -@@ -@@                 :@@.      @@-         @@-     "
      "   =@@              @@=              :@@                 -              "
      " ",
      "    @@@@@@@@@@@   @@- -@@ :@=    +@@@@@@@@@@@@@@@@@@@@-         @@-     "
      "   =@@              @@-              :@@        *@@   @@.   @@+   #@@   "
      " ",
      "    @@*--%@%---  @@*  -@@ =@*             :@@.      **          @@-     "
      "   =@@              @@@%%%%%%%%%%%%@@@@@       %@@    *@%    @@:   -@@+ "
      " ",
      "    @@+.......*@@@.    @@@@@          *@@@@@@                   @@- "
      "@@@@@@@@@@@@@@@@@@      @@#..............*@@      @@*     =@@    +@@    "
      " @@* ",
      "                                                                        "
      "                                                        ",
      "                                                                        "
      "                                                        ",
      "                                                                        "
      "                                                        "};

  auto art_box = vbox({[&ascii_art] {
    Elements lines;
    for (const auto& line : ascii_art) {
      lines.push_back(text(line) | hcenter);
    }
    return lines;
  }()});

  auto title = text(
                   "Please maximize the window for better experience | "
                   "觀事在自然 | logging into the course editing system") |
               bold | color(Color::Blue) | hcenter;

  auto renderer = Renderer([&] {
    return vbox({
               filler(),
               art_box,
               separator(),
               title,
               filler(),
           }) |
           border;
  });
  std::thread([&screen] {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    screen.ExitLoopClosure()();
  }).detach();

  screen.Loop(renderer);
  system("cls");
  manage_courses();
}

// timetable function (use AI to generate but it is not in the requirement so
// plz dont ban me.)
void timetable() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();

  vector<tuple<string, string, string, string, string, string>> courses;
  ifstream fin("course.txt");
  string courseID, courseName, courseRoom, courseCredit, courseDate, courseTime;

  while (getline(fin, courseID)) {
    if (!courseID.empty()) {
      getline(fin, courseName);
      getline(fin, courseRoom);
      getline(fin, courseCredit);
      getline(fin, courseDate);
      getline(fin, courseTime);
      string empty_line;
      getline(fin, empty_line);
      courses.push_back({courseID, courseName, courseRoom, courseCredit,
                         courseDate, courseTime});
    }
  }
  fin.close();

  vector<string> time_slots = {
      "08:00", "08:30", "09:00", "09:30", "10:00", "10:30", "11:00", "11:30",
      "12:00", "12:30", "13:00", "13:30", "14:00", "14:30", "15:00", "15:30",
      "16:00", "16:30", "17:00", "17:30", "18:00", "18:30"};

  vector<string> g_rooms = {"G001", "G002", "G003", "G004", "G005",
                            "G006", "G007", "G008", "G009", "G010"};
  vector<string> floor1_rooms = {"1001", "1002", "1003", "1004", "1005",
                                 "1006", "1007", "1008", "1009", "1010"};
  vector<string> floor2_rooms = {"2001", "2002", "2003", "2004", "2005",
                                 "2006", "2007", "2008", "2009", "2010"};
  vector<string> floor3_rooms = {"3001", "3002", "3003", "3004", "3005",
                                 "3006", "3007", "3008", "3009", "3010"};
  vector<string> floor4_rooms = {"4001", "4002", "4003", "4004", "4005",
                                 "4006", "4007", "4008", "4009", "4010"};

  int current_floor = 0;  // 0=G, 1=1st, 2=2nd, 3=3rd, 4=4th
  vector<string> floor_names = {"G FLOOR", "1ST FLOOR", "2ND FLOOR",
                                "3RD FLOOR", "4TH FLOOR"};
  vector<vector<string>> all_rooms = {g_rooms, floor1_rooms, floor2_rooms,
                                      floor3_rooms, floor4_rooms};

  int current_day = 0;  // 0=MON, 1=TUE, 2=WED, 3=THU, 4=FRI
  vector<string> day_names = {"MON", "TUE", "WED", "THU", "FRI"};

  auto back_button = Button("Back", [&] { screen.ExitLoopClosure()(); });

  auto renderer = Renderer(back_button, [&] {
    Elements timetable_content;

    timetable_content.push_back(text("TIMETABLE") | bold | color(Color::Blue) |
                                hcenter);
    timetable_content.push_back(separator());

    Elements floor_nav;
    for (int i = 0; i < floor_names.size(); i++) {
      if (i == current_floor) {
        floor_nav.push_back(text("[" + floor_names[i] + "]") |
                            color(Color::Yellow) | bold);
      } else {
        floor_nav.push_back(text(floor_names[i]) | color(Color::GrayDark));
      }
      if (i < floor_names.size() - 1) {
        floor_nav.push_back(text(" | ") | color(Color::GrayDark));
      }
    }
    timetable_content.push_back(hbox(floor_nav) | hcenter);
    timetable_content.push_back(separator());

    Elements day_nav;
    for (int i = 0; i < day_names.size(); i++) {
      if (i == current_day) {
        day_nav.push_back(text("[" + day_names[i] + "]") | color(Color::Green) |
                          bold);
      } else {
        day_nav.push_back(text(day_names[i]) | color(Color::GrayDark));
      }
      if (i < day_names.size() - 1) {
        day_nav.push_back(text(" | ") | color(Color::GrayDark));
      }
    }
    timetable_content.push_back(hbox(day_nav) | hcenter);
    timetable_content.push_back(separator());

    // Function to create timetable for current floor and day
    auto create_floor_timetable = [&](const string& floor_name,
                                      const vector<string>& rooms,
                                      const string& day) {
      Elements floor_content;
      floor_content.push_back(text(floor_name + " - " + day) | bold |
                              color(Color::Green) | hcenter);
      floor_content.push_back(separator());

      Elements header_row;
      header_row.push_back(text("Time") | color(Color::Yellow) |
                           size(WIDTH, EQUAL, 8));
      for (const auto& room : rooms) {
        header_row.push_back(text(room) | color(Color::Yellow) |
                             size(WIDTH, EQUAL, 12));
      }
      floor_content.push_back(hbox(header_row) | hcenter);
      floor_content.push_back(separator());

      for (const auto& time : time_slots) {
        Elements time_row;
        time_row.push_back(text(time) | color(Color::Cyan) |
                           size(WIDTH, EQUAL, 8));

        for (const auto& room : rooms) {
          string cell_content = "";
          bool is_occupied = false;
          string course_id = "";

          for (const auto& course : courses) {
            string course_room = get<2>(course);
            string course_time = get<5>(course);
            string course_date = get<4>(course);

            if (course_room == room) {
              bool day_match = false;
              if (course_date.find(day) != string::npos) {
                day_match = true;
              }

              if (day_match) {
                string course_start_time = course_time;
                string course_end_time = "";

                int start_hour = stoi(course_start_time.substr(0, 2));
                int start_minute = stoi(course_start_time.substr(3, 2));

                int end_hour = start_hour + 2;
                int end_minute = start_minute;

                if (end_hour >= 24) {
                  end_hour -= 24;
                }

                char end_time_str[6];
                sprintf(end_time_str, "%02d:%02d", end_hour, end_minute);
                course_end_time = string(end_time_str);

                if (time >= course_start_time && time < course_end_time) {
                  is_occupied = true;
                  course_id = get<0>(course);
                  break;
                }
              }
            }
          }

          if (is_occupied) {
            int color_seed = 0;
            for (char c : course_id) {
              color_seed += (int)c;
            }
            color_seed = color_seed % 8;  // 8 different colors

            Color bg_colors[] = {
                Color::Blue, Color::Green, Color::Yellow,    Color::Magenta,
                Color::Cyan, Color::White, Color::GrayLight, Color::BlueLight};
            Color text_colors[] = {Color::White, Color::Black, Color::Black,
                                   Color::White, Color::Black, Color::Black,
                                   Color::Black, Color::Black};

            time_row.push_back(
                text(course_id) | color(text_colors[color_seed]) |
                bgcolor(bg_colors[color_seed]) | size(WIDTH, EQUAL, 12));
          } else {
            time_row.push_back(text("---") | color(Color::GrayDark) |
                               size(WIDTH, EQUAL, 12));
          }
        }
        floor_content.push_back(hbox(time_row) | hcenter);
      }

      return floor_content;
    };

    if (courses.empty()) {
      timetable_content.push_back(text("No courses scheduled.") |
                                  color(Color::Red) | hcenter);
    } else {
      auto current_floor_content = create_floor_timetable(
          floor_names[current_floor], all_rooms[current_floor],
          day_names[current_day]);
      timetable_content.insert(timetable_content.end(),
                               current_floor_content.begin(),
                               current_floor_content.end());
    }

    timetable_content.push_back(separator());
    timetable_content.push_back(text("Navigation: ????Arrow Keys (Floor) | "
                                     "????Arrow Keys (Day) | ESC: Quit") |
                                color(Color::GrayDark) | hcenter);
    timetable_content.push_back(back_button->Render() | hcenter);

    return vbox(timetable_content) | border;
  });

  auto event_handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Escape) {
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::ArrowLeft) {
      current_floor = (current_floor - 1 + 5) % 5;  // Wrap around
      return true;
    }
    if (event == Event::ArrowRight) {
      current_floor = (current_floor + 1) % 5;  // Wrap around
      return true;
    }
    if (event == Event::ArrowUp) {
      current_day = (current_day - 1 + 5) % 5;  // Wrap around
      return true;
    }
    if (event == Event::ArrowDown) {
      current_day = (current_day + 1) % 5;  // Wrap around
      return true;
    }
    return false;
  });

  screen.Loop(event_handler);
  system("cls");
  menu_admin();
}

// student's course registration function
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
           }) |
           border;
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

bool course_exists(string CourseID) {
  ifstream course("course.txt");
  string line;
  while (getline(course, line)) {
    if (line == CourseID) {
      course.close();
      return true;
    }
    for (int i = 0; i < 6; i++) {
      getline(course, line);
    }
  }
  course.close();
  return false;
}

// write in course function
void write_in_course(string CourseID,
                     string CourseName,
                     string CourseRoom,
                     string CourseCredit,
                     string CourseDate,
                     string CourseTime) {
  ofstream course("course.txt", ios::app);
  course << CourseID << endl;
  course << CourseName << endl;
  course << CourseRoom << endl;
  course << CourseCredit << endl;
  course << CourseDate << endl;
  course << CourseTime << endl;
  course << endl;
  course.close();
}

// clash timetable function
void clash_timetable(string CourseID,
                     string CourseName,
                     string CourseRoom,
                     string CourseCredit,
                     string CourseDate,
                     string CourseTime) {
  system("cls");
  auto clash_screen = ScreenInteractive::TerminalOutput();

  vector<tuple<string, string, string, string, string, string>> courses;
  ifstream fin("course.txt");
  string courseID, courseName, courseRoom, courseCredit, courseDate, courseTime;

  while (getline(fin, courseID)) {
    if (!courseID.empty()) {
      getline(fin, courseName);
      getline(fin, courseRoom);
      getline(fin, courseCredit);
      getline(fin, courseDate);
      getline(fin, courseTime);
      string empty_line;
      getline(fin, empty_line);
      courses.push_back({courseID, courseName, courseRoom, courseCredit,
                         courseDate, courseTime});
    }
  }
  fin.close();

  courses.push_back(
      {CourseID, CourseName, CourseRoom, CourseCredit, CourseDate, CourseTime});

  string floor_name = "UNKNOWN FLOOR";
  vector<string> floor_rooms;
  if (CourseRoom[0] == 'G') {
    floor_name = "G FLOOR";
    floor_rooms = {"G001", "G002", "G003", "G004", "G005",
                   "G006", "G007", "G008", "G009", "G010"};
  } else if (CourseRoom[0] == '1') {
    floor_name = "1ST FLOOR";
    floor_rooms = {"1001", "1002", "1003", "1004", "1005",
                   "1006", "1007", "1008", "1009", "1010"};
  } else if (CourseRoom[0] == '2') {
    floor_name = "2ND FLOOR";
    floor_rooms = {"2001", "2002", "2003", "2004", "2005",
                   "2006", "2007", "2008", "2009", "2010"};
  } else if (CourseRoom[0] == '3') {
    floor_name = "3RD FLOOR";
    floor_rooms = {"3001", "3002", "3003", "3004", "3005",
                   "3006", "3007", "3008", "3009", "3010"};
  } else if (CourseRoom[0] == '4') {
    floor_name = "4TH FLOOR";
    floor_rooms = {"4001", "4002", "4003", "4004", "4005",
                   "4006", "4007", "4008", "4009", "4010"};
  }

  vector<string> time_slots = {
      "08:00", "08:30", "09:00", "09:30", "10:00", "10:30", "11:00", "11:30",
      "12:00", "12:30", "13:00", "13:30", "14:00", "14:30", "15:00", "15:30",
      "16:00", "16:30", "17:00", "17:30", "18:00", "18:30"};

  auto renderer = Renderer([&] {
    Elements clash_content;

    clash_content.push_back(text("TIMETABLE CLASH DETECTED!") | bold |
                            color(Color::Red) | hcenter);
    clash_content.push_back(separator());
    clash_content.push_back(text("Conflicting courses are marked in RED") |
                            color(Color::Red) | hcenter);
    clash_content.push_back(separator());

    clash_content.push_back(text(floor_name) | bold | color(Color::Green) |
                            hcenter);
    clash_content.push_back(separator());

    Elements header_row;
    header_row.push_back(text("Time") | color(Color::Yellow) |
                         size(WIDTH, EQUAL, 8));
    for (const auto& room : floor_rooms) {
      header_row.push_back(text(room) | color(Color::Yellow) |
                           size(WIDTH, EQUAL, 12));
    }
    clash_content.push_back(hbox(header_row) | hcenter);
    clash_content.push_back(separator());

    for (const auto& time : time_slots) {
      Elements time_row;
      time_row.push_back(text(time) | color(Color::Cyan) |
                         size(WIDTH, EQUAL, 8));

      for (const auto& room : floor_rooms) {
        string cell_content = "";
        bool is_clash = false;

        for (const auto& course : courses) {
          string course_room = get<2>(course);
          string course_time = get<5>(course);
          string course_date = get<4>(course);

          if (course_room == room) {
            bool day_match = false;
            if (course_date.find(CourseDate) != string::npos) {
              day_match = true;
            }

            if (day_match) {
              string course_start_time = course_time;

              int start_hour = stoi(course_start_time.substr(0, 2));
              int start_minute = stoi(course_start_time.substr(3, 2));

              int end_hour = start_hour + 2;
              int end_minute = start_minute;

              if (end_hour >= 24) {
                end_hour -= 24;
              }

              char end_time_str[6];
              sprintf(end_time_str, "%02d:%02d", end_hour, end_minute);
              string course_end_time = string(end_time_str);

              if (time >= course_start_time && time < course_end_time) {
                cell_content = get<0>(course);
                if (course_room == CourseRoom) {
                  bool day_overlap = false;
                  if (course_date.find(CourseDate) != string::npos &&
                      CourseDate.find(CourseDate) != string::npos) {
                    day_overlap = true;
                  }

                  if (day_overlap) {
                    string new_start = CourseTime;
                    int new_hour = stoi(new_start.substr(0, 2));
                    int new_minute = stoi(new_start.substr(3, 2));
                    int new_end_hour = new_hour + 2;
                    if (new_end_hour >= 24)
                      new_end_hour -= 24;

                    if (!(new_end_hour <= start_hour || end_hour <= new_hour)) {
                      is_clash = true;
                    }
                  }
                }
              }
            }
          }
        }

        if (CourseRoom == room && CourseDate.find(CourseDate) != string::npos) {
          string new_start = CourseTime;
          int new_hour = stoi(new_start.substr(0, 2));
          int new_minute = stoi(new_start.substr(3, 2));
          int new_end_hour = new_hour + 2;
          if (new_end_hour >= 24)
            new_end_hour -= 24;

          char new_end_time_str[6];
          sprintf(new_end_time_str, "%02d:%02d", new_end_hour, new_minute);
          string new_end_time = string(new_end_time_str);

          if (time >= CourseTime && time < new_end_time) {
            if (cell_content.empty()) {
              cell_content = CourseID;
              is_clash = true;
            } else {
              cell_content += "/" + CourseID;
              is_clash = true;
            }
          }
        }

        if (is_clash) {
          time_row.push_back(text(cell_content) | color(Color::White) |
                             bgcolor(Color::Red) | size(WIDTH, EQUAL, 12));
        } else if (!cell_content.empty()) {
          time_row.push_back(text(cell_content) | color(Color::White) |
                             bgcolor(Color::Blue) | size(WIDTH, EQUAL, 12));
        } else {
          time_row.push_back(text("---") | color(Color::GrayDark) |
                             size(WIDTH, EQUAL, 12));
        }
      }
      clash_content.push_back(hbox(time_row) | hcenter);
    }

    clash_content.push_back(separator());
    clash_content.push_back(text("Press ESC to return to management") |
                            color(Color::GrayDark) | hcenter);

    return vbox(clash_content) | border;
  });

  auto event_handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Escape) {
      clash_screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  clash_screen.Loop(event_handler);
  system("cls");
}

// clash test function
bool clashtest(string CourseID,
               string CourseName,
               string CourseRoom,
               string CourseCredit,
               string CourseDate,
               string CourseTime) {
  ifstream course("course.txt");
  string courseID, courseName, courseRoom, courseCredit, courseDate, courseTime;

  while (getline(course, courseID)) {
    if (!courseID.empty()) {
      getline(course, courseName);
      getline(course, courseRoom);
      getline(course, courseCredit);
      getline(course, courseDate);
      getline(course, courseTime);
      string empty_line;
      getline(course, empty_line);

      // Ignore the same course ID to prevent self-conflict when editing
      if (courseID == CourseID) {
        continue;
      }

      if (courseRoom == CourseRoom) {
        bool day_overlap = false;

        vector<string> new_days;
        string new_date_copy = CourseDate;
        size_t pos = 0;
        while ((pos = new_date_copy.find(" ")) != string::npos) {
          new_days.push_back(new_date_copy.substr(0, pos));
          new_date_copy.erase(0, pos + 1);
        }
        new_days.push_back(new_date_copy);

        vector<string> existing_days;
        string existing_date_copy = courseDate;
        pos = 0;
        while ((pos = existing_date_copy.find(" ")) != string::npos) {
          existing_days.push_back(existing_date_copy.substr(0, pos));
          existing_date_copy.erase(0, pos + 1);
        }
        existing_days.push_back(existing_date_copy);

        for (const auto& new_day : new_days) {
          for (const auto& existing_day : existing_days) {
            if (new_day == existing_day) {
              day_overlap = true;
              break;
            }
          }
          if (day_overlap)
            break;
        }

        if (day_overlap) {
          string existing_start = courseTime;
          string new_start = CourseTime;

          int existing_hour = stoi(existing_start.substr(0, 2));
          int existing_minute = stoi(existing_start.substr(3, 2));
          int new_hour = stoi(new_start.substr(0, 2));
          int new_minute = stoi(new_start.substr(3, 2));

          int existing_end_hour = existing_hour + 2;
          int new_end_hour = new_hour + 2;

          if (existing_end_hour >= 24)
            existing_end_hour -= 24;
          if (new_end_hour >= 24)
            new_end_hour -= 24;

          if (!(new_end_hour <= existing_hour ||
                existing_end_hour <= new_hour)) {
            course.close();
            return true;  // Conflict detected
          }
        }
      }
    }
  }
  course.close();
  return false;  // No conflict
}

// add course function
void Add_course() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string CourseID, CourseName, CourseRoom, CourseCredit, CourseDate, CourseTime;
  InputOption opt;
  auto CourseID_input =
      Input(&CourseID, "(4 letters + 4 digits) e.g. MATH3301", opt);
  auto CourseName_input =
      Input(&CourseName, "not longer than 40 e.g. Algebra I", opt);
  auto CourseRoom_input =
      Input(&CourseRoom, "1 letter + 3 digits e.g. C213", opt);
  auto CourseCredit_input =
      Input(&CourseCredit, "number of credits [3 / 6]", opt);
  auto CourseDate_input =
      Input(&CourseDate, "Only one day for 3-credit courses", opt);
  auto CourseTime_input = Input(
      &CourseTime, "Only one time for 3-credit courses [:00 or :30]", opt);
  string error_message;
  auto submit_button = Button("Submit", [&] {
    if (CourseID.empty() || CourseName.empty() || CourseRoom.empty() ||
        CourseCredit.empty() || CourseDate.empty() || CourseTime.empty()) {
      error_message = "All fields are required.";
      return;
    } else if (course_exists(CourseID)) {
      error_message = "Course ID already exists.";
      return;
    }
    if (clashtest(CourseID, CourseName, CourseRoom, CourseCredit, CourseDate,
                  CourseTime)) {
      clash_timetable(CourseID, CourseName, CourseRoom, CourseCredit,
                      CourseDate, CourseTime);
      return;
    }
    write_in_course(CourseID, CourseName, CourseRoom, CourseCredit, CourseDate,
                    CourseTime);
    error_message =
        "Course added successfully! System will turn you back in 1 seconds.";
    std::thread([&screen] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      screen.ExitLoopClosure()();
    }).detach();
  });
  auto cancel_button = Button("Cancel", [&] { screen.ExitLoopClosure()(); });
  auto button_container = Container::Horizontal({submit_button, cancel_button});
  auto main_container = Container::Vertical(
      {CourseID_input, CourseName_input, CourseRoom_input, CourseCredit_input,
       CourseDate_input, CourseTime_input, button_container});
  auto renderer = Renderer(main_container, [&] {
    return vbox(
               {text("Add Course") | bold | color(Color::Blue) | hcenter,
                separator(),
                hbox(text("Course ID:  "), CourseID_input->Render()) | hcenter,
                hbox(text("Course Name:  "), CourseName_input->Render()) |
                    hcenter,
                hbox(text("Course Room:  "), CourseRoom_input->Render()) |
                    hcenter,
                hbox(text("Course Credit:  "), CourseCredit_input->Render()) |
                    hcenter,
                hbox(text("Course Date:  "), CourseDate_input->Render()) |
                    hcenter,
                hbox(text("Course Time:  "), CourseTime_input->Render()) |
                    hcenter,
                separator(),
                hbox({submit_button->Render(), text(" "),
                      cancel_button->Render()}) |
                    hcenter,
                filler(),
                text(error_message) |
                    color(string("Course added successfully!")
                                      .rfind(error_message, 0) == 0
                              ? Color::Green
                              : Color::Red) |
                    hcenter,
                text("Press esc to quit") | color(Color::GrayDark) | hcenter}) |
           border;
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
  manage_courses();
}

void Delete_course() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string ID;
  InputOption opt;
  auto ID_input = Input(&ID, "Enter course ID", opt);
  string error_message;
  bool found = false;
  int tab_index = 0;
  string found_name, found_room, found_credit, found_date, found_time;
  bool should_return = false;
  string new_name, new_room, new_credit, new_date, new_time;

  auto submit_button = Button("Next", [&] {
    if (ID.empty()) {
      error_message = "Please enter course ID.";
      return;
    }

    ifstream fin("course.txt");
    string fileID, fileName, fileRoom, fileCredit, fileDate, fileTime;
    found = false;
    while (getline(fin, fileID)) {
      if (fileID == ID) {
        found = true;
        getline(fin, found_name);
        getline(fin, found_room);
        getline(fin, found_credit);
        getline(fin, found_date);
        getline(fin, found_time);
        string empty_line;
        getline(fin, empty_line);
        break;
      }
      for (int i = 0; i < 6; i++) {
        string temp;
        getline(fin, temp);
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
    vector<tuple<string, string, string, string, string, string>> records;
    ifstream fin("course.txt");
    string fileID, fileName, fileRoom, fileCredit, fileDate, fileTime;
    while (getline(fin, fileID)) {
      if (fileID != ID) {
        getline(fin, fileName);
        getline(fin, fileRoom);
        getline(fin, fileCredit);
        getline(fin, fileDate);
        getline(fin, fileTime);
        string empty_line;
        getline(fin, empty_line);  // Skip empty line
        records.push_back(
            {fileID, fileName, fileRoom, fileCredit, fileDate, fileTime});
      } else {
        for (int i = 0; i < 6; i++) {
          string temp;
          getline(fin, temp);
        }
      }
    }
    fin.close();
    ofstream fout("course.txt");
    for (auto& rec : records) {
      fout << get<0>(rec) << endl;
      fout << get<1>(rec) << endl;
      fout << get<2>(rec) << endl;
      fout << get<3>(rec) << endl;
      fout << get<4>(rec) << endl;
      fout << get<5>(rec) << endl;
      fout << endl;
    }
    fout.close();
    error_message = "Course deleted! Returning in 1 second.";
    std::thread([&screen] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      screen.ExitLoopClosure()();
    }).detach();
  });

  auto cancel_button_1 = Button("Cancel", [&] {
    should_return = true;
    screen.ExitLoopClosure()();
  });
  auto cancel_button_2 = Button("Cancel", [&] {
    should_return = true;
    screen.ExitLoopClosure()();
  });

  auto id_container =
      Container::Vertical({ID_input, submit_button, cancel_button_1});

  auto confirm_container =
      Container::Vertical({confirm_button, cancel_button_2});

  auto main_container =
      Container::Tab({id_container, confirm_container}, &tab_index);

  auto renderer = Renderer(main_container, [&] {
    if (tab_index == 0) {
      return vbox(
                 {text("Delete Course - Enter ID") | bold | color(Color::Blue) |
                      hcenter,
                  separator(),
                  hbox(text("ID:  "), ID_input->Render()) | hcenter,
                  hbox(submit_button->Render(), text(" "),
                       cancel_button_1->Render()) |
                      hcenter,
                  filler(),
                  text(error_message) |
                      color(string("Course deleted!").rfind(error_message, 0) ==
                                    0
                                ? Color::Green
                                : Color::Red) |
                      hcenter,
                  text("Press esc to quit") | color(Color::GrayDark) |
                      hcenter}) |
             border;
    } else {
      return vbox(
                 {text("Delete Course - Confirm") | bold | color(Color::Blue) |
                      hcenter,
                  separator(),
                  text("Are you sure you want to delete this course?") |
                      color(Color::Red) | hcenter,
                  hbox(text("ID: "), text(ID) | color(Color::Blue)) | hcenter,
                  hbox(text("Name: "), text(found_name) | color(Color::Blue)) |
                      hcenter,
                  hbox(text("Room: "), text(found_room) | color(Color::Blue)) |
                      hcenter,
                  hbox(text("Credit: "),
                       text(found_credit) | color(Color::Blue)) |
                      hcenter,
                  hbox(text("Date: "), text(found_date) | color(Color::Blue)) |
                      hcenter,
                  hbox(text("Time: "), text(found_time) | color(Color::Blue)) |
                      hcenter,
                  hbox(confirm_button->Render(), text(" "),
                       cancel_button_2->Render()) |
                      hcenter,
                  filler(),
                  text(error_message) |
                      color(string("Course deleted!").rfind(error_message, 0) ==
                                    0
                                ? Color::Green
                                : Color::Red) |
                      hcenter,
                  text("Press esc to quit") | color(Color::GrayDark) |
                      hcenter}) |
             border;
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
  if (should_return)
    return;
  menu_admin();
}

void Edit_course() {
  system("cls");
  string new_name, new_room, new_credit, new_date, new_time;
  auto screen = ScreenInteractive::TerminalOutput();
  string ID;
  InputOption opt;
  auto ID_input = Input(&ID, "Enter course ID", opt);
  string error_message;
  bool found = false;
  int tab_index = 0;
  string found_name, found_room, found_credit, found_date, found_time;
  bool should_return = false;
  bool should_return_to_mgmt = false;

  auto submit_button = Button("Next", [&] {
    if (ID.empty()) {
      error_message = "Please enter course ID.";
      return;
    }

    ifstream fin("course.txt");
    string fileID, fileName, fileRoom, fileCredit, fileDate, fileTime;
    found = false;
    while (getline(fin, fileID)) {
      if (fileID == ID) {
        found = true;
        getline(fin, found_name);
        getline(fin, found_room);
        getline(fin, found_credit);
        getline(fin, found_date);
        getline(fin, found_time);
        string empty_line;
        getline(fin, empty_line);  // Skip empty line
        break;
      }
      for (int i = 0; i < 6; i++) {
        string temp;
        getline(fin, temp);
      }
    }
    fin.close();
    if (!found) {
      error_message = "Course ID not found.";
      return;
    }
    // Prefill the editing fields with the found values so inputs show defaults
    new_name = found_name;
    new_room = found_room;
    new_credit = found_credit;
    new_date = found_date;
    new_time = found_time;
    tab_index = 1;
    screen.PostEvent(Event::Custom);
  });

  auto name_input = Input(&new_name, "Course Name", opt);
  auto room_input = Input(&new_room, "Course Room", opt);
  auto credit_input = Input(&new_credit, "Course Credit", opt);
  auto date_input = Input(&new_date, "Course Date", opt);
  auto time_input = Input(&new_time, "Course Time", opt);

  auto save_button = Button("Save", [&] {
    if (new_name.empty() || new_room.empty() || new_credit.empty() ||
        new_date.empty() || new_time.empty()) {
      error_message = "All fields are required.";
      return;
    }

    // Clash test before saving (reuse same logic as create)
    if (clashtest(ID, new_name, new_room, new_credit, new_date, new_time)) {
      clash_timetable(ID, new_name, new_room, new_credit, new_date, new_time);
      return;
    }

    vector<tuple<string, string, string, string, string, string>> records;
    ifstream fin("course.txt");
    string fileID, fileName, fileRoom, fileCredit, fileDate, fileTime;
    while (getline(fin, fileID)) {
      if (fileID != ID) {
        getline(fin, fileName);
        getline(fin, fileRoom);
        getline(fin, fileCredit);
        getline(fin, fileDate);
        getline(fin, fileTime);
        string empty_line;
        getline(fin, empty_line);
        records.push_back(
            {fileID, fileName, fileRoom, fileCredit, fileDate, fileTime});
      } else {
        for (int i = 0; i < 6; i++) {
          string temp;
          getline(fin, temp);
        }
      }
    }
    fin.close();

    records.push_back({ID, new_name, new_room, new_credit, new_date, new_time});

    ofstream fout("course.txt");
    for (auto& rec : records) {
      fout << get<0>(rec) << endl;
      fout << get<1>(rec) << endl;
      fout << get<2>(rec) << endl;
      fout << get<3>(rec) << endl;
      fout << get<4>(rec) << endl;
      fout << get<5>(rec) << endl;
      fout << endl;
    }
    fout.close();
    error_message = "Course updated successfully! Returning in 1 second.";
    std::thread([&screen] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      screen.ExitLoopClosure()();
    }).detach();
  });

  auto cancel_button_1 = Button("Cancel", [&] {
    should_return_to_mgmt = true;
    screen.ExitLoopClosure()();
  });
  auto cancel_button_2 = Button("Cancel", [&] {
    should_return_to_mgmt = true;
    screen.ExitLoopClosure()();
  });

  auto id_container =
      Container::Vertical({ID_input, submit_button, cancel_button_1});

  auto edit_container = Container::Vertical(
      {name_input, room_input, credit_input, date_input, time_input,
       Container::Horizontal({save_button, cancel_button_2})});

  auto main_container =
      Container::Tab({id_container, edit_container}, &tab_index);

  auto renderer = Renderer(main_container, [&] {
    if (tab_index == 0) {
      return vbox({text("Edit Course - Enter ID") | bold | color(Color::Blue) |
                       hcenter,
                   separator(),
                   hbox(text("ID:  "), ID_input->Render()) | hcenter,
                   hbox(submit_button->Render(), text(" "),
                        cancel_button_1->Render()) |
                       hcenter,
                   filler(),
                   text(error_message) |
                       color(string("Course updated successfully!")
                                         .rfind(error_message, 0) == 0
                                 ? Color::Green
                                 : Color::Red) |
                       hcenter,
                   text("Press esc to quit") | color(Color::GrayDark) |
                       hcenter}) |
             border;
    } else {
      return vbox({text("Edit Course - Modify Information") | bold |
                       color(Color::Blue) | hcenter,
                   separator(),
                   hbox(text("Course ID: "), text(ID) | color(Color::Blue)) |
                       hcenter,
                   hbox(text("Name: "), name_input->Render()) | hcenter,
                   hbox(text("Room: "), room_input->Render()) | hcenter,
                   hbox(text("Credit: "), credit_input->Render()) | hcenter,
                   hbox(text("Date: "), date_input->Render()) | hcenter,
                   hbox(text("Time: "), time_input->Render()) | hcenter,
                   separator(),
                   hbox(save_button->Render(), text(" "),
                        cancel_button_2->Render()) |
                       hcenter,
                   filler(),
                   text(error_message) |
                       color(string("Course updated successfully!")
                                         .rfind(error_message, 0) == 0
                                 ? Color::Green
                                 : Color::Red) |
                       hcenter,
                   text("Press esc to quit") | color(Color::GrayDark) |
                       hcenter}) |
             border;
    }
  });

  auto event_handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Escape) {
      should_return_to_mgmt = true;
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(event_handler);
  system("cls");
  if (should_return)
    return;
  if (should_return_to_mgmt) {
    manage_courses();
  } else {
    menu_admin();
  }
}

// admin's manage student records function
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
  MenuOption menu_option;
  menu_option.on_enter = [&] {
    next_page = true;
    next_page_index = selected;
    screen.ExitLoopClosure()();
  };
  auto menu = Menu(&entries, &selected, menu_option);

  auto renderer = Renderer(menu, [&] {
    return vbox({
               text("Management Student Records") | bold | color(Color::Blue) |
                   hcenter,
               separator(),
               menu->Render(),
               filler(),
               text("Press esc to quit") | color(Color::GrayDark) | hcenter,
           }) |
           border;
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
      case 0:
        Create_account();
        break;
      case 1:
        Edit_account();
        break;
      case 2:
        Delete_account();
        break;
    }
  }

  system("cls");
  menu_admin();
}

void Create_account() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string ID, new_pw, verify_pw, Name;
  InputOption opt;
  opt.password = true;
  InputOption name_opt;
  auto old_input = Input(&ID, "ID", opt);
  auto name_input = Input(&Name, "Name", name_opt);
  auto new_input = Input(&new_pw, "password", opt);
  auto ver_input = Input(&verify_pw, "Verify password", opt);
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
    error_message =
        "Account registered successfully! System will turn you back in 1 "
        "seconds.";
    screen.ExitLoopClosure()();
  });
  auto cancel_button = Button("Cancel", [&] { screen.ExitLoopClosure()(); });
  auto button_container = Container::Horizontal({submit_button, cancel_button});
  auto main_container = Container::Vertical(
      {old_input, name_input, new_input, ver_input, button_container});
  auto renderer = Renderer(main_container, [&] {
    return vbox(
               {text("Create student account") | bold | color(Color::Blue) |
                    hcenter,
                separator(), hbox(text("ID:  "), old_input->Render()) | hcenter,
                hbox(text("Name:  "), name_input->Render()) | hcenter,
                hbox(text("password:  "), new_input->Render()) | hcenter,
                hbox(text("Verify password: "), ver_input->Render()) | hcenter,
                separator(),
                hbox({submit_button->Render(), text(" "),
                      cancel_button->Render()}) |
                    hcenter,
                 filler(),
                 text(error_message) |
                     color(password_changed ? Color::Green : Color::Red) |
                     hcenter,
                text("Press esc to quit") | color(Color::GrayDark) | hcenter}) |
           border;
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

// admin's edit account function
void Edit_account() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string ID, Name, Password;
  string old_Name, old_Password;
  InputOption opt;
  opt.password = true;
  InputOption name_opt;
  bool found = false;
  string error_message;
  int tab_index = 0;
  bool should_return_to_mgmt = false;

  auto ID_input = Input(&ID, "Enter student ID", name_opt);
  auto Name_input = Input(&Name, "New name (leave blank to keep)", name_opt);
  auto Password_input =
      Input(&Password, "New password (leave blank to keep)", opt);

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
    error_message =
        string("Current name: ") + old_Name + ", password is hidden.";
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

  auto cancel_button_id = Button("Cancel", [&] {
    should_return_to_mgmt = true;
    screen.ExitLoopClosure()();
  });
  auto cancel_button_edit = Button("Cancel", [&] {
    should_return_to_mgmt = true;
    screen.ExitLoopClosure()();
  });

  auto id_container =
      Container::Vertical({ID_input, next_button, cancel_button_id});

  auto edit_container = Container::Vertical(
      {Name_input, Password_input, submit_button, cancel_button_edit});

  auto main_container =
      Container::Tab({id_container, edit_container}, &tab_index);

  auto renderer = Renderer(main_container, [&] {
    if (tab_index == 0) {
      return vbox({text("Edit Account - Finding ID") | bold |
                       color(Color::Blue) | hcenter,
                   separator(),
                   hbox(text("ID:  "), ID_input->Render()) | hcenter,
                   hbox(next_button->Render(), text(" "),
                        cancel_button_id->Render()) |
                       hcenter,
                   filler(),
                   text(error_message) |
                       color(
                           string("Account updated!").rfind(error_message, 0) ==
                                   0
                               ? Color::Green
                               : Color::Red) |
                       hcenter,
                   text("Press esc to quit") | color(Color::GrayDark) |
                       hcenter}) |
             border;
    } else {
      return vbox({text("Edit Account - Update Info") | bold |
                       color(Color::Blue) | hcenter,
                   separator(),
                   text(string("Current name: ") + old_Name) | hcenter,
                   hbox(text("New name:  "), Name_input->Render()) | hcenter,
                   hbox(text("New password:  "), Password_input->Render()) |
                       hcenter,
                   hbox(submit_button->Render(), text(" "),
                        cancel_button_edit->Render()) |
                       hcenter,
                   filler(),
                   text(error_message) |
                       color(
                           string("Account updated!").rfind(error_message, 0) ==
                                   0
                               ? Color::Green
                               : Color::Red) |
                       hcenter,
                   text("Press esc to quit") | color(Color::GrayDark) |
                       hcenter}) |
             border;
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
  if (should_return_to_mgmt) {
    Management_student_records();
  } else {
    menu_admin();
  }
}

// admin's delete account function (to delete the account)
void Delete_account() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string ID;
  InputOption opt;
  auto ID_input = Input(&ID, "Enter student ID", opt);
  string error_message;
  bool found = false;
  int tab_index = 0;
  string found_name;
  bool should_return = false;

  auto submit_button = Button("Next", [&] {
    if (ID.empty()) {
      error_message = "Please enter ID.";
      return;
    }

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

  auto id_container =
      Container::Vertical({ID_input, submit_button, cancel_button_1});

  auto confirm_container =
      Container::Vertical({confirm_button, cancel_button_2});

  auto main_container =
      Container::Tab({id_container, confirm_container}, &tab_index);

  auto renderer = Renderer(main_container, [&] {
    if (tab_index == 0) {
      return vbox({text("Delete Account - Enter ID") | bold |
                       color(Color::Blue) | hcenter,
                   separator(),
                   hbox(text("ID:  "), ID_input->Render()) | hcenter,
                   hbox(submit_button->Render(), text(" "),
                        cancel_button_1->Render()) |
                       hcenter,
                   filler(),
                   text(error_message) |
                       color(
                           string("Account deleted!").rfind(error_message, 0) ==
                                   0
                               ? Color::Green
                               : Color::Red) |
                       hcenter,
                   text("Press esc to quit") | color(Color::GrayDark) |
                       hcenter}) |
             border;
    } else {
      return vbox({text("Delete Account - Confirm") | bold |
                       color(Color::Blue) | hcenter,
                   separator(),
                   text("Are you sure you want to delete this account?") |
                       color(Color::Red) | hcenter,
                   hbox(text("ID: "), text(ID) | color(Color::Blue)) | hcenter,
                   hbox(text("Name: "), text(found_name) | color(Color::Blue)) |
                       hcenter,
                   hbox(confirm_button->Render(), text(" "),
                        cancel_button_2->Render()) |
                       hcenter,
                   filler(),
                   text(error_message) |
                       color(
                           string("Account deleted!").rfind(error_message, 0) ==
                                   0
                               ? Color::Green
                               : Color::Red) |
                       hcenter,
                   text("Press esc to quit") | color(Color::GrayDark) |
                       hcenter}) |
             border;
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
  if (should_return)
    return;
  menu_admin();
}

// main function
int main() {
  login();
}
