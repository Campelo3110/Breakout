/**
 * @file main.c
 * @author Guilherme Campelo e Nicolas Busa
 * @brief Jogo Breakout
 * @copyright Copyright (c) 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*---------------------------------------------
 * Library headers.
 *-------------------------------------------*/
#include "raylib/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raylib/raygui.h"
//#include "raylib/raymath.h"

/*---------------------------------------------
 * Project headers.
 *-------------------------------------------*/

/*---------------------------------------------
 * Cores. 
 *-------------------------------------------*/

#define FUNDO CLITERAL(Color){ 28, 24, 31, 255 }
#define BLACKTRANS CLITERAL(Color){ 0, 0, 0, 150 }

/*---------------------------------------------
 * Macros. 
 *-------------------------------------------*/

    //Definimos quantas linhas e colunas a grade de tijolo vai ter. 
    #define LINHAS_TIJOLOS 6
    #define COLUNAS_TIJOLOS 6

    //O máximo de bolinhas que vai ter durante o jogo
    #define MAX_BOLINHAS 5   
    
    //Máximo de vida
    #define MAX_VIDAS 5

/*--------------------------------------------
 * Constants. 
 *------------------------------------------*/
    const int PARADO = 0;
    const int RODANDO = 1;
    const int PAUSA = 2;

/*---------------------------------------------
 * Custom types (enums, structs, unions, etc.)
 *-------------------------------------------*/

    //Estrutura para definir o estado do jogo
    typedef enum GameScreen {
        MENU,
        PERSONALIZACAO,
        JOGANDO,
        OPCOES,
        SAIR
    } GameScreen;

    //Estrutura para definir um tijolo
    typedef struct  Tijolo{
        Rectangle retan; // Retângulo
        Color cor;       // Cor do tijolo 
        bool ativo;      // Ativo ou quebrado
        int poder;
    } Tijolo;

    //Estrutura para definir uma bolinha
    typedef struct Bolinha {
        Vector2 pos;
        Vector2 vel;
        float raio;
        int vidas;
        Color cor;
    } Bolinha;

    //Estrutura para definir um jogador
    typedef struct Jogador {
        Vector2 pos;
        Vector2 dim;
        float vel;
        int pontos;
        Color cor;
    } Jogador;

    //Estrutura para definir os poderes
    typedef struct PoderAtivo{
        Vector2 pos;
        int tipo;
        Color cor;
        bool ativo;
    } PoderAtivo;

/*---------------------------------------------
 * Global variables.
 *-------------------------------------------*/

    //O jogo começa na tela menu
    GameScreen currentScreen = MENU;

    //Declaração de um array 2d do tipo Tijolo, que representa uma grade de tijolos.
    Tijolo tijolos[LINHAS_TIJOLOS][COLUNAS_TIJOLOS];
    
    //Controle quando o jogador passa de fase
    bool faseNova = false;
    bool tijolosDescendo = false;

    //Declaração de imagens
    Texture2D somLigado;
    Texture2D somDesligado;

    bool morre = true;
    int tamanhoExtra = 0;

    //Controla se o som está ativo ou desativo
    bool somAtivado = true;

    //Teclas que movem o jogaodr
    int teclaEsquerda = KEY_A; 
    int teclaDireita = KEY_D; 

    Jogador jogador;
    Bolinha bolinha[MAX_BOLINHAS];
    PoderAtivo poderesAtivos;
    
    //Começa com 1 bolinha o jogo
    int numBol = 1;

    //Variável que vai guarda o máximo de ponto que o jogador conseguir
    int recorde;

    //Controla o estado do jogo
    int estado = PARADO;

    //Cor da bolinha
    Color corSelecionada;

/*---------------------------------------------
 * Funções sendo inicializadas 
 *-------------------------------------------*/
    void update( float delta );

    void draw( void );

    //Funções do tijolo
    void inicializarTijolos();
    void desenharTijolos(int fases);
    void atualizarTijolos(float delta);
    bool todosTijolosDestruidos();

    //Funções do Jogador
    void atualizarJogador( Jogador *jogador, int teclaEsquerda, int teclaDireita, float delta );
    void desenharJogador( Jogador *jogador );
    void resolverColisao( Bolinha *b, Jogador *j, float delta);

    //Funcoes da bolinha
    bool atualizarBolinha( Bolinha *bolinha, float delta );
    void desenharBolinha();

    //Funcoes dos poderes
    void ativarPoder(int tipo, Vector2 pos);
    void desenharPoderes();
    void atualizarPoderes(float delta, Jogador *jogador);

    //Função de pontos e vidas
    void pontosVidas(int pontos);

    //Funções de menus
    void menuPrincipal();
    void menuPausa();
    bool telaMorte();

    //Função da tela de personalização
    void telaPersonalizacao(Bolinha *bolinha);

    //Função de som
    void sons(int som);

/*---------------------------------------------
 * Funções responsavel pelo controle do jogo
 *-------------------------------------------*/
    int main( void ) {

        // local variables and initial user input
        // antialiasing
        SetConfigFlags( FLAG_MSAA_4X_HINT );

        InitWindow( 800, 600, "Breakout" );

        SetExitKey(KEY_NULL);

        // init audio device only if your game uses sounds
        InitAudioDevice();
        
        somLigado = LoadTexture("resources/images/somOn.png");
        somDesligado = LoadTexture("resources/images/somOff.png");

        // FPS: frames per second
        SetTargetFPS( 60 );    

        // you must load game resources here
        inicializarTijolos();

        jogador = (Jogador) {
            .pos = {
                .x = GetScreenWidth() / 2 - 75,
                .y = GetScreenHeight() - 75
            },
            .dim = {
                .x = 150,
                .y = 30
            },
            .vel = 300,
            .pontos = 0,
            .cor = WHITE
        };

        bolinha[0] = (Bolinha) {
            .pos = {
                .x = GetScreenWidth() / 2,
                .y = GetScreenHeight() - 90
            },
            .vel = {
                .x = 300,
                .y = 300
            },
            .raio = 15,
            .vidas = 3,
            .cor = WHITE
        };

        // game loop
        while ( !WindowShouldClose() ) {
            update( GetFrameTime() );
            draw();
        }

        // you should unload game resources here

        // close audio device only if your game uses sounds
        UnloadTexture(somLigado);
        UnloadTexture(somDesligado);
        CloseAudioDevice();
        CloseWindow();

        return 0;

    }

    void update( float delta ) {

        //Verifica se o jogo está PARADO ou PAUSADO
        if ( estado == PARADO || estado == PAUSA) {
            poderesAtivos.ativo = false;

            if ( IsKeyPressed( KEY_ENTER ) && currentScreen == JOGANDO ) {
                estado = RODANDO;
                sons(1);
            }
            
            if( IsKeyPressed(KEY_ESCAPE) ){
                estado = RODANDO;
            }

        } else { //Aqui o jogo está RODANDO

            //Passa para a proxíma fase
            if(todosTijolosDestruidos() && !faseNova){
                inicializarTijolos();

                faseNova = true;
                tijolosDescendo = true;
                sons(5);

                for (int i = 0; i < LINHAS_TIJOLOS; i++) {
                    for (int j = 0; j < COLUNAS_TIJOLOS; j++) {
                        tijolos[i][j].retan.y = -100; //Tira os tijolos da tela
                    }
                }
            }

            if(tijolosDescendo){
                atualizarTijolos(delta);
            }

            atualizarJogador(&jogador, teclaEsquerda, teclaDireita, delta);

            //Percorre todas as bolinhas
            for (int i = 0; i < numBol; i++) {

                //Atualiza todas as bolinhas ativa
                if (atualizarBolinha(&bolinha[i], delta)) {
                    
                    //Remove a bolinha do array
                    for (int j = i; j < numBol - 1; j++) {
                        bolinha[j] = bolinha[j + 1];
                    }

                    //Atualiza o índice e o numero de bolinhas
                    if(numBol != 1){
                        numBol--;
                        i--;
                    }
                }

                //Verifica colisão da bolinha
                resolverColisao(&bolinha[i], &jogador, delta);
                atualizarPoderes(delta, &jogador);
            }

            if( IsKeyPressed(KEY_ESCAPE) ){
                estado = PAUSA;
            }
        }
    }

    void draw( void ) {
        
        BeginDrawing();
        ClearBackground( FUNDO );

        //Switch responsavel por mudar as telas
        switch (currentScreen) {
            case MENU:
                menuPrincipal();
                break;
            case PERSONALIZACAO:
                telaPersonalizacao(&bolinha[0]);
                break;
            case JOGANDO:
                desenharTijolos(2);
                desenharJogador( &jogador );
                desenharPoderes();

                for(int i = 0; i < numBol; i++){
                    desenharBolinha(&bolinha[i]);
                }

                pontosVidas(jogador.pontos);

                if(bolinha[0].vidas == 0){
                    estado = PARADO;
                    if(telaMorte()){
                        inicializarTijolos();
                        estado = RODANDO;
                        jogador.pontos = 0;
                        bolinha[0].vidas = 3;
                    }   
                }
                
                if (estado == PAUSA) {
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
                    menuPausa();
                }

                Rectangle botaoSom = { GetScreenWidth() - 30, 15, 50, 50 };

                DrawTexture(somAtivado ? somLigado : somDesligado, botaoSom.x, botaoSom.y, WHITE);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), botaoSom)) {
                    somAtivado = !somAtivado;
                }

                break;
            case SAIR:
                CloseWindow();
                break;
        }

        EndDrawing();

    }

/*---------------------------------------------
 * Funções do tijolo. 
 *-------------------------------------------*/
    void inicializarTijolos(){
        int telaLargura = GetScreenWidth();                // Largura da tela.
        int tijoloLargura = telaLargura / COLUNAS_TIJOLOS; // Largura do tijolo.
        int tijoloAltura = 30;                             // Altura do tijolo.
        int padding = 5;                                   // Espaçamento entre os tijolos.

        //For para inicializar os tijolos no array.
        for(int i = 0; i < LINHAS_TIJOLOS; i++){
            for(int j = 0; j < COLUNAS_TIJOLOS; j++){

                //Aqui definimos os atributos de um retângulo como largura, altura, pos x, pos y.
                tijolos[i][j].retan.width = tijoloLargura - padding; 
                tijolos[i][j].retan.height = tijoloAltura - padding;
                tijolos[i][j].retan.x = j * tijoloLargura + padding / 2;
                tijolos[i][j].retan.y = i * tijoloAltura + 50 + padding / 2;

                //Definição de poderes
                int poder = GetRandomValue(1,3);
                tijolos[i][j].poder = poder;

                //Switch para definir a cor de cada linha de tijolo.
                switch (i) {
                    case 0:
                        tijolos[i][j].cor = RED;
                        break;
                    case 1:
                        tijolos[i][j].cor = ORANGE;
                        break;
                    case 2:
                        tijolos[i][j].cor = GREEN;
                        break;
                    case 3:
                        tijolos[i][j].cor = SKYBLUE;
                        break;
                    case 4: 
                        tijolos[i][j].cor = BLUE;
                        break;
                    case 5:
                        tijolos[i][j].cor = DARKBLUE;
                        break;
                    case 6:
                        tijolos[i][j].cor = PINK;
                        break;
                    default:
                        tijolos[i][j].cor = VIOLET;
                        break;
                }

                //Aqui definimos que ele começa como ativo(não destruido).
                tijolos[i][j].ativo = true;
            }
        }
    }

    void desenharTijolos(int fases){
        for(int i = 0; i < LINHAS_TIJOLOS; i++){
            for(int j = 0; j < COLUNAS_TIJOLOS; j++){
                if(tijolos[i][j].ativo){
                    DrawRectangle(
                        tijolos[i][j].retan.x,      //Posiçao x.
                        tijolos[i][j].retan.y,      //Posiçao y.
                        tijolos[i][j].retan.width,  //Largura.
                        tijolos[i][j].retan.height, //Altura.
                        tijolos[i][j].cor //Cor.
                    );
                }
            }
        }
    }

    void atualizarTijolos(float delta){

        float velocidadeDescida = 250 * delta;

        bool todosPararam = true;

        
        bolinha->vel.y += 2;

        //Faz os tijolos descerem
        for(int i = 0; i < LINHAS_TIJOLOS; i++){
            for(int j = 0; j < COLUNAS_TIJOLOS; j++){
                if(tijolos[i][j].ativo){
                    if(tijolos[i][j].retan.y < 50 + i * 30){
                        tijolos[i][j].retan.y += velocidadeDescida;
                        todosPararam = false;
                    }
                }
            }
        }

        if(todosPararam){
            tijolosDescendo = false;
            faseNova = false;
        }

    }

    bool todosTijolosDestruidos() {
        for (int i = 0; i < LINHAS_TIJOLOS; i++) {
            for (int j = 0; j < COLUNAS_TIJOLOS; j++) {
                if (tijolos[i][j].ativo) {
                    return false;
                }
            }
        }
        return true;
    }

/*---------------------------------------------
 * Funções do Jogador 
 *-------------------------------------------*/
    void atualizarJogador( Jogador *jogador, int teclaEsquerda, int teclaDireita, float delta ) {

        //If para levar o jogador a esquerda
        if ( IsKeyDown( teclaEsquerda ) ) {
            jogador->pos.x -= jogador->vel * delta;
        }

        //If para levar o jogador a direita
        if ( IsKeyDown( teclaDireita ) ) {
            jogador->pos.x += jogador->vel * delta;
        }

        //Impede que o jogador ultrapasse  as paredes
        if ( jogador->pos.x < 0 ) {
            jogador->pos.x = 0;
        } else if ( jogador->pos.x + jogador->dim.x > GetScreenWidth() ) {
            jogador->pos.x = GetScreenWidth() - jogador->dim.x;
        }

    }

    void desenharJogador( Jogador *jogador ) {

        //Desenha o jogador
        DrawRectangle(
            jogador->pos.x,
            jogador->pos.y,
            jogador->dim.x,
            jogador->dim.y,
            jogador->cor
        );
    }

    void resolverColisao( Bolinha *b, Jogador *j, float delta) {

        // Checa colisão da bolinha com o jogador
        bool colisao = CheckCollisionCircleRec(
            b->pos,
            b->raio,
            (Rectangle) {
                .x = j->pos.x,
                .y = j->pos.y,
                .width = j->dim.x,
                .height = j->dim.y
            }
        );

        //Rebate a bolinha quando toca no jogador
        if ( colisao ) {
            b->pos.y = j->pos.y - j->dim.y - b->raio;
            b->vel.y = -b->vel.y;
            sons(3);
        }

        //Verifica a colisao da bola com o tijolo
        for (int i = 0; i < LINHAS_TIJOLOS; i++) {
            for (int j = 0; j < COLUNAS_TIJOLOS; j++) {
                if (tijolos[i][j].ativo) {
                    if (CheckCollisionCircleRec(b->pos, b->raio, tijolos[i][j].retan)) {
                        tijolos[i][j].ativo = false;
                        b->vel.y *= -1;
                        jogador.pontos++;
                        sons(3);
                        Vector2 poderPos = { tijolos[i][j].retan.x + tijolos[i][j].retan.width / 2, tijolos[i][j].retan.y + tijolos[i][j].retan.height / 2 };
                        ativarPoder(tijolos[i][j].poder, poderPos);
                        
                        break;
                    }
                }
            }
        }

    }

/*---------------------------------------------
 * Funções da bolinha. 
 *-------------------------------------------*/
    bool atualizarBolinha( Bolinha *bolinha, float delta ) {

        //Movimenta a bolinha
        bolinha->pos.x += bolinha->vel.x * delta;
        bolinha->pos.y += bolinha->vel.y * delta;
        
        if ( bolinha->pos.x + bolinha->raio > GetScreenWidth() ) { //Verifica se houve colisão com a parede da direita
            bolinha->pos.x = GetScreenWidth() - bolinha->raio;
            bolinha->vel.x = -bolinha->vel.x;
            sons(3);
        } else if ( bolinha->pos.x - bolinha->raio < 0 ) { //Verifica se houve colisão com a parede da esquerda
            bolinha->pos.x = bolinha->raio;
            bolinha->vel.x = -bolinha->vel.x;
            sons(3);
        }

        if ( bolinha->pos.y + bolinha->raio > GetScreenHeight() ) { //Verifica se houve colisão com a parte inferior
            if(morre){
                sons(4);
                bolinha->pos.x = GetScreenWidth() / 2;
                bolinha->pos.y = GetScreenHeight() - 90;
                bolinha->vel.x = 300;
                bolinha->vel.y = 300;
                bolinha->vidas--;
                if(numBol == 1){
                    estado = PARADO;
                    jogador.pos.x = GetScreenWidth() / 2 - 75;
                    jogador.pos.y = GetScreenHeight() - 75;
                }
                return true;
            }else{
                bolinha->pos.y = bolinha->raio;
                bolinha->vel.y = bolinha->vel.y;
                sons(3);   
            }
            
        } else if ( bolinha->pos.y - bolinha->raio < 0 ) { //Verifica se houve colisão com a parte superior
            bolinha->pos.y = bolinha->raio;
            bolinha->vel.y = -bolinha->vel.y;
            sons(3);
        }

        return false;
    }
    
    void desenharBolinha(Bolinha *bolinha) {
        //Desenha a bolinha
        DrawCircle(
            bolinha->pos.x,
            bolinha->pos.y,
            bolinha->raio,
            bolinha->cor
        );
    }

/*---------------------------------------------
 * Poderes. 
 *-------------------------------------------*/

    void ativarPoder(int tipo, Vector2 pos){
        
        if(!poderesAtivos.ativo){
            poderesAtivos.ativo = true;
            poderesAtivos.pos = pos;
            poderesAtivos.tipo = tipo;
            switch (tipo)
            {
            case 1:
                poderesAtivos.cor = RED;
                break;
            case 2:
                poderesAtivos.cor = WHITE;
                break;
            case 3:
                poderesAtivos.cor = BLUE;
                break;
            default:
                poderesAtivos.cor = GRAY;
                break;
            }
        }
        
    }
    
    void atualizarPoderes(float delta, Jogador *jogador) {
        
        if (poderesAtivos.ativo) {
            poderesAtivos.pos.y += 100 * delta;
            Rectangle poderRec = { poderesAtivos.pos.x - 10, poderesAtivos.pos.y - 10, 20, 20 };
            Rectangle jogadorRec = { jogador->pos.x, jogador->pos.y, jogador->dim.x, jogador->dim.y };

            if (CheckCollisionRecs(poderRec, jogadorRec)) {
                switch (poderesAtivos.tipo) {
                    case 1:
                            if(tamanhoExtra == 0){
                                jogador->dim.x += 100;
                                tamanhoExtra = 100;
                            }else{
                                tamanhoExtra = 0;
                                jogador->dim.x -= 100; 
                            }
                        break;
                    case 2:
                        if (numBol < MAX_BOLINHAS) {
                            bolinha[numBol] = bolinha[0];
                            bolinha[numBol].vel.y *= -1;
                            bolinha[numBol].pos.x = jogador->pos.x;
                            bolinha[numBol].pos.y = jogador->pos.y;
                            numBol++;
                        }
                        break;
                    case 3:
                        if(bolinha[0].vidas < MAX_VIDAS){
                            bolinha[0].vidas++;
                        }
                        break;
                }
                poderesAtivos.ativo = false;
            }
            if (poderesAtivos.pos.y > GetScreenHeight()) {
                poderesAtivos.ativo = false;
            }
        }
    }
    
    void desenharPoderes() {
        
        if (poderesAtivos.ativo) {
            DrawRectangle(poderesAtivos.pos.x - 10, poderesAtivos.pos.y - 10, 20, 20, poderesAtivos.cor);
        }
        
    }

/*---------------------------------------------
 * Função de pontos e vidas. 
 *-------------------------------------------*/
    void pontosVidas(int pontos){
        const char *pon = TextFormat("%d", pontos);
        const char *vidass = TextFormat("%d", bolinha[0].vidas);

        int centro = GetScreenWidth() / 2;

        DrawText(pon, centro, 10, 40, WHITE);

        int raio = 15;
        int espaco = 80;

    
        for(int i = 0; i < bolinha[0].vidas; i++){
            DrawCircle(25 + i * espaco / 2, 25, raio, WHITE);
        }
        
        for(int i = 0; i < MAX_VIDAS; i++){
            DrawCircleLines(25 + i * espaco / 2, 25, raio, WHITE);
        }
    }

/*---------------------------------------------
 * Funções de menus e telas. 
 *-------------------------------------------*/
    void menuPrincipal(){

        int larguraTela = GetScreenWidth();
        int alturaTela = GetScreenHeight();

        int botaoLargura = 300;
        int botaoAltura = 60;

        int x = larguraTela / 2 - botaoLargura / 2;
        int y = alturaTela / 2;
        int espaco = 25;

        const char* titulo = "BREAKOUT";
        int fonteTitulo = 70;
        int tituloX = larguraTela / 2 - MeasureText(titulo, fonteTitulo) / 2;
        int tituloY = y - 160 - espaco;

        ClearBackground(FUNDO);

        DrawText(titulo, tituloX + 2, tituloY + 2, fonteTitulo, BLACK);
        DrawText(titulo, tituloX, tituloY, fonteTitulo, WHITE);

        Color corBotao = BLACK;
        Color corTexto = WHITE;
        Color corBorda = WHITE;

        DrawRectangle(x, y - botaoAltura - espaco, botaoLargura, botaoAltura , corBotao);
        DrawRectangleLines(x, y - botaoAltura - espaco, botaoLargura, botaoAltura, corBorda);
        DrawText("JOGAR", x + 90, y - botaoAltura - espaco + 15, 30, corTexto);

        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){x, y - botaoAltura - espaco, botaoLargura, botaoAltura}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentScreen = JOGANDO;
            inicializarTijolos();
            jogador.pontos = 0;
            bolinha[0].vidas = 3;
            jogador.pos.x = GetScreenWidth() / 2 - 75;
            jogador.pos.y = GetScreenHeight() - 75;
            bolinha[0].pos.x = GetScreenWidth() / 2;
            bolinha[0].pos.y = GetScreenHeight() - 90;
        }

        DrawRectangle(x, y, botaoLargura, botaoAltura, corBotao);
        DrawRectangleLines(x, y, botaoLargura, botaoAltura, corBorda);
        DrawText("PERSONALIZAR", x + 45, y + 15, 30, corTexto);

        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){x, y, botaoLargura, botaoAltura}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentScreen = PERSONALIZACAO;
        }

        DrawRectangle(x, y + botaoAltura + espaco, botaoLargura, botaoAltura, corBotao);
        DrawRectangleLines(x, y + botaoAltura + espaco, botaoLargura, botaoAltura, corBorda);
        DrawText("SAIR", x + 110, y + botaoAltura + espaco + 15, 30, corTexto);

        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){x, y + botaoAltura + espaco, botaoLargura, botaoAltura}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentScreen = SAIR;
        }

    }
    
    void menuPausa(){

        int larg = 400;
        int alt = 250;

        int x = (GetScreenWidth() - larg) / 2;
        int y = (GetScreenHeight() - alt) / 2;

        int espaco = 20;
        int fonte = 30;

        DrawRectangle(x, y, larg, alt, BLACK);
        DrawRectangleLines(x,y,larg,alt, RAYWHITE);

        const char* titulo = "PAUSA";
        int tituloWidth = MeasureText(titulo, 50);
        DrawText(titulo, x + (larg - tituloWidth) / 2, y + 20, 50, RAYWHITE);


        DrawRectangle(x + larg / 2 - 140, y + 100, 140, 50, BLACK);
        DrawRectangleLines(x + larg / 2 - 140, y + 100, 140, 50, WHITE);
        DrawText("VOLTAR", x + larg / 2 - 135, y + 110, 30, WHITE);

        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){x + larg / 2 - 140, y + 100, 140, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            estado = RODANDO;
        }

        DrawRectangle(x + larg / 2 + 20, y + 100, 140, 50, BLACK);
        DrawRectangleLines(x + larg / 2 + 20, y + 100, 140, 50, WHITE);
        DrawText("MENU", x + larg / 2 + 50, y + 110, 30, WHITE);

        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){x + larg / 2 + 20, y + 100, 140, 50}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentScreen = MENU;
            estado = PARADO;
        }
    }

    bool telaMorte(){

        int larg = 300;
        int alt = 200;

        int x = (GetScreenWidth() - larg) / 2;
        int y = (GetScreenHeight() - alt) / 2;

        int fontTitulo = 30;
        int fontTexto = 20;
        int espacoLinha = 10;

        DrawRectangle(x, y, larg, alt, BLACK);
        DrawRectangleLines(x, y, larg, alt, WHITE);

        const char* titulo = "FIM DE JOGO";
        int tituloWidth = MeasureText(titulo, fontTitulo);
        DrawText(titulo, x + (larg - tituloWidth) / 2, y + 15, fontTitulo, WHITE);

        if (jogador.pontos > recorde) {
            recorde = jogador.pontos;
        }

        const char* labelPontos = "PONTUAÇÃO";
        const char* labelRecorde = "RECORDE";

        int labelY = y + 70;

        int labelPontosX = x + larg / 4 - MeasureText(labelPontos, fontTexto) / 2;
        int labelRecordeX = x + 3 * larg / 4 - MeasureText(labelRecorde, fontTexto) / 2;

        DrawText(labelPontos, labelPontosX, labelY, fontTexto, WHITE);
        DrawText(labelRecorde, labelRecordeX, labelY, fontTexto, WHITE);

        const char* textoPontos = TextFormat("%05i", jogador.pontos);
        const char* textoRecorde = TextFormat("%05i", recorde);

        int valorY = labelY + fontTexto + espacoLinha;

        int valorPontosX = x + larg / 4 - MeasureText(textoPontos, fontTexto) / 2;
        int valorRecordeX = x + 3 * larg / 4 - MeasureText(textoRecorde, fontTexto) / 2;

        DrawText(textoPontos, valorPontosX, valorY, fontTexto, WHITE);
        DrawText(textoRecorde, valorRecordeX, valorY, fontTexto, WHITE);

        DrawText(textoPontos, valorPontosX, valorY, fontTexto, WHITE);
        DrawText(textoRecorde, valorRecordeX, valorY, fontTexto, WHITE);

        int botaoLargura = 230;
        int botaoAltura = 40;
        int botaoX = x + (larg - botaoLargura) / 2;
        int botaoY = y + alt - botaoAltura - 20;

        DrawRectangle(botaoX, botaoY, botaoLargura, botaoAltura, BLACK);
        DrawRectangleLines(botaoX, botaoY, botaoLargura, botaoAltura, WHITE);

        const char* textoBotao = "JOGAR NOVAMENTE";
        int textoBotaoWidth = MeasureText(textoBotao, 20);
        DrawText(textoBotao, botaoX + (botaoLargura - textoBotaoWidth) / 2, botaoY + 10, 20, WHITE);

        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){botaoX, botaoY, botaoLargura, botaoAltura}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return true;
        }
        
        return false;
        }

    void telaPersonalizacao(Bolinha *bolinha){

        int larguraTela = GetScreenWidth();
        int alturaTela = GetScreenHeight();

        int painelLargura = 500;
        int painelAltura = 400;
        int painelX = larguraTela / 2 - painelLargura / 2;
        int painelY = alturaTela / 2 - painelAltura / 2;

        DrawRectangle(painelX, painelY, painelLargura, painelAltura, BLACKTRANS);
        DrawRectangleLines(painelX, painelY, painelLargura, painelAltura, WHITE);

        const char* titulo = "PERSONALIZAR";
        int fonteTitulo = 40;
        int tituloX = painelX + painelLargura / 2 - MeasureText(titulo, fonteTitulo) / 2;
        int tituloY = painelY + 15;
        DrawText(titulo, tituloX + 2, tituloY + 2, fonteTitulo, BLACK);
        DrawText(titulo, tituloX, tituloY, fonteTitulo, WHITE);

        Rectangle pickerArea = { painelX + 30, painelY + 80, 200, 200 };
        GuiColorPicker(pickerArea, NULL, &bolinha->cor);

        int previewX = painelX + painelLargura - 150;
        int previewY = painelY + 150;
        DrawCircle(previewX, previewY, 40, bolinha->cor);
        DrawCircleLines(previewX, previewY, 40, WHITE);
     
        int botaoLargura = 200;
        int botaoAltura = 60;
        int botaoX = painelX + painelLargura / 2 - botaoLargura / 2;
        int botaoY = painelY + painelAltura - botaoAltura - 20;

        DrawRectangle(botaoX, botaoY, botaoLargura, botaoAltura, BLACK);
        DrawRectangleLines(botaoX, botaoY, botaoLargura, botaoAltura, WHITE);
        DrawText("JOGAR", botaoX + 60, botaoY + 15, 30, WHITE);

        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ botaoX, botaoY, botaoLargura, botaoAltura }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentScreen = JOGANDO;
        }
    }

/*---------------------------------------------
 * Função do som 
 *-------------------------------------------*/
    void sons(int som){
        Sound comecando = LoadSound("resources/sfx/comecando.wav");
        Sound batida = LoadSound("resources/sfx/batida.wav");
        Sound batidaTijolo = LoadSound("resources/sfx/batidaTijolo.wav");
        Sound morte = LoadSound("resources/sfx/morte.wav");
        Sound proximaFase = LoadSound("resources/sfx/proximaFase.wav");

        if(somAtivado){
            switch (som)
            {
            case 1:
                PlaySound(comecando);
                break;
            case 2:
                PlaySound(batida);
                break;
            case 3:
                PlaySound(batidaTijolo);
                break;
            case 4:
                PlaySound(morte);
                break;
            case 5:
                PlaySound(proximaFase);
                break;
            default:
                break;
            }
        }
    }