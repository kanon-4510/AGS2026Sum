#pragma once
#include <memory>
#include <map>
#include <functional>
#include <string>
#include <vector>

class SceneManager;
class GameScene;
class Player;

// 行動の種類の定義
enum EnemyActionType 
{
	ENEMY_ACTION_ATTACK = 0,
	ENEMY_ACTION_ACTION = 1,
	ENEMY_ACTION_SPECIAL = 2
};

// タイムライン用の構造体
struct EnemyActionInfo 
{
	std::string name;
	int speed;
	int actionType;
	std::string skillName;
};

class Enemy
{
public:
	//コンストラクタ（画像ファイルのパスや、画面のどこに描画するかの座標も受け取るように拡張）
	Enemy(std::string name, int hp, int power, int speed, int exp,
		std::string skill1, std::string skill2, std::string skill3,
		const char* imagePath, int x, int y);

	//デストラクタ（メモリ解放や画像の削除用）
	~Enemy();

	//DxLib用の描画関数（グラフィックやHPバーを表示する）
	void Draw() const;

	//バトル用の関数
	EnemyActionInfo DecideAction() const;
	void TakeDamage(int damage);
	void Heal(int amount); // 回復用の関数

	//ゲッター
	std::string GetName() const { return name_; }
	int GetCurrentHp() const { return currentHp_; }
	int GetPower() const { return power_; }
	int GetSpeed() const { return speed_; }
	int GetExp() const { return exp_; }
	bool IsDead() const { return currentHp_ <= 0; }
private:
	std::string name_;
	int maxHp_;
	int currentHp_;
	int power_;
	int speed_;
	int exp_;
	std::string skills_[3];

	//DxLib用の画像ハンドルと座標
	int gh_;
	int x_, y_;
};

Enemy* SpawnEnemyByTurn(int turn);