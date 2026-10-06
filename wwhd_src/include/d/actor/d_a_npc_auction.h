/* Auction NPC layout in the local WWHD compatibility reconstruction. */
#pragma once
#include "d/d_npc.h"
struct daNpcAuction_c : fopNpc_npc_c {
  u8 auctionState[0x8B8 - 0x7DC];
};
WWHD_SIZE(daNpcAuction_c, 0x8B8);
WWHD_OFFSET(daNpcAuction_c, auctionState, 0x7DC);
