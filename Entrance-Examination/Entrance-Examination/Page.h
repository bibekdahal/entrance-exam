#pragma once

#include "Question.h"

const int max_q = 65;
const int qperpage = 20;

class Page
{
private:
    Question m_q[max_q];
    std::map<HWND, int> m_sizes;
    HWND m_submit, m_nextPage;
    int m_ymax, m_page;
public:
    HWND GetSubmitHandle() { return m_submit; }
    HWND GetNextPageHandle() { return m_nextPage; }

	Page()
	{

	}
	void Initialize(HWND hWnd)
	{
        m_page = 0;

		int xOffset = 250, yOffset = 50;
		RECT wndRect = { 0 };
		GetClientRect(hWnd, &wndRect);
        int y = yOffset;
		for (int i = 0; i < max_q; i++)
			m_q[i].Initialize(hWnd, i+1);

        std::fstream file;
        file.open("test.fbq", std::ios::in | std::ios::binary);
        unsigned int size;
        file.read((char*)&size, sizeof(size));
        for (unsigned int i = 0; i < size; ++i)
            m_q[i].LoadFromFile(file);
        file.close();

        m_submit = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"SUBMIT", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
        m_nextPage = CreateWindowEx(WS_EX_WINDOWEDGE, L"BUTTON", L"NEXT PAGE", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, NULL, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

	}

    void Resize(HWND hWnd, int height)
    {
        m_sizes[hWnd] = height;
    }

    void ResizeControls(HWND hWnd, int yoff)
    {
        int startq = m_page * 20;
        int endq = startq + 20;
        if (endq > max_q) endq = max_q ;

        int xOffset = 250, yOffset = 50 + yoff;
        RECT wndRect = { 0 };
        GetClientRect(hWnd, &wndRect);
        int y = yOffset; int dummy;
        for (int i = 0; i < startq; i++)
            m_q[i].Reposition(-wndRect.right, dummy, 0, m_sizes);
        for (int i = endq + 1; i < max_q; i++)
            m_q[i].Reposition(-wndRect.right, dummy, 0, m_sizes);

        for (int i = startq; i != endq; i++)
            m_q[i].Reposition(xOffset, y, wndRect.right - 2 * xOffset, m_sizes);


        if (m_page < (max_q / qperpage))
        {
            MoveWindow(m_nextPage, wndRect.right / 2 - 250 / 2, y, 250, 100, true);
            MoveWindow(m_submit, -wndRect.right, 0, 0, 0, true);
        }
        else
        {
            MoveWindow(m_submit, wndRect.right / 2 - 250 / 2, y, 250, 100, true);
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
};