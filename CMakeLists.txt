# CMakeList.txt: projeto de CMake para trab7-p2p, incluir origem e definir
# lógica específica a um projeto aqui.
#
cmake_minimum_required (VERSION 3.10)

# Habilite a Recarga Dinâmica para compiladores MSVC, se houver suporte.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("trab7-p2p")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(extern/glfw)

# Adicione a origem ao executável deste projeto.
add_executable (trab7-p2p
    "trab7-p2p.cpp"
    "trab7-p2p.h"

    "src/model/Node.cpp"

    "src/search/Flooding.cpp"
    "src/search/RandomSearch.cpp"
    "src/search/SearchEngine.cpp"

    "src/network/P2pNetwork.cpp"
    "src/network/NetworkLoader.cpp"
    "src/network/NetworkValidator.cpp"

    "src/frontend/App.cpp"
    "src/frontend/GraphViewport.cpp"
    "src/frontend/InspectorPanel.cpp"
    "src/frontend/TopMenuBar.cpp"
    "src/frontend/SearchAnimation.cpp"
    "src/frontend/GraphLayout.cpp"

    "extern/imgui/imgui.cpp"
    "extern/imgui/imgui_draw.cpp"
    "extern/imgui/imgui_tables.cpp"
    "extern/imgui/imgui_widgets.cpp"
    "extern/imgui/backends/imgui_impl_glfw.cpp"
    "extern/imgui/backends/imgui_impl_opengl3.cpp"
)

target_include_directories(trab7-p2p PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    extern/imgui
    extern/imgui/backends
    extern/glfw/include
)

target_link_libraries(trab7-p2p PRIVATE
    glfw
    opengl32
    comdlg32
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET trab7-p2p PROPERTY CXX_STANDARD 20)
endif()

# TODO: Adicione testes e instale destinos, se necessário.
