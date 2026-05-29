#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>

using namespace std;

string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл: " + filename);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    for (char& c : content) {
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
    return content;
}

int main() {
    setlocale(LC_ALL, "Russian");
    string filename = "input.pas";
    string test_code;

    try {
        test_code = readFile(filename);
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    string baseName = "program";
    string lexerFilename = "lexer.txt";
    string parserFilename = "syntax.txt";
    string semanticFilename = "semantic.txt";
    string errorsFilename = "errors.txt";

    ofstream lexerOutput(lexerFilename);
    ofstream parserOutput(parserFilename);
    ofstream semanticOutput(semanticFilename);
    ofstream errorsOutput(errorsFilename);

    if (!lexerOutput.is_open() || !parserOutput.is_open() || !semanticOutput.is_open() || !errorsOutput.is_open()) {
        cerr << "Ошибка: не удалось создать файлы вывода." << endl;
        return 1;
    }

    streambuf* consoleBuf = cout.rdbuf();
    cout << "Исходный файл: " << filename << ".\n\n";
    cout << "Используется метод простого предшествования.\n\n";

    Lexer lexer;
    lexer.analyze(test_code);

    cout.rdbuf(lexerOutput.rdbuf());
    cout << "Исходный код:\n" << test_code << "\n\n";

    if (lexer.getErrors().empty()) {
        cout << "Лексических ошибок не обнаружено.\n\n";
    }
    else {
        cout << "Обнаружены лексические ошибки:\n";
        for (const auto& err : lexer.getErrors()) {
            cout << "Строка " << get<0>(err) << ", позиция " << get<1>(err) << ": символ '" << get<2>(err) << "'\n";
        }
        cout << "\n";
    }

    lexer.printTables();
    lexerOutput.close();
    cout.rdbuf(consoleBuf);

    SimplePrecedenceParser parser(
        lexer.getTokens(),
        lexer.getSymbolicNames(),
        lexer.getConstants(),
        lexer.getKeywords(),
        lexer.getSeparators());

    cout.rdbuf(parserOutput.rdbuf());

    parser.printPrecedenceMatrix();

    bool parseSuccess = parser.parseProgram();
    cout << "\n";

    if (parseSuccess) {
        cout << "Синтаксических ошибок не обнаружено\n\n";
        parser.printReductions();
    }
    else {
        cout << parser.getLastError() << "\n";
    }

    parserOutput.close();
    cout.rdbuf(consoleBuf);

    cout.rdbuf(semanticOutput.rdbuf());
    if (parseSuccess) {
        parser.printSemanticRecords();
        cout << "\nСемантических ошибок не обнаружено\n";
    }
    else {
        cout << parser.getLastError() << "\n";
    }
    semanticOutput.close();
    cout.rdbuf(consoleBuf);

    ofstream errorsFile(errorsFilename);
    if (!parseSuccess) {
        errorsFile << parser.getLastError() << "\n";
    }
    else {
        errorsFile << "Ошибок не обнаружено.\n";
    }
    errorsFile.close();

    cout << "РЕЗУЛЬТАТ КОМПИЛЯЦИИ\n\n";
    cout << "Лексический отчет " << lexerFilename << " успешно создан.\n";
    cout << "Синтаксический отчет " << parserFilename << " успешно создан.\n";
    cout << "Семантический отчет " << semanticFilename << " успешно создан.\n";
    cout << "Отчет об ошибках " << errorsFilename << " успешно создан.\n";

    if (!parseSuccess) {
        cout << parser.getLastError() << endl;
        return 1;
    }

    Compiler compiler(parser.getFullProgramRpn(), parser.getSymbolTable());
    if (!compiler.generateAndCompile(baseName)) {
        cout << "Ошибка при компиляции программы." << endl;
        return 1;
    }

    string cppFilename = baseName + ".cpp";
    string exeFilename = baseName + ".exe";
    cout << "Сгенерированный файл C++ " << cppFilename << " успешно создан.\n";
    cout << "Исполняемый файл " << exeFilename << " успешно создан.\n";
    cout << "Компиляция прошла успешно.\n";

    return 0;
}