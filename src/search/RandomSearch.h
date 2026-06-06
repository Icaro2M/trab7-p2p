#pragma once

#include "Search.h"
#include "SearchResult.h"

class RandomSearch : public Search
{
	SearchResult search(
		P2PNetwork* network,
		int sourceNodeId,
		std::string requestedId,
		int TTL
	) override;

private:
	SearchResult randomSearch(
		P2PNetwork* network,
		int sourceNodeId,
		std::string requestedId,
		int TTL
	);
};