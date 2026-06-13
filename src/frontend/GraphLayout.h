#pragma once

#include "../network/P2PNetwork.h"

#include "imgui.h"

#include <vector>

struct VisualNode
{
    int nodeId = -1;
    ImVec2 position;
};

class GraphLayout
{
public:
    void update(const P2PNetwork& network, const ImVec2& origin, const ImVec2& size);

    const std::vector<VisualNode>& getNodes() const;
    const VisualNode* findNode(int nodeId) const;

private:
    std::vector<VisualNode> nodes;
};
