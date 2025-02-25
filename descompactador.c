#include <stdio.h>
#include "leitor_bit.h"
#include "Arvore.h"
#include "pilha.h"

///realmente o descompactador -----------------
#define ARVORE "tree.bin"
#define CODIGO "code.h4k"
#define ALTURA_IMAGEM 720
#define LARGURA_IMAGEM 1280
#define TAM_GRID 8
#define CANAIS 3


void mostra_saida(uint8_t*** img)
{
    for (int y = 0 ; y < ALTURA_IMAGEM ; y++)
    {
        printf("\n");
        for (int x = 0 ; x < LARGURA_IMAGEM ; x++)
            printf("(%d , %d , %d)", img[y][x][0], img[y][x][1], img[y][x][2]);
    }
}

void descompacta_grid(int altura, int largura, Leitor_de_bytes *arvore, Leitor_de_bytes *codigo, uint8_t*** saida, int x_ini_saida, int y_ini_saida)
{

    //abrindo o arquivo
    printf("Chamou descompacta_grid\n");
    //abrindo o arquivo

    int bit1, bit2; //pra poder receber -1
    Arvore* raiz = aloca_arvore();
    Arvore* atual = raiz;
    Pilha* pilha_de_filhos = inicializa_pilha(100); //tamanho 100 empirico pra pilha

    //lendo cada bit
    while ((bit1 = ler_bit(arvore)) != -1)
    {
        bit2 =  ler_bit(arvore);
        printf("par lido %d%d\n", bit1, bit2);
        system("pause");
        //criando filhos se tiver
        if(bit2 == 1)
        {
            atual->dir = aloca_arvore();
            coloca_na_pilha(pilha_de_filhos, atual -> dir);
        }
        if (bit1 == 1)
        {
            atual->esq = aloca_arvore();
            coloca_na_pilha(pilha_de_filhos, atual -> esq);
        }
        //iniciando a cor no Nó se for uma folha
        if((bit1 == 0) && (bit2 == 0))
            le_cor(atual, arvore);

        //pega o proximo ponteiro
        atual = retira_da_pilha(pilha_de_filhos);

        //se acabou a arvore encerra o loop
        if (atual == NULL)
            break;
    }

    //Aqui a arvore ja foi lida logo a gente garante que o proximo leitor vai começar na proxima arvore e nao em padding
    //while(arvore -> posicao_bit < 8)
        //bit1 == ler_bit(arvore);

    //debugando com print pq né
    mostra_arvore(raiz, 0);
    desaloca_pilha(pilha_de_filhos);

    //arvore feita ======================================
    printf("\nArvore lida e feita\n");

    //lendo cada bit
    atual = raiz;

    for (int y = 0 ; y < altura ; y++)
    {
        //printf("y: %d\n", y);
        for (int x = 0 ; x < largura ; x++)
        {
            //printf("x: %d\n", x);
            while (!(atual -> folha))
            {
                //codigo de huffman até encontrar a cor
                bit1 = ler_bit(codigo);
                if (bit1 == 1)
                    atual = atual->esq;
                else if (bit1 == 0)
                    atual = atual -> dir;
            }
            //colocando a cor na saida
            printf(" x: %d, y: %d\n", x_ini_saida + x,  y_ini_saida + y);
            saida[y_ini_saida + y][x_ini_saida + x][0] = atual -> r;
            saida[y_ini_saida + y][x_ini_saida + x][1] = atual -> g;
            saida[y_ini_saida + y][x_ini_saida + x][2] = atual -> b;

            //reiniciando o atual na raiz
            atual = raiz;
        }
    }


    desaloca_arvore(raiz);


    //fecha o arquivo do codigo
    //fechar_Leitor_de_bytes(byte_lido);

    //mostra_saida(saida);

}


int main()
{

    Leitor_de_bytes* huff_bytes = criar_Leitor_de_bytes(CODIGO);
    Leitor_de_bytes* huff_tree = criar_Leitor_de_bytes(ARVORE);

    uint8_t*** saida = (uint8_t***)malloc(sizeof(uint8_t**) * ALTURA_IMAGEM);
    for (int i = 0 ; i < ALTURA_IMAGEM ; i++)
    {
        saida[i] = (uint8_t**)malloc(sizeof(uint8_t*) * LARGURA_IMAGEM);
        for (int j = 0 ; j < LARGURA_IMAGEM ; j++)
            saida[i][j] = (uint8_t*)malloc(sizeof(uint8_t) * CANAIS);
    }

    for (int y_out = 0 ; y_out < ALTURA_IMAGEM ; y_out += TAM_GRID)
        for(int x_out = 0 ; x_out < LARGURA_IMAGEM ; x_out+= TAM_GRID)
            descompacta_grid(TAM_GRID, TAM_GRID, huff_tree, huff_bytes, saida, x_out, y_out);

    mostra_saida(saida);
}
