#pragma once

#include "Search.h"

#include <string>
#include <vector>

class RandomSearch : public Search
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
    void updateCacheForNodes(
        P2PNetwork& network,
        const std::vector<int>& nodeIds,
        const std::string& resourceId,
        int ownerNodeId
    ) const;
};