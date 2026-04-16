#pragma once
#include <memory>
#include <chrono>

class SceneBase;
class Fader;

class SceneManager
{
public:
	//60FPS固定の定数
	static constexpr float DEFAULT_FPS = 60.0f;

	// シーン管理用
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		DEMO,
		GAME,
		OVER,
		CLEAR
	};

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static SceneManager& GetInstance(void);

	void Init(void);
	void Update(void);
	void Draw(void);

	// リソースの破棄
	void Destroy(void);

	// 状態遷移
	void ChangeScene(SCENE_ID nextId);

	// シーンIDの取得
	SCENE_ID GetSceneID(void);

	// デルタタイムの取得
	float GetDeltaTime(void) const;

private:

	// 静的インスタンス
	static SceneManager* instance_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;

	// フェード
	std::unique_ptr<SceneBase> scene_;

	// 各種シーン
	std::unique_ptr<Fader> fader_;

	// シーン遷移中判定
	bool isSceneChanging_;

	bool isLoading_ = false;
	int loadingTimer_ = 0;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);
	// コピーコンストラクタも同様
	SceneManager(const SceneManager& manager) = default;
	// デストラクタも同様
	~SceneManager(void) = default;

	// デルタタイムをリセットする
	void ResetDeltaTime(void);

	// シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	// フェード
	void Fade(void);

};
