#pragma once
#include "semantic.h"
#include <fstream>
#include <iostream>

using namespace std;

class Compiler {
public:
    Compiler(const vector<string>& rpn, const vector<SemanticAnalyzer::SymbolEntry>& symbols);
    bool generateAndCompile(string baseName);

private:
    vector<string> programRpn;
    vector<SemanticAnalyzer::SymbolEntry> symbolTable;

    bool generateCppCode(string baseName);
    bool compileExecutable(string baseName);

    static bool isIntegerLiteral(const string& s);
    static bool isIdentifier(const string& s);
    static bool isLabelName(const string& s);
    static string cppOperator(const string& op);
    static bool isBinaryOperator(const string& s);
};