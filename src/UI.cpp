#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "IntermediateCodeGen.h"
#include "Optimizer.h"
#include "CodeGenerator.h"
#include "UI.h"
#undef UNICODE
#undef _UNICODE
#include <windowsx.h>
#include <sstream>

// Singleton instance pointer
CompilerUI* CompilerUI::instance = nullptr;

CompilerUI::CompilerUI(HINSTANCE hInstance) {
    instance = this;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = StaticWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"CustomCompilerWndClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    RegisterClass(&wc);

    hwndMain = CreateWindowExW(
        0, L"CustomCompilerWndClass", L"Custom Language Compiler",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        nullptr, nullptr, hInstance, nullptr
    );

    createControls(hwndMain);

    ShowWindow(hwndMain, SW_SHOW);
    UpdateWindow(hwndMain);
}

int CompilerUI::run() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

void CompilerUI::createControls(HWND hwnd) {
    // Code input box
    hwndCodeInput = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", nullptr,
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN,
        20, 30, 400, 400,
        hwnd, (HMENU)IDC_CODE_INPUT, GetModuleHandle(nullptr), nullptr
    );

    // Compile button
    hwndCompileBtn = CreateWindowW(
        L"BUTTON", L"Compile",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        440, 30, 100, 30,
        hwnd, (HMENU)IDC_COMPILE_BTN, GetModuleHandle(nullptr), nullptr
    );

    // Error output box
    hwndErrorOutput = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", nullptr,
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        560, 30, 300, 200,
        hwnd, (HMENU)IDC_ERROR_OUTPUT, GetModuleHandle(nullptr), nullptr
    );

    // C code output box
    hwndCCodeOutput = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", nullptr,
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        440, 250, 420, 180,
        hwnd, (HMENU)IDC_CCODE_OUTPUT, GetModuleHandle(nullptr), nullptr
    );

    // Labels
    CreateWindowW(L"STATIC", L"Your Code:",
        WS_VISIBLE | WS_CHILD, 20, 10, 100, 20,
        hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

    CreateWindowW(L"STATIC", L"Errors:",
        WS_VISIBLE | WS_CHILD, 560, 10, 100, 20,
        hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

    CreateWindowW(L"STATIC", L"Generated C Code:",
        WS_VISIBLE | WS_CHILD, 440, 230, 140, 20,
        hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
}

// Static window procedure for Windows message routing
LRESULT CALLBACK CompilerUI::StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (instance) {
        return instance->WindowProc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Member window procedure
LRESULT CALLBACK CompilerUI::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_COMPILE_BTN) {
            onCompile();
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

std::string CompilerUI::getCodeInput() const {
    int len = GetWindowTextLengthA(hwndCodeInput);
    if (len <= 0) return "";
    std::string buffer(len, '\0');
    GetWindowTextA(hwndCodeInput, &buffer[0], len + 1);
    return buffer;
}


// Show error messages in the error output box
void CompilerUI::setErrorOutput(const std::vector<std::string>& errors) {
    std::ostringstream oss;
    for (const auto& e : errors) {
        oss << e << "\r\n";
    }
    SetWindowTextA(hwndErrorOutput, oss.str().c_str());
    lastErrors = errors;
}

// Show generated C code in the C code output box
void CompilerUI::setCCodeOutput(const std::string& ccode) {
    SetWindowTextA(hwndCCodeOutput, ccode.c_str());
    lastCCode = ccode;
}


void CompilerUI::onCompile() {
    setErrorOutput({});
    setCCodeOutput("");
    
    // 1. Get code from input
    std::string code = getCodeInput();

    // 2. Run the compiler pipeline
    std::vector<std::string> errors;
    std::string ccode;

    // --- LEXICAL ANALYSIS ---
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    // Check for lexical errors
    bool lexError = false;
    for (const auto& t : tokens) {
        if (t.type == TokenType::INVALID) {
            errors.push_back("Lexical error at line " + std::to_string(t.line) +
                             ", column " + std::to_string(t.column) + ": Invalid token '" + t.lexeme + "'");
            lexError = true;
        }
    }

    // --- SYNTAX ANALYSIS ---
    Parser parser(tokens);
    auto ast = parser.parse();
    auto parseErrors = parser.getErrors();
    errors.insert(errors.end(), parseErrors.begin(), parseErrors.end());

    // --- SEMANTIC ANALYSIS ---
    SemanticAnalyzer sema;
    sema.analyze(ast.get());
    auto semaErrors = sema.getErrors();
    errors.insert(errors.end(), semaErrors.begin(), semaErrors.end());

    // If there are any errors, display them and stop
    if (!errors.empty()) {
        setErrorOutput(errors);
        setCCodeOutput("// No C code generated due to errors.");
        return;
    }

    // --- INTERMEDIATE CODE GENERATION ---
    IntermediateCodeGen icg;
    icg.generate(ast.get());

    // --- OPTIMIZATION ---
    Optimizer optimizer;
    optimizer.optimize(icg.getIR());

    // --- CODE GENERATION ---
    CodeGenerator codegen;
    codegen.generate(optimizer.getOptimizedIR());
    ccode = codegen.getCCode();

    // Show outputs
    setErrorOutput({ "No errors." });
    setCCodeOutput(ccode);
}