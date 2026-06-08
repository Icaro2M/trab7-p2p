#pragma once

#include "Edge.h"
#include "NetworkNode.h"

#include <map>
#include <vector>

class P2PNetwork
{
private:
    std::vector<NetworkNode> networkNodes;
    std::vector<Edge> edges;
    std::map<int, std::vector<int>> adjacency;

public:
    P2PNetwork() = default;

    P2PNetwork(
        std::vector<NetworkNode> networkNodes,
        std::vector<Edge> edges,
        std::map<int, std::vector<int>> adjacency = {}
    );

    const std::vector<NetworkNode>& getNetworkNodes() const;
    const std::vector<Edge>& getEdges() const;
    const std::map<int, std::vector<int>>& getAdjacency() const;

    bool addNetworkNode(const NetworkNode& networkNode);
    bool addEdge(const Edge& edge);

    bool hasNode(int id) const;

    Node* getNodeById(int id);
    const Node* getNodeById(int id) const;

    NetworkNode* getNetworkNodeById(int id);
    const NetworkNode* getNetworkNodeById(int id) const;

    const std::vector<int>& getNeighbors(int nodeId) const;
};