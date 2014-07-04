#include <windows.h>
#include <CommCtrl.h>
#include <Richedit.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "resource.h"

#include "User.h"
#include "Page.h"
#include "Keyboard.h"

HWND g_main;
#pragma comment( lib, "comctl32.lib" )
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " "version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow);

char GetRandChar()
{
	char rch = 'a' + rand() % 26;
	return rch;
}

std::string GetRandomString()
{
	char rstr[16] = { 0 };
	for (int i = 0; i < 8; i++)
	{
		rstr[i] = GetRandChar();
	}
	return std::string(rstr);
}

class UserStream
{
public:
	static int idCounter;
	std::string id;
	std::string pw;
	UserStream()
	{
		char buff[10] = { 0 };
		itoa(++idCounter, buff, 10);
		id = std::string(buff);
		pw = GetRandomString();
	}
	void WriteOP(std::ofstream& formatted, std::ofstream& encrypted)
	{
		std::string eid = id, epw = pw;
		for (int i = 0; i < 8; i++)
		{
			epw[i] = epw[i] + 100;
		}
		for (int i = 0; i < 4; i++)
		{
			eid[i] = eid[i] + 100;
		}
		encrypted.write(eid.c_str(), 4);
		encrypted.write(epw.c_str(), 8);
		formatted << "Userid: " << id << std::endl;
		formatted << "Password: " << pw << std::endl;
	}
};

int UserStream::idCounter = 1200;

void BuildPass()
{
	std::ofstream of, off;
	of.open("uli.dat");
	off.open("unpw.txt");
	for (int i = 0; i < 300; i++)
	{
		UserStream us;
		us.WriteOP(off, of);
		off << std::endl;
	}
	of.close();
	off.close();
}


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

    g_main = CreateWindowEx(0, L"frobi-entranceexam", L"Entrance Examination", WS_OVERLAPPEDWINDOW | WS_VSCROLL, 
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, hInstance, 0);

    // Two calls are made so that:
    // after first call, the rich edit controls are created and initialized with text, and
    // after second call, a Request-Resize message is sent to each rich edit control and controls are resized according to their contents
	ShowWindow(g_main, SW_NORMAL);
    //ShowWindow(g_main, SW_MAXIMIZE);

	//Enable to go fullscreen
	SetWindowLong(g_main, GWL_STYLE, 0);
	ShowWindow(g_main, SW_NORMAL);

    MSG Msg = { 0 };
    while (GetMessageA(&Msg, 0, 0, 0))
    {
        if (!IsDialogMessage(g_main, &Msg))
        {
            TranslateMessage(&Msg);
            DispatchMessageA(&Msg);
        }
    }
    return 0;
}

HWND hEditName = 0;
HWND hEditUN = 0;
HWND lastFocus = 0;
HWND hLblName = 0;
HWND hLblUN = 0;
HWND hLblPW = 0;
HWND hEditPW = 0;
HWND hLoginSubmitBttn = 0;

void CreateLoginForm(HWND hWnd)
{
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);
	int x = w/2 - 210, y = 220;
	static HFONT hFont = CreateFont(24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	hLblName = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"Name:", WS_VISIBLE | WS_CHILD | SS_SIMPLE,
		x, y, 110, 30, hWnd, NULL, hInst, NULL);
    hEditName = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
		x + 120, y - 4, 300, 30, hWnd, NULL, hInst, NULL);
    hLblUN = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"Exam Roll No:", WS_VISIBLE | WS_CHILD | SS_SIMPLE,
		x, y + 60, 110, 30, hWnd, NULL, hInst, NULL);
    hEditUN = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
		x + 120, y + 60 - 4, 300, 30, hWnd, NULL, hInst, NULL);
    hLblPW = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"Password:", WS_VISIBLE | WS_CHILD | SS_SIMPLE,
		x, y + 120, 110, 160, hWnd, NULL, hInst, NULL);
    hEditPW = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
		x + 120, y + 120 - 4, 300, 30, hWnd, NULL, hInst, NULL);
	hLoginSubmitBttn = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Login", WS_VISIBLE | WS_CHILD, 
		x + 420 - 100, y + 180, 100, 30, hWnd, NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
	SendMessage(hLblName, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hEditName, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hLblUN, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hEditUN, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hLblPW, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hEditPW, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hLoginSubmitBttn, WM_SETFONT, (WPARAM)hFont, TRUE);
	SetFocus(hEditName);
}

void RemoveLoginForm()
{
	DestroyWindow(hLblName);
	DestroyWindow(hEditName);
	DestroyWindow(hLblUN);
	DestroyWindow(hLblPW);
	DestroyWindow(hEditUN);
	DestroyWindow(hEditPW);
	DestroyWindow(hLoginSubmitBttn);
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

RECT rcStatus = { 0 };

void DrawStatusText(HWND hwnd, HDC hdc, User& user, Page& mainPage)
{ 
	int seconds = 0;
	int minutes = 0;
	int hours = 0;
	int count = 60 * 60;
	RECT rc = { 0 };
	static HFONT hFont = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
	SelectObject(hdc, hFont);
	GetClientRect(hwnd, &rc);
	rc.left = 30;
	rc.right = 300;
	rc.top = 30;
	wchar_t fbuff[256] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, user.UserName().c_str(), -1, fbuff, sizeof(fbuff));
	HBRUSH hBR = CreateSolidBrush(RGB(230, 230, 230));
	RECT rcf = { 0, 0, 200, GetSystemMetrics(SM_CYSCREEN) };
	FillRect(hdc, &rcf, hBR);
	rcf.left = GetSystemMetrics(SM_CXSCREEN) - 200;
	rcf.right = GetSystemMetrics(SM_CXSCREEN);
	FillRect(hdc, &rcf, hBR);
	SetBkMode(hdc, TRANSPARENT); hours = count / 3600;
	minutes = (count / 60) % 60;
	seconds = count % 60;
	wchar_t szBuffer[512];
	wsprintf(szBuffer, L"Remaining Time:\n%2d min : %2d sec\nExam Roll No.: %s\nQuestion Solved: %2d/65", minutes, seconds, fbuff, mainPage.GetNoOfSolvedQuestions());
	DrawText(hdc, szBuffer, -1, &rc, DT_LEFT);
	rcStatus = rc;
}

void EraseStatusText(HDC hdc)
{
	HBRUSH hBR = CreateSolidBrush(RGB(230, 230, 230));
	FillRect(hdc, &rcStatus, hBR);
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
    static int count = 60 * 60;
    int seconds = 0;
    int minutes = 0;
    int hours = 0;

    switch (msg)
    {
	case WM_CREATE:
		//mainPage.Initialize(hwnd);
        mainPage.CreateTitleAndLogo(hwnd);
		CreateLoginForm(hwnd);
		onScreenKeyboard.Init(hwnd, GetSystemMetrics(SM_CXSCREEN) / 2 - 10 * (g_bw + g_bhm) / 2, GetSystemMetrics(SM_CYSCREEN)-4*(g_bh+g_bvm)-20);
		//SetTimer(hwnd, ID_TIMER, 1000, NULL);
		break;
    case WM_PAINT:
        if (count > 0 && examrunning)
        {
            hdc = BeginPaint(hwnd, &ps);
			DrawStatusText(hwnd, hdc, user, mainPage);
            EndPaint(hwnd, &ps);
        }
        else if (examrunning)
        {
            MessageBox(g_main, L"Your exam time is over.", L"Thank you!", 0);
            mainPage.Submit(user);
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
        {
            if (IDYES != MessageBox(hwnd, L"Are you sure you want to submit?\n\rOnce submitted, you can't change your answers", L"Confirm", MB_YESNO | MB_ICONINFORMATION))
                break;

            VscrollPos = 0;
            SetScrollPos(hwnd, SB_VERT, 0, TRUE);
            mainPage.ResizeControls(g_main, 0);
            mainPage.Submit(user);
            GetClientRect(g_main, &rc);
            si.cbSize = sizeof(si);
            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMin = 0;
            si.nMax = rc.bottom - rc.top;
            si.nPage = rc.bottom - rc.top;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

            examrunning = false;
            InvalidateRect(g_main, NULL, TRUE);
        }
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
				SetFocus(lastFocus);
				
			}
			if ((HWND)lParam == hEditPW)
			{
				lastFocus = hEditPW;
			}
			else if ((HWND)lParam == hEditUN)
			{
				lastFocus = hEditUN;
			}
			else if ((HWND)lParam == hEditName)
			{
				lastFocus = hEditName;
			}
			if ((HWND)lParam == hLoginSubmitBttn)
			{
				wchar_t wbuffUN[256] = { 0 };
				wchar_t wbuffPW[256] = { 0 };
				wchar_t wbuffName[256] = { 0 };
				char buffUN[256] = { 0 };
				char buffPW[256] = { 0 };
				char buffName[256] = { 0 };
				GetWindowText(hEditUN, wbuffUN, sizeof(wbuffUN));
				GetWindowText(hEditPW, wbuffPW, sizeof(wbuffPW));
				GetWindowText(hEditName, wbuffName, sizeof(wbuffName));
				WideCharToMultiByte(CP_ACP, 0, wbuffUN, -1, buffUN, sizeof(buffUN), 0, FALSE);
				WideCharToMultiByte(CP_ACP, 0, wbuffPW, -1, buffPW, sizeof(buffPW), 0, FALSE);
				WideCharToMultiByte(CP_ACP, 0, wbuffName, -1, buffName, sizeof(buffName), 0, FALSE);
				if (buffName[0] == 0)
				{
					MessageBox(hwnd, L"Enter your name", L"Form Incomplete", MB_OK | MB_ICONINFORMATION);
					SetFocus(hEditName);
					break;
				}
				if (buffUN[0] == 0)
				{
					MessageBox(hwnd, L"Enter your Exam Roll Number\n\r(1201 - 1500)", L"Form Incomplete", MB_OK | MB_ICONINFORMATION);
					SetFocus(hEditUN);
					break;
				}
				else
				{

					int ern = atoi(buffUN);
					if (ern < 1201 && ern < 1500)
					{
						MessageBox(hwnd, L"The Exam Roll Number is not valid\n\rYour Roll Number should be in the range of 1201 to 1500", L"Invalid Data",
							MB_OK | MB_ICONINFORMATION);
				SetFocus(hEditUN);
						break;
					}
				}
				if (buffPW[0] == 0)
				{
					MessageBox(hwnd, L"Enter your password", L"Form Incomplete", MB_OK | MB_ICONINFORMATION);
					SetFocus(hEditPW);
					break;
			}
				try
				{
			
					if (user.LogIn(buffName, buffUN, buffPW))
					{
						RemoveLoginForm();
						onScreenKeyboard.CleanUp();
						Start(mainPage, g_main);
					}
					else
					{
						MessageBox(hwnd, L"Your Exam Roll No. and password did not match", L"Error Logging in", MB_OK | MB_ICONERROR);
						break;
					}
				}
				catch (...)
				{
				}
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
			//HDC hDC = GetDC(hwnd);
			//EraseStatusText(hdc);
            ScrollWindowEx(g_main, 0, prevpos - VscrollPos, NULL, NULL, NULL, &rc, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
			//DrawStatusText(hwnd, hdc, user, mainPage);
			//ReleaseDC(hwnd, hdc);
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
			/*RemoveLoginForm();
			onScreenKeyboard.CleanUp();
			user.LogIn("Ankit", "ksdfso");
            Start(mainPage, g_main);*/
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