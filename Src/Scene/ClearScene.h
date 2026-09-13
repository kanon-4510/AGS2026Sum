#pragma once
#include <memory>
#include "SceneBase.h"

class SceneManager;

class ClearScene : public SceneBase
{
public:
	// フォントサイズ
	static constexpr int TITLE_FONT_SIZE = 70;
	static constexpr int OPTION_FONT_SIZE = 30;

	static constexpr int GAMECLEAR_MESSAGE_X = (Application::SCREEN_HALFSIZE_X) / 3;	//負けた時のメッセージ
	static constexpr int GAMECLEAR_MESSAGE_Y = Application::SCREEN_HALFSIZE_Y - 200;			//負けた時のメッセージ

	static constexpr int CLEAR_MESSAGE_X = (Application::SCREEN_SIZE_X - 170) / 2;	//メッセージ
	static constexpr int CLEAR_MESSAGE_Y = Application::SCREEN_SIZE_Y - 200;		//メッセージ

	static constexpr int ARROW_X = CLEAR_MESSAGE_X - 40;	//矢印の表示位置X
	static constexpr int ARROW_Y = CLEAR_MESSAGE_Y;			//矢印の表示位置Y
	static constexpr int EXIT_ARROW_X = 500;				//ゲーム終了確認メッセージ

	//矢印のオフセット値
	static constexpr int TEXT_OFFSET_X = 30;		//文字の間隔
	static constexpr int TEXT_OFFSET_Y = 40;		//文字の間隔

	static constexpr int TITLE_TEXT_X = CLEAR_MESSAGE_X + 30;				//タイトル選択文字の表示位置X
	static constexpr int TITLE_TEXT_Y = Application::SCREEN_SIZE_Y - 160;	//タイトル選択文字の表示位置Y

	// 終了確認画面（isFinishCheck_ == true）用レイアウトパラメータ
	static constexpr int EXIT_ARROW_Y = 400;

	//"終了しますか"メッセージの表示位置
	static constexpr int EXIT_MESSAGE_X = (Application::SCREEN_SIZE_X - 220) / 2;
	static constexpr int EXIT_MESSAGE_Y = Application::SCREEN_SIZE_Y / 2;

	//はいの選択肢の表示位置
	static constexpr int EXIT_YES_X = (Application::SCREEN_SIZE_X - 210) / 2 + 125;
	static constexpr int EXIT_YES_Y = Application::SCREEN_SIZE_Y / 2 + 40;

	//いいえの選択肢の表示位置
	static constexpr int EXIT_NO_X = (Application::SCREEN_SIZE_X - 210) / 2;
	static constexpr int EXIT_NO_Y = Application::SCREEN_SIZE_Y / 2 + 40;

	static constexpr int EXIT_OPTION_OFFSET_Y = 40;

	//強制遷移
	static constexpr int FORCE_RETURN_TIME = 700;

	ClearScene(void);	//コンストラクタ
	~ClearScene(void);	//デストラクタ

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:
	InputManager& ins_ = InputManager::GetInstance();//inputManagerのインスタンスを取得

	bool isFinishCheck_ = false;	//終了するかの確認
	int selectIndex_ = 0;			//選択肢のインデックス

	//矢印のオフセット値
	int normalOffsetX_ = 0;
	int normalOffsetY_ = 0;
	int exitOffset_ = 0;
};