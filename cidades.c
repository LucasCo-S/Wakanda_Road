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
    //Abrindo arquivo 
    FILE *arq = fopen(nomeArquivo, "r");
    if(arq == NULL){
        printf("ERROR - Não foi possivel abrir o arquivo.\n");
        return NULL;
    }

    //Verificando se está vazio
    int ch = fgetc(arq);
    if(ch == EOF) {
        printf("ERROR - Arquivo vazio.\n");
        fclose(arq);
        return NULL;
    }
    ungetc(ch, arq);

    //Leitura e armazenamento dos dados (com verificação da integridade dos dados)
    Estrada *wakanda = (Estrada *)malloc(sizeof(Estrada));
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

    //Verificando a integridade de tamanho
    if((wakanda->T < MIN_T || wakanda->T > MAX_T) || (wakanda->N < MIN_N || wakanda->N > MAX_N)){
        printf("ERROR - Valores min/max excedidos.\n");
        free(wakanda);
        fclose(arq);
        return NULL;
    }

    Cidade *cidades = (Cidade *)malloc(sizeof(Cidade) * wakanda->N);
    int i;
    int verif = 2; //Número de posição fora dos limites. 
    for(i=0;i<(wakanda->N);i++){
        if((fscanf(arq, "%d %[^\n]", &cidades[i].Posicao, cidades[i].Nome)) != 2){
            printf("ERROR - Leitura inválida na linha: %d.\n", i+3);
            free(wakanda);
            fclose(arq);
            return NULL;
        }

        if(!(cidades[i].Posicao > 0 && cidades[i].Posicao <= wakanda->T)){
            printf("ERROR - Posicao da cidade fora dos limites especificados.\n");
            free(wakanda);
            fclose(arq);
            return NULL;
        }

        if(cidades[i].Posicao == verif){
            printf("ERROR - Posicoes de cidades iguais.\n");
            free(wakanda);
            fclose(arq);
            return NULL;
        }
        verif = cidades[i].Posicao;
    }
    i++;
    if((fscanf(arq, "%d %[^\n]", &cidades[i].Posicao, cidades[i].Nome)) == 2){
        printf("ERROR - Quantidade de cidades maior que o especificado.\n");
        free(wakanda);
        fclose(arq);
        return NULL;
    }

    wakanda->C = cidades;

    fclose(arq);

    return wakanda;
}

int partition(Estrada *wakanda, int low, int high){
    int pivot = (low + high)/2;
    pivot = wakanda->C[pivot].Posicao;

    int i = low; 
    int j = high;

    while(wakanda->C[i].Posicao < pivot) i++;
    while(wakanda->C[j].Posicao > pivot) j--;

    if(i <= j){
        Cidade aux = wakanda->C[i];
        wakanda->C[i] = wakanda->C[j];
        wakanda->C[j] = aux;

        i++;
        j--;
    }

    return i;
}

void cidadesSort(Estrada *wakanda, int low, int high){
    if(high > low){
        int P = partition(wakanda, low, high);

        if (low < P - 1) cidadesSort(wakanda, low, P - 1);
        if (P < high) cidadesSort(wakanda, P, high);
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

    VIZINHANCA *wkd_vz =  calcularVizinhanca(wakanda);

    double smallestVz = 10000000000.0;
    for(int i=0;i<wakanda->N;i++){
        if(wkd_vz[i].Viz < smallestVz){
            smallestVz = wkd_vz[i].Viz;
        }
    }

    return smallestVz;
}

char *cidadeMenorVizinhanca(const char *nomeArquivo){
    Estrada *wakanda = getEstrada(nomeArquivo);

    VIZINHANCA *wkd_vz =  calcularVizinhanca(wakanda);
    
    double smallestVz = 10000000000.0;
    char *town = (char *)malloc(sizeof(char) * 500);
    for(int i=0;i<wakanda->N;i++){
        if(wkd_vz[i].Viz < smallestVz){
            smallestVz = wkd_vz[i].Viz;
            strcpy(town, wkd_vz[i].index.Nome);
        }
    }

    return town;
}