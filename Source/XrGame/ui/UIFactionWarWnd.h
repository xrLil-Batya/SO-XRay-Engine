#pragma once
#include "UIWindow.h"
#include "UIWndCallback.h"
#include "../../xrServerEntities/associative_vector.h"
#include "../GameTaskDefs.h"
#include "UICheckButton.h"

class CUIMapWnd;
class CUIStatic;
class CGameTask;
class CUIXml;
class CUIEncyclopediaItem;
class CUI3tButton;
class CUIListBox;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUICheckButton;
class UITaskListWnd;
class UIMapLegend;
class UIHint;
class CUIScrollView;

class CUIEncyclopediaWnd			:	public CUIWindow, 
									public CUIWndCallback
{
private:
	typedef CUIWindow		inherited;

	CUIFrameWindow*			m_background;
	CUIFrameWindow*			m_buttons_background;
	CUIFrameWindow*			m_categories_background;
	CUIFrameWindow*			m_descriptions_background;
	CUIFrameWindow*			m_buttons_over;
	CUIFrameWindow*			m_categories_over;
	CUIFrameWindow*			m_descriptions_over;
	
	CUITextWnd*				m_center_caption;

	CUI3tButton*			m_btn_notes;
	CUI3tButton*			m_btn_barter;
	CUI3tButton*			m_btn_gived_info;
	CUI3tButton*			m_btn_passwords;
	CUI3tButton*			m_btn_recipes;
	CUI3tButton*			m_btn_anomalis;
	CUI3tButton*			m_btn_arts;
	CUI3tButton*			m_btn_monsters;
	CUI3tButton*			m_btn_locs;
	CUI3tButton*			m_btn_recipes_m;

	CUITextWnd*				m_category_caption;
	CUITextWnd*				m_description_caption;
	CUIListBox*				m_lb_category;
	CUITextWnd*				m_t_description;
	CUIStatic*				m_t_picture;
	CUIScrollView*			m_scroll_descriptions;

public:
	UIHint*					hint_wnd;

public:
								CUIEncyclopediaWnd				();
	virtual						~CUIEncyclopediaWnd				();
	virtual void				SendMessage				(CUIWindow* pWnd, s16 msg, void* pData);
			void				Init					();
	virtual void				Show					(bool status);
	virtual void				Reset					(bool all);

private:


	void	xr_stdcall	OnBtnNotes				(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnBarter				(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnGivedInfo			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnPasswords			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnRecipes			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnAnomalis			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnArts				(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnMonsters			(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnLocs				(CUIWindow* w, void* d);
	void	xr_stdcall	OnBtnRecipesM			(CUIWindow* w, void* d);
	void	xr_stdcall	OnListCategoryClick		(CUIWindow* w, void* d);
};