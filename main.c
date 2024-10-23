#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 600

#define MAX_BULLETS 500
#define MAX_ENEMY_BULLETS 500
#define MAX_ENEMIES 50
#define MAX_ELITE 20
#define MAX_EXPLOSION 50
#define MAX_BOSSES 1
#define MAX_POWERUPS 10

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

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    bool active;
    float radius;
    float lifetime;
} Powerup;

// Struct for enemies
typedef struct
{
    Vector2 position;
    Vector2 velocity1;
    Vector2 velocity2;
    bool active;
    float hp;
    bool moving_down;
    float last_shot;
} Enemy;

// Struct for elite enemies
typedef struct
{
    Vector2 position;
    Vector2 velocity1;
    Vector2 velocity2;
    bool active;
    float hp;
    bool moving_down;
    float last_shot;
} Elite_enemy;

// Struct for boss enemies
typedef struct
{
    Vector2 position;
    Vector2 velocity1;
    Vector2 velocity2;
    bool active;
    float hp;
    bool moving_down;
    float last_shot;
} Boss_enemy;

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    bool active;
} Enemy_bullet;

typedef struct
{
    int num_enemy;
    int num_elite;
    int num_boss;
    float spawn_rate;
    bool active;
} Game_wave;

// Initializing bullets
Bullet bullet[MAX_BULLETS] = {0};

// Initializing explosions
Explosion explosion[MAX_EXPLOSION] = {0};

// Initializing powerups
Powerup powerup[MAX_POWERUPS] = {0};

// Initializing enemies
Enemy enemy[MAX_ENEMIES] = {0};
int g_curr_num_enemies = 0;

// Initializing elite enemies
Elite_enemy elite[MAX_ELITE] = {0};

// Initializing boss enemy
Boss_enemy boss[MAX_BOSSES] = {0};

Enemy_bullet enemy_bullet[MAX_ENEMY_BULLETS] = {9};

Game_wave wave[MAX_WAVES] = {0};

Weapon weapons[9] = {0};

// Initializing player
Player player = {
    .position = {SCREENWIDTH / 2, SCREENHEIGHT * 0.8},
    .velocity = {200.0f, 200.0f},
    .radius = 10,
    .hp = 100,
    .max_hp = 100,
    .damage = 10};

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

                            g_curr_num_enemies--;
                        }
                    }
                }
            }

            for (int j = 0; j < MAX_ELITE; j++)
            {
                if (elite[j].active)
                {
                    if (CheckCollisionCircleRec(explosion[i].position, explosion[i].radius, (Rectangle){elite[j].position.x, elite[j].position.y, 32, 10}))
                    {
                        elite[j].hp -= current_weapon.dps;

                        if (elite[j].hp <= 0)
                        {
                            elite[j].active = false;
                        }
                    }
                }
            }

            for (int j = 0; j < MAX_BOSSES; j++)
            {
                if (boss[j].active)
                {
                    if (CheckCollisionCircleRec(explosion[i].position, explosion[i].radius, (Rectangle){boss[j].position.x, boss[j].position.y, 32, 10}))
                    {
                        boss[j].hp -= current_weapon.dps;

                        if (boss[j].hp <= 0)
                        {
                            boss[j].active = false;
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

void create_powerup(Vector2 position, Vector2 direction)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!powerup[i].active)
        {
            powerup[i] = (Powerup){
                .position = position,
                .velocity = direction,
                .active = true,
                .radius = 5,
                .lifetime = 5.0f,
            };
            return;
        }
    }
}

void update_powerup(Weapon *current_weapon, Player player)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (powerup[i].active)
        {
            powerup[i].position.x += powerup[i].velocity.x * GetFrameTime();
            powerup[i].position.y += powerup[i].velocity.y * GetFrameTime();

            if (powerup[i].position.x > SCREENWIDTH || powerup[i].position.x < 0 ||
                powerup[i].position.y > SCREENHEIGHT || powerup[i].position.y < 0)
            {
                powerup[i].active = false;
            }

            switch (current_weapon->projectile_type)
            {
            case weapon_bullet:
            {
                if (CheckCollisionCircles(powerup[i].position, powerup[i].radius, player.position, player.radius))
                {
                    current_weapon->rate_of_fire /= 2.0f;
                    powerup[i].active = false;
                }
            }
            break;
            case weapon_laser:
            {
                if (CheckCollisionCircles(powerup[i].position, powerup[i].radius, player.position, player.radius))
                {
                    current_weapon->dps *= 2.0f;
                    powerup[i].active = false;
                }
            }
            break;
            case weapon_rocket:
            {
                if (CheckCollisionCircles(powerup[i].position, powerup[i].radius, player.position, player.radius))
                {
                    current_weapon->projectile_size *= 2.0f;
                    powerup[i].active = false;
                }
            }
            break;
            default:
            {
                if (CheckCollisionCircles(powerup[i].position, powerup[i].radius, player.position, player.radius))
                {
                    current_weapon->rate_of_fire /= 2.0f;
                    powerup[i].active = false;
                }
            }
            break;
            }
        }
    }
}

void draw_powerup()
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (powerup[i].active)
        {
            DrawCircleV(powerup[i].position, powerup[i].radius, YELLOW);
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
                    if (CheckCollisionCircleRec(bullet[i].position, current_weapon.projectile_size, (Rectangle){enemy[j].position.x, enemy[j].position.y, 32, 10}))
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
                            g_curr_num_enemies--;

                            int chance = GetRandomValue(0, 100);
                            if (chance < 3)
                            {
                                Vector2 powerup_position = enemy[j].position;
                                Vector2 powerup_direction = {0, 50};
                                create_powerup(powerup_position, powerup_direction);
                            }
                        }
                    }
                }
            }

            for (int j = 0; j < MAX_ELITE; j++)
            {
                if (elite[j].active)
                {
                    if (CheckCollisionCircleRec(bullet[i].position, current_weapon.projectile_size, (Rectangle){elite[j].position.x, elite[j].position.y, 40, 12}))
                    {
                        if (current_weapon.projectile_type == weapon_rocket)
                        {
                            explode(bullet[i].position, current_weapon.projectile_size * 2);
                        }

                        bullet[i].active = false;
                        elite[j].hp -= current_weapon.dps;
                        if (elite[j].hp <= 0)
                        {
                            elite[j].active = false;
                        }
                    }
                }
            }

            for (int j = 0; j < MAX_BOSSES; j++)
            {
                if (boss[j].active)
                {
                    if (CheckCollisionCircleRec(bullet[i].position, current_weapon.projectile_size, (Rectangle){boss[j].position.x, boss[j].position.y, 75, 32}))
                    {
                        if (current_weapon.projectile_type == weapon_rocket)
                        {
                            explode(bullet[i].position, current_weapon.projectile_size * 2);
                        }

                        bullet[i].active = false;
                        boss[j].hp -= current_weapon.dps;
                        if (boss[j].hp <= 0)
                        {
                            boss[j].active = false;
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

void enemy_shoot_bullets(Vector2 position, Vector2 direction)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!enemy_bullet[i].active)
        {
            enemy_bullet[i].position = position;
            enemy_bullet[i].velocity = direction;
            enemy_bullet[i].active = true;
            break;
        }
    }
}

void update_enemy_bullet()
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (enemy_bullet[i].active)
        {
            enemy_bullet[i].position.x += enemy_bullet[i].velocity.x * GetFrameTime();
            enemy_bullet[i].position.y += enemy_bullet[i].velocity.y * GetFrameTime();

            if (enemy_bullet[i].position.x > SCREENWIDTH || enemy_bullet[i].position.x < 0 ||
                enemy_bullet[i].position.y > SCREENHEIGHT || enemy_bullet[i].position.y < 0)
            {
                enemy_bullet[i].active = false;
            }

            if (CheckCollisionCircles(enemy_bullet[i].position, 5, player.position, player.radius))
            {
                player.hp -= 10;
                enemy_bullet[i].active = false;
                if (player.hp <= 0)
                {
                }
            }
        }
    }
}

void draw_enemy_bullet()
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (enemy_bullet[i].active)
        {
            DrawCircleV(enemy_bullet[i].position, 5, PURPLE);
        }
    }
}

// Spawning enemies
void spawn_enemy(Vector2 position, Vector2 direction_down, Vector2 direction_up)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemy[i].active)
        {
            enemy[i] = (Enemy){
                .position = position,
                .velocity1 = direction_down,
                .velocity2 = direction_up,
                .active = true,
                .hp = 50.0f,
                .moving_down = true,
                .last_shot = 0.0f};

            g_curr_num_enemies++;

            return;
        }
    }
}

float time_since_last_shot = 2.0f;
// Update enemies
void update_enemy(float deltatime)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemy[i].active)
        {
            if (enemy[i].moving_down)
            {
                enemy[i].position.x += enemy[i].velocity1.x * GetFrameTime();
                enemy[i].position.y += enemy[i].velocity1.y * GetFrameTime();

                if (enemy[i].position.y >= SCREENHEIGHT - 10)
                {
                    enemy[i].moving_down = false;
                }
            }
            else
            {
                enemy[i].position.x += enemy[i].velocity2.x * GetFrameTime();
                enemy[i].position.y += enemy[i].velocity2.y * GetFrameTime();

                if (enemy[i].position.y <= 0)
                {
                    enemy[i].moving_down = true;
                }
            }

            // If the enemy goes outside the screen they will deactivate
            if (enemy[i].position.x > SCREENWIDTH || enemy[i].position.x < 0)
            {
                enemy[i].active = false;
                g_curr_num_enemies--;
            }

            if (enemy[i].last_shot >= time_since_last_shot)
            {
                Vector2 bullet_direction = {0, 200};
                enemy_shoot_bullets(enemy[i].position, bullet_direction);
                enemy[i].last_shot = 0.0f;
            }
            else
            {
                enemy[i].last_shot += deltatime;
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
void spawn_elite(Vector2 position, Vector2 direction_down, Vector2 direction_up)
{
    for (int i = 0; i < MAX_ELITE; i++)
    {
        if (!elite[i].active)
        {
            elite[i] = (Elite_enemy){
                .position = position,
                .velocity1 = direction_down,
                .velocity2 = direction_up,
                .active = true,
                .hp = 250.0f,
                .moving_down = true,
                .last_shot = 0.0f};

            return;
        }
    }
}

// Update elite enemies
void update_elite(float deltatime)
{
    for (int i = 0; i < MAX_ELITE; i++)
    {
        if (elite[i].active)
        {
            if (elite[i].moving_down)
            {
                elite[i].position.x += elite[i].velocity1.x * GetFrameTime();
                elite[i].position.y += elite[i].velocity1.y * GetFrameTime();

                if (elite[i].position.y >= SCREENHEIGHT - 10)
                {
                    elite[i].moving_down = false;
                }
            }
            else
            {
                elite[i].position.x += elite[i].velocity2.x * GetFrameTime();
                elite[i].position.y += elite[i].velocity2.y * GetFrameTime();

                if (elite[i].position.y <= 0)
                {
                    elite[i].moving_down = true;
                }
            }

            // If the enemy goes outside the screen they will deactivate
            if (elite[i].position.x > SCREENWIDTH || elite[i].position.x < 0)
            {
                elite[i].active = false;
            }

            if (elite[i].last_shot >= time_since_last_shot)
            {
                Vector2 bullet_direction = {0, 200};
                enemy_shoot_bullets(elite[i].position, bullet_direction);
                elite[i].last_shot = 0.0f;
            }
            else
            {
                elite[i].last_shot += deltatime;
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
            DrawRectangleV(elite[i].position, (Vector2){40, 12}, RED);
        }
    }
}

// Spawning boss enemy
void spawn_boss(Vector2 position, Vector2 direction_down, Vector2 direction_up)
{
    for (int i = 0; i < MAX_BOSSES; i++)
    {
        if (!boss[i].active)
        {
            boss[i] = (Boss_enemy){
                .position = position,
                .velocity1 = direction_down,
                .velocity2 = direction_up,
                .active = true,
                .hp = 750.0f,
                .moving_down = true,
                .last_shot = 0.0f};

            return;
        }
    }
}

// Update elite enemies
void update_boss(float deltatime)
{
    for (int i = 0; i < MAX_BOSSES; i++)
    {
        if (boss[i].active)
        {
            if (boss[i].moving_down)
            {
                boss[i].position.x += boss[i].velocity1.x * GetFrameTime();
                boss[i].position.y += boss[i].velocity1.y * GetFrameTime();

                if (boss[i].position.y >= SCREENHEIGHT - 10)
                {
                    boss[i].moving_down = false;
                }
            }
            else
            {
                boss[i].position.x += boss[i].velocity2.x * GetFrameTime();
                boss[i].position.y += boss[i].velocity2.y * GetFrameTime();

                if (boss[i].position.y <= 0)
                {
                    boss[i].moving_down = true;
                }
            }

            // If the enemy goes outside the screen they will deactivate
            if (boss[i].position.x > SCREENWIDTH || boss[i].position.x < 0 ||
                boss[i].position.y > SCREENHEIGHT || boss[i].position.y < 0)
            {
                boss[i].active = false;
            }

            if (boss[i].last_shot >= time_since_last_shot)
            {
                Vector2 bullet_direction = {0, 200};
                enemy_shoot_bullets(boss[i].position, bullet_direction);
                boss[i].last_shot = 0.0f;
            }
            else
            {
                boss[i].last_shot += deltatime;
            }
        }
    }
}

// Draw elite enemies
void draw_boss()
{
    for (int i = 0; i < MAX_BOSSES; i++)
    {
        if (boss[i].active)
        {
            DrawRectangleV(boss[i].position, (Vector2){75, 32}, GREEN);
        }
    }
}

void initialize_waves()
{
    for (int i = 0; i < MAX_WAVES; i++)
    {
        wave[i] = (Game_wave){
            .num_enemy = 10 + i * 4,         // Starts with a base of 10 enemies and adds 4 with each wave.
            .num_elite = i > 3 ? i / 2 : 0,  // Elite start spawning on wave 5
            .num_boss = i == 20 - 1 ? 1 : 0, // One boss spawns on wave 20
            .spawn_rate = 1.0f - i * 0.5f,
            .active = false};
    }
}

int current_wave_index = 0;
float time_since_last_spawn = 0.0f;

void wave_progress()
{
    Game_wave *current_wave = &wave[current_wave_index];
    if (current_wave->active)
    {
        time_since_last_spawn = 0.0f;

        for (int i = 0; i < current_wave->num_enemy; i++)
        {
            Vector2 enemy_position = {GetRandomValue(0, SCREENWIDTH - 32), 5};
            Vector2 enemy_direction_down = {0, 100};
            Vector2 enemy_direction_up = {0, -100};
            spawn_enemy(enemy_position, enemy_direction_down, enemy_direction_up);
        }

        for (int j = 0; j < current_wave->num_elite; j++)
        {
            Vector2 elite_position = {GetRandomValue(0, SCREENWIDTH - 40), 5};
            Vector2 elite_direction_down = {0, 50};
            Vector2 elite_direction_up = {0, -50};
            spawn_elite(elite_position, elite_direction_down, elite_direction_up);
        }

        for (int k = 0; k < current_wave->num_boss; k++)
        {
            Vector2 boss_position = {SCREENWIDTH / 2, 5};
            Vector2 boss_direction_down = {0, 20};
            Vector2 boss_dircetion_up = {0, -20};
            spawn_boss(boss_position, boss_direction_down, boss_dircetion_up);
        }

        current_wave->active = false;
    }

    if (g_curr_num_enemies == 0)
    {
        current_wave_index++;
        if (current_wave_index >= MAX_WAVES)
        {
            current_wave_index = 0;
        }
        wave[current_wave_index].active = true;
    }
}

int main()
{
    // Setting up window
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "GAME PROJECT");
    SetTargetFPS(60);
    bool gameover = false;

    initialize_weapon(weapons);

    initialize_waves();

    float last_shot = 0.0f;

    while (!WindowShouldClose() && !gameover)
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
        if (player.position.y - player.radius < 0)
        {
            player.position.y = 0 + player.radius;
        }

        if (player.position.y + player.radius > SCREENHEIGHT)
        {
            player.position.y = SCREENHEIGHT - player.radius;
        }

        if (player.position.x - player.radius < 0)
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
                    g_curr_num_enemies--;
                    player.hp -= 10;

                    if (player.hp <= 0)
                    {
                        gameover = true;
                    }
                }
            }
        }

        switch_weapons();

        Weapon *current_weapon = &weapons[current_weapon_index];

        // Shooting and cooldown logic
        if (last_shot >= current_weapon->rate_of_fire)
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                Vector2 bullet_direction = {0, -1};
                shoot_bullets(player.position, bullet_direction, *current_weapon);
                last_shot = 0.0f;
            }
        }
        else
        {
            last_shot += deltatime;
        }

        // if (enemy_last_shot >= time_since_last_shot)
        // {
        //     Vector2 enemy_bullet_direction = {0, 200};
        //     enemy_shoot_bullets(enemy->position, enemy_bullet_direction);
        //     enemy_last_shot = 0.0f;
        // }
        // else
        // {
        //     enemy_last_shot += deltatime;
        // }

        wave_progress();

        update_bullets(*current_weapon);
        update_enemy(deltatime);
        update_elite(deltatime);
        update_boss(deltatime);
        update_enemy_bullet();
        update_explode(*current_weapon);
        update_powerup(current_weapon, player);

        BeginDrawing();
        ClearBackground(BLACK);

        DrawCircleV(player.position, player.radius, WHITE);

        draw_bullets(*current_weapon);

        draw_powerup();

        draw_enemy();

        draw_elite();

        draw_boss();

        draw_enemy_bullet();

        draw_explosion();

        DrawText(TextFormat("Weapon id: %d", current_weapon_index + 1), 8, 42, 20, WHITE);

        DrawText(TextFormat("Current wave: %d", current_wave_index + 1), 8, 68, 20, WHITE);

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