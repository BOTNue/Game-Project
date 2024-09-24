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
} Bullet;


int main()
{
    // Setting up window
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "GAME PROJECT");
    SetTargetFPS(60);

    // Initializing player
    Player player = { {SCREENWIDTH / 2, SCREENHEIGHT * 0.8}, {200.0f, 200.0f}, 10};

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

        BeginDrawing();
        ClearBackground(BLACK);
        DrawCircleV(player.position, player.radius, WHITE);

        EndDrawing();
    }
}