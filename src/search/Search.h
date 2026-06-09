#pragma once

#include "../network/P2PNetwork.h"
#include "SearchResult.h"

#include <string>

class Search
{
public:
    virtual ~Search() = default;

    virtual SearchResult search(
        P2PNetwork& network,
        int sourceNodeId,
        const std::string& requestedId,
        int ttl,
        bool useCache
    ) = 0;
};