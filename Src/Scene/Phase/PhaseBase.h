#pragma once

class PhaseBase
{

public:
    // コンストラクタでステータスの場所を教えてもらう
    virtual ~PhaseBase() {}

    // 子クラスで中身を書くための仮想関数
    virtual void Update() = 0;
    virtual void Draw() = 0;

    // フェーズが終了したかどうかを親に伝える
    virtual bool IsFinished() const = 0;

protected:

};

