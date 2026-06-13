#pragma once

#include "../network/P2PNetwork.h"
#include "../search/SearchEngine.h"
#include "../search/SearchRequest.h"
#include "../search/SearchResult.h"
#include "GraphViewport.h"
#include "InspectorPanel.h"
#include "SearchAnimation.h"
#include "TopMenuBar.h"

#include <array>
#include <cstddef>
#include <string>
#include <vector>

class App
{
public:
    App();

    int run();
    void render();

    bool loadNetworkFromCurrentPath();
    void reset();
    void executeSearch(const std::string& resourceId, int ttl, SearchAlgorithmType algorithm);

    bool hasNetwork() const;
    const P2PNetwork& getNetwork() const;
    P2PNetwork& getNetwork();

    int getSelectedNodeId() const;
    void setSelectedNodeId(int nodeId);
    void clearSelection();

    char* getPathBuffer();
    std::size_t getPathBufferSize() const;
    const std::string& getCurrentPath() const;
    const std::string& getStatusMessage() const;

    SearchAnimation& getAnimation();
    const SearchAnimation& getAnimation() const;
    std::vector<std::string> collectResourceIds() const;

private:
    void processPendingSearch();
    void runSearchNow(const SearchRequest& request);
    void renderMainLayout();

    std::array<char, 512> pathBuffer{};
    std::string currentPath;
    std::string statusMessage;

    P2PNetwork network;
    bool networkLoaded = false;
    int selectedNodeId = -1;

    SearchEngine searchEngine;
    SearchResult currentResult;
    SearchRequest pendingSearch;
    bool hasPendingSearch = false;
    SearchAnimation animation;

    TopMenuBar topMenuBar;
    GraphViewport graphViewport;
    InspectorPanel inspectorPanel;
};
