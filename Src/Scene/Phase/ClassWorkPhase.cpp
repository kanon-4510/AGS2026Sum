#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/Color.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../GameScene.h"
#include "ClassWorkPhase.h"

ClassWorkPhase::ClassWorkPhase(PlayerStatus* playerstatus, GameScene& gameScene):playerStatus_(playerstatus), gameScene_(gameScene)
{
	bgImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::GAME_SCENE).handleId_;
	playerImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::PLAYER).handleId_;
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
	DrawGraph(0, 0, bgImg_, true);
	DrawGraph(700, 200, playerImg_, true);
	DrawTutorial();

	SetFontSize(24);
	DrawFormatString(TEXT_POS_X, TEXT_POS_Y, (select_ == CLASSWORK_SELECT::PHARMACY ? Color::YELLOW : Color::WHITE), "薬学調合");
	DrawFormatString(TEXT_POS_X, TEXT_POS_Y + 50, (select_ == CLASSWORK_SELECT::MARTIALARTS ? Color::YELLOW : Color::WHITE), "武術訓練");
	DrawFormatString(TEXT_POS_X, TEXT_POS_Y + 100, (select_ == CLASSWORK_SELECT::MAGICKNOWLEDGE ? Color::YELLOW : Color::WHITE), "魔法知識");
	DrawFormatString(TEXT_POS_X, TEXT_POS_Y + 150, (select_ == CLASSWORK_SELECT::FAITH ? Color::YELLOW : Color::WHITE), "信仰");
	DrawFormatString(TEXT_POS_X, TEXT_POS_Y + 200, (select_ == CLASSWORK_SELECT::ARCHAEOLOGY ? Color::YELLOW : Color::WHITE), "考古学");
	DrawFormatString(TEXT_POS_X, TEXT_POS_Y + 250, (select_ == CLASSWORK_SELECT::ASTROLOGY ? Color::YELLOW : Color::WHITE), "占星術");
	
	DrawSkillBonus();
	SetFontSize(DEFAULT_FONT_SIZE);
}

void ClassWorkPhase::DrawSkillBonus(void)
{
	DrawString(bonusX, bonusY, "技能ボーナス", Color::YELLOW);

	//現在のボーナス値と、授業を受けた後のボーナス値を比較するための変数
	std::string effectName = "";
	int reqInterval = 0;	//次のレベルまでの必要値（例：薬学なら次の治癒力アップまでの必要値）

	//選択されている項目に応じて、一瞬だけステータスを上げて効果の変化を測定する
	SetFontSize(23);
	switch (select_)
	{
	case CLASSWORK_SELECT::PHARMACY:
		DrawString(bonusX, bonusY - 200
			, "魔法薬の調合だけでなく、魔石鑑\n定や魔道具の扱い方を学ぶ。\n受講すると回復魔法の効果が上昇\nする。"
			, Color::WHITE);
		effectName = "治癒力";
		reqInterval = 10 - playerStatus_->pharmacy_ % 10; //10ごとにアップ
		break;

	case CLASSWORK_SELECT::MARTIALARTS:
		DrawString(bonusX, bonusY - 200
			, "魔法一筋では生きていくことなど\n出来ない。基礎的な体術や武器の\n取回しを学ぶ。\n受講すると会心発生率が上昇する。"
			, Color::WHITE);
		effectName = "会心率";
		reqInterval = 5 - playerStatus_->martialArts_ % 5; //5ごとにアップ
		break;

	case CLASSWORK_SELECT::MAGICKNOWLEDGE:
		DrawString(bonusX, bonusY - 200
			, "魔法についての知見を深める授業。\n魔法の原理から法陣図や記述式も\n学ぶ。\n受講すると使える魔法が増える。"
			, Color::WHITE);
		effectName = "使える魔法の種類";
		reqInterval = 50 - playerStatus_->magicKnowledge_ % 50; //50ごとにアップ
		break;

	case CLASSWORK_SELECT::FAITH:
		DrawString(bonusX, bonusY - 200
			, "神が人に魔法を授けるまでの神話\nと、もたらされた加護と寵愛を主\nに学ぶ。\n受講すると守備力が上昇する。"
			, Color::WHITE);
		effectName = "被ダメージ軽減";
		reqInterval = 15 - playerStatus_->faith_ % 15; //15ごとにアップ
		break;

	case CLASSWORK_SELECT::ARCHAEOLOGY:
		DrawString(bonusX, bonusY - 200
			, "古代魔術やルーンの解読、封印術\nの解呪など魔法がたどった歴史を\n学ぶ。\n受講すると獲得経験値が上昇する。"
			, Color::WHITE);
		effectName = "獲得経験値";
		reqInterval = 10 - playerStatus_->archaeology_ % 10; //10ごとにアップ
		break;

	case CLASSWORK_SELECT::ASTROLOGY:
		DrawString(bonusX, bonusY - 200
			, "天文を知り、星の導きから運命力\nや透視などの予見について学ぶ。\n受講すると回避率が上昇する。"
			, Color::WHITE);
		effectName = "回避率";
		reqInterval = 5 - playerStatus_->astrology_ % 5; //5ごとにアップ
		break;
	}
	
	SetFontSize(24);
	//画面に効果の仕様を出力
	DrawFormatString(bonusX, bonusY + 30, Color::WHITE, "効果内容: %s", effectName.c_str());

	//「〇〇ごとに効果アップ！」と表示する
	DrawFormatString(bonusX, bonusY + 55, Color::GREEN, "残り　%d で能力アップ！", reqInterval);
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
			//ボーナス計算をする関数に渡す
			playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Pharmacy, skill);
			break;
		case CLASSWORK_SELECT::MARTIALARTS:
			playerStatus_->AddSkillPoint(PlayerStatus::SkillType::MartialArts, skill);
			break;
		case CLASSWORK_SELECT::MAGICKNOWLEDGE:
			playerStatus_->AddSkillPoint(PlayerStatus::SkillType::MagicKnowledge, skill);
			break;
		case CLASSWORK_SELECT::FAITH:
			playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Faith, skill);
			break;
		case CLASSWORK_SELECT::ARCHAEOLOGY:
			playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Archaeology, skill);
			break;
		case CLASSWORK_SELECT::ASTROLOGY:
			playerStatus_->AddSkillPoint(PlayerStatus::SkillType::Astrology, skill);
			break;
		}

		// どの授業を選んでも最終的にターンを進めるので、外にまとめる！
		PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN;
		isFinished_ = true;
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