#pragma once

#include "lexer.h"
#include "semantic.h"
#include <set>
#include <map>

using namespace std;

class SimplePrecedenceParser {

private:
    struct ExprNode {
        string symbol;
        vector<string> rpn;
    };

    const vector<pair<int, int>>& tokens;
    const vector<string>& symbolic_names;
    const vector<string>& constants;
    const vector<string>& keywords;
    const vector<string>& separators;

    int pos = 0;
    vector<string> reductions;
    vector<string> statements;
    string last_error;
    string last_reduction;
    bool trace_enabled = true;
    int trace_step = 1;

    SemanticAnalyzer semantic;

    map<string, map<string, string>> precedenceMatrix;

    void initializePrecedenceMatrix();
    void trace(const string& message);
    string stackToString(const vector<ExprNode>& stackSymbols) const;
    string rpnToString(const vector<string>& rpn) const;
    string getLexemeValue(int table, int index) const;
    string lexemeAt(int index) const;
    string currentLexemeValue() const;
    bool matchType(int table) const;
    bool expect(const string& lexeme, const string& message);
    bool expectIdentifier(const string& message, string* outName = nullptr);
    bool error(const string& message);
    bool semanticError(const string& message);
    bool semanticErrorAt(const string& message, int errorPos);
    bool addSemanticRecord(const string& kind, const vector<string>& rpn, bool validate = true);

    bool isBinaryOperator(const string& s) const;
    bool isUnaryOperator(const string& s) const;
    int precedenceLevel(const string& op) const;
    string precedenceRelation(const string& a, const string& b) const;
    string semanticTokenForUnary(const string& symbol) const;
    bool reduce(vector<ExprNode>& stackSymbols);

    bool parseExpression(const set<string>& stopTokens, vector<string>* outRpn = nullptr);
    bool consumeOperand(vector<string>& operandRpn, int operandPos);
    string classifyLookahead(bool expectOperandNow, const set<string>& stopTokens);
    bool shiftSymbol(const string& symbol, bool& expectOperandNow, vector<ExprNode>& stackSymbols, int exprStartPos);
    string topTerminal(const vector<ExprNode>& stackSymbols) const;

    bool parseIdentifierList(vector<string>& identifiers, vector<int>* positions = nullptr);
    string ioOpcode(const string& ioType) const;
    bool parseIO(const string& ioType);
    bool parseDeclaration();
    bool parseAssignment();
    bool parseWhileLoop();
    bool parseCompoundStatement();

public:
    SimplePrecedenceParser(const vector<pair<int, int>>& t,
        const vector<string>& sn,
        const vector<string>& c,
        const vector<string>& kw,
        const vector<string>& sep);

    bool parseProgram();
    bool parseStatement();
    void printSemanticRecords() const;
    const vector<string>& getFullProgramRpn() const;
    const vector<SemanticAnalyzer::SymbolEntry>& getSymbolTable() const;
    void printReductions() const;
    void printPrecedenceMatrix() const;
    const string& getLastError() const;
};