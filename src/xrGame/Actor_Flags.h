#pragma once

enum{
		AF_GODMODE = (1 << 0),
		AF_NO_CLIP = (1 << 1),
		AF_UNLIMITEDAMMO = (1 << 3),
		AF_RUN_BACKWARD = (1 << 4),
		AF_AUTOPICKUP = (1 << 5),
		AF_PSP = (1 << 6),
		AF_DYNAMIC_MUSIC = (1 << 7),
		AF_GODMODE_RT = (1 << 8),
		AF_IMPORTANT_SAVE = (1 << 9),
		AF_CROUCH_TOGGLE = (1 << 10),
		AF_3DSCOPE_ENABLE = (1 << 11),
		AF_PNV_W_SCOPE_DIS = (1 << 12),
		AF_ZOOM_NEW_FD = (1 << 13),
		AF_ONLY_CAMPFIRES = (1 << 14),
		AF_FIREPOS = (1 << 15),
		AF_WALK_TOGGLE = (1 << 16),
		AF_SPRINT_TOGGLE = (1 << 17),
		AF_LOOKOUT_TOGGLE = (1 << 18),
		AF_FREELOOK_TOGGLE = (1 << 19),
		AF_3D_PDA = (1 << 20),
		AF_SIMPLE_PDA = (1 << 21),
};

extern Flags32	psActorFlags;
extern BOOL		GodMode	();	

extern int		psActorSleepTime;

