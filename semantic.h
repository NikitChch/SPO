#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <iomanip>

using namespace std;

class SemanticAnalyzer {
public:
    struct Record {
        string kind;
        vector<string> rpn;
        bool valid;
        string error;
    };

    struct SymbolEntry {
        string name;
        string type;
        bool declared;
        bool initialized;
    };

private:
    vector<Record> records;
    vector<SymbolEntry> symbols;
    vector<string> fullProgramRpn;
    int labelCounter = 0;

    static bool isBinaryExpressionOperator(const string& token);
    static bool isUnaryExpressionOperator(const string& token);
    SymbolEntry* lookupVisible(const string& name);
    const SymbolEntry* lookupVisible(const string& name) const;

public:
    SemanticAnalyzer();
    string createLabel();
    bool declareSymbol(const string& name, const string& type, bool initialized, string& error);
    bool ensureDeclared(const string& name, const string& context, string& error) const;
    bool ensureInitialized(const string& name, const string& context, string& error) const;
    void markInitialized(const string& name);
    bool validateRPN(const vector<string>& rpn, string& error) const;
    bool addRecord(const string& kind, const vector<string>& rpn, bool validate, string& error);

    void printRecords() const;
    void printFullProgramRPN() const;
    void printSymbolTable() const;

    const vector<string>& getFullProgramRpn() const;
    const vector<SymbolEntry>& getSymbols() const;
};