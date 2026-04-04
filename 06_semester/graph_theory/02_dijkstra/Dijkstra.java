public class Dijkstra {
  public static void main(String[] args) {

    // ======================================================================= //
    //                             INICIALIZAÇÃO                               //
    // ======================================================================= //
    int[][] matriz_adj = {
      {0, 20, 30, 50},
      {20, 0, 40, 15},
      {30, 40, 0, 15},
      {50, 15, 15, 0}
    };
    
    int n = 4;                           // Qtd total de nós na matriz de adjacências
    int origem = 2;                      // CIDADE 3 - Indexação vai de 0 a 3, então a cidade 3 é representada pelo índice 2
    
    int[] dist = new int[n];             // Vetor de distâncias    (distâncias de cada nó em relação à cidade 3)
    int[] pred = new int[n];             // Vetor de predecessores (se pred de 4 for 3 e pred de 3 for 2, então a rota foi 2 -> 3 -> 4)
    boolean[] visitado = new boolean[n]; // Vetor de visitados     (nós simplesmente verificados não necessariamente foram visitados)
    
    for (int i = 0; i < n; i++) {        // Inicializando vetores
      dist[i] = Integer.MAX_VALUE;       // Todos os nós começam com sua distância em relação à CIDADE 3 infinita...*
      pred[i] = -1;                      // Nenhum nó possui predecessores definido
    }
    dist[origem] = 0;                    // *...Nó inicial é CIDADE 3 (índice 2), logo, a distância da CIDADE 3 em relação a ela mesma é 0
    

    // ======================================================================= //
    //                                DIJKSTRA                                 //
    // ======================================================================= //
    for (int i = 0; i < n - 1; i++) {
      
      // --- Acha o nó não visitado mais próximo de CIDADE 3 ---
      int u = -1;                                     // u é o nó que queremos encontrar
      int min = Integer.MAX_VALUE;                    // min é variável auxiliar

      for (int j = 0; j < n; j++) {                   // Para todos os nós da matriz de adjacências,
      
        // !visitado[j]  -> SE nó atual (j) não foi visitado, E...
        // dist[j] < min -> Nó atual (j) foi o mais próximo de CIDADE 3 encontrado até agora...
        if (!visitado[j] && dist[j] < min) {
          // ...Então definimos ele como o mais próximo encontrado até agora
          min = dist[j];                              // Guardamos sua distância em relação à CIDADE 3 para comparar com os próximos
          u = j;                                      // Ele fica selecionado como nó mais próximo encontrado até agora, por enquanto
        }
      }
        
      visitado[u] = true;                             // Visitamos o nó escolhido
        
      // --- Atualiza distâncias dos vizinhos do nó u ---
      for (int v = 0; v < n; v++) {
        // Se o vizinho do nó u (v) não foi visitado, E ...
        // a distância do nó u para o vizinho (v) não é 0 (ou seja, não estamos indo do nó u para ele mesmo), E ...
        // a distância do nó u em relação à CIDADE 3 somada à distância da aresta entre o nó u e v vai diminuir a distância entre o nó v e CIDADE 3?
        if (!visitado[v] && matriz_adj[u][v] != 0 && dist[u] + matriz_adj[u][v] < dist[v]) {
          // ...Se sim, então atualizamos a distância do nó v em relação à CIDADE 3 para a nova menor distância encontrada
          dist[v] = dist[u] + matriz_adj[u][v];       // Atualizamos a distância do vizinho
          pred[v] = u;                                // O predecessor desse vizinho v agora é o nó u
        }
      }
    }
    
    // ======================================================================= //
    //                                IMPRESSÃO                                //
    // ======================================================================= //
    System.out.println("\n=-=-= Dijkstra com origem na CIDADE 3 =-=-=\n");

    System.out.print("distâncias    = ");
    for (int i = 0; i < n; i++) {
      System.out.print(dist[i] + " ");
    }

    System.out.print("\npredecessores = ");
    for (int i = 0; i < n; i++) {
      System.out.print(pred[i] + " ");
    }
    
    System.out.print("\n\n");
  }
}