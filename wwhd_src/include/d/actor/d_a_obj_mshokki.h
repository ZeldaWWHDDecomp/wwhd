#pragma once
#include "gc_api/gc_api.h"
class daObjMshokki_c : public fopAc_ac_c {
  public:
    daObjMshokki_c();
    enum Param_e { PRM_ARG_W = 2, PRM_ARG_S = 0 };
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mModel;
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCir;
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    be<s32> mType;  /* was m60C: _create 0237D260 sets the dish type from the parameter (0 when out of range); create_heap 0237CE88 picks the bdl, _create the cylinder radius / height, break_proc 0237D6DC the particle, set_se 0237D574 the sound and _execute 0237D89C the attention height by it (tww the same) */
    u8 m610[2];
    be<s16> mGroundTouched;  /* was m612: _execute 0237D89C sets 1 once the dish rests on the ground (ground hit without a landing); a later landing (after being knocked up) then breaks it (break_proc) (tww the same) */
    be<u32> mFrameCount;  /* was m614: _execute 0237D89C adds 1 per frame; co_hitCallback 0237CF48 compares it with mLaunchFrame (tww the same) */
    be<u32> mLaunchFrame;  /* was m618: co_hitCallback 0237CF48 stores mFrameCount when the rolling player knocks the dish up, and knocks it up again only more than 2 frames later (tww the same) */
    be<s16> mPushAngleY;  /* was m61C: co_hitCallback 0237CF48 sets the pusher's facing + 0x2000 when it pushes the dish over; set_se 0237D574 plays the rattle only while shape_angle.y is within 4096 of it (tww the same) */
    u8 _73A[2];
    void set_mtx();
    bool create_heap();

    cPhs_State _create();
    bool _delete();
    void set_se();
    void break_proc();
    bool checkCollision();
    bool _execute();
    bool _draw();
};
WWHD_OFFSET(daObjMshokki_c, mModel, 0x3B4);
WWHD_OFFSET(daObjMshokki_c, mAcch, 0x3B8);
WWHD_OFFSET(daObjMshokki_c, mAcchCir, 0x57C);
WWHD_OFFSET(daObjMshokki_c, mStts, 0x5BC);
WWHD_OFFSET(daObjMshokki_c, mCyl, 0x5F8);
WWHD_OFFSET(daObjMshokki_c, mType, 0x728);
WWHD_OFFSET(daObjMshokki_c, mPushAngleY, 0x738);
WWHD_SIZE(daObjMshokki_c, 0x73C);

struct MshData {
    be<f32> mAttentionOffsetY;  /* was m00: _execute 0237D89C: attention_info.position.y = current.pos.y + it (tww the same) */
    be<f32> radius;
    be<f32> height;
    be<f32> mMaxTilt;  /* was m0C: co_hitCallback 0237CF48: the push tilt shape_angle.x is strength * it; set_se 0237D574 scales the sound by the tilt / it (tww the same) */
};
struct MshDataTable {
    MshData values[3];
};
struct MshIntTable {
    be<u32> values[3];
};
struct MshParticleTable {
    be<u16> values[3];
};
struct MshCylinderSource {
    u8 bytes[0x44];
};
struct MshParticleController;
struct MshPlay {
    u8 pad0[0x12A0];
    dBgS mBgS;
    u8 gap[0x26A4 - 0x12A0 - sizeof(dBgS)];
    u8 mCcS;
    u8 gap2[0x5AB0 - 0x26A5];
    gptr<MshParticleController> mParticle;
    u8 gap3[0x5B2C - 0x5AB4];
    gptr<fopAc_ac_c> mPlayer;
};
inline MshPlay *mshPlay() {
    return gabi::at<MshPlay>(dComIfGp_ea());
}
void mshAcchCtor(dBgS_ObjAcch *);
void mshStartCTg(dCcD_Cyl *, cXyz *);
void mshMoveCTg(dCcD_Cyl *, cXyz *);
void mshParticleSet(MshParticleController *, s32, u32, cXyz *, csXyz *, void *, s32, void *, s32,
                    GXColor *, s32, s32);
void mshSoundSet(cXyz *, s32, u32, s32);
f32 mshSqrt(f32);
void mshCcSet(u8 *, dCcD_Cyl *);
void mshWallCylDt(void *, s32);
void mshAcchDt(dBgS_ObjAcch *, s32);
void mshActorDt(void *, s32);
