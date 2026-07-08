#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/Color.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../GameScene.h"
#include "ClassWorkPhase.h"

ClassWorkPhase::ClassWorkPhase(PlayerStatus* playerstatus, GameScene& gameScene):playerStatus_(playerstatus), gameScene_(gameScene)
{

}

ClassWorkPhase::~ClassWorkPhase(void)
{
}

void ClassWorkPhase::Update(void)
{
	auto& ins = InputManager::GetInstance();

	ProcessTutorial();

	ProcessClassworkSelection();
	ProcessClassworkDecision();
}

void ClassWorkPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Class Work", 0xFFFFFF);

	DrawTutorial();

	DrawFormatString(100, 100, (select_ == CLASSWORK_SELECT::PHARMACY ? Color::YELLOW : Color::WHITE), "薬学調合");
	DrawFormatString(100, 140, (select_ == CLASSWORK_SELECT::MARTIALARTS ? Color::YELLOW : Color::WHITE), "武術訓練");
	DrawFormatString(100, 180, (select_ == CLASSWORK_SELECT::MAGICKNOWLEDGE ? Color::YELLOW : Color::WHITE), "魔法知識");
	DrawFormatString(100, 220, (select_ == CLASSWORK_SELECT::FAITH ? Color::YELLOW : Color::WHITE), "信仰");
	DrawFormatString(100, 260, (select_ == CLASSWORK_SELECT::ARCHAEOLOGY ? Color::YELLOW : Color::WHITE), "考古学");
	DrawFormatString(100, 300, (select_ == CLASSWORK_SELECT::ASTROLOGY ? Color::YELLOW : Color::WHITE), "占星術");

	DrawSkillBonus();
}

void ClassWorkPhase::DrawSkillBonus(void)
{

	DrawString(bonusX, bonusY, "【技能効果ボーナス】", Color::YELLOW);

	//現在のボーナス値と、授業を受けた後のボーナス値を比較するための変数
	std::string effectName = "";
	int reqInterval = 0;	//次のレベルまでの必要値（例：薬学なら次の回復量アップまでの必要値）

	//選択されている項目に応じて、一瞬だけステータスを上げて効果の変化を測定する
	switch (select_)
	{
	case CLASSWORK_SELECT::PHARMACY:
		effectName = "回復量";
		reqInterval = 10; // 10ごとにアップ
		break;

	case CLASSWORK_SELECT::MARTIALARTS:
		effectName = "会心率";
		reqInterval = 5; // 5ごとにアップ
		break;

	case CLASSWORK_SELECT::MAGICKNOWLEDGE:
		effectName = "魔法威力";
		reqInterval = 10; // 10ごとにアップ
		break;

	case CLASSWORK_SELECT::FAITH:
		effectName = "被ダメージ軽減";
		reqInterval = 10; // 10ごとにアップ
		break;

	case CLASSWORK_SELECT::ARCHAEOLOGY:
		effectName = "獲得経験値";
		reqInterval = 5; // 5ごとにアップ
		break;

	case CLASSWORK_SELECT::ASTROLOGY:
		effectName = "回避率";
		reqInterval = 5; // 5ごとにアップ
		break;
	}

	//画面に効果の仕様を出力
	DrawFormatString(bonusX, bonusY + 30, Color::WHITE, "効果内容: %s", effectName.c_str());

	//「〇〇ごとに効果アップ！」と表示する
	DrawFormatString(bonusX, bonusY + 55, Color::GREEN, "能力値 %d ごとに効果アップ！", reqInterval);
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
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) || 
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		int rand = GetRand(30)-15;	//乱数の取得
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
	if (ins_.IsTrgDown(KEY_INPUT_TAB) || 
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::CANCEL;
		isFinished_ = true;
	}
}

void ClassWorkPhase::ProcessTutorial(void)
{
	if (!SceneManager::GetInstance().IsTutorialEnabled())
	{
		return;
	}
	if (gameScene_.GetTurn() == 2)
	{
		select_ = CLASSWORK_SELECT::MAGICKNOWLEDGE;
	}
}

void ClassWorkPhase::DrawTutorial(void)
{
	if (!SceneManager::GetInstance().IsTutorialEnabled())
	{
		return;
	}
	if (gameScene_.GetTurn() == 2)
	{
		DrawString(0, 500
			,"ここでは授業を選択することができます\n受ける授業によって得られるスキルが異なります\n今回は魔法知識を選択します"
			, 0xFFFFFF);
	}
		
}