#include "Flooding.h"

#include <algorithm>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace
{
    struct Delivery
    {
        int fromNodeId = -1;
        int toNodeId = -1;
        int ttlBefore = 0;
        int ttlAfter = 0;
    };
}

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

    std::set<int> visited;
    std::map<int, int> parent;
    std::vector<std::pair<int, int>> frontier;

    frontier.push_back({ sourceNodeId, ttl });
    visited.insert(sourceNodeId);

    result.visitedNodes.push_back(sourceNodeId);

    result.steps.push_back({
        SearchStepType::VisitNode,
        -1,
        sourceNodeId,
        ttl,
        ttl,
        false,
        false,
        false,
        0
        });

    if (sourceNode->hasResource(requestedId))
    {
        result.success = true;
        result.foundNode = sourceNodeId;
        result.informedByNode = sourceNodeId;
        result.path.push_back(sourceNodeId);
        result.involvedNodesCount = 1;
        result.remainingTTL = ttl;

        result.steps.push_back({
            SearchStepType::ResourceFound,
            sourceNodeId,
            sourceNodeId,
            ttl,
            ttl,
            true,
            false,
            false,
            0
            });

        result.steps.push_back({
            SearchStepType::SearchFinished,
            -1,
            sourceNodeId,
            ttl,
            ttl,
            true,
            false,
            false,
            1
            });

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

        result.steps.push_back({
            SearchStepType::CacheHit,
            sourceNodeId,
            sourceNodeId,
            ttl,
            ttl,
            false,
            true,
            false,
            0
            });

        result.steps.push_back({
            SearchStepType::SearchFinished,
            -1,
            sourceNodeId,
            ttl,
            ttl,
            true,
            true,
            false,
            1
            });

        return result;
    }

    int wave = 1;

    while (!frontier.empty())
    {
        std::set<int> receivedThisWave;
        std::vector<Delivery> deliveries;
        std::vector<std::pair<int, int>> nextFrontier;

        for (const auto& item : frontier)
        {
            const auto [currentNodeId, currentTTL] = item;

            if (currentTTL <= 0)
            {
                continue;
            }

            const auto& neighbors = network.getNeighbors(currentNodeId);

            for (int neighborId : neighbors)
            {
                result.messageCount++;

                int nextTTL = currentTTL - 1;
                bool wasAlreadyVisited = visited.find(neighborId) != visited.end() ||
                    receivedThisWave.find(neighborId) != receivedThisWave.end();

                result.steps.push_back({
                    SearchStepType::SendMessage,
                    currentNodeId,
                    neighborId,
                    currentTTL,
                    nextTTL,
                    false,
                    false,
                    wasAlreadyVisited,
                    wave
                    });

                if (wasAlreadyVisited)
                {
                    continue;
                }

                receivedThisWave.insert(neighborId);
                deliveries.push_back({ currentNodeId, neighborId, currentTTL, nextTTL });
            }
        }

        for (const Delivery& delivery : deliveries)
        {
            visited.insert(delivery.toNodeId);
            parent[delivery.toNodeId] = delivery.fromNodeId;

            result.visitedNodes.push_back(delivery.toNodeId);

            result.steps.push_back({
                SearchStepType::VisitNode,
                delivery.fromNodeId,
                delivery.toNodeId,
                delivery.ttlBefore,
                delivery.ttlAfter,
                false,
                false,
                false,
                wave
                });

            Node* neighbor = network.getNodeById(delivery.toNodeId);

            if (neighbor == nullptr)
            {
                continue;
            }

            if (neighbor->hasResource(requestedId))
            {
                result.steps.push_back({
                    SearchStepType::ResourceFound,
                    delivery.fromNodeId,
                    delivery.toNodeId,
                    delivery.ttlBefore,
                    delivery.ttlAfter,
                    true,
                    false,
                    false,
                    wave
                    });

                if (!result.success)
                {
                    result.success = true;
                    result.foundNode = delivery.toNodeId;
                    result.informedByNode = delivery.toNodeId;
                    result.remainingTTL = delivery.ttlAfter;
                    result.path = buildPath(sourceNodeId, delivery.toNodeId, parent);
                }

                continue;
            }

            if (useCache && neighbor->hasCachedResource(requestedId))
            {
                int ownerNodeId = neighbor->getCachedOwner(requestedId);

                result.steps.push_back({
                    SearchStepType::CacheHit,
                    delivery.fromNodeId,
                    delivery.toNodeId,
                    delivery.ttlBefore,
                    delivery.ttlAfter,
                    false,
                    true,
                    false,
                    wave
                    });

                if (!result.success)
                {
                    result.success = true;
                    result.cacheHit = true;
                    result.foundNode = ownerNodeId;
                    result.informedByNode = delivery.toNodeId;
                    result.remainingTTL = delivery.ttlAfter;
                    result.path = buildPath(sourceNodeId, delivery.toNodeId, parent);
                }

                continue;
            }

            nextFrontier.push_back({ delivery.toNodeId, delivery.ttlAfter });
        }

        frontier = std::move(nextFrontier);
        ++wave;
    }

    result.involvedNodesCount = static_cast<int>(visited.size());

    if (result.success)
    {
        updateCacheForNodes(network, result.path, requestedId, result.foundNode);
    }
    else
    {
        result.remainingTTL = 0;
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
        wave
        });

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
