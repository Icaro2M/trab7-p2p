#include "P2PNetwork.h"

#include <algorithm>
#include <utility>

P2PNetwork::P2PNetwork(
    std::vector<NetworkNode> networkNodes,
    std::vector<Edge> edges,
    std::map<int, std::vector<int>> adjacency
)
    : networkNodes(std::move(networkNodes)),
    edges(std::move(edges)),
    adjacency(std::move(adjacency))
{
    for (const auto& networkNode : this->networkNodes)
    {
        this->adjacency.try_emplace(networkNode.node.getId());
    }

    if (this->adjacency.empty())
    {
        for (const auto& edge : this->edges)
        {
            this->adjacency[edge.nodeAId].push_back(edge.nodeBId);
            this->adjacency[edge.nodeBId].push_back(edge.nodeAId);
        }
    }
}

const std::vector<NetworkNode>& P2PNetwork::getNetworkNodes() const
{
    return networkNodes;
}

const std::vector<Edge>& P2PNetwork::getEdges() const
{
    return edges;
}

const std::map<int, std::vector<int>>& P2PNetwork::getAdjacency() const
{
    return adjacency;
}

bool P2PNetwork::addNetworkNode(const NetworkNode& networkNode)
{
    int id = networkNode.node.getId();

    if (hasNode(id))
    {
        return false;
    }

    networkNodes.push_back(networkNode);
    adjacency.try_emplace(id);

    return true;
}

bool P2PNetwork::addEdge(const Edge& edge)
{
    if (edge.isSelfLoop())
    {
        return false;
    }

    if (!hasNode(edge.nodeAId) || !hasNode(edge.nodeBId))
    {
        return false;
    }

    auto& neighborsA = adjacency[edge.nodeAId];

    if (std::find(neighborsA.begin(), neighborsA.end(), edge.nodeBId) != neighborsA.end())
    {
        return false;
    }

    edges.push_back(edge);
    adjacency[edge.nodeAId].push_back(edge.nodeBId);
    adjacency[edge.nodeBId].push_back(edge.nodeAId);

    return true;
}

bool P2PNetwork::hasNode(int id) const
{
    return getNodeById(id) != nullptr;
}

Node* P2PNetwork::getNodeById(int id)
{
    for (auto& networkNode : networkNodes)
    {
        if (networkNode.node.getId() == id)
        {
            return &networkNode.node;
        }
    }

    return nullptr;
}

const Node* P2PNetwork::getNodeById(int id) const
{
    for (const auto& networkNode : networkNodes)
    {
        if (networkNode.node.getId() == id)
        {
            return &networkNode.node;
        }
    }

    return nullptr;
}

NetworkNode* P2PNetwork::getNetworkNodeById(int id)
{
    for (auto& networkNode : networkNodes)
    {
        if (networkNode.node.getId() == id)
        {
            return &networkNode;
        }
    }

    return nullptr;
}

const NetworkNode* P2PNetwork::getNetworkNodeById(int id) const
{
    for (const auto& networkNode : networkNodes)
    {
        if (networkNode.node.getId() == id)
        {
            return &networkNode;
        }
    }

    return nullptr;
}

const std::vector<int>& P2PNetwork::getNeighbors(int nodeId) const
{
    static const std::vector<int> emptyNeighbors;

    auto it = adjacency.find(nodeId);

    if (it == adjacency.end())
    {
        return emptyNeighbors;
    }

    return it->second;
}