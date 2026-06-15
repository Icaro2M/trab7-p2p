#include "NetworkLoader.h"

#include "../model/Resource.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <queue>
#include <random>
#include <sstream>
#include <vector>

namespace
{
constexpr float pi = 3.14159265358979323846f;

float distanceBetween(const Position& a, const Position& b)
{
    const float dx = b.x - a.x;
    const float dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}
}

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

    std::vector<Position> positions = generateClusteredPositions(numNodes, edges);

    for (int i = 1; i <= numNodes; ++i)
    {
        std::vector<Resource> nodeResources;

        auto it = resourcesByNode.find(i);

        if (it != resourcesByNode.end())
        {
            nodeResources = it->second;
        }

        Node node(i, nodeResources);
        Position position = positions[static_cast<std::size_t>(i - 1)];

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

std::vector<Position> NetworkLoader::generateClusteredPositions(int totalNodes, const std::vector<Edge>& edges)
{
    std::vector<Position> positions(static_cast<std::size_t>(std::max(totalNodes, 0)));

    if (totalNodes <= 0)
    {
        return positions;
    }

    std::vector<std::vector<int>> adjacency(static_cast<std::size_t>(totalNodes));

    for (const auto& edge : edges)
    {
        if (edge.nodeAId < 1 || edge.nodeAId > totalNodes || edge.nodeBId < 1 || edge.nodeBId > totalNodes)
        {
            continue;
        }

        int a = edge.nodeAId - 1;
        int b = edge.nodeBId - 1;

        adjacency[static_cast<std::size_t>(a)].push_back(b);
        adjacency[static_cast<std::size_t>(b)].push_back(a);
    }

    const int clusterCount = std::max(1, std::min(6, static_cast<int>(std::ceil(std::sqrt(static_cast<float>(totalNodes)) / 1.6f))));
    const int targetClusterSize = std::max(1, static_cast<int>(std::ceil(static_cast<float>(totalNodes) / static_cast<float>(clusterCount))));

    std::vector<int> nodeCluster(static_cast<std::size_t>(totalNodes), -1);
    int currentCluster = 0;

    for (int start = 0; start < totalNodes; ++start)
    {
        if (nodeCluster[static_cast<std::size_t>(start)] != -1)
        {
            continue;
        }

        std::queue<int> pending;
        pending.push(start);

        int assignedInCluster = 0;

        while (!pending.empty())
        {
            int nodeIndex = pending.front();
            pending.pop();

            if (nodeCluster[static_cast<std::size_t>(nodeIndex)] != -1)
            {
                continue;
            }

            nodeCluster[static_cast<std::size_t>(nodeIndex)] = currentCluster;
            ++assignedInCluster;

            for (int neighborIndex : adjacency[static_cast<std::size_t>(nodeIndex)])
            {
                if (nodeCluster[static_cast<std::size_t>(neighborIndex)] == -1)
                {
                    pending.push(neighborIndex);
                }
            }

            if (assignedInCluster >= targetClusterSize && currentCluster < clusterCount - 1)
            {
                break;
            }
        }

        if (assignedInCluster >= targetClusterSize && currentCluster < clusterCount - 1)
        {
            ++currentCluster;
        }
    }

    const int actualClusterCount = std::max(1, currentCluster + 1);
    std::mt19937 random(static_cast<unsigned int>(totalNodes * 7919 + edges.size() * 104729));
    std::uniform_real_distribution<float> angleDistribution(0.0f, 2.0f * pi);
    std::uniform_real_distribution<float> radiusDistribution(35.0f, 115.0f);
    std::uniform_real_distribution<float> jitterDistribution(-28.0f, 28.0f);

    std::vector<Position> clusterCenters(static_cast<std::size_t>(actualClusterCount));
    const float clusterRingRadius = 220.0f + static_cast<float>(actualClusterCount) * 35.0f;

    for (int clusterIndex = 0; clusterIndex < actualClusterCount; ++clusterIndex)
    {
        const float angle = -pi * 0.5f + 2.0f * pi * static_cast<float>(clusterIndex) / static_cast<float>(actualClusterCount);

        clusterCenters[static_cast<std::size_t>(clusterIndex)].x = std::cos(angle) * clusterRingRadius + jitterDistribution(random);
        clusterCenters[static_cast<std::size_t>(clusterIndex)].y = std::sin(angle) * clusterRingRadius + jitterDistribution(random);
    }

    for (int nodeIndex = 0; nodeIndex < totalNodes; ++nodeIndex)
    {
        const int clusterIndex = std::max(0, std::min(actualClusterCount - 1, nodeCluster[static_cast<std::size_t>(nodeIndex)]));
        const float angle = angleDistribution(random);
        const float radius = radiusDistribution(random);
        const Position& center = clusterCenters[static_cast<std::size_t>(clusterIndex)];

        positions[static_cast<std::size_t>(nodeIndex)].x = center.x + std::cos(angle) * radius;
        positions[static_cast<std::size_t>(nodeIndex)].y = center.y + std::sin(angle) * radius;
    }

    for (int iteration = 0; iteration < 140; ++iteration)
    {
        std::vector<Position> displacement(positions.size());
        const float cooling = 1.0f - static_cast<float>(iteration) / 140.0f;
        const float maxStep = 9.0f * cooling + 1.0f;

        for (int a = 0; a < totalNodes; ++a)
        {
            for (int b = a + 1; b < totalNodes; ++b)
            {
                float dx = positions[static_cast<std::size_t>(a)].x - positions[static_cast<std::size_t>(b)].x;
                float dy = positions[static_cast<std::size_t>(a)].y - positions[static_cast<std::size_t>(b)].y;
                float distance = std::sqrt(dx * dx + dy * dy) + 0.01f;
                float force = 3600.0f / (distance * distance);

                dx /= distance;
                dy /= distance;

                displacement[static_cast<std::size_t>(a)].x += dx * force;
                displacement[static_cast<std::size_t>(a)].y += dy * force;
                displacement[static_cast<std::size_t>(b)].x -= dx * force;
                displacement[static_cast<std::size_t>(b)].y -= dy * force;
            }
        }

        for (const auto& edge : edges)
        {
            int a = edge.nodeAId - 1;
            int b = edge.nodeBId - 1;

            if (a < 0 || a >= totalNodes || b < 0 || b >= totalNodes)
            {
                continue;
            }

            float dx = positions[static_cast<std::size_t>(b)].x - positions[static_cast<std::size_t>(a)].x;
            float dy = positions[static_cast<std::size_t>(b)].y - positions[static_cast<std::size_t>(a)].y;
            float distance = std::sqrt(dx * dx + dy * dy) + 0.01f;
            float force = (distance - 105.0f) * 0.045f;

            dx /= distance;
            dy /= distance;

            displacement[static_cast<std::size_t>(a)].x += dx * force;
            displacement[static_cast<std::size_t>(a)].y += dy * force;
            displacement[static_cast<std::size_t>(b)].x -= dx * force;
            displacement[static_cast<std::size_t>(b)].y -= dy * force;
        }

        for (int nodeIndex = 0; nodeIndex < totalNodes; ++nodeIndex)
        {
            const int clusterIndex = std::max(0, std::min(actualClusterCount - 1, nodeCluster[static_cast<std::size_t>(nodeIndex)]));
            const Position& center = clusterCenters[static_cast<std::size_t>(clusterIndex)];

            displacement[static_cast<std::size_t>(nodeIndex)].x += (center.x - positions[static_cast<std::size_t>(nodeIndex)].x) * 0.003f;
            displacement[static_cast<std::size_t>(nodeIndex)].y += (center.y - positions[static_cast<std::size_t>(nodeIndex)].y) * 0.003f;

            float step = distanceBetween(Position{}, displacement[static_cast<std::size_t>(nodeIndex)]);

            if (step > maxStep)
            {
                displacement[static_cast<std::size_t>(nodeIndex)].x *= maxStep / step;
                displacement[static_cast<std::size_t>(nodeIndex)].y *= maxStep / step;
            }

            positions[static_cast<std::size_t>(nodeIndex)].x += displacement[static_cast<std::size_t>(nodeIndex)].x;
            positions[static_cast<std::size_t>(nodeIndex)].y += displacement[static_cast<std::size_t>(nodeIndex)].y;
        }
    }

    return positions;
}
