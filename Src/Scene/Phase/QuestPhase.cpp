#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/AsoUtility.h"
#include "QuestPhase.h"

QuestPhase::QuestPhase(PlayerStatus* playerStatus) : playerStatus_(playerStatus), command_(COMMAND::ATTACK)
{
}

void QuestPhase::Update(void)
{
	// --- 決定処理 ---
	if (ins_.IsTrgDown(KEY_INPUT_RETURN)) {
		// ここで command_ の値を見て処理を分岐させる
		// 0: 攻撃, 1: 魔法, 2: アイテム

		if(command_ == COMMAND::ATTACK)
		{
			//攻撃の処理
			playerStatus_->Damage(1);
			enemyHp_ -= playerStatus_->power_;
		}
		if(command_ == COMMAND::MAGIC)
		{
			//魔法の処理
			playerStatus_->Damage(2);
			enemyHp_ -= playerStatus_->magic_;
		}
		if(command_ == COMMAND::ITEM)
		{
			//アイテムの処理
			playerStatus_->hp_ += 10;
			playerStatus_->Damage(2);
			enemyHp_ -= 0;
		}
	}

	//プレイヤーの行動の処理などをここに書く
	ProcessPlayerAction();

	if (enemyHp_ <= 0)
	{
		isFinished_ = true;
		playerStatus_->LevelUp();
	}
}

void QuestPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Quest", 0xFFFFFF);
	DrawFormatString(0, 20, 0xFFFFFF, "command %d", static_cast<int>(command_));

	DrawFormatString(0, 100, 0xFFFFFF, "プレイヤーのHP %d", playerStatus_->hp_);
	DrawFormatString(0, 120, 0xFFFFFF, "敵のHP %d", enemyHp_);

	Utility::DrawCommandMenu(0, 200, { "攻撃", "魔法", "アイテム" }, static_cast<int>(command_));
}

bool QuestPhase::IsFinished() const
{
	return isFinished_;
}

void QuestPhase::ProcessPlayerAction()
{
	// 選択肢の数（今回は仮に3つ：「攻撃」「魔法」「アイテム」）
	int maxItems = static_cast<int>(COMMAND::MAX);

	// --- カーソル移動 ---
	if (ins_.IsTrgDown(KEY_INPUT_UP)) {
		command_ = static_cast<COMMAND>((static_cast<int>(command_) - 1 + maxItems) % maxItems); // 上にループ
	}
	if (ins_.IsTrgDown(KEY_INPUT_DOWN)) {
		command_ = static_cast<COMMAND>((static_cast<int>(command_) + 1) % maxItems); // 下にループ
	}
}
