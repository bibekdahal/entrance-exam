#pragma once

#include <Windows.h>

const char g_keys[] = {
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'
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
	}
	void Move(int x, int y)
	{
		MoveWindow(hKey, x, y, g_bw, g_bh, TRUE);
	}
};


class Keyboard
{
private:
	Key m_keys[g_maxKeys];
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
		for (int j = 0; j < g_maxKeys / 10; j++)
		{
			for (int i = 0; i < 10; i++)
			{
				m_keys[j * 10 + i].Move(x + i*(g_bhm + g_bw), y + j*(g_bvm + g_bh));
			}
		}
	}

};