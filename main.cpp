#include <Novice.h>
#include<math.h>
#include<cstdlib>
#include<time.h>

const char kWindowTitle[] = "GC1C_08_ネイ_トゥーアウン";

typedef struct Vector2 {
	float x;
	float y;
} Vector2;

Vector2 camera;

typedef struct Player {
	Vector2 position;
	float radius;
	float speed;
	int health;
	int dashTimer;

	int dashCoolTimer;
	int dashSpeed;
	Vector2 velocity;
	bool isJump;
	bool isDash;
	bool isHit;
	bool isDamage;
	int weapon;   // 0:剣 1:弓 2:標
	int facing;   // 1:右 -1:左

	static const int dashTime = 15;
	static const int dashCoolTime = 60;
} Player;
typedef struct Enemy {
	Vector2 position;
	float radius;
	int health;
	int attackPattern;
	int patternTimer;
	int patternCD;
	float speed;
	int dashTimer;
	int dashCoolTimer;
	int dashSpeed;
	int direction;
	int dashDirection;
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
	float smashCharge = 0.0f;
	float smashCooldown = 0.0f;
} Enemy;

typedef struct Bullet {
	Vector2 position;
	float radius;
	float speed;
	bool isShot;
	bool isHit;
	float velX;
	float velY;
	float velSaveX;
	float velSaveY;
} Bullet;

typedef struct SkyBall {
	Vector2 position;
	float radius;
	int speed;
	bool isFalling;
}SkyBall;

struct Weapon
{
	Vector2 position;
	Vector2 velosity;
	int state = 0; // 0:待機 1:飛翔 2:帰還
	int facing = 1;
	int flyTime;
	int flyTimer;
	int charge = 0;
	int isHit = 0;
	int hitReady = false;
};
Weapon weapon1;
Weapon weapon2;
Weapon weapon3;

struct Sprite {
	Vector2 position;
	float radius;
	int texture;
	int frame;
};
Sprite playerWalkSprite;
Sprite playerW1Sprite;
Sprite playerW2Sprite;
Sprite playerW3Sprite;
Sprite playerFSprite;
Sprite enemySprite;

int backEndData = true;


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	srand((unsigned int)time(NULL));
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	Player player;
	player.position.x = 320.0f;
	player.position.y = 640.0f;
	player.radius = 32.0f;
	player.health = 100;
	player.speed = 5.0f;
	player.weapon = -1; // 0:剣 1:弓 2:標
	player.facing = 1;
	player.dashCoolTimer = 60;
	player.dashTimer = 15;
	player.dashSpeed = 30;
	player.velocity.y = 20;
	player.velocity.x = 0;
	player.isJump = false;
	player.isDash = false;
	player.isHit = false;
	player.isDamage = false;

	Enemy enemy;
	enemy.position.x = 840.0f;
	enemy.position.y = 640.0f;
	enemy.radius = 64.0f;
	enemy.health = 400;
	enemy.speed = 3.0f;
	enemy.speedInitial = enemy.speed;  // store initial
	enemy.patternTimer = 180;
	enemy.patternCD = 180;
	enemy.dashTargetPosition.x = 0.0f;
	enemy.dashTargetPosition.y = 0.0f;
	enemy.dashCoolTimer = 90;
	enemy.dashCoolTimerInitial = enemy.dashCoolTimer; // store initial
	enemy.dashTimer = 15;
	enemy.dashTimerInitial = enemy.dashTimer; // store initial
	enemy.dashSpeed = 40;
	enemy.dashCount = 0;
	enemy.maxDashCount = 2;
	enemy.direction = 0;
	enemy.dashDirection = 0;
	enemy.chargeTimer = 60;     // e.g., 60 frames charging
	enemy.isCharging = false;
	enemy.isDash = false;
	enemy.attackPattern = rand() % 3 + 1;
	enemy.patternChange = true;

	Bullet bullet[3];
	for (int i = 0; i < 3; i++)
	{
		bullet[i].position.x = -100.0f;
		bullet[i].position.y = -100.0f;
		bullet[i].radius = 16.0f;
		bullet[i].speed = 10.0f;
		bullet[i].isShot = false;

	}
	SkyBall sb[3];
	for (int i = 0; i < 3; i++)
	{
		sb[i].position.x = rand() % 1230 + 50.0f;
		sb[i].position.y = -100.0f;
		sb[i].radius = 50.0f;
		sb[i].speed = 10;
		sb[i].isFalling = false;
	}

	weapon1.flyTime = 15;
	weapon1.flyTimer = 15;
	weapon2.flyTime = 60;
	weapon2.flyTimer = 60;
	weapon3.flyTime = 90;
	weapon3.flyTimer = 90;

	//imageResourceLoad();
	/*int playerWalkTexture = Novice::LoadTexture("./Resource/image/playerWalk.png");
	int playerW1Texture = Novice::LoadTexture("./Resource/image/playerW1.png");
	int playerW2Texture = Novice::LoadTexture("./Resource/image/playerW2.png");
	int playerW3Texture = Novice::LoadTexture("./Resource/image/playerW3.png");*/
	int playerFTexture = Novice::LoadTexture("./Resource/image/playerF.png");
	int enemyTexture = Novice::LoadTexture("./Resource/image/enemy.png");
	int hpUITexture = Novice::LoadTexture("./Resource/image/HpUI.png");

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
			if (player.dashTimer == player.dashTime)
			{
				player.velocity.x = 0;
			}
			player.isDash = true;
		}
		if (player.isDash)
		{
			player.dashCoolTimer--;
			if (player.dashCoolTimer >= player.dashCoolTime - player.dashTime)
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
			player.dashTimer = player.dashTime;
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
			player.velocity.y -= 1;
			player.position.y -= player.velocity.y;
		}
		if (player.position.y >= 640)
		{
			player.velocity.y = 20;
			player.position.y = 640;
			player.isJump = false;
			if (player.velocity.x > 1)
			{
				player.velocity.x -= 1;
			}
			else if (player.velocity.x < -1)
			{
				player.velocity.x += 1;
			}
			else
			{
				player.velocity.x = 0;
			}

		}

		///// 武器 for debug
		//if (backEndData)
		//{
		//	if (weapon1.charge > 100)
		//	{
		//		weapon1.charge = 100;
		//	}
		//	if (!keys[DIK_J] && preKeys[DIK_J])
		//	{
		//		if (weapon1.state == 0)
		//		{
		//			weapon1.position.x = player.position.x+player.facing*48;
		//			weapon1.position.y = player.position.y;
		//			weapon1.state = 1;
		//			weapon1.facing = player.facing;
		//			weapon1.charge = 0;
		//		}
		//	}

		//	if (weapon1.state == 1)
		//	{
		//		weapon1.flyTimer -= 1;
		//		if (weapon1.isHit)
		//		{

		//		}

		//		if (weapon1.flyTimer <= 0)
		//		{
		//			weapon1.state = 0;
		//			weapon1.flyTimer = weapon1.flyTime;
		//		}
		//	}

		//	if (keys[DIK_J])
		//	{
		//		weapon1.charge++;
		//	}
		//	else
		//	{
		//		weapon1.charge = 0;
		//	}

		//	if (weapon2.charge > 100)
		//	{
		//		weapon2.charge = 100;
		//	}
		//	if (!keys[DIK_K] && preKeys[DIK_K])
		//	{
		//		if (weapon2.state == 0)
		//		{
		//			weapon2.position.x = player.position.x;
		//			weapon2.position.y = player.position.y;
		//			weapon2.velosity.x = (float)weapon2.charge * player.facing / 2;
		//			weapon2.velosity.y = -5;
		//			weapon2.state = 1;
		//			weapon2.facing = player.facing;
		//			weapon2.charge = 0;
		//		}
		//	}

		//	if (weapon2.state == 1)
		//	{
		//		weapon2.flyTimer -= 1;
		//		weapon2.velosity.y += 0.5f; // 重力
		//		if (weapon2.flyTimer <= 0)
		//		{
		//			weapon2.state = 0;
		//			weapon2.flyTimer = weapon2.flyTime;
		//		}
		//	}

		//	if (keys[DIK_L] && !preKeys[DIK_L])
		//	{
		//		if (weapon3.state == 0)
		//		{
		//			weapon3.position.x = player.position.x;
		//			weapon3.position.y = player.position.y;
		//			weapon3.velosity.x = 30.0f * player.facing;
		//			weapon3.state = 1;
		//			weapon3.facing = player.facing;
		//		}
		//	}

		//	if (weapon3.state == 1)
		//	{
		//		weapon3.flyTimer -= 1;
		//		weapon3.velosity.x -= weapon3.facing;
		//		if (weapon3.velosity.x * weapon3.facing < 0)
		//		{
		//			weapon3.state = 2;
		//		}
		//	}
		//	else if (weapon3.state == 2)
		//	{
		//		weapon3.flyTimer -= 1;
		//		weapon3.velosity.x -= weapon3.facing;
		//		if (weapon3.flyTimer <= 0)
		//		{
		//			weapon3.state = 0;
		//			weapon3.flyTimer = weapon3.flyTime;
		//		}
		//	}
		//}

		/// 武器1(Sword)

		if (weapon1.charge > 100)
		{
			weapon1.charge = 100;
		}
		if (!keys[DIK_J] && preKeys[DIK_J])
		{
			player.weapon = 0;
			if (weapon1.state == 0)
			{
				weapon1.position.x = player.position.x + player.facing * 48;
				weapon1.position.y = player.position.y;
				weapon1.state = 1;
				weapon1.facing = player.facing;
				weapon1.hitReady = true;
			}
		}

		if (weapon1.state == 0)
		{
			if (keys[DIK_J])
			{
				weapon1.charge++;
			}
			else
			{
				weapon1.charge = 0;
			}
		}
		else if (weapon1.state == 1)
		{
			weapon1.flyTimer -= 1;
			if (weapon1.isHit)//敵を命中したら
			{
				weapon1.hitReady = false;
				enemy.health -= 2 + weapon1.charge/20;
			}

			if (weapon1.flyTimer <= 0)
			{
				weapon1.state = 0;
				weapon1.flyTimer = weapon1.flyTime;
			}
			
		}
		
		//W1当たり判定
		float w1eDistX = enemy.position.x - weapon1.position.x;
		float w1eDistY = enemy.position.y - weapon1.position.y;
		float w1eDistSq = w1eDistX * w1eDistX + w1eDistY * w1eDistY;
		float w1eR = enemy.radius + 16.0f; // 武器の半径を16と仮定
		if (w1eDistSq <= w1eR * w1eR && weapon1.hitReady)
		{
			weapon1.isHit = 1;
		}
		else
		{
			weapon1.isHit = 0;
		}

		/// 武器2(arrow)

		if (weapon2.charge > 100)
		{
			weapon2.charge = 100;
		}
		if (!keys[DIK_K] && preKeys[DIK_K])
		{
			player.weapon = 1;
			if (weapon2.state == 0)
			{
				weapon2.position.x = player.position.x;
				weapon2.position.y = player.position.y;
				weapon2.velosity.x = (float)weapon2.charge * player.facing / 2;
				weapon2.velosity.y = -5;
				weapon2.state = 1;
				weapon2.facing = player.facing;
				weapon2.hitReady = true;
			}
		}

		if (weapon2.state == 0)
		{
			if (keys[DIK_K])
			{
				weapon2.charge++;
			}
			else
			{
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
			if (weapon2.isHit)//敵を命中したら
			{
				weapon2.hitReady = false;
				weapon2.velosity.x = 0;
				weapon2.velosity.y = 0;
				enemy.health -= 2 + weapon2.charge / 20;
			}
		}

		if (weapon2.position.y > 700)
		{
			weapon2.velosity.x = 0;
			weapon2.velosity.y = 0;
		}

		weapon2.position.x += weapon2.velosity.x;
		weapon2.position.y += weapon2.velosity.y;

		//w2当たり判定
		float w2eDistX = enemy.position.x - weapon2.position.x;
		float w2eDistY = enemy.position.y - weapon2.position.y;
		float w2eDistSq = w2eDistX * w2eDistX + w2eDistY * w2eDistY;
		float w2eR = enemy.radius + 10.0f; // 武器の半径を10と仮定
		if (w2eDistSq <= w2eR * w2eR && weapon2.hitReady)
		{
			weapon2.isHit = 1;
		}
		else
		{
			weapon2.isHit = 0;
		}

		/// 武器3(ブーメラン)
		if (keys[DIK_L] && !preKeys[DIK_L])
		{
			player.weapon = 2;
			if (weapon3.state == 0)
			{
				weapon3.position.x = player.position.x;
				weapon3.position.y = player.position.y;
				weapon3.velosity.x = 30.0f * player.facing;
				weapon3.state = 1;
				weapon3.facing = player.facing;
				weapon3.hitReady = true;
			}
		}

		if (weapon3.state == 0)
		{
			//待機中は特に処理なし
		}
		else if (weapon3.state == 1)
		{
			weapon3.flyTimer -= 1;
			weapon3.velosity.x -= weapon3.facing;
			if (weapon3.velosity.x * weapon3.facing < 0)
			{
				weapon3.state = 2;
				weapon3.hitReady = true;
			}
			if (weapon3.isHit )
			{
				weapon3.hitReady = false;
				enemy.health -= 5 ;
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
			if (weapon3.isHit)
			{
				weapon3.hitReady = false;
				enemy.health -= 5 ;
			}
			
		}
		weapon3.position.x += weapon3.velosity.x;
		weapon3.position.y += weapon3.velosity.y;

		//w3当たり判定
		float w3eDistX = enemy.position.x - weapon3.position.x;
		float w3eDistY = enemy.position.y - weapon3.position.y;
		float w3eDistSq = w3eDistX * w3eDistX + w3eDistY * w3eDistY;
		float w3eR = enemy.radius + 10.0f; // 武器の半径を10と仮定
		if (w3eDistSq <= w3eR * w3eR && weapon3.hitReady)
		{
			weapon3.isHit = 1;
		}
		else
		{
			weapon3.isHit = 0;
		}

		float w3pDistX = player.position.x - weapon3.position.x;
		float w3pDistY = player.position.y - weapon3.position.y;
		float w3pDistSq = w3pDistX * w3pDistX + w3pDistY * w3pDistY;
		float w3pR = player.radius + 10.0f; // 武器の半径を10と仮定
		if (w3pDistSq <= w3pR * w3pR && weapon3.state == 2)
		{
			weapon3.state = 0;
			weapon3.flyTimer = weapon3.flyTime;
			weapon3.position.y = -100.0f;
		}
		

		/// 敵の移動
		enemySprite.frame = 0;
		enemy.position.x += enemy.speed;
		if (enemy.position.x > 1280.0f || enemy.position.x < 0.0f)
		{
			enemy.speed *= -1;
		}
		if (enemy.patternCD >= 180)
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
			enemy.patternTimer = 180;

		}
		if (enemy.patternTimer == 180)
		{
			enemy.patternCD--;
			if (enemy.patternCD <= 0)
			{
				enemy.attackPattern = rand() % 3 + 1;
				enemy.patternCD = 180;
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
				enemySprite.frame = 1;

				if (enemy.chargeTimer <= 0) {
					// Charge ends → start dash
					enemy.isCharging = false;
					enemy.isDash = true;
					enemy.dashTimer = enemy.dashTimerInitial;
					
					// **Lock the target position at dash‐start**
					enemy.dashTargetPosition.x = player.position.x;
					enemy.dashTargetPosition.y = player.position.y;  // you may ignore Y if only X matters
					if (enemy.dashTargetPosition.x > enemy.position.x) {
						enemy.dashDirection = 1;
					}
					else {
						enemy.dashDirection = -1;
					}
				}
				
			}
			else if (enemy.isDash) {
				enemy.dashCoolTimer--;
				
				
				// Compute direction vector toward the locked target
				//float dx = enemy.dashTargetPosition.x - enemy.position.x;
				//float dy = enemy.dashTargetPosition.y - enemy.position.y;  // optional if Y moves
				//float len = sqrtf(dx * dx + dy * dy);
				//if (len != 0.0f) {
				//	dx /= len;
				//	dy /= len;
				//}

				// If you only want horizontal (X axis) dash and maintain ground Y:
				enemy.position.x += enemy.dashDirection * enemy.dashSpeed;
				enemy.position.y = 640.0f;  // keeps Y fixed


				enemy.dashTimer--;

				if (enemy.dashTimer <= 0) {
					enemy.dashCount++;
					enemy.isDash = false;
					enemy.speed = enemy.speedInitial;

					//// After finishing dash, face player again for next charge
					//if (enemy.dashTargetPosition.x > enemy.position.x) {
					//	enemy.dashDirection = 1;
					//}
					//else {
					//	enemy.dashDirection = -1;
					//}

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
				enemySprite.frame = 2;
			}
			if (enemy.isDash)
			{
				float ax = player.position.x - enemy.position.x;
				float dy = player.position.y - enemy.position.y;
				float distSq = ax * ax + dy * dy;
				float sumR = player.radius + enemy.radius;
				if (distSq <= sumR * sumR) {
					player.isDamage = true;
					if (player.isDamage)
					{
						player.health -= 2;
					}
				}
				else
				{
					player.isDamage = false;
				}
			}
		
			
			// Reset cooldown if needed
			if (enemy.dashCoolTimer <= 0) {
				enemy.dashCoolTimer = enemy.dashCoolTimerInitial;
			}
		}
		// --- Bullet shooting for pattern 2 ---
		if (enemy.attackPattern == 2) {
			enemy.speed = 0.0f; // stationary while shooting
			// Let's use a counter to track how many bullets fired this burst
			static int shotCount = 0;
			static int shotTimer = 0;
			const int delayBetweenShots = 10; // frames

			// If not currently shooting a burst, start it
			if (shotCount == 0 && shotTimer == 0) {
				shotCount = 12;
				shotTimer = delayBetweenShots;
			}

			if (shotCount > 0) {
				shotTimer--;
				if (shotTimer <= 0) {
					// Find an inactive bullet in your bullet array
					// --- When you shoot a bullet (spawn / fire) ---
					for (int i = 0; i < 3; i++) {
						if (!bullet[i].isShot) {
							// initialize bullet for new shot
							bullet[i].isShot = true;
							bullet[i].isHit = false;                 // reset hit flag
							bullet[i].position = enemy.position;      // spawn at enemy (or shooter) pos
							// compute normalized direction to player
							float dx = player.position.x - enemy.position.x;
							float dy = player.position.y - enemy.position.y;
							float len = sqrtf(dx * dx + dy * dy);
							if (len != 0.0f) {
								dx /= len; dy /= len;
							}
							// set velocity
							bullet[i].velX = dx * bullet[i].speed;
							bullet[i].velY = dy * bullet[i].speed;
							bullet[i].velSaveX = bullet[i].velX;
							bullet[i].velSaveY = bullet[i].velY;
							break;  // spawn one bullet per call
						}
					}

					
					shotCount--;
					shotTimer = delayBetweenShots;
					if (shotCount <= 0)
					{
						enemy.attackPattern = 0;
						enemy.speed = enemy.speedInitial;
						shotCount = 12;
					}
				}

			}
			enemySprite.frame = 3;
		}
		// --- Bullet‐Player collision check ---
			// --- Each frame: update all bullets (movement + collision + deactivate) ---
		for (int i = 0; i < 3; i++) {
			if (!bullet[i].isShot) continue;

			// Move bullet
			bullet[i].position.x +=bullet[i].velSaveX;
			bullet[i].position.y += bullet[i].velSaveY;
			
			// Optional: off-screen check → deactivate
			if (bullet[i].position.x < 0 || bullet[i].position.x > 1280 ||
				bullet[i].position.y < 0 || bullet[i].position.y > 720) {
				bullet[i].isShot = false;
				continue;
			}

			// Collision check: player vs bullet (circle-circle)
			float dx = player.position.x - bullet[i].position.x;
			float dy = player.position.y - bullet[i].position.y;
			float distSq = dx * dx + dy * dy;
			float radiusSum = player.radius + bullet[i].radius;

			if (distSq <= radiusSum * radiusSum) {
				// Hit detected — apply damage
				player.health -= 5;
				if (player.health < 0) player.health = 0;

				// Deactivate bullet so it doesn’t hit again
				bullet[i].isShot = false;
			}
		}

		if (enemy.attackPattern == 3) {
			for (int i = 0; i < 3; i++) {
				// If this ball is not currently falling, start it
				if (!sb[i].isFalling) {
					sb[i].isFalling = true;
					sb[i].position.y = -100.0f;      // start at top
					sb[i].position.x = (float)(rand() % 1230 + 50);
				}

				// If falling, move it
				if (sb[i].isFalling) {
					sb[i].position.y += sb[i].speed;
				}

				// If it exits screen, reset it
				if (sb[i].position.y > 720) {
					sb[i].isFalling = false;
					sb[i].position.y = -100.0f;
					sb[i].position.x = (float)(rand() % 1230 + 50);
				}

				// Collision with player
				float dx = player.position.x - sb[i].position.x;
				float dy = player.position.y - sb[i].position.y;
				float distance = sqrtf(dx * dx + dy * dy);
				float combinedRadius = player.radius + sb[i].radius;
				if (distance < combinedRadius) {
					// Hit detected
					player.isDamage = true;
					if (player.isDamage)
					{
						player.health -= 10;
						// Immediately hide/disable this skyball
						sb[i].isFalling = false;
						sb[i].position.y = -100.0f;
						sb[i].position.x = (float)(rand() % 1230 + 50);
						player.isDamage = false;
					}

					// **Process damage logic here** (e.g., reduce player HP, play effect)

				}
			}
			enemySprite.frame = 3;
		}

		for (int i = 0; i < 3; i++)
		{
			if (sb[i].isFalling && enemy.attackPattern != 3)
			{
				sb[i].position.y += sb[i].speed;
			}

		}
		
		player.position.x += player.velocity.x;
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
		else
		{
			player.isHit = false;
		}

		/*float bx = enemy.dashTargetPosition.x - enemy.position.x;
		float ey = enemy.dashTargetPosition.y - enemy.position.y;
		float distSqDTP = bx * bx + ey * ey;
		if (distSqDTP <= enemy.radius * enemy.radius) {
			enemy.dashTimer = 0;
		}*/

		//bump
		if (player.isHit)
		{
			player.velocity.y += 1;
			player.position.x = enemy.position.x + sumR * cosf(atan2f(dy, ax));
			player.position.y = enemy.position.y + sumR * sinf(atan2f(dy, ax));
			float trackInnerDirection = -atan2f(0, ax);
			//player.velocity.x = 5 * cosf(trackInnerDirection);

			if (enemy.isDash)
			{
				player.velocity.x = 15 * cosf(trackInnerDirection); //方向判定彈射
				player.velocity.y = 25;
				player.isJump = true;
			}


		}

		static float smashFlashTimer = 0.0f;
		// タイマー更新
		smashFlashTimer += 1.0f;
		if (smashFlashTimer >= 20.0f) smashFlashTimer = 0.0f;

		// 剣が敵に当たったらゲージ増加
		static bool wasHitLastFrame = false;

		if (weapon1.isHit && weapon1.hitReady && !wasHitLastFrame) {
			enemy.smashCharge += 20.0f;
			if (enemy.smashCharge >= 200.0f) {   
				enemy.smashCharge = 200.0f;  
			}
			weapon1.hitReady = false;
		}
		wasHitLastFrame = weapon1.isHit;

	
		// 攻撃してないときは少しずつ減少
		if (!(keys[DIK_J] && weapon1.state == 1 && weapon1.hitReady)) {
			if (enemy.smashCharge > 0.0f) {  
				enemy.smashCharge -= 0.05f;
				if (enemy.smashCharge < 0.0f) enemy.smashCharge = 0.0f;
			}
		}

		// クールダウン減少
		if (enemy.smashCooldown > 0.0f) {
			enemy.smashCooldown -= 1.0f;
		}

		if (enemy.smashCharge >= 199.9f && enemy.smashCooldown <= 0.0f) {  
			enemy.smashCharge = 0.0f;
			enemy.smashCooldown = 150.0f;
		}
		// スマッシュ中は敵の判定巨大化
		float currentEnemyRadius = enemy.radius;
		if (enemy.smashCooldown > 90.0f) {
			currentEnemyRadius = 140.0f;
		}

		// 当たり判定
		float dx_hit = player.position.x - enemy.position.x;
		float dy_hit = player.position.y - enemy.position.y;
		float distSq_hit = dx_hit * dx_hit + dy_hit * dy_hit;
		float sumR_hit = player.radius + currentEnemyRadius;

		if (distSq_hit <= sumR_hit * sumR_hit) {
			player.isHit = true;
			if (enemy.smashCooldown > 90.0f) {
				player.velocity.x = 22.0f * (player.position.x > enemy.position.x ? 1.0f : -1.0f);
				player.velocity.y = 35.0f;
				player.isJump = true;
			}
			else if (enemy.isDash) {
				player.velocity.x = 18.0f * (player.position.x > enemy.position.x ? 1.0f : -1.0f);
				player.velocity.y = 30.0f;
				player.isJump = true;
			}
		}


		//camera
		if (player.position.x < 300)
		{
			camera.x = 300 - player.position.x;
		}
		else if (player.position.x >= 300 && player.position.x <= 1000)
		{
			camera.x = 0;
		}
		else if (player.position.x > 1000)
		{
			camera.x = 1000 - player.position.x;;
		}

		if (keys[DIK_F1] && !preKeys[DIK_F1])
		{
			backEndData = !backEndData;
		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawBox(static_cast<int>(camera.x), 0, 1280, 720, 0.0f, 0x87CEEBFF, kFillModeSolid); // sky
		Novice::DrawSprite(0, 0, hpUITexture, 1.0f, 1.0f, 0.0f, WHITE);
		//UI
		//playerHP Bar
		//int playerHp = 80;  //max=100
		Novice::DrawBox(83, 99, player.health * 330 / 100, 26, 0.0f, RED, kFillModeSolid);

		//EnemyHP Bar
		//enemy.health = 80;   //max=100
		Novice::DrawBox(211, 19, enemy.health * 858 / 400, 26, 0.0f, RED, kFillModeSolid);

		//weapon1 CD Bar
		if (weapon1.flyTimer >= weapon1.flyTime)
		{
			Novice::DrawBox(106, 201, 27, 84, 0.0f, GREEN, kFillModeSolid);
		}
		else
		{
			Novice::DrawBox(106, 201, 27, (weapon1.flyTime - weapon1.flyTimer) * 84 / weapon1.flyTime, 0.0f, RED, kFillModeSolid);
		}
		Novice::DrawBox(126, 201, 7, weapon1.charge * 84 / 101, 0.0f, BLUE, kFillModeSolid);

		//weapon2 CD Bar
		if (weapon2.flyTimer >= weapon2.flyTime)
		{
			Novice::DrawBox(186, 201, 28, 84, 0.0f, GREEN, kFillModeSolid);
		}
		else
		{
			Novice::DrawBox(186, 201, 28, (weapon2.flyTime - weapon2.flyTimer) * 84 / weapon2.flyTime, 0.0f, RED, kFillModeSolid);
		}
		Novice::DrawBox(206, 201, 8, weapon2.charge * 84 / 101, 0.0f, BLUE, kFillModeSolid);

		//weapon3 CD Bar
		if (weapon3.flyTimer >= weapon3.flyTime)
		{
			Novice::DrawBox(266, 201, 27, 84, 0.0f, GREEN, kFillModeSolid);
		}
		else
		{
			Novice::DrawBox(266, 201, 27, (weapon3.flyTime - weapon3.flyTimer) * 84 / weapon3.flyTime, 0.0f, RED, kFillModeSolid);
		}


		//player・enemy・bullet・weapon描画
		if (player.isHit)
		{
			Novice::DrawEllipse(static_cast<int>(player.position.x + camera.x), static_cast<int>(player.position.y), (int)player.radius, (int)player.radius, 0.0f, BLACK, kFillModeSolid);
		}
		else
		{
			Novice::DrawEllipse(static_cast<int>(player.position.x + camera.x), static_cast<int>(player.position.y), (int)player.radius, (int)player.radius, 0.0f, 0x0000FFFF, kFillModeWireFrame);
		}

		/*Novice::DrawSpriteRect(static_cast<int>(player.position.x - 32 + camera.x), static_cast<int>(player.position.y - 32), playerWalkSprite.frame, 0, 80, 80,
			playerWalkTexture,
			0.5f, 1.0f, 0.0f, WHITE);
		if (player.weapon == 0)
		{
			Novice::DrawSpriteRect(static_cast<int>(player.position.x - 32 + camera.x), static_cast<int>(player.position.y - 32), playerW2Sprite.frame, 0, 80, 80,
				playerW1Texture,
				0.5f, 1.0f, 0.0f, WHITE);
		}
		else if (player.weapon == 1)
		{
			Novice::DrawSpriteRect(static_cast<int>(player.position.x - 32 + camera.x), static_cast<int>(player.position.y - 32), playerW2Sprite.frame, 0, 80, 80,
				playerW2Texture,
				0.5f, 1.0f, 0.0f, WHITE);
		}
		else if (player.weapon == 2)
		{
			Novice::DrawSpriteRect(static_cast<int>(player.position.x - 32 + camera.x), static_cast<int>(player.position.y - 32), playerW3Sprite.frame, 0, 80, 80,
				playerW3Texture,
				0.5f, 1.0f, 0.0f, WHITE);
		}*/

		if (player.facing == 1)
		{
			playerFSprite.frame = (player.weapon+1) * 2 ;
		}
		else
		{
			playerFSprite.frame = (player.weapon + 1) *2 + 1;
		}
		Novice::DrawSpriteRect(static_cast<int>(player.position.x - 48 + camera.x), static_cast<int>(player.position.y - 32), playerFSprite.frame*96, 0, 96, 96,
			playerFTexture,
			0.125f, 1.0f, 0.0f, WHITE);

		for (int i = 0; i < 3; i++)
		{
			if (sb[i].isFalling)
			{
				Novice::DrawEllipse(static_cast<int>(sb[i].position.x + camera.x), static_cast<int>(sb[i].position.y), (int)sb[i].radius, (int)sb[i].radius, 0.0f, 0x0000FFFF, kFillModeSolid);
			}
		}
		if (player.isDamage)
		{
			Novice::DrawEllipse(static_cast<int>(player.position.x + camera.x), static_cast<int>(player.position.y), (int)player.radius, (int)player.radius, 0.0f, RED, kFillModeSolid);
		}
		for (int i = 0; i < 3; i++)
		{
			if (bullet[i].isShot)
			{

				Novice::DrawEllipse((int)(bullet[i].position.x + camera.x), (int)bullet[i].position.y, (int)bullet[i].radius, (int)bullet[i].radius, 0.0f, 0xffffffff, kFillModeSolid);


			}
		}
		//Novice::DrawEllipse(static_cast<int>(enemy.position.x + camera.x), static_cast<int>(enemy.position.y), (int)enemy.radius, (int)enemy.radius, 0.0f, 0xFF0000FF, kFillModeSolid);
		if ((weapon1.isHit&& weapon1.hitReady) || (weapon2.isHit && weapon2.hitReady) || (weapon3.isHit && weapon3.hitReady))
		{
			Novice::DrawEllipse(static_cast<int>(enemy.position.x + camera.x), static_cast<int>(enemy.position.y), (int)enemy.radius, (int)enemy.radius, 0.0f, BLACK, kFillModeSolid);
		}
		else
		{
			Novice::DrawEllipse(static_cast<int>(enemy.position.x + camera.x), static_cast<int>(enemy.position.y), (int)enemy.radius, (int)enemy.radius, 0.0f, 0x0000FFFF, kFillModeWireFrame);
		}
		Novice::DrawSpriteRect(static_cast<int>(enemy.position.x - 48 + camera.x), static_cast<int>(enemy.position.y - 32), enemySprite.frame * 128, 0, 128, 128,
			enemyTexture,
			0.25f, 1.0f, 0.0f, WHITE);
		Novice::DrawEllipse(static_cast<int>(weapon3.position.x + camera.x), static_cast<int>(weapon3.position.y), 32, 32, 0.0f, WHITE, kFillModeSolid);
		Novice::DrawEllipse(static_cast<int>(weapon2.position.x + camera.x), static_cast<int>(weapon2.position.y), 10, 10, 0.0f, WHITE, kFillModeSolid);
		Novice::DrawEllipse(static_cast<int>(weapon1.position.x + camera.x), static_cast<int>(weapon1.position.y), 32, 32, 0.0f, WHITE, kFillModeWireFrame);


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
			Novice::ScreenPrintf(0, 110, "enemy dir: %d  target dir: %d", enemy.direction, enemy.dashDirection);
			Novice::ScreenPrintf(0, 130, "isCharging: %d", enemy.isCharging);
			Novice::ScreenPrintf(0, 150, "isDash: %d", enemy.isDash);
			Novice::ScreenPrintf(0, 170, "w1 timer : %d  charge : %d", weapon1.flyTimer, weapon1.charge);
			Novice::ScreenPrintf(0, 190, "w2 timer : %d  charge : %d", weapon2.flyTimer, weapon2.charge);
			Novice::ScreenPrintf(0, 210, "w3 timer : %d", weapon3.flyTimer);
			Novice::ScreenPrintf(0, 400, "player x : %0.2f  y : %0.2f", player.position.x, player.position.y);
			Novice::ScreenPrintf(0, 430, "playerV x : %0.2f  y : %0.2f", player.velocity.x, player.velocity.y);
			Novice::ScreenPrintf(0, 460, "SmashCharge: %.1f / 200.0", enemy.smashCharge);
			if (enemy.smashCooldown > 90.0f) 
			for (int i = 0; i < 3; i++)

			{
				Novice::ScreenPrintf(0, 480 + i * 20, "HIT bullet %d! health=%d", i, player.health);

			}

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