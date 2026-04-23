#pragma once

class Fader
{
public:
	//フェードが進む速さ
	static constexpr float SPEED_ALPHA = 5;

	//状態
	enum class STATE
	{
		NONE, 
		FADE_OUT,	//徐々に暗転
		FADE_IN		//徐々に明転
	};

	Fader(void);//コンストラクタ
	~Fader(void);//デストラクタ

	void Init(void);
	void Update(void);
	void Draw(void);

	STATE GetState(void);		//状態の取得
	bool IsEnd(void);			//フェード処理が終了しているか
	void SetFade(STATE state);	//指定フェードを開始する

	//画像
	int imgLoad_[15];
	int loadingTimer_;
private:
	STATE state_;	//状態
	float alpha_;	//透明度

	//状態(STATE)を保ったまま終了判定を行うため、
	bool isPreEnd_;	//Update -> Draw フレーム変化 Updateの1フレーム判定用
	bool isEnd_;	//フェード処理の終了判定
};