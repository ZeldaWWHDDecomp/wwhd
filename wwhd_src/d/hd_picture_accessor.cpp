// HD photograph encoding into save records and three-slot summaries. Private source.
#include "d/hd_picture_statics.h"
#include "d/hd_picture_accessor.h"
#include "d/d_menu_save.h"
#include "d/hd_save_info_holder.h"
static u32 sAccessorSafeStringVtable;
static hdSave::SaveMgr* sSaveManager;
static hdSaveInfoHolder_c* sGameInfo;
static hdPictureStaticState_c sPictureAccessorHeaderState;
static hdPictureStaticTokens_c sPictureAccessorHeaderTokens;
static const hdPictureAngles_c sPictureAccessorHeaderAngles;
/* 02726FA0 */
void accessorReadSaveBytes(void* output, u32 size, u32 offset, u32 slot) {
    hdSave::SlotData* slots = gabi::at<hdSave::SlotData>(sSaveManager->mpSaveData);
    const u8* source = reinterpret_cast<const u8*>(&slots[slot]) + offset;
    accessorCopy(output, source, size, 0);
}
/* 02726FC4 */
void hdPictureSlotSummaries_c::refresh() {
    for (u32 i = 0; i < 3; i++) {
        hdPictureSlotSummary_c* dest = &mSlots[i];
        accessorReadSaveBytes(&dest->mLife, 2, 0, i);
        accessorReadSaveBytes(&dest->mMaximumLife, 2, 2, i);
        accessorReadSaveBytes(dest->mTime, 8, 0x18, i);
        accessorReadSaveBytes(mDisplayNameBuffer, 8, 0x30, i);
        accessorReadSaveBytes(&dest->mStage, 1, 0x38, i);
        accessorReadSaveBytes(&dest->mPlayerSlot, 1, 0xB5, i);
        accessorReadSaveBytes(&dest->mPlayerMode, 1, 0xBC, i);
        accessorReadSaveBytes(&dest->mPlayerCompletion, 1, 0xBD, i);
        accessorReadSaveBytes(&dest->mRupees, 2, 0x153, i);
        accessorReadSaveBytes(&dest->mPlayerState, 1, 0x19B, i);
        hdSaveInfoHolder_c* gameInfo = sGameInfo;
        // GHS keeps the game-info base live through the name lookup and virtual calls.
        gabi::cpu->r[10] = gabi::ea(gameInfo);
        hdSaveName_l* source = accessorCardName(&gameInfo->mHolders, i);
        u8* output = dest->mPlayerName.mpText;
        u32 firstTarget = source->mVtable->mEnsureTerminated;
        gabi::call_ptr<void>(firstTarget, source, firstTarget);
        be<u16>* cursor = source->mpText.get();
        u32 firstCharacter = *cursor;
        u32 lastCharacter = gabi::cpu->r[6];
        s32 length = 0;
        if (firstCharacter != 0) {
            do {
                length++;
                cursor++;
                if (length > 0x40000) { length = 0; break; }
                lastCharacter = *cursor;
            } while (lastCharacter != 0);
        }
        s32 capacity = dest->mPlayerName.mCapacity;
        hdSaveNameVtable_l* vtable = source->mVtable;
        u32 target = vtable->mEnsureTerminated;
        if (length >= capacity) length = capacity - 1;
        // GHS leaves the scan's character registers live at this virtual call.
        gabi::call_ptr<void>(target, source, gabi::cpu->r[4], firstCharacter,
                            lastCharacter, vtable, cursor, target, gabi::cpu->r[10]);
        accessorCopy(output, source->mpText.get(), u32(length) * 2, 0);
        reinterpret_cast<be<u16>*>(output)[length] = 0;
        accessorFormat(&dest->mDisplayName, reinterpret_cast<char*>(mDisplayNameBuffer));
    }
}
/* 02727190 */
static hdPictureSlotSummaries_c* constructSlotSummaries(hdPictureSlotSummaries_c* self) {
    if (!self) {
        self = static_cast<hdPictureSlotSummaries_c*>(accessorAllocate(sizeof(hdPictureSlotSummaries_c)));
        if (!self) return NULL;
    }
    accessorConstructArray(self, 3, sizeof(hdPictureSlotSummary_c), constructPictureSlotSummary);
    self->refresh(); return self;
}
/* 027271F0 */
hdPictureSlotSummary_c* hdPictureSlotSummaries_c::slot(u32 index) {
    return index < 3 ? &mSlots[index] : NULL;
}
/* 0272720C */
static hdPictureAccessor_c* constructPictureAccessor(hdPictureAccessor_c* self, u32 size, u32 heap) {
    if (!self) {
        self = static_cast<hdPictureAccessor_c*>(accessorAllocate(sizeof(hdPictureAccessor_c)));
        if (!self) return NULL;
    }
    hdJpegSafeString_c name;
    name.mpText = gabi::guest_str("PictureAccessor"); name.mVtable = gabi::ea(&sAccessorSafeStringVtable);
    self->mpHeap = accessorCreateHeap(heap, &name, size, 1, 1);
    self->mSucceeded = 0;
    return self;
}
/* 027272A0 */
void hdPictureAccessor_c::destroy(u32 flags) {
    if (this == NULL) return;
    hdAccessorHeap_c* heap = mpHeap.get();
    u32 target = heap->mVtable->mDestroy;
    gabi::call_ptr<void>(target, heap);
    if (flags & 1) accessorFree(this);
}
/* 027272FC */
void hdPictureAccessor_c::encode(u32 image, hdPictureDimensions_c* dimensions, u32 quality, u32 slot) {
    hdPictureEncoder_c encoder;
    accessorEncoderConstruct(&encoder);
    accessorEncoderInitialize(&encoder, mpHeap, 0);
    u32 height = dimensions->mHeight;
    u32 width = dimensions->mWidth;
    u32 format = dimensions->mFormat;
    accessorEncoderDimensions(&encoder, width, height, format);
    accessorEncoderQuality(&encoder, quality);
    hdSavePicture_l* store = accessorCardPictures(&sGameInfo->mHolders);
    u32 data = accessorPictureData(store, slot);
    u32 result = accessorEncoderEncode(&encoder, data, 0x50000, image, 0);
    mSucceeded = result;
    if (result) {
        store = accessorCardPictures(&sGameInfo->mHolders);
        accessorPictureRecord(store, slot)->markSaved();
        store = accessorCardPictures(&sGameInfo->mHolders);
        accessorPictureRecord(store, slot)->enable();
        store = accessorCardPictures(&sGameInfo->mHolders);
        accessorPictureRecord(store, slot)->setDataSize(encoder.mDataSize);
        store = accessorCardPictures(&sGameInfo->mHolders);
        accessorPictureRecord(store, slot)->updateChecksum();
    }
    accessorEncoderFinish(&encoder);
    accessorEncoderDestroy(&encoder, 2);
}
/* 02727420 */
u32 hdPictureAccessor_c::setSubject(u32 slot, u32 subject) {
    hdSavePicture_l* store = accessorCardPictures(&sGameInfo->mHolders);
    return accessorSetSubject(store, slot, subject);
}
/* 02727470 */
u32 hdPictureAccessor_c::setLabel(u32 slot, u32 label) {
    hdSavePicture_l* store = accessorCardPictures(&sGameInfo->mHolders);
    return accessorSetLabel(store, slot, label);
}
/* 027274C0 */
static void initializePictureAccessorStatics() {
    initializePictureHeaderStatics(&sPictureAccessorHeaderState, &sPictureAccessorHeaderTokens, &sPictureAccessorHeaderAngles);
}
/* WWHD glue
 data sAccessorSafeStringVtable 10140DB0 nocheck
 data sPictureAccessorHeaderState 1049F868
 data sPictureAccessorHeaderTokens 101F8604
 data sPictureAccessorHeaderAngles 10140E50
 data sSaveManager 101F852C
 data sGameInfo 101F84DC
 str 10140E40 "PictureAccessor"
 ext 0273AD10 void* accessorAllocate(u32)
 ext C0009988 void accessorCopy(void*, const void*, u32, u32)
 ext 028EFFD0 void accessorConstructArray(void*, u32, u32, hdPictureSummaryConstructor)
 ext 027275E0 hdPictureSlotSummary_c* constructPictureSlotSummary(hdPictureSlotSummary_c*) callback
 ext 02754B38 hdAccessorHeap_c* accessorCreateHeap(u32, hdJpegSafeString_c*, u32, u32, u32)
 ext 0273AF40 void accessorFree(void*)
 ext 02759C28 void accessorFormat(hdPictureDisplayName_c*, const char*)
 ext 02720144 hdSavePicture_l* accessorCardPictures(hdSaveHolders_c*)
 ext 02720154 hdSaveName_l* accessorCardName(hdSaveHolders_c*, u32)
 ext 02726164 u32 accessorPictureData(hdSavePicture_l*, u32)
 ext 0272609C hdJpeg_c* accessorPictureRecord(hdSavePicture_l*, u32)
 ext 02726100 u32 accessorSetSubject(hdSavePicture_l*, u32, u32)
 ext 02726144 u32 accessorSetLabel(hdSavePicture_l*, u32, u32)
 ext 02036AB8 void accessorEncoderConstruct(hdPictureEncoder_c*)
 ext 02036B30 void accessorEncoderInitialize(hdPictureEncoder_c*, hdAccessorHeap_c*, u32)
 ext 02036C28 void accessorEncoderDimensions(hdPictureEncoder_c*, u32, u32, u32)
 ext 02036C38 void accessorEncoderQuality(hdPictureEncoder_c*, u32)
 ext 02036C40 u32 accessorEncoderEncode(hdPictureEncoder_c*, u32, u32, u32, u32)
 ext 02036BBC void accessorEncoderFinish(hdPictureEncoder_c*)
 ext 02036B1C void accessorEncoderDestroy(hdPictureEncoder_c*, u32)
*/
