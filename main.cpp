#include <Novice.h>
#include<math.h>
#include<cstdlib>
#include<time.h>
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

const char kWindowTitle[] = "GC1C_08_ネイ_トゥーアウン";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	srand((unsigned int)time(NULL));
	// ライブラリの初期化
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
		}
		if (keys[DIK_D])
		{
			dirX += 1;
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
			if (player.dashCoolTimer >= 165)
			{
				player.speed = (float)player.dashSpeed;
				player.dashTimer--;
			}
			if (player.dashTimer <= 0)
			{
				
				player.dashTimer = 15;
				
				player.speed = 5;
			}
		}
		if (player.dashCoolTimer <= 0)
		{
			player.dashCoolTimer = 180;
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
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		if (player.isJump)
		{
			Novice::ScreenPrintf(0, 30, "isJump");
		}
		Novice::ScreenPrintf(0, 0, "dashCooldown : %d", player.dashCoolTimer);
		Novice::ScreenPrintf(0, 20, "dashTimer : %d", player.dashTimer);
		Novice::ScreenPrintf(0, 50, "pattern : %d", enemy.attackPattern);
		Novice::ScreenPrintf(0, 70, "patternTimer : %d", enemy.patternTimer);
		Novice::ScreenPrintf(0, 90, "patternCD : %d", enemy.patternCD);
		Novice::DrawBox(static_cast<int>(player.position.x), static_cast<int>(player.position.y), 50, 50, 0.0f, 0x0000FFFF,kFillModeSolid);
		Novice::DrawBox(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y), 50, 50, 0.0f, 0xFF0000FF, kFillModeSolid);
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
