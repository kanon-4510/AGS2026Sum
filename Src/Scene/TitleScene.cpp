#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Common/Color.h"
#include "TitleScene.h"

////デフォルトコンストラクタ
//SceneTitle::SceneTitle(void)
//{
//	titleImage_ = 0;
//}

////デストラクタ
//SceneTitle::~SceneTitle(void)
//{
//
//}

//初期化処理
void TitleScene::Init(void)
{
	titleImage_ = -1;
}

//更新処理
void TitleScene::Update(void)
{
	ProcessTitleMouse();

	if (mode_ == TITLE_MODE::NORMAL)
	{
		ProcessTitleDecision();
		ProcessTitleSelection();
	}
	else if(mode_ == TITLE_MODE::TUTORIAL)
	{
		Tutorial();
	}
	else if (mode_ == TITLE_MODE::EXIT)
	{
		ExitGame();
	}
}

//描画処理
void TitleScene::Draw(void)
{
	DrawString(0, 0, "Scene : Title", 0xFFFFFF);

	if (mode_ == TITLE_MODE::NORMAL)
	{
		DrawFormatString(ARROW_X, ARROW_Y + arrow_offset_, Color::WHITE, "→");

		DrawFormatString(TITLE_MESSAGE_X, TITLE_MESSAGE_Y, Color::WHITE, "ゲーム開始");

		DrawFormatString(TITLE_MESSAGE_X, Application::SCREEN_SIZE_Y - 160, Color::WHITE, "ゲーム終了");
	}
	else if(mode_ == TITLE_MODE::TUTORIAL)
	{
		if (tutorialIndex_ == 0)
		{
			if (SceneManager::GetInstance().IsTutorialEnabled()) {
				DrawString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y - 250, "チュートリアル：【 ON 】", Color::YELLOW);
			}
			else {
				DrawString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y - 250, "チュートリアル：【 OFF 】", Color::YELLOW);
			}
			DrawFormatString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y - 200, Color::WHITE, "ゲーム開始");
		}
		else
		{
			if (SceneManager::GetInstance().IsTutorialEnabled()) {
				DrawString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y - 250, "チュートリアル：【 ON 】", Color::WHITE);
			}
			else {
				DrawString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y - 250, "チュートリアル：【 OFF 】", Color::WHITE);
			}
			DrawFormatString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y - 200, Color::YELLOW, "ゲーム開始");
		}
	}
	else if (mode_ == TITLE_MODE::EXIT)
	{
		DrawFormatString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y / 2, Color::WHITE, "ゲームを終了しますか？");
		
		if (confirmIndex_ == 0) // 「はい」が選択されているとき
		{
			DrawString((Application::SCREEN_SIZE_X - 210) / 2, Application::SCREEN_SIZE_Y / 2 + 40, "はい", Color::YELLOW);  // 選択中は黄色
			DrawString((Application::SCREEN_SIZE_X - 220) / 2 + 130, Application::SCREEN_SIZE_Y / 2 + 40, "いいえ", Color::WHITE);
		}
		else // 「いいえ」が選択されているとき
		{
			DrawString((Application::SCREEN_SIZE_X - 210) / 2, Application::SCREEN_SIZE_Y / 2 + 40, "はい", Color::WHITE);
			DrawString((Application::SCREEN_SIZE_X - 220) / 2 + 130, Application::SCREEN_SIZE_Y / 2 + 40, "いいえ", Color::YELLOW); // 選択中は黄色
		}
	}
}

//解放処理
void TitleScene::Release(void)
{
}

void TitleScene::ProcessTitleSelection(void)
{
	if (ins_.IsTrgDown(KEY_INPUT_UP) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP))
	{
		int index = static_cast<int>(titleSelection_);
		index = (index + (static_cast<int>(TITLE_SELECTION::MAX) - 1)) % static_cast<int>(TITLE_SELECTION::MAX);
		titleSelection_ = static_cast<TITLE_SELECTION>(index);
		arrow_offset_ = index * 40; //矢印のオフセット値を更新
	}
	else if (ins_.IsTrgDown(KEY_INPUT_DOWN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		int index = static_cast<int>(titleSelection_);
		index = (index + 1) % static_cast<int>(TITLE_SELECTION::MAX);
		titleSelection_ = static_cast<TITLE_SELECTION>(index);
		arrow_offset_ = index * 40; //矢印のオフセット値を更新
	}
}

void TitleScene::ProcessTitleDecision(void)
{
	if(ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		switch (titleSelection_)
		{
		case TitleScene::START_GAME:
			mode_ = TITLE_MODE::TUTORIAL;
			break;
		/*case TitleScene::TUTORIAL:
			break;*/
		case TitleScene::EXIT_GAME:
			mode_ = TITLE_MODE::EXIT;
			break;
		case TitleScene::MAX:
			break;
		default:
			break;
		}
	}
}

void TitleScene::ProcessTitleMouse(void)
{
	// シングルトンからインスタンスを引っ張ってくる
	auto& input = InputManager::GetInstance();

	// タイトルの選択肢をマウスで選択する処理
	if (mode_ == TITLE_MODE::NORMAL)
	{
		if (input.IsTrgMouseLeft())
		{
			if (input.IsMouseOverRect(TITLE_MESSAGE_X, TITLE_MESSAGE_Y - 5, 90, 20))
			{
				mode_ = TITLE_MODE::TUTORIAL;
				arrow_offset_ = 0; // 矢印のオフセット値を更新
			}
			else if (input.IsMouseOverRect(TITLE_MESSAGE_X, TITLE_MESSAGE_Y + 35, 90, 20))
			{
				mode_ = TITLE_MODE::EXIT;
				arrow_offset_ = 40; // 矢印のオフセット値を更新
			}
		}
	}
	else if(mode_ == TITLE_MODE::TUTORIAL)
	{
		if (input.IsTrgMouseLeft())
		{
			if (input.IsMouseOverRect(TITLE_MESSAGE_X - 55, TITLE_MESSAGE_Y - 55, 215, 25))
			{
				SceneManager::GetInstance().ToggleTutorial(); //ON/OFFを切り替える
			}
			else if (input.IsMouseOverRect(515, 515, 95, 25))
			{
				//ゲームシーンへ遷移
				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			}
		}
	}
	else if(mode_ == TITLE_MODE::EXIT)
	{
		if (input.IsTrgMouseLeft())
		{
			if (input.IsMouseOverRect((Application::SCREEN_SIZE_X - 210) / 2, Application::SCREEN_SIZE_Y / 2 + 40, 30, 20))
			{
				Application::isRunning_ = false; //ゲームを終了
			}
			else if (input.IsMouseOverRect((Application::SCREEN_SIZE_X - 220) / 2 + 130, Application::SCREEN_SIZE_Y / 2 + 40, 50, 20))
			{
				mode_ = TITLE_MODE::NORMAL; //通常メニューに戻る
			}
		}
	}
}

void TitleScene::Tutorial(void)
{
	//上下キーで切り替える
	if (ins_.IsTrgDown(KEY_INPUT_UP) || ins_.IsTrgDown(KEY_INPUT_DOWN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		tutorialIndex_ = 1 - tutorialIndex_;
	}

	//最終決定
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		if (tutorialIndex_ == 0)
		{
			SceneManager::GetInstance().ToggleTutorial();
		}
		else
		{
			//ゲームシーンへ遷移
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
		}
	}
}

void TitleScene::ExitGame(void)
{
	//左右キーで「はい」「いいえ」を切り替える
	if (ins_.IsTrgDown(KEY_INPUT_LEFT) || ins_.IsTrgDown(KEY_INPUT_RIGHT) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_LEFT) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_RIGHT))
	{
		confirmIndex_ = 1 - confirmIndex_; //0と1を反転させる
	}

	//最終決定
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) || 
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		if (confirmIndex_ == 0)
		{
			//「はい」なら本当にゲーム終了
			Application::isRunning_ = false;
		}
		else
		{
			//「いいえ」なら通常メニューモードに戻る
			mode_ = TITLE_MODE::NORMAL;
		}
	}
}
