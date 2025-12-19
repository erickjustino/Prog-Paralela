#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

const int NX = 256, NY = 256, NT = 10000;
const double DT = 0.01, NU = 1.00, DX = 1.0, DY = 1.0;
const int FREQUENCIA_SAIDA = 50;

// Para um teste de desempenho puro, mantenha 1.
#define DISABLE_FILE_OUTPUT 1 

// --- Funcoes Auxiliares ---
double** alocar_matriz(int linhas, int colunas) {
    double* dados = (double*)malloc(linhas * colunas * sizeof(double));
    double** matriz = (double**)malloc(linhas * sizeof(double*));
    for (int i = 0; i < linhas; i++) { matriz[i] = &(dados[i * colunas]); }
    return matriz;
}

void liberar_matriz(double** matriz) { 
    free(matriz[0]); 
    free(matriz); 
}

void salvar_campo(const char* nome, double** campo) {
    #if !DISABLE_FILE_OUTPUT
        // ... codigo para salvar arquivos ...
    #endif
}

int main() {
    double **u = alocar_matriz(NX, NY), **v = alocar_matriz(NX, NY);
    double **u_novo = alocar_matriz(NX, NY), **v_novo = alocar_matriz(NX, NY);

    #pragma omp parallel for
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            u[i][j] = 0.0; v[i][j] = 0.0;
            double dx_d = i - NX/2.0, dy_d = j - NY/2.0;
            double dist = sqrt(dx_d*dx_d + dy_d*dy_d);
            if (dist < 20.0) {
                u[i][j] = 2.0 * exp(-dist*dist / 50.0);
                v[i][j] = 1.5 * exp(-dist*dist / 50.0);
            }
        }
    }

    printf("Iniciando a simulacao com schedule(dynamic)...\n");
    double tempo_inicio = omp_get_wtime();

    for (int passo = 1; passo <= NT; passo++) {
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int i = 1; i < NX - 1; i++) {
            for (int j = 1; j < NY - 1; j++) {
                double d2u_dx2 = (u[i+1][j]-2*u[i][j]+u[i-1][j])/(DX*DX);
                double d2u_dy2 = (u[i][j+1]-2*u[i][j]+u[i][j-1])/(DY*DY);
                double d2v_dx2 = (v[i+1][j]-2*v[i][j]+v[i-1][j])/(DX*DX);
                double d2v_dy2 = (v[i][j+1]-2*v[i][j]+v[i][j-1])/(DY*DY);
                u_novo[i][j] = u[i][j] + DT*NU*(d2u_dx2 + d2u_dy2);
                v_novo[i][j] = v[i][j] + DT*NU*(d2v_dx2 + d2v_dy2);
            }
        }
        
        for (int i=0; i<NX; i++) { 
            u_novo[i][0]=u_novo[i][NY-2]; 
            u_novo[i][NY-1]=u_novo[i][1]; 
            v_novo[i][0]=v_novo[i][NY-2]; 
            v_novo[i][NY-1]=v_novo[i][1]; 
        }
        for (int j=0; j<NY; j++) { 
            u_novo[0][j]=u_novo[NX-2][j]; 
            u_novo[NX-1][j]=u_novo[1][j]; 
            v_novo[0][j]=v_novo[NX-2][j]; 
            v_novo[NX-1][j]=v_novo[1][j]; 
        }

        double **temp_u = u; u = u_novo; u_novo = temp_u;
        double **temp_v = v; v = v_novo; v_novo = temp_v;
    }

    double tempo_fim = omp_get_wtime();
    printf("Tempo de execucao: %.6f segundos\n", tempo_fim - tempo_inicio);

    liberar_matriz(u); 
    liberar_matriz(v); 
    liberar_matriz(u_novo); 
    liberar_matriz(v_novo);
    return 0;
}