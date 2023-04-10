#pragma once

#include "Object.h"

class PlayerObject : public Object
{
public:
	float Rotation = 0.0f;
	float FOV = 3.14159f / 3.0f;
	float Sensitivity = 1.2f;
	float ViewDistance = 32.0f;

	float Heigth = 0.5f;
	float vFOV = 3.14159f / 6.0f;

	float Speed = 5.0f;
	float Radius = 0.25f;

	PlayerObject(FVector2D Loc, float Rot = 0.0f) : Object(Loc)
	{
		Rotation = Rot;
	}
};