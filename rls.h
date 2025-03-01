#define n 2

class RLSFilter {
private:
  //int n; // Ordem do filtro
  float lambda=0.99; // Fator de esquecimento
  

public:
  float w[n]; // Vetor de pesos
  float dw[n]; // Vetor de incrementos dos pesos
  float P[n][n]; // Matriz de inversão
  float P1[n][n]; // Matriz Auxiliar
  float res1[n];
  float res2=0.0;
  float res3[n];
  float P2;
  float p2_aux = 0;
  float delta;
  
  // Construtor
  RLSFilter(float forgettingFactor, float delta) {
    //n = order;
    lambda = forgettingFactor;
    delta = delta;

    for (int i = 0; i < n; i++) {
      w[i] = 0;
      dw[i] = 0;
      for (int j = 0; j < n; j++) {
        if (i == j){
          P[i][j] = 1.0 / delta;  // Diagonal principal
        }
        else{
          P[i][j] = 0.0;  // Restante dos elementos
        }
      }
    }
/*    
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        cout << P[i][j];
        cout << ",";
      }
      cout << "\n";
    }
*/    
    
      
  }
  
  // Filtra o vetor de entrada
  float filter(float *x) {
    float y = 0;
    for (int i = 0; i < n; i++) {
      y = w[i] * x[i];
    }
    return y;
  }

  // Atualiza os pesos do filtro
  void update(float desired, float *x) {
    
    float e = desired - filter(x); // Calcula o erro

/**
 * Primeira parte: Cálculo de P1
 * P1 é dado por:
 * P1 = P.x.x.P
 * Então faremos da seguinte maneira:
 * res1 = P.x
 * res2 = res1.x
 * P1 = res2.P
 * Voltando:
 * P1 = resl.x.P
 * P1 = P.x.x.P
 */

    //Primeira parte: Cálculo de P1
    //P1 = P.x.x.P
    
    // Multiplicação de P (nxn) por x (nx1), resultando em um vetor auxiliar res1(nx1)
    for (int i = 0; i < n; i++) {
      res1[i] = 0;
      for (int j = 0; j < n; j++) {
        res1[i] += P[i][j] * x[j];
      }
    }
    // Multiplicação de res1(nx1) por x (1xn), resultando em um float (res2)
    res2 = 0;
    for (int i = 0; i < n; i++) {
      res2 += res1[i] * x[i];
    }
    
    // Multiplicação do float (res2) pela matriz P
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        P1[i][j] = res2 * P[i][j];
      }
    }
  
//Segunda parte: Cálculo de P2
//P2 = lambda + x.P.x
    
    // Multiplicar o vetor x(1xn) por P(nxn), resultando em um vetor auxiliar res3(1xn)
    for (int i = 0; i < n; i++) {
      res3[i] = 0;
      for (int j = 0; j < n; j++) {
        res3[i] += x[j] * P[j][i];
      }
    }

    p2_aux = 0;
    for (int i = 0; i < n; i++) {
      p2_aux += res3[i] * x[i];
    }        

    P2 = lambda + p2_aux;

// Terceira parte: Calculo de P
// P = (1/lambda)*(P - P1/P2)
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        P[i][j] = (1.0 / lambda) * (P[i][j] - (P1[i][j] / P2));
      }
    }    

    // Multiplicação da matriz P pelo vetor x
    for (int i = 0; i < n; i++) {
      dw[i] = 0; 
      for (int j = 0; j < n; j++) {
        dw[i] += P[i][j] * x[j] * e;
      }
    }

    for (int i = 0; i < n; i++){
      w[i] = w[i] + dw[i];
    }

  }
};
