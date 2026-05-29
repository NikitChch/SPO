#include "parser.h"
#include <iostream>

void SimplePrecedenceParser::initializePrecedenceMatrix() {
    vector<string> terminals = { "$", "i", "(", ")", "u+", "u-",
                                  "+", "-", "*", "mod", "div",
                                  "<", ">", "<=", ">=", "=", "<>" };

    for (const auto& a : terminals) {
        for (const auto& b : terminals) {
            precedenceMatrix[a][b] = "";
        }
    }

    for (const auto& term : terminals) {
        if (term != "$") {
            precedenceMatrix["$"][term] = "<";
        }
    }
    precedenceMatrix["$"]["$"] = "=";

    precedenceMatrix["i"]["i"] = ">";
    precedenceMatrix["i"]["("] = ">";
    precedenceMatrix["i"][")"] = ">";
    precedenceMatrix["i"]["$"] = ">";
    precedenceMatrix["i"]["+"] = ">";
    precedenceMatrix["i"]["-"] = ">";
    precedenceMatrix["i"]["*"] = ">";
    precedenceMatrix["i"]["mod"] = ">";
    precedenceMatrix["i"]["div"] = ">";
    precedenceMatrix["i"]["<"] = ">";
    precedenceMatrix["i"][">"] = ">";
    precedenceMatrix["i"]["<="] = ">";
    precedenceMatrix["i"][">="] = ">";
    precedenceMatrix["i"]["="] = ">";
    precedenceMatrix["i"]["<>"] = ">";
    precedenceMatrix["i"]["u+"] = ">";
    precedenceMatrix["i"]["u-"] = ">";

    precedenceMatrix["("]["("] = "<";
    precedenceMatrix["("][")"] = "=";
    precedenceMatrix["("]["i"] = "<";
    precedenceMatrix["("]["u+"] = "<";
    precedenceMatrix["("]["u-"] = "<";
    precedenceMatrix["("]["$"] = "";

    for (const auto& op : { "+", "-", "*", "mod", "div", "<", ">", "<=", ">=", "=", "<>" }) {
        precedenceMatrix["("][op] = "<";
    }

    precedenceMatrix[")"][")"] = ">";
    precedenceMatrix[")"]["i"] = "";
    precedenceMatrix[")"]["("] = "";
    precedenceMatrix[")"]["$"] = ">";

    for (const auto& op : { "+", "-", "*", "mod", "div", "<", ">", "<=", ">=", "=", "<>" }) {
        precedenceMatrix[")"][op] = ">";
    }
    precedenceMatrix[")"]["u+"] = "";
    precedenceMatrix[")"]["u-"] = "";

    precedenceMatrix["u+"]["i"] = "<";
    precedenceMatrix["u+"]["("] = "<";
    precedenceMatrix["u+"]["u+"] = "<";
    precedenceMatrix["u+"]["u-"] = "<";
    precedenceMatrix["u+"]["$"] = ">";

    for (const auto& op : { "+", "-", "*", "mod", "div", "<", ">", "<=", ">=", "=", "<>" }) {
        precedenceMatrix["u+"][op] = ">";
    }
    precedenceMatrix["u+"][")"] = ">";

    precedenceMatrix["u-"]["i"] = "<";
    precedenceMatrix["u-"]["("] = "<";
    precedenceMatrix["u-"]["u+"] = "<";
    precedenceMatrix["u-"]["u-"] = "<";
    precedenceMatrix["u-"]["$"] = ">";

    for (const auto& op : { "+", "-", "*", "mod", "div", "<", ">", "<=", ">=", "=", "<>" }) {
        precedenceMatrix["u-"][op] = ">";
    }
    precedenceMatrix["u-"][")"] = ">";

    vector<string> compareOps = { "<", ">", "<=", ">=", "=", "<>" };
    vector<string> lowPriority = { "+", "-" };
    vector<string> highPriority = { "*", "mod", "div" };

    for (const auto& op : compareOps) {
        precedenceMatrix[op]["i"] = "<";
        precedenceMatrix[op]["("] = "<";
        precedenceMatrix[op]["u+"] = "<";
        precedenceMatrix[op]["u-"] = "<";
        precedenceMatrix[op][")"] = ">";
        precedenceMatrix[op]["$"] = ">";

        for (const auto& other : compareOps) {
            precedenceMatrix[op][other] = ">";
        }
        for (const auto& other : lowPriority) {
            precedenceMatrix[op][other] = "<";
        }
        for (const auto& other : highPriority) {
            precedenceMatrix[op][other] = "<";
        }
    }

    for (const auto& op : lowPriority) {
        precedenceMatrix[op]["i"] = "<";
        precedenceMatrix[op]["("] = "<";
        precedenceMatrix[op]["u+"] = "<";
        precedenceMatrix[op]["u-"] = "<";
        precedenceMatrix[op][")"] = ">";
        precedenceMatrix[op]["$"] = ">";

        for (const auto& other : compareOps) {
            precedenceMatrix[op][other] = ">";
        }
        for (const auto& other : lowPriority) {
            precedenceMatrix[op][other] = ">";
        }
        for (const auto& other : highPriority) {
            precedenceMatrix[op][other] = "<";
        }
    }

    for (const auto& op : highPriority) {
        precedenceMatrix[op]["i"] = "<";
        precedenceMatrix[op]["("] = "<";
        precedenceMatrix[op]["u+"] = "<";
        precedenceMatrix[op]["u-"] = "<";
        precedenceMatrix[op][")"] = ">";
        precedenceMatrix[op]["$"] = ">";

        for (const auto& other : compareOps) {
            precedenceMatrix[op][other] = ">";
        }
        for (const auto& other : lowPriority) {
            precedenceMatrix[op][other] = ">";
        }
        for (const auto& other : highPriority) {
            precedenceMatrix[op][other] = ">";
        }
    }
}

void SimplePrecedenceParser::printPrecedenceMatrix() const {
    cout << "\nМАТРИЦА ПРЕДШЕСТВОВАНИЯ\n\n";

    set<string> terminals;
    for (const auto& entry : precedenceMatrix) {
        terminals.insert(entry.first);
        for (const auto& rel : entry.second) {
            terminals.insert(rel.first);
        }
    }

    vector<string> sortedTerminals(terminals.begin(), terminals.end());

    cout << "      ";
    for (const auto& term : sortedTerminals) {
        cout << setw(6) << term;
    }
    cout << "\n";
    cout << "    " << string(sortedTerminals.size() * 6, '-') << "\n";

    for (const auto& a : sortedTerminals) {
        cout << setw(4) << a << " |";
        for (const auto& b : sortedTerminals) {
            auto it = precedenceMatrix.find(a);
            if (it != precedenceMatrix.end()) {
                auto rel = it->second.find(b);
                if (rel != it->second.end() && !rel->second.empty()) {
                    cout << setw(6) << rel->second;
                }
                else {
                    cout << setw(6) << "-";
                }
            }
            else {
                cout << setw(6) << "-";
            }
        }
        cout << "\n";
    }
    cout << "\n";
}

void SimplePrecedenceParser::trace(const string& message) {
    if (trace_enabled) {
        cout << "[Шаг " << trace_step++ << "] " << message << endl;
    }
}

string SimplePrecedenceParser::stackToString(const vector<ExprNode>& stackSymbols) const {
    string result;
    for (size_t i = 0; i < stackSymbols.size(); ++i) {
        if (i > 0) result += " ";
        result += stackSymbols[i].symbol;
    }
    return result;
}

string SimplePrecedenceParser::rpnToString(const vector<string>& rpn) const {
    string result;
    for (size_t i = 0; i < rpn.size(); ++i) {
        if (i > 0) result += " ";
        result += rpn[i];
    }
    return result;
}

string SimplePrecedenceParser::getLexemeValue(int table, int index) const {
    switch (table) {
    case 1: return (index < static_cast<int>(symbolic_names.size())) ? symbolic_names[index] : "?";
    case 2: return (index < static_cast<int>(constants.size())) ? constants[index] : "?";
    case 3: return (index < static_cast<int>(keywords.size())) ? keywords[index] : "?";
    case 4: return (index < static_cast<int>(separators.size())) ? separators[index] : "?";
    default: return "?";
    }
}

string SimplePrecedenceParser::lexemeAt(int index) const {
    if (index < 0 || index >= static_cast<int>(tokens.size())) {
        return "EOF";
    }
    return getLexemeValue(tokens[index].first, tokens[index].second);
}

string SimplePrecedenceParser::currentLexemeValue() const {
    return lexemeAt(pos);
}

bool SimplePrecedenceParser::matchType(int table) const {
    return pos < static_cast<int>(tokens.size()) && tokens[pos].first == table;
}

bool SimplePrecedenceParser::expect(const string& lexeme, const string& message) {
    if (currentLexemeValue() == lexeme) {
        trace("Совпадение ожидаемой лексемы '" + lexeme + "'");
        ++pos;
        return true;
    }
    return error(message + ", получено '" + currentLexemeValue() + "'");
}

bool SimplePrecedenceParser::expectIdentifier(const string& message, string* outName) {
    if (!matchType(1)) {
        return error(message + ", получено '" + currentLexemeValue() + "'");
    }
    if (outName) {
        *outName = currentLexemeValue();
    }
    trace("Распознан идентификатор '" + currentLexemeValue() + "'");
    ++pos;
    return true;
}

bool SimplePrecedenceParser::error(const string& message) {
    last_error = "СИНТАКСИЧЕСКАЯ ОШИБКА: " + message +
        " (позиция " + to_string(pos + 1) + ", текущая лексема: '" + currentLexemeValue() + "')";
    trace(last_error);
    return false;
}

bool SimplePrecedenceParser::semanticError(const string& message) {
    last_error = "СЕМАНТИЧЕСКАЯ ОШИБКА: " + message +
        " (позиция " + to_string(pos + 1) + ", текущая лексема: '" + currentLexemeValue() + "')";
    trace(last_error);
    return false;
}

bool SimplePrecedenceParser::semanticErrorAt(const string& message, int errorPos) {
    last_error = "СЕМАНТИЧЕСКАЯ ОШИБКА: " + message +
        " (позиция " + to_string(errorPos + 1) + ", текущая лексема: '" + lexemeAt(errorPos) + "')";
    trace(last_error);
    return false;
}

bool SimplePrecedenceParser::addSemanticRecord(const string& kind, const vector<string>& rpn, bool validate) {
    string semError;
    if (!semantic.addRecord(kind, rpn, validate, semError)) {
        if (validate) {
            return semanticError("некорректная польская запись для конструкции '" + kind + "': " + semError);
        }
    }
    trace("Сформирована польская запись для конструкции '" + kind + "': " + rpnToString(rpn));
    return true;
}

bool SimplePrecedenceParser::isBinaryOperator(const string& s) const {
    static const set<string> ops = { "+", "-", "*", "mod", "div", "<", ">", "<=", ">=", "=", "<>" };
    return ops.count(s) > 0;
}

bool SimplePrecedenceParser::isUnaryOperator(const string& s) const {
    return s == "u+" || s == "u-";
}

int SimplePrecedenceParser::precedenceLevel(const string& op) const {
    if (op == "=" || op == "<>" || op == "<" || op == ">" || op == "<=" || op == ">=") return 1;
    if (op == "+" || op == "-") return 2;
    if (op == "*" || op == "mod" || op == "div") return 3;
    if (op == "u+" || op == "u-") return 4;
    return -1;
}

string SimplePrecedenceParser::precedenceRelation(const string& a, const string& b) const {
    auto it = precedenceMatrix.find(a);
    if (it != precedenceMatrix.end()) {
        auto rel = it->second.find(b);
        if (rel != it->second.end() && !rel->second.empty()) {
            return rel->second;
        }
    }
    return "";
}

string SimplePrecedenceParser::semanticTokenForUnary(const string& symbol) const {
    return symbol == "u-" ? "NEG" : "POS";
}

bool SimplePrecedenceParser::reduce(vector<ExprNode>& stackSymbols) {
    if (stackSymbols.empty()) {
        return error("внутренняя ошибка редукции: пустой стек");
    }

    const int n = static_cast<int>(stackSymbols.size());

    if (n >= 1 && stackSymbols[n - 1].symbol == "i") {
        stackSymbols[n - 1].symbol = "E";
        last_reduction = "E ::= id|const";
        reductions.push_back(last_reduction);
        return true;
    }

    if (n >= 3 && stackSymbols[n - 3].symbol == "(" &&
        stackSymbols[n - 2].symbol == "E" && stackSymbols[n - 1].symbol == ")") {
        vector<string> exprRpn = stackSymbols[n - 2].rpn;
        stackSymbols.erase(stackSymbols.end() - 3, stackSymbols.end());
        stackSymbols.push_back({ "E", exprRpn });
        last_reduction = "E ::= (E)";
        reductions.push_back(last_reduction);
        return true;
    }

    if (n >= 2 && isUnaryOperator(stackSymbols[n - 2].symbol) && stackSymbols[n - 1].symbol == "E") {
        vector<string> exprRpn = stackSymbols[n - 1].rpn;
        exprRpn.push_back(semanticTokenForUnary(stackSymbols[n - 2].symbol));
        string op = stackSymbols[n - 2].symbol == "u-" ? "-" : "+";
        stackSymbols.erase(stackSymbols.end() - 2, stackSymbols.end());
        stackSymbols.push_back({ "E", exprRpn });
        last_reduction = "E ::= " + op + "E";
        reductions.push_back(last_reduction);
        return true;
    }

    if (n >= 3 && stackSymbols[n - 3].symbol == "E" &&
        isBinaryOperator(stackSymbols[n - 2].symbol) && stackSymbols[n - 1].symbol == "E") {
        vector<string> exprRpn = stackSymbols[n - 3].rpn;
        exprRpn.insert(exprRpn.end(), stackSymbols[n - 1].rpn.begin(), stackSymbols[n - 1].rpn.end());
        exprRpn.push_back(stackSymbols[n - 2].symbol);
        string op = stackSymbols[n - 2].symbol;
        stackSymbols.erase(stackSymbols.end() - 3, stackSymbols.end());
        stackSymbols.push_back({ "E", exprRpn });
        last_reduction = "E ::= E " + op + " E";
        reductions.push_back(last_reduction);
        return true;
    }

    return error("невозможно выполнить свертку основы методом простого предшествования");
}

bool SimplePrecedenceParser::parseExpression(const set<string>& stopTokens, vector<string>* outRpn) {
    vector<ExprNode> stackSymbols = { {"$", {}} };
    bool expectOperandNow = true;
    int exprStartPos = pos;

    trace("Начало разбора выражения методом простого предшествования");

    while (true) {
        string lookahead = classifyLookahead(expectOperandNow, stopTokens);

        if (lookahead.empty()) {
            return error("недопустимый символ в выражении: '" + currentLexemeValue() + "'");
        }

        string a = topTerminal(stackSymbols);
        trace("Стек: [" + stackToString(stackSymbols) + "] ; вход: '" + currentLexemeValue() +
            "' ; верхний терминал: '" + a + "' ; lookahead: '" + lookahead + "'");

        if (a == "$" && lookahead == "$" && stackSymbols.size() == 2 && stackSymbols[1].symbol == "E") {
            if (outRpn) {
                *outRpn = stackSymbols[1].rpn;
            }
            trace("Польская запись выражения: " + rpnToString(stackSymbols[1].rpn));
            trace("Выражение разобрано успешно");
            return true;
        }

        string relation = precedenceRelation(a, lookahead);

        if (relation == "<" || relation == "=") {
            if (lookahead == "$") {
                return error("ожидалось выражение перед завершающим символом");
            }
            trace("Отношение '" + relation + "' -> выполняется перенос символа '" + lookahead + "'");
            if (!shiftSymbol(lookahead, expectOperandNow, stackSymbols, exprStartPos)) {
                return false;
            }
        }
        else if (relation == ">") {
            trace("Отношение '>' -> выполняется свертка");
            if (!reduce(stackSymbols)) {
                return false;
            }
            trace("Свертка выполнена: " + last_reduction + "; новый стек: [" + stackToString(stackSymbols) + "]");
            expectOperandNow = false;
        }
        else {
            return error("отсутствует отношение предшествования между '" + a + "' и '" + lookahead + "'");
        }
    }
}

bool SimplePrecedenceParser::consumeOperand(vector<string>& operandRpn, int operandPos) {
    if (matchType(2)) {
        trace("Распознана константа '" + currentLexemeValue() + "'");
        operandRpn = { currentLexemeValue() };
        ++pos;
        return true;
    }

    string ident;
    if (!expectIdentifier("ожидался идентификатор или константа", &ident)) {
        return false;
    }

    string semError;
    if (!semantic.ensureInitialized(ident, "в выражении", semError)) {
        int currentPos = pos;
        pos = operandPos;
        bool result = semanticErrorAt(semError, operandPos);
        pos = currentPos;
        return result;
    }

    operandRpn = { ident };
    return true;
}

string SimplePrecedenceParser::classifyLookahead(bool expectOperandNow, const set<string>& stopTokens) {
    if (pos >= static_cast<int>(tokens.size()) || stopTokens.count(currentLexemeValue()) > 0) {
        return "$";
    }

    string lex = currentLexemeValue();

    if (matchType(1) || matchType(2)) {
        return "i";
    }

    if (lex == "(") return "(";
    if (lex == ")") return ")";

    if (lex == "+" || lex == "-") {
        if (expectOperandNow) {
            return (lex == "+") ? "u+" : "u-";
        }
        return lex;
    }

    if (lex == "*" || lex == "mod" || lex == "div" ||
        lex == "<" || lex == ">" || lex == "<=" || lex == ">=" || lex == "=" || lex == "<>") {
        return lex;
    }

    return "";
}

bool SimplePrecedenceParser::shiftSymbol(const string& symbol, bool& expectOperandNow,
    vector<ExprNode>& stackSymbols, int exprStartPos) {
    if (symbol == "i") {
        vector<string> operandRpn;
        int operandPos = pos;
        if (!consumeOperand(operandRpn, operandPos)) {
            return false;
        }
        stackSymbols.push_back({ "i", operandRpn });
        expectOperandNow = false;
        return true;
    }

    if (symbol == "u+" || symbol == "u-" || symbol == "+" || symbol == "-" ||
        symbol == "*" || symbol == "mod" || symbol == "div" ||
        symbol == "<" || symbol == ">" || symbol == "<=" || symbol == ">=" || symbol == "=" || symbol == "<>" ||
        symbol == "(" || symbol == ")") {
        stackSymbols.push_back({ symbol, {} });
        ++pos;
        expectOperandNow = (symbol != "i" && symbol != ")");
        return true;
    }

    return error("неизвестный символ выражения при переносе: '" + symbol + "'");
}

string SimplePrecedenceParser::topTerminal(const vector<ExprNode>& stackSymbols) const {
    for (int i = static_cast<int>(stackSymbols.size()) - 1; i >= 0; --i) {
        if (stackSymbols[i].symbol != "E") {
            return stackSymbols[i].symbol;
        }
    }
    return "$";
}

bool SimplePrecedenceParser::parseIdentifierList(vector<string>& identifiers, vector<int>* positions) {
    int identPos = pos;
    string ident;

    if (!expectIdentifier("ожидался идентификатор в списке", &ident)) {
        return false;
    }

    identifiers.push_back(ident);
    if (positions) {
        positions->push_back(identPos);
    }

    while (currentLexemeValue() == ",") {
        ++pos;
        identPos = pos;
        if (!expectIdentifier("ожидался идентификатор после запятой", &ident)) {
            return false;
        }
        identifiers.push_back(ident);
        if (positions) {
            positions->push_back(identPos);
        }
    }

    return true;
}

string SimplePrecedenceParser::ioOpcode(const string& ioType) const {
    if (ioType == "read") return "READ";
    if (ioType == "readln") return "READLN";
    if (ioType == "write") return "WRITE";
    if (ioType == "writeln") return "WRITELN";
    return ioType;
}

bool SimplePrecedenceParser::parseIO(const string& ioType) {
    trace("Разбор оператора " + ioType);
    ++pos;

    bool isRead = (ioType == "read" || ioType == "readln");
    bool isWrite = (ioType == "write" || ioType == "writeln");
    bool isLineMode = (ioType == "readln" || ioType == "writeln");

    if (!expect("(", "ожидалась '(' после " + ioType)) {
        return false;
    }

    if (isRead) {
        vector<string> identifiers;
        if (currentLexemeValue() == ")") {
            if (ioType == "readln") {
                vector<string> rpn = { "READLN_EMPTY" };
                if (!addSemanticRecord(ioType, rpn, false)) {
                    return false;
                }
            }
            else {
                return error("оператор read требует хотя бы одного идентификатора");
            }
        }
        else {
            string ident;
            int identPos = pos;
            if (!expectIdentifier("ожидался идентификатор в операторе ввода", &ident)) {
                return false;
            }

            string semError;
            if (!semantic.ensureDeclared(ident, "в операторе ввода", semError)) {
                return semanticErrorAt(semError, identPos);
            }
            semantic.markInitialized(ident);
            identifiers.push_back(ident);

            while (currentLexemeValue() == ",") {
                ++pos;
                identPos = pos;
                if (!expectIdentifier("ожидался идентификатор после запятой", &ident)) {
                    return false;
                }
                if (!semantic.ensureDeclared(ident, "в операторе ввода", semError)) {
                    return semanticErrorAt(semError, identPos);
                }
                semantic.markInitialized(ident);
                identifiers.push_back(ident);
            }

            vector<string> allRpn;
            for (size_t i = 0; i < identifiers.size(); ++i) {
                allRpn.push_back(identifiers[i]);
                allRpn.push_back(isLineMode ? "READLN" : "READ");
            }
            if (!addSemanticRecord(ioType, allRpn, false)) {
                return false;
            }
        }
    }
    else if (isWrite) {
        vector<vector<string>> expressions;
        if (currentLexemeValue() == ")") {
            if (ioType == "writeln") {
                vector<string> rpn = { "WRITELN_EMPTY" };
                if (!addSemanticRecord(ioType, rpn, false)) {
                    return false;
                }
            }
        }
        else {
            vector<string> exprRpn;
            if (!parseExpression({ ",", ")" }, &exprRpn)) {
                return false;
            }
            expressions.push_back(exprRpn);

            while (currentLexemeValue() == ",") {
                ++pos;
                exprRpn.clear();
                if (!parseExpression({ ",", ")" }, &exprRpn)) {
                    return false;
                }
                expressions.push_back(exprRpn);
            }

            vector<string> allRpn;
            for (size_t i = 0; i < expressions.size(); ++i) {
                allRpn.insert(allRpn.end(), expressions[i].begin(), expressions[i].end());
                allRpn.push_back(isLineMode ? "WRITELN" : "WRITE");
            }
            if (!addSemanticRecord(ioType, allRpn, false)) {
                return false;
            }
        }
    }

    if (!expect(")", "ожидалась ')' после списка аргументов")) {
        return false;
    }

    return true;
}

bool SimplePrecedenceParser::parseDeclaration() {
    trace("Разбор объявления переменных");

    if (!expect("var", "ожидалось ключевое слово var")) {
        return false;
    }

    do {
        vector<string> identifiers;
        vector<int> identPositions;

        if (!parseIdentifierList(identifiers, &identPositions)) {
            return false;
        }

        if (!expect(":", "ожидалось ':' после списка идентификаторов")) {
            return false;
        }

        if (!expect("integer", "ожидался тип integer")) {
            return false;
        }

        if (!expect(";", "ожидалась ';' после объявления типа")) {
            return false;
        }

        for (size_t i = 0; i < identifiers.size(); ++i) {
            const string& id = identifiers[i];
            string semError;
            if (!semantic.declareSymbol(id, "INTEGER", false, semError)) {
                return semanticErrorAt(semError, identPositions[i]);
            }

            vector<string> declRpn;
            declRpn.push_back(id);
            declRpn.push_back("DECL");
            if (!addSemanticRecord("declaration", declRpn)) {
                return false;
            }
        }
    } while (matchType(1) && currentLexemeValue() != "var" && currentLexemeValue() != "begin");

    return true;
}

bool SimplePrecedenceParser::parseAssignment() {
    trace("Разбор присваивания");

    int identPos = pos;
    string identifier = currentLexemeValue();
    ++pos;

    string semError;
    if (!semantic.ensureDeclared(identifier, "как целевая переменная", semError)) {
        return semanticErrorAt(semError, identPos);
    }

    if (!expect(":=", "ожидался оператор присваивания ':='")) {
        return false;
    }

    vector<string> exprRpn;
    if (!parseExpression({ ";", "end" }, &exprRpn)) {
        return false;
    }

    vector<string> assignRpn;
    assignRpn.insert(assignRpn.end(), exprRpn.begin(), exprRpn.end());
    assignRpn.push_back(identifier);
    assignRpn.push_back("ASSIGN");

    semantic.markInitialized(identifier);

    if (!addSemanticRecord("assignment", assignRpn)) {
        return false;
    }

    return true;
}

bool SimplePrecedenceParser::parseWhileLoop() {
    trace("Разбор цикла while");

    if (!expect("while", "ожидалось ключевое слово while")) {
        return false;
    }

    string startLabel = semantic.createLabel();
    string endLabel = semantic.createLabel();

    vector<string> startLabelRpn;
    startLabelRpn.push_back(startLabel);
    startLabelRpn.push_back("LABEL");
    if (!addSemanticRecord("label", startLabelRpn, false)) {
        return false;
    }

    vector<string> conditionRpn;
    if (!parseExpression({ "do" }, &conditionRpn)) {
        return false;
    }

    vector<string> jnzRpn;
    jnzRpn.insert(jnzRpn.end(), conditionRpn.begin(), conditionRpn.end());
    jnzRpn.push_back(endLabel);
    jnzRpn.push_back("JZ");

    if (!expect("do", "ожидалось ключевое слово do")) {
        return false;
    }

    if (!addSemanticRecord("while-condition", jnzRpn, false)) {
        return false;
    }

    if (!parseStatement()) {
        return false;
    }

    vector<string> loopBackRpn;
    loopBackRpn.push_back(startLabel);
    loopBackRpn.push_back("JMP");
    if (!addSemanticRecord("while-loop-back", loopBackRpn, false)) {
        return false;
    }

    vector<string> endLabelRpn;
    endLabelRpn.push_back(endLabel);
    endLabelRpn.push_back("LABEL");
    if (!addSemanticRecord("label", endLabelRpn, false)) {
        return false;
    }

    return true;
}

bool SimplePrecedenceParser::parseCompoundStatement() {
    trace("Разбор составного оператора");

    if (!expect("begin", "ожидалось ключевое слово begin")) {
        return false;
    }

    bool firstStatement = true;
    while (currentLexemeValue() != "end" && pos < static_cast<int>(tokens.size())) {
        if (!parseStatement()) {
            return false;
        }

        if (currentLexemeValue() == ";") {
            ++pos;
            firstStatement = false;
        }
        else if (currentLexemeValue() != "end") {
            if (firstStatement) {
                return error("ожидалась ';' или 'end' после оператора");
            }
        }
        firstStatement = false;
    }

    if (!expect("end", "ожидалось ключевое слово end")) {
        return false;
    }

    return true;
}

SimplePrecedenceParser::SimplePrecedenceParser(const vector<pair<int, int>>& t,
    const vector<string>& sn,
    const vector<string>& c,
    const vector<string>& kw,
    const vector<string>& sep)
    : tokens(t), symbolic_names(sn), constants(c), keywords(kw), separators(sep) {
    initializePrecedenceMatrix();
}

bool SimplePrecedenceParser::parseProgram() {
    pos = 0;
    reductions.clear();
    statements.clear();
    last_error.clear();
    last_reduction.clear();
    trace_step = 1;

    trace("Начало синтаксического анализа программы");

    if (currentLexemeValue() == "program") {
        ++pos;
        string progName;
        if (!expectIdentifier("ожидался идентификатор программы")) {
            return false;
        }
        if (!expect(";", "ожидалась ';' после имени программы")) {
            return false;
        }
    }

    while (currentLexemeValue() == "var") {
        if (!parseDeclaration()) {
            return false;
        }
    }

    if (!parseCompoundStatement()) {
        return false;
    }

    if (!expect(".", "ожидалась '.' в конце программы")) {
        return false;
    }

    if (pos != static_cast<int>(tokens.size())) {
        return error("после завершения разбора остались необработанные лексемы");
    }

    trace("Все лексемы успешно обработаны");
    return true;
}

bool SimplePrecedenceParser::parseStatement() {
    string current = currentLexemeValue();
    trace("Начало разбора оператора, текущая лексема: '" + current + "'");

    if (current == "while") {
        return parseWhileLoop();
    }

    if (current == "begin") {
        return parseCompoundStatement();
    }

    if (current == "read" || current == "readln" || current == "write" || current == "writeln") {
        return parseIO(current);
    }

    if (matchType(1)) {
        return parseAssignment();
    }

    if (matchType(2)) {
        if (pos + 1 < static_cast<int>(tokens.size()) && lexemeAt(pos + 1) == ":=") {
            return error("ожидался идентификатор перед присваивании, получена константа '" + current + "'");
        }
        return error("не удалось распознать начало оператора: '" + current + "'");
    }

    return error("не удалось распознать начало оператора: '" + current + "'");
}

void SimplePrecedenceParser::printSemanticRecords() const {
    semantic.printRecords();
    semantic.printFullProgramRPN();
    semantic.printSymbolTable();
}

const vector<string>& SimplePrecedenceParser::getFullProgramRpn() const {
    return semantic.getFullProgramRpn();
}

const vector<SemanticAnalyzer::SymbolEntry>& SimplePrecedenceParser::getSymbolTable() const {
    return semantic.getSymbols();
}

void SimplePrecedenceParser::printReductions() const {
    cout << "СВЕРТКИ МЕТОДА ПРОСТОГО ПРЕДШЕСТВОВАНИЯ\n";
    if (reductions.empty()) {
        cout << "(свертки отсутствуют)\n";
    }
    else {
        for (size_t i = 0; i < reductions.size(); ++i) {
            cout << i + 1 << ") " << reductions[i] << "\n";
        }
    }
}

const string& SimplePrecedenceParser::getLastError() const {
    return last_error;
}