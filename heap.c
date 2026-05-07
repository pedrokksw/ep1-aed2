#include <float.h>
#include <stdlib.h>
#include <stdio.h>

#define INFINITO_FLOAT FLT_MAX

/*
    (aula 10 e 11 - arvore geradora minima e algoritmo de prim)
    O Algoritmo de Prim é um algoritmo guloso, ou seja, ele "confia" que a menor aresta
    disponível é a escolha certa, e isso só é possível, como vimos na implementação da Busca em
    Largura (bfs.c), por que temos um grafo conectado/conexo.

    É justamente essa lógica gulosa que impõe uma exigência estrutural:
    a cada passo, o algoritmo precisa responder uma única pergunta: "qual é a aresta mais barata
    para expandir a árvore agora?", e precisa respondê-la repetidamente, com os candidatos mudando
    a cada iteração conforme novos vértices são descobertos e pesos são atualizados.

    O que o Algoritmo de Prim precisa é de uma estrutura que mantenha os candidatos permanentemente organizados
    por peso, suporte a entrada de novos elementos e permita atualizar prioridades quando uma aresta
    mais barata é encontrada. Essa estrutura é a fila de prioridades implementada como um heap. Com ela,
    o menor candidato está sempre na raiz, pronto para ser retirado.
*/

typedef struct
{
    int vertice;
    float chave;
} NOH;

typedef struct
{
    NOH *nos;
    int *posicao;
    int tamanho;
} Heap;

/*
    Heap *inicializaHeap(int n):
    inicializa o heap com todos os vertices = INFINITO_FLOAT
*/
Heap *inicializaHeap(int n)
{
    Heap *h = malloc(sizeof(Heap));
    h->nos = malloc(n * sizeof(NOH));
    h->posicao = malloc(n * sizeof(int));
    h->tamanho = n;

    for (int i = 0; i < n; i++)
    {
        h->nos[i].vertice = i;
        h->nos[i].chave = INFINITO_FLOAT;
        h->posicao[i] = i;
    }
    return h;
}

/*
    void troca(Heap *h, int i, int j):
    atualiza a posição dos vértices envolvido.
*/
void troca(Heap *h, int i, int j)
{
    h->posicao[h->nos[i].vertice] = j;
    h->posicao[h->nos[j].vertice] = i;

    NOH tmp = h->nos[i];
    h->nos[i] = h->nos[j];
    h->nos[j] = tmp;
}

/*
    void sobePosicao(Heap *h, int i):
    Compara o nó na posição i com seu pai ((i-1)/2) e sobe enquanto for menor que ele.
    Enquanto o increase-key do max-heap (que vimos nos slides) sobe um elemento enquanto
    ele for maior que seu pai, o sobePosicao do min-heap faz o inverso: sobe um elemento
    enquanto ele for menor que seu pai, porque no min-heap a propriedade a manter é que
    o pai sempre é menor que os filhos.

*/
void sobePosicao(Heap *h, int i)
{
    while (i > 0)
    {
        int pai = (i - 1) / 2;
        if (h->nos[pai].chave > h->nos[i].chave)
        {
            troca(h, pai, i);
            i = pai;
        }
        else
            break;
    }
}

/*
    void desceRaiz(Heap *h):
    Essa função o desce o ultimo nó do heap, comparando com os filhos esquerdo (2*i+1) e
    direito (2*i+2), trocando sempre com o menor deles até encontrar sua posição correta.
*/
void desceRaiz(Heap *h)
{
    int i = 0;
    int n = h->tamanho;
    while (1)
    {
        int menor = i;
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;

        if (esq < n && h->nos[esq].chave < h->nos[menor].chave)
            menor = esq;
        if (dir < n && h->nos[dir].chave < h->nos[menor].chave)
            menor = dir;

        if (menor != i)
        {
            troca(h, i, menor);
            i = menor;
        }
        else
            break;
    }
}

/*
    NOH extraiMin(Heap *h):
    remove o vértice de menor chave e o retorna a raíz.
    Guarda a raiz, coloca o último nó no lugar dela, decrementa o
    tamanho e chama desceRaiz e retorna a raiz.
*/
NOH extraiMin(Heap *h)
{
    NOH raiz = h->nos[0];
    h->tamanho--;

    h->nos[0] = h->nos[h->tamanho];
    h->posicao[h->nos[0].vertice] = 0;
    desceRaiz(h);

    return raiz;
}

/*
    void diminuiChave(Heap *h, int vertice, float novaChave):
    aqui temos o "decrease-key", o inverso ao increase-key que vimos nos slides.
        increase-key (max-heap): aumenta a chave de um elemento, que pode ter ficado maior que o pai, então sobe
        diminuiChave (min-heap): diminui a chave de um elemento, que pode ter ficado menor que o pai, então sobe

    os dois sobem, mas por razões opostas. No max-heap o elemento sobe porque ficou grande demais para
    estar abaixo do pai. No min-heap o elemento sobe porque ficou pequeno demais para estar abaixo do pai.
    E os contextos de uso também são inversos: o increase-key é chamado quando um peso aumenta, o diminuiChave
    quando um peso diminui, que é exatamente o que deve acontecer no Algoritmo de Prim ao encontrar uma aresta mais barata
    para um vértice.
*/

void diminuiChave(Heap *h, int vertice, float novaChave)
{
    int i = h->posicao[vertice];
    h->nos[i].chave = novaChave;
    sobePosicao(h, i);
}

/*
    int estaNoHeap(Heap *h, int vertice):
    a cada iteração um vértice é extraído via extraiMin e incorporado à AGM. Mas o grafo inteiro ainda está na
    memória, e, ao percorrer as arestas de um vértice recém-extraído, o Algoritmo de Prim encontra vizinhos que
    podem já ter sido extraídos em iterações anteriores. Sem o estaNoHeap, o Prim tentaria chamar diminuiChave
    em vértices que já saíram do heap, o que corromperia a estrutura, pois o posicao[] desses vértices aponta
    para índices além do tamanho ativo.

    simplificando, ele "confere" a cor do vértice (que vimos na busca em largura) e retorna pro
    Algoritmo de Prim o estado dele - se ele já foi visitado ou não - ou seja, se ele ainda está no heap, para
    impedir que o Algoritmo de Prim processe um vértice que já pertence à AGM.
*/
int estaNoHeap(Heap *h, int vertice)
{
    return h->posicao[vertice] < h->tamanho;
}