#include "PlayerStatus.h"
#include <EffekseerForDXLib.h>
#include "../Manager/SceneManager.h"

PlayerStatus::PlayerStatus()
{
}

void PlayerStatus::Draw()
{
	//ステータスの描画処理
	DrawFormatString(STATUS_X, 10, STATUS_COLOR, "Level: %d", level_);
	DrawFormatString(STATUS_X, 30, STATUS_COLOR, "HP: %d/%d", hp_, GetMaxHp());
	DrawFormatString(STATUS_X, 50, STATUS_COLOR, "Physical: %d", Attack());
	DrawFormatString(STATUS_X, 70, STATUS_COLOR, "Magical: %d", MagicAttack());
	DrawFormatString(STATUS_X, 90, STATUS_COLOR, "Speed: %d", speed_);

	DrawFormatString(STATUS_X, 130, STATUS_COLOR, "Job: %s", job.c_str());
	DrawFormatString(STATUS_X, 150, STATUS_COLOR, "薬: %d", pharmacy_);
	DrawFormatString(STATUS_X, 170, STATUS_COLOR, "武: %d", martialArts_);
	DrawFormatString(STATUS_X, 190, STATUS_COLOR, "魔: %d", magicKnowledge_);
	DrawFormatString(STATUS_X, 210, STATUS_COLOR, "信: %d", faith_);
	DrawFormatString(STATUS_X, 230, STATUS_COLOR, "古: %d", archaeology_);
	DrawFormatString(STATUS_X, 250, STATUS_COLOR, "星: %d", astrology_);

	//技能ステータスのボーナス分を描画
	unsigned int GREEN = STATUS_BONUS_COLOR;

	int itemBonus = SkillBonus(BonusType::ItemBonus, 0);
	if (itemBonus > 0) DrawFormatString(STATUS_BONUS_X, 150, GREEN, "(回復+%d)", itemBonus);

	int atkBonus = SkillBonus(BonusType::AttackBonus, 0); //基準値0で呼ぶとボーナス量だけが返る
	if (atkBonus > 0) DrawFormatString(STATUS_BONUS_X, 170, GREEN, "(攻撃+%d)", atkBonus);

	int magBonus = SkillBonus(BonusType::MagicBonus, 0);
	if (magBonus > 0) DrawFormatString(STATUS_BONUS_X, 190, GREEN, "(魔法+%d)", magBonus);

	int defBonus = (faith_ / 5); //軽減するダメージ量
	if (defBonus > 0) DrawFormatString(STATUS_BONUS_X, 210, GREEN, "(防御+%d)", defBonus);

	int expBonus = SkillBonus(BonusType::ExpBonus, 0);
	if (expBonus > 0) DrawFormatString(STATUS_BONUS_X, 230, GREEN, "(経験+%d)", expBonus);

	int luckBonus = SkillBonus(BonusType::LuckBonus, 0);
	if (luckBonus > 0) DrawFormatString(STATUS_BONUS_X, 250, GREEN, "(幸運+%d)", luckBonus);

	//職業ボーナスの描画
	int jobHpBonus = GetJobBonus().hp; //職業ボーナスも表示
	if (jobHpBonus > 0) DrawFormatString(STATUS_BONUS_X + 80, 30, GREEN, "(職業ボーナス+%d)", jobHpBonus);

	int jobAtkBonus = GetJobBonus().power; //職業ボーナスも表示
	if (jobAtkBonus > 0) DrawFormatString(STATUS_BONUS_X + 80, 50, GREEN, "(職業ボーナス+%d)", jobAtkBonus);

	int jpbSpeedBonus = GetJobBonus().speed;
	if (jpbSpeedBonus > 0) DrawFormatString(STATUS_BONUS_X + 80, 90, GREEN, "(職業ボーナス+%d)", jpbSpeedBonus);
}

void PlayerStatus::InitJob()
{
	//すでに職業が初期化されている場合は何もしない
	if (!jobList.empty()) return;

	//職業の初期化
	//名前, LV, POW, MAG, 薬学, 武術, 魔法知, 信仰, 考古, 占星
	jobList.push_back(JobData("一般魔法使い",2,0,0, 0,0,10,0,0,0));

	jobList.push_back(JobData("付加術師",3,0,0,100,  0,  0,  0,  0,  0));
	jobList.push_back(JobData("魔剣士"	,3,0,0,  0,100,  0,  0,  0,  0));
	jobList.push_back(JobData("魔導師"	,3,0,0,  0,  0,130,  0,  0,  0));
	jobList.push_back(JobData("聖職者"	,3,0,0,  0,  0,  0,100,  0,  0));
	jobList.push_back(JobData("呪術師"	,3,0,0,  0,  0,  0,  0,100,  0));
	jobList.push_back(JobData("占い師"	,3,0,0,  0,  0,  0,  0,  0,100));
}

int PlayerStatus::Attack()
{
	//基礎攻撃力
	int base = this->power_;

	//職業ごとのプラス値を加える
	int jobBonus = GetJobBonus().power;
	int powerWithJob = base + jobBonus;

	//技能ステータスのボーナスを加える
	return SkillBonus(BonusType::AttackBonus, powerWithJob);
}

int PlayerStatus::MagicAttack()
{
	int base = this->magic_;

	int jobBonus = GetJobBonus().magic;
	int magicWithJob = base + jobBonus;

	return SkillBonus(BonusType::MagicBonus, magicWithJob);
}

void PlayerStatus::Heal()
{
	hp_ += SkillBonus(BonusType::ItemBonus, heal_);
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
		hp_ = 0;
		Death();
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

	//60%の確率でアップ
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
		//武術10につき、物理攻撃のダメージを+1する
		return baseValue + (martialArts_ / 10);

	case BonusType::MagicBonus:
		//魔法知10につき、魔法威力を+1する
		return baseValue + (magicKnowledge_ / 10);

	case BonusType::DefenseBonus:
	{
		//信仰10につき、受けるダメージを-1する
		int finalDamage = baseValue - (faith_ / 10);
		//ダメージがマイナス（回復）になってしまうのを防ぐため、最低でも1ダメージは受ける
		if (finalDamage < 1) finalDamage = 1;
		return finalDamage;
	}

	case BonusType::ExpBonus:
		//考古学10につき、獲得経験値を+2する（固定値追加）
		//例：基本経験値10、考古学10 → 10 + (10 / 10 * 2) = 12
		return baseValue + (archaeology_ / 10 * 2);

	case BonusType::LuckBonus:
		//占星術10につき、回避率のステータスを+1する
		return baseValue + (astrology_ / 5);
	}
	return baseValue;
}

PlayerStatus::JobBonus PlayerStatus::GetJobBonus()
{
	JobBonus bonus;

	//職業ごとのステータスボーナスを設定
	if (this->job == "一般魔法使い") 
	{
		bonus.hp = 10;
		bonus.power = 10;
		bonus.speed = 10;
	}
	if (this->job == "付加術師") 
	{
		bonus.power = 10;
	}
	if (this->job == "魔剣士") 
	{
		bonus.power = 10;
	}
	if (this->job == "魔導師") 
	{
		bonus.power = 10;
	}
	if (this->job == "聖職者") 
	{
		bonus.power = 10;
	}
	if (this->job == "呪術師") 
	{
		bonus.power = 10;
	}
	if (this->job == "占い師") 
	{
		bonus.power = 10;
	}

	return bonus;
}
