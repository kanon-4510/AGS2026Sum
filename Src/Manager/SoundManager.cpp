#include "../Application.h"
#include "SoundManager.h"
#include "Sound.h"

SoundManager* SoundManager::instance_ = nullptr;

void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SoundManager();
	}
	instance_->Init();
}

SoundManager& SoundManager::GetInstance(void)
{
	return *instance_;
}

void SoundManager::Init(void)
{
	using RES = Sound;
	using RES_T = RES::TYPE;

	static std::string PATH_EFF = Application::PATH_SOUND;
	Sound res;

	//タイトルBGM
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Title_BGM.mp3");
	res.ChangeMaxVolume(0.5);
	soundMap_.emplace(SRC::TITLE_BGM, res);

	//ゲームシーンBGM
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Game_BGM.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::GAME_BGM, res);

	//クエストBGM
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Quest_BGM.mp3");
	res.ChangeMaxVolume(0.55);
	soundMap_.emplace(SRC::QUEST_BGM, res);

	//ゲームクリアBGM
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Clear_BGM.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::GAMECLEAR_BGM, res);

	//ゲームオーバーBGM
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Over_BGM.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::GAMEOVER_BGM, res);

	//選択SE
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Select_SE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::SELECT_SE, res);

	//ダメージSE
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Damage_SE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::DAMAGE_SE, res);
	
	//攻撃SE
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Attack_SE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::ATTACK_SE, res);
}

void SoundManager::Release(void)
{
	for (auto& p : soundMap_)
	{
		p.second.Release();
	}

	soundMap_.clear();
}

void SoundManager::Destroy(void)
{
	Release();
	soundMap_.clear();
	delete instance_;
}

bool SoundManager::Play(SRC src, Sound::TIMES times)
{

	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		if (!lPair->second.CheckLoad())
		{
			lPair->second.Load();
		}
		return lPair->second.Play(times);
	}
	return false;
}

bool SoundManager::Play(SRC src, Sound::TIMES times, VECTOR pos, float radius)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		if (!lPair->second.CheckLoad())
		{
			lPair->second.Load();
		}
		return lPair->second.Play(pos, radius, times);
	}
	return false;
}

void SoundManager::Stop(SRC src)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		return lPair->second.Stop();
	}
}

bool SoundManager::CheckMove(SRC src)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		return lPair->second.CheckMove();
	}
	return false;
}

void SoundManager::ChengeVolume(SRC src, float per)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		return lPair->second.ChangeVolume(per);
	}
}
