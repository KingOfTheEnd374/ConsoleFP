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

	float WalkHeight = 0.8f;
	float CrouchHeight = 0.3f;
	float Height = WalkHeight;

	float FOV = 3.14159f / 2.0f;
	// Will be itialized based on screen resolution
	float vFOV = 0.0f;

	float WalkSpeed = 5.0f;
	float CrouchSpeed = 1.5f;
	float Speed = WalkSpeed;

	float Radius = 0.25f;

	PlayerObject(FVector2D Loc, float Rot = 0.0f) : Object(Loc)
	{
		Rotation.X = Rot;
	}

	void Crouch()
	{
		Height = CrouchHeight;
		Speed = CrouchSpeed;
	}

	void UnCrouch()
	{
		Height = WalkHeight;
		Speed = WalkSpeed;
	}
};