// Flower interactions and pooled animations. HD replaces display lists with GX2 geometry.
// Register widths, guest layouts, stack objects and call order follow the verified source.
#include "d/d_flower.h"
using namespace gabi;
static FlowerRegularIndices regularIndices;
static FlowerRegularCutIndices regularCutIndices;
static FlowerPinkIndices pinkIndices;
static FlowerPinkCutIndices pinkCutIndices;
static FlowerOasisIndices oasisIndices;
static FlowerOasisCutIndices oasisCutIndices;
static flower::RoomColors flowerRoomColors;
static flower::DestructorRecords flowerDestructorRecords;
static u8 flowerFirstStaticFlag;
static u8 flowerSecondStaticFlag;
static u32 flowerTransformVtable;
static u32 flowerTextureTransformVtable;
static csXyz flowerParticleAngles;
static Mtx34 flowerMatrixStack;
static char flowerFrustum;
static flower::CosineTable flowerCosineTable;
static u32 flowerPacketVtable;
static char flowerVertexDeclaration;
static flower::MeshCounts flowerMeshCounts;
static FlowerRegularPositions regularPositions;
static FlowerRegularNormals regularNormals;
static FlowerRegularTexCoords regularTexCoords;
static FlowerRegularColors regularColors;
static FlowerRegularCutPositions regularCutPositions;
static FlowerRegularCutNormals regularCutNormals;
static FlowerRegularCutTexCoords regularCutTexCoords;
static FlowerRegularCutColors regularCutColors;
static FlowerPinkPositions pinkPositions;
static FlowerPinkNormals pinkNormals;
static FlowerPinkTexCoords pinkTexCoords;
static FlowerPinkColors pinkColors;
static FlowerPinkCutPositions pinkCutPositions;
static FlowerPinkCutNormals pinkCutNormals;
static FlowerPinkCutTexCoords pinkCutTexCoords;
static FlowerPinkCutColors pinkCutColors;
static FlowerOasisPositions oasisPositions;
static FlowerOasisNormals oasisNormals;
static FlowerOasisTexCoords oasisTexCoords;
static FlowerOasisColors oasisColors;
static FlowerOasisCutPositions oasisCutPositions;
static FlowerOasisCutNormals oasisCutNormals;
static FlowerOasisCutTexCoords oasisCutTexCoords;
static FlowerOasisCutColors oasisCutColors;
static u32 flowerWallQueryVtable;
static u32 flowerPolygonQueryVtable;
static u32 flowerGroundQueryVtable;
static u32 flowerBackgroundQueryVtable;
static u32 flowerWallBaseVtable;

static u32 flowerSafeStringVtable;
static flower::GraphicsState flowerDefaultGraphicsState;
static u8 flowerShaderContext;
static flower::LightDefaults flowerInitialLight;
static Mtx34 flowerViewMatrix;
static Mtx34 flowerProjection;
static s8 flowerCurrentRoom;
static f32 textureEpsilon;
static f32 groundRaise;
static f32 groundMinimum;
static f32 flowerZero;
static f32 collisionDistanceSquared;
static f32 cutDistance;
static f32 particleRise;
static f32 particleOne;
static f32 pinkParticleScale;
static f32 collisionRadius;
static f32 collisionHeight;
static f32 drawDepth;
static f32 swayScale;
static f32 colorDivisor;
static f32 cullRadius;
static f32 normalAnimationRange;
static f32 staticFirstValue;
static f32 staticSecondValue;
static f32 textureOne;
static f32 textureZero;
static u8 cutSoundFlag;
static u32 resourceManager;
static u32 graphicsLock;
static u32 heapManager;
static u32 swayCounter;
static f32 firstStaticValue;
static f32 secondStaticValue;
static u32 firstStaticHead;
static u32 secondStaticHead;
static u32 firstStaticTail;
static u32 secondStaticTail;
static u32 particleAnglesReady;
static u32 flowerDrawList;
static u32 flowerCamera;

// HD: renderer helpers are declared here to retain the GameCube function order.
static void flowerApplyTexture(void* emitter, Local<flower::EmitterColor>& texture, f32 one);
static void flowerHeapReleaseField(be<u32>* field);
static void flowerCleanupBuffers(flower::MaterialBuffers* materials);
static void flowerCleanupShaderArray(flower::ShapeGroups* descriptor);
static flower::ShapeGroup* flowerShaderRow(flower::ShapeGroups* descriptor, u32 index);
static void flowerIndexedDraw(flower::IndexBuffer* geometry);
static void flowerDrawUniforms(flower::ShaderMaterial* shader, flower::UniformData* object);
static void* flowerHeapAllocate(u32 size, u32 align);
static void flowerShaderLoad(u32 descriptor, void* name);
static void flowerShaderBuffers(u32 descriptorAddress, u32 buffers);
static u32 flowerInlineClear(u32 group, u32 side, u32 bytes);
static void flowerVertices(flower::Vertex* output, be<u32>* count, cXyz* position, cXyz* normal,
                           flower::Vector2* texcoord, flower::Vector4* color, bool setters);

/* 02544ABC */
f32 checkGroundY(void* position) {
    // HD: the wall query adds a trailing word to the GameCube ground-check record.
    Local<flower::GroundCheck> object;
    flowerGroundConstruct(object.get());
    auto* pos = static_cast<cXyz*>(position);
    f32 originalY = pos->y, x = pos->x;
    f32 raise = groundRaise;
    for (u32 k = 0; k < 7; ++k)
        object->mFlags[k] = 0;
    object->mWallVtable = ea(&flowerWallQueryVtable);
    object->mPolygonVtable = ea(&flowerPolygonQueryVtable);
    object->mWallEnabled = 1;
    f32 raised = fadds_ppc(originalY, raise);
    object->mPosition.y = raised;
    pos->y = raised;
    object->mCheckVtable = ea(&flowerGroundQueryVtable);
    object->mPosition.x = x;
    object->mWallInterface = ea(&object->mWallVtable);
    object->mBaseVtable = ea(&flowerBackgroundQueryVtable);
    object->mBackgroundInterface = ea(&object->mBaseVtable);
    object->mPosition.z = pos->z;
    auto* background = dComIfG_Bgsp();
    f32 y = flowerCBgS_GroundCross(background, object.get());
    f32 restored = fsubs_ppc(pos->y, raise);
    object->mBaseVtable = ea(&flowerBackgroundQueryVtable);
    object->mPolygonVtable = ea(&flowerPolygonQueryVtable);
    f32 threshold = groundMinimum;
    object->mWallVtable = ea(&flowerWallBaseVtable);
    pos->y = restored;
    flowerGroundDestruct(object.get(), u32(0));
    return y > threshold ? y : restored;
}

/* 02544D04 */
void dFlower_data_c::WorkCo(fopAc_ac_c* actor, u32 mask, s32 room) {
    f32 x = fsubs_ppc((f32)mPosition.x, f32(static_cast<fopAc_ac_c*>(actor)->current.pos.x));
    f32 z = fsubs_ppc((f32)mPosition.z, f32(static_cast<fopAc_ac_c*>(actor)->current.pos.z));
    Local<cXyz> delta;
    delta->y = flowerZero;
    delta->x = x;
    delta->z = z;
    f32 squared = flowerVectorSquareMagnitude(delta.get());
    if (squared > collisionDistanceSquared)
        return;
    s32 angle = flowerAtan2(x, z);
    f32 distance = flowerSquareRoot(squared);
    if ((s8)mAnmIdx < 8) {
        if (!(u8)mParticleTimer) {
            u8 flags = (u8)mFlags;
            if (!(flags & dFlower_data_c::FLAG_CUT) &&
                f32(static_cast<fopAc_ac_c*>(actor)->speedF) > cutDistance) {
                Local<cXyz> position;
                f32 y = (f32)mPosition.y, pz = (f32)mPosition.z, rise = particleRise,
                    px = (f32)mPosition.x;
                position->z = pz;
                position->x = px;
                position->y = fadds_ppc(y, rise);
                u32 particle = (flags & dFlower_data_c::FLAG_PINK)
                                   ? ((flags & dFlower_data_c::FLAG_OASIS) ? flower::PARTICLE_OASIS
                                                                           : flower::PARTICLE_PINK)
                                   : flower::PARTICLE_REGULAR;
                dComIfGp_get();
                u32 color = ea(at<flower::RoomColor>(ea(&flowerRoomColors) +
                                                     u32(room) * sizeof(flower::RoomColor))
                                   ->mColor);
                u32 play = ea(dComIfGp_get()),
                    control = u32(at<dComIfG_inf_c>(play)->play.mParticle.v);
                void* emitter = flowerDPa_control_c_set(
                    at<void>(control), u32(0), particle, position.get(), u32(0), u32(0), u32(0xFF),
                    u32(0), s32(s8(room)), at<void>(color), at<void>(color), u32(0));
                if (emitter) {
                    f32 one = particleOne;
                    static_cast<flower::Emitter*>(emitter)->mScaleX =
                        particle == flower::PARTICLE_OASIS ? pinkParticleScale : one;
                    Local<flower::EmitterColor> texture;
                    texture->mValues[1] = one;
                    texture->mValues[3] = one;
                    texture->mEnabled = 1;
                    texture->mValues[0] = one;
                    texture->mPadding[1] = 0;
                    texture->mPadding[2] = 0;
                    texture->mVtable = ea(&flowerSafeStringVtable);
                    texture->mPadding[0] = 0;
                    texture->mValues[2] = one;
                    texture->mpName.v = guest_str("");
                    play = ea(dComIfGp_get());
                    control = u32(at<dComIfG_inf_c>(play)->play.mParticle.v);
                    for (u32 k = 0; k < 4; ++k)
                        texture->mWords[k] = u32(at<flower::ParticleControl>(control)->mColor[k]);
                    flowerEmitterSetColor(emitter, texture.get());
                    mParticleTimer = 0x10;
                }
            }
        }
        u32 play = ea(dComIfGp_get());
        s32 index =
            at<dFlower_packet_c>(u32(at<dComIfG_inf_c>(play)->play.mpFlowerPacket))->newAnm();
        if (index < 0)
            return;
        mAnmIdx = u8(index);
    }
    u32 play = ea(dComIfGp_get());
    s32 index = (s8)mAnmIdx;
    u32 packet = u32(at<dComIfG_inf_c>(play)->play.mpFlowerPacket);
    f32 bend = fsubs_ppc(collisionRadius, distance), height = collisionHeight;
    u32 animation = packet + u32(index) * 56 + 0x35BC;
    at<dFlower_anm_c>(animation)->mAngleY = u16(angle);
    s32 tilt = flowerAtan2(bend, height);
    at<dFlower_anm_c>(animation)->mBend = u16(tilt);
}

/* 02545040 */
void dFlower_data_c::WorkAt_NoCutAnim(fopAc_ac_c* actor, u32 mask, s32 room, flower::HitInf* hit,
                                      void* collision) {
    u32 object = ea(flowerDCcD_GetGObjInf(collision));
    f32 x = f32(at<dCcD_GObjInf>(object)->mGObjAt.mVec.x),
        z = f32(at<dCcD_GObjInf>(object)->mGObjAt.mVec.z), zero = flowerZero;
    Local<cXyz> first;
    Local<cXyz> fallback;
    Local<cXyz> vector;
    first->z = z;
    first->x = x;
    first->y = zero;
    f32 squared = flowerVectorSquareMagnitude(first.get());
    if (__builtin_fabsf(squared) < textureEpsilon && actor &&
        s16(static_cast<fopAc_ac_c*>(actor)->mProcName) == flower::fpcNm_PLAYER_e) {
        f32 ax = f32(static_cast<fopAc_ac_c*>(actor)->current.pos.x), sz = (f32)mPosition.z,
            sx = (f32)mPosition.x, az = f32(static_cast<fopAc_ac_c*>(actor)->current.pos.z);
        x = fsubs_ppc(sx, ax);
        z = fsubs_ppc(sz, az);
        fallback->y = zero;
        fallback->x = x;
        fallback->z = z;
        flowerVectorSquareMagnitude(fallback.get());
    }
    vector->z = z;
    vector->x = x;
    vector->y = zero;
    squared = flowerVectorSquareMagnitude(vector.get());
    s32 angle = flowerAtan2(x, z);
    f32 distance = flowerSquareRoot(squared);
    if ((s8)mAnmIdx < 8) {
        if (!(u8)mParticleTimer) {
            u8 flags = (u8)mFlags;
            if (!(flags & dFlower_data_c::FLAG_CUT) && distance > cutDistance) {
                Local<cXyz> position;
                f32 y = (f32)mPosition.y, pz = (f32)mPosition.z, rise = particleRise,
                    px = (f32)mPosition.x;
                position->z = pz;
                position->x = px;
                position->y = fadds_ppc(y, rise);
                u32 particle = (flags & dFlower_data_c::FLAG_PINK)
                                   ? ((flags & dFlower_data_c::FLAG_OASIS) ? flower::PARTICLE_OASIS
                                                                           : flower::PARTICLE_PINK)
                                   : flower::PARTICLE_REGULAR;
                dComIfGp_get();
                u32 color = ea(at<flower::RoomColor>(ea(&flowerRoomColors) +
                                                     u32(room) * sizeof(flower::RoomColor))
                                   ->mColor);
                u32 play = ea(dComIfGp_get()),
                    control = u32(at<dComIfG_inf_c>(play)->play.mParticle.v);
                void* emitter = flowerDPa_control_c_set(
                    at<void>(control), u32(0), particle, position.get(), u32(0), u32(0), u32(0xFF),
                    u32(0), s32(s8(room)), at<void>(color), at<void>(color), u32(0));
                if (emitter) {
                    f32 one = particleOne;
                    static_cast<flower::Emitter*>(emitter)->mScaleX =
                        particle == flower::PARTICLE_OASIS ? pinkParticleScale : one;
                    Local<flower::EmitterColor> texture;
                    texture->mValues[1] = one;
                    texture->mValues[3] = one;
                    texture->mEnabled = 1;
                    texture->mValues[0] = one;
                    texture->mPadding[1] = 0;
                    texture->mPadding[2] = 0;
                    texture->mVtable = ea(&flowerSafeStringVtable);
                    texture->mPadding[0] = 0;
                    texture->mValues[2] = one;
                    texture->mpName.v = guest_str("");
                    play = ea(dComIfGp_get());
                    control = u32(at<dComIfG_inf_c>(play)->play.mParticle.v);
                    for (u32 k = 0; k < 4; ++k)
                        texture->mWords[k] = u32(at<flower::ParticleControl>(control)->mColor[k]);
                    flowerEmitterSetColor(emitter, texture.get());
                    mParticleTimer = 0x10;
                }
            }
        }
        u32 play = ea(dComIfGp_get());
        s32 index =
            at<dFlower_packet_c>(u32(at<dComIfG_inf_c>(play)->play.mpFlowerPacket))->newAnm();
        if (index < 0)
            return;
        mAnmIdx = u8(index);
    }
    u32 play = ea(dComIfGp_get());
    s32 index = (s8)mAnmIdx;
    u32 packet = u32(at<dComIfG_inf_c>(play)->play.mpFlowerPacket);
    f32 bend = distance, height = collisionHeight;
    u32 animation = packet + u32(index) * 56 + 0x35BC;
    at<dFlower_anm_c>(animation)->mAngleY = u16(angle);
    s32 tilt = flowerAtan2(bend, height);
    at<dFlower_anm_c>(animation)->mBend = u16(tilt);
}

/* 025453B8 */
void dFlower_data_c::WorkAt(fopAc_ac_c* actor, u32 mask, s32 room, flower::HitInf* hit) {
    u32 collision = static_cast<flower::HitInf*>(hit)->mpAtObject.v;
    if (collision &&
        (at<dCcD_GObjInf>(collision)->mAtType &
         (flower::AT_TYPE_WIND | flower::AT_TYPE_NORMAL_ARROW | flower::AT_TYPE_FIRE_ARROW |
          flower::AT_TYPE_ICE_ARROW | flower::AT_TYPE_LIGHT_ARROW | flower::AT_TYPE_HOOKSHOT))) {
        WorkAt_NoCutAnim(actor, mask, room, hit, at<void>(collision));
        return;
    }
    if ((s8)mAnmIdx >= 8) {
        u32 play = ea(dComIfGp_get());
        s32 index = (s8)mAnmIdx;
        u32 packet = u32(at<dComIfG_inf_c>(play)->play.mpFlowerPacket);
        at<dFlower_anm_c>(packet + offsetof(dFlower_packet_c, mAnm) +
                          u32(index) * sizeof(dFlower_anm_c))
            ->mActive = 0;
    }
    mFlags = (u8)mFlags | 8;
    if (!particleAnglesReady) {
        particleAnglesReady = 1;
        flowerCsXyz_csXyz(&flowerParticleAngles, u32(0), u32(0), u32(0));
    }
    dComIfGp_get();
    Local<cXyz> position;
    f32 z = (f32)mPosition.z, y = (f32)mPosition.y;
    u8 flags = (u8)mFlags;
    f32 x = (f32)mPosition.x;
    u32 color =
        ea(at<flower::RoomColor>(ea(&flowerRoomColors) + u32(room) * sizeof(flower::RoomColor))
               ->mColor);
    position->y = y;
    position->x = x;
    position->z = z;
    f32 one = particleOne;
    Local<flower::EmitterColor> texture1;
    Local<flower::EmitterColor> texture2;
    void* emitter = nullptr;
    u32 particle = (flags & dFlower_data_c::FLAG_PINK)
                       ? ((flags & dFlower_data_c::FLAG_OASIS) ? flower::PARTICLE_OASIS
                                                               : flower::PARTICLE_PINK)
                       : flower::PARTICLE_REGULAR;
    if (particle == flower::PARTICLE_OASIS) {
        u32 play = ea(dComIfGp_get());
        u32 control = u32(at<dComIfG_inf_c>(play)->play.mParticle.v);
        void* extra = flowerDPa_control_c_set(
            at<void>(control), u32(0), u32(flower::PARTICLE_OASIS_EXTRA), position.get(),
            &flowerParticleAngles, u32(0), u32(0xFF), u32(0), s32(s8(room)), at<void>(color),
            at<void>(color), u32(0));
        if (extra)
            flowerApplyTexture(extra, texture1, one);
    }
    u32 play = ea(dComIfGp_get());
    u32 control = u32(at<dComIfG_inf_c>(play)->play.mParticle.v);
    emitter = flowerDPa_control_c_set(at<void>(control), u32(0), particle, position.get(),
                                      &flowerParticleAngles, u32(0), u32(0xFF), u32(0),
                                      s32(s8(room)), at<void>(color), at<void>(color), u32(0));
    if (emitter)
        flowerApplyTexture(emitter, texture2, one);
    s32 item = (s8)mItemTableIdx;
    if (item >= 0)
        flowerFopAcM_createItemFromTable(&mPosition, item, s32(-1), room, u32(0), u32(0), u32(1),
                                         u32(0));
    if (!cutSoundFlag) {
        cutSoundFlag = 1;
        s32 reverb = flowerDComIfGp_getReverb(room);
        flowerStartSound(u32(0x282C), &mPosition, u32(0), reverb);
    }
}

/* 025457C4 */
void dFlower_data_c::hitCheck(fopAc_ac_c* player, s32 room) {
    Local<flower::HitInf> hit;
    Local<be<u32>> actor;
    flowerHitConstruct(hit.get());
    u32 play = ea(dComIfGp_get());
    u32 mask =
        flowerCheckMass(&at<flower::MassGameView>(play)->mMass, &mPosition, actor.get(), hit.get());
    bool attack = false;
    if (mask & 1) {
        u32 a = (*actor);
        if (a && at<fopAc_ac_c>(a)->mProcName != fpcNm_TSUBO_e &&
            at<fopAc_ac_c>(a)->mProcName != flower::fpcNm_STONE_e)
            attack = true;
    }
    if (!(mask & 2) && !attack) {
        s32 index = (s8)mAnmIdx;
        if (index < 8)
            return;
        play = ea(dComIfGp_get());
        index = (s8)mAnmIdx;
        u32 animation =
            u32(at<dComIfG_inf_c>(play)->play.mpFlowerPacket) + u32(index) * 56 + 0x35BC;
        u32 angle = u16(at<dFlower_anm_c>(animation)->mAngleY);
        s32 target = s16(angle & 0xE000);
        u32 windIndex = angle >> 13;
        play = ea(dComIfGp_get());
        u32 packet = u32(at<dComIfG_inf_c>(play)->play.mpFlowerPacket);
        s32 bend = s16(at<dFlower_anm_c>(packet + offsetof(dFlower_packet_c, mAnm) +
                                         windIndex * sizeof(dFlower_anm_c))
                           ->mBend);
        if (flowerCLib_addCalcAngleS(&at<dFlower_anm_c>(animation)->mBend, bend, s32(0x10),
                                     s32(0xFA0), s32(0x64)))
            return;
        if (!flowerCLib_chaseAngleS(&at<dFlower_anm_c>(animation)->mAngleY, target, s32(0x320)))
            return;
        play = ea(dComIfGp_get());
        index = (s8)mAnmIdx;
        packet = u32(at<dComIfG_inf_c>(play)->play.mpFlowerPacket);
        at<dFlower_anm_c>(packet + offsetof(dFlower_packet_c, mAnm) +
                          u32(index) * sizeof(dFlower_anm_c))
            ->mActive = 0;
        mAnmIdx = u16(at<dFlower_anm_c>(animation)->mAngleY) >> 13;
        return;
    }
    if (!(*actor))
        flower_assert(at<void>(guest_str("d_flower.cpp")), u32(0x1BA0),
                      at<void>(guest_str("hit_actor != (0)")));
    if (mask & 2)
        WorkCo(at<fopAc_ac_c>((*actor)), mask, room);
    if (!((u8)mFlags & dFlower_data_c::FLAG_CUT) && attack)
        WorkAt(at<fopAc_ac_c>((*actor)), mask, room, hit.get());
}

/* 02545998 */
void dFlower_room_c::newData(dFlower_data_c* data) {
    auto* flower = static_cast<dFlower_data_c*>(data);
    flower->mpNext = mpHead;
    mpHead = flower;
}

/* 025459A8 */
void dFlower_room_c::deleteData() {
    auto* head = mpHead.get();
    while (head) {
        head->mFlags = 0;
        auto* current = mpHead.get();
        flowerJAIZelBasic_seDeleteObject(&current->mPosition);
        current = mpHead.get();
        head = current->mpNext.get();
        mpHead = head;
    }
}

/* 02545A2C */
void* dFlower_packet_c__ct(void* self) {
    if (!self)
        self = flowerAllocate(u32(0x1C654));
    if (!self)
        return nullptr;
    auto* packet = static_cast<dFlower_packet_c*>(self);
    u32 root = ea(packet->mGeometry);
    flowerPacketBaseConstruct(self);
    packet->mPacketBase.mVtable = ea(&flowerPacketVtable);
    packet->mNextData = 0;
    flowerArrayConstruct(&packet->mData, u32(200), u32(0x44),
                         guest_fn(reinterpret_cast<const void*>(&dFlower_data_c__ct)));
    flowerArrayConstruct(&packet->mAnm, u32(72), u32(0x38),
                         guest_fn(reinterpret_cast<const void*>(&dFlower_anm_c__ct)));
    flowerArrayConstruct(&packet->mRoom, u32(64), u32(4),
                         guest_fn(reinterpret_cast<const void*>(&dFlower_room_c__ct)));
    packet->mPlayerSwordAngle = 0;
    packet->mPlayerSwordMoveAngle = 0;
    packet->mPlayerCutFlag = 0;
    flowerTransformSetConstruct(&packet->mTransformSet);
    flowerArrayConstruct(&packet->mTransforms, u32(200), u32(0xA8),
                         guest_fn(reinterpret_cast<const void*>(&flowerDrawNodeConstruct)));
    flowerArrayConstruct(&packet->mTextures, u32(64), u32(0x364),
                         guest_fn(reinterpret_cast<const void*>(&flowerRoomNodeConstruct)));
    u32 groups[3];
    for (u32 i = 0; i < 3; ++i) {
        auto* geometry = &packet->mGeometry[i];
        geometry->mShapes.mCount = 0;
        u32 pair = ea(&geometry->mShapes.mCapacity);
        if (!pair)
            pair = ea(flowerAllocate(u32(8)));
        if (pair) {
            auto* fields = at<be<u32>>(pair);
            fields[1] = 0;
            fields[0] = 0;
        }
        u32 group = ea(&geometry->mMaterials);
        groups[i] = group;
        if (!group)
            group = ea(flowerAllocate(u32(sizeof(flower::MaterialBuffers))));
        if (group) {
            auto* materials = at<flower::MaterialBuffers>(group);
            flowerArrayConstruct(
                materials, u32(4), u32(sizeof(flower::MaterialPart)),
                u32(i ? guest_fn(reinterpret_cast<const void*>(&flowerBuffer2Construct))
                      : guest_fn(reinterpret_cast<const void*>(&flowerBuffer1Construct))));
            materials->mFrameIndex = 0;
            materials->mState = 0;
            materials->mVertexSize = sizeof(flower::Vertex);
            materials->mReady = 0;
            materials->mVertexFormat = 0;
            for (u32 n = 0; n < 4; ++n)
                materials->mPart[n / 2][n % 2].mpVertices = nullptr;
        }
        flowerIndexBufferConstruct(&geometry->mUncut);
        flowerIndexBufferConstruct(&geometry->mCut);
        flowerSamplerConstruct(&geometry->mSampler);
        flowerSamplerResourceConstruct(geometry->mSamplerResource);
    }

    packet->mpShapeGroups = nullptr;
    packet->mpMaterialBuffers = nullptr;
    packet->mpUncutIndices = nullptr;
    packet->mpCutIndices = nullptr;
    packet->mpSampler = nullptr;
    for (u32 n = 0; n < 200; ++n)
        packet->mData[n].mFlags = 0;
    for (u32 n = 0; n < 72; ++n)
        packet->mAnm[n].mActive = 0;
    for (u32 n = 0; n < 8; ++n)
        static_cast<dFlower_packet_c*>(self)->setAnm(n, s32(s16(n * 0x2000)));
    Local<flower::String> name;
    for (u32 i = 0; i < 3; ++i) {
        name->mText = guest_str("flower_draw");
        name->mpVtable.v = ea(&flowerSafeStringVtable);
        flowerShaderLoad(root + i * 0xBCC, name.get());
    }
    for (u32 i = 0; i < 3; ++i) {
        auto* materials = at<flower::MaterialBuffers>(groups[i]);
        materials->mVertexFormat = 0x1013;
        materials->mpVertexDeclaration = &flowerVertexDeclaration;
    }
    for (u32 i = 0; i < 3; ++i) {
        u32 group = groups[i], count = i ? 605 : 90;
        for (u32 side = 0; side < 2; ++side)
            for (u32 n = 0; n < 2; ++n) {
                u32 buffer = group + side * sizeof(flower::MaterialPart) +
                             n * 2 * sizeof(flower::MaterialPart);
                auto* part = at<flower::MaterialPart>(buffer);
                u32 pointer = part->mpVertices.v;
                if (!pointer) {
                    u32 allocated = ea(flowerHeapAllocate(i ? 0x7170 : 0x10E0, 0x40));
                    if (allocated) {
                        part->mpVertexStorage.v = allocated;
                        part->mVertexCapacity = count;
                    }
                    pointer = part->mpVertexStorage.v;
                    part->mpVertices.v = pointer;
                }
                flowerConfigureVertexBuffer(&part->mVertexBuffer, at<void>(pointer), count,
                                            &at<flower::MaterialBuffers>(group)->mVertexFormat);
            }
        at<flower::MaterialBuffers>(group)->mState = 0;
        at<flower::MaterialBuffers>(group)->mReady = 1;
    }
    for (u32 i = 0; i < 3; ++i)
        flowerShaderBuffers(root + i * 0xBCC, groups[i]);
    flowerTransformSetConfigure(&packet->mTransformSet, u32(1), u32(0));
    for (u32 i = 0; i < 64; ++i)
        flowerUniformSetState(&packet->mTextures[i], u32(0));
    for (u32 i = 0; i < 200; ++i)
        flowerUniformSetState(&packet->mTransforms[i], u32(0));
    u32 geometry[6] = {root + 0x974,  root + 0x98C,  root + 0x1540,
                       root + 0x1558, root + 0x210C, root + 0x2124};
    void* const indexData[6] = {regularIndices.mValues, regularCutIndices.mValues,
                                pinkIndices.mValues,    pinkCutIndices.mValues,
                                oasisIndices.mValues,   oasisCutIndices.mValues};
    for (u32 i = 0; i < 6; ++i) {
        flowerIndexBufferSet(at<void>(geometry[i]), indexData[i], u32(4),
                             u32(flowerMeshCounts.mMeshes[i].mIndices));
        at<flower::IndexBuffer>(geometry[i])->mIndexFormat = 4;
    }
    u32 buffer = flowerInlineClear(groups[0], 0, 0x10E0);
    flowerVertices(at<flower::MaterialPart>(buffer)->mpVertices.get(),
                   &flowerMeshCounts.mMeshes[0].mVertices, regularPositions.mValues,
                   regularNormals.mValues, regularTexCoords.mValues, regularColors.mValues, false);
    buffer = flowerInlineClear(groups[0], 1, 0x10E0);
    flowerVertices(at<flower::MaterialPart>(buffer)->mpVertices.get(),
                   &flowerMeshCounts.mMeshes[1].mVertices, regularCutPositions.mValues,
                   regularCutNormals.mValues, regularCutTexCoords.mValues, regularCutColors.mValues,
                   false);
    buffer = groups[0] + u32(at<flower::MaterialBuffers>(groups[0])->mFrameIndex) * 0x254 + 4;
    for (u32 i = 0; i < 2; ++i, buffer += 0x4A8)
        flowerVertexBufferFlush(at<void>(buffer), u32(0),
                                u32(at<flower::VertexBuffer>(buffer)->mFlushSize));
    at<flower::MaterialBuffers>(groups[0])->mFrameIndex =
        at<flower::MaterialBuffers>(groups[0])->mFrameIndex == 0;
    buffer = flowerInlineClear(groups[1], 0, 0x7160);
    flowerVertices(at<flower::MaterialPart>(buffer)->mpVertices.get(),
                   &flowerMeshCounts.mMeshes[2].mVertices, pinkPositions.mValues,
                   pinkNormals.mValues, pinkTexCoords.mValues, pinkColors.mValues, false);
    buffer = ea(flowerBufferSelect(at<void>(groups[1]), u32(1), u32(1)));
    flowerVertices(at<flower::MaterialPart>(buffer)->mpVertices.get(),
                   &flowerMeshCounts.mMeshes[3].mVertices, pinkCutPositions.mValues,
                   pinkCutNormals.mValues, pinkCutTexCoords.mValues, pinkCutColors.mValues, true);
    flowerBufferRotate(at<void>(groups[1]), u32(0), u32(2));
    buffer = ea(flowerBufferSelect(at<void>(groups[2]), u32(0), u32(1)));
    flowerVertices(at<flower::MaterialPart>(buffer)->mpVertices.get(),
                   &flowerMeshCounts.mMeshes[4].mVertices, oasisPositions.mValues,
                   oasisNormals.mValues, oasisTexCoords.mValues, oasisColors.mValues, true);
    buffer = ea(flowerBufferSelect(at<void>(groups[2]), u32(1), u32(1)));
    flowerVertices(at<flower::MaterialPart>(buffer)->mpVertices.get(),
                   &flowerMeshCounts.mMeshes[5].mVertices, oasisCutPositions.mValues,
                   oasisCutNormals.mValues, oasisCutTexCoords.mValues, oasisCutColors.mValues,
                   true);
    flowerBufferRotate(at<void>(groups[2]), u32(0), u32(2));
    flowerLockGraphics(at<void>(graphicsLock));
    Local<flower::String> folder;
    Local<flower::String> archive;
    void* folderName = flowerStringConstruct(folder.get(), at<void>(guest_str("ProgramTexture")));
    void* archiveName =
        flowerStringConstruct(archive.get(), at<void>(guest_str("ProgramTexture.bfres")));
    void* resource = flowerGetResource(at<void>(resourceManager), folderName, archiveName, u32(0));
    flowerAnimationDestroy(archive.get(), u32(2));
    flowerAnimationDestroy(folder.get(), u32(2));
    void* const textures[3] = {at<void>(guest_str("Txo_ob_flower_white_64x64")),
                               at<void>(guest_str("Txo_ob_flower_pink_64x64")),
                               at<void>(guest_str("Txq_bessou_hana"))};
    for (u32 i = 0; i < 3; ++i) {
        u32 texture = root + i * 0xBCC + 0x9A4, sampler = root + i * 0xBCC + 0xB3C;
        flowerLoadSamplerResource(at<void>(sampler), resource, textures[i]);
        flowerSurfaceAssign(at<void>(texture), at<void>(sampler));
        auto* samplerState = at<flower::SamplerRecord>(texture);
        samplerState->mWords[0x160 / 4] = 1;
        samplerState->mWords[0x15C / 4] = 1;
        samplerState->mWords[0x164 / 4] = 1;
        flowerFlagsSet(&at<flower::SamplerRecord>(texture)->mFlags, u32(2));
    }
    flowerUnlockGraphics(at<void>(graphicsLock));
    packet->mpSampler = &packet->mGeometry[1].mSampler;
    packet->mpMaterialBuffers.v = groups[1];
    packet->mpUncutIndices = &packet->mGeometry[1].mUncut;
    packet->mpCutIndices = &packet->mGeometry[1].mCut;
    packet->mpShapeGroups = &packet->mGeometry[1].mShapes;
    return self;
}

/* 025491FC */
void* dFlower_room_c__ct(void* self) {
    if (!self)
        self = flowerAllocate(u32(0x4));
    if (!self)
        return nullptr;
    static_cast<dFlower_room_c*>(self)->mpHead = nullptr;
    return self;
}

/* 025491C0 */
void* dFlower_anm_c__ct(void* self) {
    if (!self)
        self = flowerAllocate(u32(0x38));
    if (!self)
        return nullptr;
    static_cast<dFlower_anm_c*>(self)->mActive = 0;
    return self;
}

/* 02544C4C */
void* dFlower_data_c__ct(void* self) {
    if (!self)
        self = flowerAllocate(u32(0x44));
    if (!self)
        return nullptr;
    auto* data = static_cast<dFlower_data_c*>(self);
    data->mParticleTimer = 0;
    data->mItemTableIdx = 0;
    data->mFlags = 0;
    data->mAnmIdx = 0;
    flowerClearUniformData(&data->mMtx, u32(sizeof(Mtx34)));
    data->mpNext = nullptr;
    data->mFlags = 0;
    return self;
}

/* 02544AB8 */
void dFlower_packet_c::draw() {}

/* 02548370 */
void dFlower_packet_c::calc() {
    f32 scale = swayScale;
    for (u32 i = 0; i < 8; ++i) {
        u32 timer = swayCounter + i * 0xFA;
        u16 angle = u16(ftoi(fmuls_ppc(f32(timer), scale)));
        f32 cosine = f32(flowerCosineTable.mValues[u32(angle) >> 3][0]);
        mAnm[i].mBend = u16(ftoi(fmadds(scale, cosine, scale)));
    }
    s32 room = flowerCurrentRoom;
    auto* head = at<dFlower_room_c>(ea(mRoom) + u32(room) * sizeof(dFlower_room_c))->mpHead.get();
    if (!head)
        return;
    u32 play = ea(dComIfGp_get());
    u32 player = u32(at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    mPlayerCutFlag = (u32(at<flower::SwordView>(player)->mCutFlags) & 0x40) != 0;
    Local<cXyz> delta;
    Local<cXyz> sword;
    f32 x = f32(at<flower::SwordView>(player)->mSwordTop.x),
        y = f32(at<flower::SwordView>(player)->mSwordTop.y),
        z = f32(at<flower::SwordView>(player)->mSwordTop.z);
    sword->x = x;
    sword->z = z;
    sword->y = y;
    delta->x = x;
    delta->y = y;
    delta->z = z;
    flowerCXyzSubtract(sword.get(), delta.get(), &at<fopAc_ac_c>(player)->current.pos);
    f32 dz = (f32)delta->z, dx = (f32)delta->x;
    s32 angle = flowerAtan2(dx, dz);
    mPlayerSwordAngle = u16(angle);
    flowerCXyzSubtract(&mPlayerSwordTop, delta.get(), sword.get());
    dx = (f32)delta->x;
    dz = (f32)delta->z;
    angle = flowerAtan2(dx, dz);
    u32 sx = (u32) reinterpret_cast<be<u32>*>(sword.get())[0],
        sy = (u32) reinterpret_cast<be<u32>*>(sword.get())[1],
        sz = (u32) reinterpret_cast<be<u32>*>(sword.get())[2];
    auto* swordTop = reinterpret_cast<be<u32>*>(&mPlayerSwordTop);
    swordTop[0] = sx;
    mPlayerSwordMoveAngle = u16(angle);
    swordTop[1] = sy;
    swordTop[2] = sz;
    cutSoundFlag = 0;
    play = ea(dComIfGp_get());
    flowerMassSetRadius(at<flower::MassGameView>(play)->mMass.mShape, collisionRadius);
    flowerMassSetHeight(at<flower::MassGameView>(play)->mMass.mShape, groundRaise);
    at<flower::MassGameView>(play)->mMass.mAttribute = 0xB;
    at<flower::MassGameView>(play)->mMass.mEnabled = 2;
    do {
        if (!(head->mFlags & dFlower_data_c::FLAG_CULLED))
            head->hitCheck(at<fopAc_ac_c>(player), room);
        head = head->mpNext.get();
    } while (head);
}

/* 02548B6C */
void dFlower_packet_c::update() {
    for (u32 i = 0; i < 72; ++i) {
        auto* animation = &mAnm[i];
        // HD: these matrix helpers receive full-width sign-extended angles.
        flowerCMtx_YrotS(&flowerMatrixStack, s32(animation->mAngleY));
        flowerCMtx_XrotM(&flowerMatrixStack, s32(animation->mBend));
        flowerCMtx_YrotM(&flowerMatrixStack, s32(s16(-s32(animation->mAngleY))));
        flowerMatrixCopy(&flowerMatrixStack, &animation->mMtx);
    }
    f32 radius = cullRadius;
    int groundUpdates = 0;
    Local<cXyz> clipPosition;
    for (u32 i = 0; i < 200; ++i) {
        auto* data = &mData[i];
        if (!(data->mFlags & dFlower_data_c::FLAG_ACTIVE))
            continue;
        flowerCLib_calcTimer(&data->mParticleTimer);
        if ((data->mFlags & dFlower_data_c::FLAG_GROUND_PENDING) && groundUpdates < 8) {
            f32 y = checkGroundY(&data->mPosition);
            u8 flags = data->mFlags;
            data->mPosition.y = y;
            data->mFlags = flags & ~dFlower_data_c::FLAG_GROUND_PENDING;
            ++groundUpdates;
        }
        // HD: culling uses the flower position without GameCube's vertical offset.
        f32 y = data->mPosition.y, x = data->mPosition.x, z = data->mPosition.z;
        clipPosition->x = x;
        clipPosition->y = y;
        clipPosition->z = z;
        s32 clipped =
            flowerFrustumCheck(&flowerFrustum, &flowerViewMatrix, clipPosition.get(), radius);
        u8 flags = data->mFlags;
        if (clipped) {
            data->mFlags = flags | dFlower_data_c::FLAG_CULLED;
            continue;
        }
        s8 index = data->mAnmIdx;
        data->mFlags = flags & ~dFlower_data_c::FLAG_CULLED;
        auto* animation = at<dFlower_anm_c>(ea(mAnm) + u32(s32(index)) * sizeof(dFlower_anm_c));
        // GHS copies translation words exactly, preserving float payloads.
        auto* position = reinterpret_cast<be<u32>*>(&data->mPosition);
        *reinterpret_cast<be<u32>*>(&animation->mMtx.m[0][3]) = position[0];
        *reinterpret_cast<be<u32>*>(&animation->mMtx.m[1][3]) = position[1];
        *reinterpret_cast<be<u32>*>(&animation->mMtx.m[2][3]) = position[2];
        flowerMatrixCopy(&animation->mMtx, &data->mMtx);
    }
    flowerDrawListEntry(at<void>(flowerDrawList), this, u32(0));
    prepareGraphics();
}

/* 02548D28 */
void dFlower_packet_c::setData(dFlower_data_c* data, s32 index, s32 type, cXyz* position, s32 room,
                               s32 item) {
    auto* flower = static_cast<dFlower_data_c*>(data);
    auto* pos = static_cast<cXyz*>(position);
    f32 ground;
    u8 flags;
    if (flowerFopOvlpM_IsPeek()) {
        ground = checkGroundY(position);
        flags = dFlower_data_c::FLAG_ACTIVE | dFlower_data_c::FLAG_CULLED;
    } else {
        flags = dFlower_data_c::FLAG_ACTIVE | dFlower_data_c::FLAG_CULLED |
                dFlower_data_c::FLAG_GROUND_PENDING;
        ground = pos->y;
    }
    if (type == 2)
        flags |= 0x20;
    f32 range = normalAnimationRange;
    flower->mFlags = flags;
    f32 random = flowerCM_rndF(range);
    flower->mAnmIdx = u8(ftoi(random));
    f32 x = pos->x, z = pos->z;
    flower->mPosition.x = x;
    flower->mPosition.y = ground;
    flower->mItemTableIdx = u8(item);
    flower->mPosition.z = z;
    flower->mParticleTimer = 0;
    auto* roomData = at<dFlower_room_c>(ea(mRoom) + u32(room) * sizeof(dFlower_room_c));
    void* selected;
    if (roomData->mpHead.get())
        selected = mpSampler.get();
    else {
        Local<flower::String> literal;
        Local<flower::String> stage;
        literal->mText = guest_str("sea");
        literal->mpVtable.v = ea(&flowerSafeStringVtable);
        auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        stage->mpVtable.v = ea(&flowerSafeStringVtable);
        stage->mText = ea(play->play.mCurStage.mName);
        call_ptr<void>(literal->mpVtable->mEnsureText.get(), literal.get());
        call_ptr<void>(literal->mpVtable->mEnsureText.get(), literal.get());
        u32 cached = literal->mText;
        call_ptr<void>(stage->mpVtable->mEnsureText.get(), stage.get());
        bool equal = cached == stage->mText;
        if (!equal) {
            u32 a = literal->mText, b = stage->mText;
            for (u32 i = 0; i < 0x40001; ++i) {
                u8 lhs = *at<be<u8>>(a + i), rhs = *at<be<u8>>(b + i);
                if (lhs != rhs)
                    break;
                if (lhs == 0) {
                    equal = true;
                    break;
                }
            }
        }
        bool oasis = equal && room == 33;
        auto* geometry = &mGeometry[oasis ? 2 : 1];
        mpShapeGroups = &geometry->mShapes;
        mpMaterialBuffers = &geometry->mMaterials;
        mpUncutIndices = &geometry->mUncut;
        mpCutIndices = &geometry->mCut;
        selected = &geometry->mSampler;
        mpSampler = selected;
    }
    if (ea(selected) == ea(&mGeometry[2].mSampler))
        flower->mFlags = u8(flower->mFlags) | 0x40;
    roomData->newData(data);
    mNextData = u16(index);
}

/* 02548FEC */
void* dFlower_packet_c::newData(s32 type, cXyz* position, s32 room, s32 variant) {
    if (u32(room) >= 64)
        flower_assert(at<void>(guest_str("d_flower.cpp")), u32(0x1F68),
                      at<void>(guest_str("0 <= i_roomNo && i_roomNo < 64")));
    u32 start = mNextData;
    u32 base = ea(mData);
    for (u32 i = start; i < 200; ++i) {
        auto* data = at<dFlower_data_c>(base + i * sizeof(dFlower_data_c));
        if (!(data->mFlags & dFlower_data_c::FLAG_ACTIVE)) {
            setData(data, i, type, position, room, variant);
            return data;
        }
    }
    for (u32 i = 0; i < start; ++i) {
        auto* data = at<dFlower_data_c>(base + i * sizeof(dFlower_data_c));
        if (!(data->mFlags & dFlower_data_c::FLAG_ACTIVE)) {
            setData(data, i, type, position, room, variant);
            return data;
        }
    }
    return nullptr;
}

/* 02544CBC */
s32 dFlower_packet_c::newAnm() {
    for (s32 i = 8; i < 72; ++i) {
        auto& animation = mAnm[i];
        if (!animation.mActive) {
            animation.mActive = 1;
            animation.mBend = 0;
            animation.mAngleY = 0;
            return i;
        }
    }
    return -1;
}

/* 02545A0C */
void dFlower_packet_c::setAnm(s32 index, s32 angle) {
    // HD: preserve guest address wrapping for the unchecked animation index.
    auto* animation = at<dFlower_anm_c>(ea(mAnm) + u32(index) * sizeof(dFlower_anm_c));
    animation->mActive = 1;
    animation->mBend = 0;
    animation->mAngleY = s16(angle);
}

// HD: shader geometry, lighting and uniforms replace the GameCube display lists.

/* 025486B8 */
void dFlower_packet_c::prepareGraphics() {
    // HD: GX2 uses per-room textures and per-flower transforms in place of display lists.
    Local<Mtx34> view;
    flowerCopyMatrix(view.get(), &flowerViewMatrix);
    flowerTransformSetView(&mTransformSet, u32(0), view.get(), &flowerProjection,
                           at<void>(flowerCamera + 0x240));
    Local<flower::TevStr> lighting;
    flower::LightObj* lightObjects[3] = {&lighting->mLightObj, &lighting->mHdLightParam[0],
                                         &lighting->mHdLightParam[1]};
    for (auto* object : lightObjects) {
        auto* defaults = reinterpret_cast<flower::LightDefaults*>(object);
        // Preserve the reference's word copies for colors and float copies for light parameters.
        for (u32 i = 0; i < 17; ++i) {
            if (i == 6 || i == 7 || i == 8)
                defaults->mWords[i] = flowerInitialLight.mWords[i];
            else
                defaults->mFloat[i] = flowerInitialLight.mFloat[i];
        }
    }
    flowerInitLight(lighting.get(), s32(flowerCurrentRoom), u32(0xFF));
    void* environment = flowerGetEnvironment();
    flowerSetupEnvironment(environment, s32(1), u32(0), lighting.get());
    Local<flower::Vector4> color;
    Local<flower::Vector4> scaled;
    auto* material = static_cast<flower::FogDescriptor*>(mTransformSet.mpDescriptor.get());
    flowerNormalizedColor(color.get(), lighting->mColorC0);
    flowerConvertFogColor(scaled.get(), color.get(), lighting->mLightObj.mFactors[1]);
    auto* words = reinterpret_cast<be<u32>*>(scaled.get());
    for (u32 i = 0; i < 4; ++i)
        material->mColors[0][i] = words[i];
    material = static_cast<flower::FogDescriptor*>(mTransformSet.mpDescriptor.get());
    flowerNormalizedColor(color.get(), lighting->mHdLightParam[1].mColorS10);
    flowerConvertFogColor(scaled.get(), color.get(), lighting->mHdLightParam[1].mFactors[1]);
    for (u32 i = 0; i < 4; ++i)
        material->mColors[1][i] = words[i];
    flowerTransformSetUpdate(&mTransformSet);
    u32 drawn = 0;
    f32 divisor = colorDivisor;
    Local<Mtx34> matrix;
    for (u32 room = 0; room < 64; ++room) {
        auto* data = mRoom[room].mpHead.get();
        if (!data)
            continue;
        auto* node = &mTextures[room];
        for (u32 i = 0; i < 4; ++i)
            node->mColors[4].mValues[i] = f32(double(s16(lighting->mColorC0[i])) / double(divisor));
        const auto* rgba = reinterpret_cast<const be<u8>*>(&lighting->mColorK0);
        for (u32 i = 0; i < 4; ++i)
            node->mColors[5].mValues[i] = f32(double(u8(rgba[i])) / double(divisor));
        flowerScaleColor(&node->mColors[5], lighting->mLightObj.mFactors[0]);
        auto* source = static_cast<flower::FogDescriptor*>(mTransformSet.mpDescriptor.get());
        auto* target = reinterpret_cast<be<u32>*>(&node->mColors[2]);
        for (u32 i = 0; i < 4; ++i)
            target[i] = source->mColors[0][i];
        flowerUniformUpdate(node);
        flowerApplyLightColor(lighting.get());
        while (data) {
            if (!(data->mFlags & dFlower_data_c::FLAG_CULLED)) {
                for (u32 i = 0; i < 12; ++i)
                    matrix->m[i / 4][i % 4] = data->mMtx.m[i / 4][i % 4];
                // The counter remains unchecked and wraps in the guest address space.
                auto* drawNode =
                    at<flower::Transform>(ea(mTransforms) + drawn * sizeof(flower::Transform));
                flowerMatrixCopy(matrix.get(), &drawNode->mMatrix);
                flowerUniformUpdate(drawNode);
                ++drawn;
            }
            data = data->mpNext.get();
        }
    }
}

/* 02547C94 */
void dFlower_packet_c::drawGraphics(flower::DrawInfo* context) {
    auto* draw = static_cast<flower::DrawInfo*>(context);
    flower::ShaderMaterial* shader = nullptr;
    s32 pass = draw->mPass;
    if (pass < 4)
        shader = static_cast<flower::ShaderMaterial*>(
            flowerShaderRow(&mGeometry[0].mShapes, u32(pass))->mpMaterial.get());
    auto* cache = static_cast<flower::ShaderCache*>(flowerGetShaderCache(&flowerShaderContext));
    auto* material = shader->mpProgram.get();
    if (material != cache->mpProgram.get()) {
        u8 flags = material->mFlags;
        u32 oldProgram = cache->mState;
        if (flags & 2) {
            material->mFlags = flags & ~2;
            flowerUpdateShaderProgram(material, u32(0));
        }
        u32 program = material->mpState->mState;
        if (oldProgram != program)
            flowerBindShaderState(at<void>(program));
        u32 size = material->mBinarySize;
        if (size)
            flowerGX2BindProgram(material->mpBinary.v, size);
        else
            flowerBindShaderProgram(material);
        cache->mState = program;
        cache->mpProgram = material;
    }
    pass = draw->mPass;
    if (pass == 0) {
        auto* object = draw->mpDrawObject.get();
        if (object)
            flowerDrawUniforms(shader, object->mpUniformData.get());
    } else if (pass == 1 || pass == 2) {
        flowerDrawUniforms(shader,
                           static_cast<flower::UniformData*>(mTransformSet.mpDescriptor.get()));
        if (pass == 2) {
            auto* object = static_cast<flower::UniformObject*>(draw->mpObject.get());
            if (object)
                call_ptr<void>(object->mpVtable->mApply.get(), object, shader);
            flowerApplyDrawObject(context, shader);
        }
    }
    Local<flower::GraphicsState> state;
    flowerGraphicsStateConstruct(state.get());
    state->mBlendMode = 2;
    state->mDepthEnabled = 1;
    state->mDepthFunction = 4;
    state->mDepthValue = drawDepth;
    u32 flags = state->mDepthBits;
    state->mDepthBits = (((flags & ~15u) + 7u) & 0xFFFFFF0Fu) + 16u;
    flowerApplyDrawMode(u32(draw->mPass), state.get());
    flowerGraphicsStateApply(state.get());
    u32 drawn = 0;
    for (u32 room = 0; room < 64; ++room) {
        auto* data = mRoom[room].mpHead.get();
        if (data) {
            auto* node = &mTextures[room];
            call_ptr<void>(node->mpVtable->mApply.get(), node, shader);
        }
        s32 texture = -1, shape = -1;
        while (data) {
            if (!(data->mFlags & dFlower_data_c::FLAG_CULLED)) {
                auto* node =
                    at<flower::Transform>(ea(mTransforms) + drawn * sizeof(flower::Transform));
                call_ptr<void>(node->mpVtable->mApply.get(), node, shader);
                ++drawn;
                bool special = (data->mFlags & dFlower_data_c::FLAG_PINK) != 0;
                s32 wantedTexture = special ? 1 : 0;
                if (texture != wantedTexture) {
                    void* params = shader->mSamplerCount ? shader->mpSamplers.get() : nullptr;
                    void* resource = special ? mpSampler.get() : &mGeometry[0].mSampler;
                    // HD: the external sampler API takes the opaque descriptor after its header word.
                    flowerBindSamplers(resource, at<void>(ea(params) + 4), u32(0), u32(0));
                    texture = wantedTexture;
                }
                bool cut = (data->mFlags & dFlower_data_c::FLAG_CUT) != 0;
                s32 wantedShape = (special ? 2 : 0) + (cut ? 1 : 0);
                if (shape != wantedShape) {
                    auto* descriptor = special
                                           ? static_cast<flower::ShapeGroups*>(mpShapeGroups.get())
                                           : &mGeometry[0].mShapes;
                    auto* row = flowerShaderRow(descriptor, u32(draw->mPass));
                    u32 index = mGeometry[0].mMaterials.mFrameIndex;
                    u32 column = (index ? u32(__builtin_clz(index)) : 32u) / 4u & ~7u;
                    auto* selected = at<flower::ShapeArray>(ea(row) + column + 4);
                    auto* buffer = selected->mpShapes.get();
                    if (cut && u32(selected->mCount) > 1)
                        buffer = at<flower::Shape>(ea(buffer) + sizeof(flower::Shape));
                    flowerShapeBind(buffer);
                    shape = wantedShape;
                }
                auto* geometry = special ? static_cast<flower::IndexBuffer*>(
                                               cut ? mpCutIndices.get() : mpUncutIndices.get())
                                         : (cut ? &mGeometry[0].mCut : &mGeometry[0].mUncut);
                flowerIndexedDraw(geometry);
            }
            data = data->mpNext.get();
        }
    }
    flowerGraphicsStateApply(&flowerDefaultGraphicsState);
}

/* inlined into dFlower_data_c::WorkAt */
static void flowerApplyTexture(void* emitter, Local<flower::EmitterColor>& texture, f32 one) {
    texture->mVtable = ea(&flowerSafeStringVtable);
    texture->mValues[1] = one;
    texture->mEnabled = 1;
    texture->mValues[3] = one;
    texture->mpName.v = guest_str("");
    texture->mPadding[1] = 0;
    texture->mValues[2] = one;
    texture->mPadding[0] = 0;
    texture->mPadding[2] = 0;
    texture->mValues[0] = one;
    u32 play = ea(dComIfGp_get()), control = u32(at<dComIfG_inf_c>(play)->play.mParticle.v);
    for (u32 k = 0; k < 4; ++k)
        texture->mWords[k] = u32(at<flower::ParticleControl>(control)->mColor[k]);
    flowerEmitterSetColor(emitter, texture.get());
}

/* inlined into dFlower_packet_c__dt */
static void flowerHeapReleaseField(be<u32>* field) {
    u32 pointer = *field;
    auto* heap =
        static_cast<flower::Heap*>(flowerFindHeap(at<void>(heapManager), at<void>(pointer)));
    u32 target = heap->mpVtable->mFree.get();
    call_ptr<void>(target, heap, at<void>(*field));
}

/* inlined into dFlower_packet_c__dt */
static void flowerCleanupBuffers(flower::MaterialBuffers* materials) {
    for (u32 pair = 0; pair < 2; ++pair)
        for (u32 member = 0; member < 2; ++member) {
            auto* part = &materials->mPart[pair][member];
            flowerShapeClear(&part->mShape);
            u32 pointer = part->mpVertexStorage.v;
            part->mpVertices = nullptr;
            if (pointer) {
                (void)u32(part->mVertexCapacity);
                flowerHeapReleaseField(&part->mpVertexStorage.v);
                part->mVertexCapacity = 0;
                part->mpVertexStorage = nullptr;
            }
        }
}

/* inlined into dFlower_packet_c__dt */
static void flowerCleanupShaderArray(flower::ShapeGroups* descriptor) {
    auto* pointer = descriptor->mpGroups.get();
    if (!pointer)
        return;
    s32 count = s32(descriptor->mCapacity);
    u32 offset = 0;
    for (s32 i = 0; i < count; ++i, offset += sizeof(flower::ShapeGroup)) {
        auto* row = at<flower::ShapeGroup>(ea(pointer) + offset);
        if (row) {
            auto* children = row->mShapes[0].mpShapes.get();
            row->mpMaterial = nullptr;
            for (u32 side = 0; side < 2; ++side) {
                auto* shapes = &row->mShapes[side];
                if (side)
                    children = shapes->mpShapes.get();
                if (children) {
                    s32 number = shapes->mCount;
                    u32 childOffset = 0;
                    for (s32 j = 0; j < number; ++j, childOffset += sizeof(flower::Shape)) {
                        auto* child = at<flower::Shape>(ea(children) + childOffset);
                        u32 target = child->mpVtable->mDestroy.get();
                        call_ptr<void>(target, child, u32(2));
                        number = shapes->mCount;
                        children = shapes->mpShapes.get();
                    }
                    flowerHeapReleaseField(&shapes->mpShapes.v);
                    shapes->mCount = 0;
                    shapes->mpShapes = nullptr;
                }
            }
            count = s32(descriptor->mCapacity);
            pointer = descriptor->mpGroups.get();
        }
    }
    flowerHeapReleaseField(&descriptor->mpGroups.v);
    descriptor->mCapacity = 0;
    descriptor->mpGroups = nullptr;
}

/* inlined into dFlower_packet_c::drawGraphics */
static flower::ShapeGroup* flowerShaderRow(flower::ShapeGroups* descriptor, u32 index) {
    auto* pointer = descriptor->mpGroups.get();
    return index < descriptor->mCapacity
               ? at<flower::ShapeGroup>(ea(pointer) + index * sizeof(flower::ShapeGroup))
               : pointer;
}

/* inlined into dFlower_packet_c::drawGraphics */
static void flowerIndexedDraw(flower::IndexBuffer* geometry) {
    u32 count = geometry->mCount;
    if (count)
        flowerGX2DrawIndexed(geometry->mIndexFormat, count, geometry->mPrimitive,
                             geometry->mIndexType, u32(0), u32(1));
}

/* inlined into dFlower_packet_c::drawGraphics */
static void flowerDrawUniforms(flower::ShaderMaterial* shader, flower::UniformData* object) {
    auto* uniforms = shader->mUniformCount ? shader->mpUniforms.get() : nullptr;
    // HD: unchecked record indices retain 32-bit guest address wrapping.
    auto* record =
        at<flower::UniformRecord>(ea(object) + offsetof(flower::UniformData, mRecords) +
                                  u32(object->mRecordIndex) * sizeof(flower::UniformRecord));
    s32 vertex = uniforms->mVertex, pixel = uniforms->mPixel, geometry = uniforms->mGeometry;
    u32 size = record->mSize, buffer = record->mBuffer;
    if (pixel != -1)
        flowerGX2PixelUniform(pixel, buffer, size);
    if (vertex != -1)
        flowerGX2VertexUniform(vertex, buffer, size);
    if (geometry != -1)
        flowerGX2GeometryUniform(geometry, buffer, size);
}

/* inlined into dFlower_packet_c__ct */
static void* flowerHeapAllocate(u32 size, u32 align) {
    auto* heap = static_cast<flower::Heap*>(flowerGetHeap(at<void>(heapManager)));
    return call_ptr<void*>(heap->mpVtable->mAllocate.get(), heap, size, align);
}

/* inlined into dFlower_packet_c__ct */
static void flowerShaderLoad(u32 descriptor, void* name) {
    auto* manager = static_cast<flower::MaterialManager*>(flowerGetMaterialManager());
    s32 index = flowerFindMaterialIndex(manager->mpResource.get(), name);
    flower::MaterialInstance* entry = nullptr;
    if (index >= 0) {
        auto* array = manager->mpInstances.get();
        u32 count = manager->mCount;
        entry = u32(index) < count ? at<flower::MaterialInstance>(
                                         ea(array) + u32(index) * sizeof(flower::MaterialInstance))
                                   : array;
        if (!entry->mReady) {
            auto* archive = manager->mpResource.get();
            flower::MaterialBinding* record = nullptr;
            if (u32(index) < archive->mCount)
                record = at<flower::MaterialBinding>(ea(archive->mpBindings.get()) +
                                                     u32(index) * sizeof(flower::MaterialBinding));
            flowerCreateMaterialInstance(entry, record, u32(0));
            array = manager->mpInstances.get();
            count = manager->mCount;
            entry = u32(index) < count
                        ? at<flower::MaterialInstance>(
                              ea(array) + u32(index) * sizeof(flower::MaterialInstance))
                        : array;
        }
    }
    flowerBindMaterialGroups(at<void>(descriptor), entry, u32(0));
}

/* inlined into dFlower_packet_c__ct */
static void flowerShaderBuffers(u32 descriptorAddress, u32 buffers) {
    auto* descriptor = at<flower::ShapeGroups>(descriptorAddress);
    auto* materials = at<flower::MaterialBuffers>(buffers);
    for (u32 i = 0; i < descriptor->mCount; ++i) {
        auto* row = flowerShaderRow(descriptor, i);
        void* shader = row->mpMaterial.get();
        row->mpMaterial = nullptr;
        for (u32 side = 0; side < 2; ++side) {
            auto* shapes = &row->mShapes[side];
            auto* pointer = shapes->mpShapes.get();
            if (pointer) {
                for (s32 n = 0; n < shapes->mCount; ++n) {
                    auto* child = at<flower::Shape>(ea(pointer) + u32(n) * sizeof(flower::Shape));
                    call_ptr<void>(child->mpVtable->mDestroy.get(), child, u32(2));
                    pointer = shapes->mpShapes.get();
                }
                flowerHeapReleaseField(&shapes->mpShapes.v);
                shapes->mCount = 0;
                shapes->mpShapes = nullptr;
            }
        }
        row->mpMaterial = shader;
        for (u32 side = 0; side < 2; ++side) {
            u32 pointer = ea(flowerHeapAllocate(2 * sizeof(flower::Shape), 4));
            for (u32 n = 0; n < 2; ++n)
                if (u32(pointer + n * sizeof(flower::Shape)))
                    flowerShapeConstruct(at<void>(pointer + n * sizeof(flower::Shape)));
            if (pointer) {
                row->mShapes[side].mpShapes.v = pointer;
                row->mShapes[side].mCount = 2;
            }
        }
        for (u32 side = 0; side < 2; ++side)
            for (u32 n = 0; n < 2; ++n) {
                u32 child = row->mShapes[side].mpShapes.v;
                if (n < u32(row->mShapes[side].mCount))
                    child += n * sizeof(flower::Shape);
                flowerConfigureShape(shader, at<void>(child),
                                     &materials->mPart[n][side].mVertexBuffer,
                                     &materials->mVertexFormat, u32(0));
            }
    }
}

/* inlined into dFlower_packet_c__ct */
static u32 flowerInlineClear(u32 group, u32 side, u32 bytes) {
    auto* materials = at<flower::MaterialBuffers>(group);
    u32 buffer = group + (u32(materials->mFrameIndex) + side * 2) * sizeof(flower::MaterialPart);
    u32 begin = at<flower::MaterialPart>(buffer)->mpVertices.v, end = begin + bytes;
    for (u32 line = begin; line < end; line += 32) {
        auto* cacheLine = at<be<u8>>(line & ~31u);
        for (u32 k = 0; k < 32; ++k)
            cacheLine[k] = 0;
    }
    if (begin < end)
        buffer = group + (u32(materials->mFrameIndex) + side * 2) * sizeof(flower::MaterialPart);
    return buffer;
}

/* inlined into dFlower_packet_c__ct */
static void flowerVertices(flower::Vertex* output, be<u32>* count, cXyz* position, cXyz* normal,
                           flower::Vector2* texcoord, flower::Vector4* color, bool setters) {
    for (u32 i = 0; i < *count; ++i) {
        auto* vertex = at<flower::Vertex>(ea(output) + i * sizeof(flower::Vertex));
        if (setters) {
            flowerSetVec3(vertex->mPosition, position[i].x, position[i].y, position[i].z);
            flowerSetVec3(vertex->mNormal, normal[i].x, normal[i].y, normal[i].z);
            flowerSetVec4(vertex->mColor, color[i].mValues[0], color[i].mValues[1],
                          color[i].mValues[2], color[i].mValues[3]);
            flowerSetVec2(vertex->mTexCoord, texcoord[i].mValues[0], texcoord[i].mValues[1]);
        } else {
            const auto* pos = reinterpret_cast<const be<f32>*>(&position[i]);
            const auto* norm = reinterpret_cast<const be<f32>*>(&normal[i]);
            for (u32 k = 0; k < 3; ++k)
                vertex->mPosition[k] = pos[k];
            for (u32 k = 0; k < 3; ++k)
                vertex->mNormal[k] = norm[k];
            for (u32 k = 0; k < 4; ++k)
                vertex->mColor[k] = color[i].mValues[k];
            if (count == &flowerMeshCounts.mMeshes[2].mVertices)
                flowerSetVec2(vertex->mTexCoord, texcoord[i].mValues[0], texcoord[i].mValues[1]);
            else
                for (u32 k = 0; k < 2; ++k)
                    vertex->mTexCoord[k] = texcoord[i].mValues[k];
        }
    }
}

// HD: compiler-generated and template code of this translation unit.

/* 02544AA4 */
void dFlower_data_c__dt(void* self, u32 flags) {
    if (self && (flags & 1))
        flowerFree(self);
}

/* 02548554 */
void flowerCopyMatrix(void* destination, void* source) {
    auto* input = static_cast<be<f32>*>(source);
    auto* output = static_cast<be<f32>*>(destination);
    // HD: load the complete matrix before storing, including overlapping inputs.
    f32 values[12];
    for (int i = 0; i < 12; ++i)
        values[i] = input[i];
    for (int i = 0; i < 12; ++i)
        output[i] = values[i];
}

/* 025485F4 */
void flowerNormalizedColor(void* destination, void* source) {
    auto* input = static_cast<be<s16>*>(source);
    auto* output = static_cast<be<f32>*>(destination);
    s16 red = input[0], blue = input[2], green = input[1];
    f32 scale = colorDivisor;
    s16 alpha = input[3];
    f32 colors[4] = {f32(f64(red) / scale), f32(f64(green) / scale), f32(f64(blue) / scale),
                     f32(f64(alpha) / scale)};
    for (int i = 0; i < 4; ++i)
        output[i] = colors[i];
}

/* 02549118 */
void flowerStaticInit() {
    firstStaticTail = 0;
    firstStaticHead = 0;
    secondStaticTail = 0;
    secondStaticHead = 0;
    flowerRegisterStatic(&flowerDestructorRecords.mRecords[0]);
    f32 first = staticFirstValue, second = staticSecondValue;
    firstStaticValue = first;
    secondStaticValue = second;
    flowerFirstStaticFlagConstruct(&flowerFirstStaticFlag);
    flowerRegisterStatic(&flowerDestructorRecords.mRecords[1]);
    flowerSecondStaticFlagConstruct(&flowerSecondStaticFlag);
    flowerRegisterStatic(&flowerDestructorRecords.mRecords[2]);
}

/* 02549290 */
void* flowerStringConstruct(void* self, void* text) {
    if (!self)
        self = flowerAllocate(u32(0x8));
    if (!self)
        return nullptr;
    auto* string = static_cast<flower::String*>(self);
    string->mText = ea(text);
    string->mpVtable.v = ea(&flowerSafeStringVtable);
    return self;
}

/* 025492E0 */
void* flowerDrawNodeConstruct(void* self) {
    if (!self)
        self = flowerAllocate(u32(0xA8));
    if (!self)
        return nullptr;
    auto* transform = static_cast<flower::Transform*>(self);
    flowerUniformConstruct(self);
    transform->mpVtable.v = ea(&flowerTransformVtable);
    flowerClearUniformData(&transform->mMatrix, u32(0x34));
    if (ea(&transform->mMatrix) == 0)
        flowerAllocate(u32(sizeof(Mtx34)));
    return self;
}

/* 02549350 */
void* flowerMatrixConstruct(void* self) {
    if (!self)
        self = flowerAllocate(u32(0x10));
    if (!self)
        return nullptr;
    return self;
}

/* 02549604 */
void* flowerBuffer1Construct(void* self) {
    if (!self)
        self = flowerAllocate(u32(0x254));
    if (!self)
        return nullptr;
    auto* part = static_cast<flower::MaterialPart*>(self);
    flowerVertexBufferConstruct(&part->mVertexBuffer);
    flowerShapeConstruct(&part->mShape);
    part->mpVertexStorage = nullptr;
    part->mVertexCapacity = 0;
    return self;
}

/* 02549660 */
void* flowerBuffer2Construct(void* self) {
    if (!self)
        self = flowerAllocate(u32(0x254));
    if (!self)
        return nullptr;
    auto* part = static_cast<flower::MaterialPart*>(self);
    flowerVertexBufferConstruct(&part->mVertexBuffer);
    flowerShapeConstruct(&part->mShape);
    part->mpVertexStorage = nullptr;
    part->mVertexCapacity = 0;
    return self;
}

/* 025491AC */
void flowerAnimationDestroy(void* self, u32 flags) {
    if (self && (flags & 1))
        flowerFree(self);
}

/* 025496BC */
void flowerSetVec3(void* self, f32 x, f32 y, f32 z) {
    auto* vector = static_cast<cXyz*>(self);
    vector->y = y;
    vector->z = z;
    vector->x = x;
}

/* 025496CC */
void flowerSetVec4(void* self, f32 x, f32 y, f32 z, f32 w) {
    auto* vector = static_cast<be<f32>*>(self);
    vector[2] = z;
    vector[1] = y;
    vector[3] = w;
    vector[0] = x;
}

/* 025496E0 */
void flowerSetVec2(void* self, f32 x, f32 y) {
    auto* vector = static_cast<be<f32>*>(self);
    vector[0] = x;
    vector[1] = y;
}

/* 02549238 */
void* flowerBufferSelect(void* self, u32 slot, u32 clear) {
    auto* materials = static_cast<flower::MaterialBuffers*>(self);
    u32 index = materials->mFrameIndex + slot * 2;
    auto* part = at<flower::MaterialPart>(ea(self) + index * sizeof(flower::MaterialPart));
    if (clear) {
        u32 begin = ea(part->mpVertices.get()), end = begin + 0x7160;
        // HD: clear complete cache lines, retaining unsigned guest-address wrapping.
        for (u32 line = begin; line < end; line += 32) {
            auto* bytes = at<be<u8>>(line & ~31u);
            for (u32 k = 0; k < 32; ++k)
                bytes[k] = 0;
        }
        if (begin < end)
            part = at<flower::MaterialPart>(ea(self) + (u32(materials->mFrameIndex) + slot * 2) *
                                                           sizeof(flower::MaterialPart));
    }
    return part;
}

/* 025496EC */
void flowerBufferRotate(void* self, u32 unused, u32 count) {
    auto* materials = static_cast<flower::MaterialBuffers*>(self);
    u32 active = materials->mFrameIndex;
    if (count) {
        u32 buffer = ea(self) + active * sizeof(flower::MaterialPart) +
                     offsetof(flower::MaterialPart, mVertexBuffer);
        for (u32 i = 0; i < count; ++i, buffer += 2 * sizeof(flower::MaterialPart)) {
            auto* vertexBuffer = at<flower::VertexBuffer>(buffer);
            flowerVertexBufferFlush(vertexBuffer, u32(0), vertexBuffer->mFlushSize);
        }
        active = materials->mFrameIndex;
    }
    materials->mFrameIndex = active == 0;
}

/* 02549768 */
void flowerSurfaceAssign(void* self, void* source) {
    auto* target = static_cast<flower::SurfaceTransfer*>(self);
    auto* input = static_cast<flower::SurfaceTransfer*>(source);
    if (target->mDimension != input->mDimension || target->mWidth != input->mWidth ||
        target->mHeight != input->mHeight || target->mDepth != input->mDepth ||
        target->mMipLevels != input->mMipLevels || target->mFormat != input->mFormat ||
        target->mSwizzle != input->mSwizzle || target->mTileMode != input->mTileMode ||
        target->mAntialias != input->mAntialias) {
        flowerCopySamplerState(self, source);
        return;
    }
    u32 image = input->mImage, mip = input->mMipmap;
    target->mImage = image;
    target->mMipmap = mip;
    target->mCachedImage = image;
    target->mCachedMipmap = mip;
}

/* 02549818 */
void flowerFlagsSet(void* self, u32 mask) {
    auto* flags = static_cast<be<u8>*>(self);
    *flags = u8(*flags) | mask;
}

/* 02549828 */
void flowerBuffer1Destroy(void* self, u32 flags) {
    if (!self)
        return;
    auto* part = static_cast<flower::MaterialPart*>(self);
    flowerShapeDestroy(&part->mShape, u32(2));
    flowerVertexBufferDestroy(&part->mVertexBuffer, u32(2));
    if (flags & 1)
        flowerFree(self);
}

/* 02549888 */
void flowerBuffer2Destroy(void* self, u32 flags) {
    if (!self)
        return;
    auto* part = static_cast<flower::MaterialPart*>(self);
    flowerShapeDestroy(&part->mShape, u32(2));
    flowerVertexBufferDestroy(&part->mVertexBuffer, u32(2));
    if (flags & 1)
        flowerFree(self);
}

/* 025498E8 */
void flowerDrawNodeDestroy(void* self, u32 flags) {
    if (!self)
        return;
    flowerUniformDestroy(self, u32(0));
    if (flags & 1)
        flowerFree(self);
}

/* 0254993C */
void flowerRoomNodeDestroy(void* self, u32 flags) {
    if (!self)
        return;
    flowerUniformDestroy(self, u32(0));
    if (flags & 1)
        flowerFree(self);
}

/* 02549990 */
void flowerEmptyResource1(void* self) {}

/* 02549994 */
void flowerEmptyResource2(void* self) {}

/* 02549998 */
u32 flowerResourceEnabled(void* self) { return 1; }

/* 0254937C */
void* flowerRoomNodeConstruct(void* self) {
    if (!self)
        self = flowerAllocate(u32(0x364));
    if (!self)
        return nullptr;
    auto* transform = static_cast<flower::TextureTransform*>(self);
    flowerUniformConstruct(self);
    transform->mpVtable.v = ea(&flowerTextureTransformVtable);
    flowerClearUniformData(transform->mColors, u32(0x2F0));
    f32 zero = textureZero, one = textureOne;
    // Eleven initial vectors, each with an alpha/homogeneous component of one.
    for (u32 i = 0; i < 44; ++i)
        transform->mColors[i / 4].mValues[i % 4] = (i % 4) == 3 ? one : zero;
    for (u32 i = 0; i < 3; ++i)
        flowerArrayConstruct(
            transform->mVectors[i], u32(2), u32(sizeof(flower::Vector4)),
            at<void>(guest_fn(reinterpret_cast<const void*>(&flowerMatrixConstruct))));
    // HD: GHS retains the wrapped null checks, including the first tail vector as a matrix-sized object.
    for (u32 i = 0; i < 9; ++i)
        if (u32(ea(transform->mMatrices) + i * sizeof(Mtx34)) == 0)
            flowerAllocate(u32(sizeof(Mtx34)));
    for (u32 i = 1; i < 6; ++i)
        if (ea(&transform->mTailVectors[i]) == 0)
            flowerAllocate(u32(sizeof(flower::Vector4)));
    return self;
}

/* 0254725C */
void dFlower_packet_c__dt(void* self, u32 flags) {
    if (!self)
        return;
    auto* packet = static_cast<dFlower_packet_c*>(self);
    packet->mPacketBase.mVtable = ea(&flowerPacketVtable);
    for (u32 i = 0; i < 3; ++i) {
        auto* materials = &packet->mGeometry[i].mMaterials;
        flowerCleanupBuffers(materials);
        materials->mState = 0;
    }
    for (u32 i : {2u, 1u, 0u}) {
        auto* geometry = &packet->mGeometry[i];
        flowerSamplerDestroy(&geometry->mSampler, u32(2));
        flowerIndexBufferDestroy(&geometry->mCut, u32(2));
        flowerIndexBufferDestroy(&geometry->mUncut, u32(2));
        auto* materials = &geometry->mMaterials;
        if (ea(materials)) {
            flowerCleanupBuffers(materials);
            materials->mState = 0;
            u32 callback = guest_fn(
                reinterpret_cast<const void*>(i ? &flowerBuffer1Destroy : &flowerBuffer2Destroy));
            flowerArrayDestroy(materials, u32(4), u32(sizeof(flower::MaterialPart)),
                               at<void>(callback), u32(0), u32(0));
        }
        flowerCleanupShaderArray(&geometry->mShapes);
    }
    flowerArrayDestroy(packet->mTextures, u32(64), u32(sizeof(flower::TextureTransform)),
                       at<void>(guest_fn(reinterpret_cast<const void*>(&flowerDrawNodeDestroy))),
                       u32(0), u32(0));
    flowerArrayDestroy(packet->mTransforms, u32(200), u32(sizeof(flower::Transform)),
                       at<void>(guest_fn(reinterpret_cast<const void*>(&flowerRoomNodeDestroy))),
                       u32(0), u32(0));
    flowerTransformSetDestroy(&packet->mTransformSet, u32(2));
    flowerPacketBaseDestroy(self, u32(0));
    if (flags & 1)
        flowerFree(self);
}

/* WWHD glue
str 1004E130 "sea" in 02548D28
str 10000160 ""
str 1004E098 "flower_draw" in 02545A2C
str 1004E0C0 "ProgramTexture" in 02545A2C
str 1004E0D0 "ProgramTexture.bfres" in 02545A2C
str 1004E0A4 "Txo_ob_flower_white_64x64" in 02545A2C
str 1004E0E8 "Txo_ob_flower_pink_64x64" in 02545A2C
str 1004E088 "Txq_bessou_hana" in 02545A2C
str 1004E134 "d_flower.cpp" in 02548FEC
str 1004E144 "0 <= i_roomNo && i_roomNo < 64" in 02548FEC
str 1004E064 "d_flower.cpp" in 025457C4
str 1004E074 "hit_actor != (0)" in 025457C4
data regularIndices 101D7540
data regularCutIndices 101D81A0
data pinkIndices 101D8FE0
data pinkCutIndices 101D96E8
data oasisIndices 101E08E8
data oasisCutIndices 101E1BE4
data flowerRoomColors 1047E6CC
data flowerDestructorRecords 101E1CBC
data flowerFirstStaticFlag 10475944
data flowerSecondStaticFlag 10475945
data flowerTransformVtable 1016EF84
data flowerTextureTransformVtable 1016EFB4
data flowerParticleAngles 1047593C
data flowerMatrixStack 1048D0CC
data flowerFrustum 1048CFF0
data flowerCosineTable 104A44FC
data flowerPacketVtable 1004E180
data flowerVertexDeclaration 1004E170
data flowerMeshCounts 101D642C
data regularPositions 101D6460
data regularNormals 101D6898
data regularTexCoords 101D7270
data regularColors 101D6CD0
data regularCutPositions 101D7660
data regularCutNormals 101D7930
data regularCutTexCoords 101D7FC0
data regularCutColors 101D7C00
data pinkPositions 101D8230
data pinkNormals 101D859C
data pinkTexCoords 101D8D98
data pinkColors 101D8908
data pinkCutPositions 101D9118
data pinkCutNormals 101D928C
data pinkCutTexCoords 101D95F0
data pinkCutColors 101D9400
data oasisPositions 101D9778
data oasisNormals 101DB3D4
data oasisTexCoords 101DF600
data oasisColors 101DD030
data oasisCutPositions 101E1404
data oasisCutNormals 101E15FC
data oasisCutTexCoords 101E1A94
data oasisCutColors 101E17F4
data flowerWallQueryVtable 1004E01C
data flowerPolygonQueryVtable 1004E00C
data flowerGroundQueryVtable 1004DFFC
data flowerBackgroundQueryVtable 1004E02C
data flowerWallBaseVtable 1004DFEC
data flowerSafeStringVtable 1004DFD4
data flowerDefaultGraphicsState 104B474C
data flowerShaderContext 104B45C0
data flowerInitialLight 1016E414
data flowerViewMatrix 104B45F8
data flowerProjection 104B470C
data flowerCurrentRoom 1047E6C8
data textureEpsilon 100030B8
data groundRaise 1004E03C
data groundMinimum 1004E040
data flowerZero 1004E044
data collisionDistanceSquared 1004E048
data cutDistance 1004E04C
data particleRise 1004E050
data particleOne 1004E054
data pinkParticleScale 1004E058
data collisionRadius 1004E05C
data collisionHeight 1004E060
data drawDepth 1004E104
data swayScale 1004E108
data colorDivisor 1004E118
data cullRadius 1004E128
data normalAnimationRange 1004E12C
data staticFirstValue 1004E168
data staticSecondValue 1004E16C
data textureOne 1014517C
data textureZero 10145180
data cutSoundFlag 101D645C
data resourceManager 101F4F7C
data graphicsLock 101F8B18
data heapManager 101F8B4C
data swayCounter 101FF560
data firstStaticValue 10475934
data secondStaticValue 10475938
data firstStaticHead 10475948
data secondStaticHead 1047594C
data firstStaticTail 10475950
data secondStaticTail 10475954
data particleAnglesReady 10475958
data flowerDrawList 104B4634
data flowerCamera 104B4708
ext 02008974 f32 flowerCBgS_GroundCross(void* a0, void* a1)
ext 02008DAC void flowerGroundDestruct(void* a0, u32 a1)
ext 02008E0C void flowerGroundConstruct(void* a0)
ext 0200F378 s32 flowerCLib_addCalcAngleS(void* a0, s32 a1, s32 a2, s32 a3, s32 a4)
ext 0200F378 short flowerCLib_addCalcAngleS(void* a0, s16 a1, s16 a2, s16 a3, s16 a4)
ext 0200F8D0 s32 flowerCLib_chaseAngleS(void* a0, s16 a1, s16 a2)
ext 0200F8D0 s32 flowerCLib_chaseAngleS(void* a0, s32 a1, s32 a2)
ext 02018428 void flowerMassSetHeight(void* a0, f32 a1)
ext 020184DC void flowerMassSetRadius(void* a0, f32 a1)
ext 020195B0 s32 flowerAtan2(f32 a0, f32 a1)
ext 020198D8 f32 flowerCM_rndF(f32 a0)
ext 0201A478 void flowerCsXyz_csXyz(void* a0, u32 a1, u32 a2, u32 a3)
ext 0201A478 void* flowerCsXyz_csXyz(void* a0, s16 a1, s16 a2, s16 a3)
ext 0201ADE0 void flowerCXyzSubtract(void* a0, void* a1, void* a2)
ext 0207A9A0 void flowerCLib_calcTimer(void* a0)
ext 025157AC void* flowerDCcD_GetGObjInf(void* a0)
ext 0251694C void flowerHitConstruct(void* a0)
ext 025170D8 u32 flowerCheckMass(void* a0, void* a1, void* a2, void* a3)
ext 02520540 s32 flowerDComIfGp_getReverb(s32 a0)
ext 02555D0C void* flowerGetEnvironment()
ext 0255F8F4 void flowerApplyLightColor(void* a0)
ext 0255FFF4 void flowerInitLight(void* a0, s32 a1, u32 a2)
ext 025626A4 void flowerSetupEnvironment(void* a0, s32 a1, u32 a2, void* a3)
ext 025626A4 void flowerSetupEnvironment(void* a0, s32 a1, void* a2, void* a3)
ext 025A847C void* flowerDPa_control_c_set(void* a0, u32 a1, u32 a2, void* a3, u32 a4, u32 a5, u32 a6, u32 a7, s32 a8, void* a9, void* a10, u32 a11)
ext 025A847C void* flowerDPa_control_c_set(void* a0, u32 a1, u32 a2, void* a3, void* a4, u32 a5, u32 a6, u32 a7, s32 a8, void* a9, void* a10, u32 a11)
ext 025A847C void* flowerDPa_control_c_set(void* a0, u8 a1, u16 a2, void* a3, void* a4, void* a5, u8 a6, void* a7, s8 a8, void* a9, void* a10, void* a11)
ext 025D8120 u32 flowerFopAcM_createItemFromTable(void* a0, s32 a1, s32 a2, s32 a3, s32 a4, void* a5, s32 a6, void* a7)
ext 025D8120 void flowerFopAcM_createItemFromTable(void* a0, s32 a1, s32 a2, s32 a3, u32 a4, u32 a5, u32 a6, u32 a7)
ext 025DBE00 s32 flowerFopOvlpM_IsPeek()
ext 025E1A40 void flowerStartSound(u32 a0, void* a1, u32 a2, s32 a3)
ext 025E1B34 void flowerJAIZelBasic_seDeleteObject(void* a0)
ext 025F1884 void flowerCMtx_YrotS(void* a0, s16 a1)
ext 025F1884 void flowerCMtx_YrotS(void* a0, s32 a1)
ext 025F1BF4 void flowerCMtx_XrotM(void* a0, s16 a1)
ext 025F1BF4 void flowerCMtx_XrotM(void* a0, s32 a1)
ext 025F1C28 void flowerCMtx_YrotM(void* a0, s16 a1)
ext 025F1C28 void flowerCMtx_YrotM(void* a0, s32 a1)
ext 026124B0 void* flowerGetResource(void* a0, void* a1, void* a2, u32 a3)
ext 0273AA24 void flower_assert(void* a0, s32 a1, void* a2)
ext 0273AA24 void flower_assert(void* a0, u32 a1, void* a2)
ext 0273AD10 void* flowerAllocate(u32 a0)
ext 0273AF40 void flowerFree(void* a0)
ext 0274D2AC void flowerScaleColor(void* a0, f32 a1)
ext 0274D458 void flowerConvertFogColor(void* a0, void* a1, f32 a2)
ext 0274FBF8 void flowerLockGraphics(void* a0)
ext 0274FCCC void flowerUnlockGraphics(void* a0)
ext 02750250 void flowerGraphicsStateConstruct(void* a0)
ext 02750370 void flowerGraphicsStateApply(void* a0)
ext 02755FEC void* flowerFindHeap(void* a0, void* a1)
ext 02756140 void* flowerGetHeap(void* a0)
ext 02773870 void flowerLoadSamplerResource(void* a0, void* a1, void* a2)
ext 027B5430 void flowerIndexBufferConstruct(void* a0)
ext 027B54A0 void flowerIndexBufferDestroy(void* a0, u32 a1)
ext 027B54E0 void flowerIndexBufferSet(void* a0, void* a1, u32 a2, u32 a3)
ext 027B5BD8 void flowerVertexBufferConstruct(void* a0)
ext 027B5CBC void flowerVertexBufferDestroy(void* a0, u32 a1)
ext 027B5E94 void flowerVertexBufferFlush(void* a0, u32 a1, u32 a2)
ext 027B90AC s32 flowerFindMaterialIndex(void* a0, void* a1)
ext 027B9F68 void flowerBindShaderState(void* a0)
ext 027BB7CC void flowerBindShaderProgram(void* a0)
ext 027BB9E0 void flowerUpdateShaderProgram(void* a0, u32 a1)
ext 027BDEB4 void flowerCopySamplerState(void* a0, void* a1)
ext 027BDF7C void flowerSamplerConstruct(void* a0)
ext 027BE2B0 void flowerSamplerDestroy(void* a0, u32 a1)
ext 027BE53C void flowerBindSamplers(void* a0, void* a1, u32 a2, u32 a3)
ext 027BE6B8 void flowerSamplerResourceConstruct(void* a0)
ext 027BF734 void flowerShapeConstruct(void* a0)
ext 027BF7E8 void flowerShapeClear(void* a0)
ext 027BF880 void flowerShapeDestroy(void* a0, u32 a1)
ext 027BFE5C void flowerShapeBind(void* a0)
ext 027F0E04 s32 flowerDrawListEntry(void* a0, void* a1, u16 a2)
ext 027F0E04 void flowerDrawListEntry(void* a0, void* a1, u32 a2)
ext 027F1278 void flowerPacketBaseConstruct(void* a0)
ext 027F13DC void flowerPacketBaseDestroy(void* a0, u32 a1)
ext 027F29D4 void* flowerGetShaderCache(void* a0)
ext 027FB40C void flowerUniformConstruct(void* a0)
ext 027FB528 void flowerUniformDestroy(void* a0, u32 a1)
ext 027FB5D4 void flowerUniformSetState(void* a0, u32 a1)
ext 027FB678 void flowerUniformUpdate(void* a0)
ext 027FD6F4 void flowerTransformSetConstruct(void* a0)
ext 027FD764 void flowerTransformSetDestroy(void* a0, u32 a1)
ext 027FD838 void flowerTransformSetConfigure(void* a0, u32 a1, u32 a2)
ext 027FDA54 void flowerTransformSetView(void* a0, u32 a1, void* a2, void* a3, void* a4)
ext 027FE010 void flowerTransformSetUpdate(void* a0)
ext 027FF478 void flowerConfigureVertexBuffer(void* a0, void* a1, u32 a2, void* a3)
ext 027FF530 void flowerConfigureShape(void* a0, void* a1, void* a2, void* a3, u32 a4)
ext 027FFCBC void* flowerGetMaterialManager()
ext 027FFE54 void flowerApplyDrawObject(void* a0, void* a1)
ext 0280037C void flowerApplyDrawMode(u32 a0, void* a1)
ext 0280068C void flowerBindMaterialGroups(void* a0, void* a1, u32 a2)
ext 02800B0C void flowerCreateMaterialInstance(void* a0, void* a1, u32 a2)
ext 0281E5A8 void flowerEmitterSetColor(void* a0, void* a1)
ext 02838148 s32 flowerFrustumCheck(void* a0, void* a1, void* a2, f32 a3)
ext 028E8DD0 f32 flowerVectorSquareMagnitude(void* a0)
ext 028E90D4 void flowerMatrixCopy(void* a0, void* a1)
ext 028EAB2C void flowerSecondStaticFlagConstruct(u32 a0)
ext 028EAB2C void flowerSecondStaticFlagConstruct(void* a0)
ext 028ED6F8 void flowerFirstStaticFlagConstruct(u32 a0)
ext 028ED6F8 void flowerFirstStaticFlagConstruct(void* a0)
ext 028EFFD0 void flowerArrayConstruct(void* a0, u32 a1, u32 a2, u32 a3)
ext 028EFFD0 void flowerArrayConstruct(void* a0, u32 a1, u32 a2, void* a3)
ext 028F0164 void flowerArrayDestroy(void* a0, u32 a1, u32 a2, void* a3, u32 a4, u32 a5)
ext 028F026C void flowerRegisterStatic(u32 a0)
ext 028F026C void flowerRegisterStatic(void* a0)
ext 028F4384 f32 flowerSquareRoot(f32 a0)
ext 028F4384 f64 flowerSquareRoot(f64 a0)
ext 028F521C void flowerClearUniformData(void* a0, u32 a1)
ext C00060E0 void flowerGX2BindProgram(u32 a0, u32 a1)
ext C0006178 void flowerGX2DrawIndexed(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5)
ext C00068A8 void flowerGX2GeometryUniform(s32 a0, u32 a1, u32 a2)
ext C0006900 void flowerGX2PixelUniform(s32 a0, u32 a1, u32 a2)
ext C0006A38 void flowerGX2VertexUniform(s32 a0, u32 a1, u32 a2)
*/
