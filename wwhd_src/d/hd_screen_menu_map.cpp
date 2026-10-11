#include "d/hd_gamepad_map.h"
using namespace gabi;
static f32 fdivs(f32 a, f32 b) { return f32(f64(a) / f64(b)); }
static HdPadUiPointer l_hdMenuMapUi;
static HdPadState l_menuMapWaitState;
static u32 l_hdMenuMapOpenSound;
static u32 l_hdMenuMapCloseSound;

/* 0269A6E4 */
void hdMenuMapDrawMap(HdPadMenuMap *self) {
    HdPadUi *ui = l_hdMenuMapUi.mOwner->mUi;
    HdPadMapOwner *map = self->mMap;
    void *icons = map->mIcons;
    HdPadMapCell *cell = hdGridFind(map->mCells, &ui->mCoordinate);
    u32 iconFrame = self->mIconFrame;
    if (iconFrame - HD_PAD_MENU_OPEN_BEGIN < HD_PAD_MENU_TRANSITION_FRAMES) {
        hdPadIconsOpening(icons);
        iconFrame = self->mIconFrame;
    } else if (iconFrame - HD_PAD_MENU_WAIT_BEGIN < HD_PAD_MENU_TRANSITION_FRAMES) {
        if (iconFrame == HD_PAD_MENU_WAIT_BEGIN)
            hdPadIconsWaitInit(icons);
        hdPadIconsWaiting(icons);
        iconFrame = self->mIconFrame;
    } else if (iconFrame - HD_PAD_MENU_CLOSE_BEGIN < HD_PAD_MENU_TRANSITION_FRAMES) {
        hdPadIconsClosing(icons);
        iconFrame = self->mIconFrame;
    } else if (iconFrame - HD_PAD_MENU_FINISH_BEGIN < HD_PAD_MENU_TRANSITION_FRAMES) {
        hdPadIconsFinish(icons);
        iconFrame = self->mIconFrame;
    }
    u32 frame = self->mSoundFrame;
    self->mIconFrame = iconFrame + 1;
    if (frame == 30 || frame == 44) {
        hdPadUiSound(&l_hdMenuMapOpenSound);
        frame = self->mSoundFrame;
    } else if (frame == 81) {
        hdPadPlaySound(0x8000005Du);
        frame = self->mSoundFrame;
    } else if (frame == 121 || frame == 128) {
        hdPadUiSound(&l_hdMenuMapCloseSound);
        frame = self->mSoundFrame;
    }
    self->mSoundFrame = frame + 1;
    if (hdPadAnimationEnded(self->mAnimation->mPlayer, 6u) && hdPadIconsEnded(icons) &&
        hdPadCellEnded(cell))
        hdPadChangeState(&self->mState, &l_menuMapWaitState);
    map = self->mMap;
    if (map)
        hdPadMapBeginDraw(map);
    call_ptr<void>(self->mVTable[HD_PAD_DRAW].get(), self);
    call_ptr<void>(self->mVTable[HD_PAD_UPDATE].get(), self);
    map = self->mMap;
    if (map)
        hdPadMapEndDraw(map);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_hdMenuMapUi 101F8344
data l_menuMapWaitState 1049751C
data l_hdMenuMapOpenSound 100FB444
data l_hdMenuMapCloseSound 100FB430
*/
