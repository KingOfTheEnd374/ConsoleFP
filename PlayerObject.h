#pragma once

#include "Object.h"

class PlayerObject : public Object
{
public:
	FVector2D Rotation;
	
	float Sensitivity = 1.8f;
	float MouseSensitivity = 0.3f;
	// not working properly
	float ViewDistance = 32.0f;
	float Height = 0.8f;

	float FOV = 3.14159f / 2.0f;
	// Will be itialized based on screen resolution
	float vFOV = 0.0f;/* = 3.14159f / 3.39f;*/

	float Speed = 5.0f;
	float Radius = 0.25f;

	PlayerObject(FVector2D Loc, float Rot = 0.0f) : Object(Loc)
	{
		Rotation.X = Rot;
	}
};