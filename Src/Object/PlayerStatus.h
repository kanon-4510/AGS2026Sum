#pragma once
#include "JobData.h"
#include <string>
#include <vector>

	//ルート
enum class PLAYER_ROUTE
{
	NONE,           //未選択（3ターン）
	BREAKTHROUGH,   //打破
	SALVATION,      //救世
	TRUTH,          //真理
	SELFLESS        //無欲
};

class PlayerStatus
{
public:
	constexpr static int STATUS_X = 500;			//ステータスの描画位置X
	constexpr static int STATUS_COLOR = 0xFFFFFF;	//ステータスの描画色

	constexpr static int STATUS_BONUS_X = 380;	//ステータスのボーナス分の描画位置X
	constexpr static int STATUS_BONUS_COLOR = 0x00FF00;	//ステータスのボーナス分の描画色

	constexpr static int NEED_EXP = 30;			//必要経験値
	constexpr static int RATE_BASE = 100;		//ステータスアップの確率の基準値
	constexpr static int SKILL_UP_RATE = 70;	//ステータスアップの確率

	constexpr static int PLAYER_POS_X = 900;	//プレイヤーの描画位置X
	constexpr static int PLAYER_POS_Y = 240;	//プレイヤーの描画位置Y

	constexpr static int ANIM_COUNT_ATTACK = 5;	//アニメーションのフレーム数
	constexpr static int ANIM_COUNT_DAMAGE = 10;	//アニメーションのフレーム数
	constexpr static int ANIM_MOVE_PIXELS = 3;	//動かすピクセル数

	int level_ = 1;
	int exp_ = 0;
	int maxHp_ = 10;
	int hp_ = maxHp_;
	int power_ = 10;
	int magic_ = 10;
	int speed_ = 10;

	int heal_ = 2;	//回復量

	//技能の種類
	enum class SkillType 
	{
		Pharmacy,		//薬学
		MartialArts,	//武術
		MagicKnowledge,	//魔法知識
		Faith,			//信仰
		Archaeology,	//考古学
		Astrology		//占星術
	};

	//技能ステータス
	int pharmacy_ = 0;
	int martialArts_ = 0;
	int magicKnowledge_ = 0;
	int faith_ = 0;
	int archaeology_ = 0;
	int astrology_ = 0;

	// 追加：ボーナス計算付きのポイント獲得関数
	int AddSkillPoint(SkillType type, int baseAmount);

	//追加効果の種類
	enum class BonusType 
	{
		ItemBonus,   //薬学　　：回復効果上昇
		AttackBonus, //武術　　：会心発生率上昇
		MagicBonus,  //魔法知識：魔法の使用許可
		DefenseBonus,//信仰　　：ダメージ軽減量
		ExpBonus,    //考古学　：獲得経験値上昇
		LuckBonus    //占星術　：回避率上昇
	};

	//職業ごとのステータスボーナス
	struct JobBonus 
	{
		int hp = 0;
		int power = 0;
		int magic = 0;
		int speed = 0;
	};
	PLAYER_ROUTE currentRoute_ = PLAYER_ROUTE::NONE; //現在のルート

	//技能ボーナス用の補正倍率（初期値は1.0倍）
	float attackMultiplier_ = 1.0f;//攻撃力補正
	float maxHpMultiplier_ = 1.0f; //最大HP補正
	float maxMpMultiplier_ = 1.0f; //最大MP補正
	float speedMultiplier_ = 1.0f; //素早さ・回避補正

	std::string job = "見習い魔女";	//現在の職業
	std::vector<JobData> jobList;	//職業リストを作成

	PlayerStatus();

	//更新処理
	void Update();

	void Draw();//描画処理
	void DrawQuestImages();//クエスト用の画像などを描画する処理

	int Attack();//攻撃処理
	int MagicAttack();//魔法攻撃処理

	//回復処理
	void Heal(int amount);
	void FullHeal();

	void Damage(int damage);//ダメージ処理
	void Death();	//死亡処理

	int GetMaxHp();//HPを外から参照できるようにする
	int GetSpeed();//速度を外から参照できるようにする
	void GetExp(int exp);//経験値処理

	//レベルアップした時の処理
	void LevelUp();

	//職業関連
	void InitJob();//職業の初期化
	bool JobCheck(const JobData& job);//転職可能かどうかを判定する関数
	int SkillBonus(BonusType type, int baseValue);//技能ごとの追加効果を処理する関数
	JobBonus GetJobBonus();//現在の職業に応じたステータスボーナスを計算する関数
	void SetJob(std::string newJobName) { job = newJobName; }//職業名を書き換える
	//ユニークスキル用のフラグ
	bool hasStartDamage = false;  //開幕固定ダメ
	bool hasFirstHitNull = false; //初撃無効
	bool hasMagicUnlock = false;  //魔法制限なし
	bool hasCritBoost = false;    //クリティカル倍率上昇
	bool hasMagicToAttack = false;//攻撃時に魔力加算
	bool hasAutoRegen = false;    //リジェネ
	bool hasGuts = false;         //根性（HP1で耐える）
	//スキルの「消費」を管理するフラグ
	bool isFirstHitUsed = false;  //初撃無効を消費したか
	bool isGutsUsed = false;      //根性を消費したか

	const std::vector<JobData>& GetJobList() const { return jobList; }//全職業リストを外から参照できるようにする
	std::string GetName() const { return name; }					  //プレイヤーの名前を外から参照できるようにする
	std::string GetJobName() const { return job; }					  //現在の職業名を外から参照できるようにする

	//攻撃アニメーションの処理
	void AttackAnimation();

	//ダメージアニメーションの処理
	void DamageAnimation();

	bool hasChallengedHellQuest_ = false; //激ムズクエストに挑戦したかどうかのフラグ
private:
	std::string name = "ルピナス";	//プレイヤーの名前

	int battlePlayer_;	//プレイヤーの画像を格納

	int attackMotionTimer_ = 0; // 演出用のタイマー（フレーム数）
	int playerPosX_ = PLAYER_POS_X;           // 画像を左にずらすピクセル数

	int damageMotionTimer_ = 0; // ダメージ演出用のタイマー（フレーム数）
};