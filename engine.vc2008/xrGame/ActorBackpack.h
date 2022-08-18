#pragma once

#include "inventory_item_object.h"

class CBackpack : public CInventoryItemObject {
private:
	typedef	CInventoryItemObject inherited;
public:
	CBackpack();
	virtual					~CBackpack();

	virtual void			Load(LPCSTR section);

	virtual void			Hit(float P, ALife::EHitType hit_type);

	virtual void			OnMoveToSlot(const SInvItemPlace& prev);
	virtual void			OnMoveToRuck(const SInvItemPlace& previous_place);
	virtual void			OnH_A_Chield();
	
	float HitThroughArmor(float hit_power, s16 element, float ap, bool& add_wound, ALife::EHitType hit_type);
	float GetDefHitTypeProtection(ALife::EHitType hit_type);

	float m_fPowerLoss;
	float m_fHealthRestoreSpeed;
	float m_fRadiationRestoreSpeed;
	float m_fSatietyRestoreSpeed;
	float m_fPowerRestoreSpeed;
	float m_fBleedingRestoreSpeed;
	float m_fPsyHealthRestoreSpeed;

	HitImmunity::HitTypeSVec m_HitTypeProtection;

	float m_additional_weight;
	float m_additional_weight2;
	float m_fJumpSpeed;
	float m_fWalkAccel;

	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			net_Export(NET_Packet& P);
	virtual void			net_Import(NET_Packet& P);

protected:
	virtual bool			install_upgrade_impl(LPCSTR section, bool test);
};