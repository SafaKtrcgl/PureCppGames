#include <iostream>
#include <Windows.h>
#include <list>
#include <thread>
using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;

struct sSnakeSegment
{
	int x;
	int y;
};

int main()
{
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
	{
		screen[i] = L' ';
	}
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;



	while (true)
	{
		list<sSnakeSegment> snake = { {60, 15}, {61, 15}, {62, 15}, {63, 15}, {64, 15}, {65, 15} };
		int nFoodX = 30;
		int nFoodY = 15;
		int nScore = 0;
		int nSnakeDirection = 3;
		bool bDead = false;
		bool bKeyLeft = false;
		bool bKeyLeftOld = false;
		bool bKeyRight = false;
		bool bKeyRightOld = false;

		while (!bDead)
		{
			//Timing & input
			//this_thread::sleep_for(200ms);

			//Get Input
			auto t1 = chrono::system_clock::now();
			while ((chrono::system_clock::now() - t1) < ((nSnakeDirection % 2 == 1) ? 120ms : 200ms))
			{
				bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
				bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;

				if (bKeyRight && !bKeyRightOld)
				{
					nSnakeDirection++;
					if (nSnakeDirection == 4)
					{
						nSnakeDirection = 0;
					}
				}
				if (bKeyLeft && !bKeyLeftOld)
				{
					nSnakeDirection--;
					if (nSnakeDirection == -1)
					{
						nSnakeDirection = 3;
					}
				}

				bKeyRightOld = bKeyRight;
				bKeyLeftOld = bKeyLeft;
			}
			
			//Game Logic
			switch (nSnakeDirection)
			{
			case 0:	//UP
				snake.push_front({ snake.front().x, snake.front().y - 1 });
				break;
			case 1:	//RIGHT
				snake.push_front({ snake.front().x + 1, snake.front().y });
				break;
			case 2:	//DOWN
				snake.push_front({ snake.front().x, snake.front().y + 1 });
				break;
			case 3:	//LEFT
				snake.push_front({ snake.front().x - 1, snake.front().y });
				break;
			}

			snake.pop_back();

			//Collision Snake V World
			if (snake.front().x < 0 || snake.front().x >= nScreenWidth)
			{
				bDead = true;
			}
			if (snake.front().y < 3 || snake.front().y >= nScreenHeight)
			{
				bDead = true;
			}

			//Collision Snake V Food
			if (snake.front().x == nFoodX && snake.front().y == nFoodY)
			{
				nScore++;
				nFoodX = rand() % nScreenWidth;
				nFoodY = (rand() % (nScreenHeight - 3)) + 3;
				while (screen[nFoodY * nScreenWidth + nFoodX] != L' ')
				{
					nFoodX = rand() % nScreenWidth;
					nFoodY = (rand() % (nScreenHeight - 3)) + 3;
				}

				for (int i = 0; i < 3; i++)
				{
					snake.push_back({snake.back().x, snake.back().y });
				}
			}

			//Collision Snake V Snake
			for (list<sSnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
			{
				if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
				{
					bDead = true;
				}
			}

			//Clear Screen
			for (int i = 0; i < nScreenHeight * nScreenWidth; i++)
			{
				screen[i] = L' ';
			}

			//Draw Stats & Border
			for (int i = 0; i < nScreenWidth; i++)
			{
				screen[i] = L'=';
				screen[2 * nScreenWidth + i] = L'=';
			}
			wsprintf(&screen[nScreenWidth + 5], L"SCORE: %d", nScore);

			//Draw Snake
			for (auto s : snake)
			{
				screen[s.y * nScreenWidth + s.x] = bDead ? L'x' : L'o';
			}

			//Draw SnakeHead
			screen[snake.front().y * nScreenWidth + snake.front().x] = bDead ? L'#' : L'O';

			//Draw Food
			screen[nFoodY * nScreenWidth + nFoodX] = L'@';

			if (bDead)
			{
				wsprintf(&screen[20 * nScreenWidth + 50], L"PRESS 'SPACE' TO PLAY AGAIN");
			}

			//Display
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		}
		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
	}

	return 0;
}
