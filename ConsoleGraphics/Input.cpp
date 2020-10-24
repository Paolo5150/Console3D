#include "Input.h"
#include <Windows.h>

std::unique_ptr<Input> Input::m_instance;
Input::Input()
{

}

Input& Input::Instance()
{
	if (m_instance == nullptr)
	{
		m_instance = std::make_unique<Input>();
	}

	return *m_instance;
}

bool Input::IsKeyDown(char c)
{
	if (GetAsyncKeyState(c) & 0x8000)
		return true;
	else
		return false;
}

