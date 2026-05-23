/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
|         ~~ DUPLA ~~         |
|                             |
| Bruna Gonçalves Corte David |
| RA: 10425696                |  Atividade 2 de MPI - Lab 05 - Cálculo de média com MPI_Reduce
|                             |
| Júlia Andrade               |
| RA: 10428513                |
|                             |
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include <mpi.h>                                      // Biblioteca do MPI
#include <stdio.h>
#include <stdlib.h>                                   // Biblioteca do atoi, srand e rand
#include <time.h>                                     // Biblioteca do time

int main(int argc, char **argv) {

  int rank, size;                                     // Cada processo MPI possui um ID chamado rank | Total de processos
  int N;                                              // Tamanho do vetor local
  float soma_local = 0.0;                             // Acumulador local
  float media_local = 0.0;                            // Média do acumulador local
  float soma_global = 0.0;                            // Destino final / soma de todos os processos (só pro root)

// ============================================================================
//                   INICIALIZAÇÃO MPI E REGRAS DE SEGURANÇA
// ============================================================================

  MPI_Init(&argc, &argv);                             // Inicialização MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);               // Obtenção do Rank do Processo
  MPI_Comm_size(MPI_COMM_WORLD, &size);               // Obtenção do total de processos

  if (argc != 2) {                                    // Checa se N foi passado como argumento no terminal
    if (rank == 0) {                                  // Processo com rank 0 executa esta parte do código
      fprintf(stderr, "Erro: N não foi passado como argumento pelo terminal\n");
    }
    MPI_Abort(MPI_COMM_WORLD, 1);                     // Aborta todos os processos MPI
  }

  N = atoi(argv[1]);                                  // Argumento é recebido como sring, então converte para int

// ============================================================================
//                 GERAÇÃO E SOMA DOS DADOS NO PROCESSO LOCAL
// ============================================================================
  
  srand(time(NULL) + rank);                           // Semente única para cada processo gera números aleatórios diferentes em cada processo
  for (int i = 0; i < N; i++) {                       // Gera N números aleatórios diferentes em cada processo
    float x = rand() / (float)RAND_MAX;               // Gera um número aleatório entre 0 e 1
    soma_local += x;                                  // Soma no acumulador local
  }
  media_local = soma_local / N;                       // Calcula a média do acumulador local

  printf("[Processo %d] Soma local: %.3f, Média local: %.4f\n", rank, soma_local, media_local);

// ============================================================================
//                           REUNIÃO DOS RESULTADOS
// ============================================================================

  // MPI_Reduce()   -> Coleta dados de todos para um (para o root nesse caso)
  // &soma_local    -> Valor que este processo contribui
  // &soma_global   -> Onde o resultado vai
  // 1              -> Quantidade de elementos
  // MPI_FLOAT      -> Tipo (nesse caso, o tipo é float)
  // MPI_SUM        -> Operação (nesse caso, a operação é soma)
  // 0              -> É o root
  // MPI_COMM_WORLD -> Comunicador
  MPI_Reduce(&soma_local, &soma_global, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

// ============================================================================
//             SOMA E MÉDIA GLOBAL DOS RESULTADOS REUNIDOS NO ROOT
// ============================================================================

  if (rank == 0) {                                    // Processo com rank 0 executa esta parte do código

    float media_global = soma_global / (N * size);    // Calcula média global com a soma global de todos os processos

    printf("\n[Soma global] %.3f\n", soma_global);
    printf("[Média global] %.4f\n", media_global);
  }

  MPI_Finalize();                                     // Finalização MPI

  return 0;
}