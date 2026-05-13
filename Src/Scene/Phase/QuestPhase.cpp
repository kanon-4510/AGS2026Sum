#include <DxLib.h>
#include <algorithm>
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
	// --- ターン管理 ---
	ManageTurn();

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

	DrawFormatString(0, 100, 0xFFFFFF, "プレイヤーのHP %d / %d", playerStatus_->hp_, playerStatus_->maxHp_);
	DrawFormatString(0, 120, 0xFFFFFF, "敵のHP %d", enemyHp_);

	Utility::DrawCommandMenu(0, 200, { "攻撃", "魔法", "アイテム" }, static_cast<int>(command_));

	if (battleMessage_ != "") {
		DrawFormatString(0, 150, 0xFF0000, battleMessage_.c_str());
	}
}

bool QuestPhase::IsFinished() const
{
	return isFinished_;
}

void QuestPhase::ManageTurn(void)
{
	switch (battleStep_)
	{
	case QuestPhase::BATTLE_STEP::COMMAND_SELECTION:
		//プレイヤーの行動の処理などをここに書く
		ProcessPlayerAction();
		break;
	case QuestPhase::BATTLE_STEP::DETERMINE:
		DetermineActionOrder();
		break;
	case QuestPhase::BATTLE_STEP::ACTION_LOOP:
		ProcessActionLoop();
		break;
	case QuestPhase::BATTLE_STEP::RESULT:
		DisplayResult();
		break;
	case QuestPhase::BATTLE_STEP::MAX:
		break;
	default:
		break;
	}
}

void QuestPhase::DetermineActionOrder(void)
{
	actionOrder_.clear();

	// プレイヤー追加 (idは0、ターゲットは今のところ敵の0番とする)
	actionOrder_.push_back({ "プレイヤー", playerStatus_->speed_, true, 0, (int)command_, 0 });

	// 敵追加 (とりあえず1体)
	if (enemyHp_ > 0) {
		actionOrder_.push_back({ "スライム", enemySpeed_, false, 0, 0, 0 });
	}

	// ソート
	std::sort(
		actionOrder_.begin(), actionOrder_.end(), [](const ActionUnit& a, const ActionUnit& b) {
		return a.speed > b.speed;
		});

	// currentActionIdx_ の初期化もここで行うのが良いです
	//currentActionIdx_ = 0;
	battleStep_ = BATTLE_STEP::ACTION_LOOP;

}

void QuestPhase::ProcessActionLoop(void)
{
	/*
	//全員の行動が終わったら、次のターン（コマンド選択）へ戻る
	if (currentActionIdx_ >= actionOrder_.size()) {
		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
		currentActionIdx_ = 0;
		return;
	}

	//現在の行動者を取得
	auto& unit = actionOrder_[currentActionIdx_];

	// --- ここで行なうこと ---
	//unit.isPlayer が true なら、プレイヤーの行動（ProcessPlayerAction）を呼ぶ
	if(unit.isPlayer)
	{
		ProcessPlayerAction();
	}
	else //false なら、敵の行動を計算する
	{
		//今回は仮に、敵は常に攻撃するものとする
		playerStatus_->Damage(enemyPow_);
	}

	// 4. Enterキーが押されたら、次の人へ（currentActionIdx_++）
	if (ins_.IsTrgDown(KEY_INPUT_RETURN)) {
		currentActionIdx_++;
	}
	*/
	if (currentActionIdx_ >= actionOrder_.size()) {
		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
		currentActionIdx_ = 0;
		battleMessage_ = ""; // ターン終了時にメッセージを消す
		return;
	}

	auto& unit = actionOrder_[currentActionIdx_];

	// まだメッセージが空なら（＝この人の行動開始の瞬間なら）セット
	if (battleMessage_ == "") {
		if (unit.isPlayer) {
			battleMessage_ = unit.name + " の攻撃！";
			// ここで実際に敵のHPを減らす処理を1回だけ呼ぶ
			enemyHp_ -= playerStatus_->power_;
		}
		else {
			battleMessage_ = unit.name + " の攻撃！";
			// ここでプレイヤーのHPを減らす処理を1回だけ呼ぶ
			playerStatus_->Damage(enemyPow_);
		}
	}

	// Enterが押されたらメッセージをリセットして次へ
	if (ins_.IsTrgDown(KEY_INPUT_RETURN)) {
		battleMessage_ = "";
		currentActionIdx_++;
	}
}

void QuestPhase::DisplayResult(void)
{
	
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

	// --- 決定処理 ---
	if (ins_.IsTrgDown(KEY_INPUT_RETURN)) {
		// ここで command_ の値を見て処理を分岐させる
		// 0: 攻撃, 1: 魔法, 2: アイテム

		if (command_ == COMMAND::ATTACK)
		{
			//攻撃の処理
			battleStep_ = BATTLE_STEP::DETERMINE;
			/*playerStatus_->Damage(enemyPow_);
			enemyHp_ -= playerStatus_->power_;*/
		}
		if (command_ == COMMAND::MAGIC)
		{
			//魔法の処理
			playerStatus_->Damage(enemyPow_);
			enemyHp_ -= playerStatus_->magic_;
		}
		if (command_ == COMMAND::ITEM)
		{
			//アイテムの処理
			playerStatus_->hp_ += 10;
			playerStatus_->Damage(enemyPow_);
		}
	}
}
