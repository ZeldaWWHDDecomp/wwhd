#pragma once
#include "gc_api/gc_api.h"
// HD: the actor base is 0x3AC; names below follow the GameCube layout.
struct kokiie_class : fopAc_ac_c {
    kokiie_class() { fopAc_ac_c_ct(this); __vtbl = 0x100134C4; }
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<s16> mFrameCounter;  /* was m298: kokiie_move (in daKokiie_Execute 021A57A0): + 1 every frame; phase of the wobble and sway (cM_ssin(it * 900) etc.); demo_camera (in daKokiie_Execute 021A57A0) phase of the camera bank shake */
    /* 0x3B6 */ be<s16> mMode;  /* was m29A: kokiie_move (in daKokiie_Execute 021A57A0): 0 hangs on its 5 ropes, 1 falls (set when all five are cut and the Boomerang is owned) and lands on mFallGroundY; daKokiie_Create 021A652C sets 1 when mSwitchNo is on */
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ be<u8> mSizeType;  /* was m2A0: daKokiie_Create 021A652C: param & 0xFF (0xFF -> 0); 1 / 2 / 3 scale the house (x, z) to 0.9 / 0.8 / 0.7, else 1.0 */
    /* 0x3BD */ u8 _3BD;
    /* 0x3BE */ be<u8> mSwitchNo;  /* was m2A2: daKokiie_Create 021A652C: param >> 24; when that switch is on no ropes are created and the house starts fallen; kokiie_move (in daKokiie_Execute 021A57A0) turns it on at the crash (0xFF none) */
    /* 0x3BF */ u8 _3BF;
    /* 0x3C0 */ cXyz mSwayOffset;  /* was m2A4: kokiie_move (in daKokiie_Execute 021A57A0): (mSwayX * sin, y, mSwayZ * sin); current.pos = home.pos + it + mSagY while hanging; y eased to 0 */
    /* 0x3CC */ be<f32> mSwayX;  /* was m2B0: kokiie_move (in daKokiie_Execute 021A57A0): amplitude of mSwayOffset.x, eased to 0 */
    /* 0x3D0 */ u8 _3D0[4];
    /* 0x3D4 */ be<f32> mSwayZ;  /* was m2B8: kokiie_move (in daKokiie_Execute 021A57A0): amplitude of mSwayOffset.z, eased to 0 */
    /* 0x3D8 */ be<f32> mWobbleAmpX;  /* was m2BC: kokiie_move (in daKokiie_Execute 021A57A0): amplitude of mWobbleAngle.x (sin(mFrameCounter * 900)), eased to 300 */
    /* 0x3DC */ u8 _3DC[4];
    /* 0x3E0 */ be<f32> mWobbleAmpZ;  /* was m2C4: kokiie_move (in daKokiie_Execute 021A57A0): amplitude of mWobbleAngle.z (sin(mFrameCounter * 700)), eased to 300 */
    /* 0x3E4 */ csXyz mWobbleAngle;  /* was m2C8: kokiie_move (in daKokiie_Execute 021A57A0): x / z = sin * mWobbleAmpX / mWobbleAmpZ; shape_angle = current.angle + it */
    /* 0x3EA */ u8 _3EA[2];
    /* 0x3EC */ be<f32> mSagY;  /* was m2D0: kokiie_move (in daKokiie_Execute 021A57A0): eased to -himo_off_yp[cut ropes] (step 50 * mTiltStepRate); added to current.pos.y while hanging */
    /* 0x3F0 */ be<u32> mShandIds[5];  /* was m2D4: himo_create (in daKokiie_Create 021A652C): process ids of the 5 SHAND rope actors it creates; fopAcM_SearchByID(it) to link them */
    /* 0x404 */ be<u8> mShandCreateStep[5];  /* was m2E8: himo_create (in daKokiie_Create 021A652C): per rope 0 = create the SHAND actor, 1 = link it (when found), 2 = done */
    /* 0x409 */ u8 _409[3];
    /* 0x40C */ cXyz mRopeEndPos[5];  /* was m2F0: daKokiie_Execute 021A57A0: rope i's end on the house (MtxPosition of (0, -170, 400) turned by i * 0x3333); himo_create (in daKokiie_Create 021A652C) gives the SHAND a pointer to it */
    /* 0x448 */ be<u8> mRopeState[5];  /* was m32C: himo_create (in daKokiie_Create 021A652C) gives the SHAND a pointer to it; the shand writes 1 when cut (hand_move in daShand_Execute 0246FB78) and 2 when back (normal, same); kokiie_move (in daKokiie_Execute 021A57A0): 1 -> 5 (cut, counted), 2 -> 0 */
    /* 0x44D */ u8 _44D;
    /* 0x44E */ be<s16> mTiltAngleY;  /* was m332: kokiie_move (in daKokiie_Execute 021A57A0): eased to himo_off_ya[cut ropes]; daKokiie_Execute 021A57A0 tilts the house by mTiltX about this direction */
    /* 0x450 */ be<s16> mTiltX;  /* was m334: kokiie_move (in daKokiie_Execute 021A57A0): eased to himo_off_xa[cut ropes], back to 0 when falling / landed; daKokiie_Execute 021A57A0 XrotM by it */
    /* 0x452 */ u8 _452[2];
    /* 0x454 */ be<f32> mTiltStepRate;  /* was m338: kokiie_move (in daKokiie_Execute 021A57A0): 1 on a rope cut, 0 when a rope is back, eased to 1; step scale of the mSagY (50 * it) and tilt (10000 * it) eases */
    /* 0x458 */ u8 _458[4];
    /* 0x45C */ Mtx34 mBgMtx;  /* was m340: daKokiie_Execute 021A57A0: the house matrix; CallbackCreateHeap 021A6418: cBgW_Set(pm_bgw, dzb, MOVE_BG, it) */
    /* 0x48C */ gptr<dBgW> pm_bgw;
    /* 0x490 */ be<f32> mFallGroundY;  /* was m374: kokiie_move (in daKokiie_Execute 021A57A0): 1380 (REG0_F(9) + 1380), the falling house stops there; daKokiie_Create 021A652C places a fallen house at it + 770 */
    /* 0x494 */ be<s16> mDemoMode;  /* was m378: kokiie_move (in daKokiie_Execute 021A57A0): 1 when the house falls; demo_camera (in daKokiie_Execute 021A57A0): 1 waits for the event, 2 runs the fall camera, 0 at its end (mDemoTimer 0x96) */
    /* 0x496 */ be<s16> mDemoTimer;  /* was m37A: demo_camera (in daKokiie_Execute 021A57A0): 0 at the start, + 1 per frame while mDemoMode is not 0; kokiie_move crashes at 0x1B, splashes at 0x40, the riddle SE at 100, the demo ends at 0x96 */
    /* 0x498 */ cXyz mDemoEye;  /* was m37C: demo_camera (in daKokiie_Execute 021A57A0): camera eye of the fall demo (house + (500, 0, -1000) turned, y 1730); passed (with the shake) as the eye to the camera Set and Reset */
    /* 0x4A4 */ cXyz mDemoCenter;  /* was m388: demo_camera (in daKokiie_Execute 021A57A0): camera centre of the fall demo (the house, y eased to pos.y - 450); passed (with the shake) as the centre to the camera Set and Reset */
    /* 0x4B0 */ u8 _4B0[8];
    /* 0x4B8 */ be<f32> mShakeAmp;  /* was m39C: kokiie_move (in daKokiie_Execute 021A57A0): 90 on the landing; demo_camera (in daKokiie_Execute 021A57A0) shakes centre / eye by sin / cos * it and the bank by it * 8, eased to 0 */
    /* 0x4BC */ be<f32> mDemoFovy;  /* was m3A0: demo_camera (in daKokiie_Execute 021A57A0): 65 at the start, eased to 80 after frame 0x32; the fovY argument of the camera Set (tww: mCamera.Set(center, eye, bank, fovY)) */
};
WWHD_OFFSET(kokiie_class, mRopeEndPos, 0x40C);
WWHD_OFFSET(kokiie_class, mBgMtx, 0x45C);
WWHD_OFFSET(kokiie_class, mDemoFovy, 0x4BC);
WWHD_SIZE(kokiie_class, 0x4C0);


// HD: only these child-actor fields are used by the house.
struct KokiieShand : fopAc_ac_c {
    u8 padding3AC[0x424 - sizeof(fopAc_ac_c)];
    be<u32> mParentId;
    u8 padding428[4];
    gptr<cXyz> mpRootPos;
    gptr<be<u8>> mpParentState;
};
WWHD_OFFSET(KokiieShand, mParentId, 0x424);
WWHD_OFFSET(KokiieShand, mpRootPos, 0x42C);
WWHD_OFFSET(KokiieShand, mpParentState, 0x430);
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
