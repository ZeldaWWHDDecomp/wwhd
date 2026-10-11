/** Logo scene resource setup and creation phases, using the tww scene names.
 * Verified against WWHD's recompiled functions.
 * HD: startup also initializes platform, Miiverse, keyboard and save managers. The verified entry
 * points return full unsigned registers, including the phase states and scene Boolean results.
 */
#include "d/d_s_logo.h"
using namespace gabi;

const gc_tu_data gc_tu = {.safeString = 0x100526DC};
static const char* l_objectResName[] = {
    "Always",  "Link",  "LkAnm",    "LkD00", "LkD01", "Agb",  "Yslvg00", "Cloth", "Dalways",
    "Ep",      "Key",   "Kbota_00", "Bk",    "Boko",  "Nata", "Ki",      "Pt",    "Mo2",
    "Kantera", "Spear", "Cc",       "Kui",   "Nz",    "Nzg",  "Knob"};
static u32 l_resourceLoader;
static u32 l_platformMgr;
static u32 l_miiverseBottleMgr;
static u32 l_dynamicLinkReady;
static u32 l_swkbdMgr;
static u32 l_resourceControl;
static u32 l_messageData;
static u32 l_resetState;
static u32 l_saveMgr;
static u8 l_audioInitFlag;
static u32 l_toonImage;
static u32 l_toonExImage;
// HD: four-entry guest dispatch table for the external phase handler.
static cPhs__Handler* const l_method = at<cPhs__Handler>(0x101EA668);

// HD: BFRES dictionary entries use wrapping self-relative pointers; zero stays null.
/* inlined into dScnLogo_Delete */
static u32 relative(be<u32>* p) {
    u32 offset = *p;
    return offset ? gabi::ea(p) + offset : 0;
}
/* 025AC770 */
u32 dScnLogo_IsDelete(dScnLogo_c* i_this) { return 1; }

/* 025ABFD4 */
u32 dScnLogo_Delete(dScnLogo_c* i_this) {
    SafeString left;
    SafeString right;
    left.__vtbl = gc_tu.safeString;
    right.__vtbl = gc_tu.safeString;
    right.mStringTop = guest_addr("Misc.bfres");
    left.mStringTop = guest_addr("Misc");
    if (dScnLogo_loadResourceHD(l_resourceLoader, &left, &right, 0u)) {
        u32 archive = dScnLogo_getResourceFileHD(),
            folder = relative(&at<dScnLogo_ResourceFileHD>(archive)->mFileDictionaryOffset),
            entry = dScnLogo_findArchiveEntryHD(folder, guest_addr("item_table.bin")),
            data = relative(at<be<u32>>(entry));
        dScnLogo_GameInfoHD* g = at<dScnLogo_GameInfoHD>(dComIfGp_getGameInfoHD());
        g->mpItemTable = data;
        folder = relative(&at<dScnLogo_ResourceFileHD>(archive)->mFileDictionaryOffset);
        entry = dScnLogo_findArchiveEntryHD(folder, guest_addr("FmapDat.bin"));
        data = relative(at<be<u32>>(entry));
        g = at<dScnLogo_GameInfoHD>(dComIfGp_getGameInfoHD());
        g->mpFmapData = data;
        folder = relative(&at<dScnLogo_ResourceFileHD>(archive)->mFileDictionaryOffset);
        entry = dScnLogo_findArchiveEntryHD(folder, guest_addr("ActorDat.bin"));
        data = relative(at<be<u32>>(entry));
        g = at<dScnLogo_GameInfoHD>(dComIfGp_getGameInfoHD());
        dADM_SetDataHD(ea(&g->mActorData), data);
    }
    right.__vtbl = gc_tu.safeString;
    left.__vtbl = gc_tu.safeString;
    right.mStringTop = guest_addr("Particle");
    left.mStringTop = guest_addr("Particle.bfres");
    if (dScnLogo_loadResourceHD(l_resourceLoader, &right, &left, 0u)) {
        u32 archive = dScnLogo_getResourceFileHD(),
            folder = relative(&at<dScnLogo_ResourceFileHD>(archive)->mFileDictionaryOffset),
            entry = dScnLogo_findArchiveEntryHD(folder, guest_addr("common.jpc")),
            data = relative(at<be<u32>>(entry));
        dScnLogo_GameInfoHD* g = at<dScnLogo_GameInfoHD>(dComIfGp_getGameInfoHD());
        dPa_control_createCommonHD(g->mParticle, data);
    }
    mDoExt_getMesgFontHD();
    mDoExt_getRubyFontHD();
    dScnLogo_GameInfoHD* g = at<dScnLogo_GameInfoHD>(dComIfGp_getGameInfoHD());
    g->mWindowNum = 0;
    for (u32 n = 0; n < 3; ++n) {
        g = at<dScnLogo_GameInfoHD>(dComIfGp_getGameInfoHD());
        g->mPictureFlag = (u8)(g->mPictureFlag & ~(1u << n));
    }
    mDoExt_setSafeGameHeapSizeHD();
    mDoExt_setSafeZeldaHeapSizeHD();
    mDoExt_setSafeArchiveHeapSizeHD();
    mDoExt_setSafeCommandHeapSizeHD();
    dScnLogo_initializeMessageDataHD(l_messageData);
    dScnLogo_initializeMessageControlHD();
    return 1;
}
// HD: bootstrap platform services before the original resource readiness checks.
/* 025AC214 */
u32 phase_0(dScnLogo_c* i_this) {
    u32 control = dScnLogo_getSystemControlHD();
    dScnLogo_setSystemControlEnabledHD(control, 0u);
    if (!l_platformMgr) {
        dScnLogo_getSystemHeapHD();
        dScnLogo_createPlatformMgrHD();
        dScnLogo_initializePlatformMgrHD(l_platformMgr);
        dScnLogo_startPlatformMgrHD(l_platformMgr);
    }
    if (!l_miiverseBottleMgr) {
        u32 owner = dScnLogo_getHeapHeaderSizeHD(4u) + 0x30D4;
        SafeString name;
        name.__vtbl = gc_tu.safeString;
        name.mStringTop = guest_addr("cking::system::MiiverseBottleMgr");
        u32 heap = dScnLogo_getSystemHeapHD(),
            resource = dScnLogo_createHeapHD(owner, &name, heap, 1u, 0u);
        dScnLogo_createMiiverseBottleMgrHD();
        dScnLogo_initializeMiiverseBottleMgrHD(l_miiverseBottleMgr);
        dScnLogo_startMiiverseBottleMgrHD(l_miiverseBottleMgr);
        call_ptr<void>(at<dScnLogo_HeapHD>(resource)->mVtable->destroy.get(), resource);
    }
    if (l_audioInitFlag && mDoAud_checkFirstWavesHD())
        return cPhs_INIT_e;
    if (!l_dynamicLinkReady)
        return cPhs_INIT_e;
    if (!l_swkbdMgr) {
        u32 owner = dScnLogo_getHeapHeaderSizeHD(4u) + 0x1C8;
        SafeString name;
        name.__vtbl = gc_tu.safeString;
        name.mStringTop = guest_addr("cking::ui::input::SwkbdMgr");
        u32 heap = dScnLogo_getUIHeapHD(),
            resource = dScnLogo_createHeapHD(owner, &name, heap, 1u, 0u);
        dScnLogo_createSwkbdMgrHD();
        dScnLogo_initializeSwkbdMgrHD(l_swkbdMgr);
        call_ptr<void>(at<dScnLogo_HeapHD>(resource)->mVtable->destroy.get(), resource);
    }
    u32 heap = mDoExt_getCommandHeapHD();
    if (dComIfG_setObjectResHD(guest_addr("System"), 0u, 0u) != 1)
        JUTAssertion_showAssertHD(guest_addr("d_s_logo.cpp"), 0x238u, guest_addr("rt == 1"));
    dScnLogo_dumpHeapHD(heap);
    return cPhs_NEXT_e;
}
/* 025AC3F8 */
u32 phase_1(dScnLogo_c* i_this) {
    if (dComIfG_syncAllObjectResHD())
        return cPhs_INIT_e;
    mDoExt_getCommandHeapHD();
    SafeString first;
    first.__vtbl = gc_tu.safeString;
    first.mStringTop = guest_addr("System");
    u32 toonImage = dRes_control_getResHD(l_resourceControl, &first, dRes_INDEX_SYSTEM_BTI_TOON_e);
    if (!toonImage)
        JUTAssertion_showAssertHD(guest_addr("d_s_logo.cpp"), 0x25Au,
                                  guest_addr("toonImage != (0)"));
    u32 owner = l_resourceControl;
    SafeString second;
    second.__vtbl = gc_tu.safeString;
    second.mStringTop = guest_addr("System");
    l_toonImage = toonImage;
    u32 toonExImage = dRes_control_getResHD(owner, &second, dRes_INDEX_SYSTEM_BTI_TOONEX_e);
    if (!toonExImage)
        JUTAssertion_showAssertHD(guest_addr("d_s_logo.cpp"), 0x25Fu,
                                  guest_addr("toonImage != (0)"));
    l_toonExImage = toonExImage;
    return cPhs_NEXT_e;
}
// HD: preload the object-name table and begin a save session; logo images are handled elsewhere.
/* 025AC4D4 */
u32 phase_2(dScnLogo_c* i_this) {
    s32 rt = dComIfG_syncObjectResHD(guest_addr("System"));
    if (rt != 0) {
        if (rt < 0)
            JUTAssertion_showAssertHD(guest_addr("d_s_logo.cpp"), 0x281u, guest_addr("rt >= 0"));
        return cPhs_INIT_e;
    }
    mDoExt_getCommandHeapHD();
    dScnLogo_GameInfoHD* g = at<dScnLogo_GameInfoHD>(dComIfGp_getGameInfoHD());
    dComIfGp_createParticleHD(ea(&g->mPlay));
    for (u32 n = 0; n < 25; ++n) {
        if (dComIfG_setObjectResHD(guest_addr(l_objectResName[n]), 0u, 0u) != 1)
            JUTAssertion_showAssertHD(guest_addr("d_s_logo.cpp"), 0x2B6u, guest_addr("rt == 1"));
    }
    u32 clock = OSGetSystemInfoHD(), ticks = *at<be<u32>>(clock);
    // GHS keeps the bus-clock divide-by-60 reciprocal multiply after the initial divide by four.
    u32 rate = (u32)(((u64)(ticks >> 2) * 0x88888889u) >> 37);
    mDoGph_setTickRateHD(rate);
    mDoGph_waitBlankingHD(0u);
    mDoGph_startFadeInHD(30u, 0u);
    at<dScnLogo_ResetStateHD>(l_resetState)->mReset = 0;
    at<dScnLogo_ResetStateHD>(l_resetState)->mResetPrepare = 0;
    at<dScnLogo_ResetStateHD>(l_resetState)->mResetPrepare = 0;
    dScnLogo_beginSaveSessionHD(l_saveMgr, 0u);
    return cPhs_NEXT_e;
}
// HD: a fourth phase waits for save initialization before restoring system control.
/* 025AC620 */
u32 phase_3(dScnLogo_c* i_this) {
    if (dComIfG_syncAllObjectResHD())
        return cPhs_INIT_e;
    u32 state = l_saveMgr;
    if (at<dScnLogo_SaveMgrHD>(state)->mThreadState == 1)
        return cPhs_INIT_e;
    u32 manager = l_resourceControl,
        heap = ea(at<dScnLogo_ResourceControlHD>(manager)->mHeap.get());
    call_ptr<void>(at<dScnLogo_HeapHD>(heap)->mVtable->dump.get(), heap);
    state = l_saveMgr;
    if (at<dScnLogo_SaveMgrHD>(state)->mResult == 0xFFFFFFFEu && !dScnLogo_openSaveFileHD(state))
        return cPhs_INIT_e;
    u32 control = dScnLogo_getSystemControlHD();
    dScnLogo_setSystemControlEnabledHD(control, 1u);
    return cPhs_COMPLEATE_e;
}
/* 025AC6C8 */
u32 dScnLogo_Create(dScnLogo_c* i_this) {
    return dComLbG_PhaseHandlerHD(ea(&i_this->mPhs), ea(l_method), ea(i_this));
}
// HD: compiler-generated and template code of this translation unit

/* 025AC6DC */
void __sinit_d_s_logo_cpp() {
    sinit_header_statics_rodata(0x1047B37C, 0x101EA678, 0x1047B388, 0x100528B4);
}
/* 025AC778 */
void dScnLogo_SafeStringDestructorHD(u32 p, u32 flags) {
    if (p && (flags & 1))
        operator_deleteHD(p);
}
/* 025AC78C */
void dScnLogo_SafeStringAssureTerminationHD(u32 p) {}

/* WWHD glue
ext 02035D78 u32 dScnLogo_getSystemControlHD()
ext 02035ED4 void dScnLogo_setSystemControlEnabledHD(u32, u32)
ext 02036EEC void dScnLogo_createMiiverseBottleMgrHD()
ext 02036F8C void dScnLogo_initializeMiiverseBottleMgrHD(u32)
ext 02036FD4 void dScnLogo_startMiiverseBottleMgrHD(u32)
ext 0203B88C void dScnLogo_createPlatformMgrHD()
ext 0203B984 void dScnLogo_initializePlatformMgrHD(u32)
ext 0203BBB0 void dScnLogo_startPlatformMgrHD(u32)
ext 0203E8C4 u32 dScnLogo_getSystemHeapHD()
ext 0203E8E8 u32 dScnLogo_getUIHeapHD()
ext 025200D4 u32 dComIfGp_getGameInfoHD()
ext 02520354 u32 dComIfG_setObjectResHD(u32, u32, u32)
ext 025203D8 s32 dComIfG_syncObjectResHD(u32)
ext 02522F50 u32 dComIfG_syncAllObjectResHD()
ext 02524BB0 void dComIfGp_createParticleHD(u32)
ext 02525FE4 u32 dComLbG_PhaseHandlerHD(u32, u32, u32)
ext 02590708 void mDoExt_getMesgFontHD()
ext 025907FC void mDoExt_getRubyFontHD()
ext 025A7838 void dPa_control_createCommonHD(u32, u32)
ext 025ABD4C void dADM_SetDataHD(u32, u32)
ext 025E18AC u32 mDoAud_checkFirstWavesHD()
ext 025E2FF0 void mDoExt_setSafeGameHeapSizeHD()
ext 025E30AC void mDoExt_setSafeZeldaHeapSizeHD()
ext 025E3184 void mDoExt_setSafeArchiveHeapSizeHD()
ext 025E3268 u32 mDoExt_getCommandHeapHD()
ext 025E3274 void mDoExt_setSafeCommandHeapSizeHD()
ext 025F05D0 void mDoGph_startFadeInHD(u32, u32)
ext 025F0968 void mDoGph_waitBlankingHD(u32)
ext 025F096C void mDoGph_setTickRateHD(u32)
ext 026066C4 u32 dRes_control_getResHD(u32, SafeString*, u32)
ext 026124B0 u32 dScnLogo_loadResourceHD(u32, SafeString*, SafeString*, u32)
ext 026193D4 void dScnLogo_createSwkbdMgrHD()
ext 026195C4 void dScnLogo_initializeSwkbdMgrHD(u32)
ext 02708748 void dScnLogo_initializeMessageDataHD(u32)
ext 027089F8 void dScnLogo_initializeMessageControlHD()
ext 02721A18 void dScnLogo_beginSaveSessionHD(u32, u32)
ext 02722894 u32 dScnLogo_openSaveFileHD(u32)
ext 0273AA24 void JUTAssertion_showAssertHD(u32, u32, u32)
ext 0273AF40 void operator_deleteHD(u32)
ext 02753004 u32 dScnLogo_createHeapHD(u32, SafeString*, u32, u32, u32)
ext 0275320C u32 dScnLogo_getHeapHeaderSizeHD(u32)
ext 027DFA24 u32 dScnLogo_findArchiveEntryHD(u32, u32)
ext 027E2DC0 u32 dScnLogo_getResourceFileHD()
ext 027EC3D4 void dScnLogo_dumpHeapHD(u32)
ext 028EAB2C void dScnLogo_initializeAngleTableHD(u32)
ext 028ED6F8 void dScnLogo_initializeMathTableHD(u32)
ext 028F026C void dScnLogo_registerGlobalObjectHD(u32)
ext C0009C80 u32 OSGetSystemInfoHD()
str 100526F4
str 10052708
str 10052714
str 100526FC
str 10052724
str 10052734
str 10052740
str 10052750
str 1005276C
str 10052790
str 10052764 in 025AC214
str 100527AC in 025AC214
str 1005275C in 025AC214
str 100527BC in 025AC3F8
str 100527C4 in 025AC3F8
str 100527D4
str 100527F8 in 025AC4D4
str 10052800 in 025AC4D4
str 100527E8
str 100527F0 in 025AC4D4
data l_objectResName 101EA604
data l_resourceLoader 101F4F7C
data l_platformMgr 1018F504
data l_miiverseBottleMgr 1018F450
data l_dynamicLinkReady 101F4F54
data l_swkbdMgr 101F50D8
data l_resourceControl 101F4F28
data l_messageData 101F8294
data l_resetState 101F4974
data l_saveMgr 101F852C
data l_audioInitFlag 101F4704
data l_toonImage 10475800
data l_toonExImage 10475804
*/
