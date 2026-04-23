#include "PlayerStatus.h"
#include <EffekseerForDXLib.h>
#include "../Manager/SceneManager.h"

PlayerStatus::PlayerStatus()
{
	level = 1;
}

void PlayerStatus::Draw()
{
	//ステータスの描画処理
	DrawFormatString(300, 10, GetColor(255, 255, 255), "Level: %d", level);
	DrawFormatString(300, 30, GetColor(255, 255, 255), "HP: %d/%d", hp, maxHp);

	/*DrawFormatString(300, 50, GetColor(255, 255, 255), "Power: %d", pow);
	DrawFormatString(300, 70, GetColor(255, 255, 255), "Magic Power: %d", mPow);
	DrawFormatString(300, 90, GetColor(255, 255, 255), "Speed: %d", speed);
	DrawFormatString(300, 110, GetColor(255, 255, 255), "Luck: %d", luck);*/
}

void PlayerStatus::Death()
{
	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
}
