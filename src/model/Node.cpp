#include "Node.h"

Node::Node(int id, std::vector<Resource> resources)
    : id(id),
    resources(std::move(resources))
{
}

int Node::getId() const
{
    return id;
}

const std::vector<Resource>& Node::getResources() const
{
    return resources;
}

Result Node::getResource(const std::string& resourceId) const
{
    Result result;

    for (const auto& resource : resources)
    {
        if (resource.id == resourceId)
        {
            result.success = true;
            result.content = resource;
            return result;
        }
    }

    return result;
}

bool Node::hasResource(const std::string& resourceId) const
{
    return getResource(resourceId).success;
}

void Node::updateCache(const std::string& resourceId, int ownerNodeId)
{
    cache[resourceId] = ownerNodeId;
}

bool Node::hasCachedResource(const std::string& resourceId) const
{
    return cache.find(resourceId) != cache.end();
}

int Node::getCachedOwner(const std::string& resourceId) const
{
    auto it = cache.find(resourceId);

    if (it == cache.end())
    {
        return -1;
    }

    return it->second;
}