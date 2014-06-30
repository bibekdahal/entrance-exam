#include <windows.h>
#include <CommCtrl.h>
#include <Richedit.h>
#include <stdio.h>
#include "resource.h"

#include "Page.h"
#include "Keyboard.h"
#include "User.h"

HWND g_main;
#pragma comment( lib, "comctl32.lib" )
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " "version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

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

HWND hEditUN = 0;
HWND lastFocus = 0;
HWND hLblUN = 0;
HWND hLblPW = 0;
HWND hEditPW = 0;
HWND hLoginSubmitBttn = 0;

void CreateLoginForm(HWND hWnd)
{
	int x = 400, y = 200;
	static HFONT hFont = CreateFont(24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	hLblUN = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"Username:", WS_VISIBLE | WS_CHILD | SS_SIMPLE,
		x, y, 100, 30, hWnd, NULL, hInst, NULL);
	hEditUN = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
		x + 110, y - 4, 300, 30, hWnd, NULL, hInst, NULL);
	hLblPW = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"Password:", WS_VISIBLE | WS_CHILD | SS_SIMPLE,
		x, y + 100, 100, 160, hWnd, NULL, hInst, NULL);
	hEditPW = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
		x + 110, y + 100 - 4, 300, 30, hWnd, NULL, hInst, NULL);
	hLoginSubmitBttn = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Login", WS_VISIBLE | WS_CHILD, 
		x + 400 - 95, y + 180, 100, 30, hWnd, NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
	SendMessage(hLblUN, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hEditUN, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hLblPW, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hEditPW, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hLoginSubmitBttn, WM_SETFONT, (WPARAM)hFont, TRUE);
}

const int ID_TIMER = 10;
bool examrunning = false;
void Start(Page & mainPage, HWND hwnd)
{
    examrunning = true;
    mainPage.Initialize(hwnd);
    mainPage.ResizeControls(hwnd, 0);
    RECT rc;
    SCROLLINFO si;
    GetClientRect(hwnd, &rc);
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE;
    si.nMin = 0;
    si.nMax = mainPage.GetYMax();
    si.nPage = rc.bottom - rc.top;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
    SetTimer(hwnd, ID_TIMER, 1000, NULL); 
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static Page mainPage;
	static User user;
	static Keyboard onScreenKeyboard;
    static int VscrollPos = 0, prevpos, xClient, yClient;
    RECT rc;
    SCROLLINFO si;

    HDC hdc;
    PAINTSTRUCT ps;
	WCHAR szBuffer[200] = { 0 };
    static int count = 60 * 60 * 3;
    int seconds = 0;
    int minutes = 0;
    int hours = 0;

    switch (msg)
    {
	case WM_CREATE:
		//mainPage.Initialize(hwnd);


		CreateLoginForm(hwnd);

		onScreenKeyboard.Init(hwnd, 300, 500);
		//SetTimer(hwnd, ID_TIMER, 1000, NULL);
		break;
    case WM_PAINT:
        if (count > 0 && examrunning)
        {
            hdc = BeginPaint(hwnd, &ps);
			static HFONT hFont = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
			SelectObject(hdc, hFont);
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

        mainPage.ResizeControls(hwnd, -VscrollPos);

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
        else if ((HWND)lParam == mainPage.GetNextPageHandle() || (HWND)lParam == mainPage.GetPrevPageHandle())
        {
            if ((HWND)lParam == mainPage.GetNextPageHandle()) mainPage.NextPage(); 
            else mainPage.PreviousPage();
            
            VscrollPos = 0;
            SetScrollPos(hwnd, SB_VERT, 0, TRUE);

            mainPage.ResizeControls(g_main, 0);

            GetClientRect(g_main, &rc);
            si.cbSize = sizeof(si);
            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMin = 0;
            si.nMax = mainPage.GetYMax();
            si.nPage = rc.bottom - rc.top;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        }
		else if (!user.LoggedIn())
		{
			//SetFocus(hEditUN);
			char ch = onScreenKeyboard.Check((HWND)lParam);
			if (ch != -1)
			{
				//INPUT ip;
				//ip.type = INPUT_KEYBOARD;
				//ip.ki.wScan = 0;
				//ip.ki.time = 0;
				//ip.ki.dwExtraInfo = 0;
				//
				//ip.ki.wVk = ch;
				//ip.ki.dwFlags = 0;
				//SendInput(1, &ip, sizeof(INPUT));
				////Sleep(20);
				//ip.ki.dwFlags = KEYEVENTF_KEYUP;
				//SendInput(1, &ip, sizeof(INPUT));
				SetFocus(hEditUN);
			}
			
		}
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
        case VK_RETURN:
            Start(mainPage, g_main);
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
    case WM_CTLCOLORSTATIC:
    {
                              HDC hdcStatic = (HDC)wParam;
                              SetBkMode(hdcStatic, TRANSPARENT); 
                              HBRUSH BGColorBrush = (HBRUSH)(COLOR_WINDOW + 1);
                              return (LRESULT)BGColorBrush;
    }
        break;

    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}