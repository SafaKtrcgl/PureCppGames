#define _UNICODE
#define UNICODE

#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
#include <stdio.h>

using namespace std;

wstring tetromino[7];

int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr;

int nScreenWidth = 120;
int nScreenHeight = 40;

int Rotate(int px, int py, int r)
{
	switch (r % 4)
	{
	case 0:
		return py * 4 + px;
	case 1:
		return 12 + py - (4 * px);
	case 2:
		return 15 - (4 * py) - px;
	case 3:
		return 3 - py + (4 * px);
	}
	return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++)
	{
		for (int py = 0; py < 4; py++)
		{
			int pi = Rotate(px, py, nRotation);

			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

int main()
{
	//Asset Creation
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

	//Determining the borders
	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++)
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
		}
	}

	//Console Prompt Size
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
	{
		screen[i] = L' ';
	}
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	//Game Data
	bool bGameOver = false;

	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	bool bKey[4];
	bool bRotateHold = false;

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;

	vector<int> vLines;

	//Game Loop
	while (!bGameOver)
	{
		// GAME TIMING -------------------------------------------------------
		this_thread::sleep_for(50ms);
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

		// INPUT -------------------------------------------------------
		for (int i = 0; i < 4; i++)
		{
			bKey[i] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[i]))) != 0;
		}
		// GAME LOGIC -------------------------------------------------------
		// nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		if (bKey[0])
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
			{
				nCurrentX -= 1;
			}
		}
		if (bKey[2])
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
			{
				nCurrentX += 1;
			}
		}
		if (bKey[3])
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			{
				nCurrentY += 1;
			}
		}
		if (bKey[1])
		{
			nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else
		{
			bRotateHold = false;
		}

		if (bForceDown)
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			{
				nCurrentY += 1;
			}
			else
			{
				for (int px = 0; px < 4; px++)
				{
					for (int py = 0; py < 4; py++)
					{
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
						{
							pField[((nCurrentY + py) * nFieldWidth + (nCurrentX + px))] = nCurrentPiece + 1;
						}
					}
				}

				nPieceCount++;
				if (nPieceCount % 10 == 0)
				{
					if (nSpeed >= 10)
					{
						nSpeed--;
					}
				}

				for (int py = 0; py < 4; py++)
				{
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 0; px < nFieldWidth; px++)
						{
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
						}

						if (bLine)
						{
							for (int px = 1; px < nFieldWidth - 1; px++)
							{
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;
							}
							vLines.push_back(nCurrentY + py);
						}
					}
				}
				nScore += 25;
				if (!vLines.empty())
				{
					nScore += (1 << vLines.size() - 1) * 100;
				}

				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
			nSpeedCounter = 0;
		}

		// RENDER OUTPUT -------------------------------------------------------



		for (int x = 0; x < nFieldWidth; x++)
		{
			for (int y = 0; y < nFieldHeight; y++)
			{
				screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
			}
		}
		
		for (int px = 0; px < 4; px++)
		{
			for (int py = 0; py < 4; py++)
			{
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
				{
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
				}
			}
		}

		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"Score: %8d", nScore);

		if (!vLines.empty())
		{
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth* nScreenHeight, { 0, 0 }, & dwBytesWritten);
			this_thread::sleep_for(400ms);

			for (auto &v : vLines)
			{
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
					{
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					}
					pField[px] = 0;
				}
			}
			vLines.clear();
		}

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth* nScreenHeight, { 0, 0 }, & dwBytesWritten);

	}

	CloseHandle(hConsole);
	cout << "Game Over! Score: " << nScore << endl;
	system("pause");

	return 0;
}
