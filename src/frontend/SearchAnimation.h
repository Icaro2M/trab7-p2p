#pragma once

#include "../search/SearchResult.h"

#include <set>
#include <utility>

class SearchAnimation
{
public:
    void clear();
    void start(const SearchResult& result);
    void update(float deltaSeconds);

    bool hasResult() const;
    bool isNodeVisited(int nodeId) const;
    bool isEdgeVisited(int nodeAId, int nodeBId) const;
    bool isNodeOnFinalPath(int nodeId) const;
    bool isEdgeOnFinalPath(int nodeAId, int nodeBId) const;

    const SearchResult& getResult() const;
    int getVisibleStepCount() const;

private:
    static std::pair<int, int> makeEdgeKey(int nodeAId, int nodeBId);
    void rebuildHighlights();

    SearchResult result;
    bool active = false;
    int visibleWave = 0;
    int visibleStepCount = 0;
    float elapsed = 0.0f;
    float secondsPerStep = 0.35f;

    std::set<int> visitedNodes;
    std::set<std::pair<int, int>> visitedEdges;
    std::set<int> finalPathNodes;
    std::set<std::pair<int, int>> finalPathEdges;
};
