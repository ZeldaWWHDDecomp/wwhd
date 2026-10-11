/** Auction flag adapters. */
#include "d/d_auction_screen.h"

using AuctionStatePtr = gptr<dAuction_state_c>;
static AuctionStatePtr auctionState;

// HD: These adapters return void; the GameCube functions return BOOL.

/* 0261BC08 */
void dAuction_screen_talkStart() {
    auctionState->mpController->talkmode = 1;
}

/* 0261BC20 */
void dAuction_screen_talkEnd() {
    auctionState->mpController->talkmode = 0;
}

/* 0261BC38 */
void dAuction_screen_slotShow() {
    auctionState->mpController->slotShow = 1;
}

/* 0261BC50 */
void dAuction_screen_slotHide() {
    auctionState->mpController->slotShow = 0;
}

/* 0261BC68 */
void dAuction_screen_gaugeShow() {
    auctionState->mpController->gaugeShow = 1;
}

/* 0261BC80 */
void dAuction_screen_gaugeHide() {
    auctionState->mpController->gaugeShow = 0;
}

/* WWHD glue
data auctionState 101F8344
*/
