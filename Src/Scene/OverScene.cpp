#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/SoundManager.h"
#include "../Common/Color.h"
#include "OverScene.h"

////デフォルトコンストラクタ
//OverScene::OverScene(void)
//{
//
//}
//
////デストラクタ
//OverScene::~OverScene(void)
//{
//
//}

//初期化処理
void OverScene::Init(void)
{
	//音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::GAMEOVER_BGM, Sound::TIMES::LOOP);
}

//更新処理
void OverScene::Update(void)
{
	if (ins_.IsTrgDown(KEY_INPUT_UP) || ins_.IsTrgDown(KEY_INPUT_DOWN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		selectIndex_ = 1 - selectIndex_;	//0と1を反転させる
		normalOffset_ = selectIndex_ * 40;	//矢印のオフセット値を更新
	}

	if (ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		//選択音
		SoundManager::GetInstance().Play(SoundManager::SRC::SELECT_SE, Sound::TIMES::ONCE);

		if (selectIndex_ == 0)
		{
			//リトライ
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			//リトライの選択時、チュートリアルが有効なら無効にする
			if (SceneManager::GetInstance().IsTutorialEnabled() == true)
			{
				SceneManager::GetInstance().ToggleTutorial();
			}

		}
		else
		{
			//タイトルへ戻る
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		}
	}
}

//描画処理
void OverScene::Draw(void)
{
	DrawString(0, 0, "Scene : GameOver", 0xFFFFFF);


	DrawFormatString(ARROW_X, ARROW_Y + normalOffset_, Color::WHITE, "→");

	DrawFormatString(OVER_MESSAGE_X, OVER_MESSAGE_Y, Color::WHITE, "リトライ");

	DrawFormatString(OVER_MESSAGE_X, Application::SCREEN_SIZE_Y - 160, Color::WHITE, "タイトル");
}

//解放処理
void OverScene::Release(void)
{
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAMEOVER_BGM);
}
