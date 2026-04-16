#pragma once
#include <map>

class Application;
class GameScene;

class UIPanel
{
public:
	//画像サイズ
	static const int IMAGE_SIZE_X = 600;
	static const int IMAGE_SIZE_Y = 190;
	
	static const int PAD_IMG_SIZE_Y = 64;

	static const int ICON_IMG_SIZE = 128;

	static const int BATTLE_IMG_X = 300;
	static const int BATTLE_IMG_Y = 100;

	//描画位置
	static const int DRAW_POS_X = 200;
	static const int DRAW_POS_Y = 700;

	//バトル画像表示位置
	static constexpr int BATTLE_IMG_START_Y = 200;
	static constexpr int ENTER_IMG_OFFSET_BOTTOM = 220;

	//アイコン表示位置
	static const int ICON_CENTER_POS_X = Application::SCREEN_SIZE_X - Application::SCREEN_HALFSIZE_X / 4;
	static const int ICON_CENTER_POS_Y = Application::SCREEN_SIZE_Y - Application::SCREEN_HALFSIZE_Y / 5;

	//アイコン中心からの距離
	static const int ICON_CENTER_DISTANCE = ICON_IMG_SIZE / 2;
	static const int PAD_CENTER_DISTANCE = PAD_IMG_SIZE_Y / 2;

	//パッドの距離調整用
	static constexpr int PAD_DISTANCE_DIVISION = 5;

	//アイコンのオフセット
	static constexpr int ICON_OFFSET = ICON_IMG_SIZE / 4;
	//アイコンY軸オフセット
	static constexpr int ICON_Y_OFFSET_DOWN = PAD_CENTER_DISTANCE + ICON_IMG_SIZE / 2;

	enum class STATUS_TYPE
	{
		HP,
		POW,
		SPEED,
		INTIMACY,
		POINT, 
		MAX
	};

	enum class ACTION_TYPE
	{
		ATTAC,
		DEFENSE,
		AVOID,
		MAX
	};

	void Init(void);
	void SetType(int type); // GameScene 側の型を直接受け取る
	void Update(void);
	void Draw(void);
	void DrawIcon(void);

private:

	int xboxAImg_;//XboxのAボタンの画像
	int xboxXImg_;//XboxのXボタンの画像
	int xboxYImg_;//XboxのYボタンの画像
	
	int enterImg_;//キーボードのEnterの画像

	int battleImg_; //バトル画面に進む用画像
	
	int statusName_[static_cast<int>(STATUS_TYPE::MAX)];//ステータスのそれぞれの名前
	
	int actionName_[static_cast<int>(ACTION_TYPE::MAX)];//行動のそれぞれの名前

	STATUS_TYPE currentType_;
	std::map<STATUS_TYPE, int> imageHandles_;

	ACTION_TYPE actionType_;
	std::map<ACTION_TYPE, int> actionImages_;
};

