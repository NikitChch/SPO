#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <set>
#include <limits>
#include <stdexcept>
#include <clocale>
#include <sstream>
using namespace std;

void setRussianConsole() {
    setlocale(LC_ALL, "rus");
}

template<typename T>
T safe_div(T a, T b) {
    if (b == 0) {
        throw runtime_error("ќшибка: деление на ноль в операции div");
    }
    return a / b;
}

template<typename T>
T safe_mod(T a, T b) {
    if (b == 0) {
        throw runtime_error("ќшибка: деление на ноль в операции mod");
    }
    return a % b;
}

bool readInteger(int& value) {
    string line;
    getline(cin, line);
    
    size_t pos = 0;
    if (line.empty()) return false;
    
    while (pos < line.length() && isspace(line[pos])) pos++;
    if (pos >= line.length()) return false;
    
    if (line[pos] == '+' || line[pos] == '-') pos++;
    if (pos >= line.length() || !isdigit(line[pos])) return false;
    
    bool hasDigit = false;
    while (pos < line.length() && isdigit(line[pos])) {
        hasDigit = true;
        pos++;
    }
    
    while (pos < line.length() && isspace(line[pos])) pos++;
    
    if (!hasDigit || pos < line.length()) return false;
    
    stringstream ss(line);
    ss >> value;
    return !ss.fail();
}

int main() {
    setRussianConsole();
    try {
        int a;
        int b;
        int c;

        while (true) {
            cout << "";
            if (readInteger(a)) {
                break;
            } else {
                cout << "ќшибка! ¬ведите ÷≈Ћќ≈ число: ";
            }
        }
        while (true) {
            cout << "";
            if (readInteger(b)) {
                break;
            } else {
                cout << "ќшибка! ¬ведите ÷≈Ћќ≈ число: ";
            }
        }
        cout << (a + b);
        c = (a + (b * 2));
        cout << c;
        L1:;
        if (!((a < 10))) goto L2;
        a = (a + 1);
        cout << a;
        goto L1;
        L2:;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
