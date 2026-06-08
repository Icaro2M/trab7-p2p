#include "SearchEngine.h"

#include "Flooding.h"
#include "RandomSearch.h"

SearchResult SearchEngine::execute(P2PNetwork& network, const SearchRequest& request) const
{
    switch (request.algorithm)
    {
    case SearchAlgorithmType::Flooding:
    case SearchAlgorithmType::InformedFlooding:
    {
        Flooding flooding;
        return flooding.search(
            network,
            request.sourceNodeId,
            request.resourceId,
            request.ttl
        );
    }

    case SearchAlgorithmType::RandomWalk:
    case SearchAlgorithmType::InformedRandomWalk:
    {
        RandomSearch randomSearch;
        return randomSearch.search(
            network,
            request.sourceNodeId,
            request.resourceId,
            request.ttl
        );
    }

    default:
    {
        SearchResult result;
        result.resourceId = request.resourceId;
        return result;
    }
    }
}

bool SearchEngine::parseAlgorithm(
    const std::string& algorithmText,
    SearchAlgorithmType& algorithm
)
{
    if (algorithmText == "flooding")
    {
        algorithm = SearchAlgorithmType::Flooding;
        return true;
    }

    if (algorithmText == "informed_flooding")
    {
        algorithm = SearchAlgorithmType::InformedFlooding;
        return true;
    }

    if (algorithmText == "random_walk")
    {
        algorithm = SearchAlgorithmType::RandomWalk;
        return true;
    }

    if (algorithmText == "informed_random_walk")
    {
        algorithm = SearchAlgorithmType::InformedRandomWalk;
        return true;
    }

    return false;
}