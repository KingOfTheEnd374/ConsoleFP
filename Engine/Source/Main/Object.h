#pragma once

#include "DataTypes.h"

class Object
{
public:
	FVector2D Location;
	short color = 0x000F;

	Object(FVector2D Loc)
	{
		Location = Loc;
	}
};