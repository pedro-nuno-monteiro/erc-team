#include <stdio.h>
#include <math.h>

double factorial(int n) {
    if (n == 0) return 1;

    double resultado = 1;
    for (int i = 1; i <= n; i++) {
        resultado *= i;
    }
    
    return resultado;
}

// A formula de Earlang_B permite calcular a probabilidade de perda(bloqueio) num sistema M/M/n/0
// onde A = tráfego e n = nº de canais 
double erlangB(double A, unsigned int n) {
    double E = 1.0;  // Se n = 0, E(A, 0) = 1
    double numerador = 0.0;
    
    for (int i = 1; i <= n; i++) {
        numerador = pow(A, i) / (factorial(i));
    }

    double I = numerador / (pow(A, n) / factorial(n));

    return 1 / E;
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
