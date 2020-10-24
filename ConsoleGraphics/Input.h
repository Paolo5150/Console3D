#pragma once
#include <iostream>

class Input
{

public:
	static Input& Instance();
	bool IsKeyDown(char c);
	Input();

private:
	static std::unique_ptr<Input> m_instance;

};