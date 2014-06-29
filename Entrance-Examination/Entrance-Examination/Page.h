#pragma once

#include "Question.h"

const int max_q = 65;

class Page
{
private:
    Question m_q[max_q];
    std::map<HWND, int> m_sizes;
public:
	Page()
	{

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
        file.close();
	}

    void Resize(HWND hWnd, int height)
    {
        m_sizes[hWnd] = height;
    }

    void Resize(HWND hWnd)
    {
        int xOffset = 250, yOffset = 50;
        RECT wndRect = { 0 };
        GetClientRect(hWnd, &wndRect);
        int y = yOffset;
        for (int i = 0; i < max_q; i++)
            m_q[i].Reposition(xOffset, y, wndRect.right - 2 * xOffset, m_sizes);
    }
};