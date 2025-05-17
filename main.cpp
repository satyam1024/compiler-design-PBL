#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "icgenerator.h"
#include "optimizer.h"
#include "codegen.h"

using namespace std;

int main() {
    string code, line;
    cout << "=== Simple Compiler ===\n";
    cout << "Enter your code below (type END on a new line to finish input):\n";

    // Multi-line input from user
    while (true) {
        getline(cin, line);
        if (line == "END") break;
        code += line + " ";
    }

    // Tokenization
    Lexer lexer(code);
    vector<Token> tokens = lexer.tokenize();

    // Parsing
    Parser parser(tokens);
    parser.parse();

    // Intermediate Code Generation
    cout << "\n--- Intermediate Code ---\n";
    ICGenerator icg(tokens);
    vector<string> ic = icg.generate();

    for (const string& line : ic) {
        cout << line << endl;
    }

    // Optimization
    cout << "\n--- Optimized Intermediate Code ---\n";
    vector<string> optimizedIC = Optimizer::optimize(ic);
    for (const string& line : optimizedIC) {
        cout << line << endl;
    }

    // Final Code Generation
    CodeGenerator::generate(optimizedIC);

    return 0;
}
