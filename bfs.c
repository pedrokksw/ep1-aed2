#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define INFINITO_INT INT_MAX

/*
    Grafo conectado (ou conexo): um grafo não direcionado conexo é aquele
    onde existe caminho entre todo par de vértices,

    A AGM do Prim, que queremos implementar, ilustra isso bem: ele parte de
    um grafo conexo cheio de ciclos e vai escolhendo arestas de menor peso,
    sempre pulando qualquer aresta que fecharia um ciclo. O resultado é a
    AGM: ainda conexa, ainda não direcionada, mas agora acíclica. Ou seja,
    uma árvore.

    Se o grafo for desconexo, ele simplesmente fica preso no componente onde
    começou e nunca alcança os demais vértices. Como resultado, não conseguiriamos
    uma AGM do grafo todo.

    E é para isso que precisamos de mecanismos como Busca Em Largura ou Busca
    Em Profundidade.

    O fluxo na prática fica assim:

        1. rodar BFS/DFS no grafo
        2. se algum vértice ficou não visitado significa que o grafo é desconexo,
            e o Algoritmo de Prim não se aplica
        3. se todos foram visitados, temos que o grafo é conexo, e podemos rodar
        o Algoritmo de Prim

    (Para esse caso, usei a Busca em Largura/BFS (Breadth-First Search)).

    Busca em Largura: expande a fronteira entre vértices descobertos e não descobertos
    uniformemente através da largura da fronteira (aula 09 - busca em largura).

    Para isso, usamos a fila por causa de sua principal característica: FIFO (first in
    first out).

    O primeiro vértice a entrar na fila é o primeiro a ser processado. Quando ele é processado,
    seus vizinhos entram no fim da fila, atrás de todos que já estavam lá. Isso significa que
    toda a camada atual é processada antes de qualquer vértice da próxima camada sequer ser tocado.

    Para representar o estado dos vértices, usamos as seguintes cores:
        ● Branco: todos os vertices são inicializados branco (não descoberto ainda)
        ● Cinza: descoberto pela primeira vez
        ● Preto: quando terminamos de processar todos os adjacentes de um vértice cinza
            ele se torna preto.

        No meu código, defini essas cores como um enum (como orientado em aula)
*/

/*
    definição e funções básicas para o uso da fila (estrutura de dados usada na busca
    em largura)
*/

typedef struct NOf
{
    int vertice;
    struct NOf *prox;
} NOf;

typedef struct
{
    NOf *inicio;
    NOf *fim;
} Fila;

void inicializaFila(Fila *f)
{
    f->inicio = NULL;
    f->fim = NULL;
}

bool filaVazia(Fila *f)
{
    return f->inicio == NULL;
}

void insereFila(Fila *f, int vertice)
{
    NOf *novo = malloc(sizeof(NOf));
    novo->vertice = vertice;
    novo->prox = NULL;

    if (filaVazia(f))
        f->inicio = novo;
    else
        f->fim->prox = novo;

    f->fim = novo;
}

int removeFila(Fila *f)
{
    NOf *aux = f->inicio;
    int vertice = aux->vertice;

    f->inicio = f->inicio->prox;
    if (f->inicio == NULL)
        f->fim = NULL;

    free(aux);
    return vertice;
}

/*
    buscaEmLargura(grafo)
    {
        Aloca vetores cor, antecessor, distancia com tamanho grafo->nrVertices(obtemNrVertices) Para cada vertice v cor[v] <-branco;
        antecessor[v] < - -1;
        distancia[v] < -infinito;
        Para cada vertice v
            se cor[v] = branco
                visitaLargura(v, grafo, cor, antecessor, distancia);
    }

    visitaLargura(origem, grafo, cor, antecessor, distancia)
    {
        cor[origem] = CINZA;
        distancia[origem] = 0;
        inicializaFila(F);
        insereFila(F, origem);
        enquanto F != 0 w = removeFila(F);
        para cada vertice u da lista de adjacencia de w
                se cor[u] == BRANCO
                                cor[u] = CINZA;
        antecessor[u] = w;
        distancia[u] = distancia[w] + 1;
        insereFila(F, u);
        cor[w] = PRETO;
    }
*/

typedef enum
{
    BRANCO,
    CINZA,
    PRETO
} Cor;

void visitaLargura(int origem, Grafo *grafo, Cor *cor, int *antecessor, int *distancia)
{
    int nrVertices = obtemNrVertices(grafo);
    Fila f;
    cor[origem] = CINZA;
    distancia[origem] = 0;
    inicializaFila(&f);
    insereFila(&f, origem);

    while (!filaVazia(&f))
    {
        int w = removeFila(&f);

        for (int u = 0; u < nrVertices; u++)
        {
            if (existeAresta(w, u, grafo))
            {
                if (cor[u] == BRANCO)
                {
                    cor[u] = CINZA;
                    antecessor[u] = w;
                    distancia[u] = distancia[w] + 1;
                    insereFila(&f, u);
                }
            }
        }
        cor[w] = PRETO;
    }
}

void buscaEmLargura(Grafo *grafo, Cor *cor, int *antecessor, int *distancia)
{
    int nrVertices = obtemNrVertices(grafo);

    for (int v = 0; v < nrVertices; v++)
    {
        if (cor[v] == BRANCO)
            visitaLargura(v, grafo, cor, antecessor, distancia);
    }
}

/*
    bool verificaGrafoConectado(Grafo *grafo):

    função para verificar se o grafo é conectado (necessário para a execução do
    Algoritmo de Prim e, consequentemente, requisito para o EP).
*/

bool verificaGrafoConectado(Grafo *grafo)
{
    int nrVertices = obtemNrVertices(grafo);
    fprintf(stderr, "nrVertices em verificaGrafoConectado: %d\n", nrVertices);

    Cor *cor = malloc(nrVertices * sizeof(Cor));
    int *antecessor = malloc(nrVertices * sizeof(int));
    int *distancia = malloc(nrVertices * sizeof(int));

    for (int v = 0; v < nrVertices; v++)
    {
        cor[v] = BRANCO;
        antecessor[v] = -1;
        distancia[v] = INFINITO_INT;
    }

    visitaLargura(0, grafo, cor, antecessor, distancia);

    bool estaConectado = true;

    for (int v = 0; v < nrVertices; v++)
    {
        if (cor[v] == BRANCO)
            estaConectado = false;
    }

    free(cor);
    free(antecessor);
    free(distancia);

    return estaConectado;
}
