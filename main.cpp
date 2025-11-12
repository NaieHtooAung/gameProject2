#include <Novice.h>
#include <math.h>
#include <cstdlib>
#include <time.h>

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef struct Player {
    Vector2 position;
    float speed;
    int dashTimer;
    int dashCoolTimer;
    int dashSpeed;
    int velocity;
    bool isJump;
    bool isDash;
} Player;

typedef struct Enemy {
    Vector2 position;
    int attackPattern;
    int patternTimer;
    int patternCD;
    float speed;
    bool patternChange;
} Enemy;

typedef struct Bullet {
    Vector2 position;
    Vector2 velocity;
    bool isActive;
    float homingSpeed; 
    int life;         
} Bullet;

const char kWindowTitle[] = "GC1C_01_ウエキ_ユウト";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    srand((unsigned int)time(NULL));

    Novice::Initialize(kWindowTitle, 1280, 720);

    Player player;
    player.position.x = 640.0f;
    player.position.y = 640.0f;
    player.speed = 5.0f;
    player.dashCoolTimer = 180;
    player.dashTimer = 15;
    player.dashSpeed = 30;
    player.velocity = 20;
    player.isJump = false;
    player.isDash = false;

    Enemy enemy;
    enemy.position.x = 640.0f;
    enemy.position.y = 640.0f;
    enemy.speed = 3.0f;
    enemy.patternTimer = 300;
    enemy.patternCD = 300;
    enemy.attackPattern = rand() % 3 + 1;
    enemy.patternChange = true;

    Bullet bullets[10];
    for (int i = 0; i < 10; i++) {
        bullets[i].isActive = false;
    }

    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    int bulletTimer = 0;

    while (Novice::ProcessMessage() == 0) {
        Novice::BeginFrame();

        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        ///
        /// ↓更新処理ここから
        ///
        float dirX = 0.0f;
        if (keys[DIK_A]) dirX -= 1;
        if (keys[DIK_D]) dirX += 1;
        if (dirX != 0) {
            player.position.x += dirX * player.speed;
        }

        // ダッシュ
        if (keys[DIK_LSHIFT] && !preKeys[DIK_LSHIFT]) player.isDash = true;
        if (player.isDash) {
            player.dashCoolTimer--;
            if (player.dashCoolTimer >= 165) {
                player.speed = (float)player.dashSpeed;
                player.dashTimer--;
            }
            if (player.dashTimer <= 0) {
                player.dashTimer = 15;
                player.speed = 5;
            }
        }
        if (player.dashCoolTimer <= 0) {
            player.dashCoolTimer = 180;
            player.isDash = false;
        }

        // ジャンプ
        if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) player.isJump = true;
        if (player.isJump) {
            player.velocity -= 1;
            player.position.y -= player.velocity;
        }
        if (player.position.y >= 640) {
            player.velocity = 20;
            player.position.y = 640;
            player.isJump = false;
        }

        // 敵の移動
        enemy.position.x += enemy.speed;
        if (enemy.position.x > 1280.0f || enemy.position.x < 0.0f) {
            enemy.speed *= -1;
        }
        if (enemy.patternCD >= 300) {
            if (enemy.patternChange) {
                enemy.patternTimer--;
            }
        }
        if (enemy.patternTimer <= 0) {
            enemy.patternCD--;
            enemy.patternChange = false;
            enemy.patternTimer = 300;
        }
        if (enemy.patternTimer == 300) {
            enemy.patternCD--;
            if (enemy.patternCD <= 0) {
                enemy.attackPattern = rand() % 4 + 1;
                enemy.patternCD = 300;
                enemy.patternChange = true;
            }
        }

        // 弾の発射
        bulletTimer++;
        if (bulletTimer > 180) { 
            bulletTimer = 0;
            for (int i = 0; i < 10; i++) {
                if (!bullets[i].isActive) {
                    bullets[i].isActive = true;
                    bullets[i].position = enemy.position;

                    int randVal = rand() % 100; 
                    if (randVal < 70) {
                        // 直線弾
                        float dx = player.position.x - enemy.position.x;
                        float dy = player.position.y - enemy.position.y;
                        float len = sqrtf(dx * dx + dy * dy);
                        if (len != 0) {
                            bullets[i].velocity.x = (dx / len) * 6.0f;
                            bullets[i].velocity.y = (dy / len) * 6.0f;
                        }
                        bullets[i].homingSpeed = 0.0f; 
                        bullets[i].life = 0;           
                    }
                    else {
                        // 追尾弾
                        bullets[i].velocity.x = 0;
                        bullets[i].velocity.y = 0;
                        bullets[i].homingSpeed = 0.1f;
                        bullets[i].life = 180; // 3秒
                    }
                    break;
                }
            }
        }

        // 弾の移動
        for (int i = 0; i < 10; i++) {
            if (bullets[i].isActive) {
                if (bullets[i].homingSpeed > 0.0f) {
                    // 追尾処理
                    float dx = player.position.x - bullets[i].position.x;
                    float dy = player.position.y - bullets[i].position.y;
                    float len = sqrtf(dx * dx + dy * dy);
                    if (len != 0) {
                        bullets[i].velocity.x += (dx / len) * bullets[i].homingSpeed;
                        bullets[i].velocity.y += (dy / len) * bullets[i].homingSpeed;
                    }

                    bullets[i].life--;
                    if (bullets[i].life <= 0) {
                        bullets[i].isActive = false;
                        continue;
                    }
                }

                bullets[i].position.x += bullets[i].velocity.x;
                bullets[i].position.y += bullets[i].velocity.y;

                // 画面外で無効化
                if (bullets[i].position.x < 0 || bullets[i].position.x > 1280 ||
                    bullets[i].position.y < 0 || bullets[i].position.y > 720) {
                    bullets[i].isActive = false;
                }
            }
        }
        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///
        if (player.isJump) {
            Novice::ScreenPrintf(0, 30, "isJump");
        }
        Novice::ScreenPrintf(0, 0, "dashCooldown : %d", player.dashCoolTimer);
        Novice::ScreenPrintf(0, 20, "dashTimer : %d", player.dashTimer);
        Novice::ScreenPrintf(0, 50, "pattern : %d", enemy.attackPattern);
        Novice::ScreenPrintf(0, 70, "patternTimer : %d", enemy.patternTimer);
        Novice::ScreenPrintf(0, 90, "patternCD : %d", enemy.patternCD);

        Novice::DrawBox(static_cast<int>(player.position.x), static_cast<int>(player.position.y),
            50, 50, 0.0f, 0x0000FFFF, kFillModeSolid);
        Novice::DrawBox(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y),
            50, 50, 0.0f, 0xFF0000FF, kFillModeSolid);

        // 弾の描画
        for (int i = 0; i < 10; i++) {
            if (bullets[i].isActive) {
                unsigned int color = 0xFFFFFFFF; // 白（通常弾）
                if (bullets[i].homingSpeed > 0.0f) {
                    color = 0xFF0000FF; // 赤（追尾弾）
                }
                Novice::DrawEllipse(
                    static_cast<int>(bullets[i].position.x + 25),
                    static_cast<int>(bullets[i].position.y + 25),
                    8, 8, 0.0f, color, kFillModeSolid);
            }
        }
        ///
        /// ↑描画処理ここまで
        ///
        Novice::EndFrame();

        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    Novice::Finalize();
    return 0;
}
