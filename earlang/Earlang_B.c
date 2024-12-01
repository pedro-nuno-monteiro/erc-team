#include <stdio.h>

// A formula de Earlang_B permite calcular a probabilidade de perda(bloqueio) num sistema M/M/n/0
// onde A = tráfego e n = nº de canais 
double erlangB(double A, unsigned int n) {
    double E = 1.0;  // Se n = 0, E(A, 0) = 1

    double AE = A * E;
    for (int i = 1; i <= n; i++) {
        E = AE / (AE + i);  // Earlang(A, i) - valor do bloqueio com i circuitos
    }

    return E;
}

int main() {
    double A;  // Intensidade de tráfego
    int n;     // Número de canais

    printf("Insira a intensidade de trafego: ");
    scanf("%lf", &A);
    
    printf("Insira o numero de canais: ");
    scanf("%d", &n);

    double prob = erlangB(A, n);
    printf("Probabilidade de bloqueio: %.6f\%\n", prob * 100);

    return 0;
}
