#include <DxLib.h>
#include "../../Manager/InputManager.h"
#include "../../Manager/SoundManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Application.h"
#include "../../Common/Color.h"
#include "Pause.h"

// ※ 必要な定数やマクロは適宜ヘッダーかこちらで定義してください
#define UI_PAUSE_IMG_HEIGHT 100 //元のコードの値を参照

Pause::Pause() {}

Pause::~Pause()
{
}

void Pause::Init() {
    pauseImg_ = LoadGraph("Data/Image/pause.png");
    // 説明用画像の読み込み（パスは元のコードに合わせて調整してください）
    explainImgs_.push_back(LoadGraph("Data/Image/explain0.png"));
    explainImgs_.push_back(LoadGraph("Data/Image/explain1.png"));
}

bool Pause::Update(InputManager& ins) {
    tikatiCnt_++;

    // TABまたはSTARTでポーズ切替
    if (ins.IsTrgDown(KEY_INPUT_TAB) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::STRAT)) {
        isPaused_ = !isPaused_;
        if (isPaused_) {
            state_ = State::Menu;
            selectIndex_ = 0;
        }
        return true;
    }

    if (!isPaused_) return false;

    if (state_ == State::Menu) {
        // 上下選択
        if (ins.IsTrgDown(KEY_INPUT_DOWN) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN)) {
            selectIndex_ = (selectIndex_ + 1) % MENU_ITEM_COUNT;
        }
        if (ins.IsTrgDown(KEY_INPUT_UP) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP)) {
            selectIndex_ = (selectIndex_ + (MENU_ITEM_COUNT - 1)) % MENU_ITEM_COUNT;
        }

        // 決定処理
        if (ins.IsTrgDown(KEY_INPUT_RETURN) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
            switch (selectIndex_) {
            case 0: isPaused_ = false; break; // 戻る
            case 1: state_ = State::ShowControls; break;
            case 2: state_ = State::ShowItems; break;
            case 3: SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE); break;
            }
        }
    }
    else {
        // 子画面から戻る
        if (ins.IsTrgDown(KEY_INPUT_RETURN) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
            SoundManager::GetInstance().Play(SoundManager::SRC::SELECT_SE, Sound::TIMES::ONCE);
            state_ = State::Menu;
        }
    }
    return true;
}

void Pause::Draw() {
    if (!isPaused_) return;

    // 背景暗転
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
    DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, Color::BLACK, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    if (state_ == State::Menu) {
        DrawRotaGraph((Application::SCREEN_SIZE_X / VALUE_TWO), UI_PAUSE_IMG_HEIGHT, PAUSE_IMG_UI_SIZE, 0, pauseImg_, true);
        SetFontSize(DEFAULT_FONT_SIZE * 5.0);

        DrawString((Application::SCREEN_SIZE_X / VALUE_TWO) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_1, "ゲームに戻る", Color::WHITE);
        if (selectIndex_ % PAUSE_MENU_ITEM_COUNT == 0)
            DrawString((Application::SCREEN_SIZE_X / VALUE_TWO) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_1, "ゲームに戻る", Color::YELLOW);

        DrawString((Application::SCREEN_SIZE_X / VALUE_TWO) - UI_WIDTH_PAUSE_1, UI_HEIGHT_PAUSE_2, "操作説明", Color::WHITE);
        if (selectIndex_ % PAUSE_MENU_ITEM_COUNT == 1)
            DrawString((Application::SCREEN_SIZE_X / VALUE_TWO) - UI_WIDTH_PAUSE_1, UI_HEIGHT_PAUSE_2, "操作説明", Color::YELLOW);

        DrawString((Application::SCREEN_SIZE_X / VALUE_TWO) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_3, "アイテム概要", Color::WHITE);
        if (selectIndex_ % PAUSE_MENU_ITEM_COUNT == VALUE_TWO)
            DrawString((Application::SCREEN_SIZE_X / VALUE_TWO) - UI_WIDTH_PAUSE_3, UI_HEIGHT_PAUSE_3, "アイテム概要", Color::YELLOW);

        DrawString((Application::SCREEN_SIZE_X / VALUE_TWO) - UI_WIDTH_PAUSE_2, UI_HEIGHT_PAUSE_4, "タイトルへ", Color::WHITE);
        if (selectIndex_ % PAUSE_MENU_ITEM_COUNT == 3)
            DrawString((Application::SCREEN_SIZE_X / VALUE_TWO) - UI_WIDTH_PAUSE_2, UI_HEIGHT_PAUSE_4, "タイトルへ", Color::YELLOW);

        SetFontSize(DEFAULT_FONT_SIZE);
    }
    else if (state_ == State::ShowControls)
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
        DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), Color::WHITE, true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        DrawGraph(0, 0, explainImgs_[0], true);
        SetFontSize(DEFAULT_FONT_SIZE * 2.5);
        DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", Color::YELLOW);
        if (tikatiCnt_ % FLASH * 2.0 <= FLASH)DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", Color::WHITE);
        SetFontSize(DEFAULT_FONT_SIZE);
    }
    else if (state_ == State::ShowItems)
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
        DrawBox(0, 0, (Application::SCREEN_SIZE_X), (Application::SCREEN_SIZE_Y), Color::WHITE, true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        DrawGraph(0, 0, explainImgs_[1], true);
        SetFontSize(DEFAULT_FONT_SIZE * 2.5);
        DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", Color::YELLOW);
        if (tikatiCnt_ % FLASH * 2.0 <= FLASH)DrawString(BACK_PAUSE_WIDTH, BACK_PAUSE_HEIGHT, "Enterキーで戻る", Color::WHITE);
        SetFontSize(DEFAULT_FONT_SIZE);
    }
}