// ======================================================================= //
//                                 DUPLA                                   //
//                Bruna Gonçalves Corte David - 10425696                   //
//                Júlia Andrade - 10428513                                 //
// ======================================================================= //

package prim;

import java.util.ArrayList;
import java.util.List;

public class Prim {

    // ======================================================================= //
    //                                GLOBAIS                                  //
    // ======================================================================= //
    static int menorCustoEncontrado; // "valor"
    static int custoTotal;           // "custo"
    static boolean[] naArvore;       // "T"    -> (true = em T, false = em V-T)
    static List<int[]> arestas;      // "E'"   -> (Guarda {verticeInterno, verticeExterno, peso})
    static int verticeInterno;       // "vint" -> (k)
    static int verticeExterno;       // "vext" -> (i)
    static int qtdVerticesNaArvore;  // Tamanho do conjunto T
    
    static TGrafo grafoAtual;
    static int numVertices;

    public static void main(String[] args) {

        // ======================================================================= //
        //                 TESTE 1: GRAFO DO MATERIAL DA AULA                      //
        // ======================================================================= //
        TGrafo grafo1 = new TGrafo(9);
        
        grafo1.insereA(0, 1, 4);
        grafo1.insereA(1, 0, 4);

        grafo1.insereA(0, 5, 5);
        grafo1.insereA(5, 0, 5);

        grafo1.insereA(1, 5, 3);
        grafo1.insereA(5, 1, 3);

        grafo1.insereA(1, 2, 7);
        grafo1.insereA(2, 1, 7);

        grafo1.insereA(2, 3, 5);
        grafo1.insereA(3, 2, 5);

        grafo1.insereA(2, 6, 6);
        grafo1.insereA(6, 2, 6);

        grafo1.insereA(3, 4, 3);
        grafo1.insereA(4, 3, 3);

        grafo1.insereA(4, 6, 2);
        grafo1.insereA(6, 4, 2);

        grafo1.insereA(4, 7, 4);
        grafo1.insereA(7, 4, 4);

        grafo1.insereA(5, 6, 7);
        grafo1.insereA(6, 5, 7);

        grafo1.insereA(5, 8, 5);
        grafo1.insereA(8, 5, 5);

        grafo1.insereA(6, 7, 6);
        grafo1.insereA(7, 6, 6);

        grafo1.insereA(7, 8, 8);
        grafo1.insereA(8, 7, 8);

        System.out.println("=-=-= TESTE 1: Grafo do Material da Aula =-=-=");
        iniciarPrim(grafo1, 9, 0); 
        
        for (int[] aresta : arestas) {
            System.out.println((aresta[0] + 1) + ", " + (aresta[1] + 1) + ": custo " + aresta[2]);
        }
        System.out.println("Custo Total: " + custoTotal);


        // ======================================================================= //
        //          TESTE 2: GRAFO DA ATIVIDADE SOLICIDATA ANTERIORMENTE           //
        // ======================================================================= //
        TGrafo grafo2 = new TGrafo(6);
        
        grafo2.insereA(0, 1, 6);
        grafo2.insereA(1, 0, 6);

        grafo2.insereA(0, 2, 15);
        grafo2.insereA(2, 0, 15);

        grafo2.insereA(0, 4, 8);
        grafo2.insereA(4, 0, 8);

        grafo2.insereA(1, 2, 20);
        grafo2.insereA(2, 1, 20);

        grafo2.insereA(1, 4, 10);
        grafo2.insereA(4, 1, 10);

        grafo2.insereA(1, 5, 8); 
        grafo2.insereA(5, 1, 8);

        grafo2.insereA(2, 3, 9);
        grafo2.insereA(3, 2, 9);

        grafo2.insereA(3, 5, 7);
        grafo2.insereA(5, 3, 7);

        grafo2.insereA(5, 4, 5);
        grafo2.insereA(4, 5, 5);

        System.out.println("\n=-=-= TESTE 2: Grafo da Atividade Solicitada Anteriormente =-=-=");
        iniciarPrim(grafo2, 6, 0); 
        
        for (int[] aresta : arestas) {
            char u = (char) (aresta[0] + 'a');
            char v = (char) (aresta[1] + 'a');
            System.out.println(u + ", " + v + ": custo " + aresta[2]);
        }
        System.out.println("Custo Total: " + custoTotal);
    }

    // ======================================================================= //
    //                          INICIALIZAÇÃO DO PRIM                          //
    // ======================================================================= //
    public static void iniciarPrim(TGrafo grafo, int n, int origem) {
        grafoAtual = grafo;
        numVertices = n;
        
        menorCustoEncontrado = Integer.MAX_VALUE; // valor <- infinito;
        custoTotal = 0;                           // custo <- 0; 
        naArvore = new boolean[numVertices];      // T <- { origem }; (Será colocado true para os vértices que estiverem na árvore e false para os que não estiverem)
        naArvore[origem] = true;                  // Coloca o vértice de origem como true, ou seja, ele já está na árvore
        qtdVerticesNaArvore = 1;
        arestas = new ArrayList<>();              // E' <- vazio; (Lista para guardar as arestas da árvore geradora mínima para impressão depois)

        // PRIM(T);
        procedimentoPRIM();
    }

    // ======================================================================= //
    //                                  PRIM                                   //
    // ======================================================================= //
    public static void procedimentoPRIM() {
         for (int k = 0; k < numVertices; k++) {
            if (naArvore[k] == true) {                                 // para todo k pertecente a T faça
                
                TNo no = grafoAtual.adj[k];                            // Percorre os vértices adjacentes a k
                while (no != null) {                                   // Percorre a lista encadeada até acabar
                    int i = no.w;                                      // Pega o próximo vértice adjacente a k
                    
                    if (naArvore[i] == false) {                        // para todo i pertencente a V - T faça (para todo i que não esteja na árvore)
                        int custoArestaAtual = no.peso;                // vki - Custo da aresta entre k e i
                
                        if (custoArestaAtual < menorCustoEncontrado) { // se custoArestaAtual < valor então
                            menorCustoEncontrado = custoArestaAtual;   // valor <- custoArestaAtual;
                            verticeInterno = k;                        // vint <- k (guarda o vértice INTERNO para adicionar a aresta caso seja o menor custo encontrada no final do loop)
                            verticeExterno = i;                        // vext <- i (guarda o vértice EXTERNO para adicionar a aresta caso seja o menor custo encontrada no final do loop)
                        } 
                    }
                    no = no.prox;                                      // Percorre a lista encadeada
                } 
            }
        } 

        custoTotal = custoTotal + menorCustoEncontrado;                // custo <- custo + valor;
        naArvore[verticeExterno] = true;                               // T <- T U { vext }; (Adiciona o vértice externo à árvore)
        qtdVerticesNaArvore++;                                         // Aumenta +1 na quantidade de vértices na árvore

        arestas.add(new int[]{verticeInterno, verticeExterno, menorCustoEncontrado}); // E' <- E' U { (vext, vint) }; (Adiciona a aresta encontrada de menor custo à lista de arestas da árvore para impressão depois)
        
        menorCustoEncontrado = Integer.MAX_VALUE;                      // valor <- infinito; (Reseta o valor para infinito para a próxima iteração)
        
        if (qtdVerticesNaArvore != numVertices) {                      // se T != V então PRIM(T); (Se a quantidade de vértices na árvore for diferente da quantidade total de vértices, ela ainda não está completa, então continua)
            procedimentoPRIM();
        }
    } 
}