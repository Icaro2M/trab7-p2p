#pragma once

#include "../network/P2PNetwork.h"
#include "SearchRequest.h"
#include "SearchResult.h"

#include <string>

class SearchEngine
{
public:
    SearchResult execute(P2PNetwork& network, const SearchRequest& request) const;

    static bool parseAlgorithm(
        const std::string& algorithmText,
        SearchAlgorithmType& algorithm
    );
};