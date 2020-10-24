int mapWidth = 16;
int mapHeight = 16;
std::string map;

typedef struct
{
	float x;
	float y;
	float FOV = 60.0f;
	float a = 0.0;
	float depth = 16.0f;
	float speed = 0.08f;
	float rotSpeed = 1.0f;

} Player;
map += "################";
map += "#P.............#";
map += "###..##...###..#";
map += "#.....#...#.#..#";
map += "#.....#...#.#..#";
map += "#.....#...#.#..#";
map += "#.....#...#.#..#";
map += "#.....#...#.#..#";
map += "#.....#...#.#..#";
map += "#..............#";
map += "#..............#";
map += "#..............#";
map += "#..............#";
map += "#..............#";
map += "#..............#";
map += "#..............#";
map += "################";



Renderer::Instance().Init(160, 50, 8, 8);
Renderer::Instance().SetWindowTitle(L"Magic");
Player player;

for (int x = 0; x < mapWidth; x++)
{
	for (int y = 0; y < mapHeight; y++)
	{
		if (map[y * mapWidth + x] == 'P')
		{
			player.x = x;
			player.y = y;
		}
	}
}

while (1)
{
	Renderer::Instance().CleanChar(PIXEL_SOLID, 0x0000);

	float playerDirX = glm::cos(glm::radians(player.a));
	float playerDirY = glm::sin(glm::radians(player.a));


	if (Input::Instance().IsKeyDown('A'))
		player.a -= player.rotSpeed;
	if (Input::Instance().IsKeyDown('D'))
		player.a += player.rotSpeed;
	if (Input::Instance().IsKeyDown('W'))
	{
		player.x += playerDirX * player.speed;
		player.y += playerDirY * player.speed;

	}

	if (Input::Instance().IsKeyDown('S'))
	{
		player.x -= playerDirX * player.speed;
		player.y -= playerDirY * player.speed;
	}

	std::wstringstream ss;
	ss << "X ";
	ss << player.x;
	ss << " Y ";
	ss << player.y;
	ss << " DX ";
	ss << playerDirX;
	ss << " DY ";
	ss << playerDirY;


	for (int x = 0; x < Renderer::Instance().m_screenWidth; x++)
	{
		float screenPerc = (float)x / (float)Renderer::Instance().m_screenWidth;
		float rayAngle = player.a - (player.FOV / 2.0f) + player.FOV * screenPerc;

		float length = 0.0;

		float rayDirX = glm::cos(glm::radians(rayAngle));
		float rayDirY = glm::sin(glm::radians(rayAngle));

		float rayOrigX = player.x;
		float rayOrigY = player.y;

		bool hitWall = false;
		float rayNextX = rayOrigX + rayDirX * length;
		float rayNextY = rayOrigY + rayDirY * length;
		while (length < player.depth && !hitWall)
		{

			rayNextX = rayOrigX + rayDirX * length;
			rayNextY = rayOrigY + rayDirY * length;

			if (map[(int)rayNextY * mapWidth + (int)rayNextX] == '#')
			{
				hitWall = true;
			}
			else
				length += 0.1f;
		}


		float midScreen = Renderer::Instance().m_screenHeight / 2.0f;


		int ceilingHeight = (float)midScreen - (float)midScreen / length ;
		int floorHeight = Renderer::Instance().m_screenHeight - ceilingHeight;

		for (int y = 0; y < Renderer::Instance().m_screenHeight; y++)
		{
			if (y <= ceilingHeight)
				Renderer::Instance().DrawAt(x, y, PIXEL_SOLID, 0x0000);

			else if (y >= floorHeight)
			{
				Renderer::Instance().DrawAt(x, y, 178, 0x0002);

			}
			else
			{

				if (length / player.depth > 0.95)
					Renderer::Instance().DrawAt(x, y, PIXEL_SOLID, 0x0000);
				else if (length / player.depth > 0.6)
					Renderer::Instance().DrawAt(x, y, 176, 0x0008);
				else if (length / player.depth > 0.4)
					Renderer::Instance().DrawAt(x, y, 177, 0x000f);
				else if (length / player.depth > 0.2)
					Renderer::Instance().DrawAt(x, y, 178, 0x008f);
				else
					Renderer::Instance().DrawAt(x, y, 176, 0x00ff);

			}
		}
	}

	Renderer::Instance().Render();
	std::this_thread::sleep_for(std::chrono::milliseconds(12));
}