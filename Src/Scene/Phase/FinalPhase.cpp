#include <DxLib.h>
#include <algorithm>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/AsoUtility.h"
#include "FinalPhase.h"

FinalPhase::FinalPhase(PlayerStatus* playerStatus)
	: playerStatus_(playerStatus)
	, command_(COMMAND::ATTACK)
	, battleStep_(BATTLE_STEP::COMMAND_SELECTION)
{
}

void FinalPhase::Update(void)
{
	ManageTurn();

	if (bossEnemyHp_ <= 0) // 例えば、300フレーム経過したらフェーズを終了する
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
	}
}

void FinalPhase::Draw(void)
{
	DrawString(0, 0, "Scene : Final Phase", 0xFFFFFF);

	DrawFormatString(0, 20, 0xFFFFFF, "command %d", static_cast<int>(command_));

	DrawFormatString(0, 100, 0xFFFFFF, "プレイヤーのHP %d / %d", playerStatus_->hp_, playerStatus_->maxHp_);
	DrawFormatString(0, 120, 0xFFFFFF, "敵のHP %d　/ %d", bossEnemyHp_, bossEnemyMaxHp_);

	Utility::DrawCommandMenu(0, 200, { "攻撃", "魔法", "アイテム" }, static_cast<int>(command_));

	if (battleMessage_ != "")
	{
		DrawFormatString(0, 150, 0xFF0000, battleMessage_.c_str());
		DrawString(0, 170, "Enterキーで次へ", 0xFF0000);
	}
}

bool FinalPhase::IsFinished() const
{
	return isFinished_;
}

void FinalPhase::ManageTurn(void)
{
	switch (battleStep_)
	{
	case FinalPhase::BATTLE_STEP::COMMAND_SELECTION:
		//プレイヤーの行動の処理などをここに書く
		ProcessPlayerAction();
		break;
	case FinalPhase::BATTLE_STEP::DETERMINE:
		DetermineActionOrder();
		break;
	case FinalPhase::BATTLE_STEP::ACTION_LOOP:
		ProcessActionLoop();
		break;
	case FinalPhase::FinalPhase::BATTLE_STEP::RESULT:
		DisplayResult();
		break;
	case FinalPhase::BATTLE_STEP::MAX:
		break;
	default:
		break;
	}
}

void FinalPhase::DetermineActionOrder(void)
{
	actionOrder_.clear();

	// プレイヤー追加 (idは0、ターゲットは今のところ敵の0番とする)
	//（名前、速度、プレイヤーかどうか、ターゲット、行動の種類、ダメージ）
	actionOrder_.push_back({ "プレイヤー", playerStatus_->speed_, true, 0, (int)command_, 0 });

	// 敵追加 (とりあえず1体)
	if (bossEnemyHp_ > 0)
	{
		actionOrder_.push_back({ "ボス", bossEnemySpeed_, false, 0, 0, 0 });
	}

	// ソート
	std::sort(actionOrder_.begin(), actionOrder_.end(), [](const ActionUnit& a, const ActionUnit& b)
		{
			return a.speed > b.speed;
		}
	);

	// currentActionIdx_ の初期化もここで行うのが良いです
	currentActionIdx_ = 0;
	battleStep_ = BATTLE_STEP::ACTION_LOOP;

}

void FinalPhase::ProcessActionLoop(void)
{
	if (currentActionIdx_ >= actionOrder_.size())
	{
		// ターン終了時にフラグを更新
		wasMagicUsedLastTurn_ = magicUsedThisTurn_;

		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
		currentActionIdx_ = 0;
		battleMessage_ = "";
		return;
	}

	auto& unit = actionOrder_[currentActionIdx_];

	if (battleMessage_ == "")
	{
		if (unit.isPlayer && command_ == COMMAND::ATTACK)
		{
			battleMessage_ = unit.name + " の攻撃！";
			bossEnemyHp_ -= playerStatus_->power_;
		}
		else if (unit.isPlayer && command_ == COMMAND::MAGIC)
		{
			battleMessage_ = unit.name + " の魔法攻撃！";
			bossEnemyHp_ -= playerStatus_->magic_;
		}
		else if (unit.isPlayer && command_ == COMMAND::ITEM)
		{
			battleMessage_ = unit.name + " がアイテム使用！";
			playerStatus_->hp_ += 200;
		}
		else
		{
			battleMessage_ = unit.name + " の攻撃！";
			playerStatus_->Damage(bossEnemyPow_);
		}
		if (bossEnemyHp_ <= 0)
		{
			battleMessage_ = unit.name + " が敵を倒した！";
		}
	}

	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		battleMessage_ = "";
		currentActionIdx_++;

		// 判定：敵を倒したか？
		if (bossEnemyHp_ <= 0)
		{
			//戦闘終了時にも忘れずに魔法フラグを更新する
			wasMagicUsedLastTurn_ = magicUsedThisTurn_;

			battleStep_ = BATTLE_STEP::RESULT;
			return;
		}
	}
}

void FinalPhase::DisplayResult(void)
{
	//ここで勝敗の結果を表示する処理を書く
	//Enterが押されたらメッセージをリセットして次へ
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		playerStatus_->GetExp(10);
		isFinished_ = true; //フェーズ終了
	}

}

void FinalPhase::ProcessPlayerAction()
{
	//選択肢の数（今回は仮に3つ：「攻撃」「魔法」「アイテム」）
	int maxItems = static_cast<int>(COMMAND::MAX);

	//--- カーソル移動 ---
	if (ins_.IsTrgDown(KEY_INPUT_UP))
	{
		command_ = static_cast<COMMAND>((static_cast<int>(command_) - 1 + maxItems) % maxItems); //上にループ
	}
	if (ins_.IsTrgDown(KEY_INPUT_DOWN))
	{
		command_ = static_cast<COMMAND>((static_cast<int>(command_) + 1) % maxItems); //下にループ
	}


	// --- 決定処理 ---
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		// 1. 魔法を選択している、かつ前回魔法を使っていたら決定させない
		if (command_ == COMMAND::MAGIC && wasMagicUsedLastTurn_)
		{
			// ここで「今は使えない！」というSEを鳴らしたり、
			// メッセージを一時的に出すと親切です
			return;
		}

		// 2. 今回魔法を使うかどうかを記録しておく
		if (command_ == COMMAND::MAGIC) {
			magicUsedThisTurn_ = true;
		}
		else {
			magicUsedThisTurn_ = false;
		}

		// 3. 次のステップへ
		battleStep_ = BATTLE_STEP::DETERMINE;
	}
}
