#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "QuestPhase.h"

QuestPhase::QuestPhase(PlayerStatus* playerStatus) : playerStatus_(playerStatus)
{
}

void QuestPhase::Update(void)
{
	auto& ins = InputManager::GetInstance();
	
	//timer_++; // フェーズの経過時間を増加させる

	//if(timer_ > COUNT_MAX) // 例えば、300フレーム経過したらフェーズを終了する
	//{
	//	isFinished_ = true;
	//}

	if (ins.IsTrgDown(KEY_INPUT_E))
	{
		playerStatus_->Damage(5);
	}
	if (ins.IsTrgDown(KEY_INPUT_R))
	{
		enemyHp_ -= 5;
	}
	if(enemyHp_ <= 0)
	{
		isFinished_ = true;
	}	
	/*if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		isFinished_ = true;
	}*/
}

void QuestPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Quest", 0xFFFFFF);
	DrawString(0, 20, "Eキーでプレイヤーにダメージ", 0xFFFFFF);
	DrawString(0, 40, "Rキーで敵にダメージ", 0xFFFFFF);

	DrawFormatString(0, 100, 0xFFFFFF, "プレイヤーのHP %d", playerStatus_->hp_);
	DrawFormatString(0, 120, 0xFFFFFF, "敵のHP %d", enemyHp_);
}

bool QuestPhase::IsFinished() const
{
	return isFinished_;
}
