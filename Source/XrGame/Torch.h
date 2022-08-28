#pragma once

#include "inventory_item_object.h"
#include "hudsound.h"
#include "script_export_space.h"
#include "../xrEngine/LightAnimLibrary.h"

class CLAItem;
class CNightVisionEffector;

class CTorch : public CInventoryItemObject {
private:
    typedef	CInventoryItemObject	inherited;

protected:
	float fBrightness;
	CLAItem* lanim;

	u16 guid_bone;
	shared_str light_trace_bone;

	float m_delta_h;
	Fvector2 m_prev_hp;
	bool m_switched_on;
	ref_light light_render;
	ref_light light_omni;
	ref_glow glow_render;
	Fvector m_focus;
	//ARTLantist
	bool m_torch_mod;
    shared_str m_light_section;
	Fcolor m_torch_color;
	Fcolor m_omny_color;
    Fvector m_torch_offset;
    Fvector m_omni_offset;
    float m_torch_inertion_speed_max;
    float m_torch_inertion_speed_min;
	//torch2 for flashlight
	float fBrightness2;
	CLAItem* lanim2;
	bool m_switched2_on;
	ref_light light2_render;
	ref_light light2_omni;
	ref_glow glow2_render;
    shared_str m_light2_section;
	Fcolor m_torch2_color;
	Fcolor m_omny2_color;
    Fvector m_torch2_offset;
    Fvector m_omni2_offset;
    float m_torch2_inertion_speed_max;
    float m_torch2_inertion_speed_min;
	//ARTLantist End
private:
	inline bool can_use_dynamic_lights();

public:
			CTorch();
	virtual ~CTorch();

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(CSE_Abstract* DC);
	virtual void net_Destroy();
	virtual void net_Export(NET_Packet& P); // export to server
	virtual void net_Import(NET_Packet& P);	// import from server

	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent(bool just_before_destroy);

	virtual void UpdateCL();

			void Switch();
			void Switch(bool light_on);
			bool torch_active() const;
			//ARTLantist
			void SetTorchMod();
			void UpdateTorch();
			void Torch_switch_spot(float value) { light_render->set_type((IRender_Light::LT)((value)?2:1)); }
			void Torch_set_color_r(float value) { m_torch_color.r = value; light_render->set_color(m_torch_color); fBrightness = m_torch_color.intensity(); }
			void Torch_set_color_g(float value) { m_torch_color.g = value; light_render->set_color(m_torch_color); fBrightness = m_torch_color.intensity(); }
			void Torch_set_color_b(float value) { m_torch_color.b = value; light_render->set_color(m_torch_color); fBrightness = m_torch_color.intensity(); }
			void Torch_set_color_a(float value) { m_torch_color.a = value; light_render->set_color(m_torch_color); fBrightness = m_torch_color.intensity(); }
			void Torch_set_offset_x(float value) { m_torch_offset.x = value; m_omni_offset.x = value; }
			void Torch_set_offset_y(float value) { m_torch_offset.y = value; m_omni_offset.y = value; }
			void Torch_set_offset_z(float value) { m_torch_offset.z = value; m_omni_offset.z = value; }
			void Torch_set_range(float value) { light_render->set_range(value); }
			void Torch_set_radius(float value) { glow_render->set_radius(value); }
			void Torch_set_texture(LPCSTR value) { light_render->set_texture(value); }
			void Torch_set_animation(LPCSTR value) { lanim = LALib.FindItem(value); }
			void Torch_set_inertion(float value) { m_torch_inertion_speed_max = value; }
			//torch2 for flashlight
			void UpdateTorch2();
			void Switch2(bool light_on);
			bool torch2_active() const;
			void Torch2_switch_spot(float value) { light2_render->set_type((IRender_Light::LT)((value)?2:1)); }
			void Torch2_set_color_r(float value) { m_torch2_color.r = value; light2_render->set_color(m_torch2_color); fBrightness2 = m_torch2_color.intensity(); }
			void Torch2_set_color_g(float value) { m_torch2_color.g = value; light2_render->set_color(m_torch2_color); fBrightness2 = m_torch2_color.intensity(); }
			void Torch2_set_color_b(float value) { m_torch2_color.b = value; light2_render->set_color(m_torch2_color); fBrightness2 = m_torch2_color.intensity(); }
			void Torch2_set_color_a(float value) { m_torch2_color.a = value; light2_render->set_color(m_torch2_color); fBrightness2 = m_torch2_color.intensity(); }
			void Torch2_set_offset_x(float value) { m_torch2_offset.x = value; m_omni2_offset.x = value; }
			void Torch2_set_offset_y(float value) { m_torch2_offset.y = value; m_omni2_offset.y = value; }
			void Torch2_set_offset_z(float value) { m_torch2_offset.z = value; m_omni2_offset.z = value; }
			void Torch2_set_range(float value) { light2_render->set_range(value); }
			void Torch2_set_radius(float value) { glow2_render->set_radius(value); }
			void Torch2_set_texture(LPCSTR value) { light2_render->set_texture(value); }
			void Torch2_set_animation(LPCSTR value) { lanim2 = LALib.FindItem(value); }
			void Torch2_set_inertion(float value) { m_torch2_inertion_speed_max = value; }
			//ARTLantist End

	virtual bool can_be_attached	() const;

	//CAttachableItem
	virtual	void enable(bool value);
 
public:
			void SwitchNightVision();
			void SwitchNightVision(bool light_on, bool use_sounds=true);

			bool GetNightVisionStatus() { return m_bNightVisionOn; }
CNightVisionEffector* GetNightVision() { return m_night_vision; }
protected:
	bool m_bNightVisionEnabled;
	bool m_bNightVisionOn;

	CNightVisionEffector* m_night_vision;
	HUD_SOUND_COLLECTION m_sounds;

	enum EStats{
		eTorchActive = (1<<0),
		eNightVisionActive = (1<<1),
		eAttached = (1<<2)
	};

public:

	virtual bool use_parent_ai_locations	() const
	{
		return (!H_Parent());
	}
	virtual void create_physic_shell		();
	virtual void activate_physic_shell	();
	virtual void setup_physic_shell		();

	virtual void afterDetach				();
	virtual void renderable_Render		();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

class CNightVisionEffector
{
	CActor* m_pActor;
	HUD_SOUND_COLLECTION m_sounds;
public:
	enum EPlaySounds{
		eStartSound	= 0,
		eStopSound,
		eIdleSound,
		eBrokeSound
	};
				CNightVisionEffector(const shared_str& sect);
	void Start(const shared_str& sect, CActor* pA, bool play_sound=true);
	void Stop(const float factor, bool play_sound=true);
	bool IsActive();
	void OnDisabled(CActor* pA, bool play_sound=true);
	void PlaySounds(EPlaySounds which);
};

add_to_type_list(CTorch)
#undef script_type_list
#define script_type_list save_type_list(CTorch)
