#include <iostream>
//#include <vector>
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

const int ResX = 200;
const int ResY = 100;

PlayerObject Player(FVector2D(7.5f, 7.5f));

bool Debug = false;
bool Debug2 = false;

Object* Objects[16];
int ObjectsCount = 0;

float ProjectionPlaneDistance = 0.0f;
float ScreenSpaceAnglesX[ResX];
float ScreenSpaceAnglesY[ResY];
float DepthBuffer[ResX];

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
	Map += L"#######...######"; //15

	Console.CreateConsole(ResX, ResY, 8, 8);
	SetCursorPos(500, 500);

	InitScreenSpace();

	Player.vFOV = 2.0f * atanf((Console.ScreenHeight / 2.0f) / ProjectionPlaneDistance);

	Objects[ObjectsCount] = new Object(FVector2D(8.5f, 10.5f));
	ObjectsCount++;
	Objects[0]->color = 0x000C;

	Objects[ObjectsCount] = new Object(FVector2D(8.5f, 11.5f));
	ObjectsCount++;
	Objects[1]->color = 0x000B;

	Console.Start();

	return 0;
}

void InitScreenSpace()
{
	ProjectionPlaneDistance = (Console.ScreenWidth / 2.0f) / tanf(Player.FOV / 2.0f);

	for (int x = 0; x < ResX; x++)
	{
		ScreenSpaceAnglesX[x] = atanf((x - Console.ScreenWidth / 2.0f) / ProjectionPlaneDistance);
	}

	for (int y = 0; y < ResY; y++)
	{
		ScreenSpaceAnglesY[y] = atanf((y - Console.ScreenHeight / 2.0f) / ProjectionPlaneDistance);
	}
}

void EventTick(float DeltaT)
{
	DeltaTime = DeltaT;

	SunDirection = FVector2D(cosf(0.08726f * DeltaTime) * SunDirection.X - sinf(0.08726f * DeltaTime) * SunDirection.Y, sinf(0.08726f * DeltaTime) * SunDirection.X + cosf(0.08726f * DeltaTime) * SunDirection.Y);

	HandleInput();

	CalculatePixels();

	DrawUI();

	RenderObjects();
}

void HandleInput()
{
	FVector2D LookDir = FVector2D(cosf(Player.Rotation.X), sinf(Player.Rotation.X));

	// Rotation
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState((unsigned short)'Q'))
	{
		Player.Rotation.X += -Player.Sensitivity * DeltaTime;
	}
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState((unsigned short)'E'))
	{
		Player.Rotation.X += Player.Sensitivity * DeltaTime;
	}
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState((unsigned short)'Z'))
	{
		Player.Rotation.Y += +Player.Sensitivity * DeltaTime;
	}
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState((unsigned short)'X'))
	{
		Player.Rotation.Y += -Player.Sensitivity * DeltaTime;
	}

	if (GetAsyncKeyState((unsigned short)'C'))
	{
		Player.Height = 0.3f;
		Player.Speed = 1.5f;
	}
	else
	{
		Player.Height = 0.8f;
		Player.Speed = 5.0f;
	}

	if (GetAsyncKeyState((unsigned short)'G'))
	{
		Debug = true;
	}
	if (GetAsyncKeyState((unsigned short)'H'))
	{
		Debug = false;
	}

	if (GetAsyncKeyState((unsigned short)'B'))
	{
		Debug2 = true;
	}
	if (GetAsyncKeyState((unsigned short)'N'))
	{
		Debug2 = false;
	}

	POINT pos;
	GetCursorPos(&pos);
	if (pos.x != 500)
	{
		Player.Rotation.X += (pos.x - 500) * (Player.MouseSensitivity * 0.01f);
	}
	if (pos.y != 500)
	{
		Player.Rotation.Y += (pos.y - 500) * (-Player.MouseSensitivity * 0.01f);
	}
	Player.Rotation.Y = clamp(Player.Rotation.Y, -3.14159f / 6.0f, 3.14159f / 6.0f);
	SetCursorPos(500, 500);

	// Build and Destroy
	if (GetAsyncKeyState((unsigned short)'F'))
	{
		Hit Destroy = LineTrace(Player.Location, Player.Location + LookDir * 1.0f, true);
		Destroy.Location -= Destroy.Normal * 0.1f;
		if (Destroy.DidHit && Map[(int)Destroy.Location.Y * MapWidth + (int)Destroy.Location.X] == '#')
		{
			Map[(int)Destroy.Location.Y * MapWidth + (int)Destroy.Location.X] = '.';
		}
	}
	if (GetAsyncKeyState((unsigned short)'R'))
	{
		FVector2D BuildLoc = Player.Location + LookDir * 1.41f;
		Hit Build = LineTrace(Player.Location, BuildLoc, true);
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

Hit LineTrace(FVector2D Start, FVector2D End, bool OnlyWalls)
{
	Hit HitData;

	HitData.Start = Start;
	HitData.End = End;

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
			if ((!OnlyWalls && Map[CheckedCell.Y * MapWidth + CheckedCell.X] != '.') || (OnlyWalls && Map[CheckedCell.Y * MapWidth + CheckedCell.X] == '#'))
			{
				HitData.DidHit = true;

				HitData.Tile = Map[CheckedCell.Y * MapWidth + CheckedCell.X];

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

void CalculatePixels()
{
	for (int x = 0; x < Console.ScreenWidth; x++)
	{
		// For each Screen "pixel", calculate a ray angle
		float RayAngle = Player.Rotation.X + ScreenSpaceAnglesX[x];

		// Direction vector of the ray
		FVector2D LookDir(cosf(RayAngle), sinf(RayAngle));

		Hit Impact = LineTrace(Player.Location, Player.Location + LookDir * (Player.ViewDistance + 0.1f), true);

		if (Impact.DidHit)
		{
			// Adjust distance to remove distortion
			Impact.Distance = Impact.Distance * cosf(ScreenSpaceAnglesX[x]);
		}

		DepthBuffer[x] = Impact.Distance;
		
		CalculateShading(Impact, x);
	}
}

void CalculateShading(Hit HitData, int x)
{
	// MOVE Ceiling and Floor TO SEPARATE FUNCTION

	int Wall = GetWallStart(HitData);
	int Floor = GetFloorStart(HitData);

	short Shade = ' ';

	for (int y = 0; y < Console.ScreenHeight; y++)
	{
		Console.Screen[y * Console.ScreenWidth + x].Attributes = 0x000F;

		// Ceiling
		if (y < Wall)
		{
			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = ' ';
		}
		// Wall
		else if (y >= Wall && y <= Floor && HitData.Distance < Player.ViewDistance)
		{
			Shade = 0x2588;
			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = Shade;
			WallLighting(x, y, HitData);
		}
		// Floor
		else if (y > Floor)
		{
			Shade = 0x2587;
			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = Shade;
			FloorLighting(x, y, HitData);
		}
		// Sky when no wall?
		else
		{
			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = Shade;
		}
	}
}

int GetWallStart(Hit HitData)
{
	// How many radians from view center to hit the wall top
	float AngleToWall = atanf((1.0 - Player.Height) / (HitData.Distance)) - Player.Rotation.Y;
	if (AngleToWall <= Player.vFOV / 2.0f)
	{
		return ProjectionPlaneDistance * tanf(-AngleToWall) + Console.ScreenHeight / 2.0f;
	}
	else
	{
		return -1;
	}
}

int GetFloorStart(Hit HitData)
{
	// How many radians from view center to hit the floor top
	float AngleToFloor = atanf(Player.Height / (HitData.Distance)) + Player.Rotation.Y;
	if (AngleToFloor <= Player.vFOV / 2.0f)
	{
		return ProjectionPlaneDistance * tanf(AngleToFloor) + Console.ScreenHeight / 2.0f;
	}
	else
	{
		return Console.ScreenHeight;
	}
}

void WallLighting(int x, int y, Hit& HitData)
{
	// Find angle between SunDirection and Wall Normal
	float AngleToSun = acosf(HitData.Normal * SunDirection);

	FVector2D UV;
	
	float RayAngle = 3.14159f / 2.0f + Player.Rotation.Y - ScreenSpaceAnglesY[y];

	float PointHeight = 1.0f - Player.Height + HitData.Distance / tanf(RayAngle);

	// Floating precision error might make it 1.0f or bigger
	UV.Y = min(PointHeight, 0.9999f);

	if (HitData.Normal.X > 0.9f)
	{
		UV.X = (int)HitData.Location.Y + 1.0f - HitData.Location.Y;
	}
	else if (HitData.Normal.X < -0.9f)
	{
		UV.X = HitData.Location.Y - (int)HitData.Location.Y;
	}
	else if (HitData.Normal.Y > 0.9f)
	{
		UV.X = HitData.Location.X - (int)HitData.Location.X;
	}
	else if (HitData.Normal.Y < -0.9f)
	{
		UV.X = (int)HitData.Location.X + 1.0f - HitData.Location.X;
	}

	Console.Screen[y * Console.ScreenWidth + x].Attributes = DrawTexture(UV);

	// If angle is smaller than 90 degrees, this side is lit
	if (3.14f / 2.0f > AngleToSun)
	{
		if (abs(HitData.Normal.X) > 0.9f)
		{
			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = 0x2588;
		}
		else
		{
			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = 0x2593;
		}

		FVector2D EndLoc = HitData.Location + SunDirection * 1.0f;

		Hit LightRay = LineTrace(HitData.Location, EndLoc, true);
		if (LightRay.DidHit && LightRay.Tile == '#')
		{
			if (LightRay.Distance <= PointHeight)
			{
				Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = 0x2592;
			}
		}
	}
	else
	{
		if (abs(HitData.Normal.Y) > 0.9f)
		{
			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = 0x2592;
		}
		else
		{
			Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = 0x2591;
		}
	}
}

void FloorLighting(int x, int y, Hit& HitData)
{
	float RayAngle = 3.14159f / 2.0f + Player.Rotation.Y - ScreenSpaceAnglesY[y];

	// Floor location relative to the Player
	float RelativeFloorLoc = tanf(RayAngle) * Player.Height / cosf(ScreenSpaceAnglesX[x]);
	// Direction from the Player towards FloorLoc
	FVector2D FloorLocDirection = (HitData.Location - Player.Location).Normalize();
	// Floor location in World Space
	FVector2D FloorLoc = Player.Location + FloorLocDirection * RelativeFloorLoc;


	FVector2D UV;
	UV.X = FloorLoc.X - (int)FloorLoc.X; // If I want to fix glitchy textures in out of bounds, min the value to 0.0f for both X and Y
	UV.Y = FloorLoc.Y - (int)FloorLoc.Y;

	Console.Screen[y * Console.ScreenWidth + x].Attributes = DrawTexture(UV);

	FVector2D EndLoc = FloorLoc + SunDirection * 1.0f;

	Hit LightRay = LineTrace(FloorLoc, EndLoc, true);
	if (LightRay.DidHit)
	{
		Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = 0x2592;
	}
	else
	{
		Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = 0x2588;
	}
}

void RenderObjects()
{
	for (int i = 0; i < ObjectsCount; i++)
	{
		for (int x = 0; x < Console.ScreenWidth; x++)
		{
			FVector2D ObjLoc = Objects[i]->Location;

			float Distance = (ObjLoc - Player.Location).Lenght();

			//FVector2D LookDir(cosf(Player.Rotation.X), sinf(Player.Rotation.X));

			// For each Screen "pixel", calculate a ray angle
			float RayAngle = (Player.Rotation.X - Player.FOV / 2.0f) + ((float)x / (float)Console.ScreenWidth * Player.FOV);

			// Direction vector of the ray
			FVector2D LookDir(cosf(RayAngle), sinf(RayAngle));

			float Angle = acosf(LookDir * (ObjLoc - Player.Location).Normalize()) /** 57.2957795f*/;
			if (Angle > 3.14159f / 2.0f)
			{
				return;
			}
			float distFromCenter = tanf(Angle / 2.0f) * Distance * 2.0f;

			float AngleToCeiling = atanf((1.0 - Player.Height) / Distance) - Player.Rotation.Y;
			float PrecentageWallTakesOfTopHalfScreen = AngleToCeiling / (Player.vFOV / 2.0f);

			// How many radians from horizon to hit floor
			float AngleToFloor = atanf(Player.Height / Distance) + Player.Rotation.Y;
			float PrecentageWallTakesOfBottomHalfScreen = AngleToFloor / (Player.vFOV / 2.0f);

			// Calculate ceiling and floor size
			int Ceiling = Console.ScreenHeight / 2.0f - (Console.ScreenHeight / 2.0f) * PrecentageWallTakesOfTopHalfScreen;
			int Floor = Console.ScreenHeight / 2.0f + (Console.ScreenHeight / 2.0f) * PrecentageWallTakesOfBottomHalfScreen;

			if (distFromCenter < 0.05f)
			{
				if (DepthBuffer[x] < Distance)
				{
					return;
				}
				DepthBuffer[x] = Distance;
				for (int y = 0; y < Console.ScreenHeight; y++)
				{
					if (Ceiling <= y && y <= Floor)
					{
						FVector2D UV;

						float RayAngle2 = 3.14159f / 2.0f + Player.Rotation.Y - (y - Console.ScreenHeight / 2.0f) / (Console.ScreenHeight / 2.0f) * (Player.vFOV / 2.0f);
						float PointHeight = 1.0f - Player.Height + Distance / tanf(RayAngle2);
						UV.Y = PointHeight;
						UV.X = distFromCenter / 0.05f;

						short Texture = DrawTexture(UV, true);
						if (Texture != 0x0000)
						{
							Console.Screen[y * Console.ScreenWidth + x].Attributes = Objects[i]->color;//Texture;
							Console.Screen[y * Console.ScreenWidth + x].Char.UnicodeChar = 0x2588;
						}
					}
				}
			}
		}
	}
}

short DrawTexture(FVector2D UV, bool a)
{
	int X = 4, Y = 4;
	short* Texture;
	short TextureOut;

	if (a)
	{
		X = 2;
		Texture = new short[X * Y];
		Texture[0] = 0x000F;
		Texture[1] = 0x000F;
		Texture[2] = 0x000F;
		Texture[3] = 0x0000;
		Texture[4] = 0x0000;
		Texture[5] = 0x0000;
		Texture[6] = 0x0000;
		Texture[7] = 0x0000;
	}
	else
	{
		Texture = new short[X * Y];
		Texture[0] = 0x000F;
		Texture[1] = 0x0007;
		Texture[2] = 0x000F;
		Texture[3] = 0x0007;
		Texture[4] = 0x0007;
		Texture[5] = 0x000F;
		Texture[6] = 0x0007;
		Texture[7] = 0x000F;
		Texture[8] = 0x000F;
		Texture[9] = 0x0007;
		Texture[10] = 0x000F;
		Texture[11] = 0x0007;
		Texture[12] = 0x0007;
		Texture[13] = 0x000F;
		Texture[14] = 0x0007;
		Texture[15] = 0x000F;
	}

	TextureOut = Texture[(int)(UV.X * X) + (int)(UV.Y * Y) * X];

	delete[] Texture;

	//DEBUG UVs
	if (Debug)
	{
		char c = '0' + (int)(UV.Y / 0.1f);
		return c;
	}



	return TextureOut;
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