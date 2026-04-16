#include <DxLib.h>
#include "../../Manager/ResourceManager.h"
#include "../../Application.h"
#include "UIPanel.h"

void UIPanel::Init(void)
{
    //トレーニング画面用の画像
    imageHandles_[STATUS_TYPE::HP] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::HP_UP).handleId_;

    imageHandles_[STATUS_TYPE::POW] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::POWER_UP).handleId_;

    imageHandles_[STATUS_TYPE::SPEED] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::SPEED_UP).handleId_;

    imageHandles_[STATUS_TYPE::INTIMACY] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::INTIMACY_UP).handleId_;
   
	//ステータス名画像
    statusName_[static_cast<int>(STATUS_TYPE::HP)] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::HP).handleId_;
    
    statusName_[static_cast<int>(STATUS_TYPE::POW)] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::POWER).handleId_;
    
    statusName_[static_cast<int>(STATUS_TYPE::SPEED)] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::SPEED).handleId_;
    
    statusName_[static_cast<int>(STATUS_TYPE::INTIMACY)] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::INTIMACY).handleId_;
    
    statusName_[static_cast<int>(STATUS_TYPE::POINT)] = 
        ResourceManager::GetInstance().Load(ResourceManager::SRC::POINT).handleId_;
    
	//バトル画面に進む用画像
    battleImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::BATTLE_IMG).handleId_;
    
	//操作説明用の画像
    enterImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::KEY_ENTER).handleId_;
    xboxAImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::XBOX_A).handleId_;
    xboxXImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::XBOX_X).handleId_;
    xboxYImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::XBOX_Y).handleId_;

    //バトル画面用の画像
    actionName_[static_cast<int>(ACTION_TYPE::ATTAC)] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::ICON_ATTACK).handleId_;

    actionName_[static_cast<int>(ACTION_TYPE::DEFENSE)] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::ICON_DEFENSE).handleId_;

    actionName_[static_cast<int>(ACTION_TYPE::AVOID)] =
        ResourceManager::GetInstance().Load(ResourceManager::SRC::ICON_AVOID).handleId_;
}

void UIPanel::SetType(int type)
{
    currentType_ = static_cast<STATUS_TYPE>(type);
}

void UIPanel::Update(void)
{
}

void UIPanel::Draw(void)
{
    if (GetJoypadNum() == 0)
    {
        DrawGraph(Application::SCREEN_SIZE_X - BATTLE_IMG_X - DRAW_POS_X, Application::SCREEN_SIZE_Y - ENTER_IMG_OFFSET_BOTTOM, enterImg_, true);
    }
    else
    {
        DrawGraph(Application::SCREEN_SIZE_X - BATTLE_IMG_X - DRAW_POS_X, Application::SCREEN_SIZE_Y - ENTER_IMG_OFFSET_BOTTOM, xboxAImg_, true);
    }

    for (int i = 0; i < static_cast<int>(STATUS_TYPE::MAX); ++i)
    {
        int y = BATTLE_IMG_START_Y + i * BATTLE_IMG_Y;

        DrawGraph(DRAW_POS_X, y, statusName_[i], true);
    }

    int img = imageHandles_[currentType_];
    DrawGraph(DRAW_POS_X, DRAW_POS_Y, img, true);
    
    DrawRotaGraph(Application::SCREEN_SIZE_X - BATTLE_IMG_X, Application::SCREEN_SIZE_Y - BATTLE_IMG_START_Y,
        1,
        0,
        battleImg_,
        true);
}

void UIPanel::DrawIcon(void)
{
    int centerPosX_ = ICON_CENTER_POS_X;
    int centerPosY_ = ICON_CENTER_POS_Y - ICON_IMG_SIZE;
    int show = PAD_DISTANCE_DIVISION;

	//操作説明パッド
    DrawGraph(centerPosX_, centerPosY_ + (PAD_CENTER_DISTANCE + PAD_CENTER_DISTANCE / show), xboxAImg_, true);   //A
    DrawGraph(centerPosX_, centerPosY_ - (PAD_CENTER_DISTANCE + PAD_CENTER_DISTANCE / show), xboxYImg_, true);   //Y
    DrawGraph(centerPosX_ - (PAD_CENTER_DISTANCE + PAD_CENTER_DISTANCE / show), centerPosY_, xboxXImg_, true);   //X
   
	//操作説明アイコン
    DrawGraph(centerPosX_ - (ICON_OFFSET), centerPosY_ + (ICON_Y_OFFSET_DOWN), actionName_[(int)ACTION_TYPE::ATTAC], true);   //下
    DrawGraph(centerPosX_ - (ICON_OFFSET), centerPosY_ - (PAD_CENTER_DISTANCE + ICON_IMG_SIZE), actionName_[(int)ACTION_TYPE::DEFENSE], true); //上
    DrawGraph(centerPosX_ - (ICON_OFFSET) - ICON_IMG_SIZE, centerPosY_ - (ICON_OFFSET), actionName_[(int)ACTION_TYPE::AVOID], true);   //左
}
