#pragma once


#include <MsHTML.h>
#include <comdef.h>
#include <comdefsp.h>
#include <exdisp.h>
#include "ax.h"


class Question
{
private:
    HWND m_redit, m_opta, m_optb, m_optc, m_optd, m_optar, m_optbr, m_optcr, m_optdr, m_lbl;
    char m_ans;
    HWND CreateBrowser(HWND parent, HINSTANCE hinstance)
    {
        HWND hd = CreateWindowEx(0, L"AX", L"{8856F961-340A-11D0-A96B-00C04FD705A2}", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, parent, NULL, hinstance, NULL);
        SendMessage(hd, AX_INPLACE, 1, 0);
        return hd;
    }
    void Refresh(HWND hd)
    {
        IWebBrowser2* wb = 0;
        SendMessage(hd, AX_QUERYINTERFACE, (WPARAM)&IID_IWebBrowser2, (LPARAM)&wb);
        if (wb) {
            IHTMLDocument2 * document;
            IDispatch * dispatch;
            wb->get_Document((IDispatch**)&dispatch);
            if (dispatch != NULL) {
                dispatch->QueryInterface(IID_IHTMLDocument2, (void**)&document);

                IHTMLElement * body1;
                IHTMLElement2 * body;
                document->get_body(&body1);
                body1->QueryInterface(IID_IHTMLElement2, (void**)&body);

                IHTMLStyle * style;
                IHTMLStyle2 * style2;
                body1->get_style(&style);
                style->QueryInterface(IID_IHTMLStyle2, (void**)&style2);
                style2->put_overflowY(L"hidden");
                style->put_border(L"0px");
                style->Release();
                style2->Release();
                body->Release();
                body1->Release();

                document->Release();
                document = NULL;
            }
            wb->Release();
        }
    }
    int GetHeight(HWND hwnd)
    {
        long p = 50;
        IWebBrowser2* wb = 0;
        SendMessage(hwnd, AX_QUERYINTERFACE, (WPARAM)&IID_IWebBrowser2, (LPARAM)&wb);
        if (wb) {
            IHTMLDocument2 * document;
            IDispatch * dispatch;
            wb->get_Document((IDispatch**)&dispatch);
            if (dispatch != NULL) {
                dispatch->QueryInterface(IID_IHTMLDocument2, (void**)&document);
                
                IHTMLElement * body1;
                IHTMLElement2 * body;
                document->get_body(&body1);
                body1->QueryInterface(IID_IHTMLElement2, (void**)&body);
                body->get_scrollHeight(&p);                
                body->Release();
                body1->Release();

                document->Release();
                document = NULL;
            }
            wb->Release();
        }
        return (int)p + 10;
    }
    void SetHTML(HWND hwnd, std::string content)
    {
        std::string html;
        char myExeDir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, myExeDir);
       
        html = "<head><base href=\"file:///" + std::string(myExeDir) + "\\\" target=\"_blank\"></head>" + content;
           

        IWebBrowser2* wb = 0;
        SendMessage(hwnd, AX_QUERYINTERFACE, (WPARAM)&IID_IWebBrowser2, (LPARAM)&wb);
        if (wb) {
            wb->Navigate(L"about:blank", 0, 0, 0, 0);

            IHTMLDocument2 * document;
            IDispatch * dispatch;
            wb->get_Document((IDispatch**)&dispatch);
            if (dispatch != NULL) {
                dispatch->QueryInterface(IID_IHTMLDocument2, (void**)&document);
                SAFEARRAY *safe_array = SafeArrayCreateVector(VT_VARIANT, 0, 1);
                VARIANT *variant;
                SafeArrayAccessData(safe_array, (LPVOID *)&variant);
                variant->vt = VT_BSTR;
                
                const size_t cSize = html.size() + 1;
                wchar_t* wc = new wchar_t[cSize];
                size_t c; mbstowcs_s(&c, wc, cSize, html.c_str(), cSize);

                variant->bstrVal = SysAllocString(wc);
                SafeArrayUnaccessData(safe_array);
                document->write(safe_array);
                document->Release();
                document = NULL;

                delete[] wc;
            }
            wb->Release();
        }
    }
public:
	Question()
	{
	}
	void Initialize(HWND hWnd, int qn)
	{
		static HFONT hFont = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
        std::wstringstream ss(L"");
        ss << qn << L". ";

		HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		RECT wndRect = { 0 };
		GetClientRect(hWnd, &wndRect);
        
        m_lbl = CreateWindowEx(0, L"STATIC", ss.str().c_str(), WS_VISIBLE | WS_CHILD | SS_SIMPLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
		SendMessage(m_lbl, WM_SETFONT, (WPARAM)hFont, TRUE);

        m_redit = CreateBrowser(hWnd, hInstance);
            //CreateWindowEx(WS_EX_TRANSPARENT, MSFTEDIT_CLASS, L"QUESTION", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
            //0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        //SendMessage(m_redit, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);

        m_opta = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        m_optar = CreateBrowser(hWnd, hInstance);
        //CreateWindowEx(WS_EX_TRANSPARENT, MSFTEDIT_CLASS, L"OPTION A", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
           // 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        //SendMessage(m_optar, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);

        m_optb = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        m_optbr = CreateBrowser(hWnd, hInstance);
        //CreateWindowEx(WS_EX_TRANSPARENT, MSFTEDIT_CLASS, L"OPTION B", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
          //  0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        //SendMessage(m_optbr, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);

        m_optc = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        m_optcr = CreateBrowser(hWnd, hInstance);
        //CreateWindowEx(WS_EX_TRANSPARENT, MSFTEDIT_CLASS, L"OPTION C", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
          //  0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        //SendMessage(m_optcr, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);

        m_optd = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        m_optdr = CreateBrowser(hWnd, hInstance);
        //CreateWindowEx(WS_EX_TRANSPARENT, MSFTEDIT_CLASS, L"OPTION D", ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
          //  0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        //SendMessage(m_optdr, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);
	}

    void Destroy()
    {
        DestroyWindow(m_redit);
        DestroyWindow(m_lbl);
        DestroyWindow(m_opta);
        DestroyWindow(m_optb);
        DestroyWindow(m_optc);
        DestroyWindow(m_optd);
        DestroyWindow(m_optar);
        DestroyWindow(m_optbr);
        DestroyWindow(m_optcr);
        DestroyWindow(m_optdr);
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
                /*SendMessage(m_redit, EM_SETREADONLY, FALSE, NULL);
                SendMessage(m_redit, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)str);
                SendMessage(m_redit, EM_SETREADONLY, TRUE, NULL);*/
                SetHTML(m_redit, str);
                
            }
            else if (j == 1) {
                /*SendMessage(m_optar, EM_SETREADONLY, FALSE, NULL);
                SendMessage(m_optar, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)str);
                SendMessage(m_optar, EM_SETREADONLY, TRUE, NULL);*/
                SetHTML(m_optar, str);
            }
            else if (j == 2) {
                /*SendMessage(m_optbr, EM_SETREADONLY, FALSE, NULL);
                SendMessage(m_optbr, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)str);
                SendMessage(m_optbr, EM_SETREADONLY, TRUE, NULL);*/
                SetHTML(m_optbr, str);
            }
            else if (j == 3) {
                /*SendMessage(m_optcr, EM_SETREADONLY, FALSE, NULL);
                SendMessage(m_optcr, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)str);
                SendMessage(m_optcr, EM_SETREADONLY, TRUE, NULL);*/
                SetHTML(m_optcr, str);
            }
            else if (j == 4) {
                /*SendMessage(m_optdr, EM_SETREADONLY, FALSE, NULL);
                SendMessage(m_optdr, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)str);
                SendMessage(m_optdr, EM_SETREADONLY, TRUE, NULL);*/
                SetHTML(m_optdr, str);
            }
            delete[] str;
        }
        file.read((char*)&m_ans, sizeof(m_ans));

        Refresh(m_redit); Refresh(m_optar); Refresh(m_optbr); Refresh(m_optcr); Refresh(m_optdr);
    }

	void Reposition(int x, int &y, int w)
	{
		int rOff = 10;

        int sz = GetHeight(m_redit);
        MoveWindow(m_redit, x + rOff, y, w, sz, true);
        MoveWindow(m_lbl, x - 25, y, 25, sz, true);
        y += sz + 10;

        sz = GetHeight(m_optar);
        MoveWindow(m_opta, x + rOff, y, 14, 20, true);
        MoveWindow(m_optar, x + rOff + 18, y, w - 40 - rOff, sz, true);

        y += sz + 10;
        sz = GetHeight(m_optbr);
        MoveWindow(m_optb, x + rOff, y, 14, 20, true);
        MoveWindow(m_optbr, x + rOff + 18, y, w - 40 - rOff, sz, true);

        y += sz + 10;
        sz = GetHeight(m_optcr);
        MoveWindow(m_optc, x + rOff, y, 14, 20, true);
        MoveWindow(m_optcr, x + rOff + 18, y, w - 40 - rOff, sz, true);

        y += sz + 10;
        sz = GetHeight(m_optdr);
        MoveWindow(m_optd, x + rOff, y, 14, 20, true);
        MoveWindow(m_optdr, x + rOff + 18, y, w - 40 - rOff, sz, true);
        y += sz + 50;
	}
};
