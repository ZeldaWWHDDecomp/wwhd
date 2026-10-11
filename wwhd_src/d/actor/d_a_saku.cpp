/**
 * d_a_saku.cpp (WWHD)
 * Object - Brown wooden barricade
 *
 * Reconstructed for the WWHD layout from the
 * GameCube decompilation (zeldaret/tww src/d/actor/d_a_saku.cpp) and the HD code,
 * verified against cking.rpx.
 */
#include "d/actor/d_a_saku.h"
#include <cmath>

static const SakuTUData saku_tu = {
    .mCylinderBaseVtable = 0x100015A8,
    .mCylinderVtable = 0x100397AC,
    .mCollisionObjectVtable = 0x1004B108,
    .mCylinderShapeVtable = 0x1004B150,
    .mCylinderGeometryVtable = 0x1004B160,
    .mActorVtable = 0x100397BC,
    .mStatusBaseVtable = 0x1004AE88,
    .mStatusGameVtable = 0x1004AEC0,
    .mHeapAssertFile = 0x10039854,
    .mHeapAssertText = 0x10039834,
    .mSafeStringVtable = 0x10039794,
    .mModelAssertFile = 0x100397F8,
    .mModelAssertText = 0x10039808,
    .mCylinderSource = 0x10039888,
    .mMaterialAssertFile = 0x10039754,
    .mMaterialAssertText = 0x10039764,
    .mMainHeapName = 0x10039780,
    .mSpareHeapName = 0x10039788,
    .mHIOName = 0x10039790,
    .mHeaderDtorLink0 = 0x101CFE2C,
    .mHeaderDtorLink1 = 0x101CFE38,
    .mHeaderDtorLink2 = 0x101CFE44,
    .mHIOVtable = 0x100397CC,
};

static const u8 l_stateModelIndex[] = {0, 1, 2};
static sakuHIO_c l_sakuHIO;
static SakuColor dust_color;
static u32 l_save;
static u32 l_resourceManager;
static u32 l_arcname[3] = {};
static f32 l_calcMtx[12] = {};
static u32 l_drawList;
static u32 l_xluDrawList;
static SakuHeaderDefaults l_defaults;
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_headerGuard;
static u8 l_headerByte;


/* 02464018 */
void daSaku_c::CreateInit() {
    for (s32 half = 0; half < 2; ++half) {
        mParticleTimers[half] = 0;
        mpEmitter[half] = 0;
        modelAlpha[half][0] = 255;
        modelAlpha[half][1] = 0;
        collisionTimers[half] = -1;
        heapReleaseDelay[half] = 2;
    }
    u32 model = mModels[0][0];
    burning = 0;
    fireTimer = 0;
    cullMtx = model ? model + 0xC8 : 0;
    Saku_statusInit(&mStts, 255, 255, this);
    this->setCol();
    this->setMtx();
    for (s32 half = 0; half < 2; ++half) {
        for (u32 i = 0; i < 4; ++i)
            mSmokeCb[half].mColor[i] = reinterpret_cast<be<u8>*>(&dust_color)[i];
        mSmokeCb[half].mRateOff = 1;
    }
}

/* 024644DC */
s32 daSaku_c::saku_draw_sub(s32 half) {
    u8 threshold = l_sakuHIO.mDustA;
    u8 alpha = modelAlpha[half][0];
    s32 depthWrite = alpha >= threshold;
    if (m_heap[half][0] != 0 && mModels[half][0] != 0 && alpha != 0) {
        void* lighting = Saku_getEnvLight();
        Saku_settingTevStruct(lighting, 0, &current.pos, &tevStr);
        lighting = Saku_getEnvLight();
        u32 model = mModels[half][0];
        Saku_setLightTevColorType(lighting, Saku_guest<void>(model), &tevStr);
        alpha = modelAlpha[half][0]; model = mModels[half][0];
        matAlphaAnim(Saku_guest<void>(model), alpha, depthWrite);
        u32 game = Saku_getPlay();
        u32 list = Saku_guest<SakuPlayView>(game)->mpBGDraw;
        l_drawList = list;
        game = Saku_getPlay();
        list = Saku_guest<SakuPlayView>(game)->mpBGXluDraw;
        l_xluDrawList = list;
        model = mModels[half][0];
        Saku_modelUpdateDL(Saku_guest<void>(model), 0);
        game = Saku_getPlay();
        list = Saku_guest<SakuPlayView>(game)->mpDraw;
        l_drawList = list;
        game = Saku_getPlay();
        list = Saku_guest<SakuPlayView>(game)->mpXluDraw;
        l_xluDrawList = list;
        model = mModels[half][0];
        matAlphaAnim(Saku_guest<void>(model), 255, 1);
    }
    if (m_heap[half][1] != 0 && mModels[half][1] != 0 && modelAlpha[half][1] != 0) {
        void* lighting = Saku_getEnvLight();
        Saku_settingTevStruct(lighting, 0, &current.pos, &tevStr);
        lighting = Saku_getEnvLight();
        u32 model = mModels[half][1];
        Saku_setLightTevColorType(lighting, Saku_guest<void>(model), &tevStr);
        model = mModels[half][1];
        matAlphaAnim(Saku_guest<void>(model), 255, depthWrite ^ 1);
        model = mModels[half][1];
        Saku_modelUpdateDL(Saku_guest<void>(model), 0);
        model = mModels[half][1];
        matAlphaAnim(Saku_guest<void>(model), 255, 1);
    }
    return 1;
}

/* 02464D30 */
s32 daSaku_c::mode_break_none(s32 half) {
    u32 broke = 0;
    for (s32 i = 0; i < 3; ++i) {
        void* cylinder = &targetCylinders[half][i];
        if (!Saku_checkTargetHit(cylinder)) continue;
        void* hit = Saku_getTargetHit(cylinder);
        if (!hit) continue;
        bool alternate = mType != 0;
        u32 attack = static_cast<SakuAttackView*>(hit)->mAttackType;
        u32 mask = alternate ? 0x04000C20 : 0x14010C2A;
        broke |= (attack & mask) != 0;
        if (broke) {
            u32 game = Saku_getPlay();
            gabi::Local<cXyz> direction;
            direction->x = 0.0f; direction->y = 1.0f; direction->z = 0.0f;
            Saku_startShock(Saku_guest<SakuPlayView>(game)->mVibration, 4, -0x21, direction.get());
            attack = static_cast<SakuAttackView*>(hit)->mAttackType;
        }
        if (attack & 0x00060200) return this->burn();
    }
    if (broke) {
        if (half == 1 && this->state[0] == SAKU_INTACT) this->broken(0);
        return this->broken(half);
    }
    return 1;
}

/* 0246505C */
s32 daSaku_c::mode_break_fire(s32 half) {
    if (mParticleTimers[0] > 10) {
        u8 speed = reinterpret_cast<be<u8>*>(&l_sakuHIO.mAlphaSpeed)[1];
        Saku_chaseUC(&modelAlpha[half][1], 255, speed);
        speed = reinterpret_cast<be<u8>*>(&l_sakuHIO.mAlphaSpeed)[1];
        if (Saku_chaseUC(&modelAlpha[half][0], 0, speed)) {
            if (m_heap[half][0] != 0) {
                u8 delay = heapReleaseDelay[half];
                if (delay != 0) {
                    delay = u8(delay - 1);
                    heapReleaseDelay[half] = delay;
                    if (delay == 0) {
                        u32 heap = m_heap[half][0];
                        Saku_destroySolidHeap(Saku_guest<void>(heap));
                        m_heap[half][0] = 0;
                        mModels[half][0] = 0;
                    }
                }
            }
        }
    }
    return 1;
}

/* 02464F0C */
s32 daSaku_c::mode_break_throw_obj(s32 half) {
    if (m_heap[half][0] != 0 && m_heap[half][1] != 0) {
        u8 delay = heapReleaseDelay[half];
        if (delay != 0) {
            delay = u8(delay - 1);
            heapReleaseDelay[half] = delay;
            if (delay == 0) {
                u32 heap = m_heap[half][0];
                Saku_destroySolidHeap(Saku_guest<void>(heap));
                m_heap[half][0] = 0;
                mModels[half][0] = 0;
            }
        }
    }
    void* callback = &mSmokeCb[half];
    if (mParticleTimers[half] >= 10 && static_cast<SakuSmokeCallback*>(callback)->mpEmitter != 0) {
        u8 fade = l_sakuHIO.mStartAlpha;
        f32 step = f32(fade) / 10200.0f;
        Saku_chaseF(&smokeAlpha[half], 0.0f, step);
        f32 alpha = std::fabs(f32(smokeAlpha[half]));
        smokeAlpha[half] = alpha;
        u8 byteAlpha = u8(gabi::ftoi(alpha * 255.0f));
        u32 emitter = static_cast<SakuSmokeCallback*>(callback)->mpEmitter;
        Saku_guest<SakuEmitterView>(emitter)->mGlobalAlpha = byteAlpha;
        if (byteAlpha == 0) {
            u32 table = static_cast<SakuSmokeCallback*>(callback)->mVtable;
            u32 end = Saku_guest<SakuSmokeVtable>(table)->mEnd;
            gabi::call(end, callback);
            mpEmitter[half] = 0;
        }
    }
    return 1;
}

/* 024646A4 */
s32 daSaku_c::RecreateHeap(s32 heapId, s32 half) {
    u32 heap = m_heap[half][heapId];
    if (!heap) {
        Saku_assert(Saku_guest<void>(saku_tu.mHeapAssertFile), 0x38B, Saku_guest<void>(saku_tu.mHeapAssertText));
        heap = m_heap[half][heapId];
    }
    u32 table = Saku_guest<SakuHeapView>(heap)->mVtable;
    u32 freeAll = Saku_guest<SakuHeapVtable>(table)->mFreeAll;
    gabi::call(freeAll, Saku_guest<void>(heap));
    heap = m_heap[half][heapId];
    u32 previous = Saku_setCurrentHeap(Saku_guest<void>(heap));
    this->CreateHeap(1, half);
    Saku_setCurrentHeap(Saku_guest<void>(previous));
    return 1;
}

/* 02463A88 */
s32 daSaku_c::CreateHeap(s32 heap, s32 half) {
    u32 state = this->state[half];
    if (state < 1 || state > 3) return 0;
    u8 model = l_stateModelIndex[state - 1];
    if (!this->loadModel(model, heap, half)) return 0;
    s32 dzb = this->GetDzbId(half);
    return this->loadMoveBG(dzb, heap, half) != 0;
}

/* 024638F8 */
s32 daSaku_c::GetDzbId(s32 half) {
    s32 state = this->state[half];
    if (half == 1 || this->state[1] == SAKU_NONE) return state == 1 ? 0 : 1;
    if (state == 2 || state == 3) return 3;
    u32 save = l_save;
    s8 room = home.roomNo;
    u32 sw = mTopHalfDestroyedSwitch;
    return Saku_isSwitch(Saku_guest<SakuSaveView>(save)->mZone, sw, room) ? 4 : 2;
}

/* 02463C04 */
s32 daSaku_c::CreateDummyHeap(s32 half) {
    // HD: all nonzero types select the alternate model, including unrecognized types.
    s32 model = mType != 0;
    if (!this->loadModel(model, 1, half)) return 0;
    return this->loadMoveBG(1, 1, half) != 0;
}

/* 02463998 */
s32 daSaku_c::loadMoveBG(s32 index, s32 heap, s32 half) {
    const s32 resources[5] = {
        dRes_INDEX_KSAKUCO_DZB_KSAKU_00_e, dRes_INDEX_KSAKUCO_DZB_KSAKU_01_e,
        dRes_INDEX_KSAKUCO_DZB_KSAKU_02_e, dRes_INDEX_KSAKUCO_DZB_KSAKU_03_e,
        dRes_INDEX_KSAKUCO_DZB_KSAKU_00_e};
    u32 background = Saku_bgNew(0);
    backgrounds[half][heap] = background;
    if (!background) return 0;
    s32 resource = resources[index];
    u32 manager = l_resourceManager;
    u32 archive = l_arcname[0];
    gabi::Local<be<u32>[2]> name;
    (*name)[0] = archive; (*name)[1] = saku_tu.mSafeStringVtable;
    void* data = Saku_getObjectRes(Saku_guest<void>(manager), name.get(), resource);
    background = backgrounds[half][heap];
    return Saku_bgSet(Saku_guest<void>(background), data, 1,
                             &mMtx[half][0]) == 0;
}

/* 0246379C */
s32 daSaku_c::loadModel(s32 index, s32 heap, s32 half) {
    const s32 brown[6] = {
        dRes_INDEX_KSAKU_00_BDL_KSAKU_00_e, dRes_INDEX_KSAKU_00_BDL_KSAKU_04_e,
        dRes_INDEX_KSAKU_00_BDL_KSAKU_02_e, dRes_INDEX_KSAKU_00_BDL_KSAKU_01_e,
        dRes_INDEX_KSAKU_00_BDL_KSAKU_03_e, dRes_INDEX_KSAKU_00_BDL_KSAKU_05_e};
    const s32 pale[6] = {
        dRes_INDEX_KNSAK_00_BDL_KNSAK_00_e, dRes_INDEX_KNSAK_00_BDL_KNSAK_02_e,
        dRes_INDEX_KNSAK_00_BDL_KNSAK_01_e, dRes_INDEX_KNSAK_00_BDL_KNSAK_03_e,
        dRes_INDEX_KNSAK_00_BDL_KNSAK_05_e, dRes_INDEX_KNSAK_00_BDL_KNSAK_04_e};
    if (half == 1) index = s32(u32(index) + 3);
    bool alternate = mType != 0;
    u32 resourceManager = l_resourceManager;
    u32 archive = l_arcname[alternate ? 2 : 1];
    s32 resource = alternate ? pale[index] : brown[index];
    gabi::Local<be<u32>[2]> name;
    (*name)[0] = archive; (*name)[1] = saku_tu.mSafeStringVtable;
    void* data = Saku_getObjectRes(Saku_guest<void>(resourceManager), name.get(), resource);
    if (!data) Saku_assert(Saku_guest<void>(saku_tu.mModelAssertFile), 0x466, Saku_guest<void>(saku_tu.mModelAssertText));
    u32 model = Saku_modelCreate(data, 0, 0x11020203);
    mModels[half][heap] = model;
    return model != 0;
}

/* 02464850 */
s32 daSaku_c::burn() {
    if (burning == 0) {
        for (s32 half = 0; half < 2; ++half) {
            if (this->state[half] == SAKU_INTACT) {
                this->state[half] = SAKU_BURNING;
                this->RecreateHeap(1, half);
                collisionTimers[half] = 50;
            }
        }
        u32 model = mModels[0][1];
        if (!model) model = mModels[1][1];
        if (model) cullMtx = model + 0xC8;
        this->setEffFire(0);
        s8 room = home.roomNo;
        fireTimer = 90;
        u32 save = l_save;
        u32 sw = mBottomHalfDestroyedSwitch;
        Saku_onSwitch(Saku_guest<SakuSaveView>(save)->mZone, sw, room);
        if (this->state[1] != SAKU_NONE) {
            room = home.roomNo;
            save = l_save;
            sw = mTopHalfDestroyedSwitch;
            Saku_onSwitch(Saku_guest<SakuSaveView>(save)->mZone, sw, room);
        }
        burning = 1;
    }
    return 1;
}

/* 02464C34 */
s32 daSaku_c::broken(s32 half) {
    this->setEffBreak(half);
    collisionTimers[half] = 0;
    this->state[half] = SAKU_BROKEN;
    u32 save = l_save;
    s8 room = home.roomNo;
    u32 sw = half == 0 ? u32(mBottomHalfDestroyedSwitch) : u32(mTopHalfDestroyedSwitch);
    Saku_onSwitch(Saku_guest<SakuSaveView>(save)->mZone, sw, room);
    this->RecreateHeap(1, half);
    if (half == 0) {
        u32 model = mModels[0][1];
        cullMtx = model ? model + 0xC8 : 0;
    }
    modelAlpha[half][0] = 0;
    modelAlpha[half][1] = 255;
    return 1;
}

/* 02465138 */
s32 daSaku_c::changeCollision(s32 half) {
    if (this->state[half] == SAKU_NONE) return 0;
    s32 timer = collisionTimers[half];
    if (timer >= 0) {
        if (timer == 0) {
            u32 game = Saku_getPlay();
            u32 background = activeBackgrounds[half];
            Saku_bgRelease(Saku_guest<SakuPlayView>(game)->mBgS, Saku_guest<void>(background));
            this->MoveBGResist(1, half);
            timer = collisionTimers[half];
        }
        collisionTimers[half] = s32(u32(timer) - 1);
    }
    return 1;
}

/* 02463E40 */
void daSaku_c::setMtx() {
    void* matrix = l_calcMtx;
    for (s32 half = 0; half < 2; ++half) {
        if (half == 1 && this->state[1] == SAKU_NONE) break;
        for (s32 modelId = 0; modelId < 2; ++modelId) {
            u32 modelAddress = mModels[half][modelId];
            if (!modelAddress) continue;
            void* model = Saku_guest<void>(modelAddress);
            f32 sx = scale.x, sy = scale.y, sz = scale.z;
            static_cast<SakuModelView*>(model)->mScaleX = sx;
            if (half == 0) {
                static_cast<SakuModelView*>(model)->mScaleZ = sz; static_cast<SakuModelView*>(model)->mScaleY = sy;
            } else {
                static_cast<SakuModelView*>(model)->mScaleY = sy; static_cast<SakuModelView*>(model)->mScaleZ = sz;
            }
            f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
            if (half == 1) y += 200.0f;
            Saku_transS(matrix, x, y, z);
            s16 rx = shape_angle.x, ry = shape_angle.y, rz = shape_angle.z;
            Saku_ZXYrotM(matrix, rx, ry, rz);
            f32 values[12];
            for (u32 i = 0; i < 12; ++i) values[i] = static_cast<be<f32>*>(matrix)[i];
            // HD: each copy loads the entire matrix before its first store.
            const u8 lowerOrder[12] = {10,6,11,3,4,9,2,0,7,8,1,5};
            const u8 upperOrder[12] = {2,6,4,3,10,9,5,0,1,8,11,7};
            const u8* order = half == 0 ? lowerOrder : upperOrder;
            for (u32 i = 0; i < 12; ++i) {
                u32 component = order[i];
                static_cast<SakuModelView*>(model)->mBaseMtx[component] = values[component];
            }
        }
    }
}

/* 024636D8 */
void daSaku_c::setMoveBGMtx() {
    void* matrix = l_calcMtx;
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    Saku_transS(matrix, x, y, z);
    s16 yaw = shape_angle.y;
    Saku_YrotM(matrix, yaw);
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    Saku_scaleM(sx, sy, sz);
    Saku_mtxCopy(matrix, &mMtx[0][0]);
    if (this->state[1] != SAKU_NONE) {
        y = f32(current.pos.y) + 200.0f;
        x = current.pos.x; z = current.pos.z;
        Saku_transS(matrix, x, y, z);
        yaw = shape_angle.y;
        Saku_YrotM(matrix, yaw);
        sx = scale.x; sy = scale.y; sz = scale.z;
        Saku_scaleM(sx, sy, sz);
        Saku_mtxCopy(matrix, &mMtx[1][0]);
    }
}

/* 024651FC */
void daSaku_c::checkCol() {
    if (fireTimer != 0) {
        for (s32 i = 0; i < 3; ++i) {
            void* cylinder = &fireCylinders[i];
            Saku_cylinderSetC(static_cast<SakuCylinder*>(cylinder)->mGeometry, &mPos[0][i]);
            u32 game = Saku_getPlay();
            Saku_collisionSet(Saku_guest<SakuPlayView>(game)->mCcS, cylinder);
        }
    }
    for (s32 half = 0; half < 2; ++half) {
        if (this->state[half] != SAKU_INTACT) continue;
        for (s32 i = 0; i < 3; ++i) {
            void* cylinder = &targetCylinders[half][i];
            Saku_cylinderSetC(static_cast<SakuCylinder*>(cylinder)->mGeometry, &mPos[half][i]);
            u32 game = Saku_getPlay();
            Saku_collisionSet(Saku_guest<SakuPlayView>(game)->mCcS, cylinder);
        }
    }
}

/* 02463C94 */
void daSaku_c::setCol() {
    void* matrix = l_calcMtx;
    for (s32 half = 0; half < 2; ++half) {
        if (half == 1 && this->state[1] == SAKU_NONE) break;
        f32 height = half == 0 ? 20.0f : 220.0f;
        mPos[half][0].x = 0.0f;
        mPos[half][0].y = height;
        mPos[half][0].z = 0.0f;
        mPos[half][1].x = -100.0f;
        mPos[half][1].y = height;
        mPos[half][1].z = 0.0f;
        mPos[half][2].x = 100.0f;
        mPos[half][2].y = height;
        mPos[half][2].z = 0.0f;
        f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
        Saku_transS(matrix, x, y, z);
        s16 rx = shape_angle.x, ry = shape_angle.y, rz = shape_angle.z;
        Saku_ZXYrotM(matrix, rx, ry, rz);
        for (s32 i = 0; i < 3; ++i) {
            cXyz* position = &mPos[half][i];
            Saku_multVec(matrix, position, position);
            void* cylinder = &targetCylinders[half][i];
            Saku_setCylinder(cylinder, Saku_guest<void>(saku_tu.mCylinderSource));
            static_cast<SakuCylinder*>(cylinder)->mStatus = gabi::ea(&mStts);
        }
    }
}

/* 02463B54 */
s32 daSaku_c::MoveBGResist(s32 heap, s32 half) {
    u32 game = Saku_getPlay();
    u32 background = backgrounds[half][heap];
    if (Saku_bgRegist(Saku_guest<SakuPlayView>(game)->mBgS,
                         Saku_guest<void>(background), this)) return 0;
    background = backgrounds[half][heap];
    activeBackgrounds[half] = background;
    Saku_bgMove(Saku_guest<void>(background));
    return 1;
}

/* 02464758 */
s32 daSaku_c::setEffFire(s32 unused) {
    gabi::Local<cXyz> position;
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    position->x = x; position->z = z; position->y = y;
    u32 game = Saku_getPlay();
    u32 particles = Saku_guest<SakuPlayView>(game)->mpParticle;
    Saku_particleSet(Saku_guest<void>(particles), 0, 0x045C, position.get(),
               &current.angle, 0, 255, 0, -1, 0, 0, 0);
    game = Saku_getPlay();
    particles = Saku_guest<SakuPlayView>(game)->mpParticle;
    Saku_particleSet(Saku_guest<void>(particles), 0, 0x245E, position.get(),
               &current.angle, 0, 230, 0, -1, 0, 0, 0);
    s8 room = current.roomNo;
    mParticleTimers[0] = 1;
    mParticleTimers[1] = 1;
    u32 reverb = Saku_getReverb(room);
    Saku_seStart(0x6924, &eyePos, 0, reverb);
    return 1;
}

/* 02464980 */
s32 daSaku_c::setEffBreak(s32 half) {
    gabi::Local<cXyz> position;
    f32 y = f32(current.pos.y) + 100.0f;
    f32 z = current.pos.z, x = current.pos.x;
    position->z = z; position->x = x;
    if (half == 1) y += 200.0f;
    position->y = y;
    u8 debrisEnabled = l_sakuHIO.mDebrisEnabled;
    if (debrisEnabled) {
        u32 game = Saku_getPlay();
        u32 particles = Saku_guest<SakuPlayView>(game)->mpParticle;
        void* color = &static_cast<SakuActorColorView*>(static_cast<void*>(this))->mColorK0;
        Saku_particleSet(Saku_guest<void>(particles), 0, 0x045D, position.get(),
                   &current.angle, &scale, 255, 0, -1, color, color, 0);
        x = position->x; z = position->z; y = position->y;
    }
    u8 alpha = l_sakuHIO.mStartAlpha;
    smokeAlpha[half] = f32(alpha) / 255.0f;
    u8 red = l_sakuHIO.mDustR;
    u8 blue = l_sakuHIO.mDustB;
    u8 green = l_sakuHIO.mDustG;
    dust_color.r = red;
    dust_color.g = green;
    dust_color.b = blue;
    smokePositions[half].x = x;
    smokePositions[half].y = y;
    smokePositions[half].z = z;
    s8 room = current.roomNo;
    void* callback = &mSmokeCb[half];
    alpha = l_sakuHIO.mStartAlpha;
    u32 game = Saku_getPlay();
    u32 particles = Saku_guest<SakuPlayView>(game)->mpParticle;
    Saku_particleSet(Saku_guest<void>(particles), 2, 0x2027, &smokePositions[half],
               &current.angle, 0, alpha, callback, room, 0, 0, 0);
    if (static_cast<SakuSmokeCallback*>(callback)->mpEmitter != 0) {
        f32 intensity = smokeAlpha[half];
        u32 emitter = static_cast<SakuSmokeCallback*>(callback)->mpEmitter;
        Saku_guest<SakuEmitterView>(emitter)->mGlobalAlpha = u8(gabi::ftoi(intensity * 255.0f));
        emitter = static_cast<SakuSmokeCallback*>(callback)->mpEmitter;
        u32 mFlag = Saku_guest<SakuEmitterView>(emitter)->mStatus;
        Saku_guest<SakuEmitterView>(emitter)->mStatus = mFlag | 0x40;
        emitter = static_cast<SakuSmokeCallback*>(callback)->mpEmitter;
        Saku_guest<SakuEmitterView>(emitter)->mParticleX = 3.2f;
        Saku_guest<SakuEmitterView>(emitter)->mParticleY = 3.2f;
        Saku_guest<SakuEmitterView>(emitter)->mParticleZ = 1.0f;
        emitter = static_cast<SakuSmokeCallback*>(callback)->mpEmitter;
        Saku_guest<SakuEmitterView>(emitter)->mDynamicsX = 2.0f;
        Saku_guest<SakuEmitterView>(emitter)->mDynamicsY = 2.0f;
        Saku_guest<SakuEmitterView>(emitter)->mDynamicsZ = 2.0f;
        emitter = static_cast<SakuSmokeCallback*>(callback)->mpEmitter;
        Saku_guest<SakuEmitterView>(emitter)->mScaleX = 1.0f;
        Saku_guest<SakuEmitterView>(emitter)->mScaleY = 0.5f;
        Saku_guest<SakuEmitterView>(emitter)->mScaleZ = 0.7f;
        emitter = static_cast<SakuSmokeCallback*>(callback)->mpEmitter;
        Saku_guest<SakuEmitterView>(emitter)->mRate = 40.0f;
        emitter = static_cast<SakuSmokeCallback*>(callback)->mpEmitter;
        Saku_guest<SakuEmitterView>(emitter)->mMaxFrame = 1;
    }
    u8 type = this->mType;
    room = current.roomNo;
    u32 sound = type == 0 ? 0x6847 : 0x693F;
    u32 reverb = Saku_getReverb(room);
    Saku_seStart(sound, &eyePos, 0, reverb);
    mParticleTimers[half] = 1;
    return 1;
}

/* 02463408 */
s32 matAlphaAnim(void* model, u32 alpha, s32 depthWrite) {
    // HD: applies PE state and normalized shader colors through the HD material resource.
    // Keep full-register alpha and depthWrite parameters from the verified HD body.
    if (!model) Saku_assert(Saku_guest<void>(saku_tu.mMaterialAssertFile), 0x5FC, Saku_guest<void>(saku_tu.mMaterialAssertText));
    Saku_setModelXlu(model, alpha < 255 ? 1 : 0);
    u32 data = static_cast<SakuModelView*>(model)->mpModelData;
    void* resource = Saku_getNativeModelData(Saku_guest<void>(data));
    u16 count = static_cast<SakuNativeResourceView*>(resource)->mMaterialCount;
    for (u16 i = 0; i < count; i = u16(i + 1)) {
        data = static_cast<SakuModelView*>(model)->mpModelData;
        void* modelData = Saku_guest<void>(data);
        u32 materialCount = static_cast<SakuModelDataView*>(modelData)->mMaterialCount;
        u32 material = static_cast<SakuModelDataView*>(modelData)->mpMaterials;
        if (i < materialCount) material += u32(i) * 0x39C;
        void* node = Saku_guest<void>(material);
        u32 nativeData = static_cast<SakuMaterialView*>(node)->mpNativeData;
        s32 relative = Saku_guest<SakuNativeMaterialView>(nativeData)->mPixelEngineOffset;
        u32 pixelEngine = relative ? nativeData + 0x20 + u32(relative) : 0;
        void* pe = Saku_guest<void>(pixelEngine);
        Saku_setBlendMode(pe, 3);
        Saku_setBlendSrc(Saku_guest<SakuPixelEngineView>(pixelEngine)->mBlend, 4);
        Saku_setBlendSrcAlpha(Saku_guest<SakuPixelEngineView>(pixelEngine)->mBlend, 4);
        Saku_setBlendDst(Saku_guest<SakuPixelEngineView>(pixelEngine)->mBlend, 5);
        Saku_setBlendDstAlpha(Saku_guest<SakuPixelEngineView>(pixelEngine)->mBlend, 5);
        Saku_setZUpdate(Saku_guest<SakuPixelEngineView>(pixelEngine)->mZMode, depthWrite != 0);
        u32 tev = static_cast<SakuMaterialView*>(node)->mpTevBlock;
        u32 table = Saku_guest<SakuTevBlockView>(tev)->mVtable;
        u32 getColor = Saku_guest<SakuTevVtable>(table)->mGetKColor;
        void* color = gabi::call<void*>(getColor, Saku_guest<void>(tev), 3);
        static_cast<SakuColor*>(color)->a = alpha;
        tev = static_cast<SakuMaterialView*>(node)->mpTevBlock;
        table = Saku_guest<SakuTevBlockView>(tev)->mVtable;
        u32 setColor = Saku_guest<SakuTevVtable>(table)->mSetKColor;
        gabi::call(setColor, Saku_guest<void>(tev), 3, color);
        gabi::Local<be<f32>[4]> normalized;
        sakuColorToFloat(normalized.get(), color);
        gabi::Local<be<f32>[4]> converted;
        Saku_convertColor(converted.get(), normalized.get(), 1.0f);
        u32 flag = static_cast<SakuMaterialView*>(node)->mShaderFlags;
        static_cast<SakuMaterialView*>(node)->mShaderFlags = flag | 0x400;
        void* shaderColor = Saku_getShaderColor(&Saku_guest<SakuMaterialView>(material)->mShaderFlags, 10);
        u8 outputAlpha = static_cast<SakuColor*>(color)->a;
        f32 red = (*converted)[0], green = (*converted)[1], blue = (*converted)[2];
        static_cast<SakuFloatColor*>(shaderColor)->g = green;
        static_cast<SakuFloatColor*>(shaderColor)->b = blue;
        static_cast<SakuFloatColor*>(shaderColor)->r = red;
        static_cast<SakuFloatColor*>(shaderColor)->a = f32(outputAlpha) / 255.0f;
        data = static_cast<SakuModelView*>(model)->mpModelData;
        resource = Saku_getNativeModelData(Saku_guest<void>(data));
        count = static_cast<SakuNativeResourceView*>(resource)->mMaterialCount;
    }
    return 1;
}

/* 02464114 */
s32 daSaku_Create(daSaku_c* actor) {
    // HD: both solid heaps request size zero and receive distinct debug names.
    u32 condition = actor->actor_condition;
    if (!(condition & 8)) {
        if (actor) {
            sakuConstruct(actor);
            condition = actor->actor_condition;
        }
        actor->actor_condition = condition | 8;
    }
    u32 params = actor->mParameters;
    u8 mType = u8((params >> 4) & 15);
    actor->mType = mType;
    u32 archive = l_arcname[mType == 0 ? 1 : 2];
    s32 phase = Saku_resLoad(&actor->modelPhase, Saku_guest<void>(archive));
    if (phase != SAKU_PHASE_COMPLETE) return phase;
    archive = l_arcname[0];
    phase = Saku_resLoad(&actor->collisionPhase, Saku_guest<void>(archive));
    if (phase != SAKU_PHASE_COMPLETE) return phase;
    params = actor->mParameters;
    s8 room = actor->home.roomNo;
    actor->state[0] = SAKU_INTACT;
    actor->mTopHalfDestroyedSwitch = (params >> 16) & 255;
    u32 bottom = (params >> 8) & 255;
    actor->mBottomHalfDestroyedSwitch = bottom;
    u32 save = l_save;
    if (Saku_isSwitch(Saku_guest<SakuSaveView>(save)->mZone, bottom, room)) actor->state[0] = SAKU_BROKEN;
    actor->state[1] = SAKU_NONE;
    params = actor->mParameters;
    if (params & 15) {
        actor->state[1] = SAKU_INTACT;
        room = actor->home.roomNo;
        save = l_save;
        u32 top = actor->mTopHalfDestroyedSwitch;
        if (Saku_isSwitch(Saku_guest<SakuSaveView>(save)->mZone, top, room)) actor->state[1] = SAKU_BROKEN;
    }
    actor->setMoveBGMtx();
    for (s32 half = 0; half < 2; ++half) {
        if (actor->state[half] == SAKU_NONE) continue;
        u32 heap = Saku_createSolidHeap(0, 0x20);
        actor->m_heap[half][0] = heap;
        if (!heap) return SAKU_PHASE_ERROR;
        Saku_guest<SakuHeapView>(heap)->mName = saku_tu.mMainHeapName;
        s32 created = actor->CreateHeap(0, half);
        if (created) actor->MoveBGResist(0, half);
        Saku_restoreCurrentHeap();
        heap = actor->m_heap[half][0];
        Saku_adjustSolidHeap(Saku_guest<void>(heap));
        if (!created) return SAKU_PHASE_ERROR;
        actor->mModels[half][1] = 0;
        if (actor->state[half] != SAKU_INTACT) continue;
        heap = Saku_createSolidHeap(0, 0x20);
        actor->m_heap[half][1] = heap;
        if (!heap) return SAKU_PHASE_ERROR;
        Saku_guest<SakuHeapView>(heap)->mName = saku_tu.mSpareHeapName;
        created = actor->CreateDummyHeap(half);
        actor->mModels[half][1] = 0;
        Saku_restoreCurrentHeap();
        heap = actor->m_heap[half][1];
        Saku_adjustSolidHeap(Saku_guest<void>(heap));
        if (!created) return SAKU_PHASE_ERROR;
    }
    actor->CreateInit();
    s8 child = l_sakuHIO.mChild;
    if (child < 0) {
        s32 id = Saku_createHIOChild(Saku_guest<void>(saku_tu.mHIOName), &l_sakuHIO);
        l_sakuHIO.mChild = s8(id);
    }
    return SAKU_PHASE_COMPLETE;
}

/* 024643B0 */
s32 daSaku_Delete(daSaku_c* actor) {
    s8 child = l_sakuHIO.mChild;
    if (child >= 0) {
        Saku_deleteHIOChild(child);
        l_sakuHIO.mChild = -1;
    }
    for (s32 half = 0; half < 2; ++half) {
        void* callback = &actor->mSmokeCb[half];
        u32 table = static_cast<SakuSmokeCallback*>(callback)->mVtable;
        u32 end = Saku_guest<SakuSmokeVtable>(table)->mEnd;
        gabi::call(end, callback);
    }
    for (s32 half = 0; half < 2; ++half) {
        if (actor->state[half] != SAKU_NONE) {
            u32 game = Saku_getPlay();
            u32 background = actor->activeBackgrounds[half];
            Saku_bgRelease(Saku_guest<SakuPlayView>(game)->mBgS, Saku_guest<void>(background));
        }
    }
    for (s32 half = 0; half < 2; ++half) {
        for (s32 heapId = 0; heapId < 2; ++heapId) {
            u32 heap = actor->m_heap[half][heapId];
            if (heap != 0) {
                Saku_destroySolidHeap(Saku_guest<void>(heap));
                actor->m_heap[half][heapId] = 0;
                actor->mModels[half][heapId] = 0;
            }
        }
    }
    u32 archive = l_arcname[0];
    Saku_resDelete(&actor->collisionPhase, Saku_guest<void>(archive));
    bool alternate = actor->mType != 0;
    archive = l_arcname[alternate ? 2 : 1];
    Saku_resDelete(&actor->modelPhase, Saku_guest<void>(archive));
    return 1;
}

/* 02465608 */
s32 daSaku_IsDelete(daSaku_c* actor) {
    return 1;
}

/* 02464658 */
s32 daSaku_Draw(daSaku_c* actor) {
    actor->saku_draw_sub(0);
    if (actor->state[1] != SAKU_NONE) actor->saku_draw_sub(1);
    return 1;
}

/* 02465310 */
s32 daSaku_Execute(daSaku_c* actor) {
    for (s32 half = 0; half < 2; ++half) {
        s32 timer = actor->mParticleTimers[half];
        if (timer != 0 && timer < SAKU_MAX_PARTICLE_TIMER) actor->mParticleTimers[half] = s32(u32(timer) + 1);
    }
    s32 fire = actor->fireTimer;
    u32 age = actor->collisionTimers[2];
    if (fire != 0) actor->fireTimer = s32(u32(fire) - 1);
    actor->collisionTimers[2] = s32(age + 1);
    for (s32 half = 0; half < 2; ++half) {
        switch (s32(actor->state[half])) {
        case 1: actor->mode_break_none(half); break;
        case 2: actor->mode_break_fire(half); break;
        case 3: actor->mode_break_throw_obj(half); break;
        }
    }
    for (s32 half = 0; half < 2; ++half) actor->changeCollision(half);
    actor->setMtx();
    actor->checkCol();
    return 1;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0246334C */
u32 sakuSmokeConstruct(void* callback) {
    return Saku_smokeCallbackCtor(callback, 1);
}

/* 02463354 */
void sakuColorToFloat(void* output, void* color) {
    // HD: read all components before publishing the result, as in the HD body.
    f32 r = f32(u8(static_cast<SakuColor*>(color)->r)) / 255.0f;
    f32 g = f32(u8(static_cast<SakuColor*>(color)->g)) / 255.0f;
    f32 b = f32(u8(static_cast<SakuColor*>(color)->b)) / 255.0f;
    f32 a = f32(u8(static_cast<SakuColor*>(color)->a)) / 255.0f;
    static_cast<SakuFloatColor*>(output)->r = r;
    static_cast<SakuFloatColor*>(output)->g = g;
    static_cast<SakuFloatColor*>(output)->b = b;
    static_cast<SakuFloatColor*>(output)->a = a;
}

/* 02465610 */
void sakuSmokeDestruct(void* callback, s32 mFlag) {
    if (callback && (mFlag & 1)) Saku_delete(callback);
}

/* 024656E8 */
void sakuEmptyVirtual(void* actor) {
}

/* 02465568 */
void* sakuCylinderConstruct(void* object) {
    if (!object) object = Saku_new(0x130);
    if (object) {
        Saku_collisionObjectCtor(object);
        static_cast<SakuCylinder*>(object)->mGeometryVtable = saku_tu.mCylinderBaseVtable;
        static_cast<SakuCylinder*>(object)->mCylinderVtable = saku_tu.mCylinderVtable;
        Saku_cylinderGeometryCtor(static_cast<SakuCylinder*>(object)->mGeometry);
        static_cast<SakuCylinder*>(object)->mObjectVtable = saku_tu.mCollisionObjectVtable;
        static_cast<SakuCylinder*>(object)->mShapeVtable = saku_tu.mCylinderShapeVtable;
        static_cast<SakuCylinder*>(object)->mGeometryVtable = saku_tu.mCylinderGeometryVtable;
    }
    return object;
}

/* 024655F4 */
void sakuCylinderDestruct(void* object, s32 mFlag) {
    if (object && (mFlag & 1)) Saku_delete(object);
}

/* 02463610 */
void* sakuConstruct(daSaku_c* actor) {
    if (!actor) actor = static_cast<daSaku_c*>(Saku_new(0x1024));
    if (actor) {
        Saku_actorCtor(actor);
        actor->__vtbl = saku_tu.mActorVtable;
        Saku_constructArray(&actor->mSmokeCb[0], 2, 0x20, gabi::guest_fn((const void*)&sakuSmokeConstruct));
        Saku_statusBaseCtor(&actor->mStts);
        Saku_statusCtor(&actor->mStts.mGameVtable);
        actor->mStts.mBaseVtable = saku_tu.mStatusBaseVtable;
        actor->mStts.mGameVtable = saku_tu.mStatusGameVtable;
        Saku_constructArray(&actor->targetCylinders[0][0], 6, 0x130, gabi::guest_fn((const void*)&sakuCylinderConstruct));
        Saku_constructArray(&actor->fireCylinders[0], 3, 0x130, gabi::guest_fn((const void*)&sakuCylinderConstruct));
    }
    return actor;
}

/* 02465624 */
void sakuDestruct(daSaku_c* actor, s32 mFlag) {
    if (!actor) return;
    Saku_destroyArray(&actor->fireCylinders[0], 3, 0x130, SakuCylinderDestructorAddress, 0, 0);
    Saku_destroyArray(&actor->targetCylinders[0][0], 6, 0x130, SakuCylinderDestructorAddress, 0, 0);
    Saku_statusDtor(&actor->mStts, 2);
    Saku_destroyArray(&actor->mSmokeCb[0], 2, 0x20, gabi::guest_fn((const void*)&sakuSmokeDestruct), 0, 0);
    Saku_actorDtor(actor, 0);
    if (mFlag & 1) Saku_delete(actor);
}

/* 02465460 */
void __sinit_d_a_saku_cpp() {
    void* defaults = &l_defaults;
    static_cast<SakuHeaderDefaults*>(defaults)->mWord2 = 0;
    static_cast<SakuHeaderDefaults*>(defaults)->mWord0 = 0;
    static_cast<SakuHeaderDefaults*>(defaults)->mWord3 = 0;
    static_cast<SakuHeaderDefaults*>(defaults)->mWord1 = 0;
    Saku_registerDestructor(Saku_guest<void>(saku_tu.mHeaderDtorLink0));
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    Saku_headerGuardCtor(&l_headerGuard);
    Saku_registerDestructor(Saku_guest<void>(saku_tu.mHeaderDtorLink1));
    Saku_headerByteCtor(&l_headerByte);
    Saku_registerDestructor(Saku_guest<void>(saku_tu.mHeaderDtorLink2));
    void* hio = &l_sakuHIO;
    static_cast<sakuHIO_c*>(hio)->mDebugHeight = 70;
    static_cast<sakuHIO_c*>(hio)->mDebugWidth = 70;
    static_cast<sakuHIO_c*>(hio)->mDebugDepth = 65;
    static_cast<sakuHIO_c*>(hio)->mDebugMode = 7;
    static_cast<sakuHIO_c*>(hio)->mVtable = saku_tu.mHIOVtable;
    static_cast<sakuHIO_c*>(hio)->mChild = -1;
    static_cast<sakuHIO_c*>(hio)->mEnabled = 1;
    static_cast<sakuHIO_c*>(hio)->mDebrisEnabled = 1;
    static_cast<sakuHIO_c*>(hio)->mStartAlpha = 180;
    static_cast<sakuHIO_c*>(hio)->mDustR = 105;
    static_cast<sakuHIO_c*>(hio)->mDustG = 91;
    static_cast<sakuHIO_c*>(hio)->mDustB = 48;
    static_cast<sakuHIO_c*>(hio)->mAlphaSpeed = 5;
    static_cast<sakuHIO_c*>(hio)->mDustA = 100;
}

/* WWHD glue (tools/verify/autoglue.py): actor-local ABI bindings
data l_stateModelIndex 1003981C
data l_sakuHIO 1046D7BC nocheck
data dust_color 101CFE50 nocheck
data l_save 101F84DC nocheck
data l_resourceManager 101F4F28 nocheck
data l_arcname 101CFE84 nocheck
data l_calcMtx 1048D0CC nocheck
data l_drawList 104B4634 nocheck
data l_xluDrawList 104B4638 nocheck
data l_defaults 1046D7D4 nocheck
data l_minAngle 1046D7B0 nocheck
data l_maxAngle 1046D7B4 nocheck
data l_headerGuard 1046D7B8 nocheck
data l_headerByte 1046D7B9 nocheck
ext 020087EC void Saku_bgRelease(void*, void*)
ext 0200A030 s32 Saku_bgSet(void*, void*, s32, void*)
ext 0200BD2C void Saku_statusBaseCtor(void*)
ext 0200E240 void Saku_collisionSet(void*, void*)
ext 0200F4FC s32 Saku_chaseUC(be<u8>*, s32, s32)
ext 0200F5C8 s32 Saku_chaseF(be<f32>*, f32, f32)
ext 020182E0 void Saku_cylinderSetC(void*, const cXyz*)
ext 02018590 void Saku_cylinderGeometryCtor(void*)
ext 024EEA6C s32 Saku_bgRegist(void*, void*, daSaku_c*)
ext 024F23F4 u32 Saku_bgNew(s32)
ext 024F43DC void Saku_bgMove(void*)
ext 02515860 void Saku_statusDtor(void*, s32)
ext 02515DA0 void Saku_statusCtor(void*)
ext 02515F14 void Saku_statusInit(void*, s32, s32, daSaku_c*)
ext 02515FB8 void Saku_collisionObjectCtor(void*)
ext 025162A4 s32 Saku_checkTargetHit(void*)
ext 02516300 void* Saku_getTargetHit(void*)
ext 02516518 void Saku_setCylinder(void*, const void*)
ext 025200D4 u32 Saku_getPlay()
ext 02520460 s32 Saku_resLoad(request_of_phase_process_class*, void*)
ext 025204C8 void Saku_resDelete(request_of_phase_process_class*, void*)
ext 02520540 u32 Saku_getReverb(s32)
ext 02555D0C void* Saku_getEnvLight()
ext 025626A4 void Saku_settingTevStruct(void*, s32, const cXyz*, void*)
ext 02562F5C void Saku_setLightTevColorType(void*, void*, void*)
ext 025A5B18 u32 Saku_smokeCallbackCtor(void*, s32)
ext 025A847C void Saku_particleSet(void*, s32, s32, const cXyz*, const csXyz*, const cXyz*, s32, void*, s32, void*, void*, void*)
ext 025B9E38 void Saku_onSwitch(void*, u32, s32)
ext 025BA0C0 s32 Saku_isSwitch(void*, u32, s32)
ext 025CB374 void Saku_startShock(void*, s32, s32, const cXyz*)
ext 025D4ED0 void Saku_actorCtor(daSaku_c*)
ext 025D50BC void Saku_actorDtor(daSaku_c*, s32)
ext 025E1A40 void Saku_seStart(u32, const cXyz*, s32, u32)
ext 025E2DE0 void Saku_modelUpdateDL(void*, s32)
ext 025E3570 u32 Saku_setCurrentHeap(void*)
ext 025E3630 u32 Saku_createSolidHeap(s32, s32)
ext 025E3678 void Saku_adjustSolidHeap(void*)
ext 025E37D8 void Saku_restoreCurrentHeap()
ext 025E3868 void Saku_destroySolidHeap(void*)
ext 025E38E0 u32 Saku_modelCreate(void*, s32, u32)
ext 025F0A10 s32 Saku_createHIOChild(void*, void*)
ext 025F0A18 void Saku_deleteHIOChild(s32)
ext 025F1B48 void Saku_ZXYrotM(void*, s16, s16, s16)
ext 025F1C28 void Saku_YrotM(void*, s16)
ext 025F2518 void Saku_scaleM(f32, f32, f32)
ext 026066C4 void* Saku_getObjectRes(void*, void*, s32)
ext 0273AA24 void Saku_assert(void*, s32, void*)
ext 0273AD10 void* Saku_new(s32)
ext 0273AF40 void Saku_delete(void*)
ext 0274D458 void Saku_convertColor(void*, void*, f32)
ext 027E18F8 void Saku_setZUpdate(void*, bool)
ext 027E19B4 void Saku_setBlendSrc(void*, s32)
ext 027E19C4 void Saku_setBlendDst(void*, s32)
ext 027E19E4 void Saku_setBlendSrcAlpha(void*, s32)
ext 027E19F4 void Saku_setBlendDstAlpha(void*, s32)
ext 027E212C void Saku_setBlendMode(void*, s32)
ext 027F3F8C void* Saku_getNativeModelData(void*)
ext 027F58E0 void Saku_setModelXlu(void*, s32)
ext 027F9F0C void* Saku_getShaderColor(void*, s32)
ext 028E8F64 void Saku_multVec(void*, cXyz*, cXyz*)
ext 028E90D4 void Saku_mtxCopy(void*, void*)
ext 028E93CC void Saku_transS(void*, f32, f32, f32)
ext 028EAB2C void Saku_headerByteCtor(void*)
ext 028ED6F8 void Saku_headerGuardCtor(void*)
ext 028EFFD0 void Saku_constructArray(void*, s32, s32, u32)
ext 028F0164 void Saku_destroyArray(void*, s32, s32, u32, s32, s32)
ext 028F026C void Saku_registerDestructor(void*)
*/
