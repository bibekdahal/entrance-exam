#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <Richedit.h>

class Question
{
private:
	HWND m_redit, m_opta, m_optb, m_optc, m_optd, m_optar, m_optbr, m_optcr, m_optdr;
public:
	Question()
	{
	}
	void Initialize(HWND hWnd)
	{
		HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		RECT wndRect = { 0 };
		GetClientRect(hWnd, &wndRect);
		int w = wndRect.right - 100;
		int h = 100;
		//int qw = 
		m_redit = CreateWindowEx(0, MSFTEDIT_CLASS, L"", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP,
			0, 0, 640, 150, hWnd, NULL, hInstance, NULL);

		m_opta = CreateWindowEx(WS_EX_WINDOWEDGE | WS_EX_TRANSPARENT, L"BUTTON", L"",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
			10, 160, 150, 20, hWnd, NULL, hInstance, NULL);

		m_optar = CreateWindowEx(0, MSFTEDIT_CLASS, L"OPTION A", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
			0, 0, 0, 0, hWnd, NULL, hInstance, NULL);

		m_optb = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 200, 160, 150, 20, hWnd, NULL, hInstance, NULL);

		m_optbr = CreateWindowEx(0, MSFTEDIT_CLASS, L"OPTION B", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
			0, 0, 0, 0, hWnd, NULL, hInstance, NULL);

		m_optc = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 10, 200, 150, 20,
			hWnd, NULL, hInstance, NULL);

		m_optcr = CreateWindowEx(0, MSFTEDIT_CLASS, L"OPTION C", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
			0, 0, 0, 0, hWnd, NULL, hInstance, NULL);

		m_optd = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			200, 200, 150, 20, hWnd, NULL, hInstance, NULL);
		m_optdr = CreateWindowEx(0, MSFTEDIT_CLASS, L"OPTION D", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
			0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
	}

	void Reposition(int x, int y, int w)
	{
		int rOff = 10;
		MoveWindow(m_redit, x + rOff, y, w, 150, true);

		MoveWindow(m_opta, x + rOff, y + 160, 14, 14, true);
		MoveWindow(m_optb, x + rOff, y + 200, 14, 14, true);
		MoveWindow(m_optc, x + rOff, y + 240, 14, 14, true);
		MoveWindow(m_optd, x + rOff, y + 280, 14, 14, true);

		MoveWindow(m_optar, x + rOff + 20, y + 160, w - 40 - rOff, 20, true);
		MoveWindow(m_optbr, x + rOff + 20, y + 200, w - 40 - rOff, 20, true);
		MoveWindow(m_optcr, x + rOff + 20, y + 240, w - 40 - rOff, 20, true);
		MoveWindow(m_optdr, x + rOff + 20, y + 280, w - 40 - rOff, 20, true);
	}
	void GetRelativeCtrlRect(HWND hWnd, RECT *rc) {
		GetWindowRect(hWnd, rc);
		ScreenToClient(GetParent(hWnd), (LPPOINT)&((LPPOINT)rc)[0]);
		ScreenToClient(GetParent(hWnd), (LPPOINT)&((LPPOINT)rc)[1]);
		ScreenToClient(GetParent(hWnd), (LPPOINT)&((LPPOINT)rc)[2]);
		ScreenToClient(GetParent(hWnd), (LPPOINT)&((LPPOINT)rc)[3]);
	}

	void Scroll(int dy)
	{
		RECT cR = { 0 };
		GetRelativeCtrlRect(m_redit, &cR);
		this->Reposition(cR.left - 10, cR.top + dy, cR.right - cR.left);
	}
};
