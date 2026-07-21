#include "PlayerStatus.h"
#include <EffekseerForDXLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"

PlayerStatus::PlayerStatus()
{
	battlePlayer_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::BATTLE_PLAYER).handleId_;
}

void PlayerStatus::Update()
{
	if (attackMotionTimer_ > 0)
	{
		attackMotionTimer_--;
		playerPosX_ -= ANIM_MOVE_PIXELS; // 演出中は左にずらす
	}
	else
	{
		playerPosX_ = PLAYER_POS_X; // 演出が終わったら完全に元の位置に戻る
	}
	if (damageMotionTimer_ > 0)
	{
		damageMotionTimer_--;
	}
}

void PlayerStatus::Draw()
{
	SetFontSize(20);
	
	//ステータスの描画処理
	DrawFormatString(STATUS_X, 120, STATUS_COLOR, "レベル: %d", level_);
	DrawFormatString(STATUS_X, 150, STATUS_COLOR, "体力: %d", GetMaxHp());
	DrawFormatString(STATUS_X, 180, STATUS_COLOR, "筋力: %d", Attack());
	DrawFormatString(STATUS_X, 210, STATUS_COLOR, "魔力: %d", MagicAttack());
	DrawFormatString(STATUS_X, 240, STATUS_COLOR, "速力: %d", speed_);
	DrawFormatString(STATUS_X, 270, STATUS_COLOR, "職業: %s", job.c_str());

	DrawFormatString(STATUS_X, 310, STATUS_COLOR, "薬学: %d", pharmacy_);
	DrawFormatString(STATUS_X, 340, STATUS_COLOR, "武術: %d", martialArts_);
	DrawFormatString(STATUS_X, 370, STATUS_COLOR, "魔法: %d", magicKnowledge_);
	DrawFormatString(STATUS_X, 400, STATUS_COLOR, "信仰: %d", faith_);
	DrawFormatString(STATUS_X, 430, STATUS_COLOR, "考古: %d", archaeology_);
	DrawFormatString(STATUS_X, 460, STATUS_COLOR, "占星: %d", astrology_);

	//技能ステータスのボーナス分を描画
	unsigned int GREEN = STATUS_BONUS_COLOR;

	int itemBonus = SkillBonus(BonusType::ItemBonus, 0);
	if (itemBonus > 0) DrawFormatString(STATUS_BONUS_X, 310, GREEN, "(治癒力+%d)", itemBonus);

	int atkBonus = SkillBonus(BonusType::AttackBonus, 0); //基準値0で呼ぶとボーナス量だけが返る
	if (atkBonus > 0) DrawFormatString(STATUS_BONUS_X, 340, GREEN, "(会心率+%d)", atkBonus);

	int magBonus = SkillBonus(BonusType::MagicBonus, 0);
	if (magBonus > 0) DrawFormatString(STATUS_BONUS_X, 370, GREEN, "(魔法+%d)", magBonus);

	int defBonus = (faith_ / 15); //軽減するダメージ量
	if (defBonus > 0) DrawFormatString(STATUS_BONUS_X, 400, GREEN, "(守備力+%d)", defBonus);

	int expBonus = SkillBonus(BonusType::ExpBonus, 0);
	if (expBonus > 0) DrawFormatString(STATUS_BONUS_X, 430, GREEN, "(経験値+%d)", expBonus);

	int luckBonus = SkillBonus(BonusType::LuckBonus, 0);
	if (luckBonus > 0) DrawFormatString(STATUS_BONUS_X, 460, GREEN, "(回避率+%d)", luckBonus);

	//職業ボーナスの描画
	int jobHpBonus = GetJobBonus().hp; //職業ボーナスも表示
	if (jobHpBonus > 0) DrawFormatString(STATUS_X + 100, 150, GREEN, "(+%d)", jobHpBonus);

	int jobAtkBonus = GetJobBonus().power; //職業ボーナスも表示
	if (jobAtkBonus > 0) DrawFormatString(STATUS_X + 100, 180, GREEN, "(+%d)", jobAtkBonus);

	int jobMagBonus = GetJobBonus().magic;
	if (jobMagBonus > 0) DrawFormatString(STATUS_X + 100, 210, GREEN, "(+%d)", jobMagBonus);

	int jobSpeedBonus = GetJobBonus().speed;
	if (jobSpeedBonus > 0) DrawFormatString(STATUS_X + 100, 240, GREEN, "(+%d)", jobSpeedBonus);
	SetFontSize(DEFAULT_FONT_SIZE);
}

void PlayerStatus::DrawQuestImages()
{
	//敵の名前とHPを文字で表示（色の指定は白: GetColor(255,255,255)）
	unsigned int white = GetColor(255, 255, 255);
	DrawString(PLAYER_POS_X, PLAYER_POS_Y - 40, name.c_str(), white);
	DrawFormatString(PLAYER_POS_X, PLAYER_POS_Y - 20, white, "HP: %d / %d", hp_, GetMaxHp());

	int barWidth = 100; //バーの最大幅
	int currentBarWidth = barWidth * hp_ / GetMaxHp();
	//赤い背景
	DrawBox(PLAYER_POS_X, PLAYER_POS_Y - 5, PLAYER_POS_X + barWidth, PLAYER_POS_Y, GetColor(255, 0, 0), TRUE);
	//緑の現在値
	DrawBox(PLAYER_POS_X, PLAYER_POS_Y - 5, PLAYER_POS_X + currentBarWidth, PLAYER_POS_Y, GetColor(0, 255, 0), TRUE);

	bool isVisible = true; // 描画するかどうかのフラグ

	if (damageMotionTimer_ > 0)
	{
		//2フレームごとにパタパタ点滅
		if ((damageMotionTimer_ / 2) % 3 == 0)
		{
			isVisible = false;
		}

		//赤く染める
		SetDrawBright(255, 0, 0);
	}

	if (isVisible)
	{
		DrawGraph(playerPosX_, PLAYER_POS_Y, battlePlayer_, true);
	}

	if (damageMotionTimer_ > 0)
	{
		SetDrawBright(255, 255, 255);
	}
}

void PlayerStatus::InitJob()
{
	//すでに職業が初期化されている場合は何もしない
	if (!jobList.empty()) return;

	//職業の初期化
	//名前, LV, POW, MAG, 薬学, 武術, 魔法知, 信仰, 考古, 占星
	jobList.push_back(JobData("一般魔法使い",2,0,0, 0,0,10,0,0,0));

	jobList.push_back(JobData("付加術師",5,0,0,100,  0,  0,  0,  0,  0));
	jobList.push_back(JobData("魔剣士"	,5,0,0,  0,100,  0,  0,  0,  0));
	jobList.push_back(JobData("魔導師"	,5,0,0,  0,  0,130,  0,  0,  0));
	jobList.push_back(JobData("聖職者"	,5,0,0,  0,  0,  0,100,  0,  0));
	jobList.push_back(JobData("呪術師"	,5,0,0,  0,  0,  0,  0,100,  0));
	jobList.push_back(JobData("占い師"	,5,0,0,  0,  0,  0,  0,  0,100));

	jobList.push_back(JobData("錬金術師",20,0,0,170,  0, 30,  0,100,  0));
	jobList.push_back(JobData("聖騎士"	,20,0,0,  0,230,  0, 70,  0,  0));
	jobList.push_back(JobData("賢者"	,20,0,0,  0,  0,200,  0, 50, 50));
	jobList.push_back(JobData("悪魔祓い",20,0,0, 70, 30,  0,200,  0,  0));
	jobList.push_back(JobData("死霊術師",20,0,0,  0,  0,100,  0,200,  0));
	jobList.push_back(JobData("予言者"	,20,0,0, 30,  0,  0, 40,  0,230));

	jobList.push_back(JobData("大魔法使い",40,0,0,200,200,200,200,200,200));

	//ジョブチェンジした時用
	hasStartDamage = false;
	hasFirstHitNull = false; 
	hasMagicUnlock = false;
	hasCritBoost = false; 
	hasMagicToAttack = false; 
	hasAutoRegen = false; 
	hasGuts = false;
	isFirstHitUsed = false; 
	isGutsUsed = false;
}

int PlayerStatus::Attack()
{
	//基礎攻撃力
	int base = this->power_;

	//職業ごとのプラス値を加える
	int jobBonus = GetJobBonus().power;
	int powerWithJob = base + jobBonus;

	AttackAnimation();

	//技能ステータスのボーナスを加える
	return powerWithJob;
}

int PlayerStatus::MagicAttack()
{
	int base = this->magic_;

	int jobBonus = GetJobBonus().magic;
	int magicWithJob = base + jobBonus;

	return SkillBonus(BonusType::MagicBonus, magicWithJob);
}

void PlayerStatus::Heal(int amount)
{
	hp_ += SkillBonus(BonusType::ItemBonus, amount);
	if (hp_ > GetMaxHp()) 
	{
		hp_ = GetMaxHp();
	}
}

void PlayerStatus::FullHeal()
{
	hp_ = GetMaxHp();
}

void PlayerStatus::Damage(int damage)
{
	int finalDamage = SkillBonus(BonusType::DefenseBonus, damage);

	hp_ -= finalDamage;
	if (hp_ <= 0)
	{
		//食いしばり発動チェック
		if (hasGuts && !isGutsUsed)
		{
			hp_ = 1; //1で耐える
			isGutsUsed = true; //消費する
			//※画面に「致命傷を耐えた」と出したい場合は、フラグを返り値にするかQuestPhaseで判定します
		}
		else
		{
			hp_ = 0;
			Death();
		}
	}
}

void PlayerStatus::Death()
{
	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
}

int PlayerStatus::GetMaxHp()
{
	//初期HP
	int base = this->maxHp_;

	//職業ごとのプラス値を加える
	int jobBonus = GetJobBonus().hp;
	int hpWithJob = base + jobBonus;

	return hpWithJob;
}

int PlayerStatus::GetSpeed()
{
	//初期速度
	int base = this->speed_;

	//職業ごとのプラス値を加える
	int jobBonus = GetJobBonus().speed;
	int speedWithJob = base + jobBonus;

	return speedWithJob;
}

void PlayerStatus::GetExp(int exp)
{
	//経験値処理
	int finalExp = SkillBonus(BonusType::ExpBonus, exp);

	exp_ += finalExp;
	while (exp_ >= NEED_EXP) 
	{
		exp_ -= NEED_EXP;
		LevelUp();
	}
}

void PlayerStatus::LevelUp()
{
	//レベルアップした時の処理
	level_++;
	maxHp_++;
	hp_ = GetMaxHp();

	//各ステータスの抽選処理

	//70%の確率でアップ
	if ((rand() % RATE_BASE) < SKILL_UP_RATE) power_ += 1;
	if ((rand() % RATE_BASE) < SKILL_UP_RATE) magic_ += 1;
	if ((rand() % RATE_BASE) < SKILL_UP_RATE) speed_ += 1;
}

bool PlayerStatus::JobCheck(const JobData& job)
{
	//基礎ステータスのチェック
	if (this->level_ < job.status.reqLevel_) return false;
	if (this->power_ < job.status.reqPower_) return false;
	if (this->magic_ < job.status.reqMagic_) return false;

	//技術ステータスのチェック
	if (this->pharmacy_ < job.status.reqPharmacy_) return false;
	if (this->martialArts_ < job.status.reqMartialArts_) return false;
	if (this->magicKnowledge_ < job.status.reqMagicKnowledge_) return false;
	if (this->faith_ < job.status.reqFaith_) return false;
	if (this->archaeology_ < job.status.reqArchaeology_) return false;
	if (this->astrology_ < job.status.reqAstrology_) return false;

	return true;
}

int PlayerStatus::SkillBonus(BonusType type, int baseValue)
{
	switch (type) {
	case BonusType::ItemBonus:
		//薬学10につき、アイテム回復量を+1する
		//基本回復5、薬学30 → 5 + (30 / 10) = 8
		return baseValue + (pharmacy_ / 10);

	case BonusType::AttackBonus:
		//武術5につき、会心率を+1する
		return baseValue + (martialArts_ / 5);

	case BonusType::MagicBonus:
		//魔法知につき、魔法を増やす
		return baseValue;

	case BonusType::DefenseBonus:
	{
		//信仰15につき、受けるダメージを-1する
		int finalDamage = baseValue - (faith_ / 15);
		//ダメージがマイナス（回復）になってしまうのを防ぐため、最低でも1ダメージは受ける
		if (finalDamage < 1) finalDamage = 1;
		return finalDamage;
	}

	case BonusType::ExpBonus:
		//考古学10につき、獲得経験値を+2する（固定値追加）
		//例：基本経験値10、考古学10 → 10 + (10 / 5) = 12
		return baseValue + (archaeology_ / 5);

	case BonusType::LuckBonus:
		//占星術5につき、回避率のステータスを+1する
		return baseValue + (astrology_ / 5);
	}
	return baseValue;
}

PlayerStatus::JobBonus PlayerStatus::GetJobBonus()
{
	JobBonus bonus;

	//ジョブチェンジした時用
	hasStartDamage = false;
	hasFirstHitNull = false;
	hasMagicUnlock = false;
	hasCritBoost = false;
	hasMagicToAttack = false;
	hasAutoRegen = false;
	hasGuts = false;

	//職業ごとのステータスボーナスを設定
	if (this->job == "一般魔法使い") 
	{
		bonus.hp = 7;
		bonus.magic = 5;
	}

	if (this->job == "付加術師")
	{
		bonus.hp = 14;
		bonus.speed = 11;
	}
	if (this->job == "魔剣士")
	{
		bonus.hp = 6;
		bonus.power = 12;
		bonus.speed = 7;
	}
	if (this->job == "魔導師")
	{
		bonus.magic = 25;
	}
	if (this->job == "聖職者")
	{
		bonus.hp = 16;
		bonus.magic = 9;
	}
	if (this->job == "呪術師")
	{
		bonus.hp = 2;
		bonus.power = 8;
		bonus.magic = 15;
	}
	if (this->job == "占い師")
	{
		bonus.magic = 10;
		bonus.speed = 15;
	}

	if (this->job == "錬金術師")
	{
		bonus.hp = 24;
		bonus.speed = 21;
		hasAutoRegen = true; //毎ターンHP回復
	}
	if (this->job == "聖騎士")
	{
		bonus.hp = 12;
		bonus.power = 20;
		bonus.speed = 13;
		hasCritBoost = true; //クリティカル倍率上昇
	}
	if (this->job == "賢者")
	{
		bonus.magic = 45;
		hasMagicToAttack = true; //攻撃時に魔力加算
	}
	if (this->job == "悪魔祓い")
	{
		bonus.hp = 27;
		bonus.magic = 18;
		hasGuts = true; //食いしばり
	}
	if (this->job == "死霊術師")
	{
		bonus.hp = 7;
		bonus.power = 13;
		bonus.magic = 25;
		hasStartDamage = true; //開幕呪い
	}
	if (this->job == "予言者")
	{
		bonus.magic = 20;
		bonus.speed = 25;
		hasFirstHitNull = true; //初撃無効
	}

	if (this->job == "大魔法使い")
	{
		bonus.hp = 20;
		bonus.power = 10;
		bonus.magic = 25;
		bonus.speed = 15;
		hasMagicUnlock = true; //魔法制限なし
	}

	return bonus;
}

void PlayerStatus::AttackAnimation()
{
	attackMotionTimer_ = ANIM_COUNT_ATTACK;
}

void PlayerStatus::DamageAnimation()
{
	damageMotionTimer_ = ANIM_COUNT_DAMAGE;
}

int PlayerStatus::AddSkillPoint(SkillType type, int baseAmount)
{
	float multiplier = 1.0f; //基本は等倍（1.0倍）

	// 選んだルートと、上がる技能の組み合わせでボーナス（1.5倍など）をかける
	switch (currentRoute_)
	{
	case PLAYER_ROUTE::BREAKTHROUGH: //打破：武術と考古学
		if (type == SkillType::MartialArts || type == SkillType::Archaeology) multiplier = 1.5f;
		break;
	case PLAYER_ROUTE::SALVATION:    //救世：信仰と薬学
		if (type == SkillType::Faith || type == SkillType::Pharmacy) multiplier = 1.5f;
		break;
	case PLAYER_ROUTE::TRUTH:        //真理：魔法知識と占星術
		if (type == SkillType::MagicKnowledge || type == SkillType::Astrology) multiplier = 1.5f;
		break;
	case PLAYER_ROUTE::SELFLESS:     //無欲：すべてに少しボーナス
		multiplier = 1.3f;
		break;
	}

	// 実際の獲得量を計算（端数切り捨て）
	int finalAmount = static_cast<int>(baseAmount * multiplier);

	// 該当する技能に加算
	switch (type)
	{
	case SkillType::MartialArts   : martialArts_ += finalAmount; break;
	case SkillType::Archaeology	  : archaeology_ += finalAmount; break;
	case SkillType::Faith		  : faith_ += finalAmount; break;
	case SkillType::Pharmacy	  : pharmacy_ += finalAmount; break;
	case SkillType::MagicKnowledge: magicKnowledge_ += finalAmount; break;
	case SkillType::Astrology	  : astrology_ += finalAmount; break;
	}

	return finalAmount;
}