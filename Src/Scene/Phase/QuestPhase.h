#pragma once

#include "PhaseBase.h"

class QuestPhase : public PhaseBase
{
public:

	static constexpr int COUNT_MAX = 100; // フェーズが終了するまでのカウントの最大値

	//QuestPhase(void);		//デフォルトコンストラクタ
	//~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	// フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;

private:

	int timer_ = 0; // フェーズの経過時間を管理するタイマー

	bool isFinished_ = false; // フェーズが終了したかどうかを管理するフラグ
};

