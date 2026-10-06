// Qualified two-entry HD thread-heap context subset; local use only.
#include "gabi.h"
using namespace gabi;
namespace heap_context {
u32 set_thread_heap(u32 ignored,u32 incoming){
 WWHD_FUNC(0x02756170,u32,ignored,incoming);
 u32 module=load<u32>(0x101F8B94u);
 u32 context=call<u32>(0xC0009CC8u,load<u32>(module+0x24));
 u32 previous=load<u32>(context+0x70);store<u32>(context+0x70,incoming);return previous;
}
VERIFY(0x02756170,set_thread_heap);
u32 current_heap_wrapper(){
 WWHD_FUNC(0x027EC230,u32);
 return call<u32>(0x02756140,load<u32>(0x101F8B4Cu));
}
VERIFY(0x027EC230,current_heap_wrapper);
}
