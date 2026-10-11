/** HD HUD heart refresh, selection and refill; private decomp source. */
#include "bindings.h"
#include "d/hd_screen_command_a.h"
static hdHudSaveInfo* sSaveInfo;
static u8 sHeartVisible;
static hdHudStateDescriptor sHeartHiddenState;

/* 0266D334 */
static void refreshHudHearts(hdHudHeart* i_this) {
    for (u32 i = 0; i < 20; i++) {
        hdHudHeartChild* child = i_this->heart(i);
        u32 value = hdHudGetHeartValue(i_this, i);
        hdHudSetHeartValue(child, value);
    }
}

/* 0266D588 */
static void selectHudHeart(hdHudHeart* i_this) {
    u32 shown = u32(i_this->mShownContainers) * 4;
    u32 maximum = i_this->mShownLife;
    u32 amount = maximum > shown ? shown : maximum;
    u32 index = amount != 0 ? (amount - 1) >> 2 : 0xFFFFFFFFu;
    u32 old = i_this->mSelectedHeart;
    if (old == index) {
        return;
    }
    if (s32(index) >= 0) {
        i_this->heart(index)->mNextState = 1;
        old = i_this->mSelectedHeart;
    }
    if (s32(old) >= 0) {
        i_this->heart(old)->mNextState = 2;
    }
    i_this->mSelectedHeart = index;
}

/* 0266D624 */
static void refillHudHearts(hdHudHeart* i_this) {
    u32 timer = i_this->mRefillTimer;
    hdHudSaveInfo* save;
    if (timer != 0) {
        i_this->mRefillTimer = timer - 1;
        save = sSaveInfo;
    } else {
        save = sSaveInfo;
        u32 target = u32(save->mStatusA.mMaxLife) >> 2;
        u32 shown = i_this->mShownContainers;
        if (shown != target) {
            i_this->mRefillTimer = 8;
            if (target > shown) {
                hdHudSetHeartVisible(i_this->heart(shown), 1);
                i_this->mShownContainers = u32(i_this->mShownContainers) + 1;
            } else {
                shown--;
                i_this->mShownContainers = shown;
                hdHudSetHeartVisible(i_this->heart(shown), 0);
            }
            selectHudHeart(i_this);
            save = sSaveInfo;
        }
    }
    u32 target = save->mStatusA.mLife;
    u32 shown = i_this->mShownLife;
    if (shown != target) {
        i_this->mShownLife = shown < target ? shown + 1 : shown - 1;
        refreshHudHearts(i_this);
        selectHudHeart(i_this);
    }
    if (sHeartVisible == 0) {
        hdHudChangeState(&i_this->mStateMachine, &sHeartHiddenState);
    }
}

/* WWHD glue (tools/verify/autoglue.py): HD heart/save state
 data sSaveInfo 101F84DC
 data sHeartVisible 1047B08F
 data sHeartHiddenState 104947F4
 ext 0266D308 u32 hdHudGetHeartValue(hdHudHeart*, u32)
 ext 026F7090 void hdHudSetHeartValue(hdHudHeartChild*, u32)
 ext 026F711C void hdHudSetHeartVisible(hdHudHeartChild*, u32)
*/
