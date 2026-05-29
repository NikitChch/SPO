#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <tuple>

using namespace std;

class Lexer {
private:
    vector<string> symbolic_names;
    vector<string> constants;
    vector<string> keywords;
    vector<string> separators;
    vector<tuple<int, int, char>> errors;

    unordered_map<string, int> symbolic_names_map;
    unordered_map<string, int> constants_map;
    unordered_map<string, int> keywords_map;
    unordered_map<string, int> separators_map;

    vector<pair<int, int>> tokens;

    int line;
    int position;

    string source_code;
    size_t source_pos;

    const string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789";

    void initializeKeywords();
    void initializeSeparators();
    void skipWhitespace();
    bool isInAlphabet(char c);
    void addError(char c, int line_num, int pos);
    bool processIdentifier();
    bool processNumber();
    bool processSeparator();

public:
    Lexer();
    void analyze(const string& code);
    void printTables();

    const vector<pair<int, int>>& getTokens() const;
    const vector<string>& getSymbolicNames() const;
    const vector<string>& getConstants() const;
    const vector<string>& getKeywords() const;
    const vector<string>& getSeparators() const;
    const vector<tuple<int, int, char>>& getErrors() const;
};