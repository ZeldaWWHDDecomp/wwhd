/* Windfall ferris wheel. HD: actor prefix grows by 0x11C; collider sizes remain unchanged.
 * GameCube unknown field names retained. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "f_op/f_op_actor.h"
namespace daObjFerris {
struct Act_c : fopAc_ac_c {
    enum Param_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    /* 0x3AC */ gptr<dBgW> mpBgW[6];
    /* 0x3C4 */ be<s32> mRideState[6];
    /* 0x3DC */ cXyz mRidePos;
    /* 0x3E8 */ request_of_phase_process_class mPhs;
    /* 0x3F0 */ dCcD_Stts mGondolaStts[5];  /* was field_0x02d4: _create 0233DD60: dCcD_Stts::Init of each gondola's first cylinder (tww: mGondolaCyl[i].SetStts(&it[i])) */
    /* 0x51C */ dCcD_Cyl mGondolaCyl[5];  /* was field_0x0400: set_collision 0233E9C4: per gondola, R 50 / H 195 at gondola-local (0, -100, -135) while the gondola is below 1820 (tww set_collision, M_cyl_src); _create 0233DD60 constructs / Sets it */
    /* 0xB0C */ dCcD_Stts mGondolaStts2[5];  /* was field_0x09f0: _create 0233DD60: dCcD_Stts::Init of each gondola's second cylinder (tww: mGondolaCyl2[i].SetStts(&it[i])) */
    /* 0xC38 */ dCcD_Cyl mGondolaCyl2[5];  /* was field_0x0b1c: set_collision 0233E9C4: per gondola, R 50 / H 148 at gondola-local (0, -56, 127) (tww set_collision, M_cyl_src) */
    /* 0x1228 */ dCcD_Stts mHangStts[5];  /* was field_0x110c: _create 0233DD60: dCcD_Stts::Init of each hanging-point sphere (tww: mHangSph[i].SetStts(&it[i])) */
    /* 0x1354 */ dCcD_Sph mHangSph[5];  /* was field_0x1238: set_collision 0233E9C4: per gondola, R 30 sphere at the gondola's hanging point on the turning wheel (the five rim offsets rotated by mRotAngle) while below 1820 (tww set_collision, M_sph_src) */
    /* 0x1930 */ gptr<J3DModel> mpModel[6];
    /* 0x1948 */ Mtx34 mMtx[6];
    /* 0x1A68 */ be<s16> mRotTimer;
    /* 0x1A6A */ be<s16> mRotState;
    /* 0x1A6C */ be<s16> mRotAngle;
    /* 0x1A6E */ be<s16> mRotSpeed;
    /* 0x1A70 */ be<s16> mEventIdx_kanban_vive;
    /* 0x1A72 */ be<s16> mEventIdx_kanban_start;
    /* 0x1A74 */ be<s16> mFrameTimer;
    /* 0x1A76 */ be<s16> mGondolaWaveAngle;
    /* 0x1A78 */ be<s16> mGondolaWaveTimer;
    /* 0x1A7A */ be<s16> mRideWaveAngle[5];
    /* 0x1A84 */ be<s16> mRideWaveTarget[5];
    /* 0x1A8E */ be<s16> mEventIdx;
    /* 0x1A90 */ be<s16> mEventState;
    /* 0x1A92 */ be<s16> field_0x1976;
    static const char M_arcname[8];
    void set_mtx(s32);
    void init_mtx();
    BOOL create_heap();
    BOOL _create();
    BOOL _delete();
    BOOL set_event(s16);
    bool now_event(s16);
    void angle_mng();
    void rot_mng();
    void exe_event();
    void make_lean();
    void set_collision();
    BOOL _execute();
    BOOL _draw();
};
WWHD_OFFSET(Act_c, mpBgW, 0x3AC);
WWHD_OFFSET(Act_c, mRideState, 0x3C4);
WWHD_OFFSET(Act_c, mRidePos, 0x3DC);
WWHD_OFFSET(Act_c, mPhs, 0x3E8);
WWHD_OFFSET(Act_c, mGondolaStts, 0x3F0);
WWHD_OFFSET(Act_c, mGondolaCyl, 0x51C);
WWHD_OFFSET(Act_c, mGondolaStts2, 0xB0C);
WWHD_OFFSET(Act_c, mGondolaCyl2, 0xC38);
WWHD_OFFSET(Act_c, mHangStts, 0x1228);
WWHD_OFFSET(Act_c, mHangSph, 0x1354);
WWHD_OFFSET(Act_c, mpModel, 0x1930);
WWHD_OFFSET(Act_c, mMtx, 0x1948);
WWHD_OFFSET(Act_c, mRotTimer, 0x1A68);
WWHD_OFFSET(Act_c, mRotState, 0x1A6A);
WWHD_OFFSET(Act_c, mRotAngle, 0x1A6C);
WWHD_OFFSET(Act_c, mRotSpeed, 0x1A6E);
WWHD_OFFSET(Act_c, mEventIdx_kanban_vive, 0x1A70);
WWHD_OFFSET(Act_c, mEventIdx_kanban_start, 0x1A72);
WWHD_OFFSET(Act_c, mFrameTimer, 0x1A74);
WWHD_OFFSET(Act_c, mGondolaWaveAngle, 0x1A76);
WWHD_OFFSET(Act_c, mGondolaWaveTimer, 0x1A78);
WWHD_OFFSET(Act_c, mRideWaveAngle, 0x1A7A);
WWHD_OFFSET(Act_c, mRideWaveTarget, 0x1A84);
WWHD_OFFSET(Act_c, mEventIdx, 0x1A8E);
WWHD_OFFSET(Act_c, mEventState, 0x1A90);
WWHD_OFFSET(Act_c, field_0x1976, 0x1A92);
WWHD_SIZE(Act_c, 0x1A94);
} // namespace daObjFerris

void ferrisNormalizeRS(cXyz *);
void ferrisSoundVolume(u32, cXyz *, u32);
void ferrisSound(u32);
s32 ferrisAtan2s(f32, f32);

struct FerrisBgView {
    u8 _00[0xA8];
    be<u32> mCorrectCallback;
    u8 _ac[4];
    gfn mRideCallback;
};
WWHD_OFFSET(FerrisBgView, mCorrectCallback, 0xA8);
WWHD_OFFSET(FerrisBgView, mRideCallback, 0xB0);
void ferrisOrderEvent(fopAc_ac_c *, s16, s32, s32, s32, s32);
s32 ferrisEventEnd(dEvent_manager_c *, s16);
s32 ferrisIsSwitch(dSv_info_c *, s32, s32);
void ferrisOffSwitch(dSv_info_c *, s32, s32);
void ferrisRegisterCollision(cCcS *, dCcD_GObjInf *);

enum {
    dRes_INDEX_SKANRAN_BDL_SGONDOR_e = 4,
    dRes_INDEX_SKANRAN_BDL_SKANRAN_e = 6,
    dRes_INDEX_SKANRAN_DZB_SGONDOR_e = 10,
    dRes_INDEX_SKANRAN_DZB_SKANRAN_e = 11,
};
dBgW *ferrisConstructBg(dBgW *);
s32 ferrisSetBg(dBgW *, cBgD_t *, u32, Mtx34 *);

// HD: translation-unit vtables and external compiler callbacks.
enum {
    FerrisActorVtable = 0x10028070,
    FerrisSphereConstructor = 0x025166F0,
    FerrisTypicalRotY = 0x024EE658,
    FerrisSphereDestructor = 0x02515AE8,
    FerrisStatusDestructor = 0x02515860,
    FerrisCylinderDestructor = 0x02515A70,
    FerrisShapeAttrVtable = 0x100015A8,
    FerrisAabVtable = 0x10028060,
    FerrisCylinderHitVtable = 0x1004B108,
    FerrisCylinderGeometryVtable = 0x1004B150,
    FerrisCylinderShapeVtable = 0x1004B160,
};
struct FerrisVectorBits {
    be<u32> x, y, z;
};
WWHD_SIZE(FerrisVectorBits, 12);
void ferrisConstructActor(fopAc_ac_c *);
void ferrisConstructArray(void *, u32, u32, u32);
s32 ferrisEventIndex(dEvent_manager_c *, const char *, s32);

enum { FerrisInitialized = 8 };
struct FerrisRegistration {
    be<u32> words[3];
};
struct FerrisShapeView {
    u8 _00[0x18];
    be<u32> mAabVtable;
    be<u32> mShapeVtable;
};
WWHD_OFFSET(FerrisShapeView, mAabVtable, 0x18);
WWHD_OFFSET(FerrisShapeView, mShapeVtable, 0x1C);
WWHD_SIZE(FerrisShapeView, 0x20);
void ferrisRegisterDestructor(FerrisRegistration *);
void ferrisConstructSignedChar(be<u8> *);
void ferrisConstructUnsignedChar(be<u8> *);
void ferrisDelete(void *);
void *ferrisAllocate(u32);
void ferrisConstructGObj(dCcD_GObjInf *);
void ferrisConstructCyl(cM3dGCyl *);
void ferrisDestroyArray(void *, u32, u32, u32, s32, s32);
void ferrisDestroyActor(fopAc_ac_c *, s32);
