#pragma once

#include "Search.h"

#include <map>
#include <vector>

class Flooding : public Search
{
public:
    SearchResult search(
        P2PNetwork& network,
        int sourceNodeId,
        const std::string& requestedId,
        int ttl,
        bool useCache
    ) override;

private:
    std::vector<int> buildPath(
        int sourceNodeId,
        int foundNodeId,
        const std::map<int, int>& parent
    ) const;

    void updateCacheForNodes(
        P2PNetwork& network,
        const std::vector<int>& nodeIds,
        const std::string& resourceId,
        int ownerNodeId
    ) const;
};