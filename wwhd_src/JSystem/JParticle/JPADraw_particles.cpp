// Intrusive-list draw passes start from tww JPADraw.cpp; callbacks may change list links and counts.
#include "JSystem/JParticle/JPADraw_passes.h"
using namespace JPADrawExtension;
void JPADrawSetChildClipboard(JPADraw_l*,s32);
void JPADrawSetParticleClipboard(JPADraw_l*,s32);
void JPADrawUpdateParticle(JPABaseParticle*,void*);
/* 0282B0D4 */
void JPADraw_l::zDrawChild() {
    mDrawFlags = u8(mDrawFlags) | 2;
    JPADrawSetChildClipboard(this,-1);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    dc.mpActiveParticles = gabi::ea(&emitter->particles().mChild);
    for (s32 i = 0; i < u8(execution().execEmtrCVisNum); ++i)
        reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrCVis[i].get())->exec(&dc);
    emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto list = &emitter->particles().mChild;
    bool reverse = baseShape()->getListOrder() != 0;
    auto node = reverse ? list->mpTail.get() : list->mpHead.get();
    while (node) {
        auto particle = node->mpObject.get();
        if (!u8(particle->mHidden)) {
            particle->mViewIndex = -1;
            particle->mNeedsVertexUpdate = 1;
            for (s32 i = 0; i < u8(execution().execChldVisNum); ++i)
                reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecChldVis[i].get())->exec(&dc,particle);
        }
        node = reverse ? node->mpPrev.get() : node->mpNext.get();
    }
}
/* 0282B898 */
void JPADraw_l::zDrawParticle() {
    mDrawFlags = u8(mDrawFlags) & ~2;
    JPADrawSetParticleClipboard(this,-1);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    dc.mpActiveParticles = gabi::ea(&emitter->particles().mActive);
    for (s32 i = 0; i < u8(execution().execEmtrPVisNum); ++i)
        reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrPVis[i].get())->exec(&dc);
    emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto list = &emitter->particles().mActive;
    bool reverse = baseShape()->getListOrder() != 0;
    auto node = reverse ? list->mpTail.get() : list->mpHead.get();
    while (node) {
        auto particle = node->mpObject.get();
        if (!u8(particle->mHidden)) {
            particle->mViewIndex = -1;
            particle->mNeedsVertexUpdate = 1;
            for (s32 i = 0; i < u8(execution().execPtclVisNum); ++i)
                reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecPtclVis[i].get())->exec(&dc,particle);
        }
        node = reverse ? node->mpPrev.get() : node->mpNext.get();
    }
}
/* 0282BAE0 */
void JPADraw_l::drawChild() {
    mDrawFlags = u8(mDrawFlags) | 2;
    JPADrawSetChildClipboard(this,-1);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    dc.mpActiveParticles = gabi::ea(&emitter->particles().mChild);
    for (s32 i = 0; i < u8(execution().execEmtrCVisNum); ++i)
        reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrCVis[i].get())->exec(&dc);
    emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto list = &emitter->particles().mChild;
    bool reverse = baseShape()->getListOrder() != 0;
    auto node = reverse ? list->mpTail.get() : list->mpHead.get();
    while (node) {
        auto particle = node->mpObject.get();
        if (!u8(particle->mHidden)) {
            particle->mNeedsVertexUpdate = !baseShape()->isEnableAnmTone();
            JPADrawUpdateParticle(particle,dc.pbe.get());
            particle->mViewIndex = -1;
            for (s32 i = 0; i < u8(execution().execChldVisNum); ++i)
                reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecChldVis[i].get())->exec(&dc,particle);
        }
        node = reverse ? node->mpPrev.get() : node->mpNext.get();
    }
}
/* 0282BCD4 */
void JPADraw_l::drawParticle() {
    mDrawFlags = u8(mDrawFlags) & ~2;
    JPADrawSetParticleClipboard(this,-1);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    dc.mpActiveParticles = gabi::ea(&emitter->particles().mActive);
    for (s32 i = 0; i < u8(execution().execEmtrPVisNum); ++i)
        reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrPVis[i].get())->exec(&dc);
    emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto list = &emitter->particles().mActive;
    bool reverse = baseShape()->getListOrder() != 0;
    auto node = reverse ? list->mpTail.get() : list->mpHead.get();
    while (node) {
        auto particle = node->mpObject.get();
        if (!u8(particle->mHidden)) {
            particle->mNeedsVertexUpdate = !baseShape()->isEnableAnmTone();
            JPADrawUpdateParticle(particle,dc.pbe.get());
            particle->mViewIndex = -1;
            for (s32 i = 0; i < u8(execution().execPtclVisNum); ++i)
                reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecPtclVis[i].get())->exec(&dc,particle);
        }
        node = reverse ? node->mpPrev.get() : node->mpNext.get();
    }
}
/* WWHD glue
ext 0282AC2C void JPADrawSetChildClipboard(JPADraw_l*,s32)
ext 0282B280 void JPADrawSetParticleClipboard(JPADraw_l*,s32)
ext 028255F8 void JPADrawUpdateParticle(JPABaseParticle*,void*)
*/
