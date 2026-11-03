// HD packet and resource views recovered from this unit's own instructions.
#pragma once
#include "wwhd.h"

namespace j3d {
struct PacketDispatch {
    u8 mPrefix[0x24];
    be<u32> mNotify;
};
struct PacketNotification {
    u8 mPrefix[0xC];
    be<u32> mKind;
};
struct PacketView {
    u8 mDisposer[0xC];
    gptr<PacketDispatch> mMethods;
    gptr<void> mEntry;
    gptr<void> mNext;
    void clearLinks();
    void notifySelected(const PacketNotification*);
    static void destroyComplete(PacketView*,s32);
};
struct ContextMode {
    be<u32> mMode;
    be<u32> mArgument;
    static ContextMode* create(ContextMode*);
};
struct ProjectionView {
    u8 mPrefix[0x20];
    be<f32> mDepth[4];
    f32 projectDepth(const Mtx34*) const;
};
struct ResourceHeader {
    u8 mPrefix[0xC];
    be<u32> mFlags;
    be<u16> mReserved;
    be<s16> mIdentifier;
    be<u16> mReservedTrack;
    be<u16> mTrackCount;
    u8 mTrackPrefix[0x14];
    be<u32> mTracksOffset;
};
struct MaterialTrackBinding {
    be<u32> mStorage;
    be<u8> mChannel;
    u8 mPadding[3];
};
struct MaterialTrackRecord {
    u8 mPrefix[0x10];
    be<u32> mNameLinkOffset;
    u8 mTrailing[8];
};
struct MaterialTrackNameLink { be<u32> mHeader; be<u32> mNameOffset; };
struct MaterialTrackName { u8 mPrefix[6]; be<u8> mChannel; };
struct ResourceHandle;
struct ResourceHeaderCompact {
    u8 mPrefix[0xC];
    be<u16> mFlags;
    u8 mReserved[4];
    be<s16> mIdentifier;
};
struct ResourceView {
    u8 mPrefix[8];
    gptr<void> mArchive;
    gptr<ResourceHeader> mHeader;
    gptr<MaterialTrackBinding> mTracks;
    void setArchive(const gptr<void>*);
    void initializeTracks(const ResourceHandle*);
    u32 transferFlags() const;
    s32 identifier() const;
    u32 alternateTransferFlags() const;
    s32 alternateIdentifier() const;
};
struct CompactResourceView {
    u8 mPrefix[0xC];
    gptr<ResourceHeaderCompact> mHeader;
    u32 transferFlags() const;
    s32 identifier() const;
};
struct ResourceRootHeader { u8 mPrefix[0xC]; be<u32> mDataOffset; };
struct ResourceHandle {
    gptr<void> mHeader;
    void* get() const;
    void* optionalData() const;
};
struct ProjectionOwner {
    u8 mPrefix[0x9C];
    u8 mProjection[0xAC];
    static void destroy(ProjectionOwner*,s32);
};
struct ConstantWord { be<u32> mValue; };
struct ConstantPair { be<u32> mWords[2]; };
struct ColorPairState {
    u8 mPrefix[4];
    be<u32> mPrimary[2];
    be<u32> mSecondary[2];
    be<u32> mSlots[8];
    be<u32> mReserved;
    be<u32> mFlags;
    be<u32> mCount;
    void initialize();
};
struct MaterialSlotState {
    be<u32> mFlags;
    u8 mReserved[4];
    be<u16> mIndices[8];
    be<u32> mPairs[3][2];
    u8 mColorPadding[8];
    be<u32> mColors[4];
    be<u32> mCount;
    void initialize();
};
struct TextureSlotState {
    u8 mPrefix[4];
    be<u32> mSlots[8];
    be<u32> mCount;
    void clear();
};
struct RuntimeRecord { be<u32> mWords[4]; };
struct ExitRecord { be<u32> mWords[3]; };

namespace packet_text {
struct Text { static void destroy(Text*,s32); void assureTerminationImpl(); };
struct TextBuffer {
    gptr<be<u8>> mText;
    be<u32> mMethods;
    be<s32> mCapacity;
    static void destroy(TextBuffer*,s32);
    void assureTerminationImpl();
};
struct OwnedText { static void destroy(OwnedText*,s32); void assureTerminationImpl(); };
struct TemporaryText { static void destroy(TemporaryText*,s32); void assureTerminationImpl(); };
}
namespace material_text { struct Text { static void destroy(Text*,s32); }; }
namespace resource_text { struct Text { static void destroy(Text*,s32); void assureTerminationImpl(); }; }
void initializePacketRuntime();
void initializeProjectionRuntime();
void initializeDepthRuntime();
void initializeResourceRuntime();
void initializeMaterialRuntime();
void initializeFrameRuntime();
void initializeColorRuntime();
}
WWHD_OFFSET(j3d::PacketView,mMethods,0xC);
WWHD_OFFSET(j3d::PacketView,mEntry,0x10);
WWHD_OFFSET(j3d::PacketView,mNext,0x14);
WWHD_SIZE(j3d::PacketView,0x18);
WWHD_SIZE(j3d::ContextMode,8);
WWHD_OFFSET(j3d::ProjectionView,mDepth,0x20);
WWHD_SIZE(j3d::ProjectionView,0x30);
WWHD_OFFSET(j3d::ResourceHeader,mFlags,0xC);
WWHD_OFFSET(j3d::ResourceHeader,mIdentifier,0x12);
WWHD_OFFSET(j3d::ResourceHeader,mTrackCount,0x16);
WWHD_OFFSET(j3d::ResourceHeader,mTracksOffset,0x2C);
WWHD_SIZE(j3d::MaterialTrackBinding,8);
WWHD_SIZE(j3d::MaterialTrackRecord,0x1C);
WWHD_OFFSET(j3d::ResourceHeaderCompact,mIdentifier,0x12);
WWHD_OFFSET(j3d::ResourceView,mArchive,8);
WWHD_OFFSET(j3d::ResourceView,mHeader,0xC);
WWHD_OFFSET(j3d::ColorPairState,mSlots,0x14);
WWHD_OFFSET(j3d::ColorPairState,mFlags,0x38);
WWHD_OFFSET(j3d::ColorPairState,mCount,0x3C);
WWHD_SIZE(j3d::ColorPairState,0x40);
WWHD_OFFSET(j3d::MaterialSlotState,mIndices,8);
WWHD_OFFSET(j3d::MaterialSlotState,mPairs,0x18);
WWHD_OFFSET(j3d::MaterialSlotState,mColors,0x38);
WWHD_OFFSET(j3d::MaterialSlotState,mCount,0x48);
WWHD_OFFSET(j3d::TextureSlotState,mCount,0x24);
WWHD_SIZE(j3d::RuntimeRecord,16);
WWHD_SIZE(j3d::ExitRecord,12);

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_OFFSET(j3d::PacketDispatch,mNotify,0x24);
WWHD_OFFSET(j3d::PacketNotification,mKind,0xC);
WWHD_OFFSET(j3d::MaterialTrackNameLink,mNameOffset,4);
WWHD_OFFSET(j3d::MaterialTrackName,mChannel,6);
WWHD_OFFSET(j3d::CompactResourceView,mHeader,0xC);
WWHD_OFFSET(j3d::ResourceRootHeader,mDataOffset,0xC);
WWHD_OFFSET(j3d::ResourceHandle,mHeader,0);
WWHD_OFFSET(j3d::ProjectionOwner,mProjection,0x9C);
WWHD_SIZE(j3d::ConstantWord,4);
WWHD_SIZE(j3d::ConstantPair,8);
WWHD_OFFSET(j3d::packet_text::TextBuffer,mCapacity,8);
