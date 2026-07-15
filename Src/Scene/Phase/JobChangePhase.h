#pragma once
#include "PhaseBase.h"
#include "../../Application.h"
#include "../../Object/PlayerStatus.h"

class GameScene;

class JobChangePhase : public PhaseBase
{
public:

	static constexpr int COUNT_MAX = 180; //メッセージ表示のカウントの最大値

	static constexpr int jobListX = Application::SCREEN_HALFSIZE_X - 390;	//職業リストの描画位置X
	static constexpr int jobListY = Application::SCREEN_HALFSIZE_Y - 280;	//職業リストの描画位置Y

	static constexpr int JOB_LIST_SPACING = 40;	//職業リストの間隔

	static constexpr int bonusX = 400;	//職業ボーナスの描画位置X
	static constexpr int bonusY = 100;	//職業ボーナスの描画位置Y

	JobChangePhase(PlayerStatus* playerStatus, GameScene& gameScene);		//デフォルトコンストラクタ
	//~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	//職業ボーナスを描画する関数
	void DrawJobBonus(const JobData& job);

	void DrawDetails(void); //職業の詳細情報を描画する関数

    // フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;

private:
	GameScene& gameScene_; // ゲームシーンへの参照
	PlayerStatus* playerStatus_; // プレイヤーのステータスへのポインタ
	InputManager& ins_ = InputManager::GetInstance();
	int bgImg_; // 背景画像のハンドル
	int selectedIndex_ = 0; // 現在選択されている職業のインデックス

	int timer_ = 200; //メッセージ表示のカウントを管理する変数

	bool isShowingDetails_ = false; //詳細表示中かどうかを管理するフラグ
	bool isFinished_ = false; //フェーズが終了したかどうかを管理するフラグ
	
	void ProcessJobListSelection(void); //職業選択の処理
	void ProcessDetailsListSelection(void); //職業選択の処理

	void DrawTutorial(void); //チュートリアルの描画処理
};

