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

// menu function
void menu () {
    cout << studentID << " " << studentName << endl;
    cout << "#1. My course history" << endl;
    cout << "#2 My timetable" << endl;
    cout << "#3 My course registration" << endl;
    cout << "4. Change password" << endl;
    cout << "5. Logout" << endl;
    cout << "Enter your choice:" << endl;
    int choice;
    cin >> choice;
}

int main () {
    menu();
    return 0;
}