#include "NetworkValidator.h"

#include <queue>
#include <set>
#include <string>

NetworkValidationResult NetworkValidator::validate(
    const P2PNetwork& network,
    int expectedNodeCount,
    int minNeighbors,
    int maxNeighbors
) const
{
    NetworkValidationResult result;

    validateNodeCount(network, expectedNodeCount, result);
    validateSelfLoops(network, result);
    validateNeighborLimits(network, minNeighbors, maxNeighbors, result);
    validateNodeResources(network, result);
    validateConnectivity(network, result);

    return result;
}

void NetworkValidator::validateNodeCount(
    const P2PNetwork& network,
    int expectedNodeCount,
    NetworkValidationResult& result
) const
{
    int actualNodeCount = static_cast<int>(network.getNetworkNodes().size());

    if (actualNodeCount != expectedNodeCount)
    {
        result.addError(
            "Quantidade de nos diferente de num_nodes. Esperado: " +
            std::to_string(expectedNodeCount) +
            ", encontrado: " +
            std::to_string(actualNodeCount) +
            "."
        );
    }
}

void NetworkValidator::validateSelfLoops(
    const P2PNetwork& network,
    NetworkValidationResult& result
) const
{
    for (const auto& edge : network.getEdges())
    {
        if (edge.isSelfLoop())
        {
            result.addError(
                "Aresta invalida: n" +
                std::to_string(edge.nodeAId) +
                " esta conectado a ele mesmo."
            );
        }
    }
}

void NetworkValidator::validateNeighborLimits(
    const P2PNetwork& network,
    int minNeighbors,
    int maxNeighbors,
    NetworkValidationResult& result
) const
{
    for (const auto& networkNode : network.getNetworkNodes())
    {
        int nodeId = networkNode.node.getId();
        int neighborCount = static_cast<int>(network.getNeighbors(nodeId).size());

        if (neighborCount < minNeighbors || neighborCount > maxNeighbors)
        {
            result.addError(
                "No n" +
                std::to_string(nodeId) +
                " possui " +
                std::to_string(neighborCount) +
                " vizinhos, mas o limite permitido eh [" +
                std::to_string(minNeighbors) +
                ", " +
                std::to_string(maxNeighbors) +
                "]."
            );
        }
    }
}

void NetworkValidator::validateNodeResources(
    const P2PNetwork& network,
    NetworkValidationResult& result
) const
{
    for (const auto& networkNode : network.getNetworkNodes())
    {
        if (networkNode.node.getResources().empty())
        {
            result.addError(
                "No n" +
                std::to_string(networkNode.node.getId()) +
                " nao possui recursos."
            );
        }
    }
}

void NetworkValidator::validateConnectivity(
    const P2PNetwork& network,
    NetworkValidationResult& result
) const
{
    const auto& nodes = network.getNetworkNodes();

    if (nodes.empty())
    {
        result.addError("A rede nao possui nos.");
        return;
    }

    std::set<int> visited;
    std::queue<int> queue;

    int startNodeId = nodes.front().node.getId();

    visited.insert(startNodeId);
    queue.push(startNodeId);

    while (!queue.empty())
    {
        int currentNodeId = queue.front();
        queue.pop();

        const auto& neighbors = network.getNeighbors(currentNodeId);

        for (int neighborId : neighbors)
        {
            if (visited.find(neighborId) == visited.end())
            {
                visited.insert(neighborId);
                queue.push(neighborId);
            }
        }
    }

    if (visited.size() != nodes.size())
    {
        result.addError("A rede esta particionada. Nem todos os nos sao alcancaveis.");
    }
}