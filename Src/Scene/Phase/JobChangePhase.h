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

	static constexpr int FONT_SIZE= 24; //フォントサイズ
	static constexpr int TUTORIAL_FONT_SIZE = 20;	//チュートリアルのフォントサイズ

	static constexpr int COUNT_MAX = 120; //メッセージ表示のカウントの最大値

	static constexpr int jobListX = Application::SCREEN_HALFSIZE_X - 390;	//職業リストの描画位置X
	static constexpr int jobListY = Application::SCREEN_HALFSIZE_Y - 280;	//職業リストの描画位置Y

	static constexpr int ALERT_MSG_X = 480;      // 転職不可メッセージ描画位置X
	static constexpr int ALERT_MSG_Y = 650;      // 転職不可メッセージ描画位置Y

	static constexpr int INDEX_TITLE_X = 430;    // 「目次」タイトル位置X
	static constexpr int INDEX_TITLE_Y = 150;    // 「目次」タイトル位置Y

	static constexpr int JOB_LIST_LEFT_X = 400;  // 職業一覧（左列）X座標
	static constexpr int JOB_LIST_RIGHT_X = 700; // 職業一覧（右列）X座標
	static constexpr int JOB_LIST_START_Y = 200; // 職業一覧の描画開始Y座標
	static constexpr int JOB_LIST_SPACING = 40;  // リストの行間隔
	static constexpr int JOBS_PER_COLUMN = 7;    // 1列あたりの職業数（左右キー移動幅）
	static constexpr int JOB_NAME_X = 380;       // 職業名の描画位置X
	static constexpr int JOB_NAME_Y = 150;       // 職業名の描画位置Y

	static constexpr int JOB_DESC_X = 330;       // 職業説明の描画位置X
	static constexpr int JOB_DESC_Y = 180;       // 職業説明の描画位置Y
	static constexpr int SKILL_NAME_OFFSET_X = 70; // スキル名描画のXオフセット
	static constexpr int SKILL_NAME_OFFSET_Y = 160;// スキル名描画のYオフセット
	static constexpr int SKILL_DESC_OFFSET_Y = 190;// スキル説明描画のYオフセット

	static constexpr int JOB_STATUS_SPACING_X = 670; // 必要ステータス見出し位置X
	static constexpr int JOB_STATUS_SPACING_Y = 150; // 必要ステータス見出し位置Y

	static constexpr int JOB_STATUS_X = 730;    // 必要ステータス数値位置X
	static constexpr int JOB_STATUS_Y = 200;    // 必要ステータス数値位置Y

	static constexpr int JOB_BONUS_X = 730;     // 職業ボーナス描画位置X
	static constexpr int JOB_BONUS_Y = 370;     // 職業ボーナス描画位置Y
	static constexpr int BONUS_Y_INTERVAL = 40; // ボーナス各項目の縦間隔

	//本のアニメーション・画像設定
	static constexpr int PAGE_ANIME_FRAME_COUNT = 8; //アニメーション枚数
	static constexpr int PAGE_DIV_NUM_X = 4;
	static constexpr int PAGE_DIV_NUM_Y = 2;
	static constexpr int PAGE_IMAGE_SIZE_X = 790;
	static constexpr int PAGE_IMAGE_SIZE_Y = 790;

	static constexpr int PAGE_ANIME_SPEED = 5;      //コマ送り速度（フレーム数）
	static constexpr int PAGE_ANIME_MAX_TIMER = 40; //アニメーション終了タイマー上限
	static constexpr int PAGE_ANIME_OFFSET_Y = 230;//本めくり画像の表示Yオフセット

	//特殊職業インデックス
	static constexpr int ARCHMAGE_JOB_INDEX = 13;   //大魔法使いのリスト内インデックス

	//チュートリアル
	static constexpr int TUTORIAL_TARGET_TURN = 3;  //チュートリアル発動ターン

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
	int deskImg_; //背景画像(机)のハンドル
	int bookImg_; //背景画像(本)のハンドル
	int messageBoxImg_; //背景画像(メッセージ)のハンドル
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

