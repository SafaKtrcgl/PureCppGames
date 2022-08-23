#include <iostream>
#include "olcConsoleGameEngine.h";

using namespace std;

class Demo : public olcConsoleGameEngine
{
public:
	Demo()
	{

	}

protected:
	virtual bool OnUserCreate()
	{

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		for (int x = 0; x < m_nScreenWidth; x++)
		{
			for (int y = 0; y < m_nScreenHeight; y++)
			{
				Draw(x, y, L'#', rand() % 16);
			}
		}
		return true;
	}
};

int main()
{
	Demo game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();

	return 0;
}