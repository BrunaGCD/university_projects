/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
|         ~~ DUPLA ~~         |
|                             |
| Bruna Gonçalves Corte David |
| RA: 10425696                |  Atividade 3 de MPI - Lab 05 - Transformação paralela de dados com MPI
|                             |
| Júlia Andrade               |
| RA: 10428513                |
|                             |
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include <mpi.h>                                      // Biblioteca do MPI
#include <stdio.h>

#define DATA_SIZE 100
#define NUM_PROCS_REQ 5

int main(int argc, char **argv) {

  int rank, size;                                     // Cada processo MPI possui um ID chamado rank | Total de processos
  int elementos_por_proc;                             // Elementos por processo
  int vetor_original[DATA_SIZE];                      // Vetor de dados inicial (só pro root)
  int vetor_transformado[DATA_SIZE];                  // Vetor para reunir resultados finais (só pro root)

// ============================================================================
//                   INICIALIZAÇÃO MPI E REGRAS DE SEGURANÇA
// ============================================================================

  MPI_Init(&argc, &argv);                             // Inicialização MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);               // Obtenção do Rank do Processo
  MPI_Comm_size(MPI_COMM_WORLD, &size);               // Obtenção do total de processos

  if (size != NUM_PROCS_REQ) {                        // Checa se foram criados exatamente 5 processos
    if (rank == 0) {                                  // Processo com rank 0 executa esta parte do código
      fprintf(stderr, "Erro: Devem ser passados exatamente %d processos.\n", NUM_PROCS_REQ);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);                     // Aborta todos os processos MPI caso não sejam exatamente 5 processos
  }

  elementos_por_proc = DATA_SIZE / size;              // Cada processo ficará responsável por exatamente 20 elementos (100 / 5 = 20)
  int vetor_local[elementos_por_proc];                // Buffer local para a parte recebida
  int vetor_local_result[elementos_por_proc];         // Buffer local para guardar os resultados da transformação

// ============================================================================
//                        PREPARAÇÃO DOS DADOS NO ROOT
// ============================================================================

  if (rank == 0) {                                    // Processo com rank 0 executa esta parte do código
    
    for (int i = 0; i < DATA_SIZE; i++) {             // Preenche o vetor
      vetor_original[i] = i + 1;                      // Valores de 1 a 100
    }

    printf("[Processo 0] Vetor original: [");
    for (int i = 0; i < DATA_SIZE; i++) {
      printf("%d", vetor_original[i]);
      if (i < DATA_SIZE - 1) printf(", ");
    }
    printf("]\n\n");
  }

// ============================================================================
//                           DISTRIBUIÇÃO DOS DADOS
// ============================================================================

  // MPI_Scatter() -> Distribui partes do vetor 'data' para cada processo
  // vetor_global, elementos_por_proc, MPI_INT -> Envio: buffer (root), qtd e tipo
  // vetor_local, elementos_por_proc, MPI_INT  -> Recebimento: buffer, qtd e tipo
  // 0, MPI_COMM_WORLD                         -> Root (0) e comunicador
  MPI_Scatter(vetor_original, elementos_por_proc, MPI_INT, vetor_local, elementos_por_proc, MPI_INT, 0, MPI_COMM_WORLD);

// ============================================================================
//                      COMPUTAÇÃO LOCAL E TRANSFORAÇÃO
// ============================================================================

  for (int i = 0; i < elementos_por_proc; i++) {             // Lê o vetor local
    vetor_local_result[i] = vetor_local[i] * vetor_local[i]; // Função de transformação nos elementos (quadrado de cada elemento)
  }

// ============================================================================
//                           REUNIÃO DOS RESULTADOS
// ============================================================================

  // MPI_Gather()       -> Coleta vetores de todos para um (para o root nesse caso) e os junta um ao lado do outro
  // vetor_local_result -> Vetor que este processo contribui
  // elementos_por_proc -> Quantidade de elementos sendo enviados (20)
  // MPI_INT            -> Tipo dos elementos sendo enviados (nesse caso, o tipo é inteiro)
  // vetor_transformado -> Onde o resultado vai
  // elementos_por_proc -> Quantidade de elementos sendo recebidos (20)
  // MPI_INT            -> Tipo dos elementos sendo recebidos (nesse caso, o tipo é inteiro)
  // 0                  -> É o root
  // MPI_COMM_WORLD     -> Comunicador
  MPI_Gather(vetor_local_result, elementos_por_proc, MPI_INT, vetor_transformado, elementos_por_proc, MPI_INT, 0, MPI_COMM_WORLD);

// ============================================================================
//                   IMPRESSÃO DO VETOR TRANSFORMADO NO ROOT
// ============================================================================

  if (rank == 0) {                                    // Processo com rank 0 executa esta parte do código

    printf("[Processo 0] Vetor transformado: [");
    for (int i = 0; i < DATA_SIZE; i++) {
      printf("%d", vetor_transformado[i]);
      if (i < DATA_SIZE - 1) printf(", ");
    }
    printf("]\n");
  }

  MPI_Finalize();                                     // Finalização MPI

  return 0;
}