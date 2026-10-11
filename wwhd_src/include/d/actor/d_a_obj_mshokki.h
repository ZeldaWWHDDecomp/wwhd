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
    be<s32> m60C;
    u8 m610[2];
    be<s16> m612;
    be<u32> m614, m618;
    be<s16> m61C;
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
WWHD_OFFSET(daObjMshokki_c, m60C, 0x728);
WWHD_OFFSET(daObjMshokki_c, m61C, 0x738);
WWHD_SIZE(daObjMshokki_c, 0x73C);

struct MshData {
    be<f32> m00, radius, height, m0C;
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
