#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "ClassWorkPhase.h"

void ClassWorkPhase::Update(void)
{
	auto& ins = InputManager::GetInstance();

	ProcessClassworkSelection();
	ProcessClassworkDecision();

	if (timer_ >= COUNT_MAX) // 例えば、300フレーム経過したらフェーズを終了する
	{
		//isFinished_ = true;
	}
	/*if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		isFinished_ = true;
	}*/
}

void ClassWorkPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Class Work", 0xFFFFFF);
	DrawFormatString(0, 20, 0xFFFFFF, "カウント %d", timer_);

	int color = GetColor(255, 255, 255);
	int selectColor = GetColor(255, 255, 0); //選択中は黄色にする

	DrawFormatString(100, 100, (select_ == CLASSWORK_SELECT::PHARMACY ? selectColor : color), "薬学調合");
	DrawFormatString(100, 140, (select_ == CLASSWORK_SELECT::MARTIALARTS ? selectColor : color), "武術訓練");
	DrawFormatString(100, 180, (select_ == CLASSWORK_SELECT::MAGICKNOWLEDGE ? selectColor : color), "魔法知識");
	DrawFormatString(100, 220, (select_ == CLASSWORK_SELECT::FAITH ? selectColor : color), "信仰");
	DrawFormatString(100, 260, (select_ == CLASSWORK_SELECT::ARCHAEOLOGY ? selectColor : color), "考古学");
	DrawFormatString(100, 300, (select_ == CLASSWORK_SELECT::ASTROLOGY ? selectColor : color), "占星術");
}

bool ClassWorkPhase::IsFinished() const
{
	return isFinished_;
}

void ClassWorkPhase::ProcessClassworkSelection(void)
{
	if (ins_.IsTrgDown(KEY_INPUT_UP) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP))
	{
		int index = static_cast<int>(select_);
		index = (index+(static_cast<int>(CLASSWORK_SELECT::MAX)-1))% static_cast<int>(CLASSWORK_SELECT::MAX);
		select_ = static_cast<CLASSWORK_SELECT>(index);
	}
	else if (ins_.IsTrgDown(KEY_INPUT_DOWN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		int index = static_cast<int>(select_);
		index = (index + 1) % static_cast<int>(CLASSWORK_SELECT::MAX);
		select_ = static_cast<CLASSWORK_SELECT>(index);
	}
}

void ClassWorkPhase::ProcessClassworkDecision()
{
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) || ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		switch (select_) 
		{
		case CLASSWORK_SELECT::PHARMACY:
			timer_++; // フェーズの経過時間を増加させる

			isFinished_ = true;
			break;
		}
	}
}