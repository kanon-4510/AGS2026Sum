#pragma once
#include <memory>
#include <map>
#include <string>
#include "Resource.h"

class ResourceManager
{
public:
	// リソース名
	enum class SRC
	{
		
		//3Dモデル
		PLAYER_SHADOW,
		LIGHT,
		MAIN_PLANET,
		STAGE,
		SKY_DOME,
		PLAYER,
		//エネミー
		DOG,
		SABO,
		MIMIC,
		MUSH,
		ONION,
		TOGE,
		VIRUS,
		SKELETON,
		BOSS,
		//エフェクト
		FOOT_SMOKE,
		EFF_DAMAGE,
		EFF_SHOT,
		TREE_RANGE,
		EFF_POWER,
		EFF_SPEED,
		EFF_HEAL,
		//タイトル
		GAME_TITLE,
		GAME_START,
		GAME_LULE,
		GAME_END,
		YES,
		NO,
		//ゲームシーンUI
		FRAME,
		HP,
		POWER,
		SPEED,
		INTIMACY,
		POINT,
		HP_UP,
		POWER_UP,
		SPEED_UP,
		INTIMACY_UP,
		BATTLE_IMG,
		BATTLE_START_IMG,
		//ゲームオーバー
		GAMEOVER,

		//キーボード
		KEY_A,
		KEY_D,
		KEY_E,
		KEY_S,
		KEY_W,
		KEY_TAB,
		KEY_ENTER,
		KEY_UP,
		KEY_DOWN,
		KEY_RIGHT,
		KEY_LEFT,
		//Xbox
		XBOX_A,
		XBOX_B,
		XBOX_Y,
		XBOX_X,
		XBOX_LB,
		XBOX_RB,
		XBOX_UP,
		XBOX_DOWN,
		XBOX_RIGHT,
		XBOX_LEFT,
		//ゲームアイコン
		ICON_ATTACK,
		ICON_DEFENSE,
		ICON_AVOID,
		//カーソル
		CURSOR
	};

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	// リソースのロード
	const Resource& Load(SRC src);

	// リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);

private:

	// 静的インスタンス
	static ResourceManager* instance_;

	// リソース管理の対象
	std::map<SRC, std::unique_ptr<Resource>> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	Resource dummy_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);
	ResourceManager(const ResourceManager& manager) = default;
	~ResourceManager(void) = default;

	// 内部ロード
	Resource& _Load(SRC src);

};
