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
#include "wpn_elite.h"

#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#endif

#ifdef CLIENT_DLL
namespace cl {
#else
namespace sv {
#endif

LINK_ENTITY_TO_CLASS(weapon_elite, CELITE)

void CELITE::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_elite");

	Precache();
	m_iId = WEAPON_ELITE;
	SET_MODEL(ENT(pev), "models/w_elite.mdl");

	m_iDefaultAmmo = ELITE_DEFAULT_GIVE;
	m_flAccuracy = 0.88;

	FallInit();
}

void CELITE::Precache(void)
{
	PRECACHE_MODEL("models/v_elite.mdl");
	PRECACHE_MODEL("models/w_elite.mdl");

	PRECACHE_SOUND("weapons/elite_fire.wav");
	PRECACHE_SOUND("weapons/elite_reloadstart.wav");
	PRECACHE_SOUND("weapons/elite_leftclipin.wav");
	PRECACHE_SOUND("weapons/elite_clipout.wav");
	PRECACHE_SOUND("weapons/elite_sliderelease.wav");
	PRECACHE_SOUND("weapons/elite_rightclipin.wav");
	PRECACHE_SOUND("weapons/elite_deploy.wav");

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usFireELITE_LEFT = PRECACHE_EVENT(1, "events/elite_left.sc");
	m_usFireELITE_RIGHT = PRECACHE_EVENT(1, "events/elite_right.sc");
}

int CELITE::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = MAX_AMMO_9MM;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = ELITE_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 5;
	p->iId = m_iId = WEAPON_ELITE;
	p->iFlags = 0;
	p->iWeight = ELITE_WEIGHT;

	return 1;
}

BOOL CELITE::Deploy(void)
{
	m_flAccuracy = 0.88;
	m_NextInspect = gpGlobals->time + 0.75s;
	if (!(m_iClip & 1))
		m_iWeaponState |= WPNSTATE_ELITE_LEFT;
	return DefaultDeploy("models/v_elite.mdl", "models/p_elite.mdl", ELITE_DRAW, "dualpistols", UseDecrement() != FALSE);
}

void CELITE::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		ELITEFire((1.3) * (1 - m_flAccuracy), 0.2s, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
		ELITEFire((0.175) * (1 - m_flAccuracy), 0.2s, FALSE);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		ELITEFire((0.08) * (1 - m_flAccuracy), 0.2s, FALSE);
	else
		ELITEFire((0.1) * (1 - m_flAccuracy), 0.2s, FALSE);
}

void CELITE::ELITEFire(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim)
{
	flCycleTime -= 0.125s;
	m_iShotsFired++;

	if (m_iShotsFired > 1)
		return;

	if (m_flLastFire != invalid_time_point)
	{
		m_flAccuracy -= (0.325 - ((gpGlobals->time - m_flLastFire) / 1s)) * 0.275;

		if (m_flAccuracy > 0.88)
			m_flAccuracy = 0.88;
		else if (m_flAccuracy < 0.55)
			m_flAccuracy = 0.55;
	}

	m_flLastFire = gpGlobals->time;
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
		vecDir = FireBullets3(m_pPlayer->GetGunPosition() - gpGlobals->v_right * 5, gpGlobals->v_forward, flSpread, 8192, BULLET_PLAYER_9MM, 1, GetDamage(), 0.75, m_pPlayer->pev, TRUE, m_pPlayer->random_seed);
		PLAYBACK_EVENT_FULL(flags, ENT(m_pPlayer->pev), m_usFireELITE_LEFT, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.y * 100), m_iClip, FALSE, FALSE);
	}
	else
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK2);
#endif
		m_iWeaponState |= WPNSTATE_ELITE_LEFT;

		vecDir = FireBullets3(m_pPlayer->GetGunPosition() - gpGlobals->v_right * 5, gpGlobals->v_forward, flSpread, 8192, BULLET_PLAYER_9MM, 1, GetDamage(), 0.75, m_pPlayer->pev, TRUE, m_pPlayer->random_seed);
		PLAYBACK_EVENT_FULL(flags, ENT(m_pPlayer->pev), m_usFireELITE_RIGHT, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.y * 100), m_iClip, FALSE, FALSE);
	}

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2s;
	m_pPlayer->pev->punchangle.x -= 2;
}

void CELITE::Reload(void)
{
	if (m_pPlayer->ammo_9mm <= 0)
		return;
	m_NextInspect = gpGlobals->time + ELITE_RELOAD_TIME;
	if (DefaultReload(ELITE_MAX_CLIP, ELITE_RELOAD, ELITE_RELOAD_TIME))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
		if ((int)CVAR_GET_FLOAT("mp_csgospecialeffect"))
		{
			m_pPlayer->m_flNextAttack = 3.93s;
			m_flTimeWeaponIdle = ELITE_RELOAD_TIME + 0.5s;
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + ELITE_RELOAD_TIME;
		}
#endif
		m_flAccuracy = 0.88;
	}
}

void CELITE::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (m_iClip)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60s;

		if (m_iClip == 1)
			SendWeaponAnim(ELITE_IDLE_LEFTEMPTY, UseDecrement() != FALSE);
		else
			SendWeaponAnim(ELITE_IDLE, UseDecrement() != FALSE);
	}
}

float CELITE::GetDamage() const
{
	float flDamage = 36.0f;
#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flDamage = 41.0f;
	else if (g_pModRunning->DamageTrack() == DT_ZBS)
		flDamage = 41.0f;
#endif
	return flDamage;
}

void CELITE::Inspect()
{

	if (!m_fInReload)
	{
		if (gpGlobals->time > m_NextInspect)
		{
#ifndef CLIENT_DLL
			SendWeaponAnim(16, 0);
#endif
			m_NextInspect = gpGlobals->time + GetInspectTime();
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + GetInspectTime();
		}
	}

}
}
