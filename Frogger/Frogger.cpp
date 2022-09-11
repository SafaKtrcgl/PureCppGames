#include <iostream>
#include <string>
#include "olcConsoleGameEngine.h"

using namespace std;

class Frogger : public olcConsoleGameEngine
{
public:
	Frogger()
	{
		m_sAppName = L"Frogger";
	}

private:
	vector<pair<float, wstring>> vecLanes =
	{
		{0.0f,  L"wwwhhwwwhhwwwhhwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"}, // 64 elements per lane
		{-3.0f, L",,,jllk,,jllllk,,,,,,,jllk,,,,,jk,,,jlllk,,,,jllllk,,,,jlllk,,,,"}, 
		{3.0f,  L",,,,jllk,,,,,jllk,,,,jllk,,,,,,,,,jllk,,,,,jk,,,,,,jllllk,,,,,,,"},
		{2.0f,  L",,jlk,,,,,jlk,,,,,jk,,,,,jlk,,,jlk,,,,jk,,,,jllk,,,,jk,,,,,,jk,,"},
		{0.0f,  L"pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"},
		{-3.0f, L"....asdf.......asdf....asdf..........asdf........asdf....asdf..."},
		{ 3.0f, L".....ty..ty....ty....ty.....ty........ty..ty.ty......ty.......ty"},
		{-4.0f, L"..zx.....zx.........zx..zx........zx...zx...zx....zx...zx...zx.."},		
		{2.0f,  L"..ty.....ty.......ty.....ty......ty..ty.ty.......ty....ty......."},
		{0.0f,  L"pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"}
	};

	float fTimeSinceStart = 0.0f;
	int nCellSize = 8;

	float fFrogX = 8.0f;
	float fFrogY = 9.0f;

	bool* bufferDanger = nullptr;

	olcSprite *spriteCar1 = nullptr;
	olcSprite *spriteCar2 = nullptr;
	olcSprite *spriteBus = nullptr;
	olcSprite *spriteLog = nullptr;
	olcSprite *spriteWater = nullptr;
	olcSprite *spriteFrog = nullptr;
	olcSprite *spritePavement = nullptr;
	olcSprite *spriteWall = nullptr;
	olcSprite *spriteHome = nullptr;

protected:
	virtual bool OnUserCreate()
	{
		spriteCar1 = new olcSprite(L"../FroggerSprites/car1.spr");
		spriteCar2 = new olcSprite(L"../FroggerSprites/car2.spr");
		spriteBus = new olcSprite(L"../FroggerSprites/bus.spr");
		spriteLog = new olcSprite(L"../FroggerSprites/log.spr");
		spriteWater = new olcSprite(L"../FroggerSprites/water.spr");
		spriteFrog = new olcSprite(L"../FroggerSprites/frog.spr");
		spritePavement = new olcSprite(L"../FroggerSprites/pavement.spr");
		spriteWall = new olcSprite(L"../FroggerSprites/wall.spr");
		spriteHome = new olcSprite(L"../FroggerSprites/home.spr");
		
		bufferDanger = new bool[ScreenWidth() * ScreenHeight()];
		memset(bufferDanger, 0, ScreenWidth() * ScreenHeight() * sizeof(bool));

		return true;
	}
	
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		fTimeSinceStart += fElapsedTime;

		if (m_keys[VK_UP].bReleased)
		{
			fFrogY -= 1;
		}
		if (m_keys[VK_DOWN].bReleased)
		{
			fFrogY += 1;
		}
		if (m_keys[VK_RIGHT].bReleased)
		{
			fFrogX += 1;
		}
		if (m_keys[VK_LEFT].bReleased)
		{
			fFrogX -= 1;
		}

		if (fFrogY <= 3)
		{
			fFrogX -= fElapsedTime * vecLanes[(int)fFrogY].first;
		}

		bool tl = bufferDanger[(int)(fFrogY * nCellSize + 1) * ScreenWidth() + (int)(fFrogX * nCellSize + 1)];
		bool tr = bufferDanger[(int)(fFrogY * nCellSize + 1) * ScreenWidth() + (int)((fFrogX + 1) * nCellSize - 1)];
		bool bl = bufferDanger[(int)((fFrogY + 1) * nCellSize - 1) * ScreenWidth() + (int)(fFrogX * nCellSize + 1)];
		bool br = bufferDanger[(int)((fFrogY + 1) * nCellSize - 1) * ScreenWidth() + (int)((fFrogX + 1) * nCellSize - 1)];

		if (tl || tr || bl || br)
		{
			// Frogs been hit :-(
			fFrogX = 8.0f;
			fFrogY = 9.0f;
		}

		//Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

		int x = -1;
		int y = 0;
		for (auto lane :vecLanes)
		{
			int nStartPos = (int)(fTimeSinceStart * lane.first) % 64;
			int nCellOffset = (int)((float)nCellSize * fTimeSinceStart * lane.first) % nCellSize;

			if (nStartPos < 0)
			{
				nStartPos = 64 - (abs(nStartPos) % 64);
			}

			for (int  i = 0; i < (ScreenWidth() / nCellSize) + 2; i++)
			{
				wchar_t graphic = lane.second[(nStartPos + i) % 64];
				//Fill((x + i) * nCellSize - nCellOfset, y * nCellSize, (x + i + 1) * nCellSize - nCellOfset, (y + 1) * nCellSize, graphic);
			
				switch (graphic) // Graphics always make code verbose and less clear
				{					
					case L'a':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 0, 0, 8, 8);		break; // Bus 1
					case L's':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 8, 0, 8, 8);		break; // Bus 2
					case L'd':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 16, 0, 8, 8);	break; // Bus 3
					case L'f':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 24, 0, 8, 8);	break; // Bus 4

					case L'j':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteLog, 0, 0, 8, 8);		break; // Log Start
					case L'l':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteLog, 8, 0, 8, 8);		break; // Log Middle
					case L'k':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteLog, 16, 0, 8, 8);	break; // Log End
											
					case L'z': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar1, 0, 0, 8, 8);	break; // Car1 Back
					case L'x':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar1, 8, 0, 8, 8);	break; // Car1 Front

					case L't': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar2, 0, 0, 8, 8);	break; // Car2 Back
					case L'y': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar2, 8, 0, 8, 8);	break; // Car2 Front

					case L'w': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteWall, 0, 0, 8, 8);	break; // Wall
					case L'h':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteHome, 0, 0, 8, 8);	break; // Home
					case L',': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteWater, 0, 0, 8, 8);	break; // Water
					case L'p': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spritePavement, 0, 0, 8, 8); break; // Pavement
					case L'.': 	Fill((x + i)*nCellSize - nCellOffset, y*nCellSize, (x + i + 1)*nCellSize - nCellOffset, (y + 1)*nCellSize, L' ');	break; // Road
				}
				for (int j = (x + i) * nCellSize - nCellOffset; j < (x + i + 1) * nCellSize - nCellOffset; j++)
				{
					for (int k = y * nCellSize; k < (y + 1) * nCellSize; k++)
					{
						if (j >= 0 && j < ScreenWidth() && k >= 0 && k < ScreenHeight())
						{
							bufferDanger[k * ScreenWidth() + j] =
							!(	graphic == L'.' ||
								graphic == L'j' || graphic == L'k' || graphic == L'l' ||
								graphic == L'p' ||
								graphic == L'h');
						}
					}
				}
			}
			y++;
		}
		DrawSprite(fFrogX * nCellSize, fFrogY * nCellSize, spriteFrog);

		return true;
	}
};

int main()
{
	Frogger game;
	game.ConstructConsole(128, 80, 12, 12);
	game.Start();

	return 0;
}
