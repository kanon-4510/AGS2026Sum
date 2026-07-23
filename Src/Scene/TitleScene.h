//#pragma once

#include "../Application.h"
#include "../Manager/InputManager.h"
#include "SceneBase.h"

class TitleScene : public SceneBase
{
public:
	static constexpr int TITLE_MESSAGE_X = (Application::SCREEN_SIZE_X - 170) / 2;	//タイトルメッセージ
	static constexpr int TITLE_MESSAGE_Y = Application::SCREEN_SIZE_Y - 140;		//タイトルメッセージ

	static constexpr int ARROW_X = TITLE_MESSAGE_X - 40;	//矢印の表示位置X
	static constexpr int ARROW_Y = TITLE_MESSAGE_Y;			//矢印の表示位置Y

	static constexpr int TUTORIAL_ARROW_Y = 470;		//チュートリアルメッセージ
	static constexpr int EXIT_ARROW_X = Application::SCREEN_HALFSIZE_X - 180;			//ゲーム終了確認メッセージ

	enum TITLE_SELECTION
	{
		START_GAME,	//ゲームスタート
		EXIT_GAME,	//ゲーム終了
		MAX			//最大値
	};

	enum class TITLE_MODE
	{
		NORMAL,		//通常モード
		TUTORIAL,	//チュートリアルモード
		EXIT,		//終了確認モード
	};

	enum class MODE_SELECTION
	{
		NONE,
		//NORMALモードの選択肢
		NORMAL_TUTORIAL,
		NORMAL_EXIT,
		//TUTORIALモードの選択肢
		TUTORIAL_CHANGE,
		TUTORIAL_NEXT,
		//EXITモードの選択肢
		EXIT_YES,
		EXIT_NO
	};

	//TitleScene(void);		//デフォルトコンストラクタ
	//~TitleScene(void);	//デストラクタ

	void Init(void) override;		//初期化処理
	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理
	void Release(void) override;	//解放処理

private:
	InputManager& ins_ = InputManager::GetInstance();

	TITLE_MODE mode_ = TITLE_MODE::NORMAL; //初期状態は通常メニュー
	TITLE_SELECTION titleSelection_;	//タイトルの選択肢
	MODE_SELECTION nextMode = MODE_SELECTION::NONE;	//次のモード選択肢

	//矢印のオフセット値
	int normalOffset_ = 0;
	int tutorialOffsetX_ = 0;
	int tutorialOffsetY_ = 0;
	int exitOffset_ = 0;

	//メンバー変数
	int titleImage_;		//タイトル画像の格納領域
	int currentSelection_;	//現在の選択肢
	int tutorialIndex_ = 0;	//0：ON,1：OFF
	int confirmIndex_ = 0;	//ゲーム終了確認の選択肢インデックス

	void ProcessTitleSelection(void);	//タイトル選択の処理
	void ProcessTitleDecision(void);	//タイトル決定の処理
	void ProcessMouseSelection(void);	//タイトル選択のマウス処理
	void ProcessMouseDecision(MODE_SELECTION nextMode);	//タイトル決定のマウス処理
	void Tutorial(void);				//チュートリアルのON/OFF切り替え処理
	void ExitGame(void);				//ゲーム終了の確認処理
};
