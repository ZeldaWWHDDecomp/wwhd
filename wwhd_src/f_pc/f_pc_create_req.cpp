/* f_pc_create_req: WWHD process creation requests.
 * HD entries 025DD84C..025DDB83, initializer025DDB84..025DDC17.
 * Request0x48: creating14,cancelling15,methodtag18,methods34,ID3C,process40,layer44.
 * External queue/process/method operations remain verification boundaries. */
#include "bindings.h"
namespace f_pc_create_req_cpp {
static u32 ld(u32 a) { return gabi::load<u32>(a); }
static u8 lb(u32 a) { return gabi::load<u8>(a); }
static void st(u32 a,u32 v) { gabi::store<u32>(a,v); }
static void sb(u32 a,u8 v) { gabi::store<u8>(a,v); }
static BOOL isCreatingByID(u32 tag,u32 id) {
    WWHD_FUNC(0x025DD84C,BOOL,tag,id);
    return ld(ld(tag+0xC)+0x3C)==ld(id);
}
VERIFY(0x025DD84C,isCreatingByID);
static BOOL IsCreatingByID(u32 id) {
    WWHD_FUNC(0x025DD868,BOOL,id);
    gabi::Local<be<u32>> key;
    *key.get()=id;
    return gabi::call<u32>(0x025DD714,0x025DD84C,gabi::ea(key.get()))!=0;
}
VERIFY(0x025DD868,IsCreatingByID);
static BOOL Delete(u32 req) {
    WWHD_FUNC(0x025DD8A0,BOOL,req);
    gabi::call(0x025DDC28,req);
    gabi::call(0x025DEA90,ld(req+0x44));
    gabi::call(0x025DE9E0,req+0x18);
    u32 methods=ld(req+0x34);
    if(methods && !gabi::call<BOOL>(0x025DFCAC,ld(methods+8),req)) return FALSE;
    u32 proc=ld(req+0x40);
    if(proc) st(proc+0x14,0);
    gabi::call(0x0201945C,req);
    return TRUE;
}
VERIFY(0x025DD8A0,Delete);
static BOOL Cancel(u32 req) {
    WWHD_FUNC(0x025DD934,BOOL,req);
    if(!req || lb(req+0x15)) return TRUE;
    u32 proc=ld(req+0x40);
    sb(req+0x15,1);
    if(proc && !gabi::call<BOOL>(0x025DE1B8,proc)) return FALSE;
    u32 methods=ld(req+0x34);
    if(methods && !gabi::call<BOOL>(0x025DFCAC,ld(methods+4),req)) return FALSE;
    return gabi::call<BOOL>(0x025DD8A0,req);
}
VERIFY(0x025DD934,Cancel);
static BOOL IsDoing(u32 req) {
    WWHD_FUNC(0x025DD9E4,BOOL,req);
    return req ? lb(req+0x14) : FALSE;
}
VERIFY(0x025DD9E4,IsDoing);
static BOOL Do(u32 req) {
    WWHD_FUNC(0x025DD9F8,BOOL,req);
    u32 phase=4;
    u32 methods=ld(req+0x34);
    if(methods) {
        u32 handler=ld(methods);
        if(handler) {
            sb(req+0x14,1);
            phase=gabi::call_ptr<u32>(handler,req);
            sb(req+0x14,0);
        }
    }
    if(phase==4) {
        if(gabi::call<BOOL>(0x025DE720,ld(req+0x40))) return gabi::call<BOOL>(0x025DD8A0,req);
        return gabi::call<BOOL>(0x025DD934,req);
    }
    if(phase==3 || phase==5) return gabi::call<BOOL>(0x025DD934,req);
    return TRUE;
}
VERIFY(0x025DD9F8,Do);
static BOOL Handler() {
    WWHD_FUNC(0x025DDAC4,BOOL,(u32)0);
    return gabi::call<BOOL>(0x025DD6D8,0x025DD9F8,0);
}
VERIFY(0x025DDAC4,Handler);
static u32 Create(u32 layer,u32 size,u32 methods) {
    WWHD_FUNC(0x025DDAD4,u32,layer,size,methods);
    u32 req=gabi::call<u32>(0x02019430,(s32)-4,size);
    if(req) {
        gabi::call(0x025DDC2C,req,req);
        gabi::call(0x025DFE2C,req+0x18,0x025DD934,req);
        st(req+0x44,layer);
        st(req+0x34,methods);
        sb(req+0x38,0);
        st(req+0x3C,gabi::call<u32>(0x025DD290));
        sb(req+0x15,0);
        st(req+0x40,0);
        sb(req+0x14,0);
        gabi::call(0x025DEA80,ld(req+0x44));
        gabi::call(0x025DE9E4,ld(req+0x44),req+0x18);
        gabi::call(0x025DDC18,req);
    }
    return req;
}
VERIFY(0x025DDAD4,Create);
static void sinit() {
    WWHD_FUNC(0x025DDB84,void,(u32)0);
    sinit_header_statics(0x1048A6D8,0x101F395C);
}
VERIFY(0x025DDB84,sinit);
}
