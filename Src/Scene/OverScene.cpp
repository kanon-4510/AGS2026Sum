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
	normalOffsetX_ = 0;

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
		if (selectIndex_ == 0)
		{
			normalOffsetX_ = 0;	//矢印のオフセット値を更新
			normalOffsetY_ = selectIndex_ * OPTION_STEP_Y;	//矢印のオフセット値を更新
		}
		else
		{
			normalOffsetX_ = selectIndex_ * TITLE_RETURN_TEXT_OFFSET_X;	//矢印のオフセット値を更新
			normalOffsetY_ = selectIndex_ * OPTION_STEP_Y;	//矢印のオフセット値を更新
		}
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
	SetFontSize(TITLE_FONT_SIZE);
	std::string str = "世界を守れなかった";
	int width = GetDrawStringWidth(str.c_str(),str.size() );
	DrawString(Application::SCREEN_HALFSIZE_X - width / VALUE, GAMEOVER_MESSAGE_Y, str.c_str(), Color::RED);

	SetFontSize(OPTION_FONT_SIZE);
	DrawFormatString(ARROW_X - normalOffsetX_, ARROW_Y + normalOffsetY_, Color::WHITE, "→");

	DrawFormatString(OVER_MESSAGE_X, OVER_MESSAGE_Y, Color::WHITE, "別の道へ");

	DrawFormatString(OVER_MESSAGE_X - TITLE_RETURN_TEXT_OFFSET_X, Application::SCREEN_SIZE_Y - TITLE_RETURN_BOTTOM_MARGIN_Y, Color::WHITE, "受け入れる");
	SetFontSize(DEFAULT_FONT_SIZE);
}

//解放処理
void OverScene::Release(void)
{
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAMEOVER_BGM);
}
