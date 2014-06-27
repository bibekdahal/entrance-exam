#include <windows.h>
#include <CommCtrl.h>
#include <Richedit.h>
#include <stdio.h>

HWND g_main;
#pragma comment( lib, "comctl32.lib" )

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow);

int g_ny = 0;
#define MAX_QUESTIONS 10

int g_nc = 0;
struct controls
{
    HWND redit, opta, optb, optc, optd;
} g_controls[MAX_QUESTIONS];
void CreateQuestion(HINSTANCE hInstance)
{
    g_controls[g_nc].redit = CreateWindowEx(0, MSFTEDIT_CLASS, L"",
        ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
        0, g_ny + 0, 640, 150,
        g_main, NULL, hInstance, NULL);

    g_controls[g_nc].opta = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Option A",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP , 
        10, g_ny + 160,
        150, 20,
        g_main, NULL, hInstance, NULL);

    g_controls[g_nc].optb = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Option B",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        200, g_ny + 160,
        150, 20,
        g_main, NULL, hInstance, NULL);

    g_controls[g_nc].optc = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Option C",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        10, g_ny + 200,
        150, 20,
        g_main, NULL, hInstance, NULL);

    g_controls[g_nc].optd = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Option D",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        200, g_ny + 200,
        150, 20,
        g_main, NULL, hInstance, NULL);

    g_ny += 50 * 5;
    g_nc++;

}
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
    wc.lpszClassName = TEXT("frobi-entranceexam");
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassEx(&wc))
        FatalAppExitA(0, "Couldn't register window class!");

    g_main = CreateWindowEx(0, L"frobi-entranceexam", L"Entrance Examination", WS_OVERLAPPEDWINDOW | WS_VSCROLL, 64, 64, 640, 480, 0, 0, hInstance, 0);

    CreateQuestion(hInstance);
    CreateQuestion(hInstance);
    CreateQuestion(hInstance);
    CreateQuestion(hInstance);
    CreateQuestion(hInstance);

    ShowWindow(g_main, show);

    MSG Msg = { 0 };
    while (GetMessageA(&Msg, 0, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
    return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static int VscrollPos = 0, prevpos, xClient, yClient;
    RECT rc;
    SCROLLINFO si;

    switch (msg)
    {
    case WM_SIZE:       
        yClient = HIWORD(lParam);
        xClient = LOWORD(lParam);

        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = g_ny;
        si.nPage = yClient;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

        for (int i = 0; i < g_nc; ++i)
        {
            int y = i * 50 * 5;
            MoveWindow(g_controls[i].redit, 0, y, xClient, 150, true);
            MoveWindow(g_controls[i].opta, 0, y + 160, xClient / 2, 20, true);
            MoveWindow(g_controls[i].optb, xClient / 2, y + 160, xClient / 2, 20, true);
            MoveWindow(g_controls[i].optc, 0, y + 200, xClient / 2, 20, true);
            MoveWindow(g_controls[i].optd, xClient / 2, y + 200, xClient / 2, 20, true);
        }
        break;
    case WM_VSCROLL:
        GetClientRect(g_main, &rc);
        prevpos = VscrollPos;
        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            VscrollPos -= 1;
            break;
        case SB_LINEDOWN:
            VscrollPos += 1;
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            VscrollPos = HIWORD(wParam);
            break;
        default:
            break;
        }

        VscrollPos = max(0, min(VscrollPos, g_ny));

        if (VscrollPos != GetScrollPos(hwnd, SB_VERT)) {
            SetScrollPos(hwnd, SB_VERT, VscrollPos, TRUE);
            InvalidateRect(hwnd, NULL, TRUE);
            ScrollWindowEx(g_main, 0, prevpos - VscrollPos, NULL, NULL, NULL, &rc, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
            UpdateWindow(g_main);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}