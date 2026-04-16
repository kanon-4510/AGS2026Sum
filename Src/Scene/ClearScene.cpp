#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
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
	auto& ins = InputManager::GetInstance();

	//入力受付（アニメーション後）
	if (ins.IsTrgDown(KEY_INPUT_SPACE)||
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
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

}

void ClearScene::Release(void)
{
	SoundManager::GetInstance().Stop(SoundManager::SRC::GAMECLEAR_BGM);
}
