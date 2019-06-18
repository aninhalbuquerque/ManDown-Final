#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define ALTURA 832
#define LARGURA 832
#define MAX_CLIENT 6
#define TAM_MAPA 13
#define seg 100000000
struct msg_ret_t mensagem;

typedef struct{
	int qtJogadores;
	int winx[MAX_CLIENT];
} Inicio;
typedef struct {
	int x, y;
} Posicao;
typedef struct {
    Posicao pos;
    char direcao;
    int qualWinx;
} Winx;
typedef struct {
	Posicao posicoesAtualizadas[MAX_CLIENT];
	char dir[MAX_CLIENT];
	int qualWinx[MAX_CLIENT];
	Posicao macho, cristal, machoMorto;
	int qualMacho, qualCristal, qualMachoMorto;
	int pontuacao[MAX_CLIENT];
	bool anda[MAX_CLIENT];
} Movimentacao;

short int matriz[13][13] = {{22, 22, 11, 11, 22, 22, 11, 11, 11, 11, 22, 22, 22},
                           {11, 11, 11, 11, 22, 22, 11, 22, 11, 22, 22, 22, 22},
                           {22, 11, 11, 11, 22, 22, 11, 11, 11, 22, 22, 22, 22},
                           {22, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
                           {22, 11, 11, 11, 11, 11, 11, 22, 22, 11, 11, 11, 22},
                           {11, 22, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},
                           {11, 11, 11, 11, 11, 22, 11, 11, 11, 11, 11, 11, 11},
                           {22, 22, 22, 22, 22, 11, 11, 11, 11, 11, 11, 11, 11},
                           {22, 22, 22, 22, 22, 11, 11, 11, 11, 11, 11, 11, 11},
                           {22, 22, 22, 22, 22, 11, 11, 11, 11, 11, 11, 11, 11},
                           {11, 11, 11, 11, 11, 11, 22, 11, 11, 11, 11, 11, 11},
                           {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 22, 11, 22},
                           {22, 11, 11, 22, 11, 11, 11, 11, 22, 11, 11, 11, 22}};

Inicio pacoteInicial;
Movimentacao pacote;
int players=0;

bool verificaBateu(int id){
   if(matriz[pacote.posicoesAtualizadas[id].y][pacote.posicoesAtualizadas[id].x] == 22){
      return false;
   }
   if(matriz[pacote.posicoesAtualizadas[id].y][pacote.posicoesAtualizadas[id].x]>=0 && matriz[pacote.posicoesAtualizadas[id].y][pacote.posicoesAtualizadas[id].x]<6){
      return false;
   }
   return true;
}

void gerarPosMacho(){
	pacote.qualMacho++;
	if(pacote.qualMacho==4) pacote.qualMacho=0;
	pacote.macho.x = rand()%13;
	pacote.macho.y = rand()%13;
	if (matriz[pacote.macho.y][pacote.macho.x]!=11){
		gerarPosMacho();
	}
	else {
		matriz[pacote.macho.y][pacote.macho.x] = 10;
	}
}

void gerarPosCristal(){
	pacote.qualCristal++;
	if(pacote.qualCristal==3) pacote.qualCristal=0;
	pacote.cristal.x = rand()%13;
	pacote.cristal.y = rand()%13;
	if (matriz[pacote.cristal.y][pacote.cristal.x]!=11){
		gerarPosCristal();
	}
	else {
		matriz[pacote.cristal.y][pacote.cristal.x] = 20;
	}
}

void escolherWinx(int **escolhidas, char msg){
	if(msg==0){
		if((*escolhidas)[0]==0){
			fprintf(stderr, "escolheu 0\n");
			(*escolhidas)[0] = 1;
			char msg2 = msg;
			msg = 11;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
			broadcast(&msg2, sizeof(char));
			pacoteInicial.winx[mensagem.client_id] = 0;
			pacote.qualWinx[mensagem.client_id] = 0;
		}
		else{
			fprintf(stderr, "tentou escolher 0\n");
			msg = 12;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
		}
	}
	if(msg==1){
		if((*escolhidas)[1]==0){
			fprintf(stderr, "escolheu 1\n");
			(*escolhidas)[1] = 1;
			char msg2 = msg;
			msg = 11;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
			broadcast(&msg2, sizeof(char));
			pacoteInicial.winx[mensagem.client_id] = 1;
			pacote.qualWinx[mensagem.client_id] = 1;
		}
		else{
			fprintf(stderr, "tentou escolher 1\n");
			msg = 12;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
		}
	}
	if(msg==2){
		if((*escolhidas)[2]==0){
			fprintf(stderr, "escolheu 2\n");
			(*escolhidas)[2] = 1;
			char msg2 = msg;
			msg = 11;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
			broadcast(&msg2, sizeof(char));
			pacoteInicial.winx[mensagem.client_id] = 2;
			pacote.qualWinx[mensagem.client_id] = 2;
		}
		else{
			fprintf(stderr, "tentou escolher 2\n");
			msg = 12;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
		}
	}
	if(msg==3){
		if((*escolhidas)[3]==0){
			fprintf(stderr, "escolheu 3\n");
			(*escolhidas)[3] = 1;
			char msg2 = msg;
			msg = 11;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
			broadcast(&msg2, sizeof(char));
			pacoteInicial.winx[mensagem.client_id] = 3;
			pacote.qualWinx[mensagem.client_id] = 3;
		}
		else{
			fprintf(stderr, "tentou escolher 3\n");
			msg = 12;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
		}
	}
	if(msg==4){
		if((*escolhidas)[4]==0){
			fprintf(stderr, "escolheu 4\n");
			(*escolhidas)[4] = 1;
			char msg2 = msg;
			msg = 11;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
			broadcast(&msg2, sizeof(char));
			pacoteInicial.winx[mensagem.client_id] = 4;
			pacote.qualWinx[mensagem.client_id] = 4;
		}
		else{
			fprintf(stderr, "tentou escolher 4\n");
			msg = 12;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
		}
	}
	if(msg==5){
		if((*escolhidas)[5]==0){
			fprintf(stderr, "escolheu 5\n");
			(*escolhidas)[5] = 1;
			char msg2 = msg;
			msg = 11;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
			broadcast(&msg2, sizeof(char));
			pacoteInicial.winx[mensagem.client_id] = 5;
			pacote.qualWinx[mensagem.client_id] = 5;
		}
		else{
			fprintf(stderr, "tentou escolher 5\n");
			msg = 12;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
		}
	}
	if(msg==6){
		if((*escolhidas)[6]==0){
			fprintf(stderr, "escolheu 6\n");
			(*escolhidas)[6] = 1;
			char msg2 = msg;
			msg = 11;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
			broadcast(&msg2, sizeof(char));
			pacoteInicial.winx[mensagem.client_id] = 6;
		}
		else{
			fprintf(stderr, "tentou escolher 6\n");
			msg = 12;
			sendMsgToClient(&msg, sizeof(char), mensagem.client_id);
		}
	}
}

void mostraPosicao(int qtJogadores){
    int i;
    int cont=0;
    char recDirecao;
    pacote.macho.x=0; pacote.macho.y=1;
	pacote.qualMacho = 0;
	pacote.cristal.x=6; pacote.cristal.y=6;
	pacote.qualCristal = 0;
	matriz[1][0] = 10; matriz[6][6] = 20;
	pacote.qualMachoMorto = -1;
    for(i=0;i<qtJogadores;i++){
		pacote.anda[i] = true;
        pacote.pontuacao[i] = 0;
        //y é linha, x é coluna
        switch (i)
        {
        case 0:
            pacote.posicoesAtualizadas[i].x = 1; pacote.posicoesAtualizadas[i].y = 1;
			matriz[1][1]=0;
            break;
        case 1:
            pacote.posicoesAtualizadas[i].x = 12; pacote.posicoesAtualizadas[i].y = 3;
            matriz[3][12]=1;
			break;
        case 2:
            pacote.posicoesAtualizadas[i].x = 11; pacote.posicoesAtualizadas[i].y = 12;
			matriz[12][11]=2;
            break;
        case 3:
            pacote.posicoesAtualizadas[i].x = 1; pacote.posicoesAtualizadas[i].y = 12;
			matriz[12][1]=3;
            break;
        case 4:
            pacote.posicoesAtualizadas[i].x = 0; pacote.posicoesAtualizadas[i].y = 10;
			matriz[10][0]=4;
            break;
        case 5:
            pacote.posicoesAtualizadas[i].x = 0; pacote.posicoesAtualizadas[i].y = 6;
			matriz[6][0]=5;
            break;
        }
    } 
	int achouCristal=0, achouCristalTempo=0;
	int matouMachoTempo=0;
	int ok = 1;
    while(ok){
		if(achouCristal==1){
			achouCristalTempo++;
			//printf("tempo=%d\n", achouCristalTempo);
			if(achouCristalTempo>=20000){
				achouCristal=0; achouCristalTempo=0;
				for(i=0;i<players;i++) pacote.anda[i]=true;
				gerarPosCristal();
				broadcast(&pacote, sizeof(Movimentacao));
			}
		}
		if(pacote.qualMachoMorto!=-1){
			matouMachoTempo++;
			if(matouMachoTempo>=2000){
				pacote.qualMachoMorto = -1;
				matouMachoTempo = 0;
				matriz[pacote.machoMorto.y][pacote.machoMorto.x] = 11;
				broadcast(&pacote, sizeof(Movimentacao));
			}
		}
        mensagem = recvMsg(&recDirecao);
        cont++;
		if(cont >= seg/60){
			broadcast(&pacote, sizeof(Movimentacao));
		}
        if(mensagem.status == MESSAGE_OK){
            switch (recDirecao) {
				case 'w':
					//printf("%d tentou ir pra cima\n", mensagem.client_id);
					pacote.dir[mensagem.client_id] = 'w';
					pacote.posicoesAtualizadas[mensagem.client_id].y--;
					if(!verificaBateu(mensagem.client_id) || pacote.posicoesAtualizadas[mensagem.client_id].y<0 || !pacote.anda[mensagem.client_id]){ 
						pacote.posicoesAtualizadas[mensagem.client_id].y++;
					}
					else{
						matriz[pacote.posicoesAtualizadas[mensagem.client_id].y+1][pacote.posicoesAtualizadas[mensagem.client_id].x] = 11;
						if(matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] == 10){
							pacote.pontuacao[mensagem.client_id]++;
							pacote.machoMorto.x = pacote.posicoesAtualizadas[mensagem.client_id].x;
							pacote.machoMorto.y = pacote.posicoesAtualizadas[mensagem.client_id].y;
							pacote.qualMachoMorto = pacote.qualMacho;
							gerarPosMacho();
						}
						else if(matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] == 20){
							switch (pacote.qualCristal)
							{
							case 0:
								pacote.pontuacao[mensagem.client_id] = 0;
								gerarPosCristal();
								break;
							
							case 1:
								for(i=0;i<players;i++){
									if(i==mensagem.client_id) pacote.pontuacao[i] += players-1;
									else pacote.pontuacao[i]--;
								}
								gerarPosCristal();
								break;
							case 2:
								achouCristal=1;
								for(i=0;i<players;i++){
									if(i!=mensagem.client_id) pacote.anda[i] = false;
								}
								pacote.qualCristal=-1;
							}
						}
					}
					matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] = mensagem.client_id;
					broadcast(&pacote, sizeof(Movimentacao));
					break;
				case 'a':
					//printf("%d tentou ir pra esquerda\n", mensagem.client_id);
					pacote.dir[mensagem.client_id] = 'a';
					pacote.posicoesAtualizadas[mensagem.client_id].x--;
					if(!verificaBateu(mensagem.client_id) || pacote.posicoesAtualizadas[mensagem.client_id].x<0 || !pacote.anda[mensagem.client_id]){
						pacote.posicoesAtualizadas[mensagem.client_id].x++;
					}
					else{
						matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x+1] = 11;
						if(matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] == 10){
							pacote.pontuacao[mensagem.client_id]++;
							pacote.machoMorto.x = pacote.posicoesAtualizadas[mensagem.client_id].x;
							pacote.machoMorto.y = pacote.posicoesAtualizadas[mensagem.client_id].y;
							pacote.qualMachoMorto = pacote.qualMacho;
							gerarPosMacho();
						}
						else if(matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] == 20){
							switch (pacote.qualCristal)
							{
							case 0:
								pacote.pontuacao[mensagem.client_id] = 0;
								gerarPosCristal();
								break;
							
							case 1:
								for(i=0;i<players;i++){
									if(i==mensagem.client_id) pacote.pontuacao[i] += players-1;
									else pacote.pontuacao[i]--;
								}
								gerarPosCristal();
								break;
							case 2:
								achouCristal=1;
								for(i=0;i<players;i++){
									if(i!=mensagem.client_id) pacote.anda[i] = false;
								}
								pacote.qualCristal=-1;
							}
						}
					}
					matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] = mensagem.client_id;
					broadcast(&pacote, sizeof(Movimentacao));
					break;
				case 's':
					//printf("%d tentou ir pra baixo\n", mensagem.client_id);
					pacote.dir[mensagem.client_id] = 's';
					pacote.posicoesAtualizadas[mensagem.client_id].y++;
					if(!verificaBateu(mensagem.client_id) || pacote.posicoesAtualizadas[mensagem.client_id].y>=13 || !pacote.anda[mensagem.client_id]){
						pacote.posicoesAtualizadas[mensagem.client_id].y--;
					}
					else{
						matriz[pacote.posicoesAtualizadas[mensagem.client_id].y-1][pacote.posicoesAtualizadas[mensagem.client_id].x] = 11;
						if(matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] == 10){
							pacote.pontuacao[mensagem.client_id]++;
							pacote.machoMorto.x = pacote.posicoesAtualizadas[mensagem.client_id].x;
							pacote.machoMorto.y = pacote.posicoesAtualizadas[mensagem.client_id].y;
							pacote.qualMachoMorto = pacote.qualMacho;
							gerarPosMacho();
						}
						else if(matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] == 20){
							switch (pacote.qualCristal)
							{
							case 0:
								pacote.pontuacao[mensagem.client_id] = 0;
								gerarPosCristal();
								break;
							
							case 1:
								for(i=0;i<players;i++){
									if(i==mensagem.client_id) pacote.pontuacao[i] += players-1;
									else pacote.pontuacao[i]--;
								}
								gerarPosCristal();
								break;
							case 2:
								achouCristal=1;
								for(i=0;i<players;i++){
									if(i!=mensagem.client_id) pacote.anda[i] = false;
								}
								pacote.qualCristal=-1;
							}
						}
					}
					matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] = mensagem.client_id;
					broadcast(&pacote, sizeof(Movimentacao));
					break;
				case 'd':
					//printf("%d tentou ir pra esquerda\n", mensagem.client_id);
					pacote.dir[mensagem.client_id] = 'd';
					pacote.posicoesAtualizadas[mensagem.client_id].x++;
					if(!verificaBateu(mensagem.client_id) || pacote.posicoesAtualizadas[mensagem.client_id].x>=13 || !pacote.anda[mensagem.client_id]){
						pacote.posicoesAtualizadas[mensagem.client_id].x--;
					}
					else{
						matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x-1] = 11;
						if(matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] == 10){
							pacote.pontuacao[mensagem.client_id]++;
							pacote.machoMorto.x = pacote.posicoesAtualizadas[mensagem.client_id].x;
							pacote.machoMorto.y = pacote.posicoesAtualizadas[mensagem.client_id].y;
							pacote.qualMachoMorto = pacote.qualMacho;
							gerarPosMacho();
						}
						else if(matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] == 20){
							switch (pacote.qualCristal)
							{
							case 0:
								pacote.pontuacao[mensagem.client_id] = 0;
								gerarPosCristal();
								break;
							
							case 1:
								for(i=0;i<players;i++){
									if(i==mensagem.client_id) pacote.pontuacao[i] += players-1;
									else pacote.pontuacao[i]--;
								}
								gerarPosCristal();
								break;
							case 2:
								achouCristal=1;
								for(i=0;i<players;i++){
									if(i!=mensagem.client_id) pacote.anda[i] = false;
								}
								pacote.qualCristal=-1;
							}
						}
					}
					matriz[pacote.posicoesAtualizadas[mensagem.client_id].y][pacote.posicoesAtualizadas[mensagem.client_id].x] = mensagem.client_id;
					broadcast(&pacote, sizeof(Movimentacao));
					break;
					case 'e':
						printf("cabo\n");
						ok = 0;
						break;
				
				}

        }
    }


}

int main(){

	system("clear");
	fprintf(stderr,"oi\n");
	bool sair = false;
	serverInit(MAX_CLIENT);
	while(!sair){
		players = 0;
		int ret, i, j;
		int *escolhidas = (int *) calloc(MAX_CLIENT, sizeof(int));
		if(escolhidas==NULL){
			printf("Erro ao alocar escolhidas\n"); exit(0);
		}
		for(i=0;i<MAX_CLIENT;i++) pacoteInicial.winx[i] = -1;
		int apertou=0;
		while(!apertou && players<=MAX_CLIENT){
			char msg;
			ret = acceptConnection();
			if(ret != NO_CONNECTION){
				fprintf(stderr,"Usuario %d conectado ao servidor\n",ret);
				sendMsgToClient(&ret, sizeof(int), ret);
				if(ret==players) players++;
			}
			mensagem = recvMsg(&msg);
			if(mensagem.status == MESSAGE_OK){
				if(msg=='A'){
					sendMsgToClient(escolhidas, MAX_CLIENT*sizeof(int), mensagem.client_id);
				}
				if(msg=='y'){
					apertou=1; 
					broadcast(&msg, sizeof(char));
				}
				if(msg>=0 && msg<MAX_CLIENT){
					escolherWinx(&escolhidas, msg);
				}
			}
		}
		free(escolhidas);
		//rejectConnection();
		pacoteInicial.qtJogadores = players;
		broadcast(&pacoteInicial, sizeof(Inicio));
		for(i=0;i<TAM_MAPA;i++){
			for(j=0;j<TAM_MAPA;j++){
				if(matriz[i][j] != 22) matriz[i][j] = 11;
			}
		}
		mostraPosicao(players);
		int saidas=0;
		while(saidas<players){
			char msg;
			mensagem = recvMsg(&msg);
			if(mensagem.status == MESSAGE_OK){
				if(msg == 's'){
					disconnectClient(mensagem.client_id);
					saidas++;
				}
				if(msg == 'p'){
					for(int i=0;i<MAX_CLIENT;i++){
						disconnectClient(i);
						fprintf(stderr,"Usuario %d desconectado\n",i);
					}
					break;
				}
			}
		}
		if(saidas==players) sair = true;
	}
	serverReset();
	
	return 0;
}
