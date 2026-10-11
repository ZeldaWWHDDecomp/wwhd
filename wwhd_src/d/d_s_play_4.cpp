/* Play scene part 4. */
// HD: native scene, wind and resource systems extend the GameCube play scene.
#include "d/d_s_play.h"

// HD: see the file comment
/* 025B1418 */
static s32 phase_0(dScnPly::dScnPly_ply_c *i_this) {
    if (dScnPlyAPI_checkAllWaveLoadStatus() != 0)
        return cPhs_INIT_e; /* mDoAud_checkAllWaveLoadStatus */
    u32 darcIdx = dScnPlyAPI_checkEventBit(
                      gabi::ea(&gabi::at<dScnPly_SaveRoot>(l_save.mValue)->mSave.mSavedata.mEvent),
                      0x2D01) != 0; /* dComIfGs_isEventBit(0x2D01) */
    if (darcIdx == (u32)(s32)(s8)dScnPly_gameInfo()->play.mLkDArcIdx)
        return cPhs_NEXT_e;                            /* dComIfGp_getLkDemoAnmNo */
    dScnPly_gameInfo()->play.mLkDArcIdx = (u8)darcIdx; /* dComIfGp_setLkDemoAnmNo */
    gabi::Local<dScnPly_FixedString8> name;
    s32 no = (s8)dScnPly_gameInfo()->play.mLkDArcIdx;
    dScnPlyAPI_formatLinkArchive(gabi::ea(name.get()), gabi::guest_addr("LkD%02d"),
                                 no); /* FixedSafeString<8>("LkD%02d", no) */
    gabi::call_ptr(gabi::at<dScnPly_StringVtable>(name->mVtable)->assureTermination,
                   gabi::ea(name.get())); /* assureTermination */
    u32 str = name->mString;
    u32 play = dComIfGp_ea();
    gabi::Local<dScnPly_SafeString> s;
    s->mVtable = gc_tu.safeString;
    u32 base = gabi::ea(&gabi::at<dScnPly_GameInfo>(play)->play);
    s->mString = str;
    u32 dst = dScnPly_archiveInfo(play)->mpArchiveBuffer.v;
    dScnPlyAPI_assureSafeString(gabi::ea(s.get()));
    /* the SafeString length (0 if longer than 0x40000) */
    u32 p = s->mString;
    s32 len = 0;
    if ((u8)*gabi::at<be<u8>>(p) != 0) {
        for (;;) {
            len++;
            p++;
            if (len > 0x40000) {
                len = 0;
                break;
            }
            if ((u8)*gabi::at<be<u8>>(p) == 0)
                break;
        }
    }
    s32 max = (s32)dScnPly_archiveInfo(base - offsetof(dComIfG_inf_c, play))->mArchiveCapacity;
    if (!(len < max))
        len = max - 1;
    gabi::call_ptr(gabi::at<dScnPly_StringVtable>(s->mVtable)->assureTermination,
                   gabi::ea(s.get()));
    dScnPlyAPI_blockMove(dst, (u32)s->mString, len, 0); /* OSBlockMove (import) */
    *gabi::at<be<u8>>(dst + len) = 0;
    return cPhs_NEXT_e;
}

// HD: see the file comment
/* 025B1590 */
static s32 phase_1(dScnPly::dScnPly_ply_c *i_this) {
    l_roomProcessID.mValue =
        i_this != 0 ? i_this->mBsPcId
                    : (u32)-1; /* dStage_roomControl_c::setProcID(fopScnM_GetID(this)) */
    u32 next = gabi::ea(&dScnPly_gameInfo()->play.mNextStage);
    u32 play = dComIfGp_ea();
    for (int i = 0; i < 6; i++)
        gabi::at<be<u16>>(gabi::ea(&gabi::at<dScnPly_GameInfo>(play)->play.mCurStage))[i] =
            (u16)gabi::at<be<u16>>(next)[i];        /* setStartStage(getNextStartStage()) */
    dScnPly_gameInfo()->play.mNextStage.enable = 0; /* offEnableNextStage */
    u32 stage = gabi::ea(&dScnPly_gameInfo()->play.mCurStage);
    s32 room = (s8)dScnPly_gameInfo()->play.mCurStage.mRoomNo;
    dScnPlyAPI_reportStage(gabi::guest_addr("Start StageName:RoomNo [%s:%d]\\n"), stage,
                           room); /* JUTReportConsole_f("Start StageName:RoomNo [%s:%d]\n") */
    dScnPly_gameInfo()->play.mStatus = 0; /* dComIfGp_setStatus(0) */
    s32 rt = dScnPlyAPI_setStageResource(gabi::guest_addr("Stage"),
                                         0); /* dComIfG_setStageRes("Stage", NULL) */
    if (rt != 1)
        dScnPlyAPI_showAssert(gabi::guest_addr("d_s_play.cpp"), 0x1042,
                              gabi::guest_addr("rt == 1"));
    dScnPlyAPI_createEnvironment(dScnPlyAPI_getZeldaHeap());
    return cPhs_NEXT_e;
}

// HD: glue escapes the newline for its generated C++ registration; guest format bytes are unchanged.
/* WWHD glue
str 1005328C "LkD%02d" in 025B1418
str 10053294 "rt == 1" in 025B1590
str 1005329C "Stage" in 025B1590
str 100532A4 "Start StageName:RoomNo [%s:%d]\\n" renamed in 025B1590
str 100532C4 "d_s_play.cpp" in 025B1590
*/
