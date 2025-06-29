/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Main function and logic for the game. Simplified template for game
 * development in C using Raylib (https://www.raylib.com/).
 * 
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


/*--------------------------------------------
 * Constants. 
 *------------------------------------------*/
const int PARADO = 0;
const int RODANDO = 1;

/*---------------------------------------------
 * Custom types (enums, structs, unions, etc.)
 *-------------------------------------------*/
typedef struct Jogador {
    Vector2 pos;
    Vector2 dim;
    float vel;
    int pontos;
    Color cor;
} Jogador;

typedef struct Bolinha {
    Vector2 pos;
    Vector2 vel;
    float raio;
    Color cor;
} Bolinha;

/*---------------------------------------------
 * Global variables.
 *-------------------------------------------*/
Jogador jogador1;
Jogador jogador2;
Bolinha bolinha;
int estado = PARADO;

/*---------------------------------------------
 * Function prototypes. 
 *-------------------------------------------*/
/**
 * @brief Reads user input and updates the state of the game.
 */
void update( float delta );

/**
 * @brief Draws the state of the game.
 */
void draw( void );

void atualizarJogador( Jogador *jogador, int teclaCima, int teclaBaixo, float delta );
void desenharJogador( Jogador *jogador );
void atualizarBolinha( Bolinha *bolinha, float delta );
void desenharBolinha( Bolinha *bolinha );
void resolverColisao( Bolinha *b, Jogador *j1, Jogador *j2 );
void desenharQuadra( int pontosJ1, int pontosJ2 );


/**
 * @brief Game entry point.
 */
int main( void ) {

    // local variables and initial user input

    // antialiasing
    SetConfigFlags( FLAG_MSAA_4X_HINT );

    // creates a new window 800 pixels wide and 450 pixels high
    InitWindow( 800, 450, "Pong" );

    // init audio device only if your game uses sounds
    //InitAudioDevice();

    // FPS: frames per second
    SetTargetFPS( 60 );    

    // you must load game resources here
    jogador1 = (Jogador) {
        .pos = {
            .x = 30,
            .y = GetScreenHeight() / 2 - 75
        },
        .dim = {
            .x = 30,
            .y = 150
        },
        .vel = 200,
        .pontos = 0,
        .cor = WHITE
    };

    jogador2 = (Jogador) {
        .pos = {
            .x = GetScreenWidth() - 60,
            .y = GetScreenHeight() / 2 - 75
        },
        .dim = {
            .x = 30,
            .y = 150
        },
        .vel = 200,
        .pontos = 0,
        .cor = WHITE
    };

    bolinha = (Bolinha) {
        .pos = {
            .x = GetScreenWidth() / 2,
            .y = GetScreenHeight() / 2
        },
        .vel = {
            .x = 200,
            .y = 200
        },
        .raio = 15,
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

    if ( estado == PARADO ) {

        if ( IsKeyPressed( KEY_ENTER ) ) {
            estado = RODANDO;
        }

    } else {
        atualizarJogador( &jogador1, KEY_W, KEY_S, delta );
        atualizarJogador( &jogador2, KEY_UP, KEY_DOWN, delta );
        atualizarBolinha( &bolinha, delta );
        resolverColisao( &bolinha, &jogador1, &jogador2 );
    }

}

void draw( void ) {

    BeginDrawing();
    ClearBackground( BLACK );

    desenharJogador( &jogador1 );
    desenharJogador( &jogador2 );
    desenharBolinha( &bolinha );
    desenharQuadra( jogador1.pontos, jogador2.pontos ); 

    EndDrawing();

}

void atualizarJogador( Jogador *jogador, int teclaCima, int teclaBaixo, float delta ) {

    if ( IsKeyDown( teclaCima ) ) {
        jogador->pos.y -= jogador->vel * delta;
    }

    if ( IsKeyDown( teclaBaixo ) ) {
        jogador->pos.y += jogador->vel * delta;
    }

    if ( jogador->pos.y < 0 ) {
        jogador->pos.y = 0;
    } else if ( jogador->pos.y + jogador->dim.y > GetScreenHeight() ) {
        jogador->pos.y = GetScreenHeight() - jogador->dim.y;
    }

}

void desenharJogador( Jogador *jogador ) {
    DrawRectangle(
        jogador->pos.x,
        jogador->pos.y,
        jogador->dim.x,
        jogador->dim.y,
        jogador->cor
    );
}

void atualizarBolinha( Bolinha *bolinha, float delta ) {

    bolinha->pos.x -= bolinha->vel.x * delta;
    bolinha->pos.y -= bolinha->vel.y * delta;

    if ( bolinha->pos.x + bolinha->raio > GetScreenWidth() ) {
        bolinha->pos.x = GetScreenWidth() / 2;
        bolinha->pos.y = GetScreenHeight() / 2;
        bolinha->vel.x = -200;
        bolinha->vel.y = GetRandomValue( 0, 1 ) == 0 ? 200 : 200;
        jogador1.pontos++;
        estado = PARADO;
    } else if ( bolinha->pos.x - bolinha->raio < 0 ) {
        bolinha->pos.x = GetScreenWidth() / 2;
        bolinha->pos.y = GetScreenHeight() / 2;
        bolinha->vel.x = 200;
        bolinha->vel.y = GetRandomValue( 0, 1 ) == 0 ? 200 : 200;
        jogador2.pontos++;
        estado = PARADO;
    }

    if ( bolinha->pos.y + bolinha->raio > GetScreenHeight() ) {
        bolinha->pos.y = GetScreenHeight() - bolinha->raio;
        bolinha->vel.y = -bolinha->vel.y;
    } else if ( bolinha->pos.y - bolinha->raio < 0 ) {
        bolinha->pos.y = bolinha->raio;
        bolinha->vel.y = -bolinha->vel.y;
    }


}

void desenharBolinha( Bolinha *bolinha ) {
    DrawCircle(
        bolinha->pos.x,
        bolinha->pos.y,
        bolinha->raio,
        bolinha->cor
    );
}

void resolverColisao( Bolinha *b, Jogador *j1, Jogador *j2 ) {

    bool c1 = CheckCollisionCircleRec(
        b->pos,
        b->raio,
        (Rectangle) {
            .x = j1->pos.x,
            .y = j1->pos.y,
            .width = j1->dim.x,
            .height = j1->dim.y
        }
    );

    bool c2 = CheckCollisionCircleRec(
        b->pos,
        b->raio,
        (Rectangle) {
            .x = j2->pos.x,
            .y = j2->pos.y,
            .width = j2->dim.x,
            .height = j2->dim.y
        }
    );

    if ( c1 ) {
        b->pos.x = j1->pos.x + j1->dim.x + b->raio;
        b->vel.x = -b->vel.x;
    } else if ( c2 ) {
        b->pos.x = j2->pos.x - b->raio;
        b->vel.x = -b->vel.x;
    }

}

void desenharQuadra( int pontosJ1, int pontosJ2 ) {

    const char *p1 = TextFormat( "%d", pontosJ1 );
    const char *p2 = TextFormat( "%d", pontosJ2 );
    int largP1 = MeasureText( p1, 40 );

    int mLarg = 5;
    int alt = 30;
    int centro = GetScreenWidth() / 2;

    DrawText( p1, centro - 30 - largP1, 30, 40, WHITE );
    DrawText( p2, centro + 30, 30, 40, WHITE );
    
    for ( int i = 0; i < 15; i += 2 ) {
        DrawRectangle(
            centro - mLarg,
            i * alt,
            mLarg * 2,
            alt,
            WHITE
        );  
    }

}
