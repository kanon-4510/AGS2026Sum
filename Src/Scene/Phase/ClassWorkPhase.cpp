#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/Color.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../GameScene.h"
#include "ClassWorkPhase.h"

// 授業データの構造体定義
struct ClassworkData
{
	const char* name;
	const char* description;
	const char* effectName;
	int interval; // 能力アップに必要な値の間隔
	PlayerStatus::SkillType skillType;
};

// マジックナンバーおよび重複したテキストをテーブル化
static const ClassworkData CLASSWORK_TABLE[] = {
	{ "薬学調合", "魔法薬の調合だけでなく、魔石鑑\n定や魔道具の扱い方を学ぶ。\n受講すると回復魔法の効果が上昇\nする。", "治癒力", 7, PlayerStatus::SkillType::Pharmacy },
	{ "武術訓練", "魔法一筋では生きていくことなど\n出来ない。基礎的な体術や武器の\n取回しを学ぶ。\n受講すると会心発生率が上昇する。", "会心率", 5, PlayerStatus::SkillType::MartialArts },
	{ "魔法知識", "魔法についての知見を深める授業。\n魔法の原理から法陣図や記述式も\n学ぶ。\n受講すると使える魔法が増える。", "使える魔法の種類", 50, PlayerStatus::SkillType::MagicKnowledge },
	{ "信仰",     "神が人に魔法を授けるまでの神話\nと、もたらされた加護と寵愛を主\nに学ぶ。\n受講すると守備力が上昇する。", "被ダメージ軽減", 15, PlayerStatus::SkillType::Faith },
	{ "考古学",   "古代魔術やルーンの解読、封印術\nの解呪など魔法がたどった歴史を\n学ぶ。\n受講すると獲得経験値が上昇する。", "獲得経験値", 8, PlayerStatus::SkillType::Archaeology },
	{ "占星術",   "天文を知り、星の導きから運命力\nや透視などの予見について学ぶ。\n受講すると回避率が上昇する。", "回避率", 5, PlayerStatus::SkillType::Astrology },
};

// 技能タイプに対応する現在のステータス値を取得するヘルパー関数
static int GetCurrentSkillPoint(const PlayerStatus* status, PlayerStatus::SkillType type)
{
	switch (type)
	{
	case PlayerStatus::SkillType::Pharmacy:       return status->pharmacy_;
	case PlayerStatus::SkillType::MartialArts:    return status->martialArts_;
	case PlayerStatus::SkillType::MagicKnowledge: return status->magicKnowledge_;
	case PlayerStatus::SkillType::Faith:          return status->faith_;
	case PlayerStatus::SkillType::Archaeology:    return status->archaeology_;
	case PlayerStatus::SkillType::Astrology:      return status->astrology_;
	default: return 0;
	}
}

ClassWorkPhase::ClassWorkPhase(PlayerStatus* playerstatus, GameScene& gameScene):playerStatus_(playerstatus), gameScene_(gameScene)
{
	bgImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::GAME_SCENE).handleId_;
	playerImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::PLAYER).handleId_;
	messageBoxImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::MESSAGE_BOX).handleId_;
}

ClassWorkPhase::~ClassWorkPhase(void)
{
}

void ClassWorkPhase::Update(void)
{
	auto& ins = InputManager::GetInstance();


	ProcessClassworkSelection();
	ProcessTutorial();
	ProcessClassworkDecision();
}

void ClassWorkPhase::Draw(void)
{
	DrawGraph(BG_POS_X, BG_POS_Y, bgImg_, true);
	DrawGraph(PLAYER_POS_X, PLAYER_POS_Y, playerImg_, true);
	DrawTutorial();

	SetFontSize(MENU_FONT_SIZE);

	// テーブルを利用してループでメニューを描画
	for (int i = 0; i < static_cast<int>(CLASSWORK_SELECT::MAX); ++i)
	{
		auto color = (select_ == static_cast<CLASSWORK_SELECT>(i)) ? Color::YELLOW : Color::WHITE;
		DrawFormatString(TEXT_POS_X, TEXT_POS_Y + (MENU_Y_INTERVAL * i), color, CLASSWORK_TABLE[i].name);
	}

	DrawSkillBonus();
	SetFontSize(DEFAULT_FONT_SIZE);
}

void ClassWorkPhase::DrawSkillBonus(void)
{
	DrawString(BONUS_POS_X, BONUS_POS_Y, "技能ボーナス", Color::YELLOW);

	int index = static_cast<int>(select_);
	if (index < 0 || index >= static_cast<int>(CLASSWORK_SELECT::MAX)) return;

	const auto& info = CLASSWORK_TABLE[index];

	// 説明文描画
	SetFontSize(DESC_FONT_SIZE);
	DrawString(BONUS_POS_X, BONUS_POS_Y - BONUS_DESC_OFFSET_Y, info.description, Color::WHITE);

	// 次のレベルまでの必要値計算
	int currentPoint = GetCurrentSkillPoint(playerStatus_, info.skillType);
	int reqInterval = info.interval - (currentPoint % info.interval);

	// 効果仕様の出力
	SetFontSize(MENU_FONT_SIZE);
	DrawFormatString(BONUS_POS_X, BONUS_POS_Y + BONUS_EFFECT_OFFSET_Y, Color::WHITE, "効果内容: %s", info.effectName);
	DrawFormatString(BONUS_POS_X, BONUS_POS_Y + BONUS_NEXT_LEVEL_OFFSET_Y, Color::GREEN, "残り %d で能力アップ！", reqInterval);
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
		int rand = GetRand(SKILL_RAND_RANGE)- SKILL_RAND_OFFSET;	//乱数の取得
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
	if (gameScene_.GetTurn() == TUTORIAL_TARGET_TURN)
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
	if (gameScene_.GetTurn() == TUTORIAL_TARGET_TURN)
	{
		SetFontSize(TUTORIAL_FONT_SIZE);
		DrawGraph(GameScene::MESSAGE_BOX_X, GameScene::MESSAGE_BOX_Y, messageBoxImg_, true);

		DrawString(GameScene::TUTORIAL_X, GameScene::TUTORIAL_Y
			,"6つの科目によって\n上昇する技能が異なる。\n技能は戦闘や職業に関わる\n大切な要素だ。\n魔法知識を履修してみよう。"
			, Color::BLACK);
		SetFontSize(DEFAULT_FONT_SIZE);
	}	
}