# Árvore Geradora Mínima — Algoritmo de Prim

Implementação do algoritmo de Prim para encontrar a Árvore Geradora Mínima (AGM) de um grafo não-direcionado ponderado, com suporte a duas estruturas de dados: **lista de adjacência** e **matriz de adjacência**.

---

## 📚 Conceitos

### Grafo Não-Direcionado Ponderado

Um grafo onde as arestas não têm direção (a conexão entre A e B é a mesma que entre B e A) e cada aresta possui um peso associado.

### Árvore Geradora Mínima (AGM)

Dado um grafo conectado com pesos nas arestas, a AGM é um subconjunto de arestas que:

- Conecta todos os vértices do grafo
- Não forma ciclos
- Possui a menor soma de pesos possível

```
Grafo original:          AGM resultante:

    2       3                2
0 ──── 1 ──── 3         0 ──── 1
│      │                       │
6│     │1          →           │1
│      │                       │
2 ──── 3                       3
    5
```

### Busca em Largura (BFS)

Antes de executar o Prim, o programa verifica se o grafo é **conexo** — ou seja, se existe um caminho entre qualquer par de vértices. Um grafo desconexo não possui AGM, pois é impossível conectar todos os vértices com um único conjunto de arestas.

Para essa verificação, é utilizada a **Busca em Largura (BFS)**, que percorre o grafo a partir do vértice 0, expandindo os vizinhos camada a camada. Cada vértice recebe uma cor que indica seu estado:

- **BRANCO** — ainda não foi visitado
- **CINZA** — foi descoberto mas seus vizinhos ainda estão sendo processados
- **PRETO** — foi completamente processado

Ao final da BFS, se algum vértice ainda estiver **BRANCO**, significa que ele não foi alcançado a partir do vértice 0 — logo o grafo é desconexo e o programa reporta `ERRO: GRAFO NAO CONECTADO` sem executar o Prim.

### Algoritmo de Prim

O Prim constrói a AGM de forma incremental, partindo de um vértice inicial (vértice 0) e expandindo a árvore a cada passo:

1. Inicializa todos os vértices com chave `∞` e insere na fila de prioridades
2. Define a chave do vértice raiz como `0`
3. Enquanto a fila não estiver vazia:
   - Extrai o vértice `u` de menor chave
   - Para cada vizinho `v` de `u`:
     - Se `v` ainda está na fila e `peso(u,v) < chave[v]`:
       - Atualiza `chave[v] = peso(u,v)`
       - Registra `antecessor[v] = u`

### Fila de Prioridades (Min-Heap)

Para eficiência, o Prim utiliza um min-heap como fila de prioridades. Isso garante que a extração do vértice de menor chave seja feita em **O(log V)**, resultando em complexidade total **O((V + E) log V)**.

Sem o heap, seria necessário varrer todos os vértices a cada iteração, resultando em **O(V²)**.

---

## 🗂️ Estrutura do Projeto

```
.
├── ep1-aed2.c        # Arquivo principal (main, leGrafo, AGM_Prim, geraArquivoSaida)
├── grafo_listaadj.c  # Implementação por lista de adjacência
├── grafo_matrizadj.c # Implementação por matriz de adjacência
├── heap.c            # Min-heap para a fila de prioridades
└── bfs.c             # Busca em largura para verificação de conectividade
```

### Detalhes dos arquivos

| Arquivo             | Responsabilidade                                    |
| ------------------- | --------------------------------------------------- |
| `grafo_listaadj.c`  | Estrutura dinâmica com listas encadeadas            |
| `grafo_matrizadj.c` | Estrutura estática com matriz de adjacência         |
| `heap.c`            | Min-heap                                            |
| `bfs.c`             | Busca em Largura para verificar se o grafo é conexo |
| `ep1-aed2.c`        | Leitura do arquivo, validações, Prim e saída        |

---

## 🔧 Compilação

**Lista de adjacência:**

```bash
gcc -o ep1-aed2 ep1-aed2.c -lm
```

**Matriz de adjacência:**

```bash
gcc -DMATRIZ -o ep1-aed2 ep1-aed2.c -lm
```

---

## ▶️ Execução

```bash
./ep1-aed2 arquivo_entrada.txt arquivo_saida.txt
```

---

## 📄 Formato de Entrada

A primeira linha contém o número de vértices e o número de arestas, separados por espaço. Da segunda linha em diante, cada linha descreve uma aresta com o vértice de origem, o vértice de destino e o peso, separados por espaço. Os vértices são indexados de `0` a `|V|-1`.

```
6 10
0 1 6.3
0 2 1.7
0 3 5.4
1 2 2.9
1 4 5.5
2 3 2.6
2 4 6.8
2 5 4.4
3 5 4.2
4 5 3.1
```

---

## 📄 Formato de Saída

**Em caso de grafo válido:**

- Linha 1: custo total da AGM com uma casa decimal
- Linha 2: vértice mais distante da raiz (maior soma de pesos acumulados a partir do vértice 0; em caso de empate, o menor índice)
- Demais linhas: arestas da AGM no formato `v1 v2 peso`, ordenadas por `v1` crescente e em caso de empate por `v2` crescente

```
14.5
4
0 2 1.7
1 2 2.9
2 3 2.6
3 5 4.2
4 5 3.1
```

**Em caso de erro:**

```
ERRO: VERTICE INVALIDO (9)
ERRO: AUTO-LACO (0,0)
ERRO: PESO INVALIDO (-3.0)
ERRO: ARESTA PARALELA (1,2)
```

---

## ⚠️ Validações

O programa detecta e reporta os seguintes erros no arquivo de saída:

| Erro                            | Descrição                                       |
| ------------------------------- | ----------------------------------------------- |
| `ERRO: VERTICE INVALIDO (v)`    | Vértice negativo ou maior que `numVertices - 1` |
| `ERRO: AUTO-LACO (v,v)`         | Aresta conectando um vértice a si mesmo         |
| `ERRO: PESO INVALIDO (p)`       | Peso negativo ou nulo                           |
| `ERRO: ARESTA PARALELA (v1,v2)` | Duas ou mais arestas entre os mesmos vértices   |
| `ERRO: GRAFO NAO CONECTADO`     | Grafo desconexo — AGM não existe                |

Múltiplos erros na mesma aresta são reportados. Os erros são impressos na ordem em que as arestas aparecem no arquivo de entrada.

---

## 🧪 Exemplos

**Entrada válida:**

```
6 10
0 1 6.3
0 2 1.7
...
```

**Saída:**

```
14.5
4
0 2 1.7
...
```

**Entrada com erros:**

```
6 5
0 1 2.0
0 0 1.5
1 2 -3.0
2 9 1.0
1 2 4.0
```

**Saída:**

```
ERRO: AUTO-LACO (0,0)
ERRO: PESO INVALIDO (-3.0)
ERRO: VERTICE INVALIDO (9)
ERRO: ARESTA PARALELA (1,2)
```
