#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// --- Parâmetros da Simulação ---
const int NX = 256;
const int NY = 256;
const int NT = 10000;
const double DT = 0.01;
const double NU = 1.00;
const double DX = 1.0;
const double DY = 1.0;

#define DISABLE_FILE_OUTPUT 1 
const int FREQUENCIA_SAIDA = 50;

// --- Funções Auxiliares ---
double** alocar_matriz(int linhas, int colunas) {
    double* dados = (double*)malloc(linhas * colunas * sizeof(double));
    double** matriz = (double**)malloc(linhas * sizeof(double*));
    for (int i = 0; i < linhas; i++) {
        matriz[i] = &(dados[i * colunas]);
    }
    return matriz;
}

void liberar_matriz(double** matriz) {
    free(matriz[0]);
    free(matriz);
}

void salvar_campo(const char* nome_arquivo, double** campo) {
    #if !DISABLE_FILE_OUTPUT
        FILE *arquivo = fopen(nome_arquivo, "w");
        if (!arquivo) {
            perror("Não foi possível abrir o arquivo");
            return;
        }
        fprintf(arquivo, "X Y Valor\n");
        for (int i = 0; i < NX; i++) {
            for (int j = 0; j < NY; j++) {
                fprintf(arquivo, "%d %d %.6f\n", i, j, campo[i][j]);
            }
        }
        fclose(arquivo);
    #endif
}

int main() {
    double **u = alocar_matriz(NX, NY);
    double **v = alocar_matriz(NX, NY);
    double **u_novo = alocar_matriz(NX, NY);
    double **v_novo = alocar_matriz(NX, NY);

    #pragma omp parallel for
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            u[i][j] = 0.0;
            v[i][j] = 0.0;
            double dx_dist = i - NX / 2.0;
            double dy_dist = j - NY / 2.0;
            double dist = sqrt(dx_dist * dx_dist + dy_dist * dy_dist);
            if (dist < 20.0) {
                u[i][j] = 2.0 * exp(-dist * dist / 50.0);
                v[i][j] = 1.5 * exp(-dist * dist / 50.0);
            }
        }
    }

    // Salva o estado inicial se a saída de arquivos estiver habilitada
    salvar_campo("u_campo_passo0.dat", u);
    salvar_campo("v_campo_passo0.dat", v);

    printf("Iniciando a simulação paralela SIMPLES (sem collapse/schedule)...\n");
    double tempo_inicio = omp_get_wtime();

    for (int passo = 1; passo <= NT; passo++) {
        
        // --- DIRETIVA OPENMP BÁSICA ---
        // Apenas o paralelismo do laço externo 'i' é aplicado.
        #pragma omp parallel for
        for (int i = 1; i < NX - 1; i++) {
            for (int j = 1; j < NY - 1; j++) {
                double d2u_dx2 = (u[i+1][j] - 2.0*u[i][j] + u[i-1][j]) / (DX*DX);
                double d2u_dy2 = (u[i][j+1] - 2.0*u[i][j] + u[i][j-1]) / (DY*DY);
                double d2v_dx2 = (v[i+1][j] - 2.0*v[i][j] + v[i-1][j]) / (DX*DX);
                double d2v_dy2 = (v[i][j+1] - 2.0*v[i][j] + v[i][j-1]) / (DY*DY);
                
                u_novo[i][j] = u[i][j] + DT*NU*(d2u_dx2 + d2u_dy2);
                v_novo[i][j] = v[i][j] + DT*NU*(d2v_dx2 + d2v_dy2);
            }
        }
        
        // Aplicação das condições de contorno (em série)
        for (int i=0; i<NX; i++) {
            u_novo[i][0] = u_novo[i][NY-2];
            u_novo[i][NY-1] = u_novo[i][1];
            v_novo[i][0] = v_novo[i][NY-2];
            v_novo[i][NY-1] = v_novo[i][1];
        }
        for (int j=0; j<NY; j++) {
            u_novo[0][j] = u_novo[NX-2][j];
            u_novo[NX-1][j] = u_novo[1][j];
            v_novo[0][j] = v_novo[NX-2][j];
            v_novo[NX-1][j] = v_novo[1][j];
        }

        // Troca de ponteiros
        double **temp_u = u;
        u = u_novo;
        u_novo = temp_u;
        double **temp_v = v;
        v = v_novo;
        v_novo = temp_v;

        // Salva arquivos intermediários se a saída estiver habilitada
        if (passo % FREQUENCIA_SAIDA == 0) {
            char nome_arquivo_u[50], nome_arquivo_v[50];
            sprintf(nome_arquivo_u, "u_campo_passo%d.dat", passo);
            sprintf(nome_arquivo_v, "v_campo_passo%d.dat", passo);
            salvar_campo(nome_arquivo_u, u);
            salvar_campo(nome_arquivo_v, v);
        }
    }

    double tempo_fim = omp_get_wtime();
    printf("Tempo de execução: %.6f segundos\n", tempo_fim - tempo_inicio);

    liberar_matriz(u);
    liberar_matriz(v);
    liberar_matriz(u_novo);
    liberar_matriz(v_novo);

    return 0;
}