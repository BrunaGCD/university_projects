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

#ifndef PROTOCOLO_H
#define PROTOCOLO_H

// ---- Configurações do Jogo ----
#define PORTA_PADRAO 7070     // Porta default (caso o usuário não informe)
#define TAMANHO_BUFFER 1024   // Tamanho para envio e recebimento de mensagens
#define TEMPO_RODADA_SEG 10   // Tempo (segundos) máximo para digitar a palavra
#define TOTAL_RODADAS 5       // Quantidade de rodadas por partida
#define MIN_CARACTERES 5      // Tamanho mínimo que uma palavra deve ter

// ---- Prefixos de Mensagens do Protocolo ----

// (Servidor -> Cliente)
#define PREFIXO_MSG "MSG"
#define PREFIXO_NOME "NOME"
#define PREFIXO_AGUARDE "AGUARDE"
#define PREFIXO_RODADA "RODADA"
#define PREFIXO_RESULTADO "RESULTADO"
#define PREFIXO_PLACAR "PLACAR"
#define PREFIXO_FIM "FIM"
// (Cliente -> Servidor)
#define RESP_NOME "NOME"
#define RESP_PALAVRA "PALAVRA"
#define RESP_TIMEOUT "TIMEOUT"

#endif