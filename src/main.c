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

//Definimos a estrutura de um tijolo
typedef struct  Tijolo{
    Rectangle retan; // Retângulo
    Color cor;       // Cor do tijolo 
    bool ativo;      // Ativo ou quebrado
} Tijolo;

//Declaração de um array 2d do tipo Tijolo, que representa uma grade de tijolos.
Tijolo tijolos[LINHAS_TIJOLOS][COLUNAS_TIJOLOS];


void update( float delta );

void draw( void );

void inicializarTijolos();
void desenharTijolos();

int main( void ) {

    // local variables and initial user input

    // antialiasing
    SetConfigFlags( FLAG_MSAA_4X_HINT );

    // creates a new window 800 pixels wide and 450 pixels high
    InitWindow( 800, 450, "Breakout" );

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

    desenharTijolos();

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