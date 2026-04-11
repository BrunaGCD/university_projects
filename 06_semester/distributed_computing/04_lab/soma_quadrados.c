/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
|         ~~ DUPLA ~~         |
|                             |
| Bruna Gonçalves Corte David |
| RA: 10425696                |  Atividade 1 de MPI - Lab 04 - Soma de quadrados com validação
|                             |
| Júlia Andrade               |
| RA: 10428513                |
|                             |
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include <mpi.h>                                      // Biblioteca do MPI
#include <stdio.h>

#define N 40                                          // Tamanho do vetor

int main(int argc, char **argv) {

  int rank, size;                                     // Cada processo MPI possui um ID chamado rank | Total de processos
  int vetor_global[N];                                // Vetor original (só pro root)
  int elementos_por_proc;                             // Carga por processo
  int soma_local = 0;                                 // Acumulador local
  int soma_global = 0;                                // Destino final (só pro root)

// ============================================================================
//                   INICIALIZAÇÃO MPI E REGRAS DE SEGURANÇA
// ============================================================================

  MPI_Init(&argc, &argv);                             // Inicialização MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);               // Obtenção do Rank do Processo
  MPI_Comm_size(MPI_COMM_WORLD, &size);               // Obtenção do total de processos

  if (N % size != 0) {                                // Checa se a divisão é exata
    if (rank == 0) {                                  // Processo com rank 0 executa esta parte do código
      fprintf(stderr, "Erro: Numero de processos nao dividem %d exatamente\n", N);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);                     // Aborta todos os processos MPI se a divisão não for exata
  }

  elementos_por_proc = N / size;                      // Calcula a fatia de cada um
  int vetor_local[elementos_por_proc];                // Buffer local

// ============================================================================
//                        PREPARAÇÃO DOS DADOS NO ROOT
// ============================================================================

  if (rank == 0) {                                    // Processo com rank 0 executa esta parte do código
    for (int i = 0; i < N; i++) {                     // Preenche o vetor
      vetor_global[i] = i + 1;                        // Valores de 1 a 40
    }
  }

// ============================================================================
//                           DISTRIBUIÇÃO DOS DADOS
// ============================================================================

  // MPI_Scatter() -> Distribui partes do vetor 'data' para cada processo
  // vetor_global, elementos_por_proc, MPI_INT -> Envio: buffer (root), qtd e tipo
  // vetor_local, elementos_por_proc, MPI_INT  -> Recebimento: buffer, qtd e tipo
  // 0, MPI_COMM_WORLD                         -> Root (0) e comunicador
  MPI_Scatter(vetor_global, elementos_por_proc, MPI_INT, vetor_local, elementos_por_proc, MPI_INT, 0, MPI_COMM_WORLD);
  
  printf("Processo %d recebeu: ", rank);
  for(int i = 0; i < elementos_por_proc; i++) {       // Lê a fatia
    printf("%d ", vetor_local[i]);
  }
  printf("\n");

// ============================================================================
//                     COMPUTAÇÃO LOCAL E SOMA DE QUADRADOS
// ============================================================================

  for (int i = 0; i < elementos_por_proc; i++) {      // Lê o vetor local
    soma_local += (vetor_local[i] * vetor_local[i]);  // Soma os quadrados
  }
  printf("Processo %d: soma local dos quadrados = %d\n", rank, soma_local);

// ============================================================================
//                           REUNIÃO DOS RESULTADOS
// ============================================================================

  // MPI_Reduce()   -> Coleta dados de todos para um (para o root nesse caso)
  // &soma_local    -> Valor que este processo contribui
  // &soma_global   -> Onde o resultado vai
  // 1              -> Quantidade de elementos
  // MPI_INT        -> Tipo (nesse caso, o tipo é inteiro)
  // MPI_SUM        -> Operação (nesse caso, a operação é soma)
  // 0              -> É o root
  // MPI_COMM_WORLD -> Comunicador
  MPI_Reduce(&soma_local, &soma_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

// ============================================================================
//                        VALIDAÇÃO SEQUENCIAL NO ROOT
// ============================================================================

  if (rank == 0) {                                    // Processo com rank 0 executa esta parte do código
      
    int soma_esperada = (N * (N + 1) * (2 * N + 1)) / 6; // Fórmula da soma dos quadrados

    printf("\nProcesso 0: soma paralela dos quadrados = %d\n", soma_global);
    printf("Processo 0: soma sequencial esperada    = %d\n\n", soma_esperada);

    if (soma_global == soma_esperada) {              // Checa o resultado
      printf("✅ Os valores conferem!\n");          // Deu certo
    } else {                                         // Deu ruim
      printf("❌ Os valores NÃO conferem!\n");      // Aviso de erro
    }
  }

  MPI_Finalize();                                   // Finalização MPI

  return 0;
}