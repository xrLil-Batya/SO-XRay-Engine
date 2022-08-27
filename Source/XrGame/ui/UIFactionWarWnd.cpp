#include "stdafx.h"
#include "pch_script.h"
#include "UIFactionWarWnd.h"
#include "UIMapWnd.h"
#include "object_broker.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "UI3tButton.h"
#include "UIFrameLineWnd.h"
#include "UISecondTaskWnd.h"
#include "UIMapLegend.h"
#include "UIHelper.h"
#include "UIHint.h"
#include "UIListBox.h"
#include "UIListBoxItem.h"

#include "../gametask.h"
#include "../map_location.h"
#include "../map_location_defs.h"
#include "../map_manager.h"
#include "UIInventoryUtilities.h"
#include "../string_table.h"
#include "../level.h"
#include "../gametaskmanager.h"
#include "../actor.h"
#include "UICheckButton.h"

#define  PDA_ENCYCLOPEDIA_XML		"pda_encyclopedia.xml"
CUIEncyclopediaWnd::CUIEncyclopediaWnd()
{
	hint_wnd = NULL;
	Init();
}

CUIEncyclopediaWnd::~CUIEncyclopediaWnd()
{
}

void CUIEncyclopediaWnd::Init()
{
	CUIXml							xml;
	xml.Load						(CONFIG_PATH, UI_PATH, PDA_ENCYCLOPEDIA_XML);
	VERIFY							(hint_wnd);

	CUIXmlInit::InitWindow			(xml, "main_wnd", 0, this);
	
	m_background				= UIHelper::CreateFrameWindow(xml, "background", this);
	m_center_caption			= UIHelper::CreateTextWnd( xml, "center_caption", this );

	m_buttons_background		= UIHelper::CreateFrameWindow(xml, "buttons_background", this);
	m_categories_background		= UIHelper::CreateFrameWindow(xml, "categories_background", this);
	m_descriptions_background	= UIHelper::CreateFrameWindow(xml, "descriptions_background", this);
	m_buttons_over				= UIHelper::CreateFrameWindow(xml, "buttons_over", this);
	m_categories_over			= UIHelper::CreateFrameWindow(xml, "categories_over", this);
	m_descriptions_over			= UIHelper::CreateFrameWindow(xml, "descriptions_over", this);

	m_btn_notes				= UIHelper::Create3tButton( xml, "notes_button", this );
	AddCallback				(m_btn_notes, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnBtnNotes));
	m_btn_barter			= UIHelper::Create3tButton( xml, "barter_button", this );
	AddCallback				(m_btn_barter, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnBtnBarter));
	m_btn_gived_info		= UIHelper::Create3tButton( xml, "gived_info_button", this );
	AddCallback				(m_btn_gived_info, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnBtnGivedInfo));
	m_btn_passwords			= UIHelper::Create3tButton( xml, "passwords_button", this );
	AddCallback				(m_btn_passwords, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnBtnPasswords));
	m_btn_recipes			= UIHelper::Create3tButton( xml, "recipes_button", this );
	AddCallback				(m_btn_recipes, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnBtnRecipes));
	m_btn_anomalis			= UIHelper::Create3tButton( xml, "anomalis_button", this );
	AddCallback				(m_btn_anomalis, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnBtnAnomalis));
	m_btn_arts				= UIHelper::Create3tButton( xml, "arts_button", this );
	AddCallback				(m_btn_arts, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnBtnArts));
	m_btn_monsters			= UIHelper::Create3tButton( xml, "monsters_button", this );
	AddCallback				(m_btn_monsters, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnBtnMonsters));
	m_btn_locs				= UIHelper::Create3tButton( xml, "locs_button", this );
	AddCallback				(m_btn_locs, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnBtnLocs));
	m_btn_recipes_m			= UIHelper::Create3tButton( xml, "recipes_m_button", this );
	AddCallback				(m_btn_recipes_m, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnBtnRecipesM));

	m_category_caption		= UIHelper::CreateTextWnd( xml, "categories_caption", this );
	m_description_caption	= UIHelper::CreateTextWnd( xml, "descriptions_caption", this );
	
	m_lb_category				= UIHelper::CreateListBox( xml, "list_categories", this );
	AddCallback					(m_lb_category,LIST_ITEM_CLICKED,CUIWndCallback::void_function(this, &CUIEncyclopediaWnd::OnListCategoryClick));
	
	m_scroll_descriptions = xr_new<CUIScrollView>();
	CUIXmlInit::InitScrollView(xml, "descriptions_wnd", 0, m_scroll_descriptions);
	m_scroll_descriptions->SetAutoDelete(true);
	AttachChild(m_scroll_descriptions);

	m_t_description				= UIHelper::CreateTextWnd( xml, "list_descriptions", NULL );
	m_t_picture					= UIHelper::CreateStatic(xml, "list_picture", NULL);
	
	Reset(true);
}


void CUIEncyclopediaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent( pWnd, msg, pData );
}

void CUIEncyclopediaWnd::Show(bool status)
{
	inherited::Show			(status);
	Reset(true);
}
void CUIEncyclopediaWnd::Reset(bool all)
{
	m_description_caption->SetText("");
	m_t_description->SetText("");
	m_t_picture->InitTexture("ui_encyclopedia_not_picture");
	if (all){
		m_lb_category->Clear();
		m_category_caption->SetText("");
	}
}
//------- list box callbacks -------------------------------------------
void CUIEncyclopediaWnd::OnListCategoryClick(CUIWindow* w, void* d)
{
	Msg("List");
	Reset(false);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.list_click", funct)) 
		funct(m_lb_category,m_t_description,m_description_caption,m_t_picture,m_scroll_descriptions);
}

//------- buttons callbacks -------------------------------------------
void CUIEncyclopediaWnd::OnBtnNotes(CUIWindow* w, void* d)
{
	Reset(true);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.button_click", funct)) 
		funct("notes",m_lb_category,m_category_caption,m_t_picture,m_scroll_descriptions);
}
void CUIEncyclopediaWnd::OnBtnBarter(CUIWindow* w, void* d)
{
	Reset(true);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.button_click", funct)) 
		funct("barter",m_lb_category,m_category_caption,m_t_picture,m_scroll_descriptions);
}
void CUIEncyclopediaWnd::OnBtnGivedInfo(CUIWindow* w, void* d)
{
	Reset(true);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.button_click", funct)) 
		funct("gived_info",m_lb_category,m_category_caption,m_t_picture,m_scroll_descriptions);
}
void CUIEncyclopediaWnd::OnBtnPasswords(CUIWindow* w, void* d)
{
	Reset(true);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.button_click", funct)) 
		funct("passwords",m_lb_category,m_category_caption,m_t_picture,m_scroll_descriptions);
}
void CUIEncyclopediaWnd::OnBtnRecipes(CUIWindow* w, void* d)
{
	Reset(true);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.button_click", funct)) 
		funct("recipes",m_lb_category,m_category_caption,m_t_picture,m_scroll_descriptions);
}
void CUIEncyclopediaWnd::OnBtnAnomalis(CUIWindow* w, void* d)
{
	Reset(true);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.button_click", funct)) 
		funct("anomalis",m_lb_category,m_category_caption,m_t_picture,m_scroll_descriptions);
}
void CUIEncyclopediaWnd::OnBtnArts(CUIWindow* w, void* d)
{
	Reset(true);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.button_click", funct)) 
		funct("arts",m_lb_category,m_category_caption,m_t_picture,m_scroll_descriptions);
}
void CUIEncyclopediaWnd::OnBtnMonsters(CUIWindow* w, void* d)
{
	Reset(true);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.button_click", funct)) 
		funct("monsters",m_lb_category,m_category_caption,m_t_picture,m_scroll_descriptions);
}
void CUIEncyclopediaWnd::OnBtnLocs(CUIWindow* w, void* d)
{
	Reset(true);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.button_click", funct)) 
		funct("locs",m_lb_category,m_category_caption,m_t_picture,m_scroll_descriptions);
}
void CUIEncyclopediaWnd::OnBtnRecipesM(CUIWindow* w, void* d)
{
	Reset(true);
	luabind::functor<void> funct;
	if (ai().script_engine().functor("ui_pda_encyclopedia.button_click", funct)) 
		funct("recipes_m",m_lb_category,m_category_caption,m_t_picture,m_scroll_descriptions);
}