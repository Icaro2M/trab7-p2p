#pragma once

#include "Search.h"
#include "SearchResult.h"
#include <queue>

class Flooding : public Search
{
	SearchResult search(
		P2PNetwork* network,
		int sourceNodeId,
		std::string requestedId,
		int TTL
	) override;

private:
	SearchResult flooding(
		P2PNetwork* network,
		int sourceNodeId,
		std::string requestedId,
		int TTL,
		std::queue<int> nodeQueue
	);
};