#pragma once

#include "../model/Node.h"

#include <utility>

struct Position
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct NetworkNode
{
    Node node;
    Position position;

    NetworkNode(Node node, Position position)
        : node(std::move(node)),
        position(position)
    {
    }
};