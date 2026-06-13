#include "GraphLayout.h"

#include <cmath>

void GraphLayout::update(const P2PNetwork& network, const ImVec2& origin, const ImVec2& size)
{
    nodes.clear();

    const auto& networkNodes = network.getNetworkNodes();
    if (networkNodes.empty())
    {
        return;
    }

    const float pi = 3.14159265358979323846f;
    const ImVec2 center(origin.x + size.x * 0.5f, origin.y + size.y * 0.5f);
    const float radius = (size.x < size.y ? size.x : size.y) * 0.38f;
    const int count = static_cast<int>(networkNodes.size());

    nodes.reserve(networkNodes.size());

    for (int i = 0; i < count; ++i)
    {
        const float angle = -pi * 0.5f + (2.0f * pi * static_cast<float>(i) / static_cast<float>(count));

        VisualNode visualNode;
        visualNode.nodeId = networkNodes[i].node.getId();
        visualNode.position = ImVec2(
            center.x + std::cos(angle) * radius,
            center.y + std::sin(angle) * radius
        );

        nodes.push_back(visualNode);
    }
}

const std::vector<VisualNode>& GraphLayout::getNodes() const
{
    return nodes;
}

const VisualNode* GraphLayout::findNode(int nodeId) const
{
    for (const auto& node : nodes)
    {
        if (node.nodeId == nodeId)
        {
            return &node;
        }
    }

    return nullptr;
}
