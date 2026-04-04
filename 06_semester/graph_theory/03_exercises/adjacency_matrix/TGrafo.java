import java.io.File;
import java.util.Scanner;

// ======================================================================= //
//                             INICIALIZAÇÃO                               //
// ======================================================================= //

public class TGrafo {
    protected int[][] adj;      // MATRIZ de Adjacências
    protected int n;            // Vértices
    protected int m;            // Arestas

    protected float[][] pesos;  // Ex 10: Suporte a pesos reais

    public TGrafo(int n) {      // Inicializa MATRIZ de adjacências
        this.n = n;
        this.m = 0;             // Contador de arestas
        this.adj = new int[n][n];
        
        // Ex 10: Inicializa pesos com Infinito (sem aresta)
        this.pesos = new float[n][n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                pesos[i][j] = Float.POSITIVE_INFINITY;
            }
        }
    }

    // Método de inserção no Grafo
    public void insereA(int u, int v) {
        if (this.adj[u][v] == 0) {
            this.adj[u][v] = 1;
            this.m++;
        }
    }

    // Método de remoção do Grafo
    public void removeA(int u, int v) {
        if (this.adj[u][v] == 1) {
            this.adj[u][v] = 0;
            this.pesos[u][v] = Float.POSITIVE_INFINITY;
            this.m--;
        }
    }

    // Método de impressão do Grafo
    public void show() {
        System.out.println("n: " + n + " | m: " + m);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                System.out.print(adj[i][j] + " ");
            }
            System.out.println();
        }
    }

    // ======================================================================= //
    //                               EXERCÍCIOS                                //
    // ======================================================================= //

    // ------------- Ex 1 -------------
    public int inDegree(int v) {
        int grau = 0;
        for (int i = 0; i < n; i++) {
            if (adj[i][v] == 1) { 
                grau++;
            }
        }
        return grau;
    }

    // ------------- Ex 2 -------------
    public int outDegree(int v) {
        int grau = 0;
        for (int i = 0; i < n; i++) {
            if (adj[v][i] == 1) {
                grau++;
            }
        }
        return grau;
    }

    // ------------- Ex 3 -------------
    public int degree(int v) {
        return inDegree(v) + outDegree(v);
    }

    // ------------- Ex 4 -------------
    public int isFonte(int v) {
        if ((inDegree(v) == 0) && (outDegree(v) > 0)) {
            return 1;
        }
        return 0;
    }

    // ------------- Ex 5 -------------
    public int isSorvedouro(int v) {
        if ((inDegree(v) > 0) && (outDegree(v) == 0)) {
            return 1;
        }
        return 0;
    }

    // ------------- Ex 6 -------------
    public int isSimetrico() {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (adj[i][j] != adj[j][i]) {
                    return 0;
                }
            }
        }
        return 1;
    }

    // ------------- Ex 7 -------------
    public static TGrafo lerArquivo(String path) {
        try {
            Scanner sc = new Scanner(new File(path));

            int vertices = sc.nextInt();              // Primeira linha é o num de vértices
            int arestas = sc.nextInt();               // Segunda linha é o num de arestas

            TGrafo g = new TGrafo(vertices);          // Próximas linhas são p/ preencher o Grafo
            for (int i = 0; i < arestas; i++) {
                int u = sc.nextInt();
                int v = sc.nextInt();
                g.insereA(u, v);
            }

            sc.close();
            return g;

        } catch (Exception e) {
            System.out.println("Erro ao ler arquivo: " + e.getMessage());
            return null;
        }
    }

    // ------------- Ex 8 -------------
    // Classe TGrafoND criada no arquivo TGrafoND.java

    // ------------- Ex 9 -------------
    // Método criado na classe TGrafoND no arquivo TGrafoND.java

    //  ------------- Ex 10 -------------
    public void insereA(int u, int v, float peso) { // Polimorfismo para inserir com peso
        insereA(u, v);
        this.pesos[u][v] = peso;
    }

    //  ------------- Ex 11 -------------
    public void removeVertice(int v) {
        if (v < 0 || v >= n) {                     // Valida se o vértice informado existe
            return;
        }
        
        int[][] novaAdj = new int[n - 1][n - 1];   // Cria nova MATRIZ de Adjacências com -1 vértice
        int novoM = 0;                             // Contaremos as arestas da nova MATRIZ de adj do zero

        int novaLinha = 0;
        for (int i = 0; i < n; i++) {
            if (i == v) {                          // Não existirá a linha do v na novaAdj
                continue;
            }
            int novaColuna = 0;
            for (int j = 0; j < n; j++) {
                if (j == v) {                      // Não existirá a coluna do v na novaAdj
                    continue;
                }

                novaAdj[novaLinha][novaColuna] = adj[i][j]; // Matemos a linha e coluna que não são do v na novaAdj
                if (adj[i][j] == 1) {
                    novoM++;
                }
                novaColuna++;
            }
            novaLinha++;
        }
        this.adj = novaAdj;                        // Atualiza a MATRIZ de adjacências
        this.n--;                                  // Agora com -1 vértice
        this.m = novoM;                            // Atualiza quantidade de arestas
    }

    //  ------------- Ex 12 -------------
    public int isCompletoND() {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i != j && adj[i][j] == 0) {
                    return 0;
                }
            }
        }
        return 1;
    }

    //  ------------- Ex 13 -------------
    public int isCompletoD() {
        return isCompletoND(); 
    }

    //  ------------- Ex 14 -------------
    public TGrafo complementar() {
        TGrafo comp = new TGrafo(this.n);          // Cria novo grafo complementar
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i != j && this.adj[i][j] == 0) {
                    comp.insereA(i, j);
                }
            }
        }
        return comp;
    }

    //  ------------- Método auxiliar p/ ex 15 & 16 -------------
    private void dfs(int v, boolean[] visitado, int[][] matriz) {
        visitado[v] = true;
        for (int i = 0; i < n; i++) {
            if (matriz[v][i] == 1 && !visitado[i]) {
                dfs(i, visitado, matriz);
            }
        }
    }
    //  ---------------------------------------------------------

    //  ------------- Ex 15 -------------
    public int tipoConexidadeND() {

        boolean[] visitado = new boolean[n];
        dfs(0, visitado, this.adj);

        for (boolean v : visitado) {
            if (!v) {
                return 0;
            }
        }

        return 1;
    }

    // ------------- Ex 16 -------------
    public int categoriaConexidadeD() {
        boolean[][] alcanca = new boolean[n][n];
        
        // Roda DFS de todos os nós para montar a matriz de alcançabilidade
        for (int i = 0; i < n; i++) {
            dfs(i, alcanca[i], this.adj);
        }

        // Testa C3: todos alcançam todos?
        boolean c3 = true;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (!alcanca[i][j]) {
                    c3 = false;
                }
            }
        }
        if (c3) return 3;

        // Testa C2: para todo par (i,j), i alcança j OU j alcança i?
        boolean c2 = true;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                // Se nenhum alcança o outro, falhou o unilateral
                if (!alcanca[i][j] && !alcanca[j][i]) {
                    c2 = false;
                }
            }
        }
        if (c2) return 2;

        // Testa C1 e C0 (Fraco ou Desconexo)
        int[][] adjND = new int[n][n];         // Transforma em ND
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (adj[i][j] == 1) { 
                    adjND[i][j] = 1; 
                    adjND[j][i] = 1; 
                }
            }
        }
        
        boolean[] visitadoND = new boolean[n]; // Verifica se é fraco ou deconexo
        dfs(0, visitadoND, adjND);
        
        for (boolean v : visitadoND) {
            if (!v) {
                return 0;                      // C0: Desconexo
            }
        }
        return 1;                              // C1: Fracamente conexo
    }

    // ------------- Ex 17 -------------
    public TGrafo grafoReduzido() {
        boolean[][] alcanca = new boolean[n][n];
        
        for (int i = 0; i < n; i++) {                          // Descobrir quem alcança quem usando a DFS
            dfs(i, alcanca[i], this.adj);
        }

        int[] grupo = new int[n];                              // Agrupar os vértices que se alcançam mutuamente (mesmo SCC)
        for (int i = 0; i < n; i++) {
            grupo[i] = -1;                                     // -1 significa que ainda não tem grupo
        }
        
        int numGrupos = 0;
        for (int i = 0; i < n; i++) {
            if (grupo[i] == -1) {
                grupo[i] = numGrupos;                          // Define o grupo do vértice atual
            
                for (int j = i + 1; j < n; j++) {              // Procura outros vértices que formam um ciclo com o atual
                    if (grupo[j] == -1 && alcanca[i][j] && alcanca[j][i]) {
                        grupo[j] = numGrupos;
                    }
                }
                numGrupos++;                                   // Prepara o ID para o próximo grupo isolado
            }
        }

        TGrafo reduzido = new TGrafo(numGrupos);              // O número de vértices do grafo reduzido é o número de grupos

        for (int i = 0; i < n; i++) {                         // Se existia aresta entre dois grupos diferentes, cria no reduzido
            for (int j = 0; j < n; j++) {
                if (this.adj[i][j] == 1) {
                    if (grupo[i] != grupo[j]) {
                        reduzido.insereA(grupo[i], grupo[j]); // O método insereA já ignora duplicadas, então não tem problema chamar várias vezes
                    }
                }
            }
        }

        return reduzido;
    }

    // ======================================================================= //
    //                                  MAIN                                   //
    // ======================================================================= //
    public static void main(String[] args) {
        System.out.println("--- TESTANDO TODOS OS EXERCÍCIOS DA MATRIZ ---\n");

        // Usando o grafo de exemplo que o professor deu no Ex 7 (6 vértices, 8 arestas)
        System.out.println("=> Criando Grafo Direcionado G1 (Exemplos 1 a 6)");
        TGrafo g1 = new TGrafo(6);
        g1.insereA(0, 1); g1.insereA(0, 5);
        g1.insereA(1, 0); g1.insereA(1, 5);
        g1.insereA(2, 4);
        g1.insereA(3, 1); g1.insereA(3, 5);
        g1.insereA(4, 3);
        g1.show();

        System.out.println("\n--- Ex 1, 2 e 3: Graus ---");
        System.out.println("Grau de Entrada de 1: " + g1.inDegree(1));
        System.out.println("Grau de Saída de 0: " + g1.outDegree(0));
        System.out.println("Grau Total de 1: " + g1.degree(1));

        System.out.println("\n--- Ex 4 e 5: Fonte e Sorvedouro ---");
        System.out.println("O vértice 2 é fonte? " + (g1.isFonte(2) == 1 ? "Sim" : "Não"));
        System.out.println("O vértice 5 é sorvedouro? " + (g1.isSorvedouro(5) == 1 ? "Sim" : "Não"));

        System.out.println("\n--- Ex 6: Simétrico ---");
        System.out.println("G1 é simétrico? " + (g1.isSimetrico() == 1 ? "Sim" : "Não"));

        System.out.println("\n--- Ex 7: Ler Arquivo ---");
        String path = "graph_theory/03_exercises/adjacency_matrix/leitura_teste.txt";
        TGrafo gArq = TGrafo.lerArquivo(path);
        if (gArq != null) {
            System.out.println("Grafo lido do arquivo com sucesso:");
            gArq.show();
        } else {
            System.out.println("Não foi possível ler o grafo. Verifique se o arquivo e o caminho existem.");
        }

        System.out.println("\n--- Ex 8 e 9: Grafo Não Direcionado (ND) ---");
        TGrafoND gND = new TGrafoND(4);
        gND.insereA(0, 1); gND.insereA(1, 2); gND.insereA(2, 3);
        gND.show();
        System.out.println("Grau Total de 1 no ND: " + gND.degree(1));

        System.out.println("\n--- Ex 10: Inserir com Peso ---");
        TGrafo gPeso = new TGrafo(3);
        gPeso.insereA(0, 1, 5.5f);
        System.out.println("Aresta 0->1 inserida com peso 5.5 (Operação executada com sucesso).");

        System.out.println("\n--- Ex 11: Remover Vértice ---");
        System.out.println("Removendo vértice 4 de G1...");
        g1.removeVertice(4);
        g1.show();

        System.out.println("\n--- Ex 12 e 13: Grafo Completo ---");
        TGrafo gComp = new TGrafo(3);
        gComp.insereA(0,1); gComp.insereA(0,2);
        gComp.insereA(1,0); gComp.insereA(1,2);
        gComp.insereA(2,0); gComp.insereA(2,1);
        gComp.show();
        System.out.println("O Grafo de 3 vértices é completo? " + (gComp.isCompletoD() == 1 ? "Sim" : "Não"));

        System.out.println("\n--- Ex 14: Grafo Complementar ---");
        TGrafoND gIncompleto = new TGrafoND(3);
        gIncompleto.insereA(0, 1);
        System.out.println("Grafo incompleto (faltam arestas para 2):");
        gIncompleto.show();
        TGrafo comp = gIncompleto.complementar();
        System.out.println("Matriz do Complementar (deve mostrar arestas para o 2):");
        comp.show();

        System.out.println("\n--- Ex 15: Conexidade ND ---");
        gND.show();
        System.out.println("Grafo acima é conexo? " + (gND.tipoConexidadeND() == 1 ? "Sim" : "Não"));

        System.out.println("\n--- Ex 16: Categoria Conexidade Direcionado ---");
        g1.show();
        System.out.println("Categoria de G1 (após remover v4): " + g1.categoriaConexidadeD() + " (0=Desconexo, 1=Fraco, 2=Uni, 3=Forte)");

        System.out.println("\n--- Ex 17: Grafo Reduzido ---");
        TGrafo reduzido = g1.grafoReduzido();
        System.out.println("Grafo Reduzido de G1 impresso:");
        reduzido.show();
    }
}