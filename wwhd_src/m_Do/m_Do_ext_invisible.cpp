#include "m_Do/m_Do_ext_invisible.h"

static InvisibleDrawState g_invisibleDefaultState;
static InvisibleRendererPointer g_invisibleRenderer;
static InvisibleDrawBuffers g_invisibleDrawBuffers;
static InvisiblePacketMethods InvisiblePacketArray_vtable;

/* 025E87C0 */
void mDoExt_invJntPacket_draw(mDoExt_invJntPacket* self, InvisibleDrawArguments* arguments) {
    // HD: draw takes a material descriptor and dispatches through HD material methods.
    if (!arguments->mEnabled) return;
    InvisibleModel* model = self->mModel;
    InvisibleModelData* data = model->mpModelData;
    u32 index = self->mJntNum;
    u32 count = data->mJointCount;
    InvisibleJoint* joint = data->mpJoints;
    if (index < count) joint += index;
    InvisibleMesh* mesh = joint->mpMesh;
    u32 material = arguments->mMaterial;
    gabi::Local<InvisibleDrawInfo> workspace;
    InvisibleDrawInfo* draw = workspace.get();
    // HD: initialize only the 52-byte descriptor; state constructor owns its separate region.
    draw->mMaterial = 0; draw->mEnabled = 0;
    for (u32 i = 0; i < 3; ++i) draw->mUnknown[i] = 0;
    draw->mpModelState = nullptr;
    for (u32 i = 0; i < 7; ++i) draw->mUnknownTail[i] = 0;
    draw->mMaterial = material;
    draw->mEnabled = 1;
    draw->mpModelState = model->mDrawState;
    InvisibleDrawState_construct(&draw->mState);
    draw->mState.mFlags = 0;
    InvisibleDrawState_apply(&draw->mState);
    while (mesh) {
        InvisibleShape* shape = mesh->mpShape;
        u32 enabled = shape->mEnabled;
        InvisibleShapeIndex* entry = shape->mpShape;
        u32 shapeIndex = entry->mIndex;
        if (enabled) {
            model = self->mModel;
            u32 shapeCount = model->mShapeCount;
            be<u16>* materialIds = model->mpShapeMaterialIds;
            u32 materialCount = model->mMaterialCount;
            if (shapeIndex < shapeCount) materialIds += shapeIndex;
            u32 materialIndex = *materialIds;
            InvisibleMaterial* current = model->mpMaterials;
            if (materialIndex < materialCount) current += materialIndex;
            gabi::call_ptr(current->mpMethods->draw.get(), current, draw);
        }
        mesh = mesh->mpNext;
    }
    InvisibleDrawState_apply(&g_invisibleDefaultState);
}

/* 025E890C */
InvisiblePacketMember* InvisiblePacketMember_ct(InvisiblePacketMember* self, void* owner) {
    if (!self) {
        self = static_cast<InvisiblePacketMember*>(operator_new(sizeof(InvisiblePacketMember)));
        if (!self) return nullptr;
    }
    self->mpOwner = owner;
    self->mVtable = InvisiblePacketMember_vtable;
    return self;
}

/* 025E895C */
mDoExt_invisibleModel* mDoExt_invisibleModel_ct(mDoExt_invisibleModel* self) {
    if (!self) {
        self = static_cast<mDoExt_invisibleModel*>(operator_new(sizeof(mDoExt_invisibleModel)));
        if (!self) return nullptr;
    }
    self->mpPackets = nullptr;
    self->mModel = nullptr;
    return self;
}

/* 025E899C */
void mDoExt_invisibleModel_clear(mDoExt_invisibleModel* self) {
    InvisiblePacketArray* packets = self->mpPackets;
    self->mModel = nullptr;
    if (packets) gabi::call_ptr(packets->mpMethods->destroy.get(), packets, 3);
    self->mpPackets = nullptr;
}

/* 025E89F8 */
void mDoExt_invisibleModel_dt(mDoExt_invisibleModel* self, u32 flags) {
    if (!self) return;
    InvisibleModel_clear(self);
    if (flags & 1) operator_delete(self);
}

/* 025E8A48 */
u32 mDoExt_invisibleModel_create(mDoExt_invisibleModel* self, InvisibleModel* model) {
    InvisibleModelData* data = model->mpModelData;
    InvisibleJointRegistry* registry = InvisibleModelData_getJointRegistry(data);
    u32 count = registry->mJointCount;
    // HD: the owning packet wrapper is separate from its manager-allocated joint packet array.
    InvisiblePacketArray* packets = static_cast<InvisiblePacketArray*>(operator_new(sizeof(InvisiblePacketArray)));
    if (packets) {
        InvisiblePacketArray_construct(packets);
        InvisiblePacketMember_construct(&packets->mMember, &self->mpPackets);
        packets->mCount = 0;
        packets->mpPackets = nullptr;
        packets->mpMethods = &InvisiblePacketArray_vtable;
        packets->mMember.mVtable = InvisiblePacketOwner_vtable;
        if (count > 0) {
            InvisibleManager* manager = InvisibleRenderer_getManager(g_invisibleRenderer.mpRenderer);
            mDoExt_invJntPacket* buffer = gabi::call_ptr<mDoExt_invJntPacket*>(manager->mpMethods->allocate.get(), manager, count * sizeof(mDoExt_invJntPacket), 4);
            u32 iterations = count > 0 ? count : 1;
            mDoExt_invJntPacket* current = buffer;
            for (u32 i = 0; i < iterations; ++i, ++current) {
                if (current) {
                    InvisiblePacket_construct(current);
                    current->mVtable = InvisibleJointPacket_vtable;
                }
            }
            if (buffer) {
                packets->mpPackets = buffer;
                packets->mCount = count;
            }
        }
    }
    self->mpPackets = packets;
    if (!packets) return 0;
    self->mModel = model;
    mDoExt_invJntPacket* packet = self->mpPackets->mpPackets;
    u32 i = 0;
    registry = InvisibleModelData_getJointRegistry(data);
    while (i < registry->mJointCount) {
        packet->mJntNum = static_cast<u16>(i);
        packet->mModel = self->mModel;
        i = static_cast<u16>(i + 1);
        ++packet;
        registry = InvisibleModelData_getJointRegistry(data);
    }
    return 1;
}

static void invisibleEntry(mDoExt_invisibleModel* self, bool maskOff) {
    InvisibleModelData* data = self->mModel->mpModelData;
    mDoExt_invJntPacket* packet = self->mpPackets->mpPackets;
    InvisibleJointRegistry* registry = InvisibleModelData_getJointRegistry(data);
    u32 i = 0;
    gabi::Local<cXyz> position;
    while (i < registry->mJointCount) {
        // HD: each matrix component access marks the matrix buffer and reloads the model.
        InvisibleMatrixBuffer* matrices = self->mModel->mpMtxBuffer;
        Mtx34* base = matrices->mpAnmMtx;
        matrices->mFlags = matrices->mFlags | 0x10;
        f32 x = base[i].m[0][3];
        matrices = self->mModel->mpMtxBuffer;
        base = matrices->mpAnmMtx;
        matrices->mFlags = matrices->mFlags | 0x10;
        f32 y = base[i].m[1][3];
        matrices = self->mModel->mpMtxBuffer;
        base = matrices->mpAnmMtx;
        matrices->mFlags = matrices->mFlags | 0x10;
        f32 z = base[i].m[2][3];
        position.get()->y = y;
        position.get()->z = z;
        position.get()->x = x;
        InvisiblePlay* play = Invisible_getPlay();
        void* buffer = maskOff ? play->mpXluMaskOff.get() : play->mpXlu.get();
        InvisibleDrawList_entryZSort(play->mDrawList, buffer, packet, position.get());
        i = static_cast<u16>(i + 1);
        ++packet;
        registry = InvisibleModelData_getJointRegistry(data);
    }
}

/* 025E8BC4 */
void mDoExt_invisibleModel_entry(mDoExt_invisibleModel* self) { invisibleEntry(self, false); }

/* 025E8CD8 */
void mDoExt_invisibleModel_entryMaskOff(mDoExt_invisibleModel* self) { invisibleEntry(self, true); }

/* 025E8DEC */
void mDoExt_invisibleModel_updateDL(mDoExt_invisibleModel* self, J3DModel* model) {
    void* opa = g_invisibleDrawBuffers.mpOpa;
    void* xlu = g_invisibleDrawBuffers.mpXlu;
    InvisiblePlay* play = Invisible_getPlay();
    g_invisibleDrawBuffers.mpOpa = play->mpOpa;
    play = Invisible_getPlay();
    g_invisibleDrawBuffers.mpXlu = play->mpXlu;
    mDoExt_modelUpdateDL(model, 0);
    InvisibleModel_entry(self);
    g_invisibleDrawBuffers.mpOpa = opa;
    g_invisibleDrawBuffers.mpXlu = xlu;
}

/* 025E8E58 */
void mDoExt_invisibleModel_updateMorfDL(mDoExt_invisibleModel* self, void* morf) {
    void* opa = g_invisibleDrawBuffers.mpOpa;
    void* xlu = g_invisibleDrawBuffers.mpXlu;
    InvisiblePlay* play = Invisible_getPlay();
    g_invisibleDrawBuffers.mpOpa = play->mpOpa;
    play = Invisible_getPlay();
    g_invisibleDrawBuffers.mpXlu = play->mpXlu;
    InvisibleMorf_updateDL(morf);
    InvisibleModel_entry(self);
    g_invisibleDrawBuffers.mpOpa = opa;
    g_invisibleDrawBuffers.mpXlu = xlu;
}

/* 025E8EC0 */
void mDoExt_invisibleModel_entryImm(mDoExt_invisibleModel* self) {
    InvisibleModelData* data = self->mModel->mpModelData;
    mDoExt_invJntPacket* packet = self->mpPackets->mpPackets;
    InvisibleJointRegistry* registry = InvisibleModelData_getJointRegistry(data);
    u32 i = 0;
    while (i < registry->mJointCount) {
        InvisibleDrawBuffer_entryImm(g_invisibleDrawBuffers.mpOpa, packet, 0);
        i = static_cast<u16>(i + 1);
        ++packet;
        registry = InvisibleModelData_getJointRegistry(data);
    }
}

/* 025E8F58 */
mDoExt_3Dline_c* mDoExt_3Dline_ct(mDoExt_3Dline_c* self) {
    if (!self) {
        self = static_cast<mDoExt_3Dline_c*>(operator_new(sizeof(mDoExt_3Dline_c)));
        if (!self) return nullptr;
    }
    self->mPosArr = nullptr;
    self->mVtable = InvisibleLine_vtable;
    self->mpSegments = nullptr;
    self->mpSize = nullptr;
    return self;
}


/* WWHD glue
 data g_invisibleDefaultState 104B474C
 data g_invisibleRenderer 101F8B4C
 data g_invisibleDrawBuffers 104B4634
 data InvisiblePacketArray_vtable 1005874C
 ext 02750250 void InvisibleDrawState_construct(InvisibleDrawState*)
 ext 02750370 void InvisibleDrawState_apply(InvisibleDrawState*)
 ext 02752A84 void InvisiblePacketArray_construct(InvisiblePacketArray*)
 ext 027F1278 void InvisiblePacket_construct(mDoExt_invJntPacket*)
 ext 027F3F94 InvisibleJointRegistry* InvisibleModelData_getJointRegistry(InvisibleModelData*)
 ext 02756140 InvisibleManager* InvisibleRenderer_getManager(void*)
 ext 027F0E04 void InvisibleDrawBuffer_entryImm(void*, mDoExt_invJntPacket*, u32)
 ext 0252F3B0 void InvisibleDrawList_entryZSort(void*, void*, mDoExt_invJntPacket*, cXyz*)
 ext 025200D4 InvisiblePlay* Invisible_getPlay()
 ext 025E54D8 void InvisibleMorf_updateDL(void*)
 ext 025E8BC4 void InvisibleModel_entry(mDoExt_invisibleModel*)
 ext 025E899C void InvisibleModel_clear(mDoExt_invisibleModel*)
 ext 025E890C InvisiblePacketMember* InvisiblePacketMember_construct(InvisiblePacketMember*, void*)
*/
