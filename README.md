# Simulador de Rede P2P

## Equipe

- Ícaro Molina - 2310334
- Mateus Maia - 2310323
- Nelson Mateus - 2316448

## Contexto

Este projeto simula uma rede P2P em C++, permitindo carregar uma topologia de rede, visualizar os nós e executar buscas por recursos usando diferentes estratégias de propagação.

## Interface

![Interface do simulador](figs/trab7p2p.png)

## Algoritmos

### Flooding

No flooding, a busca começa em um nó de origem e é enviada para todos os seus vizinhos. Cada nó que recebe a requisição verifica se possui o recurso procurado ou, na versão informada, se conhece o recurso em cache. Caso não encontre, ele repassa a busca para seus próprios vizinhos enquanto ainda houver TTL disponível.

Para evitar repetição desnecessária, o algoritmo controla os nós já visitados e só adiciona novos nós à próxima onda de busca quando eles ainda não receberam a requisição. Quando o recurso é encontrado, o caminho é reconstruído a partir do nó de origem até o nó que respondeu.

### Random Search

No random search, a busca também parte de um nó de origem, mas não é enviada para todos os vizinhos ao mesmo tempo. Em vez disso, os vizinhos são embaralhados e o algoritmo tenta seguir um caminho por vez, de forma semelhante a uma DFS.

Cada envio consome uma unidade do TTL. O algoritmo mantém um conjunto de nós já visitados para evitar ciclos, como voltar de um nó para o anterior. Se um caminho não encontra o recurso, a busca retorna e tenta outro vizinho ainda não visitado. Na versão informada, o nó também pode responder usando informações armazenadas em cache.
