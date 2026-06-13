#pragma once

#include "GraphLayout.h"

#include "imgui.h"

class App;

class GraphViewport
{
public:
    void render(App& app, const ImVec2& size);

private:
    GraphLayout layout;
};
