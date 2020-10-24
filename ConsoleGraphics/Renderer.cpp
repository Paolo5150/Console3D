#include "Renderer.h"
#include <windows.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>
std::unique_ptr<Renderer> Renderer::m_instance;

Renderer& Renderer::Instance()
{
	if (m_instance == nullptr)
	{
		m_instance = std::make_unique<Renderer>();
	}

	return *m_instance;
}

void Renderer::ShutDown()
{
	//SetConsoleActiveScreenBuffer(m_originalHandle);

	delete[] m_bufScreen;
}

void Renderer::SetWindowTitle(std::wstring title)
{
	SetConsoleTitleW(&title.at(0));
}


void Renderer::Init(short width, short height, short fontSizeX, short fontSizeY)
{
	m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
	if (m_hConsole == INVALID_HANDLE_VALUE)
		throw new std::exception("Bad Handle");

	m_screenWidth = width;
	m_screenHeight = height;

	// Update 13/09/2017 - It seems that the console behaves differently on some systems
	// and I'm unsure why this is. It could be to do with windows default settings, or
	// screen resolutions, or system languages. Unfortunately, MSDN does not offer much
	// by way of useful information, and so the resulting sequence is the reult of experiment
	// that seems to work in multiple cases.
	//
	// The problem seems to be that the SetConsoleXXX functions are somewhat circular and 
	// fail depending on the state of the current console properties, i.e. you can't set
	// the buffer size until you set the screen size, but you can't change the screen size
	// until the buffer size is correct. This coupled with a precise ordering of calls
	// makes this procedure seem a little mystical :-P. Thanks to wowLinh for helping - Jx9

	// Change console visual size to a minimum so ScreenBuffer can shrink
	// below the actual visual size
	m_rectWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow);

	// Set the size of the screen buffer
	COORD coord = { (short)m_screenWidth, (short)m_screenHeight };
	if (!SetConsoleScreenBufferSize(m_hConsole, coord))
		throw new std::exception("SetConsoleScreenBufferSize");

	// Assign screen buffer to the console
	if (!SetConsoleActiveScreenBuffer(m_hConsole))
		throw new std::exception("SetConsoleActiveScreenBuffer");

	// Set the font size now that the screen buffer has been assigned to the console
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontSizeX;
	cfi.dwFontSize.Y = fontSizeY;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	/*	DWORD version = GetVersion();
		DWORD major = (DWORD)(LOBYTE(LOWORD(version)));
		DWORD minor = (DWORD)(HIBYTE(LOWORD(version)));*/

		//if ((major > 6) || ((major == 6) && (minor >= 2) && (minor < 4)))		
		//	wcscpy_s(cfi.FaceName, L"Raster"); // Windows 8 :(
		//else
		//	wcscpy_s(cfi.FaceName, L"Lucida Console"); // Everything else :P

		//wcscpy_s(cfi.FaceName, L"Liberation Mono");
	//wcscpy_s(cfi.FaceName, "Consolas");
	//SetCurrentConsoleFontEx(m_hConsole, false, &cfi);
		

	// Get screen buffer info and check the maximum allowed window size. Return
	// error if exceeded, so user knows their dimensions/fontsize are too large
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(m_hConsole, &csbi);

	if (m_screenHeight > csbi.dwMaximumWindowSize.Y)
		throw new std::exception("Screen Height / Font Height Too Big");
	if (m_screenWidth > csbi.dwMaximumWindowSize.X)
		throw new std::exception("Screen Width / Font Width Too Big " );

	// Set Physical Console Window Size
	m_rectWindow = { 0, 0, (short)m_screenWidth - 1, (short)m_screenHeight - 1 };
	if (!SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow))
		throw new std::exception("SetConsoleWindowInfo");

	// Set flags to allow mouse input		
	if (!SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		throw new std::exception("SetConsoleMode");

	// Allocate memory for screen buffer
	m_bufScreen = new CHAR_INFO[m_screenWidth*m_screenHeight];
	memset(m_bufScreen, 0, sizeof(CHAR_INFO) * m_screenWidth * m_screenHeight);

	//SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);


}

void Renderer::DrawTriangle(Triangle t, short c , short col)
{
	DrawTriangle(t.v1, t.v2, t.v3,c,col);
}


void Renderer::DrawTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, short c, short col)
{
	DrawLine(v1.x, v1.y,v2.x,v2.y,c,col);
	DrawLine(v2.x, v2.y, v3.x, v3.y, c, col);
	DrawLine(v3.x, v3.y, v1.x, v1.y, c, col);

}

void Renderer::ForEachPixelOnLine(int x1, int y1, int x2, int y2, std::function<void(int,int)> f)
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1; dy = y2 - y1;
	dx1 = abs(dx); dy1 = abs(dy);
	px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1; y = y1; xe = x2;
		}
		else
		{
			x = x2; y = y2; xe = x1;
		}

		f(x, y);

		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
				px = px + 2 * dy1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}
			f(x, y);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1; y = y1; ye = y2;
		}
		else
		{
			x = x2; y = y2; ye = y1;
		}

		f(x, y);

		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			f(x, y);
		}
	}
}


void Renderer::DrawLine(int x1, int y1, int x2, int y2, short c, short col)
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1; dy = y2 - y1;
	dx1 = abs(dx); dy1 = abs(dy);
	px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1; y = y1; xe = x2;
		}
		else
		{
			x = x2; y = y2; xe = x1;
		}

		DrawAt(x, y, c, col);

		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
				px = px + 2 * dy1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}
			DrawAt(x, y, c, col);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1; y = y1; ye = y2;
		}
		else
		{
			x = x2; y = y2; ye = y1;
		}

		DrawAt(x, y, c, col);

		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			DrawAt(x, y, c, col);
		}
	}
}


void Renderer::DrawAt(int x, int y ,char c, short color)
{
	Clip(x, y);
	if (x > 0 && x < m_screenWidth - 1 && y > 0 && y < m_screenHeight - 1)
	{
		m_bufScreen[y * m_screenWidth + x].Char.AsciiChar = c;
		m_bufScreen[y * m_screenWidth + x].Attributes = color;

	}
}

void Renderer::Clip(int &x, int &y)
{
	if (x < 0) x = 0;
	if (x >= m_screenWidth) x = m_screenWidth;
	if (y < 0) y = 0;
	if (y >= m_screenHeight) y = m_screenHeight;
}

void Renderer::GetNormalizedToScreenSpace(float x, float y, int& nx, int& ny)
{
	nx = ((x / 2.0 + 0.5f) * (m_screenWidth-2))+1;
	ny = ((-y / 2.0 + 0.5f) * (m_screenHeight-2)) + 1;

}


void Renderer::Render()
{

	WriteConsoleOutput(m_hConsole, m_bufScreen, { m_screenWidth, m_screenHeight }, { 0,0 }, &m_rectWindow);

}

void Renderer::CleanChar(char c, short color)
{
	for (int i = 0; i < m_screenWidth * m_screenHeight; i++)
	{
		m_bufScreen[i].Char.AsciiChar = c;
		m_bufScreen[i].Attributes = color;

	}
}


