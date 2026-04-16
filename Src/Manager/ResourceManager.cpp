#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
{

	// 推奨しませんが、どうしても使いたい方は
	using RES = Resource;
	using RES_T = RES::TYPE;
	static std::string PATH_IMG = Application::PATH_IMAGE;
	static std::string PATH_MDL = Application::PATH_MODEL;
	static std::string PATH_EFF = Application::PATH_EFFECT;

	std::unique_ptr<Resource> res;

	// プレイヤー
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "NPlayer/Player.mv1");
	resourcesMap_.emplace(SRC::PLAYER, std::move(res));

	// プレイヤー影
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Shadow.png");
	resourcesMap_.emplace(SRC::PLAYER_SHADOW, std::move(res));

	// ライト
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Light.png");
	resourcesMap_.emplace(SRC::LIGHT, std::move(res));

	// スカイドーム
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "SkyDome/SkyDome.mv1");
	resourcesMap_.emplace(SRC::SKY_DOME, std::move(res));

	// ステージ
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Stage/Stage.mv1");
	resourcesMap_.emplace(SRC::MAIN_PLANET, std::move(res));
	
	// ステージ
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Stage/untitled.mv1");
	resourcesMap_.emplace(SRC::STAGE, std::move(res));

	//エネミー
	//----------------------------------
	//犬
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/Yellow/Dog.mv1");
	resourcesMap_.emplace(SRC::DOG, std::move(res));

	//サボテン
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/Cactus/Cactus.mv1");
	resourcesMap_.emplace(SRC::SABO, std::move(res));

	//ミミック
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/mimic/Mimic.mv1");
	resourcesMap_.emplace(SRC::MIMIC, std::move(res));

	//キノコ
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/mushroom/mushroom.mv1");
	resourcesMap_.emplace(SRC::MUSH, std::move(res));

	//玉ねぎ
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/onion/onion.mv1");
	resourcesMap_.emplace(SRC::ONION, std::move(res));//敵
	
	//トゲゾー
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/thorn/thorn.mv1");
	resourcesMap_.emplace(SRC::TOGE, std::move(res));

	//ウィルス
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/virus/virus.mv1");
	resourcesMap_.emplace(SRC::VIRUS, std::move(res));
	
	//スケルトン
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/skeleton/skeleton.mv1");
	resourcesMap_.emplace(SRC::SKELETON, std::move(res));

	//ボス
	res = std::make_unique<RES>(RES_T::MODEL, PATH_MDL + "Enemy/Boss/Dragon.mv1");
	resourcesMap_.emplace(SRC::BOSS, std::move(res));

	//ゲームタイトル用画像
	//----------------------------------------------
	//ゲーム開始画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Title/Title.png");
	resourcesMap_.emplace(SRC::GAME_TITLE, std::move(res));

	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Title/GameStart.png");
	resourcesMap_.emplace(SRC::GAME_START, std::move(res));
	
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Title/GameLule.png");
	resourcesMap_.emplace(SRC::GAME_LULE, std::move(res));
	
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Title/GameEnd.png");
	resourcesMap_.emplace(SRC::GAME_END, std::move(res));

	//ゲームシーン用画像
	//----------------------------------------------
	
	//選択中のフレーム
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/test.png");
	resourcesMap_.emplace(SRC::FRAME, std::move(res));
	
	//HP
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/HP.png");
	resourcesMap_.emplace(SRC::HP, std::move(res));
	
	//攻撃力
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/Power.png");
	resourcesMap_.emplace(SRC::POWER, std::move(res));
	
	//移動速度
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/Speed.png");
	resourcesMap_.emplace(SRC::SPEED, std::move(res));
	
	//信頼度
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/Intimacy.png");
	resourcesMap_.emplace(SRC::INTIMACY, std::move(res));
	
	//ポイント
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/Point.png");
	resourcesMap_.emplace(SRC::POINT, std::move(res));
	
	//HP強化の説明画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/HpUp1.png");
	resourcesMap_.emplace(SRC::HP_UP, std::move(res));

	//攻撃力強化の説明画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/PowerUp1.png");
	resourcesMap_.emplace(SRC::POWER_UP, std::move(res));

	//移動速度強化の説明画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/SpeedUp1.png");
	resourcesMap_.emplace(SRC::SPEED_UP, std::move(res));

	//信頼度強化の説明画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/IntimacyUp1.png");
	resourcesMap_.emplace(SRC::INTIMACY_UP, std::move(res));
	
	//バトルへ画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/Battle.png");
	resourcesMap_.emplace(SRC::BATTLE_IMG, std::move(res));
	
	//確認画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameScene/BattleStart.png");
	resourcesMap_.emplace(SRC::BATTLE_START_IMG, std::move(res));
	
	//はい画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Yes.png");
	resourcesMap_.emplace(SRC::YES, std::move(res));
	
	//いいえ画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "No.png");
	resourcesMap_.emplace(SRC::NO, std::move(res));
	
	//GameOverScene
	//-------------------------------------
	// ゲームオーバー
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameOver/GameOver.png");
	resourcesMap_.emplace(SRC::GAMEOVER, std::move(res));

	//ボタン
	//------------------------------------
	//Keyboard
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Key/Enter.png");
	resourcesMap_.emplace(SRC::KEY_ENTER, std::move(res));
	
	//XboxのAボタン
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Xbox/A.png");
	resourcesMap_.emplace(SRC::XBOX_A, std::move(res));
	
	//XboxのXボタン
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Xbox/X.png");
	resourcesMap_.emplace(SRC::XBOX_X, std::move(res));
	
	//XboxのYボタン
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Xbox/Y.png");
	resourcesMap_.emplace(SRC::XBOX_Y, std::move(res));
	
	//攻撃アイコン
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Icon/Attack.png");
	resourcesMap_.emplace(SRC::ICON_ATTACK, std::move(res));
	
	//防御アイコン
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Icon/Defense.png");
	resourcesMap_.emplace(SRC::ICON_DEFENSE, std::move(res));
	
	//攻撃アイコン
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Icon/Avoid.png");
	resourcesMap_.emplace(SRC::ICON_AVOID, std::move(res));

	//カーソル
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Cursor.png");
	resourcesMap_.emplace(SRC::CURSOR, std::move(res));

	//エフェクト
	//---------------------------------------
	
	// 足煙
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "Smoke/Smoke.efkefc");
	resourcesMap_.emplace(SRC::FOOT_SMOKE, std::move(res));

	//敵のダメージ
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "Damage.efkefc");
	resourcesMap_.emplace(SRC::EFF_DAMAGE, std::move(res));
	
	//敵のShot攻撃
	res = std::make_unique<RES>(RES_T::EFFEKSEER, PATH_EFF + "Shot.efkefc");
	resourcesMap_.emplace(SRC::EFF_SHOT, std::move(res));

}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	resourcesMap_.clear();
	delete instance_;
}

const Resource& ResourceManager::Load(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(SRC src)
{

	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return lPair->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	// ロード処理
	rPair->second->Load();

	// 念のためコピーコンストラクタ
	loadedMap_.emplace(src, *rPair->second);

	return *rPair->second;

}
