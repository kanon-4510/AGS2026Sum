#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/Color.h"
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

        //現在の職業名を取得
        std::string Job = playerStatus_->GetJobName();

        if (selectedJob.status.name == Job) {
            //現在の職業と同じなら転職できない
			timer_ = 0; //メッセージ表示のカウントをリセット
        }
        //PlayerStatusのJobCheckを呼び出して判定
        else if (playerStatus_->JobCheck(selectedJob)) {
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

    auto& jobList = playerStatus_->GetJobList();

    //現在の職業名を取得
    std::string Job = playerStatus_->GetJobName();

    DrawString(100, 50, "【資格試験 - 職業選択】", Color::WHITE);

    if(timer_ < COUNT_MAX)
    {
        DrawString(100, 80, "現在の職業と同じ職業は選択できません！", Color::WHITE);
        timer_++; //メッセージ表示のカウントを増やす
	}

    for (int i = 0; i < jobList.size(); i++) {
        int color = Color::GRAY; //デフォルトはグレー（なれない）

        //条件チェック
        if (playerStatus_->JobCheck(jobList[i])) {
            color = Color::WHITE; //なれる職業は白
        }

        //もしループ中の職業が「現在の職業」なら色を緑色にする
        bool isCurrent = (jobList[i].status.name == Job);
        if (isCurrent) {
            color = Color::GREEN; //現在の職業は緑
        }

        //選択中の職業
        if (i == selectedIndex_) {
            DrawString(80, 100 + i * 25, "→", color);
        }

        DrawFormatString(100, 100 + i * 25, color, "%s", jobList[i].status.name.c_str());
    }

    DrawJobBonus(jobList[selectedIndex_]);
}

void JobChangePhase::DrawJobBonus(const JobData& job)
{
    auto& jobList = playerStatus_->GetJobList();

    if (!jobList.empty() && selectedIndex_ < jobList.size()) {

        //後で元に戻せるように、現在の本当の職業名を退避させておく
        std::string trueJob = playerStatus_->GetJobName();

        //カーソルが合っている職業名を取得
        std::string selectedJobName = jobList[selectedIndex_].status.name;

        //一時的にプレイヤーの職業を「カーソルが合っている職」に変更する
        playerStatus_->SetJob(selectedJobName);

        //GetJobBonus()を呼び出してカーソルが合っている職業のボーナスを取得
        auto bonus = playerStatus_->GetJobBonus();

        //用が済んだので、即座に本当の職業名に戻す
        playerStatus_->SetJob(trueJob);

        //ボーナスの描画処理
        DrawString(bonusX, bonusY, "【職業ボーナス】", Color::YELLOW);

        int offset = 1;
        if (bonus.hp > 0) {
            DrawFormatString(bonusX, bonusY + offset * 25, Color::WHITE, "HP    : +%d", bonus.hp);
            offset++;
        }
        if (bonus.power > 0) {
            DrawFormatString(bonusX, bonusY + offset * 25, Color::WHITE, "POW   : +%d", bonus.power);
            offset++;
        }
        if (bonus.magic > 0) {
            DrawFormatString(bonusX, bonusY + offset * 25, Color::WHITE, "MAG   : +%d", bonus.magic);
            offset++;
        }
        if (bonus.speed > 0) {
            DrawFormatString(bonusX, bonusY + offset * 25, Color::WHITE, "SPD   : +%d", bonus.speed);
            offset++;
        }

        if (offset == 1) {
            DrawString(bonusX, bonusY + 25, "ボーナスなし", GetColor(150, 150, 150));
        }
    }
}

bool JobChangePhase::IsFinished() const
{
	return isFinished_;
}
