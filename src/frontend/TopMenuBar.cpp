#include "TopMenuBar.h"

#include "App.h"

#include "imgui.h"

#include <array>
#include <optional>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>

namespace
{
std::optional<std::string> openNetworkFileDialog()
{
    std::array<char, 512> fileName{};

    OPENFILENAMEA openFileName{};
    openFileName.lStructSize = sizeof(openFileName);
    openFileName.lpstrFile = fileName.data();
    openFileName.nMaxFile = static_cast<DWORD>(fileName.size());
    openFileName.lpstrFilter = "Arquivos de texto (*.txt)\0*.txt\0Todos os arquivos (*.*)\0*.*\0";
    openFileName.nFilterIndex = 1;
    openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (!GetOpenFileNameA(&openFileName))
    {
        return std::nullopt;
    }

    return fileName.data();
}
}

void TopMenuBar::render(App& app)
{
    if (!ImGui::BeginMainMenuBar())
    {
        return;
    }

    ImGui::TextUnformatted("Arquivo");
    ImGui::SetNextItemWidth(620.0f);
    ImGui::BeginDisabled();
    ImGui::InputText("##config-path", app.getPathBuffer(), app.getPathBufferSize());
    ImGui::EndDisabled();

    if (ImGui::Button("Abrir..."))
    {
        if (const auto selectedPath = openNetworkFileDialog())
        {
            app.setPath(*selectedPath);
        }
    }

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
