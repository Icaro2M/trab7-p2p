#pragma once

#include "../model/Node.h"

struct Position
{
	float x, y, z;
};

struct NetworkNode
{
	Position position;
	Node* node;
};