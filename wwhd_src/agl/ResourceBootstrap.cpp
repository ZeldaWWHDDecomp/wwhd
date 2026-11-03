/* WWHD graphics-library resource bootstrap: settings defaults and the named heaps
 * (agl::DrawContextMgr, agl::Shader, agl::utl::PrimitiveShape, agl::utl::PrimitiveTexture & Vertex,
 * agl::utl::DynamicTextureAllocator) in which each resource singleton is created.
 * Layouts and names come from WWHD's own instructions and strings; no external reference. */
#include "agl/BitmapTransfer.h"
#include "agl/GraphicsAccess.h"
#include "agl/ResourceBootstrap.h"
using namespace agl;
struct BootstrapHeapMethods_l {u8 prefix[0x2C];gfn finish;};
struct agl::BootstrapHeap_l {u8 prefix[12];gptr<BootstrapHeapMethods_l> methods;u8 reserved[20];gptr<BootstrapHeap_l> next;};
struct BootstrapState_l {u8 prefix[0x428];be<u8> enabled;};
WWHD_SIZE(BootstrapSettings_l,0x140);
WWHD_OFFSET(BootstrapSettings_l,name,0x30);
WWHD_OFFSET(BootstrapSettings_l,enabled,0x13C);
WWHD_OFFSET(BootstrapSettings_l,secondary,0x13D);
WWHD_OFFSET(BootstrapHeap_l,next,0x24);
WWHD_OFFSET(BootstrapHeapMethods_l,finish,0x2C);
static sead::SafeTextVtable sBaseFixedTextMethods;
static sead::SafeTextVtable sIntermediateFixedTextMethods;
static sead::SafeTextVtable sFinalFixedTextMethods;
static u8 sDrawContextHeapName;
static u8 sShaderHeapName;
static u8 sPrimitiveShapeHeapName;
static u8 sPrimitiveTextureHeapName;
static u8 sDynamicTextureHeapName;
static void* sPrimitiveShapeExtras_l;
static void* sVertexRecordPool_l;
static void* sSharedGraphicsResources_l;
static u8 sBootstrapReady;
static void* allocateBootstrapObject(u32);
static void* bootstrapMove(void*,const void*,u32,u32);
static void bootstrapPrepareHeap(BootstrapHeap_l*);
static void bootstrapConfigureHeap(void*,BootstrapHeap_l*,u32,u32,u32);
static void bootstrapActivateHeap(BootstrapHeap_l*);
static void bootstrapNameHeap(void*,BootstrapHeap_l*,sead::SafeText*);
static void bootstrapSelectHeap(BootstrapHeap_l*);
static u32 bootstrapReserve(void*,BootstrapHeap_l*,u32,u32,u32,u32);
static BootstrapHeap_l* createBootstrapHeap(void*,sead::SafeText*,BootstrapHeap_l*,u32,u32);
static void bootstrapRegisterHeap(BootstrapHeap_l*);
static void bootstrapCreateSurfaceState(BootstrapHeap_l*);
static void createPrimitiveShapeResources(BootstrapHeap_l*);
static void initializePrimitiveShapeResources(void*,BootstrapHeap_l*);
static void createPrimitiveShapeExtras(BootstrapHeap_l*);
static void initializePrimitiveShapeExtras(void*,BootstrapHeap_l*);
static void createPrimitiveTextureResources(BootstrapHeap_l*);
static void initializePrimitiveTextureResources(void*,BootstrapHeap_l*);
static void createVertexRecordPool(BootstrapHeap_l*);
static void initializeVertexRecordPool(void*,BootstrapHeap_l*);
static void bootstrapCoreTargets(BootstrapHeap_l*);
static void createSharedGraphicsResources(BootstrapHeap_l*);
static void initializeSharedGraphicsResources(void*,BootstrapHeap_l*);
static void createDynamicTextureResources(BootstrapHeap_l*);
static void initializeDynamicTextureResources(void*,BootstrapHeap_l*,u32,u32,u32,u32,u32,u32);
static void createLibraryWorkPool(BootstrapHeap_l*);
static void initializeLibraryWorkPool(void*,u32);
static void finishBootstrapHeap(BootstrapHeap_l* h);
static BootstrapHeap_l* createNamedBootstrapHeap(sead::SafeText* text,be<u8>* name,BootstrapHeap_l* parent);
/* 027B5534 */
agl::BootstrapSettings_l* agl::createBootstrapSettings(BootstrapSettings_l* settings) {
 gabi::AutoFrame frame{gabi::current_function()};
 if (!settings) settings=static_cast<BootstrapSettings_l*>(allocateBootstrapObject(0x140));
 if (!settings) return nullptr;
 settings->options[5]=0;settings->options[0]=0x00800000;settings->heap=nullptr;
 settings->options[1]=0;settings->options[7]=0;settings->options[8]=0x02000000;
 settings->options[9]=0x10;settings->options[10]=0x80;settings->options[6]=0x02000000;
 settings->options[4]=0x01800000;settings->options[2]=0x03000000;settings->options[3]=0;
 auto* name=&settings->name;
 name->mText=name->mStorage;name->mVtable=&sBaseFixedTextMethods;name->mCapacity=0x100;name->mStorage[255]=0;
 name->mVtable=&sIntermediateFixedTextMethods;
 sead::SafeTextVtable* table=sFileDeviceEmptyText.mVtable;
 u32 terminate=table->mAssureTermination.mFunction;
 be<u8>* destination=name->mText;
 gabi::call_ptr<void>(terminate,&sFileDeviceEmptyText);
 be<u8>* source=sFileDeviceEmptyText.mText;
 u32 length=0;
 if (static_cast<u8>(source[0])) for (;;) {
  ++length;if (length>0x40000) {length=0;break;}
  if (!static_cast<u8>(source[length])) break;
 }
 s32 capacity=name->mCapacity;
 if (static_cast<s32>(length)>=capacity) length=static_cast<u32>(capacity)-1;
 gabi::call_ptr<void>(table->mAssureTermination.mFunction,&sFileDeviceEmptyText);
 bootstrapMove(destination,source,length,0);destination[length]=0;
 name->mVtable=&sFinalFixedTextMethods;
 settings->secondary=0;settings->enabled=1;
 return settings;
}

// Unit-local helpers are included in the canonical mutation ranges.
static void finishBootstrapHeap(BootstrapHeap_l* h) {gabi::call_ptr<void>(h->methods->finish.get(),h);}

static BootstrapHeap_l* createNamedBootstrapHeap(sead::SafeText* text,be<u8>* name,BootstrapHeap_l* parent) {
 text->mText=name;text->mVtable=&sResourceTextMethods;return createBootstrapHeap(nullptr,text,parent,1,0);
}

/* 027B56D0 */
void agl::initializeBootstrapResources(BootstrapSettings_l* settings) {
 gabi::AutoFrame frame{gabi::current_function()};
 gabi::FrameLocal<sead::SafeText> name(8),firstName(0x10);
 BootstrapHeap_l* heap=settings->heap.get();
 bootstrapPrepareHeap(heap);
 bootstrapConfigureHeap(sBitmapHeapSource_l,heap,settings->options[1],settings->options[0],settings->options[2]);
 bootstrapActivateHeap(heap);
 bootstrapNameHeap(sGraphicsResourceDebug_l,heap,&settings->name);
 u32 requested,reserved;
 if (sGraphicsAccessManager) {
  requested=settings->options[3];bootstrapSelectHeap(heap);
  reserved=bootstrapReserve(sNativeResources_l,heap,requested,settings->options[9],settings->options[10],settings->secondary);
 } else {
  heap=createNamedBootstrapHeap(firstName.get(),&sDrawContextHeapName,heap);
  bootstrapRegisterHeap(heap);finishBootstrapHeap(heap);
  heap=heap->next;
  requested=settings->options[3];bootstrapSelectHeap(heap);
  reserved=bootstrapReserve(sNativeResources_l,heap,requested,settings->options[9],settings->options[10],settings->secondary);
 }
 heap=createNamedBootstrapHeap(name.get(),&sShaderHeapName,heap);
 bootstrapCreateSurfaceState(heap);
 auto* state=static_cast<BootstrapState_l*>(sShaderResources_l.get());
 state->enabled=settings->enabled;
 finishBootstrapHeap(heap);
 heap=createNamedBootstrapHeap(name.get(),&sPrimitiveShapeHeapName,heap->next);
 createPrimitiveShapeResources(heap);initializePrimitiveShapeResources(sPrimitiveShapeResources_l,heap);
 createPrimitiveShapeExtras(heap);initializePrimitiveShapeExtras(sPrimitiveShapeExtras_l,heap);
 finishBootstrapHeap(heap);
 heap=createNamedBootstrapHeap(name.get(),&sPrimitiveTextureHeapName,heap->next);
 createPrimitiveTextureResources(heap);initializePrimitiveTextureResources(sPrimitiveTextureResources_l,heap);
 createVertexRecordPool(heap);initializeVertexRecordPool(sVertexRecordPool_l,heap);
 finishBootstrapHeap(heap);
 heap=heap->next;
 bootstrapCoreTargets(heap);createSharedGraphicsResources(heap);initializeSharedGraphicsResources(sSharedGraphicsResources_l,heap);
 heap=createNamedBootstrapHeap(name.get(),&sDynamicTextureHeapName,heap);
 createDynamicTextureResources(heap);
 initializeDynamicTextureResources(sDynamicTextureResources_l,heap,requested+reserved,static_cast<u32>(settings->options[4])-reserved,
                   settings->options[5],settings->options[6],0,0);
 finishBootstrapHeap(heap);
 createLibraryWorkPool(heap->next);initializeLibraryWorkPool(sDebugTextureResources_l,settings->options[1]);
 sBootstrapReady=1;
}
/* WWHD glue
data sBaseFixedTextMethods 10169780
data sIntermediateFixedTextMethods 10169798
data sFinalFixedTextMethods 101697B0
data sDrawContextHeapName 101697C8
data sShaderHeapName 101697DC
data sPrimitiveShapeHeapName 10169830
data sPrimitiveTextureHeapName 101697E8
data sDynamicTextureHeapName 1016980C
data sPrimitiveShapeExtras_l 101F8DDC
data sVertexRecordPool_l 101F9604
data sSharedGraphicsResources_l 101F95F8
data sBootstrapReady 101F8E00
ext 0273AD10 void* allocateBootstrapObject(u32)
ext C0009988 void* bootstrapMove(void*,const void*,u32,u32)
ext 027854AC void bootstrapPrepareHeap(BootstrapHeap_l*)
ext 027855BC void bootstrapConfigureHeap(void*,BootstrapHeap_l*,u32,u32,u32)
ext 02785BC0 void bootstrapActivateHeap(BootstrapHeap_l*)
ext 02785D54 void bootstrapNameHeap(void*,BootstrapHeap_l*,sead::SafeText*)
ext 0276AD30 void bootstrapSelectHeap(BootstrapHeap_l*)
ext 0276AE84 u32 bootstrapReserve(void*,BootstrapHeap_l*,u32,u32,u32,u32)
ext 02753004 BootstrapHeap_l* createBootstrapHeap(void*,sead::SafeText*,BootstrapHeap_l*,u32,u32)
ext 027B7168 void bootstrapRegisterHeap(BootstrapHeap_l*)
ext 02786418 void bootstrapCreateSurfaceState(BootstrapHeap_l*)
ext 027A145C void createPrimitiveShapeResources(BootstrapHeap_l*)
ext 027A5DF0 void initializePrimitiveShapeResources(void*,BootstrapHeap_l*)
ext 027A0A80 void createPrimitiveShapeExtras(BootstrapHeap_l*)
ext 027A0B7C void initializePrimitiveShapeExtras(void*,BootstrapHeap_l*)
ext 027A76C8 void createPrimitiveTextureResources(BootstrapHeap_l*)
ext 027A78F8 void initializePrimitiveTextureResources(void*,BootstrapHeap_l*)
ext 027CB8D4 void createVertexRecordPool(BootstrapHeap_l*)
ext 027CBA08 void initializeVertexRecordPool(void*,BootstrapHeap_l*)
ext 027B9C1C void bootstrapCoreTargets(BootstrapHeap_l*)
ext 027C50E4 void createSharedGraphicsResources(BootstrapHeap_l*)
ext 027C51CC void initializeSharedGraphicsResources(void*,BootstrapHeap_l*)
ext 0279F2EC void createDynamicTextureResources(BootstrapHeap_l*)
ext 0279F474 void initializeDynamicTextureResources(void*,BootstrapHeap_l*,u32,u32,u32,u32,u32,u32)
ext 02799BC8 void createLibraryWorkPool(BootstrapHeap_l*)
ext 02799D28 void initializeLibraryWorkPool(void*,u32)
*/
