#include "InspectorPanel.h"

#include "App.h"

#include "../model/Node.h"

#include "imgui.h"

#include <vector>

namespace
{
    SearchAlgorithmType algorithmFromIndex(int index)
    {
        switch (index)
        {
        case 1:
            return SearchAlgorithmType::InformedFlooding;
        case 2:
            return SearchAlgorithmType::RandomWalk;
        case 3:
            return SearchAlgorithmType::InformedRandomWalk;
        default:
            return SearchAlgorithmType::Flooding;
        }
    }
}

void InspectorPanel::render(App& app, const ImVec2& size)
{
    ImGui::BeginChild("InspectorPanel", size, true);

    ImGui::TextUnformatted("Inspetor");
    ImGui::Separator();

    if (!app.hasNetwork())
    {
        ImGui::TextWrapped("Carregue uma rede para inspecionar os nos.");
        ImGui::EndChild();
        return;
    }

    const int selectedNodeId = app.getSelectedNodeId();
    if (selectedNodeId < 0)
    {
        ImGui::TextWrapped("Selecione um no na viewport para ver detalhes e iniciar uma busca.");
        ImGui::EndChild();
        return;
    }

    const P2PNetwork& network = app.getNetwork();
    const Node* node = network.getNodeById(selectedNodeId);
    if (node == nullptr)
    {
        ImGui::TextWrapped("No selecionado nao encontrado.");
        ImGui::EndChild();
        return;
    }

    ImGui::Text("No n%d", node->getId());

    ImGui::Spacing();
    ImGui::TextUnformatted("Recursos");
    const auto& resources = node->getResources();
    if (resources.empty())
    {
        ImGui::TextUnformatted("(nenhum)");
    }
    else
    {
        for (const auto& resource : resources)
        {
            ImGui::BulletText("%s", resource.id.c_str());
        }
    }

    ImGui::Spacing();
    ImGui::TextUnformatted("Cache");
    const auto& cache = node->getCache();
    if (cache.empty())
    {
        ImGui::TextUnformatted("(vazio)");
    }
    else
    {
        for (const auto& item : cache)
        {
            ImGui::BulletText("%s -> n%d", item.first.c_str(), item.second);
        }
    }

    ImGui::Spacing();
    ImGui::TextUnformatted("Vizinhos");
    const auto& neighbors = network.getNeighbors(selectedNodeId);
    if (neighbors.empty())
    {
        ImGui::TextUnformatted("(nenhum)");
    }
    else
    {
        for (int neighborId : neighbors)
        {
            ImGui::BulletText("n%d", neighborId);
        }
    }

    ImGui::Separator();
    ImGui::TextUnformatted("Busca");

    std::vector<std::string> resourceIds = app.collectResourceIds();
    if (resourceIndex >= static_cast<int>(resourceIds.size()))
    {
        resourceIndex = 0;
    }

    const char* preview = resourceIds.empty() ? "(sem recursos)" : resourceIds[resourceIndex].c_str();
    if (ImGui::BeginCombo("Recurso", preview))
    {
        for (int i = 0; i < static_cast<int>(resourceIds.size()); ++i)
        {
            const bool selected = i == resourceIndex;
            if (ImGui::Selectable(resourceIds[i].c_str(), selected))
            {
                resourceIndex = i;
            }
            if (selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SliderInt("TTL", &ttl, 1, 20);

    const char* algorithms[] = {
        "flooding",
        "informed_flooding",
        "random_walk",
        "informed_random_walk"
    };
    ImGui::Combo("Algoritmo", &algorithmIndex, algorithms, IM_ARRAYSIZE(algorithms));

    if (ImGui::Button("Buscar") && !resourceIds.empty())
    {
        app.executeSearch(resourceIds[resourceIndex], ttl, algorithmFromIndex(algorithmIndex));
    }

    const SearchAnimation& animation = app.getAnimation();
    if (animation.hasResult())
    {
        const SearchResult& result = animation.getResult();
        const int visibleStepCount = animation.getVisibleStepCount();
        const int totalStepCount = static_cast<int>(result.steps.size());
        const int messageCount = result.messageCount;
        const int involvedNodesCount = result.involvedNodesCount;
        const int remainingTTL = result.remainingTTL;
        const bool success = result.success;
        const int foundNode = result.foundNode;

        ImGui::Separator();
        ImGui::Text("Passos exibidos: %d/%d", visibleStepCount, totalStepCount);
        ImGui::Text("Mensagens: %d", messageCount);
        ImGui::Text("Nos envolvidos: %d", involvedNodesCount);
        ImGui::Text("TTL restante: %d", remainingTTL);
        ImGui::Text("Resultado: %s", success ? "encontrado" : "nao encontrado");

        if (success)
        {
            ImGui::Text("No encontrado: n%d", foundNode);
        }
    }

    ImGui::EndChild();
}
