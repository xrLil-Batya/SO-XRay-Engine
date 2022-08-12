#include "stdafx.h"
#include "HudItem.h"
#include "physic_item.h"
#include "actor.h"
#include "actoreffector.h"
#include "Missile.h"
#include "xrmessages.h"
#include "level.h"
#include "inventory.h"
#include "../xrEngine/CameraBase.h"
#include "player_hud.h"
#include "../xrEngine/SkeletonMotions.h"

#include "ui_base.h"
#include "ui\UIScriptWnd.h"

#include "script_callback_ex.h"
#include "script_game_object.h"
#include "clsid_game.h"
#include "weaponpistol.h"
#include "HUDManager.h"
#include "CustomDevice.h"

ENGINE_API extern float psHUD_FOV_def;

CHudItem::CHudItem()
{
	RenderHud					(TRUE);
	EnableHudInertion			(TRUE);
	AllowHudInertion			(TRUE);
	m_bStopAtEndAnimIsRunning	= false;
	m_current_motion_def		= NULL;
	m_started_rnd_anim_idx		= u8(-1);

	m_fLR_CameraFactor = 0.f;
	m_fLR_MovingFactor = 0.f;
	m_fLR_InertiaFactor = 0.f;
	m_fUD_InertiaFactor = 0.f;
	
	m_nearwall_last_hud_fov = psHUD_FOV_def;
}

DLL_Pure *CHudItem::_construct	()
{
	m_object			= smart_cast<CPhysicItem*>(this);
	VERIFY				(m_object);

	m_item				= smart_cast<CInventoryItem*>(this);
	VERIFY				(m_item);

	return				(m_object);
}

CHudItem::~CHudItem()
{
}

void CHudItem::Load(LPCSTR section)
{
	hud_sect				= pSettings->r_string		(section,"hud");
	m_animation_slot		= pSettings->r_u32			(section,"animation_slot");

	m_sounds.LoadSound(section, "snd_bore", "sndBore", true);
	
	m_hud_fov_add_mod = READ_IF_EXISTS(pSettings, r_float, section, "hud_fov_addition_modifier", 0.f);
	m_nearwall_dist_min = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_min", .2f);
	m_nearwall_dist_max = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_max", 1.f);
	m_nearwall_target_hud_fov = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_target_hud_fov", 0.27f);
	m_nearwall_speed_mod = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_speed_mod", 10.f);
	m_base_fov = READ_IF_EXISTS(pSettings, r_float, section, "hud_fov", 0.f);
}


void CHudItem::PlaySound(LPCSTR alias, const Fvector& position)
{
	m_sounds.PlaySound	(alias, position, object().H_Root(), !!GetHUDmode());
}

void CHudItem::renderable_Render()
{
	UpdateXForm					();
	BOOL _hud_render			= ::Render->get_HUD() && GetHUDmode();
	
	if(_hud_render  && !IsHidden())
	{ 
	}
	else 
	{
		if (!object().H_Parent() || (!_hud_render && !IsHidden()))
		{
			on_renderable_Render		();
			debug_draw_firedeps			();
		}else
		if (object().H_Parent()) 
		{
			CInventoryOwner	*owner = smart_cast<CInventoryOwner*>(object().H_Parent());
			VERIFY			(owner);
			CInventoryItem	*self = smart_cast<CInventoryItem*>(this);
			if (owner->attached(self))
				on_renderable_Render();
		}
	}
}

void CHudItem::SwitchState(u32 S)
{
	if (OnClient()) 
		return;

	SetNextState( S );

	if (object().Local() && !object().getDestroy())	
	{
		// !!! Just single entry for given state !!!
		NET_Packet				P;
		object().u_EventGen		(P,GE_WPN_STATE_CHANGE,object().ID());
		P.w_u8					(u8(S));
		object().u_EventSend	(P);
	}
}

void CHudItem::OnEvent(NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_WPN_STATE_CHANGE:
		{
			u8				S;
			P.r_u8			(S);
			OnStateSwitch	(u32(S), GetState());
		}
		break;
	}
}

void CHudItem::OnStateSwitch(u32 S, u32 oldState)
{
	SetState			(S);
	
	if(object().Remote()) 
		SetNextState	(S);

	switch (S)
	{
	case eBore:
		SetPending		(FALSE);

		PlayAnimBore	();
		if(HudItemData())
		{
			Fvector P		= HudItemData()->m_item_transform.c;
			m_sounds.PlaySound("sndBore", P, object().H_Root(), !!GetHUDmode(), false, m_started_rnd_anim_idx);
		}

		break;
	}
}

void CHudItem::OnAnimationEnd(u32 state)
{
	switch(state)
	{
	case eBore:
		{
			SwitchState	(eIdle);
		} break;
	}
}

void CHudItem::PlayAnimBore()
{
	PlayHUDMotion	("anm_bore", TRUE, this, GetState());
}

bool CHudItem::ActivateItem() 
{
	OnActiveItem	();
	return			true;
}

void CHudItem::DeactivateItem() 
{
	OnHiddenItem	();
}
void CHudItem::OnMoveToRuck(const SInvItemPlace& prev)
{
	SwitchState(eHidden);
}

void CHudItem::SendDeactivateItem	()
{
	SendHiddenItem	();
}
void CHudItem::SendHiddenItem()
{
	if (!object().getDestroy())
	{
		NET_Packet				P;
		object().u_EventGen		(P,GE_WPN_STATE_CHANGE,object().ID());
		P.w_u8					(u8(eHiding));
		object().u_EventSend	(P, net_flags(TRUE, TRUE, FALSE, TRUE));
	}
}


void CHudItem::UpdateHudAdditional(Fmatrix& trans)
{
	CActor* pActor = smart_cast<CActor*>(object().H_Parent());
	if (!pActor)
		return;

	attachable_hud_item* hi = HudItemData();
	R_ASSERT(hi);

	if (!g_player_hud->inertion_allowed())
		return;

	static float fAvgTimeDelta = Device.fTimeDelta;
	fAvgTimeDelta = _inertion(fAvgTimeDelta, Device.fTimeDelta, 0.8f);

	float fYMag = pActor->fFPCamYawMagnitude;
	float fPMag = pActor->fFPCamPitchMagnitude;

	float fStrafeMaxTime = hi->m_measures.m_strafe_offset[2][0].y;
	// ����. ����� � ��������, �� ������� �� ���������� �� ������������ ���������
	if (fStrafeMaxTime <= EPS)
		fStrafeMaxTime = 0.01f;

	float fStepPerUpd = fAvgTimeDelta / fStrafeMaxTime; // �������� ��������� ������� ��������

														// ��������� ������� ������ �� �������� ������
	float fCamReturnSpeedMod = 1.5f;
	// ��������� �������� ������������ �������, ����������� �� �������� ������ (������ �� �����)

	// ����������� ����������� �������� �������� ������ ��� ������ �������
	float fStrafeMinAngle = hi->m_measures.m_strafe_offset[3][0].y;

	// ����������� ����������� ������ �� �������� ������
	float fCamLimitBlend = hi->m_measures.m_strafe_offset[3][0].x;

	// ������� ������ �� �������� ������
	if (abs(fYMag) > (m_fLR_CameraFactor == 0.0f ? fStrafeMinAngle : 0.0f))
	{
		//--> ������ �������� �� ��� Y
		m_fLR_CameraFactor -= (fYMag * fAvgTimeDelta * 0.75f);
		clamp(m_fLR_CameraFactor, -fCamLimitBlend, fCamLimitBlend);
	}
	else
	{
		//--> ������ �� �������������� - ������� ������
		if (m_fLR_CameraFactor < 0.0f)
		{
			m_fLR_CameraFactor += fStepPerUpd * fCamReturnSpeedMod;
			clamp(m_fLR_CameraFactor, -fCamLimitBlend, 0.0f);
		}
		else
		{
			m_fLR_CameraFactor -= fStepPerUpd * fCamReturnSpeedMod;
			clamp(m_fLR_CameraFactor, 0.0f, fCamLimitBlend);
		}
	}

	// ��������� ������� ������ �� ������ ����
	float fChangeDirSpeedMod = 3;
	// ��������� ������ ������ ����������� ����������� �������, ���� ��� � ������ ������� �� ��������
	u32 iMovingState = pActor->MovingState();
	if ((iMovingState & mcLStrafe) != 0)
	{
		// �������� �����
		float fVal = (m_fLR_MovingFactor > 0.f ? fStepPerUpd * fChangeDirSpeedMod : fStepPerUpd);
		m_fLR_MovingFactor -= fVal;
	}
	else if ((iMovingState & mcRStrafe) != 0)
	{
		// �������� ������
		float fVal = (m_fLR_MovingFactor < 0.f ? fStepPerUpd * fChangeDirSpeedMod : fStepPerUpd);
		m_fLR_MovingFactor += fVal;
	}
	else
	{
		// ��������� � ����� ������ ����������� - ������ ������� ������
		if (m_fLR_MovingFactor < 0.0f)
		{
			m_fLR_MovingFactor += fStepPerUpd;
			clamp(m_fLR_MovingFactor, -1.0f, 0.0f);
		}
		else
		{
			m_fLR_MovingFactor -= fStepPerUpd;
			clamp(m_fLR_MovingFactor, 0.0f, 1.0f);
		}
	}
	clamp(m_fLR_MovingFactor, -1.0f, 1.0f); // ������ ������� ������ �� ������ ��������� ��� ������

											// ��������� � ������������� �������� ������ �������
	float fLR_Factor = m_fLR_MovingFactor;

	// No cam strafe inertia while in freelook mode
	if (pActor->cam_freelook == eflDisabled)
		fLR_Factor += m_fLR_CameraFactor;

	clamp(fLR_Factor, -1.0f, 1.0f); // ������ ������� ������ �� ������ ��������� ��� ������

	Fvector curr_offs, curr_rot;
	Fmatrix hud_rotation;
	Fmatrix hud_rotation_y;

	if ((hi->m_measures.m_strafe_offset[2][0].x != 0.0f))
	{
		// �������� ������� ���� � �������
		curr_offs = hi->m_measures.m_strafe_offset[0][0]; // pos
		curr_offs.mul(fLR_Factor); // �������� �� ������ �������

								   // ������� ���� � �������
		curr_rot = hi->m_measures.m_strafe_offset[1][0]; // rot
		curr_rot.mul(-PI / 180.f); // ����������� ���� � �������
		curr_rot.mul(fLR_Factor); // �������� �� ������ �������

		hud_rotation.identity();
		hud_rotation.rotateX(curr_rot.x);

		hud_rotation_y.identity();
		hud_rotation_y.rotateY(curr_rot.y);
		hud_rotation.mulA_43(hud_rotation_y);

		hud_rotation_y.identity();
		hud_rotation_y.rotateZ(curr_rot.z);
		hud_rotation.mulA_43(hud_rotation_y);

		hud_rotation.translate_over(curr_offs);
		trans.mulB_43(hud_rotation);
	}

	//============= ������� ������ =============//
	// ��������� �������
	float fInertiaSpeedMod = hi->m_measures.m_inertion_params.m_tendto_speed;

	float fInertiaReturnSpeedMod = hi->m_measures.m_inertion_params.m_tendto_ret_speed;

	float fInertiaMinAngle = hi->m_measures.m_inertion_params.m_min_angle;

	Fvector4 vIOffsets; // x = L, y = R, z = U, w = D
	vIOffsets.x = hi->m_measures.m_inertion_params.m_offset_LRUD.x;
	vIOffsets.y = hi->m_measures.m_inertion_params.m_offset_LRUD.y;
	vIOffsets.z = hi->m_measures.m_inertion_params.m_offset_LRUD.z;
	vIOffsets.w = hi->m_measures.m_inertion_params.m_offset_LRUD.w;

	// ����������� ������� �� ��������� ������
	bool bIsInertionPresent = m_fLR_InertiaFactor != 0.0f || m_fUD_InertiaFactor != 0.0f;
	if (abs(fYMag) > fInertiaMinAngle || bIsInertionPresent)
	{
		float fSpeed = fInertiaSpeedMod;
		if (fYMag > 0.0f && m_fLR_InertiaFactor > 0.0f ||
			fYMag < 0.0f && m_fLR_InertiaFactor < 0.0f)
		{
			fSpeed *= 2.f; //--> �������� ������� ��� �������� � ��������������� �������
		}

		m_fLR_InertiaFactor -= (fYMag * fAvgTimeDelta * fSpeed); // ����������� (�.�. > |1.0|)
	}

	if (abs(fPMag) > fInertiaMinAngle || bIsInertionPresent)
	{
		float fSpeed = fInertiaSpeedMod;
		if (fPMag > 0.0f && m_fUD_InertiaFactor > 0.0f ||
			fPMag < 0.0f && m_fUD_InertiaFactor < 0.0f)
		{
			fSpeed *= 2.f; //--> �������� ������� ��� �������� � ��������������� �������
		}

		m_fUD_InertiaFactor -= (fPMag * fAvgTimeDelta * fSpeed); // ��������� (�.�. > |1.0|)
	}

	clamp(m_fLR_InertiaFactor, -1.0f, 1.0f);
	clamp(m_fUD_InertiaFactor, -1.0f, 1.0f);

	// ������� ��������� ������� (��������, �� ��� �������� ������� �� ������� ������� �� ������� 0.0f)
	m_fLR_InertiaFactor *= clampr(1.f - fAvgTimeDelta * fInertiaReturnSpeedMod, 0.0f, 1.0f);
	m_fUD_InertiaFactor *= clampr(1.f - fAvgTimeDelta * fInertiaReturnSpeedMod, 0.0f, 1.0f);

	// ����������� �������� ��������� ������� ��� ����� (�����������)
	if (fYMag == 0.0f)
	{
		float fRetSpeedMod = (fYMag == 0.0f ? 1.0f : 0.75f) * (fInertiaReturnSpeedMod * 0.075f);
		if (m_fLR_InertiaFactor < 0.0f)
		{
			m_fLR_InertiaFactor += fAvgTimeDelta * fRetSpeedMod;
			clamp(m_fLR_InertiaFactor, -1.0f, 0.0f);
		}
		else
		{
			m_fLR_InertiaFactor -= fAvgTimeDelta * fRetSpeedMod;
			clamp(m_fLR_InertiaFactor, 0.0f, 1.0f);
		}
	}

	// ����������� �������� ��������� ������� ��� ����� (���������)
	if (fPMag == 0.0f)
	{
		float fRetSpeedMod = (fPMag == 0.0f ? 1.0f : 0.75f) * (fInertiaReturnSpeedMod * 0.075f);
		if (m_fUD_InertiaFactor < 0.0f)
		{
			m_fUD_InertiaFactor += fAvgTimeDelta * fRetSpeedMod;
			clamp(m_fUD_InertiaFactor, -1.0f, 0.0f);
		}
		else
		{
			m_fUD_InertiaFactor -= fAvgTimeDelta * fRetSpeedMod;
			clamp(m_fUD_InertiaFactor, 0.0f, 1.0f);
		}
	}

	// ��������� ������� � ����
	float fLR_lim = (m_fLR_InertiaFactor < 0.0f ? vIOffsets.x : vIOffsets.y);
	float fUD_lim = (m_fUD_InertiaFactor < 0.0f ? vIOffsets.z : vIOffsets.w);
	Fvector fv = { fLR_lim * -1.f * m_fLR_InertiaFactor, fUD_lim * m_fUD_InertiaFactor, 0.0f };
	curr_offs = fv;

	hud_rotation.identity();
	hud_rotation.translate_over(curr_offs);
	trans.mulB_43(hud_rotation);
}

void CHudItem::UpdateCL()
{
	if(m_current_motion_def)
	{
		if(m_bStopAtEndAnimIsRunning)
		{
			const xr_vector<motion_marks>&	marks = m_current_motion_def->marks;
			if(!marks.empty())
			{
				float motion_prev_time = ((float)m_dwMotionCurrTm - (float)m_dwMotionStartTm)/1000.0f;
				float motion_curr_time = ((float)Device.dwTimeGlobal - (float)m_dwMotionStartTm)/1000.0f;
				
				xr_vector<motion_marks>::const_iterator it = marks.begin();
				xr_vector<motion_marks>::const_iterator it_e = marks.end();
				for(;it!=it_e;++it)
				{
					const motion_marks&	M = (*it);
					if(M.is_empty())
						continue;
	
					const motion_marks::interval* Iprev = M.pick_mark(motion_prev_time);
					const motion_marks::interval* Icurr = M.pick_mark(motion_curr_time);
					if(Iprev==NULL && Icurr!=NULL /* || M.is_mark_between(motion_prev_time, motion_curr_time)*/)
					{
						OnMotionMark				(m_startedMotionState, M);
					}
				}
			
			}

			m_dwMotionCurrTm					= Device.dwTimeGlobal;
			if(m_dwMotionCurrTm > m_dwMotionEndTm)
			{
				m_current_motion_def				= NULL;
				m_dwMotionStartTm					= 0;
				m_dwMotionEndTm						= 0;
				m_dwMotionCurrTm					= 0;
				m_bStopAtEndAnimIsRunning			= false;
				OnAnimationEnd						(m_startedMotionState);
			}
		}
	}
}

void CHudItem::OnH_A_Chield		()
{}

void CHudItem::OnH_B_Chield		()
{
	StopCurrentAnimWithoutCallback();
}

void CHudItem::OnH_B_Independent	(bool just_before_destroy)
{
	m_sounds.StopAllSounds	();
	UpdateXForm				();
	m_nearwall_last_hud_fov = psHUD_FOV_def;
	
	// next code was commented 
	/*
	if(HudItemData() && !just_before_destroy)
	{
		object().XFORM().set( HudItemData()->m_item_transform );
	}
	
	if (HudItemData())
	{
		g_player_hud->detach_item(this);
		Msg("---Detaching hud item [%s][%d]", this->HudSection().c_str(), this->object().ID());
	}*/
	//SetHudItemData			(NULL);
}

void CHudItem::OnH_A_Independent	()
{
	if(HudItemData())
		g_player_hud->detach_item(this);
	StopCurrentAnimWithoutCallback();
}

void CHudItem::on_b_hud_detach()
{
	m_sounds.StopAllSounds	();
}

void CHudItem::on_a_hud_attach()
{
	if(m_current_motion_def)
	{
		PlayHUDMotion_noCB(m_current_motion, FALSE);
#ifdef DEBUG
//		Msg("continue playing [%s][%d]",m_current_motion.c_str(), Device.dwFrame);
#endif // #ifdef DEBUG
	}else
	{
#ifdef DEBUG
//		Msg("no active motion");
#endif // #ifdef DEBUG
	}
}

u32 CHudItem::PlayHUDMotion(const shared_str& M, BOOL bMixIn, CHudItem* W, u32 state, float speed, float end, bool bMixIn2)
{
	u32 anim_time = PlayHUDMotion_noCB(M, bMixIn, speed, bMixIn2);
	if (anim_time > 0)
	{
		m_bStopAtEndAnimIsRunning = true;
		m_dwMotionStartTm = Device.dwTimeGlobal;
		m_dwMotionCurrTm = m_dwMotionStartTm;
		m_dwMotionEndTm = m_dwMotionStartTm + anim_time;
		m_startedMotionState = state;

		float end_modifier = 0.f;

		if (HudItemData())
		{
			player_hud_motion* anm = HudItemData()->find_motion(M);
			end_modifier = anm->m_anim_end;
		}

		if (end_modifier == 0.f)
			end_modifier = end;

		m_dwMotionEndTm -= end_modifier * 1000;
	}
	else
		m_bStopAtEndAnimIsRunning = false;

	return anim_time;
}


u32 CHudItem::PlayHUDMotion_noCB(const shared_str& motion_name, BOOL bMixIn, float speed, bool bMixIn2)
{
	m_current_motion = motion_name;

	if (bDebug && item().m_pInventory)
	{
		Msg("-[%s] as[%d] [%d]anim_play [%s][%d]",
		    HudItemData() ? "HUD" : "Simulating",
		    item().m_pInventory->GetActiveSlot(),
		    item().object_id(),
		    motion_name.c_str(),
		    Device.dwFrame);
	}
	if (HudItemData())
	{
		return HudItemData()->anim_play(motion_name, bMixIn, m_current_motion_def, m_started_rnd_anim_idx, speed, bMixIn2);
	}
	else
	{
		m_started_rnd_anim_idx = 0;
		return g_player_hud->motion_length(motion_name, HudSection(), m_current_motion_def);
	}
}

void CHudItem::StopCurrentAnimWithoutCallback()
{
	m_dwMotionStartTm			= 0;
	m_dwMotionEndTm				= 0;
	m_dwMotionCurrTm			= 0;
	m_bStopAtEndAnimIsRunning	= false;
	m_current_motion_def		= NULL;
}

BOOL CHudItem::GetHUDmode()
{
	if(object().H_Parent())
	{
		CActor* A = smart_cast<CActor*>(object().H_Parent());
		return ( A && A->HUDview() && HudItemData() && HudItemData() );
	}else
		return FALSE;
}

void CHudItem::PlayBlendAnm(LPCSTR name, float speed, float power, bool stop_old)
{
	u8 part = (object().cast_weapon()->IsZoomed() ? 2 : (g_player_hud->attached_item(1) ? 0 : 2));

	if (stop_old) g_player_hud->StopBlendAnm(name, true);
	g_player_hud->PlayBlendAnm(name, part, speed, power, false);
}

void CHudItem::PlayAnimIdle()
{
	if (TryPlayAnimIdle()) return;

	PlayHUDMotion("anm_idle", TRUE, NULL, GetState());
}

bool CHudItem::TryPlayAnimIdle()
{
	if (MovingAnimAllowedNow())
	{
		CActor* pActor = smart_cast<CActor*>(object().H_Parent());
		if (pActor && pActor->AnyMove())
		{
			if (pActor->is_safemode() && !smart_cast<CCustomDevice*>(this)) return false;

			CEntity::SEntityState st;
			pActor->g_State(st);
			if (st.bSprint)
			{
				PlayAnimIdleSprint();
				return true;
			}
			else if (!st.bCrouch)
			{
				PlayAnimIdleMoving();
				return true;
			}
			//AVO: new crouch idle animation
			else if (st.bCrouch)
			{
				if (!PlayAnimCrouchIdleMoving())
					PlayHUDMotion("anm_idle_moving", TRUE, NULL, GetState(), .7f);
				return true;
			}
		}
	}

	return false;
}

//AVO: check if animation exists
bool CHudItem::HudAnimationExist(LPCSTR anim_name)
{
	if (HudItemData()) // First person
	{
		string256 anim_name_r;
		bool is_16x9 = (float)Device.dwWidth/(float)Device.dwHeight > 1.34;
		u16 attach_place_idx = pSettings->r_u16(HudItemData()->m_sect_name, "attach_place_idx");
		xr_sprintf(anim_name_r, "%s%s", anim_name, ((attach_place_idx == 1) && is_16x9) ? "_16x9" : "");
		player_hud_motion* anm = HudItemData()->m_hand_motions.find_motion(anim_name_r);
		if (anm)
			return true;

		anm = HudItemData()->m_hand_motions.find_motion(anim_name);
		if (anm)
			return true;
	}
	else // Third person
	{
		if (g_player_hud->motion_length(anim_name, HudSection(), m_current_motion_def) > 100)
			return true;
	}
#ifdef DEBUG
    Msg("~ [WARNING] ------ Animation [%s] does not exist in [%s]", anim_name, HudSection().c_str());
#endif
	return false;
}

//-AVO

//AVO: new crouch idle animation
bool CHudItem::PlayAnimCrouchIdleMoving()
{
	if (HudAnimationExist("anm_idle_moving_crouch"))
	{
		PlayHUDMotion("anm_idle_moving_crouch", TRUE, NULL, GetState());
		return true;
	}
	return false;
}

//-AVO

bool CHudItem::NeedBlendAnm() 
{
	u32 state = GetState();
	return (state != eIdle && state != eHidden);
}

void CHudItem::PlayAnimIdleMoving()
{
	PlayHUDMotion("anm_idle_moving", TRUE, NULL, GetState());
}

void CHudItem::PlayAnimIdleSprint()
{
	PlayHUDMotion("anm_idle_sprint", TRUE, NULL,GetState());
}

void CHudItem::OnMovementChanged(ACTOR_DEFS::EMoveCommand cmd)
{
	if(GetState()==eIdle && !m_bStopAtEndAnimIsRunning)
	{
		if( (cmd == ACTOR_DEFS::mcSprint) || (cmd == ACTOR_DEFS::mcAnyMove)  )
		{
			PlayAnimIdle						();
			ResetSubStateTime					();
		}
	}
}

attachable_hud_item* CHudItem::HudItemData()
{
	attachable_hud_item* hi = NULL;
	if(!g_player_hud)		
		return				hi;

	hi = g_player_hud->attached_item(0);
	if (hi && hi->m_parent_hud_item == this)
		return hi;

	hi = g_player_hud->attached_item(1);
	if (hi && hi->m_parent_hud_item == this)
		return hi;

	return NULL;
}

bool CHudItem::HParentIsActor()
{
	CObject* O = object().H_Parent();
	if (!O)
		return false;

	CEntityAlive* EA = smart_cast<CEntityAlive*>(O);
	if (!EA)
		return false;

	return !!EA->cast_actor();
}

collide::rq_result& CHudItem::GetRQ()
{ 
	return HUD().GetCurrentRayQuery(); 
}

float CHudItem::GetHudFov()
{
	if (HParentIsActor() && Level().CurrentViewEntity() == object().H_Parent())
	{
		float dist = GetRQ().range;

		clamp(dist, m_nearwall_dist_min, m_nearwall_dist_max);
		float fDistanceMod = ((dist - m_nearwall_dist_min) / (m_nearwall_dist_max - m_nearwall_dist_min));
		// 0.f ... 1.f
		float fBaseFov = (m_base_fov ? m_base_fov : psHUD_FOV_def) + m_hud_fov_add_mod;
		clamp(fBaseFov, 0.0f, 1.f);
		float src = m_nearwall_speed_mod * Device.fTimeDelta;
		clamp(src, 0.f, 1.f);

		float fTrgFov = m_nearwall_target_hud_fov + fDistanceMod * (fBaseFov - m_nearwall_target_hud_fov);
		m_nearwall_last_hud_fov = m_nearwall_last_hud_fov * (1 - src) + fTrgFov * src;
	}

	return m_nearwall_last_hud_fov;
}
