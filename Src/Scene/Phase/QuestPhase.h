#pragma once

#include "PhaseBase.h"

class QuestPhase : public PhaseBase
{
public:
	//QuestPhase(void);		//デフォルトコンストラクタ
	//~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	// フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;

private:

	bool isFinished_ = false; // フェーズが終了したかどうかを管理するフラグ
};

