// Particle adaptor extensions over the merged view, following tww object-particle.cpp.
#pragma once
#include "JSystem/JStudio/JST_adaptors.h"
#include "JSystem/JStudio/JST_timeline.h"
namespace JStudio_JParticle {
struct TAdaptor_particle : jst_adaptors::TAdaptor_particle {
    u8 tailPadding[3];
    struct TJPACallback_ : jst_adaptors::TAdaptor_particle::TJPACallback_ {
        void executeAfter(jst_adaptors::Emitter*);
        void draw(jst_adaptors::Emitter*);
        void drawAfter(jst_adaptors::Emitter*);
    };
    struct TVVOOn_BEGIN_FADE_IN_ : jst_timeline::TOutput {};
    struct TVVOOn_END_FADE_OUT_ : jst_timeline::TOutput {};
    void adaptor_do_prepare(const jst_adaptors::TObject*);
    void adaptor_do_end(const jst_adaptors::TObject*);
    void adaptor_do_PARTICLE(u32, const void*, u32);
    void adaptor_do_PARENT(u32, const void*, u32);
    void adaptor_do_PARENT_NODE(u32, const void*, u32);
    void adaptor_do_PARENT_ENABLE(u32, const void*, u32);
};
struct ParticleVariablePrefix { be<u32> mCount; gptr<void> __vtbl; jst_timeline::TVariableValue mValues[20]; };
struct ParentSystemVTable { u8 prefix[0x6C]; gfn findObject; };
struct ParentSystem { gptr<ParentSystemVTable> __vtbl; };
struct ParentNodeVTable { u8 prefix[0x5C]; gfn getNodeID; };
struct ParticleParentPrefix {
    gptr<const ParentSystem> mSystem;
    gptr<jst_adaptors::StageObject> mObject;
    be<u32> mNodeID;
    be<u8> mEnabled;
};
WWHD_SIZE(TAdaptor_particle, 0x1D0);
WWHD_SIZE(TAdaptor_particle::TJPACallback_, 0xC);
WWHD_SIZE(ParticleVariablePrefix, 0x198);
WWHD_OFFSET(ParticleVariablePrefix, mValues, 8);
WWHD_SIZE(ParticleParentPrefix, 0xD);
WWHD_OFFSET(ParticleParentPrefix, mObject, 4);
WWHD_OFFSET(ParticleParentPrefix, mNodeID, 8);
WWHD_OFFSET(ParticleParentPrefix, mEnabled, 0xC);
WWHD_OFFSET(ParentSystemVTable, findObject, 0x6C);
WWHD_OFFSET(ParentNodeVTable, getNodeID, 0x5C);
}
