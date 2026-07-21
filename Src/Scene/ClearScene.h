#pragma once
#include <memory>
#include "SceneBase.h"

class SceneManager;

class ClearScene : public SceneBase
{
public:
	static constexpr int CLEAR_MESSAGE_X = (Application::SCREEN_SIZE_X - 120) / 2;	//メッセージ
	static constexpr int CLEAR_MESSAGE_Y = Application::SCREEN_SIZE_Y - 200;		//メッセージ

	static constexpr int ARROW_X = CLEAR_MESSAGE_X - 20;	//矢印の表示位置X
	static constexpr int ARROW_Y = CLEAR_MESSAGE_Y;			//矢印の表示位置Y
	static constexpr int EXIT_ARROW_X = 500;				//ゲーム終了確認メッセージ

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
	int normalOffset_ = 0;
	int exitOffset_ = 0;
};