#include <Novice.h>
#include<math.h>
#include<cstdlib>
#include<time.h>

const char kWindowTitle[] = "GC1C_08_ネイ_トゥーアウン";

typedef struct Vector2 {
	float x;
	float y;
} Vector2;
typedef struct Player {
	Vector2 position;
	float speed;
	int  const dashTime=15;
	int dashTimer;
	int const dashCoolTime=60;
	int dashCoolTimer;	
	int dashSpeed;
	int velocity;
	bool isJump;
	bool isDash;
	int weapon = 1; // 0:剣 1:弓 2:標
	int facing = 1; // 1:右 -1:左
} Player;
typedef struct Enemy {
	Vector2 position;
	int attackPattern;
	int patternTimer;
	int patternCD;
	float speed;
	bool patternChange;
} Enemy;

struct Weapon
{
	Vector2 position;
	Vector2 velosity;
	int state = 0; // 0:待機 1:飛翔 2:帰還
	int facing = 1;
	int flyTime ;
	int flyTimer ;
	int charge = 0;
};
Weapon weapon2;
Weapon weapon3;

int backEndData = true;


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	srand((unsigned int)time(NULL));
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	Player player;
	player.position.x = 640.0f;
	player.position.y = 640.0f;
	player.speed = 5.0f;
	player.dashCoolTimer = 90;
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

	weapon2.flyTime = 60;
	weapon2.flyTimer = 60;
	weapon3.flyTime = 90;
	weapon3.flyTimer = 90;

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		/// 横移動
		float dirX = 0.0f;
		if (keys[DIK_A])
		{
			dirX -= 1;
			player.facing = -1;
		}
		if (keys[DIK_D])
		{
			dirX += 1;
			player.facing = 1;
		}
		if (dirX != 0)
		{
			float tangant = atan2f(0, dirX);
			float moveX = cosf(tangant);
			player.position.x += moveX * player.speed;
		}
		/// ダッシュ処理
		if (keys[DIK_LSHIFT] && !preKeys[DIK_LSHIFT])
		{
			
			player.isDash = true;
		}
		if (player.isDash)
		{
			player.dashCoolTimer--;
			if (player.dashCoolTimer >= player.dashCoolTime- player.dashTime)
			{
				player.speed = (float)player.dashSpeed;
				player.dashTimer--;
			}
			if (player.dashTimer <= 0)
			{
				
				player.dashTimer = player.dashTime;
				
				player.speed = 5;
			}
		}
		if (player.dashCoolTimer <= 0)
		{
			player.dashCoolTimer = player.dashCoolTime;
			player.isDash = false;
		}
		/// ジャンプ処理
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
		{
			player.isJump = true;
		}
		if (player.isJump)
		{
			player.velocity -= 1;
			player.position.y -= player.velocity;
		}
		if (player.position.y >= 640)
		{
			player.velocity = 20;
			player.position.y = 640;
			player.isJump = false;
			
		}
		/// 敵の移動
		enemy.position.x += enemy.speed;
		if (enemy.position.x > 1280.0f || enemy.position.x < 0.0f)
		{
			enemy.speed *= -1;
		}
		if (enemy.patternCD >= 300)
		{
			if (enemy.patternChange)
			{
				enemy.patternTimer--;
			}
		}
		/// パターン変更
		if (enemy.patternTimer <= 0)
		{
			enemy.patternCD--;
			enemy.patternChange = false;
			enemy.patternTimer = 300;
			
		}
		if (enemy.patternTimer == 300)
		{
			enemy.patternCD--;
			if (enemy.patternCD <= 0)
			{
				enemy.attackPattern = rand() % 4 + 1;
				enemy.patternCD = 300;
				enemy.patternChange = true;
			}
		}

		/// 武器2(arrow)
		
		if (player.weapon == 1)
		{
			
			if (weapon2.charge > 100)
			{
				weapon2.charge = 100;
			}
			if (!keys[DIK_K] && preKeys[DIK_K])
			{
				if (weapon2.state == 0)
				{
					weapon2.position.x = player.position.x;
					weapon2.position.y = player.position.y;
					weapon2.velosity.x = (float)weapon2.charge * player.facing/2;
					weapon2.velosity.y = -5;
					weapon2.state = 1;
					weapon2.facing = player.facing;
					weapon2.charge = 0;
				}
			}

			if (weapon2.state == 1)
			{
				weapon2.flyTimer -= 1;
				weapon2.velosity.y += 0.5f; // 重力
				if (weapon2.flyTimer <= 0)
				{
					weapon2.state = 0;
					weapon2.flyTimer = weapon2.flyTime;
				}
			}

		}

		if (weapon2.position.y > 700)
		{
			weapon2.velosity.x = 0;
			weapon2.velosity.y = 0;
		}

		weapon2.position.x += weapon2.velosity.x;
		weapon2.position.y += weapon2.velosity.y;

		if (keys[DIK_K])
		{
			weapon2.charge++;
		}
		else
		{
			weapon2.charge = 0;
		}

		/// 武器3(ブーメラン)
		if (player.weapon == 2)
		{
			if (keys[DIK_L] && !preKeys[DIK_L])
			{
				if (weapon3.state == 0)
				{
					weapon3.position.x = player.position.x;
					weapon3.position.y = player.position.y;
					weapon3.velosity.x = 40.0f * player.facing;
					weapon3.state = 1;
					weapon3.facing = player.facing;
				}
			}

			if (weapon3.state == 1)
			{
				weapon3.flyTimer -= 1;
				weapon3.velosity.x -= weapon3.facing;
				if (weapon3.velosity.x * weapon3.facing < 0)
				{
					weapon3.state = 2;
				}
			}
			else if (weapon3.state == 2)
			{
				weapon3.flyTimer -= 1;
				weapon3.velosity.x -= weapon3.facing;
				if (weapon3.flyTimer <= 0)
				{
					weapon3.state = 0;
					weapon3.flyTimer = weapon3.flyTime;
				}
			}
		}
		weapon3.position.x += weapon3.velosity.x;
		weapon3.position.y += weapon3.velosity.y;


		if (keys[DIK_F1] && !preKeys[DIK_F1])
		{
			backEndData = ~backEndData;
		}		
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		Novice::DrawBox(static_cast<int>(player.position.x), static_cast<int>(player.position.y), 50, 50, 0.0f, 0x0000FFFF,kFillModeSolid);
		Novice::DrawBox(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y), 50, 50, 0.0f, 0xFF0000FF, kFillModeSolid);
		Novice::DrawEllipse(static_cast<int>(weapon2.position.x), static_cast<int>(weapon2.position.y), 10, 10, 0.0f, WHITE, kFillModeSolid);
		Novice::DrawEllipse(static_cast<int>(weapon3.position.x), static_cast<int>(weapon3.position.y), 32, 32, 0.0f, WHITE, kFillModeSolid);

		
		if (backEndData)
		{
			if (player.isJump)
			{
				Novice::ScreenPrintf(0, 30, "isJump");
			}
			Novice::ScreenPrintf(0, 0, "dashCooldown : %d", player.dashCoolTimer);
			Novice::ScreenPrintf(0, 20, "dashTimer : %d", player.dashTimer);
			Novice::ScreenPrintf(0, 50, "pattern : %d", enemy.attackPattern);
			Novice::ScreenPrintf(0, 70, "patternTimer : %d", enemy.patternTimer);
			Novice::ScreenPrintf(0, 90, "patternCD : %d", enemy.patternCD);
			Novice::ScreenPrintf(0, 110, "w2 timer : %d  charge : %d", weapon2.flyTimer, weapon2.charge);
			Novice::ScreenPrintf(0, 130, "w3 timer : %d", weapon3.flyTimer);
		}
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
