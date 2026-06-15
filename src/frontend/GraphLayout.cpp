#include "GraphLayout.h"

#include <algorithm>
#include <limits>

void GraphLayout::update(const P2PNetwork& network, const ImVec2& origin, const ImVec2& size)
{
    nodes.clear();

    const auto& networkNodes = network.getNetworkNodes();
    if (networkNodes.empty())
    {
        return;
    }

    const ImVec2 center(origin.x + size.x * 0.5f, origin.y + size.y * 0.5f);

    nodes.reserve(networkNodes.size());

    if (networkNodes.size() == 1)
    {
        VisualNode visualNode;
        visualNode.nodeId = networkNodes.front().node.getId();
        visualNode.position = center;
        nodes.push_back(visualNode);
        return;
    }

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (const auto& networkNode : networkNodes)
    {
        minX = std::min(minX, networkNode.position.x);
        minY = std::min(minY, networkNode.position.y);
        maxX = std::max(maxX, networkNode.position.x);
        maxY = std::max(maxY, networkNode.position.y);
    }

    const float padding = 54.0f;
    const float layoutWidth = std::max(1.0f, maxX - minX);
    const float layoutHeight = std::max(1.0f, maxY - minY);
    const float usableWidth = std::max(1.0f, size.x - padding * 2.0f);
    const float usableHeight = std::max(1.0f, size.y - padding * 2.0f);
    const float scale = std::min(usableWidth / layoutWidth, usableHeight / layoutHeight);
    const float scaledWidth = layoutWidth * scale;
    const float scaledHeight = layoutHeight * scale;
    const float offsetX = origin.x + (size.x - scaledWidth) * 0.5f;
    const float offsetY = origin.y + (size.y - scaledHeight) * 0.5f;

    for (const auto& networkNode : networkNodes)
    {
        VisualNode visualNode;
        visualNode.nodeId = networkNode.node.getId();
        visualNode.position = ImVec2(
            offsetX + (networkNode.position.x - minX) * scale,
            offsetY + (networkNode.position.y - minY) * scale
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
