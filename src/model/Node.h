#pragma once

#include "Resource.h"
#include "Result.h"

#include <map>
#include <string>
#include <vector>

class Node
{
private:
    int id;
    std::vector<Resource> resources;
    std::map<std::string, int> cache;

public:
    Node(int id, std::vector<Resource> resources);

    int getId() const;
    const std::vector<Resource>& getResources() const;

    Result getResource(const std::string& resourceId) const;
    bool hasResource(const std::string& resourceId) const;

    void updateCache(const std::string& resourceId, int ownerNodeId);
    bool hasCachedResource(const std::string& resourceId) const;
    int getCachedOwner(const std::string& resourceId) const;
};