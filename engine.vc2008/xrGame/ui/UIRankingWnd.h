////////////////////////////////////////////////////////////////////////////
//	Module 		: UIRankingWnd.h
//	Created 	: 17.01.2008
//	Author		: Evgeniy Sokolov
//	Description : UI Ranking window class
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "UIWindow.h"
#include "UIWndCallback.h"
//#include "UIRankFaction.h"
#include "UIAchievements.h"

class CUIStatic;
class CUIXml;
class CUIProgressBar;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUICharacterInfo;
class CUIScrollView;

class CUIRankingWnd : public CUIWindow, public CUIWndCallback
{
private:
	typedef CUIWindow	inherited;

	CUIFrameWindow*		m_background;
	CUIFrameWindow*		m_down_background;
	CUIFrameWindow*		m_icon_overlay;

	CUICharacterInfo*	m_actor_ch_info;

	CUITextWnd*			m_money_caption;
	CUITextWnd*			m_money_value;

	CUITextWnd*			m_center_caption;

	CUIScrollView*		m_achievements;
	CUIFrameWindow*		m_achievements_background;

	DEFINE_VECTOR		(CUIAchievements*, ACHIEVES_VEC, ACHIEVES_VEC_IT);
	ACHIEVES_VEC		m_achieves_vec;

	enum				{ max_stat_info = 15 };
	CUITextWnd*			m_stat_caption[max_stat_info];
	CUITextWnd*			m_stat_info[max_stat_info];

	u32					m_delay;
	u32					m_previous_time;
	u32					m_stat_count;

public:
						CUIRankingWnd			();
	virtual				~CUIRankingWnd			();

	virtual void 		Show					(bool status);
	virtual void		Update					();
	virtual void		DrawHint				();
	virtual void		ResetAll				();

			void		Init					();
			void		update_info				();

protected:
			void		add_achievement			(CUIXml& xml, shared_str const& faction_id);
			void		get_statistic			();

}; // class CUIRankingWnd
