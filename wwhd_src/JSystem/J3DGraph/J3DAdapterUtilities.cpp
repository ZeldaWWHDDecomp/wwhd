// Packet, resource and fixed-state operations retained by the HD renderer.
#include "JSystem/J3DGraph/J3DAdapterUtilities.h"
using namespace j3d;
void deallocateJ3DAdapter(void*);
void* allocateJ3DAdapter(u32);
void destroyJ3DPacketBase(PacketView*,s32);
void destroyJ3DProjection(void*,s32);
void registerJ3DExitRecord(ExitRecord*);
static ConstantWord sPrimaryColor;
static ConstantWord sSecondaryColor;
static ConstantWord sMaterialColor;
static ConstantPair sMaterialPair;
static RuntimeRecord sPacketRuntime;
static RuntimeRecord sProjectionRuntime;
static RuntimeRecord sDepthRuntime;
static RuntimeRecord sResourceRuntime;
static RuntimeRecord sMaterialRuntime;
static RuntimeRecord sFrameRuntime;
static RuntimeRecord sColorRuntime;
static ExitRecord sPacketExit;
static ExitRecord sProjectionExit;
static ExitRecord sDepthExit;
static ExitRecord sResourceExit;
static ExitRecord sMaterialExit;
static ExitRecord sFrameExit;
static ExitRecord sColorExit;
/* inlined into the translation-unit initializers */
static void clearRuntimeRecord(RuntimeRecord* record) {
    record->mWords[2]=0;
    record->mWords[0]=0;
    record->mWords[3]=0;
    record->mWords[1]=0;
}
/* 027F1508 */
void PacketView::clearLinks() { mEntry=NULL; mNext=NULL; }
/* 027F1518 */
void PacketView::notifySelected(const PacketNotification* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (source->mKind==2)
        gabi::call_ptr<void>(mMethods->mNotify,this,source);
}
/* 027F2804 */
void packet_text::TextBuffer::destroy(TextBuffer* object,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object!=NULL && (flags&1)!=0) deallocateJ3DAdapter(object);
}
/* 027F2818 */
void packet_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text=mText;
    u32 capacity=mCapacity;
    text[capacity-1]=0;
}
/* 027F2830 */
void packet_text::Text::destroy(Text* object,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object!=NULL && (flags&1)!=0) deallocateJ3DAdapter(object);
}
/* 027F2844 */
void packet_text::Text::assureTerminationImpl() { }
/* 027F2848 */
void packet_text::OwnedText::destroy(OwnedText* object,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object!=NULL && (flags&1)!=0) deallocateJ3DAdapter(object);
}
/* 027F285C */
void packet_text::OwnedText::assureTerminationImpl() { }
/* 027F2860 */
void PacketView::destroyComplete(PacketView* object,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object==NULL) return;
    destroyJ3DPacketBase(object,0);
    if((flags&1)!=0) deallocateJ3DAdapter(object);
}
/* 027F28B4 */
void packet_text::TemporaryText::assureTerminationImpl() { }
/* 027F28B8 */
void packet_text::TemporaryText::destroy(TemporaryText* object,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object!=NULL && (flags&1)!=0) deallocateJ3DAdapter(object);
}
/* 027F2AA8 */
ContextMode* ContextMode::create(ContextMode* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object==NULL) object=static_cast<ContextMode*>(allocateJ3DAdapter(sizeof(ContextMode)));
    if(object!=NULL) { object->mMode=3; object->mArgument=0; }
    return object;
}
/* 027F2AEC */
void ProjectionOwner::destroy(ProjectionOwner* object,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object==NULL) return;
    destroyJ3DProjection(object->mProjection,2);
    if((flags&1)!=0) deallocateJ3DAdapter(object);
}
/* 027F2B68 */
f32 ProjectionView::projectDepth(const Mtx34* point) const {
    f32 y=point->m[0][1];
    f32 depthY=mDepth[1];
    f32 depthX=mDepth[0];
    f32 product=gabi::fmuls_ppc(depthY,y);
    f32 x=point->m[0][0];
    f32 z=point->m[0][2];
    f32 xy=gabi::fmadds(depthX,x,product);
    f32 depthZ=mDepth[2];
    f32 xyz=gabi::fmadds(depthZ,z,xy);
    f32 translation=mDepth[3];
    return gabi::fadds_ppc(xyz,translation);
}
/* 027F3478 */
void ResourceView::setArchive(const gptr<void>* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    mArchive=static_cast<void*>(gabi::at<ResourceHandle>(gabi::ea(archive))->get());
}
/* 027F35B0 */
u32 ResourceView::transferFlags() const { return (u32(mHeader->mFlags)&4)!=0?2:0; }
/* 027F35CC */
s32 ResourceView::identifier() const { return mHeader->mIdentifier; }
/* 027F35D8 */
u32 ResourceView::alternateTransferFlags() const { return (u32(mHeader->mFlags)&4)!=0?2:0; }
/* 027F35F4 */
s32 ResourceView::alternateIdentifier() const { return mHeader->mIdentifier; }
/* 027F3600 */
u32 CompactResourceView::transferFlags() const { return (u16(mHeader->mFlags)&4)!=0?2:0; }
/* 027F361C */
s32 CompactResourceView::identifier() const { return mHeader->mIdentifier; }
/* 027F3628 */
void material_text::Text::destroy(Text* object,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object!=NULL && (flags&1)!=0) deallocateJ3DAdapter(object);
}
/* 027F3D60 */
void resource_text::Text::destroy(Text* object,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object!=NULL && (flags&1)!=0) deallocateJ3DAdapter(object);
}
/* 027F3D74 */
void resource_text::Text::assureTerminationImpl() { }
/* 027F3F8C */
void* ResourceHandle::get() const { return mHeader; }
/* 027F3F94 */
void* ResourceHandle::optionalData() const {
    ResourceRootHeader* header=static_cast<ResourceRootHeader*>(static_cast<void*>(mHeader));
    u32 offset=header->mDataOffset;
    return offset==0?NULL:gabi::at<void>(gabi::ea(&header->mDataOffset)+offset);
}
/* 02843C5C */
void TextureSlotState::clear() {
    for(u32 i=0;i<8;++i) mSlots[i]=0;
    mCount=0;
}
/* 02843B1C */
void ColorPairState::initialize() {
    u32 primary=sPrimaryColor.mValue;
    for(u32 i=0;i<2;++i) mPrimary[i]=primary;
    u32 secondary=sSecondaryColor.mValue;
    for(u32 i=0;i<2;++i) mSecondary[i]=secondary;
    for(u32 i=0;i<8;++i) mSlots[i]=0;
    mFlags=0;
    mCount=0;
}
/* 02843BC4 */
void MaterialSlotState::initialize() {
    for(u32 i=0;i<8;++i) mIndices[i]=0xFFFF;
    u32 high=sMaterialPair.mWords[1];
    u32 low=sMaterialPair.mWords[0];
    for(u32 i=0;i<3;++i) { mPairs[i][0]=low; mPairs[i][1]=high; }
    u32 color=sMaterialColor.mValue;
    for(u32 i=0;i<4;++i) mColors[i]=color;
    mFlags=0;
    mCount=0;
}
/* 027F27DC */
void j3d::initializePacketRuntime() {
    gabi::AutoFrame frame{gabi::current_function()};
    clearRuntimeRecord(&sPacketRuntime); registerJ3DExitRecord(&sPacketExit);
}
/* 027F2B40 */
void j3d::initializeProjectionRuntime() {
    gabi::AutoFrame frame{gabi::current_function()};
    clearRuntimeRecord(&sProjectionRuntime); registerJ3DExitRecord(&sProjectionExit);
}
/* 027F2B98 */
void j3d::initializeDepthRuntime() {
    gabi::AutoFrame frame{gabi::current_function()};
    clearRuntimeRecord(&sDepthRuntime); registerJ3DExitRecord(&sDepthExit);
}
/* 027F3588 */
void j3d::initializeResourceRuntime() {
    gabi::AutoFrame frame{gabi::current_function()};
    clearRuntimeRecord(&sResourceRuntime); registerJ3DExitRecord(&sResourceExit);
}
/* 027F3D38 */
void j3d::initializeMaterialRuntime() {
    gabi::AutoFrame frame{gabi::current_function()};
    clearRuntimeRecord(&sMaterialRuntime); registerJ3DExitRecord(&sMaterialExit);
}
/* 02801280 */
void j3d::initializeFrameRuntime() {
    gabi::AutoFrame frame{gabi::current_function()};
    clearRuntimeRecord(&sFrameRuntime); registerJ3DExitRecord(&sFrameExit);
}
/* 02843C80 */
void j3d::initializeColorRuntime() {
    gabi::AutoFrame frame{gabi::current_function()};
    clearRuntimeRecord(&sColorRuntime); registerJ3DExitRecord(&sColorExit);
}
/* WWHD glue
 ext 0273AF40 void deallocateJ3DAdapter(void*)
 ext 0273AD10 void* allocateJ3DAdapter(u32)
 ext 027F13DC void destroyJ3DPacketBase(j3d::PacketView*,s32)
 ext 0274E474 void destroyJ3DProjection(void*,s32)
 ext 028F026C void registerJ3DExitRecord(j3d::ExitRecord*)
 data sPrimaryColor 1016E36C
 data sSecondaryColor 1016E370
 data sMaterialColor 1016E374
 data sMaterialPair 1016E364
 data sPacketRuntime 104B4598
 data sProjectionRuntime 104B4898
 data sDepthRuntime 104B48A8
 data sResourceRuntime 104B48B8
 data sMaterialRuntime 104B48C8
 data sFrameRuntime 104B4EA4
 data sColorRuntime 104B6584
 data sPacketExit 101F98D8
 data sProjectionExit 101F98FC
 data sDepthExit 101F9908
 data sResourceExit 101F9914
 data sMaterialExit 101F9920
 data sFrameExit 101F9ADC
 data sColorExit 101FB014
*/
