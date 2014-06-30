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

	//ShowWindow(g_main, SW_SHOWMAXIMIZED);
	
	//Enable to go fullscreen
	SetWindowLong(g_main, GWL_STYLE, 0);
	ShowWindow(g_main, SW_NORMAL);

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
	static Page mainPage;
    static int VscrollPos = 0, prevpos, xClient, yClient;
    RECT rc;
    SCROLLINFO si;
	static HDC hdcStatic;
    int xoffset = 50, yoffset = 50;
	static HWND hSubmit = 0;
    switch (msg)
    {
	case WM_CTLCOLORSTATIC:
		hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkMode(hdcStatic, TRANSPARENT);
		SetBkColor(hdcStatic, RGB(255, 255, 255));
		return (LRESULT)GetStockObject(NULL_BRUSH);
	case WM_CREATE:
		mainPage.Initialize(hwnd);
    case WM_SIZE:
        yClient = HIWORD(lParam);
        xClient = LOWORD(lParam);

        mainPage.Resize(hwnd);

        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
		si.nMax = mainPage.GetTotalHeight() + 50;
        si.nPage = yClient;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
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

		VscrollPos = max(0, min(VscrollPos, mainPage.GetTotalHeight() + 50));

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
			if (hSubmit == 0)
			{
				//hSubmit = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", "Submit", WS_VISIBLE | WS_CHILD | SS_SIMPLE, 0, 0, 0, 0, hwnd, NULL, hInstance, NULL);
			}
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}