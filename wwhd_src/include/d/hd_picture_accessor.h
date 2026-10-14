// HD JPEG encoder access and save-slot summaries. Private decomp-derived source.
#pragma once
#include "d/hd_jpeg.h"
#include "d/hd_player_name.h"
#include "d/hd_save_holders.h"
struct hdPictureDisplayName_c { be<u32> mpText, mVtable, mCapacity; be<u8> mBuffer[8]; };
WWHD_OFFSET(hdPictureDisplayName_c, mpText, 0);
WWHD_OFFSET(hdPictureDisplayName_c, mVtable, 4);
WWHD_OFFSET(hdPictureDisplayName_c, mCapacity, 8);
WWHD_SIZE(hdPictureDisplayName_c, 0x14);
struct hdPictureSlotSummary_c {
    be<u16> mLife, mMaximumLife;
    be<u32> mTime[2];
    hdPictureDisplayName_c mDisplayName;
    be<u8> mStage, mPlayerSlot, mPlayerMode, mPlayerCompletion;
    be<u16> mRupees;
    be<u8> mPlayerState;
    u8 mReserved;
    hdNameString_c mPlayerName;
};
WWHD_OFFSET(hdPictureSlotSummary_c, mLife, 0x0);
WWHD_OFFSET(hdPictureSlotSummary_c, mMaximumLife, 0x2);
WWHD_OFFSET(hdPictureSlotSummary_c, mTime, 0x4);
WWHD_OFFSET(hdPictureSlotSummary_c, mDisplayName, 0xC);
WWHD_OFFSET(hdPictureSlotSummary_c, mPlayerSlot, 0x21);
WWHD_OFFSET(hdPictureSlotSummary_c, mPlayerMode, 0x22);
WWHD_OFFSET(hdPictureSlotSummary_c, mPlayerCompletion, 0x23);
WWHD_OFFSET(hdPictureSlotSummary_c, mRupees, 0x24);
WWHD_OFFSET(hdPictureSlotSummary_c, mPlayerState, 0x26);
WWHD_OFFSET(hdPictureSlotSummary_c, mStage, 0x20);
WWHD_OFFSET(hdPictureSlotSummary_c, mPlayerName, 0x28);
WWHD_SIZE(hdPictureSlotSummary_c, 0x48);
struct hdPictureSlotSummaries_c {
    hdPictureSlotSummary_c mSlots[3];
    be<u8> mDisplayNameBuffer[8];
    void refresh();
    hdPictureSlotSummary_c* slot(u32 index);
};
WWHD_OFFSET(hdPictureSlotSummaries_c, mDisplayNameBuffer, 0xD8);
WWHD_SIZE(hdPictureSlotSummaries_c, 0xE0);
struct hdPictureEncoder_c {
    be<u32> mHeap, mBuffer, mBufferSize, mQuality, mWidth, mHeight, mFormat, mDataSize;
};
WWHD_OFFSET(hdPictureEncoder_c, mHeap, 0);
WWHD_OFFSET(hdPictureEncoder_c, mBuffer, 4);
WWHD_OFFSET(hdPictureEncoder_c, mBufferSize, 8);
WWHD_OFFSET(hdPictureEncoder_c, mQuality, 12);
WWHD_OFFSET(hdPictureEncoder_c, mWidth, 16);
WWHD_OFFSET(hdPictureEncoder_c, mHeight, 20);
WWHD_OFFSET(hdPictureEncoder_c, mFormat, 24);
WWHD_OFFSET(hdPictureEncoder_c, mDataSize, 28);
WWHD_SIZE(hdPictureEncoder_c, 0x20);
struct hdPictureDimensions_c { be<u32> mWidth, mHeight, mFormat; };
WWHD_OFFSET(hdPictureDimensions_c, mWidth, 0);
WWHD_OFFSET(hdPictureDimensions_c, mHeight, 4);
WWHD_OFFSET(hdPictureDimensions_c, mFormat, 8);
WWHD_SIZE(hdPictureDimensions_c, 12);
struct hdAccessorHeap_c;
struct hdPictureAccessor_c {
    gptr<hdAccessorHeap_c> mpHeap;
    be<u8> mSucceeded;
    u8 mReserved[3];
    void destroy(u32 flags);
    void encode(u32 image, hdPictureDimensions_c* dimensions, u32 quality, u32 slot);
    u32 setSubject(u32 slot, u32 subject);
    u32 setLabel(u32 slot, u32 label);
};
WWHD_OFFSET(hdPictureAccessor_c, mpHeap, 0);
WWHD_OFFSET(hdPictureAccessor_c, mSucceeded, 4);
WWHD_SIZE(hdPictureAccessor_c, 8);
struct hdAccessorHeapVtable_c { u8 mReserved[0x24]; be<u32> mDestroy; };
WWHD_OFFSET(hdAccessorHeapVtable_c, mDestroy, 0x24);
struct hdAccessorHeap_c { u8 mDisposer[12]; gptr<hdAccessorHeapVtable_c> mVtable; };
WWHD_OFFSET(hdAccessorHeap_c, mVtable, 12);
void* accessorAllocate(u32);
void accessorCopy(void*, const void*, u32, u32);
void accessorReadSaveBytes(void*, u32, u32, u32);
using hdPictureSummaryConstructor = hdPictureSlotSummary_c* (*)(hdPictureSlotSummary_c*);
hdPictureSlotSummary_c* constructPictureSlotSummary(hdPictureSlotSummary_c*);
void accessorConstructArray(void*, u32, u32, hdPictureSummaryConstructor);
hdAccessorHeap_c* accessorCreateHeap(u32, hdJpegSafeString_c*, u32, u32, u32);
void accessorFree(void*);
void accessorFormat(hdPictureDisplayName_c*, const char*);
hdSavePicture_l* accessorCardPictures(hdSaveHolders_c*);
hdSaveName_l* accessorCardName(hdSaveHolders_c*, u32);
u32 accessorPictureData(hdSavePicture_l*, u32);
hdJpeg_c* accessorPictureRecord(hdSavePicture_l*, u32);
u32 accessorSetSubject(hdSavePicture_l*, u32, u32);
u32 accessorSetLabel(hdSavePicture_l*, u32, u32);
void accessorEncoderConstruct(hdPictureEncoder_c*);
void accessorEncoderInitialize(hdPictureEncoder_c*, hdAccessorHeap_c*, u32);
void accessorEncoderDimensions(hdPictureEncoder_c*, u32, u32, u32);
void accessorEncoderQuality(hdPictureEncoder_c*, u32);
u32 accessorEncoderEncode(hdPictureEncoder_c*, u32, u32, u32, u32);
void accessorEncoderFinish(hdPictureEncoder_c*);
void accessorEncoderDestroy(hdPictureEncoder_c*, u32);
