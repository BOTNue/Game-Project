#include "raylib.h"
#include <stdio.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 600

// Struct for player
typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float move_speed;
    float radius;
} Player;


int main()
{
    // Setting up window
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "GAME PROJECT");
    SetTargetFPS(60);

    // initializing objects 
    Player player = { {SCREENWIDTH / 2, SCREENHEIGHT * 0.8}, {0, 0}, 200.0f, 10};

    while(!WindowShouldClose())
    {   
        // Implementing player movement 
        if(IsKeyDown(KEY_W))
        {
            player.position.y -= player.move_speed * GetFrameTime();
        }

        if(IsKeyDown(KEY_S))
        {
            player.position.y += player.move_speed * GetFrameTime();
        }

        if(IsKeyDown(KEY_D))
        {
            player.position.x += player.move_speed * GetFrameTime();
        }

        if(IsKeyDown(KEY_A))
        {
            player.position.x -= player.move_speed * GetFrameTime();
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawCircleV(player.position, player.radius, WHITE);

        EndDrawing();
    }
}