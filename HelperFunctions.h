#pragma once
#include <utility>

int clamp(int value, int minimum, int maximum)
{
	return max(min(value, maximum), minimum);
}

float clamp(float value, float minimum, float maximum)
{
	return max(min(value, maximum), minimum);
}