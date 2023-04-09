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