#include "Enemy.h"
#include <cstdlib>
#include <DxLib.h>

//コンストラクタ
Enemy::Enemy(std::string name, int hp, int power1, int power2,int power3, int speed, int exp,
    std::string skill1, std::string skill2, std::string skill3,
    const char* imagePath, int x, int y)
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
    gh_ = LoadGraph(imagePath);
}

//デストラクタ（読み込んだ画像をメモリから消去する）
Enemy::~Enemy() 
{
    DeleteGraph(gh_);
}

/*//技のランダム決定
EnemyActionInfo Enemy::DecideAction()const
{
    int chosenSkill = rand() % 3;
    return { name_, speed_, chosenSkill, skills_[chosenSkill] };
}*/

//ダメージ処理
void Enemy::Damage(int damage) 
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
    if (maxHp_ > 0) 
    {
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
        int enemyType = GetRand(4);
        switch (enemyType) 
        {
        case 0: return new Enemy("バレットマキナ",150,10,10,10,10,0,"こうげき","砲撃",    "電力チャージ","",spawnX,spawnY);
        case 1: return new Enemy("レイス",        150,10,10,10,10,0,"こうげき","金縛り",  "呪われた包丁","",spawnX,spawnY);
        case 2: return new Enemy("悪の雷魔術師",  150,10,10,10,10,0,"こうげき","電光斬",  "エレキビーム","",spawnX,spawnY);
        case 3: return new Enemy("夜の騎士",      150,10,10,10,10,0,"こうげき","切断",    "守りの構え",  "",spawnX,spawnY);
        case 4: return new Enemy("ダークゴルゴン",150,10,10,10,10,0,"こうげき","薙ぎ払い","蛇にらみ",    "",spawnX,spawnY);
        }
    }
    //10〜12ターン目
    else if (turn >= 10)
    {
        int enemyType = GetRand(4);
        switch (enemyType)
        {
        case 0: return new Enemy("下級タウロス",100,10,10,10,10,0,"こうげき","こうげき","自己再生",  "",spawnX,spawnY);
        case 1: return new Enemy("ヴァンパイア",100,10,10,10,10,0,"こうげき","血槍",    "沈黙の呪い","",spawnX,spawnY);
        case 2: return new Enemy("カラス天狗",  100,10,10,10,10,0,"こうげき","居合切り","破魔空間",  "",spawnX,spawnY);
        case 3: return new Enemy("悪の炎魔術師",100,10,10,10,10,0,"こうげき","ひのたま","火炎放射",  "",spawnX,spawnY);
        case 4: return new Enemy("ゴルゴン",    100,10,10,10,10,0,"こうげき","薙ぎ払い","かみつく",  "",spawnX,spawnY);
        }
    }
    //7〜9ターン目
    else if (turn >= 7) 
    {
        int enemyType = GetRand(4);
        switch (enemyType) 
        {
        case 0: return new Enemy("溶岩スライム",80,10,10,10,10,0,"こうげき","たいあたり","たいあたり","",spawnX,spawnY);
        case 1: return new Enemy("ゴースト",    80,10,10,10,10,0,"こうげき","のろい",    "絶叫",      "",spawnX,spawnY);
        case 2: return new Enemy("朝の騎士",    80,10,10,10,10,0,"こうげき","斬撃",      "守る",      "",spawnX,spawnY);
        case 3: return new Enemy("マジックボム",80,10,10,10,10,0,"こうげき","ばくはつ",  "放熱",      "",spawnX,spawnY);
        case 4: return new Enemy("トレント",    80,10,10,10,10,0,"こうげき","毒の粉",    "大地の恵み","",spawnX,spawnY);
        }
    }
    //4〜6ターン目
    else if (turn >= 4)
    {
        int enemyType = GetRand(4);
        switch (enemyType)
        {
        case 0: return new Enemy("毒スライム",    50,10,10,10,10,0,"こうげき","こうげき",  "たいあたり","",spawnX,spawnY);
        case 1: return new Enemy("ホワイトウルフ",50,10,10,10,10,0,"こうげき","ひっかく",  "きりさく",  "",spawnX,spawnY);
        case 2: return new Enemy("ブラックウルフ",50,10,10,10,10,0,"こうげき","ひっかく",  "きりさく",  "",spawnX,spawnY);
        case 3: return new Enemy("天狗",          50,10,10,10,10,0,"こうげき","ふきつな風","いあいぎり","",spawnX,spawnY);
        case 4: return new Enemy("剣スケルトン",  50,10,10,10,10,0,"こうげき","ざんげき",  "まもる",    "",spawnX,spawnY);
        }
    }
    //1〜3ターン目
    else
    {
        int enemyType = GetRand(3);
        switch (enemyType)
        {
        case 0: return new Enemy("スライム",    19,1,1,1, 7,5,"こうげき","こうげき","たいあたり","",spawnX,spawnY);
        case 1: return new Enemy("オオカミ男",  24,1,1,1,12,8,"こうげき","こうげき","ひっかく",  "",spawnX,spawnY);
        case 2: return new Enemy("弓スケルトン",26,2,2,2, 9,7,"こうげき","こうげき","どくのや",  "",spawnX,spawnY);
        case 3: return new Enemy("ゾンビ",      21,1,1,1, 9,6,"こうげき","こうげき","こうげき",  "",spawnX,spawnY);
        }
    }
    //万一バグでここまで来た場合、強制的にスライム
    //(NAME,HP,POW,SPD,EXP,ACT1,ACT2,ACT3,)
    return new Enemy("スライム",1,1,1,1,1,1,"こうげき","こうげき","こうげき","",spawnX,spawnY);
}

Enemy* SpawnRushEnemy(int stage)
{
    int spawnX = 270;
    int spawnY = 180;

    switch (stage)
    {
    case 0: return new Enemy("ミノタウロス",  100,10,10,10,10,0,"こうげき","こうげき","自己再生",    "",spawnX,spawnY);
    case 1: return new Enemy("ソードマキナ",  100,10,10,10,10,0,"こうげき","雷連撃",  "電力チャージ","",spawnX,spawnY);
    case 2: return new Enemy("妖狐",          100,10,10,10,10,0,"こうげき","鬼火",    "蒼炎",        "",spawnX,spawnY);
    case 3: return new Enemy("落武者",        100,10,10,10,10,0,"こうげき","切り裂く","受流しの構え","",spawnX,spawnY);
    case 4: return new Enemy("イビルゴルゴン",100,10,10,10,10,0,"こうげき","毒牙",    "石化の魔眼",  "",spawnX,spawnY);
    }
    return nullptr;
}