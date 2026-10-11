/* Laser beam actor, with the original verified HD call and copy order. */
#include "d/actor/d_a_beam.h"
// HD: guest_fn cannot register instance methods. These names encode the original
// GHS PTMF targets while m550/m55C retain their exact eight-byte descriptors.
enum BeamProcHD : u32 {
    Beam_FIX_SEARCH_HD = 0x02076794,
    Beam_MOVE_SEARCH_HD = 0x02076260,
    Beam_WAIT_PROC_HD = 0x02076598
};
struct BeamVtablesHD {
    u32 safeString;
    u32 hio;
    u32 actor;
    u32 stts;
    u32 gStts;
    u32 aab;
    u32 shapeBase;
    u32 hitInf;
    u32 shape;
    u32 cps;
    u32 frameCtrlBase;
    u32 frameCtrl;
    u32 calculator;
    u32 objPolyPass;
    u32 objChk;
    u32 objGrpPass;
    u32 objPolyInfo;
    u32 polyPass;
    u32 grpPass;
    u32 polyInfo;
};
static const BeamVtablesHD l_vtables = {0x1000859C, 0x1000869C, 0x1000868C, 0x1004AE88, 0x1004AEC0,
                                        0x100085B4, 0x100015A8, 0x1004AF18, 0x1004AF70, 0x1004AF60,
                                        0x1016E54C, 0x100085D4, 0x1016D820, 0x1000867C, 0x1000864C,
                                        0x1000866C, 0x1000865C, 0x1000863C, 0x100085FC, 0x100085C4};

namespace {
void* ptr(u32 a, u32 o = 0) { return gabi::at<void>(a + o); }
// Keep live guest locals above the EABI linkage and four outgoing stack words.
struct CallLinkage {
    u8 bytes[32];
};
// HD: every original call reserves EABI linkage plus four outgoing words.
template <class F, class... A> decltype(auto) beamCall(F function, A... args) {
    gabi::Local<CallLinkage> frame;
    return function(args...);
}
template <class R = void, class... A> R beamCallIndirect(u32 method, A... args) {
    gabi::Local<CallLinkage> frame;
    return gabi::call_ptr<R>(method, args...);
}
// HD: integer-word copies preserve signaling NaNs and the original store order.
void copyWords(u32 dst, u32 src, u32 size) {
    auto* destination = gabi::at<be<u32>>(dst);
    auto* source = gabi::at<be<u32>>(src);
    for (u32 i = 0; i < size / 4; ++i)
        destination[i] = source[i];
}
f32 sinAngle(u16 a) { return gabi::at<BeamSinCosHD>(0x104A44F8)[a >> 3].mSin; }
f32 cosAngle(u16 a) { return gabi::at<BeamSinCosHD>(0x104A44F8)[a >> 3].mCos; }
void invalidateEmitter(u32 address) {
    auto* emitter = gabi::at<BeamEmitterHD>(address);
    u32 flags = emitter->mFlags;
    emitter->mMaxFrame = -1;
    emitter->mFlags = flags | 1;
}
void searchProc(daBeam_c* b, u32 target) {
    b->m550.d = 0;
    b->m550.i = -1;
    b->m550.f = target;
}
bool isSwitch(daBeam_c* b, s32 sw) {
    auto* save = dComIfGs_info();
    s8 room = b->home.roomNo;
    return beamCall(dSv_info_isSwitch, save, sw, room) != 0;
}
void endpoint(daBeam_c* b, cXyz* out, f32 distance) {
    u16 pitch = b->current.angle.x, yaw = b->current.angle.y;
    f32 horizontal = distance * cosAngle(pitch);
    f32 px = b->current.pos.x, py = b->current.pos.y, pz = b->current.pos.z;
    // Preserve the additive NaN payload chosen by the recompiled fused expression.
    f32 x = std::isnan(px) ? px : gabi::fmadds(horizontal, sinAngle(yaw), px);
    f32 z = std::isnan(pz) ? pz : gabi::fmadds(horizontal, cosAngle(yaw), pz);
    out->set(x, gabi::fnmsubs(distance, sinAngle(pitch), py), z);
}
void fade(daBeam_c* b) {
    if (b->m5F4 == 1) {
        f32 frame = b->m588;
        if (frame < 5.0f)
            b->m588 = frame + 1.0f;
        f32 fade = b->m5A8;
        if (fade < 4.0f)
            b->m5A8 = fade + 1.0f;
        else {
            b->m588 = 0.0f;
            b->m5A8 = 0.0f;
            b->m5F4 = 0;
        }
    } else {
        b->m5A8 = 0.0f;
        b->m588 = 0.0f;
    }
}
} // namespace

namespace {
void hitDirection(daBeam_c* b, void* at, void* other, void* tg) {
    auto* i_actor = static_cast<fopAc_ac_c*>(other);
    u32 actor = gabi::ea(other);
    if (other && i_actor->mProcName == fpcNm_PLAYER_e) {
        if (actor + 0x314) {
            s8 room = i_actor->current.roomNo;
            s32 reverb = beamCall(dComIfGp_getReverbHD, room);
            beamCall(mDoAud_seStartHD, JA_SE_LK_BEAM_HIT, &i_actor->current.pos, 0, reverb);
        }
        if (b->m54C) {
            u16 yaw = b->current.angle.y;
            gabi::Local<cXyz> direction, delta;
            direction->set(-cosAngle(yaw), 0.0f, sinAngle(yaw));
            beamCall(cXyz_subHD, &i_actor->current.pos, delta.get(), &b->current.pos);
            f32 dot = beamCall(VECDotProductHD, direction.get(), delta.get());
            f32 x = direction->x, y = direction->y, z = direction->z;
            if (dot < 0.0f) {
                x = -x;
                z = -z;
            }
            static_cast<dCcD_GObjInf*>(at)->mGObjAt.mVec.set(x, y, z);
            static_cast<dCcD_GObjInf*>(tg)->mGObjTg.mRVec.set(x, y, z);
        }
    } else if (other && i_actor->mProcName == fpcNm_AM2_e) {
        u32 vtable = static_cast<dCcD_GObjInf*>(tg)->__vtbl_hitinf;
        u32 method = gabi::at<BeamHitMethodsHD>(vtable)->mpHit;
        beamCallIndirect(method, tg);
    }
}
} // namespace

/* 02075124 */
void daBeam_c::set_mtx() {
    f32 distance = 100.0f * (f32)scale.z;
    gabi::Local<cXyz> delta;
    beamCall(cXyz_subHD, &m668, delta.get(), &current.pos);
    f32 square = beamCall(VECSquareMagHD, delta.get());
    f32 length = beamCall(sqrtfHD, square) / distance;
    f32 width = ((f32)m588 * (5.0f - (f32)m5A8)) / 50.0f + 0.5f;
    auto* model = M_mdl.get();
    auto scaleVec = scale.get();
    model->mBaseScale.y = scaleVec.y;
    model->mBaseScale.x = scaleVec.x;
    model->mBaseScale.z = scaleVec.z;
    u32 matrix = 0x1048D0CC;
    auto pos = current.pos.get();
    beamCall(MTXTransHD, ptr(matrix), pos.x, pos.y, pos.z);
    s16 x = current.angle.x, y = current.angle.y, z = current.angle.z;
    beamCall(mDoMtx_ZXYrotMHD, ptr(matrix), x, y, z);
    beamCall(mDoMtx_stack_scaleMHD, width, width, length);
    f32 m[12];
    auto* source = gabi::at<be<f32>>(matrix);
    for (int i = 0; i < 12; ++i)
        m[i] = source[i];
    model = M_mdl.get();
    auto* destination = reinterpret_cast<be<f32>*>(&model->mBaseTransformMtx);
    for (int i = 0; i < 12; ++i)
        destination[i] = m[i];
    if (gabi::ea(m690.get())) {
        pos = current.pos.get();
        beamCall(MTXTransHD, ptr(matrix), pos.x, pos.y, pos.z);
        x = current.angle.x;
        y = current.angle.y;
        z = current.angle.z;
        beamCall(mDoMtx_ZXYrotMHD, ptr(matrix), x, y, z);
        beamCall(mDoMtx_stack_transMHD, 0.0f, 0.0f, (f32)m694);
        auto* emitter = m690.get();
        beamCall(JPASetRMtxTVecfromMtxHD, ptr(matrix), &emitter->mGlobalRot, &emitter->mGlobalTrs);
        emitter = m690.get();
        f32 s = scale.x;
        emitter->mGlobalPrmScl.y = s;
        emitter->mGlobalScl.x = s;
        emitter->mGlobalScl.z = s;
        emitter->mGlobalScl.y = s;
        emitter->mGlobalPrmScl.z = s;
        emitter->mGlobalPrmScl.x = s;
    }
    if (gabi::ea(m68C.get())) {
        pos = m668.get();
        beamCall(MTXTransHD, ptr(matrix), pos.x, pos.y, pos.z);
        beamCall(mDoMtx_stack_quatMHD, &m674);
        y = current.angle.y;
        beamCall(mDoMtx_YrotMHD, ptr(matrix), y);
        beamCall(mDoMtx_stack_scaleMHD, 1.0f, 1.0f, (f32)m688);
        beamCall(MTXCopyHD, ptr(matrix), &m5C4);
        auto* floor = m68C.get();
        floor->mtx_p = &m5C4;
        floor->mbSpawnParticle = 1;
    }
}

/* 02074C94 */
BOOL CheckCreateHeap(daBeam_c* b) { return beamCall(Beam_CreateHeapHD, b); }

/* 02074A4C */
BOOL daBeam_c::CreateHeap() {
    struct SafeString {
        be<u32> text, vtable;
    };
    gabi::Local<SafeString> name;
    name->text = 0x10008758;
    name->vtable = l_vtables.safeString;
    u32 manager = gabi::ea(*gabi::at<gptr<void>>(0x101F4F28));
    u32 data = beamCall(dRes_control_getResIndexHD, ptr(manager), name.get(),
                        dRes_INDEX_YLESR00_BMD_YLESR00_e);
    if (!data)
        beamCall(Beam_assertHD, ptr(0x100086B0), 304, ptr(0x100086C0));
    M_mdl = gabi::at<J3DModel>(beamCall(mDoExt_J3DModel_createHD, ptr(data), 0, 0x11020203));
    if (!gabi::ea(M_mdl.get()))
        beamCall(Beam_assertHD, ptr(0x100086B0), 306, ptr(0x100086D4));
    gabi::Local<SafeString> n2;
    n2->text = 0x10008758;
    n2->vtable = l_vtables.safeString;
    M_bck = gabi::at<J3DAnmTransform>(beamCall(dRes_control_getResIndexHD,
                                               ptr(gabi::ea(*gabi::at<gptr<void>>(0x101F4F28))),
                                               n2.get(), dRes_INDEX_YLESR00_BCK_YLESR00_e));
    if (!gabi::ea(M_bck.get()))
        beamCall(Beam_assertHD, ptr(0x100086B0), 310, ptr(0x100086E4));
    gabi::Local<SafeString> n3;
    n3->text = 0x10008758;
    n3->vtable = l_vtables.safeString;
    M_brk = gabi::at<J3DAnmTevRegKey>(beamCall(dRes_control_getResIndexHD,
                                               ptr(gabi::ea(*gabi::at<gptr<void>>(0x101F4F28))),
                                               n3.get(), dRes_INDEX_YLESR00_BRK_YLESR00_e));
    if (!gabi::ea(M_brk.get()))
        beamCall(Beam_assertHD, ptr(0x100086B0), 314, ptr(0x100086F4));
    gabi::Local<SafeString> n4;
    n4->text = 0x10008758;
    n4->vtable = l_vtables.safeString;
    M_btk = gabi::at<J3DAnmTextureSRTKey>(beamCall(dRes_control_getResIndexHD,
                                                   ptr(gabi::ea(*gabi::at<gptr<void>>(0x101F4F28))),
                                                   n4.get(), dRes_INDEX_YLESR00_BTK_YLESR00_e));
    // HD: the final resource assertion rechecks BRK, as in the verified binary.
    if (!gabi::ea(M_brk.get()))
        beamCall(Beam_assertHD, ptr(0x100086B0), 318, ptr(0x100086F4));
    s32 a = beamCall(mDoExt_bckAnm_initHD, &mBckAnm, ptr(data), ptr(gabi::ea(M_bck.get())), 0, 2,
                     1.0f, 0, -1, 0);
    s32 c = beamCall(mDoExt_brkAnm_initHD, &mBrkAnm, ptr(data), ptr(gabi::ea(M_brk.get())), 0, 2,
                     1.0f, 0, -1, 0, 0);
    s32 d = beamCall(mDoExt_btkAnm_initHD, &mBtkAnm, ptr(data), ptr(gabi::ea(M_btk.get())), 1, 2,
                     1.0f, 0, -1, 0, 0);
    return gabi::ea(M_mdl.get()) && a && c && d;
}

/* 02074C98 */
void daBeam_AtHitCallback(daBeam_c* b, void* at, void* other, void* tg) {
    if (!beamCall(fopAcM_IsActorHD, other))
        return;
    hitDirection(b, at, other, tg);
    u32 parent = b->parentActorID;
    u32 id = other ? static_cast<fopAc_ac_c*>(other)->mBsPcId : 0xFFFFFFFF;
    if (id != parent) {
        b->m540.copy(b->mCps1.mGObjAt.mHitPos);
        b->m53C = 1;
    }
}

/* 02074E90 */
void daBeam_AtHitDummyCallback(daBeam_c* b, void* at, void* other, void* tg) {
    if (beamCall(fopAcM_IsActorHD, other))
        hitDirection(b, at, other, tg);
}

/* 02075078 */
void daBeam_checkHitCallback(daBeam_c* b, void* at, void* other, void* tg) {
    if (!beamCall(fopAcM_IsActorHD, other))
        return;
    u32 parent = b->parentActorID;
    u32 id = other ? static_cast<fopAc_ac_c*>(other)->mBsPcId : 0xFFFFFFFF;
    if (parent == id)
        return;
    if (other && static_cast<fopAc_ac_c*>(other)->mProcName == fpcNm_PLAYER_e &&
        !(static_cast<dCcD_GObjInf*>(at)->mGObjAt.mRPrm & 1))
        return;
    b->m540.copy(b->mCps2.mGObjAt.mHitPos);
    b->m53C = 1;
}

/* 02075384 */
s32 daBeam_c::CreateInit() {
    beamCall(dCcD_Stts_InitHD, &mStts, 255, 255, this);
    u32 mode = (u32)mParameters >> 30;
    beamCall(dCcD_Cps_SetHD, &mCps1, ptr(0x10190BEC));
    gabi::Local<cXyz> end;
    f32 distance = 100.0f * (f32)scale.z;
    mCps1.mStts = &mStts;
    endpoint(this, end.get(), distance);
    beamCall(cM3dGLin_SetStartEndHD, &mCps1.mCps, &current.pos, end.get());
    u32 flags = mCps1.mGObjAt.mSPrm;
    mCps1.mCps.mRadius = 25.0f;
    mCps1.mGObjAt.mHitCallback = mode ? gabi::guest_fn((void*)daBeam_AtHitDummyCallback)
                                      : gabi::guest_fn((void*)daBeam_AtHitCallback);
    mCps1.mGObjAt.mSPrm = flags | 6;
    beamCall(dCcD_Cps_SetHD, &mCps2, ptr(0x10190C38));
    mCps2.mStts = &mStts;
    beamCall(cM3dGLin_SetStartEndHD, &mCps2.mCps, &current.pos, end.get());
    flags = mCps2.mObjAt.mSPrm;
    mCps2.mCps.mRadius = 25.0f;
    mCps2.mObjAt.mSPrm = flags | 16;
    if (!mode)
        mCps2.mGObjAt.mHitCallback = gabi::guest_fn((void*)daBeam_checkHitCallback);
    copyWords(gabi::ea(&m674), 0x101E9C38, 16);
    m684 = -1;
    m68C = nullptr;
    beamCall(Beam_set_mtxHD, this);
    u32 model = gabi::ea(M_mdl.get());
    cullMtx = model ? gabi::ea(&M_mdl->mBaseTransformMtx) : 0;
    beamCall(fopAcM_setCullSizeBoxHD, this, -50.0f, -50.0f, 0.0f, 50.0f, 50.0f, 2000.0f);
    if (parentActorID == 0xFFFFFFFF) {
        u8 kind = (u32)mParameters;
        kind = kind > 2 ? 2 : kind;
        m550.d = 0;
        m550.i = -1;
        // HD: kind 2 uses move_search; there is no separate timer_change entry.
        u32 target = kind == 0 ? Beam_FIX_SEARCH_HD : Beam_MOVE_SEARCH_HD;
        if (kind < 2) {
            m55C.d = 0;
            m55C.i = -1;
            m55C.f = target;
        }
        m550.f = target;
        u32 parameters = mParameters;
        scale.set(2.5f, 2.5f, 20.0f);
        f32 offset = *gabi::at<be<f32>>(0x1047BBD8);
        m690 = nullptr;
        m698 = (parameters >> 16) & 255;
        m694 = 0.001f * offset;
        s32 sw = m698;
        if (sw != 255 && isSwitch(this, sw))
            searchProc(this, Beam_WAIT_PROC_HD);
        m54C = 0;
    } else {
        u32 parameters = mParameters;
        m694 = 0.0f;
        m690 = nullptr;
        m5F8 = 1 - ((parameters >> 28) & 1);
        m54C = (parameters >> 24) & 15;
        m5F9 = 1 - ((parameters >> 29) & 1);
    }
    return cPhs_COMPLEATE_e;
}

/* 02075988 */
BOOL daBeam_c::_execute() {
    s16 slot = m550.i;
    if (slot) {
        beamCall(this->*m550);
        s32 sw = m698;
        if (sw != 255) {
            if (isSwitch(this, sw))
                searchProc(this, Beam_WAIT_PROC_HD);
            else {
                u32 descriptor = *reinterpret_cast<be<u32>*>(&m55C);
                u32 target = m55C.f;
                *reinterpret_cast<be<u32>*>(&m550) = descriptor;
                m550.f = target;
            }
        }
    }
    beamCall(mDoExt_baseAnm_playHD, &mBtkAnm);
    gabi::Local<cXyz> end, hitEnd, direction, delta, lineEnd, normal;
    endpoint(this, end.get(), 100.0f * (f32)scale.z);
    beamCall(cM3dGLin_SetStartEndHD, &mCps2.mCps, &current.pos, end.get());
    mCps2.mCps.mRadius = 25.0f;
    auto* gameInfo = beamCall(dComIfGp_getHD);
    beamCall(cCcS_SetHD, &gameInfo->play.mCcS, &mCps2);
    if (m5F4 == 1) {
        f32 length = 100.0f * (f32)scale.z;
        if (m53C == 1) {
            beamCall(cXyz_subHD, &m540, delta.get(), &current.pos);
            f32 square = beamCall(VECSquareMagHD, delta.get());
            length = beamCall(sqrtfHD, square);
        }
        endpoint(this, hitEnd.get(), length);
        beamCall(cXyz_subHD, hitEnd.get(), direction.get(), &current.pos);
        if (!beamCall(cXyz_normalizeRS_HSquaredHD, direction.get()))
            copyWords(gabi::ea(direction.get()), 0x101FFBA8, 12);
        beamCall(cM3dGLin_SetStartEndHD, &mCps1.mCps, &current.pos, hitEnd.get());
        mCps1.mGObjAt.mVec.copy(*direction.get());
        mCps1.mCps.mRadius = 25.0f;
        gameInfo = beamCall(dComIfGp_getHD);
        beamCall(cCcS_SetHD, &gameInfo->play.mCcS, &mCps1);
    }
    f32 frame = m588;
    if (!(frame > 0.0f)) {
        m68C = nullptr;
        m53C = 0;
        m684 = -1;
        return 0;
    }
    endpoint(this, lineEnd.get(), (frame * 20.0f) * (f32)scale.z);
    mLinChk.mBackFlag = 0;
    beamCall(dBgS_LinChk_SetHD, &mLinChk, &current.pos, lineEnd.get(), this);
    if (m53C == 1) {
        m668.copy(m540);
        m684 = -1;
        m688 = 1.0f;
        m68C = nullptr;
    } else {
        gameInfo = beamCall(dComIfGp_getHD);
        if (beamCall(cBgS_LineCrossHD, &gameInfo->play.mBgS, &mLinChk)) {
            gameInfo = beamCall(dComIfGp_getHD);
            u16 group = mLinChk.mBgIndex, triangle = mLinChk.mPolyIndex;
            u32 plane = beamCall(cBgS_GetTriPlaHD, &gameInfo->play.mBgS, group, triangle);
            m668.copy(mLinChk.mLin.mEnd);
            if (plane) {
                auto* planeNormal = gabi::at<cXyz>(plane);
                normal->set(planeNormal->x, planeNormal->y, planeNormal->z);
                beamCall(Quaternion_setRotateHD, &m674, normal.get());
            }
            if (m684 == 0xFFFFFFFF) {
                u32 parameters = (!m5F8 ? 1 : 0) | (!m5F9 ? 2 : 0);
                m684 = beamCall(fopAcM_createHD, fpcNm_Hot_Floor_e, parameters, &m668, -1, ptr(0),
                                ptr(0), -1, ptr(0));
            }
            m688 = 1.0f;
        } else {
            m668.copy(*lineEnd.get());
            m688 = 1.0f;
            m684 = -1;
            m68C = nullptr;
        }
    }
    if (m684 != 0xFFFFFFFF && !gabi::ea(m68C.get())) {
        gabi::Local<be<u32>> id;
        *id.get() = m684;
        m68C = gabi::at<daHot_Floor_c>(beamCall(fopAcM_SearchHD, ptr(0x025E1234), id.get()));
    }
    s8 room = current.roomNo;
    s32 reverb = beamCall(dComIfGp_getReverbHD, room);
    beamCall(mDoAud_seStartHD, JA_SE_OBJ_STATUE_BEAM, &m668, 0, reverb);
    m53C = 0;
    return 0;
}

/* 02076084 */
BOOL daBeam_c::checkRange(csXyz* angle) {
    u16 yaw = home.angle.y;
    f32 sn = sinAngle(yaw), cs = cosAngle(yaw);
    gabi::Local<cXyz> forward, side, position, delta, flat;
    forward->set(sn, 0.0f, cs);
    side->set(cs, 0.0f, -sn);
    auto* gameInfo = beamCall(dComIfGp_getHD);
    auto* player = gameInfo->play.mPlayerInfo[0].mpPlayer.get();
    position->set(player->current.pos.x, player->current.pos.y, player->current.pos.z);
    u32 vtable = player->__vtbl;
    u32 method = gabi::at<BeamPlayerMethodsHD>(vtable)->mpEyeHeight;
    position->y = beamCallIndirect<f32>(method, player);
    beamCall(cXyz_subHD, position.get(), delta.get(), &current.pos);
    f32 horizontal = beamCall(VECDotProductHD, side.get(), delta.get());
    f32 vertical = -(f32)delta->y;
    f32 depth = beamCall(VECDotProductHD, forward.get(), delta.get());
    if (!(depth < 500.0f) || !(depth > 0.0f) || !(std::fabs(horizontal) < 150.0f) ||
        !(std::fabs(vertical) < 250.0f))
        return 0;
    if (angle) {
        flat->set(horizontal, 0.0f, depth);
        f32 square = beamCall(VECSquareMagHD, flat.get());
        f32 length = beamCall(sqrtfHD, square);
        s16 pitch = beamCall(cM_atan2sHD, vertical, length);
        angle->x = pitch;
        f32 x = delta->x, z = delta->z;
        angle->y = beamCall(cM_atan2sHD, x, z);
    }
    return 1;
}

/* 02076260 */
void daBeam_c::move_search() {
    gabi::Local<csXyz> angle;
    if (beamCall(Beam_checkRangeHD, this, angle.get())) {
        s16 pitch = angle->x;
        s32 delta = beamCall(cLib_addCalcAngleSHD, &current.angle.x, pitch, 2, 0x400, 0);
        s16 yaw = angle->y;
        beamCall(cLib_addCalcAngleS2HD, &current.angle.y, yaw, 4, 0x400);
        u32 magnitude = delta < 0 ? 0u - (u32)delta : (u32)delta;
        if ((s32)magnitude < 0x400 && m5F4 == 0) {
            if (!gabi::ea(m690.get())) {
                auto* gameInfo = beamCall(dComIfGp_getHD);
                u32 particles = gabi::ea(gameInfo->play.mParticle);
                u32 emitter =
                    beamCall(dPa_control_setHD, ptr(particles), 0, ID_AK_SN_LASERROOT00,
                             &current.pos, ptr(0), ptr(0), 255, ptr(0), -1, ptr(0), ptr(0), ptr(0));
                s16 active = m5F4;
                m690 = gabi::at<BeamEmitterHD>(emitter);
                if (active != 0) {
                    m588 = 5.0f;
                    m5A8 = 0.0f;
                    return;
                }
            }
            f32 frame = m588;
            m5A8 = 0.0f;
            if (frame < 5.0f)
                m588 = frame + 1.0f;
            else {
                m5A8 = 0.0f;
                m588 = 5.0f;
                m5F4 = 1;
            }
        }
    } else {
        if (m5F4 == 0 || !((f32)m588 > 0.4f)) {
            s16 yaw = home.angle.y;
            beamCall(cLib_addCalcAngleS2HD, &current.angle.y, yaw, 4, 0x400);
            s16 pitch = home.angle.x;
            beamCall(cLib_addCalcAngleS2HD, &current.angle.x, pitch, 4, 0x400);
        }
        u32 emitter = gabi::ea(m690.get());
        if (emitter) {
            invalidateEmitter(emitter);
            m690 = nullptr;
        }
        fade(this);
    }
}

/* 02076794 */
void daBeam_c::fix_search() {}

/* 02076598 */
void daBeam_c::wait_proc() {
    u32 emitter = gabi::ea(m690.get());
    if (emitter) {
        invalidateEmitter(emitter);
        m690 = nullptr;
    }
    fade(this);
    if (!m5F4) {
        current.angle.z = home.angle.z;
        current.angle.y = home.angle.y;
        current.angle.x = home.angle.x;
    }
}

/* 02075720 */
// HD: GameCube inline _create and GHS constructors form one verified entry.
s32 daBeamCreate(daBeam_c* b) {
    if (!((u32)b->actor_condition & 8)) {
        if (b) {
            beamCall(fopAc_ac_ctHD, b);
            b->__vtbl = l_vtables.actor;
            beamCall(cCcD_Stts_ctHD, &b->mStts);
            beamCall(dCcD_GStts_ctHD, &b->mStts.__vtbl_gstts);
            b->mStts.__vtbl = l_vtables.stts;
            b->mStts.__vtbl_gstts = l_vtables.gStts;
            beamCall(dCcD_GObjInf_ctHD, &b->mCps1);
            b->mCps1.mAab.__vtbl = l_vtables.aab;
            b->mCps1.__vtbl_shape = l_vtables.shapeBase;
            beamCall(cM3dGLin_ctHD, &b->mCps1.mCps);
            b->mCps1.__vtbl_hitinf = l_vtables.hitInf;
            b->mCps1.__vtbl_shape = l_vtables.shape;
            b->mCps1.mCps.__vtbl = l_vtables.cps;
            beamCall(dCcD_GObjInf_ctHD, &b->mCps2);
            b->mCps2.__vtbl_shape = l_vtables.shapeBase;
            b->mCps2.mAab.__vtbl = l_vtables.aab;
            beamCall(cM3dGLin_ctHD, &b->mCps2.mCps);
            b->mCps2.__vtbl_shape = l_vtables.shape;
            b->mCps2.__vtbl_hitinf = l_vtables.hitInf;
            b->mCps2.mCps.__vtbl = l_vtables.cps;
            beamCall(mDoExt_bckAnm_ctHD, &b->mBckAnm, 0);
            reinterpret_cast<BeamBckHD*>(&b->mBckAnm)->mFrameCtrlVtable = l_vtables.frameCtrlBase;
            beamCall(J3DMtxCalcAnm_ctHD, &reinterpret_cast<BeamBckHD*>(&b->mBckAnm)->mCalculator);
            reinterpret_cast<BeamBckHD*>(&b->mBckAnm)->mAnmTransform = 0;
            reinterpret_cast<BeamBckHD*>(&b->mBckAnm)->mAnm = 0;
            reinterpret_cast<BeamBckHD*>(&b->mBckAnm)->mFrameCtrlVtable = l_vtables.frameCtrl;
            reinterpret_cast<BeamBckHD*>(&b->mBckAnm)->mOldFrameMorf = 0;
            reinterpret_cast<BeamBckHD*>(&b->mBckAnm)->mCalculatorVtable = l_vtables.calculator;
            reinterpret_cast<BeamBckHD*>(&b->mBckAnm)->mOldFrame = 0;
            reinterpret_cast<BeamBckHD*>(&b->mBckAnm)->mMorf = 0;
            beamCall(mDoExt_brkAnm_ctHD, &b->mBrkAnm);
            beamCall(mDoExt_btkAnm_ctHD, &b->mBtkAnm);
            beamCall(dBgS_LinChk_ctHD, &b->mLinChk);
            b->mLinChk.mbCamThrough = 0;
            b->mLinChk.mbLinkThrough = 0;
            b->mLinChk.mbRopeThrough = 0;
            b->mLinChk.mbBoomerangThrough = 0;
            b->mLinChk.mbArrowThrough = 0;
            b->mLinChk.mbBombThrough = 0;
            b->mLinChk.mbObjThrough = 1;
            b->mLinChk.mGrp = 1;
            b->mLinChk.dBgS_PolyPassChk::__vtbl = l_vtables.objPolyPass;
            b->mLinChk.cBgS_Chk::__vtbl = l_vtables.objChk;
            b->mLinChk.mPolyPassChk = gabi::ea(&b->mLinChk.dBgS_PolyPassChk::__vtbl);
            b->mLinChk.mGrpPassChk = gabi::ea(&b->mLinChk.dBgS_GrpPassChk::__vtbl);
            b->mLinChk.dBgS_GrpPassChk::__vtbl = l_vtables.objGrpPass;
            b->mLinChk.cBgS_PolyInfo::__vtbl = l_vtables.objPolyInfo;
        }
        b->actor_condition = (u32)b->actor_condition | 8;
    }
    s32 phase = beamCall(dComIfG_resLoadHD, &b->mPhase, ptr(0x10008758));
    if (phase == cPhs_COMPLEATE_e) {
        if (beamCall(fopAcM_entrySolidHeapHD, b, ptr(gabi::guest_fn((void*)CheckCreateHeap)),
                     0x22A0))
            return beamCall(Beam_CreateInitHD, b);
        return cPhs_ERROR_e;
    }
    return phase;
}

/* 02075924 */
// HD: GameCube inline _delete and the actor wrapper form one verified entry.
BOOL daBeamDelete(daBeam_c* b) {
    beamCall(mDoAud_seDeleteObjectHD, &b->m668);
    beamCall(dComIfG_resDeleteHD, &b->mPhase, ptr(0x10008758));
    u32 emitter = gabi::ea(b->m690.get());
    if (emitter)
        invalidateEmitter(emitter);
    return 1;
}

/* 02075F8C */
BOOL daBeamExecute(daBeam_c* b) { return beamCall(Beam_executeHD, b); }

/* 02075F90 */
// HD: GameCube inline _draw and the actor wrapper form one verified entry.
BOOL daBeamDraw(daBeam_c* b) {
    if (!b->m5F4)
        return 1;
    beamCall(Beam_set_mtxHD, b);
    u32 data = gabi::ea(b->M_mdl.get()->mModelData);
    f32 frame = b->mBtkAnm.mFrameCtrl.mFrame;
    beamCall(mDoExt_btkAnm_entryHD, &b->mBtkAnm, ptr(data), frame);
    frame = (s16)gabi::ftoi(b->m5A8);
    data = gabi::ea(b->M_mdl.get()->mModelData);
    beamCall(mDoExt_brkAnm_entryHD, &b->mBrkAnm, ptr(data), frame);
    frame = (s16)gabi::ftoi(b->m588);
    data = gabi::ea(b->M_mdl.get()->mModelData);
    beamCall(mDoExt_bckAnm_entryHD, &b->mBckAnm, ptr(data), frame);
    beamCall(mDoExt_modelUpdateHD, ptr(gabi::ea(b->M_mdl.get())), 0);
    return 1;
}

/* 0207678C */
BOOL daBeamIsDelete(void* p) { return 1; }

// HD: compiler-generated and template code of this translation unit.

/* 0207667C */
void* daBeam_HIO_ct(void* p) {
    if (!p)
        p = beamCall(operator_newHD, 16);
    if (p) {
        auto* hio = static_cast<daBeam_HIO_c*>(p);
        hio->mNo = -1;
        hio->m08 = 0.0f;
        hio->m0C = 0;
        hio->__vtbl = l_vtables.hio;
    }
    return p;
}

/* 020766D8 */
void __sinit_d_a_beam_cpp() {
    for (u32 i = 0; i < 4; ++i)
        gabi::at<BeamHeaderStaticsHD>(0x104619A4)->mZero[i] = 0;
    beamCall(BeamHeaderObject_ct, ptr(0x10190C84));
    gabi::at<BeamHeaderStaticsHD>(0x104619A4)->mNegativePi = -3.1415927410125732f;
    gabi::at<BeamHeaderStaticsHD>(0x104619A4)->mPositivePi = 3.1415927410125732f;
    beamCall(BeamFirstHeader_ct, ptr(0x104619AC));
    beamCall(BeamHeaderObject_ct, ptr(0x10190C90));
    beamCall(BeamSecondHeader_ct, ptr(0x104619AD));
    beamCall(BeamHeaderObject_ct, ptr(0x10190C9C));
    beamCall(Beam_HIO_ctHD, ptr(0x104619B0));
}

/* 02076778 */
void BeamSafeString_dt(void* p, u32 flags) {
    if (p && (flags & 1))
        beamCall(operator_deleteHD, p);
}

/* 02076798 */
void daBeam_dt(daBeam_c* b, u32 flags) {
    if (!b)
        return;
    b->mLinChk.dBgS_PolyPassChk::__vtbl = l_vtables.polyPass;
    b->mLinChk.dBgS_GrpPassChk::__vtbl = l_vtables.grpPass;
    b->mLinChk.cBgS_PolyInfo::__vtbl = l_vtables.polyInfo;
    beamCall(dBgS_LinChk_dtHD, &b->mLinChk, 0);
    beamCall(J3DFrameCtrl_dtHD, &reinterpret_cast<BeamBckHD*>(&b->mBckAnm)->mFrameCtrlVtable, 0);
    beamCall(dCcD_Cps_dtHD, &b->mCps2, 2);
    beamCall(dCcD_Cps_dtHD, &b->mCps1, 2);
    beamCall(dCcD_Stts_dtHD, &b->mStts, 2);
    beamCall(fopAc_ac_dtHD, b, 0);
    if (flags & 1)
        beamCall(operator_deleteHD, b);
}

/* 0207684C */
void BeamSafeString_assureTermination() {}

// clang-format off
/* WWHD glue
ext 025BA0C0 s32 dSv_info_isSwitch(void*, s32, s32)
ext 026066C4 u32 dRes_control_getResIndexHD(void*, void*, s32)
ext 0273AA24 void Beam_assertHD(void*, s32, void*)
ext 025E38E0 u32 mDoExt_J3DModel_createHD(void*, u32, u32)
ext 025E8508 s32 mDoExt_bckAnm_initHD(void*, void*, void*, s32, s32, f32, s32, s32, s32)
ext 025E8154 s32 mDoExt_brkAnm_initHD(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32)
ext 025E7CE0 s32 mDoExt_btkAnm_initHD(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32)
ext 02074A4C BOOL Beam_CreateHeapHD(daBeam_c*)
ext 0201ADE0 void cXyz_subHD(const void*, void*, const void*)
ext 028E8DD0 f32 VECSquareMagHD(const void*)
ext 028F4384 f32 sqrtfHD(f32)
ext 028E93CC void MTXTransHD(void*, f32, f32, f32)
ext 025F1B48 void mDoMtx_ZXYrotMHD(void*, s32, s32, s32)
ext 025F2518 void mDoMtx_stack_scaleMHD(f32, f32, f32)
ext 025F24E0 void mDoMtx_stack_transMHD(f32, f32, f32)
ext 028249B0 void JPASetRMtxTVecfromMtxHD(void*, void*, void*)
ext 025F25CC void mDoMtx_stack_quatMHD(void*)
ext 025F1C28 void mDoMtx_YrotMHD(void*, s32)
ext 028E90D4 void MTXCopyHD(void*, void*)
ext 025E1B34 void mDoAud_seDeleteObjectHD(void*)
ext 025204C8 void dComIfG_resDeleteHD(void*, void*)
ext 02075124 void Beam_set_mtxHD(daBeam_c*)
ext 025E7FC4 void mDoExt_btkAnm_entryHD(void*, void*, f32)
ext 025E83FC void mDoExt_brkAnm_entryHD(void*, void*, f32)
ext 025E86B8 void mDoExt_bckAnm_entryHD(void*, void*, f32)
ext 025E2DE0 void mDoExt_modelUpdateHD(void*, s32)
ext 0273AD10 void* operator_newHD(u32)
ext 028F026C void BeamHeaderObject_ct(void*)
ext 028ED6F8 void BeamFirstHeader_ct(void*)
ext 028EAB2C void BeamSecondHeader_ct(void*)
ext 0207667C void* Beam_HIO_ctHD(void*)
ext 0273AF40 void operator_deleteHD(void*)
ext 02008B4C void dBgS_LinChk_dtHD(void*, s32)
ext 027F3628 void J3DFrameCtrl_dtHD(void*, s32)
ext 02515980 void dCcD_Cps_dtHD(void*, s32)
ext 02515860 void dCcD_Stts_dtHD(void*, s32)
ext 025D50BC void fopAc_ac_dtHD(void*, s32)
ext 02520540 s32 dComIfGp_getReverbHD(s32)
ext 025E1A40 void mDoAud_seStartHD(u32, void*, s32, s32)
ext 028E8F44 f32 VECDotProductHD(void*, void*)
ext 025D4604 s32 fopAcM_IsActorHD(void*)
ext 025200D4 dComIfG_inf_c* dComIfGp_getHD()
ext 020195B0 s16 cM_atan2sHD(f32, f32)
ext 02076084 BOOL Beam_checkRangeHD(daBeam_c*, csXyz*)
ext 0200F378 s32 cLib_addCalcAngleSHD(void*, s32, s32, s32, s32)
ext 0200F428 void cLib_addCalcAngleS2HD(void*, s32, s32, s32)
ext 025A847C u32 dPa_control_setHD(void*, s32, s32, void*, void*, void*, s32, void*, s32, void*, void*, void*)
ext 02515F14 void dCcD_Stts_InitHD(void*, s32, s32, void*)
ext 025164C0 void dCcD_Cps_SetHD(void*, void*)
ext 02018808 void cM3dGLin_SetStartEndHD(void*, void*, void*)
ext 025D674C void fopAcM_setCullSizeBoxHD(void*, f32, f32, f32, f32, f32, f32)
ext 025D4ED0 void fopAc_ac_ctHD(void*)
ext 0200BD2C void cCcD_Stts_ctHD(void*)
ext 02515DA0 void dCcD_GStts_ctHD(void*)
ext 02515FB8 void dCcD_GObjInf_ctHD(void*)
ext 02018150 void cM3dGLin_ctHD(void*)
ext 027F2BC0 void mDoExt_bckAnm_ctHD(void*, s32)
ext 027DA984 void J3DMtxCalcAnm_ctHD(void*)
ext 025E80D0 void mDoExt_brkAnm_ctHD(void*)
ext 025E7C6C void mDoExt_btkAnm_ctHD(void*)
ext 02008FEC void dBgS_LinChk_ctHD(void*)
ext 02520460 s32 dComIfG_resLoadHD(void*, void*)
ext 025D63E8 s32 fopAcM_entrySolidHeapHD(void*, void*, s32)
ext 02075384 s32 Beam_CreateInitHD(daBeam_c*)
ext 025E742C void mDoExt_baseAnm_playHD(void*)
ext 0200E240 void cCcS_SetHD(void*, void*)
ext 0201B47C s32 cXyz_normalizeRS_HSquaredHD(void*)
ext 024F1AFC void dBgS_LinChk_SetHD(void*, void*, void*, void*)
ext 02008860 s32 cBgS_LineCrossHD(void*, void*)
ext 020084C8 u32 cBgS_GetTriPlaHD(void*, u32, u32)
ext 02312548 void Quaternion_setRotateHD(void*, void*)
ext 025D5834 u32 fopAcM_createHD(s32, u32, void*, s32, void*, void*, s32, void*)
ext 025D5218 u32 fopAcM_SearchHD(void*, void*)
ext 02075988 BOOL Beam_executeHD(daBeam_c*)
*/
