#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/Color.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/InputManager.h"
#include "../GameScene.h"
#include "JobChangePhase.h"

JobChangePhase::JobChangePhase(PlayerStatus* playerStatus, GameScene& gameScene) :playerStatus_(playerStatus), gameScene_(gameScene)
{
    deskImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::DESK).handleId_;
    bookImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::BOOK).handleId_;
    messageBoxImg_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::MESSAGE_BOX).handleId_;
    LoadDivGraph("Data/Image/Book/ListBookRtoL.png",
        8,4,2,790,790, pageLeftImg_);
    LoadDivGraph("Data/Image/Book/ListBookLtoR.png",
        8,4,2,790,790, pageRightImg_);
    playerStatus_->InitJob(); //職業の初期化
}

void JobChangePhase::Update(void)
{
    if (!isShowingDetails_)
    {
        //職業リストの選択処理
        ProcessJobListSelection();
    }
    else
    {
        //職業詳細の選択処理
        ProcessDetailsListSelection();
    }
}

void JobChangePhase::Draw(void)
{
	DrawString(0, 0, "Scene : Job Change", 0xFFFFFF);
	DrawGraph(0, 0, deskImg_, true);
	DrawGraph(jobListX, jobListY, bookImg_, true);
    auto& jobList = playerStatus_->GetJobList();

    //現在の職業名を取得
    std::string Job = playerStatus_->GetJobName();

    if(timer_ < COUNT_MAX)
    {
        DrawString(100, 80, "現在の職業と同じ職業は選択できません！", Color::RED);
        timer_++; //メッセージ表示のカウントを増やす
	}

    SetFontSize(FONT_SIZE);
    if (!isShowingDetails_)
    {
        DrawString(430, 150, "目 次", Color::BROWN);

        int half = static_cast<int>(jobList.size() / 2);
        if (jobList.size() % 2 != 0) {
            half++; // 奇数の場合、前半（左列）が1つ多くなるように調整
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

            //描画位置の設定
            int drawX = 400;
            int drawY = 200;

            if (i < half) {
                //前半半分左側の列
                drawY = 200 + i * JOB_LIST_SPACING;
            }
            else {
                //後半半分右側の列
                drawX = 700; //右列のX座標。職業名の長さに合わせて数値を調整してください
                drawY = 200 + (i - half) * JOB_LIST_SPACING; //Y座標を上（200）から再スタート
            }

            //選択中の職業
            //選択中の職業（矢印「→」）
            if (i == selectedIndex_) {
                //描画するX座標（drawX）の少し左（-20 ピクセル）に矢印を出すように自動化
                DrawString(drawX - JOB_LIST_SPACING, drawY, "→", color);
            }

            //計算した drawX, drawY で描画
            DrawFormatString(drawX, drawY, color, "%s", jobList[i].status.name.c_str());
        }
        DrawAnimation();
    }
    else
    {
        DrawDetails();
        DrawJobBonus(jobList[selectedIndex_]);
        DrawAnimation();
    }
    
    DrawTutorial();

    SetFontSize(DEFAULT_FONT_SIZE);
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
        DrawString(JOB_BONUS_X, JOB_BONUS_Y, "職業ボーナス", Color::BROWN);

        int offset = 1;
        if (bonus.hp > 0) {
            DrawFormatString(JOB_BONUS_X, JOB_BONUS_Y + offset * 40, Color::BLACK, " HP   : +%d", bonus.hp);
            offset++;
        }
        if (bonus.power > 0) {
            DrawFormatString(JOB_BONUS_X, JOB_BONUS_Y + offset * 40, Color::BLACK, "POW   : +%d", bonus.power);
            offset++;
        }
        if (bonus.magic > 0) {
            DrawFormatString(JOB_BONUS_X, JOB_BONUS_Y + offset * 40, Color::BLACK, "MAG   : +%d", bonus.magic);
            offset++;
        }
        if (bonus.speed > 0) {
            DrawFormatString(JOB_BONUS_X, JOB_BONUS_Y + offset * 40, Color::BLACK, "SPD   : +%d", bonus.speed);
            offset++;
        }
    }
}

void JobChangePhase::DrawDetails(void)
{
    if (!isShowingDetails_)
    {
		return; // すでに詳細表示中なら何もしない
    }

    // プレイヤーが持っている全職業リストを取得
    auto& jobList = playerStatus_->GetJobList();

    // 関数を呼び出してテキストを取得
    JobText text = GetJobText(selectedIndex_);

    //職業名
    DrawFormatString(JOB_NAME_X, JOB_NAME_Y, Color::BROWN, "%s", jobList[selectedIndex_].status.name.c_str());
    
    //説明文の描画
    DrawString(JOB_DESC_X, JOB_DESC_Y, text.desc, Color::BLACK);

    if (text.skillName != nullptr)
    {
        DrawFormatString(JOB_DESC_X + 70, JOB_DESC_Y + 160, Color::BROWN, "%s", text.skillName);
        
        DrawString(JOB_DESC_X, JOB_DESC_Y + 220, text.skillDesc, Color::BLACK);
    }

    //ヘッダーテキスト
    DrawString(JOB_STATUS_SPACING_X, JOB_STATUS_SPACING_Y, "必要レベルとステータス", Color::BROWN);
    
    //必要レベル
    DrawFormatString(JOB_STATUS_X, JOB_STATUS_Y, Color::BLACK, "レベル: %d", jobList[selectedIndex_].status.reqLevel_);

    // ステータス描画用の初期Y座標
    int currentY = JOB_STATUS_Y + JOB_LIST_SPACING;
	if (selectedIndex_ == 13)   //大魔法使いの場合は全技能200と表示する
    {
        DrawFormatString(JOB_STATUS_X, currentY, Color::BLACK, "全技能: 100");
    }
    else
    {
        if (jobList[selectedIndex_].status.reqPharmacy_ > 0)
        {
            DrawFormatString(JOB_STATUS_X, currentY, Color::BLACK, "　薬学: %d", jobList[selectedIndex_].status.reqPharmacy_);
            currentY += JOB_LIST_SPACING;
        }
        
        if (jobList[selectedIndex_].status.reqMagicKnowledge_ > 0)
        {
            DrawFormatString(JOB_STATUS_X, currentY, Color::BLACK, "魔法学: %d", jobList[selectedIndex_].status.reqMagicKnowledge_);
            currentY += JOB_LIST_SPACING;
        }

        if (jobList[selectedIndex_].status.reqMartialArts_ > 0)
        {
            DrawFormatString(JOB_STATUS_X, currentY, Color::BLACK, "　武術: %d", jobList[selectedIndex_].status.reqMartialArts_);
            currentY += JOB_LIST_SPACING;
        }

        if (jobList[selectedIndex_].status.reqFaith_ > 0)
        {
            DrawFormatString(JOB_STATUS_X, currentY, Color::BLACK, "　信仰: %d", jobList[selectedIndex_].status.reqFaith_);
            currentY += JOB_LIST_SPACING;
        }

        if (jobList[selectedIndex_].status.reqArchaeology_ > 0)
        {
            DrawFormatString(JOB_STATUS_X, currentY, Color::BLACK, "考古学: %d", jobList[selectedIndex_].status.reqArchaeology_);
            currentY += JOB_LIST_SPACING;
        }

        if (jobList[selectedIndex_].status.reqAstrology_ > 0)
        {
            DrawFormatString(JOB_STATUS_X, currentY, Color::BLACK, "占星術: %d", jobList[selectedIndex_].status.reqAstrology_);
            currentY += JOB_LIST_SPACING;
        }
    }
}

void JobChangePhase::DrawAnimation(void)
{
    if (pageAnimeTimer_ >= 0)
    {
        int animSpeed = 5;  // コマの切り替わる速さ（3フレームごとに1コマ進む）
        int currentFrame = pageAnimeTimer_ / animSpeed;

        if (currentFrame > 7) {
            currentFrame = 7; // 配列の範囲（0～7）を超えないようにガード
        }
        if(ispageLR_)
        {
            //本のめくりアニメーションを描画（座標は画面に合わせて調整してください）
            DrawGraph(jobListX, jobListY - 230, pageLeftImg_[currentFrame], true);
        }
        else
        {
            //本のめくりアニメーションを描画（座標は画面に合わせて調整してください）
            DrawGraph(jobListX, jobListY - 230, pageRightImg_[currentFrame], true);
		}
    }
}

bool JobChangePhase::IsFinished() const
{
	return isFinished_;
}

void JobChangePhase::ProcessJobListSelection(void)
{
    if (isShowingDetails_)
    {
        return; //詳細表示中でなければ何もしない
    }

    if (pageAnimeTimer_ >= 0)
    {
        pageAnimeTimer_++;
        if (pageAnimeTimer_ >= 40)
        {
            pageAnimeTimer_ = -1; // 24フレーム経ったら停止状態に戻す
        }
    }

    //プレイヤーが持っている全職業リストを取得
    auto& jobList = playerStatus_->GetJobList();

    //上下キーで選択中の職業（selectedIndex）を動かす処理
    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_UP) ||
        ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_UP)) {
        selectedIndex_ = (selectedIndex_ - 1 + jobList.size()) % jobList.size();
    }
    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_DOWN) ||
        ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_DOWN)) {
        selectedIndex_ = (selectedIndex_ + 1) % jobList.size();
    }
    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_RIGHT) ||
        ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_RIGHT)) {
        selectedIndex_ = (selectedIndex_ + 7) % jobList.size();
    }
    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_LEFT) ||
        ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_LEFT)) {
        selectedIndex_ = (selectedIndex_ - 7 + jobList.size()) % jobList.size();
    }

    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_RETURN) ||
        ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
    {
        ispageLR_ = true; //右にめくる
        pageAnimeTimer_ = 0;
        isShowingDetails_ = true;
    }
    else if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_TAB) ||
        ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
    {
        //キャンセルキーが押されたらフェーズを終了する
        PhaseBase::phaseResult_ = PhaseBase::PHASE_RESULT::CANCEL; //コマンド選択に戻る
        isFinished_ = true;
    }
}

void JobChangePhase::ProcessDetailsListSelection(void)
{
    if (!isShowingDetails_)
    {
        return; //詳細表示中でなければ何もしない
    }

    if (pageAnimeTimer_ >= 0)
    {
        pageAnimeTimer_++;
        if (pageAnimeTimer_ >= 40)
        {
            pageAnimeTimer_ = -1; // 24フレーム経ったら停止状態に戻す
        }
    }
    //プレイヤーが持っている全職業リストを取得
    auto& jobList = playerStatus_->GetJobList();

    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_RIGHT) ||
        ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_RIGHT)) {
        selectedIndex_ = (selectedIndex_ + 1) % jobList.size();
		ispageLR_ = true; //右にめくる
        pageAnimeTimer_ = 0;
    }
    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_LEFT) ||
        ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DG_LEFT)) {
        selectedIndex_ = (selectedIndex_ - 1 + jobList.size()) % jobList.size();
        ispageLR_ = false; //左にめくる
        pageAnimeTimer_ = 0;
    }

    //決定キーが押されたら
    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_RETURN) ||
        ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)) {
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
    else if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_TAB) ||
        ins_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
    {
        //キャンセルキーが押されたら詳細表示を閉じる
        ispageLR_ = false; //左にめくる
        pageAnimeTimer_ = 0;
        isShowingDetails_ = false;
    }
}

JobText JobChangePhase::GetJobText(int index) const
{
    static const JobText JOB_TEXTS[] = {
        //{職業の説明,クラススキル名,スキルの説明}
        { "資格を取得し魔法の使用\nが公に認められた一般的\nな魔法使い。\nこれから魔法とともに冒\n険の旅に出る者。", nullptr, nullptr },
        { "物体に魔法や術式を施し\nて戦うことを得意とする\n魔法使い。\n魔道具の扱いに長けてい\nる。", nullptr, nullptr },
        { "武器に魔力を流し、強化\nされた物理攻撃を得意と\nする。\n剣や斧から鎧など魔力を\n流せるものは様々である。", nullptr, nullptr },
        { "一般魔法使いより多くの\n魔法を使いこなすことが\n出来る。\n多様な魔法を用いて皆を\n導くエリート魔法使い。", nullptr, nullptr },
        { "神からの啓示を受け、聖\nなる力を使う魔法使い。\nモンスターの浄化や呪い\nの解呪など民を守ること\nが多い職業。", nullptr, nullptr },
        { "魔法を私利私欲に使い、\n邪悪を振り撒かんとする\n者達の総称。\n汚染・隷属など攻撃的な\n魔法を多く扱う。", nullptr, nullptr },
        { "水晶玉を通すことで、映\nされた者が辿る運命を見\n通す力を持つ魔法使い。\n直後から最期まで指定し\nた未来を知ることが出来\nる。", nullptr, nullptr },
        { "物質の変換や錬成など、\nこの世の真理とも呼べる\n技術を探求し模索する研\n究家。", "エリクサー", "幻の素材により調合され\nる奇跡の秘薬。\n毎ターンHPを回復する。" },
        { "生来の肉体に加えて、天\nからの護りも加わること\nで突破力と堅牢さを兼ね\n備えた武闘派の魔法使い\n…？", "極聖光", "その拳は敵の弱点をさら\nに強く撃ち抜く。\n会心発生時の威力が上昇\nする。" },
        { "魔導師よりもさらに魔法\nを極めた者にのみ与えら\nれる称号。\nその数は世界中で数える\nほどしかいない。", "カドゥケウス", "その杖からは魔力が際限\nなく溢れ出る。\n物理攻撃に魔力の半分を\n加算する。" },
        { "魔を浄めることに心血を\n注ぎ続けた者。人々を救\nうための瞳と光も、魔族\nには冷酷に映るだろう。", "神秘の護り", "死をも拒絶する神の護り。\n敵の最初の攻撃を防ぐ。" },
        { "死者の蘇生と魂の隷従、\n2つの禁忌の呪文を自在\nに操る恐ろしい魔法使い。\n歪な執着は倫理を超え命\nを支配する。", "ネクロマンス", "死者の軍勢は全てを呑み\n込む。\nWave開始時魔力に基づく\nダメージを与える。" },
        { "占いは人に留まらず星の\n未来まで見通すことが出\n来る。\n次に見えるのは吉祥かは\nたまた凶兆か。", "未来視", "未来を見通し勝利の運命\nを手繰る。\n死亡するような攻撃を1\n度だけ耐える。" },
        { "全てを求めた先にあるの\nはかつての師の背中。\nそこから見える景色は虹\nと影のどちらに満ちてい\nるのだろうか。", "無詠唱魔術", "どの記録にもない師の教\nえ。魔法使用のクールタ\nイムを解除する。" },
    };
    // 配列の要素数を自動計算
    const int totalJobs = sizeof(JOB_TEXTS) / sizeof(JOB_TEXTS[0]);

    // インデックスが範囲外の場合は空のデータを返す安全対策
    if (index < 0 || index >= totalJobs)
    {
        return { "", nullptr, nullptr };
    }

    return JOB_TEXTS[index];
}

void JobChangePhase::DrawTutorial(void)
{
    if (!SceneManager::GetInstance().IsTutorialEnabled()) return;

	DrawGraph(MESSAGE_BOX_X, MESSAGE_BOX_Y, messageBoxImg_, true);
	SetFontSize(20);
	if (gameScene_.GetTurn() == 3)
    {
        if(isShowingDetails_)
        {
            DrawString(45, 570
                , "転職に必要なステータスを\n確認できます。\n詳細画面で決定ボタンを押す\nと転職することができます。\nキャンセルで目次に戻ります"
                , Color::BLACK);
        }
        else
        {
            DrawString(45, 570
                , "転職が可能になると白く表示\nされます。\n決定キーを押すことで詳細\nを確認できます。\nキャンセルで選択に戻ります"
                , Color::BLACK);
		}
    }
	SetFontSize(DEFAULT_FONT_SIZE);
}