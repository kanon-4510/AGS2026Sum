#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "JobChangePhase.h"

void JobChangePhase::Update(void)
{
	auto& ins = InputManager::GetInstance();
	timer_++; // フェーズの経過時間を増加させる

	if (timer_ > COUNT_MAX) // 例えば、300フレーム経過したらフェーズを終了する
	{
		isFinished_ = true;
	}

	/*if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		isFinished_ = true;
	}*/
}

void JobChangePhase::Draw(void)
{
	DrawString(0, 0, "Scene : Job Change", 0xFFFFFF);
	DrawFormatString(0, 20, 0xFFFFFF, "カウント %d", timer_);
}

bool JobChangePhase::IsFinished() const
{
	return isFinished_;
}
