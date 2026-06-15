#pragma once

#include "NetworkLoadResult.h"

#include <string>

class NetworkLoader
{
public:
    NetworkLoadResult loadFromFile(const std::string& filePath) const;

private:
    static std::string trim(const std::string& value);
    static bool startsWith(const std::string& value, const std::string& prefix);

    static int parseNodeId(const std::string& value);
    static std::vector<std::string> splitByComma(const std::string& value);

    static std::vector<Position> generateClusteredPositions(int totalNodes, const std::vector<Edge>& edges);
};
