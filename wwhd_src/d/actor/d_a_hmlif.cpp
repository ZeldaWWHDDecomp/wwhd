/* Tower of the Gods glowing platforms: GameCube methods and verified HD layout.
 * HD resource, collision, matrix and call-order semantics are retained. */
#include "bindings.h"
#include "d/actor/d_a_hmlif.h"
using gabi::at;
static const char* hmlifArcNames[3] = {"Hmlif", "Hyuf1", "Hyuf2"};
static s16 hmlifBmdIndices[3] = {5, 6, 5};
static s16 hmlifDzbIndices[3] = {11, 15, 11};
static s16 hmlifBtpIndices[3] = {-1, 12, 8};
static s16 hmlifBrkIndices[3] = {8, 9, -1};
static s16 hmlifHeapSizes[3] = {3168, 6976, 6976};
static f32 hmlifSpeeds[16] = {
    1.6666666269302368f, 3.3333332538604736f, 5.0f,  6.666666507720947f, 8.333333015441895f, 10.0f,
    11.666666984558105f, 13.333333015441895f, 15.0f, 16.66666603088379f, 18.33333396911621f, 20.0f,
    21.66666603088379f,  23.33333396911621f,  25.0f, 26.66666603088379f};
static f32 hmlifOne;
static f32 hmlifZero;
static f32 hmlifSmoothAccel;
static f32 hmlifSmoothThreshold;
static f32 hmlifNormalAccel;
static f32 hmlifSpeedStepDivisor;
static f32 hmlifBrakeThreshold;
static f32 hmlifBrakeDivisor;
static f32 hmlifUpperSphereOffset;
static f32 hmlifLowerSphereOffset;
static f32 hmlifDirectionThreshold;
static f32 hmlifInitialSphereOffset;
static f32 hmlifInitialSphereRadius;
static f32 hmlifNegativePiSource;
static f32 hmlifPositivePiSource;
static hmlifHD::CullBox hmlifCullBoxes[3] = {{-200.0f, -100.0f, -200.0f, 200.0f, 50.0f, 200.0f},
                                             {-200.0f, -100.0f, -200.0f, 200.0f, 450.0f, 200.0f},
                                             {-200.0f, -100.0f, -200.0f, 200.0f, 450.0f, 200.0f}};
static Mtx34 hmlifMatrix;
static hmlifHD::SphereSource hmlifSphereSource;
static hmlifHD::SaveBlock* hmlifSave;
static void* hmlifResourceControl;
static u32 hmlifCurrentOpa;
static u32 hmlifCurrentXlu;
static hmlifHD::HeaderZeros hmlifHeaderZeros;
static f32 hmlifNegativePi;
static f32 hmlifPositivePi;
static u8 hmlifAngleTag;
static u8 hmlifByteTag;
static hmlifHD::DestructorRecord hmlifHeaderRecord0;
static hmlifHD::DestructorRecord hmlifHeaderRecord1;
static hmlifHD::DestructorRecord hmlifHeaderRecord2;
// HD: native adapters preserve the verified register widths and argument order.
void hmlifConstructMoveBg(void*);
void hmlifConstructSphere(void*);
void hmlifConstructStatus(void*);
void hmlifConstructGameStatus(void*);
s32 hmlifLoadResource(void*, void*);
s32 hmlifCreateMoveBg(void*, void*, s32, void*, s32);
s32 hmlifDeleteMoveBg(void*);
void hmlifDeleteResource(void*, void*);
s32 hmlifExecuteMoveBg(void*);
void* hmlifGetObjectResource(void*, HmlifName*, s32);
void hmlifAssert(const char*, s32, const char*);
mDoExt_btpAnm* hmlifNewBtp(mDoExt_btpAnm*);
mDoExt_brkAnm* hmlifNewBrk(mDoExt_brkAnm*);
u32 hmlifInitBtp(mDoExt_btpAnm*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, s32,
                 s32);
u32 hmlifInitBrk(mDoExt_brkAnm*, J3DModelData*, J3DAnmTevRegKey*, s32, s32, f32, s32, s32, s32,
                 s32);
u32 hmlifIsSwitch(void*, u32, s32);
void hmlifOnSwitch(void*, u32, s32);
u32 hmlifPositionsDiffer(void*, void*);
u32 hmlifPositionsEqual(void*, void*);
s32 hmlifGetReverb(s32);
void hmlifStartSound(u32, void*, u32, s32);
hmlifHD::Path* hmlifGetRoomPath(u32, s32);
hmlifHD::Path* hmlifGetNextRoomPath(hmlifHD::Path*, s32);
s32 hmlifAtan2s(f32, f32);
f32 hmlifSqrt(f32);
void hmlifAddCalcAngle(void*, s32, s32, s32);
void hmlifAddCalcSpeed(void*, f32, f32, f32);
void hmlifMtxYrotS(void*, s32);
void hmlifMtxMultVec(void*, cXyz*, cXyz*);
f32 hmlifAddCalc(void*, f32, f32, f32, f32);
void hmlifSubtractPosition(void*, cXyz*, void*);
f32 hmlifVectorSquare(cXyz*);
void hmlifAddCalcPos(void*, void*, f32, f32);
u32 hmlifCheckTargetHit(void*);
u32 hmlifGetTargetHit(void*);
void hmlifClearTargetHit(void*);
void hmlifSetSphereCenter(void*, cXyz*);
u32 hmlifGetGameInfo();
void hmlifRegisterCollider(void*, void*);
void hmlifMtxTrans(void*, f32, f32, f32);
void hmlifMtxZXYrotM(void*, s32, s32, s32);
void hmlifMtxCopy(void*, void*);
u32 hmlifGetEnvironment();
void hmlifSetLighting(void*, s32, void*, void*);
void hmlifSetModelLighting(void*, void*, void*);
void hmlifSetCullBox(void*, f32, f32, f32, f32, f32, f32);
f32 hmlifRandomRange(f32);
f32 hmlifRandom();
void hmlifSetSphere(void*, cXyz*, f32);
void hmlifInitStatus(void*, s32, s32, void*);
void hmlifSetSphereSource(void*, void*);
void hmlifRegisterDestructor(void*);
void hmlifConstructAngleTag(void*);
void hmlifConstructByteTag(void*);
void hmlifDeleteObject(void*);
void hmlifDestroyStatus(void*, s32);
void hmlifDestroySphere(void*, s32);
void hmlifDestroyActor(void*, s32);
/* 021766D4 */
s32 daHmlif_c::Delete() {
    return 1;
}

/* 021754B0 */
s32 daHmlif_c::daHmlifDelete() {
    s32 result = hmlifDeleteMoveBg(this);
    hmlifDeleteResource(static_cast<void*>(&mPhase),
                        at<void>(gabi::ea((const char*)hmlifArcNames[u8(m489)])));
    return result;
}
/* 02175520 */
s32 daHmlif_c::CreateHeap() {
    gabi::Local<HmlifName> name0;
    name0->vt = 0x10011364;
    name0->text = gabi::ea((const char*)hmlifArcNames[u8(m489)]);
    J3DModelData* modelData = static_cast<J3DModelData*>(hmlifGetObjectResource(
        hmlifResourceControl, name0.get(), s32(s16(hmlifBmdIndices[u8(m489)]))));
    if (!modelData)
        hmlifAssert("d_a_hmlif.cpp", 0x18f, "modelData != (0)");
    s16 idx = s16(hmlifBtpIndices[u8(m489)]);
    J3DModel* model =
        mDoExt_J3DModel__create(modelData, 0x80000, idx == -1 ? 0x11000022 : 0x11020022);
    mpModel = model;
    if (!model)
        return 0;
    u32 t = u8(m489);
    m478 = u8(u16(home.angle.z));
    mpBtp = at<mDoExt_btpAnm>(0);
    idx = s16(hmlifBtpIndices[t]);
    if (idx != -1) {
        f32 speed = f32(hmlifOne);
        gabi::Local<HmlifName> name1;
        name1->vt = 0x10011364;
        name1->text = gabi::ea((const char*)hmlifArcNames[t]);
        J3DAnmTexPattern* pbtp = static_cast<J3DAnmTexPattern*>(
            hmlifGetObjectResource(hmlifResourceControl, name1.get(), s32(idx)));
        if (!pbtp)
            hmlifAssert("d_a_hmlif.cpp", 0x1a6, "pbtp != (0)");
        mDoExt_btpAnm* btp = hmlifNewBtp(nullptr);
        mpBtp = btp;
        if (!btp)
            return 0;
        if (!hmlifInitBtp(btp, modelData, pbtp, 0, 0, speed, 0, -1, 0, 0))
            return 0;
        u32 current = gabi::ea((mDoExt_btpAnm*)mpBtp);
        m48A = 0;
        if (current) {
            s8 room = s8(home.roomNo);
            hmlifHD::SaveBlock* save = hmlifSave;
            u32 sw = u32(m478);
            if (hmlifIsSwitch(&save->save, sw, room))
                m48A = 3;
        }
    }
    t = u8(m489);
    mpBrk = at<mDoExt_brkAnm>(0);
    idx = s16(hmlifBrkIndices[t]);
    if (idx != -1) {
        f32 speed = f32(hmlifOne);
        gabi::Local<HmlifName> name2;
        name2->vt = 0x10011364;
        name2->text = gabi::ea((const char*)hmlifArcNames[t]);
        J3DAnmTevRegKey* pbrk = static_cast<J3DAnmTevRegKey*>(
            hmlifGetObjectResource(hmlifResourceControl, name2.get(), s32(idx)));
        if (!pbrk)
            hmlifAssert("d_a_hmlif.cpp", 0x1bf, "pbrk != (0)");
        mDoExt_brkAnm* brk = hmlifNewBrk(nullptr);
        mpBrk = brk;
        if (!brk)
            return 0;
        if (!hmlifInitBrk(brk, modelData, pbrk, 1, 2, speed, 0, -1, 0, 0))
            return 0;
        mpBrk->mFrameCtrl.mRate = f32(hmlifZero);
    }
    return 1;
}
/* 021762C8 */
s32 daHmlif_c::Create() {
    J3DModel* model = mpModel;
    u8 type = u8(m489);
    hmlifHD::CullBox* box = &hmlifCullBoxes[type];
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    hmlifSetCullBox(this, f32(box->minX), f32(box->minY), f32(box->minZ), f32(box->maxX),
                    f32(box->maxY), f32(box->maxZ));
    cullSizeFar = f32(hmlifOne);
    set_mtx();
    u32 param = u32(mParameters);
    u8 id = param >> 8;
    m474 = param & 255;
    m444 = id;
    m445 = id;
    if (id == 255)
        return 0;
    hmlifHD::Path* path = hmlifGetRoomPath(u32(id), s32(s8(current.roomNo)));
    ppd = path;
    if (!path) {
        m445 = 255;
        return 0;
    }
    u32 sw = u32(m474);
    u16 count = u16(path->m_num);
    if (sw != 255 && u8(path->mArg0) > count)
        return 0;
    u32 prm = u32(mParameters), start = (prm >> 22) & 31,
        points = gabi::ea((hmlifHD::Point*)path->m_points);
    // HD: the five-bit start field retains the unreachable comparison with 255.
    if (start == 255) {
        f32 pick = hmlifRandomRange(f32(s32(count) - 1));
        start = u8(gabi::ftoi(pick));
        count = u16(ppd->m_num);
    }
    if (start >= count)
        start = 0;
    u32 point = points + start * 16;
    f32 x = f32(at<hmlifHD::Point>(point)->m_position.x);
    current.pos.x = x;
    f32 y = f32(at<hmlifHD::Point>(point)->m_position.y);
    u32 xb = *reinterpret_cast<be<u32>*>(&current.pos.x);
    current.pos.y = y;
    f32 z = f32(at<hmlifHD::Point>(point)->m_position.z);
    *reinterpret_cast<be<u32>*>(&m45C.x) = xb;
    current.pos.z = z;
    u32 zb = *reinterpret_cast<be<u32>*>(&current.pos.z),
        yb = *reinterpret_cast<be<u32>*>(&current.pos.y);
    m448 = start;
    *reinterpret_cast<be<u32>*>(&m45C.y) = yb;
    m450.x = x;
    m450.y = y;
    u32 direction = (u32(mParameters) >> 20) & 3;
    m450.z = z;
    *reinterpret_cast<be<u32>*>(&m45C.z) = zb;
    if (direction == 2) {
        m446 = 1;
        f32 choice = hmlifRandom();
        if (choice < f32(hmlifDirectionThreshold))
            m447 = u8(m446);
        else {
            m446 = -1;
            m447 = -1;
        }
    } else {
        u8 dir = direction == 1 ? 255 : 1;
        m446 = dir;
        m447 = dir;
    }
    prm = u32(mParameters);
    u32 speed = (prm >> 16) & 15;
    if (speed > 8)
        speed = 8;
    sw = u32(m474);
    f32 value = f32(hmlifSpeeds[speed]);
    m47E = prm >> 31;
    speedF = value;
    m480 = value;
    if (sw != 255) {
        path = ppd;
        point = gabi::ea((hmlifHD::Point*)path->m_points) + u8(path->mArg0) * 16;
        m468.x = f32(at<hmlifHD::Point>(point)->m_position.x);
        m468.y = f32(at<hmlifHD::Point>(point)->m_position.y);
        m468.z = f32(at<hmlifHD::Point>(point)->m_position.z);
    }
    type = u8(m489);
    if (type == 1 || type == 2) {
        gabi::Local<cXyz> center;
        f32 cy = f32(current.pos.y), cx = f32(current.pos.x);
        f32 height = f32(hmlifInitialSphereOffset), radius = f32(hmlifInitialSphereRadius);
        center->x = cx;
        center->z = f32(current.pos.z);
        center->y = cy + height;
        hmlifSetSphere(&mSph.mSph, center.get(), radius);
        u32 flags = u32(mSph.mObjTg.mSPrm);
        mSph.mTgType = 0x001c4000;
        mSph.mObjTg.mSPrm = (flags & ~0xeu) | 8;
        hmlifInitStatus(static_cast<void*>(&mStts), 255, 255, this);
        mSph.SetStts(&mStts);
        hmlifSetSphereSource(static_cast<void*>(&mSph), &hmlifSphereSource);
    }
    return 1;
}
/* 021753A8 */
s32 daHmlif_c::daHmlifCreate() {
    u32 a = gabi::ea(this);
    u32 flags = u32(actor_condition);
    if (!(flags & fopAcCnd_INIT_e)) {
        if (a) {
            hmlifConstructMoveBg(this);
            __vtbl = 0x100114f0;
            hmlifConstructSphere(static_cast<void*>(&mSph));
            hmlifConstructStatus(static_cast<void*>(&mStts));
            hmlifConstructGameStatus(static_cast<void*>(&mStts.__vtbl_gstts));
            flags = u32(actor_condition);
            mStts.__vtbl = 0x1004ae88;
            mStts.__vtbl_gstts = 0x1004aec0;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    u32 type = (u32(mParameters) >> 27) & 15;
    if (type >= 3)
        type = 0;
    m489 = type;
    s32 phase = hmlifLoadResource(static_cast<void*>(&mPhase),
                                  at<void>(gabi::ea((const char*)hmlifArcNames[type])));
    if (phase == cPhs_COMPLEATE_e) {
        u32 t = u8(m489);
        phase = hmlifCreateMoveBg(this, at<void>(gabi::ea((const char*)hmlifArcNames[t])),
                                  s32(s16(hmlifDzbIndices[t])), at<void>(0x024ee708),
                                  s32(s16(hmlifHeapSizes[t])));
    }
    return phase;
}
/* 021760A8 */
void daHmlif_c::set_mtx() {
    f32 sx = f32(scale.x), sy = f32(scale.y), sz = f32(scale.z);
    J3DModel* model = mpModel;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = sz;
    hmlifMtxTrans(&hmlifMatrix, f32(current.pos.x), f32(current.pos.y), f32(current.pos.z));
    hmlifMtxZXYrotM(&hmlifMatrix, s32(s16(current.angle.x)), s32(s16(current.angle.y)),
                    s32(s16(current.angle.z)));
    f32 m[12];
    for (u32 i = 0; i < 12; i++)
        m[i] = f32(hmlifMatrix.m[i / 4][i % 4]);
    model = mpModel;
    // All source words are cached before the first model write, including
    // aliases.
    model->mBaseTransformMtx.m[1][1] = m[5];
    model->mBaseTransformMtx.m[0][3] = m[3];
    model->mBaseTransformMtx.m[1][3] = m[7];
    model->mBaseTransformMtx.m[2][1] = m[9];
    model->mBaseTransformMtx.m[1][0] = m[4];
    model->mBaseTransformMtx.m[2][2] = m[10];
    model->mBaseTransformMtx.m[0][1] = m[1];
    model->mBaseTransformMtx.m[0][0] = m[0];
    model->mBaseTransformMtx.m[1][2] = m[6];
    model->mBaseTransformMtx.m[0][2] = m[2];
    model->mBaseTransformMtx.m[2][0] = m[8];
    model->mBaseTransformMtx.m[2][3] = m[11];
    hmlifMtxCopy(&hmlifMatrix, static_cast<void*>(&mBgMtx));
}
/* 02176194 */
s32 daHmlif_c::Execute(be<u32>* output) {
    move();
    set_se();
    anim_play();
    check_col();
    *output = gabi::ea(&mBgMtx);
    set_mtx();
    return 1;
}
/* 02175DE4 */
void daHmlif_c::move() {
    if (u8(m445) == 255)
        return;
    if (u8(m47E))
        lift_smooth_move();
    else
        lift_normal_move();
}
/* 02175E04 */
void daHmlif_c::set_se() {
    if (hmlifPositionsDiffer(static_cast<void*>(&old.pos), static_cast<void*>(&current.pos))) {
        s32 reverb = hmlifGetReverb(s32(s8(current.roomNo)));
        hmlifStartSound(0x3025, static_cast<void*>(&eyePos), 0, reverb);
    }
}
/* 02175E60 */
void daHmlif_c::anim_play() {
    s8 room = s8(home.roomNo);
    hmlifHD::SaveBlock* save = hmlifSave;
    u32 on = hmlifIsSwitch(&save->save, u32(m478), room);
    s16 frame = s16(s16(m48A) + (on ? 1 : -1));
    if (on) {
        if (frame > 3)
            frame = 3;
    } else if (frame < 0)
        frame = 0;
    mDoExt_brkAnm* brk = mpBrk;
    m48A = frame;
    if (!brk)
        return;
    u32 stopped =
        hmlifPositionsEqual(static_cast<void*>(&old.pos), static_cast<void*>(&current.pos));
    brk = mpBrk;
    if (stopped)
        brk->mFrameCtrl.mAttribute = J3DFrameCtrl::EMode_NONE;
    else {
        brk->mFrameCtrl.mAttribute = J3DFrameCtrl::EMode_LOOP;
        mpBrk->mFrameCtrl.mRate = f32(hmlifOne);
    }
    mpBrk->play();
}

/* 02175F48 */
void daHmlif_c::check_col() {
    if (hmlifCheckTargetHit(static_cast<void*>(&mSph))) {
        u32 hit = hmlifGetTargetHit(static_cast<void*>(&mSph));
        if (hit && (u32(at<dCcD_GObjInf>(hit)->mAtType) & 0x001c4000)) {
            hmlifClearTargetHit(static_cast<void*>(&mSph));
            hmlifHD::SaveBlock* save = hmlifSave;
            s8 room = s8(home.roomNo);
            if (!hmlifIsSwitch(&save->save, u32(m478), s32(room))) {
                s32 reverb = hmlifGetReverb(s32(s8(current.roomNo)));
                hmlifStartSound(0x695e, static_cast<void*>(&eyePos), 0, reverb);
            }
            room = s8(home.roomNo);
            save = hmlifSave;
            hmlifOnSwitch(&save->save, u32(m478), s32(room));
        }
    }
    u8 type = u8(m489);
    if (type != 1 && type != 2)
        return;
    gabi::Local<cXyz> pos;
    pos->x = f32(current.pos.x);
    pos->z = f32(current.pos.z);
    f32 y = f32(current.pos.y);
    pos->y = type == 1 ? y + f32(hmlifUpperSphereOffset) : y - f32(hmlifLowerSphereOffset);
    hmlifSetSphereCenter(&mSph.mSph, pos.get());
    u32 play = hmlifGetGameInfo();
    hmlifRegisterCollider(&at<dComIfG_inf_c>(play)->play.mCcS, static_cast<void*>(&mSph));
}
/* 02175AD0 */
void daHmlif_c::lift_smooth_move() {
    f32 oldx = f32(m45C.x), tx = f32(m450.x), oldz = f32(m45C.z), tz = f32(m450.z);
    s32 yaw = hmlifAtan2s(tx - oldx, tz - oldz);
    f32 dy = f32(m450.y) - f32(current.pos.y), dx = f32(m450.x) - f32(current.pos.x),
        dz = f32(m450.z) - f32(current.pos.z);
    f32 square = gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy)), speed = f32(speedF);
    f32 distance = hmlifSqrt(square);
    hmlifAddCalcAngle(static_cast<void*>(&current.angle.y), yaw, 10, 0x400);
    hmlifAddCalcSpeed(static_cast<void*>(&speedF), f32(m480), f32(hmlifSmoothAccel), f32(hmlifOne));
    gabi::Local<cXyz> v, out;
    v->x = f32(hmlifZero);
    v->y = f32(hmlifZero);
    v->z = speed;
    hmlifMtxYrotS(&hmlifMatrix, s32(s16(current.angle.y)));
    hmlifMtxMultVec(&hmlifMatrix, v.get(), out.get());
    f32 y = f32(current.pos.y) + f32(out->y), x = f32(current.pos.x) + f32(out->x),
        z = f32(current.pos.z) + f32(out->z);
    f32 threshold = f32(hmlifSmoothThreshold);
    current.pos.y = y;
    current.pos.x = x;
    current.pos.z = z;
    if (distance < threshold)
        set_next_pnt();
}
/* 02175C34 */
void daHmlif_c::lift_normal_move() {
    u8 state = u8(m488);
    f32 factor = f32(hmlifNormalAccel), zero = f32(hmlifZero), one = f32(hmlifOne),
        divisor = f32(hmlifSpeedStepDivisor);
    if (state == MOVE_INIT) {
        m484 = 0;
        m488 = MOVE_ACCELERATE;
        set_next_pnt();
        state = 1;
    }
    if (state == MOVE_ACCELERATE) {
        f32 target = f32(m480);
        u32 count = u32(m484);
        m484 = count + 1;
        f32 result =
            hmlifAddCalc(static_cast<void*>(&speedF), target, factor, one, target / divisor);
        if (result == zero)
            m488 = MOVE_CRUISE;
    } else if (state == MOVE_CRUISE) {
        gabi::Local<cXyz> diff;
        m484 = 0;
        hmlifSubtractPosition(static_cast<void*>(&current.pos), diff.get(),
                              static_cast<void*>(&m450.x));
        f32 square = hmlifVectorSquare(diff.get());
        f32 distance = hmlifSqrt(square);
        if (distance < f32(hmlifBrakeThreshold))
            m488 = MOVE_DECELERATE;
    } else if (state == MOVE_DECELERATE) {
        f32 target = f32(m480);
        u32 count = u32(m484);
        f32 step = target / divisor;
        f32 divider = f32(hmlifBrakeDivisor);
        m484 = count + 1;
        f32 result = hmlifAddCalc(static_cast<void*>(&speedF), target / divider, factor, one, step);
        if (result == zero)
            m488 = MOVE_INIT;
    }
    hmlifAddCalcPos(static_cast<void*>(&current.pos), static_cast<void*>(&m450.x), one,
                    f32(speedF));
}

/* 02175908 */
void daHmlif_c::set_next_pnt() {
    s32 index = s8(m448);
    if (u8(m445) == 255)
        return;
    bool switched = false;
    if (chkSwitchPathType()) {
        s8 room = s8(current.roomNo);
        hmlifHD::Path* first = hmlifGetRoomPath(u32(u8(m444)), s32(room));
        hmlifHD::Path* path = ppd;
        bool junction = path == first && u32(index) == u8(path->mArg0);
        if (!junction) {
            first = hmlifGetRoomPath(u32(u8(m444)), s32(room));
            junction = path != first && index == 0;
        }
        if (junction) {
            u32 change = setNextPath();
            if (!gabi::ea((hmlifHD::Path*)ppd))
                hmlifAssert("d_a_hmlif.cpp", 0x3f5, "mPath != (0)");
            switched = change != 0;
        }
        if (!switched)
            index = s8(m448);
    }
    if (!switched) {
        index = s8(index + u8(m446));
        hmlifHD::Path* path = ppd;
        m448 = index;
        if (u8(path->m_closed) & 1) {
            u16 count = u16(path->m_num);
            if (index > s32(s8(count)) - 1)
                m448 = 0;
            else if (index < 0)
                m448 = count - 1;
        } else {
            u16 count = u16(path->m_num);
            if (index > s32(count) - 1) {
                hmlifHD::Path* p = ppd;
                m446 = -1;
                m448 = u16(p->m_num) - 2;
            } else if (index < 0) {
                m446 = 1;
                m448 = 1;
            }
        }
    }
    u32 y = *reinterpret_cast<be<u32>*>(&m450.y);
    hmlifHD::Path* path = ppd;
    *reinterpret_cast<be<u32>*>(&m45C.y) = y;
    u32 z = *reinterpret_cast<be<u32>*>(&m450.z), x = *reinterpret_cast<be<u32>*>(&m450.x);
    s32 point = s8(m448);
    *reinterpret_cast<be<u32>*>(&m45C.x) = x;
    *reinterpret_cast<be<u32>*>(&m45C.z) = z;
    u32 data = gabi::ea((hmlifHD::Point*)path->m_points) + u32(point) * 16;
    m450.x = f32(at<hmlifHD::Point>(data)->m_position.x);
    m450.y = f32(at<hmlifHD::Point>(data)->m_position.y);
    m450.z = f32(at<hmlifHD::Point>(data)->m_position.z);
}
/* 021757CC */
s32 daHmlif_c::setNextPath() {
    s8 room = s8(current.roomNo);
    hmlifHD::Path* old = ppd;
    hmlifHD::Path* path = hmlifGetRoomPath(u32(u8(m444)), s32(room));
    if (!path)
        hmlifAssert("d_a_hmlif.cpp", 0x42f, "homep != (0)");
    hmlifHD::Path* next = hmlifGetNextRoomPath(path, s32(room));
    hmlifHD::SaveBlock* save = hmlifSave;
    s8 saveRoom = s8(home.roomNo);
    if (hmlifIsSwitch(&save->save, u32(m474), s32(saveRoom))) {
        if (old == path)
            m447 = u8(m446);
        ppd = next;
        m448 = 1;
        m446 = 1;
        return 1;
    }
    if (static_cast<hmlifHD::Path*>(ppd) == path)
        return 0;
    ppd = path;
    m445 = u8(m444);
    u8 anchor = u8(path->mArg0);
    u16 count = u16(path->m_num);
    if (anchor == count)
        m447 = -1;
    else if (u8(path->mArg0) == 0)
        m447 = 1;
    else {
        u8 dir = u8(m447);
        hmlifHD::Path* p = ppd;
        m446 = dir;
        m448 = u8(p->mArg0) + dir;
        return 1;
    }
    hmlifHD::Path* p = ppd;
    u8 dir = u8(m447);
    m446 = dir;
    m448 = u8(p->mArg0) + dir;
    return 1;
}
/* 021757B8 */
u32 daHmlif_c::chkSwitchPathType() {
    return m474 != 255;
}
/* 021761F8 */
s32 daHmlif_c::Draw() {
    u32 env = hmlifGetEnvironment();
    hmlifSetLighting(at<void>(env), 0, static_cast<void*>(&current.pos), &tevStr);
    env = hmlifGetEnvironment();
    hmlifSetModelLighting(at<void>(env), at<void>(gabi::ea((J3DModel*)mpModel)), &tevStr);
    mDoExt_btpAnm* btp = mpBtp;
    if (btp) {
        J3DModel* model = mpModel;
        s16 frame = s16(m48A);
        btp->entry(model->getModelData(), frame);
    }
    mDoExt_brkAnm* brk = mpBrk;
    if (brk) {
        J3DModel* model = mpModel;
        f32 frame = f32(brk->mFrameCtrl.mFrame);
        brk->entry(model->getModelData(), frame);
    }
    u32 play = hmlifGetGameInfo();
    hmlifCurrentOpa = at<hmlifHD::DrawLists>(play)->bgOpa;
    play = hmlifGetGameInfo();
    hmlifCurrentXlu = at<hmlifHD::DrawLists>(play)->bgXlu;
    mDoExt_modelUpdateDL(mpModel, 0);
    play = hmlifGetGameInfo();
    hmlifCurrentOpa = at<hmlifHD::DrawLists>(play)->opa;
    play = hmlifGetGameInfo();
    hmlifCurrentXlu = at<hmlifHD::DrawLists>(play)->xlu;
    return 1;
}
/* 021754AC */
s32 daHmlif_Create(void* actor) {
    return static_cast<daHmlif_c*>(actor)->daHmlifCreate();
}
/* 02175508 */
s32 daHmlif_Delete(void* actor) {
    return static_cast<daHmlif_c*>(actor)->daHmlifDelete();
}
/* 0217550C */
s32 daHmlif_Draw(void* actor) {
    return static_cast<daHmlif_c*>(actor)->Draw_v();
}
/* 0217551C */
s32 daHmlif_Execute(void* actor) {
    return hmlifExecuteMoveBg(actor);
}
/* 021766B0 */
s32 daHmlif_IsDelete(void* actor) {
    return 1;
}
// HD: compiler-generated and template code of this translation unit.

/* 0217661C */
void __sinit_d_a_hmlif_cpp() {
    hmlifHeaderZeros.third = 0;
    hmlifHeaderZeros.first = 0;
    hmlifHeaderZeros.fourth = 0;
    hmlifHeaderZeros.second = 0;
    hmlifRegisterDestructor(&hmlifHeaderRecord0);
    f32 lo = f32(hmlifNegativePiSource), hi = f32(hmlifPositivePiSource);
    hmlifNegativePi = lo;
    hmlifPositivePi = hi;
    hmlifConstructAngleTag(&hmlifAngleTag);
    hmlifRegisterDestructor(&hmlifHeaderRecord1);
    hmlifConstructByteTag(&hmlifByteTag);
    hmlifRegisterDestructor(&hmlifHeaderRecord2);
}
/* 021766B8 */
void hmlifSafeStringDelete(void* actor, u32 flags) {
    if (actor && (flags & 1))
        hmlifDeleteObject(actor);
}
/* 021766CC */
s32 hmlifMoveBgIsDelete(void* actor) {
    return 1;
}
/* 021766DC */
void hmlifActorDelete(void* actor, u32 flags) {
    u32 a = gabi::ea(actor);
    if (!a)
        return;
    hmlifDestroyStatus(static_cast<void*>(&static_cast<daHmlif_c*>(actor)->mStts), 2);
    hmlifDestroySphere(static_cast<void*>(&static_cast<daHmlif_c*>(actor)->mSph), 2);
    hmlifDestroyActor(actor, 0);
    if (flags & 1)
        hmlifDeleteObject(actor);
}
/* 02176748 */
void hmlifSafeStringAssureTermination() {}
/* WWHD glue
data hmlifSave 101F84DC
data hmlifResourceControl 101F4F28
data hmlifCurrentOpa 104B4634
data hmlifCurrentXlu 104B4638
data hmlifHeaderZeros 10464734
data hmlifNegativePi 10464728
data hmlifPositivePi 1046472C
data hmlifAngleTag 10464730
data hmlifByteTag 10464731
data hmlifHeaderRecord0 101B7114
data hmlifHeaderRecord1 101B7120
data hmlifHeaderRecord2 101B712C
data hmlifArcNames 101B7168
data hmlifBmdIndices 10011440
data hmlifDzbIndices 10011448
data hmlifBtpIndices 10011450
data hmlifBrkIndices 10011458
data hmlifHeapSizes 10011460
data hmlifSpeeds 10011468
data hmlifOne 1001137C
data hmlifZero 10011380
data hmlifSmoothAccel 10011400
data hmlifSmoothThreshold 10011404
data hmlifNormalAccel 10011408
data hmlifSpeedStepDivisor 1001140C
data hmlifBrakeThreshold 10011410
data hmlifBrakeDivisor 10011414
data hmlifUpperSphereOffset 10011418
data hmlifLowerSphereOffset 1001141C
data hmlifDirectionThreshold 10011428
data hmlifInitialSphereOffset 1001142C
data hmlifInitialSphereRadius 10011430
data hmlifNegativePiSource 10011438
data hmlifPositivePiSource 1001143C
data hmlifCullBoxes 100114A8
data hmlifMatrix 1048D0CC
str 1001139C in 02175520
str 100113AC
str 10011384
str 10011390
str 100113C0 in 021757CC
str 100113D0
str 100113E0 in 02175908
str 100113F0
data hmlifSphereSource 101B70B4
ext 024F1D40 void hmlifConstructMoveBg(void*)
ext 025166F0 void hmlifConstructSphere(void*)
ext 0200BD2C void hmlifConstructStatus(void*)
ext 02515DA0 void hmlifConstructGameStatus(void*)
ext 02520460 s32 hmlifLoadResource(void*, void*)
ext 024F1D9C s32 hmlifCreateMoveBg(void*, void*, s32, void*, s32)
ext 024F1F64 s32 hmlifDeleteMoveBg(void*)
ext 025204C8 void hmlifDeleteResource(void*, void*)
ext 024F1E9C s32 hmlifExecuteMoveBg(void*)
ext 026066C4 void* hmlifGetObjectResource(void*, HmlifName*, s32)
ext 0273AA24 void hmlifAssert(const char*, s32, const char*)
ext 025E7820 mDoExt_btpAnm* hmlifNewBtp(mDoExt_btpAnm*)
ext 025E80D0 mDoExt_brkAnm* hmlifNewBrk(mDoExt_brkAnm*)
ext 025E789C u32 hmlifInitBtp(mDoExt_btpAnm*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, s32, s32)
ext 025E8154 u32 hmlifInitBrk(mDoExt_brkAnm*, J3DModelData*, J3DAnmTevRegKey*, s32, s32, f32, s32, s32, s32, s32)
ext 025BA0C0 u32 hmlifIsSwitch(void*, u32, s32)
ext 025B9E38 void hmlifOnSwitch(void*, u32, s32)
ext 0201AFD8 u32 hmlifPositionsDiffer(void*, void*)
ext 0201AF98 u32 hmlifPositionsEqual(void*, void*)
ext 02520540 s32 hmlifGetReverb(s32)
ext 025E1A40 void hmlifStartSound(u32, void*, u32, s32)
ext 025AAF88 hmlifHD::Path* hmlifGetRoomPath(u32, s32)
ext 025AB070 hmlifHD::Path* hmlifGetNextRoomPath(hmlifHD::Path*, s32)
ext 020195B0 s32 hmlifAtan2s(f32, f32)
ext 028F4384 f32 hmlifSqrt(f32)
ext 0200F428 void hmlifAddCalcAngle(void*, s32, s32, s32)
ext 0200ED84 void hmlifAddCalcSpeed(void*, f32, f32, f32)
ext 025F1884 void hmlifMtxYrotS(void*, s32)
ext 028E8F64 void hmlifMtxMultVec(void*, cXyz*, cXyz*)
ext 0200ECD4 f32 hmlifAddCalc(void*, f32, f32, f32, f32)
ext 0201ADE0 void hmlifSubtractPosition(void*, cXyz*, void*)
ext 028E8DD0 f32 hmlifVectorSquare(cXyz*)
ext 0200F164 void hmlifAddCalcPos(void*, void*, f32, f32)
ext 025162A4 u32 hmlifCheckTargetHit(void*)
ext 02516300 u32 hmlifGetTargetHit(void*)
ext 0251621C void hmlifClearTargetHit(void*)
ext 02018D40 void hmlifSetSphereCenter(void*, cXyz*)
ext 025200D4 u32 hmlifGetGameInfo()
ext 0200E240 void hmlifRegisterCollider(void*, void*)
ext 028E93CC void hmlifMtxTrans(void*, f32, f32, f32)
ext 025F1B48 void hmlifMtxZXYrotM(void*, s32, s32, s32)
ext 028E90D4 void hmlifMtxCopy(void*, void*)
ext 02555D0C u32 hmlifGetEnvironment()
ext 025626A4 void hmlifSetLighting(void*, s32, void*, void*)
ext 02562F5C void hmlifSetModelLighting(void*, void*, void*)
ext 025D674C void hmlifSetCullBox(void*, f32, f32, f32, f32, f32, f32)
ext 020198D8 f32 hmlifRandomRange(f32)
ext 02019788 f32 hmlifRandom()
ext 02018E88 void hmlifSetSphere(void*, cXyz*, f32)
ext 02515F14 void hmlifInitStatus(void*, s32, s32, void*)
ext 0251677C void hmlifSetSphereSource(void*, void*)
ext 028F026C void hmlifRegisterDestructor(void*)
ext 028ED6F8 void hmlifConstructAngleTag(void*)
ext 028EAB2C void hmlifConstructByteTag(void*)
ext 0273AF40 void hmlifDeleteObject(void*)
ext 02515860 void hmlifDestroyStatus(void*, s32)
ext 02515AE8 void hmlifDestroySphere(void*, s32)
ext 025D50BC void hmlifDestroyActor(void*, s32)
*/
