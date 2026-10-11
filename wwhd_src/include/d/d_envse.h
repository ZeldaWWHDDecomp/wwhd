/* Environmental sound controller. GameCube fields are shifted by +4 after the
 * HD process vtable. Unaccessed room/camera members remain padding. */
#pragma once
#include "bindings.h"
#include "d/d_path.h"
#include "f_pc/f_pc_base.h"

struct dEnvSe_words {
    be<u32> x, y, z;
};
WWHD_SIZE(dEnvSe_words, 12);
struct dEnvSe_soundEntry {
    u8 mUnaccessed[0x14];
    be<u8> field_0x14;
    u8 mPadding[2];
    be<u8> field_0x17;
    be<u8> field_0x18;
    u8 mRemaining[3];
};
WWHD_SIZE(dEnvSe_soundEntry, 0x1C);
WWHD_OFFSET(dEnvSe_soundEntry, field_0x17, 0x17);
WWHD_OFFSET(dEnvSe_soundEntry, field_0x18, 0x18);
struct dEnvSe_soundInfo {
    be<u32> num;
    gptr<dEnvSe_soundEntry> m_entries;
};
WWHD_SIZE(dEnvSe_soundInfo, 8);
struct dEnvSe_roomDt {
    struct VTable {
        u8 mUnaccessed[0x1BC];
        gfn getSoundInf;
    };
    gptr<VTable> __vtbl;
    dEnvSe_soundInfo* getSoundInf() {
        return gabi::call_ptr<dEnvSe_soundInfo*>(__vtbl->getSoundInf.get(), this);
    }
};
WWHD_SIZE(dEnvSe_roomDt, 4);
WWHD_OFFSET(dEnvSe_roomDt::VTable, getSoundInf, 0x1BC);
struct dEnvSe_camera {
    u8 mUnaccessed[0x264];
    cXyz mEye;
    u8 mPadding[0x7C0 - 0x270];
    cXyz mEyeOffset;
};
WWHD_OFFSET(dEnvSe_camera, mEye, 0x264);
WWHD_OFFSET(dEnvSe_camera, mEyeOffset, 0x7C0);
WWHD_SIZE(dEnvSe_camera, 0x7CC);
struct dEnvSe_cameraInfo {
    gptr<dEnvSe_camera> mpCamera;
    u8 mUnaccessed[0x30];
};
WWHD_SIZE(dEnvSe_cameraInfo, 0x34);
struct dEnvSe_gameInfo {
    u8 mUnaccessed[0x51CC];
    u8 mRoomCtrl[0x5AF8 - 0x51CC];
    dEnvSe_cameraInfo mCameraInfo[1];
    u8 mPlayerPrefix[4];
    be<s8> mPlayerCameraID;
    u8 mPadding[0x5D00 - 0x5B31];
    be<u16> mWaveFrame;
};
WWHD_OFFSET(dEnvSe_gameInfo, mRoomCtrl, 0x51CC);
WWHD_OFFSET(dEnvSe_gameInfo, mCameraInfo, 0x5AF8);
WWHD_OFFSET(dEnvSe_gameInfo, mPlayerCameraID, 0x5B30);
WWHD_OFFSET(dEnvSe_gameInfo, mWaveFrame, 0x5D00);
WWHD_SIZE(dEnvSe_gameInfo, 0x5D02);
class dEnvSe_c : public base_process_class {
  public:
    BOOL execute();
    u8 mUnaccessed[0xE0 - 0xB8];
    cXyz mPos;
    u8 mKankyoRemaining[0xFC - 0xEC];
    union {
        be<u32> field_0xf8;
        struct {
            u8 mHigh[2];
            be<u16> mLow;
        } mWaveWords;
    };
    be<u32> field_0xfc;
};
WWHD_OFFSET(dEnvSe_c, mPos, 0xE0);
WWHD_OFFSET(dEnvSe_c, field_0xf8, 0xFC);
WWHD_OFFSET(dEnvSe_c, field_0xfc, 0x100);
WWHD_SIZE(dEnvSe_c, 0x104);
enum dEnvSe_type {
    dEnvSe_SEA_e = 0,
    dEnvSe_SHORE_e = 1,
    dEnvSe_RIVER_e = 2,
    dEnvSe_WATERFALL_e = 3,
    dEnvSe_WINDOW_e = 4,
};
// HD: distance comparisons retain the full double result register.
f64 dEnvSe_squareDistance(const cXyz* a, const cXyz* b);
void dEnvSe_setLine(cM3dGLin* line, const cXyz* start, const cXyz* end);
u32 dEnvSe_projectLine(cM3dGLin* line, const cXyz* point, cXyz* out, be<f32>* distance);
void dEnvSe_addEye(const cXyz* a, cXyz* out, const cXyz* b);
dEnvSe_roomDt* dEnvSe_getStatusRoomDt(void* control, s32 room);
path_layout::dPath* dEnvSe_getRoomPath(u32 index, s32 room);
path_layout::dPath* dPath_GetNextRoomPath(path_layout::dPath* path, s32 room);
// HD: preserve the full reverb result register at the audio calls.
u32 dEnvSe_getReverb(s32 room);
void mDoAud_initSeaEnvPos();
void mDoAud_registSeaEnvPos(cXyz* pos);
void mDoAud_seaEnvSePlay(u32 value);
void mDoAud_startSeaShoreSE(cXyz* pos, u32 value);
void mDoAud_continueSeaShoreSE(cXyz* pos, u32 value);
void mDoAud_registRiverPos(cXyz* pos);
void mDoAud_riverSePlay(u32 value, u32 reverb);
void mDoAud_initRiverPos();
void mDoAud_waterfallSePlay(u32 value, cXyz* pos, u32 reverb);
void mDoAud_initWindowPos();
void mDoAud_registWindowPos(cXyz* pos);
void dEnvSe_processDelete(base_process_class* object, u32 flags);
void dEnvSe_processConstruct(base_process_class* object);
