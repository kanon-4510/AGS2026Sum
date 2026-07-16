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

	//ゲームタイトル用画像
	//----------------------------------------------
	//ゲーム開始画像
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Title/Title.png");
	resourcesMap_.emplace(SRC::GAME_TITLE, std::move(res));
	//ゲームシーン用画像
	//----------------------------------------------
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Stage/Stage_.png");
	resourcesMap_.emplace(SRC::GAME_SCENE, std::move(res));
	//GameOverScene
	//-------------------------------------
	// ゲームオーバー
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "GameOver/GameOver.png");
	resourcesMap_.emplace(SRC::GAME_OVER, std::move(res));

	//プレイヤー
	//------------------------------------
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "/Player/Player.png");
	resourcesMap_.emplace(SRC::PLAYER, std::move(res));
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "/Player/Battle_Playe1.png");
	resourcesMap_.emplace(SRC::BATTLE_PLAYER, std::move(res));
	//ステージ
	//------------------------------------
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Stage_1.png");
	resourcesMap_.emplace(SRC::STAGE, std::move(res));
	//机
	//------------------------------------
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Desk.png");
	resourcesMap_.emplace(SRC::DESK, std::move(res));
	//本
	//------------------------------------
	res = std::make_unique<RES>(RES_T::IMG, PATH_IMG + "Book/JobBook.png");
	resourcesMap_.emplace(SRC::BOOK, std::move(res));

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
