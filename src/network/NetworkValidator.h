#pragma once

#include "NetworkValidationResult.h"
#include "P2PNetwork.h"

class NetworkValidator
{
public:
    NetworkValidationResult validate(
        const P2PNetwork& network,
        int expectedNodeCount,
        int minNeighbors,
        int maxNeighbors
    ) const;

private:
    void validateNodeCount(
        const P2PNetwork& network,
        int expectedNodeCount,
        NetworkValidationResult& result
    ) const;

    void validateSelfLoops(
        const P2PNetwork& network,
        NetworkValidationResult& result
    ) const;

    void validateNeighborLimits(
        const P2PNetwork& network,
        int minNeighbors,
        int maxNeighbors,
        NetworkValidationResult& result
    ) const;

    void validateNodeResources(
        const P2PNetwork& network,
        NetworkValidationResult& result
    ) const;

    void validateConnectivity(
        const P2PNetwork& network,
        NetworkValidationResult& result
    ) const;
};