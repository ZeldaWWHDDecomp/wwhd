/* Particle control and callbacks; verified HD behavior, GameCube naming. */
#include "d/particle/d_particle_local.h"
using namespace pa;
static void releaseRoom(u32 a) {
    pa_mDoExt_destroySolidHeap(p((u32)gabi::at<pa::dPa_control_c>(a)->mSceneHeap));
    pa_heapFree(p((u32)gabi::at<pa::dPa_control_c>(a)->mpData));
    u8 keep = (u8)gabi::at<pa::dPa_control_c>(a)->mNumSimpleCommon,
       count = (u8)gabi::at<pa::dPa_control_c>(a)->mNumSimple;
    gabi::at<pa::dPa_control_c>(a)->mSceneHeap = 0;
    gabi::at<pa::dPa_control_c>(a)->mSceneResMng = 0;
    if (keep < count) {
        u32 item = a + 0x148 + u32(keep) * 0x290;
        s32 i = keep;
        do {
            W(item + 4, 0);
            ++i;
            item += 0x290;
        } while (i < (u8)gabi::at<pa::dPa_control_c>(a)->mNumSimple);
        keep = (u8)gabi::at<pa::dPa_control_c>(a)->mNumSimpleCommon;
    }
    gabi::at<pa::dPa_control_c>(a)->mNumSimple = keep;
}
/* 025A7EAC */
s32 pa::dPa_control_c::removeRoomScene(s32 force, s32 room) {
    u32 a = gabi::ea(this);
    if (!(u32)this->mSceneHeap)
        return 0;
    if (!force && s32((u32)this->mSceneReleaseTimer) >= 0) {
        s32 timer = s32((u32)this->mSceneReleaseTimer - 1);
        this->mSceneReleaseTimer = u32(timer);
        if (timer > 0)
            return 0;
        releaseRoom(a);
        return 1;
    }
    u8 refs = (u8)this->mCount;
    if (!refs)
        return 0;
    refs = u8(refs - 1);
    this->mCount = refs;
    if (refs)
        return 0;
    if (!force && s32((u32)this->mSceneReleaseTimer) <= 0) {
        pa_notifyParticleRoomRelease(s32(s8(room)));
        if (s32((u32)this->mSceneReleaseTimer) > 0)
            return 0;
        this->mSceneReleaseTimer = 2;
        pa_clearResourceManager(p((u32)l_particleEmitterManager), u32(1));
        return 0;
    }
    if (force)
        pa_clearResourceManager(p((u32)l_particleEmitterManager), u32(1));
    releaseRoom(a);
    pa_notifyParticleRoomRelease(s32(s8(room)));
    return 1;
}
/* 025A809C */
void pa::dPa_control_c::removeScene() {
    u32 a = gabi::ea(this);
    pa_removeRoomScene(this, u32(1), u32(0));
    u32 heap = (u32)this->mSceneHeap;
    u8 count = (u8)this->mNumSimpleCommon;
    this->mNumSimple = count;
    if (heap)
        pa_mDoExt_destroySolidHeap(p(heap));
    if ((u32)this->mSceneResMng) {
        pa_heapFree(p((u32)this->mpData));
        this->mSceneResMng = 0;
    }
    u32 i = 0, item = a + 0x148;
    while (i < (u8)this->mNumSimpleCommon) {
        W(item + 4, 0);
        i++;
        item += 0x290;
    }
    pa_forceDeleteAllEmitter(p((u32)l_particleEmitterManager));
    pa_modelControlDtor(p((u32)this->mModelControl));
}
/* 025A7C30 */
void pa::dPa_control_c::createRoomScene(void* data) {

    void* heap = pa_mDoExt_createSolidHeapFromGame(u32(0), nullptr);
    this->mSceneHeap = gabi::ea(heap);
    if (!heap) {
        pa_JUTAssertion_showAssert(p(0x10051E90), u32(0x785), p(0x10051E60));
        heap = p((u32)this->mSceneHeap);
    }
    W(gabi::ea(heap) + 16, 0x10051E74);
    heap = p((u32)this->mSceneHeap);
    this->mpData = gabi::ea(data);
    void* rm = pa_operatorNewHeap(u32(12), heap, u32(4));
    if (rm) {
        u32 input = (u32)this->mpData, bank = (u32)this->mTextureBank,
            currentHeap = (u32)this->mSceneHeap;
        rm = pa_JPAResourceManager_ctor(rm, p(input), p(currentHeap), p(bank));
    }
    this->mSceneResMng = gabi::ea(rm);
    if (!rm) {
        pa_JUTAssertion_showAssert(p(0x10051E90), u32(0x78C), p(0x10051E4C));
        rm = p((u32)this->mSceneResMng);
    }
    W((u32)l_particleEmitterManager + 0x34, gabi::ea(rm));
    pa_mDoExt_adjustSolidHeap(p((u32)this->mSceneHeap));
    for (u32 i = 0; i < 34; i++) {
        u32 r = (u32)this->mSceneResMng;
        u16 id = h(0x101EA51C + i * 2);
        if (pa_JPAResourceManager_getResource(p(w(r + 4)), u32(id))) {
            u32 group = id & 0x4000   ? dPtclGroup_Projection_e
                        : id & 0x2000 ? dPtclGroup_Toon_e
                                      : dPtclGroup_Normal_e;
            pa_newSimple(this, u32(id), group);
        }
    }
    this->mSceneReleaseTimer = ~u32(0);
}
/* 025A7724 */
void pa::dPa_control_c::applyDefaultParameters() {
    u32 a = gabi::ea(this);
    f32 one = (f32)l_particleOne, a03 = f(0x10051D2C), a08 = f(0x10051D28), a125 = f(0x10051D30),
        a02 = f(0x10051D34), a12 = f(0x10051D38), half = (f32)l_particleHalf;
    gabi::at<pa::dPa_control_c>(a)->mParameters[0].mScale[0] = one;
    gabi::at<pa::dPa_control_c>(a)->mParameters[0].mScale[2] = one;
    gabi::at<pa::dPa_control_c>(a)->mParameters[0].mName = 0x10051D48;
    gabi::at<pa::dPa_control_c>(a)->mParameters[2].mScale[3] = a03;
    gabi::at<pa::dPa_control_c>(a)->mParameters[3].mScale[1] = one;
    gabi::at<pa::dPa_control_c>(a)->mParameters[3].mScale[3] = one;
    gabi::at<pa::dPa_control_c>(a)->mParameters[1].mName = 0x10051D54;
    gabi::at<pa::dPa_control_c>(a)->mParameters[3].mScale[2] = a125;
    gabi::at<pa::dPa_control_c>(a)->mEnvironmentColor.a = 255;
    gabi::at<pa::dPa_control_c>(a)->mParameters[0].mScale[3] = one;
    gabi::at<pa::dPa_control_c>(a)->mParameters[4].mScale[2] = one;
    gabi::at<pa::dPa_control_c>(a)->mParameters[3].mName = 0x10051D44;
    gabi::at<pa::dPa_control_c>(a)->mParameters[2].mName = 0x10051D3C;
    gabi::at<pa::dPa_control_c>(a)->mPrimaryColor.a = 255;
    gabi::at<pa::dPa_control_c>(a)->mEnvironmentColor.g = 0;
    gabi::at<pa::dPa_control_c>(a)->mParameters[4].mScale[0] = a02;
    gabi::at<pa::dPa_control_c>(a)->mParameters[3].mScale[0] = one;
    gabi::at<pa::dPa_control_c>(a)->mRenderState[0] = 0;
    gabi::at<pa::dPa_control_c>(a)->mPrimaryColor.g = 230;
    gabi::at<pa::dPa_control_c>(a)->mParameters[4].mName = 0x10051D60;
    gabi::at<pa::dPa_control_c>(a)->mParameters[1].mScale[3] = a08;
    gabi::at<pa::dPa_control_c>(a)->mEnvironmentColor.r = 8;
    gabi::at<pa::dPa_control_c>(a)->mColorBlend = half;
    gabi::at<pa::dPa_control_c>(a)->mRenderState[2] = 0;
    gabi::at<pa::dPa_control_c>(a)->mEnvironmentColor.b = 20;
    gabi::at<pa::dPa_control_c>(a)->mPrimaryColor.r = 200;
    gabi::at<pa::dPa_control_c>(a)->mParameters[0].mScale[1] = one;
    gabi::at<pa::dPa_control_c>(a)->mPrimaryColor.b = 230;
    gabi::at<pa::dPa_control_c>(a)->mParameters[4].mScale[1] = a08;
    gabi::at<pa::dPa_control_c>(a)->mParameters[4].mScale[3] = a12;
    gabi::at<pa::dPa_control_c>(a)->mRenderState[1] = 0;
    u32 setting = w(0x104A0264);
    gabi::at<pa::dPa_control_c>(a)->mRenderState[3] = 0;
    W(a + 0x10C, setting);
    gabi::at<pa::dPa_control_c>(a)->mMenuDrawEnabled = 0;
    W(a + 0x108, setting);
    gabi::at<pa::dPa_control_c>(a)->mDrawColorBlend = half;
}
/* 025AA790 */
void dPa_globalInit() {
    u32 a = 0x1047B168;
    W(a + 12, 0);
    W(a + 8, 0);
    W(a + 4, 0);
    W(a, 0);
    pa_registerGlobalObject(p(0x101EA450));
    f32 lo = f(0x10051F70), hi = f(0x10051F74);
    F(0x1047B15C, lo);
    F(0x1047B160, hi);
    pa_initializeMathHeaderPrimary(p(0x1047B164));
    pa_registerGlobalObject(p(0x101EA45C));
    pa_initializeMathHeaderSecondary(p(0x1047B165));
    pa_registerGlobalObject(p(0x101EA468));
    a = 0x1047B24C;
    B(a + 4, 0);
    W(a + 16, 0x100520A8);
    B(a + 12, 1);
    B(a + 20, 2);
    W(a, 0x100520A8);
    W(a + 24, 0x100520A8);
    W(a + 8, 0x100520A8);
    B(a + 28, 3);
    pa_registerGlobalObject(p(0x101EA474));
    a = 0x1047B26C;
    W(a, 0x10051B94);
    copy(0x101EA420, a + 4, 4);
    W(a + 8, 0x10051B94);
    copy(0x101EA424, a + 12, 4);
    W(a + 16, 0x10051B94);
    copy(0x101EA428, a + 20, 4);
    pa_registerGlobalObject(p(0x101EA480));
    W(0x1047B2D8, 0x10052320);
    pa_registerGlobalObject(p(0x101EA48C));
    W(0x1047B2DC, 0x10052360);
    pa_registerGlobalObject(p(0x101EA498));
    pa_smokeCtorFlags(p(0x1047B284), u32(0), u32(1), u32(1), u32(1));
    pa_registerGlobalObject(p(0x101EA4A4));
    W(0x1047B2E0, 0x100522C0);
    pa_registerGlobalObject(p(0x101EA4B0));
    W(0x1047B2E4, 0x10052128);
    pa_registerGlobalObject(p(0x101EA4BC));
    W(0x1047B2E8, 0x10052398);
    pa_registerGlobalObject(p(0x101EA4C8));
    W(0x1047B2EC, 0x100520E8);
    pa_registerGlobalObject(p(0x101EA4D4));
    a = 0x1047B1E4;
    W(a + 20, 0x10051A34);
    W(a + 56, 0x10051F88);
    W(a + 40, 0x10051FF4);
    W(a, 0x10051F78);
    W(a + 44, 0x10051A34);
    W(a + 48, 0x10052008);
    W(a + 32, 0x10051FA8);
    W(a + 52, 0x10051A34);
    W(a + 12, 0x10051A34);
    W(a + 68, 0x10051A34);
    W(a + 4, 0x10051A34);
    W(a + 16, 0x10051FE4);
    W(a + 84, 0x10051A34);
    W(a + 72, 0x10052040);
    W(a + 8, 0x10052018);
    W(a + 80, 0x10052058);
    W(a + 24, 0x1005202C);
    W(a + 28, 0x10051A34);
    W(a + 88, 0x10051FBC);
    W(a + 36, 0x10051A34);
    W(a + 76, 0x10051A34);
    W(a + 64, 0x10051F98);
    W(a + 60, 0x10051A34);
    W(a + 92, 0x10051A34);
    W(a + 96, 0x10051FD0);
    W(a + 100, 0x10051A34);
}
static void initBoundCallback(u32 place, u32 owner, u32 function) {
    void* obj = allocate(p(place), 32);
    if (!obj)
        return;
    u32 a = gabi::ea(obj);
    void* base = allocate(obj, 8);
    if (base) {
        W(gabi::ea(base) + 4, 0x10051A34);
        W(gabi::ea(base), 0x10051A19);
    }
    B(a + 8, 0);
    W(a + 12, 0x10051BFC);
    u32 functor = a + 16;
    if (functor) {
        W(functor + 4, owner);
        W(functor + 8, 0x0000FFFF);
        W(functor, 0x10051C0C);
        W(functor + 12, function);
    }
}
/* 025A7398 */
void* dPa_controlCtor(void* obj) {
    obj = allocate(obj, 0x41A4);
    if (!obj)
        return obj;
    u32 a = gabi::ea(obj);
    pa_constructArray(obj, u32(10), u32(24), p(0x025AACDC));
    gabi::at<pa::dPa_control_c>(a)->mPrimaryColor.g = 0;
    gabi::at<pa::dPa_control_c>(a)->mPrimaryColor.a = 255;
    gabi::at<pa::dPa_control_c>(a)->mEnvironmentColor.a = 255;
    gabi::at<pa::dPa_control_c>(a)->mPrimaryColor.r = 0;
    f32 zero = (f32)l_particleZero;
    gabi::at<pa::dPa_control_c>(a)->mColorBlend = zero;
    gabi::at<pa::dPa_control_c>(a)->mPrimaryColor.b = 0;
    gabi::at<pa::dPa_control_c>(a)->mEnvironmentColor.r = 0;
    gabi::at<pa::dPa_control_c>(a)->mEnvironmentColor.g = 0;
    gabi::at<pa::dPa_control_c>(a)->mEnvironmentColor.b = 0;
    pa_constructArray(p(a + 0xFC), u32(3), u32(4), p(0x025AAD64));
    gabi::at<pa::dPa_control_c>(a)->mNumSimpleCommon = 0;
    gabi::at<pa::dPa_control_c>(a)->mRenderState[3] = 0;
    gabi::at<pa::dPa_control_c>(a)->mSceneResMng = 0;
    gabi::at<pa::dPa_control_c>(a)->mRenderState[0] = 0;
    gabi::at<pa::dPa_control_c>(a)->mDrawEnvironmentColor.a = 255;
    gabi::at<pa::dPa_control_c>(a)->mDrawPrimaryColor.b = 0;
    gabi::at<pa::dPa_control_c>(a)->mDrawEnvironmentColor.g = 0;
    gabi::at<pa::dPa_control_c>(a)->mDrawColorBlend = zero;
    gabi::at<pa::dPa_control_c>(a)->mSceneNo = 0;
    gabi::at<pa::dPa_control_c>(a)->mHeap = 0;
    gabi::at<pa::dPa_control_c>(a)->mNumSimple = 0;
    gabi::at<pa::dPa_control_c>(a)->mSceneHeap = 0;
    gabi::at<pa::dPa_control_c>(a)->mDrawPrimaryColor.g = 0;
    gabi::at<pa::dPa_control_c>(a)->mSceneReleaseTimer = 0;
    gabi::at<pa::dPa_control_c>(a)->mDrawEnvironmentColor.b = 0;
    gabi::at<pa::dPa_control_c>(a)->mCount = 0;
    gabi::at<pa::dPa_control_c>(a)->mDrawPrimaryColor.a = 255;
    gabi::at<pa::dPa_control_c>(a)->mMenuDrawEnabled = 0;
    gabi::at<pa::dPa_control_c>(a)->mpData = 0;
    gabi::at<pa::dPa_control_c>(a)->mCommonResMng = 0;
    gabi::at<pa::dPa_control_c>(a)->mDrawPrimaryColor.r = 0;
    gabi::at<pa::dPa_control_c>(a)->mRenderState[1] = 0;
    gabi::at<pa::dPa_control_c>(a)->mRenderState[2] = 0;
    gabi::at<pa::dPa_control_c>(a)->mDrawEnvironmentColor.r = 0;
    gabi::at<pa::dPa_control_c>(a)->mModelControl = 0;
    pa_constructArray(p(a + 0x148), u32(25), u32(0x290), p(0x025A4A60));
    gabi::at<pa::dPa_control_c>(a)->mRenderControl = 0;
    gabi::at<pa::dPa_control_c>(a)->mTextureBank = 0;
    W(a + 0x4160, 0x10051BD4);
    initBoundCallback(a + 0x4164, a, 0x025A8D8C);
    initBoundCallback(a + 0x4184, a, 0x025A8F08);
    void* system = pa_getParticleSystemHeap();
    gabi::Local<u32[2]> name;
    W(gabi::ea(name.get()) + 4, 0x10051A34);
    W(gabi::ea(name.get()), 0x10051CCC);
    void* heap = pa_createParticleHeap(u32(0x02E00000), name.get(), system, u32(1), u32(0));
    gabi::at<pa::dPa_control_c>(a)->mHeap = gabi::ea(heap);
    if (!heap)
        pa_JUTAssertion_showAssert(p(0x10051CE0), u32(0x6B9), p(0x10051CF0));
    gabi::at<pa::dPa_control_c>(a)->mCount = 0;
    gabi::at<pa::dPa_control_c>(a)->mSceneNo = 255;
    l_particleEmitterManager = 0;
    gabi::at<pa::dPa_control_c>(a)->mSceneReleaseTimer = ~u32(0);
    gabi::at<pa::dPa_control_c>(a)->mCommonResMng = 0;
    gabi::at<pa::dPa_control_c>(a)->mNumSimple = 0;
    gabi::at<pa::dPa_control_c>(a)->mSceneResMng = 0;
    return obj;
}
static void makeSafeName(u32 a, u32 text) {
    W(a, text);
    W(a + 4, 0x10051A34);
}
static void heapReport(u32 owner, u32 format) {
    u32 heap = w(owner + 0x128);
    u32 size = pa_getParticleHeapSize(p(heap));
    pa_OSReport(p(format), size);
}
/* 025A7838 */
void pa::dPa_control_c::createCommon(void* data) {
    u32 a = gabi::ea(this), oldMode = 1, heap = (u32)this->mHeap;
    if (heap)
        oldMode = pa_exchangeParticleContext(p(w(0x101F8B4C)), p(heap));
    gabi::Local<u32[2]> firstName, secondName, resNameA, resNameB;
    u32 n1 = gabi::ea(firstName.get()), n2 = gabi::ea(secondName.get());
    makeSafeName(n1, 0x10051D78);
    makeSafeName(n2, 0x10051D84);
    pa_loadParticleTextureBank(p(w(0x101F4F7C)), firstName.get(), secondName.get(), u32(0));
    void* bank = pa_getParticleTextureBank();
    this->mTextureBank = gabi::ea(bank);
    heapReport(a, 0x10051D94);
    heap = (u32)this->mHeap;
    void* rm = pa_operatorNewHeap(u32(12), p(heap), u32(4));
    if (rm) {
        bank = p((u32)this->mTextureBank);
        heap = (u32)this->mHeap;
        rm = pa_JPAResourceManager_ctor(rm, data, p(heap), bank);
    }
    this->mCommonResMng = gabi::ea(rm);
    if (!rm)
        pa_JUTAssertion_showAssert(p(0x10051DA4), u32(0x711), p(0x10051DB4));
    heapReport(a, 0x10051DCC);
    heap = (u32)this->mHeap;
    void* renderControl = pa_operatorNewHeap(u32(16), p(heap), u32(4));
    if (renderControl) {
        u32 r = gabi::ea(renderControl);
        W(r, 0);
        W(r + 4, 0);
        void* list = allocate(p(r + 8), 8);
        if (list) {
            W(gabi::ea(list) + 4, 0);
            W(gabi::ea(list), 0);
        }
    }
    this->mRenderControl = gabi::ea(renderControl);
    u32 sys = w(0x101F8A94), count = w(sys + 16);
    u32 texture = 0;
    if (count > 1)
        texture = w(sys + 20) + 0x5C;
    heap = (u32)this->mHeap;
    pa_initializeParticleRenderControl(renderControl, p(texture), p(heap));
    heap = (u32)this->mHeap;
    void* manager = pa_operatorNewHeap(u32(0x580BB8), p(heap), u32(4));
    if (manager) {
        rm = p((u32)this->mCommonResMng);
        u32 ctrl = (u32)this->mRenderControl;
        heap = (u32)this->mHeap;
        manager =
            pa_JPAEmitterManager_ctor(manager, rm, u32(3000), u32(150), u32(200), p(heap), p(ctrl));
    }
    l_particleEmitterManager = gabi::ea(manager);
    if (!manager)
        pa_JUTAssertion_showAssert(p(0x10051DA4), u32(0x726), p(0x10051DDC));
    heapReport(a, 0x10051DF0);
    makeSafeName(n2, 0x10051D84);
    makeSafeName(n1, 0x10051D78);
    if (pa_loadParticleTextureBank(p(w(0x101F4F7C)), firstName.get(), secondName.get(), u32(0))) {
        u32 res = gabi::ea(pa_getParticleTextureBank()), off = w(res + 0x4C);
        u32 tableBase = off ? res + 0x4C + off : 0;
        u32 table = gabi::ea(pa_findParticleTexture(p(tableBase), p(0x10051E00)));
        if (table) {
            u32 relative = w(table), size = w(table + 4), cursor = relative ? table + relative : 0,
                n = size >> 3;
            while (n--) {
                u32 id = w(cursor), tex = w(cursor + 4);
                cursor += 8;
                if (tex)
                    pa_registerParticleTexture(p((u32)l_particleEmitterManager), p(tex), u16(id));
            }
        }
    }
    heap = (u32)this->mHeap;
    void* modelControl = pa_operatorNewHeap(u32(16), p(heap), u32(4));
    if (modelControl) {
        makeSafeName(gabi::ea(resNameA.get()), 0x10051D70);
        u32 resctl = w(0x101F4F28);
        void* first = pa_dRes_control_getRes(p(resctl), resNameA.get(), u32(0x31));
        makeSafeName(gabi::ea(resNameB.get()), 0x10051D70);
        resctl = w(0x101F4F28);
        void* second = pa_dRes_control_getRes(p(resctl), resNameB.get(), u32(0x30));
        heap = (u32)this->mHeap;
        modelControl = pa_modelControlCtor(modelControl, first, second, p(heap));
    }
    this->mModelControl = gabi::ea(modelControl);
    if (!modelControl)
        pa_JUTAssertion_showAssert(p(0x10051DA4), u32(0x74F), p(0x10051E14));
    heapReport(a, 0x10051E2C);
    for (u32 i = 0; i < 8; i++) {
        u32 r = (u32)this->mCommonResMng;
        u16 id = h(0x101EA50C + i * 2);
        if (pa_JPAResourceManager_getResource(p(w(r + 4)), u32(id)))
            pa_newSimple(this, u32(id), u32(id & 0x4000 ? 4 : 0));
    }
    heapReport(a, 0x10051E3C);
    this->mNumSimpleCommon = (u8)this->mNumSimple;
    pa_applyDefaultParameters(this);
    if (oldMode != 1)
        pa_restoreParticleContext(p(w(0x101F8B4C)), oldMode);
}
/* WWHD glue
*/
