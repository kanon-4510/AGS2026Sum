#pragma once
#include "BaseStatus.h"
#include "JobData.h"
#include <string>
#include <vector>

class PlayerStatus
{
public:
	int level_ = 1;
	int maxHp_ = 10;
	int hp_ = maxHp_;
	int power_ = 10;
	int magic_ = 10;
	int speed_ = 10;
	int luck_ = 10;

	//技能の種類
	enum class SkillType {
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

	std::string job = "魔法使い";	//現在の職業
	std::vector<JobData> jobList;	//職業リストを作成

	PlayerStatus();

	//描画処理
	void Draw();

	//職業の初期化
	void InitJob();

	//ダメージ処理
	void Damage(int damage);

	//死亡処理
	void Death();

	//レベルアップした時の処理
	void LevelUp();

	//技能を成長させる処理
	void GrowSkill(SkillType type, int amount);

	//転職可能かどうかを判定する関数
	bool JobCheck(const JobData& job);

	//全職業リストを外から参照できるようにする
	const std::vector<JobData>& GetJobList() const { return jobList; }

	//職業名を書き換える
	void SetJob(std::string newJobName) { job = newJobName; }

};

