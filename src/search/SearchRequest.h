#pragma once

#include <string>

enum class SearchAlgorithmType
{
    Flooding,
    InformedFlooding,
    RandomWalk,
    InformedRandomWalk
};

struct SearchRequest
{
    int sourceNodeId = -1;
    std::string resourceId;
    int ttl = 0;
    SearchAlgorithmType algorithm = SearchAlgorithmType::Flooding;

    bool useCache() const
    {
        return algorithm == SearchAlgorithmType::InformedFlooding ||
            algorithm == SearchAlgorithmType::InformedRandomWalk;
    }
};