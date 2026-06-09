#pragma once

#include <string>
#include <vector>

struct SearchResult
{
    std::string resourceId;
    bool success = false;
    bool cacheHit = false;

    std::vector<int> path;
    std::vector<int> visitedNodes;

    int foundNode = -1;
    int informedByNode = -1;
    int remainingTTL = 0;

    int messageCount = 0;
    int involvedNodesCount = 0;
};