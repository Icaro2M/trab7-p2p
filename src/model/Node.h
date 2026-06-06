#pragma once 

#include "Resource.h"
#include "Result.h"

#include <vector>

class Node
{
	
private:
	const int id;
	std::vector<Resource> resources;

public:

	Node(
		int id,
		std::vector<Resource> resources
	);

	const int getId() const;
	const std::vector<Resource>& getResources() const;

	Result getResource(std::string& id);
};