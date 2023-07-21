#pragma once

#include "DataTypes.h"

void EventTick(float DeltaT);

void HandleInput();

void HandleCollision(FVector2D& MovementVector);

void CalculatePixels();

Hit LineTrace(FVector2D Start, FVector2D End, bool OnlyWalls = false);

void CalculateShading(Hit HitData, int x);

short DrawTexture(FVector2D UV, bool a = false);

void WallLighting(int x, int y, Hit& HitData);

void FloorLighting(int x, int y, Hit& HitData);

void RenderObjects();

void DrawUI();