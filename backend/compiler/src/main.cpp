#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib> // for system()

#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "IntermediateCodeGen.h"
#include "Optimizer.h"
#include "CodeGenerator.h"

std::string irInstructionToString(const IRInstruction& instr) {
    std::stringstream ss;
    ss << "Line " << instr.line << ": " << instr.opcode;
    if (!instr.operands.empty()) {
        ss << " ";
        for (size_t i = 0; i < instr.operands.size(); ++i) {
            ss << instr.operands[i];
            if (i != instr.operands.size() - 1) ss << ", ";
        }
    }
    return ss.str();
}

void writeToFile(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    if (out.is_open()) {
        out << content;
    }
}

std::string vectorToString(const std::vector<IRInstruction>& vec) {
    std::stringstream ss;
    for (const auto& instr : vec) {
        ss << irInstructionToString(instr) << "\n";
    }
    return ss.str();
}

void writeListToFile(const std::string& path, const std::vector<std::string>& lines) {
    std::ofstream out(path);
    if (out.is_open()) {
        for (const auto& line : lines) {
            out << line << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    std::string code;
    std::string outputDir;

    // Interactive input
    if (argc == 1) {
        std::cout << "Type your code below. Type END on a new line to finish input:\n";
        std::string line;
        while (true) {
            std::getline(std::cin, line);
            if (line == "END") break;
            code += line + "\n";
        }

        std::cout << "Enter output directory name: ";
        std::getline(std::cin, outputDir);

        // create output dir using system command (cross-platform safe for basic cases)
        system(("mkdir " + outputDir).c_str());
    }
    // File input
    else if (argc >= 3) {
        std::string inputPath = argv[1];
        outputDir = argv[2];

        std::ifstream inFile(inputPath);
        if (!inFile.is_open()) {
            std::cerr << "Failed to open input file.\n";
            return 1;
        }

        code = std::string((std::istreambuf_iterator<char>(inFile)),
                           std::istreambuf_iterator<char>());
        system(("mkdir " + outputDir).c_str());
    }
    else {
        std::cerr << "Usage: compiler.exe <input_file> <output_dir>\n"
                  << "Or just run without arguments for interactive mode.\n";
        return 1;
    }

    std::vector<std::string> errors;
    std::string ccode;
    std::vector<IRInstruction> irCode;
    std::vector<IRInstruction> optimizedIR;

    // --- LEXER ---
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    std::ostringstream tokenStream;
    for (const auto& token : tokens) {
        tokenStream << "Type: " << static_cast<int>(token.type)
                    << ", Lexeme: " << token.lexeme
                    << ", Line: " << token.line
                    << ", Col: " << token.column << "\n";
        if (token.type == TokenType::INVALID) {
            errors.push_back("Lexical error at line " + std::to_string(token.line) +
                             ", column " + std::to_string(token.column) + ": Invalid token '" + token.lexeme + "'");
        }
    }

    writeToFile(outputDir + "/tokens.txt", tokenStream.str());

    // --- PARSER ---
    Parser parser(tokens);
    auto ast = parser.parse();
    auto parseErrors = parser.getErrors();
    errors.insert(errors.end(), parseErrors.begin(), parseErrors.end());

    // --- SEMANTIC ANALYZER ---
    SemanticAnalyzer sema;
    sema.analyze(ast.get());
    auto semaErrors = sema.getErrors();
    errors.insert(errors.end(), semaErrors.begin(), semaErrors.end());

    // Write errors
    if (!errors.empty()) {
        writeListToFile(outputDir + "/errors.txt", errors);
        writeToFile(outputDir + "/c_code.txt", "// No C code generated due to errors.\n");
        return 0;
    } else {
        writeListToFile(outputDir + "/errors.txt", { "No errors." });
    }

    // --- INTERMEDIATE CODE GEN ---
    IntermediateCodeGen icg;
    icg.generate(ast.get());
    irCode = icg.getIR();
    writeToFile(outputDir + "/ir.txt", vectorToString(irCode));

    // --- OPTIMIZATION ---
    Optimizer optimizer;
    optimizer.optimize(irCode);
    optimizedIR = optimizer.getOptimizedIR();
    writeToFile(outputDir + "/optimized_ir.txt", vectorToString(optimizedIR));

    // --- CODE GENERATION ---
    CodeGenerator codegen;
    codegen.generate(optimizedIR);
    ccode = codegen.getCCode();
    writeToFile(outputDir + "/c_code.txt", ccode);

    // Optional: simulated final program output
    writeToFile(outputDir + "/output.txt", "Program compiled successfully.");

    return 0;
}
