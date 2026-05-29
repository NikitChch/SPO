#include "semantic.h"

bool SemanticAnalyzer::isBinaryExpressionOperator(const string& token) {
    static const set<string> ops = { "+", "-", "*", "mod", "div", "<", ">", "<=", ">=", "=", "<>" };
    return ops.count(token) > 0;
}

bool SemanticAnalyzer::isUnaryExpressionOperator(const string& token) {
    return token == "NEG" || token == "POS";
}

SemanticAnalyzer::SymbolEntry* SemanticAnalyzer::lookupVisible(const string& name) {
    for (int i = static_cast<int>(symbols.size()) - 1; i >= 0; --i) {
        if (symbols[i].name == name) {
            return &symbols[i];
        }
    }
    return nullptr;
}

const SemanticAnalyzer::SymbolEntry* SemanticAnalyzer::lookupVisible(const string& name) const {
    for (int i = static_cast<int>(symbols.size()) - 1; i >= 0; --i) {
        if (symbols[i].name == name) {
            return &symbols[i];
        }
    }
    return nullptr;
}

SemanticAnalyzer::SemanticAnalyzer() {
}

string SemanticAnalyzer::createLabel() {
    return "L" + to_string(++labelCounter);
}

bool SemanticAnalyzer::declareSymbol(const string& name, const string& type, bool initialized, string& error) {
    for (const auto& s : symbols) {
        if (s.name == name) {
            error = "повторное объявление идентификатора '" + name + "'";
            return false;
        }
    }

    symbols.push_back({ name, type, true, initialized });
    return true;
}

bool SemanticAnalyzer::ensureDeclared(const string& name, const string& context, string& error) const {
    if (lookupVisible(name) == nullptr) {
        error = "идентификатор '" + name + "' используется " + context + ", но не был объявлен";
        return false;
    }
    return true;
}

bool SemanticAnalyzer::ensureInitialized(const string& name, const string& context, string& error) const {
    const SymbolEntry* entry = lookupVisible(name);
    if (entry == nullptr) {
        error = "идентификатор '" + name + "' используется " + context + ", но не был объявлен";
        return false;
    }
    if (!entry->initialized) {
        error = "идентификатор '" + name + "' используется " + context + ", но не был инициализирован";
        return false;
    }
    return true;
}

void SemanticAnalyzer::markInitialized(const string& name) {
    SymbolEntry* entry = lookupVisible(name);
    if (entry != nullptr) {
        entry->initialized = true;
    }
}

bool SemanticAnalyzer::validateRPN(const vector<string>& rpn, string& error) const {
    int stackDepth = 0;

    for (size_t i = 0; i < rpn.size(); i++) {
        const string& token = rpn[i];

        if (isUnaryExpressionOperator(token)) {
            if (stackDepth < 1) {
                error = "Недостаточно операндов для унарной операции '" + token + "'";
                return false;
            }
            continue;
        }

        if (isBinaryExpressionOperator(token)) {
            if (stackDepth < 2) {
                error = "Недостаточно операндов для бинарной операции '" + token + "'";
                return false;
            }
            stackDepth -= 1;
            continue;
        }

        if (token == "READ" || token == "READLN") {
            if (stackDepth < 1) {
                error = "Недостаточно операндов для операции '" + token + "'";
                return false;
            }
            stackDepth -= 1;
            continue;
        }

        if (token == "WRITE" || token == "WRITELN") {
            if (stackDepth < 1) {
                error = "Недостаточно операндов для операции '" + token + "'";
                return false;
            }
            stackDepth -= 1;
            continue;
        }

        if (token == "WRITELN_EMPTY" || token == "READLN_EMPTY") {
            continue;
        }
        if (token == "DECL") {
            if (stackDepth < 1) {
                error = "Недостаточно операндов для операции '" + token + "'";
                return false;
            }
            stackDepth -= 1;
            continue;
        }

        if (token == "ASSIGN") {
            if (stackDepth < 2) {
                error = "Недостаточно операндов для операции '" + token + "'";
                return false;
            }
            stackDepth -= 2;
            continue;
        }

        if (token == "JZ") {
            if (stackDepth < 2) {
                error = "Недостаточно операндов для операции '" + token + "'";
                return false;
            }
            stackDepth -= 2;
            continue;
        }

        if (token == "JMP") {
            if (stackDepth < 1) {
                error = "Недостаточно операндов для операции '" + token + "'";
                return false;
            }
            stackDepth -= 1;
            continue;
        }

        if (token == "LABEL") {
            if (stackDepth < 1) {
                error = "Недостаточно операндов для операции '" + token + "'";
                return false;
            }
            stackDepth -= 1;
            continue;
        }

        stackDepth += 1;
    }

    if (stackDepth != 0 && stackDepth != 1) {
        error = "Некорректная глубина семантического стека после проверки польской записи (осталось " +
            to_string(stackDepth) + " элементов)";
        return false;
    }

    return true;
}

bool SemanticAnalyzer::addRecord(const string& kind, const vector<string>& rpn, bool validate, string& error) {
    bool valid = true;
    string localError;
    if (validate) {
        valid = validateRPN(rpn, localError);
    }

    records.push_back({ kind, rpn, valid, localError });
    fullProgramRpn.insert(fullProgramRpn.end(), rpn.begin(), rpn.end());
    error = localError;
    return valid;
}

void SemanticAnalyzer::printRecords() const {
    cout << "ПРОМЕЖУТОЧНЫЙ КОД (ОБРАТНАЯ ПОЛЬСКАЯ ЗАПИСЬ)\n";
    if (records.empty()) {
        cout << "(польская запись не построена)\n";
    }
    else {
        for (size_t i = 0; i < records.size(); ++i) {
            cout << i + 1 << ") [" << records[i].kind << "] ";
            for (const auto& token : records[i].rpn) {
                cout << token << ' ';
            }
            if (records[i].valid) {
                cout << "| корректно";
            }
            else {
                cout << "| ошибка: " << records[i].error;
            }
            cout << "\n";
        }
    }
}

void SemanticAnalyzer::printFullProgramRPN() const {
    cout << "\nПОЛНАЯ ПОЛЬСКАЯ ЗАПИСЬ ПРОГРАММЫ:\n";
    if (fullProgramRpn.empty()) {
        cout << "(пусто)\n";
        return;
    }
    for (const auto& token : fullProgramRpn) {
        cout << token << ' ';
    }
    cout << "\n";
}

void SemanticAnalyzer::printSymbolTable() const {
    cout << "\nТАБЛИЦА СИМВОЛОВ\n";
    const int wName = 20;
    const int wType = 16;
    const int wDeclared = 14;
    const int wInit = 18;

    cout << left
        << setw(wName) << "Имя" << " | "
        << setw(wType) << "Тип" << " | "
        << setw(wDeclared) << "Объявлена" << " | "
        << setw(wInit) << "Инициализирована" << "\n";
    cout << string(wName + wType + wDeclared + wInit + 11, '-') << "\n";

    if (symbols.empty()) {
        cout << "(таблица пуста)\n";
        return;
    }

    for (const auto& entry : symbols) {
        cout << left
            << setw(wName) << entry.name << " | "
            << setw(wType) << entry.type << " | "
            << setw(wDeclared) << (entry.declared ? "Да" : "Нет") << " | "
            << setw(wInit) << (entry.initialized ? "Да" : "Нет") << "\n";
    }
}

const vector<string>& SemanticAnalyzer::getFullProgramRpn() const {
    return fullProgramRpn;
}

const vector<SemanticAnalyzer::SymbolEntry>& SemanticAnalyzer::getSymbols() const {
    return symbols;
}