#include "TopMenuBar.h"

#include "App.h"

#include "imgui.h"

void TopMenuBar::render(App& app)
{
    if (!ImGui::BeginMainMenuBar())
    {
        return;
    }

    ImGui::TextUnformatted("Arquivo");
    ImGui::SetNextItemWidth(620.0f);
    ImGui::InputText("##config-path", app.getPathBuffer(), app.getPathBufferSize());

    if (ImGui::Button("Carregar"))
    {
        app.loadNetworkFromCurrentPath();
    }

    if (ImGui::Button("Reset"))
    {
        app.reset();
    }

    ImGui::Separator();
    ImGui::TextUnformatted(app.getStatusMessage().c_str());

    ImGui::EndMainMenuBar();
}
