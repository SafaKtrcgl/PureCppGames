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
	float ox, oy;

	float fSimTimeRemaining;
};

struct sLineSegment
{
	float sx, sy;
	float ex, ey;
	float radius;
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

	vector<sLineSegment> vecLines;
	sLineSegment* pSelectedLine = nullptr;
	bool bStartOfTheLine = false;

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
		//modelCircle.push_back({ 0.0f, 0.0f });
		//int nPoints = 20;
		//for (int i = 0; i < nPoints; i++)
		//{
		//	modelCircle.push_back({cosf(i / (float)(nPoints - 1) * 2.0f * 3.1415f), sinf(i / (float)(nPoints - 1) * 2.0f * 3.1415f)});
		//}

		//AddBall(ScreenWidth() * .25f, ScreenHeight() * .5f);
		//AddBall(ScreenWidth() * .75f, ScreenHeight() * .5f);

		for (int i = 0; i < 50; i++)
		{
			AddBall(rand() % ScreenWidth(), rand() % ScreenHeight(), 2, rand() % 16 + 1);
		}

		vecLines.push_back({30.0f, 30.0f, 80.0f, 60.0f, 4.0f});
		vecLines.push_back({(float)ScreenWidth() - 30.0f, 30.0f, (float)ScreenWidth() - 80.0f, 60.0f, 4.0f});

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

			pSelectedLine = nullptr;
			for (auto &line : vecLines)
			{
				if (IsPointInCircle(line.sx, line.sy, line.radius, m_mousePosX, m_mousePosY))
				{
					pSelectedLine = &line;
					bStartOfTheLine = true;
					break;
				}
				if (IsPointInCircle(line.ex, line.ey, line.radius, m_mousePosX, m_mousePosY))
				{
					pSelectedLine = &line;
					bStartOfTheLine = false;
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

			if (pSelectedLine != nullptr)
			{
				if (bStartOfTheLine)
				{
					pSelectedLine->sx = m_mousePosX;
					pSelectedLine->sy = m_mousePosY;
				}
				else
				{
					pSelectedLine->ex = m_mousePosX;
					pSelectedLine->ey = m_mousePosY;
				}
			}
		}

		if (m_mouse[0].bReleased)
		{
			pSelectedBall = nullptr;
			pSelectedLine = nullptr;
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

		if (m_mouse[2].bPressed)
		{
			for (auto &ball : vecBalls)
			{
				ball.vx = 0.0f;
				ball.vy = 0.0f;
				ball.ax = 0.0f;
				ball.ay = 0.0f;
			}
		}

		vector<pair<sBall*, sBall*>> vecCollidingPairs;
		vector<sBall*> vecFakeBalls;

		int nSimulationUpdate = 4;

		float fSimElapsedTime = fElapsedTime / (float)nSimulationUpdate;

		int nMaxSimulationSteps = 15;


		//Physics
		for (int i = 0; i < nSimulationUpdate; i++)
		{
			for (auto &ball : vecBalls)
			{
				ball.fSimTimeRemaining = fSimElapsedTime;
			}

			for (int j = 0; j < nMaxSimulationSteps; j++)
			{
				// Updates balls position and adjust the acceleration & velocity
				for (auto& ball : vecBalls)
				{
					if (ball.fSimTimeRemaining > 0.0f)
					{
						ball.ox = ball.px;
						ball.oy = ball.py;

						ball.ax = -ball.vx * .75f;
						ball.ay = -ball.vy * .75f + 25.0f;

						ball.vx += ball.ax * ball.fSimTimeRemaining;
						ball.vy += ball.ay * ball.fSimTimeRemaining;
						ball.px += ball.vx * ball.fSimTimeRemaining;
						ball.py += ball.vy * ball.fSimTimeRemaining;

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
				}

				for (auto& ball : vecBalls)
				{
					for (auto& edge : vecLines)
					{
						float fLineX1 = edge.ex - edge.sx;
						float fLineY1 = edge.ey - edge.sy;
						
						float fLineX2 = ball.px - edge.sx;
						float fLineY2 = ball.py - edge.sy;

						float fEdgeLength = fLineX1 * fLineX1 + fLineY1 * fLineY1;
						float t = max(0, min(fEdgeLength, (fLineX1 * fLineX2 + fLineY1 * fLineY2))) / fEdgeLength;
						
						float fClosestPointX = edge.sx + t * fLineX1;
						float fClosestPointY = edge.sy + t * fLineY1;

						float fDistance = sqrtf((ball.px - fClosestPointX) * (ball.px - fClosestPointX) + (ball.py - fClosestPointY) * (ball.py - fClosestPointY));

						if (fDistance <= ball.radius + edge.radius)
						{
							sBall* fakeBall = new sBall();
							fakeBall->radius = edge.radius;
							fakeBall->mass = ball.mass * 1.0f;
							fakeBall->px = fClosestPointX;
							fakeBall->py = fClosestPointY;
							fakeBall->vx = -ball.vx;
							fakeBall->vy = -ball.vy;

							vecFakeBalls.push_back(fakeBall);
							vecCollidingPairs.push_back({ &ball, fakeBall});
						
							float fOverlap = 1.0f * (fDistance - ball.radius - fakeBall->radius);

							ball.px -= fOverlap * (ball.px - fakeBall->px) / fDistance;
							ball.py -= fOverlap * (ball.py - fakeBall->py) / fDistance;
						}
					}

					for (auto& target : vecBalls)
					{
						if (ball.id == target.id)
						{
							continue;
						}
						if (DoCirclesOverLap(ball.px, ball.py, ball.radius, target.px, target.py, target.radius))
						{
							vecCollidingPairs.push_back({ &ball, &target });
							float fDistance = sqrtf((ball.px - target.px) * (ball.px - target.px) + (ball.py - target.py) * (ball.py - target.py));

							float fOverlap = (fDistance - (ball.radius + target.radius)) / 2.0f;

							ball.px -= fOverlap * (ball.px - target.px) / fDistance;
							ball.py -= fOverlap * (ball.py - target.py) / fDistance;


							target.px += fOverlap * (ball.px - target.px) / fDistance;
							target.py += fOverlap * (ball.py - target.py) / fDistance;
						}
					}

					float fIntendedSpeed = sqrtf(ball.vx * ball.vx + ball.vy * ball.vy);
					float fIntendedDistance = fIntendedSpeed * ball.fSimTimeRemaining;
					float fActualDistance = sqrtf((ball.px - ball.ox) * (ball.px - ball.ox) + (ball.py - ball.oy) * (ball.py - ball.oy));
					float fActualTime = fActualDistance / fIntendedSpeed;

					ball.fSimTimeRemaining -= fActualTime;
				}


				for (auto c : vecCollidingPairs)
				{
					sBall* b1 = c.first;
					sBall* b2 = c.second;

					// Distance between balls
					float fDistance = sqrtf((b1->px - b2->px) * (b1->px - b2->px) + (b1->py - b2->py) * (b1->py - b2->py));

					// Normal
					float nx = (b2->px - b1->px) / fDistance;
					float ny = (b2->py - b1->py) / fDistance;

					// Tangent
					float tx = -ny;
					float ty = nx;

					/*
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
					*/

					// Wikipedia Version - Maths is smarter but same
					float kx = (b1->vx - b2->vx);
					float ky = (b1->vy - b2->vy);
					float p = 2.0 * (nx * kx + ny * ky) / (b1->mass + b2->mass);
					b1->vx = b1->vx - p * b2->mass * nx;
					b1->vy = b1->vy - p * b2->mass * ny;
					b2->vx = b2->vx + p * b1->mass * nx;
					b2->vy = b2->vy + p * b1->mass * ny;
				}
				for (auto &b : vecFakeBalls)
				{
					delete b;
				}
				vecFakeBalls.clear();

				vecCollidingPairs.clear();
			}
		}

		Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ');

		//Draw Lines
		for (auto line : vecLines)
		{
			FillCircle(line.sx, line.sy, line.radius, PIXEL_HALF, FG_WHITE);
			FillCircle(line.ex, line.ey, line.radius, PIXEL_HALF, FG_WHITE);
		
			float nx = line.sy - line.ey;
			float ny = line.ex - line.sx;
			
			float d = sqrt(nx * nx + ny * ny);
			
			nx /= d;
			ny /= d;

			DrawLine((line.sx + nx * line.radius), (line.sy + ny * line.radius), (line.ex + nx * line.radius), (line.ey + ny * line.radius));
			DrawLine((line.sx - nx * line.radius), (line.sy - ny * line.radius), (line.ex - nx * line.radius), (line.ey - ny * line.radius));
		}

		//Draw Balls
		for (sBall ball : vecBalls)
		{
			FillCircle(ball.px, ball.py, ball.radius, PIXEL_SOLID, FG_WHITE);
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
	if (game.ConstructConsole(180, 120, 8, 8))
	{
		game.Start();
	}
	else
	{
		wcout << L"Console construction error" << endl;
	}

	return 0;
}
