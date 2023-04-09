#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
using namespace std;

#include <Windows.h>
#include "ConsoleFP.h"

#include "HelperFunctions.h"
#include "PlayerObject.h"

int ScreenWidth = 512;
int ScreenHeight = 128;

float DeltaTime;
wchar_t* Screen;
wstring Map;

int MapHeight = 16;
int MapWidth = 16;

PlayerObject Player(FVector2D(8.0f, 8.0f));

int main()
{
    // Create Screen buffer
    Screen = new wchar_t[ScreenWidth * ScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    
	Map += L"################";
	Map += L"#..............#";
	Map += L"#..............#";
	Map += L"#.......#......#";
	Map += L"#.......#......#";
	Map += L"#..............#";
	Map += L"#..............#";
	Map += L"#..........#....";
	Map += L"#...#.......##..";
	Map += L"#.............#.";
	Map += L"#.............#.";
	Map += L"#..............#";
	Map += L"#..............#";
	Map += L"#..............#";
	Map += L"#..............#";
	Map += L"################";


	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();


	// Game loop
    while (1)
    {
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elaspedTime = tp2 - tp1;
		tp1 = tp2;
		DeltaTime = elaspedTime.count();

		if (GetAsyncKeyState(VK_ESCAPE))
		{
			break;
		}

		EventTick();

        Screen[ScreenWidth * ScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, Screen, ScreenWidth * ScreenHeight, { 0, 0 }, &dwBytesWritten);
    }


    return 0;
}

void EventTick()
{
	HandleInput();

	CalculatePixels();

	DrawUI();
}

void HandleInput()
{
	// Rotation
	if (GetAsyncKeyState((unsigned short)'Q'))
	{
		Player.Rotation += -Player.Sensitivity * DeltaTime;
	}
	if (GetAsyncKeyState((unsigned short)'E'))
	{
		Player.Rotation += Player.Sensitivity * DeltaTime;
	}

	// Movement
	FVector2D MovementVector;
	if (GetAsyncKeyState((unsigned short)'W'))
	{
		MovementVector.X += cosf(Player.Rotation);
		MovementVector.Y += sinf(Player.Rotation);
	}
	if (GetAsyncKeyState((unsigned short)'S'))
	{
		MovementVector.X -= cosf(Player.Rotation);
		MovementVector.Y -= sinf(Player.Rotation);
	}
	if (GetAsyncKeyState((unsigned short)'D'))
	{
		MovementVector.X += -sinf(Player.Rotation);
		MovementVector.Y += cosf(Player.Rotation);
	}
	if (GetAsyncKeyState((unsigned short)'A'))
	{
		MovementVector.X -= -sinf(Player.Rotation);
		MovementVector.Y -= cosf(Player.Rotation);
	}

	HandleCollision(MovementVector);
}

void HandleCollision(FVector2D& MovementVector)
{
	if (MovementVector.X != 0 || MovementVector.Y != 0)
	{

		MovementVector = MovementVector.Normalize();

		FVector2D MoveLocation;

		MoveLocation.X = Player.Location.X + MovementVector.X * Player.Speed * DeltaTime;
		MoveLocation.Y = Player.Location.Y + MovementVector.Y * Player.Speed * DeltaTime;

		IVector2D MoveCell = IVector2D(ceilf(MoveLocation.X), ceilf(MoveLocation.Y));
		IVector2D CurrentCell = IVector2D(floorf(Player.Location.X), floorf(Player.Location.Y));

		IVector2D TopLeft;
		TopLeft.X = clamp(CurrentCell.X - 1, 0, MoveCell.X - 1);
		TopLeft.Y = clamp(CurrentCell.Y - 1, 0, MoveCell.Y - 1);

		IVector2D BottomRight;
		BottomRight.X = clamp(CurrentCell.X + 1, MoveCell.X + 1, MapWidth);
		BottomRight.Y = clamp(CurrentCell.Y + 1, MoveCell.Y + 1, MapHeight);

		for (int x = TopLeft.X; x <= BottomRight.X; x++)
		{
			for (int y = TopLeft.Y; y < BottomRight.Y; y++)
			{
				if (Map[y * MapWidth + x] == '#')
				{
					FVector2D NearestPoint;
					NearestPoint.X = clamp(MoveLocation.X, (float)x, x + 1.0f);
					NearestPoint.Y = clamp(MoveLocation.Y, (float)y, y + 1.0f);

					FVector2D ToNearest = FVector2D(NearestPoint.X - MoveLocation.X, NearestPoint.Y - MoveLocation.Y);
					if (Player.Radius > ToNearest.Lenght())
					{
						MoveLocation -= ToNearest.Normalize() * (Player.Radius - ToNearest.Lenght());
					}
				}
			}
		}

		Player.Location = MoveLocation;
	}
}

void CalculatePixels()
{
	for (int x = 0; x < ScreenWidth; x++)
	{
		// For each Screen "pixel", calculate a ray angle
		float RayAngle = (Player.Rotation - Player.FOV / 2.0f) + ((float)x / (float)ScreenWidth * Player.FOV);

		float DistanceToWall = 0.0f;
		bool Boundary = false;

		// Direction vector of the ray
		FVector2D LookDir(cosf(RayAngle), sinf(RayAngle));

		TraceForWalls(LookDir, DistanceToWall, Boundary);

		CalculateShading(DistanceToWall, x, Boundary);
	}
}

void TraceForWalls(FVector2D LookDir, float &DistanceToWall, bool &Boundary)
{
	FVector2D RayStep = FVector2D(abs(1.0f / LookDir.X), abs(1.0f / LookDir.Y));

	IVector2D CheckedCell = IVector2D(Player.Location.X, Player.Location.Y);

	FVector2D RayLength1D;
	IVector2D Step;

	if (LookDir.X < 0)
	{
		Step.X = -1;
		RayLength1D.X = (Player.Location.X - CheckedCell.X) * RayStep.X;
	}
	else
	{
		Step.X = 1;
		RayLength1D.X = (CheckedCell.X + 1 - Player.Location.X) * RayStep.X;
	}

	if (LookDir.Y < 0)
	{
		Step.Y = -1;
		RayLength1D.Y = (Player.Location.Y - CheckedCell.Y) * RayStep.Y;
	}
	else
	{
		Step.Y = 1;
		RayLength1D.Y = (CheckedCell.Y + 1 - Player.Location.Y) * RayStep.Y;
	}

	bool HitWall = false;
	
	while (!HitWall && DistanceToWall < Player.ViewDistance)
	{
		if (RayLength1D.X < RayLength1D.Y)
		{
			CheckedCell.X += Step.X;
			DistanceToWall = RayLength1D.X;
			RayLength1D.X += RayStep.X;
		}
		else
		{
			CheckedCell.Y += Step.Y;
			DistanceToWall = RayLength1D.Y;
			RayLength1D.Y += RayStep.Y;
		}

		if (CheckedCell.X >= 0 && CheckedCell.X < MapWidth && CheckedCell.Y >= 0 && CheckedCell.Y < MapHeight)
		{
			if (Map[CheckedCell.Y * MapWidth + CheckedCell.X] == '#')
			{
				HitWall = true;

				vector<pair<float, float>> p;

				for (int tx = 0; tx < 2; tx++)
				{
					for (int ty = 0; ty < 2; ty++)
					{
						float vx = CheckedCell.X + tx - Player.Location.X;
						float vy = CheckedCell.Y + ty - Player.Location.Y;
						float d = sqrt(vx * vx + vy * vy);
						float dot = (LookDir.X * vx / d) + (LookDir.Y * vy / d);
						p.push_back(make_pair(d, dot));
					}
				}

				// Sort Pairs from closest to farthest
				sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first; });

				float fBound = 0.001f;
				if (acos(p.at(0).second) < fBound) Boundary = true;
				if (acos(p.at(1).second) < fBound) Boundary = true;
				//if (acos(p.at(2).second) < fBound) bBoundary = true;
			}
		}
	}
}

void CalculateShading(float fDistanceToWall, int x, bool bBoundary)
{
	// Calculate ceiling and floor size
	int Ceiling = ScreenHeight / 2.0f - ScreenHeight / fDistanceToWall;
	int Floor = ScreenHeight - Ceiling;

	short Shade = ' ';

	for (int y = 0; y < ScreenHeight; y++)
	{
		if (y < Ceiling)
		{
			Screen[y * ScreenWidth + x] = ' ';
		}
		else if (y > Ceiling && y <= Floor)
		{
			if (fDistanceToWall <= Player.ViewDistance / 4.0f)			Shade = 0x2588;	// Very close	
			else if (fDistanceToWall < Player.ViewDistance / 3.0f)		Shade = 0x2593;
			else if (fDistanceToWall < Player.ViewDistance / 2.0f)		Shade = 0x2592;
			else if (fDistanceToWall < Player.ViewDistance)				Shade = 0x2591;
			else														Shade = ' ';		// Too far away

			if (bBoundary) Shade = ' ';

			Screen[y * ScreenWidth + x] = Shade;
		}
		else
		{
			float b = 1.0f - ((y - ScreenHeight / 2.0f) / (ScreenHeight / 2.0f));
			if (b < 0.75f)		Shade = '#';
			else if (b < 0.85f)	Shade = 'x';
			else if (b < 0.9f)	Shade = '-';
			else if (b < 0.95f)	Shade = '.';
			else				Shade = ' ';

			Screen[y * ScreenWidth + x] = Shade;
		}
	}
}

void DrawUI()
{
	swprintf_s(Screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", Player.Location.X, Player.Location.Y, Player.Rotation, 1.0f / DeltaTime);

	for (int x = 0; x < MapWidth; x++)
	{
		for (int y = 0; y < MapWidth; y++)
		{
			Screen[(y + 1) * ScreenWidth + x] = Map[y * MapWidth + x];
		}
	}
	short MapPlayer = 0x2194;
	FVector2D LookDir = FVector2D(cosf(Player.Rotation), sinf(Player.Rotation));
	if (LookDir.X > 0.9f)
	{
		MapPlayer = 0x2192;
	}
	else if(LookDir.X < -0.9f)
	{
		MapPlayer = 0x2190;
	}
	else if (LookDir.Y > 0.9f)
	{
		MapPlayer = 0x2193;
	}
	else if (LookDir.Y < -0.9f)
	{
		MapPlayer = 0x2191;
	}
	else if (LookDir.X > 0.1f && LookDir.Y < -0.1f)
	{
		MapPlayer = 0x2197;
	}
	else if (LookDir.X < -0.1f && LookDir.Y < -0.1f)
	{
		MapPlayer = 0x2196;
	}
	else if (LookDir.X < -0.1f && LookDir.Y > 0.1f)
	{
		MapPlayer = 0x2199;
	}
	else if (LookDir.X > 0.1f && LookDir.Y > 0.1f)
	{
		MapPlayer = 0x2198;
	}
	

	Screen[((int)Player.Location.Y + 1) * ScreenWidth + (int)Player.Location.X] = MapPlayer;
}
