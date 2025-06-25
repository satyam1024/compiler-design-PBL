#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>

#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "IntermediateCodeGen.h"
#include "Optimizer.h"
#include "CodeGenerator.h"

namespace fs = std::filesystem;


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

void writeToFile(const fs::path& path, const std::string& content) {
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

void writeListToFile(const fs::path& path, const std::vector<std::string>& lines) {
    std::ofstream out(path);
    if (out.is_open()) {
        for (const auto& line : lines) {
            out << line << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: compiler.exe <input_file> <output_dir>\n";
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputDir = argv[2];

    std::ifstream inFile(inputPath);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open input file.\n";
        return 1;
    }

    std::string code((std::istreambuf_iterator<char>(inFile)),
                     std::istreambuf_iterator<char>());

    fs::create_directories(outputDir);

    std::vector<std::string> errors;
    std::string ccode;
    std::vector<IRInstruction> irCode;
    std::vector<IRInstruction> optimizedIR;

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
    
    writeToFile(fs::path(outputDir) / "tokens.txt", tokenStream.str());

    Parser parser(tokens);
    auto ast = parser.parse();
    auto parseErrors = parser.getErrors();
    errors.insert(errors.end(), parseErrors.begin(), parseErrors.end());

    SemanticAnalyzer sema;
    sema.analyze(ast.get());
    auto semaErrors = sema.getErrors();
    errors.insert(errors.end(), semaErrors.begin(), semaErrors.end());

    if (!errors.empty()) {
        writeListToFile(fs::path(outputDir) / "errors.txt", errors);
        writeToFile(fs::path(outputDir) / "c_code.txt", "// No C code generated due to errors.\n");
        return 0;
    } else {
        writeListToFile(fs::path(outputDir) / "errors.txt", { "No errors." });
    }

    IntermediateCodeGen icg;
    icg.generate(ast.get());
    irCode = icg.getIR();
    std::string irstr = vectorToString(irCode);
    writeToFile(fs::path(outputDir) / "ir.txt", irstr);

    Optimizer optimizer;
    optimizer.optimize(irCode);
    optimizedIR = optimizer.getOptimizedIR();
    std::string oIrstr = vectorToString(optimizedIR);
    writeToFile(fs::path(outputDir) / "optimized_ir.txt", oIrstr);

    CodeGenerator codegen;
    codegen.generate(optimizedIR);
    ccode = codegen.getCCode();
    writeToFile(fs::path(outputDir) / "c_code.txt", ccode);

    writeToFile(fs::path(outputDir) / "output.txt", "Program compiled successfully.");

    return 0;
}



