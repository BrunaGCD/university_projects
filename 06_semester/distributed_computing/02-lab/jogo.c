/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
|         ~~ DUPLA ~~         |
|                             |
| Bruna Gonçalves Corte David |
| RA: 10425696                |  Atividade - Lab 02 - Batalha de Palavras em Rede
|                             |
| Júlia Andrade               |
| RA: 10428513                |
|                             |
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/select.h> // Para a função select() usada no timeout

#include "jogo.h"
#include "protocolo.h"



// ============================================================================
//                              LÓGICA DO JOGO
// ============================================================================
/* Implementação da interface definida em jogo.h
   Possui a validação das regras do jogo no lado do cliente
   Possui a função de leitura com timeout
   Limita os 10 segundos da rodada no lado do cliente */

char sortear_letra(void) {
    // rand() -> semente (srand) é inicializada na main do servidor
    // (rand() % 26) -> Sorteia um número de 0 a 25 e soma com o código ASCII de 'A'
    return 'A' + (rand() % 26);
}

bool validar_palavra(const char *palavra, char letra_rodada) {
    if (strlen(palavra) < MIN_CARACTERES) { // Retorna falso se a palavra não tiver o tamanho mínimo
        return false;
    }

    if (toupper(palavra[0]) != toupper(letra_rodada)) { // Retorna falso se a palavra não tiver a letra inicial correta
        return false;
    }

    for (int i = 0; palavra[i] != '\0'; i++) { // Percorre caracteres da palavra e retorna falso se encontrar algum que não seja letra
        if (!isalpha(palavra[i])) {
            return false;
        }
    }

    // Se passou por todas as verificações, a palavra é válida NO LADO DO CLIENTE*
    // *Ainda falta validar no lado do servidor se o outro cliente rival enviou a mesma palavra
    return true;
}



// ============================================================================
//                           AUXILIARES DE STRING
// ============================================================================

void remover_quebra_linha(char *string) { // Remove \n, \r ou espaços em branco que possam estar no final da string
    size_t tamanho = strlen(string);
    while (tamanho > 0 && (string[tamanho - 1] == '\n' || string[tamanho - 1] == '\r' || string[tamanho - 1] == ' ')) {
        string[tamanho - 1] = '\0';
        tamanho--;
    }
}

int receber_com_timeout(char *buffer, int tamanho_maximo, int segundos) { // Lê a entrada do usuário com um tempo limite usando select()
    fd_set fds_leitura;                 // "Lista" de FDs (vai monitorar apenas teclado agora)
    struct timeval tempo_limite;
    int resultado_select;

    // Configura o fd_set para monitorar o teclado (STDIN_FILENO = 0)
    FD_ZERO(&fds_leitura);              // Zera (limpa) a lista de FDs
    FD_SET(STDIN_FILENO, &fds_leitura); // Monitora teclado (FD = 0)

    // Configura o tempo limite para o select
    tempo_limite.tv_sec = segundos;     // Segundos para o timeout
    tempo_limite.tv_usec = 0;           // Microsegundos (0)

    // Chamar o select() para aguardar entrada ou timeout
    resultado_select = select(STDIN_FILENO + 1, &fds_leitura, NULL, NULL, &tempo_limite);

    if (resultado_select < 0) { // Ocorreu um erro na execução do select
        perror("Erro no select()");
        return -1;
    } else if (resultado_select == 0) { // O tempo se esgotou (timeout) e o usuário não digitou nada
        return 0;
    } else { // Ler a entrada do teclado com segurança
        if (FD_ISSET(STDIN_FILENO, &fds_leitura)) {             // Verifica se o teclado foi a fonte da entrada
            if (fgets(buffer, tamanho_maximo, stdin) != NULL) { // Lê a linha digitada pelo usuário
                remover_quebra_linha(buffer);                   // Limpa \n do final da string
                return 1;
            }
        }
    }

    return -1; // Falha ao tentar ler a string
}