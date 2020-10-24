#include <Windows.h>
#include <iostream>       // std::cout, std::endl
#include <thread>         // std::this_thread::sleep_for
#include <chrono>
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "Input.h"
#include <sstream>


float sign(int p1x, int p1y, int p2x, int p2y,  int p3x, int p3y)
{
	return (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
}

bool PointInTriangle(int p1x, int p1y, int p2x, int p2y, int p3x, int p3y, int px, int py)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(px,py, p1x, p1y, p2x,p2y);
	d2 = sign(px, py, p2x, p2y, p3x, p3y);
	d3 = sign(px, py, p3x, p3y, p1x, p1y);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}


void main()
{
	Renderer::Instance().Init(150,50,5,5);

	//Unsure whether this "include" hack is clever or nasty....
	#include "FPS.h"
	//#include "Light2D.h"

	system("pause");
	Renderer::Instance().ShutDown();
}