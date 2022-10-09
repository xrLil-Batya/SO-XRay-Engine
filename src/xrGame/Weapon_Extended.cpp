#include "StdAfx.h"

#include "WeaponMagazined.h"
#include "WeaponMagazinedWGrenade.h"

bool CWeaponMagazined::CheckAnimationExist(LPCSTR anim_name)
{
	auto ptr = psAnimationList.count(anim_name);

	if (ptr > 0)
	{
		return psAnimationList[anim_name];
	}
	else
	{
		bool res = pSettings->line_exist(hud_sect, anim_name);
		psAnimationList[anim_name] = res;
		return res;
	}
}
void CWeaponMagazined::ReloadAnimationsExist()
{
	for (auto &it : psAnimationList)
	{
		it.second = pSettings->line_exist(hud_sect, it.first);
	}
}