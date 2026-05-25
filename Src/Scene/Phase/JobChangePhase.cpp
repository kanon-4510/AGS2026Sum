#include <DxLib.h>
#include "../../Application.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "JobChangePhase.h"

JobChangePhase::JobChangePhase(PlayerStatus* playerStatus) :playerStatus_(playerStatus)
{
    playerStatus_->InitJob(); //職業の初期化
}

void JobChangePhase::Update(void)
{
	auto& ins = InputManager::GetInstance();
	timer_++; // フェーズの経過時間を増加させる

	if (timer_ > COUNT_MAX) // 例えば、300フレーム経過したらフェーズを終了する
	{
		//isFinished_ = true;
	}

	/*if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		isFinished_ = true;
	}*/

    //プレイヤーが持っている全職業リストを取得
    auto& jobList = playerStatus_->GetJobList();

    //上下キーで選択中の職業（selectedIndex）を動かす処理
    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_UP)) {
        selectedIndex_ = (selectedIndex_ - 1 + jobList.size()) % jobList.size();
    }
    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_DOWN)) {
        selectedIndex_ = (selectedIndex_ + 1) % jobList.size();
    }

    //決定キーが押されたら
    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_RETURN)) {
        const auto& selectedJob = jobList[selectedIndex_];

        //PlayerStatusのJobCheckを呼び出して判定
        if (playerStatus_->JobCheck(selectedJob)) {
            //条件クリア！職業を変更する
            playerStatus_->SetJob(selectedJob.status.name);
			PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::NEXT_TURN; //次のターンへ
            //転職成功のSEなどを鳴らすと良い
            isFinished_ = true;
        }
        else {
            //条件を満たしていない
        }
    }
    else if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_TAB)) {
        //キャンセルキーが押されたらフェーズを終了する
        PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::CANCEL; //コマンド選択に戻る
        isFinished_ = true;
	}
}

void JobChangePhase::Draw(void)
{
	DrawString(0, 0, "Scene : Job Change", 0xFFFFFF);
	DrawFormatString(0, 20, 0xFFFFFF, "カウント %d", timer_);

    auto& jobList = playerStatus_->GetJobList();

    DrawString(100, 50, "【資格試験 - 職業選択】", GetColor(255, 255, 255));

    for (int i = 0; i < jobList.size(); i++) {
        int color = GetColor(100, 100, 100); //デフォルトはグレー（なれない）

        //条件チェック
        if (playerStatus_->JobCheck(jobList[i])) {
            color = GetColor(255, 255, 255); //なれる職業は白
        }

        //選択中の職業は黄色
        if (i == selectedIndex_) {
            DrawString(80, 100 + i * 25, "→", color);
        }

        DrawFormatString(100, 100 + i * 25, color, "%s", jobList[i].status.name.c_str());
    }
}

bool JobChangePhase::IsFinished() const
{
	return isFinished_;
}
