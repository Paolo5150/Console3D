int mapWidth = 32;
int mapHeight = 16;
std::string map;

typedef struct
{
	float x;
	float y;
	float FOV = 60.0f;
	float a = 0.0;
	float depth = 16.0f;
	float speed = 0.008f;
	float rotSpeed = 1.0f;

} Player;

Player player;
player.x = 1;
player.y = 1;
/*      ################################                                      */
map += "################################";
map += "#                              #";
map += "#########                      #";
map += "#       #    #####  ########   #";
map += "#       #    #       #     #   #";
map += "#   #   #    #       #     #   #";
map += "#   #   #    #       #     #   #";
map += "#   #   #  ###       #     #   #";
map += "#   #   #            #     #   #";
map += "#   #   #    #       #     #   #";
map += "#   #####    #       #     #   #";
map += "#       ###  #########         #";
map += "#       #            #         #";
map += "#                              #";
map += "#                              #";
map += "################################";
#


while (1)
{

	Renderer::Instance().CleanChar(PIXEL_SOLID, 0);
	float rayAngleLeft = player.a + (player.FOV / 2.0f);
	float rayAngleRight = player.a - (player.FOV / 2.0f);

	float leftDirX = glm::cos(glm::radians(rayAngleLeft));
	float leftDirY = glm::sin(glm::radians(rayAngleLeft));

	float rightDirX = glm::cos(glm::radians(rayAngleRight));
	float rightDirY = glm::sin(glm::radians(rayAngleRight));

	float fwdX = glm::cos(glm::radians(player.a));
	float fwdY = glm::sin(glm::radians(player.a));

	// Find length
	float length = 8;



	int leftPointX = player.x + leftDirX * length;
	int leftPointY = player.y + leftDirY * length;

	int rightPointX = player.x + rightDirX * length;
	int rightPointY = player.y + rightDirY * length;

	auto toScreenCoords = [](float v, float map, float screen) {
		return v / map * screen;
	};
	int px = toScreenCoords(player.x, mapWidth, Renderer::Instance().m_screenWidth);
	int py = toScreenCoords(player.y, mapHeight, Renderer::Instance().m_screenHeight);
	int rx = toScreenCoords(leftPointX, mapWidth, Renderer::Instance().m_screenWidth);
	int ry = toScreenCoords(leftPointY, mapHeight, Renderer::Instance().m_screenHeight);
	int rrx = toScreenCoords(rightPointX, mapWidth, Renderer::Instance().m_screenWidth);
	int rry = toScreenCoords(rightPointY, mapHeight, Renderer::Instance().m_screenHeight);


	if (Input::Instance().IsKeyDown('A'))
		player.a -= player.rotSpeed;
	else if (Input::Instance().IsKeyDown('D'))
		player.a += player.rotSpeed;
	else if (Input::Instance().IsKeyDown('W'))
	{
		float nextX = player.x + fwdX * player.speed * 50.0;
		float nextY = player.y + fwdY * player.speed * 50.0;

		/*std::wstringstream ss;
		ss << "FX ";
		ss << nextX;
		ss << " FY ";
		ss << nextY;
		ss << " C ";
		ss << map[(int)nextY * mapWidth + (int)nextX];
		Renderer::Instance().SetWindowTitle(ss.str());*/

		if (map[(int)nextY * mapWidth + (int)nextX] != '#')
		{
			player.x += fwdX * player.speed;
			player.y += fwdY * player.speed;
		}

	}

	else if (Input::Instance().IsKeyDown('S'))
	{
		player.x -= fwdX * player.speed;
		player.y -= fwdY * player.speed;
	}

	for (int x = 0; x < Renderer::Instance().m_screenWidth; x++)
	{
		for (int y = 0; y < Renderer::Instance().m_screenHeight; y++)
		{
			if (x == px && y == py)
			{
				Renderer::Instance().DrawAt(x, y, 219, 0x0002);
				continue;
			}

			int mapX = (float)x / Renderer::Instance().m_screenWidth * mapWidth;
			int mapY = (float)y / Renderer::Instance().m_screenHeight * mapHeight;

			//if (PointInTriangle(px, py, rx, ry, rrx, rry, x, y))
			if (PointInTriangle(px, py, rx, ry, rrx, rry, x, y))
			{
				//Renderer::Instance().DrawAt(x, y, 219, 0x0004);
				Renderer::Instance().DrawAt(x, y, map[mapY * mapWidth + mapX], 0x000f);
			}
			else
				Renderer::Instance().DrawAt(x, y, 219, 0x0000);
		}
	}


	//Renderer::Instance().DrawLine(px, py, rx, ry, 219, 0x00ff);
	//Renderer::Instance().DrawLine(px, py, rrx, rry, 219, 0x00ff);
	Renderer::Instance().Render();
}
