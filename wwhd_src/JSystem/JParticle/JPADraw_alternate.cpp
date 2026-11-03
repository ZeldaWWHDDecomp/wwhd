// HD: alternate passes partition a GC-style intrusive walk by traversal ordinal.
#include "JSystem/JParticle/JPADraw_passes.h"
using namespace JPADrawExtension;
void JPADrawAlternateSetChildClipboard(JPADraw_l*,s32);
void JPADrawAlternateSetParticleClipboard(JPADraw_l*,s32);
void JPADrawAlternateUpdateParticle(JPABaseParticle*,void*);
void JPADrawHDUploadUniform(JPADraw_l*,u32);
void JPADrawHDUpdateFrameTexture(JPADraw_l*,JPABaseParticle*);
/* 0282C448 */
void JPADraw_l::zDrawAlternateChild(u32 parity) {
    mDrawFlags = u8(mDrawFlags) | 2;
    JPADrawAlternateSetChildClipboard(this,parity);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto list = &emitter->particles().mChild;
    dc.mpActiveParticles = gabi::ea(list);
    bool reverse = baseShape()->getListOrder() != 0;
    u32 ordinal = 0;
    auto node = reverse ? list->mpTail.get() : list->mpHead.get();
    while (node) {
        ++ordinal;
        if ((ordinal & 1) == parity) {
            auto particle = node->mpObject.get();
            if (!u8(particle->mHidden)) {
                particle->mViewIndex = parity;
                particle->mNeedsVertexUpdate = 1;
                for (s32 i = 0; i < u8(execution().execChldVisNum); ++i)
                    reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecChldVis[i].get())->exec(&dc,particle);
            }
        }
        node = reverse ? node->mpPrev.get() : node->mpNext.get();
    }
}
/* 0282C5CC */
void JPADraw_l::zDrawAlternateParticle(u32 parity) {
    mDrawFlags = u8(mDrawFlags) & ~2;
    JPADrawAlternateSetParticleClipboard(this,parity);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto list = &emitter->particles().mActive;
    dc.mpActiveParticles = gabi::ea(list);
    bool reverse = baseShape()->getListOrder() != 0;
    u32 ordinal = 0;
    auto node = reverse ? list->mpTail.get() : list->mpHead.get();
    while (node) {
        ++ordinal;
        if ((ordinal & 1) == parity) {
            auto particle = node->mpObject.get();
            if (!u8(particle->mHidden)) {
                particle->mViewIndex = parity;
                particle->mNeedsVertexUpdate = 1;
                for (s32 i = 0; i < u8(execution().execPtclVisNum); ++i)
                    reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecPtclVis[i].get())->exec(&dc,particle);
            }
        }
        node = reverse ? node->mpPrev.get() : node->mpNext.get();
    }
}
/* 0282C804 */
void JPADraw_l::drawAlternateChild(u32 parity) {
    mDrawFlags = u8(mDrawFlags) | 2;
    JPADrawAlternateSetChildClipboard(this,parity);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto list = &emitter->particles().mChild;
    dc.mpActiveParticles = gabi::ea(list);
    bool reverse = baseShape()->getListOrder() != 0;
    u32 ordinal = 0;
    auto node = reverse ? list->mpTail.get() : list->mpHead.get();
    while (node) {
        ++ordinal;
        if ((ordinal & 1) == parity) {
            auto particle = node->mpObject.get();
            if (!u8(particle->mHidden)) {
                particle->mNeedsVertexUpdate = !baseShape()->isEnableAnmTone();
                JPADrawAlternateUpdateParticle(particle,dc.pbe.get());
                particle->mViewIndex = parity;
                for (s32 i = 0; i < u8(execution().execChldVisNum); ++i)
                    reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecChldVis[i].get())->exec(&dc,particle);
            }
        }
        node = reverse ? node->mpPrev.get() : node->mpNext.get();
    }
}
/* 0282C9D0 */
void JPADraw_l::drawAlternateParticle(u32 parity) {
    mDrawFlags = u8(mDrawFlags) & ~2;
    JPADrawAlternateSetParticleClipboard(this,parity);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto list = &emitter->particles().mActive;
    dc.mpActiveParticles = gabi::ea(list);
    bool reverse = baseShape()->getListOrder() != 0;
    u32 ordinal = 0;
    auto node = reverse ? list->mpTail.get() : list->mpHead.get();
    while (node) {
        ++ordinal;
        if ((ordinal & 1) == parity) {
            auto particle = node->mpObject.get();
            if (!u8(particle->mHidden)) {
                particle->mNeedsVertexUpdate = !baseShape()->isEnableAnmTone();
                JPADrawAlternateUpdateParticle(particle,dc.pbe.get());
                particle->mViewIndex = parity;
                for (s32 i = 0; i < u8(execution().execPtclVisNum); ++i)
                    reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecPtclVis[i].get())->exec(&dc,particle);
            }
        }
        node = reverse ? node->mpPrev.get() : node->mpNext.get();
    }
}
/* 0282D4E8 */
void JPADraw_l::drawHDChild() {
    JPADrawHDUploadUniform(this,1);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    dc.mpActiveParticles = gabi::ea(&emitter->particles().mChild);
    for (s32 i = 0; i < u8(execution().hdExecEmtrCVisNum); ++i)
        reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpHDExecEmtrCVis[i].get())->exec(&dc);
    emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto list = &emitter->particles().mChild;
    bool reverse = baseShape()->getListOrder() != 0;
    auto node = reverse ? list->mpTail.get() : list->mpHead.get();
    while (node) {
        auto particle = node->mpObject.get();
        if (!u8(particle->mHidden)) {
            JPADrawHDUpdateFrameTexture(this,particle);
            for (s32 i = 0; i < u8(execution().hdExecChldVisNum); ++i)
                reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpHDExecChldVis[i].get())->exec(&dc,particle);
        }
        node = reverse ? node->mpPrev.get() : node->mpNext.get();
    }
}
/* 0282D754 */
void JPADraw_l::drawHDParticle() {
    JPADrawHDUploadUniform(this,0);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    dc.mpActiveParticles = gabi::ea(&emitter->particles().mActive);
    for (s32 i = 0; i < u8(execution().hdExecEmtrPVisNum); ++i)
        reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpHDExecEmtrPVis[i].get())->exec(&dc);
    emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto list = &emitter->particles().mActive;
    bool reverse = baseShape()->getListOrder() != 0;
    auto node = reverse ? list->mpTail.get() : list->mpHead.get();
    while (node) {
        auto particle = node->mpObject.get();
        if (!u8(particle->mHidden)) {
            JPADrawHDUpdateFrameTexture(this,particle);
            for (s32 i = 0; i < u8(execution().hdExecPtclVisNum); ++i)
                reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpHDExecPtclVis[i].get())->exec(&dc,particle);
        }
        node = reverse ? node->mpPrev.get() : node->mpNext.get();
    }
}
/* WWHD glue
ext 0282AC2C void JPADrawAlternateSetChildClipboard(JPADraw_l*,s32)
ext 0282B280 void JPADrawAlternateSetParticleClipboard(JPADraw_l*,s32)
ext 028255F8 void JPADrawAlternateUpdateParticle(JPABaseParticle*,void*)
ext 0282D248 void JPADrawHDUploadUniform(JPADraw_l*,u32)
ext 0282D3A0 void JPADrawHDUpdateFrameTexture(JPADraw_l*,JPABaseParticle*)
*/
