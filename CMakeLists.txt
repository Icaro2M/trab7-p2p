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

# Adicione a origem ao executável deste projeto.
add_executable (trab7-p2p "trab7-p2p.cpp"
"trab7-p2p.h"
"src/model/Node.cpp"
"src/search/Flooding.cpp"
"src/search/RandomSearch.cpp"
"src/network/P2pNetwork.cpp"
"src/network/NetworkLoader.cpp"
"src/network/NetworkValidator.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET trab7-p2p PROPERTY CXX_STANDARD 20)
endif()

# TODO: Adicione testes e instale destinos, se necessário.
