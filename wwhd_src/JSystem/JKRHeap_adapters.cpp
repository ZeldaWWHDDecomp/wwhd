/* Six attributable heap entry adapters; mapped027F3F94 is a resource accessor and excluded. */
#include "bindings.h"
namespace heap_adapters {
static u32 arrayNew(u32 size,s32 alignment){
 WWHD_FUNC(0x0273AEC4,u32,size,alignment);
 u32 heap=gabi::call<u32>(0x02756140,gabi::load<u32>(0x101F8B4C));
 if(!heap)return 0;
 return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(heap+0xC)+0x34),heap,size,alignment);
}
VERIFY(0x0273AEC4,arrayNew);
static void scalarDelete(u32 memory){
 WWHD_FUNC(0x0273AF40,void,memory);
 if(!memory)return;
 u32 root=gabi::load<u32>(0x101F8B4C);
 if(root){u32 heap=gabi::call<u32>(0x02755FEC,root,memory);if(heap)gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(heap+0xC)+0x3C),heap,memory);}
 else gabi::call_ptr(gabi::load<u32>(0xC1002000),memory);
}
VERIFY(0x0273AF40,scalarDelete);
static void arrayDelete(u32 memory){
 WWHD_FUNC(0x0273AFC8,void,memory);
 if(!memory)return;
 u32 root=gabi::load<u32>(0x101F8B4C);
 if(root){u32 heap=gabi::call<u32>(0x02755FEC,root,memory);if(heap)gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(heap+0xC)+0x3C),heap,memory);}
 else gabi::call_ptr(gabi::load<u32>(0xC1002000),memory);
}
VERIFY(0x0273AFC8,arrayDelete);
static u32 currentHeap(){
 WWHD_FUNC(0x02756140,u32,(u32)0);
 u32 manager=gabi::load<u32>(0x101F8B94);
 u32 context=gabi::call<u32>(0xC0009CC8,gabi::load<u32>(manager+0x24));
 return gabi::load<u32>(context+0x70);
}
VERIFY(0x02756140,currentHeap);
static void freeWrapper(u32 memory){WWHD_FUNC(0x027EC0B0,void,memory);gabi::call(0x0273AF40,memory);}
VERIFY(0x027EC0B0,freeWrapper);
static u32 becomeCurrent(u32 heap){WWHD_FUNC(0x027EC220,u32,heap);return gabi::call<u32>(0x02756170,gabi::load<u32>(0x101F8B4C),heap);}
VERIFY(0x027EC220,becomeCurrent);
}
