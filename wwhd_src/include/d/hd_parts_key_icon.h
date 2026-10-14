/* HD: the "KeyIcon" UI part (hd_parts_key_icon.cpp): a key/button icon that plays In, waits, plays
 * Decide on request (then waits again or goes Out) and Out; states ::StateID_In / _Wait / _Decide /
 * _Out, idle in hdParts_c's ::StateID_Blank. No GameCube counterpart. "KeyIcon", the animation and
 * group names and the state names are HD strings; class, member and method names are ours. Local
 * */
#pragma once
#include "d/hd_parts.h"

/* animation entries of the layout (sAnimNames: "In", "Out", "Loop", "Decide") */
enum hdPartsKeyIconAnim_e {
    KEY_ICON_ANIM_IN = 0,
    KEY_ICON_ANIM_OUT = 1,
    KEY_ICON_ANIM_LOOP = 2,
    KEY_ICON_ANIM_DECIDE = 3,
};
enum hdPartsKeyIconRequest_e {
    KEY_ICON_REQUEST_NONE = 0,
    KEY_ICON_REQUEST_DECIDE = 3,
    KEY_ICON_REQUEST_OUT = 4,
};

struct hdPartsKeyIcon_c : hdParts_c {
    /* 0x50 */ be<s32> mRequest;      /* hdPartsKeyIconRequest_e */
    /* 0x54 */ be<u8> mDecideToWait;  /* after Decide: back to Wait (else Out) */
    /* 0x55 */ u8 _55[3];

    static void destroy(hdPartsKeyIcon_c* i_this, s32 flags);
    void inEnter();
    void inExecute();
    void waitEnter();
    void waitExecute();
    void decideEnter();
    void decideExecute();
    void outEnter();
    void outExecute();
    static hdPartsKeyIcon_c* construct(hdPartsKeyIcon_c* i_this);
    u32 init(void* resource, u32 arg1, u32 arg2, hdPartsDesc_c* desc);
    void finalize();
    void executeState();
    void requestIn();
    void requestDecide();
    void requestDecideAndWait();
    void requestOut();
    void hide();
    u32 isWaiting();
    u8 isNotDeciding();
    u32 isHidden();
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
};
WWHD_OFFSET(hdPartsKeyIcon_c, mRequest, 0x50);
WWHD_OFFSET(hdPartsKeyIcon_c, mDecideToWait, 0x54);
WWHD_SIZE(hdPartsKeyIcon_c, 0x58);
