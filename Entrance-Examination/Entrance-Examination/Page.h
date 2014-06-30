#pragma once

#include "Question.h"

const int max_q = 65;
const int qperpage = 20;

class Page
{
private:
    Question m_q[max_q];
    std::map<HWND, int> m_sizes;
    HWND m_submit, m_nextPage, m_prevPage, m_title;
    int m_ymax, m_page;
    bool m_initialized;
public:
    HWND GetSubmitHandle() { return m_submit; }
    HWND GetNextPageHandle() { return m_nextPage; }
    HWND GetPrevPageHandle() { return m_prevPage; }

    Page() : m_initialized(false)
	{

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

        m_submit = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"SUBMIT", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        m_nextPage = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"NEXT PAGE", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        m_prevPage = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"Previous PAGE", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        
        
        m_title = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"IOE ENTRANCE EXAM - 2071", WS_VISIBLE | WS_CHILD | SS_SIMPLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
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

        int xOffset = 250, yOffset = 90 + yoff;
        RECT wndRect = { 0 };
        GetClientRect(hWnd, &wndRect);
        int y = yOffset; int dummy;

        MoveWindow(m_title, 450, 20, 500, 80, true);

        for (int i = 0; i < startq; i++)
            m_q[i].Reposition(-wndRect.right*2, dummy, wndRect.right - 2 * xOffset, m_sizes);
        for (int i = endq; i < max_q; i++)
            m_q[i].Reposition(-wndRect.right*2, dummy, wndRect.right - 2 * xOffset, m_sizes);

        for (int i = startq; i < endq; i++)
            m_q[i].Reposition(xOffset, y, wndRect.right - 2 * xOffset, m_sizes);


        if (m_page < (max_q / qperpage))
        {
            if (m_page != 0){
                MoveWindow(m_nextPage, wndRect.right / 2 + 10, y, 130, 100, true);
                MoveWindow(m_prevPage, wndRect.right / 2 - 130 - 10, y, 130, 100, true);
            }
            else {
                MoveWindow(m_nextPage, wndRect.right / 2 - 250 / 2, y, 250, 100, true);
                MoveWindow(m_prevPage, -wndRect.right, 0, 0, 0, true);
            }
            MoveWindow(m_submit, -wndRect.right, 0, 0, 0, true);
        }
        else
        {
            MoveWindow(m_submit, wndRect.right / 2 + 10, y, 130, 100, true);
            MoveWindow(m_prevPage, wndRect.right / 2 - 130 - 10, y, 130, 100, true);
            MoveWindow(m_nextPage, -wndRect.right, 0, 0, 0, true);
        }

        m_ymax = y + 150 - yoff;
    }
    int GetYMax() { return m_ymax; }

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