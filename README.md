# Mandelbrot - Problem C - Paralelização utilizando OpenMP

1. Gabriel Soleira Borges
2. Gustavo Orpinelli de Marco
3. Juliano dos Reis Cruz
4. Vitor Bersani Balan

## 1. Introdução

O objetivo foi pegar o problema **Mandelbrot** inicialmente na versão sequencial e fazermos uma versão do mesmo paralelizada utilizando para isso o OpenMP, e a partir disso avaliar o ganho ou não de desempenho utilizando diferentes quantidades de threads.

## 2. Tradução do enunciado Mandelbrot

Existe uma forma interessante de representar graficamente um conjunto de pontos de uma equação complexa. Normalmente, esse tipo de imagem recebe o nome de **fractal**. O professor Benoit Mandelbrot foi a primeira pessoa que usou um computador para desenhar imagens e visualizar esse conjunto, em 1979.

**Tarefa:** escrever um programa paralelo que gere uma imagem do conjunto de Mandelbrot.

### Entrada

A entrada contém apenas um caso de teste. A primeira linha contém dois inteiros: a largura e a altura da imagem, com `1 <= W, H <= 2^13 - 1`. A próxima linha contém o raio a partir do centro do fractal, com `0 < R <= 2`. A última linha contém um número complexo que representa o centro da imagem do fractal. Esse número complexo é formado por uma parte real e uma parte imaginária, com `-3 <= A, B <= 3`.

A entrada deve ser lida de um arquivo chamado `mandelbrot.in`.

### Saída

A saída contém uma imagem fractal de tamanho `W x H` em formato de texto.

A saída deve ser escrita em um arquivo chamado `mandelbrot.out`.

## 3. Arquivos

Temos os seguintes arquivos e diretórios presentes:
- **versao_sequencial**: diretório contendo todos os arquivos necessários para rodar a versão sequencial do problema
- **versao_paralela**: diretório contendo todos os arquivos necessários para rodar a versão paralela do problema
- **problemset.pdf**: pdf contendo o texto original do problema
- **README.md**: documentação contendo o relatório
- **tempo.txt**: arquivo txt contendo todos os valores obtidos na execução do problema na versão sequencial e nas versões paralelas

## 4. Execução

Para executarmos a versão sequencial, basta seguir a seguinte ordem:
- executar `make all` para o processo de compilação
- executar `time make run` para executar o programa e mostrar o tempo de execução

Para executarmos a versão paralela, basta seguir a seguinte ordem:
- executar `make all` para o processo de compilação
- `export OMP_NUM_THREADS=n` em que **n** corresponde ao número de threads que será usado na execução
- executar `time make run` para executar o programa e mostrar o tempo de execução
- é necessário executar o comando export cada vez que for alterado o número de threads que será utilizando para a execução do programa.

## 5. Estratégia de paralelização

A função que escolhemos para paralelização foi a `mandelbrot::create()` presente no arquivo mandelbrot.cpp. Essa função é a responsável por gerar a imagem do fractal.
Nela, a imagem é percorrida em blocos de tamanho `d`. O algoritmo começa com blocos maiores, e a cada etapa reduz `d` pela metade até chegar ao tamanho mínimo.

Na versão sequencial, o trecho principal era executado por apenas uma thread da seguinte forma:

```cpp
for (int i = 0; i < width; i += d) {
    for (int j = 0; j < height; j += d) {
        ...
    }
}
```

Na versão paralela, adicionamos a diretiva `omp parallel for` no laço externo:

```cpp
#pragma omp parallel for schedule(dynamic)
for (int i = 0; i < width; i += d) {
    for (int j = 0; j < height; j += d) {
        ...
    }
}
```

Aplicamos a paralelização no laço da variavel `i`, que percorre a largura da imagem. Fizemos essa escolha porque cada iteração do lado externo trabalha sobre uma faixa diferente da imagem. Dessa forma, diferentes threads trabalham sobre regiões distintas do fractal ao mesmo tempo

O laço mais interno, indicado pela variável `j` foi mantido sequencial. O motivo disso está no fato de que cada thread recebe uma parte da largura da imagem e assim percorre a altura correspondente normalmente. Essa estratégia faz com que reduza os riscos de conflitos de escrita.

O laço externo de refinamento, indicado pela variável `d`, não foi paralelizado. O motivo disso é que o algoritmo trabalha com etapas sucessivas, começando com blocos grandes e depois refinando a imagem com blocos menores. Como cada etapa depende do valor anterior de `d`, então mantivemos essa parte sequencial

O motivo para usarmos `schedule(dynamic)` foi porque o custo de calcular pontos do conjunto de mandelbrot pode variar. Alguns pontos escapam mais rapidamente, enquanto outros exigem mais iterações. Com o escalonamento feito de maneira dinâmica, quando uma thread termina a sua parte, ela pode receber novas iterações, melhorando o balanceamento da carga de trabalho.

## 6. Alterações realizadas

As principais alterações feitas foram:

- Inclusão da biblioteca OpenMP no arquivo `mandelbrot.cpp`;
- Adição da diretiva `#pragma omp parallel for schedule(dynamic)` no laço externo da função `create()`;
- Alteração do `Makefile` para compilar com a flag `-fopenmp`;
- Execução controlada pela variável de ambiente `OMP_NUM_THREADS`;

## 7. Resultados

Os resultamos foram obtidos rodando em uma máquina com as seguintes especificações:

- Sistema operacional: Zorin OS 18.1
- Processador: Ryzen 5 5600
- Placa de vídeo: RTX 3050TI
- Memória Ram: 32 gb

Medições feitas:

| Versão     | Threads | Tempo real (s) | Speedup |
| ---------- | :-----: | :------------: | :-----: |
| Sequencial | 1       | 6,950          | -       |
| OpenMP     | 1       | 6,857          | 0,998   |
| OpenMP     | 2       | 3,546          | 1,915   |
| OpenMP     | 4       | 1,938          | 3,534   |
| OpenMP     | 8       | 1,184          | 5,785   |
| OpenMP     | 16      | 0,974          | 7,032   |

## 8. Discussão dos speedups

Os resultados mostram que a paralelização do problema **Mandelbrot** trouxe ganho significativo de desempenho. Comparando a execução da versão sequencial em comparação a versão paralelizada com 16 threads, foi atingido um speedup de 7,032 vezes.

Com **1 thread** a versão com OpenMP apresentou um speedup de **0,998**. Isso sugere que a simples presença do OpenMP praticamente não alterou o desempenho em comparação a versão sequencial.

Com **2 threads** a versão com OpenMP apresentou um speepup de **1,915**. Esse valor é muito próximo do valor ideal de 2 threads, em que o ideal teórico seria um speedup de 2 vezes. Isso indica que a parte paralelizada representa grande parte do tempo total da aplicação

Com **4 threads** a versão com OpenMP apresentou um speedup de **3,534**. O ganho ainda é significativo, mas se encontra um pouco mais longe do ideal teórico, que seria de 4 vezes. Isso pode ocorrer por conta dos custos relacionados ao gerenciamento das threads, sincronização de laços paralelos e partes do programa que são sequenciais.

Com **8 threads** a versão com OpenMP apresentou um speedup de **5,785**. O ganho continua crescendo, mas ele diminuiu e se encontra distante do ideal teórico, que seria de 8 vezes. Isso indica que conforme aumentamos o número de threads, a escalabilidade de ganho de desempenho reduz.

Com **16 threads** a versão com OpenMp apresentou um speedup de **7,032**. Com essa quantidade de threads, foi obtido o menor tempo possível para o problema, tivemos assim como com 8 threads, uma redução na escalabilidade de ganho de desempenho.

Essa redução ela é prevista em programas paralelos. Pois mesmo quando a maior parte do processamento relacionado ao problema seja paralelizável, existem trechos sequenciais, existem os custos de criação e gerenciamento de threads, acessos a memória compartilhada entre outras coisas que afetam o desempenho e fazem com que o speedup se encontre longe do ideal teórico.

## 9. Conclusão

Com base nos resultados apresentados, podemos concluir que a paralelização do problema Mandelbrot utilizando OpenMp foi eficiente e apresentou redução significativa no tempo de execulção. Ao paralelizarmos o laço externo da funlão create(), permitimos distribuir o calculo de diferentes regiões da imagem entre múltiplas threads sem alteração da lógica do algoritmo.

O melhor resultado foi obtido utilizando **16 threads**, reduzindo um tempo de execução de **6,950 s** para **0,974 s**. Isso corresponde a um speedup de aproximadamente **7,032 vezes**.

Assim, apesar do speedup não ser linear para grandes quantidades de threads, os resultados obtidos mostram que a aplicação se beneficia bastante com a paralelização, devido ao fato de o cálculo dos pontos da fractal apresentar a parte mais custosa do programa.
