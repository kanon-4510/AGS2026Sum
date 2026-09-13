#pragma once
#include <memory>
#include "../Manager/InputManager.h"
#include "Phase/PhaseBase.h"
#include "SceneBase.h"
#include "../Scene/Phase/QuestPhase.h"
#include "../Object/PlayerStatus.h"

class GameScene : public SceneBase
{
public:
	enum QUEST_PHASE 
	{
		PHASE_QUEST,		//戦闘
		PHASE_CLASSWORK,	//授業
		PHASE_JOB_CHANGE,	//資格試験
		PHASE_FINAL,		//最終日
		PHASE_EVENT,		//イベント
		MAX					//最大値
	};

	//フォントサイズ
	static constexpr int STATUS_FONT_SIZE = 20;	//ステータスのフォントサイズ
	static constexpr int MENU_FONT_SIZE = 32;	//メニューのフォントサイズ

	static constexpr int TUTORUAL_QUEST_TURN = 1;		//チュートリアルの最大ターン数
	static constexpr int TUTORUAL_CLASSWORK_TURN = 2;		//チュートリアルの最大ターン数
	static constexpr int TUTORUAL_JOB_CHANGE_TURN = 3;		//チュートリアルの最大ターン数
	static constexpr int EVENT_TRIGGER_TURN = 4;	//イベントが発生するターン

	//最大ターン数
	static constexpr int MAX_TURN = 21;	
	//メッセージボックスの表示位置
	static constexpr int MESSAGE_BOX_X = 950;
	static constexpr int MESSAGE_BOX_Y = 50;

	// UI描画座標
	static constexpr int TURN_TEXT_X = 350;	//ターン数の表示位置X
	static constexpr int TURN_TEXT_Y = 30;	//ターン数の表示位置Y
	
	static constexpr int ROUTE_TEXT_X = 200;	//ルートの表示位置X
	static constexpr int ROUTE_TEXT_Y = 400;	//ルートの表示位置Y
	
	static constexpr int COMMAND_MENU_X = 200;	//コマンドメニューの表示位置X
	static constexpr int COMMAND_MENU_Y = 200;	//コマンドメニューの表示位置Y
	static constexpr int COMMAND_MENU_STEP_Y = 60;	//コマンドメニューの選択肢間隔Y
	
	static constexpr int PLAYER_IMG_X = 700;	//プレイヤーの画像表示位置X
	static constexpr int PLAYER_IMG_Y = 200;	//プレイヤーの画像表示位置Y

	// チュートリアル表示調整
	static constexpr int TUTORIAL_STEP3_OFFSET_X = 5;	//ターン3のチュートリアルの表示位置調整

	//チュートリアルの表示位置
	static constexpr int TUTORIAL_X = MESSAGE_BOX_X + 15;
	static constexpr int TUTORIAL_Y = MESSAGE_BOX_Y + 20;


	GameScene(void);		//デフォルトコンストラクタ
	~GameScene(void);		//デストラクタ

	void Init(void) override;		//初期化処理
	void Update(void) override;		//更新処理
	void ProcessPhaseDecision();
	void Draw(void) override;		//描画処理
	void Release(void) override;	//解放処理

	int GetTurn(void) {return turn_;}
private:
	InputManager& ins_ = InputManager::GetInstance();
	PlayerStatus* playerStatus_;

	//ポインタの取得
	//フェーズの管理
	std::unique_ptr<PhaseBase> currentPhase_;

	QUEST_PHASE phase_;	//クエストのフェーズ
	int turn_;			//現在のターン数

	int playerImg_; //プレイヤーの画像ハンドル
	int stageImg_;  //ステージの画像ハンドル
	int messageBoxImg_; //メッセージボックスの画像ハンドル

	//メンバー変数の宣言
	bool isInputBlocked_ = false; //入力ブロックフラグ

	//チュートリアル
	void ProcessTutorial(void);	//チュートリアルの処理
	void DrawTutorial(void);	//チュートリアルの描画処理

	//メンバー関数
	void ProcessPhaseSelection(void);	//フェーズ選択の処理
};