#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <functional>
enum PIXEL_TYPE
{
	PIXEL_SOLID = 219
};

typedef struct
{
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
} Triangle;
class Renderer
{
public:
	static Renderer& Instance();

	void Init(short width, short height, short fontSizeX, short fontSizeY);
	void Render();
	void CleanChar(char c = PIXEL_SOLID, short color = 0x000FF);
	void SetWindowTitle(std::wstring title);
	void DrawAt(int x, int y, char c = PIXEL_SOLID, short color = 0x000FF);
	void Clip(int& x, int& y);
	void ShutDown();
	void DrawLine(int x1, int y1, int x2, int y2, short c = PIXEL_SOLID, short col = 0x000FF);
	void ForEachPixelOnLine(int x1, int y1, int x2, int y2, std::function<void(int x, int y)> f);

	void DrawTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, short c = PIXEL_SOLID, short col = 0x000FF);
	void DrawTriangle(Triangle t, short c = PIXEL_SOLID, short col = 0x000FF);
	void GetNormalizedToScreenSpace(float x, float y, int& nx, int& ny);

	short m_screenWidth;
	short m_screenHeight;
	CHAR_INFO* m_bufScreen;
private:
	static std::unique_ptr<Renderer> m_instance;
	
	HANDLE m_hConsole;
	HANDLE m_hConsoleIn;

	SMALL_RECT m_rectWindow;
	
};