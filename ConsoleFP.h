#pragma once

#include "DataTypes.h"

void InitScreenSpace();

void EventTick(float DeltaT);

void HandleInput();

void HandleCollision(FVector2D& MovementVector);

Hit LineTrace(FVector2D Start, FVector2D End, bool OnlyWalls = false);

void CalculatePixels();

void CalculateShading(Hit HitData, int x);

int GetWallStart(Hit HitData);

int GetFloorStart(Hit HitData);

void WallLighting(int x, int y, Hit& HitData);

void FloorLighting(int x, int y, Hit& HitData);

void RenderObjects();

short DrawTexture(FVector2D UV, bool a = false);

void DrawUI();