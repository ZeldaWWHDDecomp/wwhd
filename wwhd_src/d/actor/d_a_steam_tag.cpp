/** Steam vent tag; local only. */
#include "d/actor/d_a_steam_tag.h"
static u8 mEmitterNum;
static SteamStageName l_Adanmae;
static SteamSafeStringVtable l_safeStringVtable;
enum { dItemNo_PEARL_DIN_e = 0x6A };
static const dCcD_SrcCps l_cps_src;
enum { JA_SE_OBJ_STEAM = 0x612E, dPa_name_ID_AK_SN_STEAM00 = 0x808B, dPa_name_ID_AK_SN_STEAM01 = 0x808C };
// HD: getData constants are inlined immediates.
/* 0249A5F8 */
BOOL daSteamTag_c::CreateInit() {
    m29B = (fopAcM_GetParam(this) >> 2) & 0xFF; /* daSteamTag_prm::getSchBit */
    mEmitTimer = (s16)gabi::ftoi(50.0f + cM_rndF(150.0f));
    s16 ct = (s16)gabi::ftoi(cM_rndF(300.0f));
    mpEmitter = nullptr;
    mCreateTimer = ct;
    mEmitTimer = 0;
    mGStts.Init(0xFF, 0xFF, this);
    SteamCps_Set(&mCps, &l_cps_src);
    mCps.SetStts(&mGStts);
    mPntWindCpsS.mStart.copy(current.pos);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    f32 windY = 200.0f;
    if (current.angle.x != 0) {
        windY = 50.0f;
    }
    mDoMtx_stack_c::transM(0.0f, windY, 0.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), cXyz_Zero, &mPntWindCpsS.mEnd);
    mPntWindCpsS.mRadius = 50.0f;
    SteamGeometryCps_Set(SteamCapsule(&mCps), &mPntWindCpsS);
    /* mCps.CalcAtVec(): at vector = end - start */
    PSVECSubtract(&SteamCapsule(&mCps)->mEnd, &SteamCapsule(&mCps)->mStart, &mCps.mGObjAt.mVec);
    SteamPointWind_init(&mPointWind, &mPntWindCpsS);
    fopAcM_offDraw(this);
    return TRUE;
}

// HD: group 2 particle-control call replaces setToon.
/* 0249A278 */
BOOL daSteamTag_c::createEmitter() {
    if (mEmitterNum < 8) {
        u16 particleID;
        if (((s16)gabi::ftoi(cM_rndF(100.0f)) % 2) != 0) {
            particleID = dPa_name_ID_AK_SN_STEAM00;
        } else {
            particleID = dPa_name_ID_AK_SN_STEAM01;
        }
        mpEmitter = reinterpret_cast<SteamEmitter*>(dPa_control_set(dComIfGp_getParticle(), 2, particleID, &current.pos, &current.angle, &scale, 0xB4,
                                    nullptr, -1, nullptr, nullptr, nullptr));
        if (mpEmitter) {
            mEmitterNum = u8(mEmitterNum + 1);
            return TRUE;
        }
    }
    return FALSE;
}

/* 0249A378 */
bool daSteamTag_c::endEmitter() {
    mEmitterNum = u8(mEmitterNum - 1);
    return TRUE;
}

/* 0249AA4C */
static BOOL daSteamTag_Draw() {
    return TRUE;
}

/* inlined into daSteamTag_Execute */
BOOL daSteamTag_c::execute() {
    f32 windStrength;
    if ((mEmitTimer == 0) && (mCreateTimer > 0)) {
        mCreateTimer = mCreateTimer - 1;
        if (mCreateTimer == 0) {
            if (createEmitter()) {
                /* setGlobalTranslation / setGlobalRotation / setGlobalAlpha / playCreateParticle */
                SteamEmitter* e = mpEmitter;
                s16 ax = current.angle.x, ay = current.angle.y, az = current.angle.z;
                f32 px = current.pos.x, py = current.pos.y, pz = current.pos.z;
                e->mGlobalTranslation.x = px;
                e->mGlobalTranslation.y = py;
                e->mGlobalTranslation.z = pz;
                if (e->mRotationOrder >= 7) /* HD */
                    e->mGlobalTranslation.y = -(f32)e->mGlobalTranslation.y;
                SteamGetXYZRotateMtx(ax, ay, az, &mpEmitter->mGlobalRotation);
                mpEmitter->mGlobalAlpha = 0xB4;
                mpEmitter->playCreateParticle();
                mEmitTimer = (s16)gabi::ftoi(50.0f + cM_rndF(150.0f));
            } else {
                mCreateTimer = 30;
            }
        }
    } else if ((mCreateTimer == 0) && (mEmitTimer > 0)) {
        mEmitTimer = mEmitTimer - 1;
        if (mEmitTimer == 0) {
            if (mpEmitter != nullptr) {
                windStrength = 0.0f;
                mpEmitter->stopCreateParticle();
                SteamEmitter* e = mpEmitter;
                if (e->getParticleNumber() == 0) {
                    e->becomeInvalidEmitter();
                    endEmitter();
                    mpEmitter = nullptr;
                    mCreateTimer = (s16)gabi::ftoi(100.0f + cM_rndF(300.0f));
                } else {
                    mEmitTimer = 1;
                    windStrength = 1.0f;
                }
                mPointWind.mWind.mStrength = windStrength; /* mPointWind.set_pwind_power */
            }
        } else {
            mDoAud_seStart(JA_SE_OBJ_STEAM, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
            if (mpEmitter != nullptr) {
                mpEmitter->mGlobalAlpha = 0xB4;
            }
            SteamGeometryCps_Set(SteamCapsule(&mCps), &mPntWindCpsS);
            dComIfG_Ccsp_Set(&mCps);
            SteamPointWind_move(&mPointWind);
        }
    }
    return TRUE;
}

/* 0249A390 */
static BOOL daSteamTag_Execute(daSteamTag_c* i_this) {
    return i_this->execute();
}

/* 0249A5E8 */
static BOOL daSteamTag_IsDelete() {
    return TRUE;
}

// HD: framework calls the virtual destructor separately.
/* 0249A5F0 */
static BOOL daSteamTag_Delete() {
    return TRUE;
}

// HD: deleting destructor also destroys collision members.
/* 0249AA68 */
static void daSteamTag_c_dt(daSteamTag_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        i_this->__vtbl = 0x1003D774;
        if (i_this->mpEmitter != nullptr) {
            i_this->mpEmitter->becomeInvalidEmitter();
            i_this->mpEmitter = nullptr;
        }
        SteamPointWind_delete(&i_this->mPointWind);
        SteamCps_dt(&i_this->mCps, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mGStts, 2);
        SteamActor_dt(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* inlined into daSteamTag_Create */
cPhs_State daSteamTag_c::create() {
    /* fopAcM_ct(this, daSteamTag_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x1003D774;
            dCcD_Stts_ct(&mGStts);
            dCcD_Cyl_ct(&mCyl, 0x1003D764);
            SteamCps_ct(&mCps);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    CreateInit();

    cPhs_State phase_state;
    /* strcmp(dComIfGp_getStartStageName(), "Adanmae") == 0 (HD: sead::SafeString ==) */
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::Local<SteamSafeString> a;
    a->mStringTop = l_Adanmae.name; /* "Adanmae" */
    a->__vtbl = &l_safeStringVtable;
    gabi::Local<SteamSafeString> b;
    b->mStringTop = reinterpret_cast<SteamPlayView*>(dComIfGp_get())->mStartStageName;
    b->__vtbl = &l_safeStringVtable;
    if (SteamSafeString_eq(a.get(), b.get()) && (current.roomNo == 0) && checkItemGet(dItemNo_PEARL_DIN_e, TRUE)) {
        phase_state = cPhs_ERROR_e;
    } else {
        phase_state = cPhs_COMPLEATE_e;
    }
    return phase_state;
}

/* 0249A78C */
static cPhs_State daSteamTag_Create(fopAc_ac_c* i_this) {
    return ((daSteamTag_c*)i_this)->create();
}

/* 0249A9B8 */
static void __sinit_d_a_steam_tag_cpp() {
    sinit_header_statics(0x1046E00C, 0x101D106C);
}

// HD: trivial SafeString deleting destructor.
/* 0249AA54 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: empty SafeString termination function.
/* 0249AB1C */
static void SafeString_assureTermination() {
}

/* WWHD glue (tools/verify/autoglue.py)
data l_Adanmae 1003D740
data l_safeStringVtable 1003D74C
data mEmitterNum 10475650
data l_cps_src 101D1020
ext 028E8DAC void PSVECSubtract(const cXyz*, const cXyz*, cXyz*)
ext 025164C0 void SteamCps_Set(dCcD_Cps*, const dCcD_SrcCps*)
ext 020181FC void SteamGeometryCps_Set(SteamCapsuleGeometry*, SteamCapsuleShape*)
ext 02515FB8 void SteamCps_base_ct(dCcD_Cps*)
ext 02018150 void SteamGeometryCps_ct(SteamCapsuleGeometry*)
ext 02515980 void SteamCps_dt(dCcD_Cps*, s32)
ext 025AB3F8 void SteamPointWind_init(SteamPointWindPrefix*, SteamCapsuleShape*)
ext 025AB454 void SteamPointWind_move(SteamPointWindPrefix*)
ext 025AB710 void SteamPointWind_delete(SteamPointWindPrefix*)
ext 028245AC void SteamGetXYZRotateMtx(s16, s16, s16, Mtx34*)
ext 025D50BC void SteamActor_dt(fopAc_ac_c*, u32)
*/
