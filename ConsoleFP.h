#pragma once

#include "DataTypes.h"

void EventTick();

void HandleInput();

void HandleCollision(FVector2D& MovementVector);

void CalculatePixels();

void TraceForWalls(FVector2D LookDir, float& DistanceToWall, bool& Boundary);

void CalculateShading(float fDistanceToWall, int x, bool bBoundary);

void DrawUI();