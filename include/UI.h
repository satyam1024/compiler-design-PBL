#ifndef UI_H
#define UI_H
#undef UNICODE
#undef _UNICODE
#include <windows.h>
#include <string>
#include <vector>

// IDs for controls
#define IDC_CODE_INPUT   101
#define IDC_COMPILE_BTN  102
#define IDC_ERROR_OUTPUT 103
#define IDC_CCODE_OUTPUT 104

class CompilerUI {
public:
    CompilerUI(HINSTANCE hInstance);
    int run();

    // Setters for outputs
    void setErrorOutput(const std::vector<std::string>& errors);
    void setCCodeOutput(const std::string& ccode);

    // Get code from input box
    std::string getCodeInput() const;

    // Window procedure (member version)
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Static instance pointer for message routing
    static CompilerUI* instance;

    // Compile button handler
    void onCompile();

private:
    HWND hwndMain;
    HWND hwndCodeInput;
    HWND hwndCompileBtn;
    HWND hwndErrorOutput;
    HWND hwndCCodeOutput;

    static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void createControls(HWND hwnd);

    // Store outputs for access
    std::vector<std::string> lastErrors;
    std::string lastCCode;
};

#endif // UI_H
