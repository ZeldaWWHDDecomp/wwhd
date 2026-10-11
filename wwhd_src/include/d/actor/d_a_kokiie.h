#pragma once
#include "gc_api/gc_api.h"
// HD: the actor base is 0x3AC; names below follow the GameCube layout.
struct kokiie_class : fopAc_ac_c {
    kokiie_class() { fopAc_ac_c_ct(this); __vtbl = 0x100134C4; }
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<s16> m298;
    /* 0x3B6 */ be<s16> m29A;
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ be<u8> m2A0;
    /* 0x3BD */ u8 _3BD;
    /* 0x3BE */ be<u8> m2A2;
    /* 0x3BF */ u8 _3BF;
    /* 0x3C0 */ cXyz m2A4;
    /* 0x3CC */ be<f32> m2B0;
    /* 0x3D0 */ u8 _3D0[4];
    /* 0x3D4 */ be<f32> m2B8;
    /* 0x3D8 */ be<f32> m2BC;
    /* 0x3DC */ u8 _3DC[4];
    /* 0x3E0 */ be<f32> m2C4;
    /* 0x3E4 */ csXyz m2C8;
    /* 0x3EA */ u8 _3EA[2];
    /* 0x3EC */ be<f32> m2D0;
    /* 0x3F0 */ be<u32> m2D4[5];
    /* 0x404 */ be<u8> m2E8[5];
    /* 0x409 */ u8 _409[3];
    /* 0x40C */ cXyz m2F0[5];
    /* 0x448 */ be<u8> m32C[5];
    /* 0x44D */ u8 _44D;
    /* 0x44E */ be<s16> m332;
    /* 0x450 */ be<s16> m334;
    /* 0x452 */ u8 _452[2];
    /* 0x454 */ be<f32> m338;
    /* 0x458 */ u8 _458[4];
    /* 0x45C */ Mtx34 m340;
    /* 0x48C */ gptr<dBgW> pm_bgw;
    /* 0x490 */ be<f32> m374;
    /* 0x494 */ be<s16> m378;
    /* 0x496 */ be<s16> m37A;
    /* 0x498 */ cXyz m37C;
    /* 0x4A4 */ cXyz m388;
    /* 0x4B0 */ u8 _4B0[8];
    /* 0x4B8 */ be<f32> m39C;
    /* 0x4BC */ be<f32> m3A0;
};
WWHD_OFFSET(kokiie_class, m2F0, 0x40C);
WWHD_OFFSET(kokiie_class, m340, 0x45C);
WWHD_OFFSET(kokiie_class, m3A0, 0x4BC);
WWHD_SIZE(kokiie_class, 0x4C0);


// HD: only these child-actor fields are used by the house.
struct KokiieShand : fopAc_ac_c {
    u8 padding3AC[0x424 - sizeof(fopAc_ac_c)];
    be<u32> field_308;
    u8 padding428[4];
    gptr<cXyz> field_310;
    gptr<be<u8>> field_314;
};
WWHD_OFFSET(KokiieShand, field_308, 0x424);
WWHD_OFFSET(KokiieShand, field_310, 0x42C);
WWHD_OFFSET(KokiieShand, field_314, 0x430);
struct KokiieAppend {
    be<u32> parameters;
    cXyz position;
    csXyz angle;
    u8 padding16[0xB];
    be<s8> roomNo;
};
WWHD_OFFSET(KokiieAppend, roomNo, 0x21);
struct KokiieCamera {};
struct KokiieCameraProcess {
    u8 padding[0x248];
    KokiieCamera mCamera;
};
struct KokiieBgW { u8 padding[0xA8]; be<u32> mCrrFunc; };
KokiieAppend* Kokiie_createAppend();
void* Kokiie_currentLayer();
u32 Kokiie_request(void*, s16, u32, u32, void*);
void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32);
void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32);
BOOL fopAcM_orderPotentialEvent(fopAc_ac_c*, u16, u16, u16);
void mDoAud_seStart_noPos(u32);
void Kokiie_cameraStop(KokiieCamera*);
void Kokiie_cameraStart(KokiieCamera*);
void Kokiie_cameraTrim(KokiieCamera*, s32);
void Kokiie_cameraReset(KokiieCamera*, cXyz*, cXyz*);
void Kokiie_cameraSet(KokiieCamera*, cXyz*, cXyz*, s16, f32);
void JUTReport(s32, s32, const char*, s32);
u32 Kokiie_addAngles(const csXyz*, const csXyz*);
