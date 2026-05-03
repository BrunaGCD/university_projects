// ======================================================================= //
//                                 DUPLA                                   //
//                Bruna Gonçalves Corte David - 10425696                   //
//                Júlia Andrade - 10428513                                 //
// ======================================================================= //

public class Sequential_Coloration {

    public static void main(String[] args) {

        // ======================================================================= //
        //        TESTE 1: GRAFO DO PROBLEMA DOS EXAMES - MATERIAL DE AULA         //
        // ======================================================================= //
        TGrafo grafo1 = new TGrafo(5);
        
        grafo1.insereA(0, 1);
        grafo1.insereA(1, 0);
        
        grafo1.insereA(0, 4);
        grafo1.insereA(4, 0);

        grafo1.insereA(1, 2);
        grafo1.insereA(2, 1);

        grafo1.insereA(1, 3);
        grafo1.insereA(3, 1);

        grafo1.insereA(2, 3);
        grafo1.insereA(3, 2);

        grafo1.insereA(3, 4);
        grafo1.insereA(4, 3);
        
        // ------------------------- TESTE COM ROTULAÇÃO INICIAL -------------------------
        System.out.println("=-=-= TESTE 1A: Grafo do Problema dos Exames - Material de Aula (Rotulação Inicial) =-=-=");
        int[] rotulacao_inicial1 = {0, 1, 2, 3, 4};
        coloracao_sequencial(grafo1, 5, rotulacao_inicial1, false);

        // --------------------------- TESTE COM ROTULAÇÃO NOVA --------------------------
        System.out.println("\n=-=-= TESTE 1B: Grafo do Problema dos Exames - Material de Aula (Rotulação Nova) =-=-=");
        int[] rotulacao_nova1 = {1, 3, 0, 2, 4};                       // ROTULANDO GRAFO EM ORDEM NÃO CRESCENTE DOS GRAUS DOS VÉRTICES
        coloracao_sequencial(grafo1, 5, rotulacao_nova1, false);

        // ======================================================================= //
        //           TESTE 2: GRAFO DA ATIVIDADE SOLICITADA ANTERIORMENTE          //
        // ======================================================================= //
        TGrafo grafo2 = new TGrafo(9);

        grafo2.insereA(0, 5);
        grafo2.insereA(5, 0);

        grafo2.insereA(0, 6);
        grafo2.insereA(6, 0);

        grafo2.insereA(0, 8);
        grafo2.insereA(8, 0);

        grafo2.insereA(1, 2);
        grafo2.insereA(2, 1);

        grafo2.insereA(1, 7);
        grafo2.insereA(7, 1);

        grafo2.insereA(2, 4);
        grafo2.insereA(4, 2);

        grafo2.insereA(2, 7);
        grafo2.insereA(7, 2);

        grafo2.insereA(3, 4);
        grafo2.insereA(4, 3);

        grafo2.insereA(3, 5);
        grafo2.insereA(5, 3);

        grafo2.insereA(3, 7);
        grafo2.insereA(7, 3);

        grafo2.insereA(4, 6);
        grafo2.insereA(6, 4);

        grafo2.insereA(5, 6);
        grafo2.insereA(6, 5);

        grafo2.insereA(5, 8);
        grafo2.insereA(8, 5);

        grafo2.insereA(6, 7);
        grafo2.insereA(7, 6);

        grafo2.insereA(6, 8);
        grafo2.insereA(8, 6);

      // ------------------------- TESTE COM ROTULAÇÃO INICIAL -------------------------
       System.out.println("\n=-=-= TESTE 2A: Grafo da Atividade Anteriormente (Rotulação Inicial) =-=-=");
       int[] rotulacao_inicial2 = {0, 1, 2, 3, 4, 5, 6, 7, 8};
       coloracao_sequencial(grafo2, 9, rotulacao_inicial2, true);
       
        // --------------------------- TESTE COM ROTULAÇÃO NOVA --------------------------
       System.out.println("\n=-=-= TESTE 2B: Grafo da Atividade Anteriormente (Rotulação Nova) =-=-=");
       int[] rotulacao_nova2 = {6, 5, 7, 0, 2, 3, 4, 8, 1};            // ROTULANDO GRAFO EM ORDEM NÃO CRESCENTE DOS GRAUS DOS VÉRTICES
       coloracao_sequencial(grafo2, 9, rotulacao_nova2, true);
    }

    public static void inserirAresta(TGrafo g, int u, int v) {
        g.insereA(u, v);
        g.insereA(v, u);
    }

    // ======================================================================= //
    //                      ALGORITMO COLORAÇÃO SEQUENCIAL                     //
    // ======================================================================= //
    public static void coloracao_sequencial(TGrafo grafo, int n, int[] ordemDeVisita, boolean imprime_com_letra) {
        
        int[] cores = new int[n];                  // Inicializa vetor de classes de cores (0 significa que o vértice ainda não tem cor)
        int maxCor = 0;                            // Inicializa Total de cores usadas

        for (int p = 0; p < n; p++) {              // Percorre cada vértice para descobrir sua cor um de cada vez
            int i = ordemDeVisita[p];              // ordemDeVisita é um vetor AUXILIAR pois estou fazendo dois testes por teste, em duas ordens de rotulação
            int corAtual = 1;                      // A cor atual a ser verificada sempre começa como a primeira (1)
          
            while (true) {                         // Procura qual cor esse vértice pode ser colorido
                boolean flag = true;               // Flag para confirmar se a cor atual pode ou não ser usada para colorir o vértice atual
                TNo no = grafo.adj[i];             // Olha para os vizinhos do vértice atual (i)
                while (no != null) {               // Verifica se algum vizinho de i já está colorido com a cor atual
                    if (cores[no.w] == corAtual) { // Se a cor do vizinho atual for igual a cor atual,
                        flag = false;              // Então não podemos utilizar essa cor para colorir o vértice atual (i)
                        break;                     // (Não precisamos olhar os outros vizinhos mais)
                    }
                    no = no.prox;                  // Prossegue a verificação para o próximo vizinho
                }
                if (flag) {                        // Se nenhum vizinho possuía a mesma cor que a atual,
                    cores[i] = corAtual;           // Então colorimos o vértice atual (i) com a cor atual
                    if (corAtual > maxCor) {       // Se a cor atual utilizada foi uma nova que não existia ainda (ou seja, valor maior encontrado até agora)
                      maxCor = corAtual;           // Então aumentamos o "contador" do total de cores usadas
                    }
                    break;                         // Encerra loop (já encontramos a cor pra esse vértice atual)
                } else {                           // Se um vizinho possuía a mesma cor que a atual,
                    corAtual++;                    // Não podemos colorir o vértice atual com ela, então tentamos novamente com uma próxima cor
                }
            }
        }
        
        // --------------------------- IMPRESSÃO --------------------------
        for (int i = 0; i < n; i++) {
          String impressao_vertice = imprime_com_letra ? String.valueOf((char) (i + 'A')) : String.valueOf(i + 1);
            System.out.println(impressao_vertice + ": Cor " + cores[i]);
        }
        System.out.println("Total de Cores: " + maxCor);
  
    }
}