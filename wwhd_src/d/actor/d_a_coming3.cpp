/**
 * d_a_coming3.cpp (WWHD)
 * Coming3 - barrel unk_47E spawner (daComing3::Act_c)
 *
 * Written from the WWHD code with
 * the GameCube decompilation (zeldaret/tww src/d/actor/d_a_coming3.cpp) as reference, verified
 * against cking.rpx.
 */
#include "d/actor/d_a_coming3.h"
using daComing3::Act_c;

// HD: GHS eight-byte member descriptors differ from host member pointers.
static u32 memberFunctionTarget(Act_c *&a, u32 table, u32 index) {
    auto *entry = &(*gabi::at<Coming3MethodHD[3]>(table))[index];
    s16 slot = entry->slot, adjustment = entry->adjustment;
    a = gabi::at<Act_c>(gabi::ea(a) + adjustment);
    if (slot < 0)
        return entry->target;
    s16 vptrOffset = (s16)(u32)entry->target;
    auto *vptr = gabi::at<gptr<Coming3MethodHD>>(gabi::ea(a) + vptrOffset)->get();
    return vptr[slot].target;
}

static void initializeGroundFilter(u32 addr, u32 guard, u32 registration) {
    auto *initialized = gabi::at<be<u32>>(guard);
    if (*initialized)
        return;
    *initialized = 1;
    auto *ground = gabi::at<Coming3GroundHD>(addr);
    coming3GroundConstruct(ground);
    ground->mGroup = gabi::at<u8>(addr + 0x40);
    ground->mPoly = gabi::at<u8>(addr + 0x4C);
    ground->mFlags = 1;
    ground->mVtable = COMING3_OBJ_GROUND_VTABLE;
    ground->mPolyVtable = COMING3_GROUND_POLY_VTABLE;
    ground->mCheckVtable = COMING3_GROUND_CHECK_VTABLE;
    ground->mGroupFlags[0] = 1;
    for (u32 i = 1; i <= 6; ++i)
        ground->mGroupFlags[i] = 0;
    ground->mGroupVtable = COMING3_GROUND_GROUP_VTABLE;
    coming3RegisterStatic(gabi::at<void>(registration));
}

/* 021150F4 */
BOOL daComing3::Act_c::get_water_height(be<f32> *height, be<s32> *room, const cXyz *pos) {
    auto *water = gabi::at<Coming3WaterHD>(0x10462E8C);
    if (!(u32)*gabi::at<be<u32>>(0x10462F84)) {
        *gabi::at<be<u32>>(0x10462F84) = 1;
        coming3WaterConstruct(water);
        coming3RegisterStatic(gabi::at<void>(0x101B3CF0));
    }
    f32 waterHeight = -1000000000.f;
    s32 roomID = 0;
    bool sea = coming3SeaCheckArea((f32)pos->x, (f32)pos->z) != 0;
    f32 x = pos->x, z = pos->z;
    if (sea)
        waterHeight = coming3SeaWave(x, z);
    f32 y = pos->y;
    if (sea) {
        x = pos->x;
        z = pos->z;
    }
    water->mPos.x = x;
    water->mPos.y = y - 1000.f;
    water->mUpper = y + 1000.f;
    water->mPos.z = z;
    if (coming3WaterCheck(dComIfG_Bgsp(), water)) {
        if (!sea)
            waterHeight = water->mHeight;
        roomID = coming3RoomReadId(dComIfG_Bgsp(), water);
    }
    if (room)
        *room = roomID;
    if (height)
        *height = waterHeight;
    if (!(waterHeight > -1000000000.f))
        return FALSE;
    auto *ground = gabi::at<Coming3GroundHD>(0x10462EDC);
    initializeGroundFilter(gabi::ea(ground), 0x10462F88, 0x101B3CFC);
    x = pos->x;
    y = pos->y;
    z = pos->z;
    ground->mPos.x = x;
    ground->mPos.z = z;
    ground->mPos.y = y + 20000.f;
    f32 groundHeight = coming3GroundCross(dComIfG_Bgsp(), ground);
    return !(groundHeight > waterHeight - 100.f);
}

/* 021156E4 */
void *position_is_safety_call_back(fopAc_ac_c *actor, SafetyCallback *cb) {
    if (coming3IsActor(actor) && actor->group == fopAc_ENEMY_e) {
        cXyz *p = cb->pos;
        f32 z = actor->current.pos.z - p->z, x = actor->current.pos.x - p->x;
        f32 square = gabi::fmadds(x, x, z * z);
        if (square < 200.f) {
            cb->ret = 0;
            return nullptr;
        }
    }
    return actor;
}

/* 02115694 */
s32 daComing3::Act_c::get_challenge_id() { return (unk_47E - 1) & 1; }

/* 021156A4 */
f32 daComing3::Act_c::get_limit_dist() {
    gabi::Local<be<u32>[2]> limits;
    (*limits)[0] = (u32)*gabi::at<be<u32>>(0x1000CB94);
    (*limits)[1] = (u32)*gabi::at<be<u32>>(0x1000CB98);
    s32 idx = get_challenge_id();
    return (f32)(*gabi::at<be<f32>[2]>(gabi::ea(limits.get())))[idx];
}

/* 02115668 */
BOOL daComing3::Act_c::coming_start_init() {
    unk_404 = 3;
    unk_400 = 0xffffffff;
    unk_478 = 0;
    unk_47C = 30;
    return TRUE;
}

/* 02115788 */
void daComing3::Act_c::coming_start_main() {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    if (!player)
        return;
    gabi::Local<cXyz> playerPos, actorPos;
    playerPos->copy(player->current.pos);
    actorPos->x = current.pos.x;
    actorPos->y = 0;
    actorPos->z = current.pos.z;
    playerPos->y = 0;
    f32 squared = coming3SquareDistance(playerPos.get(), actorPos.get());
    f32 distance = coming3SquareRoot(squared);
    unk_478 = distance;
    f32 limit = get_limit_dist();
    if (!(distance > limit)) {
        unk_47C = 30;
        return;
    }
    s16 delay = unk_47C;
    if (delay < 0)
        return;
    if (delay > 0) {
        delay = (s16)(delay - 1);
        unk_47C = delay;
        if (delay)
            return;
    }
    gabi::Local<be<f32>> water;
    if (!get_water_height(water.get(), nullptr, &current.pos))
        return;
    gabi::Local<SafetyCallback> cb;
    cb->pos = &current.pos;
    cb->ret = 1;
    coming3ActorSearch(COMING3_SAFETY_CALLBACK, cb.get());
    if (cb->ret != 1)
        return;
    gabi::Local<cXyz> position;
    position->x = current.pos.x;
    position->y = *water;
    position->z = current.pos.z;
    if (unk_400 == 0xffffffff) {
        s32 room = current.roomNo;
        s32 idx = get_challenge_id();
        u32 item = (u32)(*gabi::at<be<u32>[2]>(0x101B3D14))[idx];
        gabi::Local<csXyz> angle;
        coming3AngleConstruct(angle.get(), 0, (s32)(s16)shape_angle.y, 0);
        u32 parameters = 0x117F0100 | (item & 63);
        u32 id = coming3ActorCreate(457, parameters, position.get(), room, angle.get(), 0, -1, 0);
        unk_400 = id;
        if (id == 0xffffffff)
            return;
    }
    coming_process_init(1);
}

/* 02115958 */
BOOL daComing3::Act_c::coming_game_init() {
    unk_404 = 0;
    return unk_400 != 0xffffffff;
}

/* 02115978 */
void daComing3::Act_c::coming_game_main() {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    gabi::Local<gptr<Coming3BarrelHD>> barrel;
    s32 found = coming3SearchByID((u32)unk_400, barrel.get());
    Coming3BarrelHD *b = *barrel;
    s32 state = unk_404;
    if (b) {
        current.pos.copy(b->current.pos);
        shape_angle.x = b->shape_angle.x;
        shape_angle.y = b->shape_angle.y;
        shape_angle.z = b->shape_angle.z;
    }
    switch (state) {
    case 0:
        if (player) {
            f32 sq = coming3SquareDistance(&player->current.pos, &current.pos);
            f32 dist = coming3SquareRoot(sq);
            unk_478 = dist;
            b = *barrel;
        }
        if (b)
            unk_404 = 1;
        break;
    case 1:
        if (player) {
            f32 sq = coming3SquareDistance(&player->current.pos, &current.pos);
            unk_478 = coming3SquareRoot(sq);
        }
        if (collision_main() || !found) {
            unk_404 = 3;
            eff_break_tsubo();
        } else {
            f32 limit = get_limit_dist();
            if (!((f32)unk_478 > limit)) {
                b = *barrel;
                if (b) {
                    b->mExitReq = 1;
                    unk_404 = 2;
                }
            }
        }
        break;
    case 2:
        if (!found) {
            unk_400 = 0xffffffff;
            coming_process_init(0);
        }
        break;
    default:
        coming_process_init(2);
        break;
    }
}

/* ---- leftover functions of the translation unit ---- */

/* 02115D64 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 1000C9B4, after
 * the destructor 02115D50 */

/* 02115B40 */
BOOL daComing3::Act_c::coming_wait_init() {
    unk_404 = 3;
    return TRUE;
}

/* 02115B50 */
void daComing3::Act_c::coming_wait_main() {
    gabi::Local<gptr<Coming3BarrelHD>> barrel;
    coming3SearchByID((u32)unk_400, barrel.get());
    Coming3BarrelHD *b = *barrel;
    if (b) {
        gabi::Local<gptr<Act_c>> item;
        u32 id = b->mItemId;
        if (!coming3SearchByID(id, item.get())) {
            b->mExitReq = 1;
            coming3ActorDelete(this);
        }
    }
}

/* 02114BA8 */
bool daComing3::Act_c::coming_process_init(s32 idx) {
    if ((u32)(s32)idx >= 3)
        return false;
    Act_c *receiver = this;
    u32 target = memberFunctionTarget(receiver, 0x1000CB2C, idx);
    if (!gabi::call_ptr<s32>(target, receiver))
        return false;
    unk_444 = idx;
    return true;
}

/* 021153B4 */
void daComing3::Act_c::eff_break_tsubo() {
    auto *ground = gabi::at<Coming3GroundHD>(0x10462F30);
    initializeGroundFilter(gabi::ea(ground), 0x10462F8C, 0x101B3D08);
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    ground->mPos.x = x;
    ground->mPos.z = z;
    ground->mPos.y = y + 200.f;
    ground->mActorPid = mBsPcId;
    coming3GroundCross(dComIfG_Bgsp(), ground);
    s32 material = coming3MaterialSoundId(dComIfG_Bgsp(), &ground->mPolyInfo);
    s32 reverb = coming3Reverb((s32)(s8)current.roomNo);
    coming3SoundStart(0x6806, &current.pos, material, reverb);
    gabi::Local<cXyz> center, scale, origin;
    center->copy(current.pos);
    scale->x = 2;
    scale->y = 2;
    scale->z = 2;
    origin->x = 0;
    origin->y = 0;
    origin->z = 0;
    coming3MatrixVector(&unk_410, origin.get(), center.get());
    void *modelData = dComIfG_getObjectRes("Always", 49, 0x1000C9A0);
    void *pattern = dComIfG_getObjectRes("Always", 103, 0x1000C9A0);
    void *emitter = coming3ParticleSet(dComIfGp_getParticle(), 0, 23, center.get(), nullptr,
                                       scale.get(), 255, nullptr, -1, nullptr, nullptr, 0);
    if (emitter) {
        void *modelEmitter =
            coming3ModelEmitterConstruct(nullptr, emitter, modelData, 0, &tevStr, pattern, 0, 0);
        if (modelEmitter) {
            u32 particle = gabi::ea(dComIfGp_getParticle());
            coming3AppendEmitter(gabi::at<Coming3ParticleHD>(particle)->mpModelEmitterList.get(),
                                 modelEmitter);
        }
    }
    // HD: preserve the color pointer's guest stack slot (sp+0x18); the
    // outgoing particle call observes this address as its second stack argument.
    gabi::Local<u8[16]> colorLinkage;
    gabi::Local<be<u8>[4]> color;
    (*color)[1] = (u8)tevStr.mColorC0.g;
    (*color)[2] = (u8)tevStr.mColorC0.b;
    (*color)[3] = (u8)tevStr.mColorC0.a;
    (*color)[0] = (u8)tevStr.mColorC0.r;
    coming3ParticleSet(dComIfGp_getParticle(), 0, 24, center.get(), nullptr, scale.get(), 255,
                       gabi::at<void>(0x1047B254), -1, color.get(), &tevStr.mColorK0, 0);
}

/* 02114F04 */
void daComing3::Act_c::coming_process_main() {
    s16 idx = unk_444;
    if ((u32)(s32)idx >= 3)
        return;
    Act_c *receiver = this;
    u32 target = memberFunctionTarget(receiver, 0x1000CB50, idx);
    gabi::call_ptr(target, receiver);
}

/* 02114C60 */
void daComing3::Act_c::set_mtx() {
    J3DModel *m = unk_440;
    m->mBaseScale.z = 2;
    m->mBaseScale.y = 2;
    m->mBaseScale.x = 2;
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    s16 rz = shape_angle.z, rx = shape_angle.x, ry = shape_angle.y;
    coming3MatrixZXYRotate(mDoMtx_stack_c::get(), rx, ry, rz);
    mDoMtx_stack_c::transM(0, 390, 0);
    mDoMtx_stack_c::scaleM(2, 2, 2);
    J3DModel_setBaseTRMtx(unk_440, mDoMtx_stack_c::get());
    coming3MatrixCopy(mDoMtx_stack_c::get(), &unk_410);
    coming3ModelCalc(unk_440.get());
}

/* 02114B2C */
void daComing3::Act_c::collision_init() {
    coming3SttsInit(&mStts, 255, 255, this);
    coming3CylSet(&mCyl, gabi::at<void>(0x1000CAC8));
    mCyl.mStts = &mStts;
    u32 zeroX = (u32)*gabi::at<be<u32>>(0x101FFBA8);
    u32 flags = mCyl.mGObjTg.mSPrm;
    coming3Word(&mCyl.mGObjTg.mVec.x) = zeroX;
    coming3Word(&mCyl.mGObjTg.mVec.y) = (u32)*gabi::at<be<u32>>(0x101FFBAC);
    u32 zeroZ = (u32)*gabi::at<be<u32>>(0x101FFBB0);
    mCyl.mGObjTg.mSPrm = flags | 4;
    coming3Word(&mCyl.mGObjTg.mVec.z) = zeroZ;
}

/* 02115004 */
bool daComing3::Act_c::collision_main() {
    void *cyl = &mCyl;
    if (coming3CheckTargetHit(cyl)) {
        coming3ClearTargetHit(cyl);
        return true;
    }
    gabi::Local<cXyz> origin, center;
    origin->x = 0;
    origin->y = 0;
    origin->z = 0;
    coming3MatrixVector(&unk_410, origin.get(), center.get());
    coming3CylinderRadius(&mCyl.mCyl, 130.f);
    coming3CylinderHeight(&mCyl.mCyl, 225.f);
    coming3CylinderCenter(&mCyl.mCyl, center.get());
    coming3CollisionSet(dComIfG_Ccsp(), cyl);
    f32 y = center->y, x = center->x;
    eyePos.x = x;
    y += 50.f;
    eyePos.z = center->z;
    eyePos.y = y;
    return false;
}

/* 02114B28 */
// HD: free adapter for the static tww method until autoglue supports static methods.
u8 solidHeapCB(
    Act_c *a) { // HD: forward the unextended u8 result rather than normalizing it through bool.
    return coming3CreateHeapResult(a);
}

/* 02114A78 */
bool daComing3::Act_c::create_heap() {
    void *data = dComIfG_getObjectRes("Always", 35, 0x1000C9A0);
    if (!data)
        coming3Panic("d_a_coming3.cpp", 847, "mdl_data != 0");
    J3DModel *m = coming3CreateModel(data, 0, 0x11020203);
    unk_440 = m;
    return data && m;
}

/* 02114D90 */
s32 daComing3::Act_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & 8)) {
        // HD: preserve the inline constructor null-this guard and store order.
        if (this) {
            coming3ActorConstruct(this);
            __vtbl = COMING3_ACTOR_VTABLE;
            coming3SttsConstruct(&mStts);
            coming3GSttsConstruct(&mStts.__vtbl_gstts);
            mStts.__vtbl = COMING3_STTS_VTABLE;
            mStts.__vtbl_gstts = COMING3_GSTTS_VTABLE;
            coming3CollisionConstruct(&mCyl);
            coming3Ctor(this)->mShapeVtable = COMING3_SHAPE_BASE_VTABLE;
            coming3Ctor(this)->mShapeBaseVtable = COMING3_SHAPE_VTABLE;
            coming3CylinderConstruct(&mCyl.mCyl);
            mCyl.__vtbl_hitinf = COMING3_CYL_HIT_VTABLE;
            coming3Ctor(this)->mShapeVtable = COMING3_SHAPE_FINAL_VTABLE;
            flags = actor_condition;
            mCyl.mCyl.__vtbl = COMING3_CYL_VTABLE;
        }
        actor_condition = flags | 8;
    }
    s32 result = coming3ResourceLoad(&mPhase, "Always");
    if (result == 4) {
        if (coming3SolidHeap(this, COMING3_HEAP_CALLBACK, 0)) {
            unk_47E = (s16)(shape_angle.z + 1);
            collision_init();
            coming_process_init(0);
            set_mtx();
        } else
            result = 5;
    }
    return result;
}

/* 02114ED4 */
bool daComing3::Act_c::_delete() {
    coming3ResourceDelete(&mPhase, "Always");
    return true;
}

/* 02114F5C */
bool daComing3::Act_c::_execute() {
    coming_process_main();
    set_mtx();
    return true;
}

/* 02114F94 */
bool daComing3::Act_c::_draw() {
    s32 state = unk_404;
    if (state == 1 || state == 2) {
        void *env = dKy_getEnvlight();
        coming3SettingTevStruct(env, 0, &current.pos, &tevStr);
        env = dKy_getEnvlight();
        coming3SetLightTevColor(env, unk_440.get(), &tevStr);
        coming3ModelUpdate(unk_440.get(), 0);
    }
    return true;
}

/* 02115BBC */
s32 Mthd_Create(Act_c *a) { return a->_create(); }

/* 02115BC0 */
bool Mthd_Delete(Act_c *a) { return a->_delete(); }

/* 02115BC4 */
bool Mthd_Execute(Act_c *a) { return a->_execute(); }

/* 02115BC8 */
bool Mthd_Draw(Act_c *a) { return a->_draw(); }

/* 02115DD4 */
BOOL Mthd_IsDelete(Act_c *a) { return TRUE; }

/* 02115BCC */
void sinit() { sinit_header_statics_z(0x10462E70, 0x101B3D1C, 0x10462E7C); }

/* 02115C60 */
void objground_destructor(Coming3GroundHD *p, s32 flags) {
    if (p) {
        p->mCheckVtable = COMING3_GROUND_CHECK_BASE_VTABLE;
        p->mGroupVtable = COMING3_GROUND_GROUP_BASE_VTABLE;
        p->mPolyVtable = COMING3_POLY_BASE_VTABLE;
        coming3GroundDestroy(p, 0);
        if (flags & 1)
            coming3Delete(p);
    }
}

/* 02115CD8 */
void water_destructor(Coming3WaterHD *p, s32 flags) {
    if (p) {
        p->mVtable = COMING3_WATER_CHECK_BASE_VTABLE;
        p->mGroupVtable = COMING3_WATER_GROUP_BASE_VTABLE;
        p->mPolyVtable = COMING3_POLY_BASE_VTABLE;
        coming3CheckDestroy(gabi::at<u8>(gabi::ea(p) + 0x10), 0);
        if (flags & 1)
            coming3Delete(p);
    }
}

/* 02115D50 */
void safe_string_destructor(void *p, s32 flags) {
    if (p && (flags & 1))
        coming3Delete(p);
}

/* 02115D68 */
void actor_destructor(Act_c *a, s32 flags) {
    if (a) {
        coming3CollisionDestroy(&a->mCyl, 2);
        coming3SttsDestroy(&a->mStts, 2);
        coming3ActorDestroy(a, 0);
        if (flags & 1)
            coming3Delete(a);
    }
}

/* 02115D64 */
void coming3_SafeString_assureTermination(void *p) {}

// clang-format off
/* WWHD glue
ext 02114A78 u8 coming3CreateHeapResult(daComing3::Act_c*)
str 1000CBA8
str 1000CB88 in 021153B4
str 1000CB0C
str 1000CB1C
ext 0273AA24 void coming3Panic(const char*,s32,const char*)
ext 025E38E0 J3DModel* coming3CreateModel(void*,u32,u32)
ext 02515F14 void coming3SttsInit(void*,s32,s32,fopAc_ac_c*)
ext 02516518 void coming3CylSet(void*,const void*)
ext 025F1B48 void coming3MatrixZXYRotate(void*,s32,s32,s32)
ext 028E90D4 void coming3MatrixCopy(const void*,void*)
ext 027F4D5C void coming3ModelCalc(J3DModel*)
ext 025D4ED0 void coming3ActorConstruct(fopAc_ac_c*)
ext 0200BD2C void coming3SttsConstruct(void*)
ext 02515DA0 void coming3GSttsConstruct(void*)
ext 02515FB8 void coming3CollisionConstruct(void*)
ext 02018590 void coming3CylinderConstruct(void*)
ext 02520460 s32 coming3ResourceLoad(void*,const char*)
ext 025D63E8 s32 coming3SolidHeap(fopAc_ac_c*,u32,u32)
ext 025204C8 void coming3ResourceDelete(void*,const char*)
ext 025626A4 void coming3SettingTevStruct(void*,u32,const cXyz*,void*)
ext 02562F5C void coming3SetLightTevColor(void*,J3DModel*,void*)
ext 025E2DE0 void coming3ModelUpdate(J3DModel*,u32)
ext 025D54C4 s32 coming3SearchByID(u32,void*)
ext 025D57E0 void coming3ActorDelete(fopAc_ac_c*)
ext 02008DAC void coming3GroundDestroy(void*,s32)
ext 0273AF40 void coming3Delete(void*)
ext 02008B4C void coming3CheckDestroy(void*,s32)
ext 02515A70 void coming3CollisionDestroy(void*,s32)
ext 02515860 void coming3SttsDestroy(void*,s32)
ext 025D50BC void coming3ActorDestroy(fopAc_ac_c*,s32)
ext 025162A4 s32 coming3CheckTargetHit(void*)
ext 0251621C void coming3ClearTargetHit(void*)
ext 028E8F64 void coming3MatrixVector(const void*,const cXyz*,cXyz*)
ext 020184DC void coming3CylinderRadius(void*,f32)
ext 02018428 void coming3CylinderHeight(void*,f32)
ext 020182E0 void coming3CylinderCenter(void*,const cXyz*)
ext 0200E240 void coming3CollisionSet(void*,void*)
ext 025D4604 s32 coming3IsActor(fopAc_ac_c*)
ext 02008E0C void coming3GroundConstruct(void*)
ext 028F026C void coming3RegisterStatic(void*)
ext 024F22DC void coming3WaterConstruct(void*)
ext 0246B6A4 s32 coming3SeaCheckArea(f32,f32)
ext 0246BA0C f32 coming3SeaWave(f32,f32)
ext 024EF7C0 s32 coming3WaterCheck(void*,void*)
ext 024EECE8 s32 coming3RoomReadId(void*,void*)
ext 02008974 f32 coming3GroundCross(void*,void*)
ext 024EECAC s32 coming3MaterialSoundId(void*,void*)
ext 02520540 s32 coming3Reverb(s32)
ext 025E1A40 void coming3SoundStart(u32,const cXyz*,u32,s32)
ext 025A847C void* coming3ParticleSet(void*,u32,u32,const cXyz*,const csXyz*,const cXyz*,u32,void*,s32,const void*,const void*,u32)
ext 025A3BDC void* coming3ModelEmitterConstruct(void*,void*,void*,u32,void*,void*,u32,u32)
ext 0200FE78 void coming3AppendEmitter(void*,void*)
ext 028E8DE8 f32 coming3SquareDistance(const cXyz*,const cXyz*)
ext 028F4384 f32 coming3SquareRoot(f32)
ext 025D5218 void coming3ActorSearch(u32,void*)
ext 0201A478 void coming3AngleConstruct(csXyz*,s32,s32,s32)
ext 025D5834 u32 coming3ActorCreate(s32,u32,const cXyz*,s32,const csXyz*,void*,s32,u32)
*/
