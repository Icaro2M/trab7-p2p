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
        int ttl
    ) override;

private:
    std::vector<int> buildPath(
        int sourceNodeId,
        int foundNodeId,
        const std::map<int, int>& parent
    ) const;
};