#include "stdafx.h"
#include "ActorBackpack.h"
#include "Actor.h"
#include "Inventory.h"

CBackpack::CBackpack()
{
	m_flags.set(FUsingCondition, TRUE);
	m_HitTypeProtection.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeProtection[i] = 1.0f;
}

CBackpack::~CBackpack()
{
}

void CBackpack::Load(LPCSTR section)
{
	inherited::Load(section);

	m_HitTypeProtection[ALife::eHitTypeBurn] = READ_IF_EXISTS(pSettings, r_float, section, "burn_protection", 0.0f);
	m_HitTypeProtection[ALife::eHitTypeStrike] = READ_IF_EXISTS(pSettings, r_float, section, "strike_protection", 0.0f);
	m_HitTypeProtection[ALife::eHitTypeShock] = READ_IF_EXISTS(pSettings, r_float, section, "shock_protection", 0.0f);
	m_HitTypeProtection[ALife::eHitTypeWound] = READ_IF_EXISTS(pSettings, r_float, section, "wound_protection", 0.0f);
	m_HitTypeProtection[ALife::eHitTypeRadiation] = READ_IF_EXISTS(pSettings, r_float, section, "radiation_protection", 0.0f);
	m_HitTypeProtection[ALife::eHitTypeTelepatic] = READ_IF_EXISTS(pSettings, r_float, section, "telepatic_protection", 0.0f);
	m_HitTypeProtection[ALife::eHitTypeChemicalBurn] = READ_IF_EXISTS(pSettings, r_float, section, "chemical_burn_protection", 0.0f);
	m_HitTypeProtection[ALife::eHitTypeExplosion] = READ_IF_EXISTS(pSettings, r_float, section, "explosion_protection", 0.0f);
	m_HitTypeProtection[ALife::eHitTypeFireWound] = READ_IF_EXISTS(pSettings, r_float, section, "fire_wound_protection", 0.0f);
	m_HitTypeProtection[ALife::eHitTypeLightBurn] = m_HitTypeProtection[ALife::eHitTypeBurn];
	
	m_fHealthRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "health_restore_speed", 0.0f);
	m_fRadiationRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "radiation_restore_speed", 0.0f);
	m_fSatietyRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "satiety_restore_speed", 0.0f);
	m_fPowerRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "power_restore_speed", 0.0f);
	m_fBleedingRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "bleeding_restore_speed", 0.0f);
	m_fPsyHealthRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "psyhealth_restore_speed", 0.0f);
	m_fPowerLoss = READ_IF_EXISTS(pSettings, r_float, section, "power_loss", 1.0f);
	clamp(m_fPowerLoss, 0.0f, 1.0f);

	m_additional_weight = READ_IF_EXISTS(pSettings, r_float, section, "additional_inventory_weight", 0.f);
	m_additional_weight2 = READ_IF_EXISTS(pSettings, r_float, section, "additional_inventory_weight2", 0.f);
	m_fJumpSpeed = READ_IF_EXISTS(pSettings, r_float, section, "jump_speed", 0.f);
	m_fWalkAccel = READ_IF_EXISTS(pSettings, r_float, section, "walk_accel", 0.f);
}

BOOL CBackpack::net_Spawn(CSE_Abstract* DC)
{
	return(inherited::net_Spawn(DC));
}

void CBackpack::net_Export(NET_Packet& P)
{
	inherited::net_Export(P);
}

void CBackpack::net_Import(NET_Packet& P)
{
	inherited::net_Import(P);
}

void CBackpack::OnH_A_Chield()
{
	inherited::OnH_A_Chield();
}

void CBackpack::OnMoveToSlot(const SInvItemPlace& previous_place)
{
	inherited::OnMoveToSlot(previous_place);
}

void CBackpack::OnMoveToRuck(const SInvItemPlace& previous_place)
{
	inherited::OnMoveToRuck(previous_place);
}

void CBackpack::Hit(float hit_power, ALife::EHitType hit_type)
{
	hit_power *= GetHitImmunity(hit_type);
	ChangeCondition(-hit_power);
}

float CBackpack::GetDefHitTypeProtection(ALife::EHitType hit_type)
{
	return m_HitTypeProtection[hit_type]*GetCondition();
}

bool CBackpack::install_upgrade_impl(LPCSTR section, bool test)
{
	bool result = inherited::install_upgrade_impl(section, test);

	result |= process_if_exists(section, "burn_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeBurn], test);
	result |= process_if_exists(section, "shock_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeShock], test);
	result |= process_if_exists(section, "strike_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeStrike], test);
	result |= process_if_exists(section, "wound_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeWound], test);
	result |= process_if_exists(section, "radiation_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeRadiation], test);
	result |= process_if_exists(section, "telepatic_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeTelepatic], test);
	result |= process_if_exists(section, "chemical_burn_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeChemicalBurn], test);
	result |= process_if_exists(section, "explosion_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeExplosion], test);
	result |= process_if_exists(section, "fire_wound_protection", &CInifile::r_float, m_HitTypeProtection[ALife::eHitTypeFireWound], test);
	
	result |= process_if_exists(section, "health_restore_speed", &CInifile::r_float, m_fHealthRestoreSpeed, test);
	result |= process_if_exists(section, "radiation_restore_speed", &CInifile::r_float, m_fRadiationRestoreSpeed, test);
	result |= process_if_exists(section, "satiety_restore_speed", &CInifile::r_float, m_fSatietyRestoreSpeed, test);
	result |= process_if_exists(section, "bleeding_restore_speed", &CInifile::r_float, m_fBleedingRestoreSpeed, test);
	result |= process_if_exists(section, "power_restore_speed", &CInifile::r_float, m_fPowerRestoreSpeed, test);
	result |= process_if_exists(section, "power_loss", &CInifile::r_float, m_fPowerLoss, test);
	clamp(m_fPowerLoss, 0.0f, 1.0f);

	result |= process_if_exists(section, "additional_inventory_weight", &CInifile::r_float, m_additional_weight, test);
	result |= process_if_exists(section, "additional_inventory_weight2", &CInifile::r_float, m_additional_weight2, test);

	return result;
}

float CBackpack::HitThroughArmor(float hit_power, s16 element, float ap, bool& add_wound, ALife::EHitType hit_type)
{
	float NewHitPower = hit_power;
	float one = 0.1f;
	if(hit_type == ALife::eHitTypeStrike || 
	   hit_type == ALife::eHitTypeWound || 
	   hit_type == ALife::eHitTypeWound_2 || 
	   hit_type == ALife::eHitTypeExplosion || 
	   hit_type == ALife::eHitTypeFireWound)
	{
		one = 1.0f;
	}
	float protect = GetDefHitTypeProtection(hit_type);
	NewHitPower -= protect * one;

	if(NewHitPower < 0.f)
		NewHitPower = 0.f;

	//увеличить изношенность рюкзака
	Hit(hit_power, hit_type);

	return NewHitPower;
}
