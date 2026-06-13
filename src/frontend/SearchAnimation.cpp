#include "SearchAnimation.h"

#include <algorithm>

void SearchAnimation::clear()
{
    result = SearchResult{};
    active = false;
    visibleWave = 0;
    visibleStepCount = 0;
    elapsed = 0.0f;
    visitedNodes.clear();
    visitedEdges.clear();
    finalPathNodes.clear();
    finalPathEdges.clear();
}

void SearchAnimation::start(const SearchResult& newResult)
{
    result = newResult;
    active = true;
    visibleWave = 0;
    visibleStepCount = 0;
    elapsed = 0.0f;

    visitedNodes.clear();
    visitedEdges.clear();
    finalPathNodes.clear();
    finalPathEdges.clear();

    for (const SearchStep& step : result.steps)
    {
        if (step.wave != 0)
        {
            continue;
        }

        ++visibleStepCount;

        if (step.fromNode >= 0)
        {
            visitedNodes.insert(step.fromNode);
        }

        if (step.toNode >= 0)
        {
            visitedNodes.insert(step.toNode);
        }

        if (step.fromNode >= 0 && step.toNode >= 0 && step.fromNode != step.toNode)
        {
            visitedEdges.insert(makeEdgeKey(step.fromNode, step.toNode));
        }
    }
}

void SearchAnimation::update(float deltaSeconds)
{
    if (!active)
    {
        return;
    }

    int maxWave = 0;
    for (const SearchStep& step : result.steps)
    {
        maxWave = std::max(maxWave, step.wave);
    }

    if (visibleWave >= maxWave)
    {
        return;
    }

    elapsed += deltaSeconds;

    while (elapsed >= secondsPerStep && visibleWave < maxWave)
    {
        elapsed -= secondsPerStep;
        ++visibleWave;
    }

    rebuildHighlights();
}

bool SearchAnimation::hasResult() const
{
    return active;
}

bool SearchAnimation::isNodeVisited(int nodeId) const
{
    return visitedNodes.find(nodeId) != visitedNodes.end();
}

bool SearchAnimation::isEdgeVisited(int nodeAId, int nodeBId) const
{
    return visitedEdges.find(makeEdgeKey(nodeAId, nodeBId)) != visitedEdges.end();
}

bool SearchAnimation::isNodeOnFinalPath(int nodeId) const
{
    return finalPathNodes.find(nodeId) != finalPathNodes.end();
}

bool SearchAnimation::isEdgeOnFinalPath(int nodeAId, int nodeBId) const
{
    return finalPathEdges.find(makeEdgeKey(nodeAId, nodeBId)) != finalPathEdges.end();
}

const SearchResult& SearchAnimation::getResult() const
{
    return result;
}

int SearchAnimation::getVisibleStepCount() const
{
    return visibleStepCount;
}

std::pair<int, int> SearchAnimation::makeEdgeKey(int nodeAId, int nodeBId)
{
    if (nodeAId > nodeBId)
    {
        std::swap(nodeAId, nodeBId);
    }

    return { nodeAId, nodeBId };
}

void SearchAnimation::rebuildHighlights()
{
    visitedNodes.clear();
    visitedEdges.clear();
    finalPathNodes.clear();
    finalPathEdges.clear();

    visibleStepCount = 0;

    for (int i = 0; i < static_cast<int>(result.steps.size()); ++i)
    {
        const SearchStep& step = result.steps[i];

        if (step.wave > visibleWave)
        {
            continue;
        }

        ++visibleStepCount;

        if (step.fromNode >= 0)
        {
            visitedNodes.insert(step.fromNode);
        }

        if (step.toNode >= 0)
        {
            visitedNodes.insert(step.toNode);
        }

        if (step.fromNode >= 0 && step.toNode >= 0 && step.fromNode != step.toNode)
        {
            visitedEdges.insert(makeEdgeKey(step.fromNode, step.toNode));
        }
    }

    const bool animationFinished = visibleStepCount >= static_cast<int>(result.steps.size());
    if (!result.success || !animationFinished)
    {
        return;
    }

    for (int nodeId : result.path)
    {
        finalPathNodes.insert(nodeId);
    }

    for (std::size_t i = 1; i < result.path.size(); ++i)
    {
        finalPathEdges.insert(makeEdgeKey(result.path[i - 1], result.path[i]));
    }
}
