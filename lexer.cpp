#include "lexer.h"
#include <cctype>
#include <iomanip>
#include <sstream>

void Lexer::initializeKeywords() {
    vector<string> kw = { "while", "do", "integer", "read", "write", "begin", "end", "mod", "div", "var", "program", "readln", "writeln" };
    for (const auto& word : kw) {
        keywords.push_back(word);
        keywords_map[word] = static_cast<int>(keywords.size() - 1);
    }
}

void Lexer::initializeSeparators() {
    vector<string> sep = { ":=", "+", "-", "*", "(", ")", ";", ",", ":", "<", ">", "<=", ">=", "=", "<>", "." };
    for (const auto& s : sep) {
        separators.push_back(s);
        separators_map[s] = static_cast<int>(separators.size() - 1);
    }
}

void Lexer::skipWhitespace() {
    while (source_pos < source_code.length() && isspace(static_cast<unsigned char>(source_code[source_pos]))) {
        if (source_code[source_pos] == '\n') {
            line++;
            position = 1;
        }
        else {
            position++;
        }
        source_pos++;
    }
}

bool Lexer::isInAlphabet(char c) {
    return alphabet.find(c) != string::npos ||
        string(":+-*()[]{};,<>.=").find(c) != string::npos ||
        isspace(static_cast<unsigned char>(c));
}

void Lexer::addError(char c, int line_num, int pos) {
    errors.push_back({ line_num, pos, c });
}

bool Lexer::processIdentifier() {
    size_t start = source_pos;
    while (source_pos < source_code.length() &&
        (isalnum(static_cast<unsigned char>(source_code[source_pos])) || source_code[source_pos] == '_')) {
        source_pos++;
        position++;
    }

    string ident = source_code.substr(start, source_pos - start);

    if (keywords_map.find(ident) != keywords_map.end()) {
        tokens.push_back({ 3, keywords_map[ident] });
        return true;
    }

    if (symbolic_names_map.find(ident) == symbolic_names_map.end()) {
        symbolic_names.push_back(ident);
        symbolic_names_map[ident] = static_cast<int>(symbolic_names.size() - 1);
    }
    tokens.push_back({ 1, symbolic_names_map[ident] });
    return true;
}

bool Lexer::processNumber() {
    size_t start = source_pos;
    while (source_pos < source_code.length() &&
        isdigit(static_cast<unsigned char>(source_code[source_pos]))) {
        source_pos++;
        position++;
    }

    string num = source_code.substr(start, source_pos - start);

    if (constants_map.find(num) == constants_map.end()) {
        constants.push_back(num);
        constants_map[num] = static_cast<int>(constants.size() - 1);
    }
    tokens.push_back({ 2, constants_map[num] });
    return true;
}

bool Lexer::processSeparator() {
    string sep(1, source_code[source_pos]);

    if (source_pos + 1 < source_code.length()) {
        string two_char = source_code.substr(source_pos, 2);
        if (separators_map.find(two_char) != separators_map.end()) {
            tokens.push_back({ 4, separators_map[two_char] });
            source_pos += 2;
            position += 2;
            return true;
        }
    }

    if (separators_map.find(sep) != separators_map.end()) {
        tokens.push_back({ 4, separators_map[sep] });
        source_pos++;
        position++;
        return true;
    }

    return false;
}

Lexer::Lexer() : line(1), position(1), source_pos(0) {
    initializeKeywords();
    initializeSeparators();
}

void Lexer::analyze(const string& code) {
    source_code = code;
    source_pos = 0;
    line = 1;
    position = 1;

    tokens.clear();
    symbolic_names.clear();
    constants.clear();
    errors.clear();
    symbolic_names_map.clear();
    constants_map.clear();

    while (source_pos < source_code.length()) {
        skipWhitespace();
        if (source_pos >= source_code.length()) break;

        char current = source_code[source_pos];

        if (!isInAlphabet(current)) {
            addError(current, line, position);
            source_pos++;
            position++;
            continue;
        }

        if (current == ':') {
            if (source_pos + 1 < source_code.length() && source_code[source_pos + 1] == '=') {
                processSeparator();
            }
            else {
                tokens.push_back({ 4, separators_map[":"] });
                source_pos++;
                position++;
            }
            continue;
        }

        if (isalpha(static_cast<unsigned char>(current)) || current == '_') {
            processIdentifier();
            continue;
        }

        if (isdigit(static_cast<unsigned char>(current))) {
            processNumber();
            continue;
        }

        if (processSeparator()) {
            continue;
        }

        source_pos++;
        position++;
    }
}

void Lexer::printTables() {
    cout << "ТАБЛИЦЫ ЛЕКСЕРА\n\n";

    cout << "1. Таблица символических имен\n";
    cout << left << setw(20) << "Номер добавления" << " | " << setw(20) << "Лексема" << "\n";
    cout << string(42, '-') << "\n";
    for (size_t i = 0; i < symbolic_names.size(); i++) {
        cout << left << setw(20) << i + 1 << " | " << setw(20) << symbolic_names[i] << "\n";
    }
    if (symbolic_names.empty()) cout << left << setw(20) << "(пусто)" << " | " << setw(20) << "" << "\n";
    cout << "\n";

    cout << "2. Таблица констант\n";
    cout << left << setw(20) << "Номер добавления" << " | " << setw(20) << "Лексема" << "\n";
    cout << string(42, '-') << "\n";
    for (size_t i = 0; i < constants.size(); i++) {
        cout << left << setw(20) << i + 1 << " | " << setw(20) << constants[i] << "\n";
    }
    if (constants.empty()) cout << left << setw(20) << "(пусто)" << " | " << setw(20) << "" << "\n";
    cout << "\n";

    cout << "3. Таблица ключевых слов\n";
    cout << left << setw(20) << "Номер добавления" << " | " << setw(20) << "Лексема" << "\n";
    cout << string(42, '-') << "\n";
    for (size_t i = 0; i < keywords.size(); i++) {
        cout << left << setw(20) << i + 1 << " | " << setw(20) << keywords[i] << "\n";
    }
    cout << "\n";

    cout << "4. Таблица разделителей\n";
    cout << left << setw(20) << "Номер добавления" << " | " << setw(20) << "Лексема" << "\n";
    cout << string(42, '-') << "\n";
    for (size_t i = 0; i < separators.size(); i++) {
        cout << left << setw(20) << i + 1 << " | " << setw(20) << separators[i] << "\n";
    }
    cout << "\n";

    cout << "5. Таблица ошибок\n";
    cout << left << setw(15) << "Номер строки" << " | " << setw(15) << "Номер позиции" << " | " << setw(20) << "Лексема" << "\n";
    cout << string(55, '-') << "\n";
    for (size_t i = 0; i < errors.size(); i++) {
        cout << left << setw(15) << get<0>(errors[i]) << " | "
            << setw(15) << get<1>(errors[i]) << " | "
            << setw(20) << string(1, get<2>(errors[i])) << "\n";
    }
    if (errors.empty()) cout << left << setw(15) << "(ошибок нет)" << " | " << setw(15) << "" << " | " << setw(20) << "" << "\n";
    cout << "\n";

    cout << "Данные анализа\n";
    cout << left << setw(20) << "Номер таблицы" << " | " << setw(20) << "Номер строки" << "\n";
    cout << string(42, '-') << "\n";
    for (size_t i = 0; i < tokens.size(); i++) {
        cout << left << setw(20) << tokens[i].first << " | " << setw(20) << tokens[i].second << "\n";
    }
    if (tokens.empty()) cout << left << setw(20) << "(пусто)" << " | " << setw(20) << "" << "\n";
    cout << "\n";

    cout << "Расшифровка лексем:\n";
    for (size_t i = 0; i < tokens.size(); i++) {
        int table = tokens[i].first;
        int index = tokens[i].second;
        string value;

        switch (table) {
        case 1: value = symbolic_names[index]; break;
        case 2: value = constants[index]; break;
        case 3: value = keywords[index]; break;
        case 4: value = separators[index]; break;
        default: value = "UNKNOWN";
        }

        cout << value << " ";
    }
    cout << "\n";
}

const vector<pair<int, int>>& Lexer::getTokens() const { return tokens; }
const vector<string>& Lexer::getSymbolicNames() const { return symbolic_names; }
const vector<string>& Lexer::getConstants() const { return constants; }
const vector<string>& Lexer::getKeywords() const { return keywords; }
const vector<string>& Lexer::getSeparators() const { return separators; }
const vector<tuple<int, int, char>>& Lexer::getErrors() const { return errors; }