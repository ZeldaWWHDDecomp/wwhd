// Packet-owned inline text and optional heap-backed debug names in the HD build.
#include "JSystem/J3DGraph/J3DPacketAdapter.h"
using namespace j3d;
void* allocatePacketObject(u32);
void deallocatePacketObject(void*);
void constructPacketDisposer(sead::Disposer*);
void finalizePacketDisposer(sead::Disposer*,u32);
void freePacketName(void*);
void assertPacketState(const char*,u32,const char*);
static sead::DisposerVtable sPacketMethods;
static sead::DisposerVtable sShapePacketMethods;
static sead::SafeTextVtable sPacketTextBaseMethods;
static sead::SafeTextVtable sPacketTextBufferMethods;
static sead::SafeTextVtable sPacketTextMethods;
static PacketCounter sPacketCounter;
// Owned data-only heap word; the game image supplies its initial value.
static PacketStringHeap sPacketStringHeap;
/* inlined into the packet constructor */
static sead::FixedSafeText<104>* constructPacketName(sead::FixedSafeText<104>* name) {
    if(name==NULL) name=static_cast<sead::FixedSafeText<104>*>(allocatePacketObject(sizeof(sead::FixedSafeText<104>)));
    if(name!=NULL) {
        sead::SafeTextBuffer* buffer=name;
        if(buffer==NULL) buffer=static_cast<sead::SafeTextBuffer*>(allocatePacketObject(sizeof(sead::SafeTextBuffer)));
        if(buffer!=NULL) {
            buffer->mVtable=&sPacketTextBaseMethods;
            buffer->mText=name->mStorage;
            buffer->mCapacity=104;
            name->mStorage[103]=0;
        }
        be<u8>* first=name->mText;
        name->mVtable=&sPacketTextBufferMethods;
        first[0]=0;
        name->mVtable=&sPacketTextMethods;
    }
    return name;
}
/* 027F1278 */
PacketObject* PacketObject::create(PacketObject* packet) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(packet==NULL) packet=static_cast<PacketObject*>(allocatePacketObject(sizeof(PacketObject)));
    if(packet==NULL) return packet;
    constructPacketDisposer(packet);
    packet->mVtable=&sPacketMethods;
    packet->mEntry=NULL;
    packet->mNext=NULL;
    packet->mState=0;
    constructPacketName(&packet->mLocalName);
    packet->mEntryCount=0;
    packet->mName=&packet->mLocalName;
    sead::RegistryHeap* heap=sPacketStringHeap.mHeap;
    if(heap!=NULL) {
        sead::FixedSafeText<104>* name=gabi::call_ptr<sead::FixedSafeText<104>*>(heap->dispatch()->mAllocate.mFunction,heap,u32(sizeof(sead::FixedSafeText<104>)),4);
        packet->mName=name;
        if(name!=NULL) constructPacketName(name);
    }
    sPacketCounter.mCount=u32(sPacketCounter.mCount)+1;
    return packet;
}
/* 027F13DC */
void PacketObject::destroy(PacketObject* object,s32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(object==NULL) return;
    object->mVtable=&sPacketMethods;
    sPacketCounter.mCount=u32(sPacketCounter.mCount)-1;
    if(object->mEntryCount!=0) assertPacketState("J3DPacket.cpp",157,"mEntryPtr == (0)");
    sead::FixedSafeText<104>* name=object->mName;
    if(gabi::ea(name)!=gabi::ea(&object->mLocalName)) {
        gabi::call_ptr<void>(name->mVtable->mDestroy.mFunction,name,2);
        sead::RegistryHeap* heap=sPacketStringHeap.mHeap;
        if(heap==NULL) {
            assertPacketState("J3DPacket.cpp",161,"sStringHeap != (0)");
            heap=sPacketStringHeap.mHeap;
        }
        if(heap!=NULL) gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,static_cast<sead::FixedSafeText<104>*>(object->mName));
        else freePacketName(object->mName);
    }
    finalizePacketDisposer(object,0);
    if((flags&1)!=0) deallocatePacketObject(object);
}
/* 027F1534 */
ShapePacket* ShapePacket::create(ShapePacket* packet) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(packet==NULL) packet=static_cast<ShapePacket*>(allocatePacketObject(sizeof(ShapePacket)));
    if(packet==NULL) return packet;
    PacketObject::create(packet);
    packet->mModel=NULL;
    packet->mVtable=&sShapePacketMethods;
    packet->mResource=NULL;
    packet->mDirty=0;
    packet->mProgram=NULL;
    packet->mBufferType=0;
    return packet;
}
/* WWHD glue
 ext 0273AD10 void* allocatePacketObject(u32)
 ext 0273AF40 void deallocatePacketObject(void*)
 ext 02752A84 void constructPacketDisposer(sead::Disposer*)
 ext 02752BEC void finalizePacketDisposer(sead::Disposer*,u32)
 ext 027EC0B0 void freePacketName(void*)
 ext 0273AA24 void assertPacketState(const char*,u32,const char*)
 str 1016E258 "J3DPacket.cpp"
 str 1016E268 "mEntryPtr == (0)" renamed
 str 1016E27C "sStringHeap != (0)" renamed
 data sPacketMethods 1016E2F4
 data sShapePacketMethods 1016E324
 data sPacketTextBaseMethods 1016E210
 data sPacketTextBufferMethods 1016E228
 data sPacketTextMethods 1016E240
 data sPacketCounter 104B45A8
 data sPacketStringHeap 104B45AC
*/
