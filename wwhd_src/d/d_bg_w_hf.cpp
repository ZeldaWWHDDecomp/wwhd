#include "wwhd.h"
#include "gabi.h"
#include <cmath>
namespace {
template<class T> T get(u32 p,u32 o=0) { return *gabi::at<be<T>>(p+o); }
template<class T> void put(u32 p,u32 o,T v) { *gabi::at<be<T>>(p+o)=v; }
void assertion(u32 file,u32 line,u32 expr) { gabi::call<void>(0x0273AA24,file,line,expr); }
struct Vec_l { be<f32> x,y,z; };
}
void dBgWHf_CalcPlane(u32 p) {
    WWHD_FUNC(0x024F4B0C,void,p);
    if(!get<u32>(p,0x90)) return;
    u32 nx=get<u32>(p,0xCC),nz=get<u32>(p,0xD0),bg=get<u32>(p,0x94);
    u32 tri=get<u32>(bg,0xC);
    if(nx*nz*2!=get<u32>(bg,8)) { assertion(0x10044078,0x61,0x10044088); nz=get<u32>(p,0xD0); }
    s32 rows=s32(nz*2); u32 index=0;
    gabi::Local<Vec_l> normal;
    for(s32 z=0;z<rows;++z) {
        s32 count=s32(get<u32>(p,0xCC));
        for(s32 x=0;x<count;++x) {
            u32 slot=get<u16>(get<u32>(p,0xC8),index*2),t=tri+slot*10;
            u16 v0=get<u16>(t),v1=get<u16>(t,2); f32 ny=get<f32>(p,0xC4);
            u32 vertices=get<u32>(p,0x90); u16 v2=get<u16>(t,4);
            u32 a=vertices+v0*12,b=vertices+v1*12,c=vertices+v2*12;
            f32 ay=get<f32>(a,4),by=get<f32>(b,4);
            f32 dx=(z&1)?gabi::fsubs_ppc(ay,by):gabi::fsubs_ppc(by,ay);
            normal->y=ny; normal->x=dx;
            f32 cy=get<f32>(c,4); by=get<f32>(b,4);
            u32 plane=get<u32>(p,0x88)+slot*24;
            normal->z=(z&1)?gabi::fsubs_ppc(cy,by):gabi::fsubs_ppc(by,cy);
            gabi::call<void>(0x02018A7C,plane,normal.get(),a);
            ++index; count=s32(get<u32>(p,0xCC));
            rows=s32(get<u32>(p,0xD0)*2);
        }
    }
}
VERIFY(0x024F4B0C,dBgWHf_CalcPlane);
void dBgWHf_ClassifyPlane(u32 p) {
    WWHD_FUNC(0x024F4D04,void,p);
    if(!get<u32>(p,0x90)) return;
    u32 bg=get<u32>(p,0x94); s32 count=s32(get<u32>(bg,0x10));
    gabi::Local<be<u32>> previous;
    for(s32 b=0;b<count;++b) {
        u32 table=get<u32>(bg,0x14),blocks=get<u32>(p,0x98); s32 t=get<u16>(table,u32(b)*2);
        s32 last=u32(b)==u32(count-1)?s32(get<u32>(bg,8)-1):s32(get<u16>(table,u32(b)*2+2))-1;
        put<u16>(blocks,u32(b)*6,0xFFFF); blocks=get<u32>(p,0x98);
        put<u16>(blocks,u32(b)*6+2,0xFFFF); blocks=get<u32>(p,0x98);
        put<u16>(blocks,u32(b)*6+4,0xFFFF); *previous.get()=0xFFFF;
        for(;t<=last;++t) {
            if(t<0 || t>=s32(get<u32>(get<u32>(p,0x94),8))) assertion(0x100440B4,0xAC,0x100440E4);
            gabi::call<void>(0x0200A564,p,get<u32>(p,0x98)+u32(b)*6+4,previous.get(),t);
        }
        bg=get<u32>(p,0x94); count=s32(get<u32>(bg,0x10));
    }
}
VERIFY(0x024F4D04,dBgWHf_ClassifyPlane);
void dBgWHf_MakeBlckMinMaxHf(u32 p,s32 index,u32 minimum,u32 maximum) {
    WWHD_FUNC(0x024F4E78,void,p,index,minimum,maximum);
    if(index<0 || index>=s32(get<u32>(get<u32>(p,0x94)))) assertion(0x10044104,0xD3,0x10044114);
    u32 vertex=get<u32>(p,0x90)+u32(index)*12;
    f32 lo=get<f32>(minimum),value=get<f32>(vertex,4);
    if(lo>value) { put<f32>(minimum,0,value); value=get<f32>(vertex,4); }
    if(get<f32>(maximum)<value) put<f32>(maximum,0,value);
}
VERIFY(0x024F4E78,dBgWHf_MakeBlckMinMaxHf);
void dBgWHf_MakeBlckBndHf(u32 p,s32 block,u32 minimum,u32 maximum) {
    WWHD_FUNC(0x024F4F24,void,p,block,minimum,maximum);
    u32 bg=get<u32>(p,0x94),count=get<u32>(bg,0x10);
    if(block<0 || block>=s32(count)) { assertion(0x1004414C,0xEA,0x1004415C); bg=get<u32>(p,0x94); count=get<u32>(bg,0x10); }
    u32 table=get<u32>(bg,0x14); s32 first=get<u16>(table,u32(block)*2);
    s32 last=u32(block)==count-1?s32(get<u32>(bg,8)-1):s32(get<u16>(table,u32(block)*2+2))-1;
    f32 lo=get<f32>(0x10044140),hi=get<f32>(0x10044144); put<f32>(minimum,0,lo); put<f32>(maximum,0,hi);
    for(s32 t=first;t<=last;++t) {
        bg=get<u32>(p,0x94);
        if(t<0 || t>=s32(get<u32>(bg,8))) { assertion(0x1004414C,0xFB,0x10044188); bg=get<u32>(p,0x94); }
        u32 tri=get<u32>(bg,0xC); u16 v=get<u16>(tri,u32(t)*10);
        gabi::call<void>(0x024F4E78,p,s32(v),minimum,maximum);
        tri=get<u32>(get<u32>(p,0x94),0xC); v=get<u16>(tri,u32(t)*10+2);
        gabi::call<void>(0x024F4E78,p,s32(v),minimum,maximum);
        tri=get<u32>(get<u32>(p,0x94),0xC); v=get<u16>(tri,u32(t)*10+4);
        gabi::call<void>(0x024F4E78,p,s32(v),minimum,maximum);
    }
    f32 step=get<f32>(0x10044148); put<f32>(minimum,0,gabi::fsubs_ppc(get<f32>(minimum),step));
    put<f32>(maximum,0,gabi::fadds_ppc(get<f32>(maximum),step));
}
VERIFY(0x024F4F24,dBgWHf_MakeBlckBndHf);
void dBgWHf_MakeNodeTreeRpHf(u32 p,s32 index) {
    WWHD_FUNC(0x024F50B8,void,p,index);
    u32 bg=get<u32>(p,0x94);
    if(index<0 || index>=s32(get<u32>(bg,0x18))) { assertion(0x100441AC,0x10F,0x100441BC); bg=get<u32>(p,0x94); }
    u32 node=get<u32>(bg,0x1C)+u32(index)*20,offset=u32(index)*28;
    if(get<u16>(node)&1) {
        u16 child=get<u16>(node,4);
        if(child!=0xFFFF) { u32 box=get<u32>(p,0xA0)+offset; gabi::call<void>(0x024F4F24,p,s32(child),box+4,box+16); }
    } else {
        gabi::call<void>(0x02017DD8,get<u32>(p,0xA0)+offset);
        for(u32 n=0;n<8;++n) {
            u16 child=get<u16>(node,4+n*2);
            if(child==0xFFFF) continue;
            gabi::call<void>(0x024F50B8,p,s32(child));
            child=get<u16>(node,4+n*2); u32 boxes=get<u32>(p,0xA0);
            f32 value=get<f32>(boxes,u32(child)*28+4);
            gabi::call<void>(0x02017EE8,boxes+offset,value);
            child=get<u16>(node,4+n*2); boxes=get<u32>(p,0xA0);
            value=get<f32>(boxes,u32(child)*28+16);
            gabi::call<void>(0x02017EE8,boxes+offset,value);
        }
    }
    u32 box=get<u32>(p,0xA0)+offset; f32 inf=get<f32>(0x10044140);
    bool bad=get<f32>(box)==inf || get<f32>(box,4)==inf || get<f32>(box,8)==inf;
    if(!bad) { f32 neg=get<f32>(0x10044144); bad=get<f32>(box,12)==neg || get<f32>(box,16)==neg || get<f32>(box,20)==neg; }
    if(bad) { assertion(0x100441AC,0x169,0x100441F0); box=get<u32>(p,0xA0)+offset; }
    f32 delta=gabi::fsubs_ppc(get<f32>(box,12),get<f32>(box)),epsilon=get<f32>(0x100030B8);
    bad=!(std::fabs(delta)<epsilon) && get<f32>(box)>get<f32>(box,12);
    if(!bad) { delta=gabi::fsubs_ppc(get<f32>(box,16),get<f32>(box,4)); bad=!(std::fabs(delta)<epsilon) && get<f32>(box,4)>get<f32>(box,16); }
    if(!bad) { delta=gabi::fsubs_ppc(get<f32>(box,20),get<f32>(box,8)); bad=!(std::fabs(delta)<epsilon) && get<f32>(box,8)>get<f32>(box,20); }
    if(bad) assertion(0x100441AC,0x16F,0x10044340);
}
VERIFY(0x024F50B8,dBgWHf_MakeNodeTreeRpHf);
void dBgWHf_MakeNodeTreeGrpRpHf(u32 p,s32 index) {
    WWHD_FUNC(0x024F5528,void,p,index);
    u32 bg=get<u32>(p,0x94);
    if(index<0 || index>=s32(get<u32>(bg,0x20))) { assertion(0x10044524,0x186,0x10044534); bg=get<u32>(p,0x94); }
    u32 group=get<u32>(bg,0x24)+u32(index)*52,boxoffset=u32(index)*32;
    u16 tree=get<u16>(group,0x2E);
    if(tree!=0xFFFF) {
        gabi::call<void>(0x024F50B8,p,s32(tree));
        group=get<u32>(get<u32>(p,0x94),0x24)+u32(index)*52; tree=get<u16>(group,0x2E);
        f32 value=get<f32>(get<u32>(p,0xA0),u32(tree)*28+4);
        gabi::call<void>(0x02017EC0,get<u32>(p,0x9C)+boxoffset,value);
        group=get<u32>(get<u32>(p,0x94),0x24)+u32(index)*52; tree=get<u16>(group,0x2E);
        value=get<f32>(get<u32>(p,0xA0),u32(tree)*28+16);
        gabi::call<void>(0x02017ED4,get<u32>(p,0x9C)+boxoffset,value);
        group=get<u32>(get<u32>(p,0x94),0x24)+u32(index)*52;
    }
    u16 child=get<u16>(group,0x28);
    while(child!=0xFFFF) {
        gabi::call<void>(0x024F5528,p,s32(child));
        u32 boxes=get<u32>(p,0x9C); f32 value=get<f32>(boxes,u32(child)*32+4);
        gabi::call<void>(0x02017EC0,boxes+boxoffset,value);
        boxes=get<u32>(p,0x9C); value=get<f32>(boxes,u32(child)*32+16);
        gabi::call<void>(0x02017ED4,boxes+boxoffset,value);
        child=get<u16>(get<u32>(get<u32>(p,0x94),0x24)+u32(child)*52,0x26);
    }
}
VERIFY(0x024F5528,dBgWHf_MakeNodeTreeGrpRpHf);
void dBgWHf_MakeNodeTreeHf(u32 p) {
    WWHD_FUNC(0x024F5684,void,p);
    u32 vertices=get<u32>(p,0x90),bg=get<u32>(p,0x94); s32 count=s32(get<u32>(bg,0x20));
    if(!vertices) {
        if(count>0) { u32 groups=get<u32>(bg,0x24); for(s32 i=0;i<count;++i) if(get<u16>(groups,u32(i)*52+0x24)==0xFFFF) { put<u32>(p,0xA4,u32(i)); return; } }
        return;
    }
    for(s32 i=0;i<count;++i) { gabi::call<void>(0x02017DD8,get<u32>(p,0x9C)+u32(i)*32); bg=get<u32>(p,0x94); count=s32(get<u32>(bg,0x20)); }
    u32 root=get<u32>(p,0xA4);
    if(root!=0xFFFF) { gabi::call<void>(0x024F5528,p,root); return; }
    if(count>0) { u32 groups=get<u32>(bg,0x24); for(s32 i=0;i<count;++i) if(get<u16>(groups,u32(i)*52+0x24)==0xFFFF) { put<u32>(p,0xA4,u32(i)); gabi::call<void>(0x024F5528,p,i); return; } }
}
VERIFY(0x024F5684,dBgWHf_MakeNodeTreeHf);
u32 dBgWHf_Ctor(u32 p) {
    WWHD_FUNC(0x024F5800,u32,p);
    if(!p) { p=gabi::call<u32>(0x0273AD10,u32(0xD4)); if(!p) return p; }
    gabi::call<void>(0x024F5A18,p); put<u32>(p,4,0x10044560); put<f32>(p,0xC4,get<f32>(0x10044554));
    put<u32>(p,0xCC,0); put<u32>(p,0xD0,0); put<u32>(p,0xC8,0); return p;
}
VERIFY(0x024F5800,dBgWHf_Ctor);
bool dBgWHf_Set(u32 p,u32 bg,u32 map,f32 normal,s32 nx,s32 nz,u32 flags) {
    WWHD_FUNC(0x024F5870,bool,p,bg,map,normal,nx,nz,flags);
    put<u32>(p,0xC8,map); put<u32>(p,0xCC,u32(nx)); put<u32>(p,0xD0,u32(nz)); put<f32>(p,0xC4,normal);
    if(gabi::call<s32>(0x0200A030,p,bg,u32(0x33),u32(0))) return true;
    put<u32>(p,0xBC,flags); if(flags&1) return false;
    u32 size=get<u32>(get<u32>(p,0x94))*12; u32 vertices=gabi::call<u32>(0x0273ADAC,size);
    put<u32>(p,0xC0,vertices); return vertices==0;
}
VERIFY(0x024F5870,dBgWHf_Set);
void dBgWHf_MoveHf(u32 p) {
    WWHD_FUNC(0x024F5910,void,p);
    u8 flag=get<u8>(p,0xBA),lock=get<u8>(p,0x6C); put<u8>(p,0xBA,u8(flag|1));
    if(lock&0x80) return;
    u32 vtable=get<u32>(p,4); gabi::call_ptr<void>(get<u32>(vtable,0x1C),p);
    vtable=get<u32>(p,4); gabi::call_ptr<void>(get<u32>(vtable,0x24),p);
    gabi::call<void>(0x024F5684,p);
}
VERIFY(0x024F5910,dBgWHf_MoveHf);
void dBgWHf_MatrixCrrPos(u32 p) { WWHD_FUNC(0x024F5A14,void,p); }
VERIFY(0x024F5A14,dBgWHf_MatrixCrrPos);
void dBgWHf_Initializer() {
    WWHD_FUNC(0x024F5980,void);
    put<u32>(0x1046EE14,8,0); put<u32>(0x1046EE14,0,0); put<u32>(0x1046EE14,12,0); put<u32>(0x1046EE14,4,0);
    gabi::call<void>(0x028F026C,u32(0x101D539C));
    f32 a=get<f32>(0x10044558); f32 b=get<f32>(0x1004455C);
    put<f32>(0x1046EE08,0,a); put<f32>(0x1046EE0C,0,b);
    gabi::call<void>(0x028ED6F8,u32(0x1046EE10));
    gabi::call<void>(0x028F026C,u32(0x101D53A8));
    gabi::call<void>(0x028EAB2C,u32(0x1046EE11));
    gabi::call<void>(0x028F026C,u32(0x101D53B4));
}
VERIFY(0x024F5980,dBgWHf_Initializer);
