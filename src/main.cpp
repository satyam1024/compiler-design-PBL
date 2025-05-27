#include "UI.h"
#undef UNICODE
#undef _UNICODE
#include <windows.h>

// For ANSI (non-Unicode) build:
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    CompilerUI app(hInstance);
    return app.run();
}

// For Unicode build (if you use L"..." and Unicode flags):
// int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
//     CompilerUI app(hInstance);
//     return app.run();
// }
