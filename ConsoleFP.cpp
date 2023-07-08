#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
using namespace std;

#include <Windows.h>
#include "ConsoleFP.h"
#include "ConsoleDisplay.h"

#include "HelperFunctions.h"
#include "PlayerObject.h"

float DeltaTime;

wstring Map;
int MapHeight = 16;
int MapWidth = 16;
FVector2D SunDirection = FVector2D(1.0f, -1.0f).Normalize();

PlayerObject Player(FVector2D(7.5f, 7.5f));

class Display : public ConsoleDisplay
{
public:
	virtual void OnUpdate(float DeltaTime)
	{
		EventTick(DeltaTime);
	}
};

Display Console;


int main()
{
		   //0123456789012345
	Map += L"################"; //0
	Map += L"#..............#"; //1
	Map += L"#..............#"; //2
	Map += L"#......#.......#"; //3
	Map += L"#......#.......#"; //4
	Map += L"#..............#"; //5
	Map += L"#..............."; //6
	Map += L"#..........##..."; //7
	Map += L"#...#........#.."; //8
	Map += L"#.............#."; //9
	Map += L"#..............#"; //10
	Map += L"#..............#"; //11
	Map += L"#..............#"; //12
	Map += L"#..............#"; //13
	Map += L"#..............#"; //14
	Map += L"################"; //15

	Console.CreateConsole(200, 100, 8, 8);
	SetCursorPos(500, 500);
	Console.Start();

	return 0;
}

void EventTick(float DeltaT)
{
	DeltaTime = DeltaT;

	//SunDirection = FVector2D(cosf(0.08726f * DeltaTime) * SunDirection.X - sinf(0.08726f * DeltaTime) * SunDirection.Y, sinf(0.08726f * DeltaTime) * SunDirection.X + cosf(0.08726f * DeltaTime) * SunDirection.Y);

	HandleInput();

	CalculatePixels();

	DrawUI();
}


void HandleInput()
{
	FVector2D LookDir = FVector2D(cosf(Player.Rotation.X), sinf(Player.Rotation.X));

	// Rotation
	if (GetAsyncKeyState(VK_LEFT/*(unsigned short)'Q'*/))
	{
		Player.Rotation.X += -Player.Sensitivity * 1.8f * DeltaTime;
	}
	if (GetAsyncKeyState(VK_RIGHT/*(unsigned short)'E'*/))
	{
		Player.Rotation.X += Player.Sensitivity * 1.8f * DeltaTime;
	}
	if (GetAsyncKeyState(VK_UP/*(unsigned short)'Z'*/))
	{
		Player.Rotation.Y += +Player.Sensitivity * 1.8f * DeltaTime;
	}
	if (GetAsyncKeyState(VK_DOWN/*(unsigned short)'X'*/))
	{
		Player.Rotation.Y += -Player.Sensitivity * 1.8f * DeltaTime;
	}

	POINT pos;
	GetCursorPos(&pos);
	if (pos.x != 500)
	{
		Player.Rotation.X += (pos.x - 500) * (Player.Sensitivity * 0.01f/* * DeltaTime*/);
	}
	if (pos.y != 500)
	{
		Player.Rotation.Y += (pos.y - 500) * (-Player.Sensitivity * 1.5f * 0.01f/* * DeltaTime*/);
		Player.Rotation.Y = clamp(Player.Rotation.Y, -3.14159f / 4.0f, 3.14159f / 4.0f);
	}
	SetCursorPos(500, 500);

	// Build and Destroy
	if (GetAsyncKeyState((unsigned short)'F'))
	{
		Hit Destroy = LineTrace(Player.Location, Player.Location + LookDir * 1.0f);
		Destroy.Location -= Destroy.Normal * 0.1f;
		if (Destroy.DidHit && Map[(int)Destroy.Location.Y * MapWidth + (int)Destroy.Location.X] == '#')
		{
			Map[(int)Destroy.Location.Y * MapWidth + (int)Destroy.Location.X] = '.';
		}
	}
	if (GetAsyncKeyState((unsigned short)'R'))
	{
		FVector2D BuildLoc = Player.Location + LookDir * 1.41f;
		Hit Build = LineTrace(Player.Location, BuildLoc);
		if (!Build.DidHit && Map[(int)BuildLoc.Y * MapWidth + (int)BuildLoc.X] == '.')
		{
			Map[(int)BuildLoc.Y * MapWidth + (int)BuildLoc.X] = '#';
		}
	}

	// Movement
	FVector2D MovementVector;
	if (GetAsyncKeyState((unsigned short)'W'))
	{
		MovementVector.X += cosf(Player.Rotation.X);
		MovementVector.Y += sinf(Player.Rotation.X);
	}
	if (GetAsyncKeyState((unsigned short)'S'))
	{
		MovementVector.X -= cosf(Player.Rotation.X);
		MovementVector.Y -= sinf(Player.Rotation.X);
	}
	if (GetAsyncKeyState((unsigned short)'D'))
	{
		MovementVector.X += -sinf(Player.Rotation.X);
		MovementVector.Y += cosf(Player.Rotation.X);
	}
	if (GetAsyncKeyState((unsigned short)'A'))
	{
		MovementVector.X -= -sinf(Player.Rotation.X);
		MovementVector.Y -= cosf(Player.Rotation.X);
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
	for (int x = 0; x < Console.ScreenWidth; x++)
	{
		// For each Screen "pixel", calculate a ray angle
		float RayAngle = (Player.Rotation.X - Player.FOV / 2.0f) + ((float)x / (float)Console.ScreenWidth * Player.FOV);

		// Direction vector of the ray
		FVector2D LookDir(cosf(RayAngle), sinf(RayAngle));

		Hit Impact =  LineTrace(Player.Location, Player.Location + LookDir * Player.ViewDistance);

		CalculateShading(Impact, x);
	}
}

Hit LineTrace(FVector2D Start, FVector2D End)
{
	Hit HitData;

	FVector2D LookDir = (End - Start).Normalize();

	FVector2D RayStep = FVector2D(abs(1.0f / LookDir.X), abs(1.0f / LookDir.Y));

	IVector2D CheckedCell = IVector2D(Start.X, Start.Y);

	FVector2D RayLength1D;
	IVector2D Step;

	if (LookDir.X < 0)
	{
		Step.X = -1;
		RayLength1D.X = (Start.X - CheckedCell.X) * RayStep.X;
	}
	else
	{
		Step.X = 1;
		RayLength1D.X = (CheckedCell.X + 1 - Start.X) * RayStep.X;
	}

	if (LookDir.Y < 0)
	{
		Step.Y = -1;
		RayLength1D.Y = (Start.Y - CheckedCell.Y) * RayStep.Y;
	}
	else
	{
		Step.Y = 1;
		RayLength1D.Y = (CheckedCell.Y + 1 - Start.Y) * RayStep.Y;
	}

	while (!HitData.DidHit && HitData.Distance < (End - Start).Lenght())
	{
		if (RayLength1D.X < RayLength1D.Y)
		{
			CheckedCell.X += Step.X;
			HitData.Distance = RayLength1D.X;
			RayLength1D.X += RayStep.X;
		}
		else
		{
			CheckedCell.Y += Step.Y;
			HitData.Distance = RayLength1D.Y;
			RayLength1D.Y += RayStep.Y;
		}

		if (HitData.Distance > (End - Start).Lenght())
		{
			break;
		}

		if (CheckedCell.X >= 0 && CheckedCell.X < MapWidth && CheckedCell.Y >= 0 && CheckedCell.Y < MapHeight)
		{
			if (Map[CheckedCell.Y * MapWidth + CheckedCell.X] == '#')
			{
				HitData.DidHit = true;

				HitData.Location = Start + LookDir * HitData.Distance;
				FVector2D Normal;
				if (abs(HitData.Location.X - CheckedCell.X - 0.5f) > abs(HitData.Location.Y - CheckedCell.Y - 0.5f))
				{
					if (HitData.Location.X - CheckedCell.X - 0.5f > 0.0f)
					{
						HitData.Normal.X = 1.0f;
					}
					else
					{
						HitData.Normal.X = -1.0f;
					}
				}
				else
				{
					if (HitData.Location.Y - CheckedCell.Y - 0.5f > 0.0f)
					{
						HitData.Normal.Y = 1.0f;
					}
					else
					{
						HitData.Normal.Y = -1.0f;
					}
				}
			}
		}
	}

	if (!HitData.DidHit)
	{
		HitData.Location = Start + LookDir * (End - Start).Lenght();
	}

	return HitData;
}

void CalculateShading(Hit HitData, int x)
{
	// How many radians from horizon to hit ceiling
	float AngleToCeiling = atanf(Player.Heigth / HitData.Distance) - Player.Rotation.Y;
	float PrecentageWallTakesOfTopHalfScreen = AngleToCeiling / (Player.vFOV / 2.0f);

	// How many radians from horizon to hit floor
	float AngleToFloor = atanf(Player.Heigth / HitData.Distance) + Player.Rotation.Y;
	float PrecentageWallTakesOfBottomHalfScreen = AngleToFloor / (Player.vFOV / 2.0f);

	// Calculate ceiling and floor size
	int Ceiling = Console.ScreenHeight / 2.0f - (Console.ScreenHeight / 2.0f) * PrecentageWallTakesOfTopHalfScreen;
	int Floor = Console.ScreenHeight / 2.0f + (Console.ScreenHeight / 2.0f) * PrecentageWallTakesOfBottomHalfScreen;
	//int Floor = Console.ScreenHeight - Ceiling;

	short Shade = ' ';

	for (int y = 0; y < Console.ScreenHeight; y++)
	{
		Console.Screen[y * Console.ScreenWidth + x].Attributes = 0x000F;

		if (y < Ceiling)
		{
			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = ' ';
		}
		else if (y >= Ceiling && y <= Floor)
		{
			if (HitData.Distance <= Player.ViewDistance / 4.0f)			Shade = 0x2588;	// Very close
			else if (HitData.Distance < Player.ViewDistance / 3.0f)		Shade = 0x2588;//0x2593;
			else if (HitData.Distance < Player.ViewDistance / 2.0f)		Shade = 0x2588;//0x2592;
			else if (HitData.Distance < Player.ViewDistance)			Shade = 0x2588;//0x2591;
			else														Shade = ' ';	// Too far away

			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = Shade;

			WallLighting(x, y, HitData);
		}
		else
		{
			Shade = '#';
			/*float b = 1.0f - ((y - Console.ScreenHeight / 2.0f) / (Console.ScreenHeight / 2.0f));
			if (b < 0.75f)		Shade = '#';//'#';
			else if (b < 0.85f)	Shade = '#';//'x';
			else if (b < 0.9f)	Shade = '#';//'.';
			else if (b < 0.95f)	Shade = '#';//'-';
			else				Shade = ' ';*/

			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = Shade;

			FloorLighting(x, y, HitData);
		}
	}
}

void WallLighting(int x, int y, Hit& HitData)
{
	// Find angle between SunDirection and Wall Normal
	float AngleToSun = acosf(HitData.Normal * SunDirection);

	// If angle is smaller than 90 degrees, this side is lit
	if (3.14f / 2.0f > AngleToSun)
	{
		if (abs(HitData.Normal.X) > 0.9f)
		{
			Console.Screen[y * Console.ScreenWidth + x].Attributes = 0x0007;
		}
		else
		{
			Console.Screen[y * Console.ScreenWidth + x].Attributes = 0x000F;
		}

		float RayAngle = 3.14159f / 2.0f + Player.Rotation.Y - (y - Console.ScreenHeight / 2.0f) / (Console.ScreenHeight / 2.0f) * (Player.vFOV / 2.0f);
		float PointHeight = 1.0f - Player.Heigth + HitData.Distance / tanf(RayAngle);
		FVector2D EndLoc = HitData.Location + SunDirection * 1.0f;

		Hit LightRay = LineTrace(HitData.Location, EndLoc);
		if (LightRay.DidHit)
		{
			if (LightRay.Distance <= PointHeight)
			{
				Console.Screen[y * Console.ScreenWidth + x].Attributes = 0x000B;
			}
		}
	}
	else
	{
		if (abs(HitData.Normal.Y) > 0.9f)
		{
			Console.Screen[y * Console.ScreenWidth + x].Attributes = 0x0001;
		}
		else
		{
			Console.Screen[y * Console.ScreenWidth + x].Attributes = 0x0009;
		}
	}
}

void FloorLighting(int x, int y, Hit& HitData)
{
	// y is always bigger than half ScreenHeight, due to it being bottom half of the screen
	float PrecentageOfBottomHalfScreen = (y - Console.ScreenHeight / 2.0f) / (Console.ScreenHeight / 2.0f);
	// Multiply the precentage with player bottom half vFOV and subtract it from angle between Floor and Player look direction
	float RayAngle = 3.14159f / 2.0f + Player.Rotation.Y - PrecentageOfBottomHalfScreen * (Player.vFOV / 2.0f);

	// Floor location relative to the Player
	float RelativeFloorLoc = tanf(RayAngle) * Player.Heigth;
	// Direction from the Player towards FloorLoc
	FVector2D FloorLocDirection = (HitData.Location - Player.Location).Normalize();
	// Floor location in World Space
	FVector2D FloorLoc = Player.Location + FloorLocDirection * RelativeFloorLoc;

	FVector2D EndLoc = FloorLoc + SunDirection * 1.0f;

	Hit LightRay = LineTrace(FloorLoc, EndLoc);
	if (LightRay.DidHit)
	{
		Console.Screen[y * Console.ScreenWidth + x].Attributes = 0x000B;
	}
	else
	{
		Console.Screen[y * Console.ScreenWidth + x].Attributes = 0x000F;
	}
}

void DrawUI()
{
	//wchar_t s[256];
	//swprintf_s(s, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", Player.Location.X, Player.Location.Y, Player.Rotation, 1.0f / DeltaTime);

	for (int x = 0; x < MapWidth; x++)
	{
		for (int y = 0; y < MapWidth; y++)
		{
			Console.Screen[(y + 1) * Console.ScreenWidth + x].Char.UnicodeChar = Map[y * MapWidth + x];
		}
	}
	short MapPlayer = 0x2194;
	FVector2D LookDir = FVector2D(cosf(Player.Rotation.X), sinf(Player.Rotation.X));
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
	

	Console.Screen[((int)Player.Location.Y + 1) * Console.ScreenWidth + (int)Player.Location.X].Char.UnicodeChar = MapPlayer;
}