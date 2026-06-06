#pragma once

#include "NetworkNode.h"
#include <cmath>

struct Edge
{
	NetworkNode n1;
	NetworkNode n2;

	float distance = sqrt(
		pow((n1.position.x - n2.position.x), 2)
		+
		pow((n1.position.y - n2.position.y), 2)
	)
		
};