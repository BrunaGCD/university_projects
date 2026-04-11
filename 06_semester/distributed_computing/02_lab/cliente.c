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
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "protocolo.h"
#include "jogo.h"

static char ultima_palavra[TAMANHO_BUFFER] = ""; // Guarda o que foi digitado na última rodada para exibir o RESULTADO

// ============================================================================
//                         INTERPRETADOR DO PROTOCOLO
// ============================================================================

void processar_mensagem_servidor(char *linha, int sock_fd) { // Lê o prefixo da mensagem separada por '\n' e toma a ação apropriada
    if (strncmp(linha, PREFIXO_NOME "|", 5) == 0) { // Servidor pediu o nome
        printf("\n  Digite seu nome: ");
        char nome[50];
        fgets(nome, sizeof(nome), stdin); // Lê o nome do input
        remover_quebra_linha(nome); // Remove \n
        printf("  Bem-vindo, %s!\n", nome);

        char resp[TAMANHO_BUFFER];
        snprintf(resp, sizeof(resp), "%s|%s\n", RESP_NOME, nome); // Monta o pacote do nome
        send(sock_fd, resp, strlen(resp), 0); // Envia o nome de volta formatado
    }
    else if (strncmp(linha, PREFIXO_AGUARDE "|", 8) == 0) { // Aguardando outro jogador
        printf("\n  ⏳ Conectado! %s\n", linha + 8);
    }
    else if (strncmp(linha, PREFIXO_MSG "|", 4) == 0) { // Recebeu mensagem genérica do servidor
        printf("\n  🎮 Batalha de Palavras! %s — %d rodadas. Boa sorte!\n", linha + 4, TOTAL_RODADAS);
    }
    else if (strncmp(linha, PREFIXO_RODADA "|", 7) == 0) { // Servidor anunciou nova rodada
        int rodada, tempo;
        char letra;
        
        sscanf(linha, "RODADA|%d|%c|%d", &rodada, &letra, &tempo); // Extrai as informações separadas por pipe (|)

        // Print do Banner de Rodada
        printf("\n  ╔══════════════════════════════════╗\n");
        printf("  ║        RODADA %d de %d             ║\n", rodada, TOTAL_RODADAS);
        printf("  ║  Letra: [%c]   Tempo: %d seg      ║\n", letra, tempo);
        printf("  ║  Mínimo: %d caracteres            ║\n", MIN_CARACTERES);
        printf("  ╚══════════════════════════════════╝\n");
        printf("  Sua palavra: ");
        fflush(stdout); // Garante que o texto apareça no console antes da digitação

        // ---- Leitura de entrada com limite de tempo ----
        int status = receber_com_timeout(ultima_palavra, sizeof(ultima_palavra), tempo); // Lê entrada respeitando o limite

        char pacote_envio[TAMANHO_BUFFER];
        if (status == 1 && strlen(ultima_palavra) > 0) { // Se usuário digitou a tempo
            printf("  Enviado: \"%s\" — aguardando resultado...\n", ultima_palavra);
            snprintf(pacote_envio, sizeof(pacote_envio), "%s|%s\n", RESP_PALAVRA, ultima_palavra);
        } else { // Se o tempo esgotou ou apertou enter vazio
            printf("\n  ⏰ Tempo esgotado (ou entrada vazia)!\n");
            strcpy(ultima_palavra, "-"); // Para print depois
            snprintf(pacote_envio, sizeof(pacote_envio), "%s|\n", RESP_TIMEOUT);
        }
        
        send(sock_fd, pacote_envio, strlen(pacote_envio), 0); // Envia a ação para o servidor
    }
    else if (strncmp(linha, PREFIXO_RESULTADO "|", 10) == 0) { // Recebeu a pontuação da última rodada
        char *resultado_ponto = linha + 10;
        if (strcmp(resultado_ponto, "+1") == 0) { // Palavra foi aceita
            printf("  📋 Palavra \"%s\" válida! +1 ponto.\n", ultima_palavra);
        } else { 
            // Se a última palavra foi "-", significa que foi timeout. Não exibe o erro de sintaxe.
            if (strcmp(ultima_palavra, "-") == 0) {
                printf("  ⏳ Nenhum ponto marcado (tempo esgotado).\n");
            } else { // Caso contrário, o usuário digitou uma palavra errada de fato
                printf("  ❌ Palavra \"%s\" inválida ou repetida! 0 pontos.\n", ultima_palavra);
            }
        }
    }
    else if (strncmp(linha, PREFIXO_PLACAR "|", 7) == 0) { // Recebeu os placares atualizados
        char nome1[50] = "", nome2[50] = "";
        int pts1 = 0, pts2 = 0;
        sscanf(linha, "PLACAR|%[^|]|%d|%[^|]|%d", nome1, &pts1, nome2, &pts2); // Adquire dados dos 2 jogadores

        printf("  ┌─────────────────────────────┐\n");
        printf("  │  PLACAR: %s %d x %d %s \n", nome1, pts1, pts2, nome2);
        printf("  └─────────────────────────────┘\n");
    }
    else if (strncmp(linha, PREFIXO_FIM "|", 4) == 0) { // Servidor encerrou a partida
        printf("\n  🏆 FIM DE JOGO! %s\n\n", linha + 4);
    }
}

// ============================================================================
//                              MAIN DO CLIENTE
// ============================================================================

int main(int argc, char *argv[]) {
    char ip_servidor[50] = "127.0.0.1";
    int porta = PORTA_PADRAO;

    if (argc >= 3) { // Extrair argumentos da linha de comando, se fornecidos
        strcpy(ip_servidor, argv[1]);
        porta = atoi(argv[2]);
    }

    printf("╔══════════════════════════════════════╗\n");
    printf("║      BATALHA DE PALAVRAS — Cliente   ║\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("  Conectando a %s:%d...\n", ip_servidor, porta);

    // ---- Configurar socket e conectar ----
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0); // Socket TCP IPv4
    if (sock_fd < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // Inicializa a struct de endereço com zero
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(porta); // Converte ordem dos bytes

    if (inet_pton(AF_INET, ip_servidor, &serv_addr.sin_addr) <= 0) { // Converte IP string pra binário
        perror("Endereço IP inválido");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { // Estabelece conexão com o servidor
        perror("Erro ao conectar no servidor");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("  Conectado!\n");

    // ---- Loop de recepção de pacotes ----
    char buffer_recepcao[TAMANHO_BUFFER * 2];

    while (1) {
        memset(buffer_recepcao, 0, sizeof(buffer_recepcao)); // Zera o buffer para receber novos dados
        
        int bytes = recv(sock_fd, buffer_recepcao, sizeof(buffer_recepcao) - 1, 0); // Bloqueia a execução até receber dados do servidor
        if (bytes <= 0) { // Servidor desconectado
            printf("\n  ❌ Conexão com o servidor encerrada.\n");
            break;
        }

        // O TCP (stream) pode "grudar" várias mensagens. A lógica abaixo divide o buffer pelo delimitador '\n':
        char *ponteiro_atual = buffer_recepcao; // Marca onde a mensagem atual começa
        char *posicao_quebra_linha;             // Guarda onde a mensagem atual termina (posição do '\n')
        
        while ((posicao_quebra_linha = strchr(ponteiro_atual, '\n')) != NULL) { // Procura a primeira quebra de linha
            *posicao_quebra_linha = '\0'; // Substitui o \n por um terminador de string nulo temporário
            
            processar_mensagem_servidor(ponteiro_atual, sock_fd); // Envia linha pro interpretador processar

            if (strncmp(ponteiro_atual, PREFIXO_FIM "|", 4) == 0) { // Se a mensagem for de FIM, encerra o programa
                close(sock_fd);
                return EXIT_SUCCESS;
            }

            ponteiro_atual = posicao_quebra_linha + 1; // Avança o ponteiro para a próxima mensagem do bloco
        }
    }

    close(sock_fd); // Desconecta
    return EXIT_SUCCESS;
}