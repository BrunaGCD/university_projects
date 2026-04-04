  // ======================================================================= //
  //                                 DUPLA                                   //
  //                Bruna Gonçalves Corte David - 10425696                   //
  //                Júlia Andrade - 10428513                                 //
  // ======================================================================= //

import java.util.LinkedList;
import java.util.Queue;
import java.util.Scanner;
import java.util.Stack;

public class TGrafo {

  // ======================================================================= //
  //                          ALGORITMOS DFS E BFS                           //
  // ======================================================================= //

  static void DFS(int[][] matriz, int n, int inicio) {
    boolean[] visitado = new boolean[n];                // Inicializando lista de visitados
    Stack<Integer> pilha = new Stack<>();               // Inicializando pilha

    visitado[inicio] = true;                            // Vértice inicial automaticamente visitado
    System.out.print((char) ('a' + inicio) + " ");      // Converte int para char novamente e adiciona um espaço para impressão
    pilha.push(inicio);                                 // Vértice inicial é o primeiro na pilha

    while (!pilha.isEmpty()) {                          // Enquanto a pilha não estiver vazia,
      int atual = pilha.peek();                         // Olha para o vértice no topo da pilha sem removê-lo
      boolean achouVizinho = false;

      for (int i = 0; i < n; i++) {                     // Olha para todos os vizinhos do vértice atual
        if (matriz[atual][i] == 1 && !visitado[i]) {    // Se houver uma aresta do vértice atual ao vértice i que já não foi visitado,
          visitado[i] = true;                           // visita esse novo vértice encontrado i.
          System.out.print((char) ('a' + i) + " ");     // Converte int para char para impressão da letra/nome do vértice i
          pilha.push(i);                                // Insere vértice i na pilha para continuar percurso a partir dele
          achouVizinho = true;                          // Voltaremos nesse vértice atual depois
          break;
        }
      }

      if (!achouVizinho) {                              // Se nenhum vizinho mais for encontrado,
        pilha.pop();                                    // realiza backtracking
      }
    }
  }

  static void BFS(int[][] matriz, int n, int inicio) {
    boolean[] visitado = new boolean[n];                // Inicializando lista de visitados
    Queue<Integer> fila = new LinkedList<>();           // Inicializando fila

    visitado[inicio] = true;                            // Vértice inicial automaticamente visitado
    fila.add(inicio);                                   // Vértice inicial é o primeiro na fila

    while (!fila.isEmpty()) {                           // Enquanto a fila não estiver vazia,
      int atual = fila.poll();                          // Olha para o vértice no topo da pilha, removendo-o
      System.out.print((char) ('a' + atual) + " ");     // Converte int para char novamente e adiciona um espaço para impressão

      for (int i = 0; i < n; i++) {                     // Olha para todos os vizinhos do vértice atual
        if (matriz[atual][i] == 1 && !visitado[i]) {    // Se houver uma aresta do vértice atual ao vértice i que já não foi visitado,
          visitado[i] = true;                           // visita esse novo vértice encontrado i.
          fila.add(i);                                  // Insere vértice i na fila para continuar percurso nele depois
        }
      }
    }
  }

  // ======================================================================= //
  //                                  MAIN                                   //
  // ======================================================================= //

  public static void main(String[] args) {
    Scanner sc = new Scanner(System.in);

    System.out.print("Bem-vindo! Neste programa, você pode percorrer um grafo qualquer por DFS ou BFS\n");
    System.out.print("1 - DFS (Depth First Search)\n");
    System.out.print("2 - BFS (Breadth First Search)\n");
    System.out.print("Opção: ");
    int opc = sc.nextInt();


    System.out.print("\n\nO grafo é direcionado?\n");
    System.out.print("1 - Sim\n");
    System.out.print("2 - Não\n");
    System.out.print("Opção: ");
    int direcionado = sc.nextInt();


    System.out.print("\n\nNúmero de vértices do grafo: ");
    int n = sc.nextInt();


    System.out.println("\n\nDigite as arestas em pares \"u v\" sem as aspas para indicar u -> v");
    System.out.println("Quando terminar, digite \"fim\" sem aspas:\n\n");

    int[][] matriz = new int[n][n];          // Cria mtriz de adjacências p/ n vértices
    while (true) {

      String str_u = sc.next();              // Pegando vértice u em formato de str ou "fim"
      if (str_u.equals("fim")) {   // Conferindo se é "fim"
        break;                               // Se for "fim", encerra leitura
      }
      String str_v = sc.next();             // Se não for "fim", pega a vértice v também

      // Converte vértices em string (letras/nomes) para int para inserir na matriz
      int u = str_u.charAt(0) - 'a'; 
      int v = str_v.charAt(0) - 'a';

      matriz[u][v] = 1;                     // Cria uma aresta de u para v (u -> v)
      if (direcionado == 2) {               // Se for não-direcionado,
        matriz[v][u] = 1;                   // Torna a aresta bidirecional (u <-> v)
      }
    }

    System.out.print("\n\nDigite o vértice inicial (onde iniciaremos o percurso): ");
    String str_inicio = sc.next();
    int inicio = str_inicio.charAt(0) - 'a';

    System.out.print("\n\nIniciando percurso...\n");
    System.out.print("A ordem em que o grafo foi percorrido é:\n\n");
    if (opc == 1) {
      DFS(matriz, n, inicio);
    } else {
      BFS(matriz, n, inicio);
    }

    System.out.println();
    sc.close();
  }
}
