// Gohdan: head/hands, beam effects, resources, collision, and creation.
// HD: expanded lighting/collision layouts and compiler-generated code are
// retained.
#include "d/actor/d_a_bst_base.h"

using namespace bst_move_names;
using namespace bst_base_names;

static bst_class *bstConstructor(bst_class *i_this);

static u16 set_bdl_d[4] = {0x2C, 0x2F,
                           0x31, 0};
static u16 set_za_bdl[4] = {0x30, 0x33,
                            0x34, 0};
static u16 set_bck_d[4] = {0xE, 0x1E,
                           0x27, 0};
static u16 set_btk_d[4] = {0x55, 0x5B,
                           0x63, 0};
static u16 set_brk_d[4] = {0x3A, 0x41,
                           0x49, 0};
static u16 set_za_btk[4] = {0x5E, 0x66,
                            0x69, 0};
static u16 set_za_brk[4] = {0x44, 0x4C,
                            0x4F, 0};
static u16 fly_bck_d[4] = {0xC, 0x1B,
                           0x24, 0};

/* 020E46A0 */
static BOOL message_set(bst_class *i_this, u32 number) {
    u32 address = gabi::ea(i_this);
    if ((u32)(bstBaseGlobals().msg) != 0xFFFFFFFF)
        return FALSE;
    u32 control = (u32)(bstBaseMessagePointer());
    u32 message =
        BstBase_messageSet(bstBaseAddress<void>(control), number, &bstBaseActor(address)->eyePos);
    bstBaseGlobals().msg = message;
    if (message != 0xFFFFFFFF)
        bstBaseGlobals().msg_end = 0;
    return TRUE;
}

/* 020E471C */
static void message_end() {
    u32 control = (u32)(bstBaseMessagePointer());
    if (control && control + 0x14)
        bstBaseAddress<BstBaseMessage>(control)->mSend = 1;
    bstBaseGlobals().msg_end = 1;
}

/* 020E474C */
static void set_hand_AT(bst_class *i_this, u8 enabled) {
    for (u32 i = 0; i < 19; ++i) {
        u32 sphere = gabi::ea(&bstBaseActor(gabi::ea(i_this))->mFingerSphs[i]);
        u32 flags = (u32)(bstBaseAddress<dCcD_GObjInf>(sphere)->mObjAt.mSPrm);
        bstBaseAddress<dCcD_GObjInf>(sphere)->mGObjAt.mSpl = 1;
        bstBaseAddress<dCcD_GObjInf>(sphere)->mObjAt.mSPrm = enabled ? flags | 1 : flags & ~1u;
    }
}

/* 020E4790 */
static void set_hand_CO(bst_class *i_this, u8 enabled) {
    for (u32 i = 0; i < 19; ++i) {
        u32 flagsAddress = gabi::ea(&bstBaseActor(gabi::ea(i_this))->mFingerSphs[i].mObjCo.mSPrm);
        u32 flags = (u32)(*bstBaseCoFlags(flagsAddress));
        *bstBaseCoFlags(flagsAddress) = enabled ? flags | 1 : flags & ~1u;
    }
}

/* 020E7188 */
static BOOL daBst_IsDelete(bst_class *i_this) { return TRUE; }

/* inlined into anm_init, end_brkbtk_set, useHeapInit */
static void *bstResource(s32 index) {
    SafeString name;
    name.mStringTop = gabi::guest_addr("Bst");
    name.__vtbl = gc_tu.safeString;
    u32 control = (u32)(bstMoveResourceTable());
    return BstBase_getResource(bstBaseAddress<void>(control), &name, index);
}

/* 020E47D0 */
static void anm_init(bst_class *i_this, s32 index, f32 morph, u8 loop, f32 speed, s32 sound) {
    u32 address = gabi::ea(i_this);
    void *animation = bstResource(index);
    void *audio = sound >= 0 ? bstResource(sound) : nullptr;
    u32 controller = (u32)(bstBaseActor(address)->m02B8.v);
    f32 first = (f32)(bstMoveConstants().mZero), last = (f32)(bstBaseMinusOne());
    BstBase_setAnimation(bstMoveFrame(controller), animation, loop, morph, speed, first, last,
                         audio);
}

/* 020E48F8 */
static BOOL nodeCallBackHead(void *node, s32 timing) {
    if (timing == 0) {
        void *joint = BstBase_getJoint(node);
        u32 model = (u32)(bstBaseCurrentModel());
        u32 i_this = (u32)(bstMoveModel(model)->mUserArea);
        u16 number = (u16)(reinterpret_cast<BstBaseJoint *>(joint)->mJntNo);
        if (i_this) {
            u32 block = (u32)(bstBasePointerWord(bstMoveModel(model)->mpMtxBuffer));
            u32 matrices = (u32)(bstBasePointerWord(bstMoveMatrixBuffer(block)->mpAnmMtx));
            u16 flags = (u16)(bstMoveMatrixBuffer(block)->mFlags);
            bstMoveMatrixBuffer(block)->mFlags = flags | 0x10;
            u32 current = (u32)(bstMoveCalcMatrix());
            BstBase_PSMTXCopy(&bstBaseAddress<Mtx34>(matrices)[number],
                              bstBaseAddress<Mtx34>(current));
            if (number == 9 || number == 12 || number == 10 || number == 11) {
                s16 angle = (s16)bstBaseActor(i_this)->m2E78[number == 9 || number == 12 ? 1 : 0];
                if (number == 9 || number == 10)
                    angle = (s16)-angle;
                current = (u32)(bstMoveCalcMatrix());
                BstBase_mDoMtx_YrotM(bstBaseAddress<Mtx34>(current), angle);
            }
            block = (u32)(bstBasePointerWord(bstMoveModel(model)->mpMtxBuffer));
            current = (u32)(bstMoveCalcMatrix());
            flags = (u16)(bstMoveMatrixBuffer(block)->mFlags);
            matrices = (u32)(bstBasePointerWord(bstMoveMatrixBuffer(block)->mpAnmMtx));
            bstMoveMatrixBuffer(block)->mFlags = flags | 0x10;
            mtx_copy(&bstBaseAddress<Mtx34>(matrices)[number], bstBaseAddress<Mtx34>(current));
            current = (u32)(bstMoveCalcMatrix());
            BstBase_PSMTXCopy(bstBaseAddress<Mtx34>(current), &bstBaseJ3DMatrix());
        }
    }
    return TRUE;
}

/* 020E4CD0 */
static void pos_move(bst_class *i_this, u8 straight) {
    u32 address = gabi::ea(i_this);
    cXyz vector;
    if (!straight) {
        cXyz difference;
        BstBase_cXyz_subtract(&bstBaseActor(address)->mTargetPos, &difference,
                              &bstBaseActor(address)->current.pos);
        f32 x = difference.x, y = difference.y, z = difference.z;
        vector.x = x;
        vector.y = y;
        vector.z = z;
        s16 yaw = BstBase_cM_atan2s(x, z);
        z = vector.z;
        x = vector.x;
        f32 distance = BstBase_cM_sqrt(gabi::fmadds(x, x, z * z));
        y = vector.y;
        s16 pitch = (s16)-BstBase_cM_atan2s(y, distance);
        f32 scale = (f32)(bstBaseActor(address)->m10EC.y);
        f32 limit = (f32)(bstBaseActor(address)->m10EC.x);
        s16 divisor = (s16)((s16)(bstMoveTuning().mRegS[3]) + 5);
        BstBase_cLib_addCalcAngleS2(&bstBaseActor(address)->current.angle.y, yaw, divisor,
                                    (s16)gabi::ftoi(limit * scale));
        limit = (f32)(bstBaseActor(address)->m10EC.x);
        scale = (f32)(bstBaseActor(address)->m10EC.y);
        divisor = (s16)((s16)(bstMoveTuning().mRegS[3]) + 5);
        BstBase_cLib_addCalcAngleS2(&bstBaseActor(address)->current.angle.x, pitch, divisor,
                                    (s16)gabi::ftoi(limit * scale));
    }
    f32 one = (f32)(bstMoveConstants().mOne);
    f32 step = (f32)(bstMoveConstants().mPositionChaseRatio);
    BstBase_cLib_addCalc(&bstBaseActor(address)->m10EC.y, one, one, step);
    f32 zero = (f32)(bstMoveConstants().mZero);
    vector.y = zero;
    vector.x = zero;
    f32 speed = (f32)(bstBaseActor(address)->speedF);
    u32 matrix = (u32)(bstMoveCalcMatrix());
    vector.z = speed;
    s16 yaw = (s16)(bstBaseActor(address)->current.angle.y);
    BstBase_mDoMtx_YrotS(bstBaseAddress<Mtx34>(matrix), yaw);
    s16 pitch = (s16)(bstBaseActor(address)->current.angle.x);
    matrix = (u32)(bstMoveCalcMatrix());
    BstBase_mDoMtx_XrotM(bstBaseAddress<Mtx34>(matrix), pitch);
    BstBase_MtxPosition(&vector, &bstBaseActor(address)->speed);
    BstBase_PSVECAdd(&bstBaseActor(address)->current.pos, &bstBaseActor(address)->speed,
                     &bstBaseActor(address)->current.pos);
}

/* 020E4E48 */
static BOOL player_way_check(bst_class *i_this) {
    u32 address = gabi::ea(i_this);
    void *play = BstBase_dComIfGp_get();
    u32 player = (u32)(bstBasePointerWord(
        reinterpret_cast<dComIfG_inf_c *>(play)->play.mPlayerInfo[0].mpPlayer));
    s16 angle = (s16)(bstBaseActor(address)->shape_angle.y);
    s16 playerAngle = (s16)(bstBaseActor(player)->shape_angle.y);
    s16 difference = (s16)(angle - playerAngle);
    if (difference < 0)
        difference = (s16)-difference;
    return (u16)difference >= 0x4000;
}

/* 020E7190 */
static BOOL daBst_Delete(bst_class *i_this) {
    u32 address = gabi::ea(i_this);
    BstBase_dComIfG_resDelete(&bstBaseActor(address)->mPhs, "Bst");
    if ((u8)(bstBaseActor(address)->mHioSet)) {
        s8 child = (s8)(bstBaseGlobals().m4);
        bstBaseHioSet() = 0;
        BstBase_hioDelete(child);
    }
    if ((u8)(bstBaseActor(address)->mBstPartType) == Type_HEAD_e) {
        BstBaseID id;
        for (u32 i = 0; i < 2; ++i) {
            u32 value = (u32)(bstBaseActor(address)->m2E90[i]);
            id = value;
            void *attachment = nullptr;
            if (value != 0xFFFFFFFF)
                attachment = BstBase_searchActor(BstBaseFunction::SearchByID, &id);
            if (attachment)
                BstBase_deleteActor(attachment);
        }
    }
    u32 callback = (u32)(bstBaseActor(address)->mPa_smokeEcallBack.__vtbl);
    u32 target = (u32)(bstBaseAddress<BstBaseSmokeVtable>(callback)->mRemove);
    gabi::call(target, &bstBaseActor(address)->mPa_smokeEcallBack);
    for (u32 i = 0; i < 10; ++i)
        BstBase_stopSound(&bstBaseActor(address)->m03B8[i]);
    return TRUE;
}

/* 020E4EA4 */
static void beam_eff_set(cXyz *position, s16 direction, u8 oriented) {
    csXyz angles;
    if (oriented)
        BstBase_setAngles(&angles, (s16)-0x4000, direction, (s16)0);
    for (u32 i = 0; i < 3; ++i) {
        void *play = BstBase_dComIfGp_get();
        u32 particles =
            (u32)(bstBasePointerWord(reinterpret_cast<dComIfG_inf_c *>(play)->play.mParticle));
        u32 effect = i == 0 ? 0x81BF : 0xA1BF + i;
        BstBase_dPa_control_set(bstBaseAddress<void>(particles), 0, effect, position,
                                oriented ? &angles : nullptr, nullptr, 255, nullptr, -1, 0, nullptr,
                                nullptr);
    }
}

/* 020E5068 */
static void end_brkbtk_set(bst_class *i_this) {
    const s32 brkMain[3] = {dRes_INDEX_BST_BRK_LH_END_e, dRes_INDEX_BST_BRK_RH_END_e,
                            dRes_INDEX_BST_BRK_BST_END_e},
              btkMain[3] = {dRes_INDEX_BST_BTK_LH_END_e, dRes_INDEX_BST_BTK_RH_END_e,
                            dRes_INDEX_BST_BTK_BST_END_e};
    const s32 brkPedestal[3] = {dRes_INDEX_BST_BRK_UDEL_END_e, dRes_INDEX_BST_BRK_UDER_END_e,
                                dRes_INDEX_BST_BRK_MUNE_END_e},
              btkPedestal[3] = {dRes_INDEX_BST_BTK_UDEL_END_e, dRes_INDEX_BST_BTK_UDER_END_e,
                                dRes_INDEX_BST_BTK_MUNE_END_e};
    f32 speed = 0;
    for (u32 pedestal = 0; pedestal < 2; ++pedestal) {
        for (u32 part = 0; part < 3; ++part) {
            for (u32 texture = 0; texture < 2; ++texture) {
                u32 boss = (u32)bstBasePart(part);
                u32 model;
                if (pedestal)
                    model = (u32)(bstBaseActor(boss)->m02C8.v);
                else {
                    u32 morph = (u32)(bstBaseActor(boss)->m02B8.v);
                    model = (u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel));
                }
                s32 index = pedestal ? (texture ? btkPedestal[part] : brkPedestal[part])
                                     : (texture ? btkMain[part] : brkMain[part]);
                void *animation = bstResource(index);
                // The first BRK setup loads its model data before the speed constant;
                // subsequent BTK setups fetch the actor before reading model data.
                u32 data;
                if (!texture)
                    data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
                if (!pedestal && !part && !texture)
                    speed = (f32)(bstMoveConstants().mOne);
                boss = (u32)bstBasePart(part);
                if (texture)
                    data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
                u32 controller = (u32)(pedestal ? (texture ? bstBaseActor(boss)->mpBtkAnm.v
                                                           : bstBaseActor(boss)->mpBrkAnm.v)
                                                : (texture ? bstBaseActor(boss)->mpTexMtxAnimator.v
                                                           : bstBaseActor(boss)->mpTevRegAnimator.v));
                (texture ? BstBase_initBtk : BstBase_initBrk)(bstMoveFrame(controller),
                                                              bstBaseAddress<J3DModelData>(data),
                                                              animation, 1, 0, 0, -1, 1, speed, 0);
            }
        }
    }
}

static void bstCopyTevDefaults(BstBaseLight *light, const BstBaseDefaults &defaults) {
    for (u32 i = 0; i < 6; ++i)
        light->mPositionDirection[i] = defaults.initial[i];
    for (u32 i = 0; i < 4; ++i)
        light->mColor[i] = defaults.flags[i];
    for (u32 i = 0; i < 4; ++i)
        light->mShortParams[i] = defaults.angles[i];
    for (u32 i = 0; i < 8; ++i)
        light->mFloatParams[i] = defaults.tail[i];
}

static void bstEmitterMatrix(u32 address, u32 emitter) {
    u32 morph = (u32)(bstBaseActor(address)->m02B8.v);
    u32 model = (u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel));
    u32 block = (u32)(bstBasePointerWord(bstMoveModel(model)->mpMtxBuffer));
    u16 flags = (u16)(bstMoveMatrixBuffer(block)->mFlags);
    u32 matrix = (u32)(bstBasePointerWord(bstMoveMatrixBuffer(block)->mpAnmMtx));
    bstMoveMatrixBuffer(block)->mFlags = flags | 0x10;
    BstBase_emitterMatrix(bstBaseAddress<Mtx34>(matrix),
                          &bstEmitter(bstBaseAddress<JPABaseEmitter>(emitter))->mGlobalRotation,
                          &bstEmitter(bstBaseAddress<JPABaseEmitter>(emitter))->mGlobalTranslation);
}

/* 020EB174 */
static void end_demo(bst_class *i_this) {
    u32 address = gabi::ea(i_this);
    bstBaseActor(address)->attention_info.flags = 0;
    s16 mode = (s16)(bstBaseActor(address)->mDamage);
    bstBaseActor(address)->mState = 10;
    f32 speed = (f32)(bstMoveConstants().mOne);
    switch (mode) {
    case 0:
        bstBaseActor(address)->mDamage = (s16)(mode + 1);
        [[fallthrough]];
    case 1: {
        u32 morph = (u32)(bstBaseActor(address)->m02B8.v);
        f32 frame = (f32)(bstMoveMorf(morph)->mFrameCtrl.mFrame);
        if (gabi::ftoi(frame) == 20 && gabi::ea(&bstBaseActor(address)->eyePos)) {
            s8 room = (s8)(bstBaseActor(address)->current.roomNo);
            s8 reverb = BstBase_reverb(room);
            BstBase_mDoAud_seStart(JA_SE_CM_BST_JUMP_L, &bstBaseActor(address)->eyePos, 0, reverb);
        }
        break;
    }
    case 2: {
        anm_init(i_this, dRes_INDEX_BST_BCK_BST_HIRAKU_e, speed, 0, speed, -1);
        s16 value = (s16)(bstBaseActor(address)->mDamage);
        bstBaseActor(address)->mDamage = (s16)(value + 1);
        void *play = BstBase_dComIfGp_get();
        u32 particles =
            (u32)(bstBasePointerWord(reinterpret_cast<dComIfG_inf_c *>(play)->play.mParticle));
        u32 emitter = BstBase_dPa_control_set(bstBaseAddress<void>(particles), 0, 0x81E8,
                                              &bstBaseActor(address)->current.pos, nullptr, nullptr,
                                              255, nullptr, -1, 0, nullptr, nullptr);
        bstBaseActor(address)->m2EF4.v = emitter;
        if (emitter)
            bstEmitterMatrix(address, emitter);
        break;
    }
    case 10: {
        u8 part = (u8)(bstBaseActor(address)->mBstPartType);
        s32 index = (u16)(part ? set_bck_d[part] : fly_bck_d[0]);
        f32 morph = (f32)(bstMoveConstants().mTwo);
        anm_init(i_this, index, morph, part ? 0 : 2, speed, -1);
        s16 value = (s16)(bstBaseActor(address)->mDamage);
        bstBaseActor(address)->mDamage = (s16)(value + 1);
        [[fallthrough]];
    }
    case 11: {
        f32 factor = (f32)(bstBaseChaseRatio());
        f32 five = (f32)(bstMoveConstants().mFive);
        for (u32 i = 0; i < 3; ++i) {
            f32 limit = (f32)(bstMoveTuning().mRegF[9]) + five;
            f32 target = (f32)(*bstBaseAxis(&bstBaseActor(address)->home.pos, i));
            BstBase_cLib_addCalc(bstBaseAxis(&bstBaseActor(address)->current.pos, i), target,
                                 factor, limit);
        }
        s16 angle = (s16)bstBaseActor(address)->home.angle.y;
        BstBase_cLib_addCalcAngleS2(&bstBaseActor(address)->shape_angle.y, angle, 10, 128);
        angle = (s16)bstBaseActor(address)->home.angle.x;
        BstBase_cLib_addCalcAngleS2(&bstBaseActor(address)->shape_angle.x, angle, 10, 128);
        angle = (s16)bstBaseActor(address)->home.angle.z;
        BstBase_cLib_addCalcAngleS2(&bstBaseActor(address)->shape_angle.z, angle, 10, 128);
        break;
    }
    }
    if ((u8)(bstBaseActor(address)->mBstPartType) == Type_HEAD_e) {
        for (u32 i = 0; i < 4; ++i) {
            u32 emitter = (u32)(bstBaseActor(address)->m2EE0[i + 1].v);
            if (!emitter)
                continue;
            if ((s16)(bstBaseActor(address)->m10FC[3]) == 1) {
                bstEmitter(bstBaseAddress<JPABaseEmitter>(emitter))->becomeInvalidEmitter();
                bstBaseActor(address)->m2EE0[i + 1].v = 0;
            } else
                bstEmitterMatrix(address, emitter);
        }
    }
}

/* 020EDC8C */
static void beam_move(bst_class *i_this) {
    u32 address = gabi::ea(i_this);
    BstBaseGround ground;

    dBgS_GndChk_ct(&ground, BstBaseGroundVtable, false);
    f32 zero = (f32)(bstMoveConstants().mZero);
    f32 above = (f32)(bstMoveConstants().mFiveHundred);
    f32 one = (f32)(bstMoveConstants().mOne);
    f32 half = (f32)(bstMoveConstants().mHalf);
    f32 five = (f32)(bstMoveConstants().mFive);
    f32 fifty = (f32)(bstMoveConstants().mFifty);
    f32 ceiling = (f32)(bstMoveConstants().mTwoThousand);
    for (u32 i = 0; i < 10; ++i) {
        if ((s8)(bstBaseActor(address)->m04E4[i]) == 0)
            continue;
        u32 position = gabi::ea(&bstBaseActor(address)->m03B8[i]);
        u32 velocity = gabi::ea(&bstBaseActor(address)->m0430[i]);
        BstBase_PSVECAdd(bstBaseAddress<cXyz>(position), bstBaseAddress<cXyz>(velocity),
                         bstBaseAddress<cXyz>(position));
        f32 z = (f32)(bstBaseAddress<cXyz>(position)->z),
            y = (f32)(bstBaseAddress<cXyz>(position)->y),
            x = (f32)(bstBaseAddress<cXyz>(position)->x);
        u32 model = (u32)(bstBaseActor(address)->m0390[i].v);
        BstBase_matrixTranslate(&bstBaseModelMatrix(), x, y, z);
        s16 yaw = (s16)(bstBaseActor(address)->m04A8[i].y);
        BstBase_mDoMtx_YrotM(&bstBaseModelMatrix(), yaw);
        s16 pitch = (s16)(bstBaseActor(address)->m04A8[i].x);
        BstBase_mDoMtx_XrotM(&bstBaseModelMatrix(), pitch);
        f32 length = (f32)(bstBaseActor(address)->m10A8[i]);
        BstBase_mDoMtx_stack_scaleM(one, one, length);
        mtx_copy(&bstMoveModel(model)->mBaseTransformMtx, &bstBaseModelMatrix());
        f32 target = (f32)(bstMoveTuning().mRegF[4]) + five;
        f32 step = (f32)(bstMoveTuning().mRegF[5]) + half;
        BstBase_cLib_addCalc(&bstBaseActor(address)->m10A8[i], target, one, step);
        y = (f32)(bstBaseAddress<cXyz>(position)->y);
        z = (f32)(bstBaseAddress<cXyz>(position)->z);
        x = (f32)(bstBaseAddress<cXyz>(position)->x);
        ground.m_pos.z = z;
        ground.m_pos.x = x;
        ground.m_pos.y = y + above;
        bool hit = false;
        void *play = BstBase_dComIfGp_get();
        f32 height =
            BstBase_dBgS_GroundCross(&reinterpret_cast<dComIfG_inf_c *>(play)->play.mBgS, &ground);
        y = (f32)(bstBaseAddress<cXyz>(position)->y);
        if (!(y > height)) {
            bstBaseAddress<cXyz>(position)->y = height;
            beam_eff_set(bstBaseAddress<cXyz>(position), 0, 0);
            hit = true;
        } else {
            BstBaseLine line;

            dBgS_LinChk_ct(&line, BstBaseLineVtable, false);
            cXyz difference;
            cXyz start;
            cXyz end;
            BstBase_cXyz_subtract(bstBaseAddress<cXyz>(position), &difference,
                                  bstBaseAddress<cXyz>(velocity));
            start.x = (f32)difference.x;
            start.z = (f32)difference.z;
            f32 elevated = (f32)difference.y + fifty;
            start.y = elevated;
            end.copy(*bstBaseAddress<cXyz>(position));
            end.y = elevated;
            BstBase_lineSet(&line, &start, &end, nullptr);
            play = BstBase_dComIfGp_get();
            BOOL crosses =
                BstBase_dBgS_LineCross(&reinterpret_cast<dComIfG_inf_c *>(play)->play.mBgS, &line);
            if (crosses) {
                bstBaseAddress<cXyz>(position)->copy(*&line.mLin.mEnd);
            }
            line.dBgS_PolyPassChk::__vtbl = BstBaseVtable::LinePoly;
            line.dBgS_GrpPassChk::__vtbl = BstBaseVtable::Group;
            line.cBgS_PolyInfo::__vtbl = BstBaseVtable::LineInfo;
            BstBase_lineDtor(&line, 0);
            if (crosses) {
                yaw = (s16)(bstBaseActor(address)->m04A8[i].y);
                beam_eff_set(bstBaseAddress<cXyz>(position), yaw, 1);
                hit = true;
            }
        }
        y = (f32)(bstBaseAddress<cXyz>(position)->y);
        if (y > ceiling)
            bstBaseActor(address)->m04E4[i] = 0;
        if (hit) {
            bstBaseActor(address)->m04E4[i] = 0;
            play = BstBase_dComIfGp_get();
            s32 strength = (s16)(bstMoveTuning().mRegS[2]) + 3;
            cXyz impulse;
            impulse.z = zero;
            impulse.y = one;
            impulse.x = zero;
            BstBase_shock(&reinterpret_cast<dComIfG_inf_c *>(play)->play.mVibration, strength, -33,
                          &impulse);
            s8 room = (s8)(bstBaseActor(address)->current.roomNo);
            s8 reverb = BstBase_reverb(room);
            BstBase_mDoAud_seStart(JA_SE_CM_BST_BEAM_BOMB, bstBaseAddress<cXyz>(position), 0,
                                   reverb);
        }
        u32 sphere = gabi::ea(&bstBaseActor(address)->mCcD_beams[i]);
        if ((s8)(bstBaseActor(address)->m04E4[i]) == 1) {
            BstBase_sphereStart(bstBaseAddress<dCcD_Sph>(sphere), bstBaseAddress<cXyz>(position));
            u8 state = (u8)(bstBaseActor(address)->m04E4[i]);
            bstBaseActor(address)->m04E4[i] = (u8)(state + 1);
        } else
            BstBase_sphereMove(bstBaseAddress<dCcD_Sph>(sphere), bstBaseAddress<cXyz>(position));
        play = BstBase_dComIfGp_get();
        BstBase_collisionSet(&reinterpret_cast<dComIfG_inf_c *>(play)->play.mCcS,
                             bstBaseAddress<dCcD_Sph>(sphere));
    }
    ground.dBgS_PolyPassChk::__vtbl = BstBaseVtable::GroundPoly;
    ground.dBgS_GrpPassChk::__vtbl = BstBaseVtable::Group;
    ground.cBgS_PolyInfo::__vtbl = BstBaseVtable::GroundInfo;
    BstBase_groundDtor(&ground, 0);
}

/* 020E4A64 */
static BOOL daBst_Draw(bst_class *i_this) {
    u32 address = gabi::ea(i_this);
    if ((u8)(bstBaseActor(address)->mBstPartType)) {
        u32 boss = (u32)(bstBaseGlobals().boss);
        s8 state = (s8)(bstBaseActor(boss)->m2E9A);
        if ((u32)((s32)state - 1) < 9)
            return TRUE;
    }
    u32 morph = (u32)(bstBaseActor(address)->m02B8.v);
    u32 model = (u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel));
    void *env = BstBase_dKy_getEnvlight();
    BstBase_tevSetup(env, 0, &bstBaseActor(address)->current.pos, &bstBaseActor(address)->tevStr);
    env = BstBase_dKy_getEnvlight();
    BstBase_tevApply(env, bstMoveModel(model), &bstBaseActor(address)->tevStr);
    u32 animator = (u32)(bstBaseActor(address)->mpTexMtxAnimator.v);
    u32 data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
    f32 frame = (f32)(bstMoveFrame(animator)->mFrame);
    BstBase_btkEntry(bstMoveFrame(animator), bstBaseAddress<J3DModelData>(data), frame);
    animator = (u32)(bstBaseActor(address)->mpTevRegAnimator.v);
    data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
    frame = (f32)(bstMoveFrame(animator)->mFrame);
    BstBase_brkEntry(bstMoveFrame(animator), bstBaseAddress<J3DModelData>(data), frame);
    morph = (u32)(bstBaseActor(address)->m02B8.v);
    BstBase_modelEntry(bstMoveMorf(morph));
    data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
    bstBaseAddress<BstBaseModelData>(data)->mpTextureMatrixAnimator = 0;
    data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
    bstBaseAddress<BstBaseModelData>(data)->mpTevAnimator = 0;
    model = (u32)(bstBaseActor(address)->m02C8.v);
    animator = (u32)(bstBaseActor(address)->mpBtkAnm.v);
    data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
    frame = (f32)(bstMoveFrame(animator)->mFrame);
    BstBase_btkEntry(bstMoveFrame(animator), bstBaseAddress<J3DModelData>(data), frame);
    animator = (u32)(bstBaseActor(address)->mpBrkAnm.v);
    frame = (f32)(bstMoveFrame(animator)->mFrame);
    data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
    BstBase_brkEntry(bstMoveFrame(animator), bstBaseAddress<J3DModelData>(data), frame);
    env = BstBase_dKy_getEnvlight();
    BstBase_tevSetup(env, 0, &bstBaseActor(address)->home.pos, &bstBaseActor(address)->mEnvLight);
    env = BstBase_dKy_getEnvlight();
    BstBase_tevApply(env, bstMoveModel(model), &bstBaseActor(address)->mEnvLight);
    BstBase_updateModel(bstMoveModel(model), 0);
    if ((u8)(bstBaseActor(address)->mBstPartType) == Type_HEAD_e) {
        s16 blur = (s16)(bstBaseActor(address)->m2E98);
        if (blur > 1) {
            bstBaseBlur().mRate = (u8)blur;
            BstBase_blurOn();
        } else if (blur == 1) {
            bstBaseActor(address)->m2E98 = 0;
            bstBaseBlur().mEnabled = 0;
        }
        for (u32 i = 0; i < 10; ++i)
            if ((s8)(bstBaseActor(address)->m04E4[i])) {
                u32 beam = (u32)(bstBaseActor(address)->m0390[i].v);
                BstBase_updateModel(bstMoveModel(beam), 0);
            }
        if ((u32)(bstBaseGlobals().mCenterGuard) == 0) {
            f32 zero = (f32)(bstMoveConstants().mZero);
            bstBaseGlobals().mCenterGuard = 1;
            bstBaseGlobals().center_pos.x = zero;
            bstBaseGlobals().center_pos.z = zero;
            bstBaseGlobals().center_pos.y = zero;
        }
        env = BstBase_dKy_getEnvlight();
        BstBase_tevSetup(env, 1, &bstBaseGlobals().center_pos, &bstBaseActor(address)->m2F20);
        env = BstBase_dKy_getEnvlight();
        model = (u32)(bstBaseActor(address)->m2FDC.v);
        BstBase_tevApply(env, bstMoveModel(model), &bstBaseActor(address)->m2F20);
        model = (u32)(bstBaseActor(address)->m2FDC.v);
        animator = (u32)(bstBaseActor(address)->m2FE0.v);
        data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
        frame = (f32)(bstMoveFrame(animator)->mFrame);
        BstBase_brkEntry(bstMoveFrame(animator), bstBaseAddress<J3DModelData>(data), frame);
        model = (u32)(bstBaseActor(address)->m2FDC.v);
        BstBase_updateModel(bstMoveModel(model), 0);
        env = BstBase_dKy_getEnvlight();
        model = (u32)(bstBaseActor(address)->m2FD0.v);
        BstBase_tevApply(env, bstMoveModel(model), &bstBaseActor(address)->m2F20);
        model = (u32)(bstBaseActor(address)->m2FD0.v);
        animator = (u32)(bstBaseActor(address)->m2FD8.v);
        data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
        frame = (f32)(bstMoveFrame(animator)->mFrame);
        BstBase_brkEntry(bstMoveFrame(animator), bstBaseAddress<J3DModelData>(data), frame);
        model = (u32)(bstBaseActor(address)->m2FD0.v);
        animator = (u32)(bstBaseActor(address)->m2FD4.v);
        data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
        frame = (f32)(bstMoveFrame(animator)->mFrame);
        BstBase_btkEntry(bstMoveFrame(animator), bstBaseAddress<J3DModelData>(data), frame);
        model = (u32)(bstBaseActor(address)->m2FD0.v);
        BstBase_updateModel(bstMoveModel(model), 0);
    }
    f32 one = (f32)(bstMoveConstants().mOne);
    BstBase_dSnap_RegistFig(201, i_this, one, one, one);
    return TRUE;
}

static u32 bstNewAnimator(bool texture) {
    void *result = BstBase_alloc(texture ? 0x74 : 0x78);
    if (result)
        result = (texture ? BstBase_btkCtor : BstBase_brkCtor)(result);
    return gabi::ea(result);
}

static BOOL bstInitAnimator(be<u32> *slot, u32 model, void *resource, bool texture, f32 speed,
                            s32 loop = 0, s32 mode = 0) {
    u32 controller = (u32)*slot;
    return (texture ? BstBase_initBtk : BstBase_initBrk)(
        bstMoveFrame(controller), bstMoveModel(model), resource, 1, loop, 0, -1, mode, speed, 0);
}

/* 020E7278 */
static BOOL useHeapInit(bst_class *i_this) {
    u32 address = gabi::ea(i_this);
    u8 part = (u8)(bstBaseActor(address)->mBstPartType);
    s32 index = (u16)set_bdl_d[part];
    void *modelData = bstResource(index);
    part = (u8)(bstBaseActor(address)->mBstPartType);
    index = (u16)set_bck_d[part];
    void *animation = bstResource(index);
    f32 speed = (f32)(bstMoveConstants().mOne);
    u32 morph = BstBase_morphCtor(nullptr, modelData, nullptr, nullptr, animation, 2, 0, -1, 1,
                                  nullptr, 0, 0x11020203, speed);
    bstBaseActor(address)->m02B8.v = morph;
    if (!morph)
        return FALSE;
    if (!(u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel)))
        return FALSE;
    part = (u8)(bstBaseActor(address)->mBstPartType);
    if (part == Type_HEAD_e) {
        u32 model = (u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel));
        u32 data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
        void *joints = BstBase_getJoints(bstBaseAddress<J3DModelData>(data));
        u16 count = (u16)(reinterpret_cast<BstBaseJoint *>(joints)->mCount);
        for (u16 i = 0; i < count; ++i) {
            if ((u32)(i - 9u) < 4) {
                morph = (u32)(bstBaseActor(address)->m02B8.v);
                model = (u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel));
                data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
                u32 limit = (u32)(bstBaseAddress<BstBaseModelData>(data)->mJointCount);
                u32 joint = (u32)(bstBaseAddress<BstBaseModelData>(data)->mpJoints);
                if (i < limit)
                    joint += i * 0x1C;
                bstBaseAddress<BstBaseJointNode>(joint)->mCallback =
                    gabi::guest_fn(reinterpret_cast<const void *>(nodeCallBackHead));
            }
            morph = (u32)(bstBaseActor(address)->m02B8.v);
            model = (u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel));
            data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
            joints = BstBase_getJoints(bstBaseAddress<J3DModelData>(data));
            count = (u16)(reinterpret_cast<BstBaseJoint *>(joints)->mCount);
        }
        morph = (u32)(bstBaseActor(address)->m02B8.v);
        model = (u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel));
        bstMoveModel(model)->mUserArea = address;
        morph = (u32)(bstBaseActor(address)->m02B8.v);
    }
    u32 model = (u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel));
    u32 data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
    u32 created = BstBase_createModel(bstBaseAddress<J3DModelData>(data), 0, 0x11020203);
    bstBaseActor(address)->m0388.v = created;
    if (!created)
        return FALSE;
    u32 controller = bstNewAnimator(true);
    bstBaseActor(address)->mpTexMtxAnimator.v = controller;
    if (!controller)
        return FALSE;
    part = (u8)(bstBaseActor(address)->mBstPartType);
    morph = (u32)(bstBaseActor(address)->m02B8.v);
    index = (u16)set_btk_d[part];
    model = (u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel));
    animation = bstResource(index);
    controller = (u32)(bstBaseActor(address)->mpTexMtxAnimator.v);
    data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
    if (!BstBase_initBtk(bstMoveFrame(controller), bstBaseAddress<J3DModelData>(data), animation, 1,
                         0, 0, -1, 0, speed, 0))
        return FALSE;
    controller = bstNewAnimator(false);
    bstBaseActor(address)->mpTevRegAnimator.v = controller;
    if (!controller)
        return FALSE;
    part = (u8)(bstBaseActor(address)->mBstPartType);
    morph = (u32)(bstBaseActor(address)->m02B8.v);
    index = (u16)set_brk_d[part];
    model = (u32)(bstBasePointerWord(bstMoveMorf(morph)->mpModel));
    animation = bstResource(index);
    controller = (u32)(bstBaseActor(address)->mpTevRegAnimator.v);
    data = (u32)(bstBasePointerWord(bstMoveModel(model)->mModelData));
    if (!BstBase_initBrk(bstMoveFrame(controller), bstBaseAddress<J3DModelData>(data), animation, 1,
                         0, 0, -1, 0, speed, 0))
        return FALSE;
    if ((u8)(bstBaseActor(address)->mBstPartType) == Type_HEAD_e) {
        modelData = bstResource(dRes_INDEX_BST_BDL_TAMA_e);
        for (u32 i = 0; i < 10; ++i) {
            created = BstBase_createModel(modelData, 0, 0x11020203);
            bstBaseActor(address)->m0390[i].v = created;
            if (!created)
                return FALSE;
        }
        modelData = bstResource(dRes_INDEX_BST_BDL_HBSITA1_e);
        created = BstBase_createModel(modelData, 0, 0x11020203);
        bstBaseActor(address)->m2FD0.v = created;
        if (!created)
            return FALSE;
        controller = bstNewAnimator(true);
        bstBaseActor(address)->m2FD4.v = controller;
        if (!controller)
            return FALSE;
        animation = bstResource(dRes_INDEX_BST_BTK_HBSITA1_e);
        if (!bstInitAnimator(&bstBaseActor(address)->m2FD4.v, gabi::ea(modelData), animation, true,
                             speed, 2))
            return FALSE;
        controller = bstNewAnimator(false);
        bstBaseActor(address)->m2FD8.v = controller;
        if (!controller)
            return FALSE;
        animation = bstResource(dRes_INDEX_BST_BRK_HBSITA1_e);
        f32 zero = (f32)(bstMoveConstants().mZero);
        if (!bstInitAnimator(&bstBaseActor(address)->m2FD8.v, gabi::ea(modelData), animation, false,
                             zero))
            return FALSE;
        modelData = bstResource(dRes_INDEX_BST_BDL_HBYOKO1_e);
        created = BstBase_createModel(modelData, 0, 0x11020203);
        bstBaseActor(address)->m2FDC.v = created;
        if (!created)
            return FALSE;
        controller = bstNewAnimator(false);
        bstBaseActor(address)->m2FE0.v = controller;
        if (!controller)
            return FALSE;
        animation = bstResource(dRes_INDEX_BST_BRK_HBYOKO1_e);
        if (!bstInitAnimator(&bstBaseActor(address)->m2FE0.v, gabi::ea(modelData), animation, false,
                             zero, 2))
            return FALSE;
    }
    part = (u8)(bstBaseActor(address)->mBstPartType);
    index = (u16)set_za_bdl[part];
    modelData = bstResource(index);
    created = BstBase_createModel(modelData, 0, 0x11020203);
    bstBaseActor(address)->m02C8.v = created;
    if (!created)
        return FALSE;
    controller = bstNewAnimator(true);
    bstBaseActor(address)->mpBtkAnm.v = controller;
    if (!controller)
        return FALSE;
    part = (u8)(bstBaseActor(address)->mBstPartType);
    index = (u16)set_za_btk[part];
    animation = bstResource(index);
    if (!bstInitAnimator(&bstBaseActor(address)->mpBtkAnm.v, gabi::ea(modelData), animation, true,
                         speed))
        return FALSE;
    controller = bstNewAnimator(false);
    bstBaseActor(address)->mpBrkAnm.v = controller;
    if (!controller)
        return FALSE;
    part = (u8)(bstBaseActor(address)->mBstPartType);
    index = (u16)set_za_brk[part];
    animation = bstResource(index);
    return bstInitAnimator(&bstBaseActor(address)->mpBrkAnm.v, gabi::ea(modelData), animation, false,
                           speed)
               ? TRUE
               : FALSE;
}

static void bstDamageSound(u32 address, u32 sound, bool testActor) {
    if ((!testActor || address) && gabi::ea(&bstBaseActor(address)->eyePos)) {
        s8 room = (s8)(bstBaseActor(address)->current.roomNo);
        s8 reverb = BstBase_reverb(room);
        BstBase_mDoAud_seStart(sound, &bstBaseActor(address)->eyePos, 0, reverb);
    }
}

/* inlined into damage_check */
static void bstDamageRecoil(u32 address, u32 player, s16 duration, f32 seventy, f32 zero,
                            bool head) {
    f32 height = (f32)(bstMoveTuning().mRegF[11]) + seventy;
    bstBaseActor(address)->mHeadHurtSpeed = height;
    s16 timer = (s16)(bstMoveTuning().mRegS[2]);
    bstBaseActor(address)->mHeadHurtAngle = (s16)(timer + duration);
    cXyz difference;
    BstBase_cXyz_subtract(&bstBaseActor(address)->current.pos, &difference,
                          &bstBaseActor(player)->current.pos);
    f32 x = difference.x, z = difference.z;
    s16 yaw = BstBase_cM_atan2s(x, z);
    z = difference.z;
    x = difference.x;
    f32 square = gabi::fmadds(x, x, z * z);
    bstBaseActor(address)->mHurtRecoilAngle1 = yaw;
    f32 distance = BstBase_cM_sqrt(square);
    f32 y = difference.y;
    s16 pitch = (s16)-BstBase_cM_atan2s(y, distance);
    if (head) {
        bstBaseActor(address)->mHurtRecoilAngle2 = pitch;
        bstBaseActor(address)->speedF = zero;
    } else {
        bstBaseActor(address)->speedF = zero;
        bstBaseActor(address)->mHurtRecoilAngle2 = pitch;
    }
}

/* inlined into damage_check */
static void bstDamageParticles(bst_class *i_this, u32 impact, f32 two) {
    void *play = BstBase_dComIfGp_get();
    u32 control =
        (u32)(bstBasePointerWord(reinterpret_cast<dComIfG_inf_c *>(play)->play.mParticle));
    BstBase_dPa_control_set(bstBaseAddress<void>(control), 0, 16, bstBaseAddress<cXyz>(impact),
                            nullptr, nullptr, 255, nullptr, -1, 0, nullptr, nullptr);
    cXyz scale;
    csXyz angle;
    scale.x = two;
    scale.y = two;
    scale.z = two;
    angle.x = 0;
    angle.z = 0;
    play = BstBase_dComIfGp_get();
    u32 player = (u32)(bstBasePointerWord(
        reinterpret_cast<dComIfG_inf_c *>(play)->play.mPlayerInfo[0].mpPlayer));
    angle.y = BstBase_playerAngle(i_this, bstBaseAddress<void>(player));
    play = BstBase_dComIfGp_get();
    control = (u32)(bstBasePointerWord(reinterpret_cast<dComIfG_inf_c *>(play)->play.mParticle));
    BstBase_dPa_control_set(bstBaseAddress<void>(control), 0, 13, bstBaseAddress<cXyz>(impact),
                            &angle, &scale, 255, nullptr, -1, 0, nullptr, nullptr);
    cXyz flash;
    flash.x = (f32)(bstBaseAddress<cXyz>(impact)->x);
    flash.y = (f32)(bstBaseAddress<cXyz>(impact)->y);
    flash.z = (f32)(bstBaseAddress<cXyz>(impact)->z);
    BstBase_hitFlash(&flash, 1);
}

/* 020EA6B4 */
static void damage_check(bst_class *i_this) {
    u32 address = gabi::ea(i_this);
    void *play = BstBase_dComIfGp_get();
    u32 player = (u32)(bstBasePointerWord(
        reinterpret_cast<dComIfG_inf_c *>(play)->play.mPlayerInfo[0].mpPlayer));
    BstBase_statusMove(&bstBaseActor(address)->mStts.__vtbl_gstts);
    BstBaseHitInfo hitInfo; /* CcAtInfo 0x1C: cc_at_check 025192A8 writes +0x18
                             (stack guard sweep 2026-10-05) */
    hitInfo.mWord[5] = 0;
    if ((s16)(bstBaseActor(address)->mDamageTimer) == 0) {
        bool found = false;
        u32 target = 0;
        if ((u8)(bstBaseActor(address)->mBstPartType) == Type_HEAD_e) {
            if (BstBase_checkTargetHit(&bstBaseActor(address)->mHeadHurtCyl)) {
                target = BstBase_getTargetHit(&bstBaseActor(address)->mHeadHurtCyl);
                found = true;
            }
        } else {
            for (u32 i = 0; i < 19; ++i) {
                u32 sphere = gabi::ea(&bstBaseActor(address)->mFingerSphs[i]);
                if (BstBase_checkTargetHit(bstBaseAddress<dCcD_Sph>(sphere))) {
                    target = BstBase_getTargetHit(bstBaseAddress<dCcD_Sph>(sphere));
                    found = true;
                }
            }
        }
        if (found) {
            hitInfo.mWord[0] = target;
            BstBase_hitSound(i_this, bstBaseAddress<void>(target), 0x42);
            bstBaseActor(address)->mDamageTimer = 7;
        }
    }
    if ((s16)(bstBaseActor(address)->mState))
        return;
    f32 seventy = (f32)(bstBaseSeventy());
    u8 part = (u8)(bstBaseActor(address)->mBstPartType);
    f32 two = (f32)(bstMoveConstants().mTwo), zero = (f32)(bstMoveConstants().mZero);
    if (part == Type_HEAD_e) {

        BOOL hit = BstBase_checkTargetHit(&bstBaseActor(address)->mEyeSphs[0]);
        if (!hit && !BstBase_checkTargetHit(&bstBaseActor(address)->mEyeSphs[1]))
            return;
        bstBaseActor(address)->mState = 10;
        if (!player_way_check(i_this))
            return;
        u32 eye = 0, target;
        if (BstBase_checkTargetHit(&bstBaseActor(address)->mEyeSphs[0]))
            target = BstBase_getTargetHit(&bstBaseActor(address)->mEyeSphs[0]);
        else {
            target = BstBase_getTargetHit(&bstBaseActor(address)->mEyeSphs[1]);
            eye = 1;
        }
        hitInfo.mWord[0] = target;
        if ((s8)(bstBaseActor(address)->mRoomState) != 1)
            bstBaseActor(address)->mRoomState = 8;
        bstBaseActor(address)->mDamage = 0;
        bstBaseActor(address)->mActionType = ACTION_DAMAGE_e;
        bstDamageSound(address, JA_SE_CM_BST_EYE_HIT, true);
        s8 health = (s8)((s8)(bstBaseActor(address)->mEyeHealth[eye]) - 1);
        bstBaseActor(address)->mEyeHealth[eye] = health;
        if (health <= 0) {
            if ((s8)(bstBaseActor(address)->mEyeHealth[1 - eye]) <= 0) {
                bstBaseActor(address)->mDamage = 0;
                bstBaseActor(address)->mActionType = ACTION_HEAD_DAMAGE_e;
            }
            bstDamageSound(address, 0x6995, true);
        }
        bstDamageRecoil(address, player, 7, seventy, zero, true);
        bstDamageParticles(i_this, gabi::ea(&bstBaseActor(address)->mEyeSphs[eye].mGObjTg.mHitPos),
                           two);
    } else {
        if (!BstBase_checkTargetHit(&bstBaseActor(address)->mHandHurtCyl))
            return;
        bstBaseActor(address)->mState = 10;
        if (!player_way_check(i_this))
            return;
        u32 target = BstBase_getTargetHit(&bstBaseActor(address)->mHandHurtCyl);
        hitInfo.mWord[0] = target;
        hitInfo.mWord[5] = gabi::ea(&bstBaseActor(address)->mHandHurtCyl.mGObjTg.mHitPos);
        u32 result = BstBase_attackCheck(i_this, &hitInfo);
        hitInfo.mWord[1] = result;
        bstDamageSound(address, JA_SE_CM_BST_EYE_HIT, true);
        if ((s8)(bstBaseActor(address)->health) <= 0)
            bstDamageSound(address, 0x6995, false);
        bstDamageRecoil(address, player, 15, seventy, zero, false);
        bstDamageParticles(i_this, gabi::ea(&bstBaseActor(address)->mHandHurtCyl.mGObjTg.mHitPos),
                           two);
        part = (u8)(bstBaseActor(address)->mBstPartType);
        u32 other = 2u - part;
        bstBaseActor(address)->mActionType = ACTION_DAMAGE_e;
        bstBaseActor(address)->mDamage = 0;
        if (other < 2) {
            u32 hand = (u32)bstBaseGlobals().hand[other];
            if ((s16)(bstBaseActor(hand)->mActionType) >= ACTION_DOWN_ATTACK_e) {
                bstBaseActor(hand)->mActionType = ACTION_FLY_e;
                hand = (u32)bstBaseGlobals().hand[other];
                bstBaseActor(hand)->mDamage = 0;
                hand = (u32)bstBaseGlobals().hand[other];
                bstBaseActor(hand)->speedF = zero;
            }
        }
    }
}

static void bstCopyActorLighting(BstBaseLighting *src, BstBaseLighting *dst) {
    // HD: copy only the verified members, in the original load/store order.
    for (u32 i = 0; i < 6; ++i)
        dst->mLight.mPositionDirection[i] = (f32)src->mLight.mPositionDirection[i];
    for (u32 i = 0; i < 8; ++i)
        dst->mLight.mFloatParams[i] = (f32)src->mLight.mFloatParams[i];
    for (u32 i = 0; i < 3; ++i)
        dst->mFogParams[i] = (f32)src->mFogParams[i];
    for (u32 i = 0; i < 6; ++i)
        dst->mHdLight1.mPositionDirection[i] = (f32)src->mHdLight1.mPositionDirection[i];
    for (u32 i = 0; i < 8; ++i)
        dst->mHdLight1.mFloatParams[i] = (f32)src->mHdLight1.mFloatParams[i];
    for (u32 i = 0; i < 6; ++i)
        dst->mHdLight2.mPositionDirection[i] = (f32)src->mHdLight2.mPositionDirection[i];
    for (u32 i = 0; i < 8; ++i)
        dst->mHdLight2.mFloatParams[i] = (f32)src->mHdLight2.mFloatParams[i];
    for (u32 i = 0; i < 4; ++i)
        dst->mLight.mColor[i] = (u8)src->mLight.mColor[i];
    for (u32 i = 0; i < 9; ++i)
        dst->mFlags[i] = (u8)src->mFlags[i];
    for (u32 i = 0; i < 4; ++i)
        dst->mHdLight1.mColor[i] = (u8)src->mHdLight1.mColor[i];
    for (u32 i = 0; i < 4; ++i)
        dst->mHdLight2.mColor[i] = (u8)src->mHdLight2.mColor[i];
    for (u32 i = 0; i < 4; ++i)
        dst->mLight.mShortParams[i] = (u16)src->mLight.mShortParams[i];
    for (u32 i = 0; i < 4; ++i)
        dst->mColorC0[i] = (u16)src->mColorC0[i];
    for (u32 i = 0; i < 4; ++i)
        dst->mFogColor[i] = (u16)src->mFogColor[i];
    for (u32 i = 0; i < 4; ++i)
        dst->mHdLight1.mShortParams[i] = (u16)src->mHdLight1.mShortParams[i];
    for (u32 i = 0; i < 4; ++i)
        dst->mHdLight2.mShortParams[i] = (u16)src->mHdLight2.mShortParams[i];
    for (u32 i = 0; i < 3; ++i)
        dst->mLightPosWorld[i] = (u32)src->mLightPosWorld[i];
    for (u32 i = 0; i < 2; ++i)
        dst->mColorK[i] = (u32)src->mColorK[i];
}

/* 020E7DC0 */
static s32 daBst_Create(bst_class *i_this) {
    u32 address = gabi::ea(i_this);
    u32 status = (u32)(bstBaseActor(address)->actor_condition);
    if (!(status & 8)) {
        if (i_this) {
            bstConstructor(i_this);
            status = (u32)(bstBaseActor(address)->actor_condition);
        }
        bstBaseActor(address)->actor_condition = status | 8;
    }
    s32 phase = BstBase_dComIfG_resLoad(&bstBaseActor(address)->mPhs, "Bst");
    if (phase == cPhs_COMPLEATE_e) {
        u32 parameters = (u32)(bstBaseActor(address)->mParameters);
        bstBaseActor(address)->mPa_smokeEcallBack.m12 = 1;
        bstBaseActor(address)->mBstPartType = (u8)parameters;
        if (!BstBase_entryHeap(i_this, gabi::guest_fn(reinterpret_cast<const void *>(useHeapInit)),
                               0x96000))
            return cPhs_ERROR_e;
        bstBaseActor(address)->attention_info.flags = 4;
        bstBaseActor(address)->attention_info.distances[2] = 4;
        if ((u8)(bstBaseHioSet()) == 0) {
            bstBaseActor(address)->mHioSet = 1;
            bstBaseHioSet() = 1;
            // HD: localized HIO label is bound to the original guest string.
            u8 child = BstBase_hioCreate("Gohdan", &bstBaseGlobals().m4);
            bstBaseGlobals().m4 = child;
        }
        BstBase_acchSet(&bstBaseActor(address)->mAcch, &bstBaseActor(address)->current.pos,
                        &bstBaseActor(address)->old.pos, i_this, 1,
                        &bstBaseActor(address)->mAcchCir, &bstBaseActor(address)->speed, nullptr,
                        nullptr);
        f32 radius = (f32)(bstMoveConstants().mTen), height = (f32)(bstMoveConstants().mTwoHundred);
        bstBaseActor(address)->mAcch.mSameActorChk = 0;
        BstBase_acchCircleSet(&bstBaseActor(address)->mAcchCir, height, radius);
        u8 part = (u8)(bstBaseActor(address)->mBstPartType);
        if (part == Type_HEAD_e) {
            bstBaseGlobals().boss = address;
            bstBaseGlobals().msg = 0xFFFFFFFF;
            part = (u8)(bstBaseActor(address)->mBstPartType);
        }
        if (part == Type_LEFT_HAND_e) {
            bstBaseGlobals().hand[0] = address;
            part = (u8)(bstBaseActor(address)->mBstPartType);
        }
        if (part == Type_RIGHT_HAND_e) {
            bstBaseGlobals().hand[1] = address;
            part = (u8)(bstBaseActor(address)->mBstPartType);
        }
        BstBase_statusInit(&bstBaseActor(address)->mStts, part ? 230 : 255, 255, i_this);
        BstBase_cylinderSet(&bstBaseActor(address)->mHandHurtCyl, &bstBaseHandCylinder());
        part = (u8)(bstBaseActor(address)->mBstPartType);
        bstBasePointerWord(bstBaseActor(address)->mHandHurtCyl.mStts) =
            gabi::ea(&bstBaseActor(address)->mStts);
        if (part == Type_HEAD_e) {
            BstBase_cylinderSet(&bstBaseActor(address)->mHeadHurtCyl, &bstBaseHeadCylinder());
            bstBasePointerWord(bstBaseActor(address)->mHeadHurtCyl.mStts) =
                gabi::ea(&bstBaseActor(address)->mStts);
            for (u32 i = 0; i < 2; ++i) {
                u32 sphere = gabi::ea(&bstBaseActor(address)->mEyeSphs[i]);
                BstBase_sphereSet(bstBaseAddress<dCcD_Sph>(sphere), &bstBaseEyeSphere());
                bstBasePointerWord(bstBaseAddress<dCcD_GObjInf>(sphere)->mStts) =
                    gabi::ea(&bstBaseActor(address)->mStts);
            }
            bstBaseActor(address)->health = 3;
            bstBaseActor(address)->mEyeHealth[1] = 2;
            bstBaseActor(address)->max_health = 3;
            bstBaseActor(address)->mEyeHealth[0] = 2;
            for (u32 i = 0; i < 2; ++i) {
                s8 room = (s8)(bstBaseActor(address)->current.roomNo);
                u32 attachment =
                    BstBase_createActor(fpcNm_ATT_e, 100, &bstBaseActor(address)->current.pos, room,
                                        nullptr, nullptr, -1, nullptr);
                bstBaseActor(address)->m2E90[i] = attachment;
            }
            for (u32 i = 0; i < 10; ++i) {
                u32 sphere = gabi::ea(&bstBaseActor(address)->mCcD_beams[i]);
                BstBase_sphereSet(bstBaseAddress<dCcD_Sph>(sphere), &bstBaseBeamSphere());
                bstBasePointerWord(bstBaseAddress<dCcD_GObjInf>(sphere)->mStts) =
                    gabi::ea(&bstBaseActor(address)->mStts);
            }
            u32 save = (u32)(bstMoveSaveTable());
            if (!BstBase_dungeonItem(bstBaseStageBits(save), 3)) {
                save = (u32)(bstMoveSaveTable());
                if (BstBase_dungeonItem(bstBaseStageBits(save), 5)) {
                    f32 one = (f32)(bstMoveConstants().mOne),
                        zero = (f32)(bstMoveConstants().mZero);
                    bstBaseActor(address)->m2ED0 = one;
                    cXyz center;
                    center.x = zero;
                    center.y = zero;
                    center.z = zero;
                    bstBaseActor(address)->mRoomState = 1;
                    void *play = BstBase_dComIfGp_get();
                    u32 particles = (u32)(bstBasePointerWord(
                        reinterpret_cast<dComIfG_inf_c *>(play)->play.mParticle));
                    u32 emitter = BstBase_dPa_control_set(bstBaseAddress<void>(particles), 0,
                                                          0x81E9, &center, nullptr, nullptr, 255,
                                                          nullptr, -1, 0, nullptr, nullptr);
                    bstBaseActor(address)->m2EF8.v = emitter;
                    u32 room = (u32)(bstBaseRoomPointer());
                    bstBaseAddress<BstBaseRoom>(room)->mBossClear = 1;
                }
            }
        } else {
            for (u32 i = 0; i < 19; ++i) {
                u32 sphere = gabi::ea(&bstBaseActor(address)->mFingerSphs[i]);
                BstBase_sphereSet(bstBaseAddress<dCcD_Sph>(sphere), &bstBaseHandSphere());
                bstBasePointerWord(bstBaseAddress<dCcD_GObjInf>(sphere)->mStts) =
                    gabi::ea(&bstBaseActor(address)->mStts);
            }
            bstBaseActor(address)->health = 4;
            bstBaseActor(address)->max_health = 4;
        }
    }
    f32 spread = (f32)bstBaseSpread();
    s16 counter = (s16)gabi::ftoi(BstBase_cM_rndF(spread));
    bstCopyActorLighting(reinterpret_cast<BstBaseLighting *>(&bstBaseActor(address)->tevStr),
                         &bstBaseLighting(bstBaseActor(address)->mEnvLight));
    bstBaseActor(address)->mUpdateLastFacingDirIfMultipleOf32 = counter;
    bstCopyActorLighting(reinterpret_cast<BstBaseLighting *>(&bstBaseActor(address)->tevStr),
                         &bstBaseLighting(bstBaseActor(address)->m2F20));
    return phase;
}

// HD: compiler-generated and template code of this translation unit.

/* 020E8718 */
static void *bstHioConstructor(void *self) {
    if (!self)
        self = BstBase_alloc(8);
    if (self) {
        u32 address = gabi::ea(self);
        bstBaseAddress<BstBaseHio>(address)->m4 = -1;
        bstBaseAddress<BstBaseHio>(address)->mVtable = BstBaseVtable::Hio;
        bstBaseAddress<BstBaseHio>(address)->m6 = 0;
        bstBaseAddress<BstBaseHio>(address)->m5 = 0;
    }
    return self;
}

/* 020E876C */
static void bstStaticInit() {
    bstBaseGlobals().mHeaderZeros[2] = 0;
    bstBaseGlobals().mHeaderZeros[0] = 0;
    bstBaseGlobals().mHeaderZeros[3] = 0;
    bstBaseGlobals().mHeaderZeros[1] = 0;
    BstBase_registerGlobalObject(&bstBaseZeroHeaders()[0]);
    f32 lower = (f32)(bstBaseInitialHeaderValues()[0]);
    f32 upper = (f32)(bstBaseInitialHeaderValues()[1]);
    bstBaseGlobals().mHeaderValues[0] = lower;
    bstBaseGlobals().mHeaderValues[1] = upper;
    BstBase_angleCtor(&bstBaseGlobals().mAngle);
    BstBase_registerGlobalObject(&bstBaseZeroHeaders()[1]);
    BstBase_vectorCtor(&bstBaseGlobals().mVector);
    BstBase_registerGlobalObject(&bstBaseZeroHeaders()[2]);
    bstHioConstructor(&bstBaseGlobals().m4);
}

/* 020E880C */
static void bstSoundStart(bst_class *i_this, u32 sound, u32 parameter) {
    u32 address = gabi::ea(i_this);
    if (address && gabi::ea(&bstBaseActor(address)->eyePos)) {
        s8 room = (s8)(bstBaseActor(address)->current.roomNo);
        s8 reverb = BstBase_reverb(room);
        BstBase_mDoAud_seStart(sound, &bstBaseActor(address)->eyePos, parameter, reverb);
    }
}

/* 020E8878 */
static void bstHioDestructor(void *self, u32 flags) {
    if (self && (flags & 1))
        BstBase_free(self);
}

/* 020F01C8 */
static void bstDestructor(bst_class *i_this, u32 flags) {
    if (!i_this)
        return;
    u32 address = gabi::ea(i_this);
    BstBase_destroyArray(&bstBaseActor(address)->mEyeSphs[0], 2, 0x12C,
                         BstBaseFunction::SphereDestructor, 0, 0);
    BstBase_destroyArray(&bstBaseActor(address)->mFingerSphs[0], 19, 0x12C,
                         BstBaseFunction::SphereDestructor, 0, 0);
    BstBase_cylinderDtor(&bstBaseActor(address)->mHandHurtCyl, 2);
    BstBase_cylinderDtor(&bstBaseActor(address)->mHeadHurtCyl, 2);
    BstBase_statusDtor(&bstBaseActor(address)->mStts, 2);
    bstBaseActor(address)->mAcch.dBgS_GrpPassChk::__vtbl = BstBaseVtable::AcchGroup;
    bstBaseActor(address)->mAcch.dBgS_PolyPassChk::__vtbl = BstBaseVtable::AcchPoly;
    BstBase_acchDtor(&bstBaseActor(address)->mAcch, 0);
    BstBase_circleDtor(&bstBaseActor(address)->mAcchCir.m_cir, 2);
    BstBase_destroyArray(&bstBaseActor(address)->mCcD_beams[0], 10, 0x12C,
                         BstBaseFunction::SphereDestructor, 0, 0);
    BstBase_actorDtor(i_this, 0);
    if (flags & 1)
        BstBase_free(i_this);
}

/* 020F02D0 */
static void bstEmptyVirtual(void *self) {}

/* 020E78C4 */
static bst_class *bstConstructor(bst_class *i_this) {
    if (!i_this)
        i_this = static_cast<bst_class *>(BstBase_alloc(0x3334));
    if (!i_this)
        return nullptr;
    u32 address = gabi::ea(i_this);
    BstBase_actorCtor(i_this);
    bstBaseActor(address)->__vtbl = BstBaseVtable::Actor;
    BstBaseDefaults defaults{}; // HD: host register cache across subobject calls.
    // The HD constructor keeps these defaults in registers across every
    // subobject constructor, including the smoke callback's constructor.
    for (u32 i = 0; i < 6; ++i) {
        defaults.initial[i] = (f32)bstBaseLightDefaults().mPositionDirection[i];
        bstBaseLighting(bstBaseActor(address)->mEnvLight).mLight.mPositionDirection[i] = defaults.initial[i];
    }
    for (u32 i = 0; i < 4; ++i) {
        defaults.flags[i] = (u8)bstBaseLightDefaults().mColor[i];
        bstBaseLighting(bstBaseActor(address)->mEnvLight).mLight.mColor[i] = defaults.flags[i];
    }
    for (u32 i = 0; i < 4; ++i) {
        defaults.angles[i] = (s16)bstBaseLightDefaults().mShortParams[i];
        bstBaseLighting(bstBaseActor(address)->mEnvLight).mLight.mShortParams[i] = defaults.angles[i];
    }
    for (u32 i = 0; i < 8; ++i) {
        defaults.tail[i] = (f32)bstBaseLightDefaults().mFloatParams[i];
        bstBaseLighting(bstBaseActor(address)->mEnvLight).mLight.mFloatParams[i] = defaults.tail[i];
    }
    bstCopyTevDefaults(&bstBaseLighting(bstBaseActor(address)->mEnvLight).mHdLight1, defaults);
    bstCopyTevDefaults(&bstBaseLighting(bstBaseActor(address)->mEnvLight).mHdLight2, defaults);
    BstBase_constructArray(&bstBaseActor(address)->mCcD_beams[0], 10, 0x12C,
                           BstBaseFunction::SphereConstructor);
    BstBase_acchCircleCtor(&bstBaseActor(address)->mAcchCir);
    BstBase_acchCtor(&bstBaseActor(address)->mAcch);
    bstBaseActor(address)->mAcch.cBgS_Chk::__vtbl = BstBaseVtable::Acch;
    bstBaseActor(address)->mAcch.dBgS_PolyPassChk::__vtbl = BstBaseVtable::AcchPoly;
    bstBaseActor(address)->mAcch.dBgS_GrpPassChk::__vtbl = BstBaseVtable::AcchGroup;
    bstBaseActor(address)->mAcch.mbObjThrough = 1;
    BstBase_statusCtor(&bstBaseActor(address)->mStts);
    BstBase_globalStatusCtor(&bstBaseActor(address)->mStts.__vtbl_gstts);
    bstBaseActor(address)->mStts.__vtbl = BstBaseVtable::Status;
    bstBaseActor(address)->mStts.__vtbl_gstts = BstBaseVtable::GlobalStatus;
    BstBase_cylinderCtor(&bstBaseActor(address)->mHeadHurtCyl);
    bstBaseShape(bstBaseActor(address)->mHeadHurtCyl.mShapeAttr)->mShapeVtable =
        BstBaseVtable::CylinderShape;
    bstBaseShape(bstBaseActor(address)->mHeadHurtCyl.mShapeAttr)->mVtable =
        BstBaseVtable::ShapeAttribute;
    BstBase_cylinderShapeCtor(&bstBaseActor(address)->mHeadHurtCyl.mCyl);
    bstBaseActor(address)->mHeadHurtCyl.__vtbl_hitinf = BstBaseVtable::Cylinder;
    bstBaseShape(bstBaseActor(address)->mHeadHurtCyl.mShapeAttr)->mShapeVtable =
        BstBaseVtable::CylinderAttribute;
    bstBaseActor(address)->mHeadHurtCyl.mCyl.__vtbl = BstBaseVtable::CylinderGeometry;
    BstBase_cylinderCtor(&bstBaseActor(address)->mHandHurtCyl);
    bstBaseShape(bstBaseActor(address)->mHandHurtCyl.mShapeAttr)->mVtable =
        BstBaseVtable::ShapeAttribute;
    bstBaseShape(bstBaseActor(address)->mHandHurtCyl.mShapeAttr)->mShapeVtable =
        BstBaseVtable::CylinderShape;
    BstBase_cylinderShapeCtor(&bstBaseActor(address)->mHandHurtCyl.mCyl);
    bstBaseActor(address)->mHandHurtCyl.mCyl.__vtbl = BstBaseVtable::CylinderGeometry;
    bstBaseActor(address)->mHandHurtCyl.__vtbl_hitinf = BstBaseVtable::Cylinder;
    bstBaseShape(bstBaseActor(address)->mHandHurtCyl.mShapeAttr)->mShapeVtable =
        BstBaseVtable::CylinderAttribute;
    BstBase_constructArray(&bstBaseActor(address)->mFingerSphs[0], 19, 0x12C,
                           BstBaseFunction::SphereConstructor);
    BstBase_constructArray(&bstBaseActor(address)->mEyeSphs[0], 2, 0x12C,
                           BstBaseFunction::SphereConstructor);
    BstBase_smokeCtor(&bstBaseActor(address)->mPa_smokeEcallBack, 1);
    bstCopyTevDefaults(&bstBaseLighting(bstBaseActor(address)->m2F20).mLight, defaults);
    bstCopyTevDefaults(&bstBaseLighting(bstBaseActor(address)->m2F20).mHdLight1, defaults);
    bstCopyTevDefaults(&bstBaseLighting(bstBaseActor(address)->m2F20).mHdLight2, defaults);
    return i_this;
}

/* WWHD glue (tools/verify/autoglue.py)
data fly_bck_d 10192878
data set_za_brk 10192908
data set_za_btk 10192900
data set_brk_d 101928F8
data set_btk_d 101928F0
data set_bck_d 10192870
data set_za_bdl 10192868
data set_bdl_d 10192860
str 1000B85C in 020E47D0
str 1000B868 in 020E5068
str 1000B8FC in 020E7278
str 1000B8F8 in 020E7190
str 1000B904 in 020E7DC0
str 1000B908 "Gohdan" renamed
ext 025F7DB0 u32 BstBase_messageSet(void*,u32,void*)
ext 0273AD10 void* BstBase_alloc(s32)
ext 028F026C void BstBase_registerGlobalObject(void*)
ext 028ED6F8 void BstBase_angleCtor(void*)
ext 028EAB2C void BstBase_vectorCtor(void*)
ext 02520540 s8 BstBase_reverb(s8)
ext 025E1A40 void BstBase_mDoAud_seStart(u32,void*,u32,s8)
ext 0273AF40 void BstBase_free(void*)
ext 028F0164 void BstBase_destroyArray(void*,s32,s32,u32,s32,s32)
ext 02515A70 void BstBase_cylinderDtor(void*,s32)
ext 02515860 void BstBase_statusDtor(void*,s32)
ext 024EFD9C void BstBase_acchDtor(void*,s32)
ext 02018034 void BstBase_circleDtor(void*,s32)
ext 025D50BC void BstBase_actorDtor(void*,s32)
ext 026066C4 void* BstBase_getResource(void*,SafeString*,s32)
ext 025E4A98 void BstBase_setAnimation(void*,void*,u8,f32,f32,f32,f32,void*)
ext 027F7878 void* BstBase_getJoint(void*)
ext 028E90D4 void BstBase_PSMTXCopy(void*,void*)
ext 025F1C28 void BstBase_mDoMtx_YrotM(void*,s16)
ext 0201ADE0 void BstBase_cXyz_subtract(void*,void*,void*)
ext 020195B0 s16 BstBase_cM_atan2s(f32,f32)
ext 028F4384 f32 BstBase_cM_sqrt(f32)
ext 0200F428 void BstBase_cLib_addCalcAngleS2(void*,s16,s16,s16)
ext 0200ED84 void BstBase_cLib_addCalc(void*,f32,f32,f32)
ext 025F1884 void BstBase_mDoMtx_YrotS(void*,s16)
ext 025F1BF4 void BstBase_mDoMtx_XrotM(void*,s16)
ext 0200FCD8 void BstBase_MtxPosition(void*,void*)
ext 028E8D88 void BstBase_PSVECAdd(void*,void*,void*)
ext 025200D4 void* BstBase_dComIfGp_get()
ext 025204C8 void BstBase_dComIfG_resDelete(void*,const char*)
ext 025F0A18 void BstBase_hioDelete(s8)
ext 025D5218 void* BstBase_searchActor(u32,void*)
ext 025D57E0 void BstBase_deleteActor(void*)
ext 025E1B34 void BstBase_stopSound(void*)
ext 0201A478 void BstBase_setAngles(void*,s16,s16,s16)
ext 025A847C u32 BstBase_dPa_control_set(void*,s32,u32,void*,void*,void*,s32,void*,s32,s32,void*,void*)
ext 025D4ED0 void BstBase_actorCtor(void*)
ext 028EFFD0 void BstBase_constructArray(void*,s32,s32,u32)
ext 024EFE94 void BstBase_acchCircleCtor(void*)
ext 024F0474 void BstBase_acchCtor(void*)
ext 0200BD2C void BstBase_statusCtor(void*)
ext 02515DA0 void BstBase_globalStatusCtor(void*)
ext 02515FB8 void BstBase_cylinderCtor(void*)
ext 02018590 void BstBase_cylinderShapeCtor(void*)
ext 025A5B18 void BstBase_smokeCtor(void*,s32)
ext 028249B0 void BstBase_emitterMatrix(void*,void*,void*)
ext 028E93CC void BstBase_matrixTranslate(void*,f32,f32,f32)
ext 025F2518 void BstBase_mDoMtx_stack_scaleM(f32,f32,f32)
ext 02008974 f32 BstBase_dBgS_GroundCross(void*,void*)
ext 024F1AFC void BstBase_lineSet(void*,void*,void*,void*)
ext 02008860 BOOL BstBase_dBgS_LineCross(void*,void*)
ext 02008B4C void BstBase_lineDtor(void*,s32)
ext 025CB374 void BstBase_shock(void*,s32,s32,void*)
ext 025167C0 void BstBase_sphereStart(void*,void*)
ext 025167E4 void BstBase_sphereMove(void*,void*)
ext 0200E240 void BstBase_collisionSet(void*,void*)
ext 02008DAC void BstBase_groundDtor(void*,s32)
ext 02555D0C void* BstBase_dKy_getEnvlight()
ext 025626A4 void BstBase_tevSetup(void*,s32,void*,void*)
ext 02562F5C void BstBase_tevApply(void*,void*,void*)
ext 025E7FC4 void BstBase_btkEntry(void*,void*,f32)
ext 025E83FC void BstBase_brkEntry(void*,void*,f32)
ext 025E5590 void BstBase_modelEntry(void*)
ext 025E2DE0 void BstBase_updateModel(void*,s32)
ext 025F064C void BstBase_blurOn()
ext 025BED80 void BstBase_dSnap_RegistFig(s32,void*,f32,f32,f32)
ext 025E4F64 u32 BstBase_morphCtor(void*,void*,void*,void*,void*,s32,s32,s32,s32,void*,s32,u32,f32)
ext 027F3F94 void* BstBase_getJoints(void*)
ext 025E38E0 u32 BstBase_createModel(void*,s32,u32)
ext 025E7CE0 BOOL BstBase_initBtk(void*,void*,void*,s32,s32,s32,s32,s32,f32,s32)
ext 025E8154 BOOL BstBase_initBrk(void*,void*,void*,s32,s32,s32,s32,s32,f32,s32)
ext 025E7C6C void* BstBase_btkCtor(void*)
ext 025E80D0 void* BstBase_brkCtor(void*)
ext 025D6894 s16 BstBase_playerAngle(void*,void*)
ext 0255F554 void BstBase_hitFlash(void*,s32)
ext 02515E50 void BstBase_statusMove(void*)
ext 025162A4 BOOL BstBase_checkTargetHit(void*)
ext 02516300 u32 BstBase_getTargetHit(void*)
ext 02518CC8 void BstBase_hitSound(void*,void*,s32)
ext 025192A8 u32 BstBase_attackCheck(void*,void*)
ext 02520460 s32 BstBase_dComIfG_resLoad(void*,const char*)
ext 025D63E8 BOOL BstBase_entryHeap(void*,u32,u32)
ext 025F0A10 u8 BstBase_hioCreate(const char*,void*)
ext 024F06B4 void BstBase_acchSet(void*,void*,void*,void*,s32,void*,void*,void*,void*)
ext 024EFF44 void BstBase_acchCircleSet(void*,f32,f32)
ext 02515F14 void BstBase_statusInit(void*,s32,s32,void*)
ext 02516518 void BstBase_cylinderSet(void*,void*)
ext 0251677C void BstBase_sphereSet(void*,void*)
ext 025D5834 u32 BstBase_createActor(s32,s32,void*,s8,void*,void*,s32,void*)
ext 025B9100 BOOL BstBase_dungeonItem(void*,s32)
ext 02019918 f32 BstBase_cM_rndF(f32)
*/
