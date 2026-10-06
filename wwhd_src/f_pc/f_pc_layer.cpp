#include "wwhd.h"
#include "gabi.h"

namespace {
template<class T> T get(u32 p,u32 o=0) { return *gabi::at<be<T>>(p+o); }
template<class T> void put(u32 p,u32 o,T v) { *gabi::at<be<T>>(p+o)=v; }
void clearLayer(u32 p) {
    for(u32 i=0;i<11;++i) put<u32>(p,i*4,get<u32>(0x101F3AEC,i*4));
}
}
void fpcLy_CancelQTo(u32 p) { WWHD_FUNC(0x025DE9E0,void,p); gabi::call<void>(0x025DFE28,p); }
VERIFY(0x025DE9E0,fpcLy_CancelQTo);
s32 fpcLy_ToCancelQ(u32 p,u32 q) { WWHD_FUNC(0x025DE9E4,s32,p,q); return gabi::call<s32>(0x025DFE24,p+0x1C,q); }
VERIFY(0x025DE9E4,fpcLy_ToCancelQ);
bool fpcLy_CancelMethod(u32 p) { WWHD_FUNC(0x025DE9EC,bool,p); return gabi::call<s32>(0x025DFE14,p)==1; }
VERIFY(0x025DE9EC,fpcLy_CancelMethod);
s32 fpcLy_IntoQueue(u32 p,s32 n,u32 tag,s32 index) { WWHD_FUNC(0x025DEA18,s32,p,n,tag,index); return gabi::call<s32>(0x0201A7D4,p+0x10,n,tag,index); }
VERIFY(0x025DEA18,fpcLy_IntoQueue);
s32 fpcLy_ToQueue(u32 p,s32 n,u32 tag) { WWHD_FUNC(0x025DEA20,s32,p,n,tag); return gabi::call<s32>(0x0201A770,p+0x10,n,tag); }
VERIFY(0x025DEA20,fpcLy_ToQueue);
s32 fpcLy_QueueTo(u32 p,u32 tag) { WWHD_FUNC(0x025DEA28,s32,p,tag); return gabi::call<s32>(0x0201A724,tag); }
VERIFY(0x025DEA28,fpcLy_QueueTo);
s32 fpcLy_IsDeletingMesg(u32 p) { WWHD_FUNC(0x025DEA30,s32,p); return get<s16>(p,0x2A)>0; }
VERIFY(0x025DEA30,fpcLy_IsDeletingMesg);
void fpcLy_DeletingMesg(u32 p) { WWHD_FUNC(0x025DEA44,void,p); put<u16>(p,0x2A,u16(get<u16>(p,0x2A)+1)); }
VERIFY(0x025DEA44,fpcLy_DeletingMesg);
void fpcLy_DeletedMesg(u32 p) { WWHD_FUNC(0x025DEA54,void,p); s16 v=get<s16>(p,0x2A); if(v>0) put<u16>(p,0x2A,u16(v-1)); }
VERIFY(0x025DEA54,fpcLy_DeletedMesg);
s32 fpcLy_IsCreatingMesg(u32 p) { WWHD_FUNC(0x025DEA6C,s32,p); return get<s16>(p,0x28)>0; }
VERIFY(0x025DEA6C,fpcLy_IsCreatingMesg);
void fpcLy_CreatingMesg(u32 p) { WWHD_FUNC(0x025DEA80,void,p); put<u16>(p,0x28,u16(get<u16>(p,0x28)+1)); }
VERIFY(0x025DEA80,fpcLy_CreatingMesg);
void fpcLy_CreatedMesg(u32 p) { WWHD_FUNC(0x025DEA90,void,p); s16 v=get<s16>(p,0x28); if(v>0) put<u16>(p,0x28,u16(v-1)); }
VERIFY(0x025DEA90,fpcLy_CreatedMesg);
u32 fpcLy_RootLayer() { WWHD_FUNC(0x025DEAA8,u32); return get<u32>(0x101F3B18); }
VERIFY(0x025DEAA8,fpcLy_RootLayer);
void fpcLy_SetCurrentLayer(u32 p) { WWHD_FUNC(0x025DEAB4,void,p); put<u32>(0x101F3AE8,0,p); }
VERIFY(0x025DEAB4,fpcLy_SetCurrentLayer);
u32 fpcLy_Layer(u32 id) {
    WWHD_FUNC(0x025DEAC0,u32,id);
    if(id==0) return gabi::call<u32>(0x025DEAA8);
    u32 p=gabi::call<u32>(0x025DEAA8);
    if(get<u32>(p,0xC)==id) return p;
    u32 current=get<u32>(0x101F3AE8);
    if(id==0xFFFFFFFD || get<u32>(current,0xC)==id) return current;
    while(p) { if(get<u32>(p,0xC)==id) break; p=get<u32>(p,8); }
    return p;
}
VERIFY(0x025DEAC0,fpcLy_Layer);
s32 fpcLy_Delete(u32 p) {
    WWHD_FUNC(0x025DEB58,s32,p);
    u32 lists=get<u32>(p,0x10);
    if(get<u32>(lists,8)!=0 || get<u32>(p,0x24)!=0) return 0;
    gabi::call<void>(0x0200FDF4,p); clearLayer(p); return 1;
}
VERIFY(0x025DEB58,fpcLy_Delete);
s32 fpcLy_Cancel(u32 p) { WWHD_FUNC(0x025DEBE0,s32,p); return gabi::call<s32>(0x025DFD78,p+0x1C,u32(0x025DE9EC)); }
VERIFY(0x025DEBE0,fpcLy_Cancel);
void fpcLy_Create(u32 p,u32 node,u32 lists,s32 count) {
    WWHD_FUNC(0x025DEBF0,void,p,node,lists,count);
    clearLayer(p); gabi::call<void>(0x02019CA8,p,u32(0));
    u32 id=get<u32>(0x101F3B28); put<u32>(0x101F3B28,0,id+1); put<u32>(p,0xC,id);
    put<u32>(p,0x18,node);
    if(get<u32>(0x101F3B24)==1) {
        put<u32>(0x101F3B24,0,0);
        gabi::call<void>(0x02010008,u32(0x101F3B18));
        gabi::call<void>(0x025DEAB4,p);
    }
    put<u32>(p,0x10,lists); put<u32>(p,0x14,u32(count));
    gabi::call<void>(0x0201AB04,p+0x10,lists,count);
    gabi::call<void>(0x0200FE78,u32(0x101F3B18),p);
}
VERIFY(0x025DEBF0,fpcLy_Create);
void fpcLy_Initializer() {
    WWHD_FUNC(0x025DECD0,void);
    put<u32>(0x1048A7FC,8,0); put<u32>(0x1048A7FC,0,0); put<u32>(0x1048A7FC,12,0); put<u32>(0x1048A7FC,4,0);
    gabi::call<void>(0x028F026C,u32(0x101F3B2C));
    f32 a=get<f32>(0x100582FC); f32 b=get<f32>(0x10058300);
    put<f32>(0x1048A7F0,0,a); put<f32>(0x1048A7F4,0,b);
    gabi::call<void>(0x028ED6F8,u32(0x1048A7F8));
    gabi::call<void>(0x028F026C,u32(0x101F3B38));
    gabi::call<void>(0x028EAB2C,u32(0x1048A7F9));
    gabi::call<void>(0x028F026C,u32(0x101F3B44));
}
VERIFY(0x025DECD0,fpcLy_Initializer);
u32 fpcLy_CurrentLayer() { WWHD_FUNC(0x025DED64,u32); return get<u32>(0x101F3AE8); }
VERIFY(0x025DED64,fpcLy_CurrentLayer);
