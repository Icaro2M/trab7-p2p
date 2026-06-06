#pragma once

#include <vector>
#include <string>

struct SearchResult
{
	std::string resourceId;

	bool success = false;

	std::vector<int> path;

	std::vector<int> visitedNodes;

	int foundNode;

	int remaningTTL;

};