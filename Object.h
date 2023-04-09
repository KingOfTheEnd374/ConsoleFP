#pragma once

#include "DataTypes.h"

class Object
{
public:
	FVector2D Location;

	Object(FVector2D Loc)
	{
		Location = Loc;
	}
};