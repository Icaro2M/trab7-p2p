#include "NetworkLoader.h"

#include "../model/Resource.h"

#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

NetworkLoadResult NetworkLoader::loadFromFile(const std::string& filePath) const
{
    NetworkLoadResult result;

    std::ifstream file(filePath);

    if (!file.is_open())
    {
        result.errorMessage = "Nao foi possivel abrir o arquivo de configuracao.";
        return result;
    }

    int numNodes = 0;
    int minNeighbors = 0;
    int maxNeighbors = 0;

    std::map<int, std::vector<Resource>> resourcesByNode;
    std::vector<Edge> edges;

    enum class Section
    {
        None,
        Resources,
        Edges
    };

    Section section = Section::None;

    std::string line;

    while (std::getline(file, line))
    {
        line = trim(line);

        if (line.empty())
        {
            continue;
        }

        if (line[0] == '#')
        {
            continue;
        }

        if (startsWith(line, "num_nodes:"))
        {
            std::string value = trim(line.substr(std::string("num_nodes:").size()));
            numNodes = std::stoi(value);
            continue;
        }

        if (startsWith(line, "min_neighbors:"))
        {
            std::string value = trim(line.substr(std::string("min_neighbors:").size()));
            minNeighbors = std::stoi(value);
            continue;
        }

        if (startsWith(line, "max_neighbors:"))
        {
            std::string value = trim(line.substr(std::string("max_neighbors:").size()));
            maxNeighbors = std::stoi(value);
            continue;
        }

        if (line == "resources:")
        {
            section = Section::Resources;
            continue;
        }

        if (line == "edges:")
        {
            section = Section::Edges;
            continue;
        }

        if (section == Section::Resources)
        {
            std::size_t separatorPosition = line.find(':');

            if (separatorPosition == std::string::npos)
            {
                result.errorMessage = "Linha invalida na secao resources: " + line;
                return result;
            }

            std::string nodeIdText = trim(line.substr(0, separatorPosition));
            std::string resourcesText = trim(line.substr(separatorPosition + 1));

            int nodeId = parseNodeId(nodeIdText);

            if (nodeId <= 0)
            {
                result.errorMessage = "ID de no invalido na secao resources: " + nodeIdText;
                return result;
            }

            std::vector<std::string> resourceIds = splitByComma(resourcesText);

            for (const auto& resourceId : resourceIds)
            {
                if (!resourceId.empty())
                {
                    resourcesByNode[nodeId].push_back(Resource(resourceId, resourceId));
                }
            }

            continue;
        }

        if (section == Section::Edges)
        {
            std::vector<std::string> nodeIds = splitByComma(line);

            if (nodeIds.size() != 2)
            {
                result.errorMessage = "Linha invalida na secao edges: " + line;
                return result;
            }

            int nodeAId = parseNodeId(nodeIds[0]);
            int nodeBId = parseNodeId(nodeIds[1]);

            if (nodeAId <= 0 || nodeBId <= 0)
            {
                result.errorMessage = "ID de no invalido na secao edges: " + line;
                return result;
            }

            edges.push_back(Edge(nodeAId, nodeBId));
            continue;
        }

        result.errorMessage = "Linha fora de uma secao reconhecida: " + line;
        return result;
    }

    if (numNodes <= 0)
    {
        result.errorMessage = "num_nodes deve ser maior que zero.";
        return result;
    }

    if (minNeighbors < 0)
    {
        result.errorMessage = "min_neighbors nao pode ser negativo.";
        return result;
    }

    if (maxNeighbors < minNeighbors)
    {
        result.errorMessage = "max_neighbors deve ser maior ou igual a min_neighbors.";
        return result;
    }

    P2PNetwork network;

    for (int i = 1; i <= numNodes; ++i)
    {
        std::vector<Resource> nodeResources;

        auto it = resourcesByNode.find(i);

        if (it != resourcesByNode.end())
        {
            nodeResources = it->second;
        }

        Node node(i, nodeResources);
        Position position = generatePosition(i - 1, numNodes);

        network.addNetworkNode(NetworkNode(node, position));
    }

    for (const auto& edge : edges)
    {
        bool added = network.addEdge(edge);

        if (!added)
        {
            result.errorMessage = "Aresta invalida ou duplicada encontrada.";
            return result;
        }
    }

    result.success = true;
    result.numNodes = numNodes;
    result.minNeighbors = minNeighbors;
    result.maxNeighbors = maxNeighbors;
    result.network = network;

    return result;
}

std::string NetworkLoader::trim(const std::string& value)
{
    std::size_t start = value.find_first_not_of(" \t\r\n");

    if (start == std::string::npos)
    {
        return "";
    }

    std::size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

bool NetworkLoader::startsWith(const std::string& value, const std::string& prefix)
{
    return value.rfind(prefix, 0) == 0;
}

int NetworkLoader::parseNodeId(const std::string& value)
{
    std::string text = trim(value);

    if (text.empty())
    {
        return -1;
    }

    if (text[0] == 'n' || text[0] == 'N')
    {
        text = text.substr(1);
    }

    if (text.empty())
    {
        return -1;
    }

    return std::stoi(text);
}

std::vector<std::string> NetworkLoader::splitByComma(const std::string& value)
{
    std::vector<std::string> parts;
    std::stringstream stream(value);

    std::string item;

    while (std::getline(stream, item, ','))
    {
        parts.push_back(trim(item));
    }

    return parts;
}

Position NetworkLoader::generatePosition(int nodeIndex, int totalNodes)
{
    Position position;

    if (totalNodes <= 0)
    {
        return position;
    }

    constexpr float radius = 250.0f;

    double angle = (2.0 * M_PI * static_cast<double>(nodeIndex)) / static_cast<double>(totalNodes);

    position.x = static_cast<float>(std::cos(angle) * radius);
    position.y = static_cast<float>(std::sin(angle) * radius);
    position.z = 0.0f;

    return position;
}