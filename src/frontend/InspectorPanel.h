#pragma once

#include "imgui.h"

class App;

class InspectorPanel
{
public:
    void render(App& app, const ImVec2& size);

private:
    int ttl = 4;
    int algorithmIndex = 0;
    int resourceIndex = 0;
};
