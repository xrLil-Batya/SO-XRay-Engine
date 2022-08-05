#pragma once

#include "UIWindow.h"
#include "UIDoubleProgressBar.h"
#include "..\..\xrServerEntities\alife_space.h"

class CCustomOutfit;
class CHelmet;
class CUIStatic;
class CUIDoubleProgressBar;
class CUIXml;

class CUIOutfitImmunity : public CUIWindow
{
public:
					CUIOutfitImmunity	();
	virtual			~CUIOutfitImmunity	();

			void	InitFromXml			( CUIXml& xml_doc, LPCSTR base_str, u32 hit_type );
			void	Init				( CUIXml& xml, LPCSTR section );
			void	SetProgressValue	( float cur, float comp );
			void	SetValue( float value );

protected:
	CUIStatic*				m_caption;
	CUITextWnd*				m_val;
	CUIStatic				m_name; // texture + name
	CUIDoubleProgressBar	m_progress;
	CUITextWnd				m_value; // 100%
	float					m_magnitude;
	shared_str				m_unit_str;

}; // class CUIOutfitImmunity

// -------------------------------------------------------------------------------------

class CUIOutfitInfo : public CUIWindow
{
public:
					CUIOutfitInfo		();
	virtual			~CUIOutfitInfo		();

			void 	InitFromXml			( CUIXml& xml_doc );
			void 	UpdateInfo			( CCustomOutfit* cur_outfit, CCustomOutfit* slot_outfit = NULL );	
			void 	UpdateInfo			( CHelmet* cur_helmet, CHelmet* slot_helmet = NULL );	
protected:
	enum				{ max_count = ALife::eHitTypeMax-2 };
	
	CUIStatic*			m_Prop_line;
	CUIOutfitImmunity*	m_condition;
	CUIOutfitImmunity*	m_items[max_count];

}; // class CUIOutfitInfo
