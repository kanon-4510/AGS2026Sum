#include "PlayerStatus.h"
#include <EffekseerForDXLib.h>
#include "../Manager/SceneManager.h"

PlayerStatus::PlayerStatus()
{
}

void PlayerStatus::Draw()
{
	//ステータスの描画処理
	DrawFormatString(STATUS_X, 10, STATUS_COLOR, "Level: %d", level_);
	DrawFormatString(STATUS_X, 30, STATUS_COLOR, "HP: %d/%d", hp_, maxHp_);
	DrawFormatString(STATUS_X, 50, STATUS_COLOR, "Power: %d", power_);
	DrawFormatString(STATUS_X, 70, STATUS_COLOR, "Magic Power: %d", magic_);
	DrawFormatString(STATUS_X, 90, STATUS_COLOR, "Speed: %d", speed_);
	DrawFormatString(STATUS_X, 110, STATUS_COLOR, "Luck: %d", luck_);

	DrawFormatString(300, 130, GetColor(255, 255, 255), "Job: %s", job.c_str());
	DrawFormatString(300, 150, GetColor(255, 255, 255), "薬: %d", pharmacy_);
	DrawFormatString(300, 170, GetColor(255, 255, 255), "武: %d", martialArts_);
	DrawFormatString(300, 190, GetColor(255, 255, 255), "魔: %d", magicKnowledge_);
	DrawFormatString(300, 210, GetColor(255, 255, 255), "信: %d", faith_);
	DrawFormatString(300, 230, GetColor(255, 255, 255), "古: %d", archaeology_);
	DrawFormatString(300, 250, GetColor(255, 255, 255), "星: %d", astrology_);
	DrawFormatString(STATUS_X, 130, STATUS_COLOR, "Job: %s", job.c_str());
	DrawFormatString(STATUS_X, 150, STATUS_COLOR, "Faith: %d", faith_);
}

void PlayerStatus::InitJob()
{
	//すでに職業が初期化されている場合は何もしない
	if (!jobList.empty()) return;

	//職業の初期化
	//名前, LV, POW, MAG, 薬学, 武術, 魔法知, 信仰, 考古, 占星
	jobList.push_back(JobData("聖職者"	, 1, 0, 0, 0, 0, 0, 5, 0, 0));
	jobList.push_back(JobData("占い師"	, 1, 0, 0, 0, 0, 0, 0, 0, 5));
	jobList.push_back(JobData("詠唱者"	, 1, 0, 5, 0, 0, 0, 0, 5, 0));

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

void PlayerStatus::GetExp(int exp)
{
	//経験値処理
	exp_ += exp;
	if (exp_ >= NEED_EXP) {
		exp_ -= NEED_EXP;
		LevelUp();
	}
}

void PlayerStatus::LevelUp()
{
	//レベルアップした時の処理
	level_++;
	maxHp_ += 5;
	hp_ = maxHp_;

	//各ステータスの抽選処理

	//60%の確率でPOWアップ
	if ((rand() % RATE_BASE) < SKILL_UP_RATE) {
		power_ += 1;
	}

	//60%の確率でMAGICアップ
	if ((rand() % RATE_BASE) < SKILL_UP_RATE) {
		magic_ += 1;
	}

	//60%の確率でSPEEDアップ
	if ((rand() % RATE_BASE) < SKILL_UP_RATE) {
		speed_ += 1;
	}

	//60%の確率でLUCKアップ
	if ((rand() % RATE_BASE) < SKILL_UP_RATE) {
		luck_ += 1;
	}
}

void PlayerStatus::SkillUp(SkillType type, int amount)
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

void PlayerStatus::JobBonus(const JobData& job)
{
	//職業に応じたステータスアップ
	if (job.status.name == "聖職者") {
		this->maxHp_ += 50;
		this->hp_ = this->maxHp_; //全回復
	}
	else if (job.status.name == "魔導師") {
		this->magic_ += 10;
	}
}
