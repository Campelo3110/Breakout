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
 * Macros. 
 *-------------------------------------------*/

    //Definimos quantas linhas e colunas a grade de tijolo vai ter. 
    #define LINHAS_TIJOLOS 5
    #define COLUNAS_TIJOLOS 10

/*--------------------------------------------
 * Constants. 
 *------------------------------------------*/

/*---------------------------------------------
 * Custom types (enums, structs, unions, etc.)
 *-------------------------------------------*/

    //Estrutura para definir o estado do jogo
    typedef enum GameScreen {
        MENU,
        JOGANDO,
        OPCOES,
        SAIR
    } GameScreen;

    //Estrutura para definir oum tijolo
    typedef struct  Tijolo{
        Rectangle retan; // Retângulo
        Color cor;       // Cor do tijolo 
        bool ativo;      // Ativo ou quebrado
    } Tijolo;

/*---------------------------------------------
 * Global variables.
 *-------------------------------------------*/

    //O jogo começa na tela menu
    GameScreen currentScreen = MENU;

    //Declaração de um array 2d do tipo Tijolo, que representa uma grade de tijolos.
    Tijolo tijolos[LINHAS_TIJOLOS][COLUNAS_TIJOLOS];

    int volumeMusica = 70;
    int volumeEfeitos = 80;
    bool musicaAtiva = true;
    bool efeitosAtivos = true;
    bool fullscreen = false;

    int teclaEsquerda = KEY_LEFT; 
    int teclaDireita = KEY_RIGHT; 

    const int resolucoes[4][2] = {
        {800, 600},
        {1024, 768},
        {1280, 720},
        {1920, 1080}
    };

    int resolucaoSelecionada = 0;

/*---------------------------------------------
 * Function prototypes. 
 *-------------------------------------------*/

    void update( float delta );

    void draw( void );

    void inicializarTijolos();
    void desenharTijolos();
    void menuPrincipal();
    void menuOpcoes();

    int main( void ) {

        // local variables and initial user input

        // antialiasing
        SetConfigFlags( FLAG_MSAA_4X_HINT );

        InitWindow( resolucoes[resolucaoSelecionada][0], resolucoes[resolucaoSelecionada][0], "Breakout" );

        // init audio device only if your game uses sounds
        //InitAudioDevice();

        // FPS: frames per second
        SetTargetFPS( 60 );    

        // you must load game resources here
        inicializarTijolos();

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

    }

    void draw( void ) {

        BeginDrawing();
        ClearBackground( BLACK );

        //Switch responsavel por mudar as telas
        switch (currentScreen) {
            case MENU:
                menuPrincipal();
                break;
            case JOGANDO:
                desenharTijolos();
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
                    ); //Função da RayLib para desenhar um retânglo.
                }
            }
        }
    }

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

    void menuOpcoes(){
        int larguraTela = GetScreenWidth();
        int alturaTela = GetScreenHeight();

        DrawText("Em construcao", larguraTela / 2, alturaTela / 2, 40, WHITE);
    }