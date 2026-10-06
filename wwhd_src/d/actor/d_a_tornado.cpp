/**
 * d_a_tornado.cpp (WWHD)
 * Object - Ballad of Gales tornado
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tornado.cpp) to the WWHD layout and verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * HD: the joint callback writes the joint matrices to an actor-owned block (mJointMtx) and copies
 * all eleven into the model's matrix block when the last joint is reached.
 */
#include "bindings.h"

#define l_arcName STR(0x101D2A08)  /* "Trnd" (.data) */
#define SAFESTRING_VTBL 0x10040924 /* this TU's sead::SafeString vtable */
#define TORNADO_VTBL 0x10040964    /* daTornado_c vtable (HD virtual destructor) */
#define BCK_TU_VTBL 0x1004093C     /* this TU's vtable for the mDoExt_bckAnm sub-object (+0x10) */
#define l_joint_scale(i) gabi::load<f32>(0x10040974 + 4 * (i))
#define joint_offset(i) gabi::load<f32>(0x100409A0 + 4 * (i))
#define FILE_NAME STR(0x10040A10)

enum {
    dRes_INDEX_TRND_BCK_YTRND00_e = 6,
    dRes_INDEX_TRND_BCK_YWUWT00_e = 7,
    dRes_INDEX_TRND_BDL_YTRND00_e = 0xA,
    dRes_INDEX_TRND_BDL_YWUWT00_e = 0xB,
    dRes_INDEX_TRND_BRK_YTRND00_e = 0xE,
    dRes_INDEX_TRND_BRK_YWUWT00_e = 0xF,
    dRes_INDEX_TRND_BTK_YTRND00_e = 0x12,
    dRes_INDEX_TRND_BTK_YWUWT00_e = 0x13,
};
enum { TEV_TYPE_BG1 = 2 };
enum { JA_SE_OBJ_TORNADE_SUS = 0x106B };
enum { dPa_name_ID_AK_SN_TORNADOWIND00 = 0x8213, dPa_name_ID_AK_SN_WINDUPWATER00 = 0x81BB };
enum { fopAcStts_SHOWMAP_e = 0x20 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* mDoExt_bckAnm inline constructor (HD 0x8C, as in d_a_arrow_iceeff.cpp) */
static inline void mDoExt_bckAnm_ct(mDoExt_bckAnm* b, u32 tu_vtbl) {
    u32 p = gabi::ea(b);
    gabi::call(0x027F2BC0, b, 0);
    gabi::store<u32>(p + 0x10, 0x1016E54C);
    gabi::call(0x027DA984, gabi::at<void>(p + 0x14));
    gabi::store<u32>(p + 0x84, 0);
    gabi::store<u32>(p + 0x80, 0);
    gabi::store<u32>(p + 0x58, 0);
    gabi::store<u32>(p + 0x10, tu_vtbl);
    gabi::store<u32>(p + 0x48, 0x1016D820);
    gabi::store<u32>(p + 0x88, 0);
    gabi::store<u32>(p + 0x7C, 0);
}
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0, init 025E8154 (as in d_a_bita.cpp) */
static inline void mDoExt_brkAnm_ct(void* p) { gabi::call(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* k, bool play, s32 mode, f32 speed, s16 start, s16 end,
                                      bool b, s32 i) {
    return gabi::call<BOOL>(0x025E8154, a, d, k, play, mode, speed, start, end, b, i);
}
/* J3DAnmBase::getFrameMax(): HD virtual (vtable at +4, slot 0x14), returns an int */
static inline s32 anm_getFrameMax(u32 anm) { return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0x14), gabi::at<void>(anm)); }
/* 027F58E0: HD J3DModel helper called after creating the model: stores the argument at +0xA0 of each
 * of the model's 0xAC-byte entries (count +0x128, array +0x12C) */
static inline void J3DModel_027F58E0(J3DModel* m, u32 v) { gabi::call(0x027F58E0, m, v); }
/* 027F3F94 (matcher: __nw): J3DModelData accessor; the result's u16 at +8 is the joint count */
static inline u32 J3DModelData_jointTree(J3DModelData* d) { return gabi::call<u32>(0x027F3F94, d); }
/* 0201B31C cXyz::normalize(this, out): normalizes in place and copies the result to out */
static inline void cXyz_normalize(cXyz* a, cXyz* res) { gabi::call(0x0201B31C, a, res); }
/* 0257E77C dKyw_tornado_Notice(cXyz*) */
static inline void dKyw_tornado_Notice(cXyz* pos) { gabi::call(0x0257E77C, pos); }
/* 025602F0 dKy_get_seacolor(GXColor* amb, GXColor* dif) */
static inline void dKy_get_seacolor(GXColor* amb, GXColor* dif) { gabi::call(0x025602F0, amb, dif); }
/* 028E9108 PSMTXConcat, 028E945C PSMTXScale */
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline void PSMTXScale(Mtx34* m, f32 x, f32 y, f32 z) { gabi::call(0x028E945C, m, x, y, z); }
/* save info: event flags at +0x644, temporary flags at +0x1178 */
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178), f); }
/* play object: ship actor +0x5B3C, link +0x5B34, event running flag +0x5292, player status 0 +0x5CD8 */
static inline u32 dComIfGp_getShipActor() { return gabi::load<u32>(dComIfGp_ea() + 0x5B3C); }
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
static inline bool dComIfGp_checkPlayerStatus0_shipRide() { return (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) != 0; }
/* daShip_c: mTornadoID +0x700, mTornadoActor +0x704 */
static inline void ship_offTornadoFlg(u32 ship) {
    gabi::store<s32>(ship + 0x700, -1);
    gabi::store<u32>(ship + 0x704, 0);
}
/* dComIfGd_setListMaskOff: the j3dSys draw buffers from play+0x5D84/0x5D88 */
static inline void dComIfGd_setListMaskOff() {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D84));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D88));
}
#define J3DSys_mCurrentMtx gabi::at<Mtx34>(0x104B4868)

struct daTornado_c : fopAc_ac_c {
    BOOL jointCallBack(int);
    BOOL draw();
    BOOL execute();
    BOOL tornado_delete();
    BOOL createHeap();
    cPhs_State create();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_bckAnm mBck;
    /* 0x444 */ mDoExt_btkAnm mBtk;      /* anm at +0x68 */
    /* 0x4B8 */ u8 mBrk[0x78];           /* mDoExt_brkAnm; anm at +0x10 */
    /* 0x530 */ gptr<J3DModel> mpModelUnder;
    /* 0x534 */ mDoExt_bckAnm mBckUnder;
    /* 0x5C0 */ mDoExt_btkAnm mBtkUnder;
    /* 0x634 */ u8 mBrkUnder[0x78];
    /* 0x6AC */ be<s16> mAngle1;
    /* 0x6AE */ be<s16> mAngle2;
    /* 0x6B0 */ be<s16> m31c;
    /* 0x6B2 */ be<s16> mPtclTimer;
    /* 0x6B4 */ be<f32> mBtkFrame;
    /* 0x6B8 */ be<f32> mBtkUnderFrame;
    /* 0x6BC */ be<f32> mBrkFrame;
    /* 0x6C0 */ be<f32> m32c;
    /* 0x6C4 */ be<f32> mJointX[11];
    /* 0x6F0 */ be<f32> mJointZ[11];
    /* 0x71C */ be<f32> mJointScale[11];
    /* 0x748 */ cXyz mCenter;
    /* 0x754 */ u8 mPtclCb[0x14];        /* dPa_followEcallBack; emitter at +4 */
    /* 0x768 */ Mtx34 mJointMtx[11];     /* HD */

    u32 btkAnm() { return gabi::load<u32>(gabi::ea(this) + 0x444 + 0x68); }
    u32 btkUnderAnm() { return gabi::load<u32>(gabi::ea(this) + 0x5C0 + 0x68); }
    u32 brkAnm() { return gabi::load<u32>(gabi::ea(this) + 0x4B8 + 0x10); }
};
WWHD_OFFSET(daTornado_c, mBtk, 0x444);
WWHD_OFFSET(daTornado_c, mpModelUnder, 0x530);
WWHD_OFFSET(daTornado_c, mAngle1, 0x6AC);
WWHD_OFFSET(daTornado_c, mJointScale, 0x71C);
WWHD_OFFSET(daTornado_c, mCenter, 0x748);
WWHD_OFFSET(daTornado_c, mJointMtx, 0x768);
WWHD_SIZE(daTornado_c, 0x978);

/* 024C8640 */
BOOL daTornado_c::jointCallBack(int jntNo) {
    WWHD_FUNC(0x024C8640, BOOL, this, jntNo);
    int jntIdx = jntNo - 1;
    if ((u32)jntIdx >= 11) /* (jntIdx < ROOT) || (jntIdx >= JOINT11) */
        return TRUE;

    PSMTXTrans(mDoMtx_stack_c::get(), mJointX[jntIdx], 0.0f, mJointZ[jntIdx]);
    if (jntIdx != 10 && jntIdx != 0) {
        s16 rx = (s16)gabi::ftoi(-(3572.0f * mJointZ[jntIdx] * 0.001f));
        s16 rz = (s16)gabi::ftoi(3572.0f * mJointX[jntIdx] * 0.001f);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), rx, 0, rz);
    }
    if (fopAcM_GetParam(this) != 0) {
        f32 s = mJointScale[jntIdx];
        mDoMtx_stack_c::scaleM(s, s, s);
    }
    /* mDoMtx_stack_c::revConcat(mpModel->getAnmMtx(jntNo)) (HD: getAnmMtx marks the block dirty) */
    u32 blk = gabi::load<u32>(gabi::ea(mpModel.get()) + 0x2C);
    u32 anm = jntNo * 0x30 + gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    PSMTXConcat(gabi::at<Mtx34>(anm), mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
    /* HD: setAnmMtx into the actor's own block */
    PSMTXCopy(mDoMtx_stack_c::get(), &mJointMtx[jntIdx]);
    PSMTXTrans(mDoMtx_stack_c::get(), mJointX[jntIdx], 0.0f, mJointZ[jntIdx]);
    PSMTXConcat(J3DSys_mCurrentMtx, mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx);

    if (jntNo == 11) {
        /* HD: copy the eleven joint matrices into the model (joints 1..11) */
        for (int i = 0; i < 11; i++) {
            u32 b = gabi::load<u32>(gabi::ea(mpModel.get()) + 0x2C);
            gabi::store<u16>(b + 4, gabi::load<u16>(b + 4) | 0x10);
            mtx_copy(gabi::at<Mtx34>(gabi::load<u32>(b + 0x10) + 0x30 + i * 0x30), &mJointMtx[i]);
        }
    }
    return TRUE;
}
VERIFY(0x024C8640, &daTornado_c::jointCallBack);

/* 024C8854 */
static BOOL daTornado_jointCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x024C8854, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 model = gabi::load<u32>(0x104B462C); /* j3dSys.getModel() */
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        daTornado_c* i_this = gabi::at<daTornado_c>(gabi::load<u32>(model + 0xB8)); /* getUserArea */
        i_this->jointCallBack(jntNo);
    }
    return TRUE;
}
VERIFY(0x024C8854, daTornado_jointCallBack);

/* 024C8894 */
BOOL daTornado_c::draw() {
    WWHD_FUNC(0x024C8894, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mBck.entry(J3DModel_getModelData(mpModel), mBck.getFrame());
    mBtk.entry(J3DModel_getModelData(mpModel), mBtkFrame);
    mDoExt_brkAnm_entry((mDoExt_brkAnm*)mBrk, J3DModel_getModelData(mpModel), mBrkFrame);
    if (dComIfGs_isTmpBit(0x404)) {
        dComIfGd_setListMaskOff();
        mDoExt_modelUpdateDL(mpModel);
        dComIfGd_setList();
    } else {
        mDoExt_modelUpdateDL(mpModel);
    }
    if (fopAcM_GetParam(this) == 0) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG1, &current.pos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), mpModelUnder, &tevStr);
        mBckUnder.entry(J3DModel_getModelData(mpModelUnder), mBck.getFrame());
        mBtkUnder.entry(J3DModel_getModelData(mpModelUnder), mBtkUnderFrame);
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)mBrkUnder, J3DModel_getModelData(mpModelUnder), mBrkFrame);
        dComIfGd_setListMaskOff();
        mDoExt_modelUpdateDL(mpModelUnder);
        dComIfGd_setList();
    }
    return TRUE;
}
VERIFY(0x024C8894, &daTornado_c::draw);

/* 024C8A54 */
static BOOL daTornado_Draw(daTornado_c* i_this) {
    WWHD_FUNC(0x024C8A54, BOOL, i_this);
    return i_this->draw();
}
VERIFY(0x024C8A54, daTornado_Draw);

/* 024C8A58 */
BOOL daTornado_c::execute() {
    WWHD_FUNC(0x024C8A58, BOOL, this);
    /* static cXyz wind_scale(20.0f, 20.0f, 20.0f) (HD: guard 0x1046EA1C) */
    const u32 wind_scale = 0x1046E9EC;
    if (gabi::load<u32>(0x1046EA1C) == 0) {
        gabi::store<f32>(wind_scale + 4, 20.0f);
        gabi::store<f32>(wind_scale + 8, 20.0f);
        gabi::store<u32>(0x1046EA1C, 1);
        gabi::store<f32>(wind_scale + 0, 20.0f);
    }

    mBck.play();
    f32 f = mBtkFrame + 1.0f;
    mBtkFrame = f;
    if (!(f < (f32)anm_getFrameMax(btkAnm()))) {
        f32 max = (f32)anm_getFrameMax(btkAnm());
        mBtkFrame = mBtkFrame - max;
    }
    f = mBtkUnderFrame + 1.0f;
    mBtkUnderFrame = f;
    if (!(f < (f32)anm_getFrameMax(btkUnderAnm()))) {
        f32 max = (f32)anm_getFrameMax(btkUnderAnm());
        mBtkUnderFrame = mBtkUnderFrame - max;
    }

    mAngle2 = (s16)(mAngle2 + 250);
    m32c = 10000.0f;
    mAngle1 = (s16)(mAngle1 + 500);
    u32 param = fopAcM_GetParam(this);
    f32 fVar8;
    if (param == 1) {
        if (dComIfGp_getShipActor() != 0) {
            u32 ship = dComIfGp_getShipActor();
            fVar8 = (gabi::load<f32>(ship + 0x318) - home.pos.y) / 500.0f;
            if (fVar8 < 0.0f) {
                fVar8 = 0.0f;
            } else if (fVar8 > 1.0f) {
                fVar8 = 1.0f;
            }
        } else {
            fVar8 = 1.0f;
        }
    } else if (param == 0 && dComIfGp_getShipActor() != 0 && gabi::load<u32>(dComIfGp_getShipActor() + 0x704) != 0) {
        fVar8 = 0.25f;
    } else {
        fVar8 = 1.0f;
    }

    for (int i = 0; i < 11; i++) {
        f32 fVar1 = (joint_offset(i) * fVar8) * (cM_ssin(mAngle1 - 0x1000 * i) + 1.0f);
        s32 angle2 = mAngle2 - 0x1800 * i;
        mJointX[i] = fVar1 * cM_ssin(angle2) * scale.x;
        mJointZ[i] = fVar1 * cM_scos(angle2) * scale.x;
    }

    param = fopAcM_GetParam(this);
    if (param == 0) {
        if (!dComIfGp_event_runCheck()) {
            s16 target = fopAcM_searchActorAngleY(this, dComIfGp_getLinkPlayer());
            cLib_addCalcAngleS(&current.angle.y, target, 10, 0x1000, 0x100);
            cLib_chaseF(&speedF, 20.0f, 0.2f);
        }
        if (dComIfGp_checkPlayerStatus0_shipRide() && dComIfGp_getShipActor() != 0) {
            u32 ship = dComIfGp_getShipActor();
            gabi::Local<cXyz> diff;
            cXyz_mi(gabi::at<cXyz>(ship + 0x314), diff, &current.pos);
            gabi::Local<cXyz> xz;
            xz->x = diff->x;
            xz->y = 0.0f;
            xz->z = diff->z;
            if (PSVECSquareMag(xz) < 10000.0f * 10000.0f) {
                gabi::store<u32>(ship + 0x700, gabi::load<u32>(gabi::ea(this) + 4)); /* onTornadoFlg(fopAcM_GetID(this)) */
                speedF = 0.0f;
            }
        }
        if (!dComIfGp_event_runCheck()) {
            fopAcM_posMoveF(this, nullptr);
            gabi::Local<cXyz> diff;
            cXyz_mi(&current.pos, diff, &home.pos);
            f32 dx = diff->x;
            diff->y = 0.0f;
            gabi::Local<cXyz> xz;
            xz->x = dx;
            xz->y = 0.0f;
            xz->z = diff->z;
            if (PSVECSquareMag(xz) > 7500.0f * 7500.0f) {
                gabi::Local<cXyz> tmp;
                cXyz_normalize(diff, tmp);
                gabi::Local<cXyz> ofs;
                cXyz_ml(diff, ofs, 7500.0f);
                cXyz_pl(&home.pos, tmp, ofs);
                gabi::store<u32>(gabi::ea(&current.pos) + 0, gabi::load<u32>(gabi::ea(tmp.get()) + 0));
                gabi::store<u32>(gabi::ea(&current.pos) + 4, gabi::load<u32>(gabi::ea(tmp.get()) + 4));
                gabi::store<u32>(gabi::ea(&current.pos) + 8, gabi::load<u32>(gabi::ea(tmp.get()) + 8));
            }
        }
        if (mPtclTimer != 0) {
            mPtclTimer = (s16)(mPtclTimer - 1);
        } else {
            mPtclTimer = 10;
            dComIfGp_particle_set(dPa_name_ID_AK_SN_TORNADOWIND00, &current.pos, nullptr, gabi::at<cXyz>(wind_scale));
        }
        /* fopAcM_seStartCurrent (HD: no NULL checks here) */
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(JA_SE_OBJ_TORNADE_SUS, &current.pos, 100, reverb);
        if (dComIfGs_isEventBit(0x2710)) {
            dPa_followEcallBack_end((dPa_followEcallBack*)mPtclCb); /* mPtclCb.remove() */
            u32 ship = dComIfGp_getShipActor();
            if (ship != 0)
                ship_offTornadoFlg(ship);
            f32 b = mBrkFrame + 0.3f;
            mBrkFrame = b;
            if (!(b < (f32)anm_getFrameMax(brkAnm())))
                fopAcM_delete(this);
        } else {
            dKyw_tornado_Notice(&current.pos);
        }
    } else if (m31c != 0) {
        if (param == 1) {
            for (int i = 0; i < 11; i++) {
                cLib_chaseF(&mJointScale[i], l_joint_scale(i), 0.007f * (f32)(11 - i));
                mJointX[i] = mJointX[i] * mJointScale[i];
                mJointZ[i] = mJointZ[i] * mJointScale[i];
            }
        } else {
            for (int i = 0; i < 11; i++) {
                cLib_chaseF(&mJointScale[i], 0.0f, 0.001f * (f32)(i + 1));
                mJointX[i] = mJointX[i] * mJointScale[i];
                mJointZ[i] = mJointZ[i] * mJointScale[i];
            }
            if (mJointScale[10] < 1e-06f) {
                fopAcM_delete(this);
                return TRUE;
            }
        }
    }

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    {
        u32 x = gabi::load<u32>(gabi::ea(&current.pos) + 0), y = gabi::load<u32>(gabi::ea(&current.pos) + 4),
            z = gabi::load<u32>(gabi::ea(&current.pos) + 8);
        gabi::store<u32>(gabi::ea(this) + 0x390, x); /* attention_info.position = current.pos */
        gabi::store<u32>(gabi::ea(this) + 0x394, y);
        gabi::store<u32>(gabi::ea(this) + 0x398, z);
        gabi::store<u32>(gabi::ea(&eyePos) + 0, x); /* eyePos = current.pos */
        gabi::store<u32>(gabi::ea(&eyePos) + 4, y);
        gabi::store<u32>(gabi::ea(&eyePos) + 8, z);
    }
    mCenter.x = current.pos.x + mJointX[0];
    mCenter.y = current.pos.y;
    mCenter.z = current.pos.z + mJointZ[0];
    if (gabi::load<u32>(gabi::ea(this) + 0x754 + 4) != 0) { /* mPtclCb.getEmitter() */
        gabi::Local<GXColor> colorAmb;
        gabi::Local<GXColor> colorDif;
        dKy_get_seacolor(colorAmb, colorDif);
        u32 emtr = gabi::load<u32>(gabi::ea(this) + 0x754 + 4);
        /* setGlobalPrmColor(r, g, b) */
        gabi::store<u8>(emtr + 0x244, gabi::load<u8>(gabi::ea(colorAmb.get()) + 0));
        gabi::store<u8>(emtr + 0x245, gabi::load<u8>(gabi::ea(colorAmb.get()) + 1));
        gabi::store<u8>(emtr + 0x246, gabi::load<u8>(gabi::ea(colorAmb.get()) + 2));
    }
    mDoMtx_stack_c::transS(mCenter.x, mCenter.y, mCenter.z);
    J3DModel_setBaseTRMtx(mpModelUnder, mDoMtx_stack_c::get());
    return TRUE;
}
VERIFY(0x024C8A58, &daTornado_c::execute);

/* 024C9368 */
static BOOL daTornado_Execute(daTornado_c* i_this) {
    WWHD_FUNC(0x024C9368, BOOL, i_this);
    return i_this->execute();
}
VERIFY(0x024C9368, daTornado_Execute);

/* 024C936C */
static BOOL daTornado_IsDelete(daTornado_c* i_this) {
    WWHD_FUNC(0x024C936C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024C936C, daTornado_IsDelete);

/* 024C9374 */
BOOL daTornado_c::tornado_delete() {
    WWHD_FUNC(0x024C9374, BOOL, this);
    /* mPtclCb.remove(): virtual (slot 0x44) */
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(this) + 0x754) + 0x44), gabi::at<void>(gabi::ea(this) + 0x754));
    if (fopAcM_GetParam(this) == 0) {
        u32 ship = dComIfGp_getShipActor();
        if (ship != 0)
            ship_offTornadoFlg(ship);
    }
    dComIfG_resDelete(&mPhs, l_arcName);
    return TRUE;
}
VERIFY(0x024C9374, &daTornado_c::tornado_delete);

/* 024C93F0 */
static BOOL daTornado_Delete(daTornado_c* i_this) {
    WWHD_FUNC(0x024C93F0, BOOL, i_this);
    i_this->tornado_delete();
    return TRUE;
}
VERIFY(0x024C93F0, daTornado_Delete);

/* 024C9414 */
BOOL daTornado_c::createHeap() {
    WWHD_FUNC(0x024C9414, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BDL_YTRND00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x20b, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x20b, STR(0x10040A20));
    J3DModel* m = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000202);
    mpModel = m;
    if (m == nullptr)
        return FALSE;
    J3DModel_027F58E0(m, 1); /* HD */
    if (!mBck.init(modelData, (J3DAnmTransform*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BCK_YTRND00_e, SAFESTRING_VTBL),
                   true, 2 /* EMode_LOOP */, 1.0f, 0, -1, false))
        return FALSE;
    if (!mBtk.init(modelData, (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BTK_YTRND00_e, SAFESTRING_VTBL),
                   false, 2, 1.0f, 0, -1, false, 0))
        return FALSE;
    if (!mDoExt_brkAnm_init(mBrk, modelData, dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BRK_YTRND00_e, SAFESTRING_VTBL), false,
                            2, 1.0f, 0, -1, false, 0))
        return FALSE;
    modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BDL_YWUWT00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x236, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x236, STR(0x10040A20));
    m = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000202);
    mpModelUnder = m;
    if (m == nullptr)
        return FALSE;
    if (!mBckUnder.init(modelData, (J3DAnmTransform*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BCK_YWUWT00_e, SAFESTRING_VTBL),
                        false, 2, 1.0f, 0, -1, false))
        return FALSE;
    if (!mBtkUnder.init(modelData,
                        (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BTK_YWUWT00_e, SAFESTRING_VTBL), false,
                        2, 1.0f, 0, -1, false, 0))
        return FALSE;
    if (!mDoExt_brkAnm_init(mBrkUnder, modelData, dComIfG_getObjectRes(l_arcName, dRes_INDEX_TRND_BRK_YWUWT00_e, SAFESTRING_VTBL),
                            false, 2, 1.0f, 0, -1, false, 0))
        return FALSE;
    return TRUE;
}
VERIFY(0x024C9414, &daTornado_c::createHeap);

/* 024C9708 */
static BOOL daTornado_createHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024C9708, BOOL, i_this);
    return ((daTornado_c*)i_this)->createHeap();
}
VERIFY(0x024C9708, daTornado_createHeap);

/* function-local static cXyz with a guard word (HD) */
static inline void static_cXyz(u32 guard, u32 p, f32 x, f32 y, f32 z) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<f32>(p + 0, x);
        gabi::store<f32>(p + 4, y);
        gabi::store<f32>(p + 8, z);
        gabi::store<u32>(guard, 1);
    }
}
static inline void setBaseScale_from(J3DModel* m, u32 s) {
    f32 x = gabi::load<f32>(s + 0), y = gabi::load<f32>(s + 4), z = gabi::load<f32>(s + 8);
    gabi::store<f32>(gabi::ea(m) + 0xBC, x);
    gabi::store<f32>(gabi::ea(m) + 0xC0, y);
    gabi::store<f32>(gabi::ea(m) + 0xC4, z);
}

/* 024C970C */
cPhs_State daTornado_c::create() {
    WWHD_FUNC(0x024C970C, cPhs_State, this);
    const u32 small_scale = 0x1046E9F8, under_small_scale = 0x1046EA04, under_scale = 0x1046EA10;
    static_cXyz(0x1046EA20, small_scale, 0.25f, 0.175f, 0.25f);
    static_cXyz(0x1046EA24, under_small_scale, 0.251f, 0.25f, 0.251f);
    static_cXyz(0x1046EA28, under_scale, 1.01f, 1.0f, 1.01f);

    cPhs_State rt = dComIfG_resLoad(&mPhs, l_arcName);
    /* fopAcM_ct(this, daTornado_c): HD vtable, inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = TORNADO_VTBL;
            mDoExt_bckAnm_ct(&mBck, BCK_TU_VTBL);
            mDoExt_btkAnm::ct(&mBtk);
            mDoExt_brkAnm_ct(mBrk);
            mDoExt_bckAnm_ct(&mBckUnder, BCK_TU_VTBL);
            mDoExt_btkAnm::ct(&mBtkUnder);
            mDoExt_brkAnm_ct(mBrkUnder);
            dPa_followEcallBack_ct((dPa_followEcallBack*)mPtclCb, 0, 0);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    if (rt == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x024C9708 /* daTornado_createHeap */, 0x8000)) {
            return cPhs_ERROR_e;
        }
        u32 param = fopAcM_GetParam(this);
        if (param == 1) {
            current.pos.y = current.pos.y - 2000.0f;
            setBaseScale_from(mpModel, small_scale);
            setBaseScale_from(mpModelUnder, under_small_scale);
        } else if (param == 2) {
            setBaseScale_from(mpModel, small_scale);
            setBaseScale_from(mpModelUnder, under_small_scale);
            for (int i = 0; i < 11; i++) {
                mJointScale[i] = l_joint_scale(i);
            }
        } else {
            mParameters = 0; /* fopAcM_SetParam(this, 0) */
            if (dComIfGs_isEventBit(0x2710)) {
                return cPhs_ERROR_e;
            }
            dKyw_tornado_Notice(&current.pos);
            setBaseScale_from(mpModelUnder, under_scale);
            gabi::store<u32>(gabi::ea(&mCenter) + 0, gabi::load<u32>(gabi::ea(&current.pos) + 0)); /* mCenter = current.pos */
            gabi::store<u32>(gabi::ea(&mCenter) + 4, gabi::load<u32>(gabi::ea(&current.pos) + 4));
            gabi::store<u32>(gabi::ea(&mCenter) + 8, gabi::load<u32>(gabi::ea(&current.pos) + 8));
            dComIfGp_particle_set(dPa_name_ID_AK_SN_WINDUPWATER00, &mCenter, nullptr, nullptr, 0xFF,
                                  (dPa_levelEcallBack*)mPtclCb);
            actor_status = actor_status | fopAcStts_SHOWMAP_e;
        }
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
        J3DModel_setBaseTRMtx(mpModelUnder, mDoMtx_stack_c::get());
        J3DModelData* modelData = J3DModel_getModelData(mpModel);
        u32 md = gabi::ea(modelData);
        for (u16 i = 1; i < gabi::load<u16>(J3DModelData_jointTree(modelData) + 8); i++) {
            /* modelData->getJointNodePointer(i)->setCallBack(daTornado_jointCallBack) */
            u32 n = gabi::load<u32>(md + 8);
            if (i < gabi::load<u32>(md + 4))
                n += i * 0x1C;
            gabi::store<u32>(n + 8, 0x024C8854);
        }
        gabi::store<u32>(gabi::ea(mpModel.get()) + 0xB8, gabi::ea(this)); /* mpModel->setUserArea(this) */
        /* HD: the joint matrix block starts as zero matrices */
        PSMTXScale(mDoMtx_stack_c::get(), 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 11; i++)
            PSMTXCopy(mDoMtx_stack_c::get(), &mJointMtx[i]);
    }
    return rt;
}
VERIFY(0x024C970C, &daTornado_c::create);

/* 024C9C28 */
static cPhs_State daTornado_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024C9C28, cPhs_State, i_this);
    return ((daTornado_c*)i_this)->create();
}
VERIFY(0x024C9C28, daTornado_Create);

/* 024C9C2C */
static void __sinit_d_a_tornado_cpp() {
    WWHD_FUNC(0x024C9C2C, void, (u32)0);
    sinit_header_statics(0x1046E9D0, 0x101D2A30);
}
VERIFY(0x024C9C2C, __sinit_d_a_tornado_cpp);

/* 024C9CC0: deleting destructor of a class with a trivial destructor (sead::SafeString, per TU) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024C9CC0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024C9CC0, trivial_dt);

/* 024C9CD4: daTornado_c deleting destructor (the two bck sub-objects' destructors) */
static void daTornado_c_dt(daTornado_c* i_this, s32 flags) {
    WWHD_FUNC(0x024C9CD4, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x027F3628, gabi::at<void>(gabi::ea(i_this) + 0x544), 0);
        gabi::call(0x027F3628, gabi::at<void>(gabi::ea(i_this) + 0x3C8), 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024C9CD4, daTornado_c_dt);

/* 024C9D40: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x024C9D40, void, p);
}
VERIFY(0x024C9D40, empty_virtual);
