#include "trab7-p2p.h"

#include "src/network/NetworkLoader.h"
#include "src/network/NetworkValidator.h"

#include "src/search/SearchEngine.h"
#include "src/search/SearchRequest.h"
#include "src/search/SearchResult.h"

#include <iostream>
#include <string>
#include <vector>

void printNetworkInfo(const P2PNetwork& network)
{
    std::cout << "=== Rede carregada ===\n";
    std::cout << "Nos: " << network.getNetworkNodes().size() << "\n";
    std::cout << "Arestas: " << network.getEdges().size() << "\n\n";

    for (const auto& networkNode : network.getNetworkNodes())
    {
        int nodeId = networkNode.node.getId();

        std::cout << "n" << nodeId << " | recursos: ";

        const auto& resources = networkNode.node.getResources();

        for (std::size_t i = 0; i < resources.size(); ++i)
        {
            std::cout << resources[i].id;

            if (i + 1 < resources.size())
            {
                std::cout << ", ";
            }
        }

        std::cout << " | vizinhos: ";

        const auto& neighbors = network.getNeighbors(nodeId);

        for (std::size_t i = 0; i < neighbors.size(); ++i)
        {
            std::cout << "n" << neighbors[i];

            if (i + 1 < neighbors.size())
            {
                std::cout << ", ";
            }
        }

        std::cout << "\n";
    }

    std::cout << "\n";
}

void printValidationErrors(const NetworkValidationResult& validationResult)
{
    std::cout << "=== Erros de validacao ===\n";

    for (const auto& error : validationResult.errors)
    {
        std::cout << "- " << error << "\n";
    }

    std::cout << "\n";
}

std::string algorithmName(SearchAlgorithmType algorithm)
{
    switch (algorithm)
    {
    case SearchAlgorithmType::Flooding:
        return "Flooding";

    case SearchAlgorithmType::InformedFlooding:
        return "Informed Flooding";

    case SearchAlgorithmType::RandomWalk:
        return "Random Walk";

    case SearchAlgorithmType::InformedRandomWalk:
        return "Informed Random Walk";
    }

    return "Desconhecido";
}

void printSearchResult(const std::string& title, const SearchResult& result)
{
    std::cout << "=== " << title << " ===\n";

    std::cout << "Recurso buscado: " << result.resourceId << "\n";
    std::cout << "Encontrado: " << (result.success ? "sim" : "nao") << "\n";
    std::cout << "Usou cache: " << (result.cacheHit ? "sim" : "nao") << "\n";

    if (result.success)
    {
        std::cout << "Recurso localizado no no: n" << result.foundNode << "\n";

        if (result.cacheHit)
        {
            std::cout << "Informacao obtida pelo cache do no: n"
                << result.informedByNode << "\n";
        }
    }

    std::cout << "Mensagens trocadas: " << result.messageCount << "\n";
    std::cout << "Nos envolvidos: " << result.involvedNodesCount << "\n";
    std::cout << "TTL restante: " << result.remainingTTL << "\n";

    std::cout << "Caminho: ";

    if (result.path.empty())
    {
        std::cout << "(vazio)";
    }
    else
    {
        for (std::size_t i = 0; i < result.path.size(); ++i)
        {
            std::cout << "n" << result.path[i];

            if (i + 1 < result.path.size())
            {
                std::cout << " -> ";
            }
        }
    }

    std::cout << "\n";

    std::cout << "Nos visitados: ";

    if (result.visitedNodes.empty())
    {
        std::cout << "(nenhum)";
    }
    else
    {
        for (std::size_t i = 0; i < result.visitedNodes.size(); ++i)
        {
            std::cout << "n" << result.visitedNodes[i];

            if (i + 1 < result.visitedNodes.size())
            {
                std::cout << ", ";
            }
        }
    }

    std::cout << "\n\n";
}

void runSearchTests(P2PNetwork& network)
{
    SearchEngine searchEngine;

    std::vector<SearchRequest> requests = {
        // Primeiro executa uma busca normal para popular o cache.
        { 1, "r7", 4, SearchAlgorithmType::Flooding },

        // Depois executa a versão informada para verificar uso do cache.
        { 1, "r7", 4, SearchAlgorithmType::InformedFlooding },

        // Random walk normal. Pode encontrar ou não, pois depende da aleatoriedade.
        { 3, "r3", 4, SearchAlgorithmType::RandomWalk },

        // Random walk informado. Pode se beneficiar do cache se algum nó do caminho souber.
        { 3, "r3", 4, SearchAlgorithmType::InformedRandomWalk },

        // Busca por recurso inexistente.
        { 1, "r999", 4, SearchAlgorithmType::InformedFlooding }
    };

    for (const auto& request : requests)
    {
        std::string title =
            algorithmName(request.algorithm) +
            " | origem: n" +
            std::to_string(request.sourceNodeId) +
            " | recurso: " +
            request.resourceId +
            " | TTL: " +
            std::to_string(request.ttl);

        SearchResult result = searchEngine.execute(network, request);

        printSearchResult(title, result);
    }
}

int main()
{
    const std::string configPath = "C:/Users/icaro/unifor/computacao_distribuida/trab7-p2p/configs/rede_valida.txt";

    NetworkLoader loader;
    NetworkLoadResult loadResult = loader.loadFromFile(configPath);

    if (!loadResult.success)
    {
        std::cout << "Erro ao carregar rede:\n";
        std::cout << loadResult.errorMessage << "\n";
        return 1;
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
        printValidationErrors(validationResult);
        return 1;
    }

    std::cout << "Rede carregada e validada com sucesso.\n\n";

    printNetworkInfo(loadResult.network);

    runSearchTests(loadResult.network);

    return 0;
}