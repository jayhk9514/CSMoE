/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_g3sg1.h"

#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#endif

#ifdef CLIENT_DLL
namespace cl {
#else
namespace sv {
#endif

LINK_ENTITY_TO_CLASS(weapon_g3sg1, CG3SG1)

void CG3SG1::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_g3sg1");

	Precache();
	m_iId = WEAPON_G3SG1;
	SET_MODEL(ENT(pev), "models/w_g3sg1.mdl");
	m_iDefaultAmmo = G3SG1_DEFAULT_GIVE;

	FallInit();
}

void CG3SG1::Precache(void)
{
	PRECACHE_MODEL("models/v_g3sg1.mdl");
	PRECACHE_MODEL("models/w_g3sg1.mdl");

	PRECACHE_SOUND("weapons/g3sg1-1.wav");
	PRECACHE_SOUND("weapons/g3sg1_slide.wav");
	PRECACHE_SOUND("weapons/g3sg1_clipin.wav");
	PRECACHE_SOUND("weapons/g3sg1_clipout.wav");
	PRECACHE_SOUND("weapons/zoom.wav");

	m_iShellId = m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireG3SG1 = PRECACHE_EVENT(1, "events/g3sg1.sc");
}

int CG3SG1::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762Nato";
	p->iMaxAmmo1 = MAX_AMMO_762NATO;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = G3SG1_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_G3SG1;
	p->iFlags = 0;
	p->iWeight = G3SG1_WEIGHT;

	return 1;
}

BOOL CG3SG1::Deploy(void)
{
#ifndef CLIENT_DLL
	if (m_pPlayer->IsAlive())
		CheckWeapon(m_pPlayer, this);
#endif
	m_flAccuracy = 0.2;
	m_NextInspect = gpGlobals->time + 0.75s;
	return DefaultDeploy("models/v_g3sg1.mdl", "models/p_g3sg1.mdl", G3SG1_DRAW, "mp5", UseDecrement() != FALSE);
}

void CG3SG1::SecondaryAttack(void)
{
	switch (m_pPlayer->m_iFOV)
	{
		case 90: m_pPlayer->m_iFOV = m_pPlayer->pev->fov = 40; break;
		case 40: m_pPlayer->m_iFOV = m_pPlayer->pev->fov = 15; break;
		default: m_pPlayer->m_iFOV = m_pPlayer->pev->fov = 90; break;
	}

	m_pPlayer->ResetMaxSpeed();
	EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/zoom.wav", 0.2, 2.4);
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3s;
}

void CG3SG1::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		G3SG1Fire(0.45, 0.25s, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
		G3SG1Fire(0.15, 0.25s, FALSE);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		G3SG1Fire(0.035, 0.25s, FALSE);
	else
		G3SG1Fire(0.055, 0.25s, FALSE);
}

void CG3SG1::G3SG1Fire(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim)
{
	if (m_pPlayer->pev->fov == 90)
		flCycleTime += 0.025s;

	if (m_flLastFire != invalid_time_point)
	{
		m_flAccuracy = ((gpGlobals->time - m_flLastFire) / 1s) * 0.3 + 0.55;

		if (m_flAccuracy > 0.98)
			m_flAccuracy = 0.98;
	}
	else
		m_flAccuracy = 0.98;

	m_NextInspect = gpGlobals->time;

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
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = BIG_EXPLOSION_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	Vector vecDir = FireBullets3(m_pPlayer->GetGunPosition(), gpGlobals->v_forward, flSpread, 8192, 3, BULLET_PLAYER_762MM, GetDamage(), 0.98, m_pPlayer->pev, TRUE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, ENT(m_pPlayer->pev), m_usFireG3SG1, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), TRUE, FALSE);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flLastFire = gpGlobals->time;
#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8s;

	m_pPlayer->pev->punchangle.x -= UTIL_SharedRandomFloat(m_pPlayer->random_seed + 4, 2.75, 3.25) + m_pPlayer->pev->punchangle.x * 0.25;
	m_pPlayer->pev->punchangle.y += UTIL_SharedRandomFloat(m_pPlayer->random_seed + 5, -1.25, 1.5);
}

void CG3SG1::Reload(void)
{
	if (m_pPlayer->ammo_762nato <= 0)
		return;
	m_NextInspect = gpGlobals->time + G3SG1_RELOAD_TIME;
	if (DefaultReload(G3SG1_MAX_CLIP, G3SG1_RELOAD, G3SG1_RELOAD_TIME))
	{
		m_flAccuracy = 0.2;
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
		if ((int)CVAR_GET_FLOAT("mp_csgospecialeffect"))
		{
			m_pPlayer->m_flNextAttack = 2.53s;
			m_flTimeWeaponIdle = G3SG1_RELOAD_TIME + 0.5s;
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + G3SG1_RELOAD_TIME;
		}
#endif

		if (m_pPlayer->pev->fov != 90)
		{
			m_pPlayer->m_iFOV = m_pPlayer->pev->fov = 15;
			SecondaryAttack();
		}
	}
}

void CG3SG1::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (m_iClip)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60s;
		SendWeaponAnim(G3SG1_IDLE, UseDecrement() != FALSE);
	}
}

float CG3SG1::GetMaxSpeed(void)
{
	if (m_pPlayer->m_iFOV == 90)
		return 210;

	return 150;
}

float CG3SG1::GetDamage() const
{
	float flDamage = 80.0f;
#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flDamage = 80.0f;
	else if (g_pModRunning->DamageTrack() == DT_ZBS)
		flDamage = 80.0f;
#endif
	return flDamage;
}

void CG3SG1::Inspect()
{

	if (!m_fInReload)
	{
		if (gpGlobals->time > m_NextInspect)
		{
#ifndef CLIENT_DLL
			SendWeaponAnim(5, 0);
#endif
			m_NextInspect = gpGlobals->time + GetInspectTime();
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + GetInspectTime();
		}
	}

}
}
