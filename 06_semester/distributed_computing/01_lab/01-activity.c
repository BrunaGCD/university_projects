/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
|         ~~ DUPLA ~~         |
|                             |
| Bruna Gonçalves Corte David |
| RA: 10425696                |  Atividade - Lab 01 - Mini Sistema de Cadastro de Produtos
|                             |
| Júlia Andrade               |
| RA: 10428513                |
|                             |
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/


/* ================== INICIALIZAÇÃO ================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A struct abaixo representa as características de 1 produto
// Definição da struct
typedef struct {
    int codigo;
    char *nome;
    float preco;
    int quantidade;
} Produto;

/* ===================== FUNÇÕES ===================== */

void adicionar_produto(Produto **produtos, int *total, int *prox_codigo) {
    // Os produtos vão ser alocados de forma dinâmica com esses comandos:
    // malloc -> Memory Allocation -> Reserva um NOVO bloco de memória
    // realloc -> Re-Allocation -> REDIMENSIONA um bloco que já tinha

    printf("\n--- Adicionar Produto ---\n");


    // =-=-=-=-=-=-= Redimensiona Vetor Dinamicamente p/ Adicionar Novo Produto =-=-=-=-=-=-=

    // Realoca o vetor de produtos para ADICIONAR MAIS 1 produto
    *produtos = (Produto *) realloc(*produtos, (*total + 1) * sizeof(Produto));
    // 1. sizeof(Produto) -> Mede quantos bytes a struct Produto tem
    // 2. (*total + 1) -> Diz para redimensionar p/ o mesmo tamanho (*total) de antes + 1 produto
    // 3. realloc(*produtos, ... -> Realiza o realloc no vetor de produtos
    // 4. (Produto *) -> O realloc retorna um ponteiro genérico, então isso é necessário para
    //    o ponteiro ser considerado do tipo Produto
    
    if (*produtos == NULL) {
        printf("Erro na alocação de memória!\n");
        exit(1);
    }

 
    // =-=-=-=-=-=-=-=-=-=-= Cria Novo Produto e suas Características =-=-=-=-=-=-=-=-=-=-=-=

    // Pega o endereço do novo produto que acabou de ser criado no final do vetor
    Produto *novo = &(*produtos)[*total];
    
    // Define o código único dele
    (*novo).codigo = *prox_codigo;

    // Usa buffer temporário para ler strings
    char buffer[100];
    printf("Nome: ");
    scanf(" %[^\n]", buffer); // Lê até usuário digitar enter (^\n)

    // Aloca memória exatamente para o tamanho do nome lido (+1 para o '\0')
    // strlen() conta as letras, mas não conta o \0 final necessário para strings,
    // logo, o +1 é necessário para dar vaga à esse caractere final.
    (*novo).nome = (char *) malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy((*novo).nome, buffer);

    printf("Preço: ");
    scanf("%f", &(*novo).preco);

    printf("Quantidade: ");
    scanf("%d", &(*novo).quantidade);

    printf("Produto adicionado com código %d!\n", (*novo).codigo);

    // Se todo o processo funcionou sem falhar até aqui, posso atualizar essas variáveis
    (*total)++;
    (*prox_codigo)++;
}

void listar_produtos(Produto *produtos, int total) {
    printf("\n--- Lista de Produtos ---\n");
    
    if (total == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    printf("+--------+------------------+----------+------+---------------+\n");
    printf("| Código | Nome             | Preço    | Qtd  | Valor Estoque |\n");
    printf("+--------+------------------+----------+------+---------------+\n");

    float valor_total_estoque = 0;

    for (int i = 0; i < total; i++) {
        float valor_estoque = produtos[i].preco * produtos[i].quantidade;
        valor_total_estoque += valor_estoque;
        
        // %-16s alinha a string à esquerda com 16 espaços (IA ajudou a deixar visualmente bonitinho)
        printf("| %6d | %-16s | %8.2f | %4d | %13.2f |\n",
               produtos[i].codigo, produtos[i].nome, produtos[i].preco,
               produtos[i].quantidade, valor_estoque);
    }
    
    printf("+--------+------------------+----------+------+---------------+\n");
    printf("Valor total do estoque: R$ %.2f\n", valor_total_estoque);
}

Produto* buscar_produto(Produto *produtos, int total, int codigo) {
    // Percorre os produtos para encontrar aquele com o mesmo código passado
    for (int i = 0; i < total; i++) {
        if (produtos[i].codigo == codigo) {
            return &produtos[i]; // Retorna o endereço do produto encontrado
        }
    }
    return NULL; // Se não encontrar, retorna NULL
}

void passagem_referencia(int *qtd_atual, int nova_qtd) {
    *qtd_atual = nova_qtd;
}

void atualizar_estoque(Produto *produtos, int total) {
    printf("\n--- Atualizar Estoque ---\n");

    printf("Código do produto: ");
    int codigo;
    scanf("%d", &codigo);

    Produto *produto = buscar_produto(produtos, total, codigo);

    if (produto != NULL) { // Prossegue se o produto existir
        printf("Nova quantidade: ");
        int nova_qtd;
        scanf("%d", &nova_qtd);
        
        // Função auxiliar atualiza estoque com passagem por referência
        passagem_referencia(&((*produto).quantidade), nova_qtd);
        
        printf("Estoque atualizado com sucesso!\n");
    } 
    else {
        printf("Produto não encontrado!\n");
    }
}

void remover_produto(Produto **produtos, int *total) {
    printf("\n--- Remover Produto ---\n");

    printf("Código do produto: ");
    int codigo;
    scanf("%d", &codigo);

    // Encontra o produto
    Produto *produto = buscar_produto(*produtos, *total, codigo);

    if (produto == NULL) {
        printf("Produto não encontrado!\n");
        return;
    }

    // Aritmética de ponteiros: (endereço do item) - (endereço base do vetor) = índice
    // Ao subtrair dois ponteiros do mesmo tipo, a diferença em bytes é automaticamente
    // dividida pelo tamanho da struct (Produto), resultando no índice
    int indice = produto - *produtos;

    // Liberar a CONTEÚDO do nome PRIMEIRO
    printf("Produto \"%s\" removido com sucesso!\n", (*produtos)[indice].nome);
    free((*produtos)[indice].nome);

    // Reorganizar o vetor puxando os elementos na direita para a esquerda
    // Começa no indice atual e vai até o penúltimo produto, para evitar de sem querer
    // acessar uma posição da memória não permitida
    for (int i = indice; i < *total - 1; i++) {
        (*produtos)[i] = (*produtos)[i + 1];
    }

    // Remover 1 da contagem de produtos no vetor
    (*total)--;

    // Realocar o vetor para o novo tamanho menor, já que agora há um espaço vazio
    // no final dele ocupando memória a toa
    if (*total > 0) {
        *produtos = (Produto *) realloc(*produtos, (*total) * sizeof(Produto)); // Lembrando que o total já é -1 em relação à antes
    }
    else {
        // Se o total é zero, liberamos o vetor principal
        free(*produtos);
        *produtos = NULL;
    }
}

void liberar_memoria(Produto *produtos, int total) {
    printf("\nLiberando memória...\n");
    
    // É necessário liberar a memória dos CONTEÚDOS apontados pelos ponteiros dos nomes
    // primeiro, pois não seríamos capazes de encontrar esses conteúdos se liberássemos
    // o ponteiro pro vetor de produtos primeiro.
    for (int i = 0; i < total; i++) {
        printf("Memória do produto \"%s\" liberada.\n", produtos[i].nome); // Aproveitamos para printar que o liberamos
        free(produtos[i].nome);
    }
    
    // Depois que terminamos de liberar o CONTEÚDO dos nomes, podemos liberar o restante do CONTEÚDO
    // apontado pelo ponteiro pro vetor produtos, que inclui os PONTEIROS para os nomes em si,
    // logo eles serão automaticamente apagados aqui junto.
    free(produtos);
    printf("Vetor de produtos liberado.\n");
}

int main() {    
    Produto *produtos = NULL; // Esse será o vetor de produtos que será alocado dinamicamente, inicializado como um ponteiro apontando para nada (NULL)
    int total = 0;            // Total de produtos no estoque
    int prox_codigo = 1;      // P/ Gerar o próximo código único do próx produto corretamente
    
    int opc;
    while (opc != 6) {
        printf("\n");
        printf("========================================\n");
        printf("    SISTEMA DE CADASTRO DE PRODUTOS     \n");
        printf("========================================\n");
        printf("Menu:\n");
        printf("1. Adicionar produto\n");
        printf("2. Listar produtos\n");
        printf("3. Buscar produto\n");
        printf("4. Atualizar estoque\n");
        printf("5. Remover produto\n");
        printf("6. Sair\n\n");
        printf("Opção: ");

        scanf("%d", &opc);

        switch (opc) {

            case 1:
                adicionar_produto(&produtos, &total, &prox_codigo);
                break;

            case 2:
                listar_produtos(produtos, total);
                break;

            case 3:
                printf("\n--- Buscar Produto ---\n");
                printf("Código do produto: ");

                int cod_busca;
                scanf("%d", &cod_busca);

                // Retorna ponteiro apontando para o produto de ID correspondente ou NULL se não tiver o encontrado
                Produto *encontrado = buscar_produto(produtos, total, cod_busca);
                if (encontrado != NULL) { // Se tiver sido encontrado, o conteúdo apontado pelo ponteiro não é NULL
                    printf("Produto: %s | Preço: %.2f | Qtd: %d\n", (*encontrado).nome, (*encontrado).preco, (*encontrado).quantidade);
                } else {                  // É NULL, logo não foi encontrado
                    printf("Produto não encontrado.\n");
                }
                break;

            case 4:
                atualizar_estoque(produtos, total);
                break; 

            case 5:
                remover_produto(&produtos, &total);
                break;

            case 6:
                liberar_memoria(produtos, total); // É necessário liberar TODO o CONTEÚDO ""dentro"" do ponteiro para produtos
                produtos = NULL;                  // Agora podemos "apagar" o PONTEIRO  pro vetor de produtos em si
                printf("Programa encerrado.\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    }


    return 0;
}