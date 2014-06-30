#pragma once

#include "Question.h"

const int max_q = 65;

class Page
{
private:
    Question m_q[max_q];
    std::map<HWND, int> m_sizes;
	unsigned int m_nQ;
	int m_totalHeight;
public:
	Page()
	{
		m_nQ = 0;
		m_totalHeight = 0;
	}
	void Initialize(HWND hWnd)
	{
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
		m_nQ = size;
        file.close();
	}
	unsigned int GetNoOfQuestions()
	{
		return m_nQ;
	}
    void Resize(HWND hWnd, int height)
    {
        m_sizes[hWnd] = height;
    }
	int GetTotalHeight(){ return m_totalHeight; }
    void Resize(HWND hWnd)
    {
        int xOffset = 250, yOffset = 50;
        RECT wndRect = { 0 };
        GetClientRect(hWnd, &wndRect);
        int y = yOffset;
        for (int i = 0; i < max_q; i++)
            m_q[i].Reposition(xOffset, y, wndRect.right - 2 * xOffset, m_sizes);
		m_totalHeight = y;
    }
};