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
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>

#include "protocolo.h"
#include "jogo.h"

// ============================================================================
//                              ESTRUTURAS DE DADOS
// ============================================================================

typedef struct {
    int fd;
    char nome[50];
    int pontos;
} Jogador;

typedef struct {
    int id_partida;
    Jogador j1;
    Jogador j2;
} Partida;

// ============================================================================
//                              FUNÇÕES AUXILIARES
// ============================================================================

void enviar_mensagem(int fd, const char *msg) { // Envia strings para um socket
    send(fd, msg, strlen(msg), 0);
}

bool strings_iguais_case_insensitive(const char *s1, const char *s2) { // Compara duas strings de forma case-insensitive
    if (strlen(s1) != strlen(s2)) {
        return false;
    }
    for (int i = 0; s1[i] != '\0'; i++) {
        if (tolower(s1[i]) != tolower(s2[i])) {
            return false;
        }
    }
    return true;
}

// ============================================================================
//                  THREAD DA PARTIDA (por dupla de clientes)
// ============================================================================

void *executar_rodada(void *arg) { // Função de pthread independente (Recebe struct Partida com FDs e nomes dos 2 jogadores)
    Partida *p = (Partida *)arg;
    char msg[TAMANHO_BUFFER];
    char buffer1[TAMANHO_BUFFER], buffer2[TAMANHO_BUFFER];

    printf("[Partida #%d] Jogadores: %s vs %s\n", p->id_partida, p->j1.nome, p->j2.nome); // Print informando quem vai jogar

    // ---- Anuncia início da partida ----
    snprintf(msg, sizeof(msg), "%s|%s vs %s\n", PREFIXO_MSG, p->j1.nome, p->j2.nome); // Monta a string de aviso
    enviar_mensagem(p->j1.fd, msg); // Envia para Jogador 1
    enviar_mensagem(p->j2.fd, msg); // Envia para Jogador 2

    // ---- Executa as 5 Rodadas ----
    for (int rodada = 1; rodada <= TOTAL_RODADAS; rodada++) {
        char letra = sortear_letra();                       // Sorteia letra dessa rodada
        printf("  [Rodada %d] Letra: %c\n", rodada, letra); // Print da letra sorteada no console do servidor

        // ---- Avisa da rodada para os clientes ----
        snprintf(msg, sizeof(msg), "%s|%d|%c|%d\n", PREFIXO_RODADA, rodada, letra, TEMPO_RODADA_SEG); // Monta a mensagem de rodada
        enviar_mensagem(p->j1.fd, msg); // Envia mensagem para Jogador 1
        enviar_mensagem(p->j2.fd, msg); // Envia mensagem para Jogador 2

        memset(buffer1, 0, sizeof(buffer1)); // Zera o buffer do J1 por segurança
        memset(buffer2, 0, sizeof(buffer2)); // Zera o buffer do J2 por segurança

        // ---- Recebe as palavras simultaneamente ----
        // Isso é bloqueante, mas não 100% pq o cliente tem timeout de 10s, ele vai enviar TIMEOUT| quando o tempo passar
        recv(p->j1.fd, buffer1, TAMANHO_BUFFER - 1, 0); // Recebe palavra do J1 (ou "TIMEOUT|")
        recv(p->j2.fd, buffer2, TAMANHO_BUFFER - 1, 0); // Recebe palavra do J2 (ou "TIMEOUT|")

        // ---- Extrai as palavras enviadas pelos clientes ----
        remover_quebra_linha(buffer1); // Limpa quebras de linha do buffer do J1
        remover_quebra_linha(buffer2); // Limpa quebras de linha do buffer do J2

        char pal1[TAMANHO_BUFFER] = ""; // Variável para guardar a palavra do J1 (ou ficar vazia se TIMEOUT)
        char pal2[TAMANHO_BUFFER] = ""; // Variável para guardar a palavra do J2 (ou ficar vazia se TIMEOUT)

        if (strncmp(buffer1, RESP_PALAVRA "|", 8) == 0) { // Ignorando os prefixos "PALAVRA|" ou "TIMEOUT|"
            strcpy(pal1, buffer1 + 8);
        }
        if (strncmp(buffer2, RESP_PALAVRA "|", 8) == 0) { // Ignorando os prefixos "PALAVRA|" ou "TIMEOUT|"
            strcpy(pal2, buffer2 + 8);
        }

        // ---- Valida Palavras ----
        bool v1 = (strlen(pal1) > 0) && validar_palavra(pal1, letra); // Valida tamanho e regra pro J1
        bool v2 = (strlen(pal2) > 0) && validar_palavra(pal2, letra); // Valida tamanho e regra pro J2

        if (v1 && v2 && strings_iguais_case_insensitive(pal1, pal2)) { // Ninguém pontua se as palavras forem iguais (regra bônus)
            v1 = false;
            v2 = false;
        }

        if (v1) {
            p->j1.pontos++; // Pontua J1 se a palavra for válida
        }
        if (v2) { 
            p->j2.pontos++; // Pontua J2 se a palavra for válida
        }

        // printf() -> Mostra o Print da rodada, informando as palavras digitadas e se foram validadas ou rejeitadas
        // strlen(pal) > 0 ? pal : "TIMEOUT" -> Iprimir TIMEOUT se o jogador deixou o tempo esgotar
        printf("  [Rodada %d] %s=\"%s\"(%s) | %s=\"%s\"(%s) | Placar: %d x %d\n", rodada, p->j1.nome, strlen(pal1) > 0 ? pal1 : "TIMEOUT", v1 ? "ok" : "err", p->j2.nome, strlen(pal2) > 0 ? pal2 : "TIMEOUT", v2 ? "ok" : "err", p->j1.pontos, p->j2.pontos);

        // ---- Envio dos Resultados e Placar Até Agora ----
        snprintf(msg, sizeof(msg), "%s|%s\n", PREFIXO_RESULTADO, v1 ? "+1" : "0"); // Monta string de +1 ponto ou 0
        enviar_mensagem(p->j1.fd, msg); // Envia mensagem resultado do J1

        snprintf(msg, sizeof(msg), "%s|%s\n", PREFIXO_RESULTADO, v2 ? "+1" : "0"); // Monta string de +1 ponto ou 0
        enviar_mensagem(p->j2.fd, msg); // Envia mensagem resultado do J2

        // Junta os nomes e pontos atuais dos dois jogadores para montar a string do placar geral
        snprintf(msg, sizeof(msg), "%s|%s|%d|%s|%d\n", PREFIXO_PLACAR, p->j1.nome, p->j1.pontos, p->j2.nome, p->j2.pontos);
        
        enviar_mensagem(p->j1.fd, msg); // Atualiza J1
        enviar_mensagem(p->j2.fd, msg); // Atualiza J2
    }

    // ---- Fim da Partida ----
    if (p->j1.pontos > p->j2.pontos) { // Print de vitória J1
        snprintf(msg, sizeof(msg), "%s|%s venceu!\n", PREFIXO_FIM, p->j1.nome);
        printf("[Partida #%d] 🏆 %s venceu! Placar final: %s %d x %d %s\n", p->id_partida, p->j1.nome, p->j1.nome, p->j1.pontos, p->j2.pontos, p->j2.nome);
    } else if (p->j2.pontos > p->j1.pontos) { // Print de vitória J2
        snprintf(msg, sizeof(msg), "%s|%s venceu!\n", PREFIXO_FIM, p->j2.nome);
        printf("[Partida #%d] 🏆 %s venceu! Placar final: %s %d x %d %s\n", p->id_partida, p->j2.nome, p->j1.nome, p->j1.pontos, p->j2.pontos, p->j2.nome);
    } else {
        snprintf(msg, sizeof(msg), "%s|Empate!\n", PREFIXO_FIM); // Prepara msg de empate
        printf("[Partida #%d] 🤝 Empate! Placar final: %s %d x %d %s\n", p->id_partida, p->j1.nome, p->j1.pontos, p->j2.pontos, p->j2.nome);
    }

    enviar_mensagem(p->j1.fd, msg); // Envia mensagem do fim da partida pro J1
    enviar_mensagem(p->j2.fd, msg); // Envia mensagem do fim da partida pro J2

    // Encerrar conexões e liberar a memória da struct enviada para a thread (para evitar FD leak)
    close(p->j1.fd); // Fecha conexão com J1
    close(p->j2.fd); // Fecha conexão com J2
    free(p); // Libera o bloco alocado na main
    
    pthread_exit(NULL); // Encerra a thread e finaliza essa partida
}

// ============================================================================
//                   MAIN DO SERVIDOR PARA ACEITAR CONEXÕES
// ============================================================================

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN); // Evita que o servidor feche abruptamente se tentar enviar msg para cliente desconectado

    int porta = PORTA_PADRAO;  // Porta padrão
    if (argc >= 2) {           // Substitui a porta padrão se o usuário passar via argumento
        porta = atoi(argv[1]); // Converte o argumento string para inteiro (porta)
    }

    srand(time(NULL)); // Inicializa semente do sorteio de letras


    // ---- Criação e Configuração do Socket ----
    
    // AF_INET -> Família de endereços IPv4
    // SOCK_STREAM -> Socket TCP
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { // Verifica se houve erro na criação do socket
        perror("Erro ao criar o socket do servidor");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // Evita erro de porta presa (TIME_WAIT) forçando reuso

    struct sockaddr_in serv_addr; // Estrutura para armazenar as informações do endereço do servidor
    memset(&serv_addr, 0, sizeof(serv_addr)); // Zera a struct pra não ter lixo na memória
    serv_addr.sin_family = AF_INET; // Configura o tipo para IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Faz o bind em todas as interfaces de rede (0.0.0.0)
    serv_addr.sin_port = htons(porta); // Converte a porta para a ordem de bytes da rede (Big Endian)

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { // Associa o socket a essa porta e IP, verificando se deu erro
        perror("Erro no bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) { // Coloca o socket em modo de escuta, com backlog de 10 conexões, verificando se deu erro
        perror("Erro no listen()");
        exit(EXIT_FAILURE);
    }

    // ---- Print do Banner ----
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║      BATALHA DE PALAVRAS — Servidor          ║\n");
    printf("║  Porta: %-37d║\n", porta);
    printf("║  Aguardando jogadores (pares de 2)...        ║\n");
    printf("╚══════════════════════════════════════════════╝\n\n");

    int id_partidas = 0; // Controla o identificador das partidas iniciadas

    // ---- Loop Principal de Pareamento ----
    while (1) {
        struct sockaddr_in cli_addr; // Estrutura para armazenar as informações do endereço do cliente que se conectar
        socklen_t cli_len = sizeof(cli_addr); // Variável para guardar o tamanho da struct do cliente (necessário para accept)
        char buffer[TAMANHO_BUFFER]; // Buffer auxiliar para comunicação com os clientes (perguntar nome, etc)

        // ---- Aceitar Jogador 1 ----
        int fd1 = accept(server_fd, (struct sockaddr *)&cli_addr, &cli_len); // Bloqueia e aguarda a primeira conexão
        if (fd1 < 0) {  // Se deu erro, tenta de novo
            continue; 
        }
        
        // inet_ntoa() -> Converte o IP em bytes de volta pra string legível
        // ntohs() -> Converte a porta da rede de volta pra int da nossa máquina
        printf("[+] Jogador conectou: %s:%d (fd=%d)\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), fd1);
        
        enviar_mensagem(fd1, PREFIXO_NOME "|\n"); // Pergunta o nome do J1
        memset(buffer, 0, sizeof(buffer)); // Limpa o buffer para evitar leitura de dados residuais
        recv(fd1, buffer, sizeof(buffer) - 1, 0); // Espera J1 responder o nome
        remover_quebra_linha(buffer); // Limpa \n
        
        char nome1[50] = "Jogador 1"; // Nome base se falhar
        if (strncmp(buffer, RESP_NOME "|", 5) == 0) { // Extrai o nome se o pacote vier certinho
            strcpy(nome1, buffer + 5);
        }

        printf("[*] Aguardando mais 1 jogador(es)...\n"); // Print de espera
        enviar_mensagem(fd1, PREFIXO_AGUARDE "|Aguardando oponente...\n"); // Manda J1 esperar

        // ---- Aceitar Jogador 2 ----
        int fd2 = accept(server_fd, (struct sockaddr *)&cli_addr, &cli_len); // Bloqueia e aguarda a segunda conexão
        if (fd2 < 0) {  // Se deu erro, tenta de novo
            close(fd1); // Derruba o J1 se der erro no J2
            continue; 
        }
        printf("[+] Jogador conectou: %s:%d (fd=%d)\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), fd2); // Print da conexão de J2
        
        enviar_mensagem(fd2, PREFIXO_NOME "|\n"); // Pergunta o nome de J2
        memset(buffer, 0, sizeof(buffer)); // Zera a memória do buffer
        recv(fd2, buffer, sizeof(buffer) - 1, 0); // Lê a resposta de J2
        remover_quebra_linha(buffer); // Limpa \n
        
        char nome2[50] = "Jogador 2"; // Nome base se falhar
        if (strncmp(buffer, RESP_NOME "|", 5) == 0) { // Extrai o nome se o pacote vier certinho
            strcpy(nome2, buffer + 5);
        }

        // ---- Monta Struct da Partida e Lança Thread ----
        Partida *p = malloc(sizeof(Partida)); // Alocação dinâmica para manter os dados da partida após o fim da iteração
        if (p == NULL) {
            perror("Erro ao alocar memória para a partida"); // Exibe erro caso a alocação de memória falhe
            close(fd1); // Desconecta J1
            close(fd2); // Desconecta J2
            continue; // Recomeça o pareamento
        }

        p->id_partida = ++id_partidas; // Define ID da partida (podem ter mais de uma ao msm tempo)
        
        p->j1.fd = fd1; // Salva socket J1
        p->j1.pontos = 0; // Inicia J1 sem pontos
        strcpy(p->j1.nome, nome1); // Copia o nome do J1

        p->j2.fd = fd2; // Salva socket J2
        p->j2.pontos = 0; // Inicia J2 sem pontos
        strcpy(p->j2.nome, nome2); // Copia o nome do J2

        pthread_t tid; // Variável para guardar o ID da thread que vai rodar a partida (para não perder a referência depois de criar a thread)
        
        // pthread_create() -> Inicializa nova thread, rodando executar_rodada e recebendo p (o struct da partida recém montada) 
        if (pthread_create(&tid, NULL, executar_rodada, (void *)p) == 0) {
            pthread_detach(tid); // A thread limpará sua própria memória e FDs ao encerrar 
        } else {
            perror("Erro ao criar thread da partida"); // Print no caso de erro ao criar a thread
            close(fd1); // Desconecta J1
            close(fd2); // Desconecta J2
            free(p); // Previne vazamento de memória do struct da partida que não foi usado
        }
    }

    close(server_fd); // Close FD do servidor (loop infinito impede de chegar aqui normalmente)
    return 0;
}