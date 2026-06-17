#include "RandomSearch.h"

#include <algorithm>
#include <functional>
#include <random>
#include <set>

SearchResult RandomSearch::search(
    P2PNetwork& network,
    int sourceNodeId,
    const std::string& requestedId,
    int ttl,
    bool useCache
)
{
    SearchResult result;
    result.resourceId = requestedId;
    result.remainingTTL = ttl;

    Node* sourceNode = network.getNodeById(sourceNodeId);

    if (sourceNode == nullptr)
    {
        result.steps.push_back({
            SearchStepType::SearchFinished,
            -1,
            sourceNodeId,
            ttl,
            ttl,
            false,
            false,
            false,
            0
            });

        return result;
    }

    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());

    std::set<int> involvedNodes;
    std::set<int> activePathNodes;
    std::vector<int> currentPath;
    int wave = 0;

    currentPath.push_back(sourceNodeId);
    result.path = currentPath;
    result.visitedNodes.push_back(sourceNodeId);
    involvedNodes.insert(sourceNodeId);
    activePathNodes.insert(sourceNodeId);

    result.steps.push_back({
        SearchStepType::VisitNode,
        -1,
        sourceNodeId,
        ttl,
        ttl,
        false,
        false,
        false,
        wave
        });

    std::function<bool(int, int)> walk = [&](int currentNodeId, int currentTTL)
    {
        Node* currentNode = network.getNodeById(currentNodeId);

        if (currentNode == nullptr)
        {
            return false;
        }

        if (currentNode->hasResource(requestedId))
        {
            result.success = true;
            result.foundNode = currentNodeId;
            result.informedByNode = currentNodeId;
            result.remainingTTL = currentTTL;
            result.path = currentPath;

            result.steps.push_back({
                SearchStepType::ResourceFound,
                currentNodeId,
                currentNodeId,
                currentTTL,
                currentTTL,
                true,
                false,
                false,
                wave
                });

            return true;
        }

        if (useCache && currentNode->hasCachedResource(requestedId))
        {
            int ownerNodeId = currentNode->getCachedOwner(requestedId);

            result.success = true;
            result.cacheHit = true;
            result.foundNode = ownerNodeId;
            result.informedByNode = currentNodeId;
            result.remainingTTL = currentTTL;
            result.path = currentPath;

            result.steps.push_back({
                SearchStepType::CacheHit,
                currentNodeId,
                currentNodeId,
                currentTTL,
                currentTTL,
                false,
                true,
                false,
                wave
                });

            return true;
        }

        if (currentTTL <= 0)
        {
            result.remainingTTL = 0;
            return false;
        }

        std::vector<int> neighbors = network.getNeighbors(currentNodeId);
        std::shuffle(neighbors.begin(), neighbors.end(), generator);

        for (int nextNodeId : neighbors)
        {
            if (activePathNodes.find(nextNodeId) != activePathNodes.end())
            {
                continue;
            }

            int nextTTL = currentTTL - 1;
            ++wave;
            result.messageCount++;

            result.steps.push_back({
                SearchStepType::SendMessage,
                currentNodeId,
                nextNodeId,
                currentTTL,
                nextTTL,
                false,
                false,
                involvedNodes.find(nextNodeId) != involvedNodes.end(),
                wave
                });

            currentPath.push_back(nextNodeId);
            result.visitedNodes.push_back(nextNodeId);
            involvedNodes.insert(nextNodeId);
            activePathNodes.insert(nextNodeId);

            result.steps.push_back({
                SearchStepType::VisitNode,
                currentNodeId,
                nextNodeId,
                currentTTL,
                nextTTL,
                false,
                false,
                false,
                wave
                });

            if (walk(nextNodeId, nextTTL))
            {
                return true;
            }

            currentPath.pop_back();
            activePathNodes.erase(nextNodeId);
        }

        return false;
    };

    walk(sourceNodeId, ttl);

    result.involvedNodesCount = static_cast<int>(involvedNodes.size());

    if (result.success)
    {
        updateCacheForNodes(network, result.path, requestedId, result.foundNode);
    }

    result.steps.push_back({
        SearchStepType::SearchFinished,
        -1,
        result.informedByNode,
        result.remainingTTL,
        result.remainingTTL,
        result.success,
        result.cacheHit,
        false,
        wave + 1
        });

    return result;
}

void RandomSearch::updateCacheForNodes(
    P2PNetwork& network,
    const std::vector<int>& nodeIds,
    const std::string& resourceId,
    int ownerNodeId
) const
{
    for (int nodeId : nodeIds)
    {
        Node* node = network.getNodeById(nodeId);

        if (node != nullptr)
        {
            node->updateCache(resourceId, ownerNodeId);
        }
    }
}
