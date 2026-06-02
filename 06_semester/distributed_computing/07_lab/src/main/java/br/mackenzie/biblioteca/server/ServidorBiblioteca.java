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

import io.grpc.Server;
import io.grpc.ServerBuilder;
import java.io.IOException;

public class ServidorBiblioteca {

  public static void main(String[] args) throws IOException, InterruptedException {
    int porta = 50051;
    
    Server server = ServerBuilder.forPort(porta).addService(new BibliotecaServiceImpl()).build().start();

    System.out.println("================================================");
    System.out.println("Servidor da Biblioteca escutando na porta: " + porta);
    System.out.println("================================================");

    Runtime.getRuntime().addShutdownHook(new Thread(() -> {
      System.out.println("Encerrando servidor...");
      server.shutdown();
    }));

    server.awaitTermination();
  }
}