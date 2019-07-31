/*
wpn_infinityex1.cpp
Copyright (C) 2019 Moemod Hymei

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_infinityex1.h"

#ifdef CLIENT_DLL
namespace cl {
#else
namespace sv {
#endif

LINK_ENTITY_TO_CLASS(weapon_infinityex1, CInfinityEX1)

void CInfinityEX1::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_infinityex1");

	Precache();
	m_iId = WEAPON_ELITE;
	SET_MODEL(ENT(pev), GetCSModelName());

	m_iMaxClip = 40;
	m_iDefaultAmmo = m_iMaxClip;
	m_flAccuracy = 0.2;

	FallInit();
}

void CInfinityEX1::Precache(void)
{
	PRECACHE_MODEL("models/v_infinityex1.mdl");
	PRECACHE_MODEL("models/p_infinityex1.mdl");
	PRECACHE_MODEL("models/w_infinityex1.mdl");

	PRECACHE_SOUND("weapons/infi-1.wav");

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usFireELITE_LEFT = PRECACHE_EVENT(1, "events/infinity_left.sc");
	m_usFireELITE_RIGHT = PRECACHE_EVENT(1, "events/infinity_right.sc");
}

int CInfinityEX1::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "45acp";
	p->iMaxAmmo1 = MAX_AMMO_45ACP;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = m_iMaxClip;
	p->iSlot = 1;
	p->iPosition = 5;
	p->iId = m_iId = WEAPON_ELITE;
	p->iFlags = 0;
	p->iWeight = ELITE_WEIGHT;

	return 1;
}

BOOL CInfinityEX1::Deploy(void)
{
	m_flAccuracy = 0.2;
	m_iSpecialAttack = 0;

	if (!(m_iClip & 1))
		m_iWeaponState |= WPNSTATE_ELITE_LEFT;

	return DefaultDeploy("models/v_infinityex1.mdl", "models/p_infinityex1.mdl", ANIM_DRAW, "dualpistols", UseDecrement() != FALSE);
}

void CInfinityEX1::PrimaryAttack(void)
{
	if (m_iSpecialAttack)
	{
		m_flAccuracy = 0.2;
		m_iSpecialAttack = 0;
	}

	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		InfinityFire((0.12) * m_flAccuracy, 0.2s, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
		InfinityFire((0.055) * m_flAccuracy, 0.2s, FALSE);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		InfinityFire((0.02) * m_flAccuracy, 0.2s, FALSE);
	else
		InfinityFire((0.03) * m_flAccuracy, 0.2s, FALSE);
}

void CInfinityEX1::SecondaryAttack(void)
{
	if (!m_iSpecialAttack)
	{
		m_flAccuracy = 1.5;
		m_iSpecialAttack = 1 + !(m_iShotsFired & 1);
	}

	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		InfinityFire2((0.08) * m_flAccuracy, 0.1s, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
		InfinityFire2((0.04) * m_flAccuracy, 0.1s, FALSE);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		InfinityFire2((0.02) * m_flAccuracy, 0.1s, FALSE);
	else
		InfinityFire2((0.03) * m_flAccuracy, 0.1s, FALSE);
}

void CInfinityEX1::InfinityFire(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim)
{
	m_iShotsFired++;

	if (m_iShotsFired > 1)
		return;

	if (m_flLastFire != invalid_time_point)
	{
		m_flAccuracy = (m_iShotsFired * m_iShotsFired) / 240.0f + 0.4f;

		if (m_flAccuracy > 2)
			m_flAccuracy = 2;
		else if (m_flAccuracy < 0)
			m_flAccuracy = 0;
	}

	m_flLastFire = gpGlobals->time;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2s;
		}

		return;
	}

	m_iClip--;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_pPlayer->m_iWeaponVolume = BIG_EXPLOSION_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;

	Vector vecDir;
	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	if (m_iWeaponState & WPNSTATE_ELITE_LEFT)
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		m_iWeaponState &= ~WPNSTATE_ELITE_LEFT;

		vecDir = FireBullets3(m_pPlayer->GetGunPosition() - gpGlobals->v_right * 5, gpGlobals->v_forward, flSpread, 8192, 1, BULLET_PLAYER_45ACP, GetDamage(), 0.8, m_pPlayer->pev, TRUE, m_pPlayer->random_seed);
		PLAYBACK_EVENT_FULL(flags, ENT(m_pPlayer->pev), m_usFireELITE_LEFT, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.y * 100), m_iClip, FALSE, FALSE);
	}
	else
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK2);
#endif
		m_iWeaponState |= WPNSTATE_ELITE_LEFT;

		vecDir = FireBullets3(m_pPlayer->GetGunPosition() - gpGlobals->v_right * 5, gpGlobals->v_forward, flSpread, 8192, 1, BULLET_PLAYER_45ACP, GetDamage(), 0.8, m_pPlayer->pev, TRUE, m_pPlayer->random_seed);
		PLAYBACK_EVENT_FULL(flags, ENT(m_pPlayer->pev), m_usFireELITE_RIGHT, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.y * 100), m_iClip, FALSE, FALSE);
	}

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2s;
	m_pPlayer->pev->punchangle.x -= 2;
}

void CInfinityEX1::InfinityFire2(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim)
{
	m_iShotsFired++;

	if (m_flLastFire != invalid_time_point)
	{
		m_flAccuracy = (m_iShotsFired * m_iShotsFired) / 210.0f + 0.6f;

		if (m_flAccuracy > 2)
			m_flAccuracy = 2;
		else if (m_flAccuracy < 0)
			m_flAccuracy = 0;
	}

	m_flLastFire = gpGlobals->time;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2s;
		}

		return;
	}

	m_iClip--;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_pPlayer->m_iWeaponVolume = BIG_EXPLOSION_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;

	Vector vecDir;
	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	if (m_iWeaponState & WPNSTATE_ELITE_LEFT)
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		m_iWeaponState &= ~WPNSTATE_ELITE_LEFT;

		vecDir = FireBullets3(m_pPlayer->GetGunPosition() - gpGlobals->v_right * 5, gpGlobals->v_forward, flSpread, 8192, 1, BULLET_PLAYER_45ACP, GetDamage(), 0.8, m_pPlayer->pev, TRUE, m_pPlayer->random_seed);
		PLAYBACK_EVENT_FULL(flags, ENT(m_pPlayer->pev), m_usFireELITE_LEFT, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.y * 100), m_iClip, TRUE, m_iSpecialAttack == 2);
	}
	else
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK2);
#endif
		m_iWeaponState |= WPNSTATE_ELITE_LEFT;

		vecDir = FireBullets3(m_pPlayer->GetGunPosition() - gpGlobals->v_right * 5, gpGlobals->v_forward, flSpread, 8192, 1, BULLET_PLAYER_45ACP, GetDamage(), 0.8, m_pPlayer->pev, TRUE, m_pPlayer->random_seed);
		PLAYBACK_EVENT_FULL(flags, ENT(m_pPlayer->pev), m_usFireELITE_RIGHT, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.y * 100), m_iClip, TRUE, m_iSpecialAttack == 2);
	}

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2s;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(0.35, 0.4, 0.1, 0.15, 2.3, 3.3, 2);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(1.0, 1.0, 0.8, 0.8, 5.0, 5.0, 5);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.2, 0.35, 0.07, 0.1, 2.0, 3.0, 1);
	else
		KickBack(0.3, 0.45, 0.1, 0.2, 1.3, 1.8, 2);
}

float CInfinityEX1::GetDamage() const
{
	return m_iSpecialAttack ? 28.0f : 30.0f;
}

void CInfinityEX1::Reload(void)
{
	if (m_pPlayer->ammo_45acp <= 0)
		return;

	if (DefaultReload(m_iMaxClip, ANIM_RELOAD, 4.0s))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0.2;
		m_iSpecialAttack = 0;
	}
}

void CInfinityEX1::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (m_iClip)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60s;

		if (m_iClip == 1)
			SendWeaponAnim(ANIM_IDLE_LEFTEMPTY, UseDecrement() != FALSE);
		else
			SendWeaponAnim(ANIM_IDLE, UseDecrement() != FALSE);
	}
}

}
