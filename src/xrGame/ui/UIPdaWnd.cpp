#include "stdafx.h"
#include "UIPdaWnd.h"
#include "../Pda.h"

#include "xrUIXmlParser.h"
#include "UIXmlInit.h"
#include "UIInventoryUtilities.h"

#include "../level.h"
#include "UIGameCustom.h"

#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "UITabControl.h"
#include "UIMapWnd.h"
#include "UIFrameLineWnd.h"
#include "object_broker.h"
#include "UIMessagesWindow.h"
#include "UIMainIngameWnd.h"
#include "UITabButton.h"
#include "UIAnimatedStatic.h"

#include "UIHelper.h"
#include "UIHint.h"
#include "UIBtnHint.h"
#include "UITaskWnd.h"
#include "UIRankingWnd.h"
#include "UILogsWnd.h"

#include "UIScriptWnd.h"

#include "Actor.h"
#include "Inventory.h"
#include "../xrEngine/XR_IOConsole.h"
#include "ui\UIProgressBar.h"
#include "player_hud.h"

#define PDA_XML		"pda.xml"

u32 g_pda_info_state = 0;

void RearrangeTabButtons(CUITabControl* pTab);

CUIPdaWnd::CUIPdaWnd()
{
	pUITaskWnd = NULL;
	pUIRankingWnd = NULL;
	pUILogsWnd = NULL;
	m_hint_wnd = NULL;
	m_battery_bar = NULL;
	m_power = 0.f;
	last_cursor_pos.set(UI_BASE_WIDTH / 2.f, UI_BASE_HEIGHT / 2.f);
	m_cursor_box.set(117.f, 39.f, UI_BASE_WIDTH - 121.f, UI_BASE_HEIGHT - 37.f);
	Init();
}

CUIPdaWnd::~CUIPdaWnd()
{
	delete_data(pUITaskWnd);
	delete_data(pUIRankingWnd);
	delete_data(pUILogsWnd);
	delete_data(m_hint_wnd);
	delete_data(UINoice);
}

void CUIPdaWnd::Init()
{
	CUIXml uiXml;
	uiXml.Load(CONFIG_PATH, UI_PATH, PDA_XML);

	m_pActiveDialog = NULL;
	m_sActiveSection = "";

	CUIXmlInit::InitWindow(uiXml, "main", 0, this);

	UIMainPdaFrame = UIHelper::CreateStatic(uiXml, "background_static", this);
	m_caption = UIHelper::CreateTextWnd(uiXml, "caption_static", this);
	m_caption_const = (m_caption->GetText());
	m_clock = UIHelper::CreateTextWnd(uiXml, "clock_wnd", this);

	m_hint_wnd = UIHelper::CreateHint(uiXml, "hint_wnd");

	m_battery_bar = xr_new<CUIProgressBar>();
	m_battery_bar->SetAutoDelete(true);
	AttachChild(m_battery_bar);
	CUIXmlInit::InitProgressBar(uiXml, "battery_bar", 0, m_battery_bar);
	m_battery_bar->Show(true);

	if (IsGameTypeSingle())
	{
		pUITaskWnd = xr_new<CUITaskWnd>();
		pUITaskWnd->hint_wnd = m_hint_wnd;
		pUITaskWnd->Init();

		pUIRankingWnd = xr_new<CUIRankingWnd>();
		pUIRankingWnd->Init();

		pUILogsWnd = xr_new<CUILogsWnd>();
		pUILogsWnd->Init();
	}

	UITabControl = xr_new<CUITabControl>();
	UITabControl->SetAutoDelete(true);
	AttachChild(UITabControl);
	CUIXmlInit::InitTabControl(uiXml, "tab", 0, UITabControl);
	UITabControl->SetMessageTarget(this);

	UINoice = xr_new<CUIStatic>();
	UINoice->SetAutoDelete(true);
	CUIXmlInit::InitStatic(uiXml, "noice_static", 0, UINoice);
}

void CUIPdaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	switch (msg)
	{
	case TAB_CHANGED:
	{
		if (pWnd == UITabControl)
		{
			SetActiveSubdialog(UITabControl->GetActiveId());
		}
		break;
	}
	case BUTTON_CLICKED:
	{
		if (pWnd == m_btn_close)
		{
			if (smart_cast<CPda*>(Actor()->inventory().ActiveItem()))
				Actor()->inventory().Activate(NO_ACTIVE_SLOT);
		}
		break;
	}
	default:
	{
		if (m_pActiveDialog)
			m_pActiveDialog->SendMessage(pWnd, msg, pData);
	}
	};
}

bool CUIPdaWnd::OnMouseAction(float x, float y, EUIMessages mouse_action)
{
	switch (mouse_action)
	{
	case WINDOW_LBUTTON_DOWN:
	case WINDOW_RBUTTON_DOWN:
	case WINDOW_LBUTTON_UP:
	case WINDOW_RBUTTON_UP:
	{
		CPda* pda = smart_cast<CPda*>(Actor()->inventory().ActiveItem());
		if (pda)
		{
			if (pda->IsPending())
				return true;

			if (mouse_action == WINDOW_LBUTTON_DOWN)
				bButtonL = true;
			else if (mouse_action == WINDOW_RBUTTON_DOWN)
				bButtonR = true;
			else if (mouse_action == WINDOW_LBUTTON_UP)
				bButtonL = false;
			else if (mouse_action == WINDOW_RBUTTON_UP)
				bButtonR = false;
		}
		break;
	}
	}
	CUIDialogWnd::OnMouseAction(x, y, mouse_action);
	return true; //always true because StopAnyMove() == false
}

void CUIPdaWnd::MouseMovement(float x, float y)
{
	CPda* pda = smart_cast<CPda*>(Actor()->inventory().ActiveItem());
	if (!pda) return;

	x *= .1f;
	y *= .1f;
	clamp(x, -.15f, .15f);
	clamp(y, -.15f, .15f);

	if (_abs(x) < .05f)
		x = 0.f;

	if (_abs(y) < .05f)
		y = 0.f;

	bool buttonpressed = (bButtonL || bButtonR);

	target_buttonpress = (buttonpressed ? -.0015f : 0.f);
	target_joystickrot.set(x * -.75f, 0.f, y * .75f);

	x += y * pda->m_thumb_rot[0];
	y += x * pda->m_thumb_rot[1];

	g_player_hud->m_bone_callback_params[r_finger0]->m_target.set(y * .15f, y * -.05f, (x * -.15f) + (buttonpressed ? .002f : 0.f));
	g_player_hud->m_bone_callback_params[r_finger01]->m_target.set(0.f, 0.f, (x * -.25f) + (buttonpressed ? .01f : 0.f));
	g_player_hud->m_bone_callback_params[r_finger02]->m_target.set(0.f, 0.f, (x * .75f) + (buttonpressed ? .025f : 0.f));
}

void CUIPdaWnd::Show(bool status)
{
	inherited::Show(status);
	if (status)
	{
		InventoryUtilities::SendInfoToActor("ui_pda");
		if (m_sActiveSection == NULL || strcmp(m_sActiveSection.c_str(), "") == 0)
		{
			SetActiveSubdialog("eptTasks");
			UITabControl->SetActiveTab("eptTasks");
		}
		else
			SetActiveSubdialog(m_sActiveSection);
	}
	else
	{
		InventoryUtilities::SendInfoToActor("ui_pda_hide");
		CurrentGameUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiPdaTask, false);
		if (m_pActiveDialog)
		{
			m_pActiveDialog->Show(false);
			m_pActiveDialog = pUITaskWnd; //hack for script window
		}
		g_btnHint->Discard();
		g_statHint->Discard();
	}
}

void CUIPdaWnd::Update()
{
	inherited::Update();
	if (m_pActiveDialog)
		m_pActiveDialog->Update();

	m_clock->TextItemControl().SetText(
		InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes).c_str());

	m_battery_bar->SetProgressPos(m_power);

	pUILogsWnd->PerformWork();
}

void CUIPdaWnd::SetActiveSubdialog(const shared_str& section)
{
	if (m_pActiveDialog)
	{
		//if (m_sActiveSection == section) return;
		if (UIMainPdaFrame->IsChild(m_pActiveDialog))
			UIMainPdaFrame->DetachChild(m_pActiveDialog);
		m_pActiveDialog->Show(false);
	}

	if (section == "eptTasks")
	{
		m_pActiveDialog = pUITaskWnd;
	}
	else if (section == "eptRanking")
	{
		m_pActiveDialog = pUIRankingWnd;
	}
	else if (section == "eptLogs")
	{
		m_pActiveDialog = pUILogsWnd;
	}

	luabind::functor<CUIDialogWndEx*> funct;
	if (ai().script_engine().functor("pda.set_active_subdialog", funct))
	{
		CUIDialogWndEx* ret = funct((LPCSTR)section.c_str());
		CUIWindow* pScriptWnd = ret ? smart_cast<CUIWindow*>(ret) : (0);
		if (pScriptWnd)
			m_pActiveDialog = pScriptWnd;
	}

	if (m_pActiveDialog)
	{
		if (!UIMainPdaFrame->IsChild(m_pActiveDialog))
			UIMainPdaFrame->AttachChild(m_pActiveDialog);
		m_pActiveDialog->Show(true);
		m_sActiveSection = section;
		SetActiveCaption();
	}
	else
		m_sActiveSection = "";
}

void CUIPdaWnd::SetActiveCaption()
{
	TABS_VECTOR* btn_vec = UITabControl->GetButtonsVector();
	TABS_VECTOR::iterator it_b = btn_vec->begin();
	TABS_VECTOR::iterator it_e = btn_vec->end();
	for (; it_b != it_e; ++it_b)
	{
		if ((*it_b)->m_btn_id == m_sActiveSection)
		{
			LPCSTR cur = (*it_b)->TextItemControl()->GetText();
			string256 buf;
			strconcat(sizeof(buf), buf, m_caption_const.c_str(), cur);
			SetCaption(buf);
			UITabControl->Show(true);
			m_clock->Show(true);
			m_caption->Show(true);
			m_battery_bar->Show(true);
			return;
		}
	}

	UITabControl->Show(false);
	m_caption->Show(false);
}

#include "UICursor.h"

void CUIPdaWnd::ResetCursor()
{
	if (!last_cursor_pos.similar({ 0.f, 0.f }))
		GetUICursor().SetUICursorPosition(last_cursor_pos);
}

void CUIPdaWnd::Show_SecondTaskWnd(bool status)
{
	if (status)
	{
		SetActiveSubdialog("eptTasks");
	}
	pUITaskWnd->Show_TaskListWnd(status);
}

void CUIPdaWnd::Show_MapLegendWnd(bool status)
{
	if (status)
	{
		SetActiveSubdialog("eptTasks");
	}
	pUITaskWnd->ShowMapLegend(status);
}

void CUIPdaWnd::Draw()
{
	if (Device.dwFrame == dwPDAFrame)
		return;

	dwPDAFrame = Device.dwFrame;

	inherited::Draw();
	//.	DrawUpdatedSections();
	DrawHint();
	UINoice->Draw(); // over all
}

void CUIPdaWnd::DrawHint()
{
	if (m_pActiveDialog == pUITaskWnd)
	{
		pUITaskWnd->DrawHint();
	}
	else if (m_pActiveDialog == pUIRankingWnd)
	{
		pUIRankingWnd->DrawHint();
	}
	else if (m_pActiveDialog == pUILogsWnd)
	{

	}
	m_hint_wnd->Draw();
}

void CUIPdaWnd::UpdatePda()
{
	pUILogsWnd->UpdateNews();

	if (m_pActiveDialog == pUITaskWnd)
	{
		pUITaskWnd->ReloadTaskInfo();
	}
}

void CUIPdaWnd::UpdateRankingWnd()
{
	pUIRankingWnd->Update();
}

void CUIPdaWnd::Reset()
{
	inherited::ResetAll();

	if (pUITaskWnd)		pUITaskWnd->ResetAll();
	if (pUIRankingWnd)	pUIRankingWnd->ResetAll();
	if (pUILogsWnd)		pUILogsWnd->ResetAll();
}

void CUIPdaWnd::SetCaption(LPCSTR text)
{
	m_caption->SetText(text);
}

void RearrangeTabButtons(CUITabControl* pTab)
{
	TABS_VECTOR* btn_vec = pTab->GetButtonsVector();
	TABS_VECTOR::iterator it = btn_vec->begin();
	TABS_VECTOR::iterator it_e = btn_vec->end();

	Fvector2					pos;
	pos.set((*it)->GetWndPos());
	float						size_x;

	for (; it != it_e; ++it)
	{
		(*it)->SetWndPos(pos);
		(*it)->AdjustWidthToText();
		size_x = (*it)->GetWndSize().x + 30.0f;
		(*it)->SetWidth(size_x);
		pos.x += size_x - 6.0f;
	}

	pTab->SetWidth(pos.x + 5.0f);
	pos.x = pTab->GetWndPos().x - pos.x;
	pos.y = pTab->GetWndPos().y;
	pTab->SetWndPos(pos);
}

void CUIPdaWnd::Enable(bool status)
{
	if (status)
		ResetCursor();
	else
	{
		g_player_hud->reset_thumb(false);
		ResetJoystick(false);
		bButtonL = false;
		bButtonR = false;
	}

	inherited::Enable(status);
}

bool CUIPdaWnd::OnKeyboardAction(int dik, EUIMessages keyboard_action)
{
	if (IsShown() && (keyboard_action == WINDOW_KEY_PRESSED || keyboard_action == WINDOW_KEY_RELEASED))
	{
		if (!psActorFlags.test(AF_3D_PDA) && keyboard_action == WINDOW_KEY_PRESSED)
		{
			EGameActions action = get_binded_action(dik);

			if (action == kQUIT || action == kINVENTORY || action == kACTIVE_JOBS)
			{
				HideDialog();
				return  action == kQUIT;
			}

			return inherited::OnKeyboardAction(dik, keyboard_action);
		}

		CPda* pda = smart_cast<CPda*>(Actor()->inventory().ActiveItem());

		if (!pda)
			return inherited::OnKeyboardAction(dik, keyboard_action);

		return pda->Action(get_binded_action(dik), keyboard_action == WINDOW_KEY_PRESSED ? CMD_START : CMD_STOP);
	}
}
