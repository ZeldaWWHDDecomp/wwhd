#include "gabi.h"
namespace {
using namespace gabi;
static u32 addr(void* p) { return ea(p); }
void* cNd_Last(void* p);
void cNd_SetObject(void* p, void* data);
void cNd_Cut(void* p);
void cNd_Addition(void* p, void* q);
int cNd_LengthOf(void* p) {
    WWHD_FUNC(0x02019A90, int, p);
    u32 n=0, node=addr(p);
    while(node) { node=load<u32>(node+8); ++n; }
    return static_cast<int>(n);
}
VERIFY(0x02019A90, cNd_LengthOf);
void* cNd_First(void* p) {
    WWHD_FUNC(0x02019AB4, void*, p);
    u32 result=0,node=addr(p);
    while(node) { result=node; node=load<u32>(node); }
    return at<void>(result);
}
VERIFY(0x02019AB4, cNd_First);
void* cNd_Last(void* p) {
    WWHD_FUNC(0x02019AD8, void*, p);
    u32 result=0,node=addr(p);
    while(node) { result=node; node=load<u32>(node+8); }
    return at<void>(result);
}
VERIFY(0x02019AD8, cNd_Last);
void* cNd_Order(void* p, int index) {
    WWHD_FUNC(0x02019AFC, void*, p,index);
    u32 result=0,node=addr(p); int i=0;
    while(i<index && node) { result=node; ++i; node=load<u32>(node+8); }
    return at<void>(i<index ? result : 0);
}
VERIFY(0x02019AFC, cNd_Order);
void cNd_SingleCut(void* p) {
    WWHD_FUNC(0x02019B48, void, p);
    u32 node=addr(p),prev=load<u32>(node),next=load<u32>(node+8);
    if(prev) store<u32>(prev+8,next);
    if(next) store<u32>(next,prev);
    store<u32>(node,0);store<u32>(node+8,0);
}
VERIFY(0x02019B48, cNd_SingleCut);
void cNd_Cut(void* p) {
    WWHD_FUNC(0x02019B78, void, p);
    u32 node=addr(p),prev=load<u32>(node);
    if(prev) store<u32>(prev+8,0);
    store<u32>(node,0);
}
VERIFY(0x02019B78, cNd_Cut);
void cNd_Addition(void* p, void* q) {
    WWHD_FUNC(0x02019B94, void, p,q);
    if(!p || !q) call<void>(0x0273AA24,at<void>(0x10003608),0xDB,at<void>(0x10003614));
    if(p && q) {
        u32 last=addr(cNd_Last(p));
        store<u32>(last+8,addr(q));store<u32>(addr(q),last);
    }
}
VERIFY(0x02019B94, cNd_Addition);
void cNd_Insert(void* p, void* q) {
    WWHD_FUNC(0x02019C0C, void, p,q);
    u32 prev=load<u32>(addr(p));
    if(prev) { cNd_Cut(p); cNd_Addition(at<void>(prev),q); }
    cNd_Addition(q,p);
}
VERIFY(0x02019C0C, cNd_Insert);
void cNd_SetObject(void* p, void* data) {
    WWHD_FUNC(0x02019C6C, void, p,data);
    u32 node=addr(p);
    while(node) { store<u32>(node+4,addr(data));node=load<u32>(node+8); }
}
VERIFY(0x02019C6C, cNd_SetObject);
void cNd_ClearObject(void* p) {
    WWHD_FUNC(0x02019C8C, void, p);
    cNd_SetObject(p,nullptr);
}
VERIFY(0x02019C8C, cNd_ClearObject);
void cNd_ForcedClear(void* p) {
    WWHD_FUNC(0x02019C94, void, p);
    u32 node=addr(p);store<u32>(node,0);store<u32>(node+8,0);store<u32>(node+4,0);
}
VERIFY(0x02019C94, cNd_ForcedClear);
void cNd_Create(void* p, void* data) {
    WWHD_FUNC(0x02019CA8, void, p,data);
    u32 node=addr(p);store<u32>(node+4,addr(data));store<u32>(node,0);store<u32>(node+8,0);
}
VERIFY(0x02019CA8, cNd_Create);
void cNd_StaticInit() {
    WWHD_FUNC(0x02019CBC, void);
    store<u32>(0x101FFA2C,0);store<u32>(0x101FFA24,0);
    store<u32>(0x101FFA30,0);store<u32>(0x101FFA28,0);
    call<void>(0x028F026C,at<void>(0x1018D28C));
    f32 lower=load<f32>(0x10003640),upper=load<f32>(0x10003644);
    store<f32>(0x101FFA18,lower);store<f32>(0x101FFA1C,upper);
    call<void>(0x028ED6F8,at<void>(0x101FFA20));
    call<void>(0x028F026C,at<void>(0x1018D298));
    call<void>(0x028EAB2C,at<void>(0x101FFA21));
    call<void>(0x028F026C,at<void>(0x1018D2A4));
}
VERIFY(0x02019CBC, cNd_StaticInit);
}
