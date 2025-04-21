#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"cidades.h"

#define MAX_T 1000000
#define MIN_T 3
#define MAX_N 10000
#define MIN_N 2

typedef struct{
    double Viz;
    double Front;
    Cidade index;
}VIZINHANCA;

Estrada *getEstrada(const char *nomeArquivo){
    // Abrindo arquivo 
    FILE *arq = fopen(nomeArquivo, "r");
    if(arq == NULL){
        printf("ERROR - Não foi possivel abrir o arquivo.\n");

        return NULL;
    }

    // Verificando se está vazio
    int ch = fgetc(arq);
    if(ch == EOF){
        printf("ERROR - Arquivo vazio.\n");

        fclose(arq);
        return NULL;
    }
    ungetc(ch, arq);

    // Leitura e armazenamento dos dados (com verificação da integridade dos dados)
    Estrada *wakanda = (Estrada *)malloc(sizeof(Estrada));
    if(wakanda == NULL){
        printf("ERROR - Falha ao alocar memoria.\n");

        fclose(arq);
        return NULL;
    }

    if((fscanf(arq,"%d", &wakanda->T)) != 1){
        printf("ERROR - Leitura inválida na linha 1.\n");

        free(wakanda);
        fclose(arq);
        return NULL;
    }

    if((fscanf(arq,"%d", &wakanda->N)) != 1){
        printf("ERROR - Leitura inválida na linha 2.\n");

        free(wakanda);
        fclose(arq);
        return NULL;
    }

    // Verificando a integridade de tamanho
    if((wakanda->T < MIN_T || wakanda->T > MAX_T) || (wakanda->N < MIN_N || wakanda->N > MAX_N)){
        printf("ERROR - Valores min/max excedidos.\n");

        free(wakanda);
        fclose(arq);
        return NULL;
    }

    Cidade *cidades = (Cidade *)malloc(sizeof(Cidade) * wakanda->N);
    if(cidades == NULL){
        printf("ERROR - Falha ao alocar memoria para cidades.\n");

        free(wakanda);
        fclose(arq);
        return NULL;
    }

    Cidade *uniqueValues = (Cidade *)malloc(sizeof(Cidade) * wakanda->N); 
    if(uniqueValues == NULL){
        printf("ERROR - Falha ao alocar memoria para uniqueValues.\n");

        free(cidades);
        free(wakanda);
        fclose(arq);
        
        return NULL;
    }

    int count = 0;
    int i;

    for(i=0; i<(wakanda->N); i++){
        if((fscanf(arq, "%d %[^\n]", &cidades[i].Posicao, cidades[i].Nome)) != 2){
            printf("ERROR - Leitura inválida na linha: %d.\n", i+3);

            free(uniqueValues);
            free(cidades);
            free(wakanda);
            
            fclose(arq);
            return NULL;
        }

        if(!(cidades[i].Posicao > 0 && cidades[i].Posicao <= wakanda->T)){
            printf("ERROR - Posicao da cidade fora dos limites especificados.\n");

            free(uniqueValues);
            free(cidades);
            free(wakanda);

            fclose(arq);
            return NULL;
        }

        for(int j=0; j<count; j++){
            if((strcmp(cidades[i].Nome, uniqueValues[j].Nome) == 0) || cidades[i].Posicao == uniqueValues[j].Posicao){
                printf("ERROR - Cidades com poisicoes ou nomes iguais.\n");

                free(uniqueValues);
                free(cidades);
                free(wakanda);

                fclose(arq);
                return NULL;
            }
        }

        strcpy(uniqueValues[count].Nome, cidades[i].Nome);
        uniqueValues[count].Posicao = cidades[i].Posicao;
        count++;
    }

    int extra_city = 0;
    Cidade aux;
    if((fscanf(arq, "%d %[^\n]", &aux.Posicao, aux.Nome)) == 2){
        printf("ERROR - Quantidade de cidades maior que o especificado.\n");

        free(uniqueValues);
        free(cidades);
        free(wakanda);

        fclose(arq);
        return NULL;
    }

    wakanda->C = cidades;

    free(uniqueValues);
    fclose(arq);

    return wakanda;
}

int partition(Estrada *wakanda, int low, int high){
    if(low >= high) return low;
    
    int pivot = (low + high)/2;
    pivot = wakanda->C[pivot].Posicao;

    int i = low; 
    int j = high;

    while(i <= j){
        while(i <= high && wakanda->C[i].Posicao < pivot) i++;
        while(j >= low && wakanda->C[j].Posicao > pivot) j--;

        if(i <= j){
            Cidade aux = wakanda->C[i];
            wakanda->C[i] = wakanda->C[j];
            wakanda->C[j] = aux;
            i++;
            j--;
        }
    }

    return i;
}

void cidadesSort(Estrada *wakanda, int low, int high){
    if(low < high){
        int P = partition(wakanda, low, high);
        if(low < P - 1) cidadesSort(wakanda, low, P - 1);
        if(P < high) cidadesSort(wakanda, P, high);
    }
}

VIZINHANCA *calcularVizinhanca(Estrada *wakanda){
    VIZINHANCA *wkd_vz = (VIZINHANCA *)malloc(sizeof(VIZINHANCA) * wakanda->N);
    if(wkd_vz == NULL){
        printf("ERROR - Falha ao alocar memoria.\n");
        return NULL;
    }

    cidadesSort(wakanda, 0, wakanda->N-1);

    for(int i=0;i<wakanda->N;i++){
        wkd_vz[i].index = wakanda->C[i]; 
    }

    for(int i=0;i<(wakanda->N);i++){
        wkd_vz[i].Front = ((double)wakanda->C[i].Posicao + (double)wakanda->C[i+1].Posicao)/2;
    }
    wkd_vz[wakanda->N-1].Front = wkd_vz[wakanda->N-2].Front;

    int i = 0;
    wkd_vz[i].Viz = wkd_vz[i].Front;

    for(i=1;i<wakanda->N;i++){
        wkd_vz[i].Viz = wkd_vz[i].Front - wkd_vz[i-1].Front;
        if(wkd_vz[i].Viz < 0) wkd_vz[i].Viz*=(-1);
    }

    wkd_vz[wakanda->N-1].Viz = wakanda->T - wkd_vz[wakanda->N-1].Front;

    return wkd_vz;
}

double calcularMenorVizinhanca(const char *nomeArquivo){
    Estrada *wakanda = getEstrada(nomeArquivo);
    if(wakanda == NULL) {
        printf("ERROR - Não foi possível obter dados da estrada.\n");
        return -1.0;
    }

    VIZINHANCA *wkd_vz = calcularVizinhanca(wakanda);
    if(wkd_vz == NULL) {
        printf("ERROR - Não foi possível calcular a vizinhanca.\n");

        free(wakanda->C);
        free(wakanda);

        return -1.0;
    }

    double smallestVz = 10000000000.0;
    for(int i=0; i<wakanda->N; i++){
        if(wkd_vz[i].Viz < smallestVz){
            smallestVz = wkd_vz[i].Viz;
        }
    }

    //printf("Menor vizinhanca: %.2lf\n", smallestVz);
    
    free(wkd_vz);
    free(wakanda->C);
    free(wakanda);
    
    return smallestVz;
}

char *cidadeMenorVizinhanca(const char *nomeArquivo){
    Estrada *wakanda = getEstrada(nomeArquivo);
    if(wakanda == NULL) {
        printf("ERROR - Não foi possível obter dados da estrada.\n");

        return NULL;
    }

    VIZINHANCA *wkd_vz = calcularVizinhanca(wakanda);
    if(wkd_vz == NULL) {
        printf("ERROR - Não foi possível calcular a vizinhança.\n");

        free(wakanda->C);
        free(wakanda);

        return NULL;
    }
    
    double smallestVz = 10000000000.0;
    int smallestIdx = 0;
    
    for(int i=0; i<wakanda->N; i++){
        if(wkd_vz[i].Viz < smallestVz){
            smallestVz = wkd_vz[i].Viz;
            smallestIdx = i;
        }
    }
    
    char *town = (char *)malloc(sizeof(char) * 500);
    if(town == NULL) {
        printf("ERROR - Falha ao alocar memória para o nome da cidade.\n");

        free(wkd_vz);
        free(wakanda->C);
        free(wakanda);
        return NULL;
    }
    
    strcpy(town, wkd_vz[smallestIdx].index.Nome);
    //printf("Cidade menor vizinhanca: %s\n", town);

    free(wkd_vz);
    free(wakanda->C);
    free(wakanda);
    
    return town;
}