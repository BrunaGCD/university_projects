================================================================================
      PROJETO 2 - ANALISADOR SEMÂNTICO E GERADOR DE CÓDIGO INTERMEDIÁRIO
================================================================================

---------- GRUPO ----------
Bruna Gonçalves Corte David - RA: 10425696
Júlia Andrade - RA: 10428513


    v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v
     Digite "APRESENTACAO" no Ctrl + F no código do projeto, ao MESMO TEMPO
   que lê este Readme.txt, para ter a explicação essencial e rápida do projeto
    ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^



--------------------------------------------------------------------------------
                         EXPLICAÇÃO GERAL RÁPIDA
--------------------------------------------------------------------------------

A organização do nosso código é, de cima para baixo:
    - Variáveis Globais
    - [ Analisador Léxico ]
    - [ Analisador Semântico ]
    - [ Analisador Sintático ] & [ Gerador de Código Intermediário (G.C.I.) ]
    - Main

Nosso projeto funciona, em geral, da seguinte maneira:

    - Main inicializa o programa chamando o [ Analisador Sintático ] | linha ~1573 |
    
    - O [ Analisador Sintático ] vai chamar o [ Analisador Léxico ] múltiplas
    vezes para obter os tokens/átomos. | linha ~633 |

    - Enquanto o [ Analisador Sintático ] está rodando, o [ G.C.I. ] vai
    gerando o código intermediário ao  mesmo tempo pois utiliza a
    árvore de recursão criada pelo caminho que o [ Analisador Sintático ]
    faz pelas regras da linguagem. | linha ~762 |

    - O [ G.C.I. ] NÃO é um "bloco de código" / "conjunto de funções"
    igual o léxico, sintático e semântico. Ele está embutido DENTRO do
    [ Analisador Sintático ], modificando o [ Analisador Sintático ] o 
    suficiente para conseguir passar informações "para baixo" na árvore de 
    recursão (passar info por parâmetro nas funções) e passar informações
    "para cima" na árvore de recursão (return da info nas funções).

    - O [ Analisador Semântico ], diferente do [ G.C.I. ], NÃO está embutido
    dentro do [ Analisador Sintático ]. Ele É um "bloco de código" / "conjunto de funções"
    que fica no topo do projeto e é CHAMADO em todos os trechos do
    [ Analisador Sintático & G.C.I.] que exigem uma análise semântica. | linha ~995

    - O [ Analisador Semântico ] é o responsável por gerenciar a minitabela de
    símbolos e, quando necessário, pode realizar retornos com informações
    guardadas na minitabela de símbolos após ser chamado para fazer uma análise
    semântica. Esses retornos são recebidos pelo [ G.C.I. ]. | linha ~376 |

--------------------------------------------------------------------------------
              DECISÕES ESPECÍFICAS DE IMPLEMENTAÇÃO IMPORTANTES
--------------------------------------------------------------------------------


1. “{” “}” e ”.” não foram adicionados, pois são utilizados na linguagem para floats,
formatação de strings, dicionários, atributos e objetos. Que não estão inclusos no
escopo do projeto, especificado através do pdf e exemplos.


2. No início do projeto, programamos TODAS as funções e TODAS as recursões, inclusive
tuplas, chamadas de função, entre outros. Após confirmarmos em aula que não precisamos
e/ou nem devemos implementar elementos fora os que aparecem nos testes semânticos
disponibilizados mais tarde no Moodle, porém o excesso de implementação ainda seria
avaliado, decidimos remover a Geração de Código Intermediário e/ou Análise Semântica
de algumas funções que ainda existem no Analisador Sintático, assumindo que não
aparecerão nos testes e, consequentemente, não precisam de tratamento para interromper
o código caso aparecessem.
PS: Isso não significa que deixamos de implementar algumas coisas extras que não
apareceram nos testes. Nós usamos o senso comum para decidir o que provavelmente
era implementação excedente e o que fazia sentido manter para a entrega.


3. Nosso projeto simplificou os tipos de dados em 4 nomes: INT, BOOL, INTLIS e BOLLIS,
cada um representando:
    - INT: Número inteiro 
    - BOOL: Booleano 
    - INTLIS: Lista (vetor) de inteiros
    - BOLLIS: Lista (vetor) de booleanos

Inicialmente, também tínhamos implementado INTTUP e BOLTUP para tuplas, porém como
decidimos removê-las (explicação no parágrafo 2 acima), eles não existem mais.

O tipo STRING NÃO EXISTE no corpo do código, então caso apareça ele é barrado pelo
semântico. Porém, ele EXISTE dentro de print. Para fazer esse controle, optamos por
utilizar uma variável global para servir de flag para indicar ao [ Analisador Semântico ]
quando ele pode ou não permitir a existência de strings, sendo que essa flag só é 
ativada e desligada pela função de print | linha ~88 | linha ~738 | linha ~545 |


4. Para gerar o código intermediário da DECLARAÇÃO/ATRIBUIÇÃO de VETORES, nós precisávamos
fazer a seguinte escrita no arquivo instrucoes.txt (ex: x = [ 1 , 2 , 3 ]) :
x[0] = 1
x[1] = 2
x[2] = 3
x = x

A dificuldade que enfrentamos foi o fato de que o a função que identifica os elementos no
vetor e contabiliza os índices dos elementos para gerar essa saída sequencial NÃO sabe 
qual é o NOME/LEXEMA do vetor que ela está montando. Resumidamente, é só isso que ela
conseguia ver:
[0] = 1
[1] = 2
[2] = 3

Pra contornar esse problema, poderíamos simplesmente ter passado o NOME/LEXEMA do vetor
por parâmetro por TODAS as funções que fazem parte da recursão (que são várias) até 
chegar na função específica responsável por gerar essa sequência.
O problema, que nos incomodou, foi que esse era o único caso precisava passar uma 
informação por parâmetro nessa sequência longa de funções, que também é usada por
muitas outras funções, apenas para imprimir um nome/lexema do lado de uma sequência.
Por isso, simplificamos a implementação com uma flag global para conseguir guardar o 
nome/lexema de um vetor sendo DECLARADO, por questões de organização e deixar o código
menos confuso. | linha ~103 | linha ~873 | linha ~959 |


5. Para lidar com variáveis que estão dentro de um escopo (if/elif/else, while),
utilizamos uma variável flag global chamada flag_dentro_de_escopo, sempre ativada
e desligada pelas funções responsáveis por criar escopos (if/elif/else, while) para 
lidar ESPECIFICAMENTE com DECLARAÇÃO de identificadores/variáveis dentro de um escopo
local. Isso porque o escopo local sempre vai ter no máximo 1 linha de tamanho, logo,
sempre que um novo escopo é inicializado ele também será instantaneamente terminado,
permitindo assim o uso dessa implementação simples e direta.
O [ Analisador Semântico ], quando for lidar com a atribuição de uma nova variável/
identificador dentro de um escopo, vai adicioná-la na minitabela de símbolos com
um porém: Ela já é adicionada com a flag_existe = 0, ou seja, ela é adicionada como
se fosse o registro de uma variável/identificador que existiu mas já foi deletada,
porque sabemos que a próxima linha já não pertencerá mais ao escopo e que ela realmente
instantaneamente não existirá mais. | linha ~89 | linha ~412 |