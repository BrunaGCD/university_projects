/*

---------- GRUPO ----------
Bruna Gonçalves Corte David - RA: 10425696
Júlia Andrade - RA: 10428513




        vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
>>>>>> Digite "MUDANÇA" no Ctrl + F para visualizar rapidamente as correções feitas no código <<<<<
        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^




*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ==============================================================================================
//                                            GLOBAIS
// ==============================================================================================

// Definição dos tipos de tokens
typedef enum {

    // Outros
    ERRO, EOS,
    
    // Tipos de dados
    NUMERO, BOOLEANO, STRING,
    
    IDENTIFICADOR,
    
    // Palavras reservadas
    IF, ELSE, ELIF, WHILE, FOR, IN, RANGE, PRINT,
    INPUT, LEN, AND, OR, NOT, IS, DEF, RETURN,
    BREAK, EXEC,
    
    // Operadores
    ATRIBUICAO, SOMA, SUB, MULT, DIV, MOD, EXP, BIT_NOT,
    IGUAL, DIFERENTE, MENOR, MAIOR, MENOR_IGUAL, MAIOR_IGUAL,

    // Delimitadores
    DOIS_PONTOS, PONTO, VIRGULA, PONTO_VIRGULA,
    ABRE_PAR, FECHA_PAR, ABRE_COL, FECHA_COL, 
    ABRE_CHAVE, FECHA_CHAVE

} TAtomo;

// Estrutura do token
typedef struct {
    TAtomo tipo;
    char lexema[100];
    int linha;
} TInfoAtomo;

// Variáveis globais
FILE *fonte;
FILE *saida;
int linhaAtual = 1;

TInfoAtomo lista_identificadores[100];            // [MUDANÇA] Adicionado array de identificadores que associa cada um à um ID para impressão correta e detecção de CaseSensitive
int total_identificadores = 0;



// ----------------------------------------------------------------------------------------------
//                              FUNÇÕES AUXILIARES DO ANALISADOR LÉXICO
// ----------------------------------------------------------------------------------------------

// Função para auxiliar a definir se um lexema é token NUMERO
int eh_numero(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;                             // Não é um número se encontrar um caractere que não seja dígito
        }
    }
    return 1;                                     // É um número se todos os caracteres forem dígitos
}

int eh_identificador(const char *str) {
    if (!isalpha(str[0]) && str[0] != '_') {      // Identificadores devem começar com letra ou underscore
        return 0;
    }
    for (int i = 1; str[i] != '\0'; i++) {        // O restante dos caracteres podem ser letras, dígitos ou underscores
        if (!isalnum(str[i]) && str[i] != '_') {
            return 0;                             // Não é um identificador se encontrar um caractere inválido
        }
    }
    return 1;                                     // É um identificador válido
}

// Função para auxiliar impressão do tipo do token
char* retorna_string_tipo(TAtomo tipo) {
    if (tipo == ERRO) return "ERRO";
    else if (tipo == EOS) return "EOS";
    else if (tipo == NUMERO) return "NUMERO";
    else if (tipo == BOOLEANO) return "BOOLEANO";
    else if (tipo == STRING) return "STRING";
    else if (tipo == IDENTIFICADOR) return "IDENTIFICADOR";
    else if (tipo == IF) return "IF";
    else if (tipo == ELSE) return "ELSE";
    else if (tipo == ELIF) return "ELIF";
    else if (tipo == WHILE) return "WHILE";
    else if (tipo == FOR) return "FOR";
    else if (tipo == IN) return "IN";
    else if (tipo == RANGE) return "RANGE";
    else if (tipo == PRINT) return "PRINT";
    else if (tipo == INPUT) return "INPUT";
    else if (tipo == LEN) return "LEN";
    else if (tipo == AND) return "AND";
    else if (tipo == OR) return "OR";
    else if (tipo == NOT) return "NOT";
    else if (tipo == IS) return "IS";
    else if (tipo == DEF) return "DEF";
    else if (tipo == RETURN) return "RETURN";
    else if (tipo == BREAK) return "BREAK";
    else if (tipo == EXEC) return "EXEC";
    else if (tipo == ATRIBUICAO) return "ATRIBUICAO";
    else if (tipo == SOMA) return "SOMA";
    else if (tipo == SUB) return "SUB";
    else if (tipo == MULT) return "MULT";
    else if (tipo == DIV) return "DIV";
    else if (tipo == MOD) return "MOD";
    else if (tipo == EXP) return "EXP";
    else if (tipo == BIT_NOT) return "BIT_NOT";
    else if (tipo == IGUAL) return "IGUAL";
    else if (tipo == DIFERENTE) return "DIFERENTE"; // PS: Tanto '!=' quanto'<>' são considerados operadores de diferente
    else if (tipo == MENOR) return "MENOR";
    else if (tipo == MAIOR) return "MAIOR";
    else if (tipo == MENOR_IGUAL) return "MENOR_IGUAL";
    else if (tipo == MAIOR_IGUAL) return "MAIOR_IGUAL";
    else if (tipo == IS) return "IS";
    else if (tipo == IN) return "IN";
    else if (tipo == DOIS_PONTOS) return "DOIS_PONTOS";
    else if (tipo == PONTO) return "PONTO";
    else if (tipo == VIRGULA) return "VIRGULA";
    else if (tipo == PONTO_VIRGULA) return "PONTO_VIRGULA";
    else if (tipo == ABRE_PAR) return "ABRE_PAR";
    else if (tipo == FECHA_PAR) return "FECHA_PAR";
    else if (tipo == ABRE_COL) return "ABRE_COL";
    else if (tipo == FECHA_COL) return "FECHA_COL";
    else if (tipo == ABRE_CHAVE) return "ABRE_CHAVE";
    else if (tipo == FECHA_CHAVE) return "FECHA_CHAVE";
    else return "DESCONHECIDO";
}

// Função para imprimir os tokens ou erros léxicos
void imprime_token(TInfoAtomo token) {
    if (token.tipo == ERRO) {
        printf("ERRO LÉXICO: linha %d - (token atual: %s)\n", token.linha, token.lexema);
        fprintf(saida, "ERRO LÉXICO: linha %d - (token atual: %s)\n", token.linha, token.lexema);
        exit(1);
    } else if (token.tipo != EOS) {                                           // [MUDANÇA] Impressão correta dos identificadores e seus IDs
        if (token.tipo == IDENTIFICADOR) {                                    // Se for o token for um identificador,
            int id = 0;
            for (int i = 0; i < total_identificadores; i++) {                 // Percorro a lista de identificadores
                if (strcmp(lista_identificadores[i].lexema, token.lexema) == 0) {   // Pego o ID do identificador para impressão (sendo o ID sempre o index na lista)
                    id = i + 1;
                    break;
                }
            }
            printf("%d# IDENTIFICADOR | var %d\n", token.linha, id);          // Impressão especial para identificador
            fprintf(saida, "%d# IDENTIFICADOR | var %d\n", token.linha, id);
        } else {                                                              // Realiza a impressão normal para qualquer outro tipo de token
            printf("%d# %s | %s\n", token.linha, retorna_string_tipo(token.tipo), token.lexema);
            fprintf(saida, "%d# %s | %s\n", token.linha, retorna_string_tipo(token.tipo), token.lexema);
        }
    }
}

// ==============================================================================================
//                                        ANALISADOR LÉXICO
// ==============================================================================================
TInfoAtomo obter_atomo() {

    // ---------------------------- INICIALIZA TOKEN A SER IDENTIFICADO -----------------------------

    TInfoAtomo token;
    char c;
    int i = 0;
    token.lexema[0] = '\0';


    // --------------- EVITA QUEBRA DE LINHA, ESPAÇOS, COMENTÁRIOS E FINAL DO ARQUIVO ---------------

    while ((c = fgetc(fonte)) != EOF) {
        // Ignorar espaços e quebras de linha
        if (c == '\n') {
            linhaAtual++;
            continue;
        }
        if (isspace(c)) continue;

        // Ignorar comentários
        if (c == '#') {
            while ((c = fgetc(fonte)) != '\n' && c != EOF); // Consome até o final da linha
            if (c == '\n') linhaAtual++;                    // Se o final da linha não for EOF, conta a linha
            continue;
        }

        break;
    }

    if (c == EOF) {                                         // Token de final do arquivo
        token.tipo = EOS;
        strcpy(token.lexema, "EOF");
        token.linha = linhaAtual;
        imprime_token(token);
        return token;
    }

    // ----------------------- CASO ESPECIAL: STRINGS (ESPAÇOS SÃO INCLUSOS) ------------------------

    if (c == '"' || c == '\'') {                            // Se for aspas duplas (") ou simples (') é o início de uma string
        char delimitador = c;                               // Se a string começar com ("), ela termina com ("). Se começar com ('), ela termina com (')
        token.lexema[i++] = c;                              // Inicia o lexema com a aspa inicial da string
        while ((c = fgetc(fonte)) != delimitador && c != EOF) { // Enquanto não encontrar a aspa final ou o final do arquivo,
            token.lexema[i++] = c;                          // Monta lexema da string
        }
        if (c == EOF) {                                     // Se chegou no final do arquivo sem encontrar a aspa final, é um erro léxico
            token.tipo = ERRO;
            strcpy(token.lexema, "String não fechada");
            token.linha = linhaAtual;
            imprime_token(token);
            return token;
        }
        token.lexema[i++] = c;                              // Adiciona a aspa final do lexema
        token.lexema[i] = '\0';                             // Finaliza o lexema
        token.tipo = STRING;
        token.linha = linhaAtual;
        imprime_token(token);
        return token;
    }

    // ----------------------------- MONTA LEXEMA ATÉ ENCONTRAR ESPAÇO ------------------------------ [MUDANÇA] Leitura de lexemas agora é feita de espaço em espaço ao invés de caractere por caractere
    token.lexema[i++] = c;                                  // Inicia lexema com caractere atual (já se sabe que não é um espaço)
    while ((c = fgetc(fonte)) != EOF && !isspace(c)) {      // Monta lexema até encontrar o final do arquivo, um espaço ou um quebra linha
        token.lexema[i++] = c;
    }
    token.lexema[i] = '\0';                                 // Finaliza o lexema
    token.linha = linhaAtual;
    if (c == '\n') {                                        // Se o lexema tinha finalizado em um quebra linha, conta +1 linha
        linhaAtual++;
    }

    // -------------------------------------- CLASSIFICA TOKEN -------------------------------------- [MUDANÇA] Classificação de tokens foi adaptada e facilitada agora que sempre obtemos o lexema inteiro

    if (strcmp(token.lexema, "if") == 0) token.tipo = IF;
    else if (strcmp(token.lexema, "else") == 0) token.tipo = ELSE;
    else if (strcmp(token.lexema, "elif") == 0) token.tipo = ELIF;
    else if (strcmp(token.lexema, "while") == 0) token.tipo = WHILE;
    else if (strcmp(token.lexema, "for") == 0) token.tipo = FOR;
    else if (strcmp(token.lexema, "in") == 0) token.tipo = IN;
    else if (strcmp(token.lexema, "range") == 0) token.tipo = RANGE;
    else if (strcmp(token.lexema, "print") == 0) token.tipo = PRINT;
    else if (strcmp(token.lexema, "input") == 0) token.tipo = INPUT;
    else if (strcmp(token.lexema, "len") == 0) token.tipo = LEN;
    else if (strcmp(token.lexema, "and") == 0) token.tipo = AND;
    else if (strcmp(token.lexema, "or") == 0) token.tipo = OR;
    else if (strcmp(token.lexema, "not") == 0) token.tipo = NOT;
    else if (strcmp(token.lexema, "is") == 0) token.tipo = IS;
    else if (strcmp(token.lexema, "def") == 0) token.tipo = DEF;
    else if (strcmp(token.lexema, "return") == 0) token.tipo = RETURN;
    else if (strcmp(token.lexema, "break") == 0) token.tipo = BREAK;
    else if (strcmp(token.lexema, "exec") == 0) token.tipo = EXEC;
    else if (strcmp(token.lexema, "=") == 0) token.tipo = ATRIBUICAO;
    else if (strcmp(token.lexema, "+") == 0) token.tipo = SOMA;
    else if (strcmp(token.lexema, "-") == 0) token.tipo = SUB;
    else if (strcmp(token.lexema, "*") == 0) token.tipo = MULT;
    else if (strcmp(token.lexema, "/") == 0) token.tipo = DIV;
    else if (strcmp(token.lexema, "%") == 0) token.tipo = MOD;
    else if (strcmp(token.lexema, "**") == 0) token.tipo = EXP;
    else if (strcmp(token.lexema, "~") == 0) token.tipo = BIT_NOT;
    else if (strcmp(token.lexema, "==") == 0) token.tipo = IGUAL;
    else if (strcmp(token.lexema, "!=") == 0 || strcmp(token.lexema, "<>") == 0) token.tipo = DIFERENTE; // PS: Tanto '!=' quanto'<>' são considerados operadores de delse iferente
    else if (strcmp(token.lexema, "<") == 0) token.tipo = MENOR;
    else if (strcmp(token.lexema, ">") == 0) token.tipo = MAIOR;
    else if (strcmp(token.lexema, "<=") == 0) token.tipo = MENOR_IGUAL;
    else if (strcmp(token.lexema, ">=") == 0) token.tipo = MAIOR_IGUAL;
    else if (strcmp(token.lexema, ":") == 0) token.tipo = DOIS_PONTOS;
    else if (strcmp(token.lexema, ".") == 0) token.tipo = PONTO;
    else if (strcmp(token.lexema, ",") == 0) token.tipo = VIRGULA;
    else if (strcmp(token.lexema, ";") == 0) token.tipo = PONTO_VIRGULA;
    else if (strcmp(token.lexema, "(") == 0) token.tipo = ABRE_PAR;
    else if (strcmp(token.lexema, ")") == 0) token.tipo = FECHA_PAR;
    else if (strcmp(token.lexema, "[") == 0) token.tipo = ABRE_COL;
    else if (strcmp(token.lexema, "]") == 0) token.tipo = FECHA_COL;
    else if (strcmp(token.lexema, "{") == 0) token.tipo = ABRE_CHAVE;
    else if (strcmp(token.lexema, "}") == 0) token.tipo = FECHA_CHAVE;
    else if (strcmp(token.lexema, "True") == 0 || strcmp(token.lexema, "False") == 0) token.tipo = BOOLEANO;
    else if (eh_numero(token.lexema)) token.tipo = NUMERO;
    else if (eh_identificador(token.lexema)) token.tipo = IDENTIFICADOR;
    else token.tipo = ERRO; // Se não se encaixar em nenhum dos casos anteriores, é um erro léxico

    // -------------------------- CASO ESPECIAL: TOKEN É UM IDENTIFICADOR --------------------------- [MUDANÇA] Cada NOVO identificador encontrado é guardado no array de identificadores para associá-lo à um ID

    if (token.tipo == IDENTIFICADOR) {                      // No caso de ser identificador, é guardado num array para associà-lo à um ID
        int flag = 0;
        for (int j = 0; j < total_identificadores; j++) {   // Verifica se identificador já foi guardado antes 
            if (strcmp(lista_identificadores[j].lexema, token.lexema) == 0) {
                flag = 1;
                break;
            }
        }
        if (!flag) {                                        // Se não foi guardado ainda, guarda o identificador
            if (total_identificadores < 100) {
                lista_identificadores[total_identificadores] = token; // Guarda o identificador
                total_identificadores++;
            }
        }
    }

    // ---------------------------------- IMPRIME E RETORNA TOKEN -----------------------------------

    imprime_token(token);
    return token;
}



// ==============================================================================================
//                                      ANALISADOR SINTÁTICO
// ==============================================================================================

// Variável global lookahed do Sintático
TInfoAtomo lookahead;

// Protótipos das funções (necessário porque elas chamam umas às outras)
void lista_instrucoes();
void instrucao();
void iff();
void whilee();
void forr();
void print();
void input();
void input_aux();
void def();
void exec();
void breakk();
void identificadores();
void lista_imprimivel();
void chama_id();
void lista_argumentos();
void expressao();
void exp_or();
void exp_and();
void exp_comp();
void exp_aritmetica();
void termo();
void fator();
void primario();
void primario_aux();
void lista_declaracao();
void op_comp();
void op_soma();
void op_mult();
void op_unico();
void tupla();
void tupla_aux();
void exp_aritmetica_aux();
void if_aux();
void lista_instrucoes_aux();
void def_aux();
void lista_declaracao_aux();
void print_aux();
void lista_ids();
void lista_ids_aux();
void lista_imprimivel_aux();
void chama_id_aux();
void chamada_funcao_aux();
void identificadores_aux();
void lista_argumentos_aux();
void exp_or_aux();
void exp_and_aux();
void exp_comp_aux();
void termo_aux();
void elif();


// -------------------- FUNÇÃO AUXILIAR PARA IMPRESSÃO DOS ERROS SINTÁTICOS  --------------------

void erro_sintatico(const char *msg) {
    printf("ERRO SINTATICO: linha %d - %s (token atual: %s)\n", lookahead.linha, msg, lookahead.lexema);
    exit(1);
}


// -------------------------------------- FUNÇÃO CONSOME  ---------------------------------------

void consome(TAtomo esperado) { // Verifica se o token atual é o esperado e avança para o próximo
    if (lookahead.tipo == esperado) {
        lookahead = obter_atomo();
    } else {
        erro_sintatico("Token inesperado");
    }
}

// ------------------------------ INÍCIO DO ANALISADOR SINTÁTICO  -------------------------------

// [MUDANÇA] Gramática foi corrigida e adaptada para não ter recursão à esquerda, assim como visto em aula. Recomendo visualizá-la por completo para perceber a presença dos não-terminais auxiliares e epsilon.
// [MUDANÇA] Percebemos tardiamente que as palavras reservadas que NÃO estavam em NEGRITO na descrição do projeto eram não-obrigatórias, então algumas estão inclusas e outras não estão.
// [MUDANÇA] Corrigimos as mensagens de erro, que agora sempre apresentam corretamente a causa real de um erro sintático.

void lista_instrucoes() {                            // LISTA_INSTRUCOES -> INSTRUCAO LISTA_INSTRUCOES_AUX

    if (lookahead.tipo == EOS) {                     // Retorna se chegou no final do arquivo
        return;
    }

    instrucao();
    lista_instrucoes_aux();
}

void lista_instrucoes_aux() {                        // LISTA_INSTRUCOES_AUX -> LISTA_INSTRUCOES | ε
    if (lookahead.tipo == IDENTIFICADOR || lookahead.tipo == IF || lookahead.tipo == WHILE || lookahead.tipo == FOR || lookahead.tipo == PRINT || lookahead.tipo == LEN || lookahead.tipo == DEF || lookahead.tipo == EXEC || lookahead.tipo == BREAK) {
        lista_instrucoes();                          // LISTA_INSTRUCOES_AUX -> LISTA_INSTRUCOES
    }
    //                                               // LISTA_INSTRUCOES_AUX -> ε
}

void instrucao() {                                   // INSTRUCAO -> IDENTIFICADORES | PRINT | BREAK | EXEC | DEF | IF | WHILE | FOR
    if (lookahead.tipo == IDENTIFICADOR) {
        identificadores();
    } else if (lookahead.tipo == PRINT) {
        print();
    } else if (lookahead.tipo == BREAK) {
        breakk();
    } else if (lookahead.tipo == EXEC) {
        exec();
    } else if (lookahead.tipo == DEF) {
        def();
    } else if (lookahead.tipo == IF) {
        iff();
    } else if (lookahead.tipo == WHILE) {
        whilee();
    } else if (lookahead.tipo == FOR) {
        forr();
    } else {
        erro_sintatico("Instrução não reconhecida"); // ERRO SINTÁTICO
    }
}

void breakk() {                                      // BREAK -> break
    consome(BREAK);
}

void input() {                                      // INPUT -> input '(' INPUT_AUX ')'
    consome(INPUT);
    consome(ABRE_PAR);
    input_aux();
    consome(FECHA_PAR);
}

void input_aux() {                                   // INPUT_AUX -> EXPRESSAO | ε
    if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        expressao();
    }
    //                                               // INPUT_AUX -> ε
}

void def() {                                         // DEF -> def IDENTIFICADOR '(' DEF_AUX
    consome(DEF);
    consome(IDENTIFICADOR);
    consome(ABRE_PAR);
    def_aux();
}

void def_aux() {                                     // DEF_AUX -> LISTA_IDS ')' ':' INSTRUCAO | ')' ':' INSTRUCAO
    if (lookahead.tipo == IDENTIFICADOR) {
        lista_ids();
        consome(FECHA_PAR);
        consome(DOIS_PONTOS);
        instrucao();
    } else if (lookahead.tipo == FECHA_PAR) {
        consome(FECHA_PAR);
        consome(DOIS_PONTOS);
        instrucao();
    } else {
        erro_sintatico("Esperado ')' ou identificador"); // ERRO SINTÁTICO
    }
}

void exec() {                                        // EXEC -> exec '(' EXPRESSAO ')'
    consome(EXEC);
    consome(ABRE_PAR);
    expressao();
    consome(FECHA_PAR);
}

void print() {                                       // PRINT -> print '(' PRINT_AUX
    consome(PRINT);
    consome(ABRE_PAR);
    print_aux();
}

void print_aux() {                                   // PRINT_AUX -> ')' | LISTA_IMPRIMIVEL ')'
    if (lookahead.tipo == FECHA_PAR) {
        consome(FECHA_PAR);
    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        lista_imprimivel();
        consome(FECHA_PAR);
    } else {
        erro_sintatico("Esperado ')' ou expressão"); // ERRO SINTÁTICO
    }
}

void iff(){                                          // IF -> if EXPRESSAO ':' INSTRUCAO IF_AUX
    consome(IF);
    expressao();
    consome(DOIS_PONTOS);
    instrucao();
    if_aux();
}

void if_aux() {                                      // IF_AUX -> ELIF | else ':' INSTRUCAO | ε
    if (lookahead.tipo == ELIF) {
        elif();
    }
    if (lookahead.tipo == ELSE) {
        consome(ELSE);
        consome(DOIS_PONTOS);
        instrucao();
    }
    //                                               // IF_AUX -> ε
}

void elif() {                                        // ELIF -> elif EXPRESSAO ':' INSTRUCAO IF_AUX  
    consome(ELIF);
    expressao();
    consome(DOIS_PONTOS);
    instrucao();
    if_aux();
}

void whilee() {                                      // WHILE -> while EXPRESSAO ':' INSTRUCAO
    consome(WHILE);
    expressao();
    consome(DOIS_PONTOS);
    instrucao();
}

void forr() {                                        // FOR -> for IDENTIFICADOR in range '(' EXPRESSAO ')' ':' INSTRUCAO
    consome(FOR);
    consome(IDENTIFICADOR);
    consome(IN);
    consome(RANGE);
    consome(ABRE_PAR);
    expressao();
    consome(FECHA_PAR);
    consome(DOIS_PONTOS);
    instrucao();
}

void identificadores() {                             // IDENTIFICADORES -> IDENTIFICADOR IDENTIFICADORES_AUX
    consome(IDENTIFICADOR);
    identificadores_aux();
}

void identificadores_aux() {                         // IDENTIFICADORES_AUX -> '=' EXPRESSAO | '[' EXPRESSAO ']' '=' EXPRESSAO | '(' CHAMADA_FUNCAO_AUX)
    if (lookahead.tipo == ATRIBUICAO) {
        consome(ATRIBUICAO);
        expressao();
    } else if (lookahead.tipo == ABRE_COL) {
        consome(ABRE_COL);
        expressao();
        consome(FECHA_COL);
        consome(IGUAL);
        expressao();
    } else if (lookahead.tipo == ABRE_PAR) {
        consome(ABRE_PAR);
        chamada_funcao_aux();
    } else {
        erro_sintatico("Esperado '=', '[' ou '('");  // ERRO SINTÁTICO
    }
}

void chamada_funcao_aux() {                          // CHAMADA_FUNCAO_AUX -> ')' | LISTA_ARGUMENTOS ')'
    if (lookahead.tipo == FECHA_PAR) {
        consome(FECHA_PAR);
    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        lista_argumentos();
        consome(FECHA_PAR);
    } else {
        erro_sintatico("Esperado ')' ou expressão"); // ERRO SINTÁTICO
    }
}

void lista_ids() {                                   // LISTA_IDS -> IDENTIFICADOR LISTA_IDS_AUX
    consome(IDENTIFICADOR);
    lista_ids_aux();
}

void lista_ids_aux() {                               // LISTA_IDS_AUX -> ',' LISTA_IDS | ε
    if (lookahead.tipo == VIRGULA) {
        consome(VIRGULA);
        lista_ids();
    }
    //                                               // LISTA_IDS_AUX -> ε
}

void lista_imprimivel() {                            // LISTA_IMPRIMIVEL -> EXPRESSAO LISTA_IMPRIMIVEL_AUX
    expressao();
    lista_imprimivel_aux();
}

void lista_imprimivel_aux() {                        // LISTA_IMPRIMIVEL_AUX -> ',' LISTA_IMPRIMIVEL | ε
    if (lookahead.tipo == VIRGULA) {
        consome(VIRGULA);
        lista_imprimivel();
    }
    //                                               // LISTA_IMPRIMIVEL_AUX -> ε
}

void lista_argumentos() {                            // LISTA_ARGUMENTOS -> EXPRESSAO LISTA_ARGUMENTOS_AUX
    expressao();
    lista_argumentos_aux();
}

void lista_argumentos_aux() {                        // LISTA_ARGUMENTOS_AUX -> ',' LISTA_ARGUMENTOS | ε
    if (lookahead.tipo == VIRGULA) {
        consome(VIRGULA);
        lista_argumentos();
    }
    //                                               // LISTA_ARGUMENTOS_AUX -> ε
}

void expressao() {                                   // EXPRESSAO -> EXP_OR
    exp_or();
}

void exp_or() {                                      // EXP_OR -> EXP_AND EXP_OR_AUX
    exp_and();
    exp_or_aux();
}

void exp_or_aux() {                                  // EXP_OR_AUX -> or EXP_AND EXP_OR_AUX | ε
    if (lookahead.tipo == OR) {
        consome(OR);
        exp_and();
        exp_or_aux();
    }
    //                                               // EXP_OR_AUX -> ε
}

void exp_and() {                                     // EXP_AND -> EXP_COMP EXP_AND_AUX
    exp_comp();
    exp_and_aux();
}

void exp_and_aux() {                                 // EXP_AND_AUX -> and EXP_COMP EXP_AND_AUX | ε
    if (lookahead.tipo == AND) {
        consome(AND);
        exp_comp();
        exp_and_aux();
    }
    //                                               // EXP_AND_AUX -> ε
}

void exp_comp() {                                    // EXP_COMP -> EXP_ARITMETICA EXP_COMP_AUX
    exp_aritmetica();
    exp_comp_aux();
}

void exp_comp_aux() {                                // EXP_COMP_AUX -> OP_COMP EXP_ARITMETICA EXP_COMP_AUX | ε
    if (lookahead.tipo == IGUAL || lookahead.tipo == DIFERENTE || lookahead.tipo == MENOR || lookahead.tipo == MAIOR || lookahead.tipo == MENOR_IGUAL || lookahead.tipo == MAIOR_IGUAL || lookahead.tipo == IS || lookahead.tipo == IN) {
        op_comp();
        exp_aritmetica();
        exp_comp_aux();
    }
    //                                               // EXP_COMP_AUX -> ε
}

void exp_aritmetica() {                              // EXP_ARITMETICA -> TERMO EXP_ARITMETICA_AUX
    termo();
    exp_aritmetica_aux();
}

void exp_aritmetica_aux() {                          // EXP_ARITMETICA_AUX -> OP_SOMA TERMO EXP_ARITMETICA_AUX | ε
    if (lookahead.tipo == SOMA || lookahead.tipo == SUB) {
        op_soma();
        termo();
        exp_aritmetica_aux();
    }
    //                                               // EXP_ARITMETICA_AUX -> ε
}

void termo() {                                       // TERMO -> FATOR TERMO_AUX
    fator();
    termo_aux();
}

void termo_aux() {                                   // TERMO_AUX -> OP_MULT FATOR TERMO_AUX | ε
    if (lookahead.tipo == MULT || lookahead.tipo == DIV || lookahead.tipo == MOD) {
        op_mult();
        fator();
        termo_aux();
    }
    //                                               // TERMO_AUX -> ε
}

void fator() {                                       // FATOR -> OP_UNICO FATOR | PRIMARIO
    if (lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        op_unico();
        fator();
    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR) {
        primario();
    } else {
        erro_sintatico("Esperado operador único ou primário"); // ERRO SINTÁTICO
    }
}

void primario() {                                    // PRIMARIO -> NUMERO | BOOLEANO | STRING | CHAMA_ID | len '(' IDENTIFICADOR ')' | INPUT | LISTA_DECLARACAO | '(' PRIMARIO_AUX
    if (lookahead.tipo == NUMERO) {
        consome(NUMERO);
    } else if (lookahead.tipo == BOOLEANO) {
        consome(BOOLEANO);
    } else if (lookahead.tipo == STRING) {
        consome(STRING);
    } else if (lookahead.tipo == IDENTIFICADOR) {
        chama_id();
    } else if (lookahead.tipo == LEN) {
        consome(LEN);
        consome(ABRE_PAR);
        consome(IDENTIFICADOR);
        consome(FECHA_PAR);
    } else if (lookahead.tipo == ABRE_COL) {
        lista_declaracao();
    } else if (lookahead.tipo == INPUT) {
        input();
    } else if (lookahead.tipo == ABRE_PAR) {
        consome(ABRE_PAR);
        primario_aux();
    } else {
        erro_sintatico("Esperado número, booleano, string, chamada de ID, len, lista, expressão entre parênteses ou tupla"); // ERRO SINTÁTICO
    }
}

void primario_aux() {                                // PRIMARIO_AUX -> EXPRESSAO ')' | TUPLA
    if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        expressao();
        consome(FECHA_PAR);
    } else {
        tupla();
    }
}

void chama_id() {                                    // CHAMA_ID -> IDENTIFICADOR CHAMA_ID_AUX
    consome(IDENTIFICADOR);
    chama_id_aux();
}

void chama_id_aux() {                                // CHAMA_ID_AUX -> '[' EXPRESSAO ']' | '(' CHAMADA_FUNCAO_AUX | ε
    if (lookahead.tipo == ABRE_COL) {
        consome(ABRE_COL);
        expressao();
        consome(FECHA_COL);
    } else if (lookahead.tipo == ABRE_PAR) {
        consome(ABRE_PAR);
        chamada_funcao_aux();
    }
    //                                               // CHAMA_ID_AUX -> ε
}

void lista_declaracao() {                            // LISTA_DECLARACAO -> '[' LISTA_DECLARACAO_AUX
    consome(ABRE_COL);
    lista_declaracao_aux();
}

void lista_declaracao_aux() {                        // LISTA_DECLARACAO_AUX -> ']' | LISTA_ARGUMENTOS ']'
    if (lookahead.tipo == FECHA_COL) {
        consome(FECHA_COL);
    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        lista_argumentos();
        consome(FECHA_COL);
    } else {
        erro_sintatico("Esperado ']' ou expressão"); // ERRO SINTÁTICO
    }
}

void op_comp() {                                     // OP_COMP -> '==' | '!=' | '<>' | '<' | '>' | '<=' | '>=' | is | in
    if (lookahead.tipo == IGUAL) {
        consome(IGUAL);
    } else if (lookahead.tipo == DIFERENTE) {        // PS: Tanto '!=' quanto'<>' são considerados operadores de diferente
        consome(DIFERENTE);
    } else if (lookahead.tipo == MENOR) {
        consome(MENOR);
    } else if (lookahead.tipo == MAIOR) {
        consome(MAIOR);
    } else if (lookahead.tipo == MENOR_IGUAL) {
        consome(MENOR_IGUAL);
    } else if (lookahead.tipo == MAIOR_IGUAL) {
        consome(MAIOR_IGUAL);
    } else if (lookahead.tipo == IS) {
        consome(IS);
    } else if (lookahead.tipo == IN) {
        consome(IN);
    } else {
        erro_sintatico("Esperado operador de comparação"); // ERRO SINTÁTICO
    }
}

void op_soma() {                                     // OP_SOMA -> '+' | '-'
    if (lookahead.tipo == SOMA) {
        consome(SOMA);
    } else if (lookahead.tipo == SUB) {
        consome(SUB);
    } else {
        erro_sintatico("Esperado '+' ou '-'"); // ERRO SINTÁTICO
    }
}

void op_mult() {                                     // OP_MULT -> '*' | '/' | '%' | '**'
    if (lookahead.tipo == MULT) {
        consome(MULT);
    } else if (lookahead.tipo == DIV) {
        consome(DIV);
    } else if (lookahead.tipo == MOD) {
        consome(MOD);
    } else if (lookahead.tipo == EXP) {
        consome(EXP);
    } else {
        erro_sintatico("Esperado '*', '/', '%' ou '**'"); // ERRO SINTÁTICO
    }
}

void op_unico() {                                    // OP_UNICO -> '+' | '-' | '~' | not
    if (lookahead.tipo == SOMA) {
        consome(SOMA);
    } else if (lookahead.tipo == SUB) {
        consome(SUB);
    } else if (lookahead.tipo == BIT_NOT) {
        consome(BIT_NOT);
    } else if (lookahead.tipo == NOT) {
        consome(NOT);
    } else {
        erro_sintatico("Esperado '+', '-', '~' ou 'not'"); // ERRO SINTÁTICO
    }
}

void tupla() {                                       // TUPLA -> ')' | EXPRESSAO TUPLA_AUX
    if (lookahead.tipo == FECHA_PAR) {
        consome(FECHA_PAR);
    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        expressao();
        tupla_aux();
    } else {
        erro_sintatico("Esperado ')' ou expressão"); // ERRO SINTÁTICO
    }
}

void tupla_aux() {                                   // TUPLA_AUX -> ',' ')' | ',' LISTA_ARGUMENTOS ')'
    if (lookahead.tipo == VIRGULA) {
        consome(VIRGULA);
        if (lookahead.tipo == FECHA_PAR) {
            consome(FECHA_PAR);
        } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
            lista_argumentos();
            consome(FECHA_PAR);
        } else {
            erro_sintatico("Esperado ')' ou expressão"); // ERRO SINTÁTICO
        }
    } else {
        erro_sintatico("Esperado ','"); // ERRO SINTÁTICO
    }
}




// ==============================================================================================
//                                    MAIN PARA IMPRIMIR TESTES
// ==============================================================================================

// argc -> número de argumentos
// argv -> array de strings com os argumentos (sendo argv[0] o nome do programa)
int main(int argc, char *argv[]) {

    // ----------------------------- INICIALIZAÇÃO -----------------------------
    if (argc < 2) {                                                   // Se o arquivo fonte não for passado como argumento, avisa usuário
        printf("Arquivo fonte não foi passado como argumento.\n");
        return 1;
    }

    fonte = fopen(argv[1], "r");                                      // Tenta abrir o arquivo fonte passado como argumento
    if (fonte == NULL) {                                              // Verifica se não deu errado
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    saida = fopen("tokens.txt", "w");                                 // Cria o arquivo de saída para os tokens do analisador léxico
    if (saida == NULL) {                                              // Verifica se não deu errado
        printf("Erro ao criar arquivo de tokens do analisador lexico.\n");
        return 1;
    }
    
    // ============================= RODA PROGRAMA =============================
    lookahead = obter_atomo();                                        // Inicializa o lookahead
    lista_instrucoes();                                               // [MUDANÇA] Chama o analisador sintático diretamente ao invés de usar uma função intermediária

    printf("Compilacao terminada com sucesso!\n");

    fclose(fonte);
    fclose(saida);
    return 0;
}