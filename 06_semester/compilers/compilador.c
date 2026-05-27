/*

---------- GRUPO ----------
Bruna Gonçalves Corte David - RA: 10425696
Júlia Andrade - RA: 10428513


                        VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV

        vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
>>>>>> Digite "APRESENTACAO" no Ctrl + F, ao MESMO TEMPO que lê o Readme.txt, para ter a explicação essencial e rápida do projeto <<<<<
        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

                        /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
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
    INPUT, LEN, AND, OR, NOT, IS, DEF, RETURN, EXEC, FROM,
    
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

// -------- ARQUIVOS GLOBAIS --------
FILE *fonte;
FILE *saida;
FILE *instrucoes;
int linhaAtual = 1;

// --------- USO DO LÉXICO ----------
TInfoAtomo lista_identificadores[100];
int total_identificadores = 0;

// ------- USO DO SINTÁTICO ---------
TInfoAtomo lookahead;
int linha_com_possib_de_erro = 1;

// ------- USO DO SEMÂNTICO ---------
// Minitabela de símbolos
typedef struct _TNo {
    char ID [16]; // lexema
    int endereco;
    char tipo [7];                     // O tipo pode ser um dos 6 a seguir: INT, BOOL, INTLIS e BOLLIS (Explicação detalhada no nosso README)
    
    int flag_existe;                   // Flag que decidimos adicionar à estrutura de minitabela para poder "deletar" variáveis declaradas em um escopo quando se sai desse escopo sem precisar atualizar minitabela inteira
    int flag_usado;                    // Flag que decidimos adicionar à estrutura de minitabela para poder acompanhar quais variáveis foram ou não usadas (chamadas/referenciadas) no corpo do código após declaração

    struct _TNo *prox;
} TNo;
TNo *minitabela_de_simbolos = NULL;
int contador_enderecos = 0;
                                       
int flag_pode_string = 0;              // [APRESENTACAO] O tipo STRING existe SOMENTE em um print, ele é desconhecido em qualquer outra parte do código e é proibido (Explicação detalhada no nosso README)
int flag_dentro_de_escopo = 0;         // [APRESENTACAO] Flag global para saber quando o código atual está dentro de um escopo local (if,elif,else/while) para lidar de forma diferente com variáveis declaradas nele (Explicação detalhada no nosso README)

// --------- USO DO G.C.I. ----------
typedef struct End {                   // Usado para transportar dados de "End"s de todas as instruções no formato "end op end" (x = y op z | end op end op end)
    char lexema[100];
    char tipo[7];                      // O tipo pode ser um dos 6 a seguir: INT, BOOL, INTLIS e BOLLIS (Explicação detalhada no nosso README)
} End;
typedef struct Op {                    // Usado para transportar dados de "Op"s de todas as instruções no formato "end op end" (x = y op z | end op end op end)
    char lexema[100];
} Op;

int contador_proximo_rotulo_t = 1;
int contador_proximo_rotulo_l = 0;

End conjunto_sendo_declarado;          // [ APRESENTACAO ] Variável global para facilitar a geração de código intermediário de listas/vetores sendo declaradas (Explicação detalhada no nosso README)


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
    else if (tipo == EXEC) return "EXEC";
    else if (tipo == FROM) return "FROM";
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
    } else if (token.tipo != EOS) {
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

    // ----------------------------- MONTA LEXEMA ATÉ ENCONTRAR ESPAÇO ------------------------------
    token.lexema[i++] = c;                                  // Inicia lexema com caractere atual (já se sabe que não é um espaço)
    while ((c = fgetc(fonte)) != EOF && !isspace(c)) {      // Monta lexema até encontrar o final do arquivo, um espaço ou um quebra linha
        token.lexema[i++] = c;
    }
    token.lexema[i] = '\0';                                 // Finaliza o lexema
    token.linha = linhaAtual;
    if (c == '\n') {                                        // Se o lexema tinha finalizado em um quebra linha, conta +1 linha
        linhaAtual++;
    }

    // -------------------------------------- CLASSIFICA TOKEN --------------------------------------

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
    else if (strcmp(token.lexema, "exec") == 0) token.tipo = EXEC;
    else if (strcmp(token.lexema, "from") == 0) token.tipo = FROM;
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

    // -------------------------- CASO ESPECIAL: TOKEN É UM IDENTIFICADOR ---------------------------

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
//                                      ANALISADOR SEMÂNTICO
// ==============================================================================================

// -------------------- FUNÇÃO AUXILIAR PARA IMPRESSÃO DOS ERROS SEMÂNTICOS  --------------------
void erro_semantico(char *msg) {
    printf("ERRO SEMANTICO: linha %d - %s\n", linha_com_possib_de_erro, msg);
    exit(1);
}
void erro_semantico_sem_linha(char *msg) {
    printf("ERRO SEMANTICO: %s\n", msg);
    exit(1);
}


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= MÉTODOS DE MANIPULAR MINITABELA =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= // APRESENTACAO

TNo* busca_tabela_simbolos(char *lexema) {
    TNo *no = minitabela_de_simbolos;      // Aponta para o nó mais à esquerda na lista encadeada atual
    while (no != NULL) {                   // Percorre a lista encadeada até no máximo chegar no final dela (NULL)
        if ( (strcmp((*no).ID, lexema) == 0) && ((*no).flag_existe == 1) ) { // Verifica se o nó atual representa o identificador/variável com o lexema sendo buscado E se ele já não foi "apagado" (flag_existe = 0)
            return no;                     // Se sim, retorna ele
        }
        no = (*no).prox;                   // Se não, continua para o próximo nó da lista encadeada
    }
    return NULL;                           // Se não encontrou, retorna que não existe
}

void insere_minitabela_de_simbolos(char *lexema, char *tipo) {
    TNo *no = (TNo*) malloc(sizeof(TNo));  // Aloca espaço para novo nó na tabela
    strcpy((*no).ID, lexema);              // Insere o atributo ID/lexema nele
    strcpy((*no).tipo, tipo);              // Insere o atributo tipo nele
    (*no).endereco = contador_enderecos++; // Insere o atributo endereço nele
    (*no).flag_existe = 1;                 // Insere o atributo flag_existe nele
    (*no).flag_usado = 0;                  // Insere o atributo flag_usado nele

    (*no).prox = minitabela_de_simbolos;   // Aponta para o nó mais à esquerda na lista encadeada atual
    minitabela_de_simbolos = no;           // Se torna o nó mais à esquerda na lista encadeada atual
}


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= MÉTODOS SEMÂNTICOS PRINCIPAIS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void semantico_validar_atribuicao(End end_esquerda, End end_direita) {

    // ----------- VERIFICA SE JÁ EXISTE ------------
    TNo *no = busca_tabela_simbolos(end_esquerda.lexema); // Procura esse identificador/variável na minitabela

    if (no == NULL) {                      // Identificador/variável não existe na minitabela, ou seja, é a primeira declaração dele
        insere_minitabela_de_simbolos(end_esquerda.lexema, end_direita.tipo); // Adiciona ele na minitabela com o tipo do dado sendo atribuído (essa atribuição é válida)
        
        // APRESENTACAO
        if (flag_dentro_de_escopo) {       // EXCEÇÃO: Se o identificador/variável está sendo declarado pela primeira vez em um escopo (if,elif,else/while), ...
            (*minitabela_de_simbolos).flag_existe = 0; // .... Já marca adiantadamente como inexistente/deletada, pois na próxima linha já não existirá mais (escopos só possuem 1 linha de tamanho)
        }                                  // PS: Isso também significa que esse identificador/variável nunca será usado e haverá erro semântico. Isso acontecerá naturalmente com a flag_usado = 0
    } else {

        // ----------- VERIFICA SE TIPO DE DADO MUDOU ------------
        if (strcmp((*no).tipo, end_direita.tipo) != 0) { // Se um tipo de dado DIFERENTE do atual guardado no identificador/variavel estiver sendo atribuído à ele, então é erro
            char msg[100];                                   
            sprintf(msg, "Tentativa de atribuir dado de tipo %s em variável de tipo %s\n", end_direita.tipo, (*no).tipo);
            erro_semantico(msg);
            exit(1);
        }
        //                                 // (essa atribuição é válida)
    }
}

void semantico_validar_operacao(End end_esquerda, End end_direita) {

    // ----------- VERIFICA SE OS DOIS END NÃO SÃO DO MESMO TIPO ------------
    if (strcmp(end_esquerda.tipo, end_direita.tipo) != 0) { // Se houver tentativa de realizar uma operação entre dois tipos DIFERENTES, então é erro
        char msg[100];                                   
        sprintf(msg, "Tentativa de realizar uma operação entre dois tipos de dados diferentes (%s e %s)\n", end_esquerda.tipo, end_direita.tipo);
        erro_semantico(msg);
        exit(1);         
    }
    //                                     // (essa operação é válida)
}

End semantico_validar_chamada(End end) {

    // ----------- VERIFICA SE JÁ EXISTE ------------
    TNo *no = busca_tabela_simbolos(end.lexema); // Procura esse identificador/variável na minitabela

    if (no == NULL) {                      // Identificador/variável não existe na minitabela, ou seja, ele não foi declarado antes de ser chamado, então é erro
        char msg[100];                                   
        sprintf(msg, "Tentativa de referenciar a variável %s antes dela ser declarada\n", end.lexema);
        erro_semantico(msg);
        exit(1);          
    }
    //                                     // (essa chamada/referência é válida)
    (*no).flag_usado = 1;                  // Marca identificador/variável como usado
    strcpy(end.tipo, (*no).tipo);          // Atualiza o tipo desse end com o tipo armazenado
    return end;                            // Retorna end atualizado
}

void semantico_validar_usados() {

    TNo *no = minitabela_de_simbolos;      // Aponta para o nó mais à esquerda na lista encadeada atual
    while (no != NULL) {                   // Percorre a lista encadeada até chegar no final dela (NULL)
        // ----------- VERIFICA SE FOI USADA ------------
        if (!(*no).flag_usado) {           // Se um identificador/variável foi declarado mas nunca usado, então é erro
            char msg[100];                                   
            sprintf(msg, "Variável %s foi declarada porém nunca usada no corpo do programa\n", (*no).ID);
            erro_semantico_sem_linha(msg);
            exit(1); 
        }
        no = (*no).prox;                   // Continua para o próximo nó da lista encadeada
    }

}


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= MÉTODOS SEMÂNTICOS EXTRAS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

End semantico_validar_indexacao(End end_esquerda, End end_t) {

    // ----------- VERIFICA SE O TIPO DO END_ESQUERDA É INTLIS ------------
    if (strcmp(end_esquerda.tipo, "INTLIS") == 0) { // Se é uma LISTA DE INTEIROS (ou seja, tipo INTLIS) sendo indexada, ...
        strcpy(end_t.tipo, "INT");         // ...então a indexação é válida E o elemento sendo chamado/referenciado é tipo INT.
        return end_t;                      // Retorna end_t (elemento sendo chamado/referenciado) com o seu tipo atualizado
    }
    // ----------- VERIFICA SE O TIPO DO END_ESQUERDA É BOLLIS ------------
    else if (strcmp(end_esquerda.tipo, "BOLLIS") == 0) { // Se é uma LISTA DE BOOLEANOS (ou seja, tipo BOLLIS) sendo indexada, ...
        strcpy(end_t.tipo, "BOOL");       // ...então a indexação é válida E o elemento sendo chamado/referenciado é tipo BOOL.
        return end_t;                     // Retorna end_t (elemento sendo chamado/referenciado) com o seu tipo atualizado
    }
    //                                     // (se a variável sendo indexada não for nenhum dos tipos acima, então é erro)
    char msg[100];                                   
    sprintf(msg, "Tentativa de indexar a variável '%s' tipo %s. É obrigatório que seja tipo INTLIS ou BOLLIS\n", end_esquerda.lexema, end_esquerda.tipo);
    erro_semantico(msg);
    exit(1);

}

void semantico_validar_atribuicao_indexada(End end_vetor, End end_valor) {
    
    End end_t;                             // End auxiliar e temporário
    end_t.lexema[0] = '\0';                // "Flag" para lexema "nulo"
    end_t = semantico_validar_indexacao(end_vetor, end_t); // Valida indexação do end_vetor E solicita o tipo de dado que ele aceita armazenar

    // ----------- VERIFICA SE OS DOIS END NÃO SÃO DO MESMO TIPO ------------
    if (strcmp(end_t.tipo, end_valor.tipo) != 0) { // Se o tipo de dado que o vetor aceita armazenar for diferente do tipo de dado tentando ser armazenado, então é erro
        char msg[100];                                   
        sprintf(msg, "Tentativa de atribuir o valor '%s' tipo %s a um vetor que apenas aceita elementos tipo %s\n", end_valor.lexema, end_valor.tipo, end_t.tipo);
        erro_semantico(msg);
        exit(1);
    }
    //                                     // (essa atribuição é válida)

}

void semantico_validar_declaracao_conjunto(End end_valor) {

    // ----------- VERIFICA SE JÁ EXISTE ------------
    TNo *no = busca_tabela_simbolos(conjunto_sendo_declarado.lexema); // Procura esse vetor na minitabela

    if (no == NULL) {                      // Vetor não existe na minitabela, ou seja, é a primeira declaração dele
        
        if (strcmp(end_valor.tipo, "INT") == 0) {
            strcpy(conjunto_sendo_declarado.tipo, "INTLIS");
        }
        if (strcmp(end_valor.tipo, "BOOL") == 0) {
            strcpy(conjunto_sendo_declarado.tipo, "BOLLIS");
        }
        
        insere_minitabela_de_simbolos(conjunto_sendo_declarado.lexema, conjunto_sendo_declarado.tipo); // Adiciona ele na minitabela com o tipo do dado sendo atribuído (essa atribuição é válida)
    
    } else {                               // Vetor já possui um tipo de dado definido na minitabela que não pode mais mudar

        // ----------- VERIFICA SE TIPO DE DADO MUDOU ------------
        if ( (strcmp((*no).tipo, "INTLIS") == 0) && (strcmp(end_valor.tipo, "INT") == 0) ) {         // INTLIS deve ter INT
        } else if ( (strcmp((*no).tipo, "BOLLIS") == 0) && (strcmp(end_valor.tipo, "BOOL") == 0) ) { // BOLLIS deve ter BOOL
        } else {
            char msg[100];                                   
            sprintf(msg, "Tentativa de atribuir dado de tipo %s em vetor de tipo %s\n", end_valor.tipo, (*no).tipo);
            erro_semantico(msg);
            exit(1);    
        }
        //                                 // (essa atribuição é válida)
    }
}

void semantico_proibir_strings(){          // (Explicação detalhada no nosso README) APRESENTACAO
    if (!flag_pode_string) {               // Se a flag_pode_string for 0, significa que uma STRING foi encontrada fora de um print, então é erro
        char msg[100];                                   
        sprintf(msg, "Tipo de dado STRING é desconhecido no código, exceto dentro de um print (Explicação detalhada no nosso README)\n");
        erro_semantico(msg);
        exit(1);
    }
    //                                     // (EXCEÇÃO: Se um print estiver sendo construído, ele reconhece o tipo de dado STRING)
}

// ==============================================================================================
//               (FUNÇÕES AUXILIARES DO) GERADOR DE CÓDIGO INTERMEDIÁRIO (G.C.I.)
// ==============================================================================================
int proximo_rotulo_t() {
    return contador_proximo_rotulo_t++;
}
int proximo_rotulo_l() {
    return contador_proximo_rotulo_l++;
}

// ==============================================================================================
//                                      ANALISADOR SINTÁTICO
// ==============================================================================================

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
void identificadores();
int lista_imprimivel();
End chama_id();
int lista_argumentos(int);
End expressao();
End exp_or();
End exp_and();
End exp_comp();
End exp_aritmetica();
End termo();
End fator();
End primario();
End primario_aux();
void lista_declaracao();
Op op_comp();
Op op_soma();
Op op_mult();
Op op_unico();
void tupla();
void tupla_aux();
End exp_aritmetica_aux(End);
void if_aux(int);
void lista_instrucoes_aux();
void def_aux();
void lista_declaracao_aux();
void print_aux();
void lista_ids();
void lista_ids_aux();
int lista_imprimivel_aux();
End chama_id_aux(End);
void chamada_funcao_aux();
void identificadores_aux(End);
int lista_argumentos_aux(int);
End exp_or_aux(End);
End exp_and_aux(End);
End exp_comp_aux(End);
End termo_aux(End);
void elif(int);


// -------------------- FUNÇÃO AUXILIAR PARA IMPRESSÃO DOS ERROS SINTÁTICOS  --------------------

void erro_sintatico(const char *msg) {
    printf("ERRO SINTATICO: linha %d - %s (token atual: %s)\n", linha_com_possib_de_erro, msg, lookahead.lexema);
    exit(1);
}


// -------------------------------------- FUNÇÃO CONSOME  ---------------------------------------

void consome(TAtomo esperado) { // Verifica se o token atual é o esperado e avança para o próximo
    if (lookahead.tipo == esperado) {
        linha_com_possib_de_erro = lookahead.linha;  // Atualiza o número da linha com a possibilidade de gerar erro atual         
        lookahead = obter_atomo(); // APRESENTACAO
    } else {
        erro_sintatico("Token inesperado");
    }
}

// ------------------------------ INÍCIO DO ANALISADOR SINTÁTICO  -------------------------------

void lista_instrucoes() {                            // LISTA_INSTRUCOES -> INSTRUCAO LISTA_INSTRUCOES_AUX

    if (lookahead.tipo == EOS) {                     // Retorna se chegou no final do arquivo
        return;
    }

    instrucao();

    fprintf(instrucoes, "\n");                       // G.C.I.: Separa instruções por uma linha para facilitar leitura no instrucoes.txt

    lista_instrucoes_aux();
}

void lista_instrucoes_aux() {                        // LISTA_INSTRUCOES_AUX -> LISTA_INSTRUCOES | ε
    if (lookahead.tipo == IDENTIFICADOR || lookahead.tipo == IF || lookahead.tipo == WHILE || lookahead.tipo == FOR || lookahead.tipo == PRINT || lookahead.tipo == LEN || lookahead.tipo == DEF || lookahead.tipo == EXEC || lookahead.tipo == INPUT) {
        lista_instrucoes();                          // LISTA_INSTRUCOES_AUX -> LISTA_INSTRUCOES
    }
    //                                               // LISTA_INSTRUCOES_AUX -> ε
}

void instrucao() {                                   // INSTRUCAO -> IDENTIFICADORES | PRINT | EXEC | DEF | IF | WHILE | FOR | INPUT
    if (lookahead.tipo == IDENTIFICADOR) {
        identificadores();
    } else if (lookahead.tipo == PRINT) {
        print();
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
    } else if (lookahead.tipo == INPUT) {
        input();
    } else {
        erro_sintatico("Instrução não reconhecida"); // ERRO SINTÁTICO
    }
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

        fprintf(instrucoes, "call print, 0\n");      // G.C.I.: Escreve no arquivo a instrução "call p, n" (apesar de que, nesse caso, já sabemos que é "print, 0")

    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        // APRESENTACAO
        
        // ---------------- ANÁLISE SEMÂNTICA ----------------
        flag_pode_string = 1; // EXCEÇÃO DO PRINT: Strings não existem no resto do código e são proibidas, APENAS existem em prints
        // ---------------------------------------------------

        int numero_parametros = lista_imprimivel();  // G.C.I.: Solicita número de parâmetros

        // ---------------- ANÁLISE SEMÂNTICA ----------------
        flag_pode_string = 0; // EXCEÇÃO DO PRINT: Strings não existem no resto do código e são proibidas, APENAS existem em prints
        // ---------------------------------------------------

        fprintf(instrucoes, "call print, %d\n", numero_parametros); // G.C.I.: Escreve no arquivo a instrução "call p, n" (apesar de que, nesse caso, já sabemos que é "print")

        consome(FECHA_PAR);
    } else {
        erro_sintatico("Esperado ')' ou expressão"); // ERRO SINTÁTICO
    }
}

void iff(){                                          // IF -> if EXPRESSAO ':' INSTRUCAO IF_AUX
    consome(IF);

    int true_l = proximo_rotulo_l();                 // G.C.I.: Cria o label para o caso do if ser true // APRESENTACAO (um exemplo do G.C.I. gerand código interm. com sintático)
    int false_l = proximo_rotulo_l();                // G.C.I.: Cria o label para o caso do if ser false
    int fim_l = proximo_rotulo_l();                  // G.C.I.: Cria o label final/next (após o if, todos os elif e o else)

    End end = expressao();                           // G.C.I.: Solicita end

    fprintf(instrucoes, "if %s goto L%d\n", end.lexema, true_l); // G.C.I.: Escreve no arquivo a instrução "if end goto Ln" (true)
    fprintf(instrucoes, "goto L%d\n", false_l);      // G.C.I.: Escreve no arquivo a instrução "goto Ln" (false)

    fprintf(instrucoes, "L%d:\n", true_l);           // G.C.I.: Escreve no arquivo a label "Ln:" (true)
    consome(DOIS_PONTOS);
    flag_dentro_de_escopo = 1;                       // Flag global para indicar à ANÁLISE SEMÂNTICA que qualquer variável declarada agora estará num escopo local 
    instrucao();
    flag_dentro_de_escopo = 0;                       // Flag global para indicar à ANÁLISE SEMÂNTICA que o escopo local finalizou
    fprintf(instrucoes, "goto L%d\n", fim_l);        // G.C.I.: Escreve no arquivo a instrução "goto Ln" (fim/next)

    fprintf(instrucoes, "L%d:\n", false_l);          // G.C.I.: Escreve no arquivo a label "Ln:" (false)
    if_aux(fim_l);                                   // G.C.I.: Passa label (fim/next) "para baixo"

    fprintf(instrucoes, "L%d:\n", fim_l);            // G.C.I.: Escreve no arquivo a label "Ln:" (fim/next)
}

void if_aux(int fim_l) {                             // IF_AUX -> ELIF | else ':' INSTRUCAO | ε
    if (lookahead.tipo == ELIF) {
        elif(fim_l);
    }
    if (lookahead.tipo == ELSE) {
        consome(ELSE);
        consome(DOIS_PONTOS);
        flag_dentro_de_escopo = 1;                       // Flag global para indicar à ANÁLISE SEMÂNTICA que qualquer variável declarada agora estará num escopo local 
        instrucao();
        flag_dentro_de_escopo = 0;                       // Flag global para indicar à ANÁLISE SEMÂNTICA que o escopo local finalizou
    }
    //                                               // IF_AUX -> ε
}

void elif(int fim_l) {                               // ELIF -> elif EXPRESSAO ':' INSTRUCAO IF_AUX  
    consome(ELIF);

    int true_l = proximo_rotulo_l();                 // G.C.I.: Cria o label para o caso do if ser true
    int false_l = proximo_rotulo_l();                // G.C.I.: Cria o label para o caso do if ser false

    End end = expressao();                           // G.C.I.: Solicita end

    fprintf(instrucoes, "if %s goto L%d\n", end.lexema, true_l); // G.C.I.: Escreve no arquivo a instrução "if end goto Ln" (true)
    fprintf(instrucoes, "goto L%d\n", false_l);      // G.C.I.: Escreve no arquivo a instrução "goto Ln" (false)

    fprintf(instrucoes, "L%d:\n", true_l);           // G.C.I.: Escreve no arquivo a label "Ln:" (true)
    consome(DOIS_PONTOS);
    flag_dentro_de_escopo = 1;                       // Flag global para indicar à ANÁLISE SEMÂNTICA que qualquer variável declarada agora estará num escopo local 
    instrucao();
    flag_dentro_de_escopo = 0;                       // Flag global para indicar à ANÁLISE SEMÂNTICA que o escopo local finalizou
    fprintf(instrucoes, "goto L%d\n", fim_l);        // G.C.I.: Escreve no arquivo a instrução "goto Ln" (fim/next)

    fprintf(instrucoes, "L%d:\n", false_l);          // G.C.I.: Escreve no arquivo a label "Ln:" (false)
    if_aux(fim_l);                                   // G.C.I.: Passa label (fim/next) "para baixo"
}

void whilee() {                                      // WHILE -> while EXPRESSAO ':' INSTRUCAO
    consome(WHILE);

    int inicio_l = proximo_rotulo_l();               // G.C.I.: Cria o label para voltar para si mesmo
    int true_l = proximo_rotulo_l();                 // G.C.I.: Cria o label para o caso do if ser true
    int false_l = proximo_rotulo_l();                // G.C.I.: Cria o label para o caso do if ser false

    End end = expressao();                           // G.C.I.: Solicita end
    
    fprintf(instrucoes, "L%d:\n", inicio_l);         // G.C.I.: Escreve no arquivo a label "Ln:" (inicio)
    fprintf(instrucoes, "if %s goto L%d\n", end.lexema, true_l); // G.C.I.: Escreve no arquivo a instrução "if end goto Ln" (true)
    fprintf(instrucoes, "goto L%d\n", false_l);      // G.C.I.: Escreve no arquivo a instrução "goto Ln" (false)

    fprintf(instrucoes, "L%d:\n", true_l);           // G.C.I.: Escreve no arquivo a label "Ln:" (true)
    consome(DOIS_PONTOS);
    flag_dentro_de_escopo = 1;                       // Flag global para indicar à ANÁLISE SEMÂNTICA que qualquer variável declarada agora estará num escopo local 
    instrucao();
    flag_dentro_de_escopo = 0;                       // Flag global para indicar à ANÁLISE SEMÂNTICA que o escopo local finalizou
    fprintf(instrucoes, "goto L%d\n", inicio_l);     // G.C.I.: Escreve no arquivo a instrução "goto Ln" (inicio)

    fprintf(instrucoes, "L%d:\n", false_l);          // G.C.I.: Escreve no arquivo a label "Ln:" (false)
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
    End end;                                         // G.C.I.: Salva lexema do end antes do consome()
    strcpy(end.lexema, lookahead.lexema);
    end.tipo[0] = '\0';                              // G.C.I.: "Flag" para tipo "nulo"

    consome(IDENTIFICADOR);

    identificadores_aux(end);                        // G.C.I.: Passa end "para baixo"
}

void identificadores_aux(End end_esquerda) {         // IDENTIFICADORES_AUX -> '=' EXPRESSAO | '[' EXPRESSAO ']' '=' EXPRESSAO | '(' CHAMADA_FUNCAO_AUX
    if (lookahead.tipo == ATRIBUICAO) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é =)
        strcpy(op.lexema, "=");

        consome(ATRIBUICAO);

        // G.C.I.: FACILITAÇÃO PARA O CASO DE DECLARAÇÃO DE LISTAS E TUPLAS --------------
        conjunto_sendo_declarado = end_esquerda; // (Explicação detalhada no nosso README) [ APRESENTACAO ]
        // -------------------------------------------------------------------------------

        End end_direita = expressao();               // G.C.I.: Solicita end_direita

        // ----------------- ANÁLISE SEMÂNTICA -----------------
        semantico_validar_atribuicao(end_esquerda, end_direita);
        // -----------------------------------------------------

        fprintf(instrucoes, "%s %s %s\n", end_esquerda.lexema, op.lexema, end_direita.lexema); // G.C.I.: Escreve no arquivo a instrução "end_esquerda = end_direita"

    } else if (lookahead.tipo == ABRE_COL) {
        consome(ABRE_COL);

        End identificador = end_esquerda;        // G.C.I.: Mudando nomenclatura porque o lado esquerdo da declaração de vetores é muito direto e pouco recursivo

        End indice = expressao();                // G.C.I.: Solicita indice (nomenclatura diferente seguindo explicação acima)
        
        consome(FECHA_COL);
        consome(ATRIBUICAO);

        End valor = expressao();                 // G.C.I.: Solicita valor (nomenclatura diferente seguindo explicação acima)

        // ----------------- ANÁLISE SEMÂNTICA -----------------
        identificador = semantico_validar_chamada(identificador); // G.C.I.: Atualiza o identificador com o seu tipo guardado na minitabela
        semantico_validar_atribuicao_indexada(identificador, valor);
        // -----------------------------------------------------

        fprintf(instrucoes, "%s[%s] = %s\n", identificador.lexema, indice.lexema, valor.lexema); // G.C.I.: Escreve no arquivo a instrução "identificador[indice] = valor"

    } else if (lookahead.tipo == ABRE_PAR) {
        consome(ABRE_PAR);
        chamada_funcao_aux();
    } else {
        erro_sintatico("Esperado '=', '[' ou '('");  // ERRO SINTÁTICO
    }
}

void chamada_funcao_aux() {          // CHAMADA_FUNCAO_AUX -> ')' | LISTA_ARGUMENTOS ')'
    if (lookahead.tipo == FECHA_PAR) {
        consome(FECHA_PAR);
    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        lista_argumentos(0);                         // G.C.I.: Obriga a passar "0" "para baixo" mesmo que não seja usado
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

int lista_imprimivel() {                             // LISTA_IMPRIMIVEL -> EXPRESSAO LISTA_IMPRIMIVEL_AUX
    End end = expressao();                           // G.C.I.: Solicita end

    fprintf(instrucoes, "param %s\n", end.lexema);   // G.C.I.: Escreve no arquivo a instrução "param end"

    return 1 + lista_imprimivel_aux();               // G.C.I.: Conta +1 no número de parâmetros e procura se tem mais, depois passa o resultado "para cima"
}

int lista_imprimivel_aux() {                        // LISTA_IMPRIMIVEL_AUX -> ',' LISTA_IMPRIMIVEL | ε
    if (lookahead.tipo == VIRGULA) {
        consome(VIRGULA);
        return lista_imprimivel();                  // G.C.I.: Passa número de parâmetros contado até agora "para cima"
    }
    //                                               // LISTA_IMPRIMIVEL_AUX -> ε
    return 0;                                        // G.C.I.: Faz nada e não soma mais no número de parâmetros, encerrando a contagem
}

int lista_argumentos(int indice_atual) { // LISTA_ARGUMENTOS -> EXPRESSAO LISTA_ARGUMENTOS_AUX
    End end_valor = expressao();                    // G.C.I.: Solicita end_valor

    // ----------------- ANÁLISE SEMÂNTICA -----------------
    semantico_validar_declaracao_conjunto(end_valor);
    // -----------------------------------------------------
    fprintf(instrucoes, "%s[%d] = %s\n", conjunto_sendo_declarado.lexema, indice_atual, end_valor.lexema); // G.C.I.: Escreve no arquivo a instrução "conjunto_sendo_declarado[indice_atual] = end_valor"
    // APRESENTACAO   ------>     ------>       ^^^^^^^^^^^^

    int numero_parametros = lista_argumentos_aux(indice_atual + 1);

    return numero_parametros;
}

int lista_argumentos_aux(int indice_atual) { // LISTA_ARGUMENTOS_AUX -> ',' LISTA_ARGUMENTOS | ε
    if (lookahead.tipo == VIRGULA) {
        consome(VIRGULA);
        return lista_argumentos(indice_atual);
    }
    //                                               // LISTA_ARGUMENTOS_AUX -> ε
    return indice_atual;                             // G.C.I.: Faz nada e não soma mais no número de parâmetros, encerrando a contagem
}

End expressao() {                                   // EXPRESSAO -> EXP_OR
    return exp_or();                                // G.C.I.: Passa end "para cima"
}

End exp_or() {                                      // EXP_OR -> EXP_AND EXP_OR_AUX
    End end = exp_and();                            // G.C.I.: Solicita end
    return exp_or_aux(end);                         // G.C.I.: Passa end "para baixo" E passa o end resultante "para cima"
}

End exp_or_aux(End end_esquerda) {                  // EXP_OR_AUX -> or EXP_AND EXP_OR_AUX | ε
    if (lookahead.tipo == OR) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é or)
        strcpy(op.lexema, "or");

        consome(OR);

        End end_direita = exp_and();                 // G.C.I.: Solicita end_direita

        // ---------------- ANÁLISE SEMÂNTICA ---------------- // APRESENTACAO (um exemplo do analisador semântico sendo chamado pelo sintático+G.C.I.)
        semantico_validar_operacao(end_esquerda, end_direita);
        // ---------------------------------------------------

        End end_t;                                   // G.C.I.: Cria o nome temporário para escrever a instruçõa
        sprintf(end_t.lexema, "t%d", proximo_rotulo_t()); // G.C.I.: Cria o lexema com proximo_rotulo_t()
        strcpy(end_t.tipo, "BOOL");                  // G.C.I.: O tipo resultante de uma comparação é sempre BOOL

        fprintf(instrucoes, "%s = %s %s %s\n", end_t.lexema, end_esquerda.lexema, op.lexema, end_direita.lexema); // G.C.I.: Escreve no arquivo a instrução "end_t = end_esquerda op end_direita"

        return exp_or_aux(end_t);                    // G.C.I.: Passa end_t "para baixo" E passa o end resultante "para cima"
    }
    //                                               // EXP_OR_AUX -> ε
    return end_esquerda;                             // G.C.I.: Faz nada e retorna end_esquerda como estava antes
}

End exp_and() {                                     // EXP_AND -> EXP_COMP EXP_AND_AUX
    End end = exp_comp();                           // G.C.I.: Solicita end
    return exp_and_aux(end);                        // G.C.I.: Passa end "para baixo" E passa o end resultante "para cima"
}

End exp_and_aux(End end_esquerda) {                  // EXP_AND_AUX -> and EXP_COMP EXP_AND_AUX | ε
    if (lookahead.tipo == AND) {
        
        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é and)
        strcpy(op.lexema, "and");

        consome(AND);

        End end_direita = exp_comp();                // G.C.I.: Solicita end_direita

        // ---------------- ANÁLISE SEMÂNTICA ----------------
        semantico_validar_operacao(end_esquerda, end_direita);
        // ---------------------------------------------------

        End end_t;                                   // G.C.I.: Cria o nome temporário para escrever a instruçõa
        sprintf(end_t.lexema, "t%d", proximo_rotulo_t()); // G.C.I.: Cria o lexema com proximo_rotulo_t()
        strcpy(end_t.tipo, "BOOL");                  // G.C.I.: O tipo resultante de uma comparação é sempre BOOL

        fprintf(instrucoes, "%s = %s %s %s\n", end_t.lexema, end_esquerda.lexema, op.lexema, end_direita.lexema); // G.C.I.: Escreve no arquivo a instrução "end_t = end_esquerda op end_direita"

        return exp_and_aux(end_t);                   // G.C.I.: Passa end_t "para baixo" E passa o end resultante "para cima"
    }
    //                                               // EXP_AND_AUX -> ε
    return end_esquerda;                             // G.C.I.: Faz nada e retorna end_esquerda como estava antes
}

End exp_comp() {                                    // EXP_COMP -> EXP_ARITMETICA EXP_COMP_AUX
    End end = exp_aritmetica();                     // G.C.I.: Solicita end
    return exp_comp_aux(end);                       // G.C.I.: Passa end "para baixo" E passa o end resultante "para cima"
}

End exp_comp_aux(End end_esquerda) {                                // EXP_COMP_AUX -> OP_COMP EXP_ARITMETICA EXP_COMP_AUX | ε
    if (lookahead.tipo == IGUAL || lookahead.tipo == DIFERENTE || lookahead.tipo == MENOR || lookahead.tipo == MAIOR || lookahead.tipo == MENOR_IGUAL || lookahead.tipo == MAIOR_IGUAL || lookahead.tipo == IS || lookahead.tipo == IN) {
        
        Op op = op_comp();                           // G.C.I.: Solicita op

        End end_direita = exp_aritmetica();          // G.C.I.: Solicita end_direita

        // ---------------- ANÁLISE SEMÂNTICA ----------------
        semantico_validar_operacao(end_esquerda, end_direita);
        // ---------------------------------------------------

        End end_t;                                   // G.C.I.: Cria o nome temporário para escrever a instruçõa
        sprintf(end_t.lexema, "t%d", proximo_rotulo_t()); // G.C.I.: Cria o lexema com proximo_rotulo_t()
        strcpy(end_t.tipo, "BOOL");                  // G.C.I.: O tipo resultante de uma comparação é sempre BOOL

        fprintf(instrucoes, "%s = %s %s %s\n", end_t.lexema, end_esquerda.lexema, op.lexema, end_direita.lexema); // G.C.I.: Escreve no arquivo a instrução "end_t = end_esquerda op end_direita"

        return exp_comp_aux(end_t);                  // G.C.I.: Passa end_t "para baixo" E passa o end resultante "para cima"
    }
    //                                               // EXP_COMP_AUX -> ε
    return end_esquerda;                             // G.C.I.: Faz nada e retorna end_esquerda como estava antes
}

End exp_aritmetica() {                               // EXP_ARITMETICA -> TERMO EXP_ARITMETICA_AUX
    End end = termo();                               // G.C.I.: Solicita end
    return exp_aritmetica_aux(end);                  // G.C.I.: Passa end "para baixo" E passa o end resultante "para cima"
}

End exp_aritmetica_aux(End end_esquerda) {           // EXP_ARITMETICA_AUX -> OP_SOMA TERMO EXP_ARITMETICA_AUX | ε
    if (lookahead.tipo == SOMA || lookahead.tipo == SUB) {

        Op op = op_soma();                           // G.C.I.: Solicita op

        End end_direita = termo();                   // G.C.I.: Solicita end_direita

        // ---------------- ANÁLISE SEMÂNTICA ----------------
        semantico_validar_operacao(end_esquerda, end_direita);
        // ---------------------------------------------------

        End end_t;                                   // G.C.I.: Cria o nome temporário para escrever a instruçõa
        sprintf(end_t.lexema, "t%d", proximo_rotulo_t()); // G.C.I.: Cria o lexema com proximo_rotulo_t()
        strcpy(end_t.tipo, end_esquerda.tipo);       // G.C.I.: Tanto faz copiar o tipo do end_esquerda ou do end_direita pq a análise semântica confirmou que os dois são do mesmo tipo

        fprintf(instrucoes, "%s = %s %s %s\n", end_t.lexema, end_esquerda.lexema, op.lexema, end_direita.lexema); // G.C.I.: Escreve no arquivo a instrução "end_t = end_esquerda op end_direita"

        return exp_aritmetica_aux(end_t);            // G.C.I.: Passa end_t "para baixo" E passa o end resultante "para cima"
    }
    //                                               // EXP_ARITMETICA_AUX -> ε
    return end_esquerda;                             // G.C.I.: Faz nada e retorna end_esquerda como estava antes
}

End termo() {                                        // TERMO -> FATOR TERMO_AUX
    End end = fator();                               // G.C.I.: Solicita end
    return termo_aux(end);                           // G.C.I.: Passa end "para baixo" E passa o end resultante "para cima"
}

End termo_aux(End end_esquerda) {                    // TERMO_AUX -> OP_MULT FATOR TERMO_AUX | ε
    if (lookahead.tipo == MULT || lookahead.tipo == DIV || lookahead.tipo == MOD) {

        Op op = op_mult();                            // G.C.I.: Solicita op

        End end_direita = fator();                   // G.C.I.: Solicita end_direita

        // ---------------- ANÁLISE SEMÂNTICA ----------------
        semantico_validar_operacao(end_esquerda, end_direita);
        // ---------------------------------------------------

        End end_t;                                  // G.C.I.: Cria o nome temporário para escrever a instruçõa
        sprintf(end_t.lexema, "t%d", proximo_rotulo_t()); // G.C.I.: Cria o lexema com proximo_rotulo_t()
        strcpy(end_t.tipo, end_esquerda.tipo);       // G.C.I.: Tanto faz copiar o tipo do end_esquerda ou do end_direita pq a análise semântica confirmou que os dois são do mesmo tipo

        fprintf(instrucoes, "%s = %s %s %s\n", end_t.lexema, end_esquerda.lexema, op.lexema, end_direita.lexema); // G.C.I.: Escreve no arquivo a instrução "end_t = end_esquerda op end_direita"
        
        return termo_aux(end_t);                     // G.C.I.: Passa end_t "para baixo" E passa o end resultante "para cima"
    }
    //                                               // TERMO_AUX -> ε
    return end_esquerda;                             // G.C.I.: Faz nada e retorna end_esquerda como estava antes
}

End fator() {                                       // FATOR -> OP_UNICO FATOR | PRIMARIO
    if (lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        
        Op op = op_unico();                         // G.C.I.: Solicita op

        End end_direita = fator();                  // G.C.I.: Solicita end_direita

        End end_t;                                  // G.C.I.: Cria o nome temporário para escrever a instruçõa
        sprintf(end_t.lexema, "t%d", proximo_rotulo_t()); // G.C.I.: Cria o lexema com proximo_rotulo_t()
        strcpy(end_t.tipo, end_direita.tipo);       // G.C.I.: Cria o tipo com o seu tipo

        fprintf(instrucoes, "%s = %s %s\n", end_t.lexema, op.lexema, end_direita.lexema); // G.C.I.: Escreve no arquivo a instrução "end_t = op end_direita"

        return end_t;                               // G.C.I.: Passa end_t "para cima"

    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == INPUT || lookahead.tipo == ABRE_PAR) {
        return primario();                          // G.C.I.: Passa end "para cima"
    } else {
        erro_sintatico("Esperado operador único ou primário"); // ERRO SINTÁTICO
        exit(1);                                    // Redundância
    }
}

End primario() {                                    // PRIMARIO -> NUMERO | BOOLEANO | STRING | CHAMA_ID | len '(' IDENTIFICADOR ')' | INPUT | LISTA_DECLARACAO | '(' PRIMARIO_AUX
    if (lookahead.tipo == NUMERO) {

        End end;                                     // G.C.I.: Salva lexema do end antes do consome()
        strcpy(end.lexema, lookahead.lexema);
        strcpy(end.tipo, "INT");                     // G.C.I.: Tipo INT

        consome(NUMERO);

        return end;                                  // G.C.I.: Passa end "para cima"

    } else if (lookahead.tipo == BOOLEANO) {

        End end;                                     // G.C.I.: Salva lexema do end antes do consome()
        strcpy(end.lexema, lookahead.lexema);
        strcpy(end.tipo, "BOOL");                     // G.C.I.: Tipo BOOL

        consome(BOOLEANO);

        return end;                                  // G.C.I.: Passa end "para cima"

    } else if (lookahead.tipo == STRING) {

        // ---------------- ANÁLISE SEMÂNTICA ----------------
        semantico_proibir_strings();
        // ---------------------------------------------------

        // --------------- ÚNICA EXCEÇÃO: PRINT ---------------
        End end;                                     // G.C.I.: Salva lexema do end antes do consome()
        strcpy(end.lexema, lookahead.lexema);
        strcpy(end.tipo, "STRING");                  // G.C.I.: Tipo STRING (EXCESSÃO)

        consome(STRING);

        return end;                                  // G.C.I.: Passa end "para cima"
        // ---------------------------------------------------

    } else if (lookahead.tipo == IDENTIFICADOR) {
        End end = chama_id();                          // G.C.I.: Solicita end
        return end;                                    // G.C.I.: Passa end "para cima"
    } else if (lookahead.tipo == LEN) {
        consome(LEN);
        consome(ABRE_PAR);

        End end;                                         // G.C.I.: Salva lexema do end antes do consome()
        strcpy(end.lexema, lookahead.lexema);
        end.tipo[0] = '\0';                              // G.C.I.: "Flag" para tipo "nulo"
        
        consome(IDENTIFICADOR);

        // ------- ANÁLISE SEMÂNTICA --------
        end = semantico_validar_chamada(end);            // G.C.I.: Atualiza o end com o seu tipo guardado na minitabela
        // ----------------------------------

        
        consome(FECHA_PAR);
        
        End end_t;                                   // G.C.I.: Cria o nome temporário para escrever a instruçõa
        sprintf(end_t.lexema, "t%d", proximo_rotulo_t()); // G.C.I.: Cria o lexema com proximo_rotulo_t()
        strcpy(end_t.tipo, "INT");                   // G.C.I.: O tipo de um len é sempre INT
        
        fprintf(instrucoes, "%s = len %s\n", end_t.lexema, end.lexema); // G.C.I.: Escreve no arquivo a instrução "end_t = len end"
        
        return end_t;                                // G.C.I.: Passa end_t "para cima"

    } else if (lookahead.tipo == ABRE_COL) {
        lista_declaracao();

        // G.C.I.: FACILITAÇÃO PARA O CASO DE DECLARAÇÃO DE LISTAS E TUPLAS --------------
        return conjunto_sendo_declarado;               // G.C.I.: Passa end "para cima" // (Explicação detalhada no nosso README)
        // -------------------------------------------------------------------------------

    } else if (lookahead.tipo == INPUT) {
        input();

        End end;                                     // G.C.I.: Salva lexema do end antes do consome()
        strcpy(end.lexema, lookahead.lexema);
        strcpy(end.tipo, "DESCONHECIDO");            // G.C.I.: "Flag" para tipo "nulo"

        return end;
        
    } else if (lookahead.tipo == ABRE_PAR) {
        consome(ABRE_PAR);
        return primario_aux();
    } else {
        erro_sintatico("Esperado número, booleano, string, chamada de ID, len, lista, expressão entre parênteses ou tupla"); // ERRO SINTÁTICO
        exit(1);                                      // Redundância
    }
}

End primario_aux() {                                // PRIMARIO_AUX -> ')' | EXPRESSAO TUPLA
    if (lookahead.tipo == FECHA_PAR) {
        consome(FECHA_PAR);

        End end;                                     // G.C.I.: Cria um end vazio auxiliar para conseguir realizar retorno
        end.lexema[0] = '\0';                        // G.C.I.: "Flag" para lexema "nulo"
        end.tipo[0] = '\0';                          // G.C.I.: "Flag" para tipo "nulo"
        return end;

    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        
        End end;                                     // G.C.I.: Cria um end vazio para adquirir end resultante
        end.lexema[0] = '\0';                        // G.C.I.: "Flag" para lexema "nulo"
        end.tipo[0] = '\0';                          // G.C.I.: "Flag" para tipo "nulo"
        
        end = expressao();                           // G.C.I.: Solicita end
        tupla();

        return end;                                  // G.C.I.: Passa end "para cima"
        
    } else {
        erro_sintatico("Esperado ')' ou expressão"); // ERRO SINTÁTICO
        exit(1);                                     // Redundância
    }
}

End chama_id() {                                     // CHAMA_ID -> IDENTIFICADOR CHAMA_ID_AUX

    End end;                                         // G.C.I.: Salva lexema do end antes do consome()
    strcpy(end.lexema, lookahead.lexema);
    end.tipo[0] = '\0';                              // G.C.I.: "Flag" para tipo "nulo"

    consome(IDENTIFICADOR);

    // ------- ANÁLISE SEMÂNTICA --------
    end = semantico_validar_chamada(end);            // G.C.I.: Atualiza o end com o seu tipo guardado na minitabela
    // ----------------------------------

    return chama_id_aux(end);                        // G.C.I.: Passa end "para baixo" E passa o end resultante 
}

End chama_id_aux(End end_esquerda) {                 // CHAMA_ID_AUX -> '[' EXPRESSAO ']' | '(' CHAMADA_FUNCAO_AUX | ε
    if (lookahead.tipo == ABRE_COL) {
        consome(ABRE_COL);

        End end_direita = expressao();               // G.C.I.: Solicita end_direita

        consome(FECHA_COL);

        End end_t;                                   // G.C.I.: Cria o nome temporário para escrever a instruçõa
        sprintf(end_t.lexema, "t%d", proximo_rotulo_t()); // G.C.I.: Cria o lexema com proximo_rotulo_t()
        end_t.tipo[0] = '\0';                        // G.C.I.: "Flag" para tipo "nulo"

        // ----------------- ANÁLISE SEMÂNTICA -----------------
        end_t = semantico_validar_indexacao(end_esquerda, end_t); // G.C.I.: Atualiza o end_t com o seu tipo guardado na minitabela
        // -----------------------------------------------------

        fprintf(instrucoes, "%s = %s[%s]\n", end_t.lexema, end_esquerda.lexema, end_direita.lexema); // G.C.I.: Escreve no arquivo a instrução "end_t = end_esquerda[end_direita]"
        
        return end_t;

    } else if (lookahead.tipo == ABRE_PAR) {
        consome(ABRE_PAR);
        chamada_funcao_aux();
        return end_esquerda;                         // G.C.I.: Faz nada e retorna end_esquerda como estava antes
    }
    //                                               // CHAMA_ID_AUX -> ε
    return end_esquerda;                             // G.C.I.: Faz nada e retorna end_esquerda como estava antes
}

void lista_declaracao() {                            // LISTA_DECLARACAO -> '[' LISTA_DECLARACAO_AUX
    consome(ABRE_COL);
    lista_declaracao_aux();
}

void lista_declaracao_aux() {                        // LISTA_DECLARACAO_AUX -> ']' | LISTA_ARGUMENTOS ']'
    if (lookahead.tipo == FECHA_COL) {
        consome(FECHA_COL);
    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        lista_argumentos(0);                         // G.C.I.: Passa indice_atual "para baixo"
        consome(FECHA_COL);
    } else {
        erro_sintatico("Esperado ']' ou expressão"); // ERRO SINTÁTICO
    }
}

Op op_comp() {                                       // OP_COMP -> '==' | '!=' | '<>' | '<' | '>' | '<=' | '>=' | is | in
    if (lookahead.tipo == IGUAL) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é ==)
        strcpy(op.lexema, "==");

        consome(IGUAL);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == DIFERENTE) {        // PS: Tanto '!=' quanto'<>' são considerados operadores de diferente

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome()
        strcpy(op.lexema, lookahead.lexema);

        consome(DIFERENTE);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == MENOR) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é <)
        strcpy(op.lexema, "<");

        consome(MENOR);

        return op;                                   // G.C.I.: Passa op "para cima"
    } else if (lookahead.tipo == MAIOR) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é >)
        strcpy(op.lexema, ">");

        consome(MAIOR);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == MENOR_IGUAL) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é <=)
        strcpy(op.lexema, "<=");

        consome(MENOR_IGUAL);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == MAIOR_IGUAL) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é >=)
        strcpy(op.lexema, ">=");

        consome(MAIOR_IGUAL);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == IS) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é is)
        strcpy(op.lexema, "is");

        consome(IS);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == IN) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é in)
        strcpy(op.lexema, "in");

        consome(IN);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else {
        erro_sintatico("Esperado operador de comparação"); // ERRO SINTÁTICO
        exit(1);                                      // Redundância
    }
}

Op op_soma() {                                       // OP_SOMA -> '+' | '-'
    if (lookahead.tipo == SOMA) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é +)
        strcpy(op.lexema, "+");

        consome(SOMA);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == SUB) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é -)
        strcpy(op.lexema, "-");

        consome(SUB);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else {
        erro_sintatico("Esperado '+' ou '-'"); // ERRO SINTÁTICO
        exit(1);                                      // Redundância
    }
}

Op op_mult() {                                     // OP_MULT -> '*' | '/' | '%' | '**'
    if (lookahead.tipo == MULT) {
        
        Op op;                                     // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é *)
        strcpy(op.lexema, "*");

        consome(MULT);

        return op;                                 // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == DIV) {

        Op op;                                     // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é /)
        strcpy(op.lexema, "/");

        consome(DIV);

        return op;                                 // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == MOD) {

        Op op;                                     // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é %)
        strcpy(op.lexema, "%");

        consome(MOD);

        return op;                                 // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == EXP) {

        Op op;                                     // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é **)
        strcpy(op.lexema, "**");

        consome(EXP);

        return op;                                 // G.C.I.: Passa op "para cima"

    } else {
        erro_sintatico("Esperado '*', '/', '%' ou '**'"); // ERRO SINTÁTICO
        exit(1);                                      // Redundância
    }
}

Op op_unico() {                                    // OP_UNICO -> '+' | '-' | '~' | not
    if (lookahead.tipo == SOMA) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é +)
        strcpy(op.lexema, "+");

        consome(SOMA);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == SUB) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é -)
        strcpy(op.lexema, "-");

        consome(SUB);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == BIT_NOT) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é ~)
        strcpy(op.lexema, "~");

        consome(BIT_NOT);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else if (lookahead.tipo == NOT) {

        Op op;                                       // G.C.I.: Salva lexema do op antes do consome() (apesar de que, nesse caso, já sabemos que é not)
        strcpy(op.lexema, "not");

        consome(NOT);

        return op;                                   // G.C.I.: Passa op "para cima"

    } else {
        erro_sintatico("Esperado '+', '-', '~' ou 'not'"); // ERRO SINTÁTICO
        exit(1);                                      // Redundância
    }
}

void tupla() {                                       // TUPLA -> ')' | ',' TUPLA_AUX
    if (lookahead.tipo == FECHA_PAR) {
        consome(FECHA_PAR);
    } else if (lookahead.tipo == VIRGULA) {
        consome(VIRGULA);
        tupla_aux();
    } else {
        erro_sintatico("Esperado ')' ou ','"); // ERRO SINTÁTICO
    }
}

void tupla_aux() {                                   // TUPLA_AUX -> ')' | LISTA_ARGUMENTOS ')'
    if (lookahead.tipo == FECHA_PAR) {
        consome(FECHA_PAR);
    } else if (lookahead.tipo == NUMERO || lookahead.tipo == BOOLEANO || lookahead.tipo == STRING || lookahead.tipo == IDENTIFICADOR || lookahead.tipo == LEN || lookahead.tipo == ABRE_COL || lookahead.tipo == ABRE_PAR || lookahead.tipo == SOMA || lookahead.tipo == SUB || lookahead.tipo == BIT_NOT || lookahead.tipo == NOT) {
        lista_argumentos(0);                         // G.C.I.: Obriga a passar "0" "para baixo" mesmo que não seja usado
        consome(FECHA_PAR);
    } else {
        erro_sintatico("Esperado ')' ou expressão"); // ERRO SINTÁTICO
    }
}




// ==============================================================================================
//                                            MAIN
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

    instrucoes = fopen("instrucoes.txt", "w");                        // Cria o arquivo de instruções para o Gerador de Código Intermediário (G.C.I)
    if (instrucoes == NULL) {                                         // Verifica se não deu errado
        printf("Erro ao criar arquivo de tokens do analisador lexico.\n");
        return 1;
    }
    
    // ============================= RODA PROGRAMA =============================
    lookahead = obter_atomo();                                        // Inicializa o lookahead
    lista_instrucoes(); // APRESENTACAO

    // -- ANÁLISE SEMÂNTICA --
    semantico_validar_usados();
    // -----------------------

    printf("Compilacao terminada com sucesso!\n");

    fclose(fonte);
    fclose(saida);
    fclose(instrucoes);
    return 0;
}