#pragma once

class Fader
{

public:

	// フェードが進む速さ
	static constexpr float SPEED_ALPHA = 5;

	// 状態
	enum class STATE
	{
		NONE, 
		FADE_OUT,	// 徐々に暗転
		FADE_IN		// 徐々に明転
	};

	// コンストラクタ
	Fader(void);

	// デストラクタ
	~Fader(void);

	void Init(void);
	void Update(void);
	void Draw(void);

	// 状態の取得
	STATE GetState(void);

	// フェード処理が終了しているか
	bool IsEnd(void);

	// 指定フェードを開始する
	void SetFade(STATE state);

private:

	// 状態
	STATE state_;

	// 透明度
	float alpha_;

	// 状態(STATE)を保ったまま終了判定を行うため、
	// Update -> Draw フレーム変化 Updateの1フレーム判定用
	bool isPreEnd_;

	// フェード処理の終了判定
	bool isEnd_;

};
