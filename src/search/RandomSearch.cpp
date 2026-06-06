#include "RandomSearch.h"

SearchResult RandomSearch::search(
	P2PNetwork* network,
	int sourceNodeId,
	std::string requestedId,
	int TTL
) 
{
	SearchResult sResult;

	sResult.resourceId = requestedId;

	sResult.path = std::vector<int>{ sourceNodeId };
	sResult.visitedNodes = std::vector<int>{ sourceNodeId };

	sResult.remaningTTL = TTL;

	if (TTL == 0)
	{
		sResult.success = false;
		return sResult;
	}

	Node* sourceNode = network->getNodeById(sourceNodeId);

	Result result = sourceNode->getResource(requestedId);

	if (result.success)
	{
		sResult.success = true;
		sResult.foundNode = sourceNodeId;
		return sResult;

	}

	std::vector<int> neighbors = network->getAdjacency().at(sourceNodeId);

	for (auto id : neighbors)
	{
		if (id != sourceNodeId)
		{
			Node* neighbor = network->getNodeById(id);
			auto nResult = randomSearch(
				network,
				sourceNode->getId(),
				requestedId,
				TTL
			);

			sResult.visitedNodes.insert(
				sResult.visitedNodes.end(),
				nResult.visitedNodes.begin(),
				nResult.visitedNodes.end()
			);

			if (nResult.success)
			{

				sResult.success = true;
				sResult.foundNode = nResult.foundNode;
				sResult.remaningTTL = nResult.remaningTTL;

				sResult.path.insert(
					sResult.path.end(),
					nResult.path.begin(),
					nResult.path.end()
				);

				return sResult;
			}

		}
	}

	return sResult;
}

SearchResult RandomSearch::randomSearch(
	P2PNetwork* network,
	int sourceNodeId,
	std::string requestedId,
	int TTL
)
{
	SearchResult sResult;

	sResult.resourceId = requestedId;

	sResult.path = std::vector<int>{ sourceNodeId };
	sResult.visitedNodes = std::vector<int>{ sourceNodeId };

	sResult.remaningTTL = TTL;

	if (TTL == 0)
	{
		sResult.success = false;
		return sResult;
	}

	Node* sourceNode = network->getNodeById(sourceNodeId);

	Result result = sourceNode->getResource(requestedId);

	if (result.success)
	{
		sResult.success = true;
		sResult.foundNode = sourceNodeId;
		return sResult;

	}

	std::vector<int> neighbors = network->getAdjacency().at(sourceNodeId);

	for (auto id : neighbors)
	{
		if (id != sourceNodeId)
		{
			Node* neighbor = network->getNodeById(id);
			auto nResult = randomSearch(
				network,
				sourceNode->getId(),
				requestedId,
				TTL - 1
			);

			sResult.visitedNodes.insert(
				sResult.visitedNodes.end(),
				nResult.visitedNodes.begin(),
				nResult.visitedNodes.end()
			);

			if (nResult.success)
			{

				sResult.success = true;
				sResult.foundNode = nResult.foundNode;
				sResult.remaningTTL = nResult.remaningTTL;

				sResult.path.insert(
					sResult.path.end(),
					nResult.path.begin(),
					nResult.path.end()
				);

				return sResult;
			}

		}
	}

	return sResult;
}