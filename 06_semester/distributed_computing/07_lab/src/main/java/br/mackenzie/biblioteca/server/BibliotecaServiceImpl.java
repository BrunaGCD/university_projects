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

package br.mackenzie.biblioteca.server;

import br.mackenzie.biblioteca.*;
import io.grpc.Status;
import io.grpc.stub.StreamObserver;

import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;

public class BibliotecaServiceImpl extends BibliotecaServiceGrpc.BibliotecaServiceImplBase {

  // Memória
  private final Map<String, Livro> acervo = new ConcurrentHashMap<>();

  // ============================================================================
  //                              Cadastrar Livro
  // ============================================================================
  @Override
  public void cadastrarLivro(LivroRequest request, StreamObserver<LivroResponse> responseObserver) {
    System.out.println("Recebida requisição para cadastrar livro: " + request.getTitulo());

    if (acervo.containsKey(request.getIsbn())) {
      System.err.println("Falha: ISBN " + request.getIsbn() + " já cadastrado.");
      responseObserver.onError(Status.ALREADY_EXISTS.withDescription("Erro: Um livro com este ISBN já está cadastrado.").asRuntimeException());
      return;
    }

    String novoId = UUID.randomUUID().toString();
    Livro novoLivro = Livro.newBuilder().setId(novoId).setTitulo(request.getTitulo()).setAutor(request.getAutor()).setAno(request.getAno()).setIsbn(request.getIsbn()).build();

    acervo.put(novoLivro.getIsbn(), novoLivro);

    LivroResponse response = LivroResponse.newBuilder().setId(novoId).setStatus("Livro cadastrado com sucesso!").build();

    responseObserver.onNext(response);
    responseObserver.onCompleted();
  }

  // ============================================================================
  //                          Listar Livros por Autor
  // ============================================================================
  @Override
  public void listarLivrosPorAutor(AutorRequest request, StreamObserver<Livro> responseObserver) {
    System.out.println("Buscando livros do autor: " + request.getAutor());

    var livrosDoAutor = acervo.values().stream().filter(livro -> livro.getAutor().equalsIgnoreCase(request.getAutor())).collect(Collectors.toList());

    if (livrosDoAutor.isEmpty()) {
      System.err.println("Falha: Nenhum livro encontrado para o autor " + request.getAutor());
      responseObserver.onError(Status.NOT_FOUND.withDescription("Autor não encontrado.").asRuntimeException());
      return;
    }

    for (Livro livro : livrosDoAutor) {
      responseObserver.onNext(livro);
      try { Thread.sleep(300); } catch (InterruptedException e) {} 
    }

    responseObserver.onCompleted(); 
  }

  // ============================================================================
  //                       Registrar Múltiplos Empréstimos
  // ============================================================================
  @Override
  public StreamObserver<EmprestimoRequest> registrarEmprestimos(StreamObserver<ResumoEmprestimoResponse> responseObserver) {
    System.out.println("[Client Streaming] Abrindo canal para receber empréstimos...");
    final long startTime = System.currentTimeMillis();

    return new StreamObserver<EmprestimoRequest>() {
      int contador = 0;

      @Override
      public void onNext(EmprestimoRequest request) {
        System.out.println("Empréstimo registrado: Livro ID " + request.getLivroId() + " para " + request.getUsuario());
        contador++;
      }

      @Override
      public void onError(Throwable t) {
        System.err.println("Erro durante o stream: " + t.getMessage());
      }

      @Override
      public void onCompleted() {
        long duration = System.currentTimeMillis() - startTime;
        ResumoEmprestimoResponse response = ResumoEmprestimoResponse.newBuilder().setTotalEmprestimos(contador).setTempoProcessamento(duration + " ms").build();

        responseObserver.onNext(response);
        responseObserver.onCompleted();
        System.out.println("Resumo enviado.");
      }
    };
  }

  // ============================================================================
  //                         Chat Bibliotecário Dinâmico
  // ============================================================================
  @Override
  public StreamObserver<ChatMessage> chatBibliotecario(StreamObserver<ChatMessage> responseObserver) {
    System.out.println("Canal de chat aberto.");

    ChatMessage boasVindas = ChatMessage.newBuilder().setUsuario("Servidor").setMensagem("Bibliotecário Virtual: Olá! Digite uma palavra-chave (título, autor ou ano) para buscar!").build();
    responseObserver.onNext(boasVindas);

    return new StreamObserver<ChatMessage>() {
      @Override
      public void onNext(ChatMessage request) {
        System.out.println("Chat | " + request.getUsuario() + " busca por: " + request.getMensagem());
        String keyword = request.getMensagem().toLowerCase();

        List<Livro> achados = acervo.values().stream().filter(l -> l.getTitulo().toLowerCase().contains(keyword) || l.getAutor().toLowerCase().contains(keyword) || String.valueOf(l.getAno()).contains(keyword)).collect(Collectors.toList());

        StringBuilder respostaServidor = new StringBuilder();
        if (achados.isEmpty()) {
          respostaServidor.append("Bibliotecário Virtual: Não encontrei nenhum livro com o termo '").append(keyword).append("'.");
        } else {
          respostaServidor.append("Bibliotecário Virtual: Encontrei ").append(achados.size()).append(" livro(s): ");
          for (Livro l : achados) {
            respostaServidor.append("[").append(l.getTitulo()).append(" de ").append(l.getAutor()).append("] ");
          }
        }

        ChatMessage resposta = ChatMessage.newBuilder().setUsuario("Servidor").setMensagem(respostaServidor.toString()).build();

        responseObserver.onNext(resposta); 
      }

      @Override
      public void onError(Throwable t) {
        System.err.println("Erro no chat: " + t.getMessage());
      }

      @Override
      public void onCompleted() {
        System.out.println("O usuário encerrou o chat.");
        responseObserver.onCompleted();
      }
    };
  }
}