#pragma once

#include "PhaseBase.h"
#include "../../Object/PlayerStatus.h"

class JobChangePhase : public PhaseBase
{
public:

	static constexpr int COUNT_MAX = 180; //メッセージ表示のカウントの最大値

	static constexpr int bonusX = 400;	//職業ボーナスの描画位置X
	static constexpr int bonusY = 100;	//職業ボーナスの描画位置Y

	JobChangePhase(PlayerStatus* playerStatus);		//デフォルトコンストラクタ
	//~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	//職業ボーナスを描画する関数
	void DrawJobBonus(const JobData& job);

    // フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;

private:
	PlayerStatus* playerStatus_; // プレイヤーのステータスへのポインタ
	InputManager& ins_ = InputManager::GetInstance();
	int selectedIndex_ = 0; // 現在選択されている職業のインデックス

	int timer_ = 200; //メッセージ表示のカウントを管理する変数

	bool isFinished_ = false; // フェーズが終了したかどうかを管理するフラグ

	void DrawTutorial(void); //チュートリアルの描画処理
};

