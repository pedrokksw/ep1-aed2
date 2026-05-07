#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXNUMVERTICES 100
#define ARESTA_NULA -1 /* representa ausência de aresta */
#define VERTICE_INVALIDO -1

typedef int ApontadorVerticeAdjacente;

typedef float Peso;

typedef struct
{
    Peso matriz[MAXNUMVERTICES][MAXNUMVERTICES];
    int numVertices;
} Grafo;

/*
    inicializaGrafo(Grafo *g, int numVertices):
    Inicializa um grafo com numVertices vertices.
    Preenche as celulas com ARESTA_NULA (representando a ausencia de aresta)

    chamada da função:
        Grafo g;
        inicializaGrafo(&g, 10);

*/

bool inicializaGrafo(Grafo *g, int numVertices)
{
    if (numVertices > MAXNUMVERTICES)
    {
        fprintf(stderr, "ERRO na chamada de inicializaGrafo: Número de vértices maior que o máximo permitido de %d. \n", MAXNUMVERTICES);
        return false;
    }
    if (numVertices <= 0)
    {
        fprintf(stderr, "ERRO na chamada de inicializaGrafo: Número de vértices deve ser positivo. \n");
        return false;
    }

    g->numVertices = numVertices;

    for (int i = 0; i < numVertices; i++)
        for (int j = 0; j < numVertices; j++)
            g->matriz[i][j] = ARESTA_NULA;

    return true;
}

/*
    int obtemNrVertices(Grafo *g): retorna o númerod e vértices do grafo
*/
int obtemNrVertices(Grafo *g)
{
    return g->numVertices;
}

/*
    void verificaValidadeVertice(int v, Grafo *g):
    verifica se o nr do vértice é valido no grafo, ou seja, se ele é maior
    ou igual a zero e menor que o nr total de vértices do grafo. Se não válido,
    sai do programa.
*/
void verificaValidadeVertice(int v, Grafo *g)
{
    if (v >= g->numVertices)
    {
        fprintf(stderr, "ERRO: Número de vértices (%d) maior ou igual ao número total de vértices (%d) \n", v, g->numVertices);
        exit(1);
    }
    if (v < 0)
    {
        fprintf(stderr, "ERRO: Número de vértices (%d) deve ser não negativo. \n", v);
        exit(1);
    }
}

/*
    bool existeAresta(int v1, int v2, Grafo *g):
    retorna true se existe a aresta (v1, v2) no grafo e false caso contrário

*/
bool existeAresta(int v1, int v2, Grafo *g)
{
    verificaValidadeVertice(v1, g);
    verificaValidadeVertice(v2, g);
    return (g->matriz[v1][v2] != ARESTA_NULA);
}

/*
    void insereAresta(int v1, int v2, Peso peso, Grafo *g):
    Insere a aresta (v1, v2) com peso "peso" no grafo.
    Não verifica se a aresta ja existia (isso deve ser feito pelo usuário antes,
    se necessário)
*/
void insereAresta(int v1, int v2, Peso peso, Grafo *g)
{
    verificaValidadeVertice(v1, g);
    verificaValidadeVertice(v2, g);

    g->matriz[v1][v2] = peso;
    g->matriz[v2][v1] = peso;
}

/*
    Peso obtemPesoAresta(int v1, int v2, Grafo *g):
    retorna o peso da aresta (v1, v2) no grafo se ela existir e ARESTA_NULA
    caso contrário - O(1)
*/

Peso obtemPesoAresta(int v1, int v2, Grafo *g)
{
    verificaValidadeVertice(v1, g);
    verificaValidadeVertice(v2, g);

    return (g->matriz[v1][v2]);
}

/*
    bool removeAresta(int v1, int v2, Peso *peso, Grafo *g):
    remove a aresta (v1, v2) do grafo colocando ARESTA_NULA em sua célula
    (representando ausência de aresta).
    Se a aresta existir, retorna true. Caso contrário, retorna false. O(1)
*/
bool removeAresta(int v1, int v2, Peso *peso, Grafo *g)
{
    verificaValidadeVertice(v1, g);
    verificaValidadeVertice(v2, g);

    if (g->matriz[v1][v2] == ARESTA_NULA)
    {
        fprintf(stderr, "Aresta não existe. \n");
        return false;
    }
    else
    {
        *peso = g->matriz[v1][v2];
        g->matriz[v1][v2] = ARESTA_NULA;
    }
    return true;
}

/*
    int listaAdjVazia(int v, Grafo *g, int prox):
    Retorna true se a lista de adjacência (de vértices adjacentes)
    do vértice v é vazia. False, caso contrário.
*/
bool listaAdjVazia(int v, Grafo *g)
{
    verificaValidadeVertice(v, g);
    int i = 0;
    while ((i < g->numVertices) && (g->matriz[v][i] == ARESTA_NULA))
        i++;
    if (i == g->numVertices)
        return true;
    else
        return false;
}

/*
    int proxListaAdj(int v, Grafo *g, int prox):
    Trata-se de um iterador sobre a lista de adjacência do vértice v.
    Retorna o próximo vértice adjacente a v, partindo do vértice "atual" adjacente a v
    ou VERTICE_INVALIDO se a lista de adjacencia tiver terminado sem um novo próximo.

*/
ApontadorVerticeAdjacente proxListaAdj(int v, Grafo *g, ApontadorVerticeAdjacente atual)
{
    verificaValidadeVertice(v, g);
    atual++;
    while ((atual < g->numVertices) && (g->matriz[v][atual] == ARESTA_NULA))
        atual++;
    if (atual == g->numVertices)
        return VERTICE_INVALIDO;
    else
        return atual;
}

/*
    int primeiroListaAdj(int v, Grafo *g):
    Retorna o primeiro vertice da lista de adjacencia de v
    ou VERTICE_INVALIDO se a lista de adjacencia estiver vazia.
*/
int primeiroListaAdj(int v, Grafo *g)
{
    int primeiro = proxListaAdj(v, g, -1);
    if (primeiro == VERTICE_INVALIDO)
        fprintf(stderr, "ERRO: Lista de Adjacência vazia. \n");
    return primeiro;
}

void imprimeGrafo(Grafo *g)
{
    for (int i = 0; i < g->numVertices; i++)
        for (int j = 0; j < g->numVertices; j++)
            if (g->matriz[i][j] != ARESTA_NULA)
                printf("Aresta %d -> %d, peso: %.1f\n", i, j, g->matriz[i][j]);
}

int verticeDestino(ApontadorVerticeAdjacente p, Grafo *g)
{
    return (int)p;
}
