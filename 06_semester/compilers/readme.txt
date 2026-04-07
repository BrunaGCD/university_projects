================================================================================
                    PROJETO 1 - ANALISADOR LÉXICO E SINTÁTICO
================================================================================

---------- GRUPO ----------
Bruna Gonçalves Corte David - RA: 10425696
Júlia Andrade - RA: 10428513

--------------------------------------------------------------------------------
1. SOBRE O PROJETO
--------------------------------------------------------------------------------
Um compilador é responsável por traduzir um código feito em uma linguagem de alto
nível em linguagem de máquina e tem seu processo dividido em 6 fases. Este projeto
tem como objetivo produzir as primeiras duas fases do front-end(análises léxica e
sintática) de um compilador para a linguagem Mini Python. Para isso, fizemos a 
montagem da gramática, das expressões regulares e do autômato finito(base para o
analisador léxico) e do código (analisador léxico e sintático) utilizando a
linguagem C.

--------------------------------------------------------------------------------
2. COMO COMPILAR E EXECUTAR
--------------------------------------------------------------------------------
Compile o arquivo C:
gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador

Execute passando o arquivo de teste.py:
./compilador teste.py

--------------------------------------------------------------------------------
3. DECISÃO DE DESIGN
--------------------------------------------------------------------------------

“{” “}”e ”.” não foram adicionados, pois são utilizados na linguagem para floats,
formatação de strings, dicionários, atributos e objetos. Que não estão inclusos no
escopo do projeto, especificado através do pdf e exemplos.