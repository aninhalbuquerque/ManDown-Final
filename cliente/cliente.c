#include "client.h"
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
#define FPS 60
#define ALTURA 832
#define LARGURA 832
#define KEY_DOWN 85
#define KEY_LEFT 82
#define KEY_RIGHT 83
#define KEY_UP 84
#define TAM_MAPA 13
#define MAX_CLIENT 6
enum conn_ret_t ans;

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
typedef struct{
    int pont;
    int qualWinx;
} Final;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *filaEventos = NULL, *filaCronometro = NULL; 
ALLEGRO_BITMAP *mapa = NULL, *fim = NULL;
ALLEGRO_BITMAP **machos, **cristais, **machosMortos;
ALLEGRO_BITMAP **personagem, **winxPont, **winxRanking;
ALLEGRO_BITMAP **winxMenu, **winxMenuCinza, **auxWinxMenu;
ALLEGRO_BITMAP *inicio = NULL, *inicio2 = NULL; 
ALLEGRO_BITMAP *play1 = NULL, *play2 = NULL;
ALLEGRO_BITMAP *fundoMenu = NULL, *fundoInicio = NULL, *fundoRanking = NULL, *fundoComecar = NULL;
ALLEGRO_BITMAP *fundoIP = NULL, *fundoIPErro = NULL, *fundoMenu2;
ALLEGRO_BITMAP *fundoPont = NULL, *fundoPont2 = NULL, *fundoTempo = NULL;
ALLEGRO_BITMAP *fundo = NULL, *regras = NULL;
ALLEGRO_BITMAP *escolhaPers = NULL, *escolhaPers2 = NULL, *regrasBotao = NULL, *regrasBotao2 = NULL;
ALLEGRO_BITMAP *sairBotao = NULL, *sairBotao2 = NULL, *voltar = NULL, *voltar2 = NULL;
ALLEGRO_BITMAP *playAgain = NULL, *playAgain2 = NULL, *sairRank = NULL, *sairRank2 = NULL;
ALLEGRO_BITMAP **medalha;
ALLEGRO_TIMER *timer = NULL, *timerAnima = NULL;
ALLEGRO_FONT *fonte = NULL, *fonteIP = NULL, *fonteTempo = NULL;


char SERVER_IP[30];
Winx posicaoWinx[MAX_CLIENT];
Movimentacao pacote;
bool key[ALLEGRO_KEY_MAX], redraw = false, sair = false, fecharJogo = false;
char mandarDir;
int i = 1, j = 1, comeu = 0;
int regiao_x_folha = 0, regiao_y_folha = 0, largura_sprite = 64, altura_sprite = 64;
int id, pers;
int players = 0, pontuacao=0;
int temp = 0, segundo = 0, minuto = 0;
int tentConectar=0;


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



bool inicializar(){
    if(!al_init()){
        printf("\nERROR\n");
        return false;
    }
    if (!al_install_keyboard()){
        printf("\nERROR\n");
        return false;
    }
    if(!al_init_image_addon()){
        printf("\nERROR\n");
        return false;
    }
    if(!al_init_primitives_addon()){
        printf("ERRO\n");
        return false;
    }
    if (!al_init_font_addon()){
        fprintf(stderr, "Falha ao inicializar add-on allegro_font.\n");
        return false;
    }
    if (!al_init_ttf_addon()){
        fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
        return false;
    }
    janela = al_create_display(LARGURA, ALTURA);
    if (!janela){
        printf("ERROR - JANELA\n");
        return false;
    }
    fonte = al_load_font("fotinhas/fontes/Jarbomhollow.ttf", 30, 0);
    if (!fonte){
        printf("ERRO - FONTE\n");
        return false;
    }
    fonteIP = al_load_font("fotinhas/fontes/Annamellia.ttf", 90, 0);
    if (!fonteIP){
        printf("ERRO - FONTE_IP\n");
        return false;
    }
    fonteTempo = al_load_font("fotinhas/fontes/Pretty Girls Script Demo.ttf", 65, 0);
    if (!fonteTempo){
        printf("ERRO - FONTE_TEMPO\n");
        return false;
    }
    mapa = al_load_bitmap("fotinhas/mapa.png");
    if(!mapa){
        printf("Erro na img mapa.png\n");
        return false;
    }
    fim = al_load_bitmap("fotinhas/ranking/fimdejogo.png");
    if(!fim){
        printf("Erro na img fim.png\n");
        return false;
    }
    filaEventos = al_create_event_queue();
    if (!filaEventos){
        printf("ERROR - EVENTO\n");
        return false;
    }
    filaCronometro = al_create_event_queue();
    if(!filaCronometro){
        printf("ERROR - fila");
        return false;
    }
    timerAnima = al_create_timer(0.5);
    if(!timerAnima) {
        fprintf(stderr, "failed to create timer!\n");
        return false;
    }
    play1 = al_load_bitmap("fotinhas/inicio/play1.png");
    if(!play1){
        printf("erro play1");
        return false;
    }
    play2 = al_load_bitmap("fotinhas/inicio/play2.png");
    if(!play2){
        printf("erro play2");
        return false;
    }
    playAgain = al_load_bitmap("fotinhas/ranking/again.png");
    if(!playAgain){
        printf("erro playAgain");
        return false;
    }
    playAgain2 = al_load_bitmap("fotinhas/ranking/again2.png");
    if(!playAgain2){
        printf("erro playAgain2");
        return false;
    }
    escolhaPers = al_load_bitmap("fotinhas/inicio/escolhaPers.png");
    if(!escolhaPers){
        printf("erro escolhaPers");
        return false;
    }
    escolhaPers2 = al_load_bitmap("fotinhas/inicio/escolhaPers2.png");
    if(!escolhaPers2){
        printf("erro escolhaPers2");
        return false;
    }
    regrasBotao = al_load_bitmap("fotinhas/inicio/regrasBotao.png");
    if(!regrasBotao){
        printf("erro regrasBotao");
        return false;
    }
    regrasBotao2 = al_load_bitmap("fotinhas/inicio/regrasBotao2.png");
    if(!regrasBotao2){
        printf("erro regrasBotao2");
        return false;
    }
    sairBotao = al_load_bitmap("fotinhas/inicio/sairBotao.png");
    if(!sairBotao){
        printf("erro sairBotao");
        return false;
    }
    sairBotao2 = al_load_bitmap("fotinhas/inicio/sairBotao2.png");
    if(!sairBotao2){
        printf("erro sairBotao2");
        return false;
    }
    sairRank = al_load_bitmap("fotinhas/ranking/sairRank.png");
    if(!sairRank){
        printf("erro sairRank");
        return false;
    }
    sairRank2 = al_load_bitmap("fotinhas/ranking/sairRank2.png");
    if(!sairRank2){
        printf("erro sairRank2");
        return false;
    }
    voltar = al_load_bitmap("fotinhas/inicio/voltar.png");
    if(!voltar){
        printf("erro voltar");
        return false;
    }
    voltar2 = al_load_bitmap("fotinhas/inicio/voltar2.png");
    if(!voltar2){
        printf("erro voltar2");
        return false;
    }
    inicio = al_load_bitmap("fotinhas/inicio/inicioBrilho.png");
    if(!inicio){
        printf("Erro na img agoravai.png\n");
        return false;
    }
    inicio2 = al_load_bitmap("fotinhas/inicio/inicio2Brilho.png");
    if(!inicio2){
        printf("Erro na img agoravai2.png\n");
        return false;
    }
    fundoIP = al_load_bitmap("fotinhas/inicio/fundoIP.png");
    if(!fundoIP){
        printf("ERRO - fundoIP\n");
        return false;
    }
    fundoIPErro = al_load_bitmap("fotinhas/inicio/fundoIPErro.png");
    if(!fundoIPErro){
        printf("ERRO - fundoIPErro\n");
        return false;
    }
    fundoInicio = al_load_bitmap("fotinhas/inicio/fundoInicio.png");
    if(!fundoInicio){
        printf("ERRO - fundoInicio\n");
        return false;
    }
    fundoMenu2 = al_load_bitmap("fotinhas/inicio/fundoMenu.png");
    if(!fundoMenu2){
        printf("ERRO - fundoMenu2\n");
        return false;
    }
    fundoComecar = al_load_bitmap("fotinhas/inicio/fundoComecar.png");
    if(!fundoComecar){
        printf("ERRO - fundoComecar\n");
        return false;
    }
    fundoMenu = al_load_bitmap("fotinhas/bonecasMenu/fundoMenu.png");
    if(!fundoMenu){
        printf("ERRO - fundoMenu\n");
        return false;
    }
    fundo = al_load_bitmap("fotinhas/inicio/fundo.png");
    if(!fundo){
        printf("ERRO - fundo\n");
        return false;
    }
    regras = al_load_bitmap("fotinhas/inicio/regras.png");
    if(!regras){
        printf("ERRO - regras\n");
        return false;
    }
    auxWinxMenu = (ALLEGRO_BITMAP **) malloc (MAX_CLIENT*sizeof(ALLEGRO_BITMAP*));
    if(auxWinxMenu==NULL){
        printf("Erro ao alocar auxWinxMenu\n");
        return false;
    }
    
    winxMenu = (ALLEGRO_BITMAP **) malloc (MAX_CLIENT*sizeof(ALLEGRO_BITMAP*));
    if(winxMenu==NULL){
        printf("Erro ao alocar winxMenu\n");
        return false;
    }
    winxMenu[0] = al_load_bitmap("fotinhas/bonecasMenu/0.png");
    if(!winxMenu[0]){
        printf("ERRO - winxMenu[0]\n");
        return false;
    }
    winxMenu[1] = al_load_bitmap("fotinhas/bonecasMenu/1.png");
    if(!winxMenu[1]){
        printf("ERRO - winxMenu[1]\n");
        return false;
    }
    winxMenu[2] = al_load_bitmap("fotinhas/bonecasMenu/2.png");
    if(!winxMenu[2]){
        printf("ERRO - winxMenu[2]\n");
        return false;
    }
    winxMenu[3] = al_load_bitmap("fotinhas/bonecasMenu/3.png");
    if(!winxMenu[3]){
        printf("ERRO - winxMenu[3]\n");
        return false;
    }
    winxMenu[4] = al_load_bitmap("fotinhas/bonecasMenu/4.png");
    if(!winxMenu[4]){
        printf("ERRO - winxMenu[4]\n");
        return false;
    }
    winxMenu[5] = al_load_bitmap("fotinhas/bonecasMenu/5.png");
    if(!winxMenu[5]){
        printf("ERRO - winxMenu[5]\n");
        return false;
    }

    winxMenuCinza = (ALLEGRO_BITMAP **) malloc(MAX_CLIENT*sizeof(ALLEGRO_BITMAP *));
    if(winxMenuCinza == NULL){
        printf("Erro ao alocar winxMenuCinza\n");
        return false;
    }
    winxMenuCinza[0] = al_load_bitmap("fotinhas/bonecasMenu/0_pb.png");
    if(!winxMenuCinza[0]){
        printf("ERRO - winxMenuCinza[0]\n");
        return false;
    }
    winxMenuCinza[1] = al_load_bitmap("fotinhas/bonecasMenu/1_pb.png");
    if(!winxMenuCinza[1]){
        printf("ERRO - winxMenuCinza[1]\n");
        return false;
    }
    winxMenuCinza[2] = al_load_bitmap("fotinhas/bonecasMenu/2_pb.png");
    if(!winxMenuCinza[2]){
        printf("ERRO - winxMenuCinza[2]\n");
        return false;
    }
    winxMenuCinza[3] = al_load_bitmap("fotinhas/bonecasMenu/3_pb.png");
    if(!winxMenuCinza[3]){
        printf("ERRO - winxMenuCinza[3]\n");
        return false;
    }
    winxMenuCinza[4] = al_load_bitmap("fotinhas/bonecasMenu/4_pb.png");
    if(!winxMenuCinza[4]){
        printf("ERRO - winxMenuCinza[4]\n");
        return false;
    }
    winxMenuCinza[5] = al_load_bitmap("fotinhas/bonecasMenu/5_pb.png");
    if(!winxMenuCinza[5]){
        printf("ERRO - winxMenuCinza[5]\n");
        return false;
    }
    
    personagem = (ALLEGRO_BITMAP **) malloc(MAX_CLIENT*sizeof(ALLEGRO_BITMAP*));
    if(personagem == NULL){
        printf("Erro ao alocar personagem\n");
        return false;
    }
    personagem[0] = al_load_bitmap("fotinhas/bonecasJogo/0_anima.png");
    if(!personagem[0]){
        printf("ERRO - personagem[0]\n");
        return false;
    }
    personagem[1] = al_load_bitmap("fotinhas/bonecasJogo/1_anima.png");
    if(!personagem[1]){
        printf("ERRO - personagem[1]\n");
        return false;
    }
    personagem[2] = al_load_bitmap("fotinhas/bonecasJogo/2_anima.png");
    if(!personagem[2]){
        printf("ERRO - personagem[2]\n");
        return false;
    }
    personagem[3] = al_load_bitmap("fotinhas/bonecasJogo/3_anima.png");
    if(!personagem[3]){
        printf("ERRO - personagem[3]\n");
        return false;
    }
    personagem[4] = al_load_bitmap("fotinhas/bonecasJogo/4_anima.png");
    if(!personagem[4]){
        printf("ERRO - personagem[4]\n");
        return false;
    }
    personagem[5] = al_load_bitmap("fotinhas/bonecasJogo/5_anima.png");
    if(!personagem[5]){
        printf("ERRO - personagem[5]\n");
        return false;
    }
    fundoTempo = al_load_bitmap("fotinhas/pontuacao/fundoTempo.png");
    if(!fundoTempo){
        printf("ERRO - fundoTempo\n");
        return false;
    }
    fundoPont = al_load_bitmap("fotinhas/pontuacao/fundo1.png");
    if(!fundoPont){
        printf("ERRO - FundoPontos\n");
        return false;
    }
    fundoPont2 = al_load_bitmap("fotinhas/pontuacao/pontosNome.png");
    if(!fundoPont2){
        printf("ERRO - PontosNome\n");
        return false;
    }
    winxPont = (ALLEGRO_BITMAP **) malloc (MAX_CLIENT*sizeof(ALLEGRO_BITMAP*));
    if(winxPont==NULL){
        printf("Erro ao alocar winxPont\n");
        return false;
    }
    winxPont[0] = al_load_bitmap("fotinhas/pontuacao/0_pont.png");
    if(!winxPont[0]){
        printf("ERRO - winxPont[0]\n");
        return false;
    }
    winxPont[1] = al_load_bitmap("fotinhas/pontuacao/1_pont.png");
    if(!winxPont[1]){
        printf("ERRO - winxPont[1]\n");
        return false;
    }
    winxPont[2] = al_load_bitmap("fotinhas/pontuacao/2_pont.png");
    if(!winxPont[2]){
        printf("ERRO - winxPont[2]\n");
        return false;
    }
    winxPont[3] = al_load_bitmap("fotinhas/pontuacao/3_pont.png");
    if(!winxPont[3]){
        printf("ERRO - winxPont[3]\n");
        return false;
    }
    winxPont[4] = al_load_bitmap("fotinhas/pontuacao/4_pont.png");
    if(!winxPont[4]){
        printf("ERRO - winxPont[4]\n");
        return false;
    }
    winxPont[5] = al_load_bitmap("fotinhas/pontuacao/5_pont.png");
    if(!winxPont[5]){
        printf("ERRO - winxPont[5]\n");
        return false;
    }
    machos = (ALLEGRO_BITMAP **) malloc (4*sizeof(ALLEGRO_BITMAP*));
    if(machos==NULL){
        printf("Erro ao alocar machos\n");
        return false;
    }
    machos[0] = al_load_bitmap("fotinhas/homens/0_macho.png");
    if(!machos[0]){
        printf("ERRO - machos[0]\n");
        return false;
    }
    machos[1] = al_load_bitmap("fotinhas/homens/1_macho.png");
    if(!machos[1]){
        printf("ERRO - machos[1]\n");
        return false;
    }
    machos[2] = al_load_bitmap("fotinhas/homens/2_macho.png");
    if(!machos[2]){
        printf("ERRO - machos[2]\n");
        return false;
    }
    machos[3] = al_load_bitmap("fotinhas/homens/3_macho.png");
    if(!machos[3]){
        printf("ERRO - machos[3]\n");
        return false;
    }
    machosMortos = (ALLEGRO_BITMAP **) malloc (4*sizeof(ALLEGRO_BITMAP*));
    if(machosMortos==NULL){
        printf("Erro ao alocar machosMortos\n");
        return false;
    }
    machosMortos[0] = al_load_bitmap("fotinhas/homens/0_morto.png");
    if(!machosMortos[0]){
        printf("ERRO - machosMortos[0]\n");
        return false;
    }
    machosMortos[1] = al_load_bitmap("fotinhas/homens/1_morto.png");
    if(!machosMortos[1]){
        printf("ERRO - machosMortos[1]\n");
        return false;
    }
    machosMortos[2] = al_load_bitmap("fotinhas/homens/2_morto.png");
    if(!machosMortos[2]){
        printf("ERRO - machosMortos[2]\n");
        return false;
    }
    machosMortos[3] = al_load_bitmap("fotinhas/homens/3_morto.png");
    if(!machosMortos[3]){
        printf("ERRO - machosMortos[3]\n");
        return false;
    }
    cristais = (ALLEGRO_BITMAP **) malloc (3*sizeof(ALLEGRO_BITMAP*));
    if(cristais==NULL){
        printf("Erro ao alocar cristais\n");
        return false;
    }
    cristais[0] = al_load_bitmap("fotinhas/cristais/0_cristais.png");
    if(!cristais[0]){
        printf("ERRO - cristais[0]\n");
        return false;
    }
    cristais[1] = al_load_bitmap("fotinhas/cristais/1_cristais.png");
    if(!cristais[1]){
        printf("ERRO - cristais[1]\n");
        return false;
    }
    cristais[2] = al_load_bitmap("fotinhas/cristais/2_cristais.png");
    if(!cristais[2]){
        printf("ERRO - cristais[2]\n");
        return false;
    }
    medalha = (ALLEGRO_BITMAP **) malloc (3*sizeof(ALLEGRO_BITMAP*));
    if(medalha==NULL){
        printf("Erro ao alocar medalha\n");
        return false;
    }
    medalha[0] = al_load_bitmap("fotinhas/ranking/0_medalha.png");
    if(!medalha[0]){
        printf("ERRO - medalha[0]\n");
        return false;
    }
    medalha[1] = al_load_bitmap("fotinhas/ranking/1_medalha.png");
    if(!medalha[1]){
        printf("ERRO - medalha[1]\n");
        return false;
    }
    medalha[2] = al_load_bitmap("fotinhas/ranking/2_medalha.png");
    if(!medalha[2]){
        printf("ERRO - medalha[2]\n");
        return false;
    }
    fundoRanking = al_load_bitmap("fotinhas/ranking/fundoRanking.png");
    if(!fundoRanking){
        printf("ERRO - fundoRanking\n");
        return false;
    }
    winxRanking = (ALLEGRO_BITMAP **) malloc(MAX_CLIENT*sizeof(ALLEGRO_BITMAP*));
    if(winxRanking == NULL){
        printf("Erro ao alocar winxRanking\n");
        return false;
    }
    winxRanking[0] = al_load_bitmap("fotinhas/ranking/0_ranking.png");
    if(!winxRanking[0]){
        printf("ERRO - winxRanking[0]\n");
        return false;
    }
    winxRanking[1] = al_load_bitmap("fotinhas/ranking/1_ranking.png");
    if(!winxRanking[1]){
        printf("ERRO - winxRanking[1]\n");
        return false;
    }
    winxRanking[2] = al_load_bitmap("fotinhas/ranking/2_ranking.png");
    if(!winxRanking[2]){
        printf("ERRO - winxRanking[2]\n");
        return false;
    }
    winxRanking[3] = al_load_bitmap("fotinhas/ranking/3_ranking.png");
    if(!winxRanking[3]){
        printf("ERRO - winxRanking[3]\n");
        return false;
    }
    winxRanking[4] = al_load_bitmap("fotinhas/ranking/4_ranking.png");
    if(!winxRanking[4]){
        printf("ERRO - winxRanking[4]\n");
        return false;
    }
    winxRanking[5] = al_load_bitmap("fotinhas/ranking/5_ranking.png");
    if(!winxRanking[5]){
        printf("ERRO - winxRanking[5]\n");
        return false;
    }

    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        fprintf(stderr, "failed to create timer!\n");
        return false;
    }

    if (!al_install_mouse()){
        fprintf(stderr, "Falha ao inicializar o mouse.\n");
        al_destroy_display(janela);
        return false;
    }

    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)){
        fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
        al_destroy_display(janela);
        return false;
    }

    if (!al_install_keyboard()){
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return false;
    }

    al_set_window_title(janela, "ManDown");
    al_set_display_icon(janela, winxPont[4]);
    al_register_event_source(filaEventos, al_get_keyboard_event_source());
    al_register_event_source(filaEventos, al_get_display_event_source(janela));
    al_register_event_source(filaEventos, al_get_timer_event_source(timer));
    al_register_event_source(filaEventos, al_get_mouse_event_source());
    al_register_event_source(filaCronometro, al_get_timer_event_source(timerAnima));

   return true;
}

void libera(){
    al_destroy_bitmap(play1);
    al_destroy_bitmap(play2);
    al_destroy_bitmap(inicio);
    al_destroy_bitmap(inicio2);
    al_destroy_bitmap(mapa);
    al_destroy_bitmap(fundoPont);
    al_destroy_bitmap(fundoPont2);
    al_destroy_bitmap(fundoInicio);
    al_destroy_bitmap(fundoMenu);
    al_destroy_bitmap(fundoTempo);
    al_destroy_bitmap(fim);
    al_destroy_bitmap(fundoMenu2);
    al_destroy_bitmap(fundo);
    al_destroy_bitmap(fundoIP);
    al_destroy_bitmap(regras);
    al_destroy_bitmap(escolhaPers);
    al_destroy_bitmap(escolhaPers2);
    al_destroy_bitmap(sairBotao);
    al_destroy_bitmap(sairBotao2);
    al_destroy_bitmap(sairRank);
    al_destroy_bitmap(sairRank2);
    al_destroy_bitmap(playAgain);
    al_destroy_bitmap(playAgain2);
    al_destroy_bitmap(voltar);
    al_destroy_bitmap(voltar2);
    al_destroy_bitmap(regrasBotao);
    al_destroy_bitmap(regrasBotao2);
    al_destroy_font(fonte);
    al_destroy_font(fonteIP);
    al_destroy_font(fonteTempo);
    for(i=0;i<MAX_CLIENT;i++) al_destroy_bitmap(personagem[i]);
    free(personagem);
    for(i=0;i<MAX_CLIENT;i++) al_destroy_bitmap(winxMenu[i]);
    free(winxMenu);
    for(i=0;i<MAX_CLIENT;i++) al_destroy_bitmap(winxMenuCinza[i]);
    free(winxMenuCinza);
    for(i=0;i<MAX_CLIENT;i++) al_destroy_bitmap(winxPont[i]);
    free(winxPont);
    for(i=0;i<4;i++) al_destroy_bitmap(machos[i]);
    free(machos);
    for(i=0;i<4;i++) al_destroy_bitmap(machosMortos[i]);
    free(machosMortos);
    for(i=0;i<3;i++) al_destroy_bitmap(cristais[i]);
    free(cristais);
    for(i=0;i<3;i++) al_destroy_bitmap(medalha[i]);
    free(medalha);
    al_destroy_timer(timer);
    al_destroy_timer(timerAnima);
    al_destroy_event_queue(filaEventos);
    al_destroy_event_queue(filaCronometro);
    al_destroy_display(janela);
}

void printConnectScreen(char str[]){
    if(tentConectar==0) al_draw_bitmap(fundoIP, 0, 0, 0);
    else {
        al_draw_bitmap(fundoIPErro, 0, 0, 0);
    }
    if (strlen(str) > 0)
    {
        al_draw_text(fonteIP, al_map_rgb(240, 119, 178), LARGURA/2,
                     (ALTURA - al_get_font_ascent(fonteIP)) / 2,
                     ALLEGRO_ALIGN_CENTRE, str);
    }
    else
    {
        al_draw_text(fonteIP, al_map_rgb(240, 119, 178), LARGURA/2,
                     (ALTURA - al_get_font_ascent(fonteIP)) / 2,
                     ALLEGRO_ALIGN_CENTRE, "<0.0.0.0>");
    }
}

void readInput(ALLEGRO_EVENT event, char str[], int limit){
    if (event.type == ALLEGRO_EVENT_KEY_CHAR){
        if (strlen(str) <= limit){
            char temp[] = {event.keyboard.unichar, '\0'};
            if (event.keyboard.unichar == ' '){
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= '!' &&
                     event.keyboard.unichar <= '?'){
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'A' &&
                     event.keyboard.unichar <= 'Z'){
                strcat(str, temp);
            }
            else if (event.keyboard.unichar >= 'a' &&
                     event.keyboard.unichar <= 'z'){
                strcat(str, temp);
            }
        }

        if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0){
            str[strlen(str) - 1] = '\0';
        }
    }
}

void conectar(){
    bool foi = true;
    bool liberei = false;
    while(foi){
        if(!al_is_event_queue_empty(filaEventos)){
            ALLEGRO_EVENT connectEvent;
            al_wait_for_event(filaEventos, &connectEvent);
            readInput(connectEvent, SERVER_IP, 30);
            if (connectEvent.type == ALLEGRO_EVENT_KEY_DOWN){
                switch(connectEvent.keyboard.keycode){
                    case ALLEGRO_KEY_ENTER:
                        foi = false;
                        break;
                }
            }
            if(connectEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                foi = false;
                libera();
                fecharJogo = true;
            }
        }
        if(!foi) break;
        printConnectScreen(SERVER_IP);
        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }
}

void printaWinx(int id){
    printf("%d esta em [%d][%d]\n", id, posicaoWinx[id].pos.y, posicaoWinx[id].pos.x);
    switch (posicaoWinx[id].direcao){
    case 'w':
        regiao_x_folha = 0, regiao_y_folha = 197;
        break;
    case 's':
        regiao_x_folha = 0, regiao_y_folha = 0;
        break;
    case 'a':
        regiao_x_folha = 0, regiao_y_folha = 65;
        break;
    case 'd':
        regiao_x_folha = 0, regiao_y_folha = 132;
        break;
    }
    al_draw_bitmap_region(personagem[posicaoWinx[id].qualWinx], 
                                regiao_x_folha, regiao_y_folha,
                                 largura_sprite, altura_sprite,
                                 posicaoWinx[id].pos.x*64, posicaoWinx[id].pos.y*64, 0);
    
}

void desenhaPontuacao(){
    int i;
    al_draw_bitmap(fundoPont, 640, 0, 0);
    al_draw_bitmap(fundoPont2, 640, 0, 0);
    for(i=0;i<players;i++){
        switch(i){
            case 0:
                al_draw_bitmap(winxPont[posicaoWinx[i].qualWinx], 660, 81, 0);
                al_draw_textf(fonte, al_map_rgb(0, 0, 0), 702, 81, 0, "%d", pacote.pontuacao[i]);
                break;
            case 1:
                al_draw_bitmap(winxPont[posicaoWinx[i].qualWinx], 748, 81, 0);
                al_draw_textf(fonte, al_map_rgb(0, 0, 0), 790, 81, 0, "%d", pacote.pontuacao[i]);
                break;
            case 2:
                al_draw_bitmap(winxPont[posicaoWinx[i].qualWinx], 660, 118, 0);
                al_draw_textf(fonte, al_map_rgb(0, 0, 0), 702, 118, 0, "%d", pacote.pontuacao[i]);
                break;
            case 3:
                al_draw_bitmap(winxPont[posicaoWinx[i].qualWinx], 748, 118, 0);
                al_draw_textf(fonte, al_map_rgb(0, 0, 0), 790, 118, 0, "%d", pacote.pontuacao[i]);
                break;
            case 4:
                al_draw_bitmap(winxPont[posicaoWinx[i].qualWinx], 660, 155, 0);
                al_draw_textf(fonte, al_map_rgb(0, 0, 0), 702, 155, 0, "%d", pacote.pontuacao[i]);
                break;
            case 5:
                al_draw_bitmap(winxPont[posicaoWinx[i].qualWinx], 748, 155, 0);
                al_draw_textf(fonte, al_map_rgb(0, 0, 0), 790, 155, 0, "%d", pacote.pontuacao[i]);
                break;
        }
    }                       
}

void atualizarMapa(){
    int i, j;
    al_draw_bitmap(mapa, 0, 0, 0);
    desenhaPontuacao();
    al_draw_bitmap(fundoTempo, 0, 0, 0);
    al_draw_textf(fonteTempo, al_map_rgb(0, 0, 0), 64, 5, ALLEGRO_ALIGN_CENTRE, "%d:%d", minuto, segundo);
    for(i=0;i<TAM_MAPA;i++){
        for(j=0;j<TAM_MAPA;j++){
            if (matriz[j][i] == 10){
                al_draw_bitmap(machos[pacote.qualMacho], 64*i, 64*j, 0);
            }
            else if (matriz[j][i] == 20){
                if(pacote.qualCristal!=-1) al_draw_bitmap(cristais[pacote.qualCristal], 64*i, 64*j, 0);
            }
            else if(matriz[j][i] == -10){
                if(pacote.qualMachoMorto!=-1) al_draw_bitmap(machosMortos[pacote.qualMachoMorto], 64*i, 64*j, 0);
                else matriz[j][i] = 11;
            }
            else if (matriz[j][i] >= 0 && matriz[j][i] < 6){
                printaWinx(matriz[j][i]);
            }
        }
    }
}

void menu(){
    int emCima = 0, apertou = 0;
    al_draw_bitmap(fundoInicio, 0, 0, 0);
    al_draw_bitmap(inicio, 0, 0, 0);
    al_draw_bitmap(play1, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
    al_flip_display();
    al_start_timer(timerAnima); 
    i = 0;
    while (!apertou){
        ALLEGRO_EVENT evento;
        if (!al_is_event_queue_empty(filaEventos)){
            al_wait_for_event(filaEventos, &evento);
            if (evento.type == ALLEGRO_EVENT_MOUSE_AXES){
                if (evento.mouse.x >= LARGURA /  2 - al_get_bitmap_width(play2)/2 &&
                    evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(play2)/2 && 
                    evento.mouse.y >= 630 - (3*al_get_bitmap_height(play2))/4 &&
                    evento.mouse.y <= 630 + (3*al_get_bitmap_height(play2))/4){
                    if (emCima!=2){
                        emCima = 1;
                    }
                }
                else{
                    if (emCima!=-1){
                        emCima = 0;
                    }
                }
            }
            else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                if (evento.mouse.x >= LARGURA / 2 - al_get_bitmap_width(play2)/2 &&
                    evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(play2)/2 && 
                    evento.mouse.y >= 630 - (3*al_get_bitmap_height(play2))/4 &&
                    evento.mouse.y <= 630 + (3*al_get_bitmap_height(play2))/4){
                    apertou = 1;
                
                    break;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                apertou = 1;
                break;
            }
            else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                libera();
                apertou = 1;
                fecharJogo = true;
            }
        }
        if(apertou == 1) break;
        if(!al_is_event_queue_empty(filaCronometro)){
            al_wait_for_event(filaCronometro, &evento);
            if (evento.type == ALLEGRO_EVENT_TIMER){
                if (i == 0) {
                    i = 1;
                    al_draw_bitmap(fundoInicio, 0, 0, 0);
                    al_draw_bitmap(inicio, 0, 0, 0);
                    if (emCima == 2 || emCima == 1){
                        al_draw_bitmap(play2, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
                    }
                    else if (emCima == 0 || emCima == -1){
                        al_draw_bitmap(play1, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
                    }
                    al_flip_display();
                }
                else if (i == 1){
                    i = 0;
                    al_draw_bitmap(fundoInicio, 0, 0, 0);
                    al_draw_bitmap(inicio2, 0, 30, 0);
                    if (emCima == 2 || emCima == 1){
                        al_draw_bitmap(play2, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
                    }
                    else if (emCima == 0 || emCima == -1){
                        al_draw_bitmap(play1, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
                    }
                    al_flip_display();
                }
            }           
        }
        if (emCima == 1){
            emCima = 2;
            al_draw_bitmap(fundoInicio, 0, 0, 0);
            if (i == 0) al_draw_bitmap(inicio, 0, 0, 0);
            else if (i == 1) al_draw_bitmap(inicio2, 0, 30, 0);
            al_draw_bitmap(play2, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
            al_flip_display();
        }
        else if (emCima == 0){
            emCima = -1;
            al_draw_bitmap(fundoInicio, 0, 0, 0);
            if (i == 0) al_draw_bitmap(inicio, 0, 0, 0);
            else if (i == 1) al_draw_bitmap(inicio2, 0, 30, 0);
            al_draw_bitmap(play1, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
            al_flip_display();
        }
    }
    al_stop_timer(timerAnima);
}

void opcoes(int *escolha){
    al_draw_bitmap(fundoMenu2, 0, 0, 0);
    al_draw_bitmap(escolhaPers, LARGURA/2 - al_get_bitmap_width(escolhaPers)/2, 376, 0);
    al_draw_bitmap(regrasBotao, LARGURA/2 - al_get_bitmap_width(regrasBotao)/2, 512, 0);
    al_draw_bitmap(sairBotao, LARGURA/2 - al_get_bitmap_width(sairBotao)/2, 648, 0);
    al_flip_display();
    //al_rest(5.0);
    int apertou = 0;
    int emCima = 0;
    while(!apertou){
        ALLEGRO_EVENT evento;
        if(!al_is_event_queue_empty(filaEventos)){
            al_wait_for_event(filaEventos, &evento);
            if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                if(evento.mouse.x>=33 && evento.mouse.x<=799 && evento.mouse.y>=376 && evento.mouse.y<=412){
                    apertou = 1;
                    (*escolha) = 0;
                }
                else if(evento.mouse.x>=306 && evento.mouse.x<=526 && evento.mouse.y>=512 && evento.mouse.y<=548){
                    apertou = 1;
                    (*escolha) = 1;
                }
                else if(evento.mouse.x>=356 && evento.mouse.x<=476 && evento.mouse.y>=648 && evento.mouse.y<=684){
                    apertou = 1;
                    (*escolha) = 2;
                    libera();
                    fecharJogo = true;
                    break;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_MOUSE_AXES){
                if(evento.mouse.x>=33 && evento.mouse.x<=799 && evento.mouse.y>=376 && evento.mouse.y<=412){
                    emCima = 0;
                }
                else if(evento.mouse.x>=306 && evento.mouse.x<=526 && evento.mouse.y>=512 && evento.mouse.y<=548){
                    emCima = 1;
                }
                else if(evento.mouse.x>=356 && evento.mouse.x<=476 && evento.mouse.y>=648 && evento.mouse.y<=684){
                    emCima = 2;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                switch (evento.keyboard.keycode)
                {
                case ALLEGRO_KEY_DOWN:
                    if(emCima<2) emCima++;
                    break;
                case ALLEGRO_KEY_UP:
                    if(emCima>0) emCima--;
                    break;
                case ALLEGRO_KEY_ENTER:
                    switch (emCima)
                    {
                    case 0:
                        (*escolha) = 0;
                        apertou = 1;
                        break;
                    case 1:
                        (*escolha) = 1;
                        apertou = 1;
                        break;
                    case 2:
                        apertou = 1;
                        libera();
                        fecharJogo = true;
                        break;
                    }
                    break;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                printf("oi\n");
                libera();
                apertou = 1;
                fecharJogo = true;
                break;
            }
        }
        if(apertou == 1) break;
        switch (emCima)
        {
        case 0:
            al_draw_bitmap(fundoMenu2, 0, 0, 0);
            al_draw_bitmap(escolhaPers2, LARGURA/2 - al_get_bitmap_width(escolhaPers2)/2, 376, 0);
            al_draw_bitmap(regrasBotao, LARGURA/2 - al_get_bitmap_width(regrasBotao)/2, 512, 0);
            al_draw_bitmap(sairBotao, LARGURA/2 - al_get_bitmap_width(sairBotao)/2, 648, 0);
            al_flip_display();
            break;
        case 1:
            al_draw_bitmap(fundoMenu2, 0, 0, 0);
            al_draw_bitmap(escolhaPers, LARGURA/2 - al_get_bitmap_width(escolhaPers)/2, 376, 0);
            al_draw_bitmap(regrasBotao2, LARGURA/2 - al_get_bitmap_width(regrasBotao2)/2, 512, 0);
            al_draw_bitmap(sairBotao, LARGURA/2 - al_get_bitmap_width(sairBotao)/2, 648, 0);
            al_flip_display();
            break;
        case 2:
            al_draw_bitmap(fundoMenu2, 0, 0, 0);
            al_draw_bitmap(escolhaPers, LARGURA/2 - al_get_bitmap_width(escolhaPers)/2, 376, 0);
            al_draw_bitmap(regrasBotao, LARGURA/2 - al_get_bitmap_width(regrasBotao)/2, 512, 0);
            al_draw_bitmap(sairBotao2, LARGURA/2 - al_get_bitmap_width(sairBotao2)/2, 648, 0);
            al_flip_display();
            break;
        }
    }

}

void mostraRegras(){
    al_draw_bitmap(regras, 0, 0, 0);
    al_draw_bitmap(voltar, 10, 10, 0);
    al_flip_display();
    int apertou = 0;
    int emCima = 0;
    while(!apertou){
        ALLEGRO_EVENT evento;
        if(!al_is_event_queue_empty(filaEventos)){
            al_wait_for_event(filaEventos, &evento);
            if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                if(evento.mouse.x>=10 && evento.mouse.x<=162 && evento.mouse.y>=10 && evento.mouse.y<=35){
                    apertou = 1;
                } 
            }
            else if(evento.type == ALLEGRO_EVENT_MOUSE_AXES){
                if(evento.mouse.x>=10 && evento.mouse.x<=162 && evento.mouse.y>=10 && evento.mouse.y<=35){
                    emCima = 1;
                }
                else emCima = 0; 
            }
            else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
                    apertou = 1;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                apertou = 1;
                libera();
                fecharJogo = true;
            }
        }
        if(apertou == 1) break;
        switch (emCima)
        {
        case 0:
            al_draw_bitmap(regras, 0, 0, 0);
            al_draw_bitmap(voltar, 10, 10, 0);
            al_flip_display();
            break;
        case 1:
            al_draw_bitmap(regras, 0, 0, 0);
            al_draw_bitmap(voltar2, 10, 10, 0);
            al_flip_display();
            break;
        }
    } 
}

void menuPersonagem(){
    char msg='A';
    sendMsgToServer(&msg, sizeof(char));
    int *escolhidas = NULL;
    escolhidas = (int *) malloc(MAX_CLIENT*sizeof(int));
    if (escolhidas == NULL) {
        printf("Erro ao alocar escolhidas\n"); 
        exit(0);
    }
    recvMsgFromServer(escolhidas, WAIT_FOR_IT);
    for(i=0;i<MAX_CLIENT;i++){
        if (escolhidas[i] == 0) auxWinxMenu[i] = winxMenu[i];
        else auxWinxMenu[i] = winxMenuCinza[i];
    }
    free(escolhidas);
    al_draw_bitmap(fundoMenu, 0, 0, 0);
    al_draw_bitmap(auxWinxMenu[0], 58, 282, 0);
    al_draw_bitmap(auxWinxMenu[1], 316, 282, 0);
    al_draw_bitmap(auxWinxMenu[2], 574, 282, 0);
    al_draw_bitmap(auxWinxMenu[3], 58, 557, 0);
    al_draw_bitmap(auxWinxMenu[4], 316, 557, 0);
    al_draw_bitmap(auxWinxMenu[5], 574, 557, 0);
    al_flip_display();
    //al_rest(1.0);
    
    int apertaWinx = 0, emCima=0;
    char aux2;
    while(!apertaWinx){
        ALLEGRO_EVENT evento;
        if (!al_is_event_queue_empty(filaEventos)){
            al_wait_for_event(filaEventos, &evento);
            if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                switch(evento.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        if(emCima>=3 && emCima<=5) emCima-=3;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        if(emCima>=0 && emCima<=2) emCima+=3;
                        break;
                    case ALLEGRO_KEY_LEFT: 
                        if(emCima>0) emCima--;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        if(emCima<5) emCima++;
                        break;
                    case ALLEGRO_KEY_ENTER:
                        switch (emCima)
                        {
                        case 0:
                            msg = 0;
                            sendMsgToServer(&msg, sizeof(char));
                            recvMsgFromServer(&msg, WAIT_FOR_IT);
                            if(msg == 11){
                                apertaWinx = 1;
                            }
                            break;
                        case 1:
                            msg = 1;
                            sendMsgToServer(&msg, sizeof(char));
                            recvMsgFromServer(&msg, WAIT_FOR_IT);
                            if(msg == 11){
                                apertaWinx = 1;
                            }
                            break;
                        case 2:
                            msg = 2;
                            sendMsgToServer(&msg, sizeof(char));
                            recvMsgFromServer(&msg, WAIT_FOR_IT);
                            if(msg == 11){
                                apertaWinx = 1;
                            }
                            break;
                        case 3:
                            msg = 3;
                            sendMsgToServer(&msg, sizeof(char));
                            recvMsgFromServer(&msg, WAIT_FOR_IT);
                            if(msg == 11){
                                apertaWinx = 1;
                            }
                            break;
                        case 4:
                            msg = 4;
                            sendMsgToServer(&msg, sizeof(char));
                            recvMsgFromServer(&msg, WAIT_FOR_IT);
                            if(msg == 11){
                                apertaWinx = 1;
                            }
                            break;
                        case 5:
                            msg = 5;
                            sendMsgToServer(&msg, sizeof(char));
                            recvMsgFromServer(&msg, WAIT_FOR_IT);
                            if(msg == 11){
                                apertaWinx = 1;
                            }
                            break;
                        }
                        break;
                }
            }else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES){
                if (evento.mouse.x >= 58 && evento.mouse.x <= 258 && evento.mouse.y >= 282 && evento.mouse.y <= 482){
                    emCima=0;
					
                }
                else if (evento.mouse.x >= 316 && evento.mouse.x <= 516 && evento.mouse.y >= 282 && evento.mouse.y <= 482){
                        emCima=1;
                }
                else if (evento.mouse.x >= 574 && evento.mouse.x <= 774 && evento.mouse.y >= 282 && evento.mouse.y <= 482){
                        emCima=2;
                }
                else if (evento.mouse.x >= 58 && evento.mouse.x <= 258 && evento.mouse.y >= 557 && evento.mouse.y <= 757){
                        emCima=3;
                }
                else if (evento.mouse.x >= 316 && evento.mouse.x <= 516 && evento.mouse.y >= 557 && evento.mouse.y <= 757){
                        emCima=4;
                }
                else if (evento.mouse.x >= 574 && evento.mouse.x <= 774 && evento.mouse.y >= 557 && evento.mouse.y <= 757){
                        emCima=5;
                }
            } else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                if (evento.mouse.x >= 58 && evento.mouse.x <= 258 && evento.mouse.y >= 282 && evento.mouse.y <= 482){
                    msg = 0;
                    sendMsgToServer(&msg, sizeof(char));
					recvMsgFromServer(&msg, WAIT_FOR_IT);
					if(msg == 11){
                        apertaWinx = 1;
                        break;
                    }
					
                }
                else if (evento.mouse.x >= 316 && evento.mouse.x <= 516 && evento.mouse.y >= 282 && evento.mouse.y <= 482){
                        msg = 1;
                        sendMsgToServer(&msg, sizeof(char));
					    recvMsgFromServer(&msg, WAIT_FOR_IT);
					    if(msg == 11){
                            apertaWinx=1;
                            break;
                        }
                }
                else if (evento.mouse.x >= 574 && evento.mouse.x <= 774 && evento.mouse.y >= 282 && evento.mouse.y <= 482){
                        msg = 2;
                        sendMsgToServer(&msg, sizeof(char));
					    recvMsgFromServer(&msg, WAIT_FOR_IT);
					    if(msg == 11){
                            apertaWinx = 1;;
                            break;
                        }
                }
                else if (evento.mouse.x >= 58 && evento.mouse.x <= 258 && evento.mouse.y >= 557 && evento.mouse.y <= 757){
                        msg=3;
                        sendMsgToServer(&msg, sizeof(char));
                        recvMsgFromServer(&msg, WAIT_FOR_IT);
                        if(msg == 11){
                            apertaWinx = 1;;
                            break;
                        }
                }
                else if (evento.mouse.x >= 316 && evento.mouse.x <= 516 && evento.mouse.y >= 557 && evento.mouse.y <= 757){
                        msg = 4;
                        sendMsgToServer(&msg, sizeof(char));
                        recvMsgFromServer(&msg, WAIT_FOR_IT);
                        if(msg == 11){
                            apertaWinx = 1;;
                            break;
                        }
                }
                else if (evento.mouse.x >= 574 && evento.mouse.x <= 774 && evento.mouse.y >= 557 && evento.mouse.y <= 757){
                        msg=5;
                        sendMsgToServer(&msg, sizeof(char));
                        recvMsgFromServer(&msg, WAIT_FOR_IT);
                        if(msg == 11){
                            apertaWinx = 1;;
                            break;
                        }
                }
            } else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                libera();
                fecharJogo = true;
                apertaWinx = 1;
                break;
            }
        }
        if(apertaWinx == 1) break;
        if (recvMsgFromServer(&aux2, DONT_WAIT) != NO_MESSAGE){
            if((int)aux2>=0 && (int)aux2<MAX_CLIENT){
                auxWinxMenu[(int) aux2] = winxMenuCinza[(int) aux2];
                al_draw_bitmap(fundoMenu, 0, 0, 0);
                al_draw_bitmap(auxWinxMenu[0], 58, 282, 0);
                al_draw_bitmap(auxWinxMenu[1], 316, 282, 0);
                al_draw_bitmap(auxWinxMenu[2], 574, 282, 0);
                al_draw_bitmap(auxWinxMenu[3], 58, 557, 0);
                al_draw_bitmap(auxWinxMenu[4], 316, 557, 0);
                al_draw_bitmap(auxWinxMenu[5], 574, 557, 0);
                al_flip_display();
            }
        }
        switch (emCima)
        {
        case -1:
            al_draw_bitmap(fundoMenu, 0, 0, 0);
            al_draw_bitmap(auxWinxMenu[0], 58, 282, 0);
            al_draw_bitmap(auxWinxMenu[1], 316, 282, 0);
            al_draw_bitmap(auxWinxMenu[2], 574, 282, 0);
            al_draw_bitmap(auxWinxMenu[3], 58, 557, 0);
            al_draw_bitmap(auxWinxMenu[4], 316, 557, 0);
            al_draw_bitmap(auxWinxMenu[5], 574, 557, 0);
            al_flip_display();
            break;
        
        case 0:
            al_draw_bitmap(fundoMenu, 0, 0, 0);
            al_draw_filled_rectangle(58.0, 282.0, 258.0, 482.0, al_map_rgb(240, 119, 178));
            al_draw_bitmap(auxWinxMenu[0], 58, 282, 0);
            al_draw_bitmap(auxWinxMenu[1], 316, 282, 0);
            al_draw_bitmap(auxWinxMenu[2], 574, 282, 0);
            al_draw_bitmap(auxWinxMenu[3], 58, 557, 0);
            al_draw_bitmap(auxWinxMenu[4], 316, 557, 0);
            al_draw_bitmap(auxWinxMenu[5], 574, 557, 0);
            al_flip_display();
            break;
        case 1:
            al_draw_bitmap(fundoMenu, 0, 0, 0);
            al_draw_filled_rectangle(316.0, 282.0, 516.0, 482.0, al_map_rgb(240, 119, 178));
            al_draw_bitmap(auxWinxMenu[0], 58, 282, 0);
            al_draw_bitmap(auxWinxMenu[1], 316, 282, 0);
            al_draw_bitmap(auxWinxMenu[2], 574, 282, 0);
            al_draw_bitmap(auxWinxMenu[3], 58, 557, 0);
            al_draw_bitmap(auxWinxMenu[4], 316, 557, 0);
            al_draw_bitmap(auxWinxMenu[5], 574, 557, 0);
            al_flip_display();
            break;
        case 2:
            al_draw_bitmap(fundoMenu, 0, 0, 0);
            al_draw_filled_rectangle(574.0, 282.0, 774.0, 482.0, al_map_rgb(240, 119, 178));
            al_draw_bitmap(auxWinxMenu[0], 58, 282, 0);
            al_draw_bitmap(auxWinxMenu[1], 316, 282, 0);
            al_draw_bitmap(auxWinxMenu[2], 574, 282, 0);
            al_draw_bitmap(auxWinxMenu[3], 58, 557, 0);
            al_draw_bitmap(auxWinxMenu[4], 316, 557, 0);
            al_draw_bitmap(auxWinxMenu[5], 574, 557, 0);
            al_flip_display();
            break;
        case 3:
            al_draw_bitmap(fundoMenu, 0, 0, 0);
            al_draw_filled_rectangle(58.0, 557.0, 258.0, 757.0, al_map_rgb(240, 119, 178));
            al_draw_bitmap(auxWinxMenu[0], 58, 282, 0);
            al_draw_bitmap(auxWinxMenu[1], 316, 282, 0);
            al_draw_bitmap(auxWinxMenu[2], 574, 282, 0);
            al_draw_bitmap(auxWinxMenu[3], 58, 557, 0);
            al_draw_bitmap(auxWinxMenu[4], 316, 557, 0);
            al_draw_bitmap(auxWinxMenu[5], 574, 557, 0);
            al_flip_display();
            break;
        case 4:
            al_draw_bitmap(fundoMenu, 0, 0, 0);
            al_draw_filled_rectangle(316.0, 557.0, 516.0, 757.0, al_map_rgb(240, 119, 178));
            al_draw_bitmap(auxWinxMenu[0], 58, 282, 0);
            al_draw_bitmap(auxWinxMenu[1], 316, 282, 0);
            al_draw_bitmap(auxWinxMenu[2], 574, 282, 0);
            al_draw_bitmap(auxWinxMenu[3], 58, 557, 0);
            al_draw_bitmap(auxWinxMenu[4], 316, 557, 0);
            al_draw_bitmap(auxWinxMenu[5], 574, 557, 0);
            al_flip_display();
            break;
        case 5:
            al_draw_bitmap(fundoMenu, 0, 0, 0);
            al_draw_filled_rectangle(574.0, 557.0, 774.0, 757.0, al_map_rgb(240, 119, 178));
            al_draw_bitmap(auxWinxMenu[0], 58, 282, 0);
            al_draw_bitmap(auxWinxMenu[1], 316, 282, 0);
            al_draw_bitmap(auxWinxMenu[2], 574, 282, 0);
            al_draw_bitmap(auxWinxMenu[3], 58, 557, 0);
            al_draw_bitmap(auxWinxMenu[4], 316, 557, 0);
            al_draw_bitmap(auxWinxMenu[5], 574, 557, 0);
            al_flip_display();
            break;

        }

    }
}

void confirmaInicio(){
    int emCima = 0, apertou = 0;
    char msg;
    al_draw_bitmap(fundoComecar, 0, 0, 0);
    al_draw_bitmap(inicio, 0, 0, 0);
    al_draw_bitmap(play1, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
    al_flip_display();
    al_start_timer(timerAnima); 
    i = 0;
    while (!apertou){
        if(recvMsgFromServer(&msg, DONT_WAIT) != NO_MESSAGE){
            if(msg=='y'){
                apertou=1;
                break;
            }
        }
        if(apertou == 1) break;
        ALLEGRO_EVENT evento;
        if (!al_is_event_queue_empty(filaEventos)){
            al_wait_for_event(filaEventos, &evento);
            if (evento.type == ALLEGRO_EVENT_MOUSE_AXES){
                if (evento.mouse.x >= LARGURA /  2 - al_get_bitmap_width(play2)/2 &&
                    evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(play2)/2 && 
                    evento.mouse.y >= 630 - (3*al_get_bitmap_height(play2))/4 &&
                    evento.mouse.y <= 630 + (3*al_get_bitmap_height(play2))/4){
                    if (emCima!=2){
                        emCima = 1;
                    }
                }
                else{
                    if (emCima!=-1){
                        emCima = 0;
                    }
                }
            }
            else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                if (evento.mouse.x >= LARGURA / 2 - al_get_bitmap_width(play2)/2 &&
                    evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(play2)/2 && 
                    evento.mouse.y >= 630 - (3*al_get_bitmap_height(play2))/4 &&
                    evento.mouse.y <= 630 + (3*al_get_bitmap_height(play2))/4){
                    msg='y';
                    sendMsgToServer(&msg, sizeof(char));
                    recvMsgFromServer(&msg, WAIT_FOR_IT);
                    if(msg=='y'){
                        apertou=1;
                        break;
                    }
                }
            }
            else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                msg='y';
                sendMsgToServer(&msg, sizeof(char));
                recvMsgFromServer(&msg, WAIT_FOR_IT);
                if(msg=='y'){
                    apertou=1;
                    break;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                libera();
                apertou = 1;
                fecharJogo = true;
            }
        }
        if(apertou == 1) break;
        if(!al_is_event_queue_empty(filaCronometro)){
            al_wait_for_event(filaCronometro, &evento);
            if (evento.type == ALLEGRO_EVENT_TIMER){
                if (i == 0) {
                    i = 1;
                    al_draw_bitmap(fundoComecar, 0, 0, 0);
                    al_draw_bitmap(inicio, 0, 0, 0);
                    if (emCima == 2 || emCima == 1){
                        al_draw_bitmap(play2, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
                    }
                    else if (emCima == 0 || emCima == -1){
                        al_draw_bitmap(play1, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
                    }
                    al_flip_display();
                }
                else if (i == 1){
                    i = 0;
                    al_draw_bitmap(fundoComecar, 0, 0, 0);
                    al_draw_bitmap(inicio2, 0, 30, 0);
                    if (emCima == 2 || emCima == 1){
                        al_draw_bitmap(play2, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
                    }
                    else if (emCima == 0 || emCima == -1){
                        al_draw_bitmap(play1, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
                    }
                    al_flip_display();
                }
            }           
        }
        if (emCima == 1){
            emCima = 2;
            al_draw_bitmap(fundoComecar, 0, 0, 0);
            if (i == 0) al_draw_bitmap(inicio, 0, 0, 0);
            else if (i == 1) al_draw_bitmap(inicio2, 0, 30, 0);
            al_draw_bitmap(play2, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
            al_flip_display();
        }
        else if (emCima == 0){
            emCima = -1;
            al_draw_bitmap(fundoComecar, 0, 0, 0);
            if (i == 0) al_draw_bitmap(inicio, 0, 0, 0);
            else if (i == 1) al_draw_bitmap(inicio2, 0, 30, 0);
            al_draw_bitmap(play1, LARGURA / 2 - al_get_bitmap_width(play1)/2, 630 - (3*al_get_bitmap_height(play1))/4, 0);
            al_flip_display();
        }
    }
    al_stop_timer(timerAnima);
}

void game_over(){
    char msg = 'e';
    sendMsgToServer(&msg, sizeof(char));
    al_draw_bitmap(fim, 0, 0, 0);
    al_flip_display();
    Final ranking[players];
    for(i=0;i<players;i++) {
        ranking[i].pont = pacote.pontuacao[i];
        ranking[i].qualWinx = pacote.qualWinx[i];
    }
    for(j=0;j<players-1;j++){
        for(i=0;i<players-1;i++){
            if(ranking[i].pont<ranking[i+1].pont){
                Final aux = ranking[i];
                ranking[i] = ranking[i+1];
                ranking[i+1] = aux;
            }
        }
    }
    al_draw_bitmap(fundoRanking, 0, 0, 0);
    for(i=0;i<players;i++){
        switch (i)
        {
        case 0:
            al_draw_bitmap(winxRanking[ranking[i].qualWinx], 21, 160, 0);
            al_draw_bitmap(medalha[0], 21, 160, 0);
            break;
        case 1:
            al_draw_bitmap(winxRanking[ranking[i].qualWinx], 291, 160, 0);
            al_draw_bitmap(medalha[1], 291, 160, 0);
            break;
        case 2:
            al_draw_bitmap(winxRanking[ranking[i].qualWinx], 561, 160, 0);
            al_draw_bitmap(medalha[2], 561, 160, 0);
            break;
        case 3:
            al_draw_bitmap(winxRanking[ranking[i].qualWinx], 21, 430, 0);
            break;
        case 4:
            al_draw_bitmap(winxRanking[ranking[i].qualWinx], 291, 430, 0);
            break;
        case 5:
            al_draw_bitmap(winxRanking[ranking[i].qualWinx], 561, 430, 0);
            break;
        }
    }
    al_draw_bitmap(playAgain, 10, 786, 0);
    al_draw_bitmap(sairRank, 739, 786, 0);
    //al_rest(2.0);
    al_flip_display();
    int apertou = 0;
    int emCima = 0;
    while(!apertou){
        ALLEGRO_EVENT evento;
        if(!al_event_queue_is_empty(filaEventos)){
            al_wait_for_event(filaEventos, &evento);
            if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                if(evento.mouse.x>=10 && evento.mouse.x<=329 && evento.mouse.y>=786 && evento.mouse.y<=811){
                    msg = 'p'; sendMsgToServer(&msg, sizeof(char));
                    apertou = 1;
                }
                else if(evento.mouse.x>=739 && evento.mouse.x<=822 && evento.mouse.y>=786 && evento.mouse.y<=811){
                    msg = 's'; sendMsgToServer(&msg, sizeof(char));
                    apertou = 1;
                    libera();
                    fecharJogo = true;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_MOUSE_AXES){
                if(evento.mouse.x>=10 && evento.mouse.x<=329 && evento.mouse.y>=786 && evento.mouse.y<=811){
                    emCima = 0;
                }
                else if(evento.mouse.x>=739 && evento.mouse.x<=822 && evento.mouse.y>=786 && evento.mouse.y<=811){
                    emCima = 1;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                switch(evento.keyboard.keycode){
                    case ALLEGRO_KEY_RIGHT:
                        if(emCima==0) emCima++;
                        break;
                    case ALLEGRO_KEY_LEFT:
                        if(emCima==1) emCima--;
                        break;
                    case ALLEGRO_KEY_ENTER:
                        if(emCima == 0){
                            msg = 'p'; sendMsgToServer(&msg, sizeof(char));
                            apertou = 1;
                        }
                        else if(emCima == 1){
                            msg = 's'; sendMsgToServer(&msg, sizeof(char));
                            apertou = 1;
                            libera();
                            fecharJogo = true;
                        }
                        break;
                }
            }
            else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                apertou = 1;
                libera();
                fecharJogo = true;
            }
        }
        if(apertou == 1) break;
        switch(emCima){
            case 0:
                al_draw_bitmap(fundoRanking, 0, 0, 0);
                for(i=0;i<players;i++){
                    switch (i)
                    {
                    case 0:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 21, 160, 0);
                        al_draw_bitmap(medalha[0], 21, 160, 0);
                        break;
                    case 1:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 291, 160, 0);
                        al_draw_bitmap(medalha[1], 291, 160, 0);
                        break;
                    case 2:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 561, 160, 0);
                        al_draw_bitmap(medalha[2], 561, 160, 0);
                        break;
                    case 3:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 21, 430, 0);
                        break;
                    case 4:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 291, 430, 0);
                        break;
                    case 5:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 561, 430, 0);
                        break;
                    }
                }
                al_draw_bitmap(playAgain2, 10, 786, 0);
                al_draw_bitmap(sairRank, 739, 786, 0);
                al_flip_display();
                break;
            case 1:
                al_draw_bitmap(fundoRanking, 0, 0, 0);
                for(i=0;i<players;i++){
                    switch (i)
                    {
                    case 0:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 21, 160, 0);
                        al_draw_bitmap(medalha[0], 21, 160, 0);
                        break;
                    case 1:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 291, 160, 0);
                        al_draw_bitmap(medalha[1], 291, 160, 0);
                        break;
                    case 2:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 561, 160, 0);
                        al_draw_bitmap(medalha[2], 561, 160, 0);
                        break;
                    case 3:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 21, 430, 0);
                        break;
                    case 4:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 291, 430, 0);
                        break;
                    case 5:
                        al_draw_bitmap(winxRanking[ranking[i].qualWinx], 561, 430, 0);
                        break;
                    }
                }
                al_draw_bitmap(playAgain, 10, 786, 0);
                al_draw_bitmap(sairRank2, 739, 786, 0);
                al_flip_display();
                break;
        }
    }
}

void game_start(){
    al_rest(1.0);
    temp = 0, segundo = 0, minuto = 0;
    sair = false;
    fecharJogo = false;
    Inicio pacoteInicial;
    recvMsgFromServer(&pacoteInicial, WAIT_FOR_IT);
    pacote.qualMacho = 0; pacote.qualCristal=0; pacote.qualMachoMorto = -1;
    for(i=0;i<pacoteInicial.qtJogadores;i++){
        pacote.anda[i] = true;
        pacote.pontuacao[i] = 0;
        //y é linha, x é coluna
        switch (i){
        case 0:
            posicaoWinx[i].pos.x = 1; posicaoWinx[i].pos.y = 1;
            matriz[1][1] = 0;
            break;
        case 1:
            posicaoWinx[i].pos.x = 12; posicaoWinx[i].pos.y = 3;
            matriz[3][12] = 1;
            break;
        case 2:
            posicaoWinx[i].pos.x = 11; posicaoWinx[i].pos.y = 12;
            matriz[12][11] = 2;
            break;
        case 3:
            posicaoWinx[i].pos.x = 1; posicaoWinx[i].pos.y = 12;
            matriz[12][1] = 3;
            break;
        case 4:
            posicaoWinx[i].pos.x = 0; posicaoWinx[i].pos.y = 10;
            matriz[10][0] = 4;
            break;
        case 5:
            posicaoWinx[i].pos.x = 0; posicaoWinx[i].pos.y = 6;
            matriz[6][0] = 5;
            break;
        }
        posicaoWinx[i].direcao = 's'; posicaoWinx[i].qualWinx = pacoteInicial.winx[i];
    }
    players = pacoteInicial.qtJogadores; 
    pacote.macho.x = 0; pacote.macho.y = 1;
    pacote.cristal.x = 6; pacote.cristal.y = 6;
    matriz[1][0] = 10; matriz[6][6] = 20;
    atualizarMapa();
    al_flip_display();
    al_start_timer(timer);
    while(!sair){
        if (recvMsgFromServer(&pacote, DONT_WAIT) != NO_MESSAGE) {
            pontuacao = pacote.pontuacao[id];
            if(pacote.qualMachoMorto!=-1){
                matriz[pacote.machoMorto.y][pacote.machoMorto.x] = -10;
            }
            else matriz[pacote.machoMorto.y][pacote.machoMorto.x] = 11;
            matriz[pacote.macho.y][pacote.macho.x] = 10;
            matriz[pacote.cristal.y][pacote.cristal.x] = 20;
            for (i = 0; i < pacoteInicial.qtJogadores; i++) {
                posicaoWinx[i].pos = pacote.posicoesAtualizadas[i];
                posicaoWinx[i].direcao = pacote.dir[i];
                matriz[posicaoWinx[i].pos.y][posicaoWinx[i].pos.x] = i;
            }
            atualizarMapa();
            al_flip_display();
        }
        ALLEGRO_EVENT ev;
        if (!al_is_event_queue_empty(filaEventos)){
            al_wait_for_event(filaEventos, &ev);
            if (recvMsgFromServer(&pacote, DONT_WAIT) != NO_MESSAGE) {
                pontuacao = pacote.pontuacao[id];
                if(pacote.qualMachoMorto!=-1){
                    matriz[pacote.machoMorto.y][pacote.machoMorto.x] = -10;
                }
                else matriz[pacote.machoMorto.y][pacote.machoMorto.x] = 11;
                matriz[pacote.macho.y][pacote.macho.x] = 10;
                matriz[pacote.cristal.y][pacote.cristal.x] = 20;
                for (i = 0; i < players; i++) {
                    posicaoWinx[i].pos = pacote.posicoesAtualizadas[i];
                    posicaoWinx[i].direcao = pacote.dir[i];
                    matriz[posicaoWinx[i].pos.y][posicaoWinx[i].pos.x] = i;
                }
                atualizarMapa();
                al_flip_display();
            }
            if(ev.type == ALLEGRO_EVENT_TIMER) {
                if(key[KEY_UP]) {
                    printf("%d foi pra cima\n", id);
                    matriz[posicaoWinx[id].pos.y][posicaoWinx[id].pos.x] = 11;
                    mandarDir = 'w';
                    sendMsgToServer(&mandarDir, sizeof(char));
                }

                if(key[KEY_DOWN]) {
                    printf("%d foi pra baixo\n", id);
                    matriz[posicaoWinx[id].pos.y][posicaoWinx[id].pos.x] = 11;
                    mandarDir = 's';
                    sendMsgToServer(&mandarDir, sizeof(char));
                }

                if(key[KEY_LEFT]) {
                    printf("%d foi pra esquerda\n", id);
                    matriz[posicaoWinx[id].pos.y][posicaoWinx[id].pos.x] = 11;
                    mandarDir = 'a';
                    sendMsgToServer(&mandarDir, sizeof(char));
                }
                if(key[KEY_RIGHT]) {
                    printf("%d foi pra direita\n", id);
                    matriz[posicaoWinx[id].pos.y][posicaoWinx[id].pos.x] = 11;
                    mandarDir = 'd';
                    sendMsgToServer(&mandarDir, sizeof(char));
                }                
                key[KEY_UP] = false;
                key[KEY_DOWN] = false;
                key[KEY_LEFT] = false;
                key[KEY_RIGHT] = false;
                temp++;
                if(temp==60){
                    temp=0;
                    segundo++;
                    if(segundo==60){
                        segundo=0;
                        minuto++;
                        if(minuto==1){
                            sair = 1;
                            break;
                        }
                    }
                    atualizarMapa();
                    al_flip_display();
                }
            }
            else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                libera();
                sair = 1;
                fecharJogo = true;
                break;
            }
            else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch(ev.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        key[KEY_UP] = true;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        key[KEY_DOWN] = true;
                        break;
                    case ALLEGRO_KEY_LEFT: 
                        key[KEY_LEFT] = true;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        key[KEY_RIGHT] = true;
                        break;
                }
            }
        }
    }
}

int main(){
    int open = 1;
    int escolha = -1;
    if(!inicializar()){
        printf("Erro ao inicializar as bibliotecas\n");
        exit(0);
    }
    while(open){
        escolha = -1;
        conectar();
        if (fecharJogo){
            printf("\nJogo fechado.\n");
            exit(0);
        }
        ans = connectToServer(SERVER_IP);
        while(ans != SERVER_UP){
            tentConectar++;
            conectar();
            if (fecharJogo){
                printf("\nJogo fechado.\n");
                exit(0);
            }
            ans = connectToServer(SERVER_IP);
        }
		if(ans == SERVER_UP){
            tentConectar = 0;
            recvMsgFromServer(&id, WAIT_FOR_IT);
			fprintf(stderr,"conexao estabelecida!\n");
            for(i=0;i<TAM_MAPA;i++){
                for(j=0;j<TAM_MAPA;j++){
                    if(matriz[i][j] != 22) matriz[i][j] = 11;
                }
            }
            menu();
            if (fecharJogo){
                printf("\nJogo fechado.\n");
                exit(0);
            }
            while(escolha!=0){
                opcoes(&escolha);
                if (fecharJogo){
                    printf("\nJogo fechado.\n");
                    exit(0);
                }
                printf("escolha=%d\n", escolha);
                if(escolha==1) {
                    mostraRegras();
                    if (fecharJogo){
                        printf("\nJogo fechado.\n");
                        exit(0);
                    }
                }
            }
            menuPersonagem();
            if (fecharJogo){
                printf("\nJogo fechado.\n");
                exit(0);
            }
            confirmaInicio();
            if (fecharJogo){
                printf("\nJogo fechado.\n");
                exit(0);
            }
            game_start();
            if (fecharJogo){
                printf("\nJogo fechado.\n");
                exit(0);
            }
            game_over();
            if (fecharJogo){
                printf("\nJogo fechado.\n");
                exit(0);
            }
		}
		else {
            if(ans == SERVER_DOWN){                   
                fprintf(stderr,"Servidor nao encontrado\n");
            }
            else if(ans == SERVER_FULL){
                fprintf(stderr,"Capacidade maxima do servidor atingida\n");
            }
            else if(ans == SERVER_CLOSED){
                fprintf(stderr,"Servidor fechado para novas conexoes\n");
            }
            else if(ans == SERVER_TIMEOUT){
                fprintf(stderr,"Tempo esgotado - o servidor nao respondeu\n");
            }
            fprintf(stderr,"tente novamente ? y/n\n");
            char r;
            scanf(" %c", &r);
            if(r == 'n' || r == 'N') open = 0;
        }
	}
	fprintf(stderr,"Desconectado\n");
	return 0;
}
