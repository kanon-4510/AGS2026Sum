#include "Enemy.h"
#include <cstdlib>
#include <DxLib.h>

//コンストラクタ
Enemy::Enemy(std::string name, int hp, int power1, int power2,int power3, int speed, int exp,
    std::string skill1, std::string skill2, std::string skill3,
    const std::vector<MotionConfig>& motionList, int x, int y)
{
    name_ = name;
    maxHp_ = hp;
    currentHp_ = hp;
    power1_ = power1;
    power2_ = power2;
    power3_ = power3;
    speed_ = speed;
    exp_ = exp;
    skills_[0] = skill1;
    skills_[1] = skill2;
    skills_[2] = skill3;

    //座標のセットとDxLibでの画像読み込み
    x_ = x;
    y_ = y;
    //gh_ = LoadGraph(imagePath);
    InitAnimation(motionList);
}

//デストラクタ（読み込んだ画像をメモリから消去する）
Enemy::~Enemy() 
{
    //DeleteGraph(gh_);
    for (int i = 0; i < ENEMY_ANIM::MAX; ++i)
    {
        for (int j = 0; j < 16; ++j)
        {
            if (images_[i][j] > 0) DxLib::DeleteGraph(images_[i][j]);
        }
    }
}

void Enemy::ChangeAnim(int command)
{
    // command (0:通常, 1:中技, 2:大技, 3:ダメージ, 4:死亡) に応じてアニメーションを強制固定
    if (command == 0)      currentAnim_ = ENEMY_ANIM::ACT_1;
    else if (command == 1) currentAnim_ = ENEMY_ANIM::ACT_2;
    else if (command == 2) currentAnim_ = ENEMY_ANIM::ACT_3;
    else if (command == 3) currentAnim_ = ENEMY_ANIM::DAMAGE;
    else if (command == 4) currentAnim_ = ENEMY_ANIM::DEAD;
    else                   currentAnim_ = ENEMY_ANIM::IDLE; // 想定外の数値は待機へ

    currentFrame_ = 0; // アニメーションを最初のコマにリセット
    animeTimer_ = 0;   // タイマーもリセット

    if (currentAnim_ == ENEMY_ANIM::IDLE)  isAnimFinished_ = true;
    else isAnimFinished_ = false;
}

//技のランダム決定
EnemyActionInfo Enemy::DecideAction()const
{
    int chosenSkill = rand() % 3;
    return { name_, speed_, chosenSkill, skills_[chosenSkill] };
}

//ダメージ処理
void Enemy::Damage(int damage) 
{
    //軽減値の分だけダメージを減らす
    int Damage = damage - guard_;

    //ダメージがマイナスにならないようにする
    if (Damage < 0)Damage = 0;

    currentHp_ -= Damage;
    if (currentHp_ < 0) currentHp_ = 0;
}

//回復処理
void Enemy::Heal(int amount)
{
    currentHp_ += amount;
    if (currentHp_ > maxHp_) 
    {
        currentHp_ = maxHp_; //最大HPを超えないようにブロック
    }
}

std::string Enemy::GetSkill(int index) const
{
    //配列の範囲外(0から2以外)を指定された場合は空の文字列を返す
    if (index < 0 || index >= 3) return "";
    return skills_[index];
}

void Enemy::Update()
{
    if (++animeTimer_ < 6) return; // 6フレームごとに更新
    animeTimer_ = 0;

    int animIndex = static_cast<int>(currentAnim_);
    if (animIndex < 0 || animIndex >= ENEMY_ANIM::MAX) { currentFrame_ = 0; return; }

    int totalFrames = maxFramesTable_[animIndex];
    if (totalFrames <= 0)
    {
        currentFrame_ = 0;
        isAnimFinished_ = true; // アニメーションは完了したとみなす

        // 画像がなくても、ダメージや行動のあとは自動でIDLEに戻す
        if (currentAnim_ <= ENEMY_ANIM::ACT_3 || currentAnim_ == ENEMY_ANIM::DAMAGE)
        {
            currentAnim_ = ENEMY_ANIM::IDLE;
        }
        return; // 安全に処理を抜ける
    }

    // 次のフレームへ進める
    if (++currentFrame_ >= totalFrames)
    {
        //最後のコマまで再生し終えたので、このアニメーションは「完了」とする！
        isAnimFinished_ = true;

        //自動で待機(IDLE)に戻す
        if (currentAnim_ <= ENEMY_ANIM::ACT_3 || currentAnim_ == ENEMY_ANIM::DAMAGE)
        {
            currentAnim_ = ENEMY_ANIM::IDLE;
            currentFrame_ = 0; // IDLEの最初のコマへリセット
        }
        //死亡(DEAD)の場合
        else if (currentAnim_ == ENEMY_ANIM::DEAD)
        {
            //最後のコマ（消滅直前のドット絵など）でピタッと停止させる
            currentFrame_ = totalFrames - 1;
        }
    }
}

//描画処理
void Enemy::Draw() const 
{
    if (currentAnim_ == ENEMY_ANIM::DEAD && IsAnimFinished())
    {
        return; // ここで関数を終了して、描画をスキップする
    }

    //敵の画像を描画
    if (images_[currentAnim_][currentFrame_] > 0)
    {
        DrawRotaGraph(x_,y_,2.5,0,images_[currentAnim_][currentFrame_],true);
    }

    //敵の名前とHPを文字で表示（色の指定は白: GetColor(255,255,255)）
    unsigned int white = GetColor(255, 255, 255);
    DrawFormatString(x_-50, y_ - 72, white, "%s", name_.c_str());
    DrawFormatString(x_-50, y_ - 55, white, "HP: %d / %d", currentHp_, maxHp_);

    //簡易的なHPバー（緑色の矩形）を描画
    if (maxHp_ > 0) 
    {
        int barWidth = 100; //バーの最大幅
        int currentBarWidth = barWidth * currentHp_ / maxHp_;
        //赤い背景
        DrawBox(x_-50, y_-40, x_ + barWidth-50, y_-35, GetColor(255, 0, 0), TRUE);
        //緑の現在値
        DrawBox(x_-50, y_-40, x_ + currentBarWidth-50, y_-35, GetColor(0, 255, 0), TRUE);
    }
}

void Enemy::InitAnimation(const std::vector<MotionConfig>& motionList)
{
    // 二次元配列を 0 で初期化
    std::fill(reinterpret_cast<int*>(images_), reinterpret_cast<int*>(images_) + (ENEMY_ANIM::MAX * 16), 0);
    std::fill(maxFramesTable_, maxFramesTable_ + ENEMY_ANIM::MAX, 0);

    // アニメーション設定をループしてロード
    for (size_t i = 0; i < motionList.size() && i < ENEMY_ANIM::MAX; ++i)
    {
        const auto& config = motionList[i];

        // 画像がない場合は、ACT_1（通常攻撃）を使い回す安全処理
        if (config.imagePath == nullptr)
        {
            if (i > ENEMY_ANIM::ACT_1 && maxFramesTable_[ENEMY_ANIM::ACT_1] > 0)
            {
                maxFramesTable_[i] = maxFramesTable_[ENEMY_ANIM::ACT_1];
                std::copy(std::begin(images_[ENEMY_ANIM::ACT_1]), std::end(images_[ENEMY_ANIM::ACT_1]), std::begin(images_[i]));
            }
            continue;
        }

        int maxFrames = config.numX * config.numY;
        maxFramesTable_[i] = maxFrames;

        // DxLibの分割読み込み
        DxLib::LoadDivGraph(config.imagePath, maxFrames, config.numX, config.numY, config.sizeX, config.sizeY, images_[i]);
    }
}

Enemy* SpawnEnemyByTurn(int turn)
{
    //画面中央付近に表示するための基準座標
    int spawnX = 300;
    int spawnY = 270;

    if (turn >= 21)
    {
        std::vector<MotionConfig> BossAnims;
        return new Enemy("魔王", 999, 50, 50, 50, 50, 0, "こうげき", "回復", "状態異常", BossAnims, spawnX, spawnY);
    }
    //17〜20ターン目
    else if (turn >= 17)
    {
        int enemyType = GetRand(4);
        switch (enemyType)
        {
        case 0: {
            std::vector<MotionConfig> bulletMachinaAnims = {
                { "Data/Image/Enemy/Robot/Bulet/Idle.png",          5, 1, 128, 128 },
                { "Data/Image/Enemy/Robot/Bulet/Attack_1.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Robot/Bulet/Attack_2.png",      8, 1, 128, 128 },
                { "Data/Image/Enemy/Robot/Bulet/Attack_3.png",      3, 1, 128, 128 },
                { "Data/Image/Enemy/Robot/Bulet/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Robot/Bulet/Dead.png",          5, 1, 128, 128 }};
            return new Enemy("バレットマキナ",150,9,30,18,39, 5, "こうげき", "砲撃", "電力チャージ", bulletMachinaAnims, spawnX, spawnY);
        }case 1: {
            std::vector<MotionConfig> wraithAnims = {
                { "Data/Image/Enemy/Ghost/Ghost_Onryo/Idle.png",          6, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Onryo/Attack_1.png",      5, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Onryo/Attack_2.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Onryo/Attack_3.png",      7, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Onryo/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Onryo/Dead.png",          6, 1, 128, 128 }};
            return new Enemy("レイス",132,17,28, 10,34,2, "こうげき", "呪われた包丁", "金縛り", wraithAnims, spawnX, spawnY);
        }case 2: {
            std::vector<MotionConfig> evilLightningWizardAnims = {
                { "Data/Image/Enemy/Caster/Lightning/Idle.png",          7, 1, 128, 128 },
                { "Data/Image/Enemy/Caster/Lightning/Attack_1.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Caster/Lightning/Attack_2.png",     10, 1, 128, 128 },
                { "Data/Image/Enemy/Caster/Lightning/Attack_3.png",     13, 1, 128, 128 },
                { "Data/Image/Enemy/Caster/Lightning/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Caster/Lightning/Dead.png",          5, 1, 128, 128 }};
            return new Enemy("悪の雷魔術師",142,19,23,25,25,3, "こうげき", "電撃斬", "エレキビーム", evilLightningWizardAnims, spawnX, spawnY);
        }case 3: {
            std::vector<MotionConfig> nightKnightAnims = {
                { "Data/Image/Enemy/Knight/Night/Idle.png",          4, 1, 128, 128 },
                { "Data/Image/Enemy/Knight/Night/Attack_1.png",      5, 1, 128, 128 },
                { "Data/Image/Enemy/Knight/Night/Attack_2.png",      6, 1, 128, 128 },
                { "Data/Image/Enemy/Knight/Night/Attack_3.png",      5, 1, 128, 128 },
                { "Data/Image/Enemy/Knight/Night/Hurt.png",          2, 1, 128, 128 },
                { "Data/Image/Enemy/Knight/Night/Dead.png",          6, 1, 128, 128 }};
            return new Enemy("夜の騎士",178,16,29,18,26,4, "こうげき", "切断", "守りの構え", nightKnightAnims, spawnX, spawnY);
        }case 4: {
            std::vector<MotionConfig> darkGorgonAnims = {
                { "Data/Image/Enemy/Gorgon/Dark/Idle.png",          7, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Dark/Attack_1.png",      7, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Dark/Attack_2.png",     16, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Dark/Attack_3.png",      5, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Dark/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Dark/Dead.png",          3, 1, 128, 128 }};
            return new Enemy("ダークゴルゴン",189,21,32,8,27,5, "こうげき", "薙ぎ払い", "蛇にらみ", darkGorgonAnims, spawnX, spawnY);
        }
        }
    }
    //13〜16ターン目
    else if (turn >= 13)
    {
        int enemyType = GetRand(4);
        switch (enemyType)
        {
        case 0: {
            std::vector<MotionConfig> lesserTaurosAnims = {
                { "Data/Image/Enemy/Minotaur/Lesser/Idle.png",         10, 1, 128, 128 },
                { "Data/Image/Enemy/Minotaur/Lesser/Attack.png",        5, 1, 128, 128 },
                { "Data/Image/Enemy/Minotaur/Lesser/Attack.png",        5, 1, 128, 128 },
                { "Data/Image/Enemy/Minotaur/Lesser/Attack.png",        5, 1, 128, 128 },
                { "Data/Image/Enemy/Minotaur/Lesser/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Minotaur/Lesser/Dead.png",          5, 1, 128, 128 }};
            return new Enemy("下級タウロス",127,15,15,10,20,9, "こうげき", "こうげき", "自己再生", lesserTaurosAnims, spawnX, spawnY);
        }case 1: {
            std::vector<MotionConfig> vampireAnims = {
                { "Data/Image/Enemy/Vampire/Idle.png",          5, 1, 128, 128 },
                { "Data/Image/Enemy/Vampire/Attack_1.png",      6, 1, 128, 128 },
                { "Data/Image/Enemy/Vampire/Attack_2.png",      6, 1, 128, 128 },
                { "Data/Image/Enemy/Vampire/Attack_3.png",      6, 1, 128, 128 },
                { "Data/Image/Enemy/Vampire/Hurt.png",          2, 1, 128, 128 },
                { "Data/Image/Enemy/Vampire/Dead.png",          8, 1, 128, 128 }};
            return new Enemy("ヴァンパイア",98,10,18,5,22,8, "こうげき", "血槍", "沈黙の呪い", vampireAnims, spawnX, spawnY);
        }case 2: {
            std::vector<MotionConfig> crowTenguAnims = {
                { "Data/Image/Enemy/Tengu/Karasu/Idle.png",          6, 1, 128, 128 },
                { "Data/Image/Enemy/Tengu/Karasu/Attack_1.png",      6, 1, 128, 128 },
                { "Data/Image/Enemy/Tengu/Karasu/Attack_2.png",      3, 1, 128, 128 },
                { "Data/Image/Enemy/Tengu/Karasu/Attack_3.png",     15, 1, 128, 128 },
                { "Data/Image/Enemy/Tengu/Karasu/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Tengu/Karasu/Dead.png",          6, 1, 128, 128 }};
            return new Enemy("カラス天狗",95,9,20,3,25,6, "こうげき", "居合切り", "破魔空間", crowTenguAnims, spawnX, spawnY);
        }case 3: {
            std::vector<MotionConfig> fireCasterAnims = {
                { "Data/Image/Enemy/Caster/Fire/Idle.png",          7, 1, 128, 128 },
                { "Data/Image/Enemy/Caster/Fire/Attack_1.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Caster/Fire/Attack_2.png",      8, 1, 128, 128 },
                { "Data/Image/Enemy/Caster/Fire/Attack_3.png",     14, 1, 128, 128 },
                { "Data/Image/Enemy/Caster/Fire/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Caster/Fire/Dead.png",          6, 1, 128, 128 }};
            return new Enemy("悪の炎魔術師",80,13,15,19,16,7, "こうげき", "ひのたま", "火炎放射", fireCasterAnims, spawnX, spawnY);
        }case 4: {
            std::vector<MotionConfig> gorgonAnims = {
                { "Data/Image/Enemy/Gorgon/Nomal/Idle.png",          7, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Nomal/Attack_1.png",      7, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Nomal/Attack_2.png",     10, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Nomal/Attack_3.png",     16, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Nomal/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Nomal/Dead.png",          3, 1, 128, 128 }};
            return new Enemy("ゴルゴン",103,10,16,22,24,10, "こうげき", "かみつく", "薙ぎ払い", gorgonAnims, spawnX, spawnY);}
        }
    }
    //9〜12ターン目
    else if (turn >= 9)
    {
        int enemyType = GetRand(4);
        switch (enemyType)
        {
        case 0: {
            std::vector<MotionConfig> lavaSlimeAnims = {
                { "Data/Image/Enemy/slime/Red/Idle.png",          8, 1, 128, 128 },
                { "Data/Image/Enemy/slime/Red/Attack_1.png",     10, 1, 128, 128 },
                { "Data/Image/Enemy/slime/Red/Attack_2.png",     13, 1, 128, 128 },
                { "Data/Image/Enemy/slime/Red/Attack_2.png",     13, 1, 128, 128 },
                { "Data/Image/Enemy/slime/Red/Hurt.png",          6, 1, 128, 128 },
                { "Data/Image/Enemy/slime/Red/Dead.png",          3, 1, 128, 128 }};
            return new Enemy("溶岩スライム",70,6,9,9,18,14, "こうげき", "溶岩パンチ", "溶岩パンチ", lavaSlimeAnims, spawnX, spawnY);
        }case 1: {
            std::vector<MotionConfig> ghostAnims = {
                { "Data/Image/Enemy/Ghost/Ghost_Yurei/Idle.png",          5, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Yurei/Attack_1.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Yurei/Attack_2.png",      7, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Yurei/Attack_3.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Yurei/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Yurei/Dead.png",          4, 1, 128, 128 }};
            return new Enemy("ゴースト",62,8,0,5,17,11, "こうげき", "呪い", "発狂", ghostAnims, spawnX, spawnY);
        }case 2: {
            std::vector<MotionConfig> morningKnightAnims = {
                { "Data/Image/Enemy/Knight/Morning/Idle.png",          4, 1, 128, 128 },
                { "Data/Image/Enemy/Knight/Morning/Attack_1.png",      5, 1, 128, 128 },
                { "Data/Image/Enemy/Knight/Morning/Attack_2.png",      6, 1, 128, 128 },
                { "Data/Image/Enemy/Knight/Morning/Attack_3.png",      5, 1, 128, 128 },
                { "Data/Image/Enemy/Knight/Morning/Hurt.png",          2, 1, 128, 128 },
                { "Data/Image/Enemy/Knight/Morning/Dead.png",          6, 1, 128, 128 }
            };
            return new Enemy("朝の騎士",65,7,13,10,19,13, "こうげき", "斬撃", "守る", morningKnightAnims, spawnX, spawnY);
        }case 3: {
            std::vector<MotionConfig> magicBombAnims = {
                { "Data/Image/Enemy/Spirit/Idle.png",          6, 1, 128, 128 },
                { "Data/Image/Enemy/Spirit/Attack_1.png",     14, 1, 128, 128 },
                { "Data/Image/Enemy/Spirit/Attack_2.png",     11, 1, 128, 128 },
                { "Data/Image/Enemy/Spirit/Attack_3.png",     13, 1, 128, 128 },
                { "Data/Image/Enemy/Spirit/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Spirit/Dead.png",          5, 1, 128, 128 }};
            return new Enemy("マジックボム",53,8,12,14,20,12, "こうげき", "ばくはつ", "放熱", magicBombAnims, spawnX, spawnY);
        }case 4: {
            std::vector<MotionConfig> trentAnims = {
                { "Data/Image/Enemy/Plent/Idle.png",          5, 1, 128, 128 },
                { "Data/Image/Enemy/Plent/Attack_1.png",      8, 1, 128, 128 },
                { "Data/Image/Enemy/Plent/Attack_2.png",     11, 1, 128, 128 },
                { "Data/Image/Enemy/Plent/Attack_3.png",     15, 1, 128, 128 },
                { "Data/Image/Enemy/Plent/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Plent/Dead.png",          2, 1, 128, 128 }};
            return new Enemy("トレント",56,8,3,6,16,14, "こうげき", "毒の粉", "大地の恵み", trentAnims, spawnX, spawnY);}
        }
    }
    //5〜9ターン目
    else if (turn >= 5)
    {
        int enemyType = GetRand(4);
        switch (enemyType)
        {
        case 0: {
            std::vector<MotionConfig> poisonSlimeAnims{
                { "Data/Image/Enemy/Slime/Green/Idle.png",          8, 1, 128, 128 },
                { "Data/Image/Enemy/Slime/Green/Attack_1.png",     10, 1, 128, 128 },
                { "Data/Image/Enemy/Slime/Green/Attack_1.png",     10, 1, 128, 128 },
                { "Data/Image/Enemy/Slime/Green/Attack_2.png",     13, 1, 128, 128 },
                { "Data/Image/Enemy/Slime/Green/Hurt.png",          6, 1, 128, 128 },
                { "Data/Image/Enemy/Slime/Green/Dead.png",          3, 1, 128, 128 }};
            return new Enemy("毒スライム", 35,5,3,5,13,20, "こうげき", "毒パンチ", "こうげき", poisonSlimeAnims, spawnX, spawnY);
        }case 1: {
            std::vector<MotionConfig> whiteWolfAnims{
                { "Data/Image/Enemy/Wolf/White/Idle.png",           8, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/White/Attack_1.png",       4, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/White/Attack_2.png",      11, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/White/Attack_3.png",       7, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/White/Hurt.png",           2, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/White/Dead.png",           2, 1, 128, 128 }};
            return new Enemy("ホワイトウルフ",32,6,7,8,12,25, "こうげき", "ひっかく", "きりさく", whiteWolfAnims, spawnX, spawnY);
        }case 2: {
            std::vector<MotionConfig> blackWolfAnims{
                { "Data/Image/Enemy/Wolf/Black/Idle.png",           8, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/Black/Attack_1.png",       4, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/Black/Attack_2.png",      11, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/Black/Attack_3.png",       7, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/Black/Hurt.png",           2, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/Black/Dead.png",           2, 1, 128, 128 }};
            return new Enemy("ブラックウルフ",32,3,4,5,19,23, "こうげき", "ひっかく", "きりさく", blackWolfAnims, spawnX, spawnY);
        }case 3: {
            std::vector<MotionConfig> tengAnims{
                { "Data/Image/Enemy/Tengu/Yamabushi/Idle.png",          6, 1, 128, 128 },
                { "Data/Image/Enemy/Tengu/Yamabushi/Attack_1.png",      3, 1, 128, 128 },
                { "Data/Image/Enemy/Tengu/Yamabushi/Attack_2.png",      6, 1, 128, 128 },
                { "Data/Image/Enemy/Tengu/Yamabushi/Attack_3.png",     15, 1, 128, 128 },
                { "Data/Image/Enemy/Tengu/Yamabushi/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Tengu/Yamabushi/Dead.png",          6, 1, 128, 128 }};
            return new Enemy("天狗",34,3,8,2,15,18, "こうげき", "いあいぎり", "ふきつなかぜ", tengAnims, spawnX, spawnY);
        }case 4: {
            std::vector<MotionConfig> skeletonWarriorAnims{
                { "Data/Image/Enemy/Skelton/Warrior/Idle.png",          7, 1, 128, 128 },
                { "Data/Image/Enemy/Skelton/Warrior/Attack_1.png",      6, 1, 128, 128 },
                { "Data/Image/Enemy/Skelton/Warrior/Attack_2.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Skelton/Warrior/Attack_3.png",      1, 1, 128, 128 },
                { "Data/Image/Enemy/Skelton/Warrior/Hurt.png",          2, 1, 128, 128 },
                { "Data/Image/Enemy/Skelton/Warrior/Dead.png",          4, 1, 128, 128 }};
            return new Enemy("剣スケルトン",40,3,7,7,14,19, "こうげき", "ざんげき", "まもる", skeletonWarriorAnims, spawnX, spawnY);}
        }
    }
    //1〜4ターン目
    else
    {
        int enemyType = GetRand(3);
        switch (enemyType)
        {
        case 0:{
            std::vector<MotionConfig> slimeAnims = {
                { "Data/Image/Enemy/Slime/Blue/Idle.png",          8, 1, 128, 128 },
                { "Data/Image/Enemy/Slime/Blue/Attack_1.png",      10, 1, 128, 128 },
                { "Data/Image/Enemy/Slime/Blue/Attack_1.png",      10, 1, 128, 128 },
                { "Data/Image/Enemy/Slime/Blue/Attack_2.png",      13, 1, 128, 128 },
                { "Data/Image/Enemy/Slime/Blue/Hurt.png",          6, 1, 128, 128 },
                { "Data/Image/Enemy/Slime/Blue/Dead.png",          3, 1, 128, 128 }};
            return new Enemy("スライム", 26, 1, 1, 1, 7, 25, "こうげき", "こうげき", "パンチ", slimeAnims, spawnX, spawnY);
        }
        case 1: {
            std::vector<MotionConfig> WerewolfAnims = {
                { "Data/Image/Enemy/Wolf/Red/Idle.png",          8, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/Red/Attack_1.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/Red/Attack_1.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/Red/Attack_2.png",      7, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/Red/Hurt.png",          2, 1, 128, 128 },
                { "Data/Image/Enemy/Wolf/Red/Dead.png",          2, 1, 128, 128 }};
            return new Enemy("オオカミ男", 21, 1, 1, 1, 30, 28, "こうげき", "こうげき", "ひっかく", WerewolfAnims, spawnX, spawnY);
        }
        case 2: {
            std::vector<MotionConfig> skeletonArcherAnims = {
                { "Data/Image/Enemy/Skelton/Archer/Idle.png",          7, 1, 128, 128 },
                { "Data/Image/Enemy/Skelton/Archer/Attack_1.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Skelton/Archer/Attack_2.png",     12, 1, 128, 128 },
                { "Data/Image/Enemy/Skelton/Archer/Attack_1.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Skelton/Archer/Hurt.png",          2, 1, 128, 128 },
                { "Data/Image/Enemy/Skelton/Archer/Dead.png",          5, 1, 128, 128 }};
            return new Enemy("弓スケルトン",21,1,1,1, 9, 27, "こうげき", "どくのや", "こうげき", skeletonArcherAnims, spawnX, spawnY);
        }
        case 3: {
            std::vector<MotionConfig> zombieAnims = {
                { "Data/Image/Enemy/Zombie/Idle.png",       6, 1, 128, 128 },
                { "Data/Image/Enemy/Zombie/Attack.png",     4, 1, 128, 128 },
                { "Data/Image/Enemy/Zombie/Attack.png",     4, 1, 128, 128 },
                { "Data/Image/Enemy/Zombie/Attack.png",     4, 1, 128, 128 },
                { "Data/Image/Enemy/Zombie/Hurt.png",       4, 1, 128, 128 },
                { "Data/Image/Enemy/Zombie/Dead.png",       5, 1, 128, 128 }};
            return new Enemy("ゾンビ", 21,2,2,2,9,26,"こうげき", "こうげき", "こうげき", zombieAnims, spawnX, spawnY);}
        }
    }
}

Enemy* SpawnRushEnemy(int stage)
{
    int spawnX = 300;
    int spawnY = 270;

    switch (stage)
    {
    case 0:
    {
        std::vector<MotionConfig> minotaurAnims = {
                { "Data/Image/Enemy/Minotaur/Ark/Idle.png",         10, 1, 128, 128 },
                { "Data/Image/Enemy/Minotaur/Ark/Attack.png",        5, 1, 128, 128 },
                { "Data/Image/Enemy/Minotaur/Ark/Attack.png",        5, 1, 128, 128 },
                { "Data/Image/Enemy/Minotaur/Ark/Idle.png",         10, 1, 128, 128 },
                { "Data/Image/Enemy/Minotaur/Ark/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Minotaur/Ark/Dead.png",          5, 1, 128, 128 }
        };
        return new Enemy("ミノタウロス",200,25,25,20,30,0, "こうげき", "こうげき", "自己再生", minotaurAnims, spawnX, spawnY);
    }case 1: {
        std::vector<MotionConfig> swordMachinaAnims = {
                { "Data/Image/Enemy/Robot/Sword/Idle.png",          5, 1, 128, 128 },
                { "Data/Image/Enemy/Robot/Sword/Attack_1.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Robot/Sword/Attack_2.png",      6, 1, 128, 128 },
                { "Data/Image/Enemy/Robot/Sword/Attack_3.png",      5, 1, 128, 128 },
                { "Data/Image/Enemy/Robot/Sword/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Robot/Sword/Dead.png",          4, 1, 128, 128 }
        };
        return new Enemy("ソードマキナ",130,29,39,16,35,0, "こうげき", "雷連撃", "電力チャージ", swordMachinaAnims, spawnX, spawnY);
    }   case 2: {
        std::vector<MotionConfig> demonFoxAnims = {
                { "Data/Image/Enemy/Youko/Idle.png",         14, 1, 128, 128 },
                { "Data/Image/Enemy/Youko/Attack_1.png",     10, 1, 128, 128 },
                { "Data/Image/Enemy/Youko/Attack_2.png",     10, 1, 128, 128 },
                { "Data/Image/Enemy/Youko/Attack_3.png",      7, 1, 128, 128 },
                { "Data/Image/Enemy/Youko/Hurt.png",          2, 1, 128, 128 },
                { "Data/Image/Enemy/Youko/Dead.png",         10, 1, 128, 128 }
        };
        return new Enemy("妖狐",163,25,19,34,27,0, "こうげき", "鬼火", "蒼炎", demonFoxAnims, spawnX, spawnY);
    }case 3: {
        std::vector<MotionConfig> fallenWarriorAnims = {
                { "Data/Image/Enemy/Ghost/Ghost_Samurai/Idle.png",          6, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Samurai/Attack_1.png",      5, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Samurai/Attack_2.png",      4, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Samurai/Attack_3.png",      2, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Samurai/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Ghost/Ghost_Samurai/Dead.png",          6, 1, 128, 128 }
        };
        return new Enemy("落武者",182,15,28,26,26,0, "こうげき", "切り裂く", "受流しの構え", fallenWarriorAnims, spawnX, spawnY);
    }case 4: {
        std::vector<MotionConfig> evilGorgonAnims = {
                { "Data/Image/Enemy/Gorgon/Evil/Idle.png",          7, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Evil/Attack_1.png",      7, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Evil/Attack_2.png",     10, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Evil/Attack_3.png",      5, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Evil/Hurt.png",          3, 1, 128, 128 },
                { "Data/Image/Enemy/Gorgon/Evil/Dead.png",          3, 1, 128, 128 }};
        return new Enemy("イビルゴルゴン",173,35,28,26,39,0,"こうげき", "毒牙", "石化の魔眼", evilGorgonAnims, spawnX, spawnY);}
    }
    return nullptr;
}