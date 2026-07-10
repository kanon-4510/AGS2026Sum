#pragma once
#include <memory>
#include <map>
#include <functional>
#include <string>
#include <vector>

class SceneManager;
class GameScene;
class Player;

//敵のアニメーション状態の定義
enum ENEMY_ANIM
{
	IDLE,	//待機
	ACT_1,	//攻撃1
	ACT_2,	//攻撃2
	ACT_3,	//攻撃3
	DAMAGE,	//ダメージ
	DEAD,	//死亡
	MAX		//最大値
};

//アニメーションの設定をまとめる構造体
struct MotionConfig
{
	const char* imagePath; // 画像のパス（無い場合は nullptr）
	int numX;         // 横のコマ数
	int numY;         // 縦のコマ数
	int sizeX;        // 1コマの横幅
	int sizeY;        // 1コマの縦幅
};

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
		const std::vector<MotionConfig>& motionList, int x, int y);

	//デストラクタ（メモリ解放や画像の削除用）
	~Enemy();

	//アニメーションの更新や行動の決定などを行う関数
	void Update();

	//DxLib用の描画関数（グラフィックやHPバーを表示する）
	void Draw() const;

	//バトル用の関数
	void ChangeAnim(int command);
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
	bool IsAnimFinished() const { return isAnimFinished_; }
	std::string GetSkill(int index) const;
private:
	// クラスの内部で、外で作った「ENEMY_MOTION」や「MotionConfig」を利用する
	int images_[static_cast<int>(ENEMY_ANIM::MAX)][16];
	int maxFramesTable_[static_cast<int>(ENEMY_ANIM::MAX)];
	ENEMY_ANIM currentMotion_ = ENEMY_ANIM::IDLE;

	ENEMY_ANIM currentAnim_ = ENEMY_ANIM::IDLE; // 現在再生中のアニメ
	int animeTimer_ = 0;	// アニメーションのタイマー
	int currentFrame_ = 0;	// 現在のフレーム
	bool isAnimFinished_ = true;	//アニメーションが終了したかどうかのフラグ

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

	//アニメーション初期化用の private 関数を追加
	void InitAnimation(const std::vector<MotionConfig>& motionList);
};

Enemy* SpawnEnemyByTurn(int turn);
Enemy* SpawnRushEnemy(int stage); //ラッシュ専用