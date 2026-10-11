/** Gohdan's introduction, nose-item and defeat camera sequences. */
// HD: Event timing, shot offsets, player demo fields and report calls follow the verified HD code.
#include "d/actor/d_a_bst_camera.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/tu.h"
BstCameraPlay* bstCameraPlay();
s8 bstCameraReverb(s8);
void bstCameraSound(u32, cXyz*, u32, s8);
void bstCameraTrim(dCamera_c*, s32);
void bstCameraStart(dCamera_c*);
void bstCameraShowMeter();
void bstCameraMessage(bst_class*, u32);
void bstCameraMessageEnd();
void bstCameraChaseVector(cXyz*, cXyz*, f32, f32);
fopAc_ac_c* bstCameraSearch(void*, be<u32>*);
u32 bstCameraParticle(void*, u32, u32, cXyz*, csXyz*, u32, u32, void*, s32, u32, u32, u32);
void bstCameraOrderEvent(bst_class*, u32, u32, u32);
void bstCameraStop(dCamera_c*);
void bstCameraStreamPlay();
void bstCameraEndAnimations(bst_class*);
void bstCameraRotateY(Mtx34*, s16);
void bstCameraRotateX(Mtx34*, s16);
void bstCameraPlus(cXyz*, cXyz*, cXyz*);
void* bstCameraResource(void*, BstCameraArchive*, u32);
void bstCameraInitBtk(void*, J3DModelData*, void*, u32, s32, f32, s32, s32, u32, u32);
void bstCameraInitBrk(void*, J3DModelData*, void*, u32, s32, f32, s32, s32, u32, u32);
void bstCameraStageFlag(void*, u32);
void bstCameraBgmStart(u32);
void bstCameraCreateWarp(cXyz*, u32, s8, u32);
void bstCameraMinus(cXyz*, cXyz*, cXyz*);
void bstCameraShock(void*, s32, s32, cXyz*);
void bstCameraAnimation(bst_class*, s32, f32, u32, f32, s32);
void bstCameraCopyMatrix(Mtx34*, Mtx34*);
u32 bstCameraCreateItem(cXyz*, u32, s8, csXyz*, cXyz*, u32);
void bstCameraSet(dCamera_c*, cXyz*, cXyz*, s16, f32);
void bstCameraReport(s32, s32, const char*, s32);
static BstCameraTune sTune;
static BstCameraHands sHands;
static bst_class* sBoss;
static Mtx34* sCalcMatrix;
static void* sResourceControl;
static BstCameraSave* sSave;
static BstCameraAudio* sAudio;
static u8 sDemoMessageFlag;
static u8 sFindActorCallback;
static u16 sItemSmoke;
static BstCameraSmoke sStartSmoke;
static BstCameraTrig sTrig;
static bst_class* boss() { return sBoss; }
static bst_class* hand(u32 i) { return sHands.mHand[i]; }
/* inlined into demo_camera: retain the verified scalar PPC arithmetic */
static f32 add(f32 x, f32 y) { return gabi::fadds_ppc(x, y); }
static f32 sub(f32 x, f32 y) { return gabi::fsubs_ppc(x, y); }
static f32 mul(f32 x, f32 y) { return gabi::fmuls_ppc(x, y); }
static constexpr f32 zero = 0.0f, one = 1.0f, hundred = 100.0f, tenth = 0.1f;
/* 020EE200 */
static void demo_camera(bst_class* actor) {
    bst_class* i_this = actor;
    BstCameraPlayer* player = bstCameraPlay()->mpPlayer;
    s8 cameraId = bstCameraPlay()->mPlayerCameraId;
    dCamera_c* camera = &bstCameraPlay()->cameraInfo(cameraId).mpCamera->mCamera;
    gabi::Local<cXyz> offset, transformed, result, shakeEye, shakeCenter, scale, shock;
    gabi::Local<csXyz> itemAngle;
    gabi::Local<be<u32>> lookup;
    auto setVector = [](cXyz* p, f32 x, f32 y, f32 z) {
        p->x = x;
        p->y = y;
        p->z = z;
    };
    auto sound = [](fopAc_ac_c* owner, cXyz* position, u32 id) {
        if (owner && position) {
            s8 room = owner->current.roomNo;
            s8 reverb = bstCameraReverb(room);
            bstCameraSound(id, position, 0, reverb);
        }
    };
    auto playerPos = [&](f32 x) {
        setVector(transformed.get(), x, zero, zero);
        auto* vt = gabi::at<BstCameraPlayerVtable>(player->__vtbl);
        u32 fn = vt->mSetPlayerPosAndAngle;
        gabi::call(fn, player, transformed.get(), 0x4000); // HD: Virtual player-position slot.
    };
    auto finish = [&] {
        bstCameraTrim(camera, 0);
        bstCameraStart(camera);
        bstCameraShowMeter();
        BstCameraPlay* play = bstCameraPlay();
        play->mEvent.mEventFlag = play->mEvent.mEventFlag | 8;
    };
    auto message = [&](u32 id) { bstCameraMessage(actor, id); };
    auto messageEnd = [] { bstCameraMessageEnd(); };
    auto initialHandCamera = [&](u32 n) {
        i_this->m2EAC.copy(*&hand(n)->current.pos);
        i_this->m2EAC.y = add(f32(i_this->m2EAC.y), add(f32(sTune.mFloat[4]), 200.0f));
        i_this->m2EA0.copy(*&hand(n)->current.pos);
        i_this->m2EA0.x = add(f32(i_this->m2EA0.x), sub(f32(sTune.mFloat[5]), 2000.0f));
        i_this->m2EA0.y = add(f32(i_this->m2EA0.y), f32(sTune.mFloat[6]));
    };
    auto handCamera = [&](u32 n) {
        cLib_addCalc2(&i_this->m2EA0.x, sub(f32(hand(n)->current.pos.x), 1200.0f), tenth,
                      mul(40.0f, f32(i_this->m2EC4)));
        cLib_addCalc2(&i_this->m2EC4, one, one, 0.0099999998f);
        cLib_addCalc2(&i_this->m2EAC.x, f32(hand(n)->current.pos.x), 0.02f, 50.0f);
        cLib_addCalc2(&i_this->m2EAC.z, f32(hand(n)->current.pos.z), 0.02f, 50.0f);
        if (s32(i_this->msFrameCount) == 20) {
            hand(n)->m02D4 = 1;
            hand(n)->m02C4 = 1;
            bst_class* h = hand(n);
            sound(h, &h->current.pos, 0x58D4);
        }
        if (s32(i_this->msFrameCount) == 59) {
            if (n)
                i_this->m2ED0 = zero;
            i_this->mRoomState = 1;
        }
        if (s32(i_this->msFrameCount) == 60) {
            hand(n)->mDamage = 2;
            bst_class* h = hand(n);
            sound(h, &h->current.pos, 0x58D5);
        }
        if (s32(i_this->msFrameCount) == 90) {
            hand(n)->mDamage = 4;
            bst_class* h = hand(n);
            sound(h, &h->current.pos, 0x6983);
        }
        if (s32(i_this->msFrameCount) == 169)
            i_this->mRoomState = 0;
    };
    auto highCamera = [&] {
        i_this->m2EAC.y = add(f32(sTune.mFloat[16]), 4800.0f);
        i_this->m2EAC.x = zero;
        i_this->m2EAC.z = zero;
        i_this->m2EA0.x = add(f32(sTune.mFloat[13]), -700.0f);
        i_this->m2EA0.y = add(f32(sTune.mFloat[14]), hundred);
        i_this->m2EA0.z = add(f32(sTune.mFloat[15]), -300.0f);
    };
    auto defeatCamera = [&] {
        setVector(transformed.get(), f32(i_this->current.pos.x),
                  add(add(f32(i_this->current.pos.y), hundred), f32(sTune.mFloat[10])),
                  f32(i_this->current.pos.z));
        bstCameraChaseVector(&i_this->m2EAC, transformed.get(), 0.2f,
                             add(f32(sTune.mDefeatCenterStep), 10.0f));
        setVector(transformed.get(), sub(f32(i_this->current.pos.x), 1300.0f),
                  sub(f32(i_this->current.pos.y), 450.0f), sub(f32(i_this->current.pos.z), 500.0f));
        bstCameraChaseVector(&i_this->m2EA0, transformed.get(), 0.02f,
                             add(f32(sTune.mDefeatEyeStep), one));
    };
    auto findItem = [&] {
        u32 id = i_this->mCreatedItemId;
        *lookup = id;
        return id == 0xFFFFFFFF ? nullptr : bstCameraSearch(&sFindActorCallback, lookup.get());
    };
    auto particle = [&](u32 id, cXyz* pos, s8 room) {
        void* control = bstCameraPlay()->mpParticle;
        return bstCameraParticle(control, 2, id, pos, &i_this->shape_angle, 0, 0xB9,
                                 &i_this->mPa_smokeEcallBack, room, 0, 0, 0);
    };
    s32 state = s32(i_this->m2E9A);
    if (state == CAMERA_NOSE_START || state == CAMERA_INTRO_START || state == CAMERA_DEFEAT_START) {
        if (i_this->eventInfo.mCommand != dEvtCmd_INDEMO_e) {
            bstCameraOrderEvent(actor, 2, 0xFFFF, 0);
            i_this->eventInfo.mCondition = i_this->eventInfo.mCondition | dEvtCnd_UNK2_e;
            return;
        }
        i_this->m2E9A = state + 1;
        bstCameraStop(camera);
        bstCameraTrim(camera, 2);
        i_this->msFrameCount = 0;
        if (state == CAMERA_NOSE_START)
            i_this->m2E98 = 100;
        i_this->m2EC8 = 55.0f;
        i_this->m2EC4 = zero;
        if (state == CAMERA_NOSE_START) {
            state = CAMERA_NOSE;
        } else {
            if (state == CAMERA_INTRO_START) {
                i_this->m2EA0.x =
                    sub(add(f32(boss()->current.pos.x), f32(sTune.mFloat[1])), 3200.0f);
                i_this->m2EA0.y =
                    sub(add(f32(boss()->current.pos.y), f32(sTune.mFloat[2])), 1100.0f);
                i_this->m2EA0.z =
                    sub(add(f32(boss()->current.pos.z), f32(sTune.mFloat[3])), 300.0f);
                // HD: Centers this first shot to the left/below the head, with separate
                // tuning fields.
                i_this->m2EAC.x =
                    add(sub(f32(boss()->current.pos.x), 1000.0f), f32(sTune.mIntroCenterX));
                i_this->m2EAC.y =
                    add(add(f32(boss()->current.pos.y), -400.0f), f32(sTune.mIntroCenterY));
                i_this->m2EAC.z = f32(boss()->current.pos.z);
                bstCameraStreamPlay();
                i_this->m2FE0->setPlaySpeed(one);
                i_this->m2E98 = 120;
                state = CAMERA_INTRO;
            } else {
                i_this->m2EA0.x = add(f32(sTune.mFloat[2]), -1300.0f);
                player->mDemoParam = 0;
                player->mDemoOriginal = 3;
                bstCameraEndAnimations(actor);
                if (BstEmitter* emitter = bstEmitter(i_this->m2EF8)) {
                    emitter->becomeInvalidEmitter();
                    i_this->m2EF8 = 0;
                    sAudio->mBstHori = 0;
                }
                i_this->m2FE4 = 0;
                i_this->m2E98 = 150;
                state = CAMERA_DEFEAT_INTRO;
            }
        }
    }
    switch (state) {
    case CAMERA_NOSE:
        if (s32(i_this->msFrameCount) > (s16)(s32(sTune.mShort[4]) + 33)) {
            cLib_addCalc2(&i_this->m2EC4, -650.0f, 0.5f, add(f32(sTune.mFloat[3]), 50.0f));
            if (s32(i_this->msFrameCount) > (s16)(s32(sTune.mShort[4]) + 45))
                cLib_addCalc2(&i_this->m2EC8, 30.0f, 0.5f, add(f32(sTune.mFloat[3]), 10.0f));
        }
        bstCameraRotateY(sCalcMatrix,
                         (s16)(s32(i_this->shape_angle.y) + s32(sTune.mShort[1]) + 0xAF0));
        setVector(offset.get(),
                  s32(i_this->m2E9E) ? add(f32(sTune.mFloat[9]), -100.0f) : f32(sTune.mFloat[10]),
                  add(f32(sTune.mFloat[11]), f32(i_this->m2EC4)), f32(sTune.mFloat[12]));
        MtxPosition(offset.get(), transformed.get());
        bstCameraPlus(&i_this->eyePos, result.get(), transformed.get());
        i_this->m2EAC.copy(*result.get());
        bstCameraRotateX(sCalcMatrix, (s16)(s32(sTune.mShort[2]) + 4000));
        setVector(offset.get(), zero, f32(sTune.mFloat[7]), add(f32(sTune.mFloat[8]), 850.0f));
        MtxPosition(offset.get(), transformed.get());
        bstCameraPlus(&i_this->current.pos, result.get(), transformed.get());
        i_this->m2EA0.copy(*result.get());
        if (s32(i_this->msFrameCount) > 80) {
            i_this->mActionType = 1;
            i_this->mDamage = 0;
            finish();
            i_this->m2E9A = CAMERA_IDLE;
            i_this->m2E98 = 1;
        }
        break;
    case CAMERA_INTRO:
        playerPos(-635.0f);
        cLib_addCalc2(&i_this->m2EA0.z,
                      add(add(f32(boss()->current.pos.z), f32(sTune.mFloat[3])), 300.0f), 0.02f,
                      1.4f);
        if (s32(i_this->msFrameCount) == 60) {
            message(0x170D);
            sDemoMessageFlag = 0xFF;
        }
        if (s32(i_this->msFrameCount) == 180)
            messageEnd();
        if (s32(i_this->msFrameCount) == s32(sTune.mShort[4]) + 200)
            message(0x170E);
        if (s32(i_this->msFrameCount) == s32(sTune.mShort[4]) + 290)
            messageEnd();
        if (s32(i_this->msFrameCount) == 2 * s32(sTune.mShort[4]) + 310)
            message(0x170F);
        if (s32(i_this->msFrameCount) == 2 * s32(sTune.mShort[4]) + 410)
            messageEnd();
        if (s32(i_this->msFrameCount) != 2 * s32(sTune.mShort[4]) + 430)
            break;
        i_this->msFrameCount = 0;
        i_this->m2E9A = CAMERA_LEFT_HAND;
        initialHandCamera(0);
        [[fallthrough]];
    case CAMERA_LEFT_HAND:
        handCamera(0);
        if (s32(i_this->msFrameCount) != 170)
            break;
        i_this->msFrameCount = 0;
        i_this->m2EC4 = zero;
        i_this->m2E9A = CAMERA_RIGHT_HAND;
        initialHandCamera(1);
        [[fallthrough]];
    case CAMERA_RIGHT_HAND:
        handCamera(1);
        if (s32(i_this->msFrameCount) != 170)
            break;
        i_this->msFrameCount = 0;
        i_this->m2EC4 = zero;
        i_this->m2E9A = CAMERA_HEAD;
        i_this->m2EAC.copy(*&boss()->current.pos);
        i_this->m2EAC.y = add(f32(i_this->m2EAC.y), add(f32(sTune.mFloat[7]), 300.0f));
        i_this->m2EA0.copy(*&boss()->current.pos);
        i_this->m2EA0.x = add(f32(i_this->m2EA0.x), sub(f32(sTune.mFloat[8]), 2000.0f));
        i_this->m2EA0.y = add(f32(i_this->m2EA0.y), add(f32(sTune.mFloat[7]), 300.0f));
        [[fallthrough]];
    case CAMERA_HEAD:
        if (s32(i_this->msFrameCount) < 30)
            break;
        if (s32(i_this->msFrameCount) <= 90) {
            cLib_addCalc2(&i_this->m2EA0.x, sub(f32(boss()->current.pos.x), 1000.0f), 0.2f,
                          mul(hundred, f32(i_this->m2EC4)));
            cLib_addCalc2(&i_this->m2EC4, one, one, 0.050000001f);
            if (s32(i_this->msFrameCount) == 30) {
                boss()->m02D4 = 1;
                boss()->m02C4 = 1;
                bst_class* b = boss();
                sound(b, &b->current.pos, 0x58D6);
            }
            if (s32(i_this->msFrameCount) == 59) {
                i_this->m2ED0 = zero;
                i_this->mRoomState = 1;
            }
            if (s32(i_this->msFrameCount) == 60) {
                boss()->mDamage = 2;
                bst_class* b = boss();
                sound(b, &b->current.pos, 0x58D7);
            }
            if (s32(i_this->msFrameCount) == 90) {
                boss()->mDamage = 4;
                bst_class* b = boss();
                sound(b, &b->current.pos, 0x6984);
            }
        }
        if (s32(i_this->msFrameCount) != 150)
            break;
        i_this->m2E9A = CAMERA_BATTLE;
        boss()->mActionType = 1;
        hand(0)->mActionType = 1;
        hand(1)->mActionType = 1;
        boss()->mDamage = 0;
        hand(1)->mDamage = 0;
        hand(0)->mDamage = 0;
        i_this->m2EAC.copy(*&boss()->eyePos);
        i_this->m2EAC.y = add(f32(i_this->m2EAC.y), f32(sTune.mFloat[0]));
        i_this->m2EA0.copy(*&player->current.pos);
        i_this->m2EA0.x = sub(f32(i_this->m2EA0.x), add(f32(sTune.mFloat[1]), 500.0f));
        i_this->m2EA0.y = add(f32(i_this->m2EA0.y), add(f32(sTune.mFloat[2]), hundred));
        i_this->m2EC8 = add(f32(sTune.mFloat[4]), 75.0f);
        for (u32 n = 0; n != 6; ++n) {
            bst_class* b = n < 4 ? hand(n / 2) : boss();
            J3DModel* model = b->m02C8;
            gabi::Local<BstCameraArchive> name;
            name->mpName = gabi::at<char>(gabi::guest_addr("Bst"));
            name->mVtable = BstCameraSafeStringVtable;
            const u32 ids[6] = {dRes_INDEX_BST_BRK_UDEL_e, dRes_INDEX_BST_BTK_UDEL_e,
                                dRes_INDEX_BST_BRK_UDER_e, dRes_INDEX_BST_BTK_UDER_e,
                                dRes_INDEX_BST_BRK_MUNE_e, dRes_INDEX_BST_BTK_MUNE_e};
            void* resource = bstCameraResource(sResourceControl, name.get(), ids[n]);
            J3DModelData* data = model->mModelData;
            b = n < 4 ? hand(n / 2) : boss();
            void* animator = n % 2 ? static_cast<void*>(b->mpBtkAnm) : static_cast<void*>(b->mpBrkAnm);
            if (n % 2)
                bstCameraInitBtk(animator, data, resource, 1, 2, one, 0, -1, 1, 0);
            else
                bstCameraInitBrk(animator, data, resource, 1, 2, one, 0, -1, 1, 0);
        }
        i_this->mBombArrowDropTimer = 500;
        player->mDemoMode = 23;
        player->mDemoParam = 2;
        player->mDemoOriginal = 3;
        break;
    case CAMERA_BATTLE:
        cLib_addCalc2(&i_this->m2EAC.x, f32(boss()->current.pos.x), tenth, 50.0f);
        cLib_addCalc2(&i_this->m2EAC.y, add(f32(boss()->eyePos.y), f32(sTune.mFloat[0])), tenth,
                      50.0f);
        cLib_addCalc2(&i_this->m2EAC.z, f32(boss()->current.pos.z), tenth, 50.0f);
        i_this->m2EA0.copy(*&player->current.pos);
        i_this->m2EA0.x = sub(f32(i_this->m2EA0.x), add(f32(sTune.mFloat[1]), 500.0f));
        i_this->m2EA0.y = add(f32(i_this->m2EA0.y), add(f32(sTune.mFloat[2]), hundred));
        cLib_addCalc2(&i_this->m2EC8, 55.0f, tenth, 0.40000001f);
        if (s32(i_this->msFrameCount) == 200) {
            setVector(shock.get(), zero, zero, zero);
            void* control = bstCameraPlay()->mpParticle;
            u32 emitter =
                bstCameraParticle(control, 0, 0x81E9, shock.get(), 0, 0, 0xFF, 0, -1, 0, 0, 0);
            i_this->m2EF8 = gabi::at<JPABaseEmitter>(emitter);
            sAudio->mBstHori = 1;
        }
        if (s32(i_this->msFrameCount) == 300) {
            finish();
            i_this->m2E9A = CAMERA_IDLE;
            i_this->m2E7C = 10;
            bstCameraStageFlag(sSave->mStage, 5);
            bstCameraBgmStart(0x80000023u);
            i_this->m2E98 = 1;
            i_this->m2FE4 = 1;
            sDemoMessageFlag = 0;
        }
        break;
    case CAMERA_DEFEAT_INTRO:
        playerPos(add(f32(sTune.mFloat[1]), -300.0f));
        i_this->current.pos.z = zero;
        i_this->m2EAC.x = 300.0f;
        i_this->m2EAC.y = f32(i_this->current.pos.y);
        i_this->m2EAC.z = zero;
        i_this->shape_angle.y = -0x4000;
        i_this->current.pos.x = 300.0f;
        i_this->m2EAC.y = add(f32(i_this->m2EAC.y), add(f32(sTune.mFloat[0]), 300.0f));
        i_this->m2EA0.y = add(f32(sTune.mFloat[3]), 200.0f);
        i_this->m2EA0.z = zero;
        cLib_addCalc2(&i_this->m2EA0.x, add(f32(sTune.mFloat[2]), -600.0f), 0.2f,
                      mul(10.0f, f32(i_this->m2EC4)));
        cLib_addCalc2(&i_this->m2EC4, one, one, 0.050000001f);
        if (s32(i_this->msFrameCount) < 80)
            i_this->m2ECC = add(f32(sTune.mFloat[9]), 10.0f);
        if (s32(i_this->msFrameCount) == 90) {
            boss()->mDamage = 2;
            i_this->m2ED0 = zero;
            i_this->mRoomState = 2;
        }
        if (s32(i_this->msFrameCount) == 100)
            i_this->mRoomState = 3;
        if (s32(i_this->msFrameCount) > 100 && s32(i_this->msFrameCount) < 530)
            sound(actor, &actor->eyePos, 0x703A);
        if (s32(i_this->msFrameCount) == 120) {
            message(0x1710);
            sDemoMessageFlag = 0xFF;
        }
        if (s32(i_this->msFrameCount) == 240)
            messageEnd();
        if (s32(i_this->msFrameCount) == s32(sTune.mShort[4]) + 260)
            message(0x1711);
        if (s32(i_this->msFrameCount) == s32(sTune.mShort[4]) + 350)
            messageEnd();
        if (s32(i_this->msFrameCount) == 2 * s32(sTune.mShort[4]) + 370)
            message(0x1712);
        if (s32(i_this->msFrameCount) == 2 * s32(sTune.mShort[4]) + 520)
            messageEnd();
        if (s32(i_this->msFrameCount) == 3 * s32(sTune.mShort[4]) + 540)
            message(0x1713);
        if (s32(i_this->msFrameCount) == 3 * s32(sTune.mShort[4]) + 630)
            messageEnd();
        if (s32(i_this->msFrameCount) != 3 * s32(sTune.mShort[4]) + 660)
            break;
        i_this->msFrameCount = 0;
        i_this->m2EC4 = zero;
        i_this->m2E9A = s32(i_this->m2E9A) + 1;
        highCamera();
        bstCameraStreamPlay();
        i_this->m2E98 = 1;
        [[fallthrough]];
    case CAMERA_LOOK_UP:
        if (s32(i_this->msFrameCount) <= 30)
            break;
        i_this->m2EC4 = zero;
        i_this->m2E9A = s32(i_this->m2E9A) + 1;
        i_this->msFrameCount = 0;
        player->mDemoMode = 26;
        [[fallthrough]];
    case CAMERA_PLAYER:
        i_this->m2EAC.copy(*&player->current.pos);
        i_this->m2EAC.y = add(f32(i_this->m2EAC.y), add(f32(sTune.mFloat[10]), hundred));
        i_this->m2EA0.copy(*&player->current.pos);
        i_this->m2EA0.x = add(f32(i_this->m2EA0.x), add(f32(sTune.mFloat[11]), 150.0f));
        i_this->m2EA0.y = add(f32(i_this->m2EA0.y), add(f32(sTune.mFloat[12]), 50.0f));
        if (s32(i_this->msFrameCount) <= (s16)(s32(sTune.mShort[3]) + 65))
            break;
        i_this->m2EC4 = zero;
        i_this->m2E9A = s32(i_this->m2E9A) + 1;
        i_this->msFrameCount = 0;
        highCamera();
        i_this->mRoomState = 4;
        i_this->mDamage = 10;
        i_this->m2ED0 = one;
        bstCameraStageFlag(sSave->mStage, 3);
        setVector(offset.get(), zero, zero, zero);
        bstCameraCreateWarp(offset.get(), 0, i_this->current.roomNo, 0);
        [[fallthrough]];
    case CAMERA_RETRACT:
        if (s32(i_this->msFrameCount) == s32(sTune.mShort[4]) + 22)
            player->mDemoMode = 24;
        if (s32(i_this->msFrameCount) == s32(sTune.mShort[5]) + 150)
            player->mDemoMode = 29;
        cLib_addCalc2(&i_this->m2EAC.y, add(f32(sTune.mFloat[8]), 200.0f), 0.80000001f,
                      add(f32(sTune.mFloat[9]), 300.0f));
        if (s32(i_this->msFrameCount) != s32(sTune.mShort[6]) + 250)
            break;
        i_this->m2EAC.copy(*&i_this->current.pos);
        i_this->msFrameCount = 0;
        i_this->m2E9A = s32(i_this->m2E9A) + 1;
        i_this->m2EC4 = zero;
        i_this->m2EAC.y = add(f32(i_this->m2EAC.y), add(f32(sTune.mFloat[10]), hundred));
        // HD: Adds these tuning values after subtracting the shot offsets.
        i_this->m2EA0.x = add(sub(f32(i_this->current.pos.x), 1300.0f), f32(sTune.mFloat[11]));
        i_this->m2EA0.y = add(sub(f32(i_this->current.pos.y), 450.0f), f32(sTune.mFloat[12]));
        i_this->m2EA0.z = add(sub(f32(i_this->current.pos.z), 500.0f), f32(sTune.mFloat[13]));
        i_this->m2E98 = 100;
        [[fallthrough]];
    case CAMERA_RETURN_HOME:
        defeatCamera();
        bstCameraMinus(&i_this->home.pos, result.get(), &i_this->current.pos);
        offset->copy(*result);
        if (!(std_sqrtf(PSVECSquareMag(offset.get())) < 10.0f))
            break;
        {
            u32 id = sStartSmoke.mId[i_this->mBstPartType];
            s8 room = i_this->current.roomNo;
            particle(id, &i_this->current.pos, room);
        }
        setVector(shock.get(), zero, one, zero);
        {
            BstCameraPlay* play = bstCameraPlay();
            s32 shockStrength = s32(sTune.mShort[2]) + 3;
            bstCameraShock(play->mVibration, shockStrength, -0x21, shock.get());
        }
        sound(actor, &actor->eyePos, 0x6990);
        i_this->m2ECC = add(f32(sTune.mFloat[9]), 10.0f);
        i_this->m112A = s32(sTune.mShort[8]) + 3;
        i_this->current.pos.copy(*&i_this->home.pos);
        i_this->msFrameCount = 0;
        i_this->m2E9A = CAMERA_DROP_ITEM;
        bstCameraEndAnimations(actor);
        break;
    case CAMERA_DROP_ITEM:
        defeatCamera();
        if (s32(i_this->msFrameCount) == s32(sTune.mShort[4]) + 50) {
            bstCameraAnimation(actor, 20, 5.0f, 0, one, -1);
            sound(actor, &actor->current.pos, 0x58E2);
            i_this->m2E98 = 1;
        }
        if (gabi::ftoi(i_this->m02B8->getFrame()) == 26) {
            auto jointMatrix = [&](u32 joint) {
                bstCameraCopyMatrix(i_this->m02B8->getModel()->getAnmMtx(joint), sCalcMatrix);
            };
            jointMatrix(BST_JNT_HANAR_e);
            setVector(offset.get(), add(f32(sTune.mFloat[2]), 60.0f), f32(sTune.mFloat[3]),
                      f32(sTune.mFloat[4]));
            sound(actor, &actor->eyePos, 0x6901);
            MtxPosition(offset.get(), transformed.get());
            setVector(scale.get(), one, one, one);
            itemAngle->x = i_this->shape_angle.x;
            itemAngle->y = (s16)(s32(i_this->shape_angle.y) + s32(sTune.mShort[7]) - 300);
            itemAngle->z = i_this->shape_angle.z;
            u32 id = bstCameraCreateItem(transformed.get(), 0, i_this->current.roomNo,
                                         itemAngle.get(), scale.get(), 1);
            i_this->mCreatedItemId = id;
            sound(actor, &actor->eyePos, 0x6987);
            jointMatrix(BST_JNT_HEAD_e);
            setVector(offset.get(), zero, zero, zero);
            MtxPosition(offset.get(), transformed.get());
            s8 room = i_this->current.roomNo;
            u32 smoke = sItemSmoke;
            particle(smoke, transformed.get(), room);
        }
        if (gabi::ftoi(i_this->m02B8->getFrame()) > 26) {
            fopAc_ac_c* item = findItem();
            if (item) {
                i_this->m2E9A = CAMERA_FOLLOW_ITEM;
                item->speedF = add(f32(sTune.mFloat[6]), 65.0f);
            }
        }
        break;
    case CAMERA_FOLLOW_ITEM: {
        fopAc_ac_c* item = findItem();
        if (item) {
            cLib_addCalc2(&i_this->m2EAC.x, f32(item->current.pos.x), tenth, hundred);
            cLib_addCalc2(&i_this->m2EAC.y, f32(item->current.pos.y), tenth, hundred);
            cLib_addCalc2(&i_this->m2EAC.z, f32(item->current.pos.z), tenth, hundred);
            i_this->m2EC8 = add(f32(sTune.mFloat[4]), 55.0f);
        }
        if (s32(i_this->msFrameCount) == 170) {
            finish();
            i_this->m2E9A = CAMERA_IDLE;
            i_this->m2E7C = 100;
            i_this->m2E98 = 1;
            sDemoMessageFlag = 0;
        }
        break;
    }
    }
    if (s32(i_this->m2E9A) == CAMERA_IDLE)
        return;
    auto trig = [](s32 angle, bool cosine) {
        const auto& v = sTrig.mEntries[(u16)angle >> 3];
        return f32(cosine ? v.mCos : v.mSin);
    };
    s32 frame = s32(i_this->msFrameCount);
    f32 magnitude = f32(i_this->m2ECC);
    f32 x = mul(trig(frame * 0x3300, false), magnitude),
        y = mul(trig(frame * 0x3000, true), magnitude),
        z = mul(trig(frame * 0x3500, true), magnitude);
    setVector(shakeEye.get(), sub(f32(i_this->m2EAC.x), x), sub(f32(i_this->m2EAC.y), y),
              sub(f32(i_this->m2EAC.z), z));
    setVector(shakeCenter.get(), add(f32(i_this->m2EA0.x), x), add(f32(i_this->m2EA0.y), y),
              add(f32(i_this->m2EA0.z), z));
    s16 bank = (s16)gabi::ftoi(
        mul(mul(trig(s32(i_this->mUpdateLastFacingDirIfMultipleOf32) * 0x1C00, true), magnitude),
            7.5f));
    bstCameraSet(camera, shakeEye.get(), shakeCenter.get(), bank, f32(i_this->m2EC8));
    cLib_addCalc0(&i_this->m2ECC, one, one);
    bstCameraReport(30, 410, "K DEMO MODET  %d", s32(i_this->m2E9A));
    bstCameraReport(30, 430, "K MAIN COUNT  %d", s32(i_this->msFrameCount));
    i_this->msFrameCount = s32(i_this->msFrameCount) + 1;
}

// clang-format off
/* WWHD glue
data sTune 1047B608
data sHands 10462978
data sBoss 10462988
data sCalcMatrix 1018C7B0
data sResourceControl 101F4F28
data sSave 101F84DC
data sAudio 101FFC78
data sDemoMessageFlag 1047B07B
data sFindActorCallback 025E1234
data sItemSmoke 1019285E
data sStartSmoke 101928A0
data sTrig 104A44F8
ext 025200D4 BstCameraPlay* bstCameraPlay()
ext 02520540 s8 bstCameraReverb(s8)
ext 025E1A40 void bstCameraSound(u32, cXyz*, u32, s8)
ext 02515280 void bstCameraTrim(dCamera_c*, s32)
ext 02514F38 void bstCameraStart(dCamera_c*)
ext 0259169C void bstCameraShowMeter()
ext 020E46A0 void bstCameraMessage(bst_class*, u32)
ext 020E471C void bstCameraMessageEnd()
ext 0200F164 void bstCameraChaseVector(cXyz*, cXyz*, f32, f32)
ext 025D5218 fopAc_ac_c* bstCameraSearch(void*, be<u32>*)
ext 025A847C u32 bstCameraParticle(void*, u32, u32, cXyz*, csXyz*, u32, u32, void*, s32, u32, u32, u32)
ext 025D7B24 void bstCameraOrderEvent(bst_class*, u32, u32, u32)
ext 02514F2C void bstCameraStop(dCamera_c*)
ext 025E1944 void bstCameraStreamPlay()
ext 020E5068 void bstCameraEndAnimations(bst_class*)
ext 025F1884 void bstCameraRotateY(Mtx34*, s16)
ext 025F1BF4 void bstCameraRotateX(Mtx34*, s16)
ext 0201AD78 void bstCameraPlus(cXyz*, cXyz*, cXyz*)
ext 026066C4 void* bstCameraResource(void*, BstCameraArchive*, u32)
ext 025E7CE0 void bstCameraInitBtk(void*, J3DModelData*, void*, u32, s32, f32, s32, s32, u32, u32)
ext 025E8154 void bstCameraInitBrk(void*, J3DModelData*, void*, u32, s32, f32, s32, s32, u32, u32)
ext 025B9098 void bstCameraStageFlag(void*, u32)
ext 025E18EC void bstCameraBgmStart(u32)
ext 025D9874 void bstCameraCreateWarp(cXyz*, u32, s8, u32)
ext 0201ADE0 void bstCameraMinus(cXyz*, cXyz*, cXyz*)
ext 025CB374 void bstCameraShock(void*, s32, s32, cXyz*)
ext 020E47D0 void bstCameraAnimation(bst_class*, s32, f32, u32, f32, s32)
ext 028E90D4 void bstCameraCopyMatrix(Mtx34*, Mtx34*)
ext 025D8A5C u32 bstCameraCreateItem(cXyz*, u32, s8, csXyz*, cXyz*, u32)
ext 02514FE8 void bstCameraSet(dCamera_c*, cXyz*, cXyz*, s16, f32)
ext 027EC9E8 void bstCameraReport(s32, s32, const char*, s32)
str 1000B710 in 020EE200
str 1000B82C
str 1000B840
*/
// clang-format on
