#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 600
#define MAX_BULLETS 500

// Struct for player
typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float radius;
} Player;

// Struct for bullets
typedef struct 
{
    Vector2 position;
    Vector2 velocity;
    bool active;
    float lifetime;
    float radius;
} Bullet;


int main()
{
    // Setting up window
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "GAME PROJECT");
    SetTargetFPS(60);

    // Initializing player
    Player player = {{SCREENWIDTH / 2, SCREENHEIGHT * 0.8}, {200.0f, 200.0f}, 10};

    Bullet bullet [MAX_BULLETS];

    while(!WindowShouldClose())
    {   
        // Implementing player movement 
        if(IsKeyDown(KEY_W))
        {
            player.position.y -= player.velocity.y * GetFrameTime();
        }

        if(IsKeyDown(KEY_S))
        {
            player.position.y += player.velocity.y * GetFrameTime();
        }

        if(IsKeyDown(KEY_D))
        {
            player.position.x += player.velocity.x * GetFrameTime();
        }

        if(IsKeyDown(KEY_A))
        {
            player.position.x -= player.velocity.x * GetFrameTime();
        }

        // Player collisions

        if(player.position.y < 0)
        {
            player.position.y = 0;
        }

        if(player.position.y + player.radius > SCREENHEIGHT)
        {
            player.position.y = SCREENHEIGHT - player.radius;
        }

        if(player.position.x < 0)
        {
            player.position.x = 0;
        }

        if(player.position.x + player.radius > SCREENWIDTH)
        {
            player.position.x = SCREENWIDTH - player.radius;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawCircleV(player.position, player.radius, WHITE);

        EndDrawing();
    }
}