#include "stdafx.h"
#include "ui_af_params.h"
#include "artefact.h"
#include "UIStatic.h"

#include "..\actor.h"
#include "..\ActorCondition.h"
#include "object_broker.h"
#include "UIXmlInit.h"
#include "UIHelper.h"
#include "../string_table.h"

#include "..\CustomOutfit.h"
#include "..\ActorHelmet.h"
#include "..\ActorBackpack.h"
#include "..\Weapon.h"

u32 const red_clr   = color_argb(255,210,50,50);
u32 const green_clr = color_argb(255,170,170,170);

CUIArtefactParams::CUIArtefactParams()
{
	for ( u32 i = 0; i < ALife::infl_max_count; ++i )
	{
		m_immunity_item[i] = NULL;
	}
	for ( u32 i = 0; i < ALife::eRestoreTypeMax; ++i )
	{
		m_restore_item[i] = NULL;
	}
	m_condition = NULL;
	m_additional_weight = NULL;
	m_JumpSpeed = NULL;
	m_WalkAccel = NULL;
}

CUIArtefactParams::~CUIArtefactParams()
{
	delete_data	( m_immunity_item );
	delete_data	( m_restore_item );
	xr_delete	( m_additional_weight );
	xr_delete	( m_Prop_line );
}

LPCSTR af_immunity_section_names[] = // ALife::EInfluenceType
{
	"radiation_immunity",		// infl_rad=0
	"burn_immunity",			// infl_fire=1
	"chemical_burn_immunity",	// infl_acid=2
	"telepatic_immunity",		// infl_psi=3
	"shock_immunity",			// infl_electra=4

	"strike_immunity",			// infl_strike5
	"wound_immunity",			// infl_wound=6
	"fire_wound_immunity",		// infl_firewound=7
	"explosion_immunity",		// infl_explosion=8	
};

LPCSTR af_restore_section_names[] = // ALife::EConditionRestoreType
{
	"health_restore_speed",			// eHealthRestoreSpeed=0
	"satiety_restore_speed",		// eSatietyRestoreSpeed=1
	"power_restore_speed",			// ePowerRestoreSpeed=2
	"bleeding_restore_speed",		// eBleedingRestoreSpeed=3
	"psyhealth_restore_speed",		// ePsyHealthRestoreSpeed=5
	"radiation_restore_speed",		// eRadiationRestoreSpeed=4
};

LPCSTR af_immunity_caption[] =  // ALife::EInfluenceType
{
	"ui_inv_outfit_radiation_protection",		// "(radiation_imm)",
	"ui_inv_outfit_burn_protection",			// "(burn_imm)",
	"ui_inv_outfit_chemical_burn_protection",	// "(chemical_burn_imm)",
	"ui_inv_outfit_telepatic_protection",		// "(telepatic_imm)",
	"ui_inv_outfit_shock_protection",			// "(shock_imm)",

	"ui_inv_outfit_strike_protection",			// "(strike_imm)",
	"ui_inv_outfit_wound_protection",			// "(wound_imm)",
	"ui_inv_fire_wound",						// "(fire_wound_imm)",
	"ui_inv_outfit_explosion_protection",		// "(explosion_imm)",
};

LPCSTR af_restore_caption[] =  // ALife::EConditionRestoreType
{
	"ui_inv_health",
	"ui_inv_satiety",
	"ui_inv_power",
	"ui_inv_bleeding",
	"ui_inv_psyhealth",
	"ui_inv_radiation",
};

/*
LPCSTR af_actor_param_names[]=
{
	"satiety_health_v",
	"radiation_v",
	"satiety_v",
	"satiety_power_v",
	"wound_incarnation_v",
};
*/

void CUIArtefactParams::InitFromXml( CUIXml& xml )
{
	LPCSTR base	= "af_params";

	XML_NODE* stored_root = xml.GetLocalRoot();
	XML_NODE* base_node   = xml.NavigateToNode( base, 0 );
	if ( !base_node )
	{
		return;
	}
	CUIXmlInit::InitWindow( xml, base, 0, this );
	xml.SetLocalRoot( base_node );
	
	m_Prop_line = xr_new<CUIStatic>();
	AttachChild( m_Prop_line );
	m_Prop_line->SetAutoDelete( false );	
	CUIXmlInit::InitStatic( xml, "prop_line", 0, m_Prop_line );

	{
		m_condition = xr_new<UIArtefactParamItem>();
		m_condition->Init( xml, "art_condition" );
		m_condition->SetAutoDelete(false);

		LPCSTR nam = CStringTable().translate( "st_condition" ).c_str();
		m_condition->SetCaption( nam );

		xml.SetLocalRoot( base_node );
	}

	for ( u32 i = 0; i < ALife::infl_max_count; ++i )
	{
		m_immunity_item[i] = xr_new<UIArtefactParamItem>();
		m_immunity_item[i]->Init( xml, af_immunity_section_names[i] );
		m_immunity_item[i]->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate(af_immunity_caption[i]).c_str();
		m_immunity_item[i]->SetCaption( name );

		xml.SetLocalRoot( base_node );
	}

	for ( u32 i = 0; i < ALife::eRestoreTypeMax; ++i )
	{
		m_restore_item[i] = xr_new<UIArtefactParamItem>();
		m_restore_item[i]->Init( xml, af_restore_section_names[i] );
		m_restore_item[i]->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate(af_restore_caption[i]).c_str();
		m_restore_item[i]->SetCaption( name );

		xml.SetLocalRoot( base_node );
	}
	{
		m_WalkAccel = xr_new<UIArtefactParamItem>();
		m_WalkAccel->Init( xml, "walk_accel" );
		m_WalkAccel->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate( "ui_inv_walk_accel" ).c_str();
		m_WalkAccel->SetCaption( name );

		xml.SetLocalRoot( base_node );
	}
	{
		m_additional_weight = xr_new<UIArtefactParamItem>();
		m_additional_weight->Init( xml, "additional_weight" );
		m_additional_weight->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate( "ui_inv_weight" ).c_str();
		m_additional_weight->SetCaption( name );

		xml.SetLocalRoot( base_node );
	}
	{
		m_JumpSpeed = xr_new<UIArtefactParamItem>();
		m_JumpSpeed->Init( xml, "jump_speed" );
		m_JumpSpeed->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate( "ui_inv_jump_speed" ).c_str();
		m_JumpSpeed->SetCaption( name );
	}
	xml.SetLocalRoot( stored_root );
}

bool CUIArtefactParams::Check(CArtefact* artefact, const shared_str& af_section)
{
	return !!pSettings->line_exist(af_section, "af_actor_properties");
}

void CUIArtefactParams::SetInfo(CArtefact* artefact, shared_str const& af_section )
{
	DetachAll();
	AttachChild( m_Prop_line );

	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor )
	{
		return;
	}

	float val = 0.0f, max_val = 1.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;

	{
		val	= artefact->GetCondition();
		{
			m_condition->SetValue( val );

			pos.set( m_condition->GetWndPos() );
			pos.y = h;
			m_condition->SetWndPos( pos );

			h += m_condition->GetWndSize().y;
			AttachChild( m_condition );
		}
	}
	{
        const float* const arrayOfImmunities[ALife::infl_max_count] =
        {
            &artefact->m_ArtefactHitImmunities[ALife::eHitTypeRadiation],
            &artefact->m_ArtefactHitImmunities[ALife::eHitTypeBurn],
            &artefact->m_ArtefactHitImmunities[ALife::eHitTypeChemicalBurn],
            &artefact->m_ArtefactHitImmunities[ALife::eHitTypeTelepatic],
            &artefact->m_ArtefactHitImmunities[ALife::eHitTypeShock],
            &artefact->m_ArtefactHitImmunities[ALife::eHitTypeStrike],
            &artefact->m_ArtefactHitImmunities[ALife::eHitTypeWound],
            &artefact->m_ArtefactHitImmunities[ALife::eHitTypeFireWound],
            &artefact->m_ArtefactHitImmunities[ALife::eHitTypeExplosion]
        };

        for (u32 i = 0; i < ALife::infl_max_count; ++i)
        {
            val = *arrayOfImmunities[i] * artefact->GetCondition();
			val *= artefact->GetCondition();
            if (fis_zero(val))
                continue;
			max_val = actor->conditions().GetZoneMaxPower( (ALife::EInfluenceType)i );
			val /= max_val;
			m_immunity_item[i]->SetValue( val );

			pos.set( m_immunity_item[i]->GetWndPos() );
			pos.y = h;
			m_immunity_item[i]->SetWndPos( pos );

			h += m_immunity_item[i]->GetWndSize().y;
			AttachChild( m_immunity_item[i] );
		}
	}

	{
        const float* const arrayOfImmunities[ALife::eRestoreTypeMax] =
        {
            &artefact->m_fHealthRestoreSpeed,
            &artefact->m_fSatietyRestoreSpeed,
            &artefact->m_fPowerRestoreSpeed,
            &artefact->m_fBleedingRestoreSpeed,
            &artefact->m_fPsyHealthRestoreSpeed,
            &artefact->m_fRadiationRestoreSpeed
        };
        for (u32 i = 0; i < ALife::eRestoreTypeMax; ++i)
        {
            val = *arrayOfImmunities[i];
			val *= artefact->GetCondition();
            if (fis_zero(val))
                continue;

            m_restore_item[i]->SetValue(val);

            pos.set(m_restore_item[i]->GetWndPos());
            pos.y = h;
            m_restore_item[i]->SetWndPos(pos);

            h += m_restore_item[i]->GetWndSize().y;
            AttachChild(m_restore_item[i]);
        }
    }

	{
		val	= artefact->m_fWalkAccel;
		val *= artefact->GetCondition();
		if ( !fis_zero(val) )
		{
			m_WalkAccel->SetValue( val );

			pos.set( m_WalkAccel->GetWndPos() );
			pos.y = h;
			m_WalkAccel->SetWndPos( pos );

			h += m_WalkAccel->GetWndSize().y;
			AttachChild( m_WalkAccel );
		}
		val	= artefact->m_additional_weight;
		val *= artefact->GetCondition();
		if ( !fis_zero(val) )
		{
			m_additional_weight->SetValue( val );

			pos.set( m_additional_weight->GetWndPos() );
			pos.y = h;
			m_additional_weight->SetWndPos( pos );

			h += m_additional_weight->GetWndSize().y;
			AttachChild( m_additional_weight );
		}
		val	= artefact->m_fJumpSpeed;
		val *= artefact->GetCondition();
		if ( !fis_zero(val) )
		{
			m_JumpSpeed->SetValue( val );

			pos.set( m_JumpSpeed->GetWndPos() );
			pos.y = h;
			m_JumpSpeed->SetWndPos( pos );

			h += m_JumpSpeed->GetWndSize().y;
			AttachChild( m_JumpSpeed );
		}
	}
	
	SetHeight( h );
}
void CUIArtefactParams::SetInfo(CCustomOutfit* pInvItem)
{
	DetachAll();
	AttachChild(m_Prop_line);

	CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if (!actor)
	{
		return;
	}

	const shared_str& af_section = pInvItem->cNameSect();

	float val = 0.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y;

	{
		float val	= pInvItem->GetCondition();
		{
			m_condition->SetValue( val );

			pos.set( m_condition->GetWndPos() );
			pos.y = h;
			m_condition->SetWndPos( pos );

			h += m_condition->GetWndSize().y;
			AttachChild( m_condition );
		}
	}

	{
		val = pSettings->r_float(af_section, "additional_inventory_weight");
		if (!fis_zero(val))
		{
			val *= pInvItem->GetCondition();
			m_additional_weight->SetValue(val);

			pos.set(m_additional_weight->GetWndPos());
			pos.y = h;
			m_additional_weight->SetWndPos(pos);

			h += m_additional_weight->GetWndSize().y;
			AttachChild(m_additional_weight);
		}
	}

	SetHeight(h);
}

void CUIArtefactParams::SetInfo(CHelmet* pInvItem)
{
	DetachAll();
	AttachChild(m_Prop_line);

	CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if (!actor)
	{
		return;
	}

	const shared_str& af_section = pInvItem->cNameSect();

	float val = 0.0f, max_val = 1.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y;

	{
		float val	= pInvItem->GetCondition();
		{
			m_condition->SetValue( val );

			pos.set( m_condition->GetWndPos() );
			pos.y = h;
			m_condition->SetWndPos( pos );

			h += m_condition->GetWndSize().y;
			AttachChild( m_condition );
		}
	}

	SetHeight(h);
}

void CUIArtefactParams::SetInfo(CBackpack* pInvItem)
{
	DetachAll();
	AttachChild( m_Prop_line );

	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor )
	{
		return;
	}

	float val = 0.0f, max_val = 1.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;

	{
		val	= pInvItem->GetCondition();
		{
			m_condition->SetValue( val );

			pos.set( m_condition->GetWndPos() );
			pos.y = h;
			m_condition->SetWndPos( pos );

			h += m_condition->GetWndSize().y;
			AttachChild( m_condition );
		}
	}
	{
        const float* const arrayOfImmunities[ALife::infl_max_count] =
        {
            &pInvItem->m_HitTypeProtection[ALife::eHitTypeRadiation],
            &pInvItem->m_HitTypeProtection[ALife::eHitTypeBurn],
            &pInvItem->m_HitTypeProtection[ALife::eHitTypeChemicalBurn],
            &pInvItem->m_HitTypeProtection[ALife::eHitTypeTelepatic],
            &pInvItem->m_HitTypeProtection[ALife::eHitTypeShock],
            &pInvItem->m_HitTypeProtection[ALife::eHitTypeStrike],
            &pInvItem->m_HitTypeProtection[ALife::eHitTypeWound],
            &pInvItem->m_HitTypeProtection[ALife::eHitTypeFireWound],
            &pInvItem->m_HitTypeProtection[ALife::eHitTypeExplosion]
        };

        for (u32 i = 0; i < ALife::infl_max_count; ++i)
        {
            val = *arrayOfImmunities[i] * pInvItem->GetCondition();
			val *= pInvItem->GetCondition();
            if (fis_zero(val))
                continue;
			max_val = actor->conditions().GetZoneMaxPower( (ALife::EInfluenceType)i );
			val /= max_val;
			m_immunity_item[i]->SetValue( val );

			pos.set( m_immunity_item[i]->GetWndPos() );
			pos.y = h;
			m_immunity_item[i]->SetWndPos( pos );

			h += m_immunity_item[i]->GetWndSize().y;
			AttachChild( m_immunity_item[i] );
		}
	}

	{
        const float* const arrayOfImmunities[ALife::eRestoreTypeMax] =
        {
            &pInvItem->m_fHealthRestoreSpeed,
            &pInvItem->m_fSatietyRestoreSpeed,
            &pInvItem->m_fPowerRestoreSpeed,
            &pInvItem->m_fBleedingRestoreSpeed,
            &pInvItem->m_fPsyHealthRestoreSpeed,
            &pInvItem->m_fRadiationRestoreSpeed
        };
        for (u32 i = 0; i < ALife::eRestoreTypeMax; ++i)
        {
            val = *arrayOfImmunities[i];
			val *= pInvItem->GetCondition();
            if (fis_zero(val))
                continue;

            m_restore_item[i]->SetValue(val);

            pos.set(m_restore_item[i]->GetWndPos());
            pos.y = h;
            m_restore_item[i]->SetWndPos(pos);

            h += m_restore_item[i]->GetWndSize().y;
            AttachChild(m_restore_item[i]);
        }
    }

	{
		val	= pInvItem->m_fWalkAccel;
		val *= pInvItem->GetCondition();
		if ( !fis_zero(val) )
		{
			m_WalkAccel->SetValue( val );

			pos.set( m_WalkAccel->GetWndPos() );
			pos.y = h;
			m_WalkAccel->SetWndPos( pos );

			h += m_WalkAccel->GetWndSize().y;
			AttachChild( m_WalkAccel );
		}
		val	= pInvItem->m_additional_weight;
		val *= pInvItem->GetCondition();
		if ( !fis_zero(val) )
		{
			m_additional_weight->SetValue( val );

			pos.set( m_additional_weight->GetWndPos() );
			pos.y = h;
			m_additional_weight->SetWndPos( pos );

			h += m_additional_weight->GetWndSize().y;
			AttachChild( m_additional_weight );
		}
		val	= pInvItem->m_fJumpSpeed;
		val *= pInvItem->GetCondition();
		if ( !fis_zero(val) )
		{
			m_JumpSpeed->SetValue( val );

			pos.set( m_JumpSpeed->GetWndPos() );
			pos.y = h;
			m_JumpSpeed->SetWndPos( pos );

			h += m_JumpSpeed->GetWndSize().y;
			AttachChild( m_JumpSpeed );
		}
	}
	
	SetHeight( h );
}

void CUIArtefactParams::SetInfo(CWeapon* pInvItem)
{
	DetachAll();
	AttachChild(m_Prop_line);

	CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if (!actor)
	{
		return;
	}

	const shared_str& af_section = pInvItem->cNameSect();

	float val = 0.0f, max_val = 1.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y;

	{
		float val	= pInvItem->GetCondition();
		{
			m_condition->SetValue( val );

			pos.set( m_condition->GetWndPos() );
			pos.y = h;
			m_condition->SetWndPos( pos );
			h += m_condition->GetWndSize().y;

			AttachChild( m_condition );
		}
	}
	SetHeight(h);
}

void CUIArtefactParams::SetInfo(CInventoryItem* pInvItem)
{
	DetachAll();
	AttachChild(m_Prop_line);

	CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if (!actor)
	{
		return;
	}

	float val = 0.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y;
	{
		float val	= pInvItem->GetCondition();
		{
			m_condition->SetValue( val );

			pos.set( m_condition->GetWndPos() );
			pos.y = h;
			m_condition->SetWndPos( pos );

			h += m_condition->GetWndSize().y;
			AttachChild( m_condition );
		}
	}
	SetHeight(h);
}
/// ----------------------------------------------------------------

UIArtefactParamItem::UIArtefactParamItem()
{
	m_caption   = NULL;
	m_value     = NULL;
	m_magnitude = 1.0f;
	m_sign_inverse = false;
	
	m_unit_str._set( "" );
	m_texture_minus._set( "" );
	m_texture_plus._set( "" );
}

UIArtefactParamItem::~UIArtefactParamItem()
{
}

void UIArtefactParamItem::Init( CUIXml& xml, LPCSTR section )
{
	CUIXmlInit::InitWindow( xml, section, 0, this );
	xml.SetLocalRoot( xml.NavigateToNode( section ) );

	m_caption   = UIHelper::CreateStatic( xml, "caption", this );
	m_value     = UIHelper::CreateTextWnd( xml, "value",   this );
	m_magnitude = xml.ReadAttribFlt( "value", 0, "magnitude", 1.0f );
	m_accuracy = xml.ReadAttribFlt( "value", 0, "accuracy", 0.0f );
	m_sign_inverse = (xml.ReadAttribInt( "value", 0, "sign_inverse", 0 ) == 1);
	
	LPCSTR unit_str = xml.ReadAttrib( "value", 0, "unit_str", "" );
	m_unit_str._set( CStringTable().translate( unit_str ) );
	
	LPCSTR texture_minus = xml.Read( "texture_minus", 0, "" );
	if ( texture_minus && xr_strlen(texture_minus) )
	{
		m_texture_minus._set( texture_minus );
		
		LPCSTR texture_plus = xml.Read( "caption:texture", 0, "" );
		m_texture_plus._set( texture_plus );
		VERIFY( m_texture_plus.size() );
	}
}

void UIArtefactParamItem::SetCaption( LPCSTR name )
{
	m_caption->TextItemControl()->SetText( name );
}

void UIArtefactParamItem::SetValue( float value )
{
	value *= m_magnitude;
	string32	buf;

	if (m_accuracy==0)
		xr_sprintf( buf, "%+.0f", value );
	else if (m_accuracy==1)
		xr_sprintf( buf, "%+.1f", value );
	else if (m_accuracy==2)
		xr_sprintf( buf, "%+.2f", value );
	
	LPSTR		str;
	if ( m_unit_str.size() )
	{
		STRCONCAT( str, buf, " ", m_unit_str.c_str() );
	}
	else // = ""
	{
		STRCONCAT( str, buf );
	}
	m_value->SetText( str );

	bool positive = (value >= 0.0f);
	positive      = (m_sign_inverse)? !positive : positive;
	u32 color     = (positive      )? green_clr : red_clr;
	m_value->SetTextColor( color );

	if ( m_texture_minus.size() )
	{
		if ( positive )
		{
			m_caption->InitTexture( m_texture_plus.c_str() );
		}
		else
		{
			m_caption->InitTexture( m_texture_minus.c_str() );
		}
	}

}

int UIArtefactParamItem::isInteger(float val)
{
	float res = val - (int)val;
	if (res > 0)
		return 0;
	else
		return(int)val;
}