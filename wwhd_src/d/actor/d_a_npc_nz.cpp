/* Rat shopkeeper (WWHD). Local game- */
#include "d/actor/d_a_npc_nz.h"

BOOL nzCreateHeap(daNpc_Nz_c*);
void nzSetAnm(daNpc_Nz_c*, s32, u32);
BOOL nzCreateInit(daNpc_Nz_c*);
void nzLookBack(daNpc_Nz_c*);
void nzTailControl(daNpc_Nz_c*);
void nzSetMtx(daNpc_Nz_c*);

static u32 nzEventManager() { return gabi::call<u32>(0x025200D4) + 0x52C4; }
static void nzCutEnd(s32 staff) { u32 manager = nzEventManager(); gabi::call(0x02543280, manager, staff); }
static void nzMemberCall(daNpc_Nz_c* self, u32 member) {
    u32 adjusted = gabi::ea(self) + (s32)gabi::load<s16>(member);
    s16 index = gabi::load<s16>(member + 2);
    u32 target;
    if (index < 0) target = gabi::load<u32>(member + 4);
    else {
        u32 vt = gabi::load<u32>(adjusted + (s32)gabi::load<s16>(member + 6));
        target = gabi::load<u32>(vt + (s32)index * 8 + 4);
    }
    gabi::call_ptr(target, adjusted);
}

void nzModeProc(daNpc_Nz_c* self, s32 proc, s32 mode) {
    WWHD_FUNC(0x022A3574, void, self, proc, mode);
    if (proc == 0) { self->mCurMode = mode; nzMemberCall(self, 0x1001EE28 + (u32)mode * 20); }
    else if (proc == 1) nzMemberCall(self, 0x1001EE30 + (u32)(s32)self->mCurMode * 20);
}
VERIFY(0x022A3574, nzModeProc);

void* nzGetShopItem(daNpc_Nz_c* self, s32 slot) {
    WWHD_FUNC(0x022A3620, void*, self, slot);
    gabi::Local<be<u32>> id;
    *id = self->mShopItemId[slot == 0 ? 0 : 1];
    if ((u32)*id == 0xFFFFFFFF) return nullptr;
    return gabi::call<void*>(0x025D5218, 0x025E1234u, id.get());
}
VERIFY(0x022A3620, nzGetShopItem);

void nzDeleteShopItem(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A4B04, void, self);
    for (int slot = 0; slot < 2; ++slot) {
        if ((u32)self->mShopItemId[slot] != 0xFFFFFFFF) {
            void* item = nzGetShopItem(self, slot);
            if (item) { gabi::call(0x025D57E0, item); self->mShopItemId[slot] = 0xFFFFFFFF; }
        }
    }
}
VERIFY(0x022A4B04, nzDeleteShopItem);

s16 nzXyEvent(daNpc_Nz_c* self, s32 item) {
    WWHD_FUNC(0x022A2EA4, s16, self, item);
    u32 manager = nzEventManager();
    s16 event = gabi::call<s16>(0x02543F10, manager, 0x1001EC8Cu, 0xFF);
    self->mEventId = event;
    return event;
}
VERIFY(0x022A2EA4, nzXyEvent);
s16 nzXyEventCallback(daNpc_Nz_c* self, s32 item) {
    WWHD_FUNC(0x022A2EE8, s16, self, item); return nzXyEvent(self, item);
}
VERIFY(0x022A2EE8, nzXyEventCallback);

void* nzDeleteRatAndBomb(fopAc_ac_c* actor, void* context) {
    WWHD_FUNC(0x022A30A4, void*, actor, context);
    if (actor) {
        s16 name = gabi::load<s16>(gabi::ea(actor) + 8);
        if (name == 0xC6 || name == 0x126) gabi::call(0x025D57E0, actor);
    }
    return nullptr;
}
VERIFY(0x022A30A4, nzDeleteRatAndBomb);

s32 nzHideShopItem(void* item) {
    WWHD_FUNC(0x022A3474, s32, item);
    gabi::call(0x021842B8, item);
    gabi::store<u32>(gabi::ea(item) + 0x790, 0);
    return 4;
}
VERIFY(0x022A3474, nzHideShopItem);

void nzSetAttention(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A418C, void, self);
    u32 x = gabi::load<u32>(gabi::ea(&self->current.pos.x));
    u32 z = gabi::load<u32>(gabi::ea(&self->current.pos.z));
    gabi::store<u32>(gabi::ea(self) + 0x390, x);
    f32 y = self->eyePos.y;
    gabi::store<u32>(gabi::ea(self) + 0x398, z);
    gabi::store<f32>(gabi::ea(self) + 0x394, y);
}
VERIFY(0x022A418C, nzSetAttention);

void nzModeWait(daNpc_Nz_c* self) { WWHD_FUNC(0x022A498C, void, self); self->mTalkState = 2; }
VERIFY(0x022A498C, nzModeWait);
void nzCutEatStart(daNpc_Nz_c* self) { WWHD_FUNC(0x022A5424, void, self); nzSetAnm(self, 6, 0); }
VERIFY(0x022A5424, nzCutEatStart);
void nzCutShowStart(daNpc_Nz_c* self) { WWHD_FUNC(0x022A5918, void, self); self->mVisible = 1; }
VERIFY(0x022A5918, nzCutShowStart);
void nzCutHideStart(daNpc_Nz_c* self) { WWHD_FUNC(0x022A595C, void, self); self->mVisible = 0; }
VERIFY(0x022A595C, nzCutHideStart);

BOOL nzCreateHeapCallback(daNpc_Nz_c* self) { WWHD_FUNC(0x022A2D68, BOOL, self); return nzCreateHeap(self); }
VERIFY(0x022A2D68, nzCreateHeapCallback);
void nzModeWaitInit(daNpc_Nz_c* self) { WWHD_FUNC(0x022A5F50, void, self); }
VERIFY(0x022A5F50, nzModeWaitInit);
void nzModeEventEatInit(daNpc_Nz_c* self) { WWHD_FUNC(0x022A5F54, void, self); }
VERIFY(0x022A5F54, nzModeEventEatInit);
void nzCutGoHomeStart(daNpc_Nz_c* self) { WWHD_FUNC(0x022A5F58, void, self); }
VERIFY(0x022A5F58, nzCutGoHomeStart);
void nzCutKillAllProc(daNpc_Nz_c* self) { WWHD_FUNC(0x022A58E0, void, self); s32 staff = self->mStaffId; nzCutEnd(staff); }
VERIFY(0x022A58E0, nzCutKillAllProc);
void nzCutShowProc(daNpc_Nz_c* self) { WWHD_FUNC(0x022A5924, void, self); s32 staff = self->mStaffId; nzCutEnd(staff); }
VERIFY(0x022A5924, nzCutShowProc);
void nzCutHideProc(daNpc_Nz_c* self) { WWHD_FUNC(0x022A5968, void, self); s32 staff = self->mStaffId; nzCutEnd(staff); }
VERIFY(0x022A5968, nzCutHideProc);
void nzCutCheckItemProc(daNpc_Nz_c* self) { WWHD_FUNC(0x022A5DB4, void, self); s32 staff = self->mStaffId; nzCutEnd(staff); }
VERIFY(0x022A5DB4, nzCutCheckItemProc);

BOOL nzIsDelete(daNpc_Nz_c* self) { WWHD_FUNC(0x022A5F48, BOOL, self); return 1; }
VERIFY(0x022A5F48, nzIsDelete);
void nzHioMessage(daNpc_Nz_HIO_c* self, void* context) { WWHD_FUNC(0x022A6034, void, self, context); }
VERIFY(0x022A6034, nzHioMessage);
void nzHioDelete(daNpc_Nz_HIO_c* self, u32 flags) {
    WWHD_FUNC(0x022A5F34, void, self, flags);
    if (self && (flags & 1)) gabi::call(0x0273AF40, self);
}
VERIFY(0x022A5F34, nzHioDelete);

daNpc_Nz_HIO_c* nzHioConstruct(daNpc_Nz_HIO_c* self) {
    WWHD_FUNC(0x022A5DEC, daNpc_Nz_HIO_c*, self);
    if (!self) self = gabi::call<daNpc_Nz_HIO_c*>(0x0273AD10, 0x28);
    if (self) {
        self->mBackboneMaxX = -8000;
        self->mSearchRange = 250.0f;
        self->mHeadMaxY = 8000;
        self->mNo = -1;
        self->mVtable = 0x1001EC04;
        self->mBackboneMinY = -3000;
        self->mBaitRange = 250.0f;
        self->mHeadMaxX = 8000;
        self->mRange = 100.0f;
        self->mBackboneMaxY = -8000;
        self->mBackboneMinX = 0;
        self->mHeadMinY = 8000;
        self->mTalkingLookVelocity = 1600;
        self->mDebug = 0;
        self->mLookVelocity = 2000;
        self->mHeadMinX = 0;
    }
    return self;
}
VERIFY(0x022A5DEC, nzHioConstruct);

void nzSetAnm(daNpc_Nz_c* self, s32 animation, u32 force) {
    WWHD_FUNC(0x022A34AC, void, self, animation, force);
    s32 selected = animation;
    if (selected == 15) selected = self->mAnimation;
    else self->mAnimation = animation;
    self->m_jnt.mbHeadLock = selected != 0;
    self->m_jnt.mbBackBoneLock = selected != 0;
    mDoExt_McaMorf* morf = self->mRatMorf;
    gabi::call(0x025877F8, 0x1001F034u, morf, &self->mPreviousAnimation,
               &self->mAnimation, &self->mAnimationState, 0x1001ECC0u, 0x1001ED38u, force, 0);
}
VERIFY(0x022A34AC, nzSetAnm);

static u32 nzAnimationMatrix(u32 model, s32 joint) {
    u32 table = gabi::load<u32>(model + 0x2C);
    u32 matrices = gabi::load<u32>(table + 0x10);
    u16 flags = gabi::load<u16>(table + 4);
    gabi::store<u16>(table + 4, flags | 0x10);
    return matrices + (u32)joint * 48;
}
static void nzCopyMatrix(u32 source, u32 dest) {
    f32 values[12];
    for (int i = 0; i < 12; ++i) values[i] = gabi::load<f32>(source + i * 4);
    for (int i = 0; i < 12; ++i) gabi::store<f32>(dest + i * 4, values[i]);
}
BOOL nzNodeCallback(daNpc_Nz_c* self, void* node, s32 timing) {
    WWHD_FUNC(0x022A2838, BOOL, self, node, timing);
    if (timing == 0) {
        u32 model = gabi::load<u32>(0x104B462C);
        u32 jointData = gabi::call<u32>(0x027F7878, node);
        u16 joint = gabi::load<u16>(jointData + 4);
        u32 matrix = nzAnimationMatrix(model, joint);
        gabi::call(0x028E90D4, matrix, 0x1048D0CCu);
        if ((u32)joint == (u32)(s32)(s8)self->m_jnt.mHeadJntNum) {
            gabi::Local<cXyz> origin; origin->set(0.0f, 0.0f, 0.0f);
            gabi::call(0x025F1C28, 0x1048D0CCu, (s16)self->m_jnt.mAngles[0][1]);
            gabi::call(0x025F1C5C, 0x1048D0CCu, (s16)-(s16)self->m_jnt.mAngles[0][0]);
            gabi::call(0x028E8F64, 0x1048D0CCu, origin.get(), &self->eyePos);
        }
        if ((u32)joint == (u32)(s32)(s8)self->m_jnt.mBackboneJntNum) {
            gabi::call(0x025F1BF4, 0x1048D0CCu, (s16)self->m_jnt.mAngles[1][1]);
            gabi::call(0x025F1C5C, 0x1048D0CCu, (s16)-(s16)self->m_jnt.mAngles[1][0]);
        }
        u32 output = nzAnimationMatrix(model, joint);
        nzCopyMatrix(0x1048D0CC, output);
        gabi::call(0x028E90D4, 0x1048D0CCu, 0x104B4868u);
    }
    return 1;
}
VERIFY(0x022A2838, nzNodeCallback);
BOOL nzJointCallback(void* node, s32 timing) {
    WWHD_FUNC(0x022A29B8, BOOL, node, timing);
    u32 model = gabi::load<u32>(0x104B462C);
    return nzNodeCallback(gabi::at<daNpc_Nz_c>(gabi::load<u32>(model + 0xB8)), node, timing);
}
VERIFY(0x022A29B8, nzJointCallback);
BOOL nzTailNodeCallback(daNpc_Nz_c* self, void* node, s32 timing) {
    WWHD_FUNC(0x022A29D0, BOOL, self, node, timing);
    if (timing == 0) {
        u32 jointData = gabi::call<u32>(0x027F7878, node);
        u16 joint = gabi::load<u16>(jointData + 4);
        u32 model = gabi::load<u32>(0x104B462C);
        if (!gabi::load<u32>(0x10467D2C)) {
            f32 z = gabi::load<f32>(0x101FFBB0);
            f32 x = gabi::load<f32>(0x101FFBA8);
            gabi::store<f32>(0x10467D28, z);
            gabi::store<f32>(0x10467D20, x);
            f32 y = gabi::load<f32>(0x101FFBAC);
            gabi::store<u32>(0x10467D2C, 1);
            gabi::store<f32>(0x10467D24, y);
        }
        u32 matrix = nzAnimationMatrix(model, joint);
        gabi::call(0x028E90D4, matrix, 0x1048D0CCu);
        gabi::call(0x028E8F64, 0x1048D0CCu, 0x10467D20u, &self->mTailAnchors[joint == 9 ? 1 : 0]);
    }
    return 1;
}
VERIFY(0x022A29D0, nzTailNodeCallback);
BOOL nzTailJointCallback(void* node, s32 timing) {
    WWHD_FUNC(0x022A2AC0, BOOL, node, timing);
    u32 model = gabi::load<u32>(0x104B462C);
    return nzTailNodeCallback(gabi::at<daNpc_Nz_c>(gabi::load<u32>(model + 0xB8)), node, timing);
}
VERIFY(0x022A2AC0, nzTailJointCallback);

void nzCheckOrder(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A400C, void, self);
    if (gabi::load<u16>(gabi::ea(self) + 0xF8) == 1) {
        u8 state = self->mTalkState;
        if (state == 2) nzModeProc(self, 0, 1);
        else if (state != 1) return;
        self->mTalkState = 0;
    }
}
VERIFY(0x022A400C, nzCheckOrder);
void nzEventOrder(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A415C, void, self);
    u8 state = self->mTalkState;
    if (state == 1 || state == 2) {
        u32 flags = gabi::load<u16>(gabi::ea(self) + 0xFA);
        u8 currentState = self->mTalkState;
        gabi::store<u16>(gabi::ea(self) + 0xFA, flags | 0x21);
        if (currentState == 1) gabi::call(0x025D76A8, self);
    }
}
VERIFY(0x022A415C, nzEventOrder);
BOOL nzDelete(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A3FA8, BOOL, self);
    if (gabi::load<u32>(gabi::ea(self->mSmokeCallback) + 4)) gabi::call(0x025A5F88, self->mSmokeCallback);
    gabi::call(0x025204C8, &self->mPhase, 0x1001F034u);
    gabi::call(0x025204C8, &self->mModelPhase, 0x1001F02Cu);
    return 1;
}
VERIFY(0x022A3FA8, nzDelete);
BOOL nzDeleteCallback(daNpc_Nz_c* self) { WWHD_FUNC(0x022A4008, BOOL, self); return nzDelete(self); }
VERIFY(0x022A4008, nzDeleteCallback);
void nzDestructor(daNpc_Nz_c* self, u32 flags) {
    WWHD_FUNC(0x022A5F5C, void, self, flags);
    if (!self) return;
    gabi::call(0x025EB8B8, self->mTailMaterial, 2);
    gabi::call(0x02018034, gabi::ea(self) + 0x9E0, 2);
    gabi::store<u32>(gabi::ea(self) + 0x828, 0x1001EBE4);
    gabi::store<u32>(gabi::ea(self) + 0x81C, 0x1001EBF4);
    gabi::call(0x024EFD9C, &self->mRatAcch, 0);
    gabi::call(0x02515A70, &self->mCyl, 2);
    gabi::call(0x02515860, &self->mStts, 2);
    gabi::call(0x02018034, gabi::ea(self) + 0x628, 2);
    gabi::store<u32>(gabi::ea(self) + 0x470, 0x1001EBE4);
    gabi::store<u32>(gabi::ea(self) + 0x464, 0x1001EBF4);
    gabi::call(0x024EFD9C, &self->mObjAcch, 0);
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1) gabi::call(0x0273AF40, self);
}
VERIFY(0x022A5F5C, nzDestructor);

BOOL nzCreate(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A3E74, BOOL, self);
    u32 condition = self->actor_condition;
    if (!(condition & 8)) {
        if (self) {
            gabi::call(0x025A1458, self);
            self->__vtbl = 0x1001F038;
            gabi::call(0x024F0474, &self->mRatAcch);
            gabi::store<u32>(gabi::ea(self) + 0x818, 0x1001EBD4);
            gabi::store<u32>(gabi::ea(self) + 0x828, 0x1001EBE4);
            gabi::store<u32>(gabi::ea(self) + 0x81C, 0x1001EBF4);
            gabi::store<u8>(gabi::ea(self) + 0x820, 1);
            gabi::call(0x024EFE94, &self->mRatAcchCir);
            gabi::call(0x025A5B18, self->mSmokeCallback, 1);
            gabi::call(0x025EB82C, self->mTailMaterial);
            condition = self->actor_condition;
        }
        self->actor_condition = condition | 8;
    }
    s32 phase = gabi::call<s32>(0x02520460, &self->mPhase, 0x1001F034u);
    if (phase != 4) return phase;
    phase = gabi::call<s32>(0x02520460, &self->mModelPhase, 0x1001F02Cu);
    if (phase != 4) return phase;
    if (!gabi::call<BOOL>(0x025D63E8, self, 0x022A2D68u, 0x2FE0)) return 5;
    if (!nzCreateInit(self)) return 5;
    return 4;
}
VERIFY(0x022A3E74, nzCreate);
BOOL nzCreateCallback(daNpc_Nz_c* self) { WWHD_FUNC(0x022A3FA4, BOOL, self); return nzCreate(self); }
VERIFY(0x022A3FA4, nzCreateCallback);
void nzCutEatFirstStart(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A55AC, void, self);
    gabi::call(0x025D5218, 0x022A3094u, self);
    s16 angle = self->home.angle.y;
    self->current.angle.y = angle;
    self->shape_angle.y = angle;
    nzSetAnm(self, 10, 0);
}
VERIFY(0x022A55AC, nzCutEatFirstStart);
void nzCutCheckItemStart(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A5D60, void, self);
    if ((u8)self->mTalkState >= 3) {
        u32 play = gabi::call<u32>(0x025200D4);
        u32 player = gabi::load<u32>(play + 0x5B2C);
        s16 event = self->mEventId;
        gabi::call(0x025D7970, player, self, event, 0, 0xFFFF);
    }
}
VERIFY(0x022A5D60, nzCutCheckItemStart);
void nzStaticInit() {
    WWHD_FUNC(0x022A5E94, void);
    gabi::store<u32>(0x10467D18, 0); gabi::store<u32>(0x10467D10, 0);
    gabi::store<u32>(0x10467D1C, 0); gabi::store<u32>(0x10467D14, 0);
    gabi::call(0x028F026C, 0x101C2594u);
    gabi::store<f32>(0x10467CDC, -3.1415927410125732f);
    gabi::store<f32>(0x10467CE0, 3.1415927410125732f);
    gabi::call(0x028ED6F8, 0x10467CE4u);
    gabi::call(0x028F026C, 0x101C25A0u);
    gabi::call(0x028EAB2C, 0x10467CE5u);
    gabi::call(0x028F026C, 0x101C25ACu);
    nzHioConstruct(gabi::at<daNpc_Nz_HIO_c>(0x10467CE8));
}
VERIFY(0x022A5E94, nzStaticInit);

static u32 nzResource(u32 archive, s32 index) {
    gabi::Local<SafeString> key;
    key->mStringTop = archive; key->__vtbl = 0x1001EB6C;
    u32 control = gabi::load<u32>(0x101F4F28);
    return gabi::call<u32>(0x026066C4, control, key.get(), index);
}
static u32 nzJointNames(u32 modelData) {
    u32 block = gabi::call<u32>(0x027F68FC, modelData);
    u32 offset = gabi::load<u32>(block + 0x10);
    return offset ? block + 0x10 + offset : 0;
}
static u32 nzJointNode(u32 modelData, u32 joint) {
    u32 count = gabi::load<u32>(modelData + 4);
    u32 nodes = gabi::load<u32>(modelData + 8);
    return joint < count ? nodes + joint * 28 : nodes;
}
BOOL nzCreateHeap(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A2AD8, BOOL, self);
    u32 data = nzResource(0x1001F02C, 3);
    if (!data) gabi::call(0x0273AA24, 0x1001EC2Cu, 211, 0x1001EC3Cu);
    mDoExt_McaMorf* morf = gabi::call<mDoExt_McaMorf*>(0x025E4F64,
        0, data, 0, 0, 0, -1, 1.0f, 0, -1, 1, 0, 0x80000, 0x37441422);
    self->mRatMorf = morf;
    if (!morf || !gabi::load<u32>(gabi::ea(morf) + 0x90)) return 0;
    u32 model = gabi::load<u32>(gabi::ea(morf) + 0x90);
    gabi::store<u32>(model + 0xB8, gabi::ea(self));
    u32 names = nzJointNames(data);
    s8 head = gabi::call<s8>(0x027DF9B0, names, 0x1001EC1Cu);
    self->m_jnt.mHeadJntNum = head;
    if (head < 0) gabi::call(0x0273AA24, 0x1001EC2Cu, 222, 0x1001EC50u);
    names = nzJointNames(data);
    s8 backbone = gabi::call<s8>(0x027DF9B0, names, 0x1001EC24u);
    self->m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0) gabi::call(0x0273AA24, 0x1001EC2Cu, 225, 0x1001EC6Cu);
    u32 jointData = gabi::call<u32>(0x027F3F94, data);
    u16 count = gabi::load<u16>(jointData + 8);
    for (u16 joint = 0; joint < count; ++joint) {
        if ((u32)joint == (u32)(s32)(s8)self->m_jnt.mHeadJntNum ||
            (u32)joint == (u32)(s32)(s8)self->m_jnt.mBackboneJntNum)
            gabi::store<u32>(nzJointNode(data, joint) + 8, 0x022A29B8);
        if (joint == 1 || joint == 9) gabi::store<u32>(nzJointNode(data, joint) + 8, 0x022A2AC0);
        jointData = gabi::call<u32>(0x027F3F94, data);
        count = gabi::load<u16>(jointData + 8);
    }
    u32 texture = nzResource(0x1001F034, 46);
    return gabi::call<BOOL>(0x025EBA58, self->mTailMaterial, 1, 10, texture, 0) != 0;
}
VERIFY(0x022A2AD8, nzCreateHeap);

BOOL nzCreateInit(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A3C88, BOOL, self);
    nzSetAnm(self, 0, 0);
    nzModeProc(self, 0, 0);
    gabi::call(0x024EFF44, &self->mRatAcchCir, 30.0f, 10.0f);
    gabi::call(0x024F06B4, &self->mRatAcch, &self->current.pos, &self->old.pos,
               self, 1, &self->mRatAcchCir, &self->speed, 0, 0);
    u32 flags = gabi::load<u32>(gabi::ea(&self->mRatAcch) + 0x28);
    gabi::store<u32>(gabi::ea(&self->mRatAcch) + 0x28, flags | 0x400C);
    self->gravity = 0.0f;
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x024F08A8, &self->mRatAcch, play + 0x12A0);
    nzSetMtx(self);
    mDoExt_McaMorf* morf = self->mRatMorf;
    gabi::call(0x025E55A0, morf);
    morf = self->mRatMorf;
    u32 model = gabi::load<u32>(gabi::ea(morf) + 0x90);
    self->cullMtx = model ? model + 0xC8 : 0;
    gabi::call(0x025D674C, self, -450.0f, -50.0f, -450.0f, 450.0f, 750.0f, 450.0f);
    self->cullSizeFar = 10.0f;
    gabi::store<u8>(gabi::ea(self) + 0x38B, 0xB1);
    gabi::store<u8>(gabi::ea(self) + 0x389, 0xB1);
    gabi::store<u32>(gabi::ea(self) + 0x39C, 0x02000008);
    gabi::store<u32>(gabi::ea(self) + 0x100, 0x022A2EE8);
    gabi::store<u32>(gabi::ea(self) + 0x104, 0x022A2D6C);
    gabi::call(0x0259F814, &self->mEventCut, 0x1001EEA8u, self);
    u32 x = gabi::load<u32>(gabi::ea(&self->home.pos.x));
    u32 y = gabi::load<u32>(gabi::ea(&self->home.pos.y));
    gabi::store<u32>(gabi::ea(&self->mShopOrigin.x), x);
    u16 angle = self->home.angle.y;
    u32 z = gabi::load<u32>(gabi::ea(&self->home.pos.z));
    gabi::store<u32>(gabi::ea(&self->mShopOrigin.y), y);
    self->mTalkState = 0;
    gabi::store<u32>(gabi::ea(&self->mShopOrigin.z), z);
    self->mVisible = 0;
    self->mAnimationLoops = 0;
    u32 trig = 0x104A44F8 + ((u32)angle >> 3) * 8;
    f32 homeX = self->home.pos.x;
    f32 sin = gabi::load<f32>(trig);
    f32 homeY = self->home.pos.y;
    homeX = gabi::fnmsubs(50.0f, sin, homeX);
    f32 homeZ = self->home.pos.z;
    self->home.pos.x = homeX;
    f32 cos = gabi::load<f32>(trig + 4);
    self->mShopItemId[0] = 0xFFFFFFFF;
    homeZ = gabi::fnmsubs(50.0f, cos, homeZ);
    self->current.pos.x = homeX;
    self->current.pos.y = homeY;
    self->home.pos.z = homeZ;
    self->mShopItemId[1] = 0xFFFFFFFF;
    self->current.pos.z = homeZ;
    return 1;
}
VERIFY(0x022A3C88, nzCreateInit);

void nzCreateShopItem(daNpc_Nz_c* self, u32 first, u32 second) {
    WWHD_FUNC(0x022A4C64, void, self, first, second);
    if ((u32)self->mShopItemId[0] == 0xFFFFFFFF) {
        s8 room = self->current.roomNo;
        self->mShopItemId[0] = gabi::call<u32>(0x025D87E4, &self->current.pos, first, &self->current.angle, room, 0, 0x022A3474u);
    }
    if ((u32)self->mShopItemId[1] == 0xFFFFFFFF) {
        s8 room = self->current.roomNo;
        self->mShopItemId[1] = gabi::call<u32>(0x025D87E4, &self->current.pos, second, &self->current.angle, room, 0, 0x022A3474u);
    }
}
VERIFY(0x022A4C64, nzCreateShopItem);

void nzCutProc(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A4998, void, self);
    u32 name = self->mEventCut.mpEvtStaffName;
    u32 manager = nzEventManager();
    s32 staff = gabi::call<s32>(0x02542D88, manager, name, 0, 0);
    self->mStaffId = staff;
    if (staff == -1) return;
    manager = nzEventManager();
    s32 action = gabi::call<s32>(0x02542EDC, manager, staff, 0x101C2574u, 8, 1, 0);
    staff = self->mStaffId;
    if (action == -1) { nzCutEnd(staff); return; }
    manager = nzEventManager();
    u32 advance = gabi::call<u32>(0x025447C8, manager, staff);
    u32 member = 0x1001EEE8 + (u32)action * 16;
    if (advance) nzMemberCall(self, member);
    nzMemberCall(self, member + 8);
}
VERIFY(0x022A4998, nzCutProc);

void nzModeEventEat(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A4B84, void, self);
    if (!gabi::call<BOOL>(0x0259F858, &self->mEventCut)) nzCutProc(self);
    s16 event = self->mEventId;
    u32 manager = nzEventManager();
    if (!gabi::call<BOOL>(0x025440C8, manager, event)) return;
    u32 play = gabi::call<u32>(0x025200D4);
    u16 flags = gabi::load<u16>(play + 0x52B8);
    gabi::store<u16>(play + 0x52B8, flags | 8);
    self->mEventId = -1;
    nzDeleteShopItem(self);
    u32 x = gabi::load<u32>(gabi::ea(&self->home.pos.x));
    u16 ax = self->home.angle.x;
    gabi::store<u32>(gabi::ea(&self->old.pos.x), x);
    u32 z = gabi::load<u32>(gabi::ea(&self->home.pos.z));
    self->current.angle.x = ax;
    gabi::store<u32>(gabi::ea(&self->current.pos.z), z);
    u32 y = gabi::load<u32>(gabi::ea(&self->home.pos.y));
    self->shape_angle.x = ax;
    gabi::store<u32>(gabi::ea(&self->current.pos.y), y);
    self->mVisible = 0;
    u16 ay = self->home.angle.y;
    gabi::store<u32>(gabi::ea(&self->old.pos.y), y);
    self->shape_angle.y = ay;
    u16 az = self->home.angle.z;
    self->current.angle.y = ay;
    self->shape_angle.z = az;
    gabi::store<u32>(gabi::ea(&self->old.pos.z), z);
    self->current.angle.z = az;
    gabi::store<u32>(gabi::ea(&self->current.pos.x), x);
    nzModeProc(self, 0, 0);
}
VERIFY(0x022A4B84, nzModeEventEat);

static f32 nzHorizontalDistance(cXyz* delta, cXyz* horizontal) {
    horizontal->x = (f32)delta->x; horizontal->y = 0.0f; horizontal->z = (f32)delta->z;
    f32 square = gabi::call<f32>(0x028E8DD0, horizontal);
    return gabi::call<f32>(0x028F4384, square);
}
BOOL nzXyCheck(daNpc_Nz_c* self, s32 slot) {
    WWHD_FUNC(0x022A2D6C, BOOL, self, slot);
    u32 play = gabi::call<u32>(0x025200D4);
    if (gabi::load<u8>(play + (u32)slot + 0x5BBB) != 0x82) return 0;
    f32 offset = gabi::load<f32>(0x10467D04);
    play = gabi::call<u32>(0x025200D4);
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(play + 0x5B2C));
    gabi::Local<cXyz> position, delta, horizontal;
    f32 x = player->current.pos.x, y = player->current.pos.y, z = player->current.pos.z;
    position->set(x, y, z);
    u16 angle = player->current.angle.y;
    f32 sin = gabi::load<f32>(0x104A44F8 + ((u32)angle >> 3) * 8);
    position->x = gabi::fmadds(offset, sin, x);
    angle = player->current.angle.y;
    f32 cos = gabi::load<f32>(0x104A44FC + ((u32)angle >> 3) * 8);
    position->z = gabi::fmadds(offset, cos, z);
    gabi::call(0x0201ADE0, &self->mShopOrigin, delta.get(), position.get());
    f32 distance = nzHorizontalDistance(delta.get(), horizontal.get());
    return distance < gabi::load<f32>(0x10467D08);
}
VERIFY(0x022A2D6C, nzXyCheck);

static BOOL nzFrameEnd(mDoExt_McaMorf* morf) {
    f32 frame = (f32)gabi::load<s16>(gabi::ea(morf) + 0xA2) - 1.0f;
    return gabi::call<BOOL>(0x027F2BF8, gabi::ea(morf) + 0x98, frame);
}
static void nzVoice(daNpc_Nz_c* self, u32 sound) {
    s8 room = self->current.roomNo;
    s8 reverb = gabi::call<s8>(0x02520540, room);
    gabi::call(0x025E1A40, sound, &self->eyePos, 0, reverb);
}
void nzCutEatProc(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A5430, void, self);
    gabi::call(0x025D5218, 0x022A3094u, self);
    mDoExt_McaMorf* morf = self->mRatMorf;
    f32 frame = (f32)gabi::load<s16>(gabi::ea(morf) + 0xA2) - 1.0f;
    fopAc_ac_c* bait = self->mBait;
    if (gabi::call<BOOL>(0x027F2BF8, gabi::ea(morf) + 0x98, frame)) {
        f32 random = gabi::call<f32>(0x02019918, 4.0f);
        s16 homeAngle = self->home.angle.y;
        s16 angle = (s16)((u32)homeAngle + ((u32)gabi::ftoi(random) << 11));
        self->shape_angle.y = angle; self->current.angle.y = angle;
    }
    if (bait) {
        gabi::call(0x025D57E0, bait);
        self->mBait = nullptr; self->mBaitDistance = 3.4028234663852886e+38f;
        return;
    }
    morf = self->mRatMorf;
    if (nzFrameEnd(morf)) {
        s16 angle = self->home.angle.y;
        s32 staff = self->mStaffId;
        self->shape_angle.y = angle; self->current.angle.y = angle;
        nzCutEnd(staff);
    }
}
VERIFY(0x022A5430, nzCutEatProc);

void nzAnimationAttribute(daNpc_Nz_c* self, u16 attribute) {
    WWHD_FUNC(0x022A513C, void, self, attribute);
    s8 animation = self->mAnimation;
    if (animation == 9) {
        mDoExt_McaMorf* morf = self->mRatMorf;
        if (nzFrameEnd(morf)) nzSetAnm(self, 0, 0);
        else {
            animation = self->mAnimation;
            if (animation == 12 && nzFrameEnd(self->mRatMorf)) nzSetAnm(self, 11, 0);
        }
    } else if (animation == 12 && nzFrameEnd(self->mRatMorf)) nzSetAnm(self, 11, 0);
    u32 message = gabi::load<u32>(0x101F4B5C);
    u32 play = gabi::call<u32>(0x025200D4);
    if (!gabi::load<u8>(play + 0x5BD2)) return;
    if (gabi::call<s32>(0x025F795C, message) == 0x11) return;
    if (gabi::call<s32>(0x025F795C, message) == 0x12) return;
    if (gabi::call<s32>(0x025F795C, message) == 0x10) return;
    u32 number = gabi::load<u32>(message + 0x938);
    if (number < 0x33F7 || number > 0x3407) return;
    if (number <= 0x33FA || number > 0x3402) {
        void* item = nzGetShopItem(self, (u8)self->mSelectedItemSlot);
        if (item) gabi::call(0x021842B8, item);
        nzSetAnm(self, 0, 0);
    } else {
        void* item = nzGetShopItem(self, (u8)self->mSelectedItemSlot);
        if (item) gabi::call(0x021842C8, item);
        self->mPurchaseCount = 0;
        nzVoice(self, 0x4923);
        nzSetAnm(self, 12, 0);
    }
}
VERIFY(0x022A513C, nzAnimationAttribute);

void nzCutGoHomeProc(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A5C54, void, self);
    s16 target = gabi::call<s16>(0x0200F93C, &self->current.pos, &self->home.pos);
    gabi::call(0x0200F428, &self->current.angle.y, target, 4, 0x1000);
    s16 angle = self->current.angle.y;
    self->shape_angle.y = angle;
    s16 distance = gabi::call<s16>(0x0200FAAC, target, angle);
    if (distance < 0x4000) {
        if ((s8)self->mAnimation != 10) nzVoice(self, 0x4924);
        nzSetAnm(self, 10, 0);
        f32 remaining = gabi::call<f32>(0x0200EF78, &self->current.pos, &self->home.pos, 0.5f, 10.0f, 1.0f);
        if (remaining < 1.0f) { s32 staff = self->mStaffId; nzCutEnd(staff); }
    }
}
VERIFY(0x022A5C54, nzCutGoHomeProc);

void nzCutSetAnimationStart(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A59A0, void, self);
    s32 staff = self->mStaffId;
    u32 manager = nzEventManager();
    u32 animationPointer = gabi::call<u32>(0x0254487C, manager, staff, 0x1001EFFCu, 3);
    staff = self->mStaffId;
    manager = nzEventManager();
    u32 loopsPointer = gabi::call<u32>(0x0254487C, manager, staff, 0x1001F004u, 3);
    s32 animation = animationPointer ? gabi::load<s32>(animationPointer) : 0;
    s16 loops = loopsPointer ? gabi::load<s16>(loopsPointer + 2) : 0;
    self->mAnimationLoops = loops;
    nzSetAnm(self, (s8)animation, 0);
    if (animation == 8) nzVoice(self, 0x4923);
}
VERIFY(0x022A59A0, nzCutSetAnimationStart);

void nzSetMtx(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A369C, void, self);
    mDoExt_McaMorf* morf = self->mRatMorf;
    f32 sz = self->scale.z;
    u32 model = gabi::load<u32>(gabi::ea(morf) + 0x90);
    f32 sx = self->scale.x, sy = self->scale.y;
    gabi::store<f32>(model + 0xBC, sx); gabi::store<f32>(model + 0xC0, sy); gabi::store<f32>(model + 0xC4, sz);
    f32 y = self->current.pos.y, x = self->current.pos.x, z = self->current.pos.z;
    gabi::call(0x028E93CC, 0x1048D0CCu, x, y, z);
    s16 ax = self->shape_angle.x, ay = self->shape_angle.y, az = self->shape_angle.z;
    gabi::call(0x025F1B48, 0x1048D0CCu, ax, ay, az);
    morf = self->mRatMorf;
    model = gabi::load<u32>(gabi::ea(morf) + 0x90);
    nzCopyMatrix(0x1048D0CC, model + 0xC8);
    s8 animation = self->mAnimation;
    if (animation != 12 && animation != 11) return;
    void* item = nzGetShopItem(self, (u8)self->mSelectedItemSlot);
    if (!item) return;
    u32 itemScale = gabi::call<u32>(0x02483FA4, item);
    s32 count = (s16)self->mPurchaseCount + 1;
    if (count > 20) count = 20;
    count = (s16)count;
    self->mPurchaseCount = count;
    f32 scaleX, scaleY;
    if (count < 12) { f32 ratio = (f32)count / 12.0f; scaleX = ratio * 0.85f; scaleY = ratio * 1.4f; }
    else if (count < 16) {
        f32 ratio = (f32)(count - 12) * 0.25f;
        f32 remainder = 1.0f - ratio;
        scaleX = gabi::fmadds(ratio, 1.1f, remainder * 0.85f);
        scaleY = gabi::fmadds(ratio, 0.8f, remainder * 1.4f);
    } else {
        f32 ratio = (f32)(count - 16) * 0.5f;
        f32 remainder = 1.0f - ratio;
        scaleX = gabi::fmadds(remainder, 1.1f, ratio);
        scaleY = gabi::fmadds(remainder, 0.8f, ratio);
    }
    gabi::store<f32>(itemScale, scaleX); gabi::store<f32>(itemScale + 8, scaleX); gabi::store<f32>(itemScale + 4, scaleY);
    morf = self->mRatMorf;
    model = gabi::load<u32>(gabi::ea(morf) + 0x90);
    gabi::Local<cXyz> left, right, combined;
    u32 matrix = nzAnimationMatrix(model, 18);
    gabi::call(0x028E90D4, matrix, 0x1048D0CCu);
    gabi::call(0x028E8F64, 0x1048D0CCu, 0x101FFBA8u, left.get());
    matrix = nzAnimationMatrix(model, 21);
    gabi::call(0x028E90D4, matrix, 0x1048D0CCu);
    gabi::call(0x028E8F64, 0x1048D0CCu, 0x101FFBA8u, right.get());
    gabi::call(0x0201AD78, left.get(), combined.get(), right.get());
    u8 kind = self->mItemKind;
    f32 verticalOffset = 0.0f;
    if (kind == 0x51 || kind == 0x53 || kind == 0x83) verticalOffset = -5.0f;
    else if (kind == 0x82 || kind == 0xC || kind == 0xE) verticalOffset = -15.0f;
    else if (kind == 0x10 || kind == 0x12) verticalOffset = -10.0f;
    gabi::call(0x028E8E64, combined.get(), combined.get(), 0.5f);
    f32 lift = gabi::fmadds(scaleY, 20.0f, -20.0f);
    f32 combinedY = combined->y;
    combined->y = (combinedY + lift) + verticalOffset;
    u32 position = gabi::call<u32>(0x02483FB4, item);
    gabi::store<f32>(position, (f32)combined->x);
    gabi::store<f32>(position + 4, (f32)combined->y);
    gabi::store<f32>(position + 8, (f32)combined->z);
}
VERIFY(0x022A369C, nzSetMtx);

void nzLookBack(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A4070, void, self);
    gabi::Local<cXyz> eye, target;
    s16 angle = self->current.angle.y;
    gabi::call(0x0259D54C, eye.get(), 0.0f);
    u8 turning = self->m_jnt.mbTrn;
    target->copy(*eye);
    daNpc_Nz_HIO_c* hio = gabi::at<daNpc_Nz_HIO_c>(0x10467CE8);
    s16 p9 = hio->mHeadMaxY, p7 = hio->mBackboneMaxX;
    s16 p4 = hio->mHeadMinX, p10 = hio->mBackboneMinY;
    s16 p5 = hio->mHeadMaxX, p6 = hio->mBackboneMinX;
    s16 p0 = hio->mBackboneMaxY, p8 = hio->mHeadMinY;
    s16 velocity = turning ? (s16)hio->mTalkingLookVelocity : 0;
    s16 step = hio->mLookVelocity;
    gabi::call(0x0259E08C, &self->m_jnt, p4, p5, p6, p7, p8, p9, p10, p0, step);
    f32 z = self->eyePos.z, x = self->eyePos.x, y = self->eyePos.y;
    eye->set(x, y, z);
    gabi::call(0x0259DED0, &self->m_jnt, &self->current.angle.y, target.get(), eye.get(), angle, velocity, 1);
}
VERIFY(0x022A4070, nzLookBack);

BOOL nzExecute(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A45E4, BOOL, self);
    f32 targetSpeed = self->mTargetSpeed;
    gabi::call(0x0200ED84, &self->speedF, targetSpeed, 0.3f, 4.0f);
    nzCheckOrder(self); nzModeProc(self, 1, 2); nzLookBack(self);
    nzEventOrder(self); nzSetAttention(self);
    mDoExt_McaMorf* morf = self->mRatMorf;
    gabi::call(0x025E535C, morf, 0, 0, 0);
    morf = self->mRatMorf; gabi::call(0x025E55A0, morf);
    nzTailControl(self);
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x024F08A8, &self->mRatAcch, play + 0x12A0);
    self->mBait = nullptr; self->mBaitDistance = 3.4028234663852886e+38f;
    nzSetMtx(self);
    return 0;
}
VERIFY(0x022A45E4, nzExecute);
BOOL nzExecuteCallback(daNpc_Nz_c* self) { WWHD_FUNC(0x022A46B4, BOOL, self); return nzExecute(self); }
VERIFY(0x022A46B4, nzExecuteCallback);

void nzSmokeParticle(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A5368, void, self);
    u32 callback = gabi::ea(self->mSmokeCallback);
    if (gabi::load<u32>(callback + 4)) {
        gabi::call(0x025A5F88, callback);
        if (gabi::load<u32>(callback + 4)) return;
    }
    s8 room = self->current.roomNo;
    u32 play = gabi::call<u32>(0x025200D4);
    u32 control = gabi::load<u32>(play + 0x5AB0);
    u32 emitter = gabi::call<u32>(0x025A847C, control, 2, 0x2022, &self->current.pos,
                                 &self->current.angle, 0, 0xB9, callback, room, 0, 0, 0);
    if (emitter) {
        gabi::store<f32>(emitter + 0x34, 3.0f); gabi::store<f32>(emitter + 0x6C, 0.2f);
        gabi::store<u32>(emitter + 0x5C, 3); gabi::store<f32>(emitter + 0x58, 0.2f);
    }
}
VERIFY(0x022A5368, nzSmokeParticle);

struct NzGroundCheck {
    be<u32> firstInterface, secondInterface;
    u8 body[0x4C];
};
void nzTailControl(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A41A8, void, self);
    gabi::Local<cXyz> segment, delta, horizontal, direction, displacement;
    segment->set(0.0f, 0.0f, 5.0f);
    self->mTailPositions[0].copy(self->mTailAnchors[0]);
    gabi::call(0x0201ADE0, &self->mTailAnchors[1], delta.get(), &self->mTailAnchors[0]);
    f32 x = delta->x, z = delta->z, y = delta->y;
    s16 yaw = gabi::call<s16>(0x020195B0, x, z);
    horizontal->set(x, 0.0f, z);
    f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get());
    f32 distance = gabi::call<f32>(0x028F4384, square);
    s16 pitch = (s16)-gabi::call<s16>(0x020195B0, y, distance);
    gabi::call(0x025F1884, 0x1048D0CCu, yaw);
    gabi::call(0x025F1BF4, 0x1048D0CCu, pitch);
    gabi::call(0x028E8F64, 0x1048D0CCu, segment.get(), direction.get());
    u32 tailLines = gabi::load<u32>(gabi::ea(self) + 0xBD0);
    u32 output = gabi::load<u32>(tailLines);
    gabi::Local<NzGroundCheck> ground;
    u32 check = gabi::ea(ground.get());
    gabi::call(0x02008E0C, ground.get());
    gabi::store<u32>(check + 0x4C, 0x1001EBB4); gabi::store<u32>(check + 0x40, 0x1001EBC4);
    gabi::store<u32>(check + 0x10, 0x1001EB94); gabi::store<u32>(check + 0x20, 0x1001EBA4);
    for (int j = 0; j < 7; ++j) gabi::store<u8>(check + 0x44 + j, 0);
    gabi::store<u32>(check + 0x50, 1);
    ground->secondInterface = check + 0x4C; ground->firstInterface = check + 0x40;
    for (int i = 1; i < 10; ++i) {
        cXyz* current = &self->mTailPositions[i];
        cXyz* previous = &self->mTailPositions[i - 1];
        cXyz* velocity = &self->mTailVelocities[i];
        f32 vx = velocity->x, dz = direction->z, dy = direction->y;
        f32 vy = velocity->y;
        f32 factor = gabi::fnmsubs((f32)(i - 1), 0.1f, 1.0f);
        f32 dx = direction->x;
        f32 movingY = gabi::fmadds(dy, factor, vy);
        f32 currentY = current->y;
        f32 movingX = gabi::fmadds(dx, factor, vx);
        f32 floor = (f32)self->home.pos.y + 5.0f;
        f32 previousZ = previous->z;
        f32 vz = velocity->z;
        f32 nextY = (currentY + movingY) - 2.0f;
        f32 currentZ = current->z;
        f32 movingZ = gabi::fmadds(dz, factor, vz);
        f32 currentX = current->x, previousX = previous->x;
        if (nextY < floor) nextY = floor;
        f32 nextX = (currentX - previousX) + movingX;
        f32 nextZ = (currentZ - previousZ) + movingZ;
        f32 nextDeltaY = nextY - (f32)previous->y;
        yaw = gabi::call<s16>(0x020195B0, nextX, nextZ);
        horizontal->set(nextX, 0.0f, nextZ);
        square = gabi::call<f32>(0x028E8DD0, horizontal.get());
        distance = gabi::call<f32>(0x028F4384, square);
        pitch = (s16)-gabi::call<s16>(0x020195B0, nextDeltaY, distance);
        segment->set(0.0f, 0.0f, 20.0f);
        gabi::call(0x025F1884, 0x1048D0CCu, yaw);
        gabi::call(0x025F1BF4, 0x1048D0CCu, pitch);
        gabi::call(0x028E8F64, 0x1048D0CCu, segment.get(), displacement.get());
        velocity->copy(*current);
        f32 newX = (f32)previous->x + (f32)displacement->x;
        current->x = newX;
        current->y = (f32)previous->y + (f32)displacement->y;
        current->z = (f32)previous->z + (f32)displacement->z;
        f32 oldX = velocity->x;
        f32 oldY = velocity->y;
        velocity->x = (newX - oldX) * 0.8f;
        f32 nowY = current->y;
        f32 oldZ = velocity->z;
        velocity->y = (nowY - oldY) * 0.8f;
        velocity->z = ((f32)current->z - oldZ) * 0.8f;
    }
    for (int i = 0; i < 10; ++i) gabi::at<cXyz>(output + i * 12)->copy(self->mTailPositions[i]);
    gabi::store<u32>(check + 0x20, 0x1001EBA4);
    gabi::store<u32>(check + 0x40, 0x1001EBC4); gabi::store<u32>(check + 0x4C, 0x1001EB84);
    gabi::call(0x02008DAC, ground.get(), 0);
}
VERIFY(0x022A41A8, nzTailControl);

void nzCutEatFirstProc(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A5600, void, self);
    gabi::Local<cXyz> target;
    f32 z = self->mShopOrigin.z;
    u16 angle = self->home.angle.y;
    f32 y = self->mShopOrigin.y, x = self->mShopOrigin.x;
    u32 trig = 0x104A44F8 + ((u32)angle >> 3) * 8;
    f32 sin = gabi::load<f32>(trig), cos = gabi::load<f32>(trig + 4);
    s8 animation = self->mAnimation;
    target->set(gabi::fmadds(100.0f, sin, x), y, gabi::fmadds(100.0f, cos, z));
    if (animation == 10) {
        f32 distance = gabi::call<f32>(0x0200EF78, &self->current.pos, target.get(), 0.25f, 10.0f, 2.0f);
        s16 targetAngle = gabi::call<s16>(0x0200F93C, &self->current.pos, target.get());
        gabi::call(0x0200F428, &self->shape_angle.y, targetAngle, 8, 0x1000);
        if (!(fabsf(distance) < 7.999999968033578e-11f)) return;
        if (!nzFrameEnd(self->mRatMorf)) return;
        nzSetAnm(self, 6, 0); self->mAnimationLoops = 2;
    }
    gabi::call(0x025D5218, 0x022A3094u, self);
    fopAc_ac_c* bait = self->mBait;
    if (bait) {
        if (!nzFrameEnd(self->mRatMorf)) return;
        if (gabi::call<s16>(0x02055B64, &self->mAnimationLoops)) return;
        gabi::call(0x025D57E0, bait);
        s32 staff = self->mStaffId;
        self->mBait = nullptr;
        nzCutEnd(staff);
    } else { s32 staff = self->mStaffId; nzCutEnd(staff); }
}
VERIFY(0x022A5600, nzCutEatFirstProc);

void nzCutKillAllStart(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A5818, void, self);
    gabi::call(0x025D5218, 0x022A30A4u, self);
    gabi::call(0x025D5218, 0x022A30F0u, self);
    gabi::call(0x025D5218, 0x022A32B4u, self);
    u32 play = gabi::call<u32>(0x025200D4);
    u16 angle = self->home.angle.y;
    f32 z = self->mShopOrigin.z, y = self->mShopOrigin.y, x = self->mShopOrigin.x;
    u32 trig = 0x104A44F8 + ((u32)angle >> 3) * 8;
    f32 sin = gabi::load<f32>(trig), cos = gabi::load<f32>(trig + 4);
    u32 player = gabi::load<u32>(play + 0x5B2C);
    gabi::Local<cXyz> target;
    target->set(gabi::fmadds(250.0f, sin, x), y, gabi::fmadds(250.0f, cos, z));
    u32 vt = gabi::load<u32>(player + 0xB4), method = gabi::load<u32>(vt + 0x114);
    gabi::call_ptr(method, player, target.get(), (s16)(angle + 0x8000));
}
VERIFY(0x022A5818, nzCutKillAllStart);

void* nzSearchBait(daNpc_Nz_c* self, fopAc_ac_c* actor) {
    WWHD_FUNC(0x022A2EEC, void*, self, actor);
    if (!actor || gabi::load<s16>(gabi::ea(actor) + 8) != 0xDD || gabi::load<u8>(gabi::ea(actor) + 0x3B4)) return nullptr;
    gabi::Local<cXyz> delta, horizontal, nearer;
    gabi::call(0x0201ADE0, &actor->current.pos, delta.get(), &self->current.pos);
    mDoExt_McaMorf* morf = self->mRatMorf;
    if (gabi::load<f32>(gabi::ea(morf) + 0x9C) < 6.0f) {
        gabi::call(0x0200F164, &actor->current.pos, &self->current.pos, 0.1f, 2.5f);
        f32 x = actor->current.pos.x, z = actor->current.pos.z, y = actor->current.pos.y;
        gabi::call(0x028E93CC, 0x1048D0CCu, x, y, z);
        s16 ax = actor->current.angle.x, az = actor->current.angle.z, ay = actor->current.angle.y;
        gabi::call(0x025F1B48, 0x1048D0CCu, ax, ay, az);
        u32 model = gabi::load<u32>(gabi::ea(actor) + 0x3D8);
        nzCopyMatrix(0x1048D0CC, model + 0xC8);
    }
    f32 distance = nzHorizontalDistance(delta.get(), horizontal.get());
    if (distance < (f32)self->mBaitDistance) {
        self->mBait = actor;
        self->mBaitDistance = nzHorizontalDistance(delta.get(), nearer.get());
    }
    return nullptr;
}
VERIFY(0x022A2EEC, nzSearchBait);
void* nzSearchBaitCallback(fopAc_ac_c* actor, daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A3094, void*, actor, self); return nzSearchBait(self, actor);
}
VERIFY(0x022A3094, nzSearchBaitCallback);

void* nzScatterBait(fopAc_ac_c* bait, daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A32B4, void*, bait, self);
    if (!bait || gabi::load<s16>(gabi::ea(bait) + 8) != 0xDD) return nullptr;
    u16 angle = self->current.angle.y;
    u32 trig = 0x104A44F8 + ((u32)angle >> 3) * 8;
    f32 x = self->mShopOrigin.x, z = self->mShopOrigin.z;
    f32 cos = gabi::load<f32>(trig + 4), sin = gabi::load<f32>(trig);
    f32 targetZ = gabi::fmadds(150.0f, cos, z);
    f32 y = self->mShopOrigin.y;
    f32 targetX = gabi::fmadds(150.0f, sin, x);
    f32 random = gabi::call<f32>(0x02019918, 358.0f);
    u16 scatteredAngle = (u16)gabi::ftoi(random * 182.04444885253906f);
    f32 radius = gabi::call<f32>(0x020198D8, 30.0f);
    trig = 0x104A44F8 + ((u32)scatteredAngle >> 3) * 8;
    sin = gabi::load<f32>(trig); cos = gabi::load<f32>(trig + 4);
    targetX = gabi::fmadds(radius, sin, targetX); targetZ = gabi::fmadds(radius, cos, targetZ);
    bait->current.pos.x = targetX; bait->old.pos.x = targetX;
    bait->current.pos.y = y; bait->current.pos.z = targetZ;
    bait->old.pos.y = y; bait->old.pos.z = targetZ;
    gabi::call(0x028E93CC, 0x1048D0CCu, targetX, y, targetZ);
    s16 ay = bait->current.angle.y, az = bait->current.angle.z, ax = bait->current.angle.x;
    gabi::call(0x025F1B48, 0x1048D0CCu, ax, ay, az);
    u32 model = gabi::load<u32>(gabi::ea(bait) + 0x3D8);
    nzCopyMatrix(0x1048D0CC, model + 0xC8);
    return nullptr;
}
VERIFY(0x022A32B4, nzScatterBait);

void* nzResetContainers(fopAc_ac_c* actor, daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A30F0, void*, actor, self);
    if (!actor) return nullptr;
    s16 name = gabi::load<s16>(gabi::ea(actor) + 8);
    if (name != 0x1C5 && name != 0x1C8) return nullptr;
    gabi::Local<cXyz> delta, horizontal;
    gabi::call(0x0201ADE0, &self->mShopOrigin, delta.get(), &actor->current.pos);
    if (!(nzHorizontalDistance(delta.get(), horizontal.get()) < 300.0f)) return nullptr;
    name = gabi::load<s16>(gabi::ea(actor) + 8);
    if (name == 0x1C5) {
        if (gabi::load<u32>(gabi::ea(actor) + 0x794) != 2) {
            gabi::call(0x025D57E0, actor);
            return nullptr;
        }
        actor->current.pos.z = (f32)actor->home.pos.z;
        u16 az = actor->home.angle.z;
        actor->current.angle.z = az;
        actor->current.pos.x = (f32)actor->home.pos.x;
        f32 y = actor->home.pos.y;
        u16 ay = actor->home.angle.y;
        actor->current.pos.y = y; actor->shape_angle.y = ay;
        u16 ax = actor->home.angle.x;
        actor->current.angle.y = ay; actor->current.angle.x = ax;
        actor->shape_angle.z = az; actor->shape_angle.x = ax;
        for (u32 off = 0x7A4; off <= 0x7AC; off += 2) gabi::call(0x02006638, gabi::ea(actor) + off, 0x101FF354u);
        s16 angle = actor->current.angle.y;
        gabi::call(0x02006584, gabi::ea(actor) + 0x7AE, angle);
        gabi::call(0x028E9098, gabi::ea(actor) + 0x7D8);
        return nullptr;
    }
    name = gabi::load<s16>(gabi::ea(actor) + 8);
    if (name == 0x1C5 && !gabi::load<u32>(gabi::ea(actor) + 0x728)) {
        actor->current.pos.y = (f32)actor->home.pos.y;
        u16 ax = actor->home.angle.x; actor->shape_angle.x = ax;
        actor->current.pos.z = (f32)actor->home.pos.z;
        u16 ay = actor->home.angle.y; actor->current.angle.y = ay;
        actor->current.pos.x = (f32)actor->home.pos.x;
        u16 az = actor->home.angle.z;
        actor->current.angle.z = az; actor->shape_angle.z = az;
        actor->shape_angle.y = ay; actor->current.angle.x = ax;
        return nullptr;
    }
    gabi::call(0x025D57E0, actor);
    return nullptr;
}
VERIFY(0x022A30F0, nzResetContainers);

static u32 nzDrawMaterial(u32 modelData, u32 name, SafeString* key) {
    key->mStringTop = name; key->__vtbl = 0x1001EB6C;
    u32 table = gabi::load<u32>(modelData);
    gabi::call_ptr(0x022A6034, key);
    u32 offset = gabi::load<u32>(table + 0x18);
    u32 nameTable = offset ? table + 0x18 + offset : 0;
    u32 string = key->mStringTop;
    s32 index = gabi::call<s32>(0x027DF9B0, nameTable, string);
    u32 material = 0;
    if (index >= 0) {
        u32 count = gabi::load<u32>(modelData + 0xC);
        material = gabi::load<u32>(modelData + 0x10);
        if ((u32)index < count) material += (u32)index * 0x39C;
    }
    return gabi::load<u32>(material + 8);
}
BOOL nzDraw(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A46B8, BOOL, self);
    if (!(u8)self->mVisible) return 1;
    mDoExt_McaMorf* morf = self->mRatMorf;
    u32 model = gabi::load<u32>(gabi::ea(morf) + 0x90);
    u32 data = gabi::load<u32>(model + 0xAC);
    u32 materialTable = gabi::load<u32>(data + 8);
    gabi::Local<SafeString> key;
    u32 invisible = nzDrawMaterial(data, 0x1001EEC4, key.get());
    u32 first = nzDrawMaterial(data, 0x1001EED0, key.get());
    u32 second = nzDrawMaterial(data, 0x1001EEDC, key.get());
    gabi::store<u8>(invisible + 4, 0);
    u32 env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, env, 0, &self->current.pos, &self->tevStr);
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, env, model, &self->tevStr);
    morf = self->mRatMorf; gabi::call(0x025E5590, morf);
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D84));
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D88));
    gabi::store<u8>(invisible + 4, 1); gabi::store<u8>(first + 4, 0); gabi::store<u8>(second + 4, 0);
    gabi::call(0x027F583C, model, materialTable);
    gabi::store<u8>(first + 4, 1); gabi::store<u8>(second + 4, 1);
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
    play = gabi::call<u32>(0x025200D4);
    gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
    f32 width = (f32)self->scale.x * 5.0f;
    gabi::call(0x025EC62C, self->mTailMaterial, 10, 0x101C2570u, 6, &self->tevStr, width);
    play = gabi::call<u32>(0x025200D4);
    u32 sorter = play + 0x5FB4;
    u32 vt = gabi::load<u32>(gabi::ea(self) + 0xB7C);
    u32 method = gabi::load<u32>(vt + 0x14);
    s32 type = gabi::call_ptr<s32>(method, self->mTailMaterial);
    gabi::call(0x025EDD04, sorter + (u32)type * 0x9C, self->mTailMaterial);
    return 1;
}
VERIFY(0x022A46B8, nzDraw);
BOOL nzDrawCallback(daNpc_Nz_c* self) { WWHD_FUNC(0x022A4988, BOOL, self); return nzDraw(self); }
VERIFY(0x022A4988, nzDrawCallback);

struct NzParticleColor { be<u8> r, g, b, a; };
void nzCutSetAnimationProc(daNpc_Nz_c* self) {
    WWHD_FUNC(0x022A5A80, void, self);
    BOOL passed = nzFrameEnd(self->mRatMorf);
    s16 loops = self->mAnimationLoops;
    if (passed) { loops = (s16)(loops - 1); self->mAnimationLoops = loops; }
    if (loops <= 0) { s32 staff = self->mStaffId; nzCutEnd(staff); }
    if ((s8)self->mAnimation != 8) return;
    mDoExt_McaMorf* morf = self->mRatMorf;
    if (gabi::call<BOOL>(0x027F2BF8, gabi::ea(morf) + 0x98, 8.0f)) { nzSmokeParticle(self); return; }
    morf = self->mRatMorf;
    if (!gabi::call<BOOL>(0x027F2BF8, gabi::ea(morf) + 0x98, 13.0f)) return;
    morf = self->mRatMorf;
    s8 head = self->m_jnt.mHeadJntNum;
    u32 model = gabi::load<u32>(gabi::ea(morf) + 0x90);
    gabi::Local<cXyz> local, scale, world;
    gabi::Local<NzParticleColor> color;
    local->set(10.0f, 0.0f, 10.0f); scale->set(0.65f, 0.65f, 0.65f);
    color->r = 0x80; color->g = 0x80; color->b = 0x80;
    u32 matrix = nzAnimationMatrix(model, head);
    gabi::call(0x028E90D4, matrix, 0x1048D0CCu);
    gabi::call(0x028E8F64, 0x1048D0CCu, local.get(), world.get());
    u32 play = gabi::call<u32>(0x025200D4);
    u32 particles = gabi::load<u32>(play + 0x5AB0);
    gabi::call(0x025A847C, particles, 0, 0x57, world.get(), &self->shape_angle,
               scale.get(), 0xFF, 0, -1, color.get(), 0, 0);
}
VERIFY(0x022A5A80, nzCutSetAnimationProc);

u16 nzNextMessage(daNpc_Nz_c* self, be<u32>* messageNumber) {
    WWHD_FUNC(0x022A4CFC, u16, self, messageNumber);
    gabi::Local<u8[8]> items;
    const u8 itemNumbers[8] = {0x82,0x83,0x51,0x53,0xC,0xE,0x10,0x12};
    for (int i = 0; i < 8; ++i) gabi::store<u8>(items.a + i, itemNumbers[i]);
    u32 number = *messageNumber;
    u32 message = gabi::load<u32>(0x101F4B5C);
    if (number == 0x33F5) { *messageNumber = 0x33F6; return 15; }
    if (number == 0x33F6) {
        u32 save = gabi::load<u32>(0x101F84DC);
        u8 occupied = 0;
        for (int i = 0; i < 4; ++i) if (gabi::load<u8>(save + 0x6A + i) != 0xFF) ++occupied;
        s32 choices = occupied ? 2 : 1;
        BOOL bow = gabi::call<BOOL>(0x02520C0C, 0x31);
        save = gabi::load<u32>(0x101F84DC);
        u8 bombs = gabi::load<u8>(save + 0x68);
        if (bow) ++choices;
        if (bombs != 0xFF) ++choices;
        f32 random = gabi::call<f32>(0x020198D8, (f32)choices);
        s16 selection = (s16)gabi::ftoi(random);
        self->mStockSelection = selection;
        *messageNumber = (u32)(s32)selection + 0x33F7;
        selection = self->mStockSelection;
        // The table is a guest local in the original; its legal producer supplies0..3.
        u8 first = gabi::load<u8>(items.a + (u32)(s32)selection * 2);
        u8 second = gabi::load<u8>(items.a + (u32)(s32)selection * 2 + 1);
        nzCreateShopItem(self, first, second);
        self->mMessage = (u32)*messageNumber;
        return 15;
    }
    if (number >= 0x33F7 && number <= 0x33FA) {
        u32 choice = gabi::load<u32>(message + 0x948);
        if (choice < 2) {
            s16 selection = self->mStockSelection;
            u32 base = (u32)(s32)selection * 2;
            self->mItemKind = gabi::load<u8>(items.a + base + choice);
            choice = gabi::load<u32>(message + 0x948);
            *messageNumber = gabi::load<u32>(0x1001EFB0 + (base + choice) * 4);
            self->mSelectedItemSlot = gabi::load<u32>(message + 0x948);
        } else if (choice == 2) *messageNumber = 0x3404;
        return 15;
    }
    if (number >= 0x33FB && number <= 0x3402) {
        u32 choice = gabi::load<u32>(message + 0x948);
        if (choice == 1) { *messageNumber = (u32)self->mMessage; return 15; }
        if (choice != 0) return 15;
        u8 item = self->mItemKind;
        u32 play = gabi::call<u32>(0x025200D4);
        u32 save = gabi::load<u32>(0x101F84DC);
        s16 price = gabi::load<s16>(play + 0x5BA4);
        if (gabi::load<u16>(save + 0x24) < (s32)price) { *messageNumber = 0x3405; return 15; }
        if ((item == 0x51 || item == 0x53) && !gabi::call<BOOL>(0x025B5C54, save + 0x5C)) { *messageNumber = 0x3406; return 15; }
        if (gabi::call<BOOL>(0x02550FC4, item)) {
            save = gabi::load<u32>(0x101F84DC);
            if (gabi::load<u8>(save + 0x8A) == gabi::load<u8>(save + 0x90)) { *messageNumber = 0x3407; return 15; }
        }
        if (gabi::call<BOOL>(0x02550FDC, item)) {
            save = gabi::load<u32>(0x101F84DC);
            if (gabi::load<u8>(save + 0x89) == gabi::load<u8>(save + 0x8F)) { *messageNumber = 0x3407; return 15; }
        }
        if (item == 0x82 || item == 0x83) {
            save = gabi::load<u32>(0x101F84DC);
            if (!gabi::call<BOOL>(0x025B6E90, save + 0x96)) { *messageNumber = 0x3407; return 15; }
        }
        if (gabi::call<BOOL>(0x0254DA50, item, 0)) {
            play = gabi::call<u32>(0x025200D4);
            u32 balance = gabi::load<u32>(play + 0x5B48);
            gabi::store<u32>(play + 0x5B48, balance - (s32)price);
            gabi::call(0x0254DA38, item);
            *messageNumber = 0x3403;
            return 15;
        }
        void* held = nzGetShopItem(self, (u8)self->mSelectedItemSlot);
        if (held) gabi::call(0x021842B8, held);
        nzSetAnm(self, 0, 0);
        play = gabi::call<u32>(0x025200D4);
        price = gabi::load<s16>(play + 0x5BA4);
        play = gabi::call<u32>(0x025200D4);
        u32 balance = gabi::load<u32>(play + 0x5B48);
        gabi::store<u32>(play + 0x5B48, balance - (s32)price);
        self->mTalkState = 3;
        u32 manager = nzEventManager();
        self->mEventId = gabi::call<s16>(0x02543F10, manager, 0x1001EFD0u, 0xFF);
        item = self->mItemKind;
        play = gabi::call<u32>(0x025200D4);
        gabi::store<u8>(play + 0x52A4, item);
        return 16;
    }
    if (number >= 0x3404 && number <= 0x3407) { *messageNumber = 0x3403; return 15; }
    return 16;
}
VERIFY(0x022A4CFC, nzNextMessage);
