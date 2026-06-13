#pragma once

#include <string>
#include <vector>

enum class SearchStepType
{
    VisitNode,
    SendMessage,
    ResourceFound,
    CacheHit,
    SearchFinished
};

struct SearchStep
{
    SearchStepType type = SearchStepType::SearchFinished;

    int fromNode = -1;
    int toNode = -1;

    int ttlBefore = 0;
    int ttlAfter = 0;

    bool resourceFound = false;
    bool cacheHit = false;
    bool alreadyVisited = false;

    int wave = 0;
};

struct SearchResult
{
    std::string resourceId;

    bool success = false;
    bool cacheHit = false;

    std::vector<int> path;
    std::vector<int> visitedNodes;
    std::vector<SearchStep> steps;

    int foundNode = -1;
    int informedByNode = -1;

    int remainingTTL = 0;
    int messageCount = 0;
    int involvedNodesCount = 0;
};
