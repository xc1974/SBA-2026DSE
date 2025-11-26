// This is a course registration system for a university.
// It allows students to register for courses, view their course history, and
// view their timetable. It also allows administrators to manage courses and
// student records. It is built using the FTXUI library for the user interface.
// It is a console application that runs on Windows only.
// I use AI to help me Debug the cmakelist.txt and the main.cpp file (mainly the
// UI functions since I am not familiar with the FTXUI library) Also the
// explanation of the code is in the main.cpp file write by AI.

// C:\Users\Admin\Desktop\SBA\FTXUI\out\build\x64-Debug\bin
//
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

#include <algorithm>
#include <cstdlib>
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
void course_search_and_add();
void course_drop();
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
void timetable_student(string ID);
void show_all_clash_table_edit(string OldID,
                               string OldName,
                               string OldRoom,
                               string OldCredit,
                               string OldDate,
                               string OldTime,
                               string NewID,
                               string NewName,
                               string NewRoom,
                               string NewCredit,
                               string NewDate,
                               string NewTime);
struct Course {
  std::string id;
  std::string name;
  std::string room;
  std::string credit;
  std::string date;
  std::string time;
};

// 從 course.txt 載入所有課程 (簡單實作)
std::vector<Course> load_all_courses() {
  std::vector<Course> courses;
  std::ifstream fin("course.txt");
  std::string cid, cname, croom, ccredit, cdate, ctime;
  while (std::getline(fin, cid)) {
    if (cid.empty()) continue;
    std::getline(fin, cname);
    std::getline(fin, croom);
    std::getline(fin, ccredit);
    std::getline(fin, cdate);
    std::getline(fin, ctime);
    std::string empty;
    std::getline(fin, empty); // skip blank line
    courses.push_back({cid, cname, croom, ccredit, cdate, ctime});
  }
  return courses;
}

// 檢查學生是否已註冊課程
bool is_student_registered(const string& studentID, const string& courseID) {
  ifstream fin("registrations.txt");
  if (!fin) return false;
  
  string sID, cID;
  while (fin >> sID >> cID) {
    if (sID == studentID && cID == courseID) {
      return true;
    }
  }
  return false;
}

// 添加學生課程註冊
bool add_student_registration(const string& studentID, const string& courseID) {
  if (is_student_registered(studentID, courseID)) {
    return false; // 已經註冊過
  }
  
  ofstream fout("registrations.txt", ios::app);
  if (!fout) return false;
  
  fout << studentID << " " << courseID << endl;
  return true;
}

// 移除學生課程註冊
bool remove_student_registration(const string& studentID, const string& courseID) {
  ifstream fin("registrations.txt");
  if (!fin) return false;
  
  vector<pair<string, string>> registrations;
  string sID, cID;
  bool found = false;
  
  while (fin >> sID >> cID) {
    if (sID == studentID && cID == courseID) {
      found = true; // 跳過這一行，不添加到vector中
    } else {
      registrations.push_back({sID, cID});
    }
  }
  fin.close();
  
  if (!found) return false;
  
  ofstream fout("registrations.txt");
  if (!fout) return false;
  
  for (const auto& reg : registrations) {
    fout << reg.first << " " << reg.second << endl;
  }
  return true;
}

// 前置宣告
bool clashtest(string CourseID, string CourseName, string CourseRoom, 
               string CourseCredit, string CourseDate, string CourseTime);


// 前置宣告：get_student_courses 定義在檔案之後，但在多處使用需先宣告
std::vector<std::string> get_student_courses(std::string studentID);

// 實際定義
vector<string> get_student_courses(string studentID) {
  vector<string> registered_courses;

  ifstream fin("registrations.txt");
  if (!fin)
    return registered_courses;

  string sID, cID;
  while (fin >> sID >> cID) {
    if (sID == studentID) {  // 修復：使用參數而不是全局變量
      registered_courses.push_back(cID);
    }
  }
  return registered_courses;
}

// 獲取學生已註冊課程數量
int get_student_course_count(const string& studentID) {
  return static_cast<int>(get_student_courses(studentID).size());
}

// 檢查課程時間衝突（針對學生）
bool check_student_course_clash(const string& studentID, const string& newCourseID) {
  vector<string> studentCourses = get_student_courses(studentID);
  vector<Course> allCourses = load_all_courses();
  
  // 找到新課程的信息
  Course newCourse;
  bool foundNewCourse = false;
  for (const auto& course : allCourses) {
    if (course.id == newCourseID) {
      newCourse = course;
      foundNewCourse = true;
      break;
    }
  }
  
  if (!foundNewCourse) return false;
  
  // 檢查與已註冊課程的衝突
  for (const string& registeredCourseID : studentCourses) {
    for (const auto& course : allCourses) {
      if (course.id == registeredCourseID) {
        // 使用現有的衝突檢測函數
        if (clashtest(newCourse.id, newCourse.name, newCourse.room, 
                     newCourse.credit, newCourse.date, newCourse.time)) {
          return true; // 有衝突
        }
      }
    }
  }
  
  return false; // 沒有衝突
}

// Normalize time string to HH:MM (e.g., "8:00" -> "08:00")
string normalize_time(const string& t) {
  int h = 0, m = 0;
  // Accept formats like H:MM, HH:MM
  std::stringstream ss(t);
  string hs, ms;
  if (std::getline(ss, hs, ':') && std::getline(ss, ms)) {
    try {
      h = stoi(hs);
      m = stoi(ms);
      if (h < 0)
        h = 0;
      if (h > 23)
        h = 23;
      if (m < 0)
        m = 0;
      if (m > 59)
        m = 59;
    } catch (...) {
      h = 0;
      m = 0;
    }
  }
  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
  return string(buf);
}

// 解析時間範圍 (e.g., "09:00-12:00" -> {"09:00", "12:00"})
pair<string, string> parse_time_range(const string& time_str) {
  size_t dash_pos = time_str.find('-');
  if (dash_pos != string::npos) {
    string start_time = time_str.substr(0, dash_pos);
    string end_time = time_str.substr(dash_pos + 1);
    return {normalize_time(start_time), normalize_time(end_time)};
  } else {
    // 如果沒有範圍，假設是2小時課程
    string start = normalize_time(time_str);
    int h = stoi(start.substr(0, 2));
    int m = stoi(start.substr(3, 2));
    h += 2;
    if (h >= 24) h -= 24;
    char buf[6];
    snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
    return {start, string(buf)};
  }
}

// 檢查兩個時間範圍是否重疊
bool time_ranges_overlap(const string& start1, const string& end1, 
                        const string& start2, const string& end2) {
  return !(end1 <= start2 || end2 <= start1);
}

// name modify function (to avoid the seperation between first name and last
// name)
void Name_modify(string s) {
  size_t length = s.length();
  for (size_t i = 0; i < length; i++) {
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
  auto last_click_time =
      std::chrono::steady_clock::now() - std::chrono::milliseconds(1000);
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
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - last_click_time)
                        .count();
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
        timetable_student(ID);
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

  auto last_click_time =
      std::chrono::steady_clock::now() - std::chrono::milliseconds(1000);
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
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - last_click_time)
                        .count();
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
  
  vector<string> student_course_ids = get_student_courses(ID);
  vector<Course> all_courses = load_all_courses();
  vector<Course> student_courses;
  
  // 獲取學生已註冊課程的詳細信息
  for (const string& course_id : student_course_ids) {
    for (const auto& course : all_courses) {
      if (course.id == course_id) {
        student_courses.push_back(course);
        break;
      }
    }
  }
  
  if (student_courses.empty()) {
    auto back_button = Button("Back", [&] { screen.ExitLoopClosure()(); });
    auto renderer = Renderer(back_button, [&] {
      return vbox({
                 text("My Course History") | bold | color(Color::Blue) | hcenter,
                 separator(),
                 text("You are not registered for any courses.") | color(Color::Red) | hcenter,
                 separator(),
                 back_button->Render() | hcenter,
                 text("Press ESC to return") | color(Color::GrayDark) | hcenter,
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
    return;
  }
  
  int selected_course = 0;
  string error_message;
  
  vector<string> course_entries;
  for (const auto& course : student_courses) {
    string display_name = course.name;
    for (auto& ch : display_name) if (ch == '_') ch = ' ';
    
    string entry = course.id + " | " + display_name + " | " + 
                  course.room + " | " + course.credit + " credits | " +
                  course.date + " | " + course.time;
    course_entries.push_back(entry);
  }
  
  MenuOption menu_option;
  menu_option.on_enter = [&] {
    // 當用戶選擇課程時不做任何事，等待點擊按鈕
  };
  auto course_menu = Menu(&course_entries, &selected_course, menu_option);
  
  auto drop_button = Button("Drop Selected Course", [&] {
    if (selected_course >= 0 && selected_course < static_cast<int>(student_courses.size())) {
      Course selected = student_courses[selected_course];
      
      if (remove_student_registration(ID, selected.id)) {
        error_message = "Course " + selected.id + " dropped successfully!";
        
        // 更新課程列表
        student_course_ids = get_student_courses(ID);
        student_courses.clear();
        for (const string& course_id : student_course_ids) {
          for (const auto& course : all_courses) {
            if (course.id == course_id) {
              student_courses.push_back(course);
              break;
            }
          }
        }
        
        // 更新顯示條目
        course_entries.clear();
        for (const auto& course : student_courses) {
          string display_name = course.name;
          for (auto& ch : display_name) if (ch == '_') ch = ' ';
          
          string entry = course.id + " | " + display_name + " | " + 
                        course.room + " | " + course.credit + " credits | " +
                        course.date + " | " + course.time;
          course_entries.push_back(entry);
        }
        
        if (selected_course >= static_cast<int>(course_entries.size()) && !course_entries.empty()) {
          selected_course = static_cast<int>(course_entries.size()) - 1;
        }
      } else {
        error_message = "Failed to drop course. Please try again.";
      }
    }
  });
  
  auto back_button = Button("Back", [&] { screen.ExitLoopClosure()(); });
  
  auto button_container = Container::Horizontal({drop_button, back_button});
  auto main_container = Container::Vertical({course_menu, button_container});
  
  auto renderer = Renderer(main_container, [&] {
    Elements content;
    content.push_back(text("My Course History") | bold | color(Color::Blue) | hcenter);
    content.push_back(separator());
    
    if (student_courses.empty()) {
      content.push_back(text("No registered courses.") | color(Color::Red) | hcenter);
      content.push_back(separator());
      content.push_back(back_button->Render() | hcenter);
    } else {
      content.push_back(text("Your Registered Courses (" + to_string(student_courses.size()) + "/5):") | 
                       bold | color(Color::Green) | hcenter);
      content.push_back(course_menu->Render());
      content.push_back(separator());
      content.push_back(hbox({drop_button->Render(), text(" "), 
                             back_button->Render()}) | hcenter);
    }
    
    content.push_back(separator());
    content.push_back(text(error_message) | 
                     color(error_message.find("successfully") != string::npos ? 
                           Color::Green : Color::Red) | hcenter);
    content.push_back(text("Press ESC to return") | color(Color::GrayDark) | hcenter);
    
    return vbox(content) | border;
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

  auto last_click_time =
      std::chrono::steady_clock::now() - std::chrono::milliseconds(1000);
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
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - last_click_time)
                        .count();
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
      courseTime = normalize_time(courseTime);
      string empty_line;
      getline(fin, empty_line);
      courses.push_back({courseID, courseName, courseRoom, courseCredit,
                         courseDate, courseTime});
    }
  }
  fin.close();

  vector<string> time_slots = {
      "09:00", "09:30", "10:00", "10:30", "11:00", "11:30", "12:00", "12:30",
      "13:00", "13:30", "14:00", "14:30", "15:00", "15:30", "16:00", "16:30",
      "17:00", "17:30", "18:00"};

  vector<string> m1_rooms = {"M101", "M102", "M103", "M104", "M105",
                            "M106", "M107", "M108", "M109", "M110"};
  vector<string> m2_rooms = {"M201", "M202", "M203", "M204", "M205",
                            "M206", "M207", "M208", "M209", "M210"};
  vector<string> c1_rooms = {"C101", "C102", "C103", "C104", "C105",
                                 "C106", "C107", "C108", "C109", "C110"};
  vector<string> c2_rooms = {"C201", "C202", "C203", "C204", "C205",
                                 "C206", "C207", "C208", "C209", "C210"};

  int current_floor = 0;  // 0=M1, 1=M2, 2=C1, 3=C2
  vector<string> floor_names = {"M1 FLOOR", "M2 FLOOR", "C1 FLOOR", "C2 FLOOR"};
  vector<vector<string>> all_rooms = {m1_rooms, m2_rooms, c1_rooms, c2_rooms};

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
                auto time_range = parse_time_range(course_time);
                string course_start_time = time_range.first;
                string course_end_time = time_range.second;

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
    timetable_content.push_back(
        text("Navigation: Up / Down Arrow Keys (Floor) | "
             "Left / Right Arrow Keys (Day) | ESC: Quit") |
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
      current_floor = (current_floor + 1) % 5;  // Wrap around
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

string line;
string course_name;
string room;
string credit;
string days;
string course_time;

void search_course(string find) {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();

  auto to_lower = [](const string& s) {
    string r = s;
    transform(r.begin(), r.end(), r.begin(),
              [](unsigned char c) { return (char)tolower(c); });
    return r;
  };

  string query = to_lower(find);

  // 讀取 course.txt（每筆：ID, Name, Room, Credit, Date, Time, 空行）
  vector<string> results;
  ifstream fin("course.txt");
  if (!fin.is_open()) {
    auto ok = Button("OK", [&] { screen.ExitLoopClosure()(); });
    auto err_renderer = Renderer(ok, [&] {
      return vbox({
                 text("Search error") | bold | color(Color::Red) | hcenter,
                 separator(),
                 text("Cannot open course.txt") | color(Color::Red) | hcenter,
                 separator(),
                 ok->Render() | hcenter,
             }) |
             border;
    });
    auto handler_err = CatchEvent(err_renderer, [&](Event e) {
      if (e == Event::Escape) {
        screen.ExitLoopClosure()();
        return true;
      }
      return false;
    });
    screen.Loop(handler_err);
    system("cls");
    return;
  }

  string cid, cname, croom, ccredit, cdate, ctime;
  while (getline(fin, cid)) {
    if (cid.empty())
      continue;
    getline(fin, cname);
    getline(fin, croom);
    getline(fin, ccredit);
    getline(fin, cdate);
    getline(fin, ctime);
    string empty;
    getline(fin, empty);  // skip separator

    string hay = cid + " " + cname + " " + croom + " " + ccredit + " " + cdate +
                 " " + ctime;
    if (to_lower(hay).find(query) != string::npos) {
      for (auto& ch : cname)
        if (ch == '_')
          ch = ' ';
      results.push_back(cid + " | " + cname + " | " + croom + " | " + ccredit +
                        " | " + cdate + " | " + ctime);
    }
  }
  fin.close();

  auto back = Button("Back", [&] { screen.ExitLoopClosure()(); });

  auto renderer = Renderer(back, [&] {
    Elements content;
    content.push_back(text("Search results for: \"" + find + "\"") | bold |
                      color(Color::Blue) | hcenter);
    content.push_back(separator());

    if (results.empty()) {
      content.push_back(text("No matching courses.") | color(Color::Red) |
                        hcenter);
    } else {
      for (const auto& r : results) {
        content.push_back(text(r) | hcenter);
        content.push_back(text(""));  // small spacing
      }
    }

    content.push_back(separator());
    content.push_back(back->Render() | hcenter);
    content.push_back(text("Press ESC or Enter to return") |
                      color(Color::GrayDark) | hcenter);

    return vbox(content) | border;
  });

  auto handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Escape || event == Event::Return) {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(handler);
  system("cls");
}

// 課程搜索和添加功能
void course_search_and_add() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  
  string query;
  string error_message;
  vector<Course> search_results;
  bool show_results = false;
  int selected_course = 0;
  
  InputOption opt;
  auto query_input = Input(&query, "Enter course ID prefix (e.g., MATH)", opt);
  
  auto search_button = Button("Search", [&] {
    if (query.empty()) {
      error_message = "Please enter a search term.";
      return;
    }
    
    // 搜索課程
    vector<Course> all_courses = load_all_courses();
    search_results.clear();
    
    string query_upper = query;
    transform(query_upper.begin(), query_upper.end(), query_upper.begin(), ::toupper);
    
    for (const auto& course : all_courses) {
      string course_id_upper = course.id;
      transform(course_id_upper.begin(), course_id_upper.end(), course_id_upper.begin(), ::toupper);
      
      if (course_id_upper.find(query_upper) == 0) { // 前綴匹配
        search_results.push_back(course);
      }
    }
    
    if (search_results.empty()) {
      error_message = "No courses found with prefix: " + query;
      show_results = false;
    } else {
      error_message = "Found " + to_string(search_results.size()) + " courses. Select one to add:";
      show_results = true;
      selected_course = 0;
    }
  });
  
  auto add_button = Button("Add Selected Course", [&] {
    if (!show_results || search_results.empty()) {
      error_message = "No course selected.";
      return;
    }
    
    Course selected = search_results[selected_course];
    
    // 檢查是否已經註冊
    if (is_student_registered(ID, selected.id)) {
      error_message = "You are already registered for this course!";
      return;
    }
    
    // 檢查課程數量限制
    if (get_student_course_count(ID) >= 5) {
      error_message = "You cannot register for more than 5 courses!";
      return;
    }
    
    // 檢查時間衝突
    if (check_student_course_clash(ID, selected.id)) {
      error_message = "Time clash detected with your existing courses!";
      return;
    }
    
    // 添加課程
    if (add_student_registration(ID, selected.id)) {
      error_message = "Course " + selected.id + " added successfully!";
      show_results = false;
      search_results.clear();
    } else {
      error_message = "Failed to add course. Please try again.";
    }
  });
  
  auto back_button = Button("Back", [&] { screen.ExitLoopClosure()(); });
  
  vector<string> course_entries;
  MenuOption menu_option;
  menu_option.on_enter = [&] {
    // 選擇課程時不做任何事，等待用戶點擊添加按鈕
  };
  auto course_menu = Menu(&course_entries, &selected_course, menu_option);
  
  auto button_container = Container::Horizontal({search_button, add_button, back_button});
  auto main_container = Container::Vertical({query_input, button_container, course_menu});
  
  auto renderer = Renderer(main_container, [&] {
    Elements content;
    content.push_back(text("Add Course") | bold | color(Color::Blue) | hcenter);
    content.push_back(separator());
    content.push_back(hbox(text("Search: "), query_input->Render()) | hcenter);
    content.push_back(separator());
    content.push_back(hbox({search_button->Render(), text(" "), 
                           add_button->Render(), text(" "), 
                           back_button->Render()}) | hcenter);
    
    if (show_results && !search_results.empty()) {
      content.push_back(separator());
      content.push_back(text("Search Results:") | bold | color(Color::Green) | hcenter);
      
      // 更新課程條目
      course_entries.clear();
      for (const auto& course : search_results) {
        string display_name = course.name;
        for (auto& ch : display_name) if (ch == '_') ch = ' ';
        
        string entry = course.id + " | " + display_name + " | " + 
                      course.room + " | " + course.credit + " credits | " +
                      course.date + " | " + course.time;
        course_entries.push_back(entry);
      }
      
      content.push_back(course_menu->Render());
    }
    
    content.push_back(separator());
    content.push_back(text(error_message) | 
                     color(error_message.find("successfully") != string::npos ? 
                           Color::Green : Color::Red) | hcenter);
    content.push_back(text("Press ESC to return") | color(Color::GrayDark) | hcenter);
    
    return vbox(content) | border;
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
  course_registration();
}

// 課程註冊主選單
void course_registration() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  bool should_quit = false;
  bool next_page = false;
  int next_page_index = -1;

  auto title = text("Course Registration") | bold | color(Color::Blue);

  vector<string> menu_entries = {"Add Course", "Drop Course", "Back"};

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
        course_search_and_add();
        break;
      case 1:
        course_drop();
        break;
      case 2:
        menu_student();
        return;
    }
  } else if (should_quit) {
    system("cls");
    menu_student();
  }
}

// 課程刪除功能
void course_drop() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  
  vector<string> student_course_ids = get_student_courses(ID);
  vector<Course> all_courses = load_all_courses();
  vector<Course> student_courses;
  
  // 獲取學生已註冊課程的詳細信息
  for (const string& course_id : student_course_ids) {
    for (const auto& course : all_courses) {
      if (course.id == course_id) {
        student_courses.push_back(course);
        break;
      }
    }
  }
  
  if (student_courses.empty()) {
    auto ok_button = Button("OK", [&] { screen.ExitLoopClosure()(); });
    auto renderer = Renderer(ok_button, [&] {
      return vbox({
                 text("Drop Course") | bold | color(Color::Blue) | hcenter,
                 separator(),
                 text("You are not registered for any courses.") | color(Color::Red) | hcenter,
                 separator(),
                 ok_button->Render() | hcenter,
                 text("Press ESC to return") | color(Color::GrayDark) | hcenter,
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
    course_registration();
    return;
  }
  
  int selected_course = 0;
  string error_message;
  
  vector<string> course_entries;
  for (const auto& course : student_courses) {
    string display_name = course.name;
    for (auto& ch : display_name) if (ch == '_') ch = ' ';
    
    string entry = course.id + " | " + display_name + " | " + 
                  course.room + " | " + course.credit + " credits | " +
                  course.date + " | " + course.time;
    course_entries.push_back(entry);
  }
  
  MenuOption menu_option;
  menu_option.on_enter = [&] {
    // 當用戶選擇課程時不做任何事，等待點擊刪除按鈕
  };
  auto course_menu = Menu(&course_entries, &selected_course, menu_option);
  
  auto drop_button = Button("Drop Selected Course", [&] {
    if (selected_course >= 0 && selected_course < static_cast<int>(student_courses.size())) {
      Course selected = student_courses[selected_course];
      
      if (remove_student_registration(ID, selected.id)) {
        error_message = "Course " + selected.id + " dropped successfully!";
        
        // 更新課程列表
        student_course_ids = get_student_courses(ID);
        student_courses.clear();
        for (const string& course_id : student_course_ids) {
          for (const auto& course : all_courses) {
            if (course.id == course_id) {
              student_courses.push_back(course);
              break;
            }
          }
        }
        
        // 更新顯示條目
        course_entries.clear();
        for (const auto& course : student_courses) {
          string display_name = course.name;
          for (auto& ch : display_name) if (ch == '_') ch = ' ';
          
          string entry = course.id + " | " + display_name + " | " + 
                        course.room + " | " + course.credit + " credits | " +
                        course.date + " | " + course.time;
          course_entries.push_back(entry);
        }
        
        if (selected_course >= static_cast<int>(course_entries.size()) && !course_entries.empty()) {
          selected_course = static_cast<int>(course_entries.size()) - 1;
        }
      } else {
        error_message = "Failed to drop course. Please try again.";
      }
    }
  });
  
  auto back_button = Button("Back", [&] { screen.ExitLoopClosure()(); });
  
  auto button_container = Container::Horizontal({drop_button, back_button});
  auto main_container = Container::Vertical({course_menu, button_container});
  
  auto renderer = Renderer(main_container, [&] {
    Elements content;
    content.push_back(text("Drop Course") | bold | color(Color::Blue) | hcenter);
    content.push_back(separator());
    
    if (student_courses.empty()) {
      content.push_back(text("No courses to drop.") | color(Color::Red) | hcenter);
    } else {
      content.push_back(text("Your Registered Courses:") | bold | color(Color::Green) | hcenter);
      content.push_back(course_menu->Render());
    }
    
    content.push_back(separator());
    content.push_back(hbox({drop_button->Render(), text(" "), 
                           back_button->Render()}) | hcenter);
    content.push_back(separator());
    content.push_back(text(error_message) | 
                     color(error_message.find("successfully") != string::npos ? 
                           Color::Green : Color::Red) | hcenter);
    content.push_back(text("Press ESC to return") | color(Color::GrayDark) | hcenter);
    
    return vbox(content) | border;
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
  course_registration();
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
  // normalize time on write
  string norm_time = normalize_time(CourseTime);
  ofstream course("course.txt", ios::app);
  course << CourseID << endl;
  course << CourseName << endl;
  course << CourseRoom << endl;
  course << CourseCredit << endl;
  course << CourseDate << endl;
  course << norm_time << endl;
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
      courseTime = normalize_time(courseTime);
      string empty_line;
      getline(fin, empty_line);
      courses.push_back({courseID, courseName, courseRoom, courseCredit,
                         courseDate, courseTime});
    }
  }
  fin.close();

  courses.push_back({CourseID, CourseName, CourseRoom, CourseCredit, CourseDate,
                     normalize_time(CourseTime)});

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
          string course_time = normalize_time(get<5>(course));
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
              snprintf(end_time_str, sizeof(end_time_str), "%02d:%02d", end_hour, end_minute);
              string course_end_time = string(end_time_str);

              if (time >= course_start_time && time < course_end_time) {
                is_clash = true;
                cell_content = get<0>(course);
                break;
              }
            }
          }
        }

        // 比對編輯前後是否有衝突發生，cell_content 可能同時有新舊資料
        if (CourseRoom == room && CourseDate.find(CourseDate) != string::npos) {
          string new_start = normalize_time(CourseTime);
          int new_hour = stoi(new_start.substr(0, 2));
          int new_minute = stoi(new_start.substr(3, 2));
          int new_end_hour = new_hour + 2;
          if (new_end_hour >= 24)
            new_end_hour -= 24;

          char new_end_time_str[6];
          snprintf(new_end_time_str, sizeof(new_end_time_str), "%02d:%02d", new_end_hour, new_minute);
          string new_end_time = string(new_end_time_str);

          if (time >= normalize_time(CourseTime) && time < new_end_time) {
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
          bool is_new = (cell_content == CourseID);
          time_row.push_back(text(cell_content) |
                            color(is_new ? Color::Black : Color::White) |
                            bgcolor(is_new ? Color::Yellow : Color::Blue) |
                            size(WIDTH, EQUAL, 12));
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
      courseTime = normalize_time(courseTime);
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
          auto existing_time_range = parse_time_range(courseTime);
          auto new_time_range = parse_time_range(CourseTime);

          if (time_ranges_overlap(existing_time_range.first, existing_time_range.second,
                                 new_time_range.first, new_time_range.second)) {
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

// show all clashes with before/after compare (for edit twice crash case)
void show_all_clash_table_edit(string OldID,
                               string OldName,
                               string OldRoom,
                               string OldCredit,
                               string OldDate,
                               string OldTime,
                               string NewID,
                               string NewName,
                               string NewRoom,
                               string NewCredit,
                               string NewDate,
                               string NewTime) {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();

  // Load all courses
  vector<tuple<string, string, string, string, string, string>> courses;
  ifstream fin("course.txt");
  string cid, cname, croom, ccredit, cdate, ctime;
  while (getline(fin, cid)) {
    if (!cid.empty()) {
      getline(fin, cname);
      getline(fin, croom);
      getline(fin, ccredit);
      getline(fin, cdate);
      getline(fin, ctime);
      ctime = normalize_time(ctime);
      string empty_line;
      getline(fin, empty_line);
      courses.push_back({cid, cname, croom, ccredit, cdate, ctime});
    }
  }
  fin.close();

  // Include the new edited course as a candidate to visualize
  courses.push_back(
      {NewID, NewName, NewRoom, NewCredit, NewDate, normalize_time(NewTime)});

  // Extract unique rooms involved to build a compact table per floor
  auto build_floor_rooms = [&](const string& room_prefix) {
    vector<string> rooms;
    set<string> seen;
    for (auto& t : courses) {
      string r = get<2>(t);
      if (!r.empty() &&
          (room_prefix == "G" ? r[0] == 'G' : r[0] == room_prefix[0])) {
        if (!seen.count(r)) {
          seen.insert(r);
          rooms.push_back(r);
        }
      }
    }
    sort(rooms.begin(), rooms.end());
    return rooms;
  };

  vector<string> time_slots = {
      "08:00", "08:30", "09:00", "09:30", "10:00", "10:30", "11:00", "11:30",
      "12:00", "12:30", "13:00", "13:30", "14:00", "14:30", "15:00", "15:30",
      "16:00", "16:30", "17:00", "17:30", "18:00", "18:30"};

  auto header_compare =
      hbox({
          text("Before: ") | color(Color::GrayDark),
          text(OldID + " | " + OldName + " | " + OldRoom + " | " + OldCredit +
               " | " + OldDate + " | " + OldTime) |
              color(Color::White),
      }) |
      hcenter;

  auto header_compare2 =
      hbox({
          text("After:  ") | color(Color::GrayDark),
          text(NewID + " | " + NewName + " | " + NewRoom + " | " + NewCredit +
               " | " + NewDate + " | " + NewTime) |
              color(Color::Yellow),
      }) |
      hcenter;

  auto render_floor = [&](const string& label, const vector<string>& rooms) {
    Elements elems;
    elems.push_back(text(label) | bold | color(Color::Green) | hcenter);
    elems.push_back(separator());
    if (rooms.empty()) {
      elems.push_back(text("No rooms on this floor are involved.") |
                      color(Color::GrayDark) | hcenter);
      return elems;
    }
    Elements header_row;
    header_row.push_back(text("Time") | color(Color::Yellow) |
                         size(WIDTH, EQUAL, 8));
    for (const auto& r : rooms)
      header_row.push_back(text(r) | color(Color::Yellow) |
                           size(WIDTH, EQUAL, 12));
    elems.push_back(hbox(header_row) | hcenter);
    elems.push_back(separator());

    auto is_overlap = [&](const string& s1) {
      int h = stoi(s1.substr(0, 2));
      int m = stoi(s1.substr(3, 2));
      int eh = h + 2;
      if (eh >= 24)
        eh -= 24;
      char buf[6];
      snprintf(buf, sizeof(buf), "%02d:%02d", eh, m);
      return string(buf);
    };

    for (const auto& ts : time_slots) {
      Elements row;
      row.push_back(text(ts) | color(Color::Cyan) | size(WIDTH, EQUAL, 8));
      for (const auto& room : rooms) {
        string cell = "";
        bool clash = false, is_new = false;
        for (const auto& c : courses) {
          if (get<2>(c) != room)
            continue;
          // day match with NewDate OR with each own day to list all events same
          // day as new
          bool day_match = false;
          if (!NewDate.empty()) {
            if (get<4>(c).find(NewDate) != string::npos)
              day_match = true;
          }
          if (!day_match)
            continue;
          string st = normalize_time(get<5>(c));
          string et = is_overlap(st);
          if (ts >= st && ts < et) {
            if (!cell.empty())
              cell += "/";
            cell += get<0>(c);
            if (get<0>(c) == NewID)
              is_new = true;
          }
        }
        // mark clash when multiple or when cell is both existing and new
        if (!cell.empty() && cell.find('/') != string::npos)
          clash = true;
        if (clash) {
          row.push_back(text(cell) | color(Color::White) | bgcolor(Color::Red) |
                        size(WIDTH, EQUAL, 12));
        } else if (!cell.empty()) {
          row.push_back(text(cell) |
                        color(is_new ? Color::Black : Color::White) |
                        bgcolor(is_new ? Color::Yellow : Color::Blue) |
                        size(WIDTH, EQUAL, 12));
        } else {
          row.push_back(text("---") | color(Color::GrayDark) |
                        size(WIDTH, EQUAL, 12));
        }
      }
      elems.push_back(hbox(row) | hcenter);
    }
    return elems;
  };

  auto renderer = Renderer([&] {
    Elements content;
    content.push_back(text("ALL CLASHES (after two failed edits)") | bold |
                      color(Color::Red) | hcenter);
    content.push_back(separator());
    content.push_back(header_compare);
    content.push_back(header_compare2);
    content.push_back(separator());

    // G, 1, 2, 3, 4 floors compact tables
    auto g_rooms = build_floor_rooms("G");
    auto f1_rooms = build_floor_rooms("1");
    auto f2_rooms = build_floor_rooms("2");
    auto f3_rooms = build_floor_rooms("3");
    auto f4_rooms = build_floor_rooms("4");

    auto add = [&](const string& label, const vector<string>& rooms) {
      auto elems = render_floor(label, rooms);
      content.insert(content.end(), elems.begin(), elems.end());
      content.push_back(separator());
    };
    add("G FLOOR", g_rooms);
    add("1ST FLOOR", f1_rooms);
    add("2ND FLOOR", f2_rooms);
    add("3RD FLOOR", f3_rooms);
    add("4TH FLOOR", f4_rooms);

    content.push_back(text("Press ESC to go back") | color(Color::GrayDark) |
                      hcenter);
    return vbox(content) | border;
  });

  auto handler = CatchEvent(renderer, [&](Event e) {
    if (e == Event::Escape) {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(handler);
  system("cls");
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

// admin's edit course function
void Edit_course() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string CourseID;
  string CourseName, CourseRoom, CourseCredit, CourseDate, CourseTime;
  string old_Name, old_Room, old_Credit, old_Date, old_Time;
  InputOption opt;
  bool found = false;
  string error_message;
  int tab_index = 0;
  bool should_return = false;
  int edit_fail_count = 0;

  auto CourseID_input = Input(&CourseID, "Enter course ID", opt);
  auto CourseName_input =
      Input(&CourseName, "New name (leave blank to keep)", opt);
  auto CourseRoom_input =
      Input(&CourseRoom, "New room (leave blank to keep)", opt);
  auto CourseCredit_input =
      Input(&CourseCredit, "New credit (leave blank to keep)", opt);
  auto CourseDate_input =
      Input(&CourseDate, "New date (leave blank to keep)", opt);
  auto CourseTime_input =
      Input(&CourseTime, "New time (leave blank to keep)", opt);

  auto next_button = Button("Next", [&] {
    if (CourseID.empty()) {
      error_message = "Please enter course ID.";
      return;
    }
    ifstream fin("course.txt");
    string fileID, fileName, fileRoom, fileCredit, fileDate, fileTime;
    found = false;
    while (getline(fin, fileID)) {
      if (fileID == CourseID) {
        found = true;
        getline(fin, old_Name);
        getline(fin, old_Room);
        getline(fin, old_Credit);
        getline(fin, old_Date);
        getline(fin, old_Time);
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
      error_message = "Course ID not found.";
      return;
    }
    CourseName = "";
    CourseRoom = "";
    CourseCredit = "";
    CourseDate = "";
    CourseTime = "";
    error_message =
        "Current course found. Enter new values (leave blank to keep).";
    tab_index = 1;
    screen.PostEvent(Event::Custom);
  });

  auto submit_button = Button("Submit", [&] {
    if (!found) {
      error_message = "Please enter a valid course ID first.";
      return;
    }

    string newName = CourseName.empty() ? old_Name : CourseName;
    string newRoom = CourseRoom.empty() ? old_Room : CourseRoom;
    string newCredit = CourseCredit.empty() ? old_Credit : CourseCredit;
    string newDate = CourseDate.empty() ? old_Date : CourseDate;
    string newTime = CourseTime.empty() ? old_Time : CourseTime;

    // Check for conflicts (excluding the current course)
    if (clashtest(CourseID, newName, newRoom, newCredit, newDate, newTime)) {
      edit_fail_count++;
      if (edit_fail_count >= 2) {
        show_all_clash_table_edit(CourseID, old_Name, old_Room, old_Credit,
                                  old_Date, old_Time, CourseID, newName,
                                  newRoom, newCredit, newDate, newTime);
        edit_fail_count = 0;
        return;
      } else {
        clash_timetable(CourseID, newName, newRoom, newCredit, newDate, newTime);
        return;
      }
    }

    // Update the course
    vector<tuple<string, string, string, string, string, string>> records;
    ifstream fin("course.txt");
    string fileID, fileName, fileRoom, fileCredit, fileDate, fileTime;
    while (getline(fin, fileID)) {
      if (fileID == CourseID) {
        found = true;
        records.push_back({fileID, newName, newRoom, newCredit, newDate,
                           normalize_time(newTime)});
      } else {
        getline(fin, fileName);
        getline(fin, fileRoom);
        getline(fin, fileCredit);
        getline(fin, fileDate);
        getline(fin, fileTime);
        string empty_line;
        getline(fin, empty_line);
        records.push_back({fileID, fileName, fileRoom, fileCredit, fileDate,
                           normalize_time(fileTime)});
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
    error_message = "Course updated! Returning in 1 second.";
    std::thread([&screen] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      screen.ExitLoopClosure()();
    }).detach();
  });

  auto cancel_button_id = Button("Cancel", [&] {
    should_return = true;
    screen.ExitLoopClosure()();
  });
  auto cancel_button_edit = Button("Cancel", [&] {
    should_return = true;
    screen.ExitLoopClosure()();
  });

  auto id_container =
      Container::Vertical({CourseID_input, next_button, cancel_button_id});
  auto edit_container = Container::Vertical(
      {CourseName_input, CourseRoom_input, CourseCredit_input, CourseDate_input,
       CourseTime_input, submit_button, cancel_button_edit});

  auto main_container =
      Container::Tab({id_container, edit_container}, &tab_index);

  auto renderer = Renderer(main_container, [&] {
    if (tab_index == 0) {
      return vbox(
                 {text("Edit Course - Finding ID") | bold | color(Color::Blue) |
                      hcenter,
                  separator(),
                  hbox(text("Course ID:  "), CourseID_input->Render()) |
                      hcenter,
                  hbox(next_button->Render(), text(" "),
                       cancel_button_id->Render()) |
                      hcenter,
                  filler(),
                  text(error_message) |
                      color(string("Course updated!").rfind(error_message, 0) ==
                                    0
                              ? Color::Green
                              : Color::Red) |
                      hcenter,
                  text("Press esc to quit") | color(Color::GrayDark) |
                      hcenter}) |
             border;
    } else {
      return vbox(
                 {text("Edit Course - Update Info") | bold |
                      color(Color::Blue) | hcenter,
                  separator(), text("Current course: " + CourseID) | hcenter,
                  text("Name: " + old_Name + " | Room: " + old_Room +
                       " | Credit: " + old_Credit) |
                      hcenter,
                  text("Date: " + old_Date + " | Time: " + old_Time) | hcenter,
                  separator(),
                  hbox(text("New name:  "), CourseName_input->Render()) |
                      hcenter,
                  hbox(text("New room:  "), CourseRoom_input->Render()) |
                      hcenter,
                  hbox(text("New credit:  "), CourseCredit_input->Render()) |
                      hcenter,
                  hbox(text("New date:  "), CourseDate_input->Render()) |
                      hcenter,
                  hbox(text("New time:  "), CourseTime_input->Render()) |
                      hcenter,
                  separator(),
                  hbox(submit_button->Render(), text(" "),
                       cancel_button_edit->Render()) |
                      hcenter,
                  filler(),
                  text(error_message) |
                      color(string("Course updated!").rfind(error_message, 0) ==
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
  if (should_return) {
    manage_courses();
  } else {
    manage_courses();
  }
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
    while (fin >> fileID) {
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

// account find function
void account_find(string account) {
  // This function can be used to find account information
  // Currently implemented as a placeholder
  ifstream student("student.txt");
  string line;
  bool found = false;
  while (getline(student, line)) {
    istringstream iss(line);
    string fileID, filePassword, fileusername;
    iss >> fileID >> filePassword >> fileusername;
    if (fileID == account) {
      found = true;
      break;
    }
  }
  student.close();

  if (!found) {
    ifstream admin("admin.txt");
    while (getline(admin, line)) {
      istringstream iss(line);
      string fileID, filePassword, fileusername;
      iss >> fileID >> filePassword >> fileusername;
      if (fileID == account) {
        found = true;
        break;
      }
    }
    admin.close();
  }
}

// admin's manage student records function
void Management_student_records() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  bool should_quit = false;
  bool next_page = false;
  int next_page_index = -1;

  auto title = text("Management Student Records") | bold | color(Color::Blue);

  vector<string> entries = {
      "Create account",
      "Edit account", 
      "Delete account",
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
               title | hcenter,
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

// admin's manage course records function
void Management_course_records() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  bool should_quit = false;
  bool next_page = false;
  int next_page_index = -1;

  auto title = text("Management Course Records") | bold | color(Color::Blue);

  vector<string> entries = {
      "View all courses",
      "Search course",
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
               title | hcenter,
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
        timetable();
        break;
      case 1:
        course_registration();
        break;
    }
  }

  system("cls");
  menu_admin();
}

// Create account function
void Create_account() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string ID, password, Name;
  InputOption opt;
  InputOption password_opt;
  password_opt.password = true;
  
  auto id_input = Input(&ID, "Enter new ID", opt);
  auto name_input = Input(&Name, "Enter name", opt);
  auto password_input = Input(&password, "Enter password", password_opt);
  
  string error_message;
  bool account_created = false;
  
  auto submit_button = Button("Create", [&] {
    if (ID.empty() || Name.empty() || password.empty()) {
      error_message = "All fields are required.";
      return;
    }
    
    // Check if account already exists
    ifstream student("student.txt");
    string line;
    bool exists = false;
    while (getline(student, line)) {
      istringstream iss(line);
      string fileID, filePassword, fileusername;
      iss >> fileID >> filePassword >> fileusername;
      if (fileID == ID) {
        exists = true;
        break;
      }
    }
    student.close();
    
    if (exists) {
      error_message = "Account ID already exists.";
      return;
    }
    
    if (check_pass(password) != "ok") {
      error_message = check_pass(password);
      return;
    }
    
    new_password(ID, password, Name);
    account_created = true;
    error_message = "Account created successfully! Returning in 1 second.";
    std::thread([&screen] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      screen.ExitLoopClosure()();
    }).detach();
  });
  
  auto cancel_button = Button("Cancel", [&] { screen.ExitLoopClosure()(); });
  auto button_container = Container::Horizontal({submit_button, cancel_button});
  auto main_container = Container::Vertical({id_input, name_input, password_input, button_container});
  
  auto renderer = Renderer(main_container, [&] {
    return vbox({
               text("Create Account") | bold | color(Color::Blue) | hcenter,
               separator(),
               hbox(text("ID: "), id_input->Render()) | hcenter,
               hbox(text("Name: "), name_input->Render()) | hcenter,
               hbox(text("Password: "), password_input->Render()) | hcenter,
               separator(),
               hbox({submit_button->Render(), text(" "), cancel_button->Render()}) | hcenter,
               filler(),
               text(error_message) | color(account_created ? Color::Green : Color::Red) | hcenter,
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
  Management_student_records();
}

// Edit account function
void Edit_account() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string ID;
  InputOption opt;
  
  auto id_input = Input(&ID, "Enter account ID to edit", opt);
  string error_message;
  
  auto submit_button = Button("Find", [&] {
    if (ID.empty()) {
      error_message = "Please enter account ID.";
      return;
    }
    
    // Check if account exists
    ifstream student("student.txt");
    string line;
    bool found = false;
    while (getline(student, line)) {
      istringstream iss(line);
      string fileID, filePassword, fileusername;
      iss >> fileID >> filePassword >> fileusername;
      if (fileID == ID) {
        found = true;
        break;
      }
    }
    student.close();
    
    if (!found) {
      error_message = "Account not found.";
      return;
    }
    
    error_message = "Account found. Edit functionality not fully implemented.";
  });
  
  auto cancel_button = Button("Cancel", [&] { screen.ExitLoopClosure()(); });
  auto button_container = Container::Horizontal({submit_button, cancel_button});
  auto main_container = Container::Vertical({id_input, button_container});
  
  auto renderer = Renderer(main_container, [&] {
    return vbox({
               text("Edit Account") | bold | color(Color::Blue) | hcenter,
               separator(),
               hbox(text("ID: "), id_input->Render()) | hcenter,
               separator(),
               hbox({submit_button->Render(), text(" "), cancel_button->Render()}) | hcenter,
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
  Management_student_records();
}

// Delete account function
void Delete_account() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();
  string ID;
  InputOption opt;
  
  auto id_input = Input(&ID, "Enter account ID to delete", opt);
  string error_message;
  bool found = false;
  string found_name;
  int tab_index = 0;
  
  auto find_button = Button("Find", [&] {
    if (ID.empty()) {
      error_message = "Please enter account ID.";
      return;
    }
    
    // Check if account exists
    ifstream student("student.txt");
    string line;
    found = false;
    while (getline(student, line)) {
      istringstream iss(line);
      string fileID, filePassword, fileusername;
      iss >> fileID >> filePassword >> fileusername;
      if (fileID == ID) {
        found = true;
        found_name = fileusername;
        break;
      }
    }
    student.close();
    
    if (!found) {
      error_message = "Account not found.";
      return;
    }
    
    error_message = "Account found. Confirm deletion?";
    tab_index = 1;
    screen.PostEvent(Event::Custom);
  });
  
  auto confirm_button = Button("Delete", [&] {
    // Delete account from file
    vector<string> records;
    ifstream student("student.txt");
    string line;
    while (getline(student, line)) {
      istringstream iss(line);
      string fileID, filePassword, fileusername;
      iss >> fileID >> filePassword >> fileusername;
      if (fileID != ID) {
        records.push_back(line);
      }
    }
    student.close();
    
    ofstream studentOut("student.txt");
    for (const auto& record : records) {
      studentOut << record << endl;
    }
    studentOut.close();
    
    error_message = "Account deleted successfully! Returning in 1 second.";
    std::thread([&screen] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      screen.ExitLoopClosure()();
    }).detach();
  });
  
  auto cancel_button1 = Button("Cancel", [&] { screen.ExitLoopClosure()(); });
  auto cancel_button2 = Button("Cancel", [&] { screen.ExitLoopClosure()(); });
  
  auto find_container = Container::Vertical({id_input, find_button, cancel_button1});
  auto confirm_container = Container::Vertical({confirm_button, cancel_button2});
  auto main_container = Container::Tab({find_container, confirm_container}, &tab_index);
  
  auto renderer = Renderer(main_container, [&] {
    if (tab_index == 0) {
      return vbox({
                 text("Delete Account - Find") | bold | color(Color::Blue) | hcenter,
                 separator(),
                 hbox(text("ID: "), id_input->Render()) | hcenter,
                 separator(),
                 hbox({find_button->Render(), text(" "), cancel_button1->Render()}) | hcenter,
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
                 separator(),
                 hbox({confirm_button->Render(), text(" "), cancel_button2->Render()}) | hcenter,
                 filler(),
                 text(error_message) | color(error_message.find("successfully") != string::npos ? Color::Green : Color::Red) | hcenter,
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
  Management_student_records();
}

// Stage three
// temp global variable to store student ID
// ID = "yys";  // This line causes error, commented out
// end


void timetable_student(string ID) {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();

  // gather student's course objects
  vector<string> my_course_ids = get_student_courses(ID);
  vector<Course> all_courses = load_all_courses();
  vector<Course> my_unique_timetable;
  for (const string& myID : my_course_ids) {
    for (const auto& c : all_courses) {
      if (c.id == myID) {
        my_unique_timetable.push_back(c);
      }
    }
  }

  auto back = Button("Back", [&] { screen.ExitLoopClosure()(); });

  auto renderer = Renderer(back, [&] {
    Elements content;
    content.push_back(text("My Timetable") | bold | color(Color::Blue) |
                      hcenter);
    content.push_back(separator());

    if (my_unique_timetable.empty()) {
      content.push_back(text("No registered courses found.") |
                        color(Color::Red) | hcenter);
      content.push_back(text(""));
    } else {
      // show each course in a readable single-line format
      for (const auto& c : my_unique_timetable) {
        string display_name = c.name;
        for (auto& ch : display_name)
          if (ch == '_')
            ch = ' ';
        string line = c.id + " | " + display_name + " | " + c.room + " | " +
                      c.credit + " | " + c.date + " | " + c.time;
        content.push_back(text(line) | hcenter);
        content.push_back(text(""));  // spacing
      }
    }

    content.push_back(separator());
    content.push_back(back->Render() | hcenter);
    content.push_back(text("Press ESC or Enter to return") |
                      color(Color::GrayDark) | hcenter);

    return vbox(content) | border;
  });

  auto handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Escape || event == Event::Return) {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(handler);
  system("cls");
  menu_student();
}

// main function
int main() {
  login();
  return 0;
}
