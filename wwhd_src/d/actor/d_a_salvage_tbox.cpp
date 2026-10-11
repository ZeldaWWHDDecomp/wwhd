/* Salvaged treasure chest: tww actor/event flow and HD particle ribbon rendering.
 * Each marked function preserves the verified WWHD behavior.
 */
#include "d/actor/d_a_salvage_tbox.h"
using STBoxWord = be<u32>;
using STBoxHalf = be<s16>;
using STBoxFloat = be<f32>;
using STBoxByte = be<u8>;
using STBoxSignedByte = be<s8>;
static STBoxWord l_resourceControl;
static STBoxWord l_save;
static STBoxTable_m_bdlidx m_bdlidx;
static STBoxTable_m_heapsize m_heapsize;
static STBoxTable_crane_offset crane_offset;
static Mtx34 l_calc_mtx;
static STBoxTable_action_table action_table;
static STBoxTable_event_init_tbl event_init_tbl;
static STBoxTable_event_action_tbl event_action_tbl;
static STBoxSignedByte l_stayRoom;
static STBoxByte l_particleStatus;
static STBoxWord l_drawMode;
static STBoxWord l_drawIndex;
static STBoxWord l_drawCount;
static STBoxHeaderZeros l_headerZeros;
static STBoxFloat l_piLow;
static STBoxFloat l_piHigh;
static STBoxByte l_angle;
static STBoxByte l_circle;
static STBoxTable_l_headerDtor l_headerDtor;
static STBoxShadowTexture l_shadowTexture;
static STBoxFloat l_piLowConst;
static STBoxFloat l_piHighConst;
static STBoxFloat l_scrollConst;
static STBoxFloat l_scaleConst;
static STBoxFloat l_depthConst;

cXyz *STBox_vectorConstructor(cXyz *);
void STBox_matrixCopy(Mtx34 *, Mtx34 *);
s32 CheckCreateHeap(daSTBox_c *);
namespace {
u8 *ptr(u32 p) { return gabi::at<u8>(p); }
void copyFloat3(u32 dst, u32 src) {
    f32 x = gabi::at<cXyz>(src)->x, y = gabi::at<cXyz>(src)->y, z = gabi::at<cXyz>(src)->z;
    gabi::at<cXyz>(dst)->x = x;
    gabi::at<cXyz>(dst)->y = y;
    gabi::at<cXyz>(dst)->z = z;
}
void invalidate(STBoxEmitter *emitter) {
    u32 flags = (u32)emitter->mStatus;
    emitter->mFrameCount = (s32)(-1);
    emitter->mStatus = (u32)(flags | STBOX_INVALID);
}
void clearEmitters(daSTBox_c *i_this) {
    for (int i = 0; i < 3; i++) {
        auto *e = static_cast<STBoxEmitter *>((void *)i_this->field_0x29C[i]);
        if (e) {
            e->mStatus = (u32)((u32)e->mStatus & ~u32(STBOX_IMMORTAL));
            e = static_cast<STBoxEmitter *>((void *)i_this->field_0x29C[i]);
            invalidate(e);
            i_this->field_0x29C[i] = 0;
        }
    }
}
void removeShadow(daSTBox_c *i_this) {
    auto *e = static_cast<STBoxEmitter *>((void *)i_this->mShadowCallback.mpEmitter);
    if (e) {
        e->mpCallBack = (u32)(0);
        invalidate(static_cast<STBoxEmitter *>((void *)i_this->mShadowCallback.mpEmitter));
    }
    i_this->mShadowCallback.mpEmitter = 0;
}
} // namespace

namespace {
void *particleSet(daSTBox_c *i_this, u32 group, u32 id, u32 pos, u32 angle, u32 scale, u32 alpha,
                  u32 callback) {
    u32 play = stboxGetPlay();
    u32 control = (u32)gabi::at<STBoxPlay>(play)->mpParticleControl;
    return stboxParticleSet(ptr(control), group, id, ptr(pos), ptr(angle), ptr(scale), alpha,
                            ptr(callback), -1, 0, 0, 0);
}
void setupEmitter(daSTBox_c *i_this, int index, f32 scale, f32 y) {
    auto *e = static_cast<STBoxEmitter *>((void *)i_this->field_0x29C[index]);
    if (!e)
        return;
    e->mStatus = (u32)((u32)e->mStatus | STBOX_IMMORTAL);
    e = static_cast<STBoxEmitter *>((void *)i_this->field_0x29C[index]);
    e->mGlobalParticleScale.x = (f32)(1.5f);
    e->mGlobalParticleScale.y = (f32)(1.5f);
    e->mGlobalParticleScale.z = (f32)(1.0f);
    e = static_cast<STBoxEmitter *>((void *)i_this->field_0x29C[index]);
    e->mEmitterScale.x = (f32)(scale);
    e->mEmitterScale.y = (f32)(1.0f);
    e->mEmitterScale.z = (f32)(scale);
    e = static_cast<STBoxEmitter *>((void *)i_this->field_0x29C[index]);
    e->mEmitterTranslation.x = (f32)(0.0f);
    e->mEmitterTranslation.z = (f32)(0.0f);
    e->mEmitterTranslation.y = (f32)(y);
}
s32 eventMember(u32 table, u32 actor, s32 staff) {
    auto *entry = gabi::at<STBoxMember>(table);
    s32 delta = entry->mThisDelta, slot = entry->mVirtualSlot;
    u32 self = actor + delta;
    u32 target;
    if (slot < 0)
        target = (u32)entry->mTarget;
    else {
        u32 v = (u32)*gabi::at<be<u32>>(self + (s32)(s16)(u32)entry->mTarget);
        target = (u32)gabi::at<STBoxMember>(v + slot * 8)->mTarget;
    }
    return gabi::call_ptr<s32>(target, ptr(self), staff);
}
} // namespace

namespace {
u32 visibleParticles(STBoxEmitter *emitter) {
    if ((u32)emitter->mParticleCount < 6)
        return 0;
    u32 n = 0;
    for (u32 link = (u32)emitter->mParticleFirst; link;
         link = (u32)gabi::at<STBoxParticleLink>(link)->mpNext) {
        u32 particle = (u32)gabi::at<STBoxParticleLink>(link)->mpObject;
        if ((u8)gabi::at<STBoxParticle>(particle)->mHidden == 0)
            n++;
    }
    return n;
}
u32 third(u32 n) {
    f32 f = (f32)n * 0.3333333432674408f;
    return f < 2147483648.0f ? (u32)gabi::ftoi(f)
                             : (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}

void depthState(bool after) {
    gabi::Local<STBoxDepthState> state;
    stboxDepthConstruct(state.get());
    if (after)
        state->mFlags = (u32)state->mFlags | 2;
    state->mDepthWrite = 0;
    state->mDepthFunction = 0;
    state->mDepthTest = 0;
    stboxDepthApply(state.get());
}
} // namespace

/* 02468840 */
void daSTBox_shadowEcallBack_c::getMaxWaterY(cXyz *p) {
    f32 x = p->x, z = p->z;
    if (stboxSeaCheckArea(x, z)) {
        x = p->x;
        z = p->z;
        f32 wave = stboxSeaCalcWave(x, z);
        f32 y = gabi::fadds_ppc(wave, 2.0f);
        p->y = y;
        f32 flat = mWaterFlatY;
        if (flat > y)
            p->y = gabi::fadds_ppc(flat, 2.0f);
    } else {
        f32 flat = mWaterFlatY;
        if (flat != -1000000000.0f)
            p->y = gabi::fadds_ppc(flat, 2.0f);
        else
            p->y = mWaterY;
    }
}

/* 024688F4 */
void daSTBox_shadowEcallBack_c::execute(void *emitter) {
    auto *e = static_cast<STBoxEmitter *>(emitter);
    struct Color {
        be<u8> r, g, b, a;
    };
    gabi::Local<Color> amb, diff;
    stboxGetSeaColor(amb.get(), diff.get());
    e->mGlobalEnvColor.r = (u8)(diff->r);
    e->mGlobalEnvColor.g = (u8)(diff->g);
    e->mGlobalEnvColor.b = (u8)(diff->b);
    e->mGlobalPrmColor.r = (u8)(amb->r);
    e->mGlobalPrmColor.g = (u8)(amb->g);
    e->mGlobalPrmColor.b = (u8)(amb->b);
    if (field_0x4) {
        invalidate(e);
        mpEmitter = 0;
    }
    if ((s32)e->mFrameCount == 0 && field_0x4 == 0) {
        f32 x = mPos.x, y = mPos.y, z = mPos.z;
        u32 kind = (u8)e->mEmitterKind;
        e->mGlobalTranslation.x = (f32)(x);
        e->mGlobalTranslation.y = (f32)(y);
        e->mGlobalTranslation.z = (f32)(z);
        if (kind >= 7)
            e->mGlobalTranslation.y = (f32)(-(f32)e->mGlobalTranslation.y);
        f32 direction = field_0x50;
        u32 angle = gabi::ea((csXyz *)mpAngle);
        s32 yaw = (s16)gabi::at<csXyz>(angle)->y;
        if (direction < 0)
            yaw = (s16)((u32)yaw + 0x8000);
        stboxRotationMatrix(0, yaw, 0, &e->mGlobalRotation);
        f32 depth = mDepth;
        if (depth < 0.0f || depth > 2000.0f)
            e->mGlobalPrmColor.a = (u8)(0);
        else {
            f32 alpha = (120.0f * gabi::fsubs_ppc(2000.0f, depth)) / 2000.0f;
            s32 value = gabi::ftoi(alpha);
            // HD: the shadow also fades close to the surface.
            if (depth < 160.0f)
                value = gabi::ftoi(0.75f * depth);
            e->mGlobalPrmColor.a = (u8)(value);
        }
    } else {
        u32 kind = (u8)e->mEmitterKind;
        f32 x = (f32)e->mGlobalTranslation.x, y = mWaterY;
        e->mGlobalTranslation.x = (f32)(x);
        u32 alpha = (u8)e->mGlobalPrmColor.a;
        e->mGlobalTranslation.y = (f32)(y);
        if (kind >= 7)
            e->mGlobalTranslation.y = (f32)(-(f32)e->mGlobalTranslation.y);
        gabi::Local<be<s16>> fade;
        *fade = alpha;
        stboxChaseS(fade.get(), 0, 5);
        e->mGlobalPrmColor.a = (u8)(255);
        *fade = 255;
    }
    u32 link = (u32)e->mParticleFirst;
    while (link) {
        u32 particle = (u32)gabi::at<STBoxParticleLink>(link)->mpObject;
        f32 x = (f32)gabi::at<STBoxParticle>(particle)->mOffsetPosition.x;
        u32 next = (u32)gabi::at<STBoxParticleLink>(link)->mpNext;
        gabi::Local<cXyz> pos;
        pos->x = x;
        pos->y = (f32)gabi::at<STBoxParticle>(particle)->mOffsetPosition.y;
        pos->z = (f32)gabi::at<STBoxParticle>(particle)->mOffsetPosition.z;
        getMaxWaterY(pos.get());
        gabi::at<STBoxParticle>(particle)->mOffsetPosition.x = (f32)((f32)pos->x);
        gabi::at<STBoxParticle>(particle)->mOffsetPosition.y = (f32)((f32)pos->y);
        gabi::at<STBoxParticle>(particle)->mOffsetPosition.z = (f32)((f32)pos->z);
        link = next;
    }
}

/* 02468C20 */
void daSTBox_shadowEcallBack_c::draw(void *emitter) {
    // HD: count visible particles and build GX2 ribbon buffers instead of GX immediate vertices.
    auto *e = static_cast<STBoxEmitter *>(emitter);
    u32 count = visibleParticles(e);
    if (count < 6)
        return;
    if ((u8)l_particleStatus & 1)
        depthState(false);
    u32 steps = third(count);
    f32 increment = 1.0f / (f32)(steps - 1);
    gabi::Local<Mtx34> matrix;
    stboxMtxIdentity(matrix.get());
    f32 scroll = mExTransY, frame = (f32)e->mFrame, scale = mExScaleY;
    matrix->m[1][1] = scale;
    matrix->m[1][3] = frame * scroll;
    u32 first = (u32)e->mParticleFirst,
        particle = (u32)gabi::at<STBoxParticleLink>(first)->mpObject;
    gabi::Local<Mtx34> copy;
    STBox_matrixCopy(copy.get(), matrix.get());
    u32 material = (u32)gabi::at<STBoxParticle>(particle)->mpMaterial;
    stboxSetTextureMatrix(ptr(material), copy.get(), 1);
    u32 shadowEmitter = gabi::ea((void *)mpEmitter);
    stboxParticleMaterialSetup(ptr(particle), ptr(shadowEmitter));
    u32 link = (u32)e->mParticleFirst;
    f32 textureY = 0;
    for (u32 step = 0; step < steps; step++) {
        if (step == 0) {
            for (int j = 0; j < 3; j++) {
                u32 pt = (u32)gabi::at<STBoxParticleLink>(link)->mpObject;
                copyFloat3(gabi::ea(&field_0x14[j]),
                           gabi::ea(&gabi::at<STBoxParticle>(pt)->mGlobalPosition));
                link = (u32)gabi::at<STBoxParticleLink>(link)->mpNext;
            }
        } else {
            shadowEmitter = gabi::ea((void *)mpEmitter);
            u32 buffers = (u32)gabi::at<STBoxEmitter>(shadowEmitter)->mpRibbonBuffers;
            u32 active = (u32)gabi::at<STBoxRibbonBuffers>(buffers)->mActiveBank;
            u32 record = buffers + (active + 2 * step - 2) * 0x254;
            u32 vertex = (u32)gabi::at<STBoxRibbonRecord>(record)->mpVertices;
            u32 end = vertex + 0x60;
            for (u32 p = vertex; p < end; p += 0x20) {
                u32 aligned = p & ~31u;
                for (int k = 0; k < 32; k++)
                    gabi::at<be<u8>>(aligned)[k] = 0;
            }
            if (vertex < end) {
                active = (u32)gabi::at<STBoxRibbonBuffers>(buffers)->mActiveBank;
                record = buffers + (active + 2 * step - 2) * 0x254;
            }
            u32 written = 0;
            f32 textureX = 0;
            f32 priorY = gabi::fsubs_ppc(textureY, increment);
            for (int j = 0; j < 3;) {
                u32 pt = (u32)gabi::at<STBoxParticleLink>(link)->mpObject;
                gabi::Local<cXyz> pos;
                copyFloat3(gabi::ea(pos.get()),
                           gabi::ea(&gabi::at<STBoxParticle>(pt)->mGlobalPosition));
                getMaxWaterY(pos.get());
                pt = (u32)gabi::at<STBoxParticleLink>(link)->mpObject;
                if ((u8)gabi::at<STBoxParticle>(pt)->mHidden == 0) {
                    u32 v = (u32)gabi::at<STBoxRibbonRecord>(record)->mpVertices + written * 0x28;
                    gabi::at<STBoxRibbonVertex>(v)->mPosition.x = (f32)((f32)pos->x);
                    gabi::at<STBoxRibbonVertex>(v)->mPosition.y = (f32)((f32)pos->y);
                    gabi::at<STBoxRibbonVertex>(v)->mPosition.z = (f32)((f32)pos->z);
                    v = (u32)gabi::at<STBoxRibbonRecord>(record)->mpVertices + written * 0x28;
                    gabi::at<STBoxRibbonVertex>(v)->mTexX = (f32)(textureX);
                    gabi::at<STBoxRibbonVertex>(v)->mTexY = (f32)(textureY);
                    f32 x = field_0x14[written].x, y = field_0x14[written].y,
                        z = field_0x14[written].z;
                    v = (u32)gabi::at<STBoxRibbonRecord>(record)->mpVertices + written * 0x28;
                    gabi::at<STBoxRibbonVertex>(v)->mPreviousPosition.y = (f32)(y);
                    gabi::at<STBoxRibbonVertex>(v)->mPreviousPosition.z = (f32)(z);
                    gabi::at<STBoxRibbonVertex>(v)->mPreviousPosition.x = (f32)(x);
                    v = (u32)gabi::at<STBoxRibbonRecord>(record)->mpVertices + written * 0x28;
                    gabi::at<STBoxRibbonVertex>(v)->mPreviousTexY = (f32)(priorY);
                    gabi::at<STBoxRibbonVertex>(v)->mPreviousTexX = (f32)(textureX);
                    field_0x14[written].x = (f32)pos->x;
                    field_0x14[written].y = (f32)pos->y;
                    field_0x14[written].z = (f32)pos->z;
                    textureX = gabi::fadds_ppc(textureX, 0.5f);
                    written++;
                    j++;
                }
                link = (u32)gabi::at<STBoxParticleLink>(link)->mpNext;
            }
        }
        textureY = gabi::fadds_ppc(textureY, increment);
    }
    shadowEmitter = gabi::ea((void *)mpEmitter);
    u32 buffers = (u32)gabi::at<STBoxEmitter>(shadowEmitter)->mpRibbonBuffers,
        active = (u32)gabi::at<STBoxRibbonBuffers>(buffers)->mActiveBank;
    u32 record = buffers + active * 0x254 + 4;
    for (u32 i = 1; i < steps; i++) {
        u32 value = (u32)gabi::at<STBoxBufferDescriptor>(record)->mFlushSize;
        stboxBufferFlush(ptr(record), 0, value);
        record += 0x4A8;
    }
    active = (u32)gabi::at<STBoxRibbonBuffers>(buffers)->mActiveBank;
    gabi::at<STBoxRibbonBuffers>(buffers)->mActiveBank = (u32)(active == 0);
    material = (u32)gabi::at<STBoxParticle>(particle)->mpMaterial;
    stboxMaterialFinish(ptr(material));
}

// HD: pointer argument is modified in place; GC passes i_this vector by value.
/* 02467D34 */
f32 getWaterY(cXyz *position) {
    f32 y = position->y;
    f32 x = position->x, z = position->z;
    position->y = gabi::fadds_ppc(y, 500.0f);
    if (stboxSeaCheckArea(x, z)) {
        x = position->x;
        z = position->z;
        return stboxSeaCalcWave(x, z);
    }
    return stboxWaterGroundCheck(position);
}

/* 024683D8 */
s32 daSTBox_c::_delete() {
    clearEmitters(this);
    stboxRippleEnd(ptr(gabi::ea(mRippleCallBack)));
    removeShadow(this);
    stboxResDelete(&mPhase, "Salvage");
    u32 save = (u32)l_save;
    u32 reg = stboxGetEventReg(ptr(save + 0x644), 0xADFF);
    if (field_0x331 == 2) {
        save = (u32)l_save;
        stboxSetEventReg(ptr(save + 0x644), 0xADFF, (reg + 1) & 255);
    }
    return 1;
}

/* 02467E48 */
s32 CheckCreateHeap(daSTBox_c *i_this) { return i_this->CreateHeap(); }

/* 02467D9C */
s32 daSTBox_c::CreateHeap() {
    u32 type = field_0x331;
    u32 res = (u32)l_resourceControl;
    s32 index = (s16)m_bdlidx.mEntries[type];
    gabi::Local<SafeName> name;
    name->text = gabi::guest_str("Salvage");
    name->vt = stbox_tu::safeString;
    auto *data = stboxGetResource(ptr(res), name.get(), index);
    if (!data)
        stboxAssertion("d_a_salvage_tbox.cpp", 0x25F, "modelData != (0)");
    auto *model = stboxModelCreate(data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr;
}

/* 02467F24 */
void daSTBox_c::CreateInit() {
    // GHS leaves this stack vector untouched when no crane top is available.
    gabi::Local<cXyz> crane;
    u32 play = stboxGetPlay();
    u32 ship = (u32)gabi::at<STBoxPlay>(play)->mpShipActor;
    u32 top = ship ? (u32)gabi::at<STBoxShip>(ship)->mpCraneTop : 0;
    if (top) {
        crane->x = (f32)gabi::at<cXyz>(top)->x;
        crane->z = (f32)gabi::at<cXyz>(top)->z;
        gabi::Local<cXyz> temp;
        temp->x = (f32)gabi::at<cXyz>(top)->x;
        temp->y = (f32)gabi::at<cXyz>(top)->y;
        temp->z = (f32)gabi::at<cXyz>(top)->z;
        f32 water = getWaterY(temp.get());
        field_0x324.z = crane->z;
        field_0x324.y = water;
        field_0x324.x = crane->x;
    } else {
        field_0x324.x = crane->x;
        field_0x324.y = crane->y;
        field_0x324.z = crane->z;
    }
    u32 m = gabi::ea((J3DModel *)mpModel);
    cullMtx = m ? m + 0xC8 : 0;
    stboxSetCullBox(this, -150.0f, -0.0f, -150.0f, 150.0f, 150.0f, 150.0f);
    set_mtx();
    u32 item = (u8)(u32)mParameters;
    field_0x330 = (item - 1u < 4) ? 5 : item;
    field_0x334 = 0;
    field_0x336 = 0;
    for (int i = 0; i < 2; i++)
        field_0x29C[i] = particleSet(this, 0, STBOX_WATER_DROP, gabi::ea(&current.pos),
                                     gabi::ea(&current.angle), 0, 255, 0);
    u32 type = field_0x331;
    if (type == 1 || type == 2) {
        field_0x29C[2] = particleSet(this, 0, STBOX_WATER_DROP, gabi::ea(&current.pos),
                                     gabi::ea(&current.angle), 0, 255, 0);
        for (int i = 0; i < 3; i++)
            setupEmitter(this, i, 3.0f, 20.0f);
        if (!(void *)mShadowCallback.mpEmitter) {
            particleSet(this, 5, STBOX_SALVAGE_SHADOW, gabi::ea(&field_0x324),
                        gabi::ea(&current.angle), 0, 0, gabi::ea(&mShadowCallback));
            mShadowCallback.mPos.copy(field_0x324);
            mShadowCallback.mExScaleY = 4.0f;
            mItemPID = 0xFFFFFFFF;
            mShadowCallback.mExTransY = -0.1f;
            return;
        }
    } else if (type == 0) {
        for (int i = 0; i < 2; i++)
            setupEmitter(this, i, 3.5f, -20.0f);
    }
    mItemPID = 0xFFFFFFFF;
}

/* 024682CC */
s32 daSTBox_c::_create() {
    // HD: actor and callback constructors retain the TU-specific vtables.
    u32 condition = actor_condition;
    if (!(condition & 8)) {
        if (this) {
            stboxActorConstruct(this);
            __vtbl = stbox_tu::actor;
            stboxRippleConstruct(ptr(gabi::ea(mRippleCallBack)));
            mShadowCallback.__vtbl = stbox_tu::shadow;
            stboxConstructArray(
                ptr(gabi::ea(mShadowCallback.field_0x14)), 3, 12,
                ptr(gabi::guest_fn(reinterpret_cast<const void *>(STBox_vectorConstructor))));
            condition = actor_condition;
        }
        actor_condition = condition | 8;
    }
    field_0x331 = ((u32)mParameters >> 8) & 15;
    s32 phase = stboxResLoad(&mPhase, "Salvage");
    if (phase == cPhs_COMPLEATE_e) {
        u32 type = field_0x331;
        s32 size = (s16)m_heapsize.mEntries[type];
        if (!stboxEntrySolidHeap(
                this, ptr(gabi::guest_fn(reinterpret_cast<const void *>(CheckCreateHeap))), size))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}

/* 02467E4C */
void daSTBox_c::set_mtx() {
    f32 x = scale.x, y = scale.y, z = scale.z;
    u32 m = gabi::ea((J3DModel *)mpModel);
    gabi::at<STBoxModel>(m)->mBaseScale.x = (f32)(x);
    gabi::at<STBoxModel>(m)->mBaseScale.y = (f32)(y);
    gabi::at<STBoxModel>(m)->mBaseScale.z = (f32)(z);
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    stboxMtxTranslation(&l_calc_mtx, x, y, z);
    s32 angle = current.angle.y;
    stboxMtxYrot(&l_calc_mtx, angle);
    f32 matrix[12];
    for (int i = 0; i < 12; i++)
        matrix[i] = (f32)l_calc_mtx.m[i / 4][i % 4];
    m = gabi::ea((J3DModel *)mpModel);
    for (int i = 0; i < 12; i++)
        gabi::at<STBoxModel>(m)->mBaseTRMtx.m[i / 4][i % 4] = matrix[i];
}

/* 02468550 */
s32 daSTBox_c::_execute() {
    u32 play = stboxGetPlay();
    s32 staff = stboxGetStaffId(&gabi::at<STBoxPlay>(play)->mEventManager, "STBox", 0, 0);
    play = stboxGetPlay();
    u32 ship = (u32)gabi::at<STBoxPlay>(play)->mpShipActor;
    gabi::Local<cXyz> ripple;
    f32 water = 0;
    if (ship) {
        copyFloat3(gabi::ea(ripple.get()), gabi::ea(&gabi::at<STBoxShip>(ship)->mCraneRipplePos));
        gabi::Local<cXyz> temp;
        copyFloat3(gabi::ea(temp.get()), gabi::ea(ripple.get()));
        water = getWaterY(temp.get());
    }
    play = stboxGetPlay();
    if ((u8)gabi::at<STBoxPlay>(play)->mEventRunning && (u16)eventInfo.mCommand != 1 &&
        staff != -1) {
        play = stboxGetPlay();
        s32 action = stboxGetActionIndex(&gabi::at<STBoxPlay>(play)->mEventManager, staff,
                                         action_table.mEntries, 5, 0, 0);
        play = stboxGetPlay();
        if (action == -1)
            stboxCutEnd(&gabi::at<STBoxPlay>(play)->mEventManager, staff);
        else {
            u32 offset = (u32)action * 8;
            if (stboxGetAdvance(&gabi::at<STBoxPlay>(play)->mEventManager, staff))
                eventMember(gabi::ea(event_init_tbl.mEntries) + offset, gabi::ea(this), staff);
            if (eventMember(gabi::ea(event_action_tbl.mEntries) + offset, gabi::ea(this), staff)) {
                play = stboxGetPlay();
                stboxCutEnd(&gabi::at<STBoxPlay>(play)->mEventManager, staff);
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        auto *e = static_cast<STBoxEmitter *>((void *)field_0x29C[i]);
        if (e) {
            u32 kind = (u8)e->mEmitterKind;
            f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
            e->mGlobalTranslation.z = (f32)(z);
            e->mGlobalTranslation.y = (f32)(y);
            e->mGlobalTranslation.x = (f32)(x);
            if (kind >= 7)
                e->mGlobalTranslation.y = (f32)(-(f32)e->mGlobalTranslation.y);
        }
    }
    f32 y = current.pos.y;
    if (y < water) {
        mShadowCallback.mPos.copy(*ripple.get());
        mShadowCallback.mDepth = gabi::fsubs_ppc(water, (f32)current.pos.y);
        f32 raised = gabi::fadds_ppc(water, 2.0f);
        mShadowCallback.mWaterFlatY = raised;
        mShadowCallback.mWaterY = raised;
    } else
        removeShadow(this);
    set_mtx();
    return 1;
}

/* 0246994C */
void daSTBox_c::initWait() {}

/* 02469298 */
void daSTBox_c::initWait02(s32 staff) { field_0x332 = 20; }

/* 024692A4 */
void daSTBox_c::initWaitGetItem(s32 staff) {
    stboxOffDraw(&draw_tag);
    clearEmitters(this);
    stboxRippleEnd(ptr(gabi::ea(mRippleCallBack)));
}

/* 02469950 */
void daSTBox_c::initWaitDummy() {}

/* 0246932C */
void daSTBox_c::initDrop(s32 staff) { gravity = -4.0f; }

/* 0246933C */
s32 daSTBox_c::actWait(s32 staff) {
    u32 play = stboxGetPlay();
    u32 ship = (u32)gabi::at<STBoxPlay>(play)->mpShipActor;
    if (!ship)
        stboxAssertion("d_a_salvage_tbox.cpp", 0x3D5, "0");
    u32 top = (u32)gabi::at<STBoxShip>(ship)->mpCraneTop;
    if (!top)
        stboxAssertion("d_a_salvage_tbox.cpp", 0x3DC, "0");
    gabi::Local<cXyz> temp;
    temp->x = (f32)gabi::at<cXyz>(top)->x;
    temp->y = gabi::fadds_ppc((f32)gabi::at<cXyz>(top)->y, 5000.0f);
    temp->z = (f32)gabi::at<cXyz>(top)->z;
    f32 water = getWaterY(temp.get());
    u32 type = field_0x331;
    current.angle.y = (s16)gabi::at<STBoxShip>(ship)->mCraneHookAngleY;
    f32 y = gabi::fsubs_ppc((f32)gabi::at<cXyz>(top)->y, (f32)crane_offset.mEntries[type]),
        x = (f32)gabi::at<cXyz>(top)->x, z = (f32)gabi::at<cXyz>(top)->z;
    current.pos.x = x;
    current.pos.y = y;
    attention_info.position.y = y;
    attention_info.position.x = x;
    attention_info.position.z = z;
    current.pos.z = z;
    if ((type == 1 || type == 2) && (f32)current.pos.y > water && field_0x336 == 0) {
        stboxSubBgmStart(0x8000005Du);
        field_0x336 = 1;
    }
    if (field_0x335 == 0) {
        mParticlePos.x = current.pos.x;
        mParticlePos.z = current.pos.z;
        mParticlePos.y = gabi::fadds_ppc((f32)current.pos.y, 2500.0f);
        f32 height = stboxGetWaterHeight(&mParticlePos);
        f32 y = current.pos.y;
        mParticlePos.y = height;
        if (y > gabi::fsubs_ppc(height, 10.0f)) {
            particleSet(this, 5, STBOX_RIPPLE, gabi::ea(&mParticlePos), 0, gabi::ea(&scale), 255,
                        gabi::ea(mRippleCallBack));
            field_0x335 = 1;
            reinterpret_cast<STBoxRippleCallback *>(mRippleCallBack)->mRate = 12.0f;
        }
    }
    type = field_0x331;
    if ((type == 1 || type == 2) && (u32)mItemPID == 0xFFFFFFFF) {
        s32 room = (s8)l_stayRoom;
        u32 item = field_0x330;
        u32 pid = stboxCreateItem(&current.pos, item, -1, room, 0, 0);
        mItemPID = pid;
        if (pid != 0xFFFFFFFF) {
            play = stboxGetPlay();
            gabi::at<STBoxPlay>(play)->mItemPartnerId = (u32)(pid);
        }
    }
    return 0;
}

/* 024695C4 */
s32 daSTBox_c::actDrop(s32 staff) {
    stboxPosMove(this, 0);
    gabi::Local<cXyz> p;
    copyFloat3(gabi::ea(p.get()), gabi::ea(&current.pos));
    f32 y = current.pos.y;
    f32 water = getWaterY(p.get());
    if (y < gabi::fsubs_ppc(water, 50.0f))
        return 1;
    gabi::Local<cXyz> q;
    copyFloat3(gabi::ea(q.get()), gabi::ea(&current.pos));
    y = current.pos.y;
    water = getWaterY(q.get());
    if (y < water) {
        if (field_0x334 == 0) {
            s32 room = current.roomNo;
            s32 reverb = stboxGetReverb(room);
            stboxSoundStart(0x6919, &eyePos, 0, reverb);
            stboxCreateWaterPillar(&current.pos, 0, 0.8f, 1.0f);
            field_0x334 = 1;
        }
        stboxRippleEnd(ptr(gabi::ea(mRippleCallBack)));
    }
    return 0;
}

/* 02469554 */
s32 daSTBox_c::actWait02(s32 staff) {
    u32 play = stboxGetPlay();
    u32 ship = (u32)gabi::at<STBoxPlay>(play)->mpShipActor;
    u32 top = (u32)gabi::at<STBoxShip>(ship)->mpCraneTop;
    if (top) {
        u32 type = field_0x331;
        f32 x = (f32)gabi::at<cXyz>(top)->x, y = (f32)gabi::at<cXyz>(top)->y;
        f32 offset = (f32)crane_offset.mEntries[type];
        f32 z = (f32)gabi::at<cXyz>(top)->z;
        current.pos.x = x;
        current.pos.z = z;
        current.pos.y = gabi::fsubs_ppc(y, offset);
    }
    return 0;
}

/* 02469954 */
BOOL daSTBox_c::actWaitGetItem(int i) { return TRUE; }

/* 0246995C */
BOOL daSTBox_c::actWaitDummy(int i) { return TRUE; }

/* 024683D4 */
s32 daSTBox_Create(daSTBox_c *i_this) { return i_this->_create(); }

/* 024684F0 */
s32 daSTBox_Delete(daSTBox_c *i_this) { return i_this->_delete(); }

/* 024684F4 */
s32 daSTBox_c::_draw() {
    u32 env = stboxGetEnvironment();
    stboxSettingTev(ptr(env), 0, &current.pos, &tevStr);
    env = stboxGetEnvironment();
    auto *model = (J3DModel *)mpModel;
    stboxSetLightTevColor(ptr(env), model, &tevStr);
    model = mpModel;
    stboxModelUpdate(model, 0);
    return 1;
}

/* 0246883C */
s32 daSTBox_Execute(daSTBox_c *i_this) { return i_this->_execute(); }

/* 02469810 */
s32 daSTBox_IsDelete(void *p) { return 1; }

// HD: GX2 ribbon draw, shader archive setup, and callback entries.

/* 0246909C */
void daSTBox_shadowEcallBack_c::drawAfter(void *emitter) {
    auto *e = static_cast<STBoxEmitter *>(emitter);
    u32 count = visibleParticles(e);
    if (count < 6)
        return;
    u32 first = (u32)e->mParticleFirst;
    if ((u8)l_particleStatus & 1) {
        depthState(true);
        first = (u32)e->mParticleFirst;
    }
    u32 steps = third(count), particle = (u32)gabi::at<STBoxParticleLink>(first)->mpObject,
        material = (u32)gabi::at<STBoxParticle>(particle)->mpMaterial;
    u32 vt = (u32)gabi::at<STBoxMaterial>(material)->__vtbl,
        target = (u32)gabi::at<STBoxMaterialVtable>(vt)->mpApply;
    u32 se = gabi::ea((void *)mpEmitter), arg = (u32)gabi::at<STBoxEmitter>(se)->mpShaderProgram;
    gabi::call_ptr(target, ptr(material), ptr(arg));
    se = gabi::ea((void *)mpEmitter);
    u32 object = (u32)gabi::at<STBoxParticle>(particle)->mpShader;
    arg = (u32)gabi::at<STBoxEmitter>(se)->mpShaderProgram;
    stboxShaderBind(ptr(object), ptr(arg));
    se = gabi::ea((void *)mpEmitter);
    u32 buffers = (u32)gabi::at<STBoxEmitter>(se)->mpRibbonBuffers,
        active = (u32)gabi::at<STBoxRibbonBuffers>(buffers)->mActiveBank;
    u32 record = buffers + (active == 0 ? 1 : 0) * 0x254;
    for (u32 i = 1; i < steps; i++) {
        if ((u32)gabi::at<STBoxRibbonBuffers>(buffers)->mNeedsBind)
            stboxVertexBufferBind(ptr(record + 0x158));
        u32 arg3 = (u32)l_drawMode, arg6 = (u32)l_drawCount, arg5 = (u32)l_drawIndex;
        stboxDrawStrip(arg3, 6, arg5, arg6, 0, 1);
        record += 0x4A8;
    }
    gabi::at<STBoxParticle>(particle)->mDrawDone = (u8)(1);
}

/* 0246981C */
void daSTBox_shadowEcallBack_c::setup(void *emitter, void *pos, void *angle) {
    auto *e = static_cast<STBoxEmitter *>(emitter);
    mpAngle = (csXyz *)angle;
    mpEmitter = e;
    field_0x4 = 0;
    gabi::Local<u32[2]> name; /* sead::SafeString {text, vtable} */
    reinterpret_cast<be<u32> *>(name.get())[1] = stbox_tu::safeString;
    reinterpret_cast<be<u32> *>(name.get())[0] = gabi::guest_str("salvage"); /* "salvage" */
    u32 arc = stboxGetShaderArchive();
    s32 idx = stboxFindShader((u32)gabi::at<STBoxShaderArchive>(arc)->mpArchive, name.get());
    u32 prog;
    if (idx < 0) {
        prog = 0;
    } else {
        u32 n = (u32)gabi::at<STBoxShaderArchive>(arc)->mProgramCount;
        prog = (u32)gabi::at<STBoxShaderArchive>(arc)->mpPrograms;
        u32 ent = (u32)idx < n ? prog + idx * 0x24 : prog;
        if ((u8)gabi::at<STBoxShaderProgram>(ent)->mReady == 0) {
            u32 i_this = (u32)gabi::at<STBoxShaderArchive>(arc)->mpArchive;
            u32 prm = (u32)idx < (u32)gabi::at<STBoxShaderArchiveData>(i_this)->mCount
                          ? (u32)gabi::at<STBoxShaderArchiveData>(i_this)->mpParameters + idx * 0x84
                          : 0;
            stboxProgramSetup(ent, prm, 0);
            n = (u32)gabi::at<STBoxShaderArchive>(arc)->mProgramCount;
            prog = (u32)gabi::at<STBoxShaderArchive>(arc)->mpPrograms;
        }
        if ((u32)idx < n)
            prog += idx * 0x24;
    }
    e->mpShaderProgram = prog;
    stboxEmitterShaderSetup(gabi::ea((void *)mpEmitter));
}

/* 02469818 */
void daSTBox_shadowEcallBack_c::executeAfter() {}

/* 02468B80 */
void STBox_matrixCopy(Mtx34 *dst, Mtx34 *src) {
    f32 values[12];
    for (int i = 0; i < 12; i++)
        values[i] = (f32)src->m[i / 4][i % 4];
    for (int i = 0; i < 12; i++)
        dst->m[i / 4][i % 4] = values[i];
}

// HD: compiler-generated and template code of this translation unit.

/* 024696E8 */
void __sinit_d_a_salvage_tbox_cpp() {
    l_headerZeros.mWords[3] = 0;
    l_headerZeros.mWords[2] = 0;
    l_headerZeros.mWords[1] = 0;
    l_headerZeros.mWords[0] = 0;
    stboxRegisterGlobal(&l_headerDtor.mEntries[0]);
    f32 low = (f32)l_piLowConst, high = (f32)l_piHighConst;
    l_piLow = low;
    l_piHigh = high;
    stboxAngleConstruct(&l_angle);
    stboxRegisterGlobal(&l_headerDtor.mEntries[1]);
    stboxCircleConstruct(&l_circle);
    stboxRegisterGlobal(&l_headerDtor.mEntries[2]);
    f32 scroll = (f32)l_scrollConst, scale = (f32)l_scaleConst;
    l_shadowTexture.mAlpha = 255;
    l_shadowTexture.mFlags = 0;
    l_shadowTexture.mShadowAlpha = 120;
    f32 depth = (f32)l_depthConst;
    l_shadowTexture.mScale = scale;
    l_shadowTexture.mDepth = depth;
    l_shadowTexture.__vtbl = STBOX_SHADOW_TEXTURE_VTABLE;
    l_shadowTexture.mScroll = scroll;
}

/* 024697D0 */
void STBox_emptyDestructor(void *p, s32 flag) {
    if (p && (flag & 1))
        stboxDeallocate((void *)p);
}

/* 024697E4 */
cXyz *STBox_vectorConstructor(cXyz *p) {
    if (!p)
        p = stboxVectorAllocate(12);
    return p;
}

/* 02469964 */
static void STBox_deletingDtor(u32 p, u32 flags) {
    if (p == 0)
        return;
    stboxActorDestroy(p, 0u);
    if (flags & 1)
        stboxDeallocate(ptr(p));
}

/* 024699B8 */
static void STBox_SafeString_assureTerminationImpl(u32 p) {}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
ext 0200F564 void stboxChaseS(void*,s32,s32)
ext 0246B6A4 s32 stboxSeaCheckArea(f32,f32)
ext 0246BA0C f32 stboxSeaCalcWave(f32,f32)
ext 024F1478 f32 stboxWaterGroundCheck(void*)
ext 024F17D4 f32 stboxGetWaterHeight(void*)
ext 025200D4 u32 stboxGetPlay()
ext 02520460 s32 stboxResLoad(void*,const char*)
ext 025204C8 void stboxResDelete(void*,const char*)
ext 02520540 s32 stboxGetReverb(s32)
ext 02542D88 s32 stboxGetStaffId(void*,const char*,u32,u32)
ext 02542EDC s32 stboxGetActionIndex(void*,s32,void*,u32,u32,u32)
ext 02543280 void stboxCutEnd(void*,s32)
ext 025447C8 s32 stboxGetAdvance(void*,s32)
ext 02555D0C u32 stboxGetEnvironment()
ext 025602F0 void stboxGetSeaColor(void*,void*)
ext 025626A4 void stboxSettingTev(void*,u32,void*,void*)
ext 02562F5C void stboxSetLightTevColor(void*,void*,void*)
ext 025A847C void* stboxParticleSet(void*,u32,u32,void*,void*,void*,u32,void*,s32,u32,u32,u32)
ext 025A9084 void stboxRippleConstruct(void*)
ext 025A9270 void stboxRippleEnd(void*)
ext 025B8AF4 void stboxSetEventReg(void*,u32,u32)
ext 025B8BB0 u32 stboxGetEventReg(void*,u32)
ext 025D4ED0 void stboxActorConstruct(void*)
ext 025D50BC void stboxActorDestroy(u32,u32)
ext 025D63E8 s32 stboxEntrySolidHeap(void*,void*,s32)
ext 025D674C void stboxSetCullBox(void*,f32,f32,f32,f32,f32,f32)
ext 025D6870 void stboxPosMove(void*,u32)
ext 025D7E88 u32 stboxCreateItem(void*,u32,s32,s32,u32,u32)
ext 025DA884 void stboxOffDraw(void*)
ext 025DAE64 void stboxCreateWaterPillar(void*,u32,f32,f32)
ext 025E1918 void stboxSubBgmStart(u32)
ext 025E1A40 void stboxSoundStart(u32,void*,u32,s32)
ext 025E2DE0 void stboxModelUpdate(void*,u32)
ext 025E38E0 J3DModel* stboxModelCreate(void*,u32,u32)
ext 025F1C28 void stboxMtxYrot(void*,s32)
ext 026066C4 J3DModelData* stboxGetResource(void*,void*,s32)
ext 0273AA24 void stboxAssertion(const char*,u32,const char*)
ext 0273AD10 cXyz* stboxVectorAllocate(u32)
ext 0273AF40 void stboxDeallocate(void*)
ext 02750250 void stboxDepthConstruct(void*)
ext 02750534 void stboxDepthApply(void*)
ext 027B5E94 void stboxBufferFlush(void*,u32,u32)
ext 027B90AC s32 stboxFindShader(u32,void*)
ext 027BFE5C void stboxVertexBufferBind(void*)
ext 027FB678 void stboxMaterialFinish(void*)
ext 027FC80C void stboxSetTextureMatrix(void*,void*,u32)
ext 027FE5B8 void stboxShaderBind(void*,void*)
ext 027FFCBC u32 stboxGetShaderArchive()
ext 02800B0C void stboxProgramSetup(u32,u32,u32)
ext 0281E76C void stboxEmitterShaderSetup(u32)
ext 028245AC void stboxRotationMatrix(s32,s32,s32,void*)
ext 028255F8 void stboxParticleMaterialSetup(void*,void*)
ext 028E9098 void stboxMtxIdentity(void*)
ext 028E93CC void stboxMtxTranslation(void*,f32,f32,f32)
ext 028EAB2C void stboxCircleConstruct(void*)
ext 028ED6F8 void stboxAngleConstruct(void*)
ext 028EFFD0 void stboxConstructArray(void*,u32,u32,void*)
ext 028F026C void stboxRegisterGlobal(void*)
ext C0006178 void stboxDrawStrip(u32,u32,u32,u32,u32,u32)
data l_resourceControl 101F4F28
data l_save 101F84DC
data m_bdlidx 10039C40
data m_heapsize 10039C38
data crane_offset 10039B1C
data l_calc_mtx 1048D0CC
data action_table 101CFF78
data event_init_tbl 101CFF08
data event_action_tbl 101CFF30
data l_stayRoom 1047E6C8
data l_particleStatus 101EA4E4
data l_drawMode 104B4A88
data l_drawIndex 104B4A84
data l_drawCount 104B4A8C
data l_headerZeros 1046D840
data l_piLow 1046D834
data l_piHigh 1046D838
data l_angle 1046D83C
data l_circle 1046D83D
data l_headerDtor 101CFF8C
data l_shadowTexture 1046D850
data l_piLowConst 10039C28
data l_piHighConst 10039C2C
data l_scrollConst 10039B74
data l_scaleConst 10039B78
data l_depthConst 10039BC8
str 10039C30 "Salvage"
str 10039AC4 "salvage"
str 10039B28 "d_a_salvage_tbox.cpp" in 02467D9C
str 10039B40 "modelData != (0)"
str 10039B88 "STBox"
str 10039C00 "0"
str 10039C04 "d_a_salvage_tbox.cpp" in 0246933C
*/
