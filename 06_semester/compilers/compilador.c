/*

---------- GRUPO ----------
Bruna Gonçalves Corte David - RA: 10425696
Júlia Andrade - RA: 10428513

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ===============================================================================
//                                     GLOBAIS
// ===============================================================================

// Definição dos tipos de tokens
typedef enum {

    // Outros
    ERRO, EOS,
    
    // Tipos de dados
    NUMERO, BOOLEANO, STRING,
    
    IDENTIFICADOR,
    
    // Palavras reservadas
    IF, ELSE, WHILE, FOR, IN, RANGE, PRINT,
    INPUT, LEN, AND, OR, NOT, IS, DEF, RETURN,
    
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

// -------------------------------------------------------------------------------
//                    FUNÇÕES AUXILIARES DO ANALISADOR LÉXICO
// -------------------------------------------------------------------------------

// Função para imprimir os tokens ou erros léxicos
void imprime_token(TInfoAtomo token) {
    if (token.tipo == ERRO) {
        printf("ERRO LÉXICO: linha %d - %s\n", token.linha, token.lexema);
        fprintf(saida, "ERRO LÉXICO: linha %d - %s\n", token.linha, token.lexema);
        exit(1);
    } else if (token.tipo != EOS) {
        printf("%d# %d | %s\n", token.linha, token.tipo, token.lexema);
        fprintf(saida, "%d# %d | %s\n", token.linha, token.tipo, token.lexema);
    }
}

// Função auxiliar para espiar o próximo caractere sem consumi-lo
char peek() {
    char c = fgetc(fonte);
    ungetc(c, fonte);
    return c;
}

// Verifica se é palavra reservada, booleano ou identificador
TAtomo classificar_palavra_ou_id(char *lexema) {
    if (strcmp(lexema, "True") == 0 || strcmp(lexema, "False") == 0) return BOOLEANO;
    if (strcmp(lexema, "if") == 0) return IF;
    if (strcmp(lexema, "else") == 0) return ELSE;
    if (strcmp(lexema, "while") == 0) return WHILE;
    if (strcmp(lexema, "for") == 0) return FOR;
    if (strcmp(lexema, "in") == 0) return IN;
    if (strcmp(lexema, "range") == 0) return RANGE;
    if (strcmp(lexema, "print") == 0) return PRINT;
    if (strcmp(lexema, "input") == 0) return INPUT;
    if (strcmp(lexema, "len") == 0) return LEN;
    if (strcmp(lexema, "and") == 0) return AND;
    if (strcmp(lexema, "or") == 0) return OR;
    if (strcmp(lexema, "not") == 0) return NOT;
    if (strcmp(lexema, "is") == 0) return IS;
    if (strcmp(lexema, "def") == 0) return DEF;
    if (strcmp(lexema, "return") == 0) return RETURN;
    return IDENTIFICADOR;
}

// ===============================================================================
//                               ANALISADOR LÉXICO
// ===============================================================================
TInfoAtomo obter_atomo() {
    TInfoAtomo token;
    token.lexema[0] = '\0';
    char c;
    int i = 0;

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

        // Identificadores e Palavras Reservadas
        if (isalpha(c) || c == '_') {
            token.lexema[i++] = c;                     // Inicia o lexema com a letra ou underscore
            while (isalnum(peek()) || peek() == '_') { // isalnum() = letra ou dígito
                token.lexema[i++] = fgetc(fonte);      // Lê o caractere e avança
            }
            token.lexema[i] = '\0';                    // Finaliza o lexema
            token.tipo = classificar_palavra_ou_id(token.lexema); // Classifica como palavra reservada, booleano ou identificador
            token.linha = linhaAtual;                  // Define a linha do token
            imprime_token(token);
            return token;
        }

        // Números
        if (isdigit(c)) {
            token.lexema[i++] = c;                     // Inicia o lexema com o dígito
            while (isdigit(peek())) {                  // Enquanto o próximo caractere for dígito
                token.lexema[i++] = fgetc(fonte);      // Lê o caractere e avança
            }
            token.lexema[i] = '\0';                    // Finaliza o lexema
            token.tipo = NUMERO;                       // Classifica como número
            token.linha = linhaAtual;                  // Define a linha do token
            imprime_token(token);
            return token;
        }

        // Strings
        if (c == '"' || c == '\'') {
            char delimitador = c;
            token.lexema[i++] = c;                     // Inicia o lexema com a aspa
            while ((c = fgetc(fonte)) != delimitador && c != EOF) { // Lê até encontrar a aspa de fechamento ou EOF
                token.lexema[i++] = c;                 // Adiciona o caractere ao lexema
            }
            if (c == delimitador) token.lexema[i++] = c; // Consome a aspa de fechamento se for encontrada
            token.lexema[i] = '\0';                    // Finaliza o lexema
            token.tipo = STRING;                       // Classifica como string
            token.linha = linhaAtual;                  // Define a linha do token
            imprime_token(token);
            return token;
        }

        // Operadores (Se não foi nenhum acima, é um operador ou delimitador)
        token.lexema[i++] = c;                         // Inicia o lexema com o operador ou delimitador
        token.linha = linhaAtual;                      // Define a linha do token

        if (c == '=') {
            if (peek() == '=') {
                token.lexema[i++] = fgetc(fonte);
                token.tipo = IGUAL;
            } 
            else {
                token.tipo = ATRIBUICAO;
            }
        } else if (c == '!') {
            if (peek() == '=') {
                token.lexema[i++] = fgetc(fonte);
                token.tipo = DIFERENTE;
            }
            else {
                token.tipo = ERRO;     // Não existe '!' sozinho, então é erro
            }
        } else if (c == '<') {
            if (peek() == '=') {
                token.lexema[i++] = fgetc(fonte);
                token.tipo = MENOR_IGUAL;
            }
            else if (peek() == '>') {  // Caso seja <>
                token.lexema[i++] = fgetc(fonte);
                token.tipo = DIFERENTE;
            }
            else {
                token.tipo = MENOR;
            }
        } else if (c == '>') {
            if (peek() == '=') {
                token.lexema[i++] = fgetc(fonte);
                token.tipo = MAIOR_IGUAL;
            }
            else {
                token.tipo = MAIOR;
            }
        } else if (c == '*') {
            if (peek() == '*') {
                token.lexema[i++] = fgetc(fonte);
                token.tipo = EXP; }
            else {
                token.tipo = MULT;
            }
        } else if (c == '+') { token.tipo = SOMA;
        } else if (c == '-') { token.tipo = SUB;
        } else if (c == '/') { token.tipo = DIV;
        } else if (c == '%') { token.tipo = MOD;
        } else if (c == '~') { token.tipo = BIT_NOT;

        // Delimitadores
        } else if (c == ':') { token.tipo = DOIS_PONTOS;
        } else if (c == '.') { token.tipo = PONTO;
        } else if (c == ',') { token.tipo = VIRGULA;
        } else if (c == ';') { token.tipo = PONTO_VIRGULA;
        } else if (c == '(') { token.tipo = ABRE_PAR;
        } else if (c == ')') { token.tipo = FECHA_PAR;
        } else if (c == '[') { token.tipo = ABRE_COL;
        } else if (c == ']') { token.tipo = FECHA_COL;
        } else if (c == '{') { token.tipo = ABRE_CHAVE;
        } else if (c == '}') { token.tipo = FECHA_CHAVE;
        } else {
            token.tipo = ERRO;         // Se não for nenhum dos casos acima, é um token inválido
        }

        token.lexema[i] = '\0';        // Finaliza o lexema
        imprime_token(token);
        return token;
    }

    token.tipo = EOS;                  // Se chegou aqui, é porque encontrou EOF
    strcpy(token.lexema, "EOF");
    token.linha = linhaAtual;
    imprime_token(token);
    return token;
}




// ===============================================================================
//                             ANALISADOR SINTÁTICO
// ===============================================================================

// Variável global lookahed do Sintático
TInfoAtomo lookahead;

// Protótipos das funções (necessário porque elas chamam umas às outras)
void codigo();
void lista_instrucoes();
void instrucao();
void expressao();
void exp_or();
void exp_and();
void exp_relacional();
void exp_aritmetica();
void termo();
void fator();
void primario();
void lista_argumentos();
void lista_imprimivel();
void lista_declaracao();

// Função de erro sintático
void erro_sintatico(const char *msg) {
    printf("ERRO SINTATICO: linha %d - %s (token atual: %s)\n", lookahead.linha, msg, lookahead.lexema);
    exit(1); // Para no primeiro erro
}

// Função consome: verifica se o token atual é o esperado e avança para o próximo
void consome(TAtomo esperado) {
    if (lookahead.tipo == esperado) {
        lookahead = obter_atomo();
    } else {
        erro_sintatico("Token inesperado");
    }
}

// (INÍCIO) -> CODIGO
void codigo() {
    lista_instrucoes();
    if (lookahead.tipo != EOS) {
        erro_sintatico("Código extra no final do arquivo");
    }
}

// LISTA_INSTRUCOES -> INSTRUCAO | INSTRUCAO LISTA_INSTRUCOES
void lista_instrucoes() {
    // Fica em loop enquanto o token atual for o início de uma instrução válida
    while (lookahead.tipo == IDENTIFICADOR || lookahead.tipo == IF || lookahead.tipo == WHILE || lookahead.tipo == FOR || lookahead.tipo == PRINT || lookahead.tipo == LEN) {
        instrucao();
    }
}

// INSTRUCAO -> ATRIBUICAO | IF | WHILE | FOR | PRINT | CHAMADA_FUNCAO
void instrucao() {
    if (lookahead.tipo == IF) { // IF -> if EXPRESSAO ':' INSTRUCAO
        consome(IF);
        expressao();
        consome(DOIS_PONTOS);
        instrucao();
        if (lookahead.tipo == ELSE) { // IF -> if EXPRESSAO ':' INSTRUCAO ELSE ':' INSTRUCAO
            consome(ELSE);
            consome(DOIS_PONTOS);
            instrucao();
        }
    } else if (lookahead.tipo == WHILE) { // WHILE -> while EXPRESSAO ':' INSTRUCAO
        consome(WHILE);
        expressao();
        consome(DOIS_PONTOS);
        instrucao();
    } else if (lookahead.tipo == FOR) { // FOR -> for IDENTIFICADOR in range '(' EXPRESSAO ')' ':' INSTRUCAO
        consome(FOR);
        consome(IDENTIFICADOR);
        consome(IN);
        consome(RANGE);
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
        consome(DOIS_PONTOS);
        instrucao();
    } else if (lookahead.tipo == PRINT) { // PRINT
        consome(PRINT);
        consome(ABRE_PAR);
        if (lookahead.tipo != FECHA_PAR) { // PRINT -> print '(' LISTA_IMPRIMIVEL ')'
            lista_imprimivel();
        }
        consome(FECHA_PAR);             // PRINT -> print '(' ')'
    } else if (lookahead.tipo == LEN) { // CHAMADA_FUNCAO: len '(' IDENTIFICADOR ')'
        consome(LEN);
        consome(ABRE_PAR);
        consome(IDENTIFICADOR);
        consome(FECHA_PAR);
    } else if (lookahead.tipo == IDENTIFICADOR) { // CHAMADA_FUNCAO ou ATRIBUICAO ou VARIAVEL
        consome(IDENTIFICADOR);
        
        if (lookahead.tipo == ATRIBUICAO) { // ATRIBUICAO
            consome(ATRIBUICAO);
            if (lookahead.tipo == INPUT) {
                consome(INPUT);
                consome(ABRE_PAR);
                if (lookahead.tipo == STRING) consome(STRING); // ATRIBUICAO -> VARIAVEL '=' input '(' STRING ')'
                consome(FECHA_PAR);    // ATRIBUICAO -> VARIAVEL '=' input '(' ')'
            } else {
                expressao();
            }
        } else if (lookahead.tipo == ABRE_COL) { // VARIAVEL -> IDENTIFICADOR '[' EXPRESSAO ']'
            consome(ABRE_COL);
            expressao();
            consome(FECHA_COL);
            consome(IGUAL);
            expressao();
        } else if (lookahead.tipo == ABRE_PAR) { // CHAMADA_FUNCAO
            consome(ABRE_PAR);
            if (lookahead.tipo != FECHA_PAR) {  // CHAMADA_FUNCAO -> IDENTIFICADOR '(' LISTA_ARGUMENTOS ')'
                lista_argumentos();
            }
            consome(FECHA_PAR);                 // CHAMADA_FUNCAO -> IDENTIFICADOR '(' ')'
        } else {
            erro_sintatico("Instrução inválida após identificador");
        }
    } else {
        erro_sintatico("Instrução não reconhecida");
    }
}

// LISTA_IMPRIMIVEL -> EXPRESSAO | EXPRESSAO ',' LISTA_IMPRIMIVEL
void lista_imprimivel() {
    expressao();                           // LISTA_IMPRIMIVEL -> EXPRESSAO
    while (lookahead.tipo == VIRGULA) {    // LISTA_IMPRIMIVEL -> EXPRESSAO ',' LISTA_IMPRIMIVEL
        consome(VIRGULA);
        expressao();
    }
}

// LISTA_ARGUMENTOS -> EXPRESSAO | EXPRESSAO ',' LISTA_ARGUMENTOS
void lista_argumentos() {
    expressao();                          // LISTA_ARGUMENTOS -> EXPRESSAO
    while (lookahead.tipo == VIRGULA) {   // LISTA_ARGUMENTOS -> EXPRESSAO ',' LISTA_ARGUMENTOS
        consome(VIRGULA);
        expressao();
    }
}

// EXPRESSAO -> EXP_OR
void expressao() {
    exp_or();
}

// EXP_OR -> EXP_AND | EXP_OR or EXP_AND
void exp_or() {
    exp_and();                      // EXP_OR -> EXP_AND
    while (lookahead.tipo == OR) {  // EXP_OR -> EXP_OR or EXP_AND
        consome(OR);
        exp_and();
    }
}

// EXP_AND -> EXP_RELACIONAL | EXP_AND and EXP_RELACIONAL
void exp_and() {
    exp_relacional();               // EXP_AND -> EXP_RELACIONAL
    while (lookahead.tipo == AND) { // EXP_AND -> EXP_AND and EXP_RELACIONAL
        consome(AND);
        exp_relacional();
    }
}

// EXP_RELACIONAL -> EXP_ARITMETICA | EXP_RELACIONAL OP_RELACIONAL EXP_ARITMETICA
void exp_relacional() {
    exp_aritmetica();     // EXP_RELACIONAL -> EXP_ARITMETICA
    // EXP_RELACIONAL -> EXP_RELACIONAL OP_RELACIONAL EXP_ARITMETICA
    while (lookahead.tipo == IGUAL || lookahead.tipo == DIFERENTE || lookahead.tipo == MENOR || lookahead.tipo == MAIOR || lookahead.tipo == MENOR_IGUAL || lookahead.tipo == MAIOR_IGUAL || lookahead.tipo == IS || lookahead.tipo == IN) {
        consome(lookahead.tipo);
        exp_aritmetica();
    }
}

// EXP_ARITMETICA -> TERMO | EXP_ARITMETICA OP_SOMA TERMO
void exp_aritmetica() {
    termo();             // EXP_ARITMETICA -> TERMO
    while (lookahead.tipo == SOMA || lookahead.tipo == SUB) { // EXP_ARITMETICA -> EXP_ARITMETICA OP_SOMA TERMO
        consome(lookahead.tipo);
        termo();
    }
}

// TERMO -> FATOR | TERMO OP_MULT FATOR
void termo() {
    fator();            // TERMO -> FATOR
    // TERMO -> TERMO OP_MULT FATOR
    while (lookahead.tipo == MULT || lookahead.tipo == DIV || lookahead.tipo == MOD || lookahead.tipo == EXP) {
        consome(lookahead.tipo);
        fator();
    }
}

// FATOR -> OP_UNARIO FATOR | PRIMARIO
void fator() {
    // FATOR -> OP_UNARIO FATOR
    if (lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        consome(lookahead.tipo);
        fator();
    } else { // FATOR -> PRIMARIO
        primario();
    }
}

// LISTA_DECLARACAO -> '[' LISTA_ARGUMENTOS ']' | '[' ']'
void lista_declaracao() {
    consome(ABRE_COL);
    if (lookahead.tipo != FECHA_COL) { // LISTA_DECLARACAO -> '[' LISTA_ARGUMENTOS ']'
        lista_argumentos();
    }
    consome(FECHA_COL);               // LISTA_DECLARACAO -> '[' ']'
}

// PRIMARIO -> NUMERO | BOOLEANO | STRING | VARIAVEL | CHAMADA_FUNCAO | LISTA_DECLARACAO | '(' EXPRESSAO ')' | TUPLA
void primario() {
    if (lookahead.tipo == NUMERO) { // PRIMARIO -> NUMERO
        consome(NUMERO);
    } else if (lookahead.tipo == BOOLEANO) { // PRIMARIO -> BOOLEANO
        consome(BOOLEANO);
    } else if (lookahead.tipo == STRING) { // PRIMARIO -> STRING
        consome(STRING);
    } else if (lookahead.tipo == ABRE_PAR) {
        // ATUALIZADO: Fatoração para resolver o conflito entre '(' EXPRESSAO ')' e TUPLA
        consome(ABRE_PAR);
        
        if (lookahead.tipo == FECHA_PAR) { // PRIMARIO -> TUPLA -> '(' ')'
            consome(FECHA_PAR);            // TUPLA -> '(' ')'
        } else {
            expressao();                   // PRIMARIO -> '(' EXPRESSAO ')' ou TUPLA -> '(' EXPRESSAO ',' ... ')'
            
            if (lookahead.tipo == FECHA_PAR) { // PRIMARIO -> '(' EXPRESSAO ')'
                consome(FECHA_PAR);
            } else if (lookahead.tipo == VIRGULA) { // PRIMARIO -> TUPLA -> '(' EXPRESSAO ',' ... ')'
                consome(VIRGULA);
                if (lookahead.tipo == FECHA_PAR) { // TUPLA -> '(' EXPRESSAO ',' ')'
                    consome(FECHA_PAR);
                } else {                          // TUPLA -> '(' EXPRESSAO ',' LISTA_ARGUMENTOS ')'
                    lista_argumentos();
                    consome(FECHA_PAR);
                }
            } else {
                erro_sintatico("Esperado ')' ou ',' apos expressao");
            }
        }
    } else if (lookahead.tipo == ABRE_COL) { // PRIMARIO -> LISTA_DECLARACAO
        lista_declaracao();
    } else if (lookahead.tipo == LEN) {     // CHAMADA_FUNCAO -> len '(' IDENTIFICADOR ')'
        consome(LEN);
        consome(ABRE_PAR);
        consome(IDENTIFICADOR);
        consome(FECHA_PAR);
    } else if (lookahead.tipo == IDENTIFICADOR) { // VARIAVEL ou CHAMADA_FUNCAO
        consome(IDENTIFICADOR);
        if (lookahead.tipo == ABRE_COL) { // VARIAVEL -> IDENTIFICADOR '[' EXPRESSAO ']'
            consome(ABRE_COL);
            expressao();
            consome(FECHA_COL);
        } else if (lookahead.tipo == ABRE_PAR) { // CHAMADA_FUNCAO -> IDENTIFICADOR '(' LISTA_ARGUMENTOS ')' ou IDENTIFICADOR '(' ')'
            consome(ABRE_PAR);
            if (lookahead.tipo != FECHA_PAR) {
                lista_argumentos();
            }
            consome(FECHA_PAR);
        }
    } else {
        erro_sintatico("Expressão ou fator inválido");
    }
}




// ===============================================================================
//                             MAIN PARA IMPRIMIR TESTES
// ===============================================================================

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    fonte = fopen(argv[1], "r");
    if (fonte == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Cria o arquivo de saída
    saida = fopen("tokens.txt", "w");
    if (saida == NULL) {
        printf("Erro ao criar arquivo de tokens do analisador lexico.\n");
        return 1;
    }
    
    // Inicia o sintático
    lookahead = obter_atomo();
    codigo();
    
    printf("Compilacao terminada com sucesso!\n");

    fclose(fonte);
    return 0;
}