// Actor adaptor extensions over the merged adaptor layout, from tww object-actor.cpp.
#pragma once
#include "JSystem/JStudio/JST_adaptors.h"
#include "JSystem/JStudio/JST_timeline.h"
#include "JSystem/JStudio/JStudio_JStage/stage-prefixes.h"
namespace JStudio_JStage {
using ActorIDMethod = gabi::ptmf<void (jst_adaptors::StageObject::*)(u32)>;
struct TAdaptor_actor : jst_adaptors::TAdaptor_actor {
    gptr<jst_adaptors::StageObject> mParent;
    be<u32> mParentNodeID;
    gptr<jst_adaptors::StageObject> mRelation;
    be<u32> mRelationNodeID;
    be<u32> mAnimationMode;
    be<u32> mTextureAnimationMode;
    void adaptor_do_begin(const jst_adaptors::TObject*);
    void adaptor_do_end(const jst_adaptors::TObject*);
    void adaptor_do_prepare(const jst_adaptors::TObject*);
    void adaptor_do_data(const jst_adaptors::TObject*, const void*, u32, const void*, u32);
    void setJSG_ID_(const ActorIDMethod&, u32, const void*, u32);
    void adaptor_do_SHAPE(u32, const void*, u32);
    void adaptor_do_ANIMATION(u32, const void*, u32);
    void adaptor_do_ANIMATION_MODE(u32, const void*, u32);
    void adaptor_do_TEXTURE_ANIMATION(u32, const void*, u32);
    void adaptor_do_TEXTURE_ANIMATION_MODE(u32, const void*, u32);
    void adaptor_do_PARENT(u32, const void*, u32);
    void adaptor_do_PARENT_NODE(u32, const void*, u32);
    void adaptor_do_PARENT_ENABLE(u32, const void*, u32);
    void adaptor_do_RELATION(u32, const void*, u32);
    void adaptor_do_RELATION_NODE(u32, const void*, u32);
    void adaptor_do_RELATION_ENABLE(u32, const void*, u32);
};
struct ActorVariablePrefix {
    be<u32> mCount; gptr<void> __vtbl;
    jst_timeline::TVariableValue mValues[14];
    gptr<const StageSystem> mSystem;
};
struct ActorVTable {
    u8 prefix[0x24]; gfn getFlags; u8 gapFlags[4]; gfn setFlags;
    u8 gapParent[0x1C]; gfn setParent; u8 gapRelation[4]; gfn setRelation;
    u8 gapNode[4]; gfn findNode;
    u8 gapTranslation[0xC]; gfn getTranslation;
    u8 gapScaling[0xC]; gfn getScaling;
    u8 gapRotation[0xC]; gfn getRotation;
};
struct PackedMethod { be<s16> adjust, index; };
struct ValueOutput : jst_timeline::TOutput {
    be<s32> mValueIndex;
    gabi::ptmf<void (jst_adaptors::StageObject::*)(f32)> mSetter;
    gabi::ptmf<f32 (jst_adaptors::StageObject::*)()> mGetter;
    void output(f32, jst_adaptors::TAdaptor*) const;
};
struct AnimationFrameOutput : jst_timeline::TOutput {
    be<s32> mValueIndex; be<u32> mModeOffset;
    gabi::ptmf<void (jst_adaptors::StageObject::*)(f32)> mSetter;
    gabi::ptmf<f32 (jst_adaptors::StageObject::*)()> mGetter;
    gabi::ptmf<f32 (jst_adaptors::StageObject::*)()> mMaxGetter;
    void output(f32, jst_adaptors::TAdaptor*) const;
};
WWHD_SIZE(TAdaptor_actor, 0x144);
WWHD_OFFSET(TAdaptor_actor, mParent, 0x12C);
WWHD_OFFSET(TAdaptor_actor, mParentNodeID, 0x130);
WWHD_OFFSET(TAdaptor_actor, mRelation, 0x134);
WWHD_OFFSET(TAdaptor_actor, mRelationNodeID, 0x138);
WWHD_OFFSET(TAdaptor_actor, mAnimationMode, 0x13C);
WWHD_OFFSET(TAdaptor_actor, mTextureAnimationMode, 0x140);
WWHD_SIZE(ActorVariablePrefix, 0x124);
WWHD_OFFSET(ActorVariablePrefix, mValues, 8);
WWHD_OFFSET(ActorVariablePrefix, mSystem, 0x120);
WWHD_OFFSET(ActorVTable, getFlags, 0x24);
WWHD_OFFSET(ActorVTable, setFlags, 0x2C);
WWHD_OFFSET(ActorVTable, setParent, 0x4C);
WWHD_OFFSET(ActorVTable, setRelation, 0x54);
WWHD_OFFSET(ActorVTable, findNode, 0x5C);
WWHD_OFFSET(ActorVTable, getTranslation, 0x6C);
WWHD_OFFSET(ActorVTable, getScaling, 0x7C);
WWHD_OFFSET(ActorVTable, getRotation, 0x8C);
WWHD_SIZE(ValueOutput, 0x18);
WWHD_SIZE(AnimationFrameOutput, 0x24);
WWHD_SIZE(PackedMethod, 4);
}
