#pragma once

class Pause
{
public:
    //デバッグ &　汎用
    static constexpr float VALUE_ONE = 1.0f;
    static constexpr int VALUE_TWO = 2;
    static constexpr int  VALUE_SIXTY = 60;

	//UI関係-----------------------------------------------------
	//-------------------------------------------------------------------
	static constexpr int GAME_HEIGHT_1 = 80;			//ゲーム開始時の注意書き

	//画像サイズ
	static constexpr float IMG_GAME_UI_1_SIZE = 0.5;	//imgGameUi1_のサイズ
	static constexpr float IMG_OPEGEAR_UI_SIZE = 0.8;	//imgOpeGear_のサイズ
	static constexpr float PAUSE_IMG_UI_SIZE = 0.65;	//pauseImg_のサイズ

	//ポーズメニュー関連
	static constexpr int PAUSE_MENU_ITEM_COUNT = 4;						//ポーズメニューの数
	static constexpr int PAUSE_MENU_DOWN = 1;							//下に移動
	static constexpr int PAUSE_MENU_UP = PAUSE_MENU_ITEM_COUNT - 1;		//上に移動（+3 の代わり）

	//フェード系
	static constexpr int AUTO_FADE = 240;				//自動フェード
	static constexpr int FLASH = 45;					//点滅
	static constexpr int ONE_SECOND_FRAME = 60;			//1秒

	//設定系
	static constexpr int UI_GEAR = 100;					//imgOpeGear_のX,Yの場所

	static constexpr int UI_PAUSE_IMG_HEIGHT = 150;				//pauseImg_の高さ

	static constexpr int UI_WIDTH_PAUSE_1 = 160;				//UIを調整する
	static constexpr int UI_WIDTH_PAUSE_2 = 200;				//UIを調整する
	static constexpr int UI_WIDTH_PAUSE_3 = 240;				//UIを調整する

	static constexpr int UI_HEIGHT_PAUSE_1 = 350;				//１個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_2 = 470;				//２個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_3 = 590;				//３個目のUIの高さ
	static constexpr int UI_HEIGHT_PAUSE_4 = 710;				//４個目のUIの高さ

	static constexpr int UI_ATTACK_X = 10;						//攻撃の文字のX座標
	static constexpr int UI_NORMAL_ATTACK_Y = 450;				//通常攻撃のY座標
	static constexpr int UI_SLASH_ATTACK_Y = 500;				//スラッシュのY座標
	static constexpr int UI_EX_ATTACK_Y = 550;					//回転斬りのY座標

	static constexpr int BACK_PAUSE_WIDTH = 1600;				//ポーズに戻るときのENTERのX
	static constexpr int BACK_PAUSE_HEIGHT = 1020;				//ポーズに戻るときのENTERのY

	//-------------------------------------------------------------------

    enum class State {
        Menu,
        ShowControls,
        ShowItems
    };

    Pause();
    ~Pause();

    void Init();
    // ポーズの更新処理。戻り値がtrueならゲームメイン処理を中断させる
    bool Update(InputManager& ins);
    void Draw();

    bool IsPaused() const { return isPaused_; }
    void SetPaused(bool paused) { isPaused_ = paused; }

private:
    bool isPaused_ = false;
    State state_ = State::Menu;
    int selectIndex_ = 0;
    int pauseImg_ = -1;
    std::vector<int> explainImgs_;
    int tikatiCnt_ = 0;

    // 定数（GameSceneから移動）
    static const int MENU_ITEM_COUNT = 4;
};