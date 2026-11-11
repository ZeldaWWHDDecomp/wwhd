/* Beamos: verified WWHD layout with zeldaret/tww member names.
 * HD: actor base grows to 0x3AC; GHS eye procedure occupies eight bytes; virtual dispatch reads
 * the vtable offset from the low half of the target word, as in the shared ptmf adapter. brk animations grow to 0x78; HIO vtable moves to the tail. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/actor/d_a_beam.h"
#include "d/d_path.h"
struct daBemos_c : fopAc_ac_c {
    s32 CreateHeap1();
    s32 CreateHeap2();
    s32 CreateHeap3();
    s32 CreateHeap();
    s32 blue_eye_wait_init();
    s32 red_eye_wait_init();
    s32 yellow_range_check(s32 range, void* angle);
    s32 blue_eye_search_init();
    s32 red_eye_search_init();
    void* getBeamActor();
    s32 blue_eye_charge_init();
    s32 red_eye_charge_init();
    s32 eye_dummy();
    void set_mtx();
    s32 CreateInit1();
    s32 CreateInit2();
    s32 CreateInit3();
    s32 CreateInit();
    s32 _delete();
    s32 _create();
    s32 _execute();
    s32 _draw();
    void event_move();
    s32 yellow_eye_wait_init();
    s32 blue_eye_wait();
    s32 yellow_eye_search_init();
    s32 red_eye_wait();
    s32 eye_break_init();
    void guard_proc();
    s32 blue_range_check(s32 active, csXyz* angle);
    s32 red_range_check(s32 active, csXyz* angle);
    s32 yellow_eye_wait();
    s32 yellow_eye_search();
    s32 blue_eye_search();
    s32 red_eye_search();
    s32 blue_eye_charge();
    s32 red_eye_charge();
    s32 eye_break();

    /* 0x3AC */ gabi::ptmf<void (daBemos_c::*)()> mEyeProc;  /* was m290: tww's type EyeProc (pointer to member function); blue / red / yellow eye *_init (blue_eye_wait_init 02324428, blue_eye_charge_init 0232666C, red_eye_wait_init 02324830, yellow_eye_wait_init 02324C5C, ...) store the eye state function, _execute 02326020 calls it */
    /* 0x3B4 */ csXyz mHeadAngle;  /* was m29C: set_mtx 023252C8: rotation of the head model (y added to current.angle.y); blue_eye_charge 023268D0 / red_eye_wait 0232728C turn it toward the target or by mRotSpeed; CreateInit3 02324DC8 aims it at the next path point */
    u8 _pad3BA[0x2];
    /* 0x3BC */ cXyz mHeadPos;  /* was m2A4: CreateInit1 02324628 / CreateInit2 02324A2C: current.pos raised by 260 (+REG); set_mtx 023252C8 places the head model there; the beam child (fpcNm_Beam_e) and the BMOSBALL00 particle are created at it; mSph centres on it */
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<J3DModel> mpBodyModel;  /* was m2B8: CreateHeap1 0232409C / CreateHeap2 0232419C / CreateHeap3 0232435C: model BDL_HMOS1 / HMOS2 / HMOS3 (by mType); set_mtx 023252C8 scales and places it at current.pos; _draw 023261A0 draws it; CreateInit 02325558 takes its matrix as cullMtx */
    /* 0x3D4 */ gptr<J3DModel> mpHeadModel;  /* was m2BC: CreateHeap1 0232409C / CreateHeap2 0232419C: model BDL_HMOSUP1 / HMOSUP2 (blue and red types); set_mtx 023252C8 scales it by mHeadScale and places it at mHeadPos turned by mHeadAngle; _draw 023261A0 draws it */
    /* 0x3D8 */ gptr<J3DModel> mpHmos2tModel;  /* was m2C0: CreateHeap2 0232419C: model BDL_HMOS2T with the brk mBrkAnm2 (red type only); set_mtx 023252C8 turns it with the head yaw at current.pos; _draw 023261A0 draws it with mBrkAnm2 */
    u8 _pad3DC[0x4];
    /* 0x3E0 */ mDoExt_brkAnm mBrkAnm1;
    /* 0x458 */ gptr<J3DAnmTevRegKey> m_sch_brk;
    /* 0x45C */ mDoExt_brkAnm mBrkAnm2;
    /* 0x4D4 */ gptr<JPABaseEmitter> mpWaitEmitter;  /* was m2FC: blue_eye_wait_init 02324428 / red_eye_wait_init 02324830 / yellow_eye_wait_init 02324C5C: particle ID_AK_SN_BMOSWAIT00 (wait glow); blue_eye_charge_init 0232666C and event_move 02325B44 (switch on) stop it */
    /* 0x4D8 */ gptr<JPABaseEmitter> mpEyeEmitter;  /* was m300: blue_eye_wait_init 02324428 / red_eye_wait_init 02324830: particle ID_AK_SN_BMOSEYE00; blue_eye_wait 023267CC / red_eye_wait 0232728C move it to mBeamRootPos and set its alpha to mEyeAlpha */
    /* 0x4DC */ gptr<JPABaseEmitter> mpChargeEmitter;  /* was m304: blue_eye_charge_init 0232666C / red_eye_charge_init 0232712C / eye_break_init 0232599C: particle ID_AK_SN_BMOSBALL00 at mHeadPos; blue_eye_wait_init 02324428 / yellow_eye_wait_init 02324C5C stop it */
    /* 0x4E0 */ gptr<JPABaseEmitter> m308;
    /* 0x4E4 */ gptr<JPABaseEmitter> mpLaserRootEmitter;  /* was m30C: blue_eye_charge 023268D0: particle ID_AK_SN_LASERROOT00 at mBeamRootPos once mChargeCount passes mChargeTime, scaled by mLaserRootScale; stopped when the laser starts or the charge ends */
    /* 0x4E8 */ be<u8> mEyeAlpha;  /* was m310: CreateInit1 02324628 / CreateInit2 02324A2C set 255; blue_eye_charge 023268D0 fades it to 0, blue_eye_wait 023267CC / red_eye_wait 0232728C back to 255 (then attention off); the alpha of mpEyeEmitter */
    u8 _pad4E9[0x3];
    /* 0x4EC */ cXyz mLaserRootScale;  /* was m314: blue_eye_charge 023268D0: (6.25 - (mLaserRootTimer - 2.5)^2) * 1.6 in x, y, z, the scale of mpLaserRootEmitter; CreateInit1 / CreateInit2 clear it */
    /* 0x4F8 */ be<f32> mSearchRange;  /* was m320: CreateInit1 02324628 / CreateInit2 02324A2C: param bits 0-7 * 100 + 500; CreateInit3 02324DC8: 1000 or the distance to the target point; blue_range_check 02326274 / red_range_check 02326460: the player is found closer than it */
    u8 _pad4FC[0x30];
    /* 0x52C */ cXyz mBeamRootPos;  /* was m354: set_mtx 023252C8: the point 60 in front of the head (mHeadPos, mHeadAngle); mpEyeEmitter / mpLaserRootEmitter follow it; blue_eye_search 02326E4C / red_eye_search 02327968 put the beam actor there */
    /* 0x538 */ cXyz mHeadScale;  /* was m360: set_mtx 023252C8: base scale of mpHeadModel; CreateInit1 / blue_eye_wait_init 02324428 set (0, 0.75, 1); blue_eye_charge 023268D0 grows x to 1 (HIO mBlueHeadOpenStep) and shrinks it back (HIO mBlueHeadCloseStep), the laser starts once x is 1 */
    /* 0x544 */ cXyz mLastPlayerPos;  /* was m36C: red_eye_search 02327968 stores the player position when the fire time (mFireTimer) ends; red_range_check 02326460 re-arms (mArmed) only once the player is more than 5 away from it or out of the angle */
    /* 0x550 */ dCcD_Stts mSttsCyl;
    /* 0x58C */ dCcD_Cyl mCyl;
    /* 0x6BC */ dCcD_Stts mSttsSph;
    /* 0x6F8 */ dCcD_Sph mSph;
    /* 0x824 */ dBgS_ObjGndChk mGndChk;
    /* 0x878 */ be<f32> mGroundY;  /* was m6A0: CreateInit1 02324628 / CreateInit2 02324A2C: cBgS_GroundCross of mGndChk (the ground under the Beamos); not read in WWHD; tww's daBemos_c::_draw passes it to dComIfGd_setSimpleShadow2 as the shadow ground height */
    /* 0x87C */ be<s32> mInRange;  /* was m6A4: blue_range_check 02326274 / red_range_check 02326460 / yellow_range_check 0232665C store the last result (1 = player in range); the eye states pass it back as the active flag, which widens the range (HIO mBlueWidthInRangeAdd / mBlueRangeInRangeAdd / mRedRangeInRangeAdd) */
    /* 0x880 */ be<s32> mSwitchNo;  /* was m6A8: CreateInit3 02324DC8: param bits 0-7 (passed to the child); yellow_eye_wait 02327D98 turns the beam off and event_move 02325B44 stops mpWaitEmitter while dSv_info_c::isSwitch(it) (255: none) */
    /* 0x884 */ be<s16> mLaserRootTimer;  /* was m6AC: blue_eye_charge 023268D0 counts it up to 5 once fully charged (then blue_eye_search_init) and down again when the player leaves; drives mLaserRootScale; blue_eye_charge_init 0232666C / blue_eye_search_init 023268A8 clear it */
    /* 0x886 */ be<s16> mChargeCount;  /* was m6AE: blue_eye_charge_init 0232666C clears it or sets HIO mBlueChargeResume; blue_eye_charge 023268D0 counts it up to mChargeTime while the player is in range and down otherwise; red_eye_charge 023273F0 up to HIO mRedChargeTime */
    /* 0x888 */ be<s16> mChargeTime;  /* was m6B0: blue_eye_charge_init 0232666C: HIO mBlueChargeTime (from the wait state) or mBlueChargeResume; blue_eye_charge 023268D0: the laser root starts once mChargeCount passes it */
    /* 0x88A */ be<s16> mRotSpeed;  /* was m6B2: CreateInit2 02324A2C: param bits 8-15 (signed) * 250 + 750 / -500; red_eye_wait 0232728C adds it to mHeadAngle.y every frame (the red head turns) */
    u8 _pad88C[0x2];
    /* 0x88E */ be<s16> mFireTimer;  /* was m6B6: red_eye_search_init 023273C0 sets HIO mRedFireTime; red_eye_search 02327968 counts it down while firing and at 0 stores mLastPlayerPos and clears mArmed */
    /* 0x890 */ be<u8> mType;  /* was m6B8: _create 02325698: param bits 28-31; 0 = blue (CreateHeap1 / CreateInit1), 1 = red (2), 2 = yellow (3); set_mtx 023252C8, eye_break_init 0232599C, event_move 02325B44 and guard_proc 02325DB8 branch on it */
    /* 0x891 */ be<u8> mSubType;  /* was m6B9: CreateInit3 02324DC8: param bits 8-15 (255 -> 0), passed to the child; 2: other beam flags and no guard cylinder; yellow_eye_wait 02327D98: 1 turns the beam off while the player is in another room; _execute 02326020 tests 2 */
    /* 0x892 */ be<u8> mSwitchOn;  /* was m6BA: CreateInit3 02324DC8 / event_move 02325B44: the last dSv_info_c::isSwitch(mSwitchNo); event_move starts the hmos3cam event when it turns on */
    /* 0x893 */ be<u8> mEventStep;  /* was m6BB: event_move 02325B44: 1 orders the hmos3cam event, 2 while it runs (cutEnd until it ends, then 0); CreateInit3 02324DC8 clears it */
    /* 0x894 */ be<u8> mArmed;  /* was m6BC: CreateInit 02325558 sets 1; red_eye_search 02327968 clears it when mFireTimer ends; red_range_check 02326460 does not look for the player while it is 0 and sets it again once the player moved more than 5 from mLastPlayerPos or out of the angle */
    /* 0x895 */ be<u8> mHasPath;  /* was m6BD: CreateInit3 02324DC8: 1 when the yellow Beamos takes its position from a path (and creates the second one at the next point); event_move 02325B44 (the switch event) and guard_proc 02325DB8 (guard cylinder) run only for it */
    /* 0x896 */ be<u8> m6BE;
    /* 0x897 */ be<u8> mDropItemNo;  /* was m6BF: CreateInit1 02324628 / CreateInit2 02324A2C: shape_angle.z bits 0-5; eye_break 02328120 passes it to fopAcM_createItemFromTable as the item number */
    /* 0x898 */ be<u8> mDropItemBitNo;  /* was m6C0: CreateInit1 02324628 / CreateInit2 02324A2C: shape_angle.z bits 6-12; eye_break 02328120 passes it to fopAcM_createItemFromTable as the item bit */
    u8 _tail[3];
};
WWHD_OFFSET(daBemos_c, mEyeProc, 0x3AC);
WWHD_OFFSET(daBemos_c, mHeadAngle, 0x3B4);
WWHD_OFFSET(daBemos_c, mHeadPos, 0x3BC);
WWHD_OFFSET(daBemos_c, mPhase, 0x3C8);
WWHD_OFFSET(daBemos_c, mpBodyModel, 0x3D0);
WWHD_OFFSET(daBemos_c, mpHeadModel, 0x3D4);
WWHD_OFFSET(daBemos_c, mpHmos2tModel, 0x3D8);
WWHD_OFFSET(daBemos_c, mBrkAnm1, 0x3E0);
WWHD_OFFSET(daBemos_c, m_sch_brk, 0x458);
WWHD_OFFSET(daBemos_c, mBrkAnm2, 0x45C);
WWHD_OFFSET(daBemos_c, mpWaitEmitter, 0x4D4);
WWHD_OFFSET(daBemos_c, mpEyeEmitter, 0x4D8);
WWHD_OFFSET(daBemos_c, mpChargeEmitter, 0x4DC);
WWHD_OFFSET(daBemos_c, m308, 0x4E0);
WWHD_OFFSET(daBemos_c, mpLaserRootEmitter, 0x4E4);
WWHD_OFFSET(daBemos_c, mEyeAlpha, 0x4E8);
WWHD_OFFSET(daBemos_c, mLaserRootScale, 0x4EC);
WWHD_OFFSET(daBemos_c, mSearchRange, 0x4F8);
WWHD_OFFSET(daBemos_c, mBeamRootPos, 0x52C);
WWHD_OFFSET(daBemos_c, mHeadScale, 0x538);
WWHD_OFFSET(daBemos_c, mLastPlayerPos, 0x544);
WWHD_OFFSET(daBemos_c, mSttsCyl, 0x550);
WWHD_OFFSET(daBemos_c, mCyl, 0x58C);
WWHD_OFFSET(daBemos_c, mSttsSph, 0x6BC);
WWHD_OFFSET(daBemos_c, mSph, 0x6F8);
WWHD_OFFSET(daBemos_c, mGndChk, 0x824);
WWHD_OFFSET(daBemos_c, mGroundY, 0x878);
WWHD_OFFSET(daBemos_c, mInRange, 0x87C);
WWHD_OFFSET(daBemos_c, mSwitchNo, 0x880);
WWHD_OFFSET(daBemos_c, mLaserRootTimer, 0x884);
WWHD_OFFSET(daBemos_c, mChargeCount, 0x886);
WWHD_OFFSET(daBemos_c, mChargeTime, 0x888);
WWHD_OFFSET(daBemos_c, mRotSpeed, 0x88A);
WWHD_OFFSET(daBemos_c, mFireTimer, 0x88E);
WWHD_OFFSET(daBemos_c, mType, 0x890);
WWHD_OFFSET(daBemos_c, mSubType, 0x891);
WWHD_OFFSET(daBemos_c, mSwitchOn, 0x892);
WWHD_OFFSET(daBemos_c, mEventStep, 0x893);
WWHD_OFFSET(daBemos_c, mArmed, 0x894);
WWHD_OFFSET(daBemos_c, mHasPath, 0x895);
WWHD_OFFSET(daBemos_c, m6BE, 0x896);
WWHD_OFFSET(daBemos_c, mDropItemNo, 0x897);
WWHD_OFFSET(daBemos_c, mDropItemBitNo, 0x898);
WWHD_SIZE(daBemos_c, 0x89C);
// HD: preserve integer word copies of floating-point fields without quieting NaNs.
template <class T> inline be<u32>& bemos_bits(be<T>& field) { return *gabi::at<be<u32>>(gabi::ea(&field)); }

struct daBemos_HIO_c {
    /* 0x00 */ be<s8> mNo;
    u8 _pad1[3];
    /* 0x04 */ be<f32> mBlueSearchWidth;  /* was m08: daBemos_c::blue_range_check 02326274: the half width of the search box in front (the player's side distance must be below it) (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (400) */
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<f32> mBlueHeadOpenStep;  /* was m14: daBemos_c::blue_eye_charge 023268D0: the cLib_chaseF step of mHeadScale.x toward 1 (y a quarter of it) while the player is in range (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (0.3) */
    /* 0x14 */ be<f32> mBlueHeadCloseStep;  /* was m18: daBemos_c::blue_eye_charge 023268D0: the cLib_chaseF step of mHeadScale.x toward 0 (y a quarter of it) once the player is out of range (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (0.3) */
    /* 0x18 */ be<s16> mBlueChargeTime;  /* was m1C: daBemos_c::blue_eye_charge_init 0232666C: mChargeTime (mChargeCount 0) when the charge starts from blue_eye_wait; blue_eye_charge fires once mChargeCount passes it (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (3) */
    /* 0x1A */ be<s16> mBlueChargeResume;  /* was m1E: daBemos_c::blue_eye_charge_init 0232666C: mChargeCount and mChargeTime when the charge starts from any other state (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (7) */
    /* 0x1C */ be<f32> mRedHeadOpenStep;  /* was m20: daBemos_c::red_eye_charge 023273F0: the cLib_chaseF step of mHeadScale.x toward 1 (y a quarter of it) while the player is in range (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (0.1) */
    /* 0x20 */ be<f32> mRedHeadCloseStep;  /* was m24: daBemos_c::red_eye_charge 023273F0: the cLib_chaseF step of mHeadScale.x toward 0 (y a quarter of it) once the player is out of range (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (0.07) */
    /* 0x24 */ be<s16> mRedChargeTime;  /* was m28: daBemos_c::red_eye_charge_init 0232712C: mChargeTime (mChargeCount 0) when the charge starts from red_eye_wait; red_eye_charge fires once mChargeCount passes it (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (0) */
    /* 0x26 */ be<s16> mRedChargeResume;  /* was m2A: daBemos_c::red_eye_charge_init 0232712C: mChargeCount and mChargeTime when the charge starts from any other state (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (0) */
    /* 0x28 */ be<f32> mRedRangeInRangeAdd;  /* was m2C: daBemos_c::red_range_check 02326460: added to the search range while the player is already in range (mInRange) (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (0) */
    /* 0x2C */ be<f32> mBlueWidthInRangeAdd;  /* was m30: daBemos_c::blue_range_check 02326274: added to mBlueSearchWidth while the player is already in range (mInRange) (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (0) */
    /* 0x30 */ be<f32> mBlueRangeInRangeAdd;  /* was m34: daBemos_c::blue_range_check 02326274: added to mSearchRange while the player is already in range (mInRange) (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (0) */
    /* 0x34 */ be<f32> m38;
    /* 0x38 */ be<f32> m3C;
    /* 0x3C */ be<s16> mRedTurnStep;  /* was m40: daBemos_c::red_eye_search 02327968: the largest step of the head's yaw toward the player (cLib_addCalcAngleS2) (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (400) */
    /* 0x3E */ be<s16> mRedFireTime;  /* was m42: daBemos_c::red_eye_search_init 023273C0: mFireTimer at the start of the search (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (30) */
    /* 0x40 */ be<f32> mGuardRadius;  /* was m44: daBemos_c::guard_proc 02325DB8: the guard cylinder's radius; the cylinder sits it - 20 to the player's side (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (200) */
    /* 0x44 */ be<f32> mGuardHeight;  /* was m48: daBemos_c::guard_proc 02325DB8: the guard cylinder's height (100 while the player crouches) and half of it is taken off its y (tww the same); daBemos_HIO_c__ct 02328718 sets the default, never written elsewhere in WWHD or tww (100) */
    /* 0x48 */ be<u8> m4C;
    u8 _pad49[3];
    /* 0x4C */ be<u32> __vtbl;
};
WWHD_SIZE(daBemos_HIO_c, 0x50);
WWHD_OFFSET(daBemos_HIO_c, mNo, 0x0);
WWHD_OFFSET(daBemos_HIO_c, mBlueSearchWidth, 0x4);
WWHD_OFFSET(daBemos_HIO_c, m0C, 0x8);
WWHD_OFFSET(daBemos_HIO_c, m10, 0xC);
WWHD_OFFSET(daBemos_HIO_c, mBlueHeadOpenStep, 0x10);
WWHD_OFFSET(daBemos_HIO_c, mBlueHeadCloseStep, 0x14);
WWHD_OFFSET(daBemos_HIO_c, mBlueChargeTime, 0x18);
WWHD_OFFSET(daBemos_HIO_c, mBlueChargeResume, 0x1A);
WWHD_OFFSET(daBemos_HIO_c, mRedHeadOpenStep, 0x1C);
WWHD_OFFSET(daBemos_HIO_c, mRedHeadCloseStep, 0x20);
WWHD_OFFSET(daBemos_HIO_c, mRedChargeTime, 0x24);
WWHD_OFFSET(daBemos_HIO_c, mRedChargeResume, 0x26);
WWHD_OFFSET(daBemos_HIO_c, mRedRangeInRangeAdd, 0x28);
WWHD_OFFSET(daBemos_HIO_c, mBlueWidthInRangeAdd, 0x2C);
WWHD_OFFSET(daBemos_HIO_c, mBlueRangeInRangeAdd, 0x30);
WWHD_OFFSET(daBemos_HIO_c, m38, 0x34);
WWHD_OFFSET(daBemos_HIO_c, m3C, 0x38);
WWHD_OFFSET(daBemos_HIO_c, mRedTurnStep, 0x3C);
WWHD_OFFSET(daBemos_HIO_c, mRedFireTime, 0x3E);
WWHD_OFFSET(daBemos_HIO_c, mGuardRadius, 0x40);
WWHD_OFFSET(daBemos_HIO_c, mGuardHeight, 0x44);
WWHD_OFFSET(daBemos_HIO_c, m4C, 0x48);
WWHD_OFFSET(daBemos_HIO_c, __vtbl, 0x4C);
// HD: HIO is a guest singleton; it must stay shared with the original constructor's writes.
inline daBemos_HIO_c& bemos_hio() { return *gabi::at<daBemos_HIO_c>(0x104692C0); }

// HD: particle fields touched inline by GHS, including the coordinate-system selector.
struct BemosEmitterHD {
    u8 _pad0[0x5C];
    be<s32> mMaxFrame;
    u8 _pad60[0x1F0 - 0x60];
    Mtx34 mGlobalRotation;
    cXyz mGlobalScale, mGlobalTranslation, mGlobalPrmScale;
    GXColor mGlobalPrmColor, mGlobalEnvColor;
    u8 _pad24C[8];
    be<u32> mFlags;
    u8 _pad258[10];
    be<u8> mCoordinateSystem;
    u8 _pad263;
};
WWHD_OFFSET(BemosEmitterHD, mMaxFrame, 0x5C);
WWHD_OFFSET(BemosEmitterHD, mGlobalRotation, 0x1F0);
WWHD_OFFSET(BemosEmitterHD, mGlobalScale, 0x220);
WWHD_OFFSET(BemosEmitterHD, mGlobalTranslation, 0x22C);
WWHD_OFFSET(BemosEmitterHD, mGlobalPrmScale, 0x238);
WWHD_OFFSET(BemosEmitterHD, mGlobalPrmColor, 0x244);
WWHD_OFFSET(BemosEmitterHD, mGlobalEnvColor, 0x248);
WWHD_OFFSET(BemosEmitterHD, mFlags, 0x254);
WWHD_OFFSET(BemosEmitterHD, mCoordinateSystem, 0x262);
WWHD_SIZE(BemosEmitterHD, 0x264);

// HD: out-of-line bindings preserve the verified call-site register widths.
u32 BemosHD_dRes_control_c_getRes(void*, void*, s32);
void BemosHD_JUTAssertion_showAssert(const char*, s32, const char*);
u32 BemosHD_mDoExt_J3DModel__create(void*, s32, s32);
u32 BemosHD_mDoExt_brkAnm_init(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32);
u32 BemosHD_dComIfGp_get();
u32 BemosHD_dPa_control_c_set(void*, s32, s32, void*, void*, s32, s32, s32, s32, void*, s32, s32);
void BemosHD_operator_delete(void*);
void BemosHD_cBgS_Chk_destruct_cBgS_Chk(void*, s32);
void BemosHD_dCcD_Sph_destruct(void*, s32);
void BemosHD_dCcD_Stts_destruct(void*, s32);
void BemosHD_dCcD_Cyl_destruct(void*, s32);
void BemosHD_fopAc_ac_c_destruct_fopAc_ac_c(void*, s32);
u32 BemosHD_fopAcIt_Judge(void*, void*);
u32 BemosHD_fopAc_IsActor(void*);
u32 BemosHD_operator_new(s32);
void BemosHD___register_global_object(void*);
void BemosHD_headerFirst_construct(void*);
void BemosHD_headerSecond_construct(void*);
void BemosHD_daBemos_HIO_c_daBemos_HIO_c(void*);
void BemosHD_PSMTXTrans(void*, f32, f32, f32);
void BemosHD_mDoMtx_ZXYrotM(void*, s32, s32, s32);
void BemosHD_PSMTXMultVec(void*, void*, void*);
void BemosHD_mDoMtx_YrotM(void*, s32);
void BemosHD_mDoMtx_stack_c_transM(f32, f32, f32);
void BemosHD_dCcD_Stts_Init(void*, s32, s32, void*);
void BemosHD_dCcD_Sph_Set(void*, void*);
void BemosHD_daBemos_c_blue_eye_wait_init(void*);
f32 BemosHD_cBgS_GroundCross(void*, void*);
u32 BemosHD_fopAcM_createChild(s32, u32, u32, void*, s32, void*, void*, s32, s32);
void BemosHD_daBemos_c_red_eye_wait_init(void*);
void BemosHD_dCcD_Cyl_Set(void*, void*);
void BemosHD_daBemos_c_CreateInit1(void*);
void BemosHD_daBemos_c_CreateInit2(void*);
void BemosHD_daBemos_c_CreateInit3(void*);
void BemosHD_daBemos_c_set_mtx(void*);
void BemosHD_dComIfG_resDelete(void*, const char*);
void BemosHD_memcpy(void*, void*, s32);
void BemosHD_fopAc_ac_c_fopAc_ac_c(void*);
void BemosHD_mDoExt_brkAnm_init(void*);
void BemosHD_cCcD_Stts_construct(void*);
void BemosHD_dCcD_GStts_dCcD_GStts(void*);
void BemosHD_dCcD_GObjInf_dCcD_GObjInf(void*);
void BemosHD_cM3dGCyl_construct(void*);
void BemosHD_dCcD_Sph_construct(void*);
void BemosHD_cBgS_GndChk_cBgS_GndChk(void*);
s32 BemosHD_dComIfG_resLoad(void*, const char*);
u32 BemosHD_fopAcM_entrySolidHeap(void*, void*, u32);
s32 BemosHD_daBemos_c_CreateInit(void*);
u32 BemosHD_dCcD_GObjInf_ChkTgHit(void*);
void BemosHD_daBemos_c_eye_break_init(void*);
void BemosHD_dCcD_GObjInf_ClrTgHit(void*);
void BemosHD_daBemos_c_event_move(void*);
void BemosHD_dCcD_GStts_Move(void*);
void BemosHD_daObj_HitEff_hibana(void*, void*);
void BemosHD_daObj_HitSeStart(void*, s32, void*, s32);
void BemosHD_cM3dGCyl_SetC(void*, void*);
void BemosHD_cCcS_Set(void*, void*);
void BemosHD_daBemos_c_guard_proc(void*);
void BemosHD_fopAcM_setGbaName(void*, s32, s32, s32);
u32 BemosHD_dKy_getEnvlight();
void BemosHD_dScnKy_env_light_c_settingTevStruct(void*, s32, void*, void*);
void BemosHD_dScnKy_env_light_setLightTevColorType(void*, void*, void*);
void BemosHD_mDoExt_modelUpdateDL(void*, s32);
void BemosHD_mDoExt_brkAnm_entry(void*, void*, f32);
u32 BemosHD_dSv_info_c_isSwitch(void*, u32, s32);
u32 BemosHD_dSv_event_c_isEventBit(void*, s32);
void BemosHD_dSv_event_c_onEventBit(void*, s32);
void BemosHD_fopAcM_orderOtherEvent2(void*, const char*, s32, s32);
s32 BemosHD_dEvent_manager_c_getMyStaffId(void*, const char*, s32, s32);
u32 BemosHD_dEvent_manager_c_endCheckOld(void*, const char*);
void BemosHD_dEvent_manager_c_cutEnd(void*, s32);
void BemosHD_mDoMtx_ZXYrotS(void*, s32, s32, s32);
void BemosHD_PSVECAdd(void*, void*, void*);
u32 BemosHD_dPath_GetRoomPath(u32, s32);
s32 BemosHD_cLib_targetAngleY(void*, void*);
s32 BemosHD_cLib_targetAngleX(void*, void*);
void BemosHD_cXyz_subtractInto(void*, void*, void*);
f32 BemosHD_PSVECSquareMag(void*);
f32 BemosHD_sqrtf(f32);
void BemosHD_cM3dGCyl_SetH(void*, f32);
void BemosHD_daBemos_c_yellow_eye_wait_init(void*);
u32 BemosHD_daBemos_c_getBeamActor(void*);
u32 BemosHD_daBemos_c_blue_range_check(void*, u32, void*);
void BemosHD_daBemos_c_blue_eye_charge_init(void*);
void BemosHD_cLib_chaseUC(void*, s32, s32);
void BemosHD_mDoExt_baseAnm_play(void*);
u32 BemosHD_daBemos_c_red_range_check(void*, u32, void*);
void BemosHD_daBemos_c_red_eye_charge_init(void*);
void BemosHD_cLib_addCalcAngleS2(void*, s32, s32, s32);
f32 BemosHD_PSVECDotProduct(void*, void*);
void BemosHD_PSVECScale(void*, void*, f32);
void BemosHD_cM3dGCyl_SetR(void*, f32);
s32 BemosHD_cM_atan2s(f32, f32);
u32 BemosHD_daBemos_c_yellow_range_check(void*, u32, s32);
void BemosHD_daBemos_c_yellow_eye_search_init(void*);
void BemosHD_cM3dGSph_SetC(void*, void*);
void BemosHD_cLib_chaseF(void*, f32, f32);
void BemosHD_daBemos_blue_eye_search_init(void*);
void BemosHD_JPAGetXYZRotateMtx(s32, s32, s32, void*);
void BemosHD_cLib_addCalcAngleS(void*, s32, s32, s32, s32);
void BemosHD_daBemos_red_eye_search_init(void*);
void BemosHD_fopAcM_delete(void*);
s32 BemosHD_dComIfGp_getReverb(s32);
void BemosHD_mDoAud_seStart(s32, void*, s32, s32);
void BemosHD_fopAcM_createItemFromTable(void*, u32, u32, s32, s32, void*, s32, s32);

struct BemosShapeHD {
    cXyz mMin, mMax;
    be<u32> mAabVtable, mShapeVtable;
};
WWHD_SIZE(BemosShapeHD, 0x20);
inline BemosShapeHD& bemos_shape(dCcD_Cyl& cylinder) {
    return *gabi::at<BemosShapeHD>(gabi::ea(cylinder.mShapeAttr));
}
inline dComIfG_play_c& bemos_play(u32 game) { return gabi::at<dComIfG_inf_c>(game)->play; }
inline dSv_info_c* bemos_save() { return gabi::at<dSv_info_c>(gabi::load<u32>(0x101F84DC) + 0x20); }
struct BemosPlayerVtableHD {
    u8 _pad0[0x24];
    be<u32> getGroundY;
};
WWHD_SIZE(BemosPlayerVtableHD, 0x28);
struct BemosSinCosHD {
    be<f32> sin, cos;
};
WWHD_SIZE(BemosSinCosHD, 8);
inline BemosSinCosHD& bemos_sinCos(u16 angle) { return gabi::at<BemosSinCosHD>(0x104A44F8)[angle >> 3]; }

enum dRes_INDEX_HMOS {
    /* BDL  */
    dRes_INDEX_HMOS_BDL_HMOS1_e = 0x5,
    dRes_INDEX_HMOS_BDL_HMOS2_e = 0x6,
    dRes_INDEX_HMOS_BDL_HMOS3_e = 0x7,
    dRes_INDEX_HMOS_BDL_HMOSUP1_e = 0x8,
    dRes_INDEX_HMOS_BDL_HMOSUP2_e = 0x9,
    /* BDLM */
    dRes_INDEX_HMOS_BDL_HMOS2T_e = 0xC,
    /* BRK  */
    dRes_INDEX_HMOS_BRK_HMOS2T_e = 0xF,
    dRes_INDEX_HMOS_BRK_HMOSUP1_e = 0x10,
    dRes_INDEX_HMOS_BRK_HMOSUP2_e = 0x11,
};

extern void BemosCallback_blue_eye_wait(void*);
extern void BemosCallback_red_eye_wait(void*);
extern void BemosCallback_blue_eye_search(void*);
extern void BemosCallback_red_eye_search(void*);
extern void BemosCallback_blue_eye_charge(void*);
extern void BemosCallback_red_eye_charge(void*);
extern void BemosCallback_yellow_eye_wait(void*);
extern void BemosCallback_yellow_eye_search(void*);
extern void BemosCallback_eye_break(void*);
extern void BemosCallback_eye_dummy(void*);
extern void BemosCallback_fpcSch_JudgeByID(void*);
namespace dPa_name {
enum {
    ID_AK_SN_BMOSWAIT00 = 0x815C,
    ID_AK_SN_BMOSEYE00 = 0x815D,
    ID_AK_SN_BMOSBALL00 = 0x815E,
    ID_AK_SN_LASERROOT00 = 0x8121,
    ID_AK_SN_BMOSBREAK00 = 0x815F,
    ID_AK_SN_BMOSBREAK01 = 0x8160,
    ID_AK_JN_OK = 0xD,
};
}

inline const csXyz& bemos_zeroAngle() { return *gabi::at<csXyz>(0x101FFB14); }
inline const cXyz& bemos_zero() { return *gabi::at<cXyz>(0x101FFBA8); }
inline const cXyz& bemos_baseX() { return *gabi::at<cXyz>(0x101FFBB4); }
inline const cXyz& bemos_baseZ() { return *gabi::at<cXyz>(0x101FFBCC); }
inline const be<f32>& bemos_heightReg() { return *gabi::at<be<f32>>(0x1047BBFC); }
inline be<s16>& bemos_anglePart(csXyz& angle, u32 i) { return gabi::at<be<s16>>(gabi::ea(&angle))[i]; }
inline const be<s16>& bemos_anglePart(const csXyz& angle, u32 i) {
    return gabi::at<be<s16>>(gabi::ea(&angle))[i];
}
inline be<u32>& bemos_positionWord(cXyz& pos, u32 i) { return gabi::at<be<u32>>(gabi::ea(&pos))[i]; }
inline const be<u32>& bemos_positionWord(const cXyz& pos, u32 i) {
    return gabi::at<be<u32>>(gabi::ea(&pos))[i];
}
inline gptr<JPABaseEmitter>& bemos_emitter(daBemos_c* actor, u32 i) {
    return gabi::at<gptr<JPABaseEmitter>>(gabi::ea(&actor->mpWaitEmitter))[i];
}
enum { fpcNm_Beam_e = 0xE8, fpcNm_Bemos_e = 0xE9 };

// HD: the header initialization sequence and destructor records are TU-specific.
struct BemosHeaderStaticsHD {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mFirst, mSecond;
    u8 mPadding[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(BemosHeaderStaticsHD, mFirst, 8);
WWHD_OFFSET(BemosHeaderStaticsHD, mSecond, 9);
WWHD_OFFSET(BemosHeaderStaticsHD, mZero, 0xC);
WWHD_SIZE(BemosHeaderStaticsHD, 0x1C);
struct BemosDtorRecordHD {
    be<u32> mWords[3];
};
WWHD_SIZE(BemosDtorRecordHD, 0xC);
inline BemosHeaderStaticsHD& bemos_headerStatics() { return *gabi::at<BemosHeaderStaticsHD>(0x104692A4); }
inline BemosDtorRecordHD* bemos_dtorRecord(u32 i) { return &gabi::at<BemosDtorRecordHD>(0x101C80EC)[i]; }
inline s8 bemos_stayRoom() { return gabi::load<s8>(0x101D5F46); }
// Source descriptors have the unchanged GameCube layout; HD calls receive their original storage.
struct BemosSrcSph {
    /* 0x00 */ be<u32> mObjFlags, mAtType;
    /* 0x08 */ be<u8> mAtp;
    u8 _padding[3];
    /* 0x0C */ be<u32> mAtSPrm, mTgType, mTgSPrm, mCoSPrm;
    /* 0x1C */ be<u8> mAtSe, mAtHitMark, mAtSpl, mAtMtrl;
    /* 0x20 */ be<u32> mGAtSPrm;
    /* 0x24 */ be<u8> mTgSe, mTgHitMark, mTgSpl, mTgMtrl;
    /* 0x28 */ be<u32> mGTgSPrm, mGCoSPrm;
    /* 0x30 */ be<f32> mCenter[3];
    /* 0x3C */ be<f32> mRadius;
};
WWHD_OFFSET(BemosSrcSph, mObjFlags, 0x0);
WWHD_OFFSET(BemosSrcSph, mAtp, 0x8);
WWHD_OFFSET(BemosSrcSph, mAtSPrm, 0xC);
WWHD_OFFSET(BemosSrcSph, mAtSe, 0x1C);
WWHD_OFFSET(BemosSrcSph, mGAtSPrm, 0x20);
WWHD_OFFSET(BemosSrcSph, mTgSe, 0x24);
WWHD_OFFSET(BemosSrcSph, mGTgSPrm, 0x28);
WWHD_OFFSET(BemosSrcSph, mCenter, 0x30);
WWHD_OFFSET(BemosSrcSph, mRadius, 0x3C);
WWHD_SIZE(BemosSrcSph, 0x40);
struct BemosSrcCyl {
    /* 0x00 */ be<u32> mObjFlags, mAtType;
    /* 0x08 */ be<u8> mAtp;
    u8 _padding[3];
    /* 0x0C */ be<u32> mAtSPrm, mTgType, mTgSPrm, mCoSPrm;
    /* 0x1C */ be<u8> mAtSe, mAtHitMark, mAtSpl, mAtMtrl;
    /* 0x20 */ be<u32> mGAtSPrm;
    /* 0x24 */ be<u8> mTgSe, mTgHitMark, mTgSpl, mTgMtrl;
    /* 0x28 */ be<u32> mGTgSPrm, mGCoSPrm;
    /* 0x30 */ be<f32> mCenter[3];
    /* 0x3C */ be<f32> mRadius;
    /* 0x40 */ be<f32> mHeight;
};
WWHD_OFFSET(BemosSrcCyl, mObjFlags, 0x0);
WWHD_OFFSET(BemosSrcCyl, mAtp, 0x8);
WWHD_OFFSET(BemosSrcCyl, mAtSPrm, 0xC);
WWHD_OFFSET(BemosSrcCyl, mAtSe, 0x1C);
WWHD_OFFSET(BemosSrcCyl, mGAtSPrm, 0x20);
WWHD_OFFSET(BemosSrcCyl, mTgSe, 0x24);
WWHD_OFFSET(BemosSrcCyl, mGTgSPrm, 0x28);
WWHD_OFFSET(BemosSrcCyl, mCenter, 0x30);
WWHD_OFFSET(BemosSrcCyl, mRadius, 0x3C);
WWHD_SIZE(BemosSrcCyl, 0x44);
// HD: vtables emitted in this translation unit.
namespace BemosVtableHD {
enum : u32 {
    mGroundPolyDtor = 0x1002629C,
    mPolyPassDtor = 0x100262BC,
    mGroupPassDtor = 0x1002627C,
    mHio = 0x1002631C,
    mActor = 0x1002630C,
    mCcStts = 0x1004AE88,
    mGCcStts = 0x1004AEC0,
    mShapeAttr = 0x100015A8,
    mAab = 0x1002626C,
    mGObjInf = 0x1004B108,
    mCcCyl = 0x1004B160,
    mCyl = 0x1004B150,
    mGroundPoly = 0x100262DC,
    mGroundChk = 0x100262CC,
    mGroupPass = 0x100262EC,
    mPolyPass = 0x100262FC,
};
}
