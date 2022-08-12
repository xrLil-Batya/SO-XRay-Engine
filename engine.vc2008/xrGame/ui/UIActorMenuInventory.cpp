#include "stdafx.h"
#include "UIActorMenu.h"
#include "../inventory.h"
#include "../inventoryOwner.h"
#include "UIInventoryUtilities.h"
#include "UIItemInfo.h"
#include "../Level.h"
#include "UICellItemFactory.h"
#include "UIDragDropListEx.h"
#include "UIDragDropReferenceList.h"
#include "UICellCustomItems.h"
#include "UIItemInfo.h"
#include "UIFrameLineWnd.h"
#include "UIPropertiesBox.h"
#include "UIListBoxItem.h"
#include "UIMainIngameWnd.h"
#include "UIGameCustom.h"
#include "UI3tButton.h"
#include "UIActorStateInfo.h"
#include "UIProgressBar.h"
#include "eatable_item_object.h"

#include "../silencer.h"
#include "../scope.h"
#include "../grenadelauncher.h"
#include "../Artefact.h"
#include "../eatable_item.h"
#include "../BottleItem.h"
#include "../WeaponMagazined.h"
#include "../Medkit.h"
#include "../Antirad.h"
#include "../CustomOutfit.h"
#include "../ActorHelmet.h"
#include "../UICursor.h"
#include "../MPPlayersBag.h"
#include "../player_hud.h"
#include "../CustomDetector.h"
#include "../PDA.h"

#include "../actor_defs.h"

void move_item_from_to(u16 from_id, u16 to_id, u16 what_id);
//Directed by ARTLantist
void CUIActorMenu::InitInventoryMode()
{
	m_RightBackground->Show(true);
    m_pInventoryBagList->Show(true);
    m_pInventoryBeltList->Show(true);

    for (u8 i = 1; i <= m_slot_count; ++i)
    {
        if (m_pInvList[i])
            m_pInvList[i]->Show(true);
    }
    m_pQuickSlot->Show(true);
    m_pTrashList->Show(true);
    m_RightDelimiter->Show(false);
	m_btn_sort_all->Show(true);
    m_btn_sort_weapons->Show(true);
    m_btn_sort_armor->Show(true);
    m_btn_sort_ammo->Show(true);
    m_btn_sort_artefact->Show(true);
    m_btn_sort_tools->Show(true);
    m_btn_sort_consumables->Show(true);
    m_btn_sort_misc->Show(true);

	InitInventoryCargoBelt();
	InitInventoryPouch1();
	InitInventoryPouch2();
    InitInventoryContents(m_pInventoryBagList);
}
void CUIActorMenu::InitInventoryCargoBelt()
{
	DeinitInventoryCargoBelt(false);
	PIItem cb = m_pActorInvOwner->inventory().ItemFromSlot(ZAYAZ_SLOT);
	if (cb) {
		CGameObject* GO = smart_cast<CGameObject*>(cb);
		if (!xr_strcmp(GO->cNameSect(), "zayaz_b")) {
			m_pInvList[ZAYAZ_DOSIMETER]->ClearAll(true);
			m_pInvList[ZAYAZ_DOSIMETER]->Show(true);
			InitCellForSlot(ZAYAZ_DOSIMETER);
			m_zDosimeterElement->Show(true);

			m_pInvList[ZAYAZ_GREN]->ClearAll(true);
			m_pInvList[ZAYAZ_GREN]->Show(true);
			InitCellForSlot(ZAYAZ_GREN);
			m_zGrenadeElement[0]->Show(true);
			m_zGrenadeElement[1]->Show(true);

			m_pInvList[ZAYAZ_AMMO_1]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_1]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_1);
			m_zAmmoElement[0]->Show(true);

			m_pInvList[ZAYAZ_AMMO_2]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_2]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_2);
			m_zAmmoElement[1]->Show(true);

			m_pInvList[ZAYAZ_MED_1]->ClearAll(true);
			m_pInvList[ZAYAZ_MED_1]->Show(true);
			InitCellForSlot(ZAYAZ_MED_1);
			m_zMed1Element[0]->Show(true);
			m_zMed1Element[1]->Show(true);

			m_pInvList[ZAYAZ_MED_2]->ClearAll(true);
			m_pInvList[ZAYAZ_MED_2]->Show(true);
			InitCellForSlot(ZAYAZ_MED_2);
			m_zMed2Element[0]->Show(true);
			m_zMed2Element[1]->Show(true);

		}
		if (!xr_strcmp(GO->cNameSect(), "zayaz_a")) {
			m_pInvList[ZAYAZ_DOSIMETER]->ClearAll(true);
			m_pInvList[ZAYAZ_DOSIMETER]->Show(true);
			InitCellForSlot(ZAYAZ_DOSIMETER);
			m_zDosimeterElement->Show(true);

			m_pInvList[ZAYAZ_GREN]->ClearAll(true);
			m_pInvList[ZAYAZ_GREN]->Show(true);
			InitCellForSlot(ZAYAZ_GREN);
			m_zGrenadeElement[0]->Show(true);
			m_zGrenadeElement[1]->Show(true);

			m_pInvList[ZAYAZ_GREN_W]->ClearAll(true);
			m_pInvList[ZAYAZ_GREN_W]->Show(true);
			InitCellForSlot(ZAYAZ_GREN_W);
			m_zGrenadeWElement[0]->Show(true);
			m_zGrenadeWElement[1]->Show(true);

			m_pInvList[ZAYAZ_AMMO_1]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_1]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_1);
			m_zAmmoElement[0]->Show(true);

			m_pInvList[ZAYAZ_AMMO_2]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_2]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_2);
			m_zAmmoElement[1]->Show(true);

			m_pInvList[ZAYAZ_AMMO_3]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_3]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_3);
			m_zAmmoElement[2]->Show(true);

			m_pInvList[ZAYAZ_MED_1]->ClearAll(true);
			m_pInvList[ZAYAZ_MED_1]->Show(true);
			InitCellForSlot(ZAYAZ_MED_1);
			m_zMed1Element[0]->Show(true);
			m_zMed1Element[1]->Show(true);

			m_pInvList[ZAYAZ_MED_2]->ClearAll(true);
			m_pInvList[ZAYAZ_MED_2]->Show(true);
			InitCellForSlot(ZAYAZ_MED_2);
			m_zMed2Element[0]->Show(true);
			m_zMed2Element[1]->Show(true);
		}
		if (!xr_strcmp(GO->cNameSect(), "zayaz_h")) {
			m_pInvList[ZAYAZ_DOSIMETER]->ClearAll(true);
			m_pInvList[ZAYAZ_DOSIMETER]->Show(true);
			InitCellForSlot(ZAYAZ_DOSIMETER);
			m_zDosimeterElement->Show(true);

			m_pInvList[ZAYAZ_GREN]->ClearAll(true);
			m_pInvList[ZAYAZ_GREN]->Show(true);
			InitCellForSlot(ZAYAZ_GREN);
			m_zGrenadeElement[0]->Show(true);
			m_zGrenadeElement[1]->Show(true);

			m_pInvList[ZAYAZ_GREN_W]->ClearAll(true);
			m_pInvList[ZAYAZ_GREN_W]->Show(true);
			InitCellForSlot(ZAYAZ_GREN_W);
			m_zGrenadeWElement[0]->Show(true);
			m_zGrenadeWElement[1]->Show(true);

			m_pInvList[ZAYAZ_AMMO_1]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_1]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_1);
			m_zAmmoElement[0]->Show(true);

			m_pInvList[ZAYAZ_AMMO_2]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_2]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_2);
			m_zAmmoElement[1]->Show(true);

			m_pInvList[ZAYAZ_AMMO_3]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_3]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_3);
			m_zAmmoElement[2]->Show(true);

			m_pInvList[ZAYAZ_AMMO_4]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_4]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_4);
			m_zAmmoElement[3]->Show(true);

			m_pInvList[ZAYAZ_MED_1]->ClearAll(true);
			m_pInvList[ZAYAZ_MED_1]->Show(true);
			InitCellForSlot(ZAYAZ_MED_1);
			m_zMed1Element[0]->Show(true);

			m_pInvList[ZAYAZ_MED_2]->ClearAll(true);
			m_pInvList[ZAYAZ_MED_2]->Show(true);
			InitCellForSlot(ZAYAZ_MED_2);
			m_zMed2Element[0]->Show(true);
			m_zMed2Element[1]->Show(true);
			m_zMed2Element[2]->Show(true);
			m_zMed2Element[3]->Show(true);
		}
		if (!xr_strcmp(GO->cNameSect(), "zayaz_t")) {
			m_pInvList[ZAYAZ_DOSIMETER]->ClearAll(true);
			m_pInvList[ZAYAZ_DOSIMETER]->Show(true);
			InitCellForSlot(ZAYAZ_DOSIMETER);
			m_zDosimeterElement->Show(true);

			m_pInvList[ZAYAZ_GREN]->ClearAll(true);
			m_pInvList[ZAYAZ_GREN]->Show(true);
			InitCellForSlot(ZAYAZ_GREN);
			m_zGrenadeElement[0]->Show(true);
			m_zGrenadeElement[1]->Show(true);

			m_pInvList[ZAYAZ_GREN_W]->ClearAll(true);
			m_pInvList[ZAYAZ_GREN_W]->Show(true);
			InitCellForSlot(ZAYAZ_GREN_W);
			m_zGrenadeWElement[0]->Show(true);
			m_zGrenadeWElement[1]->Show(true);

			m_pInvList[ZAYAZ_AMMO_1]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_1]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_1);
			m_zAmmoElement[0]->Show(true);

			m_pInvList[ZAYAZ_AMMO_2]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_2]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_2);
			m_zAmmoElement[1]->Show(true);

			m_pInvList[ZAYAZ_AMMO_3]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_3]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_3);
			m_zAmmoElement[2]->Show(true);

			m_pInvList[ZAYAZ_AMMO_4]->ClearAll(true);
			m_pInvList[ZAYAZ_AMMO_4]->Show(true);
			InitCellForSlot(ZAYAZ_AMMO_4);
			m_zAmmoElement[3]->Show(true);

			m_pInvList[ZAYAZ_MED_1]->ClearAll(true);
			m_pInvList[ZAYAZ_MED_1]->Show(true);
			InitCellForSlot(ZAYAZ_MED_1);
			m_zMed1Element[0]->Show(true);
			m_zMed1Element[1]->Show(true);
			m_zMed1Element[2]->Show(true);

			m_pInvList[ZAYAZ_MED_2]->ClearAll(true);
			m_pInvList[ZAYAZ_MED_2]->Show(true);
			InitCellForSlot(ZAYAZ_MED_2);
			m_zMed2Element[0]->Show(true);
			m_zMed2Element[1]->Show(true);
			m_zMed2Element[2]->Show(true);
			m_zMed2Element[3]->Show(true);
		}
	}
}
void CUIActorMenu::InitInventoryPouch1()
{
	DeinitInventoryPouch1(false);
	PIItem p1 = m_pActorInvOwner->inventory().ItemFromSlot(BOUCH_LEFT_SLOT);
	if (p1) {
		CGameObject* GO = smart_cast<CGameObject*>(p1);
		if (!xr_strcmp(GO->cNameSect(), "bouch_small")) {
			m_pInvList[BOUCH_UP_1]->Show(true);
			InitCellForSlot(BOUCH_UP_1);
			m_BouchUpElement[0]->Show(true);
		}
		if (!xr_strcmp(GO->cNameSect(), "bouch_dual")) {
			m_pInvList[BOUCH_UP_1]->Show(true);
			InitCellForSlot(BOUCH_UP_1);
			m_BouchUpElement[0]->Show(true);

			m_pInvList[BOUCH_UP_2]->Show(true);
			InitCellForSlot(BOUCH_UP_2);
			m_BouchUpElement[1]->Show(true);
		}
	}
}
void CUIActorMenu::InitInventoryPouch2()
{	
	DeinitInventoryPouch2(false);
	PIItem p2 = m_pActorInvOwner->inventory().ItemFromSlot(BOUCH_RIGHT_SLOT);
	if (p2) {
		CGameObject* GO = smart_cast<CGameObject*>(p2);
		if (!xr_strcmp(GO->cNameSect(), "bouch_small")) {
			m_pInvList[BOUCH_DOWN_1]->Show(true);
			InitCellForSlot(BOUCH_DOWN_1);
			m_BouchDownElement[0]->Show(true);
		}
		if (!xr_strcmp(GO->cNameSect(), "bouch_dual")) {
			m_pInvList[BOUCH_DOWN_1]->Show(true);
			InitCellForSlot(BOUCH_DOWN_1);
			m_BouchDownElement[0]->Show(true);

			m_pInvList[BOUCH_DOWN_2]->Show(true);
			InitCellForSlot(BOUCH_DOWN_2);
			m_BouchDownElement[1]->Show(true);
		}
	}
}

void CUIActorMenu::DeInitInventoryMode()
{
	m_RightBackground->Show(false);
	m_pInvList[ZAYAZ_SLOT]->Show(false);
	m_pInvList[BOUCH_LEFT_SLOT]->Show(false);
	m_pInvList[BOUCH_RIGHT_SLOT]->Show(false);
    m_pTrashList->Show(false);
	DeinitInventoryCargoBelt(true);
	DeinitInventoryPouch1(true);
	DeinitInventoryPouch2(true);
}
void CUIActorMenu::DeinitInventoryCargoBelt(bool all)
{
	if (all) {
		for (u8 i = ZAYAZ_AMMO_1; i <= ZAYAZ_GREN; ++i) {
			if (m_pInvList[i])
				m_pInvList[i]->Show(false);
		}
	}
	m_zDosimeterElement->Show(false);
	m_zGrenadeElement[0]->Show(false);
	m_zGrenadeElement[1]->Show(false);
	m_zGrenadeWElement[0]->Show(false);
	m_zGrenadeWElement[1]->Show(false);
	m_zAmmoElement[0]->Show(false);
	m_zAmmoElement[1]->Show(false);
	m_zAmmoElement[2]->Show(false);
	m_zAmmoElement[3]->Show(false);
	m_zMed1Element[0]->Show(false);
	m_zMed1Element[1]->Show(false);
	m_zMed1Element[2]->Show(false);
	m_zMed2Element[0]->Show(false);
	m_zMed2Element[1]->Show(false);
	m_zMed2Element[2]->Show(false);
	m_zMed2Element[3]->Show(false);
}
void CUIActorMenu::DeinitInventoryPouch1(bool all)
{
	if (all) {
		for (u8 i = BOUCH_UP_1; i <= BOUCH_UP_2; ++i) {
			if (m_pInvList[i])
				m_pInvList[i]->Show(false);
		}
	}
	m_BouchUpElement[0]->Show(false);
	m_BouchUpElement[1]->Show(false);
}
void CUIActorMenu::DeinitInventoryPouch2(bool all)
{
	if (all) {
		for (u8 i = BOUCH_DOWN_1; i <= BOUCH_DOWN_2; ++i) {
			if (m_pInvList[i])
				m_pInvList[i]->Show(false);
		}
	}
	m_BouchDownElement[0]->Show(false);
	m_BouchDownElement[1]->Show(false);
}
//-ARTLantist
void CUIActorMenu::SendEvent_ActivateSlot(u16 slot, u16 recipient)
{
	NET_Packet						P;
	CGameObject::u_EventGen			(P, GEG_PLAYER_ACTIVATE_SLOT, recipient);
	P.w_u16							(slot);
	CGameObject::u_EventSend		(P);
}

void CUIActorMenu::SendEvent_Item2Slot(PIItem pItem, u16 recipient, u16 slot_id)
{
	if(pItem->parent_id()!=recipient)
		move_item_from_to			(pItem->parent_id(), recipient, pItem->object_id());

	NET_Packet						P;
	CGameObject::u_EventGen			(P, GEG_PLAYER_ITEM2SLOT, pItem->object().H_Parent()->ID());
	P.w_u16							(pItem->object().ID());
	P.w_u16							(slot_id);
	CGameObject::u_EventSend		(P);

	PlaySnd							(eItemToSlot);
};

void CUIActorMenu::SendEvent_Item2Belt(PIItem pItem, u16 recipient)
{
	if(pItem->parent_id()!=recipient)
		move_item_from_to			(pItem->parent_id(), recipient, pItem->object_id());

	NET_Packet						P;
	CGameObject::u_EventGen			(P, GEG_PLAYER_ITEM2BELT, pItem->object().H_Parent()->ID());
	P.w_u16							(pItem->object().ID());
	CGameObject::u_EventSend		(P);

	PlaySnd							(eItemToBelt);
};

void CUIActorMenu::SendEvent_Item2Ruck(PIItem pItem, u16 recipient)
{
	if(pItem->parent_id()!=recipient)
		move_item_from_to			(pItem->parent_id(), recipient, pItem->object_id());

	NET_Packet						P;
	CGameObject::u_EventGen			(P, GEG_PLAYER_ITEM2RUCK, pItem->object().H_Parent()->ID());
	P.w_u16							(pItem->object().ID());
	CGameObject::u_EventSend		(P);

	PlaySnd							(eItemToRuck);
};

void CUIActorMenu::SendEvent_Item_Eat(PIItem pItem, u16 recipient)
{
	if(pItem->parent_id()!=recipient)
		move_item_from_to			(pItem->parent_id(), recipient, pItem->object_id());

	NET_Packet						P;
	CGameObject::u_EventGen			(P, GEG_PLAYER_ITEM_EAT, recipient);
	P.w_u16							(pItem->object().ID());
	CGameObject::u_EventSend		(P);
};

void CUIActorMenu::SendEvent_Item_Drop(PIItem pItem, u16 recipient)
{
	R_ASSERT(pItem->parent_id()==recipient);
	if (!IsGameTypeSingle())
		pItem->DenyTrade();
	NET_Packet					P;
	pItem->object().u_EventGen	(P,GE_OWNERSHIP_REJECT,pItem->parent_id());
	P.w_u16						(pItem->object().ID());
	pItem->object().u_EventSend	(P);
	PlaySnd						(eDropItem);
}

void CUIActorMenu::DropAllCurrentItem()
{
	if ( CurrentIItem() && !CurrentIItem()->IsQuestItem() )
	{
		u32 const cnt = CurrentItem()->ChildsCount();
		for( u32 i = 0; i < cnt; ++i )
		{
			CUICellItem*	itm  = CurrentItem()->PopChild(NULL);
			PIItem			iitm = (PIItem)itm->m_pData;
			SendEvent_Item_Drop( iitm, m_pActorInvOwner->object_id() );
		}

		SendEvent_Item_Drop( CurrentIItem(), m_pActorInvOwner->object_id() );
	}
	SetCurrentItem								(NULL);
}

bool CUIActorMenu::DropAllItemsFromRuck( bool quest_force )
{
	if ( !IsShown() || !m_pInventoryBagList || m_currMenuMode != mmInventory )
	{
		return false;
	}

	u32 const ci_count = m_pInventoryBagList->ItemsCount();
	for ( u32 i = 0; i < ci_count; ++i )
	{
		CUICellItem* ci = m_pInventoryBagList->GetItemIdx( i );
		VERIFY( ci );
		PIItem item = (PIItem)ci->m_pData;
		VERIFY( item );

		if ( !quest_force && item->IsQuestItem() )
		{
			continue;
		}

		u32 const cnt = ci->ChildsCount();
		for( u32 j = 0; j < cnt; ++j )
		{
			CUICellItem*	child_ci   = ci->PopChild(NULL);
			PIItem			child_item = (PIItem)child_ci->m_pData;
			SendEvent_Item_Drop( child_item, m_pActorInvOwner->object_id() );
		}
		SendEvent_Item_Drop( item, m_pActorInvOwner->object_id() );
	}

	SetCurrentItem( NULL );
	return true;
}

bool FindItemInList(CUIDragDropListEx* lst, PIItem pItem, CUICellItem*& ci_res)
{
	u32 count = lst->ItemsCount();
	for (u32 i=0; i<count; ++i)
	{
		CUICellItem* ci				= lst->GetItemIdx(i);
		for(u32 j=0; j<ci->ChildsCount(); ++j)
		{
			CUIInventoryCellItem* ici = smart_cast<CUIInventoryCellItem*>(ci->Child(j));
			if(ici->object()==pItem)
			{
				ci_res = ici;
				return true;
			}
		}

		CUIInventoryCellItem* ici = smart_cast<CUIInventoryCellItem*>(ci);
		if(ici->object()==pItem)
		{
			ci_res = ci;
			return true;
		}
	}
	return false;
}

bool RemoveItemFromList(CUIDragDropListEx* lst, PIItem pItem)
{// fixme
	CUICellItem*	ci	= NULL;
	if(FindItemInList(lst, pItem, ci))
	{
		R_ASSERT		(ci);

		CUICellItem* dying_cell = lst->RemoveItem	(ci, false);
		xr_delete(dying_cell);

		return			true;
	}else
		return			false;
}

void CUIActorMenu::OnInventoryAction(PIItem pItem, u16 action_type)
{
	CUIDragDropListEx* all_lists[] =
	{
		m_pInventoryBeltList,
		m_pInventoryBagList,
		m_pTradeActorBagList,
		m_pTradeActorList,
		NULL
	};

	switch (action_type)
	{
		case GE_TRADE_BUY :
		case GE_OWNERSHIP_TAKE :
			{
				u32 i			= 0;
				bool b_already	= false;

				CUIDragDropListEx* lst_to_add		= NULL;
				SInvItemPlace pl						= pItem->m_ItemCurrPlace;
				if ( pItem->BaseSlot() == GRENADE_SLOT )
				{
					pl.type		= eItemPlaceRuck;
					pl.slot_id	= GRENADE_SLOT;
				}
#ifndef MASTER_GOLD
				Msg("item place [%d]", pl);
#endif // #ifndef MASTER_GOLD

				if(pl.type==eItemPlaceSlot)
					lst_to_add						= GetSlotList(pl.slot_id);
				else if(pl.type==eItemPlaceBelt)
					lst_to_add						= GetListByType(iActorBelt);
				else
				{
					if(pItem->parent_id()==m_pActorInvOwner->object_id())
						lst_to_add						= GetListByType(iActorBag);
					else
						lst_to_add						= GetListByType(iDeadBodyBag);
				}


				while ( all_lists[i] )
				{
					CUIDragDropListEx*	curr = all_lists[i];
					CUICellItem*		ci   = NULL;

					if ( FindItemInList(curr, pItem, ci) )
					{
						if ( lst_to_add != curr )
						{
							RemoveItemFromList(curr, pItem);
						}
						else
						{
							b_already = true;
						}
						//break;
					}
					++i;
				}
				CUICellItem*		ci   = NULL;
				if(GetMenuMode()==mmDeadBodySearch && FindItemInList(m_pDeadBodyBagList, pItem, ci))
					break;

				if ( !b_already )
				{
					if ( lst_to_add )
					{
						CUICellItem* itm	= create_cell_item(pItem);
						lst_to_add->SetItem	(itm);
					}
				}
				if(m_pActorInvOwner)
					m_pQuickSlot->ReloadReferences(m_pActorInvOwner);
			}break;
		case GE_TRADE_SELL :
		case GE_OWNERSHIP_REJECT :
			{
				if(CUIDragDropListEx::m_drag_item)
				{
					CUIInventoryCellItem* ici = smart_cast<CUIInventoryCellItem*>(CUIDragDropListEx::m_drag_item->ParentItem());
					R_ASSERT(ici);
					if(ici->object()==pItem)
					{
						CUIDragDropListEx*	_drag_owner		= ici->OwnerList();
						_drag_owner->DestroyDragItem		();
					}
				}

				u32 i = 0;
				while(all_lists[i])
				{
					CUIDragDropListEx* curr = all_lists[i];
					if(RemoveItemFromList(curr, pItem))
					{
#ifndef MASTER_GOLD
						Msg("all ok. item [%d] removed from list", pItem->object_id());
#endif // #ifndef MASTER_GOLD
						break;
					}
					++i;
				}
				if(m_pActorInvOwner)
					m_pQuickSlot->ReloadReferences(m_pActorInvOwner);
			}break;
	}
	UpdateItemsPlace();
	UpdateConditionProgressBars();
}
void CUIActorMenu::AttachAddon(PIItem item_to_upgrade)
{
	PlaySnd										(eAttachAddon);
	R_ASSERT									(item_to_upgrade);
	if (OnClient())
	{
		NET_Packet								P;
		CGameObject::u_EventGen					(P, GE_ADDON_ATTACH, item_to_upgrade->object().ID());
		P.w_u16									(CurrentIItem()->object().ID());
		CGameObject::u_EventSend				(P);
	};

	item_to_upgrade->Attach						(CurrentIItem(), true);

	SetCurrentItem								(NULL);
}

void CUIActorMenu::DetachAddon(LPCSTR addon_name, PIItem itm)
{
	PlaySnd										(eDetachAddon);
	if (OnClient())
	{
		NET_Packet								P;
		if(itm==NULL)
			CGameObject::u_EventGen				(P, GE_ADDON_DETACH, CurrentIItem()->object().ID());
		else
			CGameObject::u_EventGen				(P, GE_ADDON_DETACH, itm->object().ID());

		P.w_stringZ								(addon_name);
		CGameObject::u_EventSend				(P);
		return;
	}
	if(itm==NULL)
		CurrentIItem()->Detach					(addon_name, true);
	else
		itm->Detach								(addon_name, true);
}

void CUIActorMenu::InitCellForSlot( u16 slot_idx )
{
	VERIFY( KNIFE_SLOT <= slot_idx && slot_idx <= LAST_SLOT );
	CInventory*	inv	= &m_pActorInvOwner->inventory();
	PIItem item	= inv->m_slots[slot_idx].m_pIItem;
	CUIDragDropListEx* curr_list	= GetSlotList( slot_idx );
	if ( item )
	{
		CUICellItem* cell_item			= create_cell_item( item );
		curr_list->SetItem( cell_item );
		if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
			ColorizeItem( cell_item, !CanMoveToPartner( item ) );
	}
	if (inv->m_slots[slot_idx].m_ammunitions.size()) {
		for(TIItemContainer::const_iterator it = inv->m_slots[slot_idx].m_ammunitions.begin(); inv->m_slots[slot_idx].m_ammunitions.end() != it; ++it) 
		{
			PIItem itm = *it;
			if ( itm ){
				CUICellItem* cell_itm			= create_cell_item( itm );
				curr_list->SetItem( cell_itm );
				if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
					ColorizeItem( cell_itm, !CanMoveToPartner( itm ) );
			}
		}
	}
}

void CUIActorMenu::InitInventoryContents(CUIDragDropListEx* pBagList)
{
	ClearAllLists();
    m_pMouseCapturer = NULL;
    m_UIPropertiesBox->Hide();
    SetCurrentItem(NULL);

    CUIDragDropListEx* curr_list = NULL;
    // Slots
    for (u8 i = 1; i <= m_slot_count; ++i)
    {
        if (m_pInvList[i])
            InitCellForSlot(i);
    }

	curr_list					= m_pInventoryBeltList;
	TIItemContainer::iterator itb = m_pActorInvOwner->inventory().m_belt.begin();
	TIItemContainer::iterator ite = m_pActorInvOwner->inventory().m_belt.end();
	for ( ; itb != ite; ++itb )
	{
		CUICellItem* itm		= create_cell_item(*itb);
		curr_list->SetItem		(itm);
		if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
			ColorizeItem( itm, !CanMoveToPartner( *itb ) );
	}

	TIItemContainer				ruck_list;
	ruck_list					= m_pActorInvOwner->inventory().m_ruck;
	std::sort					( ruck_list.begin(), ruck_list.end(), InventoryUtilities::GreaterRoomInRuck );

	curr_list					= pBagList;

	itb = ruck_list.begin();
	ite = ruck_list.end();
	for ( ; itb != ite; ++itb )
	{
		CMPPlayersBag* bag = smart_cast<CMPPlayersBag*>( &(*itb)->object() );
		if ( bag )
			continue;

		CUICellItem* itm = create_cell_item( *itb );
		curr_list->SetItem(itm);
		if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
			ColorizeItem( itm, !CanMoveToPartner( *itb ) );
	}
    m_pQuickSlot->ReloadReferences(m_pActorInvOwner);
}

void CUIActorMenu::InitInventorySortContents(u8 type)
{
	m_pInventoryBagList->ClearAll(true);
	m_pMouseCapturer			= NULL;
	m_UIPropertiesBox->Hide		();
	SetCurrentItem				(NULL);

	CUIDragDropListEx*			curr_list = m_pInventoryBagList;
	TIItemContainer				ruck_list = m_pActorInvOwner->inventory().m_ruck;
	std::sort					( ruck_list.begin(), ruck_list.end(), InventoryUtilities::GreaterRoomInRuck );

	TIItemContainer::iterator itb = ruck_list.begin();
	TIItemContainer::iterator ite = ruck_list.end();
	for ( ; itb != ite; ++itb )
	{
		CMPPlayersBag* bag = smart_cast<CMPPlayersBag*>( &(*itb)->object() );
		if ( bag )
			continue;
		PIItem item = *itb;
		if (item&&type==1&&item->GetSortWeapons()){
			CUICellItem* itm = create_cell_item( *itb );
			curr_list->SetItem(itm);
			if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
				ColorizeItem( itm, !CanMoveToPartner( *itb ) );
		}
		if (item&&type==2&&item->GetSortArmor()){
			CUICellItem* itm = create_cell_item( *itb );
			curr_list->SetItem(itm);
			if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
				ColorizeItem( itm, !CanMoveToPartner( *itb ) );
		}
		if (item&&type==3&&item->GetSortAmmo()){
			CUICellItem* itm = create_cell_item( *itb );
			curr_list->SetItem(itm);
			if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
				ColorizeItem( itm, !CanMoveToPartner( *itb ) );
		}
		if (item&&type==4&&item->GetSortArtefact()){
			CUICellItem* itm = create_cell_item( *itb );
			curr_list->SetItem(itm);
			if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
				ColorizeItem( itm, !CanMoveToPartner( *itb ) );
		}
		if (item&&type==5&&item->GetSortTools()){
			CUICellItem* itm = create_cell_item( *itb );
			curr_list->SetItem(itm);
			if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
				ColorizeItem( itm, !CanMoveToPartner( *itb ) );
		}
		if (item&&type==6&&item->GetSortConsumables()){
			CUICellItem* itm = create_cell_item( *itb );
			curr_list->SetItem(itm);
			if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
				ColorizeItem( itm, !CanMoveToPartner( *itb ) );
		}
		if (item&&type==7&&item->GetSortMisc()){
			CUICellItem* itm = create_cell_item( *itb );
			curr_list->SetItem(itm);
			if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
				ColorizeItem( itm, !CanMoveToPartner( *itb ) );
		}
	}
}

bool CUIActorMenu::TryActiveSlot(CUICellItem* itm)
{
	PIItem	iitem	= (PIItem)itm->m_pData;
	u16 slot		= iitem->BaseSlot();
	if ( slot == GRENADE_SLOT )
	{
		PIItem	prev_iitem = m_pActorInvOwner->inventory().ItemFromSlot(slot);
		if ( prev_iitem && (prev_iitem->object().cNameSect() != iitem->object().cNameSect()) )
		{
			SendEvent_Item2Ruck( prev_iitem, m_pActorInvOwner->object_id() );
			SendEvent_Item2Slot( iitem, m_pActorInvOwner->object_id(), slot );
		}
		SendEvent_ActivateSlot( slot, m_pActorInvOwner->object_id() );
		return true;
	}
	return false;
}

bool CUIActorMenu::ToSlot(CUICellItem* itm, bool force_place, u16 slot_id)
{
	CUIDragDropListEx* old_owner = itm->OwnerList();
	PIItem iitem = (PIItem)itm->m_pData;
	bool b_own_item = (iitem->parent_id()==m_pActorInvOwner->object_id());
	if (slot_id==HELMET_SLOT)	{
		CCustomOutfit* pOutfit = m_pActorInvOwner->GetOutfit();
		if(pOutfit && !pOutfit->bIsHelmetAvaliable)
			return false;
	}
	if (slot_id==BACKPACK_SLOT)	{
		CCustomOutfit* pOutfit = m_pActorInvOwner->GetOutfit();
		if(pOutfit && !pOutfit->bIsBackpackAvaliable)
			return false;
	}
	if (slot_id>=ZAYAZ_AMMO_1&&slot_id<=ZAYAZ_GREN)	{
		PIItem CargoBelt = m_pActorInvOwner->inventory().ItemFromSlot(ZAYAZ_SLOT);
		if(!CargoBelt)
			return false;
	}
	if (slot_id>=BOUCH_UP_1&&slot_id<=BOUCH_UP_2)	{
		PIItem Pouch = m_pActorInvOwner->inventory().ItemFromSlot(BOUCH_LEFT_SLOT);
		if(!Pouch)
			return false;
	}
	if (slot_id>=BOUCH_DOWN_1&&slot_id<=BOUCH_DOWN_2)	{
		PIItem Pouch = m_pActorInvOwner->inventory().ItemFromSlot(BOUCH_RIGHT_SLOT);
		if(!Pouch)
			return false;
	}

	if(m_pActorInvOwner->inventory().CanPutInSlot(iitem, slot_id))
	{
		CUIDragDropListEx* new_owner = GetSlotList(slot_id);
		if (!new_owner->IsShown())
			return false;

		if(slot_id==OUTFIT_SLOT) {
			CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>(iitem);
			if(pOutfit && !pOutfit->bIsHelmetAvaliable)	{
				CUIDragDropListEx* helmet_list = GetSlotList(HELMET_SLOT);
				if(helmet_list->ItemsCount()==1) {
					CUICellItem* helmet_cell = helmet_list->GetItemIdx(0);
					ToBag(helmet_cell, false);
				}
			}
			if(pOutfit && !pOutfit->bIsBackpackAvaliable)	{
				CUIDragDropListEx* backpack_list = GetSlotList(BACKPACK_SLOT);
				if(backpack_list->ItemsCount()==1) {
					CUICellItem* backpack_cell = backpack_list->GetItemIdx(0);
					ToBag(backpack_cell, false);
				}
			}
		}
		bool result = (!b_own_item) || m_pActorInvOwner->inventory().Slot(slot_id, iitem);
		VERIFY(result);
		CUICellItem* i = old_owner->RemoveItem(itm, (old_owner==new_owner) );
		new_owner->SetItem(i);
		SendEvent_Item2Slot(iitem, m_pActorInvOwner->object_id(), slot_id);
		SendEvent_ActivateSlot(slot_id, m_pActorInvOwner->object_id());
		if ( slot_id == ART_BELT_SLOT )
			MoveArtefactsToBag();
		if ( slot_id == ZAYAZ_SLOT ){
			InitInventoryCargoBelt();
			MoveZayazToBag();
		}
		if ( slot_id == BOUCH_LEFT_SLOT ) {
			InitInventoryPouch1();
			MoveBouchLeftToBag();
		}
		if ( slot_id == BOUCH_RIGHT_SLOT ) {
			InitInventoryPouch2();
			MoveBouchRightToBag();
		}
		return true;
	}
	else
	{ // in case slot is busy
		if ( slot_id == NO_ACTIVE_SLOT ) 
			return false;
		CUIDragDropListEx* slot_list		= GetSlotList(slot_id);
		if (!slot_list->IsShown())
			return false;

		if ( slot_id == INV_SLOT_2 && m_pActorInvOwner->inventory().CanPutInSlot(iitem, INV_SLOT_3))
			return ToSlot(itm, force_place, INV_SLOT_3);
		if ( slot_id == INV_SLOT_3 && m_pActorInvOwner->inventory().CanPutInSlot(iitem, INV_SLOT_2))
			return ToSlot(itm, force_place, INV_SLOT_2);

		if ( slot_id == BOUCH_LEFT_SLOT && m_pActorInvOwner->inventory().CanPutInSlot(iitem, BOUCH_RIGHT_SLOT))
			return ToSlot(itm, force_place, BOUCH_RIGHT_SLOT);
		
		if (slot_id==BOUCH_UP_1) {
			for (u8 i = BOUCH_UP_2; i <= BOUCH_DOWN_2; ++i)	{
				if (m_pActorInvOwner->inventory().CanPutInSlot(iitem, i))
					return ToSlot(itm, force_place, i);
			}
		}
		if (slot_id==ZAYAZ_AMMO_1) {
			for (u8 i = ZAYAZ_AMMO_2; i <= ZAYAZ_AMMO_4; ++i)	{
				if (m_pActorInvOwner->inventory().CanPutInSlot(iitem, i))
					return ToSlot(itm, force_place, i);
			}
		}
		if (slot_id==ZAYAZ_MED_1&&m_pActorInvOwner->inventory().CanPutInSlot(iitem, ZAYAZ_MED_2))
			return ToSlot(itm, force_place, ZAYAZ_MED_2);
		
		if (!force_place) 
			return false;
		PIItem _iitem = m_pActorInvOwner->inventory().ItemFromSlot(slot_id);
		VERIFY(slot_list->ItemsCount()==1);
		CUICellItem* slot_cell = slot_list->GetItemIdx(0);
		VERIFY(slot_cell && ((PIItem)slot_cell->m_pData)==_iitem);
		bool result = ToBag(slot_cell, false);
		VERIFY(result);
		result = ToSlot(itm, false, slot_id);
		if(b_own_item && result && slot_id==DETECTOR_SLOT)	{
			CCustomDevice* det = smart_cast<CCustomDevice*>(iitem);
			det->ToggleDevice(g_player_hud->attached_item(0)!=NULL);
		}

		return result;
	}
}


bool CUIActorMenu::ToBag(CUICellItem* itm, bool b_use_cursor_pos)
{
	PIItem	iitem						= (PIItem)itm->m_pData;

	bool b_own_item						= (iitem->parent_id()==m_pActorInvOwner->object_id());

	bool b_already						= m_pActorInvOwner->inventory().InRuck(iitem);

	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= NULL;
	if(b_use_cursor_pos)
	{
			new_owner					= CUIDragDropListEx::m_drag_item->BackList();
			VERIFY						(GetListType(new_owner)==iActorBag);
	}else
			new_owner					= GetListByType(iActorBag);

	if(m_pActorInvOwner->inventory().CanPutInRuck(iitem) || (b_already && (new_owner!=old_owner)) )
	{
		bool result							= b_already || (!b_own_item || m_pActorInvOwner->inventory().Ruck(iitem) );
		VERIFY								(result);
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		if(!i)
			return false;

		if(b_use_cursor_pos)
			new_owner->SetItem				(i,old_owner->GetDragItemPosition());
		else
			new_owner->SetItem				(i);

		if(!b_already || !b_own_item)
			SendEvent_Item2Ruck					(iitem, m_pActorInvOwner->object_id());

		if ( m_currMenuMode == mmTrade && m_pPartnerInvOwner )
		{
			ColorizeItem( itm, !CanMoveToPartner( iitem ) );
		}
		return true;
	}
	return false;
}

bool CUIActorMenu::ToBelt(CUICellItem* itm, bool b_use_cursor_pos)
{
	PIItem	iitem						= (PIItem)itm->m_pData;
	bool b_own_item						= (iitem->parent_id()==m_pActorInvOwner->object_id());

	if(m_pActorInvOwner->inventory().CanPutInBelt(iitem))
	{
		CUIDragDropListEx*	old_owner		= itm->OwnerList();
		CUIDragDropListEx*	new_owner		= NULL;
		if(b_use_cursor_pos){
				new_owner					= CUIDragDropListEx::m_drag_item->BackList();
				VERIFY						(new_owner==m_pInventoryBeltList);
		}else
				new_owner					= m_pInventoryBeltList;

		bool result							= (!b_own_item) || m_pActorInvOwner->inventory().Belt(iitem);
		VERIFY								(result);
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );

		if(b_use_cursor_pos)
			new_owner->SetItem				(i,old_owner->GetDragItemPosition());
		else
			new_owner->SetItem				(i);

		if(!b_own_item)
			SendEvent_Item2Belt				(iitem, m_pActorInvOwner->object_id());

		return								true;
	}
	else
	{ // in case belt slot is busy
		if(!iitem->Belt() || m_pActorInvOwner->inventory().BeltWidth()==0)
			return false;

		CUIDragDropListEx* belt_list		= NULL;
		if(b_use_cursor_pos)
			belt_list						= CUIDragDropListEx::m_drag_item->BackList();
		else
			return false;

		Ivector2 belt_cell_pos				= belt_list->PickCell(GetUICursor().GetCursorPosition());
		if(belt_cell_pos.x==-1 && belt_cell_pos.y==-1)
			return false;

		CUICellItem* slot_cell				= belt_list->GetCellAt(belt_cell_pos).m_item;

		bool result							= ToBag(slot_cell, false);
		VERIFY								(result);

		result								= ToBelt(itm, b_use_cursor_pos);
		return result;
	}
}

CUIDragDropListEx* CUIActorMenu::GetSlotList(u16 slot_idx)
{
    if (slot_idx == NO_ACTIVE_SLOT)
        return NULL;

    if (m_pInvList[slot_idx])
        return m_pInvList[slot_idx];

    if (m_currMenuMode == mmTrade)
        return m_pTradeActorBagList;
    return m_pInventoryBagList;
}

#include "actor.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
bool CUIActorMenu::TryUseItem( CUICellItem* cell_itm )
{
	if ( !cell_itm )
	{
		return false;
	}
	PIItem item	= (PIItem)cell_itm->m_pData;
	
	CBottleItem*	pBottleItem		= smart_cast<CBottleItem*>	(item);
	CMedkit*		pMedkit			= smart_cast<CMedkit*>		(item);
	CAntirad*		pAntirad		= smart_cast<CAntirad*>		(item);
	CEatableItem*	pEatableItem	= smart_cast<CEatableItem*>	(item);

	if ( !(pMedkit || pAntirad || pEatableItem || pBottleItem) )
	{
		return false;
	}
	if ( !item->Useful() )
	{
		return false;
	}
	if (item->GetScriptAnim()){
		CActor* pActor = smart_cast<CActor*>(m_pActorInvOwner);
		pActor->callback(GameObject::eUseInvObject)((smart_cast<CGameObject*>(item))->lua_game_object());
		return false;
	}

	u16 recipient = m_pActorInvOwner->object_id();
	if ( item->parent_id() != recipient )
	{
		cell_itm->OwnerList()->RemoveItem( cell_itm, false );
	}

	SendEvent_Item_Eat		( item, recipient );
	PlaySnd					( eItemUse );
	SetCurrentItem			( NULL );
	return true;
}

bool CUIActorMenu::ToQuickSlot(CUICellItem* itm)
{
	PIItem iitem = (PIItem)itm->m_pData;
	CEatableItemObject* eat_item = smart_cast<CEatableItemObject*>(iitem);
	if(!eat_item)
		return false;

	u8 slot_idx = u8(m_pQuickSlot->PickCell(GetUICursor().GetCursorPosition()).x);
	if(slot_idx==255)
		return false;

	m_pQuickSlot->SetItem(create_cell_item(iitem), GetUICursor().GetCursorPosition());
	xr_strcpy(ACTOR_DEFS::g_quick_use_slots[slot_idx], iitem->m_section_id.c_str());
	return true;
}


bool CUIActorMenu::OnItemDropped(PIItem itm, CUIDragDropListEx* new_owner, CUIDragDropListEx* old_owner)
{
	CUICellItem*	_citem	= (new_owner->ItemsCount()==1) ? new_owner->GetItemIdx(0) : NULL;
	PIItem _iitem	= _citem ? (PIItem)_citem->m_pData : NULL;

	if(!_iitem)						return	false;
	if(!_iitem->CanAttach(itm))		return	false;

	if(old_owner != m_pInventoryBagList)	return	false;

	AttachAddon						(_iitem);

	return							true;
}

void CUIActorMenu::TryHidePropertiesBox()
{
	if ( m_UIPropertiesBox->IsShown() )
	{
		m_UIPropertiesBox->Hide();
	}
}

void CUIActorMenu::ActivatePropertiesBox()
{
	TryHidePropertiesBox();
	if ( !(m_currMenuMode == mmInventory || m_currMenuMode == mmDeadBodySearch || m_currMenuMode == mmUpgrade) )
	{
		return;
	}

	PIItem item = CurrentIItem();
	if ( !item )
	{
		return;
	}

	CUICellItem* cell_item = CurrentItem();
	m_UIPropertiesBox->RemoveAll();
	bool b_show = false;

	if ( m_currMenuMode == mmInventory || m_currMenuMode == mmDeadBodySearch)
	{
		if(!item->GetMoveToCargoBelt()&&!item->GetMoveToBouch())
			PropertiesBoxForSlots( item, b_show );
		PropertiesBoxForWeapon( cell_item, item, b_show );
		PropertiesBoxForAddon( item, b_show );
		PropertiesBoxForUsing( item, b_show );
		PropertiesBoxForPlaying(item, b_show);
		if ( m_currMenuMode == mmInventory ){
			if(item->GetMoveToCargoBelt())
				PropertiesBoxForCargoBelt( item, b_show );
			if(item->GetMoveToBouch())
				PropertiesBoxForBouch( item, b_show );
			PropertiesBoxForDrop( cell_item, item, b_show );
		}
	}
	else if ( m_currMenuMode == mmUpgrade )
	{
		PropertiesBoxForRepair( item, b_show );
	}

	if ( b_show )
	{
		m_UIPropertiesBox->AutoUpdateSize();

		Fvector2 cursor_pos;
		Frect						vis_rect;
		GetAbsoluteRect				(vis_rect);
		cursor_pos					= GetUICursor().GetCursorPosition();
		cursor_pos.sub				(vis_rect.lt);
		m_UIPropertiesBox->Show		(vis_rect, cursor_pos);
		PlaySnd						(eProperties);
	}
}

void CUIActorMenu::PropertiesBoxForSlots( PIItem item, bool& b_show )
{
	CCustomOutfit* pOutfit	= smart_cast<CCustomOutfit*>( item );
	CHelmet* pHelmet		= smart_cast<CHelmet*>		( item );
	CInventory&  inv		= m_pActorInvOwner->inventory();

	// Флаг-признак для невлючения пункта контекстного меню: Dreess Outfit, если костюм уже надет
	bool bAlreadyDressed	= false;
	u16 cur_slot			= item->BaseSlot();

	if (	!pOutfit && !pHelmet &&
			cur_slot != NO_ACTIVE_SLOT &&
			!inv.SlotIsPersistent(cur_slot) &&
			inv.CanPutInSlot(item, cur_slot) )
	{
		m_UIPropertiesBox->AddItem( "st_move_to_slot",  NULL, INVENTORY_TO_SLOT_ACTION );
		b_show = true;
	}
	if (	item->Belt() &&
			inv.CanPutInBelt( item ) )
	{
		m_UIPropertiesBox->AddItem( "st_move_on_belt",  NULL, INVENTORY_TO_BELT_ACTION );
		b_show = true;
	}

	if (	item->Ruck() &&
			inv.CanPutInRuck(item) &&
			( cur_slot == NO_ACTIVE_SLOT || !inv.SlotIsPersistent(cur_slot) ) )
	{
		if( !pOutfit )
		{
			if( !pHelmet )
				m_UIPropertiesBox->AddItem( "st_move_to_bag",  NULL, INVENTORY_TO_BAG_ACTION );
			else
				m_UIPropertiesBox->AddItem( "st_undress_helmet",  NULL, INVENTORY_TO_BAG_ACTION );
		}
		else
			m_UIPropertiesBox->AddItem( "st_undress_outfit",  NULL, INVENTORY_TO_BAG_ACTION );

		bAlreadyDressed = true;
		b_show			= true;
	}
	if ( pOutfit && !bAlreadyDressed )
	{
		m_UIPropertiesBox->AddItem( "st_dress_outfit",  NULL, INVENTORY_TO_SLOT_ACTION );
		b_show			= true;
	}

	CCustomOutfit* outfit_in_slot = m_pActorInvOwner->GetOutfit();
	if ( pHelmet && !bAlreadyDressed && (!outfit_in_slot || outfit_in_slot->bIsHelmetAvaliable))
	{
		m_UIPropertiesBox->AddItem( "st_dress_helmet",  NULL, INVENTORY_TO_SLOT_ACTION );
		b_show			= true;
	}
}

void CUIActorMenu::PropertiesBoxForWeapon( CUICellItem* cell_item, PIItem item, bool& b_show )
{
	//отсоединение аддонов от вещи
	CWeapon*	pWeapon = smart_cast<CWeapon*>( item );
	if ( !pWeapon )
	{
		return;
	}

	if ( pWeapon->GrenadeLauncherAttachable() )
	{
		if ( pWeapon->IsGrenadeLauncherAttached() )
		{
			m_UIPropertiesBox->AddItem( "st_detach_gl",  NULL, INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON );
			b_show			= true;
		}
		else
		{
		}
	}
	if ( pWeapon->ScopeAttachable() )
	{
		if ( pWeapon->IsScopeAttached() )
		{
			m_UIPropertiesBox->AddItem( "st_detach_scope",  NULL, INVENTORY_DETACH_SCOPE_ADDON );
			b_show			= true;
		}
		else
		{
		}
	}
	if ( pWeapon->SilencerAttachable() )
	{
		if ( pWeapon->IsSilencerAttached() )
		{
			m_UIPropertiesBox->AddItem( "st_detach_silencer",  NULL, INVENTORY_DETACH_SILENCER_ADDON );
			b_show			= true;
		}
		else
		{
		}
	}
	if ( smart_cast<CWeaponMagazined*>(pWeapon) && IsGameTypeSingle() )
	{
		bool b = ( pWeapon->GetAmmoElapsed() !=0 );
		if ( !b )
		{
			for ( u32 i = 0; i < cell_item->ChildsCount(); ++i )
			{
				CWeaponMagazined* weap_mag = smart_cast<CWeaponMagazined*>( (CWeapon*)cell_item->Child(i)->m_pData );
				if ( weap_mag && weap_mag->GetAmmoElapsed() )
				{
					b = true;
					break; // for
				}
			}
		}
		if ( b )
		{
			m_UIPropertiesBox->AddItem( "st_unload_magazine",  NULL, INVENTORY_UNLOAD_MAGAZINE );
			b_show = true;
		}
	}
}
#include "../string_table.h"
void CUIActorMenu::PropertiesBoxForAddon( PIItem item, bool& b_show )
{
	//присоединение аддонов к активному слоту (2 или 3)

	CScope*				pScope				= smart_cast<CScope*>			(item);
	CSilencer*			pSilencer			= smart_cast<CSilencer*>		(item);
	CGrenadeLauncher*	pGrenadeLauncher	= smart_cast<CGrenadeLauncher*>	(item);
	CInventory*			inv					= &m_pActorInvOwner->inventory();

	PIItem	item_in_slot_2 = inv->ItemFromSlot(INV_SLOT_2);
	PIItem	item_in_slot_3 = inv->ItemFromSlot(INV_SLOT_3);

	if(!item_in_slot_2 && !item_in_slot_3)	return;

	if ( pScope )
	{
		if ( item_in_slot_2 && item_in_slot_2->CanAttach(pScope) )
		{
			shared_str str = CStringTable().translate("st_attach_scope_to_pistol");
			str.printf("%s %s", str.c_str(), item_in_slot_2->m_name.c_str());
			m_UIPropertiesBox->AddItem( str.c_str(),  (void*)item_in_slot_2, INVENTORY_ATTACH_ADDON );
			b_show			= true;
		}
		if ( item_in_slot_3 && item_in_slot_3->CanAttach(pScope) )
		{
			shared_str str = CStringTable().translate("st_attach_scope_to_pistol");
			str.printf("%s %s", str.c_str(), item_in_slot_3->m_name.c_str());
			m_UIPropertiesBox->AddItem( str.c_str(),  (void*)item_in_slot_3, INVENTORY_ATTACH_ADDON );
			b_show			= true;
		}
		return;
	}

	if ( pSilencer )
	{
		if ( item_in_slot_2 && item_in_slot_2->CanAttach(pSilencer) )
		{
			shared_str str = CStringTable().translate("st_attach_silencer_to_pistol");
			str.printf("%s %s", str.c_str(), item_in_slot_2->m_name.c_str());
			m_UIPropertiesBox->AddItem( str.c_str(),  (void*)item_in_slot_2, INVENTORY_ATTACH_ADDON );
			b_show			= true;
		}
		if ( item_in_slot_3 && item_in_slot_3->CanAttach(pSilencer) )
		{
			shared_str str = CStringTable().translate("st_attach_silencer_to_pistol");
			str.printf("%s %s", str.c_str(), item_in_slot_3->m_name.c_str());
			m_UIPropertiesBox->AddItem( str.c_str(),  (void*)item_in_slot_3, INVENTORY_ATTACH_ADDON );
			b_show			= true;
		}
		return;
	}

	if ( pGrenadeLauncher )
	{
		if ( item_in_slot_2 && item_in_slot_2->CanAttach(pGrenadeLauncher) )
		{
			shared_str str = CStringTable().translate("st_attach_gl_to_rifle");
			str.printf("%s %s", str.c_str(), item_in_slot_2->m_name.c_str());
			m_UIPropertiesBox->AddItem( str.c_str(),  (void*)item_in_slot_2, INVENTORY_ATTACH_ADDON );
			b_show			= true;
		}
		if ( item_in_slot_3 && item_in_slot_3->CanAttach(pGrenadeLauncher) )
		{
			shared_str str = CStringTable().translate("st_attach_gl_to_rifle");
			str.printf("%s %s", str.c_str(), item_in_slot_3->m_name.c_str());
			m_UIPropertiesBox->AddItem( str.c_str(),  (void*)item_in_slot_3, INVENTORY_ATTACH_ADDON );
			b_show			= true;
		}
	}
}

void CUIActorMenu::PropertiesBoxForUsing( PIItem item, bool& b_show )
{
	LPCSTR act_str = NULL;
	CGameObject* GO = smart_cast<CGameObject*>(item);
	shared_str	section_name = GO->cNameSect();

	//ability to set eat string from settings
	act_str = READ_IF_EXISTS(pSettings, r_string, section_name, "default_use_text", 0);
	if (act_str)
	{
		m_UIPropertiesBox->AddItem(act_str, NULL, INVENTORY_EAT_ACTION);
		b_show = true;
	}
	else
	{
		CMedkit* pMedkit = smart_cast<CMedkit*>(item);
		CAntirad* pAntirad = smart_cast<CAntirad*>(item);
		CEatableItem * pEatableItem = smart_cast<CEatableItem*>(item);
		CBottleItem* pBottleItem = smart_cast<CBottleItem*>(item);

		if (pMedkit || pAntirad)
			act_str = "st_use";
		else if (pBottleItem)
			act_str = "st_drink";
		else if (pEatableItem) {
			act_str = "st_use";
		}
		if (act_str)
		{
			m_UIPropertiesBox->AddItem(act_str, NULL, INVENTORY_EAT_ACTION);
			b_show = true;
		}
	}

    //Custom Use action
	shared_str functor_field;
	LPCSTR functor_name = NULL;
	for (u8 i = 1; i <= 2; ++i)
	{
		functor_field.printf("use%d_functor", i);
		functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), functor_field.c_str(), 0);
		if (functor_name)
		{
			luabind::functor<LPCSTR> funct;
			if (ai().script_engine().functor(functor_name, funct))
			{
				act_str = funct(GO->lua_game_object());
				if (act_str)
				{
					m_UIPropertiesBox->AddItem(act_str, NULL, INVENTORY_EAT2_ACTION+i-1);
					b_show = true;
				}
			}
		}
	}
}

void CUIActorMenu::PropertiesBoxForPlaying(PIItem item, bool& b_show)
{
	CPda* pPda = smart_cast<CPda*>(item);
	if(!pPda || !pPda->CanPlayScriptFunction())
		return;

	LPCSTR act_str = "st_play";
	m_UIPropertiesBox->AddItem(act_str,  NULL, INVENTORY_PLAY_ACTION);
	b_show = true;
}

void CUIActorMenu::PropertiesBoxForDrop( CUICellItem* cell_item, PIItem item, bool& b_show )
{
	if ( !item->IsQuestItem() )
	{
		m_UIPropertiesBox->AddItem( "st_drop", NULL, INVENTORY_DROP_ACTION );
		b_show = true;

		if ( cell_item->ChildsCount() )
		{
			m_UIPropertiesBox->AddItem( "st_drop_all", (void*)33, INVENTORY_DROP_ACTION );
		}
	}
}

void CUIActorMenu::PropertiesBoxForRepair( PIItem item, bool& b_show )
{
	CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>( item );
	CWeapon*       pWeapon = smart_cast<CWeapon*>( item );
	CHelmet*       pHelmet = smart_cast<CHelmet*>( item );

	if ( (pOutfit || pWeapon || pHelmet) && item->GetCondition() < 0.99f )
	{
		m_UIPropertiesBox->AddItem( "ui_inv_repair", NULL, INVENTORY_REPAIR );
		b_show = true;
	}
}
//Added by ARTLantist: properties boxes for move to cargo belt and bouch slots
void CUIActorMenu::PropertiesBoxForCargoBelt( PIItem item, bool& b_show )
{
	CInventory&  inv		= m_pActorInvOwner->inventory();
	u16 cur_slot			= item->BaseSlot();
	if (item->Ruck() && inv.CanPutInRuck(item) && ( cur_slot == NO_ACTIVE_SLOT || !inv.SlotIsPersistent(cur_slot) ) )
	{
		m_UIPropertiesBox->AddItem( "st_move_to_bag",  NULL, INVENTORY_TO_BAG_ACTION );
		b_show = true;
		return;
	}
	PIItem cargo_belt = inv.ItemFromSlot(ZAYAZ_SLOT);
	if (cargo_belt) {
		m_UIPropertiesBox->AddItem("st_to_cargo_belt", NULL, INVENTORY_TO_CARGO_BELT_ACTION);
		b_show = true;
	}

}
void CUIActorMenu::PropertiesBoxForBouch( PIItem item, bool& b_show )
{
	CInventory&  inv		= m_pActorInvOwner->inventory();
	u16 cur_slot			= item->BaseSlot();
	if (item->Ruck() && inv.CanPutInRuck(item) && ( cur_slot == NO_ACTIVE_SLOT || !inv.SlotIsPersistent(cur_slot) ) )
		return;

	PIItem bouch = inv.ItemFromSlot(BOUCH_LEFT_SLOT);
	if (bouch) {
		m_UIPropertiesBox->AddItem("st_to_bouch_1", NULL, INVENTORY_TO_BOUCH_1_ACTION);
		b_show = true;
	}
	bouch = inv.ItemFromSlot(BOUCH_RIGHT_SLOT);
	if (bouch) {
		m_UIPropertiesBox->AddItem("st_to_bouch_2", NULL, INVENTORY_TO_BOUCH_2_ACTION);
		b_show = true;
	}
}
//-ARTLantist
void CUIActorMenu::ProcessPropertiesBoxClicked( CUIWindow* w, void* d )
{
	PIItem			item		= CurrentIItem();
	CUICellItem*	cell_item	= CurrentItem();
	if ( !m_UIPropertiesBox->GetClickedItem() || !item || !cell_item || !cell_item->OwnerList() )
	{
		return;
	}
	CWeapon* weapon = smart_cast<CWeapon*>( item );

	switch ( m_UIPropertiesBox->GetClickedItem()->GetTAG() )
	{
	case INVENTORY_TO_SLOT_ACTION: ToSlot( cell_item, true, item->BaseSlot() ); break;
	case INVENTORY_TO_BOUCH_1_ACTION: ToSlot( cell_item, true, BOUCH_UP_1 ); break;
	case INVENTORY_TO_BOUCH_2_ACTION: ToSlot( cell_item, true, BOUCH_DOWN_1 ); break;
	case INVENTORY_TO_CARGO_BELT_ACTION: ToSlot( cell_item, true, item->BaseSlot() ); break;
	case INVENTORY_TO_BELT_ACTION: ToBelt( cell_item, false ); break;
	case INVENTORY_TO_BAG_ACTION: ToBag ( cell_item, false ); break;
		case INVENTORY_EAT_ACTION:
		TryUseItem( cell_item );
		break;
		
	//ARTLantist Added
	case INVENTORY_EAT2_ACTION:
		{
			CGameObject* GO = smart_cast<CGameObject*>(item);
			LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), "use1_action_functor", 0);
			if (functor_name)
			{
				luabind::functor<bool>	funct1;
				if (ai().script_engine().functor(functor_name, funct1))
				{
					if (funct1(GO->lua_game_object()))
						TryUseItem(cell_item);
				}
			}
			break;
		}
	case INVENTORY_EAT3_ACTION:
		{
			CGameObject* GO = smart_cast<CGameObject*>(item);
			LPCSTR functor_name = READ_IF_EXISTS(pSettings, r_string, GO->cNameSect(), "use2_action_functor", 0);
			if (functor_name)
			{
				luabind::functor<bool>	funct2;
				if (ai().script_engine().functor(functor_name, funct2))
				{
					if (funct2(GO->lua_game_object()))
						TryUseItem(cell_item);
				}
			}
			break;
		} //-ARTLantist
	case INVENTORY_DROP_ACTION:
		{
			void* d = m_UIPropertiesBox->GetClickedItem()->GetData();
			if ( d == (void*)33 )
			{
				DropAllCurrentItem();
			}
			else
			{
				SendEvent_Item_Drop( item, m_pActorInvOwner->object_id() );
			}
			break;
		}
	case INVENTORY_ATTACH_ADDON:
		{
			PIItem item = CurrentIItem(); // temporary storing because of AttachAddon is setting curiitem to NULL
			AttachAddon((PIItem)(m_UIPropertiesBox->GetClickedItem()->GetData()));
			if(m_currMenuMode==mmDeadBodySearch)
				RemoveItemFromList(m_pDeadBodyBagList, item);
			
			break;
		}
	case INVENTORY_DETACH_SCOPE_ADDON:
		if ( weapon )
		{
			DetachAddon( weapon->GetScopeName().c_str() );
			for ( u32 i = 0; i < cell_item->ChildsCount(); ++i )
			{
				CUICellItem*	child_itm	= cell_item->Child(i);
				PIItem			child_iitm	= (PIItem)(child_itm->m_pData);
				CWeapon* wpn = smart_cast<CWeapon*>( child_iitm );
				if ( child_iitm && wpn )
				{
					DetachAddon(wpn->GetScopeName().c_str(), child_iitm);
				}
			}
		}
		break;
	case INVENTORY_DETACH_SILENCER_ADDON:
		if ( weapon )
		{
			DetachAddon( weapon->GetSilencerName().c_str() );
			for ( u32 i = 0; i < cell_item->ChildsCount(); ++i )
			{
				CUICellItem*	child_itm	= cell_item->Child(i);
				PIItem			child_iitm	= (PIItem)(child_itm->m_pData);
				CWeapon* wpn = smart_cast<CWeapon*>( child_iitm );
				if ( child_iitm && wpn )
				{
					DetachAddon(wpn->GetSilencerName().c_str(), child_iitm);
				}
			}
		}
		break;
	case INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON:
		if ( weapon )
		{
			DetachAddon( weapon->GetGrenadeLauncherName().c_str() );
			for ( u32 i = 0; i < cell_item->ChildsCount(); ++i )
			{
				CUICellItem*	child_itm	= cell_item->Child(i);
				PIItem			child_iitm	= (PIItem)(child_itm->m_pData);
				CWeapon* wpn = smart_cast<CWeapon*>( child_iitm );
				if ( child_iitm && wpn )
				{
					DetachAddon(wpn->GetGrenadeLauncherName().c_str(), child_iitm);
				}
			}
		}
		break;
	case INVENTORY_RELOAD_MAGAZINE:
		if ( weapon )
		{
			weapon->Action( kWPN_RELOAD, CMD_START );
		}
		break;
	case INVENTORY_UNLOAD_MAGAZINE:
		{
			CWeaponMagazined* weap_mag = smart_cast<CWeaponMagazined*>( (CWeapon*)cell_item->m_pData );
			if ( !weap_mag )
			{
				break;
			}
			weap_mag->UnloadMagazine();
			for ( u32 i = 0; i < cell_item->ChildsCount(); ++i )
			{
				CUICellItem*		child_itm		= cell_item->Child(i);
				CWeaponMagazined*	child_weap_mag	= smart_cast<CWeaponMagazined*>( (CWeapon*)child_itm->m_pData );
				if ( child_weap_mag )
				{
					child_weap_mag->UnloadMagazine();
				}
			}
			break;
		}
	case INVENTORY_REPAIR:
		{
			TryRepairItem(this,0);
			return;
			break;
		}
	case INVENTORY_PLAY_ACTION:
		{
			CPda* pPda = smart_cast<CPda*>(item);
			if(!pPda)
				break;
			pPda->PlayScriptFunction();
			break;
		}
	}//switch

	SetCurrentItem( NULL );
	UpdateItemsPlace();
	UpdateConditionProgressBars();
}

void CUIActorMenu::UpdateOutfit()
{
	for ( u8 i = 0; i < e_af_count ; ++i )
	{
		m_belt_list_over[i]->SetVisible( true );
	}

	u32 af_count = m_pActorInvOwner->inventory().BeltWidth();
	VERIFY( 0 <= af_count && af_count <= 5 );

	VERIFY( m_pInventoryBeltList );
	CCustomOutfit* outfit = m_pActorInvOwner->GetOutfit();
	if(outfit && !outfit->bIsHelmetAvaliable)
		m_HelmetOver->Show(true);
	else
		m_HelmetOver->Show(false);
	if(outfit && !outfit->bIsBackpackAvaliable)
		m_BackpackOver->Show(true);
	else
		m_BackpackOver->Show(false);
	PIItem cb = m_pActorInvOwner->inventory().ItemFromSlot(ZAYAZ_SLOT);
	if (!cb)
		MoveZayazToBag();
	PIItem p1 = m_pActorInvOwner->inventory().ItemFromSlot(BOUCH_LEFT_SLOT);
	if (!p1)
		MoveBouchLeftToBag();
	PIItem p2 = m_pActorInvOwner->inventory().ItemFromSlot(BOUCH_RIGHT_SLOT);
	if (!p2)
		MoveBouchRightToBag();
	PIItem belt = m_pActorInvOwner->inventory().ItemFromSlot(ART_BELT_SLOT);
	if (!belt)
	{
		MoveArtefactsToBag();
		return;
	}

	Ivector2 afc;
	afc.x = af_count;//1;
	afc.y = 1;//af_count;

	m_pInventoryBeltList->SetCellsCapacity( afc );

	for ( u8 i = 0; i < af_count ; ++i )
	{
		m_belt_list_over[i]->SetVisible( false );
	}
}

void CUIActorMenu::MoveArtefactsToBag()
{
    while (m_pInventoryBeltList->ItemsCount())
    {
        CUICellItem* ci = m_pInventoryBeltList->GetItemIdx(0);
        VERIFY(ci && ci->m_pData);
        ToBag(ci, false);
    }
    m_pInventoryBeltList->ClearAll(true);
}

//Added by ARTLantist
void CUIActorMenu::MoveZayazToBag()
{
	for (u8 i = ZAYAZ_AMMO_1; i <= ZAYAZ_GREN; ++i)
	{
		while ( m_pInvList[i]->ItemsCount() )
		{
			CUICellItem* ci = m_pInvList[i]->GetItemIdx(0);
			VERIFY( ci && ci->m_pData );
			ToBag( ci, false );
		}
		m_pInvList[i]->ClearAll(true);
	}
}
void CUIActorMenu::MoveBouchLeftToBag()
{
	while ( m_pInvList[BOUCH_UP_1]->ItemsCount() )
	{
		CUICellItem* ci = m_pInvList[BOUCH_UP_1]->GetItemIdx(0);
		VERIFY( ci && ci->m_pData );
		ToBag( ci, false );
	}
	while ( m_pInvList[BOUCH_UP_2]->ItemsCount() )
	{
		CUICellItem* ci = m_pInvList[BOUCH_UP_2]->GetItemIdx(0);
		VERIFY( ci && ci->m_pData );
		ToBag( ci, false );
	}
	m_pInvList[BOUCH_UP_1]->ClearAll(true);
	m_pInvList[BOUCH_UP_2]->ClearAll(true);
}
void CUIActorMenu::MoveBouchRightToBag()
{
	while ( m_pInvList[BOUCH_DOWN_1]->ItemsCount() )
	{
		CUICellItem* ci = m_pInvList[BOUCH_DOWN_1]->GetItemIdx(0);
		VERIFY( ci && ci->m_pData );
		ToBag( ci, false );
	}
	while ( m_pInvList[BOUCH_DOWN_2]->ItemsCount() )
	{
		CUICellItem* ci = m_pInvList[BOUCH_DOWN_2]->GetItemIdx(0);
		VERIFY( ci && ci->m_pData );
		ToBag( ci, false );
	}
	m_pInvList[BOUCH_DOWN_1]->ClearAll(true);
	m_pInvList[BOUCH_DOWN_2]->ClearAll(true);
}
void CUIActorMenu::OnBtnSortAll(CUIWindow* w, void* d)
{
	InitInventoryContents(m_pInventoryBagList);
}
void CUIActorMenu::OnBtnSortWeapons(CUIWindow* w, void* d)
{
	InitInventorySortContents(1);
}
void CUIActorMenu::OnBtnSortArmor(CUIWindow* w, void* d)
{
	InitInventorySortContents(2);
}
void CUIActorMenu::OnBtnSortAmmo(CUIWindow* w, void* d)
{
	InitInventorySortContents(3);
}
void CUIActorMenu::OnBtnSortArtefact(CUIWindow* w, void* d)
{
	InitInventorySortContents(4);
}
void CUIActorMenu::OnBtnSortTools(CUIWindow* w, void* d)
{
	InitInventorySortContents(5);
}
void CUIActorMenu::OnBtnSortConsumables(CUIWindow* w, void* d)
{
	InitInventorySortContents(6);
}
void CUIActorMenu::OnBtnSortMisc(CUIWindow* w, void* d)
{
	InitInventorySortContents(7);
}
//-ARTLantist