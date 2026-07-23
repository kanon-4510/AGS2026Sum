#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
#include "../Common/Color.h"
#include "ClearScene.h"

ClearScene::ClearScene(void)
{
}

ClearScene::~ClearScene(void)
{
}

void ClearScene::Init(void)
{
	//音楽
	SoundManager::GetInstance().Play(SoundManager::SRC::GAMECLEAR_BGM, Sound::TIMES::LOOP);
	//---------------------------------------------
}

void ClearScene::Update(void)
{
	if (ins_.IsTrgDown(KEY_INPUT_UP) || ins_.IsTrgDown(KEY_INPUT_DOWN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		selectIndex_ = 1 - selectIndex_;	//0と1を反転させる
		normalOffset_ = selectIndex_ * 40;	//矢印のオフセット値を更新
	}

	//入力受付（アニメーション後）
	if (ins_.IsTrgDown(KEY_INPUT_RETURN)||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {

		//選択音
		SoundManager::GetInstance().Play(SoundManager::SRC::SELECT_SE, Sound::TIMES::ONCE);

		if(selectIndex_ == 0)
		{
			//リトライ
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
		}
		else
		{
			//タイトルへ戻る
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
		}
		//音楽
		SoundManager::GetInstance().Play(SoundManager::SRC::SELECT_SE, Sound::TIMES::ONCE);
	}
}

void ClearScene::Draw(void)
{
	//メッセージ（下のテキスト）を表示
	DrawString(0, 0, "Scene : Clear0", 0xFFFFFF);
	SetFontSize(128);
	SetFontSize(DEFAULT_FONT_SIZE);

	if (!isFinishCheck_)
	{
		DrawFormatString(ARROW_X, ARROW_Y + normalOffset_, Color::WHITE, "→");

		DrawFormatString(CLEAR_MESSAGE_X, CLEAR_MESSAGE_Y, Color::WHITE, "リトライ");

		DrawFormatString(CLEAR_MESSAGE_X, Application::SCREEN_SIZE_Y - 160, Color::WHITE, "タイトル");
	}
	else
	{
		DrawFormatString(EXIT_ARROW_X - exitOffset_, 400, Color::WHITE, "→");

		DrawFormatString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y / 2, Color::WHITE, "ゲームを終了しますか？");

		DrawString((Application::SCREEN_SIZE_X - 210) / 2 + 125, Application::SCREEN_SIZE_Y / 2 + 40, "はい", Color::WHITE);
		DrawString((Application::SCREEN_SIZE_X - 210) / 2, Application::SCREEN_SIZE_Y / 2 + 40, "いいえ", Color::WHITE);
	}
}

void ClearScene::Release(void)
{
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAMECLEAR_BGM);
}
