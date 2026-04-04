import java.io.File;
import java.util.Scanner;

// ======================================================================= //
//                             INICIALIZAÇÃO                               //
// ======================================================================= //

public class TGrafo {
    protected int n;             // Vértices
    protected int m;             // Arestas
    protected TNo[] adj;         // Array de listas de adjacência

    public TGrafo(int n) {       // Inicializa Array de Listas
        this.n = n;
        this.m = 0;              // Contador de arestas
        this.adj = new TNo[n];
        for (int i = 0; i < n; i++) {
            this.adj[i] = null;
        }
    }

    // Método de inserção no Grafo
    public void insereA(int u, int v) {
        TNo novo = new TNo(v);
        novo.prox = adj[u];
        adj[u] = novo;
        this.m++;
    }

    // Método de remoção do Grafo
    public void removeA(int u, int v) {
        TNo atual = adj[u];
        TNo ant = null;
        
        while (atual != null && atual.w != v) {
            ant = atual;
            atual = atual.prox;
        }
        
        if (atual != null) {
            if (ant == null) {
                adj[u] = atual.prox;
            } else {
                ant.prox = atual.prox;
            }
            this.m--;
        }
    }

    // Método de impressão do Grafo
    public void show() {
        System.out.println("n: " + n + " | m: " + m);
        for (int i = 0; i < n; i++) {
            System.out.print("Vértice " + i + ": ");
            TNo atual = adj[i];
            while (atual != null) {
                System.out.print(atual.w + " -> ");
                atual = atual.prox;
            }
            System.out.println("null");
        }
    }

    // ======================================================================= //
    //                               EXERCÍCIOS                                //
    // ======================================================================= //

    // ------------- Ex 18 -------------
    public int inDegree(int v) {
        int grau = 0;
        for (int i = 0; i < n; i++) {
            TNo atual = adj[i];
            while (atual != null) {
                if (atual.w == v) {
                    grau++;
                }
                atual = atual.prox;
            }
        }
        return grau;
    }

    // ------------- Ex 19 -------------
    public int outDegree(int v) {
        int grau = 0;
        TNo atual = adj[v];
        while (atual != null) {
            grau++;
            atual = atual.prox;
        }
        return grau;
    }

    // ------------- Ex 20 -------------
    public int degree(int v) {
        return inDegree(v) + outDegree(v);
    }

    // ------------- Ex 21 -------------
    public int isIgual(TGrafo outro) {
        if (this.n != outro.n || this.m != outro.m) {
            return 0;
        }
        
        for (int i = 0; i < n; i++) {
            int[] cont1 = new int[n];
            TNo atual = this.adj[i];
            while (atual != null) {
                cont1[atual.w]++;
                atual = atual.prox;
            }
            
            int[] cont2 = new int[n];
            atual = outro.adj[i];
            while (atual != null) {
                cont2[atual.w]++;
                atual = atual.prox;
            }
            
            for (int j = 0; j < n; j++) {
                if (cont1[j] != cont2[j]) {
                    return 0; // Falso
                }
            }
        }
        return 1; // Verdadeiro
    }

    // ------------- Ex 22 -------------
    public static TGrafo converterMatrizParaLista(int[][] matriz, int n) {
        TGrafo g = new TGrafo(n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (matriz[i][j] == 1) {
                    g.insereA(i, j);
                }
            }
        }
        return g;
    }

    // ------------- Ex 23 -------------
    public TGrafo inverter() {
        TGrafo inv = new TGrafo(this.n);
        for (int i = 0; i < n; i++) {
            TNo atual = adj[i];
            while (atual != null) {
                inv.insereA(atual.w, i);
                atual = atual.prox;
            }
        }
        return inv;
    }

    // ------------- Ex 24 -------------
    public int isFonte(int v) {
        if (outDegree(v) > 0 && inDegree(v) == 0) {
            return 1;
        }
        return 0;
    }

    // ------------- Ex 25 -------------
    public int isSorvedouro(int v) {
        if (inDegree(v) > 0 && outDegree(v) == 0) {
            return 1;
        }
        return 0;
    }

    // ------------- Ex 26 -------------
    public int isSimetrico() {
        for (int u = 0; u < n; u++) {
            TNo atual = adj[u];
            while (atual != null) {
                int v = atual.w;
                boolean achouVolta = false;
                TNo temp = adj[v];
                
                while (temp != null) {
                    if (temp.w == u) {
                        achouVolta = true;
                        break;
                    }
                    temp = temp.prox;
                }
                
                if (!achouVolta) {
                    return 0;
                }
                atual = atual.prox;
            }
        }
        return 1;
    }

    // ------------- Ex 27 -------------
    public static TGrafo lerArquivo(String caminho) {
        try {
            Scanner sc = new Scanner(new File(caminho));
            
            int vertices = sc.nextInt();
            int arestas = sc.nextInt();
            
            TGrafo g = new TGrafo(vertices);
            for (int i = 0; i < arestas; i++) {
                int u = sc.nextInt();
                int v = sc.nextInt();
                g.insereA(u, v);
            }
            
            sc.close();
            return g;
            
        } catch (Exception e) {
            System.out.println("Erro ao ler ficheiro: " + e.getMessage());
            return null;
        }
    }

    // ------------- Ex 28 & 29 -------------
    public void removeVertice(int v) {
        if (v < 0 || v >= n) {
            return;
        }
        
        TNo[] novaAdj = new TNo[n - 1];
        int novoM = 0;

        for (int i = 0; i < n; i++) {
            if (i == v) {
                continue;
            }
            int novaLinha = (i > v) ? i - 1 : i;
            
            TNo atual = adj[i];
            while (atual != null) {
                if (atual.w != v) {
                    int novaColuna = (atual.w > v) ? atual.w - 1 : atual.w;
                    
                    // Insere manualmente na nova lista
                    TNo novo = new TNo(novaColuna);
                    novo.prox = novaAdj[novaLinha];
                    novaAdj[novaLinha] = novo;
                    novoM++;
                }
                atual = atual.prox;
            }
        }
        this.adj = novaAdj;
        this.n--;
        this.m = novoM;
    }

    // ------------- Ex 30 -------------
    public int isCompleto() {
        for (int i = 0; i < n; i++) {
            boolean[] temAresta = new boolean[n];
            TNo atual = adj[i];
            
            while (atual != null) {
                temAresta[atual.w] = true;
                atual = atual.prox;
            }
            
            // Verifica se este vértice 'i' se liga a todos os outros 'j'
            for (int j = 0; j < n; j++) {
                if (i != j && !temAresta[j]) {
                    return 0; // 0 = Falso, não é completo
                }
            }
        }
        return 1; // 1 = Verdadeiro
    }

    // ======================================================================= //
    //                                  MAIN                                   //
    // ======================================================================= //
    public static void main(String[] args) {
        System.out.println("--- TESTANDO TODOS OS EXERCÍCIOS DA LISTA ---\n");

        System.out.println("=> Criando Grafo Direcionado G1");
        TGrafo g1 = new TGrafo(4);
        g1.insereA(0, 1);
        g1.insereA(0, 2);
        g1.insereA(1, 2);
        g1.insereA(2, 3);
        g1.show();

        System.out.println("\n--- Ex 18, 19 e 20: Graus ---");
        System.out.println("Grau de Entrada de 2: " + g1.inDegree(2));
        System.out.println("Grau de Saída de 0: " + g1.outDegree(0));
        System.out.println("Grau Total de 2: " + g1.degree(2));

        System.out.println("\n--- Ex 21: Igualdade de Grafos ---");
        TGrafo g2 = new TGrafo(4);
        g2.insereA(0, 1); g2.insereA(0, 2); g2.insereA(1, 2); g2.insereA(2, 3);
        System.out.println("\nGrafo G2:");
        g2.show();
        System.out.println("G1 é igual a G2? " + (g1.isIgual(g2) == 1 ? "Sim" : "Não"));

        System.out.println("\n--- Ex 22: Converter Matriz para Lista ---");
        int[][] matrizExemplo = {
            {0, 1, 0},
            {0, 0, 1},
            {1, 0, 0}
        };
        TGrafo gConvertido = TGrafo.converterMatrizParaLista(matrizExemplo, 3);
        System.out.println("Grafo gerado a partir de Matriz 3x3:");
        gConvertido.show();

        System.out.println("\n--- Ex 23: Inverter Grafo ---");
        System.out.println("Inverso do G1:");
        TGrafo inv = g1.inverter();
        inv.show();

        System.out.println("\n--- Ex 24 e 25: Fonte e Sorvedouro ---");
        System.out.println("O vértice 0 (G1) é fonte? " + (g1.isFonte(0) == 1 ? "Sim" : "Não"));
        System.out.println("O vértice 3 (G1) é sorvedouro? " + (g1.isSorvedouro(3) == 1 ? "Sim" : "Não"));

        System.out.println("\n--- Ex 26: Simétrico ---");
        System.out.println("G1 é simétrico? " + (g1.isSimetrico() == 1 ? "Sim" : "Não"));

        System.out.println("\n--- Ex 27: Ler Arquivo ---");
        String path = "C:/Users/bruna/OneDrive/Área de Trabalho/GitHub Repositories/university-projects/06-semester/graph_theory/03_exercises/adjacency_list/leitura_teste.txt";
        TGrafo gArq = TGrafo.lerArquivo(path);
        if (gArq != null) {
            System.out.println("Grafo lido do arquivo com sucesso:");
            gArq.show();
        } else {
            System.out.println("Não foi possível ler o grafo. Verifique se o arquivo e o caminho existem.");
        }

        System.out.println("\n--- Ex 28 e 29: Remover Vértice ---");
        System.out.println("Removendo vértice 2 de G1...");
        g1.removeVertice(2);
        g1.show();

        System.out.println("\n--- Ex 30: Grafo Completo ---");
        TGrafo gComp = new TGrafo(3);
        gComp.insereA(0,1); gComp.insereA(0,2);
        gComp.insereA(1,0); gComp.insereA(1,2);
        gComp.insereA(2,0); gComp.insereA(2,1);
        gComp.show();
        System.out.println("O Grafo acima é completo? " + (gComp.isCompleto() == 1 ? "Sim" : "Não"));
    }
}