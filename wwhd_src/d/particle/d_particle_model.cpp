/* Particle control and callbacks; verified HD behavior, GameCube naming. */
#include "d/particle/d_particle_local.h"
using namespace pa;
// Preserve raw lighting transfers; the final copied float uses the reference's load conversion.
static void copyTevLighting(u32 src, u32 dst, u32 quietMask = 0) {
    for (u32 i = 0; i < 24; i += 4) {
        if (quietMask & (1u << (i / 4)))
            F(dst + i, f(src + i));
        else
            W(dst + i, w(src + i));
    };
    for (u32 i = 24; i < 28; i++)
        B(dst + i, b(src + i));
    for (u32 i = 28; i < 36; i += 2)
        H(dst + i, h(src + i));
    for (u32 i = 36; i < 68; i += 4) {
        if (quietMask & (1u << (i / 4)))
            F(dst + i, f(src + i));
        else
            W(dst + i, w(src + i));
    };
}
/* 025A3BDC */
void* dPa_J3DModelEmitterCtor(void* obj, void* emitter, void* modelA, void* modelB, void* tev,
                              void* pattern, u32 frame, u32 mode) {
    obj = allocate(obj, 0x1F4);
    if (!obj)
        return obj;
    u32 a = gabi::ea(obj), e = gabi::ea(emitter), t = gabi::ea(tev);
    pa_modelEmitterCtor(obj);
    gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->__vtbl = gabi::ea(l_particleModelEmitterVtable);
    copyTevLighting(0x1016E414, a + 0x2C);
    copyTevLighting(0x1016E414, a + 0xEC);
    copyTevLighting(0x1016E414, a + 0x170);
    gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->mpBaseEmitter = e;
    gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->modelData = gabi::ea(modelA);
    gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->hdModelData = gabi::ea(modelB);
    gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x26 = mode;
    copyTevLighting(t, a + 0x2C);
    for (u32 i = 0x84; i < 0x90; i += 4)
        W(a + 0x2C + i, w(t + i));
    for (u32 i = 0x90; i < 0x98; i += 2)
        H(a + 0x2C + i, h(t + i));
    copy(t + 0x98, a + 0xC4, 4);
    copy(t + 0x9C, a + 0xC8, 4);
    for (u32 i = 0xA0; i < 0xA8; i += 2)
        H(a + 0x2C + i, h(t + i));
    for (u32 i = 0xA8; i < 0xB4; i += 4)
        W(a + 0x2C + i, w(t + i));
    for (u32 i = 0xB4; i <= 0xBC; i++)
        B(a + 0x2C + i, b(t + i));
    copyTevLighting(t + 0xC0, a + 0xEC);
    copyTevLighting(t + 0x144, a + 0x170, 1u << 16);
    gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x1c = gabi::ea(pattern);
    if (!pattern) {
        gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x20 = 0;
        gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->mpHeap = 0;
    } else {
        void* heap = pa_mDoExt_createSolidHeapToCurrent(u32(0), nullptr, u32(0));
        gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->mpHeap = gabi::ea(heap);
        if (heap) {
            void* animation = pa_operatorNewHeap(u32(0x74), heap, u32(4));
            if (animation)
                animation = pa_mDoExt_btpAnm_ctor(animation);
            gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x20 = gabi::ea(animation);
            if (animation) {
                u32 data = (u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->modelData,
                    pat = (u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x1c;
                pa_mDoExt_btpAnm_init(animation, p(data), p(pat), u32(1), u32(2), u32(0), s32(-1),
                                      u32(0), u32(0), (f32)l_particleOne);
                gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->field_0x24 = frame;
            }
            pa_mDoExt_restoreCurrentHeap();
            pa_mDoExt_adjustSolidHeap(p((u32)gabi::at<pa::dPa_J3DmodelEmitter_c>(a)->mpHeap));
        }
    }
    gabi::at<pa::JPABaseEmitter>(e)->mFlags =
        (u32)gabi::at<pa::JPABaseEmitter>(e)->mFlags | JPAEmtrStts_Immortal;
    return obj;
}
/* WWHD glue
*/
