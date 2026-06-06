#include "Node.h"

Node::Node(
	int id,
	std::vector<Resource> resources
)
	: id(id), resources(resources)
{
}

const int Node::getId() const
{
	return id;
}

const std::vector<Resource>& Node::getResources() const
{
	return resources;
}

Result Node::getResource(std::string& id)
{
	Result result;

	for (auto res : resources)
	{
		if (res.id == id)
		{
			result.content = res;
			result.success = true;
			break;
		}
	}

	return;
}
