//#pragma once

#include "SceneBase.h"

class OverScene : public SceneBase
{
public:
	static constexpr int GAMEOVER_MESSAGE_X = (Application::SCREEN_HALFSIZE_X ) / 2;	//負けた時のメッセージ
	static constexpr int GAMEOVER_MESSAGE_Y = Application::SCREEN_HALFSIZE_Y - 200;			//負けた時のメッセージ
	
	static constexpr int OVER_MESSAGE_X = (Application::SCREEN_SIZE_X - 170) / 2;	//メッセージ
	static constexpr int OVER_MESSAGE_Y = Application::SCREEN_SIZE_Y - 200;			//メッセージ

	static constexpr int ARROW_X = OVER_MESSAGE_X - 40;		//矢印の表示位置X
	static constexpr int ARROW_Y = OVER_MESSAGE_Y;			//矢印の表示位置Y
	static constexpr int EXIT_ARROW_X = 500;				//ゲーム終了確認メッセージ

	//OverScene(void);		//デフォルトコンストラクタ
	//~OverScene(void);		//デストラクタ

	void Init(void) override;		//初期化処理
	void Update(void) override;		//更新処理
	void Draw(void) override;		//描画処理
	void Release(void) override;	//解放処理

private:
	InputManager& ins_ = InputManager::GetInstance();//inputManagerのインスタンスを取得

	int selectIndex_ = 0;	//選択肢のインデックス
	int gameOverImage_;		//ゲームオーバー画像格納領域

	//矢印のオフセット値
	int normalOffsetX_ = 0;
	int normalOffsetY_ = 0;
};