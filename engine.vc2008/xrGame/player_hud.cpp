#include "stdafx.h"
#include "player_hud.h"
#include "HudItem.h"
#include "ui_base.h"
#include "actor.h"
#include "physic_item.h"
#include "static_cast_checked.hpp"
#include "actoreffector.h"
#include "../xrEngine/IGame_Persistent.h"

player_hud* g_player_hud = NULL;
Fvector _ancor_pos;
Fvector _wpn_root_pos;

float CalcMotionSpeed(const shared_str& anim_name)
{
	if (!IsGameTypeSingle() && (anim_name == "anm_show" || anim_name == "anm_hide"))
		return 2.0f;
	else
		return 1.0f;
}

player_hud_motion* player_hud_motion_container::find_motion(const shared_str& name)
{
	xr_vector<player_hud_motion>::iterator it = m_anims.begin();
	xr_vector<player_hud_motion>::iterator it_e = m_anims.end();
	for (; it != it_e; ++it)
	{
		const shared_str& s = (true) ? (*it).m_alias_name : (*it).m_base_name;
		if (s == name)
			return &(*it);
	}
	return NULL;
}

void player_hud_motion_container::load(IKinematicsAnimated* model, const shared_str& sect)
{
	CInifile::Sect& _sect = pSettings->r_section(sect);
	CInifile::SectCIt _b = _sect.Data.begin();
	CInifile::SectCIt _e = _sect.Data.end();
	player_hud_motion* pm = NULL;

	string512 buff;
	MotionID motion_ID;

	for (; _b != _e; ++_b)
	{
		if (strstr(_b->first.c_str(), "anm_") == _b->first.c_str())
		{
			const shared_str& anm = _b->second;
			m_anims.resize(m_anims.size() + 1);
			pm = &m_anims.back();
			//base and alias name
			pm->m_alias_name = _b->first;

			if (_GetItemCount(anm.c_str()) == 1)
			{
				pm->m_base_name = anm;
				pm->m_additional_name = anm;
			}
			else
			{
				R_ASSERT2(_GetItemCount(anm.c_str())<=4, anm.c_str());
				string512 str_item;
				_GetItem(anm.c_str(), 0, str_item);
				pm->m_base_name = str_item;

				_GetItem(anm.c_str(), 1, str_item);
				pm->m_additional_name = str_item;

				_GetItem(anm.c_str(), 2, str_item);
				pm->m_anim_speed = atof(str_item);

				_GetItem(anm.c_str(), 3, str_item);
				pm->m_anim_end = atof(str_item);
			}

			//and load all motions for it

			for (u32 i = 0; i <= 8; ++i)
			{
				if (i == 0)
					xr_strcpy(buff, pm->m_base_name.c_str());
				else
					xr_sprintf(buff, "%s%d", pm->m_base_name.c_str(), i);

				motion_ID = model->ID_Cycle_Safe(buff);
				if (!motion_ID.valid() && i == 0)
				{
					motion_ID = model->ID_Cycle_Safe("hand_idle_doun");
				}
				if (motion_ID.valid())
				{
					pm->m_animations.resize(pm->m_animations.size() + 1);
					pm->m_animations.back().mid = motion_ID;
					pm->m_animations.back().name = buff;
#ifdef DEBUG
					//Msg(" alias=[%s] base=[%s] name=[%s]",pm->m_alias_name.c_str(), pm->m_base_name.c_str(), buff);
#endif // #ifdef DEBUG
				}
			}
			VERIFY2(pm->m_animations.size(), make_string("motion not found [%s]", pm->m_base_name.c_str()).c_str());
		}
	}
}

Fvector& attachable_hud_item::hands_attach_pos()
{
	return m_measures.m_hands_attach[0];
}

Fvector& attachable_hud_item::hands_attach_rot()
{
	return m_measures.m_hands_attach[1];
}

Fvector& attachable_hud_item::hands_offset_pos()
{
	u8 idx = m_parent_hud_item->GetCurrentHudOffsetIdx();
	return m_measures.m_hands_offset[0][idx];
}

Fvector& attachable_hud_item::hands_offset_rot()
{
	u8 idx = m_parent_hud_item->GetCurrentHudOffsetIdx();
	return m_measures.m_hands_offset[1][idx];
}

void attachable_hud_item::set_bone_visible(const shared_str& bone_name, BOOL bVisibility, BOOL bSilent)
{
	u16 bone_id;
	BOOL bVisibleNow;
	bone_id = m_model->LL_BoneID(bone_name);
	if (bone_id == BI_NONE)
	{
		if (bSilent) return;
		R_ASSERT2(0, make_string("model [%s] has no bone [%s]",pSettings->r_string(m_sect_name, "item_visual"), bone_name.c_str()).c_str());
	}
	bVisibleNow = m_model->LL_GetBoneVisible(bone_id);
	if (bVisibleNow != bVisibility)
		m_model->LL_SetBoneVisible(bone_id, bVisibility, TRUE);
}

void attachable_hud_item::update(bool bForce)
{
	if (!bForce && m_upd_firedeps_frame == Device.dwFrame) return;
	bool is_16x9 = UI().is_widescreen();

	if (!!m_measures.m_prop_flags.test(hud_item_measures::e_16x9_mode_now) != is_16x9)
		m_measures.load(m_sect_name, m_model);

	Fvector ypr = m_measures.m_item_attach[1];
	ypr.mul(PI / 180.f);
	m_attach_offset.setHPB(ypr.x, ypr.y, ypr.z);
	m_attach_offset.translate_over(m_measures.m_item_attach[0]);

	m_parent->calc_transform(m_attach_place_idx, m_attach_offset, m_item_transform);
	m_upd_firedeps_frame = Device.dwFrame;

	IKinematicsAnimated* ka = m_model->dcast_PKinematicsAnimated();
	if (ka)
	{
		ka->UpdateTracks();
		ka->dcast_PKinematics()->CalculateBones_Invalidate();
		ka->dcast_PKinematics()->CalculateBones(TRUE);
	}
}

void attachable_hud_item::setup_firedeps(firedeps& fd)
{
	update(false);
	// fire point&direction
	if (m_measures.m_prop_flags.test(hud_item_measures::e_fire_point))
	{
		Fmatrix& fire_mat = m_model->LL_GetTransform(m_measures.m_fire_bone);
		fire_mat.transform_tiny(fd.vLastFP, m_measures.m_fire_point_offset);
		m_item_transform.transform_tiny(fd.vLastFP);

		fd.vLastFD.set(0.f,0.f,1.f);
		m_item_transform.transform_dir(fd.vLastFD);
		VERIFY(_valid(fd.vLastFD));
		VERIFY(_valid(fd.vLastFD));

		fd.m_FireParticlesXForm.identity();
		fd.m_FireParticlesXForm.k.set(fd.vLastFD);
		Fvector::generate_orthonormal_basis_normalized(fd.m_FireParticlesXForm.k, fd.m_FireParticlesXForm.j, fd.m_FireParticlesXForm.i);
		VERIFY(_valid(fd.m_FireParticlesXForm));
	}

	if (m_measures.m_prop_flags.test(hud_item_measures::e_fire_point2))
	{
		Fmatrix& fire_mat = m_model->LL_GetTransform(m_measures.m_fire_bone2);
		fire_mat.transform_tiny(fd.vLastFP2,m_measures.m_fire_point2_offset);
		m_item_transform.transform_tiny(fd.vLastFP2);
		VERIFY(_valid(fd.vLastFP2));
		VERIFY(_valid(fd.vLastFP2));
	}

	if (m_measures.m_prop_flags.test(hud_item_measures::e_shell_point))
	{
		Fmatrix& fire_mat = m_model->LL_GetTransform(m_measures.m_shell_bone);
		fire_mat.transform_tiny(fd.vLastSP,m_measures.m_shell_point_offset);
		m_item_transform.transform_tiny(fd.vLastSP);
		VERIFY(_valid(fd.vLastSP));
		VERIFY(_valid(fd.vLastSP));
	}
}

bool attachable_hud_item::need_renderable()
{
	return m_parent_hud_item->need_renderable();
}

void attachable_hud_item::render()
{
	::Render->set_Transform(&m_item_transform);
	::Render->add_Visual(m_model->dcast_RenderVisual());
	debug_draw_firedeps();
	m_parent_hud_item->render_hud_mode();
}

bool attachable_hud_item::render_item_ui_query()
{
	return m_parent_hud_item->render_item_3d_ui_query();
}

void attachable_hud_item::render_item_ui()
{
	m_parent_hud_item->render_item_3d_ui();
}

void hud_item_measures::load(const shared_str& sect_name, IKinematics* K)
{
	bool is_16x9 = UI().is_widescreen();
	string64 _prefix;
	xr_sprintf(_prefix, "%s", is_16x9 ? "_16x9" : "");
	string128 val_name;

	strconcat(sizeof(val_name), val_name, "hands_position", _prefix);
	m_hands_attach[0] = pSettings->r_fvector3(sect_name, val_name);
	strconcat(sizeof(val_name), val_name, "hands_orientation", _prefix);
	m_hands_attach[1] = pSettings->r_fvector3(sect_name, val_name);

	m_item_attach[0] = pSettings->r_fvector3(sect_name, "item_position");
	m_item_attach[1] = pSettings->r_fvector3(sect_name, "item_orientation");

	shared_str bone_name;
	m_prop_flags.set(e_fire_point, pSettings->line_exist(sect_name, "fire_bone"));
	if (m_prop_flags.test(e_fire_point))
	{
		bone_name = pSettings->r_string(sect_name, "fire_bone");
		m_fire_bone = K->LL_BoneID(bone_name);
		m_fire_point_offset = pSettings->r_fvector3(sect_name, "fire_point");
	}
	else
		m_fire_point_offset.set(0, 0, 0);

	m_prop_flags.set(e_fire_point2, pSettings->line_exist(sect_name, "fire_bone2"));
	if (m_prop_flags.test(e_fire_point2))
	{
		bone_name = pSettings->r_string(sect_name, "fire_bone2");
		m_fire_bone2 = K->LL_BoneID(bone_name);
		m_fire_point2_offset = pSettings->r_fvector3(sect_name, "fire_point2");
	}
	else
		m_fire_point2_offset.set(0, 0, 0);

	m_prop_flags.set(e_shell_point, pSettings->line_exist(sect_name, "shell_bone"));
	if (m_prop_flags.test(e_shell_point))
	{
		bone_name = pSettings->r_string(sect_name, "shell_bone");
		m_shell_bone = K->LL_BoneID(bone_name);
		m_shell_point_offset = pSettings->r_fvector3(sect_name, "shell_point");
	}
	else
		m_shell_point_offset.set(0, 0, 0);

	m_hands_offset[0][0].set(0, 0, 0);
	m_hands_offset[1][0].set(0, 0, 0);

	strconcat(sizeof(val_name), val_name, "aim_hud_offset_pos", _prefix);
	m_hands_offset[0][1] = pSettings->r_fvector3(sect_name, val_name);
	strconcat(sizeof(val_name), val_name, "aim_hud_offset_rot", _prefix);
	m_hands_offset[1][1] = pSettings->r_fvector3(sect_name, val_name);

	strconcat(sizeof(val_name), val_name, "gl_hud_offset_pos", _prefix);
	m_hands_offset[0][2] = pSettings->r_fvector3(sect_name, val_name);
	strconcat(sizeof(val_name), val_name, "gl_hud_offset_rot", _prefix);
	m_hands_offset[1][2] = pSettings->r_fvector3(sect_name, val_name);

	R_ASSERT2(pSettings->line_exist(sect_name,"fire_point")==pSettings->line_exist(sect_name,"fire_bone"),		sect_name.c_str());
	R_ASSERT2(pSettings->line_exist(sect_name,"fire_point2")==pSettings->line_exist(sect_name,"fire_bone2"),	sect_name.c_str());
	R_ASSERT2(pSettings->line_exist(sect_name,"shell_point")==pSettings->line_exist(sect_name,"shell_bone"),	sect_name.c_str());

	m_prop_flags.set(e_16x9_mode_now,is_16x9);
}

attachable_hud_item::~attachable_hud_item()
{
	IRenderVisual* v = m_model->dcast_RenderVisual();
	::Render->model_Delete(v);
	m_model = NULL;
}

void attachable_hud_item::load(const shared_str& sect_name)
{
	m_sect_name = sect_name;

	// Visual
	const shared_str& visual_name = pSettings->r_string(sect_name, "item_visual");
	m_model = smart_cast<IKinematics*>(::Render->model_Create(visual_name.c_str()));

	m_attach_place_idx = pSettings->r_u16(sect_name, "attach_place_idx");
	m_measures.load(sect_name, m_model);
}

player_hud_motion* attachable_hud_item::find_motion(const shared_str& anm_name)
{
	R_ASSERT(strstr(anm_name.c_str(), "anm_")==anm_name.c_str());
	string256 anim_name_r;
	bool is_16x9 = UI().is_widescreen();
	xr_sprintf(anim_name_r, "%s%s",anm_name.c_str(),((m_attach_place_idx==1)&&is_16x9)?"_16x9":"");

	player_hud_motion* anm = m_hand_motions.find_motion(anim_name_r);
	if (!anm)
		anm = m_hand_motions.find_motion(anm_name);
	
	R_ASSERT2(anm, make_string("model [%s] has no motion alias defined [%s]", m_sect_name.c_str(), anim_name_r).c_str());
	R_ASSERT2(anm->m_animations.size(), make_string("model [%s] has no motion defined in motion_alias [%s]", pSettings->r_string(m_sect_name, "item_visual"), anim_name_r).c_str());

	return anm;
}

u32 attachable_hud_item::anim_play(const shared_str& anm_name_b, BOOL bMixIn, const CMotionDef*& md, u8& rnd_idx, float speed, bool bMixIn2)
{
	player_hud_motion* anm = find_motion(anm_name_b);
	rnd_idx = (u8)Random.randI(anm->m_animations.size());
	const motion_descr& M = anm->m_animations[rnd_idx];

	if (speed == 0)
		speed = CalcMotionSpeed(anm_name_b);
	
	u32 ret = g_player_hud->anim_play(m_attach_place_idx, M.mid, bMixIn, md, speed);
	
	if(m_model->dcast_PKinematicsAnimated())
	{
		IKinematicsAnimated* ka = m_model->dcast_PKinematicsAnimated();

		shared_str item_anm_name;
		if (anm->m_base_name != anm->m_additional_name)
			item_anm_name = anm->m_additional_name;
		else
			item_anm_name = M.name;

		MotionID M2 = ka->ID_Cycle_Safe(item_anm_name);
		if (!M2.valid())
			M2 = ka->ID_Cycle_Safe("idle");
		else
			if (bDebug)
				Msg("playing item animation [%s]",item_anm_name.c_str());
		
		R_ASSERT3(M2.valid(),"model has no motion [idle] ", pSettings->r_string(m_sect_name, "item_visual"));

		u16 root_id = m_model->LL_GetBoneRoot();
		CBoneInstance& root_binst = m_model->LL_GetBoneInstance(root_id);
		root_binst.set_callback_overwrite(TRUE);
		root_binst.mTransform.identity();

		u16 pc = ka->partitions().count();
		for (u16 pid=0; pid<pc; ++pid)
		{
			CBlend* B = ka->PlayCycle(pid, M2, bMixIn);
			R_ASSERT(B);
			B->speed *= speed;
		}

		m_model->CalculateBones_Invalidate();
	}

	R_ASSERT2(m_parent_hud_item, "parent hud item is NULL");
	CPhysicItem& parent_object = m_parent_hud_item->object();

	if (IsGameTypeSingle() && parent_object.H_Parent() == Level().CurrentControlEntity())
	{
		CActor* current_actor = static_cast_checked<CActor*>(Level().CurrentControlEntity());
		VERIFY(current_actor);
		CEffectorCam* ec = current_actor->Cameras().GetCamEffector(eCEWeaponAction);

	
		if (NULL == ec)
		{
			string_path ce_path;
			string_path anm_name;
			strconcat(sizeof(anm_name),anm_name,"camera_effects\\weapon\\", M.name.c_str(),".anm");
			if (FS.exist( ce_path, "$game_anims$", anm_name))
			{
				CAnimatorCamEffector* e = xr_new<CAnimatorCamEffector>();
				e->SetType(eCEWeaponAction);
				e->SetHudAffect(false);
				e->SetCyclic(false);
				e->Start(anm_name);
				current_actor->Cameras().AddCamEffector(e);
			}
		}
	}
	return ret;
}

player_hud::player_hud()
{
	m_model = NULL;
	script_anim_item_model = NULL;
	m_attached_items[0] = NULL;
	m_attached_items[1] = NULL;
	m_transform.identity();
	script_anim_part = u8(-1);
	script_anim_offset_factor = 0.f;
	m_item_pos.identity();
}


player_hud::~player_hud()
{
	IRenderVisual* v = m_model->dcast_RenderVisual();
	::Render->model_Delete(v);
	m_model = NULL;
	
	if (script_anim_item_model){
		v = script_anim_item_model->dcast_RenderVisual();
		::Render->model_Delete(v);
	}
	script_anim_item_model = NULL;

	xr_vector<attachable_hud_item*>::iterator it = m_pool.begin();
	xr_vector<attachable_hud_item*>::iterator it_e = m_pool.end();
	for (; it != it_e; ++it)
	{
		attachable_hud_item* a	= *it;
		xr_delete(a);
	}
	m_pool.clear();
}

void player_hud::load(const shared_str& player_hud_sect)
{
	if (player_hud_sect == m_sect_name) return;
	bool b_reload = (m_model != NULL);
	if(m_model)
	{
		IRenderVisual* v = m_model->dcast_RenderVisual();
		::Render->model_Delete(v);
	}

	m_sect_name = player_hud_sect;
	const shared_str& model_name = pSettings->r_string(player_hud_sect, "visual");
	m_model = smart_cast<IKinematicsAnimated*>(::Render->model_Create(model_name.c_str()));

	CInifile::Sect& _sect = pSettings->r_section(player_hud_sect);
	CInifile::SectCIt _b = _sect.Data.begin();
	CInifile::SectCIt _e = _sect.Data.end();
	for (; _b != _e; ++_b)
	{
		if (strstr(_b->first.c_str(), "ancor_") == _b->first.c_str())
		{
			const shared_str& _bone = _b->second;
			m_ancors.push_back(m_model->dcast_PKinematics()->LL_BoneID(_bone));
		}
	}

	if (!b_reload)
	{
		m_model->PlayCycle("hand_idle_doun");
	}
	else
	{
		if (m_attached_items[1])
			m_attached_items[1]->m_parent_hud_item->on_a_hud_attach();

		if (m_attached_items[0])
			m_attached_items[0]->m_parent_hud_item->on_a_hud_attach();
	}
	m_model->dcast_PKinematics()->CalculateBones_Invalidate();
	m_model->dcast_PKinematics()->CalculateBones(TRUE);
}

bool player_hud::render_item_ui_query()
{
	bool res = false;
	if (m_attached_items[0])
		res |= m_attached_items[0]->render_item_ui_query();

	if (m_attached_items[1])
		res |= m_attached_items[1]->render_item_ui_query();

	return res;
}

void player_hud::render_item_ui()
{
	if (m_attached_items[0])
		m_attached_items[0]->render_item_ui();

	if (m_attached_items[1])
		m_attached_items[1]->render_item_ui();
}

void player_hud::render_hud()
{
	bool b_r0 = ((m_attached_items[0] && m_attached_items[0]->need_renderable()) || script_anim_part == 0 || script_anim_part == 2);
	bool b_r1 = ((m_attached_items[1] && m_attached_items[1]->need_renderable()) || script_anim_part == 1 || script_anim_part == 2);

	if (!b_r0 && !b_r1) return;

	::Render->set_Transform(&m_transform);
	::Render->add_Visual(m_model->dcast_RenderVisual());

	if (m_attached_items[0])
		m_attached_items[0]->render();

	if (m_attached_items[1])
		m_attached_items[1]->render();

	if (script_anim_item_model)
	{
		::Render->set_Transform(&m_item_pos);
		::Render->add_Visual(script_anim_item_model->dcast_RenderVisual());
	}
}

#include "../xrEngine/motion.h"

u32 player_hud::motion_length_script(LPCSTR section, LPCSTR anm_name, float speed)
{
	if (!pSettings->section_exist(section))
	{
		Msg("!script motion section [%s] does not exist", section);
		return 0;
	}

	player_hud_motion_container* pm = get_hand_motions(section);
	if (!pm)
		return 0;

	player_hud_motion* phm = pm->find_motion(anm_name);
	if (!phm)
	{
		Msg("!script motion [%s] not found in section [%s]", anm_name, section);
		return 0;
	}

	const CMotionDef* temp;
	return motion_length(phm->m_animations[0].mid, temp, speed);
}

u32 player_hud::motion_length(const shared_str& anim_name, const shared_str& hud_name, const CMotionDef*& md)
{
	float speed = CalcMotionSpeed(anim_name);
	attachable_hud_item* pi = create_hud_item(hud_name);
	player_hud_motion* pm = pi->m_hand_motions.find_motion(anim_name);
	if (!pm)
		return 100;
	R_ASSERT2(pm,make_string ("hudItem model [%s] has no motion with alias [%s]", hud_name.c_str(), anim_name.c_str() ).c_str()	);
	return motion_length(pm->m_animations[0].mid, md, speed);
}

u32 player_hud::motion_length(const MotionID& M, const CMotionDef*& md, float speed)
{
	md = m_model->LL_GetMotionDef(M);
	VERIFY(md);
	if (md->flags & esmStopAtEnd)
	{
		CMotion* motion = m_model->LL_GetRootMotion(M);
		return iFloor(0.5f + 1000.f * motion->GetLength() / (md->Dequantize(md->speed) * speed));
	}
	return 0;
}

const Fvector& player_hud::attach_rot() const
{
	if(m_attached_items[0])
		return m_attached_items[0]->hands_attach_rot();
	else if(m_attached_items[1])
		return m_attached_items[1]->hands_attach_rot();
	return Fvector().set(0,0,0);
}

const Fvector& player_hud::attach_pos() const
{
	if(m_attached_items[0])
		return m_attached_items[0]->hands_attach_pos();
	else if(m_attached_items[1])
		return m_attached_items[1]->hands_attach_pos();
	return Fvector().set(0,0,0);
}

void player_hud::update(const Fmatrix& cam_trans)
{
	Fmatrix trans = cam_trans;
	Fmatrix trans_b = cam_trans;

	Fvector pos = attach_pos();
	Fvector rot = attach_rot();

	update_inertion(trans);
	if (m_attached_items[0])
		m_attached_items[0]->m_parent_hud_item->UpdateHudAdditional(trans);
		
	if (m_attached_items[1])
	{
		m_attached_items[1]->m_parent_hud_item->UpdateHudAdditional(trans);
	}
	
	// override hand offset for single hand animation
	if (script_anim_offset_factor != 0.f)
	{
		if (script_anim_part == 2 || (!m_attached_items[0] && !m_attached_items[1]))
		{
			pos = script_anim_offset[0];
			rot = script_anim_offset[1];
			trans = trans_b;
		}
		else
		{
			Fvector& hand_pos = pos;
			Fvector& hand_rot = rot;

			hand_pos.lerp(pos, script_anim_offset[0], script_anim_offset_factor);
			hand_rot.lerp(rot, script_anim_offset[1], script_anim_offset_factor);

			if (script_anim_part == 0)
			{
				trans_b.inertion(trans, script_anim_offset_factor);
				trans = trans_b;
			}
		}
	}
	rot.mul(PI / 180.f);
	m_attach_offset.setHPB(rot.x, rot.y, rot.z);
	m_attach_offset.translate_over(pos);
	m_transform.mul(trans, m_attach_offset);
	
	m_model->UpdateTracks();
	m_model->dcast_PKinematics()->CalculateBones_Invalidate();
	m_model->dcast_PKinematics()->CalculateBones(TRUE);
	
	xr_vector<script_layer*>::iterator it = m_script_layers.begin();
	xr_vector<script_layer*>::iterator it_e = m_script_layers.end();
	for (; it != it_e; it++)
	{
		script_layer* anm = *it;
		if (!anm || !anm->anm || (!anm->active && anm->blend_amount == 0.f))
			continue;

		if (anm->active)
			anm->blend_amount += Device.fTimeDelta / .4f;
		else
			anm->blend_amount -= Device.fTimeDelta / .4f;

		clamp(anm->blend_amount, 0.f, 1.f);

		if (anm->blend_amount > 0.f)
		{
			if (anm->anm->bLoop || anm->anm->anim_param().t_current < anm->anm->anim_param().max_t)
				anm->anm->Update(Device.fTimeDelta);
			else
				anm->Stop(false);
		}
		else
		{
			anm->Stop(true);
			continue;
		}

		Fmatrix blend = anm->XFORM();

		if (anm->m_part == 0 || anm->m_part == 1 || anm->m_part == 2)
			m_transform.mulB_43(blend);
	}
	
	if (m_attached_items[0])
		m_attached_items[0]->update(true);

	if (m_attached_items[1])
		m_attached_items[1]->update(true);

	if (script_anim_item_attached && script_anim_item_model)
		update_script_item();

	// single hand offset smoothing + syncing back to other hand animation on end
	if (script_anim_part != u8(-1))
	{
		script_anim_offset_factor += Device.fTimeDelta * 2.5f;

		if (m_bStopAtEndAnimIsRunning && Device.dwTimeGlobal >= script_anim_end)
			StopScriptAnim();
	}
	else
		script_anim_offset_factor -= Device.fTimeDelta * 5.f;

	clamp(script_anim_offset_factor, 0.f, 1.f);
}

void player_hud::PlayBlendAnm(LPCSTR name, u8 part, float speed, float power, bool bLooped, bool no_restart)
{
	xr_vector<script_layer*>::iterator it = m_script_layers.begin();
	xr_vector<script_layer*>::iterator it_e = m_script_layers.end();
	for (; it != it_e; it++)
	{
		script_layer* anm = *it;
		if (!xr_strcmp(anm->m_name, name))
		{
			if (!no_restart)
			{
				anm->anm->Play(bLooped);
				anm->blend_amount = 0.f;
			}
				
			anm->m_part = part;
			anm->anm->Speed() = speed;
			anm->m_power = power;
			anm->active = true;
			return;
		}
	}

	script_layer* anm = xr_new<script_layer>(name, part, speed, power, bLooped);
	m_script_layers.push_back(anm);
}

void player_hud::StopBlendAnm(LPCSTR name, bool bForce)
{
	xr_vector<script_layer*>::iterator it = m_script_layers.begin();
	xr_vector<script_layer*>::iterator it_e = m_script_layers.end();
	for (; it != it_e; it++)
	{
		script_layer* anm = *it;
		if (!xr_strcmp(anm->m_name, name))
		{
			anm->Stop(bForce);
			return;
		}
	}
}

void player_hud::StopAllBlendAnms(bool bForce)
{
	xr_vector<script_layer*>::iterator it = m_script_layers.begin();
	xr_vector<script_layer*>::iterator it_e = m_script_layers.end();
	for (; it != it_e; it++)
	{
		script_layer* anm = *it;
		anm->Stop(bForce);
	}
}

float player_hud::SetBlendAnmTime(LPCSTR name, float time)
{
	xr_vector<script_layer*>::iterator it = m_script_layers.begin();
	xr_vector<script_layer*>::iterator it_e = m_script_layers.end();
	for (; it != it_e; it++)
	{
		script_layer* anm = *it;
		if (!xr_strcmp(anm->m_name, name))
		{
			float speed = (anm->anm->anim_param().max_t - anm->anm->anim_param().t_current) / time;
			anm->anm->Speed() = speed;
			return speed;
		}
	}
	
	return 0;
}

void player_hud::StopScriptAnim()
{
	u8 part = script_anim_part;
	script_anim_part = u8(-1);
	script_anim_item_model = NULL;

	if (part != 2 && !m_attached_items[part])
		re_sync_anim(part + 1);
	else
		OnMovementChanged((ACTOR_DEFS::EMoveCommand) 0);
}

u32 player_hud::anim_play(u16 part, const MotionID& M, BOOL bMixIn, const CMotionDef*& md, float speed, u16 override_part)
{
	u16 part_id = u16(-1);
	if (attached_item(0) && attached_item(1))
		part_id = m_model->partitions().part_id((part == 0) ? "right_hand" : "left_hand");

	if (override_part != u16(-1))
		part_id = override_part;
	else if (script_anim_part != u8(-1))
	{
		if (script_anim_part != 2)
			part_id = script_anim_part == 0 ? 1 : 0;
		else
			return 0;
	}
	
	u16 pc = m_model->partitions().count();
	for (u16 pid=0; pid < pc; ++pid)
	{
		if(pid==0 || pid==part_id || part_id==u16(-1))
		{
			CBlend* B = m_model->PlayCycle(pid, M, bMixIn);
			R_ASSERT(B);
			B->speed *= speed;
		}
	}
	m_model->dcast_PKinematics()->CalculateBones_Invalidate	();

	return motion_length(M, md, speed);
}

player_hud_motion_container* player_hud::get_hand_motions(LPCSTR section)
{
	xr_vector<hand_motions*>::iterator it = m_hand_motions.begin();
	xr_vector<hand_motions*>::iterator it_e = m_hand_motions.end();
	for (; it != it_e; it++)
	{
		if (!xr_strcmp((*it)->section, section))
			return &(*it)->pm;
	}

	hand_motions* res = xr_new<hand_motions>();
	res->section = section;
	res->pm.load(m_model, section);
	m_hand_motions.push_back(res);

	return &res->pm;
}

void player_hud::update_script_item()
{
	Fvector ypr = item_pos[1];
	ypr.mul(PI / 180.f);
	m_attach_offset.setHPB(ypr.x, ypr.y, ypr.z);
	m_attach_offset.translate_over(item_pos[0]);

	calc_transform(m_attach_idx, m_attach_offset, m_item_pos);

	if (script_anim_item_model)
	{
		script_anim_item_model->UpdateTracks();
		script_anim_item_model->dcast_PKinematics()->CalculateBones_Invalidate();
		script_anim_item_model->dcast_PKinematics()->CalculateBones(TRUE);
	}
}

u32 player_hud::script_anim_play(u8 hand, LPCSTR section, LPCSTR anm_name, bool bMixIn, float speed)
{
	xr_string pos = "hands_position";
	xr_string rot = "hands_orientation";

	if (UI().is_widescreen())
	{
		pos.append("_16x9");
		rot.append("_16x9");
	}
	
	Fvector def = { 0.f, 0.f, 0.f };
	Fvector offs = READ_IF_EXISTS(pSettings, r_fvector3, section, pos.c_str(), def);
	Fvector rrot = READ_IF_EXISTS(pSettings, r_fvector3, section, rot.c_str(), def);

	if (pSettings->line_exist(section, "item_visual"))
	{
		script_anim_item_model = ::Render->model_Create(pSettings->r_string(section, "item_visual"))->dcast_PKinematicsAnimated();
		item_pos[0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "item_position", def);
		item_pos[1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "item_orientation", def);
		script_anim_item_attached = READ_IF_EXISTS(pSettings, r_bool, section, "item_attached", true);
		m_attach_idx = READ_IF_EXISTS(pSettings, r_u8, section, "attach_place_idx", 0);

		if (!script_anim_item_attached)
		{
			Fmatrix attach_offs;
			Fvector ypr = item_pos[1];
			ypr.mul(PI / 180.f);
			attach_offs.setHPB(ypr.x, ypr.y, ypr.z);
			attach_offs.translate_over(item_pos[0]);
			m_item_pos = attach_offs;
		}
	}

	script_anim_offset[0] = offs;
	script_anim_offset[1] = rrot;
	script_anim_part = hand;

	if (!pSettings->section_exist(section))
	{
		Msg("!script motion section [%s] does not exist", section);
		m_bStopAtEndAnimIsRunning = true;
		script_anim_end = Device.dwTimeGlobal;
		return 0;
	}

	player_hud_motion_container* pm = get_hand_motions(section);
	player_hud_motion* phm = pm->find_motion(anm_name);

	if (!phm)
	{
		Msg("!script motion [%s] not found in section [%s]", anm_name, section);
		m_bStopAtEndAnimIsRunning = true;
		script_anim_end = Device.dwTimeGlobal;
		return 0;
	}
	
	const motion_descr& M = phm->m_animations[Random.randI(phm->m_animations.size())];

	if (script_anim_item_model)
	{
		shared_str item_anm_name;
		if (phm->m_base_name != phm->m_additional_name)
			item_anm_name = phm->m_additional_name;
		else
			item_anm_name = M.name;

		MotionID M2 = script_anim_item_model->ID_Cycle_Safe(item_anm_name);
		if (!M2.valid())
			M2 = script_anim_item_model->ID_Cycle_Safe("idle");

		R_ASSERT3(M2.valid(), "model %s has no motion [idle] ", pSettings->r_string(m_sect_name, "item_visual"));

		u16 root_id = script_anim_item_model->dcast_PKinematics()->LL_GetBoneRoot();
		CBoneInstance& root_binst = script_anim_item_model->dcast_PKinematics()->LL_GetBoneInstance(root_id);
		root_binst.set_callback_overwrite(TRUE);
		root_binst.mTransform.identity();

		u16 pc = script_anim_item_model->partitions().count();
		for (u16 pid = 0; pid < pc; ++pid)
		{
			CBlend* B = script_anim_item_model->PlayCycle(pid, M2, bMixIn);
			R_ASSERT(B);
			B->speed *= speed;
		}

		script_anim_item_model->dcast_PKinematics()->CalculateBones_Invalidate();
	}

	u16 pc = m_model->partitions().count();
	for (u16 pid = 0; pid < pc; ++pid)
	{
		CBlend* B = m_model->PlayCycle(pid, M.mid, bMixIn);
		R_ASSERT(B);
		B->speed *= speed;
	}

	const CMotionDef* md;
	u32 length = motion_length(M.mid, md, speed);

	if (length > 0)
	{
		m_bStopAtEndAnimIsRunning = true;
		script_anim_end = Device.dwTimeGlobal + length;
	}
	else
		m_bStopAtEndAnimIsRunning = false;

	return length;
}

attachable_hud_item* player_hud::create_hud_item(const shared_str& sect)
{
	xr_vector<attachable_hud_item*>::iterator it = m_pool.begin();
	xr_vector<attachable_hud_item*>::iterator it_e = m_pool.end();
	for (; it != it_e; ++it)
	{
		attachable_hud_item* itm = *it;
		if (itm->m_sect_name == sect)
			return itm;
	}
	attachable_hud_item* res = xr_new<attachable_hud_item>(this);
	res->load(sect);
	res->m_hand_motions.load(m_model, sect);
	m_pool.push_back(res);

	return res;
}

bool player_hud::allow_activation(CHudItem* item)
{
	if (m_attached_items[1])
		return m_attached_items[1]->m_parent_hud_item->CheckCompatibility(item);
	else
		return true;
}

void player_hud::attach_item(CHudItem* item)
{
	attachable_hud_item* pi = create_hud_item(item->HudSection());
	int item_idx = pi->m_attach_place_idx;

	if (m_attached_items[item_idx] != pi)
	{
		if (m_attached_items[item_idx])
			m_attached_items[item_idx]->m_parent_hud_item->on_b_hud_detach();

		m_attached_items[item_idx] = pi;
		pi->m_parent_hud_item = item;

		if (item_idx == 0 && m_attached_items[1])
			m_attached_items[1]->m_parent_hud_item->CheckCompatibility(item);

		item->on_a_hud_attach();
	}
	pi->m_parent_hud_item = item;
}

//sync anim of other part to selected part (1 = sync to left hand anim; 2 = sync to right hand anim)
void player_hud::re_sync_anim(u8 part)
{
	u32 bc = m_model->LL_PartBlendsCount(part);
	for (u32 bidx = 0; bidx < bc; ++bidx)
	{
		CBlend* BR = m_model->LL_PartBlend(part, bidx);
		if (!BR)
			continue;
			
		MotionID M = BR->motionID;

		u16 pc = m_model->partitions().count(); //same on both armatures
		for (u16 pid = 0; pid < pc; ++pid)
		{
			if (pid == 0)
			{
				CBlend* B = m_model->PlayCycle(0, M, TRUE);
				B->timeCurrent = BR->timeCurrent;
				B->speed = BR->speed;
			}
			else if (pid != part)
			{
				CBlend* B = m_model->PlayCycle(pid, M, TRUE);
				B->timeCurrent = BR->timeCurrent;
				B->speed = BR->speed;
			}
		}
	}
}

void player_hud::detach_item_idx(u16 idx)
{
	if (NULL == m_attached_items[idx]) return;

	m_attached_items[idx]->m_parent_hud_item->on_b_hud_detach();
	m_attached_items[idx]->m_parent_hud_item = NULL;
	m_attached_items[idx] = NULL;

	if (idx == 1)
	{
		if (m_attached_items[0])
			re_sync_anim(2);
		else
		{
			m_model->PlayCycle("hand_idle_doun");
		}
	}
	else if (idx == 0)
	{
		if (m_attached_items[1])
		{
			//fix for a rare case where the right hand stays visible on screen after detaching the right hand's attached item
			player_hud_motion* pm = m_attached_items[1]->m_hand_motions.find_motion("anm_idle");
			const motion_descr& M = pm->m_animations[0];
			m_model->PlayCycle(0, M.mid, false);
			m_model->PlayCycle(2, M.mid, false);
		}
		else
		{
			m_model->PlayCycle("hand_idle_doun");
		}
	}
	if (!m_attached_items[0] && !m_attached_items[1])
	{
		m_model->PlayCycle("hand_idle_doun");
	}
}

void player_hud::detach_item(CHudItem* item)
{
	if (NULL == item->HudItemData()) return;
	u16 item_idx = item->HudItemData()->m_attach_place_idx;

	if (m_attached_items[item_idx] == item->HudItemData())
	{
		detach_item_idx(item_idx);
	}
}

bool player_hud::allow_script_anim()
{
	if (m_attached_items[0] && m_attached_items[0]->m_parent_hud_item->IsPending())
		return false;
	else if (m_attached_items[1] && m_attached_items[1]->m_parent_hud_item->IsPending())
		return false;
	else if (script_anim_part != u8(-1))
		return false;

	return true;
}

void player_hud::calc_transform(u16 attach_slot_idx, const Fmatrix& offset, Fmatrix& result)
{
	Fmatrix ancor_m = m_model->dcast_PKinematics()->LL_GetTransform(m_ancors[attach_slot_idx]);
	result.mul(m_transform, ancor_m);
	result.mulB_43(offset);
}

bool player_hud::inertion_allowed()
{
	attachable_hud_item* hi = m_attached_items[0];
	if (hi)
	{
		bool res = (hi->m_parent_hud_item->HudInertionEnabled() && hi->m_parent_hud_item->HudInertionAllowed());
		return res;
	}
	return true;
}

void player_hud::OnMovementChanged(ACTOR_DEFS::EMoveCommand cmd)
{
	if (cmd == 0)
	{
		if (m_attached_items[0])
		{
			if (m_attached_items[0]->m_parent_hud_item->GetState() == CHUDState::eIdle)
				m_attached_items[0]->m_parent_hud_item->PlayAnimIdle();
		}
		if (m_attached_items[1])
		{
			if (m_attached_items[1]->m_parent_hud_item->GetState() == CHUDState::eIdle)
				m_attached_items[1]->m_parent_hud_item->PlayAnimIdle();
		}
	}
	else
	{
		if (m_attached_items[0])
			m_attached_items[0]->m_parent_hud_item->OnMovementChanged(cmd);

		if (m_attached_items[1])
			m_attached_items[1]->m_parent_hud_item->OnMovementChanged(cmd);
	}
}

static const float PITCH_OFFSET_R	= 0.017f;
static const float PITCH_OFFSET_N	= 0.012f;
static const float PITCH_OFFSET_D	= 0.02f;
static const float ORIGIN_OFFSET	= -0.05f;
static const float TENDTO_SPEED		= 5.f;
void player_hud::update_inertion(Fmatrix& trans)
{
	if ( inertion_allowed() )
	{
		Fmatrix xform;
		Fvector& origin = trans.c; 
		xform = trans;

		static Fvector st_last_dir = {0,0,0};

		// calc difference
		Fvector diff_dir;
		diff_dir.sub(xform.k, st_last_dir);

		// clamp by PI_DIV_2
		Fvector last; last.normalize_safe(st_last_dir);
		float dot = last.dotproduct(xform.k);
		if (dot < EPS){
			Fvector v0;
			v0.crossproduct(st_last_dir,xform.k);
			st_last_dir.crossproduct(xform.k,v0);
			diff_dir.sub(xform.k, st_last_dir);
		}

		// tend to forward
		st_last_dir.mad(diff_dir,TENDTO_SPEED*Device.fTimeDelta);
		origin.mad(diff_dir,ORIGIN_OFFSET);

		// pitch compensation
		float pitch = angle_normalize_signed(xform.k.getP());
		origin.mad(xform.k,	-pitch * PITCH_OFFSET_D);
		origin.mad(xform.i,	-pitch * PITCH_OFFSET_R);
		origin.mad(xform.j,	-pitch * PITCH_OFFSET_N);
	}
}
