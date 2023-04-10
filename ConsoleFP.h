#pragma once

#include "DataTypes.h"

void EventTick(float DeltaT);

void HandleInput();

void HandleCollision(FVector2D& MovementVector);

void CalculatePixels();

void TraceForWalls(FVector2D LookDir, Hit& HitData, bool& Boundary);

Hit LineTrace(FVector2D Start, FVector2D End);

void CalculateShading(Hit HitData, int x, bool bBoundary);

void DrawUI();