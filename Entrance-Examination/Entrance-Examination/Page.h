#pragma once

#include "Question.h"

const int max_q = 65;
const int qperpage = 20;

class Page
{
private:
    Question m_q[max_q];
    std::map<HWND, int> m_sizes;
    HWND m_submit, m_nextPage, m_prevPage, m_title, m_logo;
    int m_ymax, m_page;
    bool m_initialized;
public:
    HWND GetSubmitHandle() { return m_submit; }
    HWND GetNextPageHandle() { return m_nextPage; }
    HWND GetPrevPageHandle() { return m_prevPage; }
    HWND GetTitleHandle() { return m_title; }
    HWND GetLogoHandle() { return m_logo; }

    Page() : m_initialized(false)
	{

	}
    void CreateTitleAndLogo(HWND hWnd)
    {
        HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
        static HFONT hFont = CreateFont(42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
        m_title = CreateWindowEx(0, L"STATIC", L"Computer Based Model Exam", WS_VISIBLE | WS_CHILD | SS_CENTER, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        SendMessage(m_title, WM_SETFONT, (WPARAM)hFont, TRUE);

        m_logo = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_BITMAP, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        HBITMAP mybitmap = LoadBitmap(hInstance, MAKEINTRESOURCEW(IDB_LOGO));
        SendMessage(m_logo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)mybitmap);

        RECT wndRect = { 0 };
        GetClientRect(hWnd, &wndRect);
        MoveWindow(m_logo, wndRect.right / 2 - 45, 10, 90, 90, true);
        MoveWindow(m_title, wndRect.right / 2 - 250, 90 + 15, 500, 80, true);
    }
	void Initialize(HWND hWnd)
	{
        m_page = 0;
        m_initialized = true;

		int xOffset = 250, yOffset = 50;
		RECT wndRect = { 0 };
		GetClientRect(hWnd, &wndRect);
        int y = yOffset;
		for (int i = 0; i < max_q; i++)
			m_q[i].Initialize(hWnd, i+1);

        std::fstream file;
        file.open("set21.fbq", std::ios::in | std::ios::binary);
        unsigned int size;
        file.read((char*)&size, sizeof(size));
        if (size > max_q) size = max_q - 1;
        for (unsigned int i = 0; i < size; ++i)
            m_q[i].LoadFromFile(file);
        file.close();

        HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		static HFONT hFont = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
        m_submit = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"SUBMIT", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
		SendMessage(m_submit, WM_SETFONT, (WPARAM)hFont, TRUE);
        m_nextPage = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"NEXT PAGE", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
		SendMessage(m_nextPage, WM_SETFONT, (WPARAM)hFont, TRUE);
        m_prevPage = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"PREV PAGE", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
		SendMessage(m_prevPage, WM_SETFONT, (WPARAM)hFont, TRUE);
         
	}

    void Resize(HWND hWnd, int height)
    {
        m_sizes[hWnd] = height;
    }

    void ResizeControls(HWND hWnd, int yoff)
    {
        if (!m_initialized) return;

        int startq = m_page * 20;
        int endq = startq + 20;
        if (endq > max_q) endq = max_q ;

        int xOffset = 250, yOffset = 200 + yoff;
        RECT wndRect = { 0 };
        GetClientRect(hWnd, &wndRect);
        int y = yOffset; int dummy;

        MoveWindow(m_logo, wndRect.right / 2 - 45, 10, 90, 90, true);
        MoveWindow(m_title, wndRect.right / 2 - 250, 90 + 15, 500, 80, true);

        for (int i = 0; i < startq; i++)
            m_q[i].Reposition(-wndRect.right*2, dummy, wndRect.right - 2 * xOffset, m_sizes);
        for (int i = endq; i < max_q; i++)
            m_q[i].Reposition(-wndRect.right*2, dummy, wndRect.right - 2 * xOffset, m_sizes);

        for (int i = startq; i < endq; i++)
            m_q[i].Reposition(xOffset, y, wndRect.right - 2 * xOffset, m_sizes);


        if (m_page < (max_q / qperpage))
        {
            if (m_page != 0){
                MoveWindow(m_nextPage, wndRect.right / 2 + 10, y, 130, 40, true);
                MoveWindow(m_prevPage, wndRect.right / 2 - 130 - 10, y, 130, 40, true);
            }
            else {
                MoveWindow(m_nextPage, wndRect.right / 2 - 250 / 2, y, 130, 30, true);
                MoveWindow(m_prevPage, -wndRect.right, 0, 0, 0, true);
            }
            MoveWindow(m_submit, -wndRect.right, 0, 0, 0, true);
        }
        else
        {
            MoveWindow(m_submit, wndRect.right / 2 + 10, y, 130, 30, true);
            MoveWindow(m_prevPage, wndRect.right / 2 - 130 - 10, y, 130, 30, true);
            MoveWindow(m_nextPage, -wndRect.right, 0, 0, 0, true);
        }

        m_ymax = y + 150 - yoff;
    }
    int GetYMax() { return m_ymax; }
	int GetNoOfSolvedQuestions()
	{
		int count = 0;
		for (unsigned int i = 0; i < max_q; ++i)
		{
			if (m_q[i].GetAnswer() != -1) ++count;
		}
		return count;
	}

    void Submit()
    {
        std::fstream file;
        file.open("answers.fba", std::ios::out | std::ios::binary);
        unsigned int size = max_q;
        file.write((char*)&size, sizeof(size));
        for (unsigned int i = 0; i < size; ++i)
        {
            char ans = m_q[i].GetAnswer();
            file.write(&ans, sizeof(ans));
        }
        file.close();
        MessageBox(0, L"Your answers are submitted.", L"Thank you!", 0);
    }

    void NextPage()
    {
        m_page++;
    }
    void PreviousPage()
    {
        m_page--;
    }
};