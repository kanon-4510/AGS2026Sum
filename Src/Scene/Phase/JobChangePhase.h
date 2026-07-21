#pragma once
#include "PhaseBase.h"
#include "../../Application.h"
#include "../../Object/PlayerStatus.h"

class GameScene;

// 職業テキスト用の構造体
struct JobText
{
	const char* desc;		//職業の説明文
	const char* skillName;	//クラススキル名
	const char* skillDesc;	//スキルの説明文
};

class JobChangePhase : public PhaseBase
{
public:

	static constexpr int COUNT_MAX = 180; //メッセージ表示のカウントの最大値

	static constexpr int jobListX = Application::SCREEN_HALFSIZE_X - 390;	//職業リストの描画位置X
	static constexpr int jobListY = Application::SCREEN_HALFSIZE_Y - 280;	//職業リストの描画位置Y

	static constexpr int JOB_LIST_SPACING = 40;		//リストの間隔

	static constexpr int JOB_NAME_X = 380;	//職業名の描画位置X
	static constexpr int JOB_NAME_Y = 150;	//職業名の描画位置Y

	static constexpr int JOB_DESC_X = 330;	//職業の説明を描画する位置X
	static constexpr int JOB_DESC_Y = 200;	//職業の説明を描画する位置Y

	static constexpr int JOB_STATUS_SPACING_X = 670;	//必要ステータス描画位置X
	static constexpr int JOB_STATUS_SPACING_Y = 150;	//必要ステータス描画位置Y

	static constexpr int JOB_STATUS_X = 730;	//必要ステータスの数値を描画位置X
	static constexpr int JOB_STATUS_Y = 200;	//必要ステータスの数値を描画位置Y

	static constexpr int JOB_BONUS_X = 730;	//職業ボーナスの描画位置X
	static constexpr int JOB_BONUS_Y = 370;	//職業ボーナスの描画位置Y

	JobChangePhase(PlayerStatus* playerStatus, GameScene& gameScene);		//デフォルトコンストラクタ
	//~QuestPhase(void);		//デストラクタ

	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理

	//職業ボーナスを描画する関数
	void DrawJobBonus(const JobData& job);

	//職業の詳細情報を描画する関数
	void DrawDetails(void);

	//職業の詳細情報を描画する関数
	void DrawAnimation(void); 

    //フェーズが終了したかどうかを親に伝える
	virtual bool IsFinished() const override;

private:
	GameScene& gameScene_; //ゲームシーンへの参照
	PlayerStatus* playerStatus_; //プレイヤーのステータスへのポインタ
	InputManager& ins_ = InputManager::GetInstance();
	int deskImg_; //背景画像のハンドル
	int bookImg_; //背景画像のハンドル
	//ページを左にめくる画像のハンドル
	int pageLeftImg_[8];	//右から左
	int pageRightImg_[8];	//左から右
	int pageAnimeTimer_ = -1;	//アニメーションのタイマー
	int currentFrame_ = 0;	//現在のフレーム

	int selectedIndex_ = 0; //現在選択されている職業のインデックス

	int timer_ = 200; //メッセージ表示のカウントを管理する変数

	bool isShowingDetails_ = false; //詳細表示中かどうかを管理するフラグ
	bool ispageLR_ = false; //右か左かフラグ(trueで右から左にめくる)
	bool isFinished_ = false; //フェーズが終了したかどうかを管理するフラグ
	
	//職業選択の処理
	void ProcessJobListSelection(void);
	//職業詳細と選択の選択処理
	void ProcessDetailsListSelection(void);

	//職業の説明文を格納するための関数
	// 指定したインデックスの職業テキストを取得するヘルパー関数
	JobText GetJobText(int index) const;

	void DrawTutorial(void); //チュートリアルの描画処理
};

