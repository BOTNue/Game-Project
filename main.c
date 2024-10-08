#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 600

#define MAX_BULLETS 500
#define MAX_ENEMIES 50
#define MAX_ELITE 20
#define MAX_EXPLOSION 50

#define MAX_WAVES 20
// Enum for different projectile types
typedef enum
{
    weapon_bullet,
    weapon_laser,
    weapon_rocket
} Projectile_type;

// Struct for weapons
typedef struct
{
    float rate_of_fire;
    float projectile_speed;
    float projectile_size;
    float dps;
    Projectile_type projectile_type;
} Weapon;

// Struct for player
typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float radius;
    float hp;
    float max_hp;
    float damage;
} Player;

// Struct for bullets
typedef struct
{
    Vector2 position;
    Vector2 velocity;
    bool active;
} Bullet;

// Struct for explosions
typedef struct
{
    Vector2 position;
    float radius;
    bool active;
    float lifetime;
} Explosion;

// Struct for enemies
typedef struct
{
    Vector2 position;
    Vector2 velocity;
    bool active;
    float hp;
} Enemy;

// Struct for elite enemies
typedef struct
{
    Vector2 position;
    Vector2 velocity;
    bool active;
    float hp;
} Elite_enemy;

// Initializing bullets
Bullet bullet[MAX_BULLETS] = {0};

// Initializing explosions
Explosion explosion[MAX_EXPLOSION] = {0};

// Initializing enemies
Enemy enemy[MAX_ENEMIES] = {0};

// Initializing elite enemies
Elite_enemy elite[MAX_ELITE] = {0};

Weapon weapons[9] = {0};

void initialize_weapon(Weapon *weapons)
{
    // Basic fast firing low damaging
    weapons[0] = (Weapon){
        .rate_of_fire = 0.1f,
        .projectile_speed = 350.0f,
        .projectile_size = 5.0f,
        .dps = 15.0f,
        .projectile_type = weapon_bullet,
    };

    // Slow firing high damage
    weapons[1] = (Weapon){
        .rate_of_fire = 1.25f,
        .projectile_speed = 500.0f,
        .projectile_size = 12.0f,
        .dps = 100.0f,
        .projectile_type = weapon_bullet,
    };

    // Medium firing and mid damage
    weapons[2] = (Weapon){
        .rate_of_fire = 0.25f,
        .projectile_speed = 375.0f,
        .projectile_size = 7.5f,
        .dps = 22.0f,
        .projectile_type = weapon_bullet,
    };

    // Laser weapon
    weapons[3] = (Weapon){
        .rate_of_fire = 0.0f,
        .projectile_speed = 700.0f,
        .projectile_size = 6.0f,
        .dps = 7.5f,
        .projectile_type = weapon_laser,
    };

    // Rocket weapon
    weapons[4] = (Weapon){
        .rate_of_fire = 0.75f,
        .projectile_speed = 450.0f,
        .projectile_size = 9.0f,
        .dps = 50.0f,
        .projectile_type = weapon_rocket,
    };
}

// Switching weapons
int current_weapon_index = 0;

void switch_weapons()
{
    if (IsKeyPressed(KEY_ONE))
    {
        current_weapon_index = 0;
    }
    if (IsKeyPressed(KEY_TWO))
    {
        current_weapon_index = 1;
    }
    if (IsKeyPressed(KEY_THREE))
    {
        current_weapon_index = 2;
    }
    if (IsKeyPressed(KEY_FOUR))
    {
        current_weapon_index = 3;
    }
    if (IsKeyPressed(KEY_FIVE))
    {
        current_weapon_index = 4;
    }
}

// Creating explosion
void explode(Vector2 position, float radius)
{
    for (int i = 0; i < MAX_EXPLOSION; i++)
    {
        if (!explosion[i].active)
        {
            explosion[i] = (Explosion){
                .position = position,
                .radius = radius,
                .active = true,
                .lifetime = 0.75f};
            return;
        }
    }
}

// Update explosion
void update_explode(Weapon current_weapon)
{
    for (int i = 0; i < MAX_EXPLOSION; i++)
    {
        if (explosion[i].active)
        {
            explosion[i].lifetime -= GetFrameTime();

            if (explosion[i].lifetime < 0.0f)
            {
                explosion[i].active = false;
                continue;
            }

            for (int j = 0; j < MAX_ENEMIES; j++)
            {
                if (enemy[j].active)
                {
                    if (CheckCollisionCircleRec(explosion[i].position, explosion[i].radius, (Rectangle){enemy[j].position.x, enemy[j].position.y, 32, 10}))
                    {
                        enemy[j].hp -= current_weapon.dps;

                        if (enemy[j].hp <= 0)
                        {
                            enemy[j].active = false;
                        }
                    }
                }
            }
        }
    }
}

void draw_explosion()
{
    for (int i = 0; i < MAX_EXPLOSION; i++)
    {
        if (explosion[i].active)
        {
            DrawCircleV(explosion[i].position, explosion[i].radius, YELLOW);
        }
    }
}

// Shoot bullets
void shoot_bullets(Vector2 position, Vector2 direction, Weapon current_weapon)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullet[i].active)
        {
            bullet[i].position = position;
            bullet[i].velocity = (Vector2){direction.x * current_weapon.projectile_speed, direction.y * current_weapon.projectile_speed};
            bullet[i].active = true;
            break;
        }
    }
}

// Update bullets
void update_bullets(Weapon current_weapon)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullet[i].active)
        {
            bullet[i].position.x += bullet[i].velocity.x * GetFrameTime();
            bullet[i].position.y += bullet[i].velocity.y * GetFrameTime();

            if (bullet[i].position.x > SCREENWIDTH || bullet[i].position.x < 0 ||
                bullet[i].position.y > SCREENHEIGHT || bullet[i].position.y < 0)
            {
                bullet[i].active = false;
            }

            for (int j = 0; j < MAX_ENEMIES; j++)
            {
                if (enemy[j].active)
                {
                    if (CheckCollisionCircleRec(bullet[i].position, 5, (Rectangle){enemy[j].position.x, enemy[j].position.y, 32, 10}))
                    {
                        if (current_weapon.projectile_type == weapon_rocket)
                        {
                            explode(bullet[i].position, current_weapon.projectile_size * 2);
                        }

                        bullet[i].active = false;
                        enemy[j].hp -= current_weapon.dps;
                        if (enemy[j].hp <= 0)
                        {
                            enemy[j].active = false;
                        }
                    }
                }
            }
        }
    }
}

// Draw bullets
void draw_bullets(Weapon current_weapon)
{

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullet[i].active)
        {
            switch (current_weapon.projectile_type)
            {
            case weapon_bullet:
            {
                DrawCircleV(bullet[i].position, current_weapon.projectile_size, RED);
            }
            break;

            case weapon_laser:
            {
                DrawRectangleV(bullet[i].position, (Vector2){current_weapon.projectile_size, current_weapon.projectile_size}, BLUE);
            }
            break;

            case weapon_rocket:
            {
                Vector2 v1 = {bullet[i].position.x, bullet[i].position.y - current_weapon.projectile_size};
                Vector2 v2 = {bullet[i].position.x - current_weapon.projectile_size, bullet[i].position.y + current_weapon.projectile_size};
                Vector2 v3 = {bullet[i].position.x + current_weapon.projectile_size, bullet[i].position.y + current_weapon.projectile_size};
                DrawTriangle(v1, v2, v3, RED);
            }
            break;

            default:
                DrawCircleV(bullet[i].position, current_weapon.projectile_size, RED);
                break;
            }
        }
    }
}

// Spawning enemies
void spawn_enemy(Vector2 position, Vector2 direction)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemy[i].active)
        {
            enemy[i] = (Enemy){
                .position = position,
                .velocity = direction,
                .active = true,
                .hp = 50.0f};

            return;
        }
    }
}

// Update enemies
void update_enemy()
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemy[i].active)
        {
            enemy[i].position.x += enemy[i].velocity.x * GetFrameTime();
            enemy[i].position.y += enemy[i].velocity.y * GetFrameTime();

            // If the enemy goes outside the screen they will deactivate
            if (enemy[i].position.x > SCREENWIDTH || enemy[i].position.x < 0 ||
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
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemy[i].active)
        {
            DrawRectangleV(enemy[i].position, (Vector2){32, 10}, WHITE);
        }
    }
}

// Spawning elite enemies
void spawn_elite(Vector2 position, Vector2 direction)
{
    for (int i = 0; i < MAX_ELITE; i++)
    {
        if (!elite[i].active)
        {
            elite[i] = (Elite_enemy){
                .position = position,
                .velocity = direction,
                .active = true,
                .hp = 250.0f};

            return;
        }
    }
}

// Update elite enemies
void update_elite()
{
    for (int i = 0; i < MAX_ELITE; i++)
    {
        if (elite[i].active)
        {
            elite[i].position.x += elite[i].velocity.x * GetFrameTime();
            elite[i].position.y += elite[i].velocity.y * GetFrameTime();

            // If the enemy goes outside the screen they will deactivate
            if (elite[i].position.x > SCREENWIDTH || elite[i].position.x < 0 ||
                elite[i].position.y > SCREENHEIGHT || elite[i].position.y < 0)
            {
                elite[i].active = false;
            }
        }
    }
}

// Draw elite enemies
void draw_elite()
{
    for (int i = 0; i < MAX_ELITE; i++)
    {
        if (elite[i].active)
        {
            DrawRectangleV(elite[i].position, (Vector2){40, 12}, WHITE);
        }
    }
}

int main()
{
    // Setting up window
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "GAME PROJECT");
    SetTargetFPS(60);

    // Initializing player
    Player player = {
        .position = {SCREENWIDTH / 2, SCREENHEIGHT * 0.8},
        .velocity = {200.0f, 200.0f},
        .radius = 10,
        .hp = 100,
        .max_hp = 100,
        .damage = 10};

    initialize_weapon(weapons);

    float last_shot = 0.0f;

    while (!WindowShouldClose())
    {
        float deltatime = GetFrameTime();

        // Implementing player movement
        if (IsKeyDown(KEY_W))
        {
            player.position.y -= player.velocity.y * GetFrameTime();
        }

        if (IsKeyDown(KEY_S))
        {
            player.position.y += player.velocity.y * GetFrameTime();
        }

        if (IsKeyDown(KEY_D))
        {
            player.position.x += player.velocity.x * GetFrameTime();
        }

        if (IsKeyDown(KEY_A))
        {
            player.position.x -= player.velocity.x * GetFrameTime();
        }

        // Player movement border
        if (player.position.y < SCREENHEIGHT * 0.6)
        {
            player.position.y = SCREENHEIGHT * 0.6;
        }

        if (player.position.y + player.radius > SCREENHEIGHT)
        {
            player.position.y = SCREENHEIGHT - player.radius;
        }

        if (player.position.x < 0 + player.radius)
        {
            player.position.x = 0 + player.radius;
        }

        if (player.position.x + player.radius > SCREENWIDTH)
        {
            player.position.x = SCREENWIDTH - player.radius;
        }

        // Player enemy collision
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemy[i].active)
            {
                if (CheckCollisionCircleRec(player.position, player.radius, (Rectangle){enemy[i].position.x, enemy[i].position.y, 32, 10}))
                {
                    enemy[i].active = false;
                    player.hp -= 10;
                }
            }
        }

        switch_weapons();

        Weapon current_weapon = weapons[current_weapon_index];

        // Shooting and cooldown logic
        if (last_shot >= current_weapon.rate_of_fire)
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                Vector2 bullet_direction = {0, -1};
                shoot_bullets(player.position, bullet_direction, current_weapon);
                last_shot = 0.0f;
            }
        }
        else
        {
            last_shot += deltatime;
        }

        int random_x = GetRandomValue(0, SCREENWIDTH);
        // Enemy logic
        Vector2 enemy_position = {random_x, 0};
        Vector2 enemy_direction = {0, 100};
        spawn_enemy(enemy_position, enemy_direction);

        Vector2 elite_position = {random_x, 15};
        Vector2 elite_direction = {0, 0};
        spawn_enemy(elite_position, elite_direction);

        update_bullets(current_weapon);
        update_enemy();
        update_explode(current_weapon);

        BeginDrawing();
        ClearBackground(BLACK);

        DrawCircleV(player.position, player.radius, WHITE);

        draw_bullets(current_weapon);

        draw_enemy();

        draw_elite();

        draw_explosion();

        DrawText(TextFormat("Weapon id: %d", current_weapon_index + 1), 8, 42, 20, WHITE);

        static const int hp_bar_rec_width = 300;
        Rectangle hp_bar_rec = {
            .x = 8,
            .y = 8,
            .height = 32,
            .width = (player.hp / player.max_hp) * hp_bar_rec_width};
        DrawRectangleRec(hp_bar_rec, RED);

        EndDrawing();
    }
    CloseWindow();
}