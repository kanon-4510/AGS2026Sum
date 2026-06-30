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
	Enemy(std::string name, int hp, int power1, int power2, int power3, int speed, int exp,
		std::string skill1, std::string skill2, std::string skill3,
		const char* imagePath, int x, int y);

	//デストラクタ（メモリ解放や画像の削除用）
	~Enemy();

	//DxLib用の描画関数（グラフィックやHPバーを表示する）
	void Draw() const;

	//バトル用の関数
	EnemyActionInfo DecideAction() const;
	void Damage(int damage);//負傷用の関数
	void Heal(int amount);	//回復用の関数
	
	//防御系
	void SetGuard(int power){guard_ = power;};//ダメージ軽減数値を設定
	void ResetGuard(){guard_ = 0;}

	//ゲッター
	std::string GetName() const { return name_; }
	int GetCurrentHp() const { return currentHp_; }
	int GetPower1() const { return power1_; }
	int GetPower2() const { return power2_; }
	int GetPower3() const { return power3_; }
	int GetSpeed() const { return speed_; }
	int GetExp() const { return exp_; }
	bool IsDead() const { return currentHp_ <= 0; }
	std::string GetSkill(int index) const;
private:
	std::string name_;
	int maxHp_;
	int currentHp_;
	int power1_;
	int power2_;
	int power3_;
	int speed_;
	int exp_;
	std::string skills_[3];

	int guard_ = 0; //軽減ダメージ量

	//DxLib用の画像ハンドルと座標
	int gh_;
	int x_, y_;
};

Enemy* SpawnEnemyByTurn(int turn);
Enemy* SpawnRushEnemy(int stage); //ラッシュ専用