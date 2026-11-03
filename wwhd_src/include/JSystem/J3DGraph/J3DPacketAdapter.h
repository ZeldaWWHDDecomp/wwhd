// HD packet storage, sized by the constructors in this unit.
#pragma once
#include "sead/HeapRegistry.h"
struct J3DModel;
namespace j3d {
struct ShapeResource_l;
struct PacketSubmission_l;
struct ShapeDrawInput_l;
struct PacketObject : sead::Disposer {
    gptr<void> mEntry;
    gptr<void> mNext;
    be<u32> mState;
    sead::FixedSafeText<104> mLocalName;
    gptr<sead::FixedSafeText<104>> mName;
    be<u32> mEntryCount;
    static PacketObject* create(PacketObject*);
    static void destroy(PacketObject*,s32);
};
struct ShapePacket : PacketObject {
    gptr<void> mResource;
    gptr<void> mModel;
    be<u32> mBufferType;
    be<u8> mDirty;
    u8 mPadding[3];
    gptr<void> mProgram;
    static ShapePacket* create(ShapePacket*);
    void bindResources(ShapeResource_l*,J3DModel*);
    void bindProgram(PacketSubmission_l*);
    void prepareDrawList();
    void submitShape(ShapeDrawInput_l*);
    void prepareShape(ShapeDrawInput_l*);
};
struct PacketCounter { be<u32> mCount; };
struct PacketStringHeap { gptr<sead::RegistryHeap> mHeap; };
}
WWHD_OFFSET(j3d::PacketObject,mEntry,0x10);
WWHD_OFFSET(j3d::PacketObject,mNext,0x14);
WWHD_OFFSET(j3d::PacketObject,mState,0x18);
WWHD_OFFSET(j3d::PacketObject,mLocalName,0x1C);
WWHD_OFFSET(j3d::PacketObject,mName,0x90);
WWHD_OFFSET(j3d::PacketObject,mEntryCount,0x94);
WWHD_SIZE(j3d::PacketObject,0x98);
WWHD_OFFSET(j3d::ShapePacket,mResource,0x98);
WWHD_OFFSET(j3d::ShapePacket,mModel,0x9C);
WWHD_OFFSET(j3d::ShapePacket,mBufferType,0xA0);
WWHD_OFFSET(j3d::ShapePacket,mProgram,0xA8);
WWHD_SIZE(j3d::ShapePacket,0xAC);

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_SIZE(j3d::PacketCounter,4);
WWHD_SIZE(j3d::PacketStringHeap,4);
