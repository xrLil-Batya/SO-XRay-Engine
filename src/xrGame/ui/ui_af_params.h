#pragma once
#include "UIWindow.h"
#include "..\..\xrServerEntities\alife_space.h"
#include "artefact.h"
#include "inventory.h"
#include "inventory_item.h"


class CUIXml;
class CUIStatic;
class CUITextWnd;
class UIArtefactParamItem;
class CCustomOutfit;
class CHelmet;
class CBackpack;
class CArtefact;
class CWeapon;

class CUIArtefactParams : public CUIWindow
{
public:
	CUIArtefactParams();
	virtual ~CUIArtefactParams();
	void InitFromXml(CUIXml& xml);
	bool Check(CArtefact* artefact, const shared_str& af_section);
	void SetInfo(CCustomOutfit* pInvItem);
	void SetInfo(CHelmet* pInvItem);
	void SetInfo(CBackpack* pInvItem);
	void SetInfo(CInventoryItem* pInvItem);
	void SetInfo(CWeapon* pInvItem);
	void SetInfo(CArtefact* artefact, const shared_str& af_section);

protected:
	UIArtefactParamItem* m_immunity_item[ALife::infl_max_count];
	UIArtefactParamItem* m_restore_item[ALife::eRestoreTypeMax];
	UIArtefactParamItem* m_condition;
	UIArtefactParamItem* m_additional_weight;
	UIArtefactParamItem* m_JumpSpeed;
	UIArtefactParamItem* m_WalkAccel;

	CUIStatic* m_Prop_line;

}; // class CUIArtefactParams

// -----------------------------------

class UIArtefactParamItem : public CUIWindow
{
public:
	UIArtefactParamItem();
	virtual		~UIArtefactParamItem();

	void	Init(CUIXml& xml, LPCSTR section);
	void	SetCaption(LPCSTR name);
	void	SetValue(float value);

private:
	CUIStatic* m_caption;
	CUITextWnd* m_value;
	float		m_magnitude;
	float		m_accuracy;
	bool		m_sign_inverse;
	shared_str	m_unit_str;
	shared_str	m_texture_minus;
	shared_str	m_texture_plus;

}; // class UIArtefactParamItem
