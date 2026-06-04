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

	if (battleStep_ != BATTLE_STEP::RESULT)
	{
		DrawFormatString(0, 100, 0xFFFFFF, "プレイヤーのHP %d / %d", playerStatus_->hp_, playerStatus_->maxHp_);
		DrawFormatString(0, 120, 0xFFFFFF, "ボスのHP %d / %d", bossEnemyHp_, bossEnemyMaxHp_);
		//画像を表示する場合はここに activeEnemy_->Draw(); を追加

	}

	if (battleStep_ == BATTLE_STEP::COMMAND_SELECTION) //難易度選択中はコマンドやHPを表示しない
	{
		DrawCommandSelection();

		if (battleMessage_ != "")
		{
			DrawFormatString(0, 150, 0xFF0000, battleMessage_.c_str());
			DrawString(0, 170, "Enterキーで次へ", 0xFF0000);
		}
	}
	else if (battleStep_ == BATTLE_STEP::COMMAND_SUB_SELECTION)
	{
		DrawFormatString(0, 40, 0xFFFFFFF, "%d", subMenuCursor_);

		//サブコマンドの描画（例：魔法の種類やアイテムの選択肢など）
		Utility::DrawCommandMenu(0, 200, subActionMessages_, subMenuCursor_);
	}

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
		ProcessPlayerAction();
		break;
	case FinalPhase::BATTLE_STEP::COMMAND_SUB_SELECTION:
		//プレイヤーの行動の処理などをここに書く
		ProcessPlayerSubAction();
		break;
	case FinalPhase::BATTLE_STEP::DETERMINE:
		//行動の順番を決定する関数
		DetermineActionOrder();
		break;
	case FinalPhase::BATTLE_STEP::ACTION_LOOP:
		//行動の順番に従って処理を行う関数
		ProcessActionLoop();
		break;
	case FinalPhase::BATTLE_STEP::RESULT:
		//結果表示の処理などをここに書く
		DisplayResult();
		break;
	default:
		break;
	}
}

void FinalPhase::DetermineActionOrder(void)
{
	actionOrder_.clear();

	//プレイヤー追加 (idは0、ターゲットは今のところ敵の0番とする)
	actionOrder_.push_back({ "プレイヤー", playerStatus_->speed_, true, 0, (int)command_, 0 });
	if (command_ == COMMAND::ATTACK)
	{
		if (subMenuCursor_ == 0) {
			actionOrder_.back().skillName = "単体攻撃";
		}
		else {
			actionOrder_.back().skillName = "全体攻撃";
		}
	}
	// 魔法攻撃（1）の場合
	else if (command_ == COMMAND::MAGIC)
	{
		// サブメニューの選択肢に応じて魔法の性質を分ける！
		if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::MAGIC_ATTACK)) {
			actionOrder_.back().magicType = MAGIC_TYPE::MAGIC_ATTACK; //攻撃魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::HEAL)) {
			actionOrder_.back().magicType = MAGIC_TYPE::HEAL;         //回復魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::BUFF)) {
			actionOrder_.back().magicType = MAGIC_TYPE::BUFF;         //強化魔法
		}
		else if (subMenuCursor_ == static_cast<int>(MAGIC_TYPE::DEBUFF)) {
			actionOrder_.back().magicType = MAGIC_TYPE::DEBUFF;         //弱化魔法
		}
		// 将来のバフ・デバフ魔法もここに条件を足すだけ！
	}

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
		if (unit.isPlayer)
		{
			if (command_ == COMMAND::ATTACK)
			{
				battleMessage_ = unit.name + " の攻撃！";
				bossEnemyHp_ -= playerStatus_->Attack();
			}
			else if (command_ == COMMAND::MAGIC)
			{
				switch (unit.magicType)
				{
				case MAGIC_TYPE::MAGIC_ATTACK:
					battleMessage_ = unit.name + " の魔法攻撃！";
					bossEnemyHp_ -= playerStatus_->MagicAttack();
					break;
				case MAGIC_TYPE::HEAL:
					battleMessage_ = unit.name + " の回復魔法！";
					playerStatus_->Heal();
					break;
				case MAGIC_TYPE::BUFF:
					battleMessage_ = unit.name + " の強化魔法！";
					playerStatus_->Heal();
					break;
				case MAGIC_TYPE::DEBUFF:
					battleMessage_ = unit.name + " の弱化魔法！";
					playerStatus_->Heal();
					break;
				default:
					break;
				}
			}
		}
		else
		{
			battleMessage_ = unit.name + " の攻撃！";
			//回避判定
				//計算式：基本回避率 運のステータス×2
				//※運の数値に合わせて「/ 2」の部分は調整
			int evasionChance = playerStatus_->luck_ / 2;

			//バランス崩壊を防ぐための安全装置（最大回避率を90%でストップさせる）
			if (evasionChance > 90) evasionChance = 90;
			int roll = GetRand(99);

			if (roll < evasionChance)
			{
				//回避成功！ダメージ処理はスキップしてメッセージだけ上書き
				battleMessage_ = "攻撃を回避！";
			}
			else
			{
				//回避失敗 通常通りダメージを受ける
				playerStatus_->Damage(bossEnemyPow_);
			}
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

	int maxItems = static_cast<int>(COMMAND::MAX);

	//カーソル移動
	Utility::ProcessCommandMenuSelection(reinterpret_cast<int&>(command_), maxItems);

	//決定処理
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		if (command_ == COMMAND::MAGIC && wasMagicUsedLastTurn_)return; //魔法の連続使用制限

		if (command_ == COMMAND::MAGIC) magicUsedThisTurn_ = true;
		else magicUsedThisTurn_ = false;

		subActionMessages_.clear(); //一度リセット
		subMenuCursor_ = 0;    //カーソルを先頭に

		switch (command_)
		{
		case FinalPhase::COMMAND::ATTACK:
			subActionMessages_ = { "単体攻撃", "全体攻撃" };
			break;
		case FinalPhase::COMMAND::MAGIC:
			subActionMessages_ = { "単体魔法","回復", "強化", "状態異常付与" };
			break;
		case FinalPhase::COMMAND::MAX:
			break;
		default:
			break;
		}

		//次のステップへ
		battleStep_ = BATTLE_STEP::COMMAND_SUB_SELECTION;
		//battleStep_ = BATTLE_STEP::DETERMINE;
	}
}

void FinalPhase::ProcessPlayerSubAction(void)
{
	//// 選択肢の数を、埋め込まれた配列のサイズから自動取得！
	int maxSubItems = static_cast<int>(subActionMessages_.size());
	if (maxSubItems == 0) return; // 念のため

	//
	Utility::ProcessCommandMenuSelection(subMenuCursor_, maxSubItems);

	//--- 決定処理 ---
	if (ins_.IsTrgDown(KEY_INPUT_RETURN))
	{
		//【重要】ここで「何番のサブメニューを選んだか」を記憶しておく！
		//例：chosenSubMenuIdx_ = subMenuCursor_; 
		//この数値を、後の DetermineActionOrder や ActionUnit に引き渡します。

		//行動決定へ進む
		battleStep_ = BATTLE_STEP::DETERMINE;
	}

	if (ins_.IsTrgDown(KEY_INPUT_TAB))
	{
		battleStep_ = BATTLE_STEP::COMMAND_SELECTION;
	}
}

void FinalPhase::DrawCommandSelection(void)
{
	Utility::DrawCommandMenu(0, 200, { "攻撃","魔法" }, static_cast<int>(command_));
}