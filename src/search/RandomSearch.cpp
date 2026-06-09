#include "RandomSearch.h"

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
        return result;
    }

    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());

    std::set<int> involvedNodes;

    int currentNodeId = sourceNodeId;
    int currentTTL = ttl;

    result.path.push_back(currentNodeId);
    result.visitedNodes.push_back(currentNodeId);
    involvedNodes.insert(currentNodeId);

    while (true)
    {
        Node* currentNode = network.getNodeById(currentNodeId);

        if (currentNode == nullptr)
        {
            break;
        }

        if (currentNode->hasResource(requestedId))
        {
            result.success = true;
            result.foundNode = currentNodeId;
            result.informedByNode = currentNodeId;
            result.remainingTTL = currentTTL;

            updateCacheForNodes(network, result.path, requestedId, currentNodeId);

            break;
        }

        if (useCache && currentNode->hasCachedResource(requestedId))
        {
            int ownerNodeId = currentNode->getCachedOwner(requestedId);

            result.success = true;
            result.cacheHit = true;
            result.foundNode = ownerNodeId;
            result.informedByNode = currentNodeId;
            result.remainingTTL = currentTTL;

            updateCacheForNodes(network, result.path, requestedId, ownerNodeId);

            break;
        }

        if (currentTTL <= 0)
        {
            result.remainingTTL = 0;
            break;
        }

        const auto& neighbors = network.getNeighbors(currentNodeId);

        if (neighbors.empty())
        {
            result.remainingTTL = currentTTL;
            break;
        }

        std::uniform_int_distribution<int> distribution(
            0,
            static_cast<int>(neighbors.size()) - 1
        );

        int nextNodeId = neighbors[distribution(generator)];

        result.messageCount++;
        currentTTL--;

        currentNodeId = nextNodeId;

        result.path.push_back(currentNodeId);
        result.visitedNodes.push_back(currentNodeId);
        involvedNodes.insert(currentNodeId);
    }

    result.remainingTTL = currentTTL;
    result.involvedNodesCount = static_cast<int>(involvedNodes.size());

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