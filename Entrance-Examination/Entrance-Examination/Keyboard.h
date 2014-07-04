#pragma once

#include <Windows.h>

const char g_keys[] = {
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M'
};

const int g_maxKeys = sizeof(g_keys);

const int g_bh = 40;
const int g_bw = 60;
const int g_bhm = 10; //horizontal margin
const int g_bvm = 10; //vertical margin

class Key
{
public:
	char m_value;
	HWND hKey;
	Key() :m_value(0){}
	void Init(HWND hWnd, int i)
	{
		m_value = g_keys[i];
		wchar_t bttnLabel[2] = { 0 };
		char bl[2] = { 0 };
		bl[0] = m_value;
		MultiByteToWideChar(CP_ACP, 0, bl, -1, bttnLabel, 2);
		hKey = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", bttnLabel, WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
		static HFONT hFont = CreateFont(24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
		SendMessage(hKey, WM_SETFONT, (WPARAM)hFont, TRUE);
	}
	void Move(int x, int y)
	{
		MoveWindow(hKey, x, y, g_bw, g_bh, TRUE);
	}
	bool Check(HWND hTest)
	{
		if (hKey == hTest)
		{
			
			INPUT ip;
			ip.type = INPUT_KEYBOARD;
			ip.ki.wScan = 0;
			ip.ki.time = 0;
			ip.ki.dwExtraInfo = 0;

			ip.ki.wVk = m_value;
			ip.ki.dwFlags = 0;	
			SendInput(1, &ip, sizeof(INPUT));

			ip.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &ip, sizeof(INPUT));
			//MessageBox(0, L"fuck yeh", 0, 0);
			return true;
		}
		return false;
	}
	void CleanUp()
	{
		DestroyWindow(hKey);
	}
};


class Keyboard
{
private:
	Key m_keys[g_maxKeys];
	HWND hSpace, hBKSpace;
public:
	Keyboard()
	{
		
	}
	void Init(HWND hWnd, int x, int y)
	{
		for (int i = 0; i < g_maxKeys; i++)
		{
			m_keys[i].Init(hWnd, i);
		}
		for (int i = 0; i < 10; i++) m_keys[i].Move(x + i*(g_bhm + g_bw), y);
		for (int i = 0; i < 10; i++) m_keys[10 + i].Move(x + i*(g_bhm + g_bw), y + (g_bvm + g_bh));
		for (int i = 0; i < 9; i++) m_keys[2 * 10 + i].Move( x + i*(g_bhm + g_bw), y + 2*(g_bvm + g_bh));
		for (int i = 0; i < 7; i++) m_keys[3 * 10 + i].Move( x + i*(g_bhm + g_bw), y + 3*(g_bvm + g_bh));
		hSpace = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Space", WS_VISIBLE | WS_CHILD, x + 6 * (g_bhm + g_bw), 
			y + 3 * (g_bvm + g_bh), (g_bhm + g_bw) * 4 - g_bvm, g_bh, hWnd, NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
		hBKSpace = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"←", WS_VISIBLE | WS_CHILD, x + 9 * (g_bhm + g_bw),
			y + 2 * (g_bvm + g_bh), g_bw, g_bh, hWnd, NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
		static HFONT hFont = CreateFont(44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
		SendMessage(hBKSpace, WM_SETFONT, (WPARAM)hFont, TRUE);
		static HFONT hFont2 = CreateFont(24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
		SendMessage(hSpace, WM_SETFONT, (WPARAM)hFont2, TRUE);
	}
	char Check(HWND hTest)
	{
		for (int i = 0; i < g_maxKeys; i++)
		{
			if (m_keys[i].Check(hTest)) return g_keys[i];
		}
		if (hTest == hSpace){
			INPUT ip;
			ip.type = INPUT_KEYBOARD;
			ip.ki.wScan = 0;
			ip.ki.time = 0;
			ip.ki.dwExtraInfo = 0;

			ip.ki.wVk = VK_SPACE;
			ip.ki.dwFlags = 0;
			SendInput(1, &ip, sizeof(INPUT));

			ip.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &ip, sizeof(INPUT));
			return true;
		}
		else if (hTest == hBKSpace){
			INPUT ip;
			ip.type = INPUT_KEYBOARD;
			ip.ki.wScan = 0;
			ip.ki.time = 0;
			ip.ki.dwExtraInfo = 0;

			ip.ki.wVk = VK_BACK;
			ip.ki.dwFlags = 0;
			SendInput(1, &ip, sizeof(INPUT));

			ip.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &ip, sizeof(INPUT));
			return true;
		}
		return -1;
	}
	void CleanUp()
	{
		for (int i = 0; i < g_maxKeys; i++)
		{
			m_keys[i].CleanUp();
		}
		DestroyWindow(hSpace);
		DestroyWindow(hBKSpace);
	}
};