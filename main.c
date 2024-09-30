#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 600

#define MAX_BULLETS 500
#define MAX_ENEMIES 50

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

// Struct for enemies
typedef struct
{
    Vector2 position;
    Vector2 velocity;
    bool active;
} Enemy;


// Initializing bullets
Bullet bullet [MAX_BULLETS] = { 0 };

// Initializing enemies
Enemy enemy [MAX_ENEMIES] = { 0 };


// Shoot bullets
void shoot_bullets(Vector2 position, Vector2 direction)
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
void update_bullets()
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullet[i].active)
        {
            bullet[i].position.x += bullet[i].velocity.x * GetFrameTime();
            bullet[i].position.y += bullet[i].velocity.y * GetFrameTime();

            if(bullet[i].position.x > SCREENWIDTH || bullet[i].position.x < 0 || 
            bullet[i].position.y > SCREENHEIGHT || bullet[i].position.y < 0)
            {
                bullet[i].active = false;
            }

            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemy[j].active) {
                if (CheckCollisionCircleRec(bullet[i].position, 5, (Rectangle){enemy[j].position.x, enemy[j].position.y, 32, 10})) {
                    bullet[i].active = false;
                    enemy[j].active = false;
                }

                }
            }
        }
    }
}

// Draw bullets 
void draw_bullets()
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullet[i].active)
        {
            DrawCircleV(bullet[i].position, 5, RED);
        }
    }
}

// Spawning enemies
void spawn_enemy(Vector2 position, Vector2 direction)
{
    for(int i = 0; i < MAX_ENEMIES; i++)
    {
        if(!enemy[i].active)
        {
            enemy[i].position = position;
            enemy[i].velocity = direction;
            enemy[i].active = true;
            return;
        }
    }
}

// Update enemies
void update_enemy()
{
    for(int i = 0; i < MAX_ENEMIES; i++)
    {
        if(enemy[i].active)
        {
            enemy[i].position.x += enemy[i].velocity.x * GetFrameTime();
            enemy[i].position.y += enemy[i].velocity.y * GetFrameTime();

            // If the enemy goes outside the screen they will deactivate 
            if(enemy[i].position.x > SCREENWIDTH || enemy[i].position.x < 0 || 
            enemy[i].position.y > SCREENHEIGHT || enemy[i].position.y < 0)
            {
                enemy[i].active = false;
            }
        }
    }
}

// Draw enemies
void draw_enemy()
{
   for(int i = 0; i < MAX_ENEMIES; i++)
   {
        if(enemy[i].active)
        {
            DrawRectangleV(enemy[i].position, (Vector2) {32, 10}, WHITE);
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

        // Player movement border
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

        // Player enemy collision
        for(int i = 0; i < MAX_ENEMIES; i++) 
        {
            if(enemy[i].active)
            {
                if(CheckCollisionCircleRec(player.position, player.radius, (Rectangle){enemy[i].position.x, enemy[i].position.y, 32, 10}))
                {
                    enemy[i].active = false;
                }
            }
        }

        // Shooting and cooldown logic
        if(last_shot >= shoot_cooldown)
        {
                if(IsKeyDown(KEY_SPACE))
            {
                Vector2 bulletdirection = {0, -400};
                shoot_bullets(player.position, bulletdirection);
                last_shot = 0.0f;
            }
        }
        else
        {
            last_shot += deltatime;
        }
        
        // Spawning enemy logic
        int randomx = GetRandomValue(0, SCREENWIDTH);
        Vector2 enemyposition = {randomx, 0};
        Vector2 enemydirection = {0, 100};
        spawn_enemy(enemyposition, enemydirection);
        
        update_bullets();
        update_enemy();

        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawCircleV(player.position, player.radius, WHITE);

        draw_bullets();

        draw_enemy();

        EndDrawing();
    }
    CloseWindow();
}