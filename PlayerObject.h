#pragma once

#include "Object.h"

class PlayerObject : public Object
{
public:
	FVector2D Rotation;
	float FOV = 3.14159f / 2.0f;
	float Sensitivity = 1.8f;
	float ViewDistance = 32.0f;

	float Heigth = 0.5f; // Do not change
	float vFOV = 3.14159f / 4.0f;

	float Speed = 5.0f;
	float Radius = 0.25f;

	PlayerObject(FVector2D Loc, float Rot = 0.0f) : Object(Loc)
	{
		Rotation.X = Rot;
	}
};