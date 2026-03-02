#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Course {
    string courseNumber;
    string title;
    vector<string> prerequisites;
};

// --- Helpers ---
static inline string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static inline string toUpper(string s) {
    for (char& ch : s) ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    return s;
}

static vector<string> splitCSVLine(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

// --- Core functions ---
bool loadCoursesFromFile(const string& filename, unordered_map<string, Course>& courses) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file \"" << filename << "\"." << endl;
        return false;
    }

    courses.clear();

    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        vector<string> parts = splitCSVLine(line);
        if (parts.size() < 2) {
            // Not enough fields to be valid (needs courseNumber and title at minimum)
            continue;
        }

        Course c;
        c.courseNumber = toUpper(parts[0]);
        c.title = parts[1];

        // Any remaining fields are prerequisites
        for (size_t i = 2; i < parts.size(); ++i) {
            if (!parts[i].empty()) c.prerequisites.push_back(toUpper(parts[i]));
        }

        courses[c.courseNumber] = c;
    }

    file.close();
    return true;
}

vector<string> getSortedCourseNumbers(const unordered_map<string, Course>& courses) {
    vector<string> keys;
    keys.reserve(courses.size());
    for (const auto& kv : courses) keys.push_back(kv.first);
    sort(keys.begin(), keys.end());
    return keys;
}

void printCourseList(const unordered_map<string, Course>& courses) {
    cout << "Here is a sample schedule:" << endl;

    vector<string> sortedKeys = getSortedCourseNumbers(courses);
    for (const string& key : sortedKeys) {
        auto it = courses.find(key);
        if (it != courses.end()) {
            cout << it->second.courseNumber << ", " << it->second.title << endl;
        }
    }
}

void printSingleCourse(const unordered_map<string, Course>& courses) {
    cout << "What course do you want to know about? ";
    string input;
    getline(cin, input);

    string key = toUpper(trim(input));
    auto it = courses.find(key);

    if (it == courses.end()) {
        cout << "Course not found." << endl;
        return;
    }

    const Course& c = it->second;
    cout << c.courseNumber << ", " << c.title << endl;

    if (c.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    } else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < c.prerequisites.size(); ++i) {
            cout << c.prerequisites[i];
            if (i + 1 < c.prerequisites.size()) cout << ", ";
        }
        cout << endl;
    }
}

void printMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
}

// --- Main ---
int main() {
    unordered_map<string, Course> courses;
    bool loaded = false;

    cout << "Welcome to the course planner." << endl;

    while (true) {
        printMenu();
        cout << "What would you like to do? ";

        string choiceLine;
        getline(cin, choiceLine);
        choiceLine = trim(choiceLine);

        int choice = -1;
        try {
            choice = stoi(choiceLine);
        } catch (...) {
            choice = -1;
        }

        if (choice == 1) {
            cout << "Enter the file name: ";
            string filename;
            getline(cin, filename);
            filename = trim(filename);

            loaded = loadCoursesFromFile(filename, courses);
            // No extra required output, but this helps you debug:
            // if (loaded) cout << "Data loaded." << endl;
        } 
        else if (choice == 2) {
            if (!loaded) {
                cout << "Please load the data structure first (Option 1)." << endl;
            } else {
                printCourseList(courses);
            }
        } 
        else if (choice == 3) {
            if (!loaded) {
                cout << "Please load the data structure first (Option 1)." << endl;
            } else {
                printSingleCourse(courses);
            }
        } 
        else if (choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
            break;
        } 
        else {
            cout << choiceLine << " is not a valid option." << endl;
        }

        // spacing like the sample run
        // (optional, but makes it readable)
        // cout << endl;
    }

    return 0;
}