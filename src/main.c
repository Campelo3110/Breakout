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
    #define LINHAS_TIJOLOS 5
    #define COLUNAS_TIJOLOS 10

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

    //Estrutura para definir oum tijolo
    typedef struct  Tijolo{
        Rectangle retan; // Retângulo
        Color cor;       // Cor do tijolo 
        bool ativo;      // Ativo ou quebrado
        int poder;
    } Tijolo;

    typedef struct Bolinha {
        Vector2 pos;
        Vector2 vel;
        float raio;
        int vidas;
        Color cor;
    } Bolinha;

    typedef struct Jogador {
        Vector2 pos;
        Vector2 dim;
        float vel;
        int pontos;
        Color cor;
    } Jogador;


/*---------------------------------------------
 * Global variables.
 *-------------------------------------------*/

    //O jogo começa na tela menu
    GameScreen currentScreen = JOGANDO;

    //Declaração de um array 2d do tipo Tijolo, que representa uma grade de tijolos.
    Tijolo tijolos[LINHAS_TIJOLOS][COLUNAS_TIJOLOS];

    Jogador jogador;
    Bolinha bolinha;
    int estado = PARADO;

    float volumeMusica = 70;
    float volumeEfeitos = 80;
    bool musicaAtiva = true;
    bool efeitosAtivos = true;
    bool fullscreen = false;

    int teclaEsquerda = KEY_A; 
    int teclaDireita = KEY_D; 

    const int resolucoes[4][2] = {
        {800, 600},
        {1024, 768},
        {1280, 720},
        {1920, 1080}
    };

    int resolucaoSelecionada = 0;

/*---------------------------------------------
 * Funções sendo inicializadas 
 *-------------------------------------------*/
    void update( float delta );

    void draw( void );

    //Funções do tijolo
    void inicializarTijolos();
    void desenharTijolos();

    //Funções do Jogador
    void atualizarJogador( Jogador *jogador, int teclaEsquerda, int teclaDireita, float delta );
    void desenharJogador( Jogador *jogador );
    void resolverColisao( Bolinha *b, Jogador *j);

    //Funcoes da bolinha
    void atualizarBolinha( Bolinha *bolinha, float delta );
    void desenharBolinha( Bolinha *bolinha );
    
    //Função de pontos e vidas
    void pontosVidas(int pontos);

    //Funções de menus
    void menuPrincipal();
    void menuOpcoes();
    void menuPausa();

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

        InitWindow( resolucoes[resolucaoSelecionada][0], resolucoes[resolucaoSelecionada][0], "Breakout" );

        SetExitKey(KEY_NULL);

        // init audio device only if your game uses sounds
        InitAudioDevice();

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
            .vel = 200,
            .pontos = 0,
            .cor = WHITE
        };

        bolinha = (Bolinha) {
            .pos = {
                .x = GetScreenWidth() / 2,
                .y = GetScreenHeight() - 90
            },
            .vel = {
                .x = 200,
                .y = 200
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
        //CloseAudioDevice();
        CloseWindow();

        return 0;

    }

    void update( float delta ) {

        if ( estado == PARADO || estado == PAUSA) {

            if ( IsKeyPressed( KEY_ENTER ) ) {
                estado = RODANDO;
            }

            if( estado == PAUSA){
                menuPausa();
                if(IsKeyPressed(KEY_ESCAPE)){
                    estado = RODANDO;
                }
            }

        } else {
            atualizarJogador( &jogador, teclaEsquerda, teclaDireita, delta );
            atualizarBolinha( &bolinha, delta );
            resolverColisao( &bolinha, &jogador);

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
                telaPersonalizacao(&bolinha);
                break;
            case JOGANDO:
                desenharTijolos();
                desenharJogador( &jogador );
                desenharBolinha( &bolinha );
                
                pontosVidas(jogador.pontos);
                break;
            case OPCOES:
                menuOpcoes();
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
                int poder = GetRandomValue(0,3);
                tijolos[i][j].poder = poder;
                printf("%d", poder);

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
                        tijolos[i][j].cor = BLUE;
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

    void desenharTijolos(){

        //For utilizado para desenhar os tijolos.
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

    void resolverColisao( Bolinha *b, Jogador *j) {

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
            sons(1);
        }

        //Verifica a colisao da bola com o tijolo
        for (int i = 0; i < LINHAS_TIJOLOS; i++) {
            for (int j = 0; j < COLUNAS_TIJOLOS; j++) {
                if (tijolos[i][j].ativo) {
                    if (CheckCollisionCircleRec(bolinha.pos, bolinha.raio, tijolos[i][j].retan)) {
                        tijolos[i][j].ativo = false;
                        bolinha.vel.y *= -1;
                        jogador.pontos++;
                        sons(2);

                        switch (tijolos[i][j].poder)
                        {
                        case 1:
                                printf("1");
                            break;
                        case 2:
                                desenharBolinha(&bolinha);
                                printf("agora");
                            break;
                        case 3:
                                printf("3");
                            break;
                        default:
                            break;
                        }

                        break;
                    }
                }
            }
        }

    }

/*---------------------------------------------
 * Funções da bolinha. 
 *-------------------------------------------*/
    void atualizarBolinha( Bolinha *bolinha, float delta ) {

        //Movimenta a bolinha
        bolinha->pos.x += bolinha->vel.x * delta;
        bolinha->pos.y += bolinha->vel.y * delta;

        if ( bolinha->pos.x + bolinha->raio > GetScreenWidth() ) { //Verifica se houve colisão com a parede da direita
            bolinha->pos.x = GetScreenWidth() - bolinha->raio;
            bolinha->vel.x = -bolinha->vel.x;
            sons(1);
        } else if ( bolinha->pos.x - bolinha->raio < 0 ) { //Verifica se houve colisão com a parede da esquerda
            bolinha->pos.x = bolinha->raio;
            bolinha->vel.x = -bolinha->vel.x;
            sons(1);
        }

        if ( bolinha->pos.y + bolinha->raio > GetScreenHeight() ) { //Verifica se houve colisão com a parte inferior
            bolinha->pos.x = GetScreenWidth() / 2;
            bolinha->pos.y = GetScreenHeight() - 30;
            bolinha->vel.x = 200;
            bolinha->vel.y = 200;
            bolinha->vidas--;
            estado = PARADO;
        } else if ( bolinha->pos.y - bolinha->raio < 0 ) { //Verifica se houve colisão com a parte superior
            bolinha->pos.y = bolinha->raio;
            bolinha->vel.y = -bolinha->vel.y;
            sons(1);
        }
    }
    
    void desenharBolinha( Bolinha *bolinha) {

        //Desenha a bolinha
        DrawCircle(
            bolinha->pos.x,
            bolinha->pos.y,
            bolinha->raio,
            bolinha->cor
        );
    }
  
/*---------------------------------------------
 * Função de pontos e vidas. 
 *-------------------------------------------*/
    void pontosVidas(int pontos){
        const char *pon = TextFormat("%d", pontos);
        const char *vidass = TextFormat("%d", bolinha.vidas);

        int centro = GetScreenWidth() / 2;

        DrawText(pon, centro, 10, 40, WHITE);
        DrawText(vidass, GetScreenWidth()/2, GetScreenHeight()/2,40,WHITE);
        DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 50, GREEN );

        if(bolinha.vidas == 0){
            currentScreen = MENU;
        }
    }

/*---------------------------------------------
 * Funções de menus. 
 *-------------------------------------------*/
    void menuPrincipal(){

        int larguraTela = GetScreenWidth();
        int alturaTela = GetScreenHeight();

        int botaoLargura = 300;
        int botaoAltura = 60;

        int x = larguraTela / 2 - botaoLargura / 2;
        int y = alturaTela / 2;
        int espaco = 20;

        const char* titulo = "BREAKOUT";
        int fonteTitulo = 70;
        int tituloX = larguraTela / 2 - MeasureText(titulo, fonteTitulo) / 2;
        int tituloY = y - 160 - espaco;

        DrawText(titulo, tituloX + 3, tituloY + 3, fonteTitulo, GRAY);
        DrawText(titulo, tituloX, tituloY, fonteTitulo, RAYWHITE);

        if (GuiButton((Rectangle){ x, y - botaoAltura - espaco, botaoLargura, botaoAltura }, "JOGAR")) {
            currentScreen = JOGANDO;
        }

        if (GuiButton((Rectangle){ x, y, botaoLargura, botaoAltura }, "OPÇÕES")) {
            currentScreen = OPCOES;
        }

        if (GuiButton((Rectangle){ x, y + botaoAltura + espaco, botaoLargura, botaoAltura }, "SAIR")) {
            currentScreen = SAIR;
        }

    }
    
    void menuOpcoes() {

        int larguraTela = GetScreenWidth();
        int alturaTela = GetScreenHeight();
        int espaco = 50;
        int largura = 500;
        int altura = 500;
        int x = larguraTela / 2 - largura / 2;
        int y = alturaTela / 2 - altura / 2;
        int xLabel = x + 20;

        DrawRectangle(x, y, largura, altura, BLACKTRANS);
        DrawRectangleLines(x, y, largura, altura, WHITE);
        DrawText("OPÇÕES", x + 170, y + 10, 30, WHITE);

        GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

        // Mudar a resolução
        GuiLabel((Rectangle){ xLabel, y + espaco, 140, 24 }, "Resolução:");
        static bool dropdownAtivo = false;
        if (GuiDropdownBox((Rectangle){ xLabel + 150, y + espaco, 200, 24 }, 
            "800x600;1024x768;1280x720;1920x1080", &resolucaoSelecionada, dropdownAtivo)) {
            dropdownAtivo = !dropdownAtivo;
            SetWindowSize(resolucoes[resolucaoSelecionada][0], resolucoes[resolucaoSelecionada][1]);
        }

        // Mudar a tela cheia
        GuiLabel((Rectangle){ xLabel, y + espaco * 2, 140, 24 }, "Tela Cheia:");
        GuiCheckBox((Rectangle){ xLabel + 150, y + espaco * 2, 20, 20 }, "Ativar", &fullscreen);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), (Rectangle){ xLabel + 150, y + espaco * 2, 20, 20 })) {
            ToggleFullscreen();
        }

        int teclaPre = 0;

        // VOLUME MÚSICA
        GuiLabel((Rectangle){ xLabel, y + espaco * 5, 180, 24 }, "Volume Música:");
        GuiSlider((Rectangle){ xLabel + 150, y + espaco * 5, 200, 20 }, NULL, NULL, &volumeMusica, 0, 100);

        // VOLUME EFEITOS
        GuiLabel((Rectangle){ xLabel, y + espaco * 6, 180, 24 }, "Volume Efeitos:");
        GuiSlider((Rectangle){ xLabel + 150, y + espaco * 6, 200, 20 }, NULL, NULL, &volumeEfeitos, 0, 100);

        // BOTÃO VOLTAR
        if (GuiButton((Rectangle){ x + largura / 2 - 75, y + altura - 40, 150, 30 }, "VOLTAR")) {
            currentScreen = MENU;
        }
    }

    void menuPausa(){

        int larg = GetScreenWidth() / 2;
        int alt = GetScreenHeight() / 2;

        int x = (GetScreenWidth() - larg) / 2;
        int y = (GetScreenHeight() - alt) / 2;

        int xLabel = (x / 2) + x;

        int espaco = 50;
        int font = 30;

        DrawRectangle(x, y, larg, alt, BLACK);
        DrawRectangleLines(x, y, larg, alt, WHITE);

        DrawText("PAUSA", xLabel, y, 50, WHITE);

        GuiSetStyle(DEFAULT, TEXT_SIZE, font);

        if (GuiLabelButton((Rectangle){ xLabel, y + font + espaco, 40, 24 }, "VOLTAR")) {
            currentScreen = MENU;
            estado = PARADO;
        }

        if (GuiLabelButton((Rectangle){ xLabel, y, 40, 24 }, "OPCOES")) {
            currentScreen = OPCOES;
            estado = PARADO;
        }

    }

    void telaPersonalizacao(Bolinha *bolinha){

        int larguraTela = GetScreenWidth();
        int alturaTela = GetScreenHeight();
        int espaco = 50;
        int largura = 400;
        int altura = 400;
        int x = larguraTela / 2 - largura / 2;
        int y = alturaTela / 2 - altura / 2;


        DrawRectangle(x + 150, y, largura, altura, BLACKTRANS);
        DrawRectangleLines(x + 150, y, largura, altura, WHITE);

        Color cor;

       // GuiColorPicker((Rectangle){ 10, 10, 200, 200 }, NULL,&cor);

        
        DrawCircle(x, GetScreenHeight()/2, 100, bolinha->cor);
    }

/*---------------------------------------------
 * Função do som 
 *-------------------------------------------*/
    void sons(int som){
        Sound batida = LoadSound("resources/sfx/batida.wav");
        Sound batidaTijolo = LoadSound("resources/sfx/batidaTijolo.wav");

        switch (som)
        {
        case 1:
            PlaySound(batida);
            break;
        case 2:
            PlaySound(batidaTijolo);
            break;
        default:
            break;
        }
    }
