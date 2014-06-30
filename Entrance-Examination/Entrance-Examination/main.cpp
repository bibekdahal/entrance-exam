#include <windows.h>
#include <CommCtrl.h>
#include <Richedit.h>
#include <stdio.h>

#include "Page.h"

HWND g_main;
#pragma comment( lib, "comctl32.lib" )

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPInst, char* line, int show)
{
    LoadLibrary(TEXT("Msftedit.dll"));
    InitCommonControls();

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = TEXT("frobi-entranceexam");
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassEx(&wc))
        FatalAppExitA(0, "Couldn't register window class!");

	g_main = CreateWindowEx(0, L"frobi-entranceexam", L"Entrance Examination", WS_OVERLAPPEDWINDOW | WS_VSCROLL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, hInstance, 0);

    // Two calls are made so that:
    // after first call, the rich edit controls are created and initialized with text, and
    // after second call, a Request-Resize message is sent to each rich edit control and controls are resized according to their contents
	ShowWindow(g_main, SW_NORMAL);
    ShowWindow(g_main, SW_MAXIMIZE);

	//Enable to go fullscreen
	//SetWindowLong(g_main, GWL_STYLE, 0);
	//ShowWindow(g_main, SW_NORMAL);

    MSG Msg = { 0 };
    while (GetMessageA(&Msg, 0, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
    return 0;
}

const int ID_TIMER = 10;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static Page mainPage;
    static int VscrollPos = 0, prevpos, xClient, yClient;
    RECT rc;
    SCROLLINFO si;

    HDC hdc;
    PAINTSTRUCT ps;
    WCHAR szBuffer[200];
    static int count = 60 * 60 * 3;
    int seconds = 0;
    int minutes = 0;
    int hours = 0;
    static bool examrunning = true;

    switch (msg)
    {
	case WM_CREATE:
        mainPage.Initialize(hwnd);
        SetTimer(hwnd, ID_TIMER, 1000, NULL);
        break;
    case WM_PAINT:
        if (count > 0 && examrunning)
        {
            hdc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &rc);
            rc.left = 10;
            rc.right = 170;
            hours = count / 3600;
            minutes = (count / 60) % 60;
            seconds = count % 60;
            wsprintf(szBuffer, L"Remaining Time:\n%d hrs : %d min : %d sec", hours, minutes, seconds);
            DrawText(hdc, szBuffer, -1, &rc, DT_LEFT);
            EndPaint(hwnd, &ps);
        }
        else if (examrunning)
        {
            examrunning = false;
            MessageBox(g_main, L"Your exam time is over.", L"Thank you!", 0);
            mainPage.Submit();
        }
        break;
    case WM_TIMER:
        count--; 
        GetClientRect(hwnd, &rc);
        rc.right = 170;
        InvalidateRect(hwnd, &rc, TRUE);
        break;
    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER);
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        yClient = HIWORD(lParam);
        xClient = LOWORD(lParam);

        mainPage.Resize(hwnd);

        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = mainPage.GetYMax();
        si.nPage = yClient;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        break;

    case WM_COMMAND:
        if ((HWND)lParam == mainPage.GetSubmitHandle())
            mainPage.Submit();
        break;
    case WM_VSCROLL:
        GetClientRect(g_main, &rc);
        prevpos = VscrollPos;
        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            VscrollPos -= 15;
            break;
        case SB_LINEDOWN:
            VscrollPos += 15;
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            VscrollPos = HIWORD(wParam);
            break;
        default:
            break;
        }

		VscrollPos = max(0, min(VscrollPos, mainPage.GetYMax()));

        if (VscrollPos != GetScrollPos(hwnd, SB_VERT)) {
            SetScrollPos(hwnd, SB_VERT, VscrollPos, TRUE);
            InvalidateRect(hwnd, NULL, TRUE);
            ScrollWindowEx(g_main, 0, prevpos - VscrollPos, NULL, NULL, NULL, &rc, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
            UpdateWindow(g_main);
		}
		break;
	case WM_KEYDOWN:
		WORD wScrollNotify;
		wScrollNotify = 0xFFFF;

		switch (wParam)
		{
		case VK_UP:
			wScrollNotify = SB_LINEUP;
			break;
		case VK_PRIOR:
			wScrollNotify = SB_PAGEUP;
			break;
		case VK_NEXT:
			wScrollNotify = SB_PAGEDOWN;
			break;
		case VK_DOWN:
			wScrollNotify = SB_LINEDOWN;
			break;
		case VK_HOME:
			wScrollNotify = SB_TOP;
			break;
		case VK_END:
			wScrollNotify = SB_BOTTOM;
			break;
		}

		if (wScrollNotify != -1)
			SendMessage(hwnd, WM_VSCROLL, MAKELONG(wScrollNotify, 0), 0L);

		break;
    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case EN_REQUESTRESIZE:
            mainPage.Resize(((REQRESIZE*)lParam)->nmhdr.hwndFrom, ((REQRESIZE*)lParam)->rc.bottom - ((REQRESIZE*)lParam)->rc.top);
            break;
        }
        break;

    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}