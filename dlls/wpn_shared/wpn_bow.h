#pragma once

#ifdef CLIENT_DLL
namespace cl {
#else
namespace sv {
#endif

#define BOW_MAX_SPEED            240

class CBow : public CBasePlayerWeapon
{
	enum bow_e
    {
        ANIM_IDLE,
        ANIM_IDLE_EMPTY,
        ANIM_SHOOT1,
        ANIM_SHOOT1_EMPTY,
        ANIM_DRAW,
        ANIM_DRAW_EMPTY,
        ANIM_CHARGE_START1,
        ANIM_CHARGE_FINISH1,
        ANIM_CHARGE_IDLE1,
        ANIM_CHARGE_IDLE2,
        ANIM_CHARGE_SHOOT1,
        ANIM_CHARGE_SHOOT1_EMPTY,
        ANIM_CHARGE_SHOOT2,
        ANIM_CHARGE_SHOOT2_EMPTY
    };
public:
	void Spawn() override;
	void Precache() override;
	int GetItemInfo(ItemInfo *p) override;
	BOOL Deploy() override;
	bool HasSecondaryAttack() override { return true; }
	float GetMaxSpeed() override { return BOW_MAX_SPEED; }
	int iItemSlot() override { return PRIMARY_WEAPON_SLOT; }
	void PrimaryAttack() override;
	void SecondaryAttack() override;
	void ItemPostFrame() override;
	float GetPrimaryAttackDamage() const;
	float GetSecondaryAttackDamage() const;
	void Reload() override;
	void WeaponIdle() override;
	BOOL UseDecrement() override
	{
#ifdef CLIENT_WEAPONS
		return TRUE;
#else
		return FALSE;
#endif
	}
	int m_iMode;
	KnockbackData GetKnockBackData() override 
	{ 
		if (m_iMode)
			return { 2500, 500, 700, 600, 0.4f };
		else
			return { 1500, 900, 1000, 900, 0.25f };
	}
	const char *GetCSModelName() override { return "models/w_bow.mdl"; }
#ifndef CLIENT_DLL
	WeaponBuyAmmoConfig GetBuyAmmoConfig() override { return { "ammo_bow" , 200 }; }
#endif
	int ExtractAmmo(CBasePlayerWeapon *pWeapon) override;
	int m_iShell;
	time_point_t fFinishTime;
	int iShellOn;

private:
	unsigned short m_usFireBow;
};

}
