#include "codegen.h"
#include <iostream>
#include <sstream>
#include <regex>
using namespace std;

void CodeGenerator::generate(const vector<string>& optimizedIC) {
    cout << "\n--- Final Target Code ---\n";

    regex assignRegex(R"((\w+)\s*=\s*(\w+))");
    regex binOpRegex(R"((\w+)\s*=\s*(\w+)\s*([+*/-])\s*(\w+))");  // '-' last me
    regex printRegex(R"(print\s+(\w+))");


    for (const string& line : optimizedIC) {
        smatch match;

        // Check for binary operation
        if (regex_match(line, match, binOpRegex)) {
            string lhs = match[1];
            string op1 = match[2];
            string op = match[3];
            string op2 = match[4];

            cout << "MOV R1, " << op1 << endl;
            if (op == "+") cout << "ADD R1, " << op2 << endl;
            else if (op == "-") cout << "SUB R1, " << op2 << endl;
            else if (op == "*") cout << "MUL R1, " << op2 << endl;
            else if (op == "/") cout << "DIV R1, " << op2 << endl;
            cout << "MOV " << lhs << ", R1" << endl;
        }

        // Check for simple assignment
        else if (regex_match(line, match, assignRegex)) {
            string lhs = match[1];
            string rhs = match[2];
            cout << "MOV " << lhs << ", " << rhs << endl;
        }

        // Check for print
        else if (regex_match(line, match, printRegex)) {
            cout << "PRINT " << match[1] << endl;
        }

        // If none matched
        else {
            cout << "; Unknown statement: " << line << endl;
        }
    }
}
