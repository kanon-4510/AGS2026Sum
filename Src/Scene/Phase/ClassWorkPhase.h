#pragma once

#include "PhaseBase.h"

class ClassWorkPhase : public PhaseBase
{
public:
	enum CLASSWORK_SELECT
	{
		PHARMACY,       //薬学
		MARTIALARTS,    //武術
		MAGICKNOWLEDGE, //魔法知識
		FAITH,          //信仰
		ARCHAEOLOGY,    //考古学
		ASTROLOGY,      //占星術
		MAX				//最大値
	};

	static constexpr int COUNT_MAX = 1; // フェーズが終了するまでのカウントの最大値

	//ClassWorkPhase(void);		//デフォルトコンストラクタ
	//~ClassWorkPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	// フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;
private:
	InputManager& ins_ = InputManager::GetInstance();
	int timer_ = 0; // フェーズの経過時間を管理するタイマー

	bool isFinished_ = false; // フェーズが終了したかどうかを管理するフラグ

	CLASSWORK_SELECT select_;

	void ProcessClassworkSelection(void);	//履修選択の処理
	void ProcessClassworkDecision(void);	//履修選択の確定
};