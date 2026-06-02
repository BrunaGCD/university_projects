/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
|         ~~ DUPLA ~~         |
|                             |
| Bruna Gonçalves Corte David |
| RA: 10425696                |  Atividade Avaliativa - Lab 08 - gRPC com Java
|                             |
| Júlia Andrade               |
| RA: 10428513                |
|                             |
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

package br.mackenzie.biblioteca.client;

import br.mackenzie.biblioteca.*;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;
import io.grpc.stub.StreamObserver;

import java.util.Iterator;
import java.util.Scanner;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

public class ClienteBiblioteca {

  private static final Scanner scanner = new Scanner(System.in);

  public static void main(String[] args) throws InterruptedException {

    ManagedChannel channel = ManagedChannelBuilder.forAddress("localhost", 50051).usePlaintext().build();

    BibliotecaServiceGrpc.BibliotecaServiceBlockingStub blockingStub = BibliotecaServiceGrpc.newBlockingStub(channel);
    BibliotecaServiceGrpc.BibliotecaServiceStub asyncStub = BibliotecaServiceGrpc.newStub(channel);

    boolean executando = true;

    while (executando) {
      System.out.println("\n==============================================");
      System.out.println("SISTEMA DE GERENCIAMENTO DE BIBLIOTECA DIGITAL");
      System.out.println("==============================================");
      System.out.println("1. Cadastrar Livro");
      System.out.println("2. Buscar Livros por Autor");
      System.out.println("3. Registrar Empréstimos em Lote");
      System.out.println("4. Falar com o Bibliotecário");
      System.out.println("5. Sair");
      System.out.print("-> Escolha uma opção: ");
      
      String opcao = scanner.nextLine();

      switch (opcao) {
        case "1":
          cadastrarLivroInterativo(blockingStub);
          break;
        case "2":
          listarPorAutorInterativo(blockingStub);
          break;
        case "3":
          registrarEmprestimosInterativo(asyncStub);
          break;
        case "4":
          iniciarChatInterativo(asyncStub);
          break;
        case "5":
          executando = false;
          break;
        default:
          System.out.println("Opção inválida!");
      }
    }

    System.out.println("Encerrando cliente...");
    channel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
  }

  // ============================================================================
  //                              Cadastrar Livro
  // ============================================================================
  private static void cadastrarLivroInterativo(BibliotecaServiceGrpc.BibliotecaServiceBlockingStub stub) {
    System.out.println("\n--- CADASTRAR NOVO LIVRO ---");
    System.out.print("Título: ");
    String titulo = scanner.nextLine();
    System.out.print("Autor: ");
    String autor = scanner.nextLine();
    System.out.print("Ano: ");
    int ano = Integer.parseInt(scanner.nextLine());
    System.out.print("ISBN: ");
    String isbn = scanner.nextLine();

    try {
      LivroRequest request = LivroRequest.newBuilder().setTitulo(titulo).setAutor(autor).setAno(ano).setIsbn(isbn).build();
      
      LivroResponse response = stub.cadastrarLivro(request);
      System.out.println("\nSucesso: " + response.getStatus() + " (ID: " + response.getId() + ")");
    } catch (StatusRuntimeException e) {
      System.err.println("\nErro ao cadastrar: " + e.getStatus().getDescription());
    }
  }

  // ============================================================================
  //                          Listar Livros por Autor
  // ============================================================================
  private static void listarPorAutorInterativo(BibliotecaServiceGrpc.BibliotecaServiceBlockingStub stub) {
    System.out.println("\n--- BUSCAR POR AUTOR ---");
    System.out.print("Digite o nome do autor: ");
    String autor = scanner.nextLine();

    try {
      AutorRequest request = AutorRequest.newBuilder().setAutor(autor).build();
      Iterator<Livro> respostaStream = stub.listarLivrosPorAutor(request);

      System.out.println("\nResultados encontrados:");
      while (respostaStream.hasNext()) {
        Livro livro = respostaStream.next();
        System.out.println(" - " + livro.getTitulo() + " (" + livro.getAno() + ") [ISBN: " + livro.getIsbn() + "]");
      }
    } catch (StatusRuntimeException e) {
      System.err.println("\nErro na busca: " + e.getStatus().getDescription());
    }
  }

  // ============================================================================
  //                       Registrar Múltiplos Empréstimos
  // ============================================================================
  private static void registrarEmprestimosInterativo(BibliotecaServiceGrpc.BibliotecaServiceStub asyncStub) throws InterruptedException {
    System.out.println("\n--- REGISTRO DE EMPRÉSTIMOS (LOTE) ---");
    System.out.print("Quantos livros deseja pegar emprestado? ");
    int qtd = Integer.parseInt(scanner.nextLine());
    
    System.out.print("Seu nome de usuário: ");
    String usuario = scanner.nextLine();

    CountDownLatch latch = new CountDownLatch(1);

    StreamObserver<ResumoEmprestimoResponse> responseObserver = new StreamObserver<ResumoEmprestimoResponse>() {
      @Override
      public void onNext(ResumoEmprestimoResponse resumo) {
        System.out.println("\nRESUMO DO SERVIDOR: " + resumo.getTotalEmprestimos() + " processados em " + resumo.getTempoProcessamento());
      }
      @Override
      public void onError(Throwable t) {
        System.err.println("Erro: " + t.getMessage());
        latch.countDown();
      }
      @Override
      public void onCompleted() {
        latch.countDown();
      }
    };

    StreamObserver<EmprestimoRequest> requestObserver = asyncStub.registrarEmprestimos(responseObserver);

    for (int i = 0; i < qtd; i++) {
      System.out.print("Digite o ID ou ISBN do Livro " + (i + 1) + ": ");
      String livroId = scanner.nextLine();
      requestObserver.onNext(EmprestimoRequest.newBuilder().setUsuario(usuario).setLivroId(livroId).build());
    }
    
    requestObserver.onCompleted(); 
    System.out.println("Enviando lote ao servidor... aguardando resposta.");
    latch.await(10, TimeUnit.SECONDS); 
  }

  // ============================================================================
  //                         Chat Bibliotecário Dinâmico
  // ============================================================================
  private static void iniciarChatInterativo(BibliotecaServiceGrpc.BibliotecaServiceStub asyncStub) throws InterruptedException {
    System.out.println("\n--- CHAT COM BIBLIOTECÁRIO ---");
    System.out.print("Seu nome: ");
    String usuario = scanner.nextLine();
    System.out.println("(Digite 'sair' para encerrar o chat a qualquer momento)\n");

    CountDownLatch latch = new CountDownLatch(1);

    StreamObserver<ChatMessage> responseObserver = new StreamObserver<ChatMessage>() {
      @Override
      public void onNext(ChatMessage msg) {
        System.out.println(msg.getUsuario() + ": " + msg.getMensagem());
      }
      @Override
      public void onError(Throwable t) {
        System.err.println("Erro: " + t.getMessage());
        latch.countDown();
      }
      @Override
      public void onCompleted() {
        latch.countDown();
      }
    };

    StreamObserver<ChatMessage> requestObserver = asyncStub.chatBibliotecario(responseObserver);

    while (true) {
      String texto = scanner.nextLine();

      if (texto.equalsIgnoreCase("sair")) {
        requestObserver.onCompleted();
        break;
      }

      requestObserver.onNext(ChatMessage.newBuilder().setUsuario(usuario).setMensagem(texto).build());
      
      Thread.sleep(200); 
    }

    latch.await(10, TimeUnit.SECONDS);
  }
}