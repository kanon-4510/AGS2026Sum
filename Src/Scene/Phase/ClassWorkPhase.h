#pragma once

#include "PhaseBase.h"

class ClassWorkPhase : public PhaseBase
{
public:
	//ClassWorkPhase(void);		//デフォルトコンストラクタ
	//~ClassWorkPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	// フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;

private:
};

