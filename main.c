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

// Shoot bullets
void shoot_bullets(Bullet bullet[], Vector2 position, Vector2 direction)
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(!bullet[i].active)
        {
            bullet[i].position = position;
            bullet[i].velocity = direction;
            bullet[i].active = true;
            break;
        }
    }
}

// Update bullets
void update_bullets(Bullet bullet[])
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullet[i].active)
        {
            bullet[i].position.x += bullet[i].velocity.x * GetFrameTime();
            bullet[i].position.y += bullet[i].velocity.y * GetFrameTime();

            if(bullet[i].position.x > SCREENWIDTH || bullet[i].position.x < 0 || bullet[i].position.y > SCREENHEIGHT || bullet[i].position.y < 0)
            {
                bullet[i].active = false;
            }
        }
    }
}

// Draw bullets 
void draw_bullets(Bullet bullet[])
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullet[i].active)
        {
            DrawCircleV(bullet[i].position, 5, RED);
        }
    }
}

int main()
{
    // Setting up window
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "GAME PROJECT");
    SetTargetFPS(60);

    // Initializing player
    Player player = {{SCREENWIDTH / 2, SCREENHEIGHT * 0.8}, {200.0f, 200.0f}, 10};

    // Defining bullets 
    Bullet bullet [MAX_BULLETS];

    float shoot_cooldown = 0.15f;

    float last_shot = 0.0f;

    while(!WindowShouldClose())
    {   
        float deltatime = GetFrameTime();

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

        if(player.position.y < SCREENHEIGHT * 0.6)
        {
            player.position.y = SCREENHEIGHT * 0.6;
        }

        if(player.position.y + player.radius > SCREENHEIGHT)
        {
            player.position.y = SCREENHEIGHT - player.radius;
        }

        if(player.position.x < 0 + player.radius)
        {
            player.position.x = 0 + player.radius;
        }

        if(player.position.x + player.radius > SCREENWIDTH)
        {
            player.position.x = SCREENWIDTH - player.radius;
        }

        // Shooting and cooldown logic
        if (last_shot >= shoot_cooldown)
        {
                if(IsKeyDown(KEY_SPACE))
            {
                Vector2 bulletdirection = {0, -400};
                shoot_bullets(bullet, player.position, bulletdirection);
                last_shot = 0.0f;
            }
        }
        else
        {
            last_shot += deltatime;
        }
        

        update_bullets(bullet);

        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawCircleV(player.position, player.radius, WHITE);

        draw_bullets(bullet);

        EndDrawing();
    }
}