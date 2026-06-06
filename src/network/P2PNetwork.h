#pragma once

#include "NetworkNode.h"
#include "Edge.h"

#include <vector>
#include <map>

class P2PNetwork
{
private:
	const std::vector<NetworkNode> networkNodes;
	const std::vector<Edge> edges;
	const std::map<int, std::vector<int>> adjacency;

public:

	P2PNetwork(
		std::vector<NetworkNode> networkNodes,
		std::vector<Edge> edges,
		std::map<int, std::vector<int>> adjacency
	);

	const std::vector<NetworkNode>& getNetworkNodes() const;
	const std::vector<Edge>& getEdges() const;
	const std::map<int, std::vector<int>>& getAdjacency() const;

	Node* getNodeById(int id);
	
};