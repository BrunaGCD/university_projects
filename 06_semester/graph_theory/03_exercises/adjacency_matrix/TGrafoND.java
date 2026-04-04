// ------------- Ex 8 -------------
class TGrafoND extends TGrafo {
  public TGrafoND(int n) {
    super(n);
  }

  @Override
  public void insereA(int u, int v) {
    super.insereA(u, v);
    super.insereA(v, u); // Garante a simetria por ser não-simétrico
  }

  @Override
  public void removeA(int u, int v) {
    super.removeA(u, v);
    super.removeA(v, u); // Garante a simetria por ser não-simétrico
  }

  @Override
  public void show() {
    System.out.println("--- Grafo Não-Direcionado ---");
    super.show();        // Chama o show original, mas com o título acima
  }

  // ------------- Ex 9 -------------
  @Override // TODO vai funcionar msm isso?
  public int degree(int v) {
    return super.outDegree(v);
  }
}


