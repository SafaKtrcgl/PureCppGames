#define _UNICODE
#define UNICODE

#include <iostream>
#include <string>
#include <algorithm>
#include "olcConsoleGameEngine.h"

using namespace std;


class UltimateFPS : public olcConsoleGameEngine
{
public:
	UltimateFPS()
	{
		m_sAppName = L"Ultimate First Person Shooter";
	}

private:
	int nMapWidth = 32;
	int nMapHeight = 32;

	float fPlayerX = 8.0f;
	float fPlayerY = 8.0f;
	float fPlayerR = 0.0f;
	float fSpeed = 5.0f;
	float fFOV = 3.14159 / 4.0;
	float fDepth = 16.0f;

	wstring map;

	olcSprite* sprite = nullptr;
	wstring sCurrentSpriteFile;

protected:
	virtual bool OnUserCreate()
	{
		map += L"################################";
		map += L"#...............#..............#";
		map += L"#...............#..............#";
		map += L"#...............#..............#";
		map += L"#...............#..............#";
		map += L"#...............#..............#";
		map += L"########........########.......#";
		map += L"########........########.......#";
		map += L"#...............#..............#";
		map += L"#...............#..............#";
		map += L"#...............#..............#";
		map += L"#...............#..............#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............#...............#";
		map += L"#..............#...............#";
		map += L"#..............#...............#";
		map += L"#..............#...............#";
		map += L"#.......########........########";
		map += L"#.......########........########";
		map += L"#..............#...............#";
		map += L"#..............#...............#";
		map += L"#..............#...............#";
		map += L"#..............#...............#";
		map += L"#..............#...............#";
		map += L"################################";

		sprite = new olcSprite(32, 32);
		sCurrentSpriteFile = L"fps_wall.spr";

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		//Handle Rotation
		if (m_keys[L'Q'].bHeld)
			fPlayerR -= (fSpeed * 0.5f) * fElapsedTime;

		if (m_keys[L'E'].bHeld)
			fPlayerR += (fSpeed * 0.5f) * fElapsedTime;

		// Handle forwards movement & collision
		if (m_keys[L'W'].bHeld)
		{
			fPlayerX += sinf(fPlayerR) * fSpeed * fElapsedTime;;
			fPlayerY += cosf(fPlayerR) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX -= sinf(fPlayerR) * fSpeed * fElapsedTime;;
				fPlayerY -= cosf(fPlayerR) * fSpeed * fElapsedTime;;
			}
		}

		// Handle backwards movement & collision
		if (m_keys[L'S'].bHeld)
		{
			fPlayerX -= sinf(fPlayerR) * fSpeed * fElapsedTime;;
			fPlayerY -= cosf(fPlayerR) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += sinf(fPlayerR) * fSpeed * fElapsedTime;;
				fPlayerY += cosf(fPlayerR) * fSpeed * fElapsedTime;;
			}
		}
		
		// Handle Left movement & collision
		if (m_keys[L'A'].bHeld)
		{
			fPlayerX -= cosf(fPlayerR) * fSpeed * fElapsedTime;;
			fPlayerY += sinf(fPlayerR) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += cosf(fPlayerR) * fSpeed * fElapsedTime;;
				fPlayerY -= sinf(fPlayerR) * fSpeed * fElapsedTime;;
			}
		}
		
		// Handle Right movement & collision
		if (m_keys[L'D'].bHeld)
		{
			fPlayerX += cosf(fPlayerR) * fSpeed * fElapsedTime;;
			fPlayerY -= sinf(fPlayerR) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX -= cosf(fPlayerR) * fSpeed * fElapsedTime;;
				fPlayerY += sinf(fPlayerR) * fSpeed * fElapsedTime;;
			}
		}

		for (int x = 0; x < ScreenWidth(); x++)
		{
			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerR - fFOV / 2.0f) + ((float)x / (float)ScreenWidth()) * fFOV;

			// Find distance to wall
			float fStepSize = 0.1f;		  // Increment size for ray casting, decrease to increase										
			float fDistanceToWall = 0.0f; //                                      resolution

			bool bHitWall = false;		// Set when ray hits wall block
			bool bBoundary = false;		// Set when ray hits boundary between two wall blocks

			float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);

			// Incrementally cast ray from player, along ray angle, testing for 
			// intersection with a block
			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;			// Just set distance to maximum depth
					fDistanceToWall = fDepth;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
					if (map.c_str()[nTestX * nMapWidth + nTestY] == '#')
					{
						// Ray has hit wall
						bHitWall = true;

						// To highlight tile boundaries, cast a ray from each corner
						// of the tile, to the player. The more coincident this ray
						// is to the rendering ray, the closer we are to a tile 
						// boundary, which we'll shade to add detail to the walls
						vector<pair<float, float>> p;

						// Test each corner of hit tile, storing the distance from
						// the player, and the calculated dot product of the two rays
						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								// Angle of corner to eye
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(make_pair(d, dot));
							}

						// Sort Pairs from closest to farthest
						sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first; });

						// First two/three are closest (we will never see all four)
						float fBound = 0.01;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}

			// Calculate distance to ceiling and floor
			int nCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToWall);
			int nFloor = ScreenHeight() - nCeiling;

			// Shader walls based on distance
			short nShade = ' ';
			if (fDistanceToWall <= fDepth / 4.0f)
			{
				nShade = 0x2588;
			}
			else if (fDistanceToWall < fDepth / 3.0f)
			{
				nShade = 0x2593;
			}
			else if (fDistanceToWall < fDepth / 2.0f)
			{
				nShade = 0x2592;
			}
			else if (fDistanceToWall < fDepth)				nShade = 0x2591;
			else											nShade = ' ';		// Too far away

			if (bBoundary)
			{
				nShade = ' ';
			}

			for (int y = 0; y < ScreenHeight(); y++)
			{
				if (y <= nCeiling)
					Draw(x, y, ' ');
				else if (y > nCeiling && y <= nFloor)
					Draw(x, y, nShade);
				else
				{
					float b = 1.0f - (((float)y - ScreenHeight() / 2.0f) / ((float)ScreenHeight() / 2.0f));
					if (b < 0.25)		nShade = '#';
					else if (b < 0.5)	nShade = 'x';
					else if (b < 0.75)	nShade = '-';
					else if (b < 0.9)	nShade = '.';
					else				nShade = ' ';
					Draw(x, y, nShade);
				}
			}
		}

		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
			{
				Draw(nx + 1, ny + 1, map[ny * nMapWidth + nx]);
			}

		Draw(1 + (int)fPlayerY, 1 + (int)fPlayerX, L'P');


		return true;
	}

};


int main()
{
	UltimateFPS game;
	game.ConstructConsole(320, 240, 4, 4);
	game.Start();

	return 0;
}
