#pragma once
#include "hud_item_object.h"

class CLAItem;

class CGlowstick : public CHudItemObject {
private:
	typedef	CHudItemObject	inherited;
	inline	bool	can_use_dynamic_lights();

protected:
	bool			m_bFastAnimMode;
	bool			m_bNeedActivation;

	float			fBrightness;
	CLAItem*		lanim;

	shared_str		light_trace_bone;

	ref_light		light_render;
	ref_light		light_omni;
	ref_glow		glow_render;
	shared_str		m_light_section;

	bool			CheckCompatibilityInt(CHudItem* itm, u16* slot_to_activate);
	void			UpdateVisibility();
public:
	enum EGlowstickStates
	{
		eIdleThrowStart = 5,
		eIdleThrow,
		eIdleThrowEnd,
		eIdleZoom,
		eIdleZoomIn,
		eIdleZoomOut
	};

	CGlowstick();
	virtual			~CGlowstick();

	virtual BOOL 	net_Spawn(CSE_Abstract* DC);
	virtual void 	Load(LPCSTR section);

	virtual void 	OnH_A_Chield();
	virtual void 	OnH_B_Independent(bool just_before_destroy);

	virtual void 	shedule_Update(u32 dt);
	virtual void 	UpdateCL();

	bool 			IsWorking();

	virtual void 	OnMoveToSlot(const SInvItemPlace& prev);
	virtual void 	OnMoveToRuck(const SInvItemPlace& prev);

	virtual void	OnActiveItem();
	virtual void	OnHiddenItem();
	virtual void	OnStateSwitch(u32 S, u32 oldState);
	virtual void	OnAnimationEnd(u32 state);
	virtual	void	UpdateXForm();

	void			ToggleGlowstick(bool bFastMode);
	void			HideGlowstick(bool bFastMode);
	void			ShowGlowstick(bool bFastMode);
	virtual bool	CheckCompatibility(CHudItem* itm);

	virtual void	create_physic_shell();
	virtual void	activate_physic_shell();
	virtual void	setup_physic_shell();

	void	Switch(bool light_on);
};