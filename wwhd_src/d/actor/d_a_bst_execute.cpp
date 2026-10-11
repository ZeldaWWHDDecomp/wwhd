// Gohdan per-frame head control, animation, transforms, and collision.
#include "d/actor/d_a_bst_execute.h"
using namespace bst_execute_names;
const gc_tu_data gc_tu = {.safeString = 0x1000B714};

/* inlined into daBst_Execute */
static void bstResumePedestal(be<u32>* global, u32 resourceId, bool brk, f32 one) {
    bst_class* owner = bstExecuteActor(*global);
    J3DModel* model = owner->m02C8;
    void* resources = bstExecuteResources(bstExecuteResourceTable());
    SafeString name;
    name.mStringTop = gabi::guest_addr("Bst");
    name.__vtbl = gc_tu.safeString;
    void* resource = BstExecute_getResource(resources, &name, resourceId);
    J3DModelData* data = model->mModelData.get();
    owner = bstExecuteActor(*global);
    J3DFrameCtrl* controller = brk ? &owner->mpBrkAnm->mFrameCtrl : &owner->mpBtkAnm->mFrameCtrl;
    (brk ? BstExecute_initBrk : BstExecute_initBtk)(controller, data, resource, 1, 2, 0, -1, 1, one,
                                                    0);
}
/* inlined into daBst_Execute */
static void main_cont(bst_class* actor, f32 half, f32 one) {
    bst_class* a = actor;
    dComIfG_inf_c* play = bstExecutePlay(BstExecute_play());
    s8 demo = static_cast<s8>(a->m2E9A);
    fopAc_ac_c* player = reinterpret_cast<fopAc_ac_c*>(play->play.mPlayerInfo[0].mpPlayer.get());
    if (demo)
        return;
    s16 timer = static_cast<s16>(a->mBombArrowDropTimer);
    if (timer)
        a->mBombArrowDropTimer = (s16)(timer - 1);
    for (u32 i = 0; i < 3; ++i) {
        s16 value = static_cast<s16>(a->m2E7E[i]);
        if (value)
            a->m2E7E[i] = (s16)(value - 1);
    }
    s8 phase = static_cast<s8>(a->m2E7C);
    if (phase == 0) {
        a->m2E7C = 1;
        return;
    }
    if (phase == 1) {
        // HD: this call site uses a stage-memory base 32 bytes after the shared layout.
        u32 save = static_cast<u32>(bstExecuteSaveTable());
        BOOL complete =
            BstExecute_isDungeonItem(bstExecuteStageBits(save), dSv_memBit_c::STAGE_BOSS_ENEMY);
        if (complete && static_cast<s16>(bstExecuteTuning().mRegS[5]) == 0)
            return;
        a->actor_status = static_cast<u32>(a->actor_status) | fopAcStts_SHOWMAP_e;
        save = static_cast<u32>(bstExecuteSaveTable());
        if (BstExecute_isDungeonItem(bstExecuteStageBits(save), dSv_memBit_c::STAGE_BOSS_DEMO)) {
            a->m2FE4 = 1;
            BstExecute_bgmStart(JA_BGM_BST_BATTLE);
            bstExecuteActor(bstExecuteGlobals().boss)->mActionType = ACTION_FLY_e;
            bstExecuteActor(bstExecuteGlobals().hand[0])->mActionType = ACTION_FLY_e;
            bstExecuteActor(bstExecuteGlobals().hand[1])->mActionType = ACTION_FLY_e;
            bstExecuteActor(bstExecuteGlobals().boss)->mDamage = 0;
            bstExecuteActor(bstExecuteGlobals().hand[1])->mDamage = 0;
            bstExecuteActor(bstExecuteGlobals().hand[0])->mDamage = 0;
            a->m2E7C = 10;
            bstResumePedestal(&bstExecuteGlobals().hand[0], 0x4A, true, one);
            bstResumePedestal(&bstExecuteGlobals().hand[0], 0x64, false, one);
            bstResumePedestal(&bstExecuteGlobals().hand[1], 0x4D, true, one);
            bstResumePedestal(&bstExecuteGlobals().hand[1], 0x67, false, one);
            bstResumePedestal(&bstExecuteGlobals().boss, 0x42, true, one);
            bstResumePedestal(&bstExecuteGlobals().boss, 0x5C, false, one);
            J3DFrameCtrl* controller = &a->m2FE0->mFrameCtrl;
            a->mBombArrowDropTimer = 400;
            controller->mRate = one;
        } else {
            bstExecuteActor(bstExecuteGlobals().hand[1])->mState = 10;
            bstExecuteActor(bstExecuteGlobals().hand[0])->mState = 10;
            bstExecuteActor(bstExecuteGlobals().boss)->mState = 10;
            f32 square = BstExecute_magnitudeSquared(&player->current.pos);
            f32 distance = BstExecute_sqrt(square);
            if (distance < static_cast<f32>(bstExecuteConstants().mApproachDistance)) {
                a->m2E9A = 10;
                a->m2E7C = 2;
            }
        }
        return;
    }
    if (phase == 10) {
        u32 status = static_cast<u32>(a->actor_status);
        a->m2E7C = (s8)(phase + 1);
        a->actor_status = status & ~fopAcStts_UNK4000_e;
        a->m2E7E[0] = 100;
        return;
    }
    if (phase == 11) {
        s16 timer = static_cast<s16>(a->m2E7E[0]);
        bst_class* head = bstExecuteActor(bstExecuteGlobals().boss);
        if (timer == 0 && static_cast<s16>(head->mActionType) != ACTION_BEAM_ATTACK_e) {
            f32 twoHundred = static_cast<f32>(bstExecuteConstants().mWallRadius),
                threeHundred = static_cast<f32>(bstExecuteConstants().mHeadWallHeight);
            BstExecute_setWall(&head->mAcchCir, twoHundred, threeHundred);
            twoHundred = static_cast<f32>(bstExecuteConstants().mWallRadius);
            bst_class* hand = bstExecuteActor(bstExecuteGlobals().hand[0]);
            BstExecute_setWall(&hand->mAcchCir, twoHundred, twoHundred);
            twoHundred = static_cast<f32>(bstExecuteConstants().mWallRadius);
            hand = bstExecuteActor(bstExecuteGlobals().hand[1]);
            BstExecute_setWall(&hand->mAcchCir, twoHundred, twoHundred);
            f32 random = cM_rndF(one),
                threshold = static_cast<f32>(bstExecuteConstants().mHaraiChance);
            auto setHand = [](be<u32>* global, s16 action) {
                bst_class* hand = bstExecuteActor(*global);
                hand->mActionType = action;
                hand = bstExecuteActor(*global);
                hand->mDamage = 0;
            };
            if (random < threshold || static_cast<s16>(bstExecuteTuning().mRegS[1]) != 0) {
                random = cM_rndF(one);
                be<u32>* global =
                    random < half ? &bstExecuteGlobals().hand[0] : &bstExecuteGlobals().hand[1];
                hand = bstExecuteActor(*global);
                if (static_cast<s16>(hand->mActionType) < ACTION_DOWN_ATTACK_e &&
                    static_cast<s16>(hand->mStateTimer) == 0)
                    setHand(global, ACTION_HARAI_ATTACK_e);
            } else {
                s8 health = static_cast<s8>(a->health);
                threshold = static_cast<f32>(bstExecuteConstants().mPairChance);
                if (health == 2)
                    threshold = static_cast<f32>(bstExecuteConstants().mTwoHealthChance);
                else if (health == 3)
                    threshold = static_cast<f32>(bstExecuteConstants().mHaraiChance);
                random = cM_rndF(one);
                bool pair = random < (health == 1 ? half : threshold) ||
                            static_cast<s16>(bstExecuteTuning().mRegS[0]) != 0;
                s16 action = ACTION_KUMI_ATTACK_e;
                if (!pair) {
                    random = cM_rndF(one);
                    pair = random < half;
                    action = ACTION_PAA_ATTACK_e;
                }
                if (pair) {
                    bst_class* first = bstExecuteActor(bstExecuteGlobals().hand[0]);
                    if (static_cast<s16>(first->mActionType) < ACTION_DAMAGE_e) {
                        bst_class* second = bstExecuteActor(bstExecuteGlobals().hand[1]);
                        if (static_cast<s16>(second->mActionType) < ACTION_DAMAGE_e &&
                            static_cast<s16>(first->mStateTimer) == 0 &&
                            static_cast<s16>(second->mStateTimer) == 0) {
                            setHand(&bstExecuteGlobals().hand[0], action);
                            setHand(&bstExecuteGlobals().hand[1], action);
                        }
                    }
                } else {
                    random = cM_rndF(one);
                    be<u32>* global =
                        random < half ? &bstExecuteGlobals().hand[0] : &bstExecuteGlobals().hand[1];
                    hand = bstExecuteActor(*global);
                    if (static_cast<s16>(hand->mActionType) < ACTION_DAMAGE_e &&
                        static_cast<s16>(hand->mStateTimer) == 0)
                        setHand(global, ACTION_DOWN_ATTACK_e);
                }
            }
            s8 health = static_cast<s8>(a->health);
            be<f32>* maxWait = health == 0   ? &bstExecuteConstants().mThirty
                               : health == 1 ? &bstExecuteConstants().mForty
                                             : &bstExecuteConstants().mFifty;
            f32 randomWait = cM_rndF(static_cast<f32>(*maxWait));
            f32 base = static_cast<f32>(health == 0   ? bstExecuteConstants().mThirty
                                        : health == 1 ? bstExecuteConstants().mFifty
                                                      : bstExecuteConstants().mSeventy);
            a->m2E7E[0] = (s16)gabi::ftoi(gabi::fadds_ppc(randomWait, base));
            head = bstExecuteActor(bstExecuteGlobals().boss);
        }
        head->m2E74[1] = 10;
        head = bstExecuteActor(bstExecuteGlobals().boss);
        head->m2E74[0] = 10;
        if (static_cast<s16>(a->m2E7E[2]) == 0) {
            bst_class* hand = bstExecuteActor(bstExecuteGlobals().hand[0]);
            if (static_cast<s16>(hand->mActionType) == ACTION_SLEEP_e) {
                hand = bstExecuteActor(bstExecuteGlobals().hand[1]);
                if (static_cast<s16>(hand->mActionType) == ACTION_SLEEP_e) {
                    a->m2E7C = 12;
                    bstExecuteActor(bstExecuteGlobals().hand[0])->m10FC[1] = 100;
                    bstExecuteActor(bstExecuteGlobals().hand[1])->m10FC[1] = 100;
                    head = bstExecuteActor(bstExecuteGlobals().boss);
                    if (head && gabi::ea(&head->eyePos)) {
                        s8 room = static_cast<s8>(head->current.roomNo),
                           reverb = dComIfGp_getReverb(room);
                        mDoAud_seStart(JA_SE_CM_BST_HEAD_EYE_OPEN, &head->eyePos, 0, reverb);
                    }
                    BstExecute_bgmStart(JA_BGM_UNK_122);
                    f32 randomWait = cM_rndF(static_cast<f32>(bstExecuteConstants().mFifty));
                    (a->m2E7E[1] = (s16)gabi::ftoi(gabi::fadds_ppc(
                         randomWait, static_cast<f32>(bstExecuteConstants().mOneFifty))));
                }
            }
        }
    } else if (phase == 12) {
        bst_class* head;
        if (static_cast<s16>(a->m2E7E[1]) == 0) {
            head = bstExecuteActor(bstExecuteGlobals().boss);
            if (static_cast<s16>(head->mActionType) == ACTION_FLY_e) {
                head->mActionType = ACTION_BEAM_ATTACK_e;
                head = bstExecuteActor(bstExecuteGlobals().boss);
                f32 minusFiveHundred = static_cast<f32>(bstExecuteConstants().mMinusFiveHundred);
                head->mDamage = 0;
                f32 target = gabi::fadds_ppc(static_cast<f32>(bstExecuteTuning().mRegF[7]),
                                             minusFiveHundred);
                head = bstExecuteActor(bstExecuteGlobals().boss);
                head->m10D0 = target;
            }
        }
        // HD: only the left hand action is reset here; both sleep timers are refreshed.
        bstExecuteActor(bstExecuteGlobals().hand[0])->mActionType = ACTION_SLEEP_e;
        bstExecuteActor(bstExecuteGlobals().hand[0])->m10FC[1] = 3;
        bstExecuteActor(bstExecuteGlobals().hand[1])->m10FC[1] = 3;
        head = bstExecuteActor(bstExecuteGlobals().boss);
        if (static_cast<s16>(head->mActionType) == ACTION_HEAD_DAMAGE_e &&
            static_cast<s16>(head->mDamage) >= 7) {
            a->m2E7C = 11;
            a->m2E7E[2] = 50;
        }
    }
}

static f32 bstSin(s32 angle) { return cM_ssin(static_cast<s16>(angle)); }
static f32 bstCos(s32 angle) { return cM_scos(static_cast<s16>(angle)); }
/* inlined into daBst_Execute */
static void bstExecuteTransformAndCollision(bst_class* actor, f32 zero, f32 half, f32 one) {
    bst_class* a = actor;
    Mtx34* matrix = bstExecuteFixedMatrix();
    s16 frequency = static_cast<s16>(bstExecuteTuning().mRegS[6]),
        frame = static_cast<s16>(a->mUpdateLastFacingDirIfMultipleOf32);
    f32 sx = bstSin(frame * (frequency + 700));
    f32 magnitude = static_cast<f32>(a->m10EC.z);
    f32 sy = bstSin(frame * (frequency + 750)), cz = bstCos(frame * (frequency + 720));
    cLib_addCalc0(&a->m10EC.z, one, one);
    mDoExt_McaMorf* morph = a->m02B8;
    f32 scaleZ = static_cast<f32>(a->scale.z);
    J3DModel* model = morph->mpModel.get();
    f32 scaleX = static_cast<f32>(a->scale.x), scaleY = static_cast<f32>(a->scale.y);
    model->mBaseScale.x = scaleX;
    model->mBaseScale.y = scaleY;
    model->mBaseScale.z = scaleZ;
    f32 x = static_cast<f32>(a->current.pos.x), y = static_cast<f32>(a->current.pos.y),
        z = static_cast<f32>(a->current.pos.z);
    BstExecute_matrixTranslate(matrix, gabi::fmadds(sx, magnitude, x),
                               gabi::fmadds(sy, magnitude, y), gabi::fmadds(cz, magnitude, z));
    s16 first = static_cast<s16>(a->mHeadHurtAngle);
    frame = static_cast<s16>(a->mUpdateLastFacingDirIfMultipleOf32);
    f32 fiveHundred = static_cast<f32>(bstExecuteConstants().mFiveHundred),
        tuning = static_cast<f32>(bstExecuteTuning().mRegF[14]);
    f32 firstStrength = gabi::fmuls_ppc((f32)first, gabi::fadds_ppc(tuning, fiveHundred));
    f32 sine = bstSin(frame * 0x2100), cosine = bstCos(frame * 0x2300);
    s16 second = static_cast<s16>(a->m112A);
    bst_class* head = bstExecuteActor(bstExecuteGlobals().boss);
    s16 yaw = (s16)gabi::ftoi(gabi::fmuls_ppc(sine, firstStrength));
    s16 pitch = (s16)gabi::ftoi(gabi::fmuls_ppc(cosine, firstStrength));
    f32 oneFifty = static_cast<f32>(bstExecuteConstants().mOneFifty);
    s16 headFrame = static_cast<s16>(head->mUpdateLastFacingDirIfMultipleOf32);
    f32 secondStrength = gabi::fmuls_ppc((f32)second, gabi::fadds_ppc(tuning, oneFifty));
    sine = bstSin(headFrame * 0x3600);
    cosine = bstCos(headFrame * 0x4300);
    s16 third = static_cast<s16>(a->m112C);
    s16 extraYaw = (s16)gabi::ftoi(gabi::fmuls_ppc(sine, secondStrength));
    s16 extraPitch = (s16)gabi::ftoi(gabi::fmuls_ppc(cosine, secondStrength));
    f32 hundred = static_cast<f32>(bstExecuteConstants().mHundred);
    cosine = bstCos(headFrame * 0x3A00);
    f32 thirdStrength = gabi::fmuls_ppc((f32)third, gabi::fadds_ppc(tuning, hundred));
    s16 lastPitch = (s16)gabi::ftoi(gabi::fmuls_ppc(cosine, thirdStrength));
    f32 thirty = static_cast<f32>(bstExecuteConstants().mThirty),
        decay = static_cast<f32>(bstExecuteTuning().mRegF[3]);
    yaw = (s16)(yaw + extraYaw);
    pitch = (s16)(pitch + extraPitch + lastPitch);
    cLib_addCalc0(&a->m10F8, one, gabi::fadds_ppc(decay, thirty));
    s16 shape = static_cast<s16>(a->shape_angle.y);
    cMtx_YrotM(matrix, (s16)(shape + yaw));
    shape = static_cast<s16>(a->shape_angle.x);
    cMtx_XrotM(matrix, (s16)(shape + pitch));
    shape = static_cast<s16>(a->shape_angle.z);
    cMtx_ZrotM(matrix, shape);
    x = static_cast<f32>(a->m1110);
    f32 ty = static_cast<f32>(bstExecuteTuning().mRegF[6]),
        tx = static_cast<f32>(bstExecuteTuning().mRegF[5]);
    y = static_cast<f32>(a->m1114);
    f32 tz = static_cast<f32>(bstExecuteTuning().mRegF[7]);
    z = static_cast<f32>(a->m1118);
    BstExecute_translateMultiply(gabi::fadds_ppc(tx, x), gabi::fadds_ppc(ty, y),
                                 gabi::fadds_ppc(tz, z));
    cLib_addCalc0(&a->m1110, one, static_cast<f32>(bstExecuteConstants().mFive));
    cLib_addCalc0(&a->m1114, one, static_cast<f32>(bstExecuteConstants().mTen));
    cLib_addCalc0(&a->m1118, one, static_cast<f32>(bstExecuteConstants().mFive));
    // GHS loads all twelve floats before writing a matrix copy.
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = static_cast<f32>(matrix->m[i / 4][i % 4]);
    for (u32 i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    for (u32 i = 0; i < 12; ++i)
        values[i] = static_cast<f32>(matrix->m[i / 4][i % 4]);
    J3DModel* decoration = a->m0388;
    for (u32 i = 0; i < 12; ++i)
        decoration->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    morph = a->m02B8;
    BstExecute_morphCalc(morph);
    x = static_cast<f32>(a->home.pos.x);
    z = static_cast<f32>(a->home.pos.z);
    y = static_cast<f32>(a->home.pos.y);
    BstExecute_translate(x, y, z, 0);
    shape = static_cast<s16>(a->home.angle.y);
    // HD: reload the mutable calculation matrix after each callee.
    matrix = bstExecuteMatrix(bstExecuteCalcMatrix());
    cMtx_YrotM(matrix, shape);
    shape = static_cast<s16>(a->home.angle.x);
    matrix = bstExecuteMatrix(bstExecuteCalcMatrix());
    cMtx_XrotM(matrix, shape);
    shape = static_cast<s16>(a->home.angle.z);
    matrix = bstExecuteMatrix(bstExecuteCalcMatrix());
    cMtx_ZrotM(matrix, shape);
    matrix = bstExecuteMatrix(bstExecuteCalcMatrix());
    J3DModel* pedestal = a->m02C8;
    for (u32 i = 0; i < 12; ++i)
        values[i] = static_cast<f32>(matrix->m[i / 4][i % 4]);
    for (u32 i = 0; i < 12; ++i)
        pedestal->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    if (static_cast<u8>(a->mBstPartType) == Type_HEAD_e) {
        morph = a->m02B8;
        model = morph->mpModel.get();
        J3DMtxBuffer_hd* block = model->mpMtxBuffer.get();
        u16 flags = static_cast<u16>(block->mFlags);
        Mtx34* matrices = bstExecuteMatrix(block->mpAnmMtx);
        block->mFlags = flags | 0x10;
        matrix = bstExecuteMatrix(bstExecuteCalcMatrix());
        BstExecute_matrixCopy(&matrices[8], matrix);
        f32 tx = static_cast<f32>(bstExecuteTuning().mRegF[0]),
            minusThirty = static_cast<f32>(bstExecuteConstants().mMinusThirty),
            minusHundred = static_cast<f32>(bstExecuteConstants().mMinusHundred);
        f32 tz = static_cast<f32>(bstExecuteTuning().mRegF[2]),
            ty = static_cast<f32>(bstExecuteTuning().mRegF[1]);
        cXyz local;
        cXyz transformed;
        local.y = ty;
        local.x = gabi::fadds_ppc(tx, minusThirty);
        local.z = gabi::fadds_ppc(tz, minusHundred);
        MtxPosition(&local, &transformed);
        BstExecute_setCylinderCenter(&a->mHeadHurtCyl.mCyl, &transformed);
        f32 height = static_cast<f32>(bstExecuteTuning().mRegF[3]),
            sixHundred = static_cast<f32>(bstExecuteConstants().mSixHundred);
        BstExecute_setCylinderHeight(&a->mHeadHurtCyl.mCyl, gabi::fadds_ppc(height, sixHundred));
        f32 radius = static_cast<f32>(bstExecuteTuning().mRegF[4]),
            oneEighty = static_cast<f32>(bstExecuteConstants().mOneEighty);
        BstExecute_setCylinderRadius(&a->mHeadHurtCyl.mCyl, gabi::fadds_ppc(radius, oneEighty));
        dComIfG_inf_c* play = bstExecutePlay(BstExecute_play());
        BstExecute_setCollision(&play->play.mCcS, &a->mHeadHurtCyl);
        u32 collisionFlags = static_cast<u32>(a->mHandHurtCyl.mObjTg.mSPrm);
        f32 oneFifty = static_cast<f32>(bstExecuteConstants().mOneFifty),
            seventy = static_cast<f32>(bstExecuteConstants().mSeventy);
        a->mHandHurtCyl.mObjTg.mSPrm = collisionFlags & ~1u;
        tx = static_cast<f32>(bstExecuteTuning().mRegF[5]);
        ty = static_cast<f32>(bstExecuteTuning().mRegF[6]);
        tz = static_cast<f32>(bstExecuteTuning().mRegF[7]);
        local.y = ty;
        local.x = gabi::fadds_ppc(tx, oneFifty);
        local.z = gabi::fadds_ppc(tz, seventy);
        MtxPosition(&local, &transformed);
        BstExecute_setCylinderCenter(&a->mHandHurtCyl.mCyl, &transformed);
        height = static_cast<f32>(bstExecuteTuning().mRegF[8]);
        BstExecute_setCylinderHeight(&a->mHandHurtCyl.mCyl, gabi::fadds_ppc(height, hundred));
        radius = static_cast<f32>(bstExecuteTuning().mRegF[9]);
        BstExecute_setCylinderRadius(&a->mHandHurtCyl.mCyl, gabi::fadds_ppc(radius, hundred));
        play = bstExecutePlay(BstExecute_play());
        BstExecute_setCollision(&play->play.mCcS, &a->mHandHurtCyl);
        f32 eyeOffset = static_cast<f32>(bstExecuteTuning().mRegF[11]),
            twoFifty = static_cast<f32>(bstExecuteConstants().mTwoFifty);
        local.z = zero;
        local.y = zero;
        local.x = gabi::fadds_ppc(eyeOffset, twoFifty);
        MtxPosition(&local, &a->eyePos);
        local.x = zero;
        local.y = zero;
        local.z = zero;
        f32 hiddenHeight = static_cast<f32>(bstExecuteConstants().mHiddenHeight),
            sixty = static_cast<f32>(bstExecuteConstants().mSixty);
        for (u32 i = 0; i < 2; ++i) {
            morph = a->m02B8;
            model = morph->mpModel.get();
            block = model->mpMtxBuffer.get();
            flags = static_cast<u16>(block->mFlags);
            matrices = bstExecuteMatrix(block->mpAnmMtx);
            block->mFlags = flags | 0x10;
            matrix = bstExecuteMatrix(bstExecuteCalcMatrix());
            BstExecute_matrixCopy(&matrices[i + 4], matrix);
            MtxPosition(&local, &transformed);
            s16 timer = static_cast<s16>(a->m2E74[i]);
            bool open = false;
            if (!timer)
                open = static_cast<s8>(a->mEyeHealth[i]) > 0;
            else
                a->m2E74[i] = (s16)(timer - 1);
            if (open)
                cLib_addCalcAngleS2(&a->m2E78[i], 0, 8, 0x200);
            else {
                s16 target = (s16)(static_cast<s16>(bstExecuteTuning().mRegS[8]) + 11000);
                transformed.y = gabi::fadds_ppc((f32)transformed.y, hiddenHeight);
                cLib_addCalcAngleS2(&a->m2E78[i], target, 1, 0x400);
            }
            dCcD_Sph* sphere = &a->mEyeSphs[i];
            BstExecute_setSphereCenter(&sphere->mSph, &transformed);
            radius = static_cast<f32>(bstExecuteTuning().mRegF[14]);
            BstExecute_setSphereRadius(&sphere->mSph, gabi::fadds_ppc(radius, sixty));
            play = bstExecutePlay(BstExecute_play());
            BstExecute_setCollision(&play->play.mCcS, sphere);
        }
    } else {
        s16 action = static_cast<s16>(a->mActionType);
        cXyz local;
        cXyz transformed;
        local.y = zero;
        local.z = zero;
        local.x = zero;
        f32 extraRadius = zero;
        if (action == ACTION_HARAI_ATTACK_e)
            extraRadius = gabi::fadds_ppc(static_cast<f32>(bstExecuteTuning().mRegF[16]),
                                          static_cast<f32>(bstExecuteConstants().mFifty));
        f32 fifty = static_cast<f32>(bstExecuteConstants().mFifty);
        for (u32 i = 0; i < 15; ++i) {
            morph = a->m02B8;
            model = morph->mpModel.get();
            J3DMtxBuffer_hd* block = model->mpMtxBuffer.get();
            u16 flags = static_cast<u16>(block->mFlags);
            Mtx34* matrices = bstExecuteMatrix(block->mpAnmMtx);
            block->mFlags = flags | 0x10;
            matrix = bstExecuteMatrix(bstExecuteCalcMatrix());
            BstExecute_matrixCopy(&matrices[i + 2], matrix);
            MtxPosition(&local, &transformed);
            dCcD_Sph* sphere = &a->mFingerSphs[i];
            BstExecute_setSphereCenter(&sphere->mSph, &transformed);
            f32 radius = gabi::fadds_ppc(static_cast<f32>(bstExecuteTuning().mRegF[14]), fifty);
            BstExecute_setSphereRadius(&sphere->mSph, gabi::fadds_ppc(radius, extraRadius));
            dComIfG_inf_c* play = bstExecutePlay(BstExecute_play());
            BstExecute_setCollision(&play->play.mCcS, sphere);
        }
        morph = a->m02B8;
        model = morph->mpModel.get();
        J3DMtxBuffer_hd* block = model->mpMtxBuffer.get();
        u16 flags = static_cast<u16>(block->mFlags);
        Mtx34* matrices = bstExecuteMatrix(block->mpAnmMtx);
        block->mFlags = flags | 0x10;
        matrix = bstExecuteMatrix(bstExecuteCalcMatrix());
        BstExecute_matrixCopy(&matrices[17], matrix);
        f32 sixtyFive = static_cast<f32>(bstExecuteConstants().mSixtyFive);
        for (u32 i = 0; i < 4; ++i) {
            u8 part = static_cast<u8>(a->mBstPartType);
            f32 localX = static_cast<f32>(bstExecutePalmOffsets().x[i]);
            f32 localY = static_cast<f32>(bstExecutePalmOffsets().y[i]),
                localZ = static_cast<f32>(bstExecutePalmOffsets().z[i]);
            local.x = part == Type_RIGHT_HAND_e ? -localX : localX;
            local.y = localY;
            local.z = localZ;
            MtxPosition(&local, &transformed);
            dCcD_Sph* sphere = &a->mFingerSphs[i + 15];
            BstExecute_setSphereCenter(&sphere->mSph, &transformed);
            f32 radius = gabi::fadds_ppc(static_cast<f32>(bstExecuteTuning().mRegF[15]), sixtyFive);
            BstExecute_setSphereRadius(&sphere->mSph, gabi::fadds_ppc(radius, extraRadius));
            dComIfG_inf_c* play = bstExecutePlay(BstExecute_play());
            BstExecute_setCollision(&play->play.mCcS, sphere);
        }
        u8 part = static_cast<u8>(a->mBstPartType);
        f32 tx = gabi::fadds_ppc(static_cast<f32>(bstExecuteTuning().mRegF[15]), hundred);
        f32 tz = gabi::fadds_ppc(static_cast<f32>(bstExecuteTuning().mRegF[17]),
                                 static_cast<f32>(bstExecuteConstants().mForty));
        local.x = part == Type_RIGHT_HAND_e ? -tx : tx;
        local.y = static_cast<f32>(bstExecuteTuning().mRegF[16]);
        local.z = part == Type_RIGHT_HAND_e ? -tz : tz;
        MtxPosition(&local, &transformed);
        tx = gabi::fadds_ppc(static_cast<f32>(bstExecuteTuning().mRegF[15]), hundred);
        f32 ty = static_cast<f32>(bstExecuteTuning().mRegF[16]);
        tz = gabi::fadds_ppc(static_cast<f32>(bstExecuteTuning().mRegF[18]),
                             static_cast<f32>(bstExecuteConstants().mTen));
        local.x = part == Type_RIGHT_HAND_e ? -tx : tx;
        local.y = ty;
        local.z = part == Type_RIGHT_HAND_e ? tz : -tz;
        MtxPosition(&local, &a->eyePos);
        BstExecute_setCylinderCenter(&a->mHandHurtCyl.mCyl, &transformed);
        f32 height = static_cast<f32>(bstExecuteTuning().mRegF[8]),
            oneForty = static_cast<f32>(bstExecuteConstants().mOneForty);
        BstExecute_setCylinderHeight(&a->mHandHurtCyl.mCyl, gabi::fadds_ppc(height, oneForty));
        f32 radius = static_cast<f32>(bstExecuteTuning().mRegF[9]),
            oneFifty = static_cast<f32>(bstExecuteConstants().mOneFifty);
        BstExecute_setCylinderRadius(&a->mHandHurtCyl.mCyl, gabi::fadds_ppc(radius, oneFifty));
        dComIfG_inf_c* play = bstExecutePlay(BstExecute_play());
        BstExecute_setCollision(&play->play.mCcS, &a->mHandHurtCyl);
    }
    u32 eyeX = static_cast<u32>(bstExecuteWords(&a->eyePos.x));
    u32 eyeY = static_cast<u32>(bstExecuteWords(&a->eyePos.y));
    bstExecuteWords(&a->attention_info.position.x) = eyeX;
    u8 part = static_cast<u8>(a->mBstPartType);
    u32 eyeZ = static_cast<u32>(bstExecuteWords(&a->eyePos.z));
    bstExecuteWords(&a->attention_info.position.y) = eyeY;
    bstExecuteWords(&a->attention_info.position.z) = eyeZ;
    if (part == Type_HEAD_e) {
        BstExecute_beamMove(actor);
        BstExecute_demoCamera(actor);
        s8 roomState = static_cast<s8>(a->mRoomState);
        if (roomState == 0)
            BstExecute_customColor(3, 0, zero);
        else if ((u32)roomState <= 8) {
            s32 first = 1, second = 0;
            switch (roomState) {
            case 1:
                first = 3;
                break;
            case 2:
                first = 0;
                second = 4;
                break;
            case 3:
                first = 3;
                second = 4;
                break;
            case 4:
                first = 5;
                second = 3;
                break;
            case 6:
            case 7:
                second = 2;
                break;
            }
            f32 color = static_cast<f32>(a->m2ED0);
            BstExecute_customColor(first, second, color);
            if (roomState == 1 || roomState == 2 || roomState == 6 || roomState == 8) {
                f32 step = half;
                if (roomState != 6)
                    step = static_cast<f32>(roomState == 2 ? bstExecuteConstants().mColorRise
                                                           : bstExecuteConstants().mColorStep);
                cLib_addCalc2(&a->m2ED0, one, one, step);
                if (roomState == 6 || roomState == 8) {
                    color = static_cast<f32>(a->m2ED0);
                    if (!(color < one))
                        a->mRoomState = roomState == 6 ? 7 : 1;
                }
            } else {
                f32 step = half;
                if (roomState != 7)
                    step = static_cast<f32>(roomState == 3   ? bstExecuteConstants().mColorFall
                                            : roomState == 4 ? bstExecuteConstants().mDecay
                                                             : bstExecuteConstants().mColorStep);
                cLib_addCalc0(&a->m2ED0, one, step);
            }
        }
        u32 messageState = static_cast<u32>(bstExecuteGlobals().msg);
        BstExecuteMessage* message = bstExecuteMessage(bstExecuteMessageTable());
        if (messageState != 0xFFFFFFFF) {
            s32 state = BstExecute_messageStatus(message);
            if (state == 14) {
                if (static_cast<s8>(bstExecuteGlobals().msg_end)) {
                    BstExecute_messageState(message, 16);
                    message->mSend = 1;
                }
            } else if (BstExecute_messageStatus(message) == 18) {
                BstExecute_messageState(message, 19);
                bstExecuteGlobals().msg = 0xFFFFFFFF;
            }
        }
        bool soundStarted = false;
        for (u32 i = 0; i < 2; ++i) {
            BstExecuteID id;
            id = static_cast<u32>(a->m2E90[i]);
            bst_class* attached = nullptr;
            if ((u32)id != 0xFFFFFFFF)
                attached = bstExecuteActor(BstExecute_searchActor(bstExecuteSearchCallback(), &id));
            if (!attached)
                continue;
            morph = a->m02B8;
            model = morph->mpModel.get();
            J3DMtxBuffer_hd* block = model->mpMtxBuffer.get();
            u16 flags = static_cast<u16>(block->mFlags);
            Mtx34* matrices = bstExecuteMatrix(block->mpAnmMtx);
            block->mFlags = flags | 0x10;
            matrix = bstExecuteMatrix(bstExecuteCalcMatrix());
            BstExecute_matrixCopy(&matrices[i + 4], matrix);
            cXyz origin;
            origin.x = zero;
            origin.y = zero;
            origin.z = zero;
            MtxPosition(&origin, &attached->current.pos);
            attached->max_health = 2;
            u8 health = static_cast<u8>(a->mEyeHealth[i]);
            attached->health = health;
            bool active = false;
            if (static_cast<s16>(a->m2E74[i]) == 0 && static_cast<s8>(a->mEyeHealth[i]) > 0) {
                s16 action = static_cast<s16>(a->mActionType);
                active = action != ACTION_HEAD_DAMAGE_e && action != ACTION_END_DEMO_e &&
                         action != ACTION_STAY_e;
            }
            if (!active)
                attached->attention_info.flags = 0;
            else {
                attached->attention_info.flags = 4;
                attached->attention_info.distances[2] = 4;
                if (!soundStarted) {
                    soundStarted = true;
                    if (gabi::ea(&a->eyePos)) {
                        s8 room = static_cast<s8>(a->current.roomNo);
                        s8 reverb = dComIfGp_getReverb(room);
                        mDoAud_seStart(JA_SE_CM_BST_HEAD_WORKING, &a->eyePos, 0, reverb);
                    }
                }
            }
        }
    }
    // HD: this helper belongs to the single verified execute function.
}

static void main_cont(bst_class*, f32, f32);
static void bstExecuteTransformAndCollision(bst_class*, f32, f32, f32);

/* 020E54A0 */
static BOOL daBst_Execute(bst_class* actor) {

    bst_class* a = actor;
    dComIfG_inf_c* play = bstExecutePlay(BstExecute_play());
    f32 zero = static_cast<f32>(bstExecuteConstants().mZero);
    s16 frame = static_cast<s16>(a->mUpdateLastFacingDirIfMultipleOf32);
    fopAc_ac_c* player = reinterpret_cast<fopAc_ac_c*>(play->play.mPlayerInfo[0].mpPlayer.get());
    cXyz origin;
    origin.x = zero;
    origin.y = zero;
    origin.z = zero;
    a->mUpdateLastFacingDirIfMultipleOf32 = (s16)(frame + 1);
    for (u32 i = 0; i < 5; ++i) {
        s16 value = static_cast<s16>(a->m10FC[i]);
        if (value)
            a->m10FC[i] = (s16)(value - 1);
    }
    s16 invulnerability = static_cast<s16>(a->mState);
    s16 damage = static_cast<s16>(a->mDamageTimer);
    if (invulnerability)
        a->mState = (s16)(invulnerability - 1);
    s16 firstHurt = static_cast<s16>(a->mHeadHurtAngle);
    if (damage)
        a->mDamageTimer = (s16)(damage - 1);
    s16 secondHurt = static_cast<s16>(a->m112A);
    if (firstHurt)
        a->mHeadHurtAngle = (s16)(firstHurt - 1);
    s16 thirdHurt = static_cast<s16>(a->m112C);
    if (secondHurt)
        a->m112A = (s16)(secondHurt - 1);
    s16 stateTimer = static_cast<s16>(a->mStateTimer);
    if (thirdHurt)
        a->m112C = (s16)(thirdHurt - 1);
    if (stateTimer)
        a->mStateTimer = (s16)(stateTimer - 1);
    u8 paused = static_cast<u8>(bstExecuteGlobals().mPause);
    f32 half = static_cast<f32>(bstExecuteConstants().mHalf),
        one = static_cast<f32>(bstExecuteConstants().mOne);
    if (!paused) {
        a->attention_info.flags = 4;
        if (static_cast<u8>(a->mBstPartType) == Type_HEAD_e) {
            if (static_cast<s8>(a->m2FE4)) {
                void* camera = BstExecute_cameraBody();
                BstExecute_cameraType(camera, "Test01", nullptr);
            }
            main_cont(actor, half, one);
            // Retire head attention and charge effects outside their active actions.
            if (static_cast<s16>(a->mActionType) != ACTION_HEAD_DAMAGE_e) {
                a->attention_info.flags = 0;
            }
            if (static_cast<s16>(a->mActionType) != ACTION_BEAM_ATTACK_e) {
                for (u32 i = 0; i < 2; ++i) {
                    BstEmitter* emitter = bstEmitter(a->m2ED8[i]);
                    if (emitter) {
                        emitter->becomeInvalidEmitter();
                        a->m2ED8[i] = 0;
                    }
                }
                if (static_cast<s8>(a->mRoomState) == 7)
                    a->mRoomState = 8;
            }
            f32 playerHeight = static_cast<f32>(player->current.pos.y),
                limit = static_cast<f32>(bstExecuteConstants().mPlayerHeightLimit);
            J3DFrameCtrl* controller = &a->m2FD8->mFrameCtrl;
            if (!(playerHeight > limit)) {
                controller->mRate = one;
                controller = &a->m2FD8->mFrameCtrl;
                controller->mAttribute = J3DFrameCtrl::EMode_LOOP;
            } else
                controller->mAttribute = J3DFrameCtrl::EMode_NONE;
            BstExecute_animatorPlay(a->m2FE0);
            BstExecute_animatorPlay(a->m2FD8);
            BstExecute_animatorPlay(a->m2FD4);
            BstExecute_matrixTranslate(bstExecuteFixedMatrix(), zero, zero, zero);
            // Matrix copies retain all twelve loaded values before their writes.
            for (gptr<J3DModel>* modelField : {&a->m2FD0, &a->m2FDC}) {
                f32 matrix[12];
                for (u32 i = 0; i < 12; ++i)
                    matrix[i] = static_cast<f32>(bstExecuteFixedMatrix()->m[i / 4][i % 4]);
                J3DModel* model = *modelField;
                for (u32 i = 0; i < 12; ++i)
                    model->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
            }
        }
        BstExecute_move(actor);
        s16 action = static_cast<s16>(a->mActionType);
        if (action != ACTION_STAY_e && action != ACTION_END_DEMO_e) {
            play = bstExecutePlay(BstExecute_play());
            BstExecute_correctPosition(&a->mAcch, &play->play.mBgS);
            BstExecute_setGroundUpY(&a->mAcch, zero);
        }
        mDoExt_McaMorf* morph = a->m02B8;
        BstExecute_morphPlay(morph, &a->eyePos, 0, 0);
        if (static_cast<s8>(a->m02C4)) {
            BstExecute_animatorPlay(a->mpTevRegAnimator);
            BstExecute_animatorPlay(a->mpTexMtxAnimator);
        }
        if (static_cast<s8>(a->m02D4)) {
            BstExecute_animatorPlay(a->mpBrkAnm);
            BstExecute_animatorPlay(a->mpBtkAnm);
        }
    }
    bstExecuteTransformAndCollision(actor, zero, half, one);
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py)
str 1000B70C "Bst"
str 1000B8F0 "Test01"
ext 0200E240 void BstExecute_setCollision(cCcS*,dCcD_GObjInf*)
ext 0200FAD8 void BstExecute_translate(f32,f32,f32,s32)
ext 020182E0 void BstExecute_setCylinderCenter(cM3dGCyl*,cXyz*)
ext 02018428 void BstExecute_setCylinderHeight(cM3dGCyl*,f32)
ext 020184DC void BstExecute_setCylinderRadius(cM3dGCyl*,f32)
ext 02018C8C void BstExecute_setSphereRadius(cM3dGSph*,f32)
ext 02018D40 void BstExecute_setSphereCenter(cM3dGSph*,cXyz*)
ext 020EB4BC void BstExecute_move(bst_class*)
ext 020EDC8C void BstExecute_beamMove(bst_class*)
ext 020EE200 void BstExecute_demoCamera(bst_class*)
ext 024EFF44 void BstExecute_setWall(dBgS_AcchCir*,f32,f32)
ext 024F08A8 void BstExecute_correctPosition(dBgS_ObjAcch*,dBgS*)
ext 024F12A8 void BstExecute_setGroundUpY(dBgS_ObjAcch*,f32)
ext 024F8044 void* BstExecute_cameraBody()
ext 02514EE4 void BstExecute_cameraType(void*,const char*,void*)
ext 025200D4 void* BstExecute_play()
ext 0255FDA4 void BstExecute_customColor(s32,s32,f32)
ext 025B9100 BOOL BstExecute_isDungeonItem(dSv_memBit_c*,s32)
ext 025D5218 u32 BstExecute_searchActor(void*,be<u32>*)
ext 025E18EC void BstExecute_bgmStart(u32)
ext 025E535C void BstExecute_morphPlay(mDoExt_McaMorf*,cXyz*,s32,s32)
ext 025E55A0 void BstExecute_morphCalc(mDoExt_McaMorf*)
ext 025E742C void BstExecute_animatorPlay(mDoExt_baseAnm*)
ext 025F24E0 void BstExecute_translateMultiply(f32,f32,f32)
ext 025F74D0 void BstExecute_messageState(BstExecuteMessage*,s32)
ext 025F795C s32 BstExecute_messageStatus(BstExecuteMessage*)
ext 026066C4 void* BstExecute_getResource(void*,SafeString*,u32)
ext 028E8DD0 f32 BstExecute_magnitudeSquared(cXyz*)
ext 028E90D4 void BstExecute_matrixCopy(Mtx34*,Mtx34*)
ext 028E93CC void BstExecute_matrixTranslate(Mtx34*,f32,f32,f32)
ext 028F4384 f32 BstExecute_sqrt(f32)
ext 025E8154 void BstExecute_initBrk(J3DFrameCtrl*,J3DModelData*,void*,s32,s32,s32,s32,s32,f32,s32)
ext 025E7CE0 void BstExecute_initBtk(J3DFrameCtrl*,J3DModelData*,void*,s32,s32,s32,s32,s32,f32,s32)
*/
