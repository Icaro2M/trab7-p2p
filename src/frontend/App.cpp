#include "App.h"

#include "../network/NetworkLoader.h"
#include "../network/NetworkValidator.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include <cstdio>
#include <exception>
#include <set>

App::App()
{
    const char* defaultPath = "C:/Users/icaro/unifor/computacao_distribuida/trab7-p2p/configs/rede_valida.txt";
    setPath(defaultPath);
}

int App::run()
{
    if (!glfwInit())
    {
        return 1;
    }

    const char* glslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "trab7-p2p", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.ChildRounding = 4.0f;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    loadNetworkFromCurrentPath();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        render();

        ImGui::Render();

        int displayWidth = 0;
        int displayHeight = 0;
        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(0.08f, 0.09f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void App::render()
{
    processPendingSearch();
    animation.update(ImGui::GetIO().DeltaTime);
    topMenuBar.render(*this);
    renderMainLayout();
}

bool App::loadNetworkFromCurrentPath()
{
    currentPath = pathBuffer.data();

    NetworkLoader loader;
    NetworkLoadResult loadResult = loader.loadFromFile(currentPath);

    if (!loadResult.success)
    {
        networkLoaded = false;
        selectedNodeId = -1;
        animation.clear();
        statusMessage = "Erro ao carregar rede: " + loadResult.errorMessage;
        return false;
    }

    NetworkValidator validator;
    NetworkValidationResult validationResult = validator.validate(
        loadResult.network,
        loadResult.numNodes,
        loadResult.minNeighbors,
        loadResult.maxNeighbors
    );

    if (!validationResult.valid)
    {
        networkLoaded = false;
        selectedNodeId = -1;
        animation.clear();
        statusMessage = "Rede invalida: " + validationResult.errors.front();
        return false;
    }

    network = loadResult.network;
    networkLoaded = true;
    selectedNodeId = -1;
    currentResult = SearchResult{};
    hasPendingSearch = false;
    animation.clear();
    statusMessage = "Rede carregada com sucesso.";
    return true;
}

void App::reset()
{
    animation.clear();
    selectedNodeId = -1;
    currentResult = SearchResult{};
    loadNetworkFromCurrentPath();
}

void App::executeSearch(const std::string& resourceId, int ttl, SearchAlgorithmType algorithm)
{
    if (!networkLoaded || selectedNodeId < 0 || resourceId.empty())
    {
        statusMessage = "Selecione um no e um recurso antes de buscar.";
        return;
    }

    SearchRequest request;
    request.sourceNodeId = selectedNodeId;
    request.resourceId = resourceId;
    request.ttl = ttl;
    request.algorithm = algorithm;

    pendingSearch = request;
    hasPendingSearch = true;
    statusMessage = "Busca agendada.";
}

void App::processPendingSearch()
{
    if (!hasPendingSearch)
    {
        return;
    }

    SearchRequest request = pendingSearch;
    hasPendingSearch = false;

    runSearchNow(request);
}

void App::runSearchNow(const SearchRequest& request)
{
    if (!networkLoaded || request.sourceNodeId < 0 || request.resourceId.empty())
    {
        statusMessage = "Selecione um no e um recurso antes de buscar.";
        return;
    }

    try
    {
        currentResult = searchEngine.execute(network, request);
        animation.start(currentResult);
    }
    catch (const std::exception& exception)
    {
        statusMessage = "Erro durante a busca: ";
        statusMessage += exception.what();
        currentResult = SearchResult{};
        animation.clear();
        return;
    }
    catch (...)
    {
        statusMessage = "Erro desconhecido durante a busca.";
        currentResult = SearchResult{};
        animation.clear();
        return;
    }

    statusMessage = currentResult.success
        ? "Busca concluida: recurso encontrado."
        : "Busca concluida: recurso nao encontrado.";
}

bool App::hasNetwork() const
{
    return networkLoaded;
}

const P2PNetwork& App::getNetwork() const
{
    return network;
}

P2PNetwork& App::getNetwork()
{
    return network;
}

int App::getSelectedNodeId() const
{
    return selectedNodeId;
}

void App::setSelectedNodeId(int nodeId)
{
    selectedNodeId = nodeId;
}

void App::clearSelection()
{
    selectedNodeId = -1;
}

char* App::getPathBuffer()
{
    return pathBuffer.data();
}

std::size_t App::getPathBufferSize() const
{
    return pathBuffer.size();
}

void App::setPath(const std::string& path)
{
    std::snprintf(pathBuffer.data(), pathBuffer.size(), "%s", path.c_str());
    currentPath = pathBuffer.data();
}

const std::string& App::getCurrentPath() const
{
    return currentPath;
}

const std::string& App::getStatusMessage() const
{
    return statusMessage;
}

SearchAnimation& App::getAnimation()
{
    return animation;
}

const SearchAnimation& App::getAnimation() const
{
    return animation;
}

std::vector<std::string> App::collectResourceIds() const
{
    std::set<std::string> uniqueResources;

    if (!networkLoaded)
    {
        return {};
    }

    for (const auto& networkNode : network.getNetworkNodes())
    {
        for (const auto& resource : networkNode.node.getResources())
        {
            uniqueResources.insert(resource.id);
        }
    }

    return { uniqueResources.begin(), uniqueResources.end() };
}

void App::renderMainLayout()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float topOffset = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + topOffset));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - topOffset));
    ImGui::Begin(
        "Main",
        nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings
    );

    const float inspectorWidth = 320.0f;
    ImVec2 available = ImGui::GetContentRegionAvail();
    ImVec2 graphSize(available.x - inspectorWidth - ImGui::GetStyle().ItemSpacing.x, available.y);

    graphViewport.render(*this, graphSize);
    ImGui::SameLine();
    inspectorPanel.render(*this, ImVec2(inspectorWidth, available.y));

    ImGui::End();
}
