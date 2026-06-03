#pragma once
#include "JobData.h"
#include <string>
#include <vector>

class PlayerStatus
{
public:
	constexpr static int STATUS_X = 300;			//ステータスの描画位置X
	constexpr static int STATUS_COLOR = 0xFFFFFF;	//ステータスの描画色

	constexpr static int STATUS_BONUS_X = 380;	//ステータスのボーナス分の描画位置X
	constexpr static int STATUS_BONUS_COLOR = 0x00FF00;	//ステータスのボーナス分の描画色

	constexpr static int NEED_EXP = 10;			//必要経験値
	constexpr static int RATE_BASE = 100;		//ステータスアップの確率の基準値
	constexpr static int SKILL_UP_RATE = 70;	//ステータスアップの確率

	int level_ = 1;
	int exp_ = 0;
	int maxHp_ = 10;
	int hp_ = maxHp_;
	int power_ = 10;
	int magic_ = 10;
	int speed_ = 10;
	int luck_ = 10;

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

	//追加効果の種類
	enum class BonusType {
		ItemBonus,      // 薬学：アイテム効果上昇
		AttackBonus,    // 武術：攻撃力上昇
		MagicBonus,     // 魔法知識：魔法威力上昇
		DefenseBonus,   // 信仰：ダメージ軽減
		ExpBonus,       // 考古学：経験値上昇
		LuckBonus       // 占星術：運の上昇
	};

	std::string job = "魔法使い";	//現在の職業
	std::vector<JobData> jobList;	//職業リストを作成

	PlayerStatus();

	//描画処理
	void Draw();

	//職業の初期化
	void InitJob();

	//攻撃処理
	int Attack();

	//魔法攻撃処理
	int MagicAttack();

	//ダメージ処理
	void Damage(int damage);

	//死亡処理
	void Death();

	//経験値処理
	void GetExp(int exp);

	//レベルアップした時の処理
	void LevelUp();

	//転職可能かどうかを判定する関数
	bool JobCheck(const JobData& job);

	//技能ごとの追加効果を処理する関数
	int SkillBonus(BonusType type, int baseValue);

	//全職業リストを外から参照できるようにする
	const std::vector<JobData>& GetJobList() const { return jobList; }

	//職業名を書き換える
	void SetJob(std::string newJobName) { job = newJobName; }

};

