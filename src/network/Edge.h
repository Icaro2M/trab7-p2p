#pragma once

struct Edge
{
    int nodeAId = -1;
    int nodeBId = -1;
    float distance = 0.0f;

    Edge() = default;

    Edge(int nodeAId, int nodeBId, float distance = 0.0f)
        : nodeAId(nodeAId),
        nodeBId(nodeBId),
        distance(distance)
    {
    }

    bool isSelfLoop() const
    {
        return nodeAId == nodeBId;
    }

    bool connects(int nodeId) const
    {
        return nodeAId == nodeId || nodeBId == nodeId;
    }

    int other(int nodeId) const
    {
        if (nodeAId == nodeId)
        {
            return nodeBId;
        }

        if (nodeBId == nodeId)
        {
            return nodeAId;
        }

        return -1;
    }
};