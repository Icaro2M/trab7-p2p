#pragma once

#include "P2PNetwork.h"

#include <string>

struct NetworkLoadResult
{
    bool success = false;
    std::string errorMessage;

    int numNodes = 0;
    int minNeighbors = 0;
    int maxNeighbors = 0;

    P2PNetwork network;
};