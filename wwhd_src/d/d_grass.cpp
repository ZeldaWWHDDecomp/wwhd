/** Grass: animation, collision and rendering packets.
 * GameCube names/layout with the verified WWHD shader/resource additions.
 * Every original function and caller workspace
 */
#include "d/d_grass.h"
using namespace GrassHD;
// HD: Resource helpers for the GX2 grass rendering path.

static void grass_StringValidate(String *string) {
    u32 table = (u32)string->mVtable;
    gabi::call_ptr<void>((u32)at<DispatchTable>(table)->mpAssureTermination, string);
}

static void grass_ReleaseAllocation(u32 owner, u32 offset) {
    u32 allocation = (u32)(at<ShapePair>(owner + offset - 4)->mpShapes);
    u32 heap = GrassHD::SDK::Heap_findContaining((u32)(*at<be<u32>>(Data::heapManager)), allocation);
    u32 table = (u32)(at<Heap>(heap)->mVtable);
    gabi::call_ptr<void>((u32)at<DispatchTable>(table)->mpFree, heap,
                         (u32)(at<ShapePair>(owner + offset - 4)->mpShapes));
}

static void grass_ReleaseResources(u32 base) {
    for (u32 i = 0; i < 2; ++i) {
        u32 resource = base + i * 0x4A8;
        GrassHD::SDK::Shape_clear(resource + 0x158);
        at<Resource>(resource)->mpBuffer = 0;
        if ((u32)(at<Resource>(resource)->mpAllocation)) {
            grass_ReleaseAllocation(resource, 0x250);
            at<Resource>(resource)->mAlignment = 0;
            at<Resource>(resource)->mpAllocation = 0;
        }
        GrassHD::SDK::Shape_clear(resource + 0x3AC);
        at<Resource>(resource + 0x254)->mpBuffer = 0;
        if ((u32)(at<Resource>(resource + 0x254)->mpAllocation)) {
            grass_ReleaseAllocation(resource, 0x4A4);
            at<Resource>(resource + 0x254)->mAlignment = 0;
            at<Resource>(resource + 0x254)->mpAllocation = 0;
        }
    }
    at<Resources>(base)->mCount = 0;
}

static u32 grass_Collection(u32 self, u32 pass) {
    u32 collection = (u32)(at<dGrass_packet_c>(self)->mMaterials.mpItems);
    if (pass < (u32)(at<dGrass_packet_c>(self)->mMaterials.mCapacity))
        collection += pass * 20;
    return collection;
}

static u32 grass_HeapAlloc(u32 size, u32 alignment) {
    u32 heap = GrassHD::SDK::Heap_getCurrent((u32)(*at<be<u32>>(Data::heapManager)));
    return gabi::call_ptr<u32>((u32)at<DispatchTable>((u32)at<Heap>(heap)->mVtable)->mpAlloc, heap, size,
                               alignment);
}

static void grass_ClearShapes(u32 item) {
    at<ShapeCollection>(item)->mpMaterial = 0;
    for (u32 offset = 8; offset <= 16; offset += 8) {
        u32 array = (u32)(at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mpShapes);
        if (!array)
            continue;
        for (s32 i = 0; i < (s32)(at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mCount); ++i) {
            u32 child = array + (u32)i * 0xF4;
            gabi::call_ptr<void>((u32)at<DispatchTable>((u32)at<Shape>(child)->mVtable)->mpDelete, child, 2);
            array = (u32)(at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mpShapes);
        }
        grass_ReleaseAllocation(item, offset);
        at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mCount = 0;
        at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mpShapes = 0;
    }
}

static void grass_ClearVertexBuffer(u32 buffer) {
    u32 end = buffer + 0xC00;
    for (u32 address = buffer; address < end; address += 32) {
        for (u32 i = 0; i < 32; i += 4)
            at<CacheLine>(address & ~31u)->mWords[i / 4] = 0;
    }
}

static void grass_PopulateVertices(u32 buffer, u32 countAddress, u32 positions, u32 normals, u32 colors,
                                   u32 texcoords) {
    for (u32 i = 0; i < (u32)*at<be<u32>>(countAddress); ++i) {
        for (u32 j = 0; j < 3; ++j)
            at<Vertex>(buffer)[i].mPosition[j] = (u32)at<FloatVectorWords>(positions)[i + 1].mWords[j];
        for (u32 j = 0; j < 3; ++j)
            at<Vertex>(buffer)[i].mNormal[j] = (u32)at<FloatVectorWords>(normals)[i + 1].mWords[j];
        for (u32 j = 0; j < 4; ++j)
            at<Vertex>(buffer)[i].mColor[j] = (u32)at<ColorWords>(colors)[i + 1].mWords[j];
        for (u32 j = 0; j < 2; ++j)
            at<Vertex>(buffer)[i].mTexCoord[j] = (u32)at<TexCoordWords>(texcoords)[i + 1].mWords[j];
    }
}

/* 02549D38 */
void setBatta(u32 pos, u32 color) {

    gabi::Local<String> stage, prefix, boss, other;
    bool spawn = false;
    if (GrassHD::SDK::dKy_rain_check())
        return;
    u32 play = GrassHD::SDK::dComIfGp_get();
    if ((u8)(at<Play>(play)->mEventRunning))
        return;
    play = GrassHD::SDK::dComIfGp_get();
    at<String>(stage.a)->mVtable = Data::safeStringVtable;
    at<String>(prefix.a)->mVtable = Data::safeStringVtable;
    at<String>(stage.a)->mpString = play + 0x5134;
    at<String>(prefix.a)->mpString = Data::stageKin;
    GrassHD::SDK::SafeString_assureTermination(stage.get());
    grass_StringValidate(stage.get());
    u32 left = (u32)(at<String>(stage.a)->mpString);
    grass_StringValidate(prefix.get());
    u32 right = (u32)(at<String>(prefix.a)->mpString);
    if (left == right)
        return;
    bool different = false;
    for (u32 i = 0; i < 3; ++i) { /* "kin" stage prefix: the original advances both cursors (r9 stage, r12
                                     "kin" Data::stageKin; ctr=3) */
        u8 a = (u8)(*at<be<u8>>(left)), b = (u8)(*at<be<u8>>(right));
        if (!a) {
            if (!b)
                return;
            different = true;
            break;
        }
        if (!b || a != b) {
            different = true;
            break;
        }
        ++left;
        ++right;
    }
    if (!different)
        return;
    at<String>(boss.a)->mVtable = Data::safeStringVtable;
    at<String>(boss.a)->mpString = Data::stageXboss;
    play = GrassHD::SDK::dComIfGp_get();
    at<String>(other.a)->mVtable = Data::safeStringVtable;
    at<String>(other.a)->mpString = play + 0x5134;
    grass_StringValidate(boss.get());
    grass_StringValidate(boss.get());
    left = (u32)(at<String>(boss.a)->mpString);
    grass_StringValidate(other.get());
    right = (u32)(at<String>(other.a)->mpString);
    if (left == right)
        return;
    left = (u32)(at<String>(boss.a)->mpString);
    right = (u32)(at<String>(other.a)->mpString);
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = (u8)(*at<be<u8>>(left)), b = (u8)(*at<be<u8>>(right));
        if (a != b)
            break;
        if (!a)
            return;
        ++left;
        ++right;
    }
    f32 random = GrassHD::SDK::cM_rnd();
    f32 threshold = (f32)(*at<be<f32>>(Data::battaProbability));
    if (random > threshold)
        spawn = true;
    if (!spawn)
        return;
    play = GrassHD::SDK::dComIfGp_get();
    GrassHD::SDK::ParticleControl_set((u32)(at<Play>(play)->mpParticle), 0, 0x453, pos, 0, 0, 255, 0, -1,
                                      color, color, 0);
}

/* 0254A198 */
void GrassHD::dGrass_data_c::WorkCo(u32 actor, u32 mode, s32 room) {
    u32 self = gabi::ea(this);

    gabi::Local<Vector> delta, position;
    gabi::Local<ParticleParams> parameters;
    f32 x = (f32)(mPos.x) - (f32)(at<Actor>(actor)->mCurrentPos.x);
    f32 z = (f32)(mPos.z) - (f32)(at<Actor>(actor)->mCurrentPos.z);
    f32 zero = (f32)(*at<be<f32>>(Data::zero));
    at<Vector>(delta.a)->y = zero;
    at<Vector>(delta.a)->x = x;
    at<Vector>(delta.a)->z = z;
    f32 squared = GrassHD::SDK::PSVECSquareMag(delta.get());
    if (squared > (f32)(*at<be<f32>>(Data::contactDistanceSquared)))
        return;
    u32 angle = GrassHD::SDK::cM_atan2s(x, z);
    f32 distance = GrassHD::SDK::sqrtf(squared);
    s32 index = (s8)(mAnimIdx);
    if (index < 8) {
        f32 speed = (f32)(at<Actor>(actor)->mSpeedF), threshold = (f32)(*at<be<f32>>(Data::runSpeed));
        if (speed > threshold) {
            f32 y = (f32)(mPos.y), pz = (f32)(mPos.z);
            f32 lift = (f32)(*at<be<f32>>(Data::runParticleLift)), px = (f32)(mPos.x);
            at<Vector>(position.a)->z = pz;
            at<Vector>(position.a)->x = px;
            at<Vector>(position.a)->y = y + lift;
            GrassHD::SDK::dComIfGp_get();
            u32 color = Data::roomControl + (u32)room * 0x22C + 0xEC;
            u32 play = GrassHD::SDK::dComIfGp_get();
            u32 packet = (u32)(at<Play>(play)->mpGrassPacket);
            u32 particle = (u16)(at<dGrass_packet_c>(packet)->mCoParticle);
            play = GrassHD::SDK::dComIfGp_get();
            GrassHD::SDK::ParticleControl_setSimple((u32)(at<Play>(play)->mpParticle), particle,
                                                    position.get(), 255, color, color, 1);
            play = GrassHD::SDK::dComIfGp_get();
            u32 background = play + 0x12A0;
            play = GrassHD::SDK::dComIfGp_get();
            packet = (u32)(at<Play>(play)->mpGrassPacket);
            u32 emitter =
                GrassHD::SDK::ParticleControl_getEmitter((u32)(at<Background>(background)->mpParticle),
                                                         (u16)(at<dGrass_packet_c>(packet)->mCoParticle));
            if (emitter) {
                u32 target = (u32)(at<Emitter>(emitter)->mpEmitter);
                if (target) {
                    at<ParticleParams>(parameters.a)->mpEmptyString = Data::emptyString;
                    at<ParticleParams>(parameters.a)->mVtable = Data::safeStringVtable;
                    at<ParticleParams>(parameters.a)->mEnable = 1;
                    at<ParticleParams>(parameters.a)->mFlags[1] = 0;
                    f32 one = (f32)(*at<be<f32>>(Data::one));
                    at<ParticleParams>(parameters.a)->mFlags[0] = 0;
                    at<ParticleParams>(parameters.a)->mColorWords[0] = one;
                    at<ParticleParams>(parameters.a)->mFlags[2] = 0;
                    at<ParticleParams>(parameters.a)->mColorWords[3] = one;
                    at<ParticleParams>(parameters.a)->mColorWords[1] = one;
                    at<ParticleParams>(parameters.a)->mColorWords[2] = one;
                    play = GrassHD::SDK::dComIfGp_get();
                    u32 control = (u32)(at<Play>(play)->mpParticle);
                    for (u32 i = 0; i < 4; ++i)
                        at<ParticleParams>(parameters.a)->mColorWords[i] =
                            (u32)at<ParticleControl>(control)->mColorWords[i];
                    GrassHD::SDK::Emitter_setColor(target, parameters.get());
                }
            }
            GrassHD::SDK::setBatta(self + 4, color);
        }
        u32 play = GrassHD::SDK::dComIfGp_get();
        s32 fresh = GrassHD::SDK::dGrass_packet_c__newAnm((u32)(at<Play>(play)->mpGrassPacket));
        if (fresh < 0)
            return;
        mAnimIdx = fresh;
    }
    u32 play = GrassHD::SDK::dComIfGp_get();
    index = (s8)(mAnimIdx);
    u32 packet = (u32)(at<Play>(play)->mpGrassPacket);
    f32 radius = (f32)(*at<be<f32>>(Data::bendRadius));
    u32 animation = packet + 0x18F0C + (u32)index * 0x38;
    at<dGrass_anm_c>(animation)->mRotY = angle;
    u32 bend = GrassHD::SDK::cM_atan2s(radius - distance, radius);
    at<dGrass_anm_c>(animation)->mRotX = bend;
    at<dGrass_anm_c>(animation)->mState = 2;
}

/* 0254A458 */
void GrassHD::dGrass_data_c::WorkAt_NoCutAnim(u32 actor, u32 mode, s32 room, u32 hit, u32 object) {
    u32 self = gabi::ea(this);

    gabi::Local<Vector> velocity, horizontal;
    u32 info = GrassHD::SDK::dCcD_GetGObjInf(object);
    f32 x = (f32)(at<HitObjectInfo>(info)->mAtVec.x);
    at<Vector>(velocity.a)->x = x;
    f32 y = (f32)(at<HitObjectInfo>(info)->mAtVec.y);
    at<Vector>(velocity.a)->y = y;
    f32 z = (f32)(at<HitObjectInfo>(info)->mAtVec.z), zero = (f32)(*at<be<f32>>(Data::zero));
    at<Vector>(horizontal.a)->y = zero;
    at<Vector>(horizontal.a)->z = z;
    at<Vector>(horizontal.a)->x = x;
    at<Vector>(velocity.a)->z = z;
    f32 magnitude = GrassHD::SDK::PSVECSquareMag(horizontal.get());
    f32 epsilon = (f32)(*at<be<f32>>(Data::epsilon));
    if (fabsf(magnitude) < epsilon) {
        u32 table = (u32)(at<CollisionObject>(object)->mVtable);
        u32 shape = gabi::call_ptr<u32>((u32)at<DispatchTable>(table)->mpGetShape, object);
        table = (u32)(at<CollisionShape>(shape)->mVtable);
        if (gabi::call_ptr<u32>((u32)at<DispatchTable>(table)->mpGetNormal, shape, self + 4,
                                velocity.get())) {
            f32 scale = (f32)(*at<be<f32>>(Data::normalScale));
            GrassHD::SDK::PSVECScale(velocity.get(), velocity.get(), scale);
            x = (f32)(at<Vector>(velocity.a)->x);
            z = (f32)(at<Vector>(velocity.a)->z);
            at<Vector>(horizontal.a)->x = x;
            at<Vector>(horizontal.a)->y = zero;
            at<Vector>(horizontal.a)->z = z;
        } else {
            f32 ax = (f32)(at<Actor>(actor)->mCurrentPos.x), px = (f32)(mPos.x),
                az = (f32)(at<Actor>(actor)->mCurrentPos.z);
            x = px - ax;
            y = (f32)(mPos.y) - (f32)(at<Actor>(actor)->mCurrentPos.y);
            z = (f32)(mPos.z) - az;
            at<Vector>(horizontal.a)->x = x;
            at<Vector>(horizontal.a)->y = zero;
            at<Vector>(velocity.a)->y = y;
            at<Vector>(horizontal.a)->z = z;
            at<Vector>(velocity.a)->x = x;
            at<Vector>(velocity.a)->z = z;
        }
        magnitude = GrassHD::SDK::PSVECSquareMag(horizontal.get());
        if (fabsf(magnitude) < epsilon)
            return;
    }
    s32 angle = GrassHD::SDK::cM_atan2s((f32)(at<Vector>(velocity.a)->x), (f32)(at<Vector>(velocity.a)->z));
    s32 index = (s8)(mAnimIdx);
    u32 play = GrassHD::SDK::dComIfGp_get(), packet = (u32)(at<Play>(play)->mpGrassPacket);
    if (index < 8) {
        s32 fresh = GrassHD::SDK::dGrass_packet_c__newAnm(packet);
        if (fresh < 0)
            return;
        mAnimIdx = fresh;
        play = GrassHD::SDK::dComIfGp_get();
        packet = (u32)(at<Play>(play)->mpGrassPacket);
    }
    index = (s8)(mAnimIdx);
    f32 weight = (f32)(*at<be<f32>>(Data::randomBendRange)), bias = (f32)(*at<be<f32>>(Data::randomBendBias));
    u32 animation = packet + 0x18F0C + (u32)index * 0x38;
    f32 random = GrassHD::SDK::cM_rnd();
    f32 factor = gabi::fmadds(random, weight, bias);
    u32 horizontalAngle = gabi::ftoi(gabi::fmuls_ppc((f32)angle, factor));
    f32 radius = (f32)(*at<be<f32>>(Data::bendRadius));
    at<dGrass_anm_c>(animation)->mRotY = horizontalAngle;
    s32 bend = GrassHD::SDK::cM_atan2s(magnitude, radius);
    random = GrassHD::SDK::cM_rnd();
    factor = gabi::fmadds(random, weight, bias);
    u32 verticalAngle = gabi::ftoi(gabi::fmuls_ppc((f32)bend, factor));
    at<dGrass_anm_c>(animation)->mState = 2;
    at<dGrass_anm_c>(animation)->mRotX = verticalAngle;
}

/* 0254A7C8 */
void GrassHD::dGrass_data_c::WorkAt(u32 actor, u32 mode, s32 room, u32 hit) {
    u32 self = gabi::ea(this);

    gabi::Local<Vector> position;
    gabi::Local<ParticleParams> parameters;
    u32 object = (u32)(at<HitInfo>(hit)->mpAtHitObj);
    if (object && ((u32)(at<CollisionObject>(object)->mAtType) & 0x3CC000)) {
        GrassHD::SDK::dGrass_data_c__WorkAt_NoCutAnim(self, actor, mode, room, hit, object);
        return;
    }
    s32 index = (s8)(mAnimIdx);
    if (index >= 8) {
        u32 play = GrassHD::SDK::dComIfGp_get();
        index = (s8)(mAnimIdx);
        u32 packet = (u32)(at<Play>(play)->mpGrassPacket);
        at<dGrass_packet_c>(packet)->mGrassAnm[(u32)index].mState = 0;
    }
    mAnimIdx = -1;
    if (!(u32)(*at<be<u32>>(Data::angleGuard))) {
        *at<be<u32>>(Data::angleGuard) = 1;
        GrassHD::SDK::csXyz__csXyz(Data::cutAngle, 0, 0, 0);
    }
    f32 x = (f32)(mPos.x), y = (f32)(mPos.y), lift = (f32)(*at<be<f32>>(Data::cutParticleLift));
    at<Vector>(position.a)->x = x;
    f32 z = (f32)(mPos.z);
    at<Vector>(position.a)->z = z;
    at<Vector>(position.a)->y = y + lift;
    GrassHD::SDK::dComIfGp_get();
    u32 color = Data::roomControl + (u32)room * 0x22C + 0xEC;
    u32 play = GrassHD::SDK::dComIfGp_get(), packet = (u32)(at<Play>(play)->mpGrassPacket);
    u32 particle = (u16)(at<dGrass_packet_c>(packet)->mAtParticle);
    play = GrassHD::SDK::dComIfGp_get();
    GrassHD::SDK::ParticleControl_setSimple((u32)(at<Play>(play)->mpParticle), particle, position.get(), 255,
                                            color, color, 1);
    play = GrassHD::SDK::dComIfGp_get();
    u32 background = play + 0x12A0;
    play = GrassHD::SDK::dComIfGp_get();
    packet = (u32)(at<Play>(play)->mpGrassPacket);
    u32 emitter = GrassHD::SDK::ParticleControl_getEmitter((u32)(at<Background>(background)->mpParticle),
                                                           (u16)(at<dGrass_packet_c>(packet)->mAtParticle));
    if (emitter) {
        u32 target = (u32)(at<Emitter>(emitter)->mpEmitter);
        if (target) {
            at<ParticleParams>(parameters.a)->mFlags[0] = 0;
            at<ParticleParams>(parameters.a)->mFlags[2] = 0;
            f32 one = (f32)(*at<be<f32>>(Data::one));
            at<ParticleParams>(parameters.a)->mFlags[1] = 0;
            at<ParticleParams>(parameters.a)->mColorWords[0] = one;
            at<ParticleParams>(parameters.a)->mColorWords[1] = one;
            at<ParticleParams>(parameters.a)->mpEmptyString = Data::emptyString;
            at<ParticleParams>(parameters.a)->mVtable = Data::safeStringVtable;
            at<ParticleParams>(parameters.a)->mColorWords[2] = one;
            at<ParticleParams>(parameters.a)->mColorWords[3] = one;
            at<ParticleParams>(parameters.a)->mEnable = 1;
            play = GrassHD::SDK::dComIfGp_get();
            u32 control = (u32)(at<Play>(play)->mpParticle);
            for (u32 i = 0; i < 4; ++i)
                at<ParticleParams>(parameters.a)->mColorWords[i] =
                    (u32)at<ParticleControl>(control)->mColorWords[i];
            GrassHD::SDK::Emitter_setColor(target, parameters.get());
        }
    }
    GrassHD::SDK::setBatta(self + 4, color);
    s32 item = (s8)(mItemIdx);
    if (item >= 0)
        GrassHD::SDK::fopAcM_createItemFromTable(self + 4, item, -1, room, 0, 0, 1, 0);
    if (!(u8)(*at<be<u8>>(Data::cutSoundFlag))) {
        *at<be<u8>>(Data::cutSoundFlag) = 1;
        u32 reverb = GrassHD::SDK::dComIfGp_getReverb(room);
        GrassHD::SDK::mDoAud_seStart(0x282C, self + 4, 0, reverb);
    }
}

/* 0254AA3C */
void GrassHD::dGrass_data_c::hitCheck(s32 room) {
    u32 self = gabi::ea(this);

    gabi::Local<HitInfo> hit;
    gabi::Local<be<u32>> actor;
    GrassHD::SDK::HitInfo_construct(hit.get());
    u32 play = GrassHD::SDK::dComIfGp_get();
    u32 result = GrassHD::SDK::dCcMassS_Mng__Chk(play + 0x4EF8, self + 4, actor.get(), hit.get());
    bool attack = false;
    if (result & 1) {
        u32 other = (u32)(*actor.get());
        if (other && (s16)(at<Actor>(other)->mProcName) != 0x1C5 &&
            (s16)(at<Actor>(other)->mProcName) != 0x1C6)
            attack = true;
    }
    u32 contact = result & 2;
    if (!contact && !attack) {
        s32 index = (s8)(mAnimIdx);
        if (index < 8)
            return;
        play = GrassHD::SDK::dComIfGp_get();
        index = (s8)(mAnimIdx);
        u32 packet = (u32)(at<Play>(play)->mpGrassPacket), animation = packet + 0x18F0C + (u32)index * 0x38;
        u32 rotation = (u16)(at<dGrass_anm_c>(animation)->mRotY);
        s32 target = (s16)(rotation & 0xE000);
        u32 baseIndex = rotation >> 13;
        play = GrassHD::SDK::dComIfGp_get();
        packet = (u32)(at<Play>(play)->mpGrassPacket);
        u32 base = packet + 0x18F0C + baseIndex * 0x38;
        if ((u8)(at<dGrass_anm_c>(animation)->mState) == 2) {
            u32 reverb = GrassHD::SDK::dComIfGp_getReverb(room);
            GrassHD::SDK::mDoAud_seStart(0x3814, self + 4, 0, reverb);
            at<dGrass_anm_c>(animation)->mState = 1;
        }
        if (GrassHD::SDK::cLib_addCalcAngleS(animation + 4, (s32)(s16)(at<dGrass_anm_c>(base)->mRotX), 16,
                                             4000, 100))
            return;
        if (!GrassHD::SDK::cLib_chaseAngleS(animation + 2, target, 800))
            return;
        play = GrassHD::SDK::dComIfGp_get();
        index = (s8)(mAnimIdx);
        packet = (u32)(at<Play>(play)->mpGrassPacket);
        at<dGrass_packet_c>(packet)->mGrassAnm[(u32)index].mState = 0;
        mAnimIdx = (u16)(at<dGrass_anm_c>(animation)->mRotY) >> 13;
    } else {
        if (!(u32)(*actor.get()))
            GrassHD::SDK::JUTAssertion__getSDevice(Data::hitAssertFile, 0x80F, Data::hitAssertCondition);
        if (contact)
            GrassHD::SDK::dGrass_data_c__WorkCo(self, (u32)(*actor.get()), result, room);
        if (attack)
            GrassHD::SDK::dGrass_data_c__WorkAt(self, (u32)(*actor.get()), result, room, hit.get());
    }
}

/* 0254AC40 */
void GrassHD::dGrass_room_c::newData(u32 data) {
    at<dGrass_data_c>(data)->mpNextData = (u32)(mpData);
    mpData = data;
}

/* 0254AC50 */
void GrassHD::dGrass_room_c::deleteData() {
    u32 data = (u32)(mpData);
    while (data) {
        at<dGrass_data_c>(data)->mState = 0;
        u32 current = (u32)(mpData);
        GrassHD::SDK::JAIZelBasic__seDeleteObject(current + 4);
        current = (u32)(mpData);
        data = (u32)(at<dGrass_data_c>(current)->mpNextData);
        mpData = data;
    }
}

/* 0254ACD8 */
// HD: Constructor thunks retain nullable allocation and original store/callback order.
u32 dGrass_packet_c__ct(u32 self) {

    if (!self) {
        self = GrassHD::SDK::operator_new(0x66408);
        if (!self)
            return 0;
    }
    GrassHD::SDK::Packet_construct(self);
    at<dGrass_packet_c>(self)->mVtable = Data::grassPacketVtable;
    GrassHD::SDK::constructArray(gabi::ea(&at<dGrass_packet_c>(self)->mGrassData[0]), 1500, 0x44,
                                 Target::dGrass_data_c__ct);
    GrassHD::SDK::constructArray(gabi::ea(&at<dGrass_packet_c>(self)->mGrassAnm[0]), 104, 0x38,
                                 Target::dGrass_anm_c__ct);
    GrassHD::SDK::constructArray(gabi::ea(&at<dGrass_packet_c>(self)->mGrassRoom[0]), 64, 4,
                                 Target::dGrass_room_c__ct);
    u32 collection = gabi::ea(&at<dGrass_packet_c>(self)->mMaterials),
        resources = gabi::ea(&at<dGrass_packet_c>(self)->mResources);
    at<MaterialCollection>(collection)->mCount = 0;
    u32 pair = collection + 4;
    if (!pair)
        pair = GrassHD::SDK::operator_new(8);
    if (pair) {
        at<ShapePair>(pair)->mpShapes = 0;
        at<ShapePair>(pair)->mCount = 0;
    }
    u32 resourceStorage = resources;
    if (!resourceStorage)
        resourceStorage = GrassHD::SDK::operator_new(0x968);
    if (resourceStorage) {
        GrassHD::SDK::constructArray(resourceStorage, 4, 0x254, Target::Resource_create);
        at<Resources>(resourceStorage)->mSelected = 0;
        at<Resources>(resourceStorage)->mCount = 0;
        at<Resources>(resourceStorage)->mVertexStride = 48;
        at<Resources>(resourceStorage)->mReady = 0;
        at<Resources>(resourceStorage)->mVertexFormat = 0;
        for (u32 i = 0; i < 2; ++i)
            for (u32 j = 0; j < 2; ++j)
                at<Resources>(resourceStorage)->mBuffer[i + j * 2].mpBuffer = 0;
    }
    GrassHD::SDK::ModelBinding_construct(gabi::ea(&at<dGrass_packet_c>(self)->mModelBinding));
    GrassHD::SDK::constructArray(gabi::ea(&at<dGrass_packet_c>(self)->mModelPackets[0]), 1500, 0xA8,
                                 Target::ModelPacket_create);
    GrassHD::SDK::constructArray(gabi::ea(&at<dGrass_packet_c>(self)->mShaderPackets[0]), 64, 0x364,
                                 Target::ShaderPacket_create);
    GrassHD::SDK::IndexBuffer_construct(gabi::ea(&at<dGrass_packet_c>(self)->mIndices[0]));
    GrassHD::SDK::IndexBuffer_construct(gabi::ea(&at<dGrass_packet_c>(self)->mIndices[1]));
    GrassHD::SDK::Texture_construct(gabi::ea(&at<dGrass_packet_c>(self)->mTexture));
    GrassHD::SDK::TextureInfo_construct(gabi::ea(&at<dGrass_packet_c>(self)->mTextureInfo));
    for (u32 i = 0; i < 1500; ++i)
        at<dGrass_packet_c>(self)->mGrassData[i].mState = 0;
    at<dGrass_packet_c>(self)->mNextIdx = 0;
    for (u32 i = 0; i < 104; ++i)
        at<dGrass_packet_c>(self)->mGrassAnm[i].mState = 0;
    for (u32 i = 0; i < 8; ++i)
        GrassHD::SDK::GrassPacket_setAnm(self, i, (s32)(s16)(i * 0x2000));
    gabi::Local<String> name, archiveName, resourceName, stage, prefix, boss, other;
    at<String>(name.a)->mpString = Data::modelResourceName;
    at<String>(name.a)->mVtable = Data::safeStringVtable;
    u32 archive = GrassHD::SDK::Archive_get();
    s32 index = GrassHD::SDK::Directory_find((u32)(at<Archive>(archive)->mpDirectory), name.get());
    u32 entry = 0;
    if (index >= 0) {
        u32 count = (u32)(at<Archive>(archive)->mCount), entries = (u32)(at<Archive>(archive)->mpEntries);
        u32 record = entries + ((u32)index < count ? (u32)index * 36 : 0);
        if (!(u8)(at<ArchiveEntry>(record)->mLoaded)) {
            u32 directory = (u32)(at<Archive>(archive)->mpDirectory), source = 0;
            if ((u32)index < (u32)(at<Directory>(directory)->mCount))
                source = (u32)(at<Directory>(directory)->mpEntries) + (u32)index * 0x84;
            GrassHD::SDK::ArchiveEntry_load(record, source, 0);
            count = (u32)(at<Archive>(archive)->mCount);
            entries = (u32)(at<Archive>(archive)->mpEntries);
        }
        entry = entries + ((u32)index < count ? (u32)index * 36 : 0);
    }
    GrassHD::SDK::MaterialCollection_build(collection, entry, 0);
    at<Resources>(resources)->mVertexFormat = 0x1013;
    at<Resources>(resources)->mpVertexLayout = Data::vertexLayout;
    for (u32 outer = 0; outer < 2; ++outer) {
        for (u32 inner = 0; inner < 2; ++inner) {
            u32 resource = resources + outer * 0x254 + inner * 0x4A8;
            u32 buffer = (u32)(at<Resource>(resource)->mpBuffer);
            if (!buffer) {
                u32 allocated = grass_HeapAlloc(0xC00, 64);
                if (allocated) {
                    at<Resource>(resource)->mpAllocation = allocated;
                    at<Resource>(resource)->mAlignment = 64;
                }
                buffer = (u32)(at<Resource>(resource)->mpAllocation);
                at<Resource>(resource)->mpBuffer = buffer;
            }
            GrassHD::SDK::VertexBuffer_bind(resource + 4, buffer, 64, resources + 0x954);
        }
    }
    at<Resources>(resources)->mCount = 0;
    at<Resources>(resources)->mReady = 1;
    for (u32 i = 0; i < (u32)(at<MaterialCollection>(collection)->mCount); ++i) {
        u32 item = (u32)(at<MaterialCollection>(collection)->mpItems);
        if (i < (u32)(at<MaterialCollection>(collection)->mCapacity))
            item += i * 20;
        u32 material = (u32)(at<ShapeCollection>(item)->mpMaterial);
        grass_ClearShapes(item);
        at<ShapeCollection>(item)->mpMaterial = material;
        for (u32 offset = 8; offset <= 16; offset += 8) {
            u32 shapes = grass_HeapAlloc(0x1E8, 4);
            for (u32 j = 0; j < 2; ++j) {
                u32 shape = shapes + j * 0xF4;
                if (shape)
                    GrassHD::SDK::Shape_construct(shape);
            }
            if (shapes) {
                at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mpShapes = shapes;
                at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mCount = 2;
            }
        }
        for (u32 outer = 0; outer < 2; ++outer)
            for (u32 inner = 0; inner < 2; ++inner) {
                u32 owner = item + 4 + outer * 8;
                u32 shape = (u32)(at<ShapePair>(owner)->mpShapes);
                if (inner < (u32)(at<ShapePair>(owner)->mCount))
                    shape += inner * 0xF4;
                GrassHD::SDK::Shape_build(material, shape, resources + 4 + outer * 0x254 + inner * 0x4A8,
                                          resources + 0x954, 0);
            }
    }
    GrassHD::SDK::ModelBinding_setFlags(gabi::ea(&at<dGrass_packet_c>(self)->mModelBinding), 1, 0);
    for (u32 i = 0; i < 64; ++i)
        GrassHD::SDK::ModelPacket_setKind(gabi::ea(&at<dGrass_packet_c>(self)->mShaderPackets[i]), 0);
    for (u32 i = 0; i < 1500; ++i)
        GrassHD::SDK::ModelPacket_setKind(gabi::ea(&at<dGrass_packet_c>(self)->mModelPackets[i]), 0);
    at<String>(archiveName.a)->mpString = Data::textureArchiveName;
    at<String>(archiveName.a)->mVtable = Data::safeStringVtable;
    at<String>(resourceName.a)->mpString = Data::textureResourceName;
    at<String>(resourceName.a)->mVtable = Data::safeStringVtable;
    u32 texture = GrassHD::SDK::ResourceManager_getTexture((u32)(*at<be<u32>>(Data::resourceManager)),
                                                           archiveName.get(), resourceName.get(), 0);
    u32 play = GrassHD::SDK::dComIfGp_get();
    at<String>(stage.a)->mpString = play + 0x5134;
    at<String>(stage.a)->mVtable = Data::safeStringVtable;
    at<String>(prefix.a)->mpString = Data::resourceStageKin;
    at<String>(prefix.a)->mVtable = Data::safeStringVtable;
    GrassHD::SDK::SafeString_assureTermination(stage.get());
    grass_StringValidate(stage.get());
    u32 left = (u32)(at<String>(stage.a)->mpString);
    grass_StringValidate(prefix.get());
    u32 right = (u32)(at<String>(prefix.a)->mpString);
    bool alternate = left == right;
    if (!alternate) {
        alternate = true;
        left = (u32)(at<String>(stage.a)->mpString);
        for (u32 i = 0; i < 3; ++i, ++left, ++right) {
            u32 a = (u8)(*at<be<u8>>(left)), b = (u8)(*at<be<u8>>(right));
            if (!a || !b) {
                alternate = a == b;
                break;
            }
            if (a != b) {
                alternate = false;
                break;
            }
        }
    }
    if (!alternate) {
        at<String>(boss.a)->mpString = Data::resourceStageXboss;
        at<String>(boss.a)->mVtable = Data::safeStringVtable;
        play = GrassHD::SDK::dComIfGp_get();
        at<String>(other.a)->mpString = play + 0x5134;
        at<String>(other.a)->mVtable = Data::safeStringVtable;
        grass_StringValidate(boss.get());
        grass_StringValidate(boss.get());
        left = (u32)(at<String>(boss.a)->mpString);
        grass_StringValidate(other.get());
        right = (u32)(at<String>(other.a)->mpString);
        alternate = left == right;
        if (!alternate) {
            left = (u32)(at<String>(boss.a)->mpString);
            alternate = true;
            for (u32 i = 0; i < 0x40001; ++i, ++left, ++right) {
                u32 a = (u8)(*at<be<u8>>(left)), b = (u8)(*at<be<u8>>(right));
                if (a != b) {
                    alternate = false;
                    break;
                }
                if (!a)
                    break;
                if (i == 0x40000)
                    alternate = false;
            }
        }
    }
    u32 indices = gabi::ea(&at<dGrass_packet_c>(self)->mIndices[0]);
    GrassHD::SDK::IndexBuffer_set(
        indices, alternate ? Data::forestIndices : Data::grassIndices, 4,
        (u32)*at<be<u32>>(alternate ? Data::grassPositions : Data::grassIndexCount));
    at<IndexBuffer>(indices)->mpIndices = 4;
    for (u32 kind = 0; kind < 2; ++kind) {
        u32 selected = resources + (u32)(at<Resources>(resources)->mSelected) * 0x254;
        u32 buffer = (u32)(at<Resource>(selected + kind * sizeof(Resource) * 2)->mpBuffer);
        grass_ClearVertexBuffer(buffer);
        selected = resources + (u32)(at<Resources>(resources)->mSelected) * 0x254;
        buffer = (u32)(at<Resource>(selected + kind * sizeof(Resource) * 2)->mpBuffer);
        if (alternate) {
            if (!kind)
                grass_PopulateVertices(buffer, Data::forestVertexCount, Data::forestPositions,
                                       Data::forestNormals, Data::forestColors, Data::forestTexCoords);
            else
                grass_PopulateVertices(buffer, Data::forestCutVertexCount, Data::forestCutPositions,
                                       Data::forestCutNormals, Data::forestCutColors,
                                       Data::forestCutTexCoords);
        } else {
            if (!kind)
                grass_PopulateVertices(buffer, Data::grassVertexCount, Data::grassPositions,
                                       Data::grassNormals, Data::grassColors, Data::grassTexCoords);
            else
                grass_PopulateVertices(buffer, Data::cutVertexCount, Data::cutPositions, Data::cutNormals,
                                       Data::cutColors, Data::cutTexCoords);
        }
    }
    u32 original = (u32)(at<Resources>(resources)->mSelected), destination = resources + (!original) * 0x254;
    for (u32 kind = 0; kind < 2; ++kind, destination += 0x4A8) {
        u32 selected = resources + (original + kind * 2) * 0x254;
        for (u32 i = 0; i < 64; ++i) {
            u32 source = (u32)(at<Resource>(selected)->mpBuffer),
                target = (u32)(at<Resource>(destination)->mpBuffer);
            for (u32 j = 0; j < 12; ++j)
                *reinterpret_cast<be<u32> *>(&at<Matrix>(target)[i].m[j]) =
                    *reinterpret_cast<be<u32> *>(&at<Matrix>(source)[i].m[j]);
        }
        original = (u32)(at<Resources>(resources)->mSelected);
    }
    u32 selected = resources + (u32)(at<Resources>(resources)->mSelected) * 0x254 + 4;
    for (u32 i = 0; i < 2; ++i)
        GrassHD::SDK::VertexBuffer_invalidate(
            selected + i * 0x4A8, 0, (u32)at<VertexBuffer>(selected + i * sizeof(Resource) * 2)->mBufferSize);
    at<Resources>(resources)->mSelected = !(u32)(at<Resources>(resources)->mSelected);
    GrassHD::SDK::DrawLock_lock((u32)(*at<be<u32>>(Data::drawLock)));
    u32 textureInfo = gabi::ea(&at<dGrass_packet_c>(self)->mTextureInfo);
    GrassHD::SDK::TextureInfo_load(textureInfo, texture,
                                   alternate ? Data::forestTextureName : Data::grassTextureName);
    GrassHD::SDK::DrawLock_unlock((u32)(*at<be<u32>>(Data::drawLock)));
    at<dGrass_packet_c>(self)->mCoParticle = alternate ? 0x8222 : 0x3DB;
    at<dGrass_packet_c>(self)->mAtParticle = alternate ? 0x8221 : 0x3DA;
    GrassHD::SDK::IndexBuffer_set(gabi::ea(&at<dGrass_packet_c>(self)->mIndices[1]), Data::cutIndices, 4,
                                  (u32)(*at<be<u32>>(Data::cutIndexCount)));
    at<dGrass_packet_c>(self)->mIndices[1].mpIndices = 4;
    u32 target = gabi::ea(&at<dGrass_packet_c>(self)->mTexture);
    bool equal = true;
    const u32 fields[] = {4, 8, 12, 16, 20, 24, 56, 52, 28};
    for (u32 offset : fields)
        if ((u32)(at<Texture>(target)->mWords[(offset) / 4]) !=
            (u32)(at<TextureInfo>(textureInfo)->mWords[(offset) / 4])) {
            equal = false;
            break;
        }
    if (!equal)
        GrassHD::SDK::Texture_assign(target, textureInfo);
    else {
        u32 first = (u32)(at<TextureInfo>(textureInfo)->mWords[(40) / 4]),
            second = (u32)(at<TextureInfo>(textureInfo)->mWords[(48) / 4]);
        at<Texture>(target)->mWords[10] = first;
        at<Texture>(target)->mFlags = (u8)(at<Texture>(target)->mFlags) | 2;
        at<Texture>(target)->mWords[55] = second;
        at<Texture>(target)->mWords[12] = second;
        at<Texture>(target)->mWords[53] = first;
    }
    at<Texture>(target)->mWords[88] = 1;
    at<Texture>(target)->mWords[87] = 1;
    at<Texture>(target)->mWords[89] = 1;
    if (!equal)
        at<Texture>(target)->mFlags = (u8)(at<Texture>(target)->mFlags) | 2;
    return self;
}

/* 0254D4D8 */
u32 dGrass_room_c__ct(u32 self) {

    if (!self)
        self = GrassHD::SDK::operator_new(4);
    if (self)
        at<dGrass_room_c>(self)->mpData = 0;
    return self;
}

/* 0254D49C */
u32 dGrass_anm_c__ct(u32 self) {

    if (!self)
        self = GrassHD::SDK::operator_new(56);
    if (self)
        at<dGrass_anm_c>(self)->mState = 0;
    return self;
}

/* 0254A0DC */
u32 dGrass_data_c__ct(u32 self) {

    if (!self) {
        self = GrassHD::SDK::operator_new(0x44);
        if (!self)
            return 0;
    }
    at<dGrass_data_c>(self)->mItemIdx = 0;
    at<dGrass_data_c>(self)->mAnimIdx = 0;
    at<dGrass_data_c>(self)->mState = 0;
    at<dGrass_data_c>(self)->mInitFlags = 0;
    GrassHD::SDK::clearMemory(self + 16, 48);
    at<dGrass_data_c>(self)->mpNextData = 0;
    at<dGrass_data_c>(self)->mState = 0;
    return self;
}

/* 0254C120 */
void GrassHD::dGrass_packet_c::draw(u32 state) {
    u32 self = gabi::ea(this);

    u32 pass = (u32)(at<RenderContext>(state)->mPass), material = 0;
    if ((s32)pass < 4)
        material = (u32)at<ShapeCollection>(grass_Collection(self, pass))->mpMaterial;
    u32 cache = GrassHD::SDK::ShaderCache_get(Data::shaderContext);
    u32 model = (u32)(at<Material>(material)->mpModel);
    if (model != (u32)(at<ShaderCache>(cache)->mpModel)) {
        u32 flag = (u8)(at<Model>(model)->mFlags), previous = (u32)(at<ShaderCache>(cache)->mpShader);
        if (flag & 2) {
            at<Model>(model)->mFlags = flag & ~2u;
            GrassHD::SDK::Model_update(model, 0);
        }
        u32 shader = (u32)at<ShaderResource>((u32)at<Model>(model)->mpShaderResource)->mpShader;
        if (previous != shader)
            GrassHD::SDK::Shader_bind(shader);
        u32 size = (u32)(at<Model>(model)->mDisplayListSize);
        if (size)
            GrassHD::SDK::GX2CallDisplayList((u32)(at<Model>(model)->mpDisplayList), size);
        else
            GrassHD::SDK::Model_draw(model);
        at<ShaderCache>(cache)->mpShader = shader;
        at<ShaderCache>(cache)->mpModel = model;
    }
    pass = (u32)(at<RenderContext>(state)->mPass);
    bool bind = pass == 0 ? (u32)(at<RenderContext>(state)->mpModelBinding) != 0 : (pass == 1 || pass == 2);
    if (bind) {
        u32 data = pass == 0
                       ? (u32)at<ModelBinding>((u32)at<RenderContext>(state)->mpModelBinding)->mpRenderer
                       : (u32)(mModelBinding.mpRenderer);
        u32 block = data + 16 + (u32)(at<UniformData>(data)->mBlockIndex) * 28;
        u32 info =
            (u32)(at<Material>(material)->mShaderCount) ? (u32)(at<Material>(material)->mpShaderInfo) : 0;
        s32 vertex = (s16)(at<ShaderInfo>(info)->mVertexBlock),
            pixel = (s16)(at<ShaderInfo>(info)->mPixelBlock),
            geometry = (s16)(at<ShaderInfo>(info)->mGeometryBlock);
        u32 buffer = (u32)(at<UniformBlock>(block)->mpBuffer), size = (u32)(at<UniformBlock>(block)->mSize);
        if (vertex != -1 || pixel != -1 || geometry != -1) {
            if (pixel != -1)
                GrassHD::SDK::GX2SetPixelUniformBlock(pixel, buffer, size);
            if (vertex != -1)
                GrassHD::SDK::GX2SetVertexUniformBlock(vertex, buffer, size);
            if (geometry != -1)
                GrassHD::SDK::GX2SetGeometryUniformBlock(geometry, buffer, size);
        }
    }
    if (pass == 2) {
        u32 object = (u32)(at<RenderContext>(state)->mpCallback);
        if (object)
            gabi::call_ptr<void>((u32)at<DispatchTable>((u32)at<ModelPacket>(object)->mVtable)->mpGetShape,
                                 object, material);
        GrassHD::SDK::RenderContext_apply(state, material);
    }
    u32 attribute =
        (u32)(at<Material>(material)->mAttributeCount) ? (u32)(at<Material>(material)->mpAttributeInfo) : 0;
    GrassHD::SDK::Texture_bind(gabi::ea(&mTexture), attribute + 4, 0, 0);
    gabi::Local<RenderState> render;
    GrassHD::SDK::RenderState_construct(render.get());
    u32 mode = (u32)(at<RenderState>(render.a)->mRasterMode);
    at<RenderState>(render.a)->mDepthBias = (f32)(*at<be<f32>>(Data::depthBias));
    at<RenderState>(render.a)->mBlendMode = 2;
    at<RenderState>(render.a)->mDepthEnable = 1;
    at<RenderState>(render.a)->mDepthFunc = 4;
    at<RenderState>(render.a)->mRasterMode = (((mode & 0xFFFFFFF0) + 7) & 0xFFFFFF0F) + 16;
    GrassHD::SDK::RenderState_bind((u32)(at<RenderContext>(state)->mPass), render.get());
    GrassHD::SDK::RenderState_apply(render.get());
    u32 packetOffset = 0;
    for (u32 room = 0; room < 64; ++room) {
        u32 data = (u32)(mGrassRoom[room].mpData);
        if (data && (u32)(at<RenderContext>(state)->mPass)) {
            u32 packet = gabi::ea(&mShaderPackets[0]) + room * 0x364;
            gabi::call_ptr<void>((u32)at<DispatchTable>((u32)at<ModelPacket>(packet)->mVtable)->mpGetShape,
                                 packet, material);
        }
        s32 kind = -1;
        while (data) {
            if (!((u8)at<dGrass_data_c>(data)->mInitFlags & 2)) {
                u32 packet = gabi::ea(&mModelPackets[0]) + packetOffset;
                gabi::call_ptr<void>(
                    (u32)at<DispatchTable>((u32)at<ModelPacket>(packet)->mVtable)->mpGetShape, packet,
                    material);
                packetOffset += 0xA8;
                bool animated = (s8)at<dGrass_data_c>(data)->mAnimIdx >= 0;
                s32 nextKind = animated ? 0 : 1;
                if (kind != nextKind) {
                    u32 collection = grass_Collection(self, (u32)(at<RenderContext>(state)->mPass));
                    if (!(u32)(mResources.mSelected))
                        collection += 8;
                    u32 shape = (u32)(at<MaterialCollection>(collection)->mpItems);
                    if (!animated && (u32)(at<MaterialCollection>(collection)->mCapacity) > 1)
                        shape += 0xF4;
                    GrassHD::SDK::Shape_bind(shape);
                    kind = nextKind;
                }
                u32 indices = gabi::ea(&mIndices[0]) + (animated ? 0 : 24);
                u32 count = (u32)(at<IndexBuffer>(indices)->mCount);
                if (count)
                    GrassHD::SDK::GX2DrawIndexedEx((u32)(at<IndexBuffer>(indices)->mpIndices), count,
                                                   (u32)(at<IndexBuffer>(indices)->mPrimitive),
                                                   (u32)(at<IndexBuffer>(indices)->mIndexType), 0, 1);
            }
            data = (u32)at<dGrass_data_c>(data)->mpNextData;
        }
    }
    GrassHD::SDK::RenderState_apply(Data::defaultRenderState);
}

/* 0254C6C4 */
void GrassHD::dGrass_packet_c::calc() {

    float wind = (f32)(*at<be<f32>>(Data::zero));
    bool enabled = true;
    if ((u8)(*at<be<u8>>(Data::monotone))) {
        gabi::Local<String> named, stage;
        at<String>(named.a)->mpString = Data::stageHyrule;
        at<String>(named.a)->mVtable = Data::safeStringVtable;
        u32 play = GrassHD::SDK::dComIfGp_get();
        at<String>(stage.a)->mpString = play + 0x5134;
        at<String>(stage.a)->mVtable = Data::safeStringVtable;
        grass_StringValidate(named.get());
        grass_StringValidate(named.get());
        u32 left = (u32)(at<String>(named.a)->mpString);
        grass_StringValidate(stage.get());
        u32 right = (u32)(at<String>(stage.a)->mpString);
        if (left == right)
            enabled = false;
        else {
            left = (u32)(at<String>(named.a)->mpString);
            for (u32 count = 0; count < 0x40001; ++count, ++left, ++right) {
                u32 a = (u8)(*at<be<u8>>(left)), b = (u8)(*at<be<u8>>(right));
                if (a != b)
                    break;
                if (!a) {
                    enabled = false;
                    break;
                }
            }
        }
    }
    if (enabled) {
        wind = (f32)(*at<be<f32>>(Data::windScale));
        float power = GrassHD::SDK::dKyw_get_wind_pow();
        wind = gabi::fmadds(wind, power, wind);
        float maximum = (f32)(*at<be<f32>>(Data::maxWind));
        if (wind - maximum >= 0)
            wind = maximum;
    }
    for (u32 i = 0; i < 8; ++i) {
        u32 time = (u32)(*at<be<u32>>(Data::counterTimer)) + i * 250;
        u16 angle = (u16)gabi::ftoi(gabi::fmuls_ppc((float)time, wind));
        float cosine = (f32)at<SinCos>(Data::sincosTable)[(u32)angle >> 3].mCos;
        mGrassAnm[i].mRotX = (u16)gabi::ftoi(gabi::fmadds(wind, cosine, wind));
    }
    s32 room = (s8)(*at<be<s8>>(Data::stayRoomNo));
    u32 data = (u32)mGrassRoom[(u32)room].mpData;
    if (!data)
        return;
    *at<be<u8>>(Data::cutSoundFlag) = 0;
    u32 play = GrassHD::SDK::dComIfGp_get(), attribute = play + 0x26A4, mass = play + 0x4EF8;
    GrassHD::SDK::cM3dGCyl__SetR(play + 0x5008, (f32)(*at<be<f32>>(Data::bendRadius)));
    GrassHD::SDK::cM3dGCyl__SetH(mass + 0x110, (f32)(*at<be<f32>>(Data::massHeight)));
    at<MassAttribute>(attribute)->mType = 11;
    at<MassAttribute>(attribute)->mRoom = 0;
    do {
        if (!((u8)(at<dGrass_data_c>(data)->mInitFlags) & 2) && (s8)(at<dGrass_data_c>(data)->mAnimIdx) >= 0)
            GrassHD::SDK::dGrass_data_c__hitCheck(data, room);
        data = (u32)(at<dGrass_data_c>(data)->mpNextData);
    } while (data);
}

/* 02549F4C */
f32 checkGroundY(u32 pos) {

    gabi::Local<Ground> ground;
    GrassHD::SDK::cBgS_GndChk__cBgS_GndChk(ground.get());
    f32 y = (f32)(at<Vector>(pos)->y), x = (f32)(at<Vector>(pos)->x);
    ground->mFlags[0] = 0;
    ground->mInterfaceVtable = Data::groundInterfaceVtable;
    f32 lift = (f32)(*at<be<f32>>(Data::groundLift));
    ground->mCheckVtable = Data::groundCheckVtable;
    ground->mFlags[5] = 0;
    y += lift;
    ground->mMask = 1;
    ground->mFlags[1] = 0;
    ground->mPos.y = y;
    at<Vector>(pos)->y = y;
    ground->mPolyVtable = Data::groundPolyVtable;
    ground->mPos.x = x;
    ground->mFlags[6] = 0;
    ground->mpInterface = ground.a + 0x4C;
    ground->mFlags[3] = 0;
    ground->mFlags[4] = 0;
    ground->mObjectVtable = Data::groundVtable;
    f32 z = (f32)(at<Vector>(pos)->z);
    ground->mFlags[2] = 0;
    ground->mVtable = ground.a + 0x40;
    ground->mPos.z = z;
    u32 play = GrassHD::SDK::dComIfGp_get();
    f32 height = GrassHD::SDK::cBgS__GroundCross(play + 0x12A0, ground.get());
    y = (f32)(at<Vector>(pos)->y) - lift;
    ground->mObjectVtable = Data::groundVtable;
    ground->mCheckVtable = Data::groundCheckVtable;
    f32 limit = (f32)(*at<be<f32>>(Data::groundLimit));
    ground->mInterfaceVtable = Data::groundBaseVtable;
    at<Vector>(pos)->y = y;
    bool valid = height > limit;
    GrassHD::SDK::cBgS_Chk___cBgS_Chk(ground.get(), 0);
    return valid ? height : y;
}

/* 0254CF78 */
void GrassHD::dGrass_packet_c::update() {
    u32 self = gabi::ea(this);

    const u32 matrix = Data::matrixStack;
    for (u32 i = 0; i < 104; ++i) {
        u32 animation = gabi::ea(&mGrassAnm[0]) + i * 0x38;
        GrassHD::SDK::mDoMtx_YrotS(matrix, (s32)(s16)(at<dGrass_anm_c>(animation)->mRotY));
        GrassHD::SDK::mDoMtx_XrotM(matrix, (s32)(s16)(at<dGrass_anm_c>(animation)->mRotX));
        GrassHD::SDK::mDoMtx_YrotM(matrix, (s32)(s16) - (s16)(at<dGrass_anm_c>(animation)->mRotY));
        GrassHD::SDK::PSMTXCopy(matrix, animation + 8);
    }
    const u32 clipper = Data::clipper;
    float scale = (f32)(*at<be<f32>>(Data::originalFar));
    at<Clipper>(clipper)->mFar = gabi::fmuls_ppc(scale, (f32)(*at<be<f32>>(Data::farScale)));
    GrassHD::SDK::J3DUClipper__calcViewFrustum(clipper);
    float radius = (f32)(*at<be<f32>>(Data::clipRadius));
    u32 corrected = 0;
    for (u32 i = 0; i < 1500; ++i) {
        u32 data = gabi::ea(&mGrassData[0]) + i * 0x44;
        u32 state = (u8)(at<dGrass_data_c>(data)->mState);
        if (!state)
            continue;
        if (state == 1 && corrected < 30) {
            float height = GrassHD::SDK::checkGroundY(data + 4);
            at<dGrass_data_c>(data)->mPos.y = height;
            ++corrected;
            at<dGrass_data_c>(data)->mState = 2;
        }
        gabi::Local<Vector> position;
        for (u32 j = 0; j < 3; ++j)
            at<FloatVectorWords>(position.a)->mWords[j] = (u32)at<dGrass_data_c>(data)->mPosWords[j];
        u32 clipped = GrassHD::SDK::J3DUClipper__clip(clipper, Data::viewMatrix, position.get(), radius);
        u32 flags = (u8)(at<dGrass_data_c>(data)->mInitFlags);
        if (clipped) {
            at<dGrass_data_c>(data)->mInitFlags = flags | 2;
            continue;
        }
        s32 index = (s8)(at<dGrass_data_c>(data)->mAnimIdx);
        at<dGrass_data_c>(data)->mInitFlags = flags & 0xFD;
        float x = (f32)(at<dGrass_data_c>(data)->mPos.x);
        if (index >= 0) {
            u32 animation = gabi::ea(&mGrassAnm[0]) + (u32)index * 0x38;
            at<dGrass_anm_c>(animation)->mAnimMtx.m[3] = x;
            *reinterpret_cast<be<u32> *>(&at<dGrass_anm_c>(animation)->mAnimMtx.m[7]) =
                (u32)(*reinterpret_cast<be<u32> *>(&at<dGrass_data_c>(data)->mPos.y));
            *reinterpret_cast<be<u32> *>(&at<dGrass_anm_c>(animation)->mAnimMtx.m[11]) =
                (u32)(*reinterpret_cast<be<u32> *>(&at<dGrass_data_c>(data)->mPos.z));
            GrassHD::SDK::PSMTXCopy(animation + 8, data + 16);
        } else {
            GrassHD::SDK::PSMTXTrans(data + 16, x, (f32)(at<dGrass_data_c>(data)->mPos.y),
                                     (f32)(at<dGrass_data_c>(data)->mPos.z));
            GrassHD::SDK::mDoMtx_YrotM(data + 16, (s32)(s16)(i * 0xDCF));
        }
    }
    // The PPC load/store restores the original bits, including signaling NaN payloads.
    // HD: Restore the exact saved word, including non-finite float encodings.
    *reinterpret_cast<be<u32> *>(&at<Clipper>(clipper)->mFar) = (u32)*at<be<u32>>(Data::originalFar);
    GrassHD::SDK::J3DUClipper__calcViewFrustum(clipper);
    GrassHD::SDK::J3DDrawBuffer__entryImm((u32)(*at<be<u32>>(Data::opaqueDrawBuffer)), self, 0);
    GrassHD::SDK::buildShaders(self);
}

/* 0254D19C */
void GrassHD::dGrass_packet_c::setData(u32 data, u32 index, u32 pos, u32 room, u32 item) {

    bool immediate = GrassHD::SDK::fopOvlpM_IsPeek() != 0;
    f32 height;
    if (immediate) {
        height = GrassHD::SDK::checkGroundY(pos);
        at<dGrass_data_c>(data)->mState = 2;
    } else {
        height = (f32)(at<Vector>(pos)->y);
        at<dGrass_data_c>(data)->mState = 1;
    }
    f32 maximum = (f32)(*at<be<f32>>(Data::baseAnimationCount));
    at<dGrass_data_c>(data)->mInitFlags = 2;
    f32 random = GrassHD::SDK::cM_rndF(maximum);
    at<dGrass_data_c>(data)->mAnimIdx = (u8)gabi::ftoi(random);
    f32 x = (f32)(at<Vector>(pos)->x), z = (f32)(at<Vector>(pos)->z);
    at<dGrass_data_c>(data)->mPos.x = x;
    at<dGrass_data_c>(data)->mPos.y = height;
    at<dGrass_data_c>(data)->mItemIdx = item;
    at<dGrass_data_c>(data)->mPos.z = z;
    GrassHD::SDK::dGrass_room_c__newData(gabi::ea(&mGrassRoom[room]), data);
    mNextIdx = index;
}

/* 0254D2D4 */
u32 GrassHD::dGrass_packet_c::newData(u32 pos, u32 room, u32 item) {
    u32 self = gabi::ea(this);

    if (room >= 64)
        GrassHD::SDK::JUTAssertion__getSDevice(Data::allocationAssertFile, 0xB00,
                                               Data::allocationAssertCondition);
    u32 start = (u16)(mNextIdx), data = gabi::ea(&mGrassData[start]);
    for (u32 index = start; index < 1500; ++index, data += 0x44) {
        if (!(u8)(at<dGrass_data_c>(data)->mState)) {
            GrassHD::SDK::dGrass_packet_c__setData(self, data, index, pos, room, item);
            return data;
        }
    }
    data = gabi::ea(&mGrassData[0]);
    for (u32 index = 0; index < start; ++index, data += 0x44) {
        if (!(u8)(at<dGrass_data_c>(data)->mState)) {
            GrassHD::SDK::dGrass_packet_c__setData(self, data, index, pos, room, item);
            return data;
        }
    }
    return 0;
}

/* 0254A14C */
s32 GrassHD::dGrass_packet_c::newAnm() {

    u32 data = gabi::ea(&mGrassAnm[8]);
    for (s32 i = 8; i < 104; ++i, data += 0x38) {
        if (!(u8)(at<dGrass_anm_c>(data)->mState)) {
            at<dGrass_anm_c>(data)->mState = 1;
            at<dGrass_anm_c>(data)->mRotX = 0;
            at<dGrass_anm_c>(data)->mRotY = 0;
            return i;
        }
    }
    return -1;
}

/* 0254ACB4 */
void GrassHD::dGrass_packet_c::setAnm(u32 index, s32 angle) {

    u32 data = gabi::ea(&mGrassAnm[index]);
    at<dGrass_anm_c>(data)->mState = 1;
    at<dGrass_anm_c>(data)->mRotX = 0;
    at<dGrass_anm_c>(data)->mRotY = angle;
}

// HD: Compiler-generated and template code of this translation unit.

/* 0254C934 */
void copyMatrix(u32 dest, u32 source) {

    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)(at<Matrix>(source)->m[i]);
    for (u32 i = 0; i < 12; ++i)
        at<Matrix>(dest)->m[i] = values[i];
}

/* 0254C9D4 */
void colorToFloat(u32 dest, u32 source) {

    f32 r = (f32)(s16)(at<ColorS>(source)->mChannel[0]), b = (f32)(s16)(at<ColorS>(source)->mChannel[2]);
    f32 g = (f32)(s16)(at<ColorS>(source)->mChannel[1]), a = (f32)(s16)(at<ColorS>(source)->mChannel[3]);
    f32 denominator = (f32)(*at<be<f32>>(Data::colorRange));
    at<ColorF>(dest)->mChannel[0] = r / denominator;
    at<ColorF>(dest)->mChannel[1] = g / denominator;
    at<ColorF>(dest)->mChannel[2] = b / denominator;
    at<ColorF>(dest)->mChannel[3] = a / denominator;
}

/* 0254D3F4 */
void __sinit_d_grass_cpp() {

    *at<be<u32>>(Data::zeroVectorX) = 0;
    *at<be<u32>>(Data::zeroAngleX) = 0;
    *at<be<u32>>(Data::zeroVectorY) = 0;
    *at<be<u32>>(Data::zeroAngleY) = 0;
    GrassHD::SDK::Vector_staticInit(Data::vectorStatic);
    f32 lower = (f32)(*at<be<f32>>(Data::piLower)), upper = (f32)(*at<be<f32>>(Data::piUpper));
    *at<be<f32>>(Data::piPair) = lower;
    *at<be<f32>>(Data::piPairUpper) = upper;
    GrassHD::SDK::Angle_staticInit(Data::byteStatic);
    GrassHD::SDK::Vector_staticInit(Data::angleStatic);
    GrassHD::SDK::ShortVector_staticInit(Data::shortStatic);
    GrassHD::SDK::Vector_staticInit(Data::shortVectorStatic);
}

/* 0254D488 */
void SafeString_delete(u32 self, u32 flags) {

    if (self && (flags & 1))
        GrassHD::SDK::operator_delete(self);
}

/* 0254D570 */
u32 ModelPacket_create(u32 self) {

    if (!self)
        self = GrassHD::SDK::operator_new(0xA8);
    if (self) {
        GrassHD::SDK::ModelPacket_construct(self);
        at<ModelPacket>(self)->mVtable = Data::modelPacketVtable;
        GrassHD::SDK::clearMemory(self + 0x74, 0x34);
        if (self + 0x74 == 0)
            GrassHD::SDK::operator_new(0x30);
    }
    return self;
}

/* 0254D5E0 */
u32 ColorF_construct(u32 self) { return self ? self : GrassHD::SDK::operator_new(0x10); }

/* 0254D514 */
u32 Resource_create(u32 self) {

    if (!self)
        self = GrassHD::SDK::operator_new(0x254);
    if (self) {
        GrassHD::SDK::VertexBuffer_construct(self + 4);
        GrassHD::SDK::Shape_construct(self + 0x158);
        at<Resource>(self)->mpAllocation = 0;
        at<Resource>(self)->mAlignment = 0;
    }
    return self;
}

/* 0254D93C */
void Resource_delete(u32 self, u32 flags) {

    if (self) {
        GrassHD::SDK::Shape_destruct(self + 0x158, 2);
        GrassHD::SDK::VertexBuffer_destruct(self + 4, 2);
        if (flags & 1)
            GrassHD::SDK::operator_delete(self);
    }
}

/* 0254D894 */
void ModelPacket_delete(u32 self, u32 flags) {

    if (self) {
        GrassHD::SDK::ModelPacket_destruct(self, 0);
        if (flags & 1)
            GrassHD::SDK::operator_delete(self);
    }
}

/* 0254D99C */
void SafeString_assureTerminationImpl(u32 self) {}

/* 0254D9A0 */
void SafeString_assureTermination(u32 self) {}

/* 0254D8E8 */
void ShaderPacket_delete(u32 self, u32 flags) {

    if (self) {
        GrassHD::SDK::ModelPacket_destruct(self, 0);
        if (flags & 1)
            GrassHD::SDK::operator_delete(self);
    }
}

/* 0254D60C */
u32 ShaderPacket_create(u32 self) {

    if (!self) {
        self = GrassHD::SDK::operator_new(0x364);
        if (!self)
            return 0;
    }
    GrassHD::SDK::ModelPacket_construct(self);
    at<ShaderPacket>(self)->mVtable = Data::shaderPacketVtable;
    GrassHD::SDK::clearMemory(self + 0x74, 0x2F0);
    f32 zero = (f32)(*at<be<f32>>(Data::shaderZero)), one = (f32)(*at<be<f32>>(Data::shaderOne));
    at<ShaderPacket>(self)->mInitialValues[0] = zero;
    at<ShaderPacket>(self)->mInitialValues[1] = zero;
    at<ShaderPacket>(self)->mInitialValues[2] = zero;
    at<ShaderPacket>(self)->mInitialValues[4] = zero;
    at<ShaderPacket>(self)->mInitialValues[11] = one;
    at<ShaderPacket>(self)->mInitialValues[14] = zero;
    at<ShaderPacket>(self)->mInitialValues[7] = one;
    at<ShaderPacket>(self)->mInitialValues[6] = zero;
    at<ShaderPacket>(self)->mInitialValues[26] = zero;
    at<ShaderPacket>(self)->mInitialValues[34] = zero;
    at<ShaderPacket>(self)->mInitialValues[22] = zero;
    at<ShaderPacket>(self)->mInitialValues[31] = one;
    at<ShaderPacket>(self)->mInitialValues[12] = zero;
    at<ShaderPacket>(self)->mInitialValues[21] = zero;
    at<ShaderPacket>(self)->mInitialValues[33] = zero;
    at<ShaderPacket>(self)->mInitialValues[29] = zero;
    at<ShaderPacket>(self)->mInitialValues[17] = zero;
    at<ShaderPacket>(self)->mInitialValues[8] = zero;
    at<ShaderPacket>(self)->mInitialValues[13] = zero;
    at<ShaderPacket>(self)->mInitialValues[15] = one;
    at<ShaderPacket>(self)->mInitialValues[9] = zero;
    at<ShaderPacket>(self)->mInitialValues[18] = zero;
    at<ShaderPacket>(self)->mInitialValues[5] = zero;
    at<ShaderPacket>(self)->mInitialValues[28] = zero;
    at<ShaderPacket>(self)->mInitialValues[30] = zero;
    at<ShaderPacket>(self)->mInitialValues[35] = one;
    at<ShaderPacket>(self)->mInitialValues[23] = one;
    at<ShaderPacket>(self)->mInitialValues[27] = one;
    at<ShaderPacket>(self)->mInitialValues[25] = zero;
    at<ShaderPacket>(self)->mInitialValues[20] = zero;
    at<ShaderPacket>(self)->mInitialValues[32] = zero;
    at<ShaderPacket>(self)->mInitialValues[10] = zero;
    at<ShaderPacket>(self)->mInitialValues[24] = zero;
    at<ShaderPacket>(self)->mInitialValues[3] = one;
    at<ShaderPacket>(self)->mInitialValues[16] = zero;
    at<ShaderPacket>(self)->mInitialValues[19] = one;
    at<ShaderPacket>(self)->mInitialValues[36] = zero;
    at<ShaderPacket>(self)->mInitialValues[37] = zero;
    at<ShaderPacket>(self)->mInitialValues[38] = zero;
    at<ShaderPacket>(self)->mInitialValues[39] = one;
    at<ShaderPacket>(self)->mInitialValues[40] = zero;
    at<ShaderPacket>(self)->mInitialValues[41] = zero;
    at<ShaderPacket>(self)->mInitialValues[42] = zero;
    at<ShaderPacket>(self)->mInitialValues[43] = one;
    for (u32 off : {0x124u, 0x144u, 0x164u})
        GrassHD::SDK::constructArray(self + off, 2, 16, Target::ColorF_construct);
    for (u32 off = 0x184; off <= 0x2D4; off += 0x30)
        if (!(self + off))
            GrassHD::SDK::operator_new(0x30);
    for (u32 off = 0x304; off <= 0x354; off += 0x10)
        if (!(self + off))
            GrassHD::SDK::operator_new(0x10);
    return self;
}

/* 0254BD34 */
void dGrass_packet_c__dt(u32 self, u32 flags) {

    if (!self)
        return;
    u32 resources = gabi::ea(&at<dGrass_packet_c>(self)->mResources),
        collection = gabi::ea(&at<dGrass_packet_c>(self)->mMaterials);
    at<dGrass_packet_c>(self)->mVtable = Data::grassPacketVtable;
    grass_ReleaseResources(resources);
    GrassHD::SDK::Texture_destruct(gabi::ea(&at<dGrass_packet_c>(self)->mTexture), 2);
    GrassHD::SDK::IndexBuffer_destruct(gabi::ea(&at<dGrass_packet_c>(self)->mIndices[1]), 2);
    GrassHD::SDK::IndexBuffer_destruct(gabi::ea(&at<dGrass_packet_c>(self)->mIndices[0]), 2);
    GrassHD::SDK::destroyArray(gabi::ea(&at<dGrass_packet_c>(self)->mShaderPackets[0]), 64, 0x364,
                               Target::ModelPacket_delete, 0, 0);
    GrassHD::SDK::destroyArray(gabi::ea(&at<dGrass_packet_c>(self)->mModelPackets[0]), 1500, 0xA8,
                               Target::ShaderPacket_delete, 0, 0);
    GrassHD::SDK::ModelBinding_destruct(gabi::ea(&at<dGrass_packet_c>(self)->mModelBinding), 2);
    if (resources) {
        grass_ReleaseResources(resources);
        GrassHD::SDK::destroyArray(resources, 4, 0x254, Target::Resource_delete, 0, 0);
    }
    u32 array = (u32)(at<MaterialCollection>(collection)->mpItems);
    if (array) {
        s32 count = (s32)(at<MaterialCollection>(collection)->mCapacity);
        for (s32 i = 0; i < count; ++i) {
            u32 item = array + (u32)i * 20;
            if (item) {
                at<ShapeCollection>(item)->mpMaterial = 0;
                for (u32 offset = 8; offset <= 16; offset += 8) {
                    u32 children = (u32)(at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mpShapes);
                    if (!children)
                        continue;
                    s32 size = (s32)(at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mCount);
                    for (s32 j = 0; j < size; ++j) {
                        u32 child = children + (u32)j * 0xF4;
                        u32 table = (u32)(at<Shape>(child)->mVtable);
                        gabi::call_ptr<void>((u32)at<DispatchTable>(table)->mpDelete, child, 2);
                        size = (s32)(at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mCount);
                        children = (u32)(at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mpShapes);
                    }
                    grass_ReleaseAllocation(item, offset);
                    at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mCount = 0;
                    at<ShapeCollection>(item)->mShape[(offset - 8) / 8].mpShapes = 0;
                }
                count = (s32)(at<MaterialCollection>(collection)->mCapacity);
                array = (u32)(at<MaterialCollection>(collection)->mpItems);
            }
        }
        grass_ReleaseAllocation(collection, 8);
        at<MaterialCollection>(collection)->mCapacity = 0;
        at<MaterialCollection>(collection)->mpItems = 0;
    }
    GrassHD::SDK::Packet_destruct(self, 0);
    if (flags & 1)
        GrassHD::SDK::operator_delete(self);
}

// HD dKy_tevstr_c is 0x1C8 bytes: dKy_tevstr_init (0255FFF4) first clears 0x72 words of it.
// The original 0x2A8-byte frame (offsets from the frame base r1, entry SP - 0x2A8): PSMTXCopy matrix at
// +0x10, light state at +0x40, colours at +0x208/+0x218/+0x228, view matrix at +0x238. Same layout here, so
// recorded callee outputs land in the same objects. The struct starts at r1+8 (entry SP - 0x2A0).
/* 0254CA98 */
// HD: GX2 shader and uniform preparation replaces the GameCube material display lists.
void GrassHD::dGrass_packet_c::buildShaders() {

    gabi::Local<ShaderFrame> frame;
    struct Slot {
        u32 a;
        u32 get() const { return a; }
    };
    const u32 base = frame.a - 8; // the original r1
    Slot view{base + 0x238}, matrix{base + 0x10}, tev{base + 0x40}, color{base + 0x208},
        scaled0{base + 0x218}, scaled1{base + 0x228};
    s32 room = (s8)(u8)(*at<be<u8>>(Data::stayRoomNo));
    GrassHD::SDK::copyMatrix(view.get(), Data::viewMatrix);
    // Light template Data::lightTemplate (0x44 bytes) into the state and its two light objects (+0xC0,
    // +0x144), all fields.
    for (u32 i = 0; i <= 0x40; i += 4)
        at<LightState>(tev.a)->mWords[(i) / 4] = (u32)at<LightState>(Data::lightTemplate)->mWords[i / 4];
    for (u32 i = 0; i <= 0x40; i += 4) {
        at<LightState>(tev.a)->mWords[(0xC0 + i) / 4] = (u32)(at<LightState>(tev.a)->mWords[(i) / 4]);
        at<LightState>(tev.a)->mWords[(0x144 + i) / 4] = (u32)(at<LightState>(tev.a)->mWords[(i) / 4]);
    }
    u32 environment = GrassHD::SDK::dKy_getEnvlight();
    if ((u8)(at<Environment>(environment)->mRoomNo) != 255) {
        environment = GrassHD::SDK::dKy_getEnvlight();
        room = (s8)(u8)(at<Environment>(environment)->mRoomNo);
    }
    GrassHD::SDK::dKy_tevstr_init(tev.get(), room, 255);
    environment = GrassHD::SDK::dKy_getEnvlight();
    GrassHD::SDK::dScnKy_env_light_c__settingTevStruct(environment, 1, 0, tev.get());
    GrassHD::SDK::dKy_setLight(tev.get());
    u32 shared = gabi::ea(&mModelBinding);
    u32 model = (u32)(*at<be<u32>>(Data::renderModel));
    GrassHD::SDK::ModelBinding_build(shared, 0, view.get(), Data::renderView, model + 0x240);
    u32 renderer = (u32)(at<ModelBinding>(shared)->mpRenderer);
    GrassHD::SDK::colorToFloat(color.get(), tev.a + 0x90);
    GrassHD::SDK::ColorF_scale(scaled0.get(), color.get(), (f32)at<LightScales>(tev.a)->mDiffuseScale);
    for (u32 i = 0; i < 4; ++i)
        at<Renderer>(renderer)->mColor0[i] =
            (u32)(*reinterpret_cast<be<u32> *>(&at<ColorF>(scaled0.a)->mChannel[i]));
    renderer = (u32)(at<ModelBinding>(shared)->mpRenderer);
    GrassHD::SDK::colorToFloat(color.get(), tev.a + 0x160);
    GrassHD::SDK::ColorF_scale(scaled1.get(), color.get(), (f32)at<LightScales>(tev.a)->mColorScale);
    for (u32 i = 0; i < 4; ++i)
        at<Renderer>(renderer)->mColor1[i] =
            (u32)(*reinterpret_cast<be<u32> *>(&at<ColorF>(scaled1.a)->mChannel[i]));
    GrassHD::SDK::ModelBinding_apply(shared);
    float denominator = (f32)(*at<be<f32>>(Data::colorRange));
    u32 shaderOffset = 0, packetOffset = 0;
    for (u32 roomIndex = 0; roomIndex < 64; ++roomIndex, shaderOffset += 0x364) {
        u32 data = (u32)(mGrassRoom[roomIndex].mpData);
        if (!data)
            continue;
        u32 shader = gabi::ea(&mShaderPackets[0]) + shaderOffset;
        for (u32 j = 0; j < 4; ++j)
            at<ShaderPacket>(shader)->mInitialValues[16 + j] =
                (float)(s16)at<ColorS>(tev.a + 0x90)->mChannel[j] / denominator;
        for (u32 j = 0; j < 4; ++j)
            at<ShaderPacket>(shader)->mInitialValues[20 + j] =
                (float)(u8)at<be<u8>>(tev.a + 0x98)[j] / denominator;
        GrassHD::SDK::ColorF_scaleInPlace(shader + 0xC4, (f32)at<LightScales>(tev.a)->mAmbientScale);
        renderer = (u32)(at<ModelBinding>(shared)->mpRenderer);
        for (u32 j = 0; j < 4; ++j)
            *reinterpret_cast<be<u32> *>(&at<ShaderPacket>(shader)->mInitialValues[8 + j]) =
                (u32)at<Renderer>(renderer)->mColor0[j];
        GrassHD::SDK::ModelPacket_update(shader);
        do {
            if (!((u8)(at<dGrass_data_c>(data)->mInitFlags) & 2)) {
                for (u32 j = 0; j < 12; ++j)
                    *reinterpret_cast<be<u32> *>(&at<Matrix>(matrix.a)->m[j]) =
                        *reinterpret_cast<be<u32> *>(&at<dGrass_data_c>(data)->mModelMtx.m[j]);
                u32 packet = gabi::ea(&mModelPackets[0]) + packetOffset;
                GrassHD::SDK::PSMTXCopy(matrix.get(), packet + 0x74);
                GrassHD::SDK::ModelPacket_update(packet);
                packetOffset += 0xA8;
            }
            data = (u32)(at<dGrass_data_c>(data)->mpNextData);
        } while (data);
    }
}

/* WWHD glue (tools/verify/autoglue.py): unit-local named HD data views */
