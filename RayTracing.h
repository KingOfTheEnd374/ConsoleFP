#pragma once

#include "DataTypes.h"

double LightData[200 * 100] = { 0.0 };

void ClearLightData()
{
	for (int i = 0; i < 100 * 200; i++)
		LightData[i] = 0.0;
}

FVector2D RandomFloorDirEndLocation(FVector2D Location, float& Pitch)
{
	float Yaw = 2.0f * 3.14159f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	Pitch = 3.14159f / 2.0f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	float dist = 1.0f / tanf(Pitch);

	return Location + FVector2D(cosf(Yaw) * dist, sinf(Yaw) * dist);
}

FVector2D RandomWallDirEndLocation(FVector2D Location, FVector2D Normal, float Height, float& Pitch)
{
	float Yaw = 2.0f * 3.14159f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	Pitch = 3.14159f / 2.0f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	float dist
	
}

short FloorRayTracing(int x, int y, FVector2D FloorLoc, unsigned long Frame, int ScreenWidth, int BounceCount = 0)
{
	float Pitch;
	FVector2D EndLoc = RandomFloorDirEndLocation(FloorLoc, Pitch);

	Hit LightRay = LineTrace(FloorLoc, EndLoc);
	if (LightRay.DidHit)
	{
		if (LightRay.Object == 's')
		{
			LightData[y * ScreenWidth + x] += 1.0;
		}
		else
		{
			float Height = tanf(Pitch) * LightRay.Distance;
			WallRayTracing(x, y, LightRay.Location, LightRay.Normal, Height, Frame, ScreenWidth, BounceCount + 1);
		}
	}
	
	double color = LightData[y * ScreenWidth + x] / Frame;
	if (color >= 0.1 && color < 1.0)
	{
		return 0x0F;
	}
	else if (color >= 0.05 && color < 0.1)
	{
		return 0x07;
	}
	else if (color >= 0.02 && color < 0.05)
	{
		return 0x08;
	}
	else
	{
		return 0x00;
	}
}

short WallRayTracing(int x, int y, FVector2D Location, FVector2D Normal, float Height, unsigned long Frame, int ScreenWidth, int BounceCount = 0)
{
	FVector2D EndLoc = RandomWallDirEndLocation();

	double color = LightData[y * ScreenWidth + x] / Frame;
	if (color >= 0.1 && color < 1.0)
	{
		return 0x0F;
	}
	else if (color >= 0.05 && color < 0.1)
	{
		return 0x07;
	}
	else if (color >= 0.02 && color < 0.05)
	{
		return 0x08;
	}
	else
	{
		return 0x00;
	}
}