#include "PlayerStatus.h"
#include <EffekseerForDXLib.h>
#include "../Manager/SceneManager.h"

PlayerStatus::PlayerStatus()
{
	level_ = 1;
	currentJob = "魔法使い";
}

void PlayerStatus::Draw()
{
	//ステータスの描画処理
	DrawFormatString(300, 10, GetColor(255, 255, 255), "Level: %d", level_);
	DrawFormatString(300, 30, GetColor(255, 255, 255), "HP: %d/%d", hp_, maxHp_);
	DrawFormatString(300, 50, GetColor(255, 255, 255), "Power: %d", pow_);
	DrawFormatString(300, 70, GetColor(255, 255, 255), "Magic Power: %d", magic_);
	DrawFormatString(300, 90, GetColor(255, 255, 255), "Speed: %d", speed_);
	DrawFormatString(300, 110, GetColor(255, 255, 255), "Luck: %d", luck_);

	DrawFormatString(300, 130, GetColor(255, 255, 255), "Current Job: %s", currentJob.c_str());
}

void PlayerStatus::InitJob()
{
	//名前, LV, POW, MAG, 薬学, 武術, 魔法知, 信仰, 考古, 占星
	//masterJobs.push_back(JobData("聖職者", 1, 0, 0, 0, 0, 0, 5, 0, 0));
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
	pow_ += 2;
	magic_ += 2;
	speed_ += 1;
	luck_ += 1;
}

bool PlayerStatus::CanEvolve(const JobData& job)
{
	//基礎ステータスのチェック
	if (this->level_ < job.status.reqLevel_) return false;
	if (this->pow_ < job.status.reqPow_) return false;
	if (this->magic_ < job.status.reqMag_) return false;

	//技術ステータスのチェック
	if (this->skill_.pharmacy_ < job.status.reqPharmacy_) return false;
	if (this->skill_.martialArts_ < job.status.reqMartialArts_) return false;
	if (this->skill_.magicKnowledge_ < job.status.reqMagicKnowledge_) return false;
	if (this->skill_.faith_ < job.status.reqFaith_) return false;
	if (this->skill_.archaeology_ < job.status.reqArchaeology_) return false;
	if (this->skill_.astrology_ < job.status.reqAstrology_) return false;

	return true;
}