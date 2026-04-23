#pragma once
#include <memory>
#include "../Manager/InputManager.h"
#include "Phase/PhaseBase.h"
#include "SceneBase.h"

class GameScene : public SceneBase
{
public:
	enum QUEST_PHASE {
		PHASE_QUEST,		//戦闘
		PHASE_CLASSWORK,	//授業
		PHASE_JOB_CHANGE,	//資格試験
		PHASE_FINAL,		//最終日
		MAX					//最大値
	};

//最大ターン数
	static constexpr int MAX_TURN = 5;	

	GameScene(void);		//デフォルトコンストラクタ
	~GameScene(void);		//デストラクタ

	void Init(void) override;		//初期化処理
	void Update(void) override;		//更新処理
	void ProcessPhaseDecision();
	void Draw(void) override;		//描画処理
	void Release(void) override;	//解放処理

private:

	InputManager& ins_ = InputManager::GetInstance();

	//ポインタの取得
	//フェーズの管理
	std::unique_ptr<PhaseBase> currentPhase_;

	QUEST_PHASE phase_;	//クエストのフェーズ

	//メンバー変数の宣言
	int turn_;		//現在のターン数

	//メンバー関数
	void ProcessPhaseSelection(void);	//フェーズ選択の処理

};