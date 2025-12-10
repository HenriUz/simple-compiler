# pt-BR
## Descrição
Este projeto contém os códigos para um compilador, usando o Flex para análise léxica, e o Bison para a análise sintática e semântica. O Flex apenas lê os tokens da linguagem, e informa eles para o Bison, informando o valor deles quando necessário, e o Bison vai construindo uma Árvore de Sintaxe Abstrata (AST), que será executada quando o estado inicial for reduzido.

A AST desenvolvida trabalha com diferentes tipos de nós (diferenciados por uma variável `type`), e cada nó vai ter valores que fazem sentido para ele (definidos por um `union`). Porém, os tipos de nós podem ser agrupados em dois grandes tipos:
- Executar: executa o código que o nó representa, então é utilizado para nós de ação, como `if`, `while` e declarações;
- Avaliar: utilizado para nós que representam um resultado, como uma expressão matemática ou uma variável, e a ação retorna esse valor.

### Detalhes
Este compilador não está 100% refinado, então alguns erros ainda podem ocorrer em tempo de execução, principalmente na parte da AST. Além disso, ele não está otimizado, pois há muitos usos de funções como `strdup()`, `strcpy()`, `memcpy()` e mais.

## Linguagem
A linguagem para esse compilador segue o comportamento:

```
PROGRAMA
    {declarações}
    {algoritmo}
FIMPROG
```

Ou seja, o arquivo deve conter as palavras reservadas `PROGRAMA` e `FIMPROG` no início e fim respectivamente. Entre essas duas palavras, o programa é dividido em declarações e algoritmo, então não é possível declarar uma variável após alguma sequência do algoritmo aparecer.

Alguns detalhes é que a linguagem não depende de identação, comentários são feitos usando `{}`, e não são aceitos números negativos.

### Declarações
As declarações seguem a sintaxe `TIPO nome`, podendo conter mais de um nome se separados por vírgula: `TIPO nome1, nome2, nome3`.

> [!NOTE]
> Não é possível atribuir um valor na declaração.

Os tipos possíveis são: `INTEIRO`, `REAL`, `LISTAINT` e `LISTAREAL`. Todos os tipos reais serão traduzidos para o tipo `double`, e os tipos de lista são vetores que devem possuir `[tamanho]` após o nome deles na declaração (eles devem ser acessados à partir do índice 0).

As variáveis, quando declaradas por meio do nó `decl` da AST, serão salvas no início de uma lista encadeada (não é a estrutura mais eficiente para isso). Porém, há algumas coisas a se atentar:
- É feito verificação se a variável já foi inicializada quando usada;
- Não é feito verifiação se a variável já existe, então qualquer declaração com o mesmo nome ira sobreescrever a variável antiga.

### Algoritmo
O algoritmo possuí alguns comandos possíveis:
- Atribuição: `var := valor`, sendo que o valor pode ser outra variável inicializada, um número direto, ou uma expressão;
- Leitura: `LEIA var` ou `LEIA var1, var2, ...`, nesse caso o código de execução transformará cada `var` em um `scanf()` próprio;
- Escrita: esse código realiza um `printf()`, porém ele só aceita no máximo dois parâmetros: `ESCREVA "frase"`, `ESCREVA var` e `ESCREVA "frase", var`. No caso de dois parâmetros o primeiro deve ser uma frase e o segundo uma variável, e as frases devem ser denotadas com aspas duplas;
- Se: `SE condição ENTAO comando SENAO comando FIMSE`, nesse caso a condição deve ser uma expressão relacional, e o comando qualquer comando do algoritmo, sendo que o `SENAO comando` é opcional;
- Enquanto: `ENQUANTO condição FACA comando FIMENQ`, similar ao `SE`.

E, além desses comandos, há as expressões aritméticas (`+`, `-`, `/`, `*`), que aceitam `()` para forçar precedência, e as relacionais, com os seguintes tokens:
- Maior que: `.MAQ.`;
- Maior or igual: `.MAI.`;
- Menor que: `.MEQ.`;
- Menor ou igual: `.MEI.`;
- Igual: `.IGU.`;
- Diferente: `.DIF.`;
- Negação: `.NAO. (expressão)` (não testado);
- Ou: `.OU.`;
- E: `.E.`.

## Uso
É necessário possuir o `gcc`, `flex` e `bison` instalado (de preferência o `make` também). O `make` possui dois perfis de geração:
- `release` (padrão): compila o compilador normalmente em `/build/compiler`;
- `debug`: adiciona alguns comentários durante a execução a fim de ajudar no debug (feito por meio de `#ifdef`). Compilado em `/build/debug`.

Antes de executar o `make`, certifique-se de ter a pasta `build` criada. E após o make, basta executar o arquivo passando o código como parâmetro:

```bash
./build/compiler main.txt
```

# en-US
## Description
This project contains the code for a compiler, using Flex for lexical analysis and Bison for syntactic and semantic analysis. Flex only reads the language tokens and reports them to Bison, informing their value when necessary, and Bison builds an Abstract Syntax Tree (AST), which will be executed when the initial state is reduced.

The developed AST works with different types of nodes (differentiated by a `type` variable), and each node will have values that make sense for it (defined by a `union`). However, node types can be grouped into two broad types:
- Execute: executes the code that the node represents, so it is used for action nodes, such as `if`, `while`, and statements;
- Evaluate: used for nodes that represent a result, such as a mathematical expression or a variable, and the action returns that value.

### Details
This compiler is not 100% refined, so some errors may still occur at runtime, especially in the AST part. In addition, it is not optimized, as there are many uses of functions such as `strdup()`, `strcpy()`, `memcpy()`, and more.

## Language
The language for this compiler follows this behavior:

```
PROGRAMA
    {statements}
    {algorithm}
FIMPROG
```

That is, the file must contain the reserved words `PROGRAMA` and `FIMPROG` at the beginning and end, respectively. Between these two words, the program is divided into statements and algorithm, so it is not possible to declare a variable after some sequence of the algorithm appears.

Some details are that the language does not depend on indentation, comments are made using `{}`, and negative numbers are not accepted.

### Statements
Statements follow the syntax `TYPE name` and may contain more than one name if separated by commas: `TYPE name1, name2, name3`.

> [!NOTE]
> It is not possible to assign a value in the declaration.

The possible types are: `INTEIRO`, `REAL`, `LISTAINT`, and `LISTAREAL`. All real types will be translated to the `double` type, and list types are vectors that must have `[size]` after their name in the declaration (they must be accessed from index 0).

When declared using the AST node `decl`, variables will be saved at the beginning of a linked list (not the most efficient structure for this). However, there are a few things to keep in mind:
- A check is made to see if the variable has already been initialized when used;
- No check is made to see if the variable already exists, so any declaration with the same name will overwrite the old variable.

### Algorithm
The algorithm has several possible commands:
- Assignment: `var := value`, where the value can be another initialized variable, a direct number, or an expression;
- Reading: `LEIA var` or `LEIA var1, var2, ...`, in which case the execution code will transform each `var` into its own `scanf()`;
- Writing: this code performs a `printf()`, but it only accepts a maximum of two parameters: `ESCREVA "phrase"`, `ESCREVA var`, and `ESCREVA "phrase", var`. In the case of two parameters, the first must be a phrase and the second a variable, and phrases must be denoted with double quotes;
- If: `SE condition ENTAO command SENAO command FIMSE`, in which case the condition must be a relational expression, and the command can be any command in the algorithm, with the `SENAO command` being optional;
- While: `ENQUANTO condition FACA command FIMENQ`, similar to `SE`.

In addition to these commands, there are arithmetic expressions (`+`, `-`, `/`, `*`), which accept `()` to force precedence, and relational expressions, with the following tokens:
- Greater than: `.MAQ.`;
- Greater than or equal to: `.MAI.`;
- Less than: `.MEQ.`;
- Less than or equal to: `.MEI.`;
- Equal: `.IGU.`;
- Different: `.DIF.`;
- Negation: `.NAO. (expression)` (not tested);
- Or: `.OU.`;
- And: `.E.`.

## Use
You must have `gcc`, `flex`, and `bison` installed (preferably `make` as well). `make` has two generation profiles:
- `release` (default): compiles the compiler normally in `/build/compiler`;
- `debug`: adds some comments during execution to help with debugging (done using `#ifdef`). Compiled in `/build/debug`.

Before running `make`, make sure you have created the `build` folder. After running `make`, simply execute the file, passing the code as a parameter:

```bash
./build/compiler main.txt
```

# Exemplo / Example
Lê uma lista de 5 números reais, e calcula a média (considerando apenas números não repetidos), e informa o maior e o menor número.

Read a list of 5 real numbers, calculate the average (considering only non-repeated numbers), and report the largest and smallest numbers.

```txt
PROGRAMA {demonstração}
    LISTAREAL lista[5], analisados[5]
    INTEIRO tamAnalisados, definidos, posicao, posicaoAnalisados, repetido
    REAL media, menor, maior

    tamAnalisados := 0
    definidos := 0
    posicao := 0
    posicaoAnalisados := 0
    repetido := 0
    media := 0.0

    ENQUANTO posicao .MEQ. 5 FACA
        ESCREVA "Digite o elemento da posicao ", posicao
        LEIA lista[posicao]

        ENQUANTO posicaoAnalisados .MEQ. tamAnalisados FACA
            SE analisados[posicaoAnalisados] .IGU. lista[posicao] ENTAO
                ESCREVA "O número digitado já existe, então não será considerado."
                repetido := 1
            FIMSE

            posicaoAnalisados := posicaoAnalisados + 1
        FIMENQ

        SE repetido .DIF. 1 ENTAO
            SE definidos .DIF. 1 ENTAO
                menor := lista[posicao]
                maior := lista[posicao]
                definidos := 1
            SENAO
                SE lista[posicao] .MEQ. menor ENTAO
                    menor := lista[posicao]
                FIMSE

                SE lista[posicao] .MAQ. maior ENTAO
                    maior := lista[posicao]
                FIMSE
            FIMSE

            analisados[tamAnalisados] := lista[posicao]
            tamAnalisados := tamAnalisados + 1
        FIMSE

        repetido := 0
        posicaoAnalisados := 0
        posicao := posicao + 1
    FIMENQ

    ESCREVA "Elementos que serão considerados: "
    ENQUANTO posicaoAnalisados .MEQ. tamAnalisados FACA
        ESCREVA analisados[posicaoAnalisados]
        media := media + analisados[posicaoAnalisados]
        posicaoAnalisados := posicaoAnalisados + 1
    FIMENQ

    media := media / tamAnalisados

    ESCREVA "Menor número: ", menor
    ESCREVA "Maior número: ", maior
    ESCREVA "Média: ", media
FIMPROG
```