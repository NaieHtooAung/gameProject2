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
	float radius;
	float speed;
	
	int dashTimer;
	
	int dashCoolTimer;
	int dashSpeed;
	int velocity;
	bool isJump;
	bool isDash;
	bool isHit;
	int weapon;   // 0:剣 1:弓 2:標
	int facing;   // 1:右 -1:左

	static const int dashTime = 15;
	static const int dashCoolTime = 60;
} Player;
typedef struct Enemy {
	Vector2 position;
	float radius;
	int attackPattern;
	int patternTimer;
	int patternCD;
	float speed;
	int dashTimer;
	int dashCoolTimer;
	int dashSpeed;
	int direction;
	int dashCount;        // how many dash attacks have been done
	int maxDashCount;     // maximum number of dash attacks (e.g., 3)
	float speedInitial;
	int dashTimerInitial;
	int dashCoolTimerInitial;
	int chargeTimer;      // time enemy spends charging up
	bool isCharging;      // whether the enemy is in the charge-up phase
	bool patternChange;
	bool isDash;

	 Vector2 dashTargetPosition;
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
	player.radius = 64.0f;
	player.speed = 5.0f;
	player.weapon = 2;
	player.facing = 1;
	player.dashCoolTimer = 180;
	player.dashTimer = 15;
	player.dashSpeed = 30;
	player.velocity = 20;
	player.isJump = false;
	player.isDash = false;
	player.isHit = false;

	Enemy enemy;
	enemy.position.x = 640.0f;
	enemy.position.y = 640.0f;
	enemy.radius = 64.0f;
	enemy.speed = 3.0f;
	enemy.speedInitial = enemy.speed;  // store initial
	enemy.patternTimer = 300;
	enemy.patternCD = 300;
	enemy.dashTargetPosition.x = 0.0f;
	enemy.dashTargetPosition.y = 0.0f;
	enemy.dashCoolTimer = 100;
	enemy.dashCoolTimerInitial = enemy.dashCoolTimer; // store initial
	enemy.dashTimer = 15;
	enemy.dashTimerInitial = enemy.dashTimer; // store initial
	enemy.dashSpeed = 40;
	enemy.dashCount = 0;
	enemy.maxDashCount = 2;
	enemy.direction = 0;
	enemy.chargeTimer = 60;     // e.g., 60 frames charging
	enemy.isCharging = false;
	enemy.isDash = false;
	enemy.attackPattern = rand() % 2 + 1;
	enemy.patternChange = true;

	weapon2.flyTime = 60;
	weapon2.flyTimer = 60;
	weapon3.flyTime = 90;
	weapon3.flyTimer = 90;

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

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
			if (player.dashCoolTimer >= player.dashCoolTimer - player.dashTimer)
			{
				player.speed = (float)player.dashSpeed;
				player.dashTimer--;
			}
			if (player.dashTimer <= 0)
			{
				
				player.speed = 5;
			}
		}
		if (player.dashCoolTimer <= 0)
		{
			player.dashTimer = 15;
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
			backEndData = !backEndData;
		}
			/// 武器3(ブーメラン)
			if (keys[DIK_L] && !preKeys[DIK_L])
			{
				if (player.weapon == 2)
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
			}
			if (player.weapon == 2)
			{
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
				backEndData = !backEndData;
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
			// 敵がプレイヤーの方向を向く
			if (player.position.x > enemy.position.x) {
				enemy.direction = 1;  // 右
			}
			else {
				enemy.direction = -1; // 左
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
				enemy.attackPattern = rand() % 2 + 1;
				enemy.patternCD = 300;
				enemy.patternChange = true;
			}
		}

		// --- Enemy “charge → dash” logic for pattern 1 ---
		if (enemy.attackPattern == 1) {
			// 1) At the very start of charge, lock facing direction
			if (!enemy.isCharging && !enemy.isDash
				&& enemy.dashCount == 0
				&& enemy.dashCoolTimer == enemy.dashCoolTimerInitial) {
				if (player.position.x > enemy.position.x) {
					enemy.direction = 1;
				}
				else {
					enemy.direction = -1;
				}
				// Start charging
				enemy.isCharging = true;
				enemy.chargeTimer = 60;     // adjust as desired
				enemy.position.y = 640.0f;  // ground Y fixed
			}

			if (enemy.isCharging) {
				enemy.chargeTimer--;
				enemy.speed = 0.0f;           // stand still while charging
				enemy.position.y = 640.0f;    // maintain ground Y

				if (enemy.chargeTimer <= 0) {
					// Charge ends → start dash
					enemy.isCharging = false;
					enemy.isDash = true;
					enemy.dashTimer = enemy.dashTimerInitial;

					// **Lock the target position at dash‐start**
					enemy.dashTargetPosition.x = player.position.x;
					enemy.dashTargetPosition.y = player.position.y;  // you may ignore Y if only X matters
				}
			}
			else if (enemy.isDash) {
				enemy.dashCoolTimer--;

				// Compute direction vector toward the locked target
				float dx = enemy.dashTargetPosition.x - enemy.position.x;
				float dy = enemy.dashTargetPosition.y - enemy.position.y;  // optional if Y moves
				float len = sqrtf(dx * dx + dy * dy);
				if (len != 0.0f) {
					dx /= len;
					dy /= len;
				}

				// If you only want horizontal (X axis) dash and maintain ground Y:
				enemy.position.x += dx * enemy.dashSpeed;
				enemy.position.y = 640.0f;  // keeps Y fixed

				enemy.dashTimer--;

				if (enemy.dashTimer <= 0) {
					enemy.dashCount++;
					enemy.isDash = false;
					enemy.speed = enemy.speedInitial;

					// After finishing dash, face player again for next charge
					if (player.position.x > enemy.position.x) {
						enemy.direction = 1;
					}
					else {
						enemy.direction = -1;
					}

					if (enemy.dashCount < enemy.maxDashCount) {
						// start next charge
						enemy.isCharging = true;
						enemy.chargeTimer = 60;
					}
					else {
						// all dashes done → reset
						enemy.dashCount = 0;
						enemy.dashCoolTimer = enemy.dashCoolTimerInitial;
						enemy.attackPattern = 0;  // or next pattern
					}
				}
			}

			// Reset cooldown if needed
			if (enemy.dashCoolTimer <= 0) {
				enemy.dashCoolTimer = enemy.dashCoolTimerInitial;
			}
		}


		// --- Collision check every frame ---
			// Reset hit state
		player.isHit = false;

		float ax = player.position.x - enemy.position.x;
		float dy = player.position.y - enemy.position.y;
		float distSq = ax * ax + dy * dy;
		float sumR = player.radius + enemy.radius;
		if (distSq <= sumR * sumR) {
			player.isHit = true;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		if (player.isHit)
		{
			Novice::DrawBox(static_cast<int>(player.position.x), static_cast<int>(player.position.y), (int)player.radius, (int)player.radius, 0.0f, BLACK, kFillModeSolid);
		}
		else
		{
			Novice::DrawBox(static_cast<int>(player.position.x), static_cast<int>(player.position.y), (int)player.radius, (int)player.radius, 0.0f, 0x0000FFFF, kFillModeSolid);
		}
		Novice::DrawBox(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y), (int)enemy.radius, (int)enemy.radius, 0.0f, 0xFF0000FF, kFillModeSolid);
		Novice::DrawEllipse(static_cast<int>(weapon3.position.x), static_cast<int>(weapon3.position.y), 32, 32, 0.0f, WHITE, kFillModeSolid);
     Novice::DrawEllipse(static_cast<int>(weapon2.position.x), static_cast<int>(weapon2.position.y), 10, 10, 0.0f, WHITE, kFillModeSolid);


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
			Novice::ScreenPrintf(0, 110, "enemy dir: %d", enemy.direction);
			Novice::ScreenPrintf(0, 130, "isCharging: %d", enemy.isCharging);
			Novice::ScreenPrintf(0, 150, "isDash: %d", enemy.isDash);
			Novice::ScreenPrintf(0, 170, "w2 timer : %d  charge : %d", weapon2.flyTimer, weapon2.charge);
			Novice::ScreenPrintf(0, 190, "w3 timer : %d", weapon3.flyTimer);
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
