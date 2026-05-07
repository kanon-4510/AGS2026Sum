#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "FinalPhase.h"

void FinalPhase::Update(void)
{
	auto& ins = InputManager::GetInstance();
	timer_++; // フェーズの経過時間を増加させる

	if (timer_ > COUNT_MAX) // 例えば、300フレーム経過したらフェーズを終了する
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
	}
}

void FinalPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Final Phase", 0xFFFFFF);
	DrawFormatString(0, 20, 0xFFFFFF, "カウント %d", timer_);
}

bool FinalPhase::IsFinished() const
{
	return isFinished_;
}
