#pragma once

class PhaseBase
{

public:

	enum PHASE_RESULT
    {
		NONE,         // まだ結果が出ていない
        NEXT_TURN,    // 通常通り、次のターン（行動ループ）へ進む
        CANCEL,       // キャンセルされたので、コマンド選択をやり直す
        MAX
    };


    // コンストラクタでステータスの場所を教えてもらう
    virtual ~PhaseBase() {}

    // 子クラスで中身を書くための仮想関数
    virtual void Update() = 0;
    virtual void Draw() = 0;

    // フェーズが終了したかどうかを親に伝える
    virtual bool IsFinished() const = 0;

	//フェーズの結果を管理する変数
    PHASE_RESULT phaseResult_ = PHASE_RESULT::NONE;

protected:
};

