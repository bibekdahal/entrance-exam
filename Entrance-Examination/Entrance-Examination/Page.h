#pragma once

#include "Question.h"

const int max_q = 10;

class Page
{
private:
	Question m_q[max_q];
public:
	Page()
	{

	}
	void Initialize(HWND hWnd)
	{
		int xOffset = 250, yOffset = 50;
		RECT wndRect = { 0 };
		GetClientRect(hWnd, &wndRect);
		for (int i = 0; i < max_q; i++)
		{
			m_q[i].Initialize(hWnd);
			m_q[i].Reposition(xOffset, yOffset + i*300, wndRect.right - 2*xOffset);
		}
	}
	void Scroll(int dy)
	{
		for (int i = 0; i < max_q; i++)
		{
			m_q[i].Scroll(dy);
		}
	}
};