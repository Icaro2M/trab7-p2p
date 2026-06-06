#pragma once

#include "../network/P2PNetwork.h"
#include "SearchResult.h"

class Search
{
public:

	virtual SearchResult search(
		P2PNetwork* network,
		int sourceNodeId,
		std::string requestedId,
		int TTL
	);
};