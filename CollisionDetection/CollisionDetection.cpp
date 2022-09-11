#include <iostream>
#include <string>
#include "olcConsoleGameEngine.h"

using namespace std;

struct sBall
{
	int id;
	float radius;
	float mass;

	float px, py;
	float vx, vy;
	float ax, ay;
};

class CollisionDetection : public olcConsoleGameEngine
{
public:
	CollisionDetection()
	{
		m_sAppName = L"CollisionDetection";
	}

private:
	vector<pair<float, float>> modelCircle;
	vector<sBall> vecBalls;
	sBall* pSelectedBall = nullptr;

	void AddBall(float x, float y, float r = 10.0f, float m = 15.0f)
	{
		sBall ball;

		ball.id = vecBalls.size();

		ball.radius = r;
		ball.mass = m;

		ball.px = x;
		ball.py = y;

		ball.vx = 0;
		ball.vy = 0;

		ball.ax = 0;
		ball.ay = 0;

		vecBalls.emplace_back(ball);
	}

public:
	bool OnUserCreate()
	{
		modelCircle.push_back({ 0.0f, 0.0f });
		int nPoints = 20;
		for (int i = 0; i < nPoints; i++)
		{
			modelCircle.push_back({cosf(i / (float)(nPoints - 1) * 2.0f * 3.1415f), sinf(i / (float)(nPoints - 1) * 2.0f * 3.1415f)});
		}

		//AddBall(ScreenWidth() * .25f, ScreenHeight() * .5f);
		//AddBall(ScreenWidth() * .75f, ScreenHeight() * .5f);

		for (int i = 0; i < 10; i++)
		{
			AddBall(rand() % ScreenWidth(), rand() % ScreenHeight(), rand() % 10 + 2, rand() % 16 + 1);
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		auto DoCirclesOverLap = [](float x1, float y1, float r1, float x2, float y2, float r2)
		{
			return (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) < (r1 + r2));
		};

		auto IsPointInCircle = [](float x1, float y1, float r1, float px, float py)
		{
			return (sqrt((x1 - px) * (x1 - px) + (y1 - py) * (y1 - py)) < r1);
		};

		if (m_mouse[0].bPressed || m_mouse[1].bPressed)
		{
			pSelectedBall = nullptr;
			for (auto &ball : vecBalls)
			{
				if (IsPointInCircle(ball.px, ball.py, ball.radius, m_mousePosX, m_mousePosY))
				{
					pSelectedBall = &ball;
					break;
				}
			}
		}

		if (m_mouse[0].bHeld)
		{
			if (pSelectedBall != nullptr)
			{
				pSelectedBall -> px = m_mousePosX;
				pSelectedBall -> py = m_mousePosY;
			}

		}

		if (m_mouse[1].bReleased)
		{
			if (pSelectedBall != nullptr)
			{
				pSelectedBall->vx = 5.0f * ((pSelectedBall->px) - (float)m_mousePosX);
				pSelectedBall->vy = 5.0f * ((pSelectedBall->py) - (float)m_mousePosY);
			}

			pSelectedBall = nullptr;
		}

		if (m_mouse[0].bReleased)
		{
			pSelectedBall = nullptr;
		}

		vector<pair<sBall*, sBall*>> vecCollidingPairs;

		for (auto &ball : vecBalls)
		{
			ball.ax = -ball.vx * .75f;
			ball.ay = -ball.vy * .75f;
			ball.vx += ball.ax * fElapsedTime;
			ball.vy += ball.ay * fElapsedTime;
			ball.px += ball.vx * fElapsedTime;
			ball.py += ball.vy * fElapsedTime;

			if (ball.px < 0) ball.px += (float)ScreenWidth();
			if (ball.px >= ScreenWidth()) ball.px -= (float)ScreenWidth();
			
			if (ball.py < 0) ball.py += (float)ScreenHeight();
			if (ball.py >= ScreenHeight()) ball.py -= (float)ScreenHeight();
		
			if (fabs(ball.vx * ball.vx + ball.vy + ball.vy) < 0.01f)
			{
				ball.vx = 0;
				ball.vy = 0;
			}
		}

		for (auto &ball : vecBalls)
		{
			for (auto &target : vecBalls)
			{
				if (ball.id == target.id)
				{
					continue;
				}
				if (DoCirclesOverLap(ball.px, ball.py, ball.radius, target.px, target.py, target.radius))
				{
					vecCollidingPairs.push_back({&ball, &target});
					float fDistance = sqrtf((ball.px - target.px) * (ball.px - target.px) + (ball.py - target.py) * (ball.py - target.py));

					float fOverlap = (fDistance - (ball.radius + target.radius)) / 2.0f;

					ball.px -= fOverlap * (ball.px - target.px) / fDistance;
					ball.py -= fOverlap * (ball.py - target.py) / fDistance;
					
					
					target.px += fOverlap * (ball.px - target.px) / fDistance;
					target.py += fOverlap * (ball.py - target.py) / fDistance;
				}
			}
		}

		for (auto c : vecCollidingPairs)
		{
			sBall *b1 = c.first;
			sBall *b2 = c.second;

			// Distance between balls
			float fDistance = sqrtf((b1->px - b2->px)*(b1->px - b2->px) + (b1->py - b2->py)*(b1->py - b2->py));

			// Normal
			float nx = (b2->px - b1->px) / fDistance;
			float ny = (b2->py - b1->py) / fDistance;

			// Tangent
			float tx = -ny;
			float ty = nx;

			// Dot Product Tangent
			float dpTan1 = b1->vx * tx + b1->vy * ty;
			float dpTan2 = b2->vx * tx + b2->vy * ty;

			// Dot Product Normal
			float dpNorm1 = b1->vx * nx + b1->vy * ny;
			float dpNorm2 = b2->vx * nx + b2->vy * ny;

			// Conservation of momentum in 1D
			float m1 = (dpNorm1 * (b1->mass - b2->mass) + 2.0f * b2->mass * dpNorm2) / (b1->mass + b2->mass);
			float m2 = (dpNorm2 * (b2->mass - b1->mass) + 2.0f * b1->mass * dpNorm1) / (b1->mass + b2->mass);

			// Update ball velocities
			b1->vx = tx * dpTan1 + nx * m1;
			b1->vy = ty * dpTan1 + ny * m1;
			b2->vx = tx * dpTan2 + nx * m2;
			b2->vy = ty * dpTan2 + ny * m2;

			// Wikipedia Version - Maths is smarter but same
			//float kx = (b1->vx - b2->vx);
			//float ky = (b1->vy - b2->vy);
			//float p = 2.0 * (nx * kx + ny * ky) / (b1->mass + b2->mass);
			//b1->vx = b1->vx - p * b2->mass * nx;
			//b1->vy = b1->vy - p * b2->mass * ny;
			//b2->vx = b2->vx + p * b1->mass * nx;
			//b2->vy = b2->vy + p * b1->mass * ny;
		}

		Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ');

		for (sBall ball : vecBalls)
		{
			DrawWireFrameModel(modelCircle, ball.px, ball.py, atan2f(ball.vy, ball.vx), ball.radius, FG_WHITE);
		}

		for (auto c : vecCollidingPairs)
		{
			DrawLine(c.first->px, c.first->py, c.second->px, c.second->py, PIXEL_SOLID, FG_RED);
		}

		if (pSelectedBall != nullptr)
		{
			DrawLine(pSelectedBall->px, pSelectedBall->py, m_mousePosX, m_mousePosY, PIXEL_SOLID, FG_BLUE);
		}

		return true;
	}
};


int main()
{
	CollisionDetection game;
	if (game.ConstructConsole(160, 120, 8, 8))
	{
		game.Start();
	}
	else
	{
		wcout << L"Console construction error" << endl;
	}

	return 0;
}
