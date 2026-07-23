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
	titleImage_ = LoadGraph("Data/Image/Title.png");
}

//更新処理
void TitleScene::Update(void)
{
	if (ins_.IsMouseMoved())
	{
		ProcessMouseSelection();
	}
	ProcessMouseDecision(nextMode);

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
	DrawGraph(0, 0, titleImage_, true);

	if (mode_ == TITLE_MODE::NORMAL)
	{
		DrawFormatString(ARROW_X, ARROW_Y + normalOffset_, Color::WHITE, "→");

		DrawFormatString(TITLE_MESSAGE_X, TITLE_MESSAGE_Y, Color::WHITE, "ゲーム開始");

		DrawFormatString(TITLE_MESSAGE_X, Application::SCREEN_SIZE_Y - 160, Color::WHITE, "ゲーム終了");
	}
	else if (mode_ == TITLE_MODE::TUTORIAL)
	{
		DrawFormatString(ARROW_X - 50, TUTORIAL_ARROW_Y + tutorialOffset_, Color::WHITE, "→");

		if (SceneManager::GetInstance().IsTutorialEnabled()) {
			DrawString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y - 250, "チュートリアル：【 ON 】", Color::WHITE);
		}
		else {
			DrawString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y - 250, "チュートリアル：【 OFF 】", Color::WHITE);
		}
		DrawFormatString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y - 200, Color::WHITE, "ゲーム開始");
	}
	else if (mode_ == TITLE_MODE::EXIT)
	{
		DrawFormatString(EXIT_ARROW_X - exitOffset_, 400, Color::WHITE, "→");

		DrawFormatString((Application::SCREEN_SIZE_X - 220) / 2, Application::SCREEN_SIZE_Y / 2, Color::WHITE, "ゲームを終了しますか？");

		DrawString((Application::SCREEN_SIZE_X - 210) / 2 + 125, Application::SCREEN_SIZE_Y / 2 + 40, "はい", Color::WHITE);
		DrawString((Application::SCREEN_SIZE_X - 210) / 2, Application::SCREEN_SIZE_Y / 2 + 40, "いいえ", Color::WHITE);
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
		normalOffset_ = index * 40; //矢印のオフセット値を更新
	}
	else if (ins_.IsTrgDown(KEY_INPUT_DOWN) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN))
	{
		int index = static_cast<int>(titleSelection_);
		index = (index + 1) % static_cast<int>(TITLE_SELECTION::MAX);
		titleSelection_ = static_cast<TITLE_SELECTION>(index);
		normalOffset_ = index * 40; //矢印のオフセット値を更新
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

void TitleScene::ProcessMouseSelection(void)
{
	nextMode = MODE_SELECTION::NONE;
	//タイトルの選択肢をマウスで選択する処理
	if (mode_ == TITLE_MODE::NORMAL)
	{
		if (ins_.IsMouseOverRect(TITLE_MESSAGE_X, TITLE_MESSAGE_Y - 5, 90, 20))
		{
			normalOffset_ = 0; //矢印のオフセット値を更新
			nextMode = MODE_SELECTION::NORMAL_TUTORIAL;
		}
		else if (ins_.IsMouseOverRect(TITLE_MESSAGE_X, TITLE_MESSAGE_Y + 35, 90, 20))
		{
			normalOffset_ = 40; //矢印のオフセット値を更新
			nextMode = MODE_SELECTION::NORMAL_EXIT;
		}
	}
	else if (mode_ == TITLE_MODE::TUTORIAL)
	{
		if (ins_.IsMouseOverRect(TITLE_MESSAGE_X - 55, TITLE_MESSAGE_Y - 55, 215, 25))
		{
			tutorialOffset_ = 0;
			nextMode = MODE_SELECTION::TUTORIAL_CHANGE;
		}
		else if (ins_.IsMouseOverRect(515, 515, 95, 25))
		{
			tutorialOffset_ = 50;
			nextMode = MODE_SELECTION::TUTORIAL_NEXT;
		}
	}
	else if (mode_ == TITLE_MODE::EXIT)
	{
		if (ins_.IsMouseOverRect((Application::SCREEN_SIZE_X - 210) / 2, Application::SCREEN_SIZE_Y / 2 + 40, 50, 20))
		{
			exitOffset_ = 0;
			nextMode = MODE_SELECTION::EXIT_NO;
		}
		else if (ins_.IsMouseOverRect((Application::SCREEN_SIZE_X - 210) / 2 + 125, Application::SCREEN_SIZE_Y / 2 + 40, 30, 20))
		{
			exitOffset_ = -130;
			nextMode = MODE_SELECTION::EXIT_YES;
		}
	}
}

void TitleScene::ProcessMouseDecision(MODE_SELECTION nextMode)
{
	if (!ins_.IsTrgMouseLeft() || nextMode == MODE_SELECTION::NONE) return;

	if (ins_.IsTrgMouseLeft())
	{
		switch (nextMode)
		{
		case MODE_SELECTION::NORMAL_TUTORIAL:
			mode_ = TITLE_MODE::TUTORIAL;
			break;
		case MODE_SELECTION::NORMAL_EXIT:
			mode_ = TITLE_MODE::EXIT;
			break;
		case MODE_SELECTION::TUTORIAL_CHANGE:
			SceneManager::GetInstance().ToggleTutorial(); //ON,OFFを切り替える
			break;
		case MODE_SELECTION::TUTORIAL_NEXT:
			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);//ゲームシーンへ遷移
			break;
		case MODE_SELECTION::EXIT_YES:
			Application::isRunning_ = false; //ゲームを終了
			break;
		case MODE_SELECTION::EXIT_NO:
			mode_ = TITLE_MODE::NORMAL; //通常メニューに戻る
			break;
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
		tutorialOffset_ = tutorialIndex_ * 50; //矢印のオフセット値を更新
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
	else if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_TAB) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		mode_ = TITLE_MODE::NORMAL;
	}
}

void TitleScene::ExitGame(void)
{
	//左右キーで「はい」「いいえ」を切り替える
	if (ins_.IsTrgDown(KEY_INPUT_LEFT) || ins_.IsTrgDown(KEY_INPUT_RIGHT) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_LEFT) ||
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_RIGHT))
	{
		confirmIndex_ = 1 - confirmIndex_;	//0と1を反転させる
		exitOffset_ = confirmIndex_ * -130; //矢印のオフセット値を更新
	}

	//最終決定
	if (ins_.IsTrgDown(KEY_INPUT_RETURN) || 
		ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		if (confirmIndex_ == 0)
		{
			//「いいえ」なら通常メニューモードに戻る
			mode_ = TITLE_MODE::NORMAL;
		}
		else
		{
			//「はい」なら本当にゲーム終了
			Application::isRunning_ = false;
		}
	}
}
