#pragma once

#include "Search.h"

class RandomSearch : public Search
{
public:
    SearchResult search(
        P2PNetwork& network,
        int sourceNodeId,
        const std::string& requestedId,
        int ttl
    ) override;
};