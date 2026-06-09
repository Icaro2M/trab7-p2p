#include "Flooding.h"

#include <algorithm>
#include <queue>
#include <set>

SearchResult Flooding::search(
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
        return result;
    }

    std::queue<std::pair<int, int>> queue;
    std::set<int> visited;
    std::map<int, int> parent;

    queue.push({ sourceNodeId, ttl });
    visited.insert(sourceNodeId);
    result.visitedNodes.push_back(sourceNodeId);

    if (sourceNode->hasResource(requestedId))
    {
        result.success = true;
        result.foundNode = sourceNodeId;
        result.informedByNode = sourceNodeId;
        result.path.push_back(sourceNodeId);
        result.involvedNodesCount = 1;
        result.remainingTTL = ttl;

        updateCacheForNodes(network, result.path, requestedId, sourceNodeId);

        return result;
    }

    if (useCache && sourceNode->hasCachedResource(requestedId))
    {
        int ownerNodeId = sourceNode->getCachedOwner(requestedId);

        result.success = true;
        result.cacheHit = true;
        result.foundNode = ownerNodeId;
        result.informedByNode = sourceNodeId;
        result.path.push_back(sourceNodeId);
        result.involvedNodesCount = 1;
        result.remainingTTL = ttl;

        return result;
    }

    while (!queue.empty())
    {
        auto [currentNodeId, currentTTL] = queue.front();
        queue.pop();

        if (currentTTL <= 0)
        {
            continue;
        }

        const auto& neighbors = network.getNeighbors(currentNodeId);

        for (int neighborId : neighbors)
        {
            result.messageCount++;

            if (visited.find(neighborId) != visited.end())
            {
                continue;
            }

            visited.insert(neighborId);
            parent[neighborId] = currentNodeId;
            result.visitedNodes.push_back(neighborId);

            Node* neighbor = network.getNodeById(neighborId);

            if (neighbor == nullptr)
            {
                continue;
            }

            int nextTTL = currentTTL - 1;

            if (neighbor->hasResource(requestedId))
            {
                result.success = true;
                result.foundNode = neighborId;
                result.informedByNode = neighborId;
                result.remainingTTL = nextTTL;
                result.path = buildPath(sourceNodeId, neighborId, parent);
                result.involvedNodesCount = static_cast<int>(visited.size());

                updateCacheForNodes(network, result.path, requestedId, neighborId);

                return result;
            }

            if (useCache && neighbor->hasCachedResource(requestedId))
            {
                int ownerNodeId = neighbor->getCachedOwner(requestedId);

                result.success = true;
                result.cacheHit = true;
                result.foundNode = ownerNodeId;
                result.informedByNode = neighborId;
                result.remainingTTL = nextTTL;
                result.path = buildPath(sourceNodeId, neighborId, parent);
                result.involvedNodesCount = static_cast<int>(visited.size());

                updateCacheForNodes(network, result.path, requestedId, ownerNodeId);

                return result;
            }

            queue.push({ neighborId, nextTTL });
        }
    }

    result.remainingTTL = 0;
    result.involvedNodesCount = static_cast<int>(visited.size());

    return result;
}

std::vector<int> Flooding::buildPath(
    int sourceNodeId,
    int foundNodeId,
    const std::map<int, int>& parent
) const
{
    std::vector<int> path;
    int current = foundNodeId;

    path.push_back(current);

    while (current != sourceNodeId)
    {
        auto it = parent.find(current);

        if (it == parent.end())
        {
            break;
        }

        current = it->second;
        path.push_back(current);
    }

    std::reverse(path.begin(), path.end());
    return path;
}

void Flooding::updateCacheForNodes(
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