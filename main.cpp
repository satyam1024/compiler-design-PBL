#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main() {
  
    
    std::ifstream infile("input.txt");
    if (!infile) {
        std::cerr << "Error: Cannot open input file "<< "\n";
        return 1;
    }
    std::stringstream buffer;
    buffer << infile.rdbuf();
    std::string src = buffer.str();
    infile.close();

    // Compile as before
    Lexer lexer(src);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto ast = parser.parseProgram();

    CodeGenerator codegen;
    std::string c_code = codegen.generate(ast);

    
    std::ofstream outfile("output.c");
    if (!outfile) {
        std::cerr << "Error: Cannot open output file " << "\n";
        return 1;
    }
    outfile << c_code;
    outfile.close();

    std::cout << "Compilation successful. Output written to "  << "\n";
    return 0;
}
