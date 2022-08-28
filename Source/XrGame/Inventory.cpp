#include "pch_script.h"
#include "inventory.h"
#include "actor.h"
#include "CustomOutfit.h"
#include "trade.h"
#include "Grenade.h"
#include "weapon.h"
#include "WeaponAmmo.h"
#include "eatable_item.h"

#include "ui/UIInventoryUtilities.h"
#include "ui/UIActorMenu.h"

#include "eatable_item.h"
#include "script_engine.h"
#include "xrmessages.h"
#include "xr_level_controller.h"
#include "level.h"
#include "ai_space.h"
#include "entitycondition.h"
#include "game_base_space.h"
#include "uigamecustom.h"
#include "clsid_game.h"
#include "static_cast_checked.hpp"
#include "player_hud.h"
#include "ui/UIDragDropListEx.h"

using namespace InventoryUtilities;

// what to block
u16	INV_STATE_LADDER		= (1<<INV_SLOT_3 | 1<<BINOCULAR_SLOT);
u16	INV_STATE_CAR			= INV_STATE_LADDER;
u16	INV_STATE_BLOCK_ALL		= 0xffff;
u16	INV_STATE_INV_WND		= INV_STATE_BLOCK_ALL;
u16	INV_STATE_BUY_MENU		= INV_STATE_BLOCK_ALL;

CInventorySlot::CInventorySlot() 
{
	m_pIItem				= NULL;
	m_bAct					= true;
	m_bPersistent			= false;
	m_ammunitions.clear();
}

void CInventorySlot::addItem(PIItem item)
{
	/** �� ��������� ��� ��������� ������. ���� ����� �� �������� ���� ��� ��� ������� m_pIItem */
	if (item == NULL) return;
	
	if (m_pIItem == item) 
		return; // � ��� ��� ���� ����� ����
		
	TIItemContainer::iterator	it = std::find(m_ammunitions.begin(), m_ammunitions.end(), item);
	if ( it != m_ammunitions.end()) 
		return; // � ��� ��� ���� ����� ����
		
	
	if (m_pIItem == NULL) {
		m_pIItem = item;
	}
	else {
		m_ammunitions.push_back(item);
	}
	
}
void CInventorySlot::eraseAll()
{
	m_ammunitions.clear();
	m_pIItem = NULL;
}
void CInventorySlot::eraseItem(PIItem item)
{
	if (item == NULL) return;
	
	TIItemContainer::iterator	it = std::find(m_ammunitions.begin(), m_ammunitions.end(), item);
	if ( it != m_ammunitions.end()) {
		m_ammunitions.erase				(it);
		item->object().processing_deactivate();
		return;
	}
	
	if (m_pIItem == item) {
		m_pIItem = NULL;
		item->object().processing_deactivate();
		//Msg("%s - ammo_size: %i", __FUNCTION__, m_ammunitions.size());
		return;
	}
}
bool CInventorySlot::checkItem(const CInventoryItem* pIItem) const
{
	if (m_pIItem == pIItem) return true;

	TIItemContainer::const_iterator it = std::find(m_ammunitions.begin(), m_ammunitions.end(), pIItem);
	return (m_ammunitions.end() != it);
}

CInventorySlot::~CInventorySlot() 
{
}

bool CInventorySlot::CanBeActivated() const 
{
	return (m_bAct);
};

CInventory::CInventory() 
{
	m_fMaxWeight								= pSettings->r_float	("inventory","max_weight");
	
	u32 sz										= pSettings->r_s32		("inventory","slots_count");
	m_slots.resize								(sz+1); //first is [1]
	
	m_iActiveSlot								= NO_ACTIVE_SLOT;
	m_iNextActiveSlot							= NO_ACTIVE_SLOT;
	m_iPrevActiveSlot							= NO_ACTIVE_SLOT;

	string256 temp;
	for(u16 i=FirstSlot(); i<=LastSlot(); ++i ) 
	{
		xr_sprintf(temp, "slot_persistent_%d", i);
		m_slots[i].m_bPersistent = !!pSettings->r_bool("inventory",temp);

		xr_sprintf			(temp, "slot_active_%d", i);
		m_slots[i].m_bAct	= !!pSettings->r_bool("inventory",temp);
		m_slots[i].m_pIItem = NULL;
		m_slots[i].m_ammunitions.clear();
	};

	m_bSlotsUseful								= true;
	m_bBeltUseful								= false;

	m_fTotalWeight								= -1.f;
	m_dwModifyFrame								= 0;
	m_drop_last_frame							= false;
	
	InitPriorityGroupsForQSwitch				();
	m_next_item_iteration_time					= 0;

	for (u16 i = 0; i < LAST_SLOT+1; ++i)
	{
		m_blocked_slots[i] = 0;
	}
}


CInventory::~CInventory() 
{
}

void CInventory::Clear()
{
	m_all.clear							();
	m_ruck.clear						();
	m_belt.clear						();
	m_zayaz.clear						();
	
	for(u16 i=FirstSlot(); i<=LastSlot(); i++){
		m_slots[i].m_ammunitions.clear();
		m_slots[i].m_pIItem				= NULL;
	}
	m_pOwner							= NULL;

	CalcTotalWeight						();
	InvalidateState						();
}

void CInventory::Take(CGameObject *pObj, bool bNotActivate, bool strict_placement)
{
	CInventoryItem *pIItem				= smart_cast<CInventoryItem*>(pObj);
	VERIFY								(pIItem);
	VERIFY								(pIItem->m_pInventory==NULL);
	VERIFY								(CanTakeItem(pIItem));
	
	pIItem->m_pInventory				= this;
	pIItem->SetDropManual				(FALSE);
	pIItem->AllowTrade					();
	//if net_Import for pObj arrived then the pObj will pushed to CrPr list (correction prediction)
	//usually net_Import arrived for objects that not has a parent object..
	//for unknown reason net_Import arrived for object that has a parent, so correction prediction schema will crash
	Level().RemoveObject_From_4CrPr		(pObj);

	m_all.push_back						(pIItem);

	if(!strict_placement)
		pIItem->m_ItemCurrPlace.type	= eItemPlaceUndefined;

	bool result							= false;
	switch(pIItem->m_ItemCurrPlace.type)
	{
	case eItemPlaceBelt:
		result							= Belt(pIItem, strict_placement); 
		if(!result)
			pIItem->m_ItemCurrPlace.type	= eItemPlaceUndefined;
#ifdef DEBUG
		if(!result) 
			Msg("cant put in belt item %s", *pIItem->object().cName());
#endif

		break;
	case eItemPlaceRuck:
		result							 = Ruck(pIItem, strict_placement);
		if(!result)
			pIItem->m_ItemCurrPlace.type = eItemPlaceUndefined;
#ifdef DEBUG
		if(!result) 
			Msg("cant put in ruck item %s", *pIItem->object().cName());
#endif

		break;
	case eItemPlaceSlot:
		result							= Slot(pIItem->m_ItemCurrPlace.slot_id, pIItem, bNotActivate, strict_placement); 
		if(!result)
			pIItem->m_ItemCurrPlace.type = eItemPlaceUndefined;
#ifdef DEBUG
		if(!result) 
			Msg("cant slot in slot item %s", *pIItem->object().cName());
#endif
		break;
	}

	if(pIItem->CurrPlace()==eItemPlaceUndefined)
	{
		if( !pIItem->RuckDefault() )
		{
			if( CanPutInSlot(pIItem, pIItem->BaseSlot()) )
			{
				result						= Slot(pIItem->BaseSlot(), pIItem, bNotActivate,strict_placement); VERIFY(result);
			}else
				if (CanPutInBelt(pIItem))
				{
					result					= Belt(pIItem,strict_placement); VERIFY(result);
				}else
				{
					result					= Ruck(pIItem,strict_placement); VERIFY(result);
				}
		}else
		{
			result						= Ruck(pIItem,strict_placement); VERIFY(result);
		}
	}
	
	m_pOwner->OnItemTake				(pIItem);

	CalcTotalWeight						();
	InvalidateState						();
	UpdateZayaz();

	pIItem->object().processing_deactivate();
	VERIFY								(pIItem->CurrPlace() != eItemPlaceUndefined);


	if( CurrentGameUI() )
	{
		CObject* pActor_owner = smart_cast<CObject*>(m_pOwner);

		if (Level().CurrentViewEntity() == pActor_owner)
		{
			CurrentGameUI()->OnInventoryAction(pIItem, GE_OWNERSHIP_TAKE);
		}
		else if(CurrentGameUI()->ActorMenu().GetMenuMode()==mmDeadBodySearch)
		{
			if(m_pOwner==CurrentGameUI()->ActorMenu().GetPartner())
				CurrentGameUI()->OnInventoryAction(pIItem, GE_OWNERSHIP_TAKE);
		}
	};
}

bool CInventory::DropItem(CGameObject *pObj, bool just_before_destroy, bool dont_create_shell) 
{
	CInventoryItem *pIItem				= smart_cast<CInventoryItem*>(pObj);
	VERIFY								(pIItem);
	VERIFY								(pIItem->m_pInventory);
	VERIFY								(pIItem->m_pInventory==this);
	VERIFY								(pIItem->m_ItemCurrPlace.type!=eItemPlaceUndefined);
	
	pIItem->object().processing_activate(); 
	
	switch(pIItem->CurrPlace())
	{
	case eItemPlaceBelt:{
			VERIFY(InBelt(pIItem));
			TIItemContainer::iterator temp_iter = std::find(m_belt.begin(), m_belt.end(), pIItem);
			if (temp_iter != m_belt.end())
			{
				m_belt.erase(temp_iter);
			} else
			{
				Msg("! ERROR: CInventory::Drop item not found in belt...");
			}
			pIItem->object().processing_deactivate();
		}break;
	case eItemPlaceRuck:{
			VERIFY(InRuck(pIItem));
			TIItemContainer::iterator temp_iter = std::find(m_ruck.begin(), m_ruck.end(), pIItem);
			if (temp_iter != m_ruck.end())
			{
				m_ruck.erase(temp_iter);
			} else
			{
				Msg("! ERROR: CInventory::Drop item not found in ruck...");
			}
		}break;
	case eItemPlaceSlot:{
			VERIFY			(InSlot(pIItem));
			if(m_iActiveSlot == pIItem->CurrSlot())
			{
				CActor* pActor	= smart_cast<CActor*>(m_pOwner);
				if (!pActor || pActor->g_Alive())
				{
					if (just_before_destroy)
					{
						Activate		(NO_ACTIVE_SLOT, true);
					} else 
					{
						Activate		(NO_ACTIVE_SLOT);
					}
				}
			}

			if (pIItem->CurrSlot() >= BOUCH_UP_1 && pIItem->CurrSlot() != NO_ACTIVE_SLOT) 
				m_slots[pIItem->CurrSlot()].eraseItem(pIItem);
			else
				m_slots[pIItem->CurrSlot()].m_pIItem = NULL;							
			pIItem->object().processing_deactivate();
		}break;
	default:
		NODEFAULT;
	};
	TIItemContainer::iterator it = std::find(m_all.begin(), m_all.end(), pIItem);
	if(it!=m_all.end())
		m_all.erase(std::find(m_all.begin(), m_all.end(), pIItem));
	else
		Msg("! CInventory::Drop item not found in inventory!!!");

	pIItem->m_pInventory = NULL;


	m_pOwner->OnItemDrop	(smart_cast<CInventoryItem*>(pObj), just_before_destroy);

	CalcTotalWeight					();
	InvalidateState					();
	UpdateZayaz();
	m_drop_last_frame				= true;

	if( CurrentGameUI() )
	{
		CObject* pActor_owner = smart_cast<CObject*>(m_pOwner);

		if (Level().CurrentViewEntity() == pActor_owner)
			CurrentGameUI()->OnInventoryAction(pIItem, GE_OWNERSHIP_REJECT);
	};
	pObj->H_SetParent(0, dont_create_shell);
	return							true;
}

//�������� ���� � ����
bool CInventory::Slot(u16 slot_id, PIItem pIItem, bool bNotActivate, bool strict_placement) 
{
	VERIFY(pIItem);
	
	if(ItemFromSlot(slot_id) == pIItem)
		return false;

	if (!IsGameTypeSingle())
	{
		u16 real_parent = pIItem->object().H_Parent() ? pIItem->object().H_Parent()->ID() : u16(-1);
		if (GetOwner()->object_id() != real_parent)
		{
			Msg("! WARNING: CL: actor [%d] tries to place to slot not own item [%d], that has parent [%d]",
				GetOwner()->object_id(), pIItem->object_id(), real_parent);
			return false;
		}
	}

	if(!strict_placement && !CanPutInSlot(pIItem,slot_id)) 
	{
#ifdef _DEBUG
		Msg("there is item %s[%d,%x] in slot %d[%d,%x]", 
				ItemFromSlot(pIItem->GetSlot())->object().cName().c_str(), 
				ItemFromSlot(pIItem->GetSlot())->object().ID(), 
				ItemFromSlot(pIItem->GetSlot()), 
				pIItem->GetSlot(), 
				pIItem->object().ID(),
				pIItem);
#endif
		return false;
	}
	for (u32 i = 0; i < m_slots.size(); i++) {
		if (m_slots[i].m_pIItem == pIItem)
		{
			if (i == m_iActiveSlot)
				Activate(NO_ACTIVE_SLOT);
			m_slots[i].m_pIItem = NULL;
			break;
		}
		if (i >= BOUCH_UP_1) {
			if (m_slots[i].checkItem(pIItem)) {
				m_slots[i].eraseItem(pIItem);
				break;
			}
		}
	}

	if (slot_id < BOUCH_UP_1)
		m_slots[slot_id].m_pIItem = pIItem;
	else
		m_slots[slot_id].addItem(pIItem);
	
	//������� �� ������� ��� �����
	TIItemContainer::iterator it_ruck = std::find(m_ruck.begin(), m_ruck.end(), pIItem);
	TIItemContainer::iterator it_belt = std::find(m_belt.begin(), m_belt.end(), pIItem);
	if (!IsGameTypeSingle())
	{
		if (it_ruck != m_ruck.end())
		{
			m_ruck.erase(it_ruck);
			R_ASSERT(it_belt == m_belt.end());
		} else if(it_belt != m_belt.end())
		{
			m_belt.erase(it_belt);
			R_ASSERT(it_ruck == m_ruck.end());
		} else
		{
			u16 real_parent = pIItem->object().H_Parent() ? pIItem->object().H_Parent()->ID() : u16(-1);
			R_ASSERT2(GetOwner()->object_id() == real_parent,
				make_string("! ERROR: CL: actor [%d] doesn't contain [%d], real parent is [%d]", 
					GetOwner()->object_id(), pIItem->object_id(), real_parent).c_str()
			);
		}
#ifdef MP_LOGGING
		Msg("--- Actor [%d] places to slot item [%d]", GetOwner()->object_id(), pIItem->object_id());
#endif //#ifdef MP_LOGGING
	} else
	{
		if (it_ruck != m_ruck.end())
			m_ruck.erase(it_ruck);
		if (it_belt != m_belt.end())
			m_belt.erase(it_belt);
	}

	bool in_slot = InSlot(pIItem);
	if(in_slot && (pIItem->CurrSlot()!=slot_id) && slot_id) 
	{
		if(GetActiveSlot() == pIItem->CurrSlot()) 
			Activate(NO_ACTIVE_SLOT);

		m_slots[pIItem->CurrSlot()].m_pIItem = NULL;
	}

	if (((m_iActiveSlot==slot_id) ||(m_iActiveSlot==NO_ACTIVE_SLOT) && m_iNextActiveSlot==NO_ACTIVE_SLOT) && (!bNotActivate))
	{
		Activate				(slot_id);
	}
	SInvItemPlace p					= pIItem->m_ItemCurrPlace;
	m_pOwner->OnItemSlot			(pIItem, pIItem->m_ItemCurrPlace);
	pIItem->m_ItemCurrPlace.type	= eItemPlaceSlot;
	pIItem->m_ItemCurrPlace.slot_id = slot_id;
	pIItem->OnMoveToSlot			(p);
	
	pIItem->object().processing_activate();
	UpdateZayaz();
	return						true;
}

bool CInventory::Belt(PIItem pIItem, bool strict_placement) 
{
	if(!strict_placement && !CanPutInBelt(pIItem))	return false;
	
	//���� ���� � �����
	bool in_slot = InSlot(pIItem);
	if(in_slot) 
	{
		if(GetActiveSlot() == pIItem->CurrSlot()) 
			Activate(NO_ACTIVE_SLOT);

		m_slots[pIItem->CurrSlot()].m_pIItem = NULL;
	}
	
	m_belt.insert(m_belt.end(), pIItem); 

	if(!in_slot)
	{
		TIItemContainer::iterator it = std::find(m_ruck.begin(), m_ruck.end(), pIItem); 
		if(m_ruck.end() != it) 
			m_ruck.erase(it);
	}

	CalcTotalWeight					();
	InvalidateState					();

	SInvItemPlace p					= pIItem->m_ItemCurrPlace;
	pIItem->m_ItemCurrPlace.type	= eItemPlaceBelt;
	m_pOwner->OnItemBelt			(pIItem, p);
	pIItem->OnMoveToBelt			(p);

	if(in_slot)
		pIItem->object().processing_deactivate();

	pIItem->object().processing_activate();
	UpdateZayaz();
	return true;
}

bool CInventory::Ruck(PIItem pIItem, bool strict_placement) 
{
	if(!strict_placement && !CanPutInRuck(pIItem)) return false;

	if (!IsGameTypeSingle())
	{
		u16 real_parent = pIItem->object().H_Parent() ? pIItem->object().H_Parent()->ID() : u16(-1);
		if (GetOwner()->object_id() != real_parent)
		{
			Msg("! WARNING: CL: actor [%d] tries to place to ruck not own item [%d], that has parent [%d]",
				GetOwner()->object_id(), pIItem->object_id(), real_parent);
			return false;
		}
	}
	
	bool in_slot = InSlot(pIItem);
	//���� ���� � �����
	if(in_slot) 
	{
		if(GetActiveSlot() == pIItem->CurrSlot()) 
			Activate(NO_ACTIVE_SLOT);
		
		if (pIItem->CurrSlot() < BOUCH_UP_1) {
			m_slots[pIItem->CurrSlot()].m_pIItem = NULL;
		}
		else {
			m_slots[pIItem->CurrSlot()].eraseItem(pIItem);
		}
	}else
	{
		//���� ���� �� ����� ��� ������ ������ ������� � �����
		TIItemContainer::iterator it = std::find(m_belt.begin(), m_belt.end(), pIItem); 
		if(m_belt.end() != it) 
			m_belt.erase(it);

		if (!IsGameTypeSingle())
		{
			u16 item_parent_id = pIItem->object().H_Parent() ? pIItem->object().H_Parent()->ID() : u16(-1) ;
			u16 inventory_owner_id = GetOwner()->object_id();
			R_ASSERT2(item_parent_id == inventory_owner_id,
				make_string("! ERROR: CL: Actor[%d] tries to place to ruck not own item [%d], real item owner is [%d]",
				inventory_owner_id, pIItem->object_id(), item_parent_id).c_str()
			);
#ifdef MP_LOGGING
			Msg("--- Actor [%d] place to ruck item [%d]", inventory_owner_id, pIItem->object_id());
#endif
		}
	}
	
	m_ruck.insert									(m_ruck.end(), pIItem); 
	
	CalcTotalWeight									();
	InvalidateState									();
	UpdateZayaz();
	m_pOwner->OnItemRuck							(pIItem, pIItem->m_ItemCurrPlace);
	SInvItemPlace prev_place						= pIItem->m_ItemCurrPlace;
	pIItem->m_ItemCurrPlace.type					= eItemPlaceRuck;
	pIItem->OnMoveToRuck							(prev_place);

	if(in_slot)
		pIItem->object().processing_deactivate();

	return true;
}

void CInventory::Activate(u16 slot, bool bForce)
{
    if (!OnServer())
    {
        return;
    }
	if(slot==GRENADE_SLOT)
	{	
		if (!ItemFromSlot(slot)){
			PIItem gr = SameSlot(ZAYAZ_GREN, NULL, false);
			if(gr)
				Slot(GRENADE_SLOT,gr);
		}
	}

    PIItem tmp_item = NULL;
    if (slot != NO_ACTIVE_SLOT)
        tmp_item = ItemFromSlot(slot);

    if (tmp_item && IsSlotBlocked(tmp_item) && (!bForce))
    {
        SetPrevActiveSlot(slot);
        return;
    }

    if (GetActiveSlot() == slot || (GetNextActiveSlot() == slot && !bForce))
    {
        m_iNextActiveSlot = slot;
#ifdef DEBUG
//		Msg("--- There's no need to activate slot [%d], next active slot is [%d]", slot, m_iNextActiveSlot);
#endif
        return;
    }

    R_ASSERT2(slot <= LastSlot(), "wrong slot number");

    if (slot != NO_ACTIVE_SLOT && !m_slots[slot].CanBeActivated())
        return;

#ifdef DEBUG
//	Msg("--- Activating slot [%d], inventory owner: [%s], Frame[%d]", slot, m_pOwner->Name(), Device.dwFrame);
#endif // #ifdef DEBUG

    //�������� ���� �� ������
    if (GetActiveSlot() == NO_ACTIVE_SLOT)
    {
        if (tmp_item)
        {
            m_iNextActiveSlot = slot;
        }
        else
        {
            if (slot == GRENADE_SLOT) // fake for grenade
            {
                PIItem gr = SameSlot(GRENADE_SLOT, NULL, true);
                if (gr)
                    Slot(GRENADE_SLOT, gr);
            }
        }
    }
    //�������� ���� ������������
    else if (slot == NO_ACTIVE_SLOT || tmp_item)
    {
        PIItem active_item = ActiveItem();
        if (active_item && !bForce)
        {
            CHudItem* tempItem = active_item->cast_hud_item();
            R_ASSERT2(tempItem, active_item->object().cNameSect().c_str());

            tempItem->SendDeactivateItem();
#ifdef DEBUG
//			Msg("--- Inventory owner [%s]: send deactivate item [%s]", m_pOwner->Name(), active_item->NameItem());
#endif // #ifdef DEBUG
        }
        else // in case where weapon is going to destroy
        {
            if (tmp_item)
                tmp_item->ActivateItem();

            m_iActiveSlot = slot;
        }
        m_iNextActiveSlot = slot;
    }
}
PIItem CInventory::ItemFromSlot(u16 slot) const
{
	VERIFY(NO_ACTIVE_SLOT != slot);
	return m_slots[slot].m_pIItem;
}

void CInventory::SendActionEvent(u16 cmd, u32 flags) 
{
	CActor *pActor = smart_cast<CActor*>(m_pOwner);
	if (!pActor) return;

	NET_Packet		P;
	pActor->u_EventGen		(P,GE_INV_ACTION, pActor->ID());
	P.w_u16					(cmd);
	P.w_u32					(flags);
	P.w_s32					(pActor->GetZoomRndSeed());
	P.w_s32					(pActor->GetShotRndSeed());
	pActor->u_EventSend		(P, net_flags(TRUE, TRUE, FALSE, TRUE));
};

bool CInventory::Action(u16 cmd, u32 flags) 
{
	CActor *pActor = smart_cast<CActor*>(m_pOwner);
	
	if (pActor)
	{
		switch(cmd)
		{
			case kWPN_FIRE:
			{
				pActor->SetShotRndSeed();
			}break;
			case kWPN_ZOOM : 
			{
				pActor->SetZoomRndSeed();
			}break;
		};
	};

	if (g_pGameLevel && OnClient() && pActor) 
	{
		switch(cmd)
		{
		case kUSE:		break;
		
		case kDROP:		
			{
				if ((flags & CMD_STOP) && !IsGameTypeSingle())
				{
					PIItem tmp_item = ActiveItem();
					if (tmp_item)
					{
						tmp_item->DenyTrade();
					}
				}
				SendActionEvent	(cmd, flags);
				return			true;
			}break;

		case kWPN_NEXT:
		case kWPN_RELOAD:
		case kWPN_FIRE:
		case kWPN_FUNC:
		case kWPN_FIREMODE_NEXT:
		case kWPN_FIREMODE_PREV:
		case kWPN_ZOOM	 : 
		case kTORCH:
		case kNIGHT_VISION:
			{
				SendActionEvent(cmd, flags);
			}break;
		}
	}


	if (	ActiveItem() && 
			ActiveItem()->Action(cmd, flags)) 
											return true;
	bool b_send_event = false;
	switch(cmd) 
	{
	case kWPN_1:
	case kWPN_2:
	case kWPN_3:
	case kWPN_4:
	case kWPN_5:
	case kWPN_6:
		{
			b_send_event = true;
			if (cmd == kWPN_6 && !IsGameTypeSingle()) return false;
			
			u16 slot = u16(cmd - kWPN_1 + 1);
			if ( flags & CMD_START )
			{
				ActiveWeapon( slot );
			}
		}break;
	case kARTEFACT:
		{
		    b_send_event = true;
			if(flags&CMD_START)
			{
                if(GetActiveSlot() == ARTEFACT_SLOT &&
					ActiveItem() /*&& IsGameTypeSingle()*/)
				{
					Activate(NO_ACTIVE_SLOT);
				}else {
					Activate(ARTEFACT_SLOT);
				}
			}
		}break;
	}

	if(b_send_event && g_pGameLevel && OnClient() && pActor)
			SendActionEvent(cmd, flags);

	return false;
}

void CInventory::ActiveWeapon( u16 slot )
{
	if ( GetActiveSlot() == slot && ActiveItem() )
	{
		if ( IsGameTypeSingle() )
			Activate(NO_ACTIVE_SLOT);
		else
			ActivateNextItemInActiveSlot();

		return;
	}
	Activate(slot);
}

void CInventory::Update() 
{
	if( OnServer() )
	{
		if(m_iActiveSlot!=m_iNextActiveSlot)
		{
			CObject* pActor_owner = smart_cast<CObject*>(m_pOwner);
			if (Level().CurrentViewEntity() == pActor_owner)
			{
				if(	(m_iNextActiveSlot!=NO_ACTIVE_SLOT) && 
					 ItemFromSlot(m_iNextActiveSlot)	&&
					 !g_player_hud->allow_activation(ItemFromSlot(m_iNextActiveSlot)->cast_hud_item())
				   )
				   return;
			}
			if( ActiveItem() )
			{
				CHudItem* hi = ActiveItem()->cast_hud_item();
				
				if(!hi->IsHidden())
				{
					if(hi->GetState()==CHUDState::eIdle && hi->GetNextState()==CHUDState::eIdle)
						hi->SendDeactivateItem();

					UpdateDropTasks	();
					return;
				}
			}
			
			if (GetNextActiveSlot() != NO_ACTIVE_SLOT)
			{
				PIItem tmp_next_active = ItemFromSlot(GetNextActiveSlot());
				if (tmp_next_active)
				{
					if (IsSlotBlocked(tmp_next_active))
					{
						Activate(m_iActiveSlot);
						return;
					} else
					{
						tmp_next_active->ActivateItem();
					}
				}
			}

			m_iActiveSlot			= GetNextActiveSlot();
		}
		if((GetNextActiveSlot()!=NO_ACTIVE_SLOT) && ActiveItem() && ActiveItem()->cast_hud_item()->IsHidden())
				ActiveItem()->ActivateItem();
	}
	UpdateDropTasks	();
}

void CInventory::UpdateDropTasks()
{
	//��������� �����
	for(u16 i=FirstSlot(); i<=LastSlot(); ++i)	
	{
		PIItem itm = ItemFromSlot(i);
		if(itm)
			UpdateDropItem		(itm);
	}

	for(u32 i = 0; i < 2; ++i)	
	{
		TIItemContainer &list			= i?m_ruck:m_belt;
		TIItemContainer::iterator it	= list.begin();
		TIItemContainer::iterator it_e	= list.end();
	
		for( ;it!=it_e; ++it)
		{
			UpdateDropItem		(*it);
		}
	}

	if (m_drop_last_frame)
	{
		m_drop_last_frame			= false;
		m_pOwner->OnItemDropUpdate	();
	}
}

void CInventory::UpdateDropItem(PIItem pIItem)
{
	if( pIItem->GetDropManual() )
	{
		pIItem->SetDropManual(FALSE);
		pIItem->DenyTrade();

		if ( OnServer() ) 
		{
			NET_Packet					P;
			pIItem->object().u_EventGen	(P, GE_OWNERSHIP_REJECT, pIItem->object().H_Parent()->ID());
			P.w_u16						(u16(pIItem->object().ID()));
			pIItem->object().u_EventSend(P);
		}
	}// dropManual
}

//���� �� ����� ������� ������ ����
PIItem CInventory::Same(const PIItem pIItem, bool bSearchRuck) const
{
	const TIItemContainer &list = bSearchRuck ? m_ruck : m_zayaz;

	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		const PIItem l_pIItem = *it;
		
		if((l_pIItem != pIItem) && 
				!xr_strcmp(l_pIItem->object().cNameSect(), 
				pIItem->object().cNameSect())) 
			return l_pIItem;
	}
	return NULL;
}

//���� �� ����� ���� ��� ����� 

PIItem CInventory::SameSlot(const u16 slot, PIItem pIItem, bool bSearchRuck) const
{
	if(slot == NO_ACTIVE_SLOT) 	return NULL;

	const TIItemContainer &list = bSearchRuck ? m_ruck : m_zayaz;
	
	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem _pIItem = *it;
		if(_pIItem&&_pIItem != pIItem && _pIItem->BaseSlot() == slot) return _pIItem;
	}

	return NULL;
}

//����� � ��������� ���� � ��������� ������
PIItem CInventory::Get(LPCSTR name, bool bSearchRuck) const
{
	const TIItemContainer &list = bSearchRuck ? m_ruck : m_zayaz;
	
	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(!xr_strcmp(pIItem->object().cNameSect(), name) && 
								pIItem->Useful()) 
				return pIItem;
	}
	return NULL;
}

PIItem CInventory::Get(CLASS_ID cls_id, bool bSearchRuck) const
{
	const TIItemContainer &list = bSearchRuck ? m_ruck : m_belt;
	
	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->object().CLS_ID == cls_id && 
								pIItem->Useful()) 
				return pIItem;
	}
	return NULL;
}

PIItem CInventory::Get(const u16 id, bool bSearchRuck) const
{
	const TIItemContainer &list = bSearchRuck ? m_ruck : m_belt;

	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->object().ID() == id) 
			return pIItem;
	}
	return NULL;
}

//search both (ruck and belt)
PIItem CInventory::GetAny(LPCSTR name) const
{
	PIItem itm = Get(name, false);
	if(!itm)
		itm = Get(name, true);
	return itm;
}

PIItem CInventory::item(CLASS_ID cls_id) const
{
	const TIItemContainer &list = m_all;

	for(TIItemContainer::const_iterator it = list.begin(); list.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(pIItem->object().CLS_ID == cls_id && 
			pIItem->Useful()) 
			return pIItem;
	}
	return NULL;
}

float CInventory::TotalWeight() const
{
	VERIFY(m_fTotalWeight>=0.f);
	return m_fTotalWeight;
}


float CInventory::CalcTotalWeight()
{
	float weight = 0;
	for(TIItemContainer::const_iterator it = m_all.begin(); m_all.end() != it; ++it) 
		weight += (*it)->Weight();

	m_fTotalWeight = weight;
	return m_fTotalWeight;
}


u32 CInventory::dwfGetSameItemCount(LPCSTR caSection, bool SearchAll)
{
	u32			l_dwCount = 0;
	TIItemContainer	&l_list = m_zayaz;
	for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (!xr_strcmp(l_pIItem->object().cNameSect(), caSection))
            ++l_dwCount;
	}
	
	return		(l_dwCount);
}
u32		CInventory::dwfGetGrenadeCount(LPCSTR caSection, bool SearchAll)
{
	u32			l_dwCount = 0;
	TIItemContainer	&l_list = SearchAll ? m_all : m_ruck;
	for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (l_pIItem->object().CLS_ID == CLSID_GRENADE_F1 || l_pIItem->object().CLS_ID == CLSID_GRENADE_RGD5)
			++l_dwCount;
	}

	return		(l_dwCount);
}
u32		CInventory::dwfGetZayazCount(u32 Type)
{
	u32			l_dwCount = 0;
	TIItemContainer	&l_list = m_zayaz;
	if (Type == 1){
		for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
		{
			PIItem item	= *l_it;
			if (item){
				CEatableItem* Eatable = smart_cast<CEatableItem*>(item);
				if (Eatable)
					++l_dwCount;
			}
		}
	}
	
	else if (Type == 2){
		for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
		{
			PIItem item	= *l_it;
			if (item){
				CGrenade* grenade = smart_cast<CGrenade*>(item);
				if (grenade)
					++l_dwCount;
			}
		}
	}
	
	else if (Type == 3){
		for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
		{
			PIItem item	= *l_it;
			if (item){
				CWeaponAmmo* ammo = smart_cast<CWeaponAmmo*>(item);
				if (ammo&&ammo->BaseSlot()!=ZAYAZ_GREN_W)
					l_dwCount += ammo->m_boxCurr;
			}
		}
	}
	
	else if (Type == 4){
		for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
		{
			PIItem item	= *l_it;
			if (item){
				CWeaponAmmo* ammo = smart_cast<CWeaponAmmo*>(item);
				if (ammo&&ammo->BaseSlot()==ZAYAZ_GREN_W)
					l_dwCount += ammo->m_boxCurr;
			}
		}
	}
	return		(l_dwCount);
}

bool CInventory::bfCheckForObject(ALife::_OBJECT_ID tObjectID)
{
	TIItemContainer	&l_list = m_all;
	for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (l_pIItem->object().ID() == tObjectID)
			return(true);
	}
	return		(false);
}

CInventoryItem *CInventory::get_object_by_id(ALife::_OBJECT_ID tObjectID)
{
	TIItemContainer	&l_list = m_all;
	for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it) 
	{
		PIItem	l_pIItem = *l_it;
		if (l_pIItem->object().ID() == tObjectID)
			return	(l_pIItem);
	}
	return		(0);
}

//������� ������� 
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
bool CInventory::Eat(PIItem pIItem)
{
	//����������� �������� �� ����
	CEatableItem* pItemToEat = smart_cast<CEatableItem*>(pIItem);
	if ( !pItemToEat )			return false;

	CEntityAlive *entity_alive = smart_cast<CEntityAlive*>(m_pOwner);
	if ( !entity_alive )		return false;

	CInventoryOwner* IO	= smart_cast<CInventoryOwner*>(entity_alive);
	if ( !IO )					return false;
	
	CInventory* pInventory = pItemToEat->m_pInventory;
	if ( !pInventory || pInventory != this )	return false;
	if ( pInventory != IO->m_inventory )		return false;
	if ( pItemToEat->object().H_Parent()->ID() != entity_alive->ID() )		return false;
	
	if (!pItemToEat->UseBy(entity_alive))
		return false;

#ifdef MP_LOGGING
	Msg( "--- Actor [%d] use or eat [%d][%s]", entity_alive->ID(), pItemToEat->object().ID(), pItemToEat->object().cNameSect().c_str() );
#endif // MP_LOGGING

	if(IsGameTypeSingle() && Actor()->m_inventory == this)
		Actor()->callback(GameObject::eUseObject)((smart_cast<CGameObject*>(pIItem))->lua_game_object());

	if(pItemToEat->Empty())
	{
		pIItem->SetDropManual(TRUE);
		return		false;
	}
	return			true;
}

bool CInventory::ClientEat(PIItem pIItem)
{
	CEatableItem* pItemToEat = smart_cast<CEatableItem*>(pIItem);
	if ( !pItemToEat )			return false;

	CEntityAlive *entity_alive = smart_cast<CEntityAlive*>(m_pOwner);
	if ( !entity_alive )		return false;

	CInventoryOwner* IO	= smart_cast<CInventoryOwner*>(entity_alive);
	if ( !IO )					return false;
	
	CInventory* pInventory = pItemToEat->m_pInventory;
	if ( !pInventory || pInventory != this )	return false;
	if ( pInventory != IO->m_inventory )		return false;
	if ( pItemToEat->object().H_Parent()->ID() != entity_alive->ID() )		return false;
	
	NET_Packet						P;
	CGameObject::u_EventGen			(P, GEG_PLAYER_ITEM_EAT, pIItem->parent_id());
	P.w_u16							(pIItem->object().ID());
	CGameObject::u_EventSend		(P);
	return true;
}

bool CInventory::InSlot(const CInventoryItem* pIItem) const
{
	if(pIItem->CurrPlace() != eItemPlaceSlot)	return false;

	VERIFY(m_slots[pIItem->CurrSlot()].m_pIItem == pIItem);

	return true;
}

bool CInventory::InBelt(const CInventoryItem* pIItem) const
{
	if(Get(pIItem->object().ID(), false)) return true;
	return false;
}

bool CInventory::InRuck(const CInventoryItem* pIItem) const
{
	if(Get(pIItem->object().ID(), true)) return true;
	return false;
}


bool CInventory::CanPutInSlot(PIItem pIItem, u16 slot_id) const
{
	if(!m_bSlotsUseful) return false;

	if( !GetOwner()->CanPutInSlot(pIItem, slot_id ) ) return false;
	CCustomOutfit* pOutfit = m_pOwner->GetOutfit();
	if(slot_id==HELMET_SLOT)
	{
		if(pOutfit && !pOutfit->bIsHelmetAvaliable)
			return false;
	}
	if(slot_id==BACKPACK_SLOT)
	{
		if(pOutfit && !pOutfit->bIsBackpackAvaliable)
			return false;
	}
	if(slot_id==BOUCH_UP_2){
		CGameObject* GOU = smart_cast<CGameObject*>(ItemFromSlot(BOUCH_LEFT_SLOT));
		if (!xr_strcmp(GOU->cNameSect(), "bouch_small"))
			return false;
	}
	if(slot_id==BOUCH_DOWN_2){
		CGameObject* GOD = smart_cast<CGameObject*>(ItemFromSlot(BOUCH_RIGHT_SLOT));
		if (!xr_strcmp(GOD->cNameSect(), "bouch_small"))
			return false;
	}
	if(slot_id==ZAYAZ_AMMO_3){
		CGameObject* GOD = smart_cast<CGameObject*>(ItemFromSlot(ZAYAZ_SLOT));
		if (!xr_strcmp(GOD->cNameSect(), "zayaz_1"))
			return false;
	}
	if(slot_id==ZAYAZ_AMMO_4){
		CGameObject* GOD = smart_cast<CGameObject*>(ItemFromSlot(ZAYAZ_SLOT));
		if (!xr_strcmp(GOD->cNameSect(), "zayaz_1")||!xr_strcmp(GOD->cNameSect(), "zayaz_2"))
			return false;
	}
	if(slot_id==NO_ACTIVE_SLOT)
	{
			return false;
	}

	CGameObject* GO1 = smart_cast<CGameObject*>(pIItem);
	u32 w1 = pSettings->r_u32(GO1->cNameSect(),"inv_grid_width");
	u32 h1 = pSettings->r_u32(GO1->cNameSect(),"inv_grid_height");
	u32 ww = w1;
	u32 hh = h1;
	for (u16 i = BOUCH_UP_1; i < ZAYAZ_GREN+1; ++i) {
		if (slot_id==i) {
			PIItem item = m_slots[slot_id].m_pIItem;
			if (item){
				CGameObject* GO2 = smart_cast<CGameObject*>(item);
				ww += pSettings->r_u32(GO2->cNameSect(),"inv_grid_width");
				hh += pSettings->r_u32(GO2->cNameSect(),"inv_grid_height");
			}
			if (m_slots[slot_id].m_ammunitions.size()) {
				for(TIItemContainer::const_iterator it = m_slots[slot_id].m_ammunitions.begin(); m_slots[slot_id].m_ammunitions.end() != it; ++it) 
				{
					PIItem itm = *it;
					if (itm){
						CGameObject* GO = smart_cast<CGameObject*>(itm);
						ww += pSettings->r_u32(GO->cNameSect(),"inv_grid_width");
						hh += pSettings->r_u32(GO->cNameSect(),"inv_grid_height");
					}
				}
			}
			if(ww<=SlotWidth(slot_id))
				return true;
			else {
				if(hh<=SlotHeight(slot_id))
					return true;
				return false;
			}
		}
	}

	if(slot_id!=NO_ACTIVE_SLOT && 
		NULL==ItemFromSlot(slot_id) )
		return true;
	
	return false;
}
void CInventory::UpdateZayaz()
{
	m_zayaz.clear();
	for ( u8 i = BOUCH_UP_1; i < ZAYAZ_GREN+1; i++ ) {
		PIItem slot_it = m_slots[i].m_pIItem;
		if (!!slot_it)
			m_zayaz.push_back(slot_it);
		
		if (m_slots[i].m_ammunitions.size()) {
			for(TIItemContainer::const_iterator it = m_slots[i].m_ammunitions.begin(); m_slots[i].m_ammunitions.end() != it; ++it) 
			{
				PIItem pIItem = *it;
				if (pIItem)
					if(!slot_it || (!!slot_it && slot_it->object().ID() != pIItem->object().ID()))
						m_zayaz.push_back(pIItem);
			}
		}
	}
}
//��������� ����� �� ��������� ���� �� ����,
//��� ���� ������� ������ �� ��������
bool CInventory::CanPutInBelt(PIItem pIItem)
{
	if(InBelt(pIItem))					return false;
	if(!m_bBeltUseful)					return false;
	if(!pIItem || !pIItem->Belt())		return false;
	if(m_belt.size() >= BeltWidth())	return false;

	return FreeRoom_inBelt(m_belt, pIItem, BeltWidth(), 1);
}
//��������� ����� �� ��������� ���� � ������,
//��� ���� ������� ������ �� ��������
bool CInventory::CanPutInRuck(PIItem pIItem) const
{
	if(InRuck(pIItem)) return false;
	return true;
}

u32	CInventory::dwfGetObjectCount()
{
	return		(m_all.size());
}

CInventoryItem	*CInventory::tpfGetObjectByIndex(int iIndex)
{
	if ((iIndex >= 0) && (iIndex < (int)m_all.size())) {
		TIItemContainer	&l_list = m_all;
		int			i = 0;
		for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it, ++i) 
			if (i == iIndex)
                return	(*l_it);
	}
	else {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"invalid inventory index!");
		return	(0);
	}
	R_ASSERT	(false);
	return		(0);
}

CInventoryItem	*CInventory::GetItemFromInventory(LPCSTR caItemName)
{
	TIItemContainer	&l_list = m_all;

	u32 crc = crc32(caItemName, xr_strlen(caItemName));

	for(TIItemContainer::iterator l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
		if ((*l_it)->object().cNameSect()._get()->dwCRC == crc){
			VERIFY(	0 == xr_strcmp( (*l_it)->object().cNameSect().c_str(), caItemName)  );
			return	(*l_it);
		}
	return	(0);
}


bool CInventory::CanTakeItem(CInventoryItem *inventory_item) const
{
	VERIFY			(inventory_item);
	VERIFY			(m_pOwner);

	if (inventory_item->object().getDestroy()) return false;

	if(!inventory_item->CanTake()) return false;

	for(TIItemContainer::const_iterator it = m_all.begin(); it != m_all.end(); it++)
		if((*it)->object().ID() == inventory_item->object().ID()) break;
	VERIFY3(it == m_all.end(), "item already exists in inventory",*inventory_item->object().cName());

	CActor* pActor = smart_cast<CActor*>(m_pOwner);
	//����� ������ ����� ����� ����
	if(!pActor && (TotalWeight() + inventory_item->Weight() > m_pOwner->MaxCarryWeight()))
		return	false;

	return	true;
}


u32  CInventory::BeltWidth() const
{
	CActor* pActor = smart_cast<CActor*>( m_pOwner );
	if ( pActor )
	{
		PIItem item = ItemFromSlot(ART_BELT_SLOT);
		if ( item )
			return item->GetArtCount();
	}
	return 0; //m_iMaxBelt;
}
u32  CInventory::SlotWidth(u16 slot_id) const
{
	u32 w = 1;
	for ( u8 i = BOUCH_UP_1; i < ZAYAZ_AMMO_4+1; i++ ) {
		if (slot_id == i) {
			w = 2;
		}
	}
	return w;
}

u32  CInventory::SlotHeight(u16 slot_id) const
{
	u32 h = 1;
	PIItem item = ItemFromSlot(ZAYAZ_SLOT);
	CGameObject* GO = smart_cast<CGameObject*>(item);
	if (slot_id == ZAYAZ_GREN||slot_id == ZAYAZ_GREN_W) {
		h = 2;
	}
	if (item&&slot_id == ZAYAZ_GREN_W&&!xr_strcmp(GO->cNameSect(), "zayaz_1")) {
		h = 0;
	}
	if (item&&slot_id == ZAYAZ_MED_1) {
		h = item->GetMed1Count();
	}
	if (item&&slot_id == ZAYAZ_MED_2) {
		h = item->GetMed2Count();
	}
	return h;
}
void  CInventory::AddAvailableItems(TIItemContainer& items_container, bool for_trade) const
{
	for(TIItemContainer::const_iterator it = m_ruck.begin(); m_ruck.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		if(!for_trade || pIItem->CanTrade())
			items_container.push_back(pIItem);
	}

	if(m_bBeltUseful)
	{
		for(TIItemContainer::const_iterator it = m_belt.begin(); m_belt.end() != it; ++it) 
		{
			PIItem pIItem = *it;
			if(!for_trade || pIItem->CanTrade())
				items_container.push_back(pIItem);
		}
	}	
	if(m_bSlotsUseful)
	{
		u16 I = FirstSlot();
		u16 E = LastSlot();
		for(;I<=E;++I)
		{
			PIItem item = ItemFromSlot(I);
			if(item && (!for_trade || item->CanTrade())  )
			{
				if(!SlotIsPersistent(I) || item->BaseSlot()==GRENADE_SLOT )
					items_container.push_back(item);
			}
		}
	}	
}

bool CInventory::isBeautifulForActiveSlot	(CInventoryItem *pIItem)
{
	if (!IsGameTypeSingle()) 
		return (true);

	u16 I = FirstSlot();
	u16 E = LastSlot();
	for(;I<=E;++I)
	{
		PIItem item = ItemFromSlot(I);
		if (item && item->IsNecessaryItem(pIItem))
			return		(true);
	}
	return				(false);
}

//.#include "WeaponHUD.h"
void CInventory::Items_SetCurrentEntityHud(bool current_entity)
{
	TIItemContainer::iterator it;
	for(it = m_all.begin(); m_all.end() != it; ++it) 
	{
		PIItem pIItem = *it;
		CWeapon* pWeapon = smart_cast<CWeapon*>(pIItem);
		if (pWeapon)
		{
			pWeapon->InitAddons();
			pWeapon->UpdateAddonsVisibility();
		}
	}
};

//call this only via Actor()->SetWeaponHideState()
void CInventory::SetSlotsBlocked(u16 mask, bool bBlock)
{
	R_ASSERT(OnServer() || Level().IsDemoPlayStarted());

	for(u16 i = FirstSlot(), ie = LastSlot(); i <= ie; ++i)
	{
		if(mask & (1<<i))
		{
			if (bBlock)
				BlockSlot(i);
			else
				UnblockSlot(i);
		}
	}
	
	if (bBlock)
	{
		TryDeactivateActiveSlot();	
	} else
	{
		TryActivatePrevSlot();
	}
}

void CInventory::TryActivatePrevSlot()
{
	u16 ActiveSlot		= GetActiveSlot();
	u16 PrevActiveSlot	= GetPrevActiveSlot();
	u16 NextActiveSlot	= GetNextActiveSlot();
	if ((
			(ActiveSlot == NO_ACTIVE_SLOT) ||
			(NextActiveSlot == NO_ACTIVE_SLOT)
		) &&
		(PrevActiveSlot != NO_ACTIVE_SLOT))
	{
		PIItem prev_active_item = ItemFromSlot(PrevActiveSlot);
		if (prev_active_item &&
			!IsSlotBlocked(prev_active_item) &&
			m_slots[PrevActiveSlot].CanBeActivated())
		{
#ifndef MASTER_GOLD
			Msg("Set slots blocked: activating prev slot [%d], Frame[%d]", PrevActiveSlot, Device.dwFrame);
#endif // #ifndef MASTER_GOLD
			Activate(PrevActiveSlot);
			SetPrevActiveSlot(NO_ACTIVE_SLOT);
		}
	}
}

void CInventory::TryDeactivateActiveSlot	()
{
	u16 ActiveSlot		= GetActiveSlot();
	u16 NextActiveSlot	= GetNextActiveSlot();

	if ((ActiveSlot == NO_ACTIVE_SLOT) && (NextActiveSlot == NO_ACTIVE_SLOT))
		return;
	
	PIItem		active_item = (ActiveSlot != NO_ACTIVE_SLOT) ? 
		ItemFromSlot(ActiveSlot) : NULL;
	PIItem		next_active_item = (NextActiveSlot != NO_ACTIVE_SLOT) ?
		ItemFromSlot(NextActiveSlot) : NULL;

	if (active_item &&
		(IsSlotBlocked(active_item) || !m_slots[ActiveSlot].CanBeActivated())
		)
	{
#ifndef MASTER_GOLD
		Msg("Set slots blocked: activating slot [-1], Frame[%d]", Device.dwFrame);
#endif // #ifndef MASTER_GOLD
		ItemFromSlot(ActiveSlot)->DiscardState();
		Activate			(NO_ACTIVE_SLOT);
		SetPrevActiveSlot	(ActiveSlot);
	} else if (next_active_item &&
		(IsSlotBlocked(next_active_item) || !m_slots[NextActiveSlot].CanBeActivated())
		)
	{
		Activate			(NO_ACTIVE_SLOT);
		SetPrevActiveSlot	(NextActiveSlot);
	}
}

void CInventory::BlockSlot(u16 slot_id)
{
	VERIFY(slot_id <= LAST_SLOT);
	
	++m_blocked_slots[slot_id];
	
	VERIFY2(m_blocked_slots[slot_id] < 5,
		make_string("blocked slot [%d] overflow").c_str());	
}

void CInventory::UnblockSlot(u16 slot_id)
{
	VERIFY(slot_id <= LAST_SLOT);
	VERIFY2(m_blocked_slots[slot_id] > 0,
		make_string("blocked slot [%d] underflow").c_str());	
	
	--m_blocked_slots[slot_id];	
}

bool CInventory::IsSlotBlocked(u16 slot_id) const
{
	VERIFY(slot_id <= LAST_SLOT);
	return m_blocked_slots[slot_id] > 0;
}

bool CInventory::IsSlotBlocked(PIItem const iitem) const
{
	VERIFY(iitem);
	return IsSlotBlocked(iitem->BaseSlot());
}