#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ARESTA_NULA -1 /* representa ausência de aresta */
#define VERTICE_INVALIDO NULL

typedef float Peso;

/* tipo estruturado Aresta: vertice destino, peso, ponteiro p/ prox. aresta */
typedef struct str_aresta
{
    int vdest;
    Peso peso;
    struct str_aresta *prox;
} Aresta;

typedef Aresta *ApontadorVerticeAdjacente;

/*
    Tipo estruturado grafo
        vetor de listas de adjacência (cada posição contém o ponteiro para o incício da lista de adjacência do vértice)
            número de vértices
*/

typedef struct
{
    ApontadorVerticeAdjacente *listaAdj;
    int numVertices;
} Grafo;

void verificaValidadeGrafo(Grafo *grafo)
{
    if (!grafo)
    {
        fprintf(stderr, "ERRO na chamada de inicializaGrafo: grafo == NULL.\n");
        exit(-1);
    }
}

/*
    bool verificaValidadeVertice(int v, Grafo *grafo): verifica se o nr do vertice e valido no grafo,
    ou seja, se ele é maior que zero e/ou menor e igual ao nr total de vertices do grafo
*/
bool verificaValidadeVertice(int v, Grafo *grafo)
{
    verificaValidadeGrafo(grafo);

    if (v >= grafo->numVertices)
    {
        fprintf(stderr, "ERRO: número do vértice (%d) maior que o número total de vértices (%d). \n", v, grafo->numVertices);
        exit(-1);
    }
    if (v < 0)
    {
        fprintf(stderr, "ERRO: número de vértice (%d) deve ser positivo.\n", v);
        exit(-1);
    }
    return true;
}

/*
    bool inicializaGrafo(Grafo *grafo, int numVertices):
    inicializa um grafo com numVertices vertices.
    aloca espaço para o vetor de apontadores de listas de adjacencias e,
    para cada vertice, inicializa o apontador de sua lista de adjacencia com null.
    Retorna true se inicializou com sucesso e false caso contrário
*/
bool inicializaGrafo(Grafo *grafo, int numVertices)
{
    if (numVertices <= 0)
    {
        fprintf(stderr, "ERRO na chamada de inicializaGrafo: número de vértices deve ser positivo.\n");
        return false;
    }

    verificaValidadeGrafo(grafo);
    grafo->numVertices = numVertices;
    if (!(grafo->listaAdj = (Aresta **)calloc(numVertices, sizeof(Aresta *))))
    {
        fprintf(stderr, "ERRO: falha na alocação de memória na função inicializaGrafo\n");
        return false;
    }

    // calloc ja inicializa com zeros... nao precisa inicializar grafo->listaAdj[i]
    return true;
}

/* int obtemNrVertices(grafo* grafo): retorna o numero de vertices do grafo*/
int obtemNrVertices(Grafo *grafo)
{
    verificaValidadeGrafo(grafo);
    return grafo->numVertices;
}

/*
    bool listaAdjVazia(int v, Grafo *grafo):
    retorna true se a lista de adjacencia (de vertices adjacentes)
    do vertice v é vazia, e false caso contrário
*/
bool listaAdjVazia(int v, Grafo *grafo)
{
    verificaValidadeVertice(v, grafo);
    return (grafo->listaAdj[v] == NULL);
}

/*
    primeiroListaAdj(v, Grafo): retorna o endereço do primeiro vertice adjacente a v.
*/

ApontadorVerticeAdjacente proxListaAdj(int v, Grafo *grafo, ApontadorVerticeAdjacente atual)
{
    verificaValidadeVertice(v, grafo);
    if (atual == NULL)
    {
        fprintf(stderr, "atual == NULL\n");
        return VERTICE_INVALIDO;
    }
    return (atual->prox);
}

ApontadorVerticeAdjacente primeiroListaAdj(int v, Grafo *grafo)
{
    verificaValidadeVertice(v, grafo);
    return (grafo->listaAdj[v]);
}

/*
    bool existeAresta(int v1, int v2, Grafo *grafo);
    Retorna true caso exista a aresta (v1, v2) no grafo e false caso contrário
*/

bool existeAresta(int v1, int v2, Grafo *grafo)
{
    ApontadorVerticeAdjacente q;

    verificaValidadeVertice(v1, grafo);
    verificaValidadeVertice(v2, grafo);

    q = grafo->listaAdj[v1];
    while ((q != NULL) && (q->vdest != v2))
        q = q->prox;
    if (q != NULL)
        return true;
    return false;
}

Peso obtemPesoAresta(int v1, int v2, Grafo *grafo)
{
    Aresta *q;

    verificaValidadeVertice(v1, grafo);
    verificaValidadeVertice(v2, grafo);

    q = grafo->listaAdj[v1];
    while (q != NULL)
    {
        if (q->vdest == v2)
            return q->peso;
        q = q->prox;
    }
    return ARESTA_NULA;
}

/*
    void insereAresta(v1, v2, Peso peso, Grafo* grafo):
    Insere a aresta (v1, v2) com peso "peso" no grafo
    Não verifica se a aresta já existia (isso deve ser feito pelo usuário antes, se necessário)
*/

void insereAresta(int v1, int v2, Peso peso, Grafo *grafo)
{
    ApontadorVerticeAdjacente p1;
    if (!(p1 = (ApontadorVerticeAdjacente)calloc(1, sizeof(Aresta))))
    {
        fprintf(stderr, "ERRO: Falha na alocação de memória\n");
        return;
    }
    p1->vdest = v2;
    p1->peso = peso;

    /* insere no inicio da lista (por eficiencia) */
    p1->prox = grafo->listaAdj[v1];
    grafo->listaAdj[v1] = p1;

    ApontadorVerticeAdjacente p2;
    if (!(p2 = (ApontadorVerticeAdjacente)calloc(1, sizeof(Aresta))))
    {
        fprintf(stderr, "ERRO: Falha na alocação de memória\n");
        free(p1);
        return;
    }
    p2->vdest = v1;
    p2->peso = peso;

    /* insere no inicio da lista (por eficiencia) */
    p2->prox = grafo->listaAdj[v2];
    grafo->listaAdj[v2] = p2;
}

bool removeArestaObtendoPeso(int v1, int v2, Peso *peso, Grafo *grafo)
{
    ApontadorVerticeAdjacente q, ant;

    if (!(verificaValidadeVertice(v1, grafo) && verificaValidadeVertice(v2, grafo)))
        return ARESTA_NULA;

    q = grafo->listaAdj[v1];
    while ((q != NULL) && (q->vdest != v2))
    {
        ant = q;
        q = q->prox;
    }
    // aresta existe
    if (q != NULL)
    {
        if (grafo->listaAdj[v1] == q)
            grafo->listaAdj[v1] = q->prox;
        else
            ant->prox = q->prox;
        *peso = q->peso;
        q->prox = NULL;
        free(q);
        q = NULL;
        return true;
    }
    // aresta nao existe
    return false;
}

void liberaGrafo(Grafo *grafo)
{
    int v;
    Aresta *p;

    verificaValidadeGrafo(grafo);

    for (v = 0; v < grafo->numVertices; v++)
    {
        while ((p = grafo->listaAdj[v]) != NULL)
        {
            grafo->listaAdj[v] = p->prox;
            p->prox = NULL;
            free(p);
        }
    }
    grafo->numVertices = 0;
    free(grafo->listaAdj);
    grafo->listaAdj = NULL;
}

/*
    void imprimeGrafo(Grafo* grafo):
    Imprime os vertices e arestas do grafo no seguinte formato:
        v1: (adj11, peso11); (adj12, peso12); ...
        v2: (adj21, peso21); (adj22, peso22); ...
    Assuma que cada vértice é um inteiro de até 2 dígitos.
*/

void imprimeGrafo(Grafo *grafo)
{
    int nrVertices = obtemNrVertices(grafo);
    for (int v = 0; v < nrVertices; v++)
    {
        Aresta *q = grafo->listaAdj[v];
        while (q != NULL)
        {
            printf("%d: (%d, %.1f); ", v, q->vdest, q->peso);
            q = q->prox;
        }
        printf("\n");
    }
}

int verticeDestino(ApontadorVerticeAdjacente p, Grafo *grafo)
{
    if (p)
        return p->vdest;
}