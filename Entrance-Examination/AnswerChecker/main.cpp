#include <windows.h>
#include <CommCtrl.h>
#include <Richedit.h>
#include <stdio.h>
#include <fstream>
#include <sstream>

HWND g_main;
#pragma comment( lib, "comctl32.lib" )
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " "version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow);

HWND g_static;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPInst, char* line, int show)
{
    LoadLibrary(TEXT("Msftedit.dll"));
    InitCommonControls();

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = TEXT("frobi-answerchecker");
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassEx(&wc))
        FatalAppExitA(0, "Couldn't register window class!");

    g_main = CreateWindowEx(WS_EX_ACCEPTFILES, L"frobi-answerchecker", L"Answers Checker", WS_OVERLAPPEDWINDOW, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, hInstance, 0);
    ShowWindow(g_main, SW_MAXIMIZE);
    
    static HFONT hFont = CreateFont(28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
    g_static = CreateWindowEx(0, MSFTEDIT_CLASS, L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY, 0, 100, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) - 300, g_main, NULL, hInstance, NULL);
    SendMessage(g_static, WM_SETFONT, (WPARAM)hFont, TRUE);

    MSG Msg = { 0 };
    while (GetMessageA(&Msg, 0, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
    return 0;
}

std::wstringstream g_text(L"");
void LoadFromFile(TCHAR * filename)
{
    std::fstream file;
    file.open(filename, std::ios::in | std::ios::binary);
    int setid;
    file.read((char*)&setid, sizeof(setid));

    std::fstream afile;
    std::stringstream afname(""); afname << "set" << setid << ".txt";
    afile.open(afname.str().c_str(), std::ios::in);
    std::string cans;
    afile >> cans;
    afile.close();

    int correctanswers = 0;
    int incorrectanswers = 0;

    unsigned int size;
    file.read((char*)&size, sizeof(size));
    char* username = new char[size + 1]; username[size] = '\0';
    file.read(username, size);
    g_text << "\t" << username << "\n";
    delete[] username;
    file.read((char*)&size, sizeof(size));
    for (unsigned int i = 0; i < size; ++i)
    {
        char ans;
        file.read(&ans, sizeof(ans));
        if (ans == cans[i] - 'a') correctanswers++;
        else if (ans != -1) incorrectanswers++;
    }
    file.close();

    g_text << "\t\tCorrect Answers: " << correctanswers << "\n";
    g_text << "\t\tIncorrect Answers: " << incorrectanswers << "\n\n";
    SetWindowText(g_static, g_text.str().c_str());
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_DROPFILES:
    {
                         TCHAR lpszFile[MAX_PATH] = { 0 };
                         UINT uFile = 0;
                         HDROP hDrop = (HDROP)wParam;
                         uFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);
                         lpszFile[0] = '\0';
                         for (UINT i = 0; i < uFile; ++i)
                         if (DragQueryFile(hDrop, i, lpszFile, MAX_PATH))
                             LoadFromFile(lpszFile);

                         DragFinish(hDrop);
                         break;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}