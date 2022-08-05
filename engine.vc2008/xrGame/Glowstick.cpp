#include "stdafx.h"
#include "Glowstick.h"
#include "inventory.h"
#include "player_hud.h"
#include "weapon.h"
#include "hudsound.h"
#include "ai_sounds.h"
#include "../xrEngine/LightAnimLibrary.h"
#include "../xrEngine/camerabase.h"
#include "../xrengine/xr_collide_form.h"
#include "Missile.h"
#include "level.h"
#include "actor.h"

ENGINE_API int g_current_renderer;

CGlowstick::CGlowstick()
{
	m_bFastAnimMode = false;
	m_bNeedActivation = false;

	light_render = ::Render->light_create();
	light_render->set_type(IRender_Light::SPOT);
	light_render->set_shadow(true);
	light_omni = ::Render->light_create();
	light_omni->set_type(IRender_Light::POINT);
	light_omni->set_shadow(false);

	glow_render = ::Render->glow_create();
	lanim = 0;
	fBrightness = 1.f;

	m_light_section = "glowstick_definition";
}

CGlowstick::~CGlowstick()
{
	light_render.destroy();
	light_omni.destroy();
	glow_render.destroy();
}

bool CGlowstick::CheckCompatibilityInt(CHudItem* itm, u16* slot_to_activate)
{
	if (itm == NULL)
		return true;

	CInventoryItem& iitm = itm->item();
	u32 slot = iitm.BaseSlot();
	bool bres = (slot == INV_SLOT_2 || slot == KNIFE_SLOT || slot == BOLT_SLOT);
	if (!bres && slot_to_activate)
	{
		*slot_to_activate = NO_ACTIVE_SLOT;
		if (m_pInventory->ItemFromSlot(BOLT_SLOT))
			*slot_to_activate = BOLT_SLOT;

		if (m_pInventory->ItemFromSlot(KNIFE_SLOT))
			*slot_to_activate = KNIFE_SLOT;

		if (m_pInventory->ItemFromSlot(INV_SLOT_3) && m_pInventory->ItemFromSlot(INV_SLOT_3)->BaseSlot() != INV_SLOT_3)
			*slot_to_activate = INV_SLOT_3;

		if (m_pInventory->ItemFromSlot(INV_SLOT_2) && m_pInventory->ItemFromSlot(INV_SLOT_2)->BaseSlot() != INV_SLOT_3)
			*slot_to_activate = INV_SLOT_2;

		if (*slot_to_activate != NO_ACTIVE_SLOT)
			bres = true;
	}

	if (itm->GetState() != CHUDState::eShowing)
		bres = bres && !itm->IsPending();

	if (bres)
	{
		CWeapon* W = smart_cast<CWeapon*>(itm);
		if (W)
			bres = bres && (W->GetState() != CHUDState::eBore) && (W->GetState() != CWeapon::eReload) && (W->GetState() != CWeapon::eSwitch);
	}
	return bres;
}

bool  CGlowstick::CheckCompatibility(CHudItem* itm)
{
	if (!inherited::CheckCompatibility(itm))
		return false;

	if (!CheckCompatibilityInt(itm, NULL))
	{
		HideGlowstick(true);
		return			false;
	}
	return true;
}

void CGlowstick::HideGlowstick(bool bFastMode)
{
	if (GetState() == eIdle)
		ToggleGlowstick(bFastMode);
}

void CGlowstick::ShowGlowstick(bool bFastMode)
{
	if (GetState() == eHidden)
		ToggleGlowstick(bFastMode);
}

void CGlowstick::ToggleGlowstick(bool bFastMode)
{
	m_bNeedActivation = false;
	m_bFastAnimMode = bFastMode;

	if (GetState() == eHidden)
	{
		PIItem iitem = m_pInventory->ActiveItem();
		CHudItem* itm = (iitem) ? iitem->cast_hud_item() : NULL;
		u16 slot_to_activate = NO_ACTIVE_SLOT;

		if (CheckCompatibilityInt(itm, &slot_to_activate))
		{
			if (slot_to_activate != NO_ACTIVE_SLOT)
			{
				m_pInventory->Activate(slot_to_activate);
				m_bNeedActivation = true;
				Switch(true);
			}
			else
			{
				SwitchState(eShowing);
				Switch(true);
			}
		}
	}
	else
		if (GetState() == eIdle){
			SwitchState(eHiding);
			Switch(false);
		}

}

void CGlowstick::OnStateSwitch(u32 S, u32 oldState)
{
	inherited::OnStateSwitch(S);

	switch (S)
	{
		case eShowing:
		{
			g_player_hud->attach_item(this);
			m_sounds.PlaySound("sndShow", Fvector().set(0, 0, 0), this, true, false);
			PlayHUDMotion(m_bFastAnimMode ? "anm_show_fast" : "anm_show", FALSE/*TRUE*/, this, GetState());
			SetPending(TRUE);
		}break;
		case eHiding:
		{
			if (oldState != eHiding)
			{
				m_sounds.PlaySound("sndHide", Fvector().set(0, 0, 0), this, true, false);
				PlayHUDMotion(m_bFastAnimMode ? "anm_hide_fast" : "anm_hide", FALSE/*TRUE*/, this, GetState());
				SetPending(TRUE);
			}
		}break;
		case eIdle:
		{
			PlayAnimIdle();
			SetPending(FALSE);
		}break;
	case eIdleThrowStart:
		{
			PlayHUDMotion ("anm_throw_begin", TRUE, this, GetState());
			SetPending(TRUE);
		}break;
	case eIdleThrow:
		{
			PlayHUDMotion ("anm_throw_idle", TRUE, this, GetState());
			SetPending(TRUE);
		}break;
	case eIdleThrowEnd:
		{
			PlayHUDMotion ("anm_throw", TRUE, this, GetState());
			SetPending(TRUE);
		}break;
	case eIdleZoom:
		{
			PlayHUDMotion ("anm_idle_zoom", TRUE, this, GetState());
			SetPending(FALSE);
		}break;
	case eIdleZoomIn:
		{
			PlayHUDMotion ("anm_zoom_in", TRUE, this, GetState());
			SetPending(FALSE);
		}break;
	case eIdleZoomOut:
		{
			PlayHUDMotion ("anm_zoom_out", TRUE, this, GetState());
			SetPending(FALSE);
		}break;
	}
}

void CGlowstick::OnAnimationEnd(u32 state)
{
	inherited::OnAnimationEnd(state);
	switch (state)
	{
		case eShowing:
		{
			SwitchState(eIdle);
		} break;
		case eHiding:
		{
			SwitchState(eHidden);
			g_player_hud->detach_item(this);
		} break;
	case eIdleThrowStart:
		{
			SwitchState(eIdleThrow);
		}break;
	case eIdleThrowEnd:
		{
			SwitchState(eIdle);
		}break;
	case eIdleZoomIn:
		{
			SwitchState(eIdleZoom);
		}break;
	case eIdleZoomOut:
		{
			SwitchState(eIdle);
		}break;
	}
}

void CGlowstick::UpdateXForm()
{
	CInventoryItem::UpdateXForm();
}

void CGlowstick::OnActiveItem()
{
	return;
}

void CGlowstick::OnHiddenItem()
{
}

BOOL CGlowstick::net_Spawn(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC))
		return FALSE;

	bool b_r2 = !!psDeviceFlags.test(rsR2);
	b_r2 |= !!psDeviceFlags.test(rsR3);
	b_r2 |= !!psDeviceFlags.test(rsR4);

	lanim = LALib.FindItem(pSettings->r_string(m_light_section, "color_animator"));

	Fcolor clr = pSettings->r_fcolor(m_light_section, (b_r2) ? "color_r2" : "color");
	fBrightness = clr.intensity();
	float range = pSettings->r_float(m_light_section, (b_r2) ? "range_r2" : "range");
	light_render->set_color(clr);
	light_render->set_range(range);
	light_render->set_hud_mode(true);

	Fcolor clr_o = pSettings->r_fcolor(m_light_section, (b_r2) ? "omni_color_r2" : "omni_color");
	float range_o = pSettings->r_float(m_light_section, (b_r2) ? "omni_range_r2" : "omni_range");
	light_omni->set_color(clr_o);
	light_omni->set_range(range_o);
	light_omni->set_hud_mode(true);

	light_render->set_cone(deg2rad(pSettings->r_float(m_light_section, "spot_angle")));
	light_render->set_texture(READ_IF_EXISTS(pSettings, r_string, m_light_section, "spot_texture", (0)));

	glow_render->set_texture(pSettings->r_string(m_light_section, "glow_texture"));
	glow_render->set_color(clr);
	glow_render->set_radius(pSettings->r_float(m_light_section, "glow_radius"));
	//glow_render->set_hud_mode(true);

	light_render->set_volumetric(!!READ_IF_EXISTS(pSettings, r_bool, m_light_section, "volumetric", 0));
	light_render->set_volumetric_quality(READ_IF_EXISTS(pSettings, r_float, m_light_section, "volumetric_quality", 1.f));
	light_render->set_volumetric_intensity(READ_IF_EXISTS(pSettings, r_float, m_light_section, "volumetric_intensity", 1.f));
	light_render->set_volumetric_distance(READ_IF_EXISTS(pSettings, r_float, m_light_section, "volumetric_distance", 1.f));
	light_render->set_type((IRender_Light::LT)(READ_IF_EXISTS(pSettings, r_u8, m_light_section, "type", 2)));
	light_omni->set_type((IRender_Light::LT)(READ_IF_EXISTS(pSettings, r_u8, m_light_section, "omni_type", 1)));

	return TRUE;
}

void CGlowstick::Load(LPCSTR section)
{
	inherited::Load(section);
	m_sounds.LoadSound(section, "snd_draw", "sndShow");
	m_sounds.LoadSound(section, "snd_holster", "sndHide");

	light_trace_bone = READ_IF_EXISTS(pSettings, r_string, section, "light_trace_bone", "");

	m_light_section = READ_IF_EXISTS(pSettings, r_string, section, "light_section", "torch_definition");
}


void CGlowstick::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
}


bool CGlowstick::IsWorking()
{
	return H_Parent() && H_Parent() == Level().CurrentViewEntity();
}

void CGlowstick::UpdateVisibility()
{
	//check visibility
	attachable_hud_item* i0 = g_player_hud->attached_item(0);
	if (i0 && HudItemData())
	{
		bool bClimb = ((Actor()->MovingState()&mcClimb) != 0);
		if (bClimb)
		{
			HideGlowstick(true);
			m_bNeedActivation = true;
		}
		else
		{
			CWeapon* wpn = smart_cast<CWeapon*>(i0->m_parent_hud_item);
			CMissile* msl = smart_cast<CMissile*>(i0->m_parent_hud_item);
			if(msl)
			{
				u32 state = msl->GetState();
				if ((state == CMissile::eThrowStart || state == CMissile::eReady) && GetState() == eIdle)
					SwitchState(eIdleThrowStart);
				else if (state == CMissile::eThrow && GetState() == eIdleThrow)
					SwitchState(eIdleThrowEnd);
				else if (state == CMissile::eHiding && (GetState() == eIdleThrowStart || GetState() == eIdleThrow))
					SwitchState(eIdle);
			}
			if(wpn)
			{
				u32 state = wpn->GetState();
			
				if(state==CWeapon::eReload || state==CWeapon::eSwitch)
				{
					HideGlowstick		(true);
					m_bNeedActivation	= true;
				}
				else if (wpn->IsZoomed())
				{
					if(GetState() == eIdle || GetState() == eIdleZoomOut)
						SwitchState(eIdleZoomIn);
				}
				else if (GetState() == eIdleZoom || GetState() == eIdleZoomIn)
					SwitchState(eIdleZoomOut);
			}
		}
	}
	else
		if (m_bNeedActivation)
		{
			attachable_hud_item* i0 = g_player_hud->attached_item(0);
			bool bClimb = ((Actor()->MovingState()&mcClimb) != 0);
			if (!bClimb)
			{
				CHudItem* huditem = (i0) ? i0->m_parent_hud_item : NULL;
				bool bChecked = !huditem || CheckCompatibilityInt(huditem, 0);

				if (bChecked)
					ShowGlowstick(true);
			}
		}
}

void CGlowstick::UpdateCL()
{
	inherited::UpdateCL();

	if (H_Parent() != Level().CurrentEntity())			
		return;

	CActor* actor = smart_cast<CActor*>(H_Parent());
	if (!actor)
		return;

	UpdateVisibility();

	if (!IsWorking())
		return;

	if (!HudItemData())
	{
		Switch(false);
		return;
	}

	firedeps dep;
	HudItemData()->setup_firedeps(dep);

	light_render->set_position(dep.vLastFP);
	light_omni->set_position(dep.vLastFP);
	glow_render->set_position(dep.vLastFP);

	light_render->set_rotation(dep.m_FireParticlesXForm.k, dep.m_FireParticlesXForm.i);
	light_omni->set_rotation(dep.m_FireParticlesXForm.k, dep.m_FireParticlesXForm.i);
	glow_render->set_direction(dep.m_FireParticlesXForm.k);

	// calc color animator
	if (!lanim)							
		return;

	int frame;

	u32 clr = lanim->CalculateBGR(Device.fTimeGlobal, frame);

	Fcolor fclr;
	fclr.set((float)color_get_B(clr), (float)color_get_G(clr), (float)color_get_R(clr), 1.f);
	fclr.mul_rgb(fBrightness / 255.f);
	if (can_use_dynamic_lights())
	{
		light_render->set_color(fclr);
		light_omni->set_color(fclr);
	}
	glow_render->set_color(fclr);
}

void CGlowstick::OnH_A_Chield()
{
	inherited::OnH_A_Chield();
}

void CGlowstick::OnH_B_Independent(bool just_before_destroy)
{
	inherited::OnH_B_Independent(just_before_destroy);

	if (GetState() != eHidden)
	{
		// Detaching hud item and animation stop in OnH_A_Independent
		SwitchState(eHidden);
	}
}

void CGlowstick::create_physic_shell()
{
	CPhysicsShellHolder::create_physic_shell();
}

void CGlowstick::activate_physic_shell()
{
	CPhysicsShellHolder::activate_physic_shell();
}

void CGlowstick::setup_physic_shell()
{
	CPhysicsShellHolder::setup_physic_shell();
}

void CGlowstick::OnMoveToRuck(const SInvItemPlace& prev)
{
	inherited::OnMoveToRuck(prev);
	if (prev.type == eItemPlaceSlot)
	{
		SwitchState(eHidden);
		g_player_hud->detach_item(this);
	}
	StopCurrentAnimWithoutCallback();
}

void CGlowstick::OnMoveToSlot(const SInvItemPlace& prev)
{
	inherited::OnMoveToSlot(prev);
}

inline bool CGlowstick::can_use_dynamic_lights()
{
	if (!H_Parent())
		return				(true);

	CInventoryOwner			*owner = smart_cast<CInventoryOwner*>(H_Parent());
	if (!owner)
		return				(true);

	return					(owner->can_use_dynamic_lights());
}

void CGlowstick::Switch(bool light_on)
{
	if (can_use_dynamic_lights())
	{
		light_render->set_active(light_on);
		light_omni->set_active(light_on);
	}
	glow_render->set_active(light_on);
}