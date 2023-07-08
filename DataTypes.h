#pragma once

#include <algorithm>

struct FVector2D
{
	float X;
	float Y;

	FVector2D(float x = 0.0f, float y = 0.0f)
	{
		X = x;
		Y = y;
	}

	float Lenght()
	{
		return sqrt(X * X + Y * Y);
	}

	FVector2D Normalize()
	{
		float len = Lenght();
		return FVector2D(X / len, Y / len);
	}

	FVector2D operator+(FVector2D const& other)
	{
		return FVector2D(this->X + other.X, this->Y + other.Y);
	}

	FVector2D operator-(FVector2D const& other)
	{
		return FVector2D(this->X - other.X, this->Y - other.Y);
	}

	// Dot product
	float operator*(FVector2D const& other)
	{
		return this->X * other.X + this->Y * other.Y;
	}

	FVector2D operator*(float const& other)
	{
		return FVector2D(this->X * other, this->Y * other);
	}

	FVector2D operator+=(FVector2D const& other)
	{
		this->X += other.X;
		this->Y += other.Y;
		return *this;
	}

	FVector2D operator-=(FVector2D const& other)
	{
		this->X -= other.X;
		this->Y -= other.Y;
		return *this;
	}
};

struct IVector2D
{
	int X;
	int Y;

	IVector2D(int x = 0, int y = 0)
	{
		X = x;
		Y = y;
	}
};

struct Hit
{
	bool DidHit = false;
	FVector2D Location;
	FVector2D Normal;
	float Distance = 0.0f;
	short Tile = ' ';
	FVector2D Start;
	FVector2D End;
};

/*enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};*/

/*if ((int)FloorLoc.Y * MapWidth + (int)FloorLoc.X >= 0 && (int)FloorLoc.Y * MapWidth + (int)FloorLoc.X < 256 && Map[(int)FloorLoc.Y * MapWidth + (int)FloorLoc.X] == 'i')
			{
				Console.Screen[y * Console.ScreenWidth + x].Attributes = 0x0004;
			}*/