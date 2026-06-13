#include "GraphViewport.h"

#include "App.h"

#include <cmath>
#include <string>

namespace
{
    float distance(const ImVec2& a, const ImVec2& b)
    {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }
}

void GraphViewport::render(App& app, const ImVec2& size)
{
    ImGui::BeginChild("GraphViewport", size, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImVec2 origin = ImGui::GetCursorScreenPos();
    const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    const ImVec2 bottomRight(origin.x + canvasSize.x, origin.y + canvasSize.y);

    drawList->AddRectFilled(origin, bottomRight, IM_COL32(24, 27, 31, 255));
    drawList->AddRect(origin, bottomRight, IM_COL32(70, 76, 84, 255));

    ImGui::InvisibleButton("graph-canvas", canvasSize, ImGuiButtonFlags_MouseButtonLeft);
    const bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
    const ImVec2 mousePos = ImGui::GetIO().MousePos;

    if (!app.hasNetwork())
    {
        drawList->AddText(ImVec2(origin.x + 18.0f, origin.y + 18.0f), IM_COL32(210, 214, 220, 255), "Carregue uma rede para visualizar o grafo.");
        ImGui::EndChild();
        return;
    }

    const P2PNetwork& network = app.getNetwork();
    const SearchAnimation& animation = app.getAnimation();
    layout.update(network, origin, canvasSize);

    for (const auto& edge : network.getEdges())
    {
        const VisualNode* a = layout.findNode(edge.nodeAId);
        const VisualNode* b = layout.findNode(edge.nodeBId);
        if (a == nullptr || b == nullptr)
        {
            continue;
        }

        ImU32 color = IM_COL32(225, 229, 235, 160);
        float thickness = 1.0f;

        if (animation.isEdgeVisited(edge.nodeAId, edge.nodeBId))
        {
            color = IM_COL32(246, 196, 76, 255);
            thickness = 2.0f;
        }

        if (animation.isEdgeOnFinalPath(edge.nodeAId, edge.nodeBId))
        {
            color = IM_COL32(72, 154, 255, 255);
            thickness = 3.0f;
        }

        drawList->AddLine(a->position, b->position, color, thickness);
    }

    const float nodeRadius = 22.0f;
    int clickedNodeId = -1;

    for (const auto& visualNode : layout.getNodes())
    {
        if (clicked && distance(mousePos, visualNode.position) <= nodeRadius)
        {
            clickedNodeId = visualNode.nodeId;
        }

        const bool selected = visualNode.nodeId == app.getSelectedNodeId();
        const bool visited = animation.isNodeVisited(visualNode.nodeId);
        const bool finalPath = animation.isNodeOnFinalPath(visualNode.nodeId);

        ImU32 fillColor = IM_COL32(35, 39, 46, 255);
        ImU32 borderColor = IM_COL32(214, 221, 230, 255);
        float borderThickness = 2.0f;

        if (visited)
        {
            borderColor = IM_COL32(246, 196, 76, 255);
            borderThickness = 3.0f;
        }

        if (finalPath)
        {
            borderColor = IM_COL32(72, 154, 255, 255);
            borderThickness = 4.0f;
        }

        if (selected)
        {
            borderColor = IM_COL32(255, 255, 255, 255);
            borderThickness = 5.0f;
        }

        drawList->AddCircleFilled(visualNode.position, nodeRadius, fillColor, 32);
        drawList->AddCircle(visualNode.position, nodeRadius, borderColor, 32, borderThickness);

        const std::string label = std::to_string(visualNode.nodeId);
        const ImVec2 labelSize = ImGui::CalcTextSize(label.c_str());
        drawList->AddText(
            ImVec2(visualNode.position.x - labelSize.x * 0.5f, visualNode.position.y - labelSize.y * 0.5f),
            IM_COL32(245, 247, 250, 255),
            label.c_str()
        );
    }

    if (clickedNodeId >= 0)
    {
        app.setSelectedNodeId(clickedNodeId);
    }

    ImGui::EndChild();
}
