#include "PlayerStatus.h"
#include <EffekseerForDXLib.h>
#include "../Manager/SceneManager.h"

PlayerStatus::PlayerStatus()
{
}

void PlayerStatus::Draw()
{
	//ステータスの描画処理
	DrawFormatString(300, 10, GetColor(255, 255, 255), "Level: %d", level_);
	DrawFormatString(300, 30, GetColor(255, 255, 255), "HP: %d/%d", hp_, maxHp_);
	DrawFormatString(300, 50, GetColor(255, 255, 255), "Power: %d", power_);
	DrawFormatString(300, 70, GetColor(255, 255, 255), "Magic Power: %d", magic_);
	DrawFormatString(300, 90, GetColor(255, 255, 255), "Speed: %d", speed_);
	DrawFormatString(300, 110, GetColor(255, 255, 255), "Luck: %d", luck_);

	DrawFormatString(300, 130, GetColor(255, 255, 255), "Job: %s", job.c_str());
	DrawFormatString(300, 150, GetColor(255, 255, 255), "Faith: %d", faith_);
}

void PlayerStatus::InitJob()
{
	//職業の初期化
	//名前, LV, POW, MAG, 薬学, 武術, 魔法知, 信仰, 考古, 占星
	jobList.push_back(JobData("聖職者"	, 1, 0, 0, 0, 0, 0, 5, 0, 0));
	jobList.push_back(JobData("占い師"	, 1, 0, 0, 0, 0, 0, 0, 0, 5));
	jobList.push_back(JobData("詠唱者"	, 1, 0, 5, 0, 0, 5, 0, 0, 0));

	jobList.push_back(JobData("魔導師"	, 1, 0, 5, 0, 0, 5, 0, 0, 0));
	jobList.push_back(JobData("魔剣士"	, 1, 3, 3, 0, 2, 2, 0, 0, 0));
	jobList.push_back(JobData("付加術師", 1, 0, 3, 2, 0, 3, 0, 0, 0));
}

void PlayerStatus::Damage(int damage)
{
	hp_ -= damage;
	if (hp_ <= 0) {
		hp_ = 0;
		Death();
	}
}

void PlayerStatus::Death()
{
	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
}

void PlayerStatus::LevelUp()
{
	//レベルアップした時の処理
	level_++;
	maxHp_ += 5;
	hp_ = maxHp_;
	power_ += 2;
	magic_ += 2;
	speed_ += 1;
	luck_ += 1;
}

void PlayerStatus::GrowSkill(SkillType type, int amount)
{
	//指定された技能を増加させる
	switch (type) {
	case SkillType::Pharmacy:       pharmacy_ += amount; break;
	case SkillType::MartialArts:    martialArts_ += amount; break;
	case SkillType::MagicKnowledge: magicKnowledge_ += amount; break;
	case SkillType::Faith:          faith_ += amount; break;
	case SkillType::Archaeology:    archaeology_ += amount; break;
	case SkillType::Astrology:      astrology_ += amount; break;
	}

	//技能が上がったことで「新しく転職可能になった職業」があるかチェック
	//CheckNewJobAvailability();
}

bool PlayerStatus::JobCheck(const JobData& job)
{
	//基礎ステータスのチェック
	if (this->level_ < job.status.reqLevel_) return false;
	if (this->power_ < job.status.reqPower_) return false;
	if (this->magic_ < job.status.reqMagic_) return false;

	//技術ステータスのチェック
	if (this->pharmacy_ < job.status.reqPharmacy_) return false;
	if (this->martialArts_ < job.status.reqMartialArts_) return false;
	if (this->magicKnowledge_ < job.status.reqMagicKnowledge_) return false;
	if (this->faith_ < job.status.reqFaith_) return false;
	if (this->archaeology_ < job.status.reqArchaeology_) return false;
	if (this->astrology_ < job.status.reqAstrology_) return false;

	return true;
}