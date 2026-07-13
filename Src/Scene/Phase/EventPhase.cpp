#include "DxLib.h"
#include "../../Manager/InputManager.h"
#include "../../Utility/AsoUtility.h"
#include "EventPhase.h"

// コンストラクタで選択肢をセット
EventPhase::EventPhase(PlayerStatus* playerStatus) : playerStatus_(playerStatus)
{
	waitTimer_ = 0;

	// 仕様書通りの4つの選択肢を用意
	routeMenu_ = {"仇を討つための強大な力を得るため",
				  "魔王から世界を守り平和をもたらすため",
				  "師の背を追い偉大な魔法使いになるため"};
}

void EventPhase::Update(void)
{
	//シーン切り替え直後のEnterキー貫通を防ぐ
	waitTimer_++;

	//1分間何も入力せずに放置した場合の隠し分岐
	if (waitTimer_ >= TIMEOUT_FRAMES)
	{
		playerStatus_->currentRoute_ = PLAYER_ROUTE::SELFLESS; //無欲ルートへ
		isFinished_ = true;
		return;
	}

	if (waitTimer_ < 8) return;

	int maxItems = static_cast<int>(routeMenu_.size());
	Utility::ProcessCommandMenuSelection(cursor_, maxItems);

	// 決定キーが押されたらルート確定
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_RETURN) ||
		InputManager::GetInstance().IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		switch (cursor_)
		{
		case 0: //打破
			playerStatus_->currentRoute_ = PLAYER_ROUTE::BREAKTHROUGH;
			break;
		case 1: //救世
			playerStatus_->currentRoute_ = PLAYER_ROUTE::SALVATION;
			break;
		case 2: //真理
			playerStatus_->currentRoute_ = PLAYER_ROUTE::TRUTH;
			break;
		}
		PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::CANCEL;//ターンを進めずにメニューに戻る
		isFinished_ = true;
	}
}

void EventPhase::Draw(void)
{
	//フレーバーテキストを描画
	DrawString(100, 100, "「何のために、魔法を学ぶの？」", GetColor(255, 255, 255));

	//選択肢の描画
	Utility::DrawCommandMenu(150, 200, routeMenu_, cursor_);
}

bool EventPhase::IsFinished(void)const
{
	return isFinished_;
}
