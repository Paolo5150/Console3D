#include <Windows.h>
#include <iostream>       // std::cout, std::endl
#include <thread>         // std::this_thread::sleep_for
#include <chrono>
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>




void main()
{

	Renderer::Instance().Init(160, 50, 8, 8);
	Renderer::Instance().SetWindowTitle(L"Magic");
	Triangle t = { {-1.0,0.9,0 }, { 1.0,0.9,0 }, { 0.0,-0.9,0 } };
	glm::vec3 position(0, 0, 0);
	glm::vec3 camPosition(0, 0, 150);

	float camSpeed = 0.5f;
	float camRotSpeed = 0.05f;
	float camYaw = -90.0f;

	while (1)
	{
		float camFwdX = cosf(glm::radians(camYaw));
		float camFwdZ = sinf(glm::radians(camYaw));

		glm::vec3 fwdPoint(camPosition + glm::vec3(camFwdX, 0.0, camFwdZ));
		glm::vec3 fwd = glm::normalize((fwdPoint - camPosition));

		if (GetAsyncKeyState('W') & 0x8000)
		{
			camPosition += fwd * camSpeed;
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			camPosition -= fwd * camSpeed;
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			//camPosition += glm::vec3(-1, 0, 0) * camSpeed;

			camYaw -= camRotSpeed;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			//camPosition += glm::vec3(1,0,0) * camSpeed;

			camYaw += camRotSpeed;
		}


		glm::mat4 transform;
		transform = glm::translate(transform, position);

		glm::mat4 view = glm::lookAt(camPosition, camPosition + fwd, glm::vec3(0, 1, 0));
		glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)150 / 50, 0.1f, 10000.0f);

		// To Screen
		glm::vec4 v1s = (proj * view *  transform * glm::vec4(t.v1, 1.0));
		glm::vec4 v2s = (proj * view *  transform * glm::vec4(t.v2, 1.0));
		glm::vec4 v3s = (proj * view *  transform * glm::vec4(t.v3, 1.0));

		// Perspective division!
		glm::vec2 v1 = (glm::vec2) (v1s / v1s.w);
		glm::vec2 v2 = (glm::vec2) (v2s / v3s.w);
		glm::vec2 v3 = (glm::vec2) (v3s / v3s.w);

		int x[3], y[3];
		Renderer::Instance().GetNormalizedToScreenSpace(v1.x, v1.y, x[0], y[0]);
		Renderer::Instance().GetNormalizedToScreenSpace(v2.x, v2.y, x[1], y[1]);
		Renderer::Instance().GetNormalizedToScreenSpace(v3.x, v3.y, x[2], y[2]);


		Renderer::Instance().CleanChar(PIXEL_SOLID, 0x0000);
		Renderer::Instance().DrawLine(x[0], y[0], x[1], y[1], 'D', 0x000f);
		Renderer::Instance().DrawLine(x[1], y[1], x[2], y[2], 'D', 0x000f);
		Renderer::Instance().DrawLine(x[2], y[2], x[0], y[0], 'D', 0x000f);


		Renderer::Instance().Render();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	system("pause");
	Renderer::Instance().ShutDown();
}