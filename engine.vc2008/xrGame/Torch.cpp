#include "stdafx.h"
#include "torch.h"
#include "entity.h"
#include "actor.h"
#include "../xrEngine/LightAnimLibrary.h"
#include "../xrphysics/PhysicsShell.h"
#include "xrserver_objects_alife_items.h"
#include "ai_sounds.h"

#include "level.h"
#include "../Include/xrRender/Kinematics.h"
#include "../xrEngine/camerabase.h"
#include "../xrengine/xr_collide_form.h"
#include "inventory.h"
#include "game_base_space.h"

#include "UIGameCustom.h"
#include "actorEffector.h"
#include "CustomOutfit.h"
#include "ActorHelmet.h"

static const float TORCH_INERTION_CLAMP = PI_DIV_6;
static const float TORCH_INERTION_SPEED_MAX = 7.5f;
static const float TORCH_INERTION_SPEED_MIN = 0.5f;
static Fvector TORCH_OFFSET = {-0.2f, +0.1f, -0.3f};
static const Fvector OMNI_OFFSET = {-0.2f, +0.1f, -0.1f};
static const float OPTIMIZATION_DISTANCE = 100.f;

static bool stalker_use_dynamic_lights	= false;

ENGINE_API int g_current_renderer;

CTorch::CTorch(void) 
{
	light_render = ::Render->light_create();
	light_render->set_type(IRender_Light::SPOT);
	light_render->set_shadow(true);
	light_omni = ::Render->light_create();
	light_omni->set_type(IRender_Light::POINT);
	light_omni->set_shadow(false);

	m_switched_on = false;
	glow_render = ::Render->glow_create();
	lanim = 0;
	fBrightness = 1.f;

	m_prev_hp.set(0,0);
	m_delta_h = 0;
	m_night_vision = NULL;

	//ARTLantist
	m_torch_mod = false;
	m_torch_offset = TORCH_OFFSET;
    m_omni_offset = OMNI_OFFSET;
    m_torch_inertion_speed_max = TORCH_INERTION_SPEED_MAX;
    m_torch_inertion_speed_min = TORCH_INERTION_SPEED_MIN;
    m_light_section = "torch_definition";
	//torch2 for flashlight
	light2_render = ::Render->light_create();
	light2_render->set_type(IRender_Light::SPOT);
	light2_render->set_shadow(true);
	light2_omni = ::Render->light_create();
	light2_omni->set_type(IRender_Light::POINT);
	light2_omni->set_shadow(false);
	
	lanim2 = 0;
	fBrightness2 = 1.f;

	m_switched2_on = false;
	glow2_render = ::Render->glow_create();
	
	m_torch2_offset = TORCH_OFFSET;
    m_omni2_offset = OMNI_OFFSET;
    m_torch2_inertion_speed_max = TORCH_INERTION_SPEED_MAX;
    m_torch2_inertion_speed_min = TORCH_INERTION_SPEED_MIN;
    m_light2_section = "glowstick_definition";
	//ARTLantist End
}

CTorch::~CTorch() 
{
	light_render.destroy();
	light_omni.destroy();
	glow_render.destroy();
	//ARTLantist: torch2 for flashlight
	light2_render.destroy();
	light2_omni.destroy();
	glow2_render.destroy();
	//ARTLantist End
	xr_delete(m_night_vision);
}

inline bool CTorch::can_use_dynamic_lights	()
{
	if (!H_Parent())
		return				(true);

	CInventoryOwner			*owner = smart_cast<CInventoryOwner*>(H_Parent());
	if (!owner)
		return				(true);

	return					(owner->can_use_dynamic_lights());
}

void CTorch::Load(LPCSTR section) 
{
	inherited::Load			(section);
	light_trace_bone		= pSettings->r_string(section,"light_trace_bone");
	//ARTLantist
    m_light_section = READ_IF_EXISTS(pSettings, r_string, section, "light_section", "torch_definition");
	if (pSettings->line_exist(section, "snd_turn_on"))
		m_sounds.LoadSound(section, "snd_turn_on", "sndTurnOn", false, SOUND_TYPE_ITEM_USING);
	if (pSettings->line_exist(section, "snd_turn_off"))
		m_sounds.LoadSound(section, "snd_turn_off", "sndTurnOff", false, SOUND_TYPE_ITEM_USING);
    m_torch_offset = READ_IF_EXISTS(pSettings, r_fvector3, section, "torch_offset", TORCH_OFFSET);
    m_omni_offset = READ_IF_EXISTS(pSettings, r_fvector3, section, "omni_offset", OMNI_OFFSET);
    m_torch_inertion_speed_max = READ_IF_EXISTS(pSettings, r_float, section, "torch_inertion_speed_max", TORCH_INERTION_SPEED_MAX);
    m_torch_inertion_speed_min = READ_IF_EXISTS(pSettings, r_float, section, "torch_inertion_speed_min", TORCH_INERTION_SPEED_MIN);

	// Disabling shift by x and z axes for 1st render,
    // because we don't have dynamic lighting in it.
	if( g_current_renderer == 1 )
    {
        m_torch_offset.x = 0;
        m_torch_offset.z = 0;
    }
	//ARTLantist: torch2 for flashlight
    m_light2_section = READ_IF_EXISTS(pSettings, r_string, section, "light2_section", "glowstick_definition");
	
    m_torch2_offset = READ_IF_EXISTS(pSettings, r_fvector3, section, "torch2_offset", TORCH_OFFSET);
    m_omni2_offset = READ_IF_EXISTS(pSettings, r_fvector3, section, "omni2_offset", OMNI_OFFSET);
    m_torch2_inertion_speed_max = READ_IF_EXISTS(pSettings, r_float, section, "torch2_inertion_speed_max", TORCH_INERTION_SPEED_MAX);
    m_torch2_inertion_speed_min = READ_IF_EXISTS(pSettings, r_float, section, "torch2_inertion_speed_min", TORCH_INERTION_SPEED_MIN);
	
	// Disabling shift by x and z axes for 1st render,
    // because we don't have dynamic lighting in it.
	if( g_current_renderer == 1 )
    {
        m_torch2_offset.x = 0;
        m_torch2_offset.z = 0;
    }
	//ARTLantist End

	m_bNightVisionEnabled = !!pSettings->r_bool(section,"night_vision");
}

void CTorch::SwitchNightVision()
{
	if (OnClient()) return;
	SwitchNightVision(!m_bNightVisionOn);	
}

void CTorch::SwitchNightVision(bool vision_on, bool use_sounds)
{
	if(!m_bNightVisionEnabled) return;
	
	m_bNightVisionOn			= vision_on;

	CActor *pA = smart_cast<CActor *>(H_Parent());
	if(!pA)						
	{
		return;
	}
	if(!m_night_vision)
		m_night_vision			= xr_new<CNightVisionEffector>(cNameSect());


	LPCSTR disabled_names	= pSettings->r_string(cNameSect(),"disabled_maps");
	LPCSTR curr_map			= *Level().name();
	u32 cnt					= _GetItemCount(disabled_names);
	bool b_allow			= true;
	string512				tmp;
	for(u32 i=0; i<cnt;++i){
		_GetItem(disabled_names, i, tmp);
		if(0==stricmp(tmp, curr_map)){
			b_allow = false;
			break;
		}
	}

	CHelmet* pHelmet	= smart_cast<CHelmet*>(pA->inventory().ItemFromSlot(HELMET_SLOT));
	CCustomOutfit* pOutfit	= smart_cast<CCustomOutfit*>(pA->inventory().ItemFromSlot(OUTFIT_SLOT));

	if(pHelmet && pHelmet->m_NightVisionSect.size() && !b_allow)
	{
		m_night_vision->OnDisabled(pA, use_sounds);
		return;
	}
	else if(pOutfit && pOutfit->m_NightVisionSect.size() && !b_allow)
	{
		m_night_vision->OnDisabled(pA, use_sounds);
		return;
	}

	bool bIsActiveNow = m_night_vision->IsActive();

	if(m_bNightVisionOn)
	{

		if(!bIsActiveNow)
		{
			if(pHelmet && pHelmet->m_NightVisionSect.size())
			{
				m_night_vision->Start(pHelmet->m_NightVisionSect, pA, use_sounds);
				return;
			}
			else if(pOutfit && pOutfit->m_NightVisionSect.size())
			{
				m_night_vision->Start(pOutfit->m_NightVisionSect, pA, use_sounds);
				return;
			}
			m_bNightVisionOn = false; // in case if there is no nightvision in helmet and outfit
		}
	}else
	{
		if(bIsActiveNow)
		{
			m_night_vision->Stop(100000.0f, use_sounds);
		}
	}
}

void CTorch::Switch()
{
	if (OnClient()) return;
	bool bActive = !m_switched_on;
	Switch(bActive);
}

void CTorch::Switch(bool light_on)
{
	CActor* pActor = smart_cast<CActor*>(H_Parent());
	if (pActor)
	{
		if (light_on && !m_switched_on) {
			if (m_sounds.FindSoundItem("SndTurnOn", false))
				m_sounds.PlaySound("SndTurnOn", pActor->Position(), NULL, !!pActor->HUDview());
		}
		else if (!light_on && m_switched_on) {
			if (m_sounds.FindSoundItem("SndTurnOff", false))
				m_sounds.PlaySound("SndTurnOff", pActor->Position(), NULL, !!pActor->HUDview());
		}
	}
	m_switched_on = light_on;
	if (can_use_dynamic_lights()) {
		light_render->set_active(light_on);
		light_omni->set_active(light_on);
	}
	glow_render->set_active(light_on);

	if (*light_trace_bone) {
		IKinematics* pVisual = smart_cast<IKinematics*>(Visual()); VERIFY(pVisual);
		u16 bi = pVisual->LL_BoneID(light_trace_bone);
		pVisual->LL_SetBoneVisible(bi, light_on, TRUE);
		pVisual->CalculateBones(TRUE);
	}
}
bool CTorch::torch_active() const { return (m_switched_on); }

//ARTLantist
void CTorch::SetTorchMod()
{
	if (OnClient()) return;
	if (!m_switched_on) return;
	bool b_r2 = !!psDeviceFlags.test(rsR2); 
	if (m_torch_mod) {
		m_torch_mod = false;
		light_render->set_range(11.f);
		light_render->set_cone(deg2rad(75.f));
	}
	else {
		m_torch_mod = true;
		light_render->set_range(26.f);
		light_render->set_cone(deg2rad(45.f));
	}
}
//ARTLantist: torch2 for flashlight
void CTorch::Switch2(bool light_on)
{
	m_switched2_on = light_on;
	if (can_use_dynamic_lights()) {
		light2_render->set_active(light_on);
		light2_omni->set_active(light_on);
	}
	glow2_render->set_active(light_on);

	if (*light_trace_bone) {
		IKinematics* pVisual = smart_cast<IKinematics*>(Visual()); VERIFY(pVisual);
		u16 bi = pVisual->LL_BoneID(light_trace_bone);
		pVisual->LL_SetBoneVisible(bi, light_on, TRUE);
		pVisual->CalculateBones(TRUE);
	}
}
bool CTorch::torch2_active() const { return (m_switched2_on); }
//ARTLantist End

BOOL CTorch::net_Spawn(CSE_Abstract* DC) 
{
	CSE_Abstract *e	= (CSE_Abstract*)(DC);
	CSE_ALifeItemTorch *torch = smart_cast<CSE_ALifeItemTorch*>(e);
	R_ASSERT(torch);
	cNameVisual_set(torch->get_visual());

	R_ASSERT(!CFORM());
	R_ASSERT(smart_cast<IKinematics*>(Visual()));
	collidable.model = xr_new<CCF_Skeleton>	(this);

	if (!inherited::net_Spawn(DC))
		return(FALSE);
	
	bool b_r2 = !!psDeviceFlags.test(rsR2);
	b_r2 |= !!psDeviceFlags.test(rsR3);

	IKinematics* K	 = smart_cast<IKinematics*>(Visual());
	CInifile* pUserData = K->LL_UserData(); 
	R_ASSERT3(pUserData,"Empty Torch user data!",torch->get_visual());

    R_ASSERT2(pUserData->section_exist(m_light_section), "Section not found in torch user data! Check 'light_section' field in config");
    R_ASSERT2(pUserData->section_exist(m_light2_section), "Section not found in torch user data! Check 'light_section' field in config");

	lanim = LALib.FindItem(pUserData->r_string(m_light_section,"color_animator"));
	guid_bone = K->LL_BoneID(pUserData->r_string(m_light_section,"guide_bone"));
	VERIFY(guid_bone!=BI_NONE);

	m_torch_color = pUserData->r_fcolor(m_light_section,(b_r2)?"color_r2":"color");
	fBrightness = m_torch_color.intensity();
	float range = pUserData->r_float(m_light_section,(b_r2)?"range_r2":"range");
	light_render->set_color(m_torch_color);
	light_render->set_range(range);

	m_omny_color = pUserData->r_fcolor(m_light_section,(b_r2)?"omni_color_r2":"omni_color");
	float range_o = pUserData->r_float(m_light_section,(b_r2)?"omni_range_r2":"omni_range");
	light_omni->set_color(m_omny_color);
	light_omni->set_range(range_o);

	light_render->set_cone(deg2rad(pUserData->r_float(m_light_section,"spot_angle")));
	light_render->set_texture(pUserData->r_string(m_light_section,"spot_texture"));

	glow_render->set_texture(pUserData->r_string(m_light_section,"glow_texture"));
	glow_render->set_color(m_torch_color);
	glow_render->set_radius(pUserData->r_float(m_light_section,"glow_radius"));
	//ARTLantist
    light_render->set_volumetric_quality(READ_IF_EXISTS(pUserData, r_float, m_light_section, "volumetric_quality", 1.f));
    light_render->set_volumetric_intensity(READ_IF_EXISTS(pUserData, r_float, m_light_section, "volumetric_intensity", 1.f));
    light_render->set_volumetric_distance(READ_IF_EXISTS(pUserData, r_float, m_light_section, "volumetric_distance", 1.f));
    light_render->set_type((IRender_Light::LT)(READ_IF_EXISTS(pUserData, r_u8, m_light_section, "type", 2)));
    light_omni->set_type((IRender_Light::LT)(READ_IF_EXISTS(pUserData, r_u8, m_light_section, "omni_type", 1)));
	//ARTLantist: torch2 for flashlight
	lanim2 = LALib.FindItem(pUserData->r_string(m_light2_section,"color_animator"));

	m_torch2_color = pUserData->r_fcolor(m_light2_section,(b_r2)?"color_r2":"color");
	fBrightness2 = m_torch2_color.intensity();
	float range2 = pUserData->r_float(m_light2_section,(b_r2)?"range_r2":"range");
	light2_render->set_color(m_torch2_color);
	light2_render->set_range(range2);

	m_omny2_color = pUserData->r_fcolor(m_light2_section,(b_r2)?"omni_color_r2":"omni_color");
	float range_o2 = pUserData->r_float(m_light2_section,(b_r2)?"omni_range_r2":"omni_range");
	light2_omni->set_color(m_omny2_color);
	light2_omni->set_range(range_o2);

	light2_render->set_cone(deg2rad(pUserData->r_float(m_light2_section,"spot_angle")));
	light2_render->set_texture(pUserData->r_string(m_light2_section,"spot_texture"));

	glow2_render->set_texture(pUserData->r_string(m_light2_section,"glow_texture"));
	glow2_render->set_color(m_torch2_color);
	glow2_render->set_radius(pUserData->r_float(m_light2_section,"glow_radius"));
	
    light2_render->set_volumetric_quality(READ_IF_EXISTS(pUserData, r_float, m_light2_section, "volumetric_quality", 1.f));
    light2_render->set_volumetric_intensity(READ_IF_EXISTS(pUserData, r_float, m_light2_section, "volumetric_intensity", 1.f));
    light2_render->set_volumetric_distance(READ_IF_EXISTS(pUserData, r_float, m_light2_section, "volumetric_distance", 1.f));
    light2_render->set_type((IRender_Light::LT)(READ_IF_EXISTS(pUserData, r_u8, m_light2_section, "type", 2)));
    light2_omni->set_type((IRender_Light::LT)(READ_IF_EXISTS(pUserData, r_u8, m_light2_section, "omni_type", 1)));
	//ARTLantist End

	//включить/выключить фонарик
	Switch(torch->m_active);
	VERIFY(!torch->m_active || (torch->ID_Parent != 0xffff));
	
	if(torch->ID_Parent == 0)		
		SwitchNightVision(torch->m_nightvision_active, false);

	m_delta_h = PI_DIV_2-atan((range*0.5f)/_abs(TORCH_OFFSET.x));

	return(TRUE);
}

void CTorch::net_Destroy() 
{
	Switch(false);
	SwitchNightVision(false);

	inherited::net_Destroy();
}

void CTorch::OnH_A_Chield() 
{
	inherited::OnH_A_Chield();
	m_focus.set(Position());
}

void CTorch::OnH_B_Independent(bool just_before_destroy) 
{
	inherited::OnH_B_Independent(just_before_destroy);

	Switch(false);
	SwitchNightVision(false);

	m_sounds.StopAllSounds();
}

void CTorch::UpdateCL()
{
	inherited::UpdateCL();
	UpdateTorch();
	UpdateTorch2();
}

//ARTLantist
void CTorch::UpdateTorch()
{
	if (!m_switched_on) return;
	
	CBoneInstance &BI = smart_cast<IKinematics*>(Visual())->LL_GetBoneInstance(guid_bone);
	Fmatrix M;
	if (H_Parent()) 
	{
		CActor* actor = smart_cast<CActor*>(H_Parent());
		if (actor) smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones_Invalidate();

		if (H_Parent()->XFORM().c.distance_to_sqr(Device.vCameraPosition)<_sqr(OPTIMIZATION_DISTANCE) || GameID() != eGameIDSingle) {
			// near camera
			smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones();
			M.mul_43(XFORM(),BI.mTransform);
		} else {
			// approximately the same
			M = H_Parent()->XFORM();
			H_Parent()->Center(M.c);
			M.c.y += H_Parent()->Radius()*2.f/3.f;
		}
		if (actor) 
		{
			if (actor->active_cam() == eacLookAt)
            {
				m_prev_hp.x = angle_inertion_var(m_prev_hp.x, -actor->cam_Active()->yaw, m_torch_inertion_speed_min, m_torch_inertion_speed_max, TORCH_INERTION_CLAMP, Device.fTimeDelta);
				m_prev_hp.y = angle_inertion_var(m_prev_hp.y, -actor->cam_Active()->pitch, m_torch_inertion_speed_min, m_torch_inertion_speed_max, TORCH_INERTION_CLAMP, Device.fTimeDelta);
            }
            else
            {
				m_prev_hp.x = angle_inertion_var(m_prev_hp.x, -actor->cam_FirstEye()->yaw, m_torch_inertion_speed_min, m_torch_inertion_speed_max, TORCH_INERTION_CLAMP, Device.fTimeDelta);
				m_prev_hp.y = angle_inertion_var(m_prev_hp.y, -actor->cam_FirstEye()->pitch, m_torch_inertion_speed_min, m_torch_inertion_speed_max, TORCH_INERTION_CLAMP, Device.fTimeDelta);
            }
			Fvector dir,right,up;	
			dir.setHP(m_prev_hp.x+m_delta_h,m_prev_hp.y);
			Fvector::generate_orthonormal_basis_normalized(dir,up,right);

			if (true) {
				Fvector offset = M.c; 
				offset.mad(M.i,m_torch_offset.x);
				offset.mad(M.j,m_torch_offset.y);
				offset.mad(M.k,m_torch_offset.z);
				light_render->set_position(offset);
				if(true) {
					offset = M.c; 
					offset.mad(M.i,m_omni_offset.x);
					offset.mad(M.j,m_omni_offset.y);
					offset.mad(M.k,m_omni_offset.z);
					light_omni->set_position(offset);
				}
			}
			glow_render->set_position(M.c);
			if (true) {
				light_render->set_rotation(dir, right);
				if(true) {
					light_omni->set_rotation(dir, right);
				}
			}
			glow_render->set_direction(dir);
		}
		else 
		{
			if (can_use_dynamic_lights()) {
				light_render->set_position	(M.c);
				light_render->set_rotation	(M.k,M.i);

				Fvector offset				= M.c; 
				offset.mad					(M.i,m_omni_offset.x);
				offset.mad					(M.j,m_omni_offset.y);
				offset.mad					(M.k,m_omni_offset.z);
				light_omni->set_position	(M.c);
				light_omni->set_rotation	(M.k,M.i);
			} 

			glow_render->set_position	(M.c);
			glow_render->set_direction	(M.k);
		}
	}
	else 
	{
		if (getVisible() && m_pPhysicsShell) 
		{
			M.mul(XFORM(),BI.mTransform);
			m_switched_on = false;
			light_render->set_active(false);
			light_omni->set_active(false);
			glow_render->set_active(false);
		} 
	}

	if (!m_switched_on) return;

	// calc color animator
	if (!lanim) return;

	int frame;
	// возвращает в формате BGR
	u32 clr = lanim->CalculateBGR(Device.fTimeGlobal,frame); 

	Fcolor fclr;
	fclr.set((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
	fclr.mul_rgb(fBrightness/255.f);
	if (can_use_dynamic_lights())
	{
		light_render->set_color(fclr);
		light_omni->set_color(fclr);
	}
	glow_render->set_color(fclr);
}

//ARTLantist: torch2 for flashlight
void CTorch::UpdateTorch2()
{
	if (!m_switched2_on) return;
	
	CBoneInstance &BI = smart_cast<IKinematics*>(Visual())->LL_GetBoneInstance(guid_bone);
	Fmatrix M;
	if (H_Parent()) 
	{
		CActor* actor = smart_cast<CActor*>(H_Parent());
		if (actor) smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones_Invalidate();

		if (H_Parent()->XFORM().c.distance_to_sqr(Device.vCameraPosition)<_sqr(OPTIMIZATION_DISTANCE) || GameID() != eGameIDSingle) {
			// near camera
			smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones();
			M.mul_43(XFORM(),BI.mTransform);
		} else {
			// approximately the same
			M = H_Parent()->XFORM();
			H_Parent()->Center(M.c);
			M.c.y += H_Parent()->Radius()*2.f/3.f;
		}
		if (actor) 
		{
			m_prev_hp.x = angle_inertion_var(m_prev_hp.x,-actor->cam_FirstEye()->yaw,m_torch2_inertion_speed_min,m_torch2_inertion_speed_max,TORCH_INERTION_CLAMP,Device.fTimeDelta);
			m_prev_hp.y = angle_inertion_var(m_prev_hp.y,-actor->cam_FirstEye()->pitch,m_torch2_inertion_speed_min,m_torch2_inertion_speed_max,TORCH_INERTION_CLAMP,Device.fTimeDelta);

			Fvector dir,right,up;	
			dir.setHP(m_prev_hp.x+m_delta_h,m_prev_hp.y);
			Fvector::generate_orthonormal_basis_normalized(dir,up,right);

			if (true) {
				Fvector offset2	= M.c; 
				offset2.mad(M.i,m_torch2_offset.x);
				offset2.mad(M.j,m_torch2_offset.y);
				offset2.mad(M.k,m_torch2_offset.z);
				light2_render->set_position(offset2);
				if(true) {
					offset2 = M.c; 
					offset2.mad(M.i,m_omni2_offset.x);
					offset2.mad(M.j,m_omni2_offset.y);
					offset2.mad(M.k,m_omni2_offset.z);
					light_omni->set_position(offset2);
				}
			}
			glow2_render->set_position(M.c);
			if (true) {
				light2_render->set_rotation(dir, right);
				if(true) {
					light2_omni->set_rotation(dir, right);
				}
			}
			glow2_render->set_direction(dir);
		}
	}
	else 
	{
		if (getVisible() && m_pPhysicsShell) 
		{
			M.mul(XFORM(),BI.mTransform);
			m_switched2_on = false;
			light2_render->set_active(false);
			light2_omni->set_active(false);
			glow2_render->set_active(false);
		} 
	}
	
	if (!m_switched2_on) return;
	if (!lanim2) return;
	int frame2;
	u32 clr2 = lanim2->CalculateBGR(Device.fTimeGlobal,frame2); 
	Fcolor fclr2;
	fclr2.set((float)color_get_B(clr2),(float)color_get_G(clr2),(float)color_get_R(clr2),1.f);
	fclr2.mul_rgb(fBrightness2/255.f);
	if (can_use_dynamic_lights())
	{
		light2_render->set_color(fclr2);
		light2_omni->set_color(fclr2);
	}
	glow_render->set_color(fclr2);
}
//ARTLantist End

void CTorch::create_physic_shell()
{
	CPhysicsShellHolder::create_physic_shell();
}

void CTorch::activate_physic_shell()
{
	CPhysicsShellHolder::activate_physic_shell();
}

void CTorch::setup_physic_shell	()
{
	CPhysicsShellHolder::setup_physic_shell();
}

void CTorch::net_Export			(NET_Packet& P)
{
	inherited::net_Export		(P);
//	P.w_u8						(m_switched_on ? 1 : 0);


	BYTE F = 0;
	F |= (m_switched_on ? eTorchActive : 0);
	F |= (m_bNightVisionOn ? eNightVisionActive : 0);
	const CActor *pA = smart_cast<const CActor *>(H_Parent());
	if (pA)
	{
		if (pA->attached(this))
			F |= eAttached;
	}
	P.w_u8(F);
}

void CTorch::net_Import			(NET_Packet& P)
{
	inherited::net_Import		(P);
	
	BYTE F = P.r_u8();
	bool new_m_switched_on				= !!(F & eTorchActive);
	bool new_m_bNightVisionOn			= !!(F & eNightVisionActive);

	if (new_m_switched_on != m_switched_on) Switch(new_m_switched_on);
	if (new_m_switched_on != m_switched2_on) Switch2(new_m_switched_on);
	if (new_m_bNightVisionOn != m_bNightVisionOn)	
	{
		const CActor *pA = smart_cast<const CActor *>(H_Parent());
		if (pA) {
			SwitchNightVision(new_m_bNightVisionOn);
		}
	}
}
bool  CTorch::can_be_attached		() const
{
	const CActor *pA = smart_cast<const CActor *>(H_Parent());
	if (pA)
		return pA->inventory().InSlot(this);
	else
		return true;
}

void CTorch::afterDetach			()
{
	inherited::afterDetach	();
	Switch					(false);
}
void CTorch::renderable_Render()
{
	inherited::renderable_Render();
}

void CTorch::enable(bool value)
{
	inherited::enable(value);

	if(!enabled() && m_switched_on)
		Switch(false);
	if(!enabled() && m_switched2_on)
		Switch2(false);

}

CNightVisionEffector::CNightVisionEffector(const shared_str& section)
:m_pActor(NULL)
{
	m_sounds.LoadSound(section.c_str(),"snd_night_vision_on", "NightVisionOnSnd", false, SOUND_TYPE_ITEM_USING);
	m_sounds.LoadSound(section.c_str(),"snd_night_vision_off", "NightVisionOffSnd", false, SOUND_TYPE_ITEM_USING);
	m_sounds.LoadSound(section.c_str(),"snd_night_vision_idle", "NightVisionIdleSnd", false, SOUND_TYPE_ITEM_USING);
	m_sounds.LoadSound(section.c_str(),"snd_night_vision_broken", "NightVisionBrokenSnd", false, SOUND_TYPE_ITEM_USING);
}

void CNightVisionEffector::Start(const shared_str& sect, CActor* pA, bool play_sound)
{
	m_pActor			= pA;
	AddEffector			(m_pActor, effNightvision, sect);
	if(play_sound)
	{
		PlaySounds(eStartSound);
		PlaySounds(eIdleSound);
	}
}

void CNightVisionEffector::Stop(const float factor, bool play_sound)
{
	if(!m_pActor)		return;
	CEffectorPP* pp		= m_pActor->Cameras().GetPPEffector((EEffectorPPType)effNightvision);
	if(pp)
	{
		pp->Stop			(factor);
		if(play_sound)
			PlaySounds(eStopSound);

		m_sounds.StopSound("NightVisionIdleSnd");
	}
}

bool CNightVisionEffector::IsActive()
{
	if(!m_pActor)	return false;
	CEffectorPP* pp = m_pActor->Cameras().GetPPEffector((EEffectorPPType)effNightvision);
	return (pp!=NULL);
}

void CNightVisionEffector::OnDisabled(CActor* pA, bool play_sound)
{
	m_pActor					= pA;
	if(play_sound)
		PlaySounds(eBrokeSound);
}

void CNightVisionEffector::PlaySounds(EPlaySounds which)
{
	if(!m_pActor)
		return;

	bool bPlaySoundFirstPerson = !!m_pActor->HUDview();
	switch(which)
	{
	case eStartSound:
		{
			m_sounds.PlaySound("NightVisionOnSnd", m_pActor->Position(), NULL, bPlaySoundFirstPerson);
		}break;
	case eStopSound:
		{
			m_sounds.PlaySound("NightVisionOffSnd", m_pActor->Position(), NULL, bPlaySoundFirstPerson);
		}break;
	case eIdleSound:
		{
			m_sounds.PlaySound("NightVisionIdleSnd", m_pActor->Position(), NULL, bPlaySoundFirstPerson, true);
		}break;
	case eBrokeSound:
		{
			m_sounds.PlaySound("NightVisionBrokenSnd", m_pActor->Position(), NULL, bPlaySoundFirstPerson);
		}break;
	default: NODEFAULT;
	}
}
