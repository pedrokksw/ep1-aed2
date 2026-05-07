#ifdef MATRIZ
#include "grafo_matrizadj.c"
#else
#include "grafo_listaadj.c"
#endif

#include "heap.c"
#include "bfs.c"

/*
    Struct auxiliar que agrupa os três valores de uma aresta da AGM (vértice origem,
    vértice destino e peso) em uma única estrutura, permitindo criar um vetor de
    arestas para ordenar
*/
typedef struct
{
    int v1, v2;
    float peso;
} ArestaAGM;

/*
    void ordenaPorAresta(ArestaAGM *arestas, int n):
    uso do insertionSort para ordenar as arestas (exibir em ordem crescente conforme exigido)
*/
void ordenaPorAresta(ArestaAGM *arestas, int n)
{
    for (int i = 1; i < n; i++)
    {
        ArestaAGM chave = arestas[i];
        int j = i - 1;

        while (j >= 0 && (arestas[j].v1 > chave.v1 || (arestas[j].v1 == chave.v1 && arestas[j].v2 > chave.v2)))
        {
            arestas[j + 1] = arestas[j];
            j--;
        }
        arestas[j + 1] = chave;
    }
}

/*
    void calculaDistancias(int *antecessor, float *chave_minPeso, int n, float *dist):
    Função para calcular a distância, como exigido na saída. Resgata os caminhos da AGM a partir dos vetores
    antecessor e chave_minPeso gerados pelo Algoritmo de Prim, calculando a soma acumulada de pesos
    do vértice 0 até cada vértice. Começa com dist[0] = 0 (a raiz tem distância zero de s imesma) e
    itera n vezes sobre todos os vértices, a cada vez que encontra
    um vértice v cujo antecessor já tem distância calculada, soma a distância acumulada. As n iterações
    garantem que todos os caminhos sejam somados independente da ordem dos antecessores na árvore.
*/

void calculaDistancias(int *antecessor, float *chave_minPeso, int n, float *dist)
{
    dist[0] = 0;
    for (int v = 1; v < n; v++)
        dist[v] = -1;

    for (int i = 0; i < n; i++)
        for (int v = 0; v < n; v++)
            if (antecessor[v] != -1 && dist[antecessor[v]] >= 0)
                dist[v] = dist[antecessor[v]] + chave_minPeso[v];
}

void geraArquivoSaida(FILE *arquivoSaida, int *antecessor, float *chave_minPeso, int n)
{
    float custoTotal = 0.0f;
    for (int v = 0; v < n; v++)
        if (antecessor[v] != -1)
            custoTotal += chave_minPeso[v];

    fprintf(arquivoSaida, "%.1f\n", custoTotal);

    float *dist = calloc(n, sizeof(float));
    calculaDistancias(antecessor, chave_minPeso, n, dist);

    int maisDistante = 0;
    float maiorDist = dist[0];
    for (int v = 1; v < n; v++)
    {
        if (dist[v] > maiorDist)
        {
            maiorDist = dist[v];
            maisDistante = v;
        }
        /*em caso de empate, o menor vértice é escolhido */
        else if (dist[v] == maiorDist && v < maisDistante)
            maisDistante = v;
    }

    fprintf(arquivoSaida, "%d\n", maisDistante);

    /*
        Percorre o vetor antecessor para reconstruir as arestas da AGM. Para cada vértice v
        com antecessor válido diferente de VERTICE_INAVLIDO, temos uma aresta entre antecessor[v] e v com
        peso chave_minPeso[v]. O if e else garante que v1 < v2, ou seja, o vértice de menor
        índice sempre vem primeiro, conforme exigido no arquivo de saída.
    */
    int numArestas = n - 1;
    ArestaAGM *arestas = malloc(numArestas * sizeof(ArestaAGM));

    int i = 0;
    for (int v = 0; v < n; v++)
    {
        if (antecessor[v] != -1)
        {
            if (antecessor[v] < v)
            {
                arestas[i].v1 = antecessor[v];
                arestas[i].v2 = v;
            }
            else
            {
                arestas[i].v1 = v;
                arestas[i].v2 = antecessor[v];
            }
            arestas[i].peso = chave_minPeso[v];
            i++;
        }
    }
    /*
        Pra ordenar as arestas por v1 crescente e em caso de empate por v2 crescente, novamente, como
        exigido no arquivo de saída.
    */
    ordenaPorAresta(arestas, numArestas);
    for (int i = 0; i < numArestas; i++)
        fprintf(arquivoSaida, "%d %d %.1f\n", arestas[i].v1, arestas[i].v2, arestas[i].peso);

    free(dist);
    free(arestas);
}

/*
    O Algoritmo de Prim constrói a AGM crescendo uma árvore a partir de um vértice inicial, sempre escolhendo
    a aresta mais barata que conecta um vértice já na árvore a um vértice ainda fora dela.
    Todos os vértices entram no heap com chave infinita, nenhum ainda é candidato real. O vértice
    raiz recebe chave 0, sobe para o topo do heap e se torna o ponto de partida.

    A cada iteração o Algoritmo de Prim extrai o vértice de menor chave (a escolha gulosa). Esse vértice é
    incorporado à AGM com a aresta que o conecta ao seu antecessor. Em seguida, o algoritmo percorre
    todos os vizinhos desse vértice: para cada vizinho que ainda está no heap, verifica se a aresta
    encontrada é mais barata do que o melhor peso conhecido até agora para aquele vizinho. Se for,
    atualiza o antecessor, atualiza o peso e chama o diminuiChave para reposicioná-lo no heap.

    O heap nos garante que a escolha gulosa seja sempre eficiente. Sem ele, seria necessário varrer todos os vértices a cada
    iteração para achar o mínimo. Com ele, o menor candidato está sempre na raiz, pronto para ser extraído.

    Para sintetizar tudo o que vimos nas aulas:
        ● Busca em Largura para conferir que o grafo é, de fato, conexo garante que todos os vértices possam ser visitados;
        ● o heap garante que a escolha gulosa seja eficiente;
        ● e o estaNoHeap garante que nenhum vértice seja processado duas vezes.
*/
void AGMPrim(Grafo *grafo, int verticeRaiz, FILE *arquivoSaida)
{
    int n = obtemNrVertices(grafo);

    float *chave_minPeso = malloc(n * sizeof(float));
    int *antecessor = malloc(n * sizeof(int));

    Heap *filaP = inicializaHeap(n);

    for (int u = 0; u < n; u++)
    {
        chave_minPeso[u] = INFINITO_FLOAT;
        antecessor[u] = -1;
    }

    chave_minPeso[verticeRaiz] = 0;
    diminuiChave(filaP, verticeRaiz, 0);

    while (filaP->tamanho > 0)
    {
        NOH u = extraiMin(filaP);
        /*
            Extrai o vértice de menor chave: a escolha gulosa. Na primeira iteração sempre
            será a raiz definida acima.
        */
        for (int v = 0; v < n; v++)
        {
            if (existeAresta(u.vertice, v, grafo))
            {
                float w = obtemPesoAresta(u.vertice, v, grafo);

                /*
                    Aqui entram as duas condições explicadas em heap.c: estaNoHeap garante que v ainda
                    não foi incorporado à AGM (ainda é branca), e w < chave_minPeso[v] é o inverso ao
                    max-heapify da implementação dos slides, garantindo que só atualiza se encontrou uma aresta mais barata.
                */
                if (estaNoHeap(filaP, v) && w < chave_minPeso[v])
                {
                    antecessor[v] = u.vertice;
                    chave_minPeso[v] = w;
                    diminuiChave(filaP, v, w);
                }
            }
        }
    }

    geraArquivoSaida(arquivoSaida, antecessor, chave_minPeso, n);

    free(chave_minPeso);
    free(antecessor);
    free(filaP->nos);
    free(filaP->posicao);
    free(filaP);
}

/*
    LeGrafo(nomearq, Grafo)
    Le o arquivo nomearq e armazena na estrutura Grafo
    Layout do arquivo:
        A 1a linha deve conter o número de vértices e o número de arestas do grafo,
        separados por espaço.
        A 2a linha em diante deve conter a informação de cada aresta, que consiste
        no índice do vértice de origem, índice do vértice de destino e o peso da
        aresta, também separados por espaços.
        Observações:
            Os vértices devem ser indexados de 0 a |V|-1

    Exemplo: O arquivo abaixo contém um grafo com 4 vértices (0,1,2,3) e
    7 arestas.

    4 7
    0 3 6
    2 1 5
    2 0 9
    1 3 1
    0 1 9
    3 1 5
    0 2 7

    Código de saída:
        1: leitura bem sucedida
        0: erro na leitura do arquivo
*/

bool verifica_validade_vertice(int v, int numVertices)
{
    return v < 0 || v >= numVertices;
}

bool leGrafo(char *nomearq, Grafo *grafo, FILE *arquivoSaida)
{
    FILE *fp;
    int nVertices, nArestas;
    int v1, v2;
    Peso peso;
    bool valido = true;

    fp = fopen(nomearq, "r");
    if (!fp)
    {
        fprintf(stderr, "Não foi possível abrir o arquivo %s\n", nomearq);
        return false;
    }
    if (fscanf(fp, "%d %d", &nVertices, &nArestas) != 2)
    {
        fprintf(stderr, "Não foi possível ler o número de vértices e de arestas\n");
        return false;
    }
    /* seria bom você imprimir aqui esses dois valores para ver se leu certo */
    fprintf(stderr, "Numero de vertices = %d e numero de arestas = %d\n", nVertices, nArestas);

    inicializaGrafo(grafo, nVertices);

    bool tentativas[nVertices][nVertices];
    for (int i = 0; i < nVertices; i++)
        for (int j = 0; j < nVertices; j++)
            tentativas[i][j] = false;

    for (int i = 0; i < nArestas; i++)
    {
        if (fscanf(fp, "%d %d %f", &v1, &v2, &peso) != 3)
        {
            fprintf(stderr, "Erro ao ler aresta %d\n", i);
            break;
        }

        bool arestaValida = true;

        /*
            ● "ERRO: VERTICE INVALIDO (<nr inválido do vértice>)", no caso de tentar criar uma
            aresta com um vértice de número inválido (o número inválido deve ser impresso entre
            parêntesis);
        */
        if (verifica_validade_vertice(v1, nVertices) || verifica_validade_vertice(v2, nVertices))
        {
            if (verifica_validade_vertice(v1, nVertices))
                fprintf(arquivoSaida, "ERRO: VERTICE INVALIDO (%d)\n", v1);
            else
                fprintf(arquivoSaida, "ERRO: VERTICE INVALIDO (%d)\n", v2);
            arestaValida = false;
        }

        /*
            ● "ERRO: AUTO-LACO (<vértice>,<vértice>)", no caso de uma aresta conectando um
            vértice a si próprio (o número do vértice que compõe o auto-laço (self-loop), separados
            por vírgula sem espaço, devem ser impressos entre parêntesis);
        */
        if (v1 == v2)
        {
            fprintf(arquivoSaida, "ERRO: AUTO-LACO (%d,%d)\n", v1, v2);
            arestaValida = false;
        }
        /*
            ● "ERRO: ARESTA PARALELA (<vértice de menor número>,<vértice de maior número>)",
            no caso de duas ou mais arestas entre dois vértices (os números dos vértices que
            compõem a aresta paralela, separados por vírgula sem espaço, devem ser impressos
            entre parêntesis, em ordem crescente);
        */
        if (!verifica_validade_vertice(v1, nVertices) && !verifica_validade_vertice(v2, nVertices) && v1 != v2)
        {
            if (tentativas[v1][v2])
            {
                if (v1 < v2)
                    fprintf(arquivoSaida, "ERRO: ARESTA PARALELA (%d,%d)\n", v1, v2);
                else
                    fprintf(arquivoSaida, "ERRO: ARESTA PARALELA (%d,%d)\n", v2, v1);
                arestaValida = false;
            }
            else
            {
                tentativas[v1][v2] = true;
                tentativas[v2][v1] = true;
            }
        }

        /*
            ● "ERRO: PESO INVALIDO (<peso>)", no caso de peso negativo ou nulo (o valor do peso
            deve ser impresso entre parêntesis);
        */
        if (peso <= 0)
        {
            fprintf(arquivoSaida, "ERRO: PESO INVALIDO (%.1f)\n", peso);
            arestaValida = false;
        }

        if (arestaValida)
            insereAresta(v1, v2, peso, grafo);

        if (!arestaValida)
            valido = false;
    }

    /*
        ● "ERRO: GRAFO NAO CONECTADO", no caso de um grafo desconexo;
    */
    bool conectado = verificaGrafoConectado(grafo);
    fprintf(stderr, "Grafo conectado: %d\n", conectado);
    if (valido && !conectado)
    {
        fprintf(arquivoSaida, "ERRO: GRAFO NAO CONECTADO\n");
        valido = false;
    }

    fclose(fp);
    return valido;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE *arquivoSaida = fopen(argv[2], "w");
    if (!arquivoSaida)
    {
        fprintf(stderr, "Não foi possível criar o arquivo %s\n", argv[2]);
        return 1;
    }

    Grafo grafo;

    if (!leGrafo(argv[1], &grafo, arquivoSaida))
    {
        fclose(arquivoSaida);
        return 1;
    }

    AGMPrim(&grafo, 0, arquivoSaida);
    imprimeGrafo(&grafo);

    fclose(arquivoSaida);
    return 0;
}

/*
Lista de Adjacência:
gcc -o ep1 ep1.c -lm

Matriz de Adjacência:
gcc -DMATRIZ -o ep1 ep1.c -lm

Execução do main:
./ep1 arquivo_entrada.txt arquivo_saida.txt
*/