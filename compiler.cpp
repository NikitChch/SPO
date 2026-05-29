#include "compiler.h"
#include <limits>
#include <set>
#include <cstdio>
#include <clocale>
#include <sstream>

Compiler::Compiler(const vector<string>& rpn, const vector<SemanticAnalyzer::SymbolEntry>& symbols)
    : programRpn(rpn), symbolTable(symbols) {
}

bool Compiler::generateAndCompile(string baseName) {
    if (!generateCppCode(baseName)) return false;
    if (!compileExecutable(baseName)) return false;
    return true;
}

bool Compiler::generateCppCode(string baseName) {
    string cppFilename = baseName + ".cpp";
    ofstream out(cppFilename);
    if (!out) {
        cerr << "Не удалось создать файл " << cppFilename << endl;
        return false;
    }

    out << "#include <iostream>\n";
    out << "#include <string>\n";
    out << "#include <vector>\n";
    out << "#include <map>\n";
    out << "#include <cctype>\n";
    out << "#include <set>\n";
    out << "#include <limits>\n";
    out << "#include <stdexcept>\n";
    out << "#include <clocale>\n";
    out << "#include <sstream>\n";
    out << "using namespace std;\n\n";

    out << "void setRussianConsole() {\n";
    out << "    setlocale(LC_ALL, \"rus\");\n";
    out << "}\n\n";

    out << "template<typename T>\n";
    out << "T safe_div(T a, T b) {\n";
    out << "    if (b == 0) {\n";
    out << "        throw runtime_error(\"Ошибка: деление на ноль в операции div\");\n";
    out << "    }\n";
    out << "    return a / b;\n";
    out << "}\n\n";

    out << "template<typename T>\n";
    out << "T safe_mod(T a, T b) {\n";
    out << "    if (b == 0) {\n";
    out << "        throw runtime_error(\"Ошибка: деление на ноль в операции mod\");\n";
    out << "    }\n";
    out << "    return a % b;\n";
    out << "}\n\n";

    out << "bool readInteger(int& value) {\n";
    out << "    string line;\n";
    out << "    getline(cin, line);\n";
    out << "    \n";
    out << "    size_t pos = 0;\n";
    out << "    if (line.empty()) return false;\n";
    out << "    \n";
    out << "    while (pos < line.length() && isspace(line[pos])) pos++;\n";
    out << "    if (pos >= line.length()) return false;\n";
    out << "    \n";
    out << "    if (line[pos] == '+' || line[pos] == '-') pos++;\n";
    out << "    if (pos >= line.length() || !isdigit(line[pos])) return false;\n";
    out << "    \n";
    out << "    bool hasDigit = false;\n";
    out << "    while (pos < line.length() && isdigit(line[pos])) {\n";
    out << "        hasDigit = true;\n";
    out << "        pos++;\n";
    out << "    }\n";
    out << "    \n";
    out << "    while (pos < line.length() && isspace(line[pos])) pos++;\n";
    out << "    \n";
    out << "    if (!hasDigit || pos < line.length()) return false;\n";
    out << "    \n";
    out << "    stringstream ss(line);\n";
    out << "    ss >> value;\n";
    out << "    return !ss.fail();\n";
    out << "}\n\n";

    out << "int main() {\n";
    out << "    setRussianConsole();\n";
    out << "    try {\n";

    for (const auto& entry : symbolTable) {
        if (entry.type == "INTEGER") {
            out << "        int " << entry.name << ";\n";
        }
    }
    out << "\n";

    vector<string> exprStack;

    for (size_t i = 0; i < programRpn.size(); ++i) {
        const string& token = programRpn[i];

        if (token == "DECL") {
            if (exprStack.empty()) {
                cerr << "Ошибка генерации: DECL без операнда" << endl;
                return false;
            }
            exprStack.pop_back();
            continue;
        }

        if (token == "ASSIGN") {
            if (exprStack.size() < 2) {
                cerr << "Ошибка генерации: недостаточно операндов для ASSIGN" << endl;
                return false;
            }
            string var = exprStack.back(); exprStack.pop_back();
            string value = exprStack.back(); exprStack.pop_back();
            out << "        " << var << " = " << value << ";\n";
            continue;
        }

        if (token == "READ" || token == "READLN") {
            if (exprStack.empty()) {
                cerr << "Ошибка генерации: READ без операнда" << endl;
                return false;
            }
            string var = exprStack.back(); exprStack.pop_back();
            out << "        while (true) {\n";
            out << "            cout << \"\";\n";
            out << "            if (readInteger(" << var << ")) {\n";
            out << "                break;\n";
            out << "            } else {\n";
            out << "                cout << \"Ошибка! Введите ЦЕЛОЕ число: \";\n";
            out << "            }\n";
            out << "        }\n";
            continue;
        }

        if (token == "WRITE" || token == "WRITELN") {
            if (exprStack.empty()) {
                cerr << "Ошибка генерации: WRITE без операнда" << endl;
                return false;
            }
            string expr = exprStack.back(); exprStack.pop_back();
            out << "        cout << " << expr;
            if (token == "WRITELN") {
                out << " << endl";
            }
            out << ";\n";
            continue;
        }

        if (token == "WRITELN_EMPTY") {
            out << "        cout << endl;\n";
            continue;
        }

        if (token == "READLN_EMPTY") {
            out << "        cin.ignore(numeric_limits<streamsize>::max(), '\\n');\n";
            continue;
        }

        if (token == "LABEL") {
            if (exprStack.empty()) {
                cerr << "Ошибка генерации: LABEL без метки" << endl;
                return false;
            }
            string label = exprStack.back(); exprStack.pop_back();
            out << "        " << label << ":;\n";
            continue;
        }

        if (token == "JZ") {
            if (exprStack.size() < 2) {
                cerr << "Ошибка генерации: недостаточно операндов для JZ" << endl;
                return false;
            }
            string label = exprStack.back(); exprStack.pop_back();
            string cond = exprStack.back(); exprStack.pop_back();
            out << "        if (!(" << cond << ")) goto " << label << ";\n";
            continue;
        }

        if (token == "JMP") {
            if (exprStack.empty()) {
                cerr << "Ошибка генерации: JMP без метки" << endl;
                return false;
            }
            string label = exprStack.back(); exprStack.pop_back();
            out << "        goto " << label << ";\n";
            continue;
        }

        if (token == "NEG") {
            if (exprStack.empty()) {
                cerr << "Ошибка генерации: недостаточно операндов для NEG" << endl;
                return false;
            }
            string operand = exprStack.back(); exprStack.pop_back();
            exprStack.push_back("-(" + operand + ")");
            continue;
        }

        if (token == "POS") {
            if (exprStack.empty()) {
                cerr << "Ошибка генерации: недостаточно операндов для POS" << endl;
                return false;
            }
            continue;
        }

        if (isBinaryOperator(token)) {
            if (exprStack.size() < 2) {
                cerr << "Ошибка генерации: недостаточно операндов для '" << token << "'" << endl;
                return false;
            }
            string right = exprStack.back(); exprStack.pop_back();
            string left = exprStack.back(); exprStack.pop_back();
            string op = cppOperator(token);
            string expr;

            if (token == "div") {
                expr = "safe_div(" + left + ", " + right + ")";
            }
            else if (token == "mod") {
                expr = "safe_mod(" + left + ", " + right + ")";
            }
            else {
                expr = "(" + left + " " + op + " " + right + ")";
            }
            exprStack.push_back(expr);
            continue;
        }

        if (isLabelName(token)) {
            exprStack.push_back(token);
            continue;
        }

        if (isIntegerLiteral(token)) {
            exprStack.push_back(token);
            continue;
        }

        if (isIdentifier(token)) {
            exprStack.push_back(token);
            continue;
        }

        cerr << "Ошибка генерации: неизвестный токен '" << token << "'" << endl;
        return false;
    }

    out << "    } catch (const exception& e) {\n";
    out << "        cerr << e.what() << endl;\n";
    out << "        return 1;\n";
    out << "    }\n";
    out << "    return 0;\n";
    out << "}\n";
    out.close();
    return true;
}

bool Compiler::compileExecutable(string baseName) {
    string cppFilename = baseName + ".cpp";
    string exeFilename = baseName + ".exe";

    string command = "g++ \"" + cppFilename + "\" -o \"" + exeFilename + "\" 2> compile_errors.txt";
    int result = system(command.c_str());

    if (result != 0) {
        ifstream errRead("compile_errors.txt");
        string errLine;
        cerr << "Ошибки компиляции:" << endl;
        while (getline(errRead, errLine)) {
            cerr << errLine << endl;
        }
        return false;
    }

    return true;
}

bool Compiler::isIntegerLiteral(const string& s) {
    if (s.empty()) return false;
    size_t start = (s[0] == '+' || s[0] == '-') ? 1 : 0;
    if (start >= s.size()) return false;
    for (size_t i = start; i < s.size(); ++i)
        if (!isdigit(static_cast<unsigned char>(s[i]))) return false;
    return true;
}

bool Compiler::isIdentifier(const string& s) {
    if (s.empty() || isdigit(s[0])) return false;
    for (char c : s)
        if (!isalnum(static_cast<unsigned char>(c)) && c != '_') return false;
    return true;
}

bool Compiler::isLabelName(const string& s) {
    if (s.empty() || s[0] != 'L') return false;
    for (size_t i = 1; i < s.size(); ++i)
        if (!isdigit(static_cast<unsigned char>(s[i]))) return false;
    return true;
}

string Compiler::cppOperator(const string& op) {
    if (op == "mod") return "%";
    if (op == "div") return "/";
    if (op == "=") return "==";
    if (op == "<>") return "!=";
    return op;
}

bool Compiler::isBinaryOperator(const string& s) {
    static const set<string> ops = { "+", "-", "*", "mod", "div", "<", ">", "<=", ">=", "=", "<>" };
    return ops.count(s) > 0;
}