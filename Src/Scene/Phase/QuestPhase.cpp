#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "QuestPhase.h"

void QuestPhase::Update(void)
{
	auto& ins = InputManager::GetInstance();
	
	timer_++; // フェーズの経過時間を増加させる

	if(timer_ > COUNT_MAX) // 例えば、300フレーム経過したらフェーズを終了する
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
	DrawFormatString(0, 20, 0xFFFFFF, "カウント %d", timer_);
}

bool QuestPhase::IsFinished() const
{
	return isFinished_;
}
