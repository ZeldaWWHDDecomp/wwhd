// HD JPEG photograph operations. Private decomp-derived source.
#include "d/hd_picture_statics.h"
#include "d/hd_jpeg.h"
static u32 sJpegSafeStringVtable;
static u32 sResourceManager;
struct hdJpegNameTable_c { be<u32> mNames[7]; };
static hdJpegNameTable_c sJpegNames;
struct hdJpegLabelTable_c { be<u8> mLabels[7]; };
static hdJpegLabelTable_c sJpegLabels;
static hdPictureStaticState_c sJpegHeaderState;
static hdPictureStaticTokens_c sJpegHeaderTokens;
static const hdPictureAngles_c sJpegHeaderAngles;
/* 027256FC */
void hdJpeg_c::initializeHeader() {
    mDataSize = 0; mChecksum = 0; mSaved = 0; mLabel = 0;
    mSubject = 0; mState = 0; mValid = 0;
}
/* 02725720 */
static hdJpegHeader_c* constructJpegHeader(hdJpegHeader_c* self) {
    if (self == NULL) {
        self = static_cast<hdJpegHeader_c*>(jpegAllocate(sizeof(hdJpegHeader_c)));
        if (self == NULL) return NULL;
    }
    ((hdJpeg_c*)self)->initializeHeader();
    return self;
}
/* 02725758 */
void hdJpeg_c::clearHeader() { initializeHeader(); mValid = 1; }
/* 02725780 */
static hdJpeg_c* constructJpeg(hdJpeg_c* self) {
    if (self == NULL) {
        self = static_cast<hdJpeg_c*>(jpegAllocate(sizeof(hdJpeg_c)));
        if (self == NULL) return NULL;
    }
    constructJpegHeader((hdJpegHeader_c*)self);
    jpegFill(self->mJpegData, 0, sizeof(self->mJpegData));
    return self;
}
/* 027257DC */
void hdJpeg_c::initialize() {
    jpegFill(mJpegData, 0, sizeof(mJpegData)); initializeHeader();
}
/* 0272581C */
u32 hdJpeg_c::calculateChecksum() {
    enum { ChecksumBufferSize = 0x50004 };
    u32 heap = jpegGetHeap();
    hdJpegChecksumBuffer_c* buffer = jpegAllocateAligned(ChecksumBufferSize, heap, 4);
    jpegFill(buffer, 0, ChecksumBufferSize);
    jpegCopy(buffer, &mDataSize, sizeof(mDataSize), 0);
    jpegCopy(buffer->mData, mJpegData, sizeof(mJpegData), 0);
    u32 sum = jpegChecksum(buffer, ChecksumBufferSize);
    jpegFree(buffer);
    return sum;
}
/* 027258C4 */
void hdJpeg_c::updateChecksum() { mChecksum = calculateChecksum(); }
/* 027258F4 */
void hdJpeg_c::erase() {
    jpegFill(mJpegData, 0, sizeof(mJpegData)); clearHeader(); updateChecksum();
}
/* 02725938 */
void hdJpeg_c::reset() {
    jpegFill(mJpegData, 0, sizeof(mJpegData)); clearHeader(); updateChecksum();
}
/* 0272597C */
void hdJpeg_c::copy(hdJpeg_c* other) { jpegCopy(this, other, sizeof(hdJpeg_c), 0); }
/* 0272598C */
u32 hdJpeg_c::valid() { return mValid != 0; }
/* 0272599C */
void hdJpeg_c::enable() { mValid = 1; }
/* 027259A8 */
void hdJpeg_c::disable() { mValid = 0; }
/* 027259B4 */
u32 hdJpeg_c::subject() { return mSubject; }
/* 027259BC */
void hdJpeg_c::setSubject(u32 value) { mSubject = value; }
/* 027259C4 */
u32 hdJpeg_c::label() { return mLabel; }
/* 027259CC */
void hdJpeg_c::setLabel(u32 value) { mLabel = value; }
/* 027259D4 */
u32 hdJpeg_c::saved() { return mSaved != 0; }
/* 027259E4 */
void hdJpeg_c::markSaved() { mSaved = 1; }
/* 027259F0 */
u32 hdJpeg_c::dataSize() { return mDataSize; }
/* 027259F8 */
void hdJpeg_c::setDataSize(u32 value) { mDataSize = value; }
/* 02725A00 */
u32 hdJpeg_c::emptyState() { return mState == 1; }
/* 02725A14 */
void hdJpeg_c::setState(u32 value) { mState = value; }
/* 02725A1C */
u8* hdJpeg_c::data() { return mJpegData; }
/* 02725A24 */
u8* hdJpeg_c::constData() { return mJpegData; }
/* 02725A2C */
void hdJpeg_c::loadResource(u32 index) {
    initialize();
    hdJpegSafeString_c archiveName;
    hdJpegSafeString_c fileName;
    archiveName.mpText = gabi::guest_str("Jpeg"); archiveName.mVtable = gabi::ea(&sJpegSafeStringVtable);
    fileName.mpText = gabi::guest_str("Jpeg.bfres"); fileName.mVtable = gabi::ea(&sJpegSafeStringVtable);
    u32 resource = jpegGetResource(sResourceManager, &archiveName, &fileName, 0);
    if (!resource) return;
    hdJpegArchive_c* archive = jpegArchive(resource);
    s32 offset = archive->mDictionaryOffset;
    u32 dictionary = offset ? gabi::ea(&archive->mDictionaryOffset) + offset : 0;
    hdJpegResource_c* jpeg = jpegFindResource(dictionary, sJpegNames.mNames[index]);
    if (!jpeg) return;
    s32 dataOffset = jpeg->mDataOffset;
    u32 size = jpeg->mDataSize;
    u8* source = dataOffset ? reinterpret_cast<u8*>(jpeg) + dataOffset : NULL;
    jpegCopy(mJpegData, source, size, 0);
    markSaved(); enable(); setLabel(0); setState(1);
    setSubject(sJpegLabels.mLabels[index]); setDataSize(size); updateChecksum();
}
/* 02725B44 */
u32 hdJpeg_c::checksum() { return mChecksum; }
/* 02725B4C */
void hdJpeg_c::setChecksum(u32 value) { mChecksum = value; }
/* 02725B54 */
static void initializeJpegStatics() {
    initializePictureHeaderStatics(&sJpegHeaderState, &sJpegHeaderTokens, &sJpegHeaderAngles);
}
/* WWHD glue
 data sJpegSafeStringVtable 10140B38 nocheck
 ext 028F026C void pictureTokenInit(dPictureInitTokenHD*)
 ext 028ED6F8 void pictureInitFlagA(be<u8>*)
 ext 028EAB2C void pictureInitFlagB(be<u8>*)
 data sJpegHeaderState 1049F7E8
 data sJpegHeaderTokens 101F8574
 data sJpegHeaderAngles 10140C0C
 data sResourceManager 101F4F7C
 data sJpegNames 101F8558
 data sJpegLabels 10140B30
 str 10140BF8 "Jpeg"
 str 10140C00 "Jpeg.bfres"
 ext 0273AD10 void* jpegAllocate(u32)
 ext C0009990 void jpegFill(void*, u32, u32)
 ext C0009988 void jpegCopy(void*, const void*, u32, u32)
 ext 0203E8B8 u32 jpegGetHeap()
 ext 0273B0D4 hdJpegChecksumBuffer_c* jpegAllocateAligned(u32, u32, u32)
 ext 0273B264 u32 jpegChecksum(void*, u32)
 ext 0273AFC8 void jpegFree(void*)
 ext 026124B0 u32 jpegGetResource(u32, hdJpegSafeString_c*, hdJpegSafeString_c*, u32)
 ext 027E2DC0 hdJpegArchive_c* jpegArchive(u32)
 ext 027DFA24 hdJpegResource_c* jpegFindResource(u32, u32)
*/
