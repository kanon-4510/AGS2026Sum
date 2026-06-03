#include "Enemy.h"
#include <cstdlib>
#include <DxLib.h>

//コンストラクタ
Enemy::Enemy(std::string name, int hp, int power, int speed, int exp,
    std::string skill1, std::string skill2, std::string skill3,
    const char* imagePath, int x, int y) 
{
    name_ = name;
    maxHp_ = hp;
    currentHp_ = hp;
    power_ = power;
    speed_ = speed;
    exp_ = exp;
    skills_[0] = skill1;
    skills_[1] = skill2;
    skills_[2] = skill3;

    //座標のセットとDxLibでの画像読み込み
    x_ = x;
    y_ = y;
    gh_ = LoadGraph(imagePath);
}

//デストラクタ（読み込んだ画像をメモリから消去する）
Enemy::~Enemy() 
{
    DeleteGraph(gh_);
}

//技のランダム決定
EnemyActionInfo Enemy::DecideAction()const
{
    int chosenType = rand() % 3;
    return { name_, speed_, chosenType, skills_[chosenType] };
}

//ダメージ処理
void Enemy::TakeDamage(int damage) 
{
    currentHp_ -= damage;
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

//DxLibの描画処理（メインループ内で毎フレーム呼び出す）
void Enemy::Draw() const 
{
    if (IsDead()) return; //死んでいたら描画しない

    //1.敵の画像を描画
    DrawGraph(x_, y_, gh_, TRUE);

    //2. 敵の名前とHPを文字で表示（色の指定は白: GetColor(255,255,255) ）
    unsigned int white = GetColor(255, 255, 255);
    DrawFormatString(x_, y_ - 40, white, "%s", name_.c_str());
    DrawFormatString(x_, y_ - 20, white, "HP: %d / %d", currentHp_, maxHp_);

    //3.【応用】簡易的なHPバー（緑色の矩形）を描画するなら
    if (maxHp_ > 0) {
        int barWidth = 100; //バーの最大幅
        int currentBarWidth = barWidth * currentHp_ / maxHp_;
        //赤い背景
        DrawBox(x_, y_ - 5, x_ + barWidth, y_, GetColor(255, 0, 0), TRUE);
        //緑の現在値
        DrawBox(x_, y_ - 5, x_ + currentBarWidth, y_, GetColor(0, 255, 0), TRUE);
    }
}

Enemy* SpawnEnemyByTurn(int turn) 
{
    //画面中央付近に表示するための基準座標
    int spawnX = 270;
    int spawnY = 180;

    //13〜15ターン目
    if (turn >= 13) 
    {
        int enemyType = GetRand(2);
        switch (enemyType) 
        {
        case 0: return new Enemy("妖狐",          130,30,50,-1,"こうげき","おにび",    "ごくえん",  "media/knight.png", spawnX,spawnY);
        case 1: return new Enemy("ミノタウロス",  180,30,40,-1,"こうげき","かいふく",  "かまえる",  "media/chimera.png",spawnX,spawnY);
        case 2: return new Enemy("イビルゴルゴン",150,30,45,-1,"こうげき","なぎはらい","へびにらみ","media/chimera.png",spawnX,spawnY);
        }
    }
    //10〜12ターン目
    else if (turn >= 10)
    {
        int enemyType = GetRand(3);
        switch (enemyType)
        {
        case 0: return new Enemy("下級タウロス",  150,20,40,1,"こうげき","こうげき","かまえる",  "media/knight.png", spawnX,spawnY);
        case 1: return new Enemy("ダークゴルゴン",140,20,45,1,"こうげき","かみつく","へびにらみ","media/chimera.png",spawnX,spawnY);
        case 2: return new Enemy("夜の騎士",      145,20,45,1,"こうげき","まもる",  "きりすてる","media/chimera.png",spawnX,spawnY);
        case 3: return new Enemy("カラス天狗",    145,20,45,1,"こうげき","はばたく","いあいぎり","media/chimera.png",spawnX,spawnY);
        }
    }
    //7〜9ターン目
    else if (turn >= 7) 
    {
        int enemyType = GetRand(4);
        switch (enemyType) 
        {
        case 0: return new Enemy("ゴルゴン",90,14,25,1,"こうげき","こうげき","なぎはらい","media/gargoyle.png",spawnX,spawnY);
        case 1: return new Enemy("朝の騎士",90,14,25,1,"こうげき","まもる",  "きりすてる","media/gargoyle.png",spawnX,spawnY);
        case 2: return new Enemy("炎使い",  90,14,25,1,"こうげき","ひのたま","やきつくす","media/gargoyle.png",spawnX,spawnY);
        case 3: return new Enemy("雷使い",  90,14,25,1,"こうげき","でんげき","レーザー",  "media/gargoyle.png",spawnX,spawnY);
        case 4: return new Enemy("水使い",  90,14,25,1,"こうげき","すいへき","めいそう",  "media/gargoyle.png",spawnX,spawnY);
        }
    }
    //4〜6ターン目
    else if (turn >= 4)
    {
        int enemyType = GetRand(4);
        switch (enemyType)
        {
        case 0: return new Enemy("マグマスライム",25,5,10,3,"こうげき","こうげき","たいあたり","media/doll.png",    spawnX,spawnY);
        case 1: return new Enemy("剣スケルトン",  40,7,11,7,"こうげき","せつだん","まもる",    "media/gargoyle.png",spawnX,spawnY);
        case 2: return new Enemy("ホワイトウルフ",35,3,15,5,"こうげき","ひっかく","きりさく",  "media/gargoyle.png",spawnX,spawnY);
        case 3: return new Enemy("ブラックウルフ",30,4,15,5,"こうげき","ひっかく","きりさく",  "media/gargoyle.png",spawnX,spawnY);
        case 4: return new Enemy("天狗",          35,6,13,6,"こうげき","こうげき","かぜおこし","media/doll.png",    spawnX,spawnY);
        }
    }
    //1〜3ターン目
    else
    {
        int enemyType = GetRand(3);
        switch (enemyType)
        {
        case 0: return new Enemy("スライム",        15,1, 9,6,"こうげき","こうげき","たいあたり","media/slime.png",spawnX,spawnY);
        case 1: return new Enemy("ポイズンスライム",15,2, 9,6,"こうげき","こうげき","たいあたり","media/beast.png",spawnX,spawnY);
        case 2: return new Enemy("弓スケルトン",    20,2, 9,6,"こうげき","こうげき","しゃげき",  "media/beast.png",spawnX,spawnY);
        case 3: return new Enemy("オオカミ男",      25,2,12,9,"こうげき","こうげき","ひっかく",  "media/beast.png",spawnX,spawnY);
        }
    }
    //万が一バグでここまで来てしまった場合は、強制的にスライムを出してゲームのフリーズを防ぐ
    return new Enemy("スライム",10,10,10,1,"こうげき","こうげき","たいあたり","media/slime.png",spawnX,spawnY);
}