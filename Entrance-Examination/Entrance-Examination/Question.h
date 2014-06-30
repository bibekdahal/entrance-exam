#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <Richedit.h>
#include <fstream>
#include <sstream>
#include <map>

class Question
{
private:
    HWND m_redit, m_opta, m_optb, m_optc, m_optd, m_optar, m_optbr, m_optcr, m_optdr, m_lbl;
    char m_ans;
public:
	Question()
	{
	}
	void Initialize(HWND hWnd, int qn)
	{
        std::wstringstream ss(L"");
        ss << qn << L". ";

		HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		RECT wndRect = { 0 };
		GetClientRect(hWnd, &wndRect);
        
        m_lbl = CreateWindowEx(0, L"STATIC", ss.str().c_str(), WS_VISIBLE | WS_CHILD | SS_SIMPLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);

        m_redit = CreateWindowEx(WS_EX_TRANSPARENT, MSFTEDIT_CLASS, L"QUESTION", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        SendMessage(m_redit, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);

        m_opta = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        m_optar = CreateWindowEx(WS_EX_TRANSPARENT, MSFTEDIT_CLASS, L"OPTION A", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        SendMessage(m_optar, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);

        m_optb = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        m_optbr = CreateWindowEx(WS_EX_TRANSPARENT, MSFTEDIT_CLASS, L"OPTION B", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        SendMessage(m_optbr, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);

        m_optc = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        m_optcr = CreateWindowEx(WS_EX_TRANSPARENT, MSFTEDIT_CLASS, L"OPTION C", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        SendMessage(m_optcr, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);

        m_optd = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        m_optdr = CreateWindowEx(WS_EX_TRANSPARENT, MSFTEDIT_CLASS, L"OPTION D", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        SendMessage(m_optdr, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);
	}

    bool GetCheck(HWND radio)
    {
        if (SendMessage(radio, BM_GETCHECK, 0, 0) == BST_CHECKED) return true;
        return false;
    }
    char GetAnswer()
    {
        char ans = -1;
        if (GetCheck(m_opta)) ans = 0;
        else if (GetCheck(m_optb)) ans = 1;
        else if (GetCheck(m_optc)) ans = 2;
        else if (GetCheck(m_optd)) ans = 3;
        return ans;
    }


    void LoadFromFile(std::fstream &file)
    {
        for (int j = 0; j < 5; ++j)
        {
            int length;
            file.read((char*)&length, sizeof(length));
            char * str = new char[length + 1];
            file.read(str, length);
            str[length] = '\0';

            SETTEXTEX st = { 0 };
            st.codepage = 1200;
            st.flags = ST_DEFAULT;
            if (j == 0) {
                SendMessage(m_redit, EM_SETREADONLY, FALSE, NULL);
                SendMessage(m_redit, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)str);
                SendMessage(m_redit, EM_SETREADONLY, TRUE, NULL);
            }
            else if (j == 1) {
                SendMessage(m_optar, EM_SETREADONLY, FALSE, NULL);
                SendMessage(m_optar, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)str);
                SendMessage(m_optar, EM_SETREADONLY, TRUE, NULL);
            }
            else if (j == 2) {
                SendMessage(m_optbr, EM_SETREADONLY, FALSE, NULL);
                SendMessage(m_optbr, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)str);
                SendMessage(m_optbr, EM_SETREADONLY, TRUE, NULL);
            }
            else if (j == 3) {
                SendMessage(m_optcr, EM_SETREADONLY, FALSE, NULL);
                SendMessage(m_optcr, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)str);
                SendMessage(m_optcr, EM_SETREADONLY, TRUE, NULL);
            }
            else if (j == 4) {
                SendMessage(m_optdr, EM_SETREADONLY, FALSE, NULL);
                SendMessage(m_optdr, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)str);
                SendMessage(m_optdr, EM_SETREADONLY, TRUE, NULL);
            }
            delete[] str;
        }

        file.read((char*)&m_ans, sizeof(m_ans));
    }

	void Reposition(int x, int &y, int w, std::map<HWND, int> &sizes)
	{
		int rOff = 10;

        int sz = max(50, sizes[m_redit]);
        MoveWindow(m_redit, x + rOff, y, w, sz, true);
        MoveWindow(m_lbl, x - 25, y, 25, sz, true);
        y += sz + 10;

        sz = max(20, sizes[m_optar]);
        MoveWindow(m_opta, x + rOff, y, 20, 20, true);
        MoveWindow(m_optar, x + rOff + 18, y, w - 40 - rOff, sz, true);

        y += sz + 10;
        sz = max(20, sizes[m_optbr]);
        MoveWindow(m_optb, x + rOff, y, 20, 20, true);
        MoveWindow(m_optbr, x + rOff + 18, y, w - 40 - rOff, sz, true);

        y += sz + 10;
        sz = max(20, sizes[m_optcr]);
        MoveWindow(m_optc, x + rOff, y, 20, 20, true);
        MoveWindow(m_optcr, x + rOff + 18, y, w - 40 - rOff, sz, true);

        y += sz + 10;
        sz = max(20, sizes[m_optdr]);
        MoveWindow(m_optd, x + rOff, y, 20, 20, true);
        MoveWindow(m_optdr, x + rOff + 18, y, w - 40 - rOff, sz, true);
        y += sz + 50;
	}
};
