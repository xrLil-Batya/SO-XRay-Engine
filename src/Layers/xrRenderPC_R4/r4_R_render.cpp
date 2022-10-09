#include "stdafx.h"
#include "../../xrEngine/igame_persistent.h"
#include "../xrRender/FBasicVisual.h"
#include "../../xrEngine/customhud.h"
#include "../../xrEngine/xr_object.h"
#include "tbb/parallel_sort.h"
#include "../xrRender/QueryHelper.h"

#include "tbb/task.h"
#include "tbb/task_group.h"

float CPU_wait_GPU_lastFrame_ = 0.f;

IC	bool	pred_sp_sort	(ISpatial*	_1, ISpatial* _2)
{
	float	d1		= _1->spatial.sphere.P.distance_to_sqr	(Device.vCameraPosition);
	float	d2		= _2->spatial.sphere.P.distance_to_sqr	(Device.vCameraPosition);
	return	d1<d2	;
}

void CRender::dynamics_visible_check(const size_t * p_from, const size_t * p_to)
{
	//Msg("* Thread: [%d] [%d]", p_from, p_to);

	size_t i = *p_from;

	for(i; i<*p_to; i++)
	{
		ISpatial* spatial = lstRenderables[i];
		CSector* sector = (CSector*)spatial->spatial.sector;

		if (sector)
		{
			if (spatial->spatial.type & STYPE_LIGHTSOURCE)
			{
				// lightsource
				light* L = (light*)(spatial->dcast_Light());

				VERIFY(L);

				float	lod = L->get_LOD();
				if (lod > EPS_L)
				{
					vis_data& vis = L->get_homdata();
					if (HOM.visible(vis))	renderable_lights.push_back(L);
				}
			}
			else if (spatial->spatial.type & STYPE_RENDERABLE && PortalTraverser.i_marker == sector->r_marker)
			{
				bool vis_frust = false;

				for (u32 v_it = 0; v_it < sector->r_frustums.size(); v_it++)
				{
					CFrustum& view = sector->r_frustums[v_it];
					if (view.testSphere_dirty(spatial->spatial.sphere.P, spatial->spatial.sphere.R))
					{
						vis_frust = true;
						break;
					}
				}

				if (vis_frust)
				{
					IRenderable* renderable = spatial->dcast_Renderable();
					if (renderable)
					{
						vis_data& v_orig = ((dxRender_Visual*)renderable->renderable.visual)->vis;
						vis_data v_copy = v_orig;
						v_copy.box.xform(renderable->renderable.xform);
						BOOL			bVisible = HOM.visible(v_copy);

						v_orig.marker = v_copy.marker;
						v_orig.accept_frame = v_copy.accept_frame;
						v_orig.hom_frame = v_copy.hom_frame;
						v_orig.hom_tested = v_copy.hom_tested;

						if (bVisible)
						{
							renderable_objects.push_back(renderable);
						}
					}
				}
			}
		}

	}
}

void CRender::render_main	(Fmatrix&	m_ViewProjection, bool _fportals)
{
	PIX_EVENT(render_main);
//	Msg						("---begin");
	marker					++;

	// Calculate sector(s) and their objects
	if (pLastSector)		{
		//!!!
		//!!! BECAUSE OF PARALLEL HOM RENDERING TRY TO DELAY ACCESS TO HOM AS MUCH AS POSSIBLE
		//!!!
		{
			// Traverse object database
			g_SpatialSpace->q_frustum
				(
				lstRenderables,
				ISpatial_DB::O_ORDERED,
				STYPE_RENDERABLE + STYPE_LIGHTSOURCE,
				ViewBase
				);

			// (almost) Exact sorting order (front-to-back)
			std::sort(lstRenderables.begin(),lstRenderables.end(),pred_sp_sort);

			// Determine visibility for dynamic part of scene
			set_Object							(0);
			u32 uID_LTRACK						= 0xffffffff;
			if (phase==PHASE_NORMAL)			{
				uLastLTRACK	++;
				if (lstRenderables.size())		uID_LTRACK	= uLastLTRACK%lstRenderables.size();

				// update light-vis for current entity / actor
				CObject*	O					= g_pGameLevel->CurrentViewEntity();
				if (O)		{
					CROS_impl*	R					= (CROS_impl*) O->ROS();
					if (R)		R->update			(O);
				}

				// update light-vis for selected entity
				// track lighting environment
				if (lstRenderables.size())		{
					IRenderable*	renderable		= lstRenderables[uID_LTRACK]->dcast_Renderable	();
					if (renderable)	{
						CROS_impl*		T = (CROS_impl*)renderable->renderable_ROS	();
						if (T)			T->update	(renderable);
					}
				}
			}
		}

		// Traverse sector/portal structure
		PortalTraverser.traverse	
			(
			pLastSector,
			ViewBase,
			Device.vCameraPosition,
			m_ViewProjection,
			CPortalTraverser::VQ_HOM + CPortalTraverser::VQ_SSA + CPortalTraverser::VQ_FADE
			//. disabled scissoring (HW.Caps.bScissor?CPortalTraverser::VQ_SCISSOR:0)	// generate scissoring info
			);

		// Determine visibility for static geometry hierrarhy
		for (u32 s_it=0; s_it<PortalTraverser.r_sectors.size(); s_it++)
		{
			CSector*	sector		= (CSector*)PortalTraverser.r_sectors[s_it];
			dxRender_Visual*	root	= sector->root();
			for (u32 v_it=0; v_it<sector->r_frustums.size(); v_it++)	{
				set_Frustum			(&(sector->r_frustums[v_it]));
				add_Geometry		(root);
			}
		}

		renderable_objects.clear();
		renderable_lights.clear();

		for (size_t i = 0; i < lstRenderables.size(); ++i)
		{
			ISpatial* spatial = lstRenderables[i];
			spatial->spatial_updatesector();
		}

		size_t p_cnt = lstRenderables.size();

		tbb::task_group ObjectsTasks;
		size_t nWorkers = CPU::ID.n_threads;

		size_t nSlice = p_cnt / 32;
		size_t nStep = ((p_cnt - nSlice) / nWorkers);

		for (size_t i = 0; i < nWorkers; ++i)
		{
			size_t p_from = i * nStep;
			size_t p_to = (i == (nWorkers - 1)) ? p_cnt : (p_from + nStep);

			ObjectsTasks.run([&, p_from, p_to]()
				{
					dynamics_visible_check(&p_from, &p_to);
				});
		}

		ObjectsTasks.wait();

		if (!renderable_objects.empty())
		{
			for (IRenderable* renderable : renderable_objects)
			{
				set_Object(renderable);
				renderable->renderable_Render();
				set_Object(0);
			}
		}

		if (!renderable_lights.empty())
		{
			for (light* light : renderable_lights)
			{
				Lights.add_light(light);
			}
		}

		if (g_pGameLevel && (phase==PHASE_NORMAL))	g_hud->Render_Last();		// HUD
	}
	else
	{
		set_Object									(0);
		if (g_pGameLevel && (phase==PHASE_NORMAL))	g_hud->Render_Last();		// HUD
	}
}

void CRender::render_menu	()
{
	PIX_EVENT(render_menu);
	//	Globals
	RCache.set_CullMode				(CULL_CCW);
	RCache.set_Stencil				(FALSE);
	RCache.set_ColorWriteEnable		();

	// Main Render
	{
		Target->u_setrt(Target->rt_Generic_0,0,0,HW.pBaseZB);		// LDR RT
		g_pGamePersistent->OnRenderPPUI_main()	;	// PP-UI
	}

	// Distort
	{
		FLOAT ColorRGBA[4] = {127.0f/255.0f, 127.0f/255.0f, 0.0f, 127.0f/255.0f};
		Target->u_setrt(Target->rt_Generic_1,0,0,HW.pBaseZB);		// Now RT is a distortion mask
		HW.pContext->ClearRenderTargetView(Target->rt_Generic_1->pRT, ColorRGBA);		
		g_pGamePersistent->OnRenderPPUI_PP	()	;	// PP-UI
	}

	// Actual Display
	Target->u_setrt					( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
	RCache.set_Shader				( Target->s_menu	);
	RCache.set_Geometry				( Target->g_menu	);

	Fvector2						p0,p1;
	u32								Offset;
	u32		C						= color_rgba	(255,255,255,255);
	float	_w						= float(Device.dwWidth);
	float	_h						= float(Device.dwHeight);
	float	d_Z						= EPS_S;
	float	d_W						= 1.f;
	p0.set							(.5f/_w, .5f/_h);
	p1.set							((_w+.5f)/_w, (_h+.5f)/_h );

	FVF::TL* pv						= (FVF::TL*) RCache.Vertex.Lock	(4,Target->g_menu->vb_stride,Offset);
	pv->set							(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
	pv->set							(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
	pv->set							(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
	pv->set							(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
	RCache.Vertex.Unlock			(4,Target->g_menu->vb_stride);
	RCache.Render					(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}

extern u32 g_r;
void CRender::Render		()
{
	PIX_EVENT(CRender_Render);

	g_r						= 1;
	VERIFY					(0==mapDistort.size());

	Target->needClearAccumulator = true;

	Target->u_setrt(Device.dwWidth, Device.dwHeight, HW.pBaseRT, NULL, NULL, HW.pBaseZB); // Set up HW base as RT and ZB

	rmNormal();

	bool	_menu_pp		= g_pGamePersistent?g_pGamePersistent->OnRenderPPUI_query():false;
	if (currentViewPort == MAIN_VIEWPORT && _menu_pp)
	{
		render_menu			()	;
		return					;
	};

	IMainMenu*	pMainMenu = g_pGamePersistent?g_pGamePersistent->m_pMainMenu:0;
	bool	bMenu = pMainMenu?pMainMenu->CanSkipSceneRendering():false;

	if( !(g_pGameLevel && g_hud)
		|| bMenu)	
	{
		Target->u_setrt				( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
		return;
	}

	if( m_bFirstFrameAfterReset )
	{
		m_bFirstFrameAfterReset = false;
		return;
	}

//.	VERIFY					(g_pGameLevel && g_pGameLevel->pHUD);

	// Configure
	RImplementation.o.distortion				= FALSE;		// disable distorion
	Fcolor					sun_color			= ((light*)Lights.sun_adapted._get())->color;
	BOOL					bSUN				= ps_r2_ls_flags.test(R2FLAG_SUN) && (u_diffuse2s(sun_color.r,sun_color.g,sun_color.b)>EPS);
	if (o.sunstatic)		bSUN				= FALSE;
	// Msg						("sstatic: %s, sun: %s",o.sunstatic?;"true":"false", bSUN?"true":"false");

	// HOM
	ViewBase.CreateFromMatrix					(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);
	View										= 0;
	if (!ps_r2_ls_flags.test(R2FLAG_EXP_MT_CALC))	{
		HOM.Enable									();
		HOM.Render									(ViewBase);
	}

	//******* Z-prefill calc - DEFERRER RENDERER
	if (ps_r2_ls_flags.test(R2FLAG_ZFILL))		
	{
		PIX_EVENT(DEFER_Z_FILL);
		Device.Statistic->RenderCALC.Begin			();
		float		z_distance	= ps_r2_zfill		;
		Fmatrix		m_zfill, m_project				;
		m_project.build_projection	(
			deg2rad(Device.fFOV/* *Device.fASPECT*/), 
			Device.fASPECT, VIEWPORT_NEAR, 
			z_distance * g_pGamePersistent->Environment().CurrentEnv->far_plane);
		m_zfill.mul	(m_project,Device.mView);
		r_pmask										(true,false);	// enable priority "0"
		set_Recorder								(NULL)		;
		phase										= PHASE_SMAP;
		render_main									(m_zfill,false)	;
		r_pmask										(true,false);	// disable priority "1"
		Device.Statistic->RenderCALC.End				( )			;

		// flush
		Target->phase_scene_prepare					();
		RCache.set_ColorWriteEnable					(FALSE);
		r_dsgraph_render_graph						(0);
		RCache.set_ColorWriteEnable					( );
	} 
	else 
	{
		Target->phase_scene_prepare					();
	}

	if (currentViewPort == SECONDARY_WEAPON_SCOPE)
	{
		Target->phase_cut();
	}

	//*******
	// Sync point
	Device.Statistic->RenderDUMP_Wait_S.Begin	();

	CTimer	T1;							T1.Start();

	if (currentViewPort == MAIN_VIEWPORT)
	{
		CTimer	T;							T.Start	();
		BOOL	result						= FALSE;
		HRESULT	hr							= S_FALSE;
		//while	((hr=q_sync_point[q_sync_count]->GetData	(&result,sizeof(result),D3DGETDATA_FLUSH))==S_FALSE) {
		while	((hr=GetData (q_sync_point[q_sync_count], &result,sizeof(result)))==S_FALSE) 
		{
			if (!SwitchToThread())			Sleep(ps_r2_wait_sleep);
			if (T.GetElapsed_ms() > 500)	{
				result	= FALSE;
				break;
			}
		}
		q_sync_count = (q_sync_count + 1) % HW.Caps.iGPUNum;
		CHK_DX(EndQuery(q_sync_point[q_sync_count]));
	}
	Device.Statistic->RenderDUMP_Wait_S.End		();

	CPU_wait_GPU_lastFrame_ = T1.GetElapsed_sec() * 1000.f;

	//******* Main calc - DEFERRER RENDERER
	// Main calc
	Device.Statistic->RenderCALC.Begin			();
	r_pmask										(true,false,true);	// enable priority "0",+ capture wmarks
	if (bSUN)									set_Recorder	(&main_coarse_structure);
	else										set_Recorder	(NULL);
	phase										= PHASE_NORMAL;
	render_main									(Device.mFullTransform,true);
	set_Recorder								(NULL);
	r_pmask										(true,false);	// disable priority "1"
	Device.Statistic->RenderCALC.End			();

	BOOL	split_the_scene_to_minimize_wait		= FALSE;
	if (ps_r2_ls_flags.test(R2FLAG_EXP_SPLIT_SCENE))	split_the_scene_to_minimize_wait=TRUE;

	//******* Main render :: PART-0	-- first
	if (!split_the_scene_to_minimize_wait)
	{
		PIX_EVENT(DEFER_PART0_NO_SPLIT);
		// level, DO NOT SPLIT
		Target->phase_scene_begin				();
		r_dsgraph_render_hud					();
		r_dsgraph_render_graph					(0);
		r_dsgraph_render_lods					(true,true);
		if(Details)	Details->Render				();
		Target->phase_scene_end					();
	} 
	else 
	{
		PIX_EVENT(DEFER_PART0_SPLIT);
		// level, SPLIT
		Target->phase_scene_begin				();
		r_dsgraph_render_graph					(0);
		Target->disable_aniso					();
	}

	//******* Occlusion testing of volume-limited light-sources
	Target->phase_occq							();
	LP_normal.clear								();
	LP_pending.clear							();
   if( RImplementation.o.dx10_msaa )
      RCache.set_ZB( RImplementation.Target->rt_MSAADepth->pZRT );
	{
		PIX_EVENT(DEFER_TEST_LIGHT_VIS);
		// perform tests
		u32	count			= 0;
		light_Package& LP = Lights.package;

		// stats
		stats.l_shadowed	= LP.v_shadowed.size();
		stats.l_unshadowed	= LP.v_point.size() + LP.v_spot.size();
		stats.l_total		= stats.l_shadowed + stats.l_unshadowed;

		// perform tests
		count				= std::max((size_t)count,LP.v_point.size());
		count				= std::max((size_t)count,LP.v_spot.size());
		count				= std::max((size_t)count,LP.v_shadowed.size());
		for (u32 it=0; it<count; it++)	{
			if (it<LP.v_point.size())		{
				light*	L			= LP.v_point	[it];
				L->vis_prepare		();
				if (L->vis.pending)	LP_pending.v_point.push_back	(L);
				else				LP_normal.v_point.push_back		(L);
			}
			if (it<LP.v_spot.size())		{
				light*	L			= LP.v_spot		[it];
				L->vis_prepare		();
				if (L->vis.pending)	LP_pending.v_spot.push_back		(L);
				else				LP_normal.v_spot.push_back		(L);
			}
			if (it<LP.v_shadowed.size())	{
				light*	L			= LP.v_shadowed	[it];
				L->vis_prepare		();
				if (L->vis.pending)	LP_pending.v_shadowed.push_back	(L);
				else				LP_normal.v_shadowed.push_back	(L);
			}
		}
	}
	LP_normal.sort							();
	LP_pending.sort							();

   //******* Main render :: PART-1 (second)
	if (split_the_scene_to_minimize_wait)	
	{
		PIX_EVENT(DEFER_PART1_SPLIT);
		// skybox can be drawn here
		if (0)
		{

			if( !RImplementation.o.dx10_msaa )
				Target->u_setrt		( Target->rt_Generic_0,	Target->rt_Generic_1,0,HW.pBaseZB );
			else
				Target->u_setrt		( Target->rt_Generic_0_r,	Target->rt_Generic_1,0,RImplementation.Target->rt_MSAADepth->pZRT );
			RCache.set_CullMode	( CULL_NONE );
			RCache.set_Stencil	( FALSE		);

			// draw skybox
			RCache.set_ColorWriteEnable					();
			//CHK_DX(HW.pDevice->SetRenderState			( D3DRS_ZENABLE,	FALSE				));
			RCache.set_Z(FALSE);
			g_pGamePersistent->Environment().RenderSky	();
			//CHK_DX(HW.pDevice->SetRenderState			( D3DRS_ZENABLE,	TRUE				));
			RCache.set_Z(TRUE);
		}

		// level
		Target->phase_scene_begin				();
		r_dsgraph_render_hud					();
		r_dsgraph_render_lods					(true,true);
		if(Details)	Details->Render				();
		Target->phase_scene_end					();
	}

	if (g_hud && g_hud->RenderActiveItemUIQuery())
	{
		Target->phase_wallmarks();
		r_dsgraph_render_hud_ui();
	}

	// Wall marks
	if(Wallmarks)	
	{
		PIX_EVENT(DEFER_WALLMARKS);
		Target->phase_wallmarks					();
		g_r										= 0;
		Wallmarks->Render						();				// wallmarks has priority as normal geometry
	}

	// Update incremental shadowmap-visibility solver
	{
		PIX_EVENT(DEFER_FLUSH_OCCLUSION);
		u32 it=0;
		for (it=0; it<Lights_LastFrame.size(); it++)	{
			if (0==Lights_LastFrame[it])	continue	;
			try {
				Lights_LastFrame[it]->svis.flushoccq()	;
			} catch (...)
			{
				Msg	("! Failed to flush-OCCq on light [%d] %X",it,*(u32*)(&Lights_LastFrame[it]));
			}
		}
		Lights_LastFrame.clear	();
	}

   // full screen pass to mark msaa-edge pixels in highest stencil bit
   if( RImplementation.o.dx10_msaa )
   {
	   PIX_EVENT( MARK_MSAA_EDGES );
      Target->mark_msaa_edges();
   }

	//	TODO: DX10: Implement DX10 rain.
	if (ps_r2_ls_flags.test(R3FLAG_DYN_WET_SURF))
	{
		PIX_EVENT(DEFER_RAIN);
		render_rain();
	}

	// Directional light - fucking sun
	if (bSUN)	
	{
		PIX_EVENT(DEFER_SUN);
		RImplementation.stats.l_visible		++;
		if( !ps_r2_ls_flags_ext.is(R2FLAGEXT_SUN_OLD))
			render_sun_cascades					();
		else
		{
			render_sun_near						();
			render_sun							();
			render_sun_filtered					();
		}
		Target->accum_direct_blend			();
	}

	{
		PIX_EVENT(DEFER_SELF_ILLUM);
		Target->phase_accumulator			();
		// Render emissive geometry, stencil - write 0x0 at pixel pos
		RCache.set_xform_project			(Device.mProject); 
		RCache.set_xform_view				(Device.mView);
		// Stencil - write 0x1 at pixel pos - 
      if( !RImplementation.o.dx10_msaa )
		   RCache.set_Stencil					( TRUE,D3DCMP_ALWAYS,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
      else
		   RCache.set_Stencil					( TRUE,D3DCMP_ALWAYS,0x01,0xff,0x7f,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		//RCache.set_Stencil				(TRUE,D3DCMP_ALWAYS,0x00,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		RCache.set_CullMode					(CULL_CCW);
		RCache.set_ColorWriteEnable			();
		RImplementation.r_dsgraph_render_emissive();
	}

	// Lighting, non dependant on OCCQ
	{
		PIX_EVENT(DEFER_LIGHT_NO_OCCQ);
		Target->phase_accumulator				();
		HOM.Disable								();
		render_lights							(LP_normal);
	}

	// Lighting, dependant on OCCQ
	{
		PIX_EVENT(DEFER_LIGHT_OCCQ);
		render_lights							(LP_pending);
	}

	// Postprocess
	{
		PIX_EVENT(DEFER_LIGHT_COMBINE);
		Target->phase_combine					();
	}

	VERIFY	(0==mapDistort.size());
}

void CRender::render_forward				()
{
	VERIFY	(0==mapDistort.size());
	RImplementation.o.distortion				= RImplementation.o.distortion_enabled;	// enable distorion

	//******* Main render - second order geometry (the one, that doesn't support deffering)
	//.todo: should be done inside "combine" with estimation of of luminance, tone-mapping, etc.
	{
		// level
		r_pmask									(false,true);			// enable priority "1"
		phase									= PHASE_NORMAL;
		render_main								(Device.mFullTransform,false);//
		//	Igor: we don't want to render old lods on next frame.
		mapLOD.clear							();
		r_dsgraph_render_graph					(1)	;					// normal level, secondary priority
		PortalTraverser.fade_render				()	;					// faded-portals
		r_dsgraph_render_sorted					()	;					// strict-sorted geoms
		g_pGamePersistent->Environment().RenderLast()	;					// rain/thunder-bolts
	}

	RImplementation.o.distortion				= FALSE;				// disable distorion
}

// Перед началом рендера мира --#SM+#-- +SecondVP+
void CRender::BeforeWorldRender() {}

ENGINE_API extern BOOL debugSecondVP;

// После рендера мира и пост-эффектов --#SM+#-- +SecondVP+
void CRender::AfterWorldRender(const bool save_bb_before_ui)
{
	if (currentViewPort == SECONDARY_WEAPON_SCOPE)
	{
		// Делает копию бэкбуфера (текущего экрана) в рендер-таргет второго вьюпорта (для использования в 3д прицеле либо в рендер-таргет вьюпорта, из которого мы вернем заберем кадр после рендера ui. Именно этот кадр будет позже выведен на экран.)
		ID3DTexture2D* pBuffer{};
		HW.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBuffer));
		HW.pContext->CopyResource(save_bb_before_ui ? Target->rt_BeforeUi->pSurface : Target->rt_secondVP->pSurface, pBuffer);
		pBuffer->Release(); // Корректно очищаем ссылку на бэкбуфер (иначе игра зависнет в опциях)
	}
}

void CRender::AfterUIRender()
{
	// Делает копию бэкбуфера (текущего экрана) в рендер-таргет второго вьюпорта (для использования в пда)
	ID3DTexture2D* pBuffer{};
	HW.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBuffer));
	HW.pContext->CopyResource(Target->rt_secondVP->pSurface, pBuffer);
	pBuffer->Release(); // Корректно очищаем ссылку на бэкбуфер (иначе игра зависнет в опциях)

	// Возвращаем на экран кадр, который сохранили до рендера ui для пда
	ID3DTexture2D* pBuffer2{};
	HW.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBuffer2));
	HW.pContext->CopyResource(pBuffer2, Target->rt_BeforeUi->pSurface);
	pBuffer2->Release(); // Корректно очищаем ссылку на бэкбуфер (иначе игра зависнет в опциях)
}