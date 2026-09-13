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
	normalOffsetX_ = 0;
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
		if (selectIndex_ == 0)
		{
			normalOffsetX_ = 0;	//矢印のオフセット値を更新
			normalOffsetY_ = selectIndex_ * TEXT_OFFSET_Y;	//矢印のオフセット値を更新
		}
		else
		{
			normalOffsetX_ = selectIndex_ * TEXT_OFFSET_X;	//矢印のオフセット値を更新
			normalOffsetY_ = selectIndex_ * TEXT_OFFSET_Y;	//矢印のオフセット値を更新
		}
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
	SetFontSize(TITLE_FONT_SIZE);
	std::string str = "世界一の魔法使いになった！";
	int width = GetDrawStringWidth(str.c_str(), str.size());
	DrawString(GAMECLEAR_MESSAGE_X, GAMECLEAR_MESSAGE_Y,str.c_str(), 0xFFFFFF);

	SetFontSize(OPTION_FONT_SIZE);
	if (!isFinishCheck_)
	{
		DrawFormatString(ARROW_X + normalOffsetX_, ARROW_Y + normalOffsetY_, Color::WHITE, "→");

		DrawFormatString(CLEAR_MESSAGE_X, CLEAR_MESSAGE_Y, Color::WHITE, "もう一度遊ぶ");

		DrawFormatString(TITLE_TEXT_X, TITLE_TEXT_Y, Color::WHITE, "タイトル");
	}
	else
	{
		DrawFormatString(EXIT_ARROW_X - exitOffset_, EXIT_ARROW_Y, Color::WHITE, "→");

		DrawFormatString(EXIT_MESSAGE_X, EXIT_MESSAGE_Y, Color::WHITE, "ゲームを終了しますか？");

		DrawString(EXIT_YES_X, EXIT_YES_Y, "はい", Color::WHITE);
		DrawString(EXIT_NO_X, EXIT_NO_Y, "いいえ", Color::WHITE);
	}
	SetFontSize(DEFAULT_FONT_SIZE);
}

void ClearScene::Release(void)
{
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAMECLEAR_BGM);
}
