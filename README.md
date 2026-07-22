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

Na versão paralela, utilizamos uma única região paralela envolvendo todas as etapas de refinamento da imagem.

```cpp
#pragma omp parallel
{
    while (d > 0) {

        #pragma omp for schedule(dynamic)
        for (int i = 0; i < width; i += d) {
            for (int j = 0; j < height; j += d) {
                ...
            }
        }

        #pragma omp single
        {
            d /= 2;
        }

        #pragma omp barrier
    }
}
```

Aplicamos a paralelização no laço da variável `i`, que percorre a largura da imagem. Fizemos essa escolha porque cada iteração do laço externo trabalha sobre uma faixa diferente da imagem. Dessa forma, diferentes threads processam regiões distintas do fractal simultaneamente, reduzindo os riscos de conflitos de escrita.

Foi utilizada uma única região paralela envolvendo todo o laço controlado pela variável `d`. Dentro dessa região, utilizamos `#pragma omp for schedule(dynamic)` para distribuir as iterações entre as threads. Ao final de cada etapa, apenas uma thread atualiza o valor de `d` através da diretiva `single`, enquanto a barreira garante que todas as threads concluíram o processamento antes do próximo refinamento.

## 6. Alterações realizadas

As principais alterações feitas foram:

- Inclusão da biblioteca OpenMP no arquivo `mandelbrot.cpp`;
- Criação de uma única região paralela na função `create()`;
- Utilização das diretivas `omp for`, `single` e `barrier` para sincronizar as etapas de refinamento;
- Utilização de `schedule(dynamic)` para balanceamento de carga entre as threads;
- Alteração do `Makefile` para compilar com a flag `-fopenmp`;
- Execução controlada pela variável de ambiente `OMP_NUM_THREADS`.

## 7. Resultados

Os resultamos foram obtidos rodando em uma máquina com as seguintes especificações:

- Sistema operacional: Fedora-44
- Processador: Ryzen 5 8600
- Placa de vídeo: RX 7600
- Memória Ram: 32 gb

Medições feitas:

| Versão     | Threads | Tempo real (s) | Speedup |
| ---------- | :-----: | :------------: | :-----: |
| Sequencial | 1       | 5,977          | -       |
| OpenMP     | 2       | 3,275          | 1,825   |
| OpenMP     | 4       | 1,762          | 3,393   |
| OpenMP     | 8       | 1,286          | 4,648   |
| OpenMP     | 16      | 0,825          | 7,245   |

## 8. Discussão dos speedups

Os resultados mostram que a paralelização do problema **Mandelbrot** trouxe ganho significativo de desempenho. Comparando a execução da versão sequencial em comparação a versão paralelizada com 16 threads, foi atingido um speedup de 7,032 vezes.

Com **2 threads**, a versão paralela apresentou um speedup de **1,825**, valor próximo ao ideal teórico de 2. Isso indica que a maior parte do tempo de execução é dedicada à região paralelizável do algoritmo.

Com **4 threads**, foi obtido um speedup de **3,393**. Embora o ganho permaneça expressivo, ele já se distancia um pouco do valor ideal de 4 devido aos custos associados ao gerenciamento das threads, sincronizações e à parcela sequencial do algoritmo.

Com **8 threads**, o speedup alcançou **4,648**. O desempenho continua melhorando, porém observa-se uma redução na escalabilidade. Isso ocorre porque o aumento do número de threads intensifica custos como sincronização, acesso à memória compartilhada e competição pelos recursos do processador.

Com **16 threads**, foi obtido o melhor resultado, com speedup de **7,245**. A utilização de uma única região paralela ao longo de toda a execução contribuiu para reduzir o overhead de criação e destruição das equipes de threads, permitindo um melhor aproveitamento dos recursos disponíveis. Ainda assim, o speedup permanece abaixo do ideal devido às partes inerentemente sequenciais do algoritmo e às limitações impostas pela Lei de Amdahl.

## 9. Conclusão

Com base nos resultados apresentados, podemos concluir que a paralelização do problema Mandelbrot utilizando OpenMp foi eficiente e apresentou redução significativa no tempo de execução. Ao paralelizarmos o laço externo da funlão create(), permitimos distribuir o calculo de diferentes regiões da imagem entre múltiplas threads sem alteração da lógica do algoritmo.

O melhor resultado foi obtido utilizando **16 threads**, reduzindo um tempo de execução de **5,977 s** para **0,825 s**. Isso corresponde a um speedup de aproximadamente **7,245 vezes**.

Assim, apesar do speedup não ser linear para grandes quantidades de threads, os resultados obtidos mostram que a aplicação se beneficia bastante com a paralelização, devido ao fato de o cálculo dos pontos da fractal apresentar a parte mais custosa do programa.
