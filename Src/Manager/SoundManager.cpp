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

	// タイトル
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Title.mp3");
	res.ChangeMaxVolume(0.5);
	soundMap_.emplace(SRC::TITEL_BGM, res);

	////デモ
	//res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "DemoBGM.mp3");
	//res.ChangeMaxVolume(0.7);
	//soundMap_.emplace(SRC::DEMO_BGM, res);

	// ゲームシーン
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Game.mp3");
	res.ChangeMaxVolume(0.55);
	soundMap_.emplace(SRC::GAME_BGM, res);

	//強化画面
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Traning.mp3");
	res.ChangeMaxVolume(0.55);
	soundMap_.emplace(SRC::TRANING_BGM, res);

	// ゲームオーバー
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameOver.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::GAMEOVER_BGM, res);

	// ゲームクリア
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameClear.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::GAMECLEAR_BGM, res);

	//攻撃
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameScene/Attack.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::ATTACK_SE, res);

	//攻撃を当てた時
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameScene/Hit.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::HIT_SE, res);
	
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameScene/Defense.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::DEFENSE_SE, res);
	
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameScene/Damage.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::DAMAGE_SE, res);

	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameScene/Avoid.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::AVOID_SE, res);

	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameScene/Shot.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::SHOT_SE, res);

	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameScene/DownSE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::DOWN_SE, res);

	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Pose.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::POSE_SE, res);

	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "Select.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::SELECT_SE, res);
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
