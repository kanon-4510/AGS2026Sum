#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "ClassWorkPhase.h"

ClassWorkPhase::ClassWorkPhase(PlayerStatus* playerstatus):playerStatus_(playerstatus)
{

}

ClassWorkPhase::~ClassWorkPhase(void)
{
}

void ClassWorkPhase::Update(void)
{
	auto& ins = InputManager::GetInstance();

	ProcessClassworkSelection();
	ProcessClassworkDecision();
}

void ClassWorkPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Class Work", 0xFFFFFF);

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
	//技能値の上昇
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) || ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		int rand = GetRand(35)-15;	//乱数の取得
		int skill = SKILL_UP + rand;//上昇幅の確定

		switch (select_) 
		{
		case CLASSWORK_SELECT::PHARMACY:
			playerStatus_->pharmacy_ += skill;
			PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN;
			isFinished_ = true;
			break;
		case CLASSWORK_SELECT::MARTIALARTS:
			playerStatus_->martialArts_ += skill;
			PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN;
			isFinished_ = true;
			break;
		case CLASSWORK_SELECT::MAGICKNOWLEDGE:
			playerStatus_->magicKnowledge_ += skill;
			PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN;
			isFinished_ = true;
			break;
		case CLASSWORK_SELECT::FAITH:
			playerStatus_->faith_ += skill;
			PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN;
			isFinished_ = true;
			break;
		case CLASSWORK_SELECT::ARCHAEOLOGY:
			playerStatus_->archaeology_ += skill;
			PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN;
			isFinished_ = true;
			break;
		case CLASSWORK_SELECT::ASTROLOGY:
			playerStatus_->astrology_ += skill;
			PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN;
			isFinished_ = true;
			break;
		}
	}
	if (ins_.IsTrgDown(KEY_INPUT_TAB) || ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::CANCEL;
		isFinished_ = true;
	}
}