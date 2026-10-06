#include "wwhd.h"
#include "gabi.h"
namespace {
template<class T> T get(u32 p,u32 o=0) { return *gabi::at<be<T>>(p+o); }
template<class T> void put(u32 p,u32 o,T v) { *gabi::at<be<T>>(p+o)=v; }
struct Vec_l { be<f32> x,y,z; };
struct Mtx_l { be<f32> values[12]; };
struct SafeString_l { be<u32> vtable,text; };
}
u32 defaultFactorCheck(u32 p,u32 x,u32 y) {
    WWHD_FUNC(0x0251B560,u32,p,x,y);
    return x==0 && (y==0 || y==get<u32>(p,0x9C)-1);
}
VERIFY(0x0251B560,defaultFactorCheck);
void clothSetMtx(u32 p,u32 matrix) {
    WWHD_FUNC(0x0251B638,void,p,matrix);
    gabi::call<void>(0x028E90D4,matrix,p+0x170);
}
VERIFY(0x0251B638,clothSetMtx);
u32 clothFactor03(u32 p,u32 x) { WWHD_FUNC(0x0251C208,u32,p,x); return x==0; }
VERIFY(0x0251C208,clothFactor03);
u32 clothFactor04(u32 p,u32 x) { WWHD_FUNC(0x0251C3B0,u32,p,x); return x==0; }
VERIFY(0x0251C3B0,clothFactor04);
void clothDrawWrapper(u32 p) {
    WWHD_FUNC(0x0251E194,void,p);
    gabi::call<void>(0x027F0E04,get<u32>(0x104B4634),p,u32(0));
    gabi::call<void>(0x0251DDE8,p);
}
VERIFY(0x0251E194,clothDrawWrapper);
void clothTexObjLoad(u32 p,u32 state) {
    WWHD_FUNC(0x0251E94C,void,p,state);
    f32 value=get<f32>(0x1004B590);put<u32>(state,0xE4,4);put<f32>(state,0xE8,value);
}
VERIFY(0x0251E94C,clothTexObjLoad);
void clothXluTevSetting(u32 p,u32 state) {
    WWHD_FUNC(0x0251EB4C,void,p,state);
    u32 flags=get<u32>(state,0xC);f32 value=get<f32>(0x1004B5A4);
    put<u32>(state,0xC,flags|1);put<u32>(state,0xE4,4);put<f32>(state,0xE8,value);
}
VERIFY(0x0251EB4C,clothXluTevSetting);
u32 clothPacketBaseCtor(u32 p) {
    WWHD_FUNC(0x0251FA94,u32,p);
    return p?p:gabi::call<u32>(0x0273AD10,u32(0x10));
}
VERIFY(0x0251FA94,clothPacketBaseCtor);
void clothPacketBaseDtor(u32 p,u32 flags) {
    WWHD_FUNC(0x0251FAC0,void,p,flags);
    if(p && (flags&1)) gabi::call<void>(0x0273AF40,p);
}
VERIFY(0x0251FAC0,clothPacketBaseDtor);
void clothEmptyDraw() { WWHD_FUNC(0x0251FAD4,void); }
VERIFY(0x0251FAD4,clothEmptyDraw);
void clothEmptyInline() { WWHD_FUNC(0x0251FC7C,void); }
VERIFY(0x0251FC7C,clothEmptyInline);
void clothSpringVector(u32 p,u32 other,u32 output,f32 distance,f32 factor) {
    WWHD_FUNC(0x0251B590,void,p,other,output,distance,factor);
    gabi::Local<Vec_l> delta,normal;
    gabi::call<void>(0x0201ADE0,other,delta.get(),p);
    gabi::call<void>(0x0201B12C,delta.get(),normal.get());
    f64 square=gabi::call<f64>(0x028E8DD0,delta.get());
    f64 length=gabi::call<f64>(0x028F4384,square);
    f32 scale=gabi::fmuls_ppc(f32(length-f64(distance)),factor);
    gabi::call<void>(0x028E8E64,normal.get(),normal.get(),scale);
    gabi::call<void>(0x028E8D88,output,normal.get(),output);
}
VERIFY(0x0251B590,clothSpringVector);
void clothInitializer() {
    WWHD_FUNC(0x0251FC80,void);
    put<u32>(0x1046F07C,8,0);put<u32>(0x1046F07C,0,0);
    put<u32>(0x1046F07C,0xC,0);put<u32>(0x1046F07C,4,0);
    gabi::call<void>(0x028F026C,u32(0x101D5DA4));
    f32 first=get<f32>(0x1004B870),second=get<f32>(0x1004B874);
    put<f32>(0x1046F070,0,first);put<f32>(0x1046F074,0,second);
    gabi::call<void>(0x028ED6F8,u32(0x1046F078));
    gabi::call<void>(0x028F026C,u32(0x101D5DB0));
    gabi::call<void>(0x028EAB2C,u32(0x1046F079));
    gabi::call<void>(0x028F026C,u32(0x101D5DBC));
}
VERIFY(0x0251FC80,clothInitializer);
void clothSetGlobalWind(u32 p,u32 wind) {
    WWHD_FUNC(0x0251E7A8,void,p,wind);
    gabi::Local<Mtx_l> matrix;
    gabi::call<void>(0x028E90D4,p+0x170,matrix.get());
    f32 zero=get<f32>(0x1004B564);
    matrix->values[11]=zero;matrix->values[7]=zero;matrix->values[3]=zero;
    gabi::call<void>(0x028E90D4,matrix.get(),u32(0x1048D0CC));
    gabi::call<void>(0x028E91EC,u32(0x1048D0CC),u32(0x1048D0CC));
    gabi::call<void>(0x028E8F64,u32(0x1048D0CC),wind,p+0xCC);
}
VERIFY(0x0251E7A8,clothSetGlobalWind);
void clothTexObjInit(u32 p,u32 image) {
    WWHD_FUNC(0x0251E830,void,p,image);
    gabi::call<void>(0x0274FBF8,get<u32>(0x101F8B18));
    gabi::call<void>(0x02773798,p+0x920,get<u32>(image,0x20));
    bool same=true;
    for(u32 o: {4u,8u,12u,16u,20u,24u,56u,52u,28u}) {
        if(get<u32>(p,0x5F0+o)!=get<u32>(p,0x920+o)) { same=false;break; }
    }
    if(!same) gabi::call<void>(0x027BDEB4,p+0x5F0,p+0x920);
    else {
        u32 first=get<u32>(p,0x948),second=get<u32>(p,0x950);
        put<u32>(p,0x618,first);put<u32>(p,0x6CC,second);
        put<u32>(p,0x6C4,first);put<u32>(p,0x620,second);
    }
    gabi::call<void>(0x0274FCCC,get<u32>(0x101F8B18));
}
VERIFY(0x0251E830,clothTexObjInit);
void clothXluDraw(u32 p) {
    WWHD_FUNC(0x0251EADC,void,p);
    u32 positions=get<u32>(p+get<u8>(p,0x1C0)*4,0xB0);
    gabi::Local<Vec_l> position;
    position->x=get<f32>(positions);position->y=get<f32>(positions,4);position->z=get<f32>(positions,8);
    u32 game=gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x0252F3B0,game+0x5D30,get<u32>(game,0x5D7C),p,position.get());
    gabi::call<void>(0x0251DDE8,p);
}
VERIFY(0x0251EADC,clothXluDraw);
void clothSnapshotMatrix(u32 output,u32 input) {
    WWHD_FUNC(0x0251D7C4,void,output,input);
    f32 values[12];
    for(u32 i=0;i<12;++i) values[i]=get<f32>(input,i*4);
    for(u32 i=0;i<12;++i) put<f32>(output,i*4,values[i]);
}
VERIFY(0x0251D7C4,clothSnapshotMatrix);
void clothColorToFloat(u32 output,u32 input) {
    WWHD_FUNC(0x0251DD24,void,output,input);
    s16 first=get<s16>(input),third=get<s16>(input,4),second=get<s16>(input,2);
    f32 divisor=get<f32>(0x1004B584);s16 fourth=get<s16>(input,6);
    f32 a=f32(f64(first)/f64(divisor)),b=f32(f64(second)/f64(divisor));
    f32 c=f32(f64(third)/f64(divisor)),d=f32(f64(fourth)/f64(divisor));
    put<f32>(output,0,a);put<f32>(output,4,b);put<f32>(output,8,c);put<f32>(output,12,d);
}
VERIFY(0x0251DD24,clothColorToFloat);
void clothMaterialLoad(u32 p) {
    WWHD_FUNC(0x0251E964,void,p);
    gabi::Local<SafeString_l> name;
    name->text=0x1004B54C;name->vtable=0x1004B594;
    u32 table=gabi::call<u32>(0x027FFCBC);
    s32 index=gabi::call<s32>(0x027B90AC,get<u32>(table,4),name.get());
    if(index>=0) {
        u32 count=get<u32>(table,8),materials=get<u32>(table,12);
        u32 entry=materials+(u32(index)<count?u32(index)*36:0);
        if(!get<u8>(entry,0x20)) {
            u32 resource=get<u32>(table,4),limit=get<u32>(resource,0x1C);
            u32 image=u32(index)<limit?get<u32>(resource,0x20)+u32(index)*132:0;
            gabi::call<void>(0x02800B0C,entry,image,u32(0));
            count=get<u32>(table,8);materials=get<u32>(table,12);
        }
        entry=materials+(u32(index)<count?u32(index)*36:0);
        gabi::call<void>(0x0280068C,p+0x1C4,entry,u32(0));
    } else gabi::call<void>(0x0280068C,p+0x1C4,u32(0),u32(0));
    u32 model=get<u32>(p,0x1D0),count=get<u32>(p,0x1C4);
    for(u32 i=0;i<count;++i) {
        u32 limit=get<u32>(p,0x1C8),entry=get<u32>(p,0x1CC)+(i<limit?i*20:0);
        gabi::call<void>(0x028003E8,entry,get<u32>(entry),model,u32(0));
        count=get<u32>(p,0x1C4);
    }
}
VERIFY(0x0251E964,clothMaterialLoad);

void clothDerivedDtor_0251FAD8(u32 p,u32 flags) {
    WWHD_FUNC(0x0251FAD8,void,p,flags);
    if(p) { gabi::call<void>(0x0251C558,p,u32(0)); if(flags&1) gabi::call<void>(0x0273AF40,p); }
}
VERIFY(0x0251FAD8,clothDerivedDtor_0251FAD8);

void clothDerivedDtor_0251FB2C(u32 p,u32 flags) {
    WWHD_FUNC(0x0251FB2C,void,p,flags);
    if(p) { gabi::call<void>(0x0251C558,p,u32(0)); if(flags&1) gabi::call<void>(0x0273AF40,p); }
}
VERIFY(0x0251FB2C,clothDerivedDtor_0251FB2C);

void clothDerivedDtor_0251FB80(u32 p,u32 flags) {
    WWHD_FUNC(0x0251FB80,void,p,flags);
    if(p) { gabi::call<void>(0x0251C558,p,u32(0)); if(flags&1) gabi::call<void>(0x0273AF40,p); }
}
VERIFY(0x0251FB80,clothDerivedDtor_0251FB80);

void clothDerivedDtor_0251FBD4(u32 p,u32 flags) {
    WWHD_FUNC(0x0251FBD4,void,p,flags);
    if(p) { gabi::call<void>(0x0251C558,p,u32(0)); if(flags&1) gabi::call<void>(0x0273AF40,p); }
}
VERIFY(0x0251FBD4,clothDerivedDtor_0251FBD4);

void clothDerivedDtor_0251FC28(u32 p,u32 flags) {
    WWHD_FUNC(0x0251FC28,void,p,flags);
    if(p) { gabi::call<void>(0x0251C558,p,u32(0)); if(flags&1) gabi::call<void>(0x0273AF40,p); }
}
VERIFY(0x0251FC28,clothDerivedDtor_0251FC28);

u32 clothFactory_0251BC80(u32 image,u32 toon,u32 nx,u32 ny,u32 lighting,u32 positions,f32 width,f32 height) {
    WWHD_FUNC(0x0251BC80,u32,image,toon,nx,ny,lighting,positions,width,height);
    u32 p=gabi::call<u32>(0x0251B648,u32(0),toon,nx,ny,lighting,positions,width,height);
    if(!p) return p;
    if(!get<u32>(p,0xB0)) return 0;
    if(!get<u32>(p,0xB4)) return 0;
    if(!get<u32>(p,0xB8)) return 0;
    if(!get<u32>(p,0xBC)) return 0;
    if(!get<u32>(p,0xC0)) return 0;
    if(!get<u32>(p,0xC4)) return 0;
    if(!get<u32>(p,0xC8)) return 0;
    if(image) {
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x4C),p,image);
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x34),p);
    }
    return p;
}
VERIFY(0x0251BC80,clothFactory_0251BC80);

u32 clothFactory_0251BE14(u32 image,u32 toon,u32 nx,u32 ny,u32 lighting,u32 positions,f32 width,f32 height) {
    WWHD_FUNC(0x0251BE14,u32,image,toon,nx,ny,lighting,positions,width,height);
    u32 p=gabi::call<u32>(0x0251BD54,u32(0),toon,nx,ny,lighting,positions,width,height);
    if(!p) return p;
    if(!get<u32>(p,0xB0)) return 0;
    if(!get<u32>(p,0xB4)) return 0;
    if(!get<u32>(p,0xB8)) return 0;
    if(!get<u32>(p,0xBC)) return 0;
    if(!get<u32>(p,0xC0)) return 0;
    if(!get<u32>(p,0xC4)) return 0;
    if(!get<u32>(p,0xC8)) return 0;
    if(image) {
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x4C),p,image);
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x34),p);
    }
    return p;
}
VERIFY(0x0251BE14,clothFactory_0251BE14);

u32 clothFactory_0251BFA4(u32 image,u32 toon,u32 lighting,u32 positions) {
    WWHD_FUNC(0x0251BFA4,u32,image,toon,lighting,positions);
    u32 p=gabi::call<u32>(0x0251BEE8,u32(0),toon,lighting,positions);
    if(!p) return p;
    if(!get<u32>(p,0xB0)) return 0;
    if(!get<u32>(p,0xB4)) return 0;
    if(!get<u32>(p,0xB8)) return 0;
    if(!get<u32>(p,0xBC)) return 0;
    if(!get<u32>(p,0xC0)) return 0;
    if(!get<u32>(p,0xC4)) return 0;
    if(!get<u32>(p,0xC8)) return 0;
    if(image) {
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x4C),p,image);
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x34),p);
    }
    return p;
}
VERIFY(0x0251BFA4,clothFactory_0251BFA4);

u32 clothFactory_0251C134(u32 image,u32 toon,u32 lighting,u32 positions) {
    WWHD_FUNC(0x0251C134,u32,image,toon,lighting,positions);
    u32 p=gabi::call<u32>(0x0251C078,u32(0),toon,lighting,positions);
    if(!p) return p;
    if(!get<u32>(p,0xB0)) return 0;
    if(!get<u32>(p,0xB4)) return 0;
    if(!get<u32>(p,0xB8)) return 0;
    if(!get<u32>(p,0xBC)) return 0;
    if(!get<u32>(p,0xC0)) return 0;
    if(!get<u32>(p,0xC4)) return 0;
    if(!get<u32>(p,0xC8)) return 0;
    if(image) {
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x4C),p,image);
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x34),p);
    }
    return p;
}
VERIFY(0x0251C134,clothFactory_0251C134);

u32 clothFactory_0251C2D0(u32 image,u32 toon,u32 lighting,u32 positions) {
    WWHD_FUNC(0x0251C2D0,u32,image,toon,lighting,positions);
    u32 p=gabi::call<u32>(0x0251C214,u32(0),toon,lighting,positions);
    if(!p) return p;
    if(!get<u32>(p,0xB0)) return 0;
    if(!get<u32>(p,0xB4)) return 0;
    if(!get<u32>(p,0xB8)) return 0;
    if(!get<u32>(p,0xBC)) return 0;
    if(!get<u32>(p,0xC0)) return 0;
    if(!get<u32>(p,0xC4)) return 0;
    if(!get<u32>(p,0xC8)) return 0;
    if(image) {
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x4C),p,image);
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x34),p);
        put<u32>(p,0xAC,0x0251C208);
    }
    return p;
}
VERIFY(0x0251C2D0,clothFactory_0251C2D0);

u32 clothFactory_0251C478(u32 image,u32 toon,u32 lighting,u32 positions) {
    WWHD_FUNC(0x0251C478,u32,image,toon,lighting,positions);
    u32 p=gabi::call<u32>(0x0251C3BC,u32(0),toon,lighting,positions);
    if(!p) return p;
    if(!get<u32>(p,0xB0)) return 0;
    if(!get<u32>(p,0xB4)) return 0;
    if(!get<u32>(p,0xB8)) return 0;
    if(!get<u32>(p,0xBC)) return 0;
    if(!get<u32>(p,0xC0)) return 0;
    if(!get<u32>(p,0xC4)) return 0;
    if(!get<u32>(p,0xC8)) return 0;
    if(image) {
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x4C),p,image);
        gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x34),p);
        put<u32>(p,0xAC,0x0251C3B0);
    }
    return p;
}
VERIFY(0x0251C478,clothFactory_0251C478);

u32 clothVobjCtor_0251BEE8(u32 p,u32 toon,u32 lighting,u32 positions) {
    WWHD_FUNC(0x0251BEE8,u32,p,toon,lighting,positions);
    if(!p) { p=gabi::call<u32>(0x0273AD10,u32(0xA50)); if(!p) return p; }
    f32 width=get<f32>(0x1004B568),height=get<f32>(0x1004B56C);
    gabi::call<void>(0x0251B648,p,toon,u32(5),u32(5),lighting,positions,width,height);
    put<u32>(p,0xC,0x1004B6D0);gabi::call<void>(0x028F521C,p+0xA40,u32(12));
    put<u8>(p,0xA4D,0);put<u8>(p,0xA4C,positions!=0);return p;
}
VERIFY(0x0251BEE8,clothVobjCtor_0251BEE8);

u32 clothVobjCtor_0251C078(u32 p,u32 toon,u32 lighting,u32 positions) {
    WWHD_FUNC(0x0251C078,u32,p,toon,lighting,positions);
    if(!p) { p=gabi::call<u32>(0x0273AD10,u32(0xA50)); if(!p) return p; }
    f32 width=get<f32>(0x1004B568),height=get<f32>(0x1004B56C);
    gabi::call<void>(0x0251B648,p,toon,u32(5),u32(5),lighting,positions,width,height);
    put<u32>(p,0xC,0x1004B738);gabi::call<void>(0x028F521C,p+0xA40,u32(12));
    put<u8>(p,0xA4D,0);put<u8>(p,0xA4C,positions!=0);return p;
}
VERIFY(0x0251C078,clothVobjCtor_0251C078);

u32 clothVobjCtor_0251C214(u32 p,u32 toon,u32 lighting,u32 positions) {
    WWHD_FUNC(0x0251C214,u32,p,toon,lighting,positions);
    if(!p) { p=gabi::call<u32>(0x0273AD10,u32(0xA50)); if(!p) return p; }
    f32 width=get<f32>(0x1004B570),height=get<f32>(0x1004B568);
    gabi::call<void>(0x0251B648,p,toon,u32(5),u32(5),lighting,positions,width,height);
    put<u32>(p,0xC,0x1004B7A0);gabi::call<void>(0x028F521C,p+0xA40,u32(12));
    put<u8>(p,0xA4D,0);put<u8>(p,0xA4C,positions!=0);return p;
}
VERIFY(0x0251C214,clothVobjCtor_0251C214);

u32 clothVobjCtor_0251C3BC(u32 p,u32 toon,u32 lighting,u32 positions) {
    WWHD_FUNC(0x0251C3BC,u32,p,toon,lighting,positions);
    if(!p) { p=gabi::call<u32>(0x0273AD10,u32(0xA50)); if(!p) return p; }
    f32 width=get<f32>(0x1004B568),height=get<f32>(0x1004B574);
    gabi::call<void>(0x0251B648,p,toon,u32(5),u32(5),lighting,positions,width,height);
    put<u32>(p,0xC,0x1004B808);gabi::call<void>(0x028F521C,p+0xA40,u32(12));
    put<u8>(p,0xA4D,0);put<u8>(p,0xA4C,positions!=0);return p;
}
VERIFY(0x0251C3BC,clothVobjCtor_0251C3BC);

u32 clothXluCtor(u32 p,u32 toon,u32 nx,u32 ny,u32 lighting,u32 positions,f32 width,f32 height) {
    WWHD_FUNC(0x0251BD54,u32,p,toon,nx,ny,lighting,positions,width,height);
    if(!p) { p=gabi::call<u32>(0x0273AD10,u32(0xA44)); if(!p) return p; }
    gabi::call<void>(0x0251B648,p,toon,nx,ny,lighting,positions,width,height);
    put<u8>(p,0xA40,0);put<u32>(p,0xC,0x1004B668);return p;
}
VERIFY(0x0251BD54,clothXluCtor);

void clothCopy_0251EE3C(u32 p) {
    WWHD_FUNC(0x0251EE3C,void,p);
    u32 current=get<u8>(p,0x1C0)^1;
    u32 ny=get<u32>(p,0x9C),nx=get<u32>(p,0x98);
    put<u8>(p,0x1C0,current&1);u32 size=nx*ny*12;
    u32 source=get<u32>(0x101D5D88),which=get<u8>(source,0x1C0);
    u32 destination=get<u32>(p+(current&1)*4,0xB8),origin=get<u32>(source+which*4,0xB8);
    gabi::call<void>(0xC000A848,destination,origin,size);
    source=get<u32>(0x101D5D88);which=get<u8>(source,0x1C0);
    ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);current=get<u8>(p,0x1C0);
    origin=get<u32>(source+which*4,0xC0);size=nx*ny*12;destination=get<u32>(p+current*4,0xC0);
    gabi::call<void>(0xC000A848,destination,origin,size);
    nx=get<u32>(p,0x98);ny=get<u32>(p,0x9C);source=get<u32>(0x101D5D88);
    destination=get<u32>(p,0xC8);size=nx*ny*12;origin=get<u32>(source,0xC8);
    gabi::call<void>(0xC000A848,destination,origin,size);
}
VERIFY(0x0251EE3C,clothCopy_0251EE3C);

void clothCopy_0251F188(u32 p) {
    WWHD_FUNC(0x0251F188,void,p);
    u32 current=get<u8>(p,0x1C0)^1;
    u32 ny=get<u32>(p,0x9C),nx=get<u32>(p,0x98);
    put<u8>(p,0x1C0,current&1);u32 size=nx*ny*12;
    u32 source=get<u32>(0x101D5D90),which=get<u8>(source,0x1C0);
    u32 destination=get<u32>(p+(current&1)*4,0xB8),origin=get<u32>(source+which*4,0xB8);
    gabi::call<void>(0xC000A848,destination,origin,size);
    source=get<u32>(0x101D5D90);which=get<u8>(source,0x1C0);
    ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);current=get<u8>(p,0x1C0);
    origin=get<u32>(source+which*4,0xC0);size=nx*ny*12;destination=get<u32>(p+current*4,0xC0);
    gabi::call<void>(0xC000A848,destination,origin,size);
    nx=get<u32>(p,0x98);ny=get<u32>(p,0x9C);source=get<u32>(0x101D5D90);
    destination=get<u32>(p,0xC8);size=nx*ny*12;origin=get<u32>(source,0xC8);
    gabi::call<void>(0xC000A848,destination,origin,size);
}
VERIFY(0x0251F188,clothCopy_0251F188);

void clothCopy_0251F4C4(u32 p) {
    WWHD_FUNC(0x0251F4C4,void,p);
    u32 current=get<u8>(p,0x1C0)^1;
    u32 ny=get<u32>(p,0x9C),nx=get<u32>(p,0x98);
    put<u8>(p,0x1C0,current&1);u32 size=nx*ny*12;
    u32 source=get<u32>(0x101D5D98),which=get<u8>(source,0x1C0);
    u32 destination=get<u32>(p+(current&1)*4,0xB8),origin=get<u32>(source+which*4,0xB8);
    gabi::call<void>(0xC000A848,destination,origin,size);
    source=get<u32>(0x101D5D98);which=get<u8>(source,0x1C0);
    ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);current=get<u8>(p,0x1C0);
    origin=get<u32>(source+which*4,0xC0);size=nx*ny*12;destination=get<u32>(p+current*4,0xC0);
    gabi::call<void>(0xC000A848,destination,origin,size);
    nx=get<u32>(p,0x98);ny=get<u32>(p,0x9C);source=get<u32>(0x101D5D98);
    destination=get<u32>(p,0xC8);size=nx*ny*12;origin=get<u32>(source,0xC8);
    gabi::call<void>(0xC000A848,destination,origin,size);
}
VERIFY(0x0251F4C4,clothCopy_0251F4C4);

void clothCopy_0251F808(u32 p) {
    WWHD_FUNC(0x0251F808,void,p);
    u32 current=get<u8>(p,0x1C0)^1;
    u32 ny=get<u32>(p,0x9C),nx=get<u32>(p,0x98);
    put<u8>(p,0x1C0,current&1);u32 size=nx*ny*12;
    u32 source=get<u32>(0x101D5DA0),which=get<u8>(source,0x1C0);
    u32 destination=get<u32>(p+(current&1)*4,0xB8),origin=get<u32>(source+which*4,0xB8);
    gabi::call<void>(0xC000A848,destination,origin,size);
    source=get<u32>(0x101D5DA0);which=get<u8>(source,0x1C0);
    ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);current=get<u8>(p,0x1C0);
    origin=get<u32>(source+which*4,0xC0);size=nx*ny*12;destination=get<u32>(p+current*4,0xC0);
    gabi::call<void>(0xC000A848,destination,origin,size);
    nx=get<u32>(p,0x98);ny=get<u32>(p,0x9C);source=get<u32>(0x101D5DA0);
    destination=get<u32>(p,0xC8);size=nx*ny*12;origin=get<u32>(source,0xC8);
    gabi::call<void>(0xC000A848,destination,origin,size);
}
VERIFY(0x0251F808,clothCopy_0251F808);

void clothVobjInit_0251ECE8(u32 p) {
    WWHD_FUNC(0x0251ECE8,void,p);
    u32 frame=get<u32>(0x101FF560),last=get<u32>(0x101D5D84);
    if(last==frame) {
        u32 current=get<u8>(p,0x1C0),nx=get<u32>(p,0x98),source=get<u32>(0x101D5D88),ny=get<u32>(p,0x9C);
        u32 other=get<u8>(source,0x1C0),destination=get<u32>(p+current*4,0xB8),origin=get<u32>(source+other*4,0xB8);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
        current=get<u8>(p,0x1C0);source=get<u32>(0x101D5D88);ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);other=get<u8>(source,0x1C0);
        destination=get<u32>(p+current*4,0xC0);origin=get<u32>(source+other*4,0xC0);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
        ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);destination=get<u32>(p,0xC8);source=get<u32>(0x101D5D88);origin=get<u32>(source,0xC8);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
    } else gabi::call<void>(0x0251CEC8,p);
    u32 game=gabi::call<u32>(0x025200D4),stage=game+0x5150;
    u32 info=gabi::call_ptr<u32>(get<u32>(get<u32>(stage),0x15C),stage);
    put<u8>(p,0xA4D,((get<u32>(info,0xC)>>16)&7)==2);
    gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x64),p);
}
VERIFY(0x0251ECE8,clothVobjInit_0251ECE8);

void clothVobjInit_0251F034(u32 p) {
    WWHD_FUNC(0x0251F034,void,p);
    u32 frame=get<u32>(0x101FF560),last=get<u32>(0x101D5D8C);
    if(last==frame) {
        u32 current=get<u8>(p,0x1C0),nx=get<u32>(p,0x98),source=get<u32>(0x101D5D90),ny=get<u32>(p,0x9C);
        u32 other=get<u8>(source,0x1C0),destination=get<u32>(p+current*4,0xB8),origin=get<u32>(source+other*4,0xB8);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
        current=get<u8>(p,0x1C0);source=get<u32>(0x101D5D90);ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);other=get<u8>(source,0x1C0);
        destination=get<u32>(p+current*4,0xC0);origin=get<u32>(source+other*4,0xC0);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
        ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);destination=get<u32>(p,0xC8);source=get<u32>(0x101D5D90);origin=get<u32>(source,0xC8);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
    } else gabi::call<void>(0x0251CEC8,p);
    u32 game=gabi::call<u32>(0x025200D4),stage=game+0x5150;
    u32 info=gabi::call_ptr<u32>(get<u32>(get<u32>(stage),0x15C),stage);
    put<u8>(p,0xA4D,((get<u32>(info,0xC)>>16)&7)==2);
    gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x64),p);
}
VERIFY(0x0251F034,clothVobjInit_0251F034);

void clothVobjInit_0251F370(u32 p) {
    WWHD_FUNC(0x0251F370,void,p);
    u32 frame=get<u32>(0x101FF560),last=get<u32>(0x101D5D94);
    if(last==frame) {
        u32 current=get<u8>(p,0x1C0),nx=get<u32>(p,0x98),source=get<u32>(0x101D5D98),ny=get<u32>(p,0x9C);
        u32 other=get<u8>(source,0x1C0),destination=get<u32>(p+current*4,0xB8),origin=get<u32>(source+other*4,0xB8);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
        current=get<u8>(p,0x1C0);source=get<u32>(0x101D5D98);ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);other=get<u8>(source,0x1C0);
        destination=get<u32>(p+current*4,0xC0);origin=get<u32>(source+other*4,0xC0);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
        ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);destination=get<u32>(p,0xC8);source=get<u32>(0x101D5D98);origin=get<u32>(source,0xC8);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
    } else gabi::call<void>(0x0251CEC8,p);
    u32 game=gabi::call<u32>(0x025200D4),stage=game+0x5150;
    u32 info=gabi::call_ptr<u32>(get<u32>(get<u32>(stage),0x15C),stage);
    put<u8>(p,0xA4D,((get<u32>(info,0xC)>>16)&7)==2);
    gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x64),p);
}
VERIFY(0x0251F370,clothVobjInit_0251F370);

void clothVobjInit_0251F6B4(u32 p) {
    WWHD_FUNC(0x0251F6B4,void,p);
    u32 frame=get<u32>(0x101FF560),last=get<u32>(0x101D5D9C);
    if(last==frame) {
        u32 current=get<u8>(p,0x1C0),nx=get<u32>(p,0x98),source=get<u32>(0x101D5DA0),ny=get<u32>(p,0x9C);
        u32 other=get<u8>(source,0x1C0),destination=get<u32>(p+current*4,0xB8),origin=get<u32>(source+other*4,0xB8);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
        current=get<u8>(p,0x1C0);source=get<u32>(0x101D5DA0);ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);other=get<u8>(source,0x1C0);
        destination=get<u32>(p+current*4,0xC0);origin=get<u32>(source+other*4,0xC0);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
        ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);destination=get<u32>(p,0xC8);source=get<u32>(0x101D5DA0);origin=get<u32>(source,0xC8);
        gabi::call<void>(0xC000A848,destination,origin,nx*ny*12);
    } else gabi::call<void>(0x0251CEC8,p);
    u32 game=gabi::call<u32>(0x025200D4),stage=game+0x5150;
    u32 info=gabi::call_ptr<u32>(get<u32>(get<u32>(stage),0x15C),stage);
    put<u8>(p,0xA4D,((get<u32>(info,0xC)>>16)&7)==2);
    gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x64),p);
}
VERIFY(0x0251F6B4,clothVobjInit_0251F6B4);

void clothMathInitializer() {
    WWHD_FUNC(0x0251FA00,void);
    put<u32>(0x1046F060,8,0);put<u32>(0x1046F060,0,0);
    put<u32>(0x1046F060,0xC,0);put<u32>(0x1046F060,4,0);
    gabi::call<void>(0x028F026C,u32(0x101D5D60));
    f32 first=get<f32>(0x1004B5F8),second=get<f32>(0x1004B5FC);
    put<f32>(0x1046F054,0,first);put<f32>(0x1046F058,0,second);
    gabi::call<void>(0x028ED6F8,u32(0x1046F05C));
    gabi::call<void>(0x028F026C,u32(0x101D5D6C));
    gabi::call<void>(0x028EAB2C,u32(0x1046F05D));
    gabi::call<void>(0x028F026C,u32(0x101D5D78));
}
VERIFY(0x0251FA00,clothMathInitializer);

void clothXluMaterialLoad(u32 p) {
    WWHD_FUNC(0x0251EB70,void,p);
    gabi::Local<SafeString_l> name;
    name->text=0x1004B54C;name->vtable=0x1004B5A8;
    u32 table=gabi::call<u32>(0x027FFCBC);
    s32 index=gabi::call<s32>(0x027B90AC,get<u32>(table,4),name.get());
    if(index>=0) {
        u32 count=get<u32>(table,8),materials=get<u32>(table,12);
        u32 entry=materials+(u32(index)<count?u32(index)*36:0);
        if(!get<u8>(entry,0x20)) {
            u32 resource=get<u32>(table,4),limit=get<u32>(resource,0x1C);
            u32 image=u32(index)<limit?get<u32>(resource,0x20)+u32(index)*132:0;
            gabi::call<void>(0x02800B0C,entry,image,u32(0));
            count=get<u32>(table,8);materials=get<u32>(table,12);
        }
        entry=materials+(u32(index)<count?u32(index)*36:0);
        gabi::call<void>(0x0280068C,p+0x1C4,entry,u32(0));
    } else gabi::call<void>(0x0280068C,p+0x1C4,u32(0),u32(0));
    u32 model=get<u32>(p,0x1D0),count=get<u32>(p,0x1C4);
    for(u32 i=0;i<count;++i) {
        u32 limit=get<u32>(p,0x1C8),entry=get<u32>(p,0x1CC)+(i<limit?i*20:0);
        gabi::call<void>(0x028003E8,entry,get<u32>(entry),model,u32(0));
        count=get<u32>(p,0x1C4);
    }
}
VERIFY(0x0251EB70,clothXluMaterialLoad);

void clothVobjMove_0251EF0C(u32 p) {
 WWHD_FUNC(0x0251EF0C,void,p);
 if(get<u8>(p,0xA4C)) {
  u32 frame=get<u32>(0x101FF560),last=get<u32>(0x101D5D84);
  if(frame==last) { gabi::call<void>(0x0251EE3C,p);return; }
  put<u32>(0x101D5D84,0,frame);put<u32>(0x101D5D88,0,p);
 }
 bool inside=get<u8>(p,0xA4D)!=0;
 auto c=[](u32 o) {return get<f32>(0x10040000+o);};
 auto w=[p](u32 o,f32 v) {put<f32>(p,o,v);};
 auto h=[p](u32 o,s16 v) {put<s16>(p,o,v);};
 f32 f9=c(0xB5B8),f11=c(0xB5C0),f8=get<f32>(0x1047BBC8),f7=c(0xB5B4),f0=get<f32>(0x1047BBC4);
f8=gabi::fadds_ppc(f8,f9);f9=c(0xB5C4);f32 f10=c(0xB5BC);f7=gabi::fadds_ppc(f0,f7);f0=c(0xB5C8);
if(inside) {
 w(0x1A0,f10);w(0x1A4,f11);h(0x1BA,0);f32 f12=c(0xB5CC);f8=c(0xB564);
 w(0x1AC,f0);f7=gabi::fmuls_ppc(f7,f12);w(0xE8,f8);h(0x1BE,-800);w(0xE4,f7);
 f32 f13=c(0xB5D0);w(0x1A8,f9);h(0x1BC,900);h(0x1B6,0);w(0x1B0,f13);
} else {
 w(0x1A0,f10);w(0x1AC,f0);h(0x1BA,0);w(0x1A8,f9);h(0x1BC,900);f32 f13=c(0xB5D0);
 w(0xE4,f7);h(0x1B6,1024);h(0x1BE,-800);w(0x1B0,f13);w(0xE8,f8);w(0x1A4,f11);
}
 gabi::call<void>(0x0251D4EC,p);
}
VERIFY(0x0251EF0C,clothVobjMove_0251EF0C);

void clothVobjMove_0251F258(u32 p) {
 WWHD_FUNC(0x0251F258,void,p);
 if(get<u8>(p,0xA4C)) {
  u32 frame=get<u32>(0x101FF560),last=get<u32>(0x101D5D8C);
  if(frame==last) { gabi::call<void>(0x0251F188,p);return; }
  put<u32>(0x101D5D8C,0,frame);put<u32>(0x101D5D90,0,p);
 }
 bool inside=get<u8>(p,0xA4D)!=0;
 auto c=[](u32 o) {return get<f32>(0x10040000+o);};
 auto w=[p](u32 o,f32 v) {put<f32>(p,o,v);};
 auto h=[p](u32 o,s16 v) {put<s16>(p,o,v);};
 f32 f8=c(0xB5B8),f11=c(0xB5D4),f10=get<f32>(0x1047BBC8),f12=c(0xB5D8),f9=gabi::fadds_ppc(f10,f8);
f10=get<f32>(0x1047BBC4);f32 f13=c(0xB5B4),f0=c(0xB5C0);f8=gabi::fadds_ppc(f10,f13);f13=c(0xB5D0);
if(inside) {
 w(0x1A8,f13);w(0x1B0,f11);f9=c(0xB564);w(0x1A0,f12);w(0xE8,f9);f10=c(0xB5CC);
 h(0x1BA,0);h(0x1BC,900);f8=gabi::fmuls_ppc(f8,f10);w(0x1A4,f0);h(0x1B6,0);w(0xE4,f8);h(0x1BE,-800);w(0x1AC,f11);
} else {
 w(0x1A8,f13);w(0x1B0,f11);w(0xE4,f8);h(0x1BA,0);h(0x1BE,-800);h(0x1BC,900);w(0x1AC,f11);h(0x1B6,1024);w(0x1A0,f12);w(0x1A4,f0);w(0xE8,f9);
}
 gabi::call<void>(0x0251D4EC,p);
}
VERIFY(0x0251F258,clothVobjMove_0251F258);

void clothVobjMove_0251F594(u32 p) {
 WWHD_FUNC(0x0251F594,void,p);
 if(get<u8>(p,0xA4C)) {
  u32 frame=get<u32>(0x101FF560),last=get<u32>(0x101D5D94);
  if(frame==last) { gabi::call<void>(0x0251F4C4,p);return; }
  put<u32>(0x101D5D94,0,frame);put<u32>(0x101D5D98,0,p);
 }
 bool inside=get<u8>(p,0xA4D)!=0;
 auto c=[](u32 o) {return get<f32>(0x10040000+o);};
 auto w=[p](u32 o,f32 v) {put<f32>(p,o,v);};
 auto h=[p](u32 o,s16 v) {put<s16>(p,o,v);};
 f32 f12=c(0xB5DC),f9=c(0xB5B8),f11=get<f32>(0x1047BBC8),f0=c(0xB5B4),f8=gabi::fadds_ppc(f11,f9),f10=get<f32>(0x1047BBC4);
f32 f7=c(0xB5E0),f6=gabi::fadds_ppc(f10,f0),f13=c(0xB5D8);
if(inside) {
 w(0x1A4,f7);w(0x1B0,f12);f8=c(0xB564);f10=c(0xB5CC);w(0xE8,f8);f11=c(0xB5C4);
 f6=gabi::fmuls_ppc(f6,f10);w(0x1A0,f13);w(0x1A8,f11);w(0xE4,f6);h(0x1BE,0);h(0x1B6,0);h(0x1BA,0);w(0x1AC,f12);h(0x1BC,0);
} else {
 w(0x1B0,f12);w(0x1A0,f13);h(0x1B6,512);w(0xE4,f6);w(0x1A4,f7);f0=c(0xB5E8);f13=c(0xB5E4);
 w(0x1AC,f0);w(0x1A8,f13);h(0x1BC,900);w(0xE8,f8);h(0x1BA,0);h(0x1BE,-800);
}
 gabi::call<void>(0x0251D4EC,p);
}
VERIFY(0x0251F594,clothVobjMove_0251F594);

void clothVobjMove_0251F8D8(u32 p) {
 WWHD_FUNC(0x0251F8D8,void,p);
 if(get<u8>(p,0xA4C)) {
  u32 frame=get<u32>(0x101FF560),last=get<u32>(0x101D5D9C);
  if(frame==last) { gabi::call<void>(0x0251F808,p);return; }
  put<u32>(0x101D5D9C,0,frame);put<u32>(0x101D5DA0,0,p);
 }
 bool inside=get<u8>(p,0xA4D)!=0;
 auto c=[](u32 o) {return get<f32>(0x10040000+o);};
 auto w=[p](u32 o,f32 v) {put<f32>(p,o,v);};
 auto h=[p](u32 o,s16 v) {put<s16>(p,o,v);};
 f32 f0=c(0xB5B8),f12=c(0xB5B4),f13=get<f32>(0x1047BBC4),f11=get<f32>(0x1047BBC8);f12=gabi::fadds_ppc(f13,f12);
f32 f7=c(0xB5EC),f8=gabi::fadds_ppc(f11,f0),f9=c(0xB5C4);
if(inside) {
 w(0x1A8,f9);w(0x1AC,f7);h(0x1BA,0);h(0x1B6,0);f32 f10=c(0xB5CC);f8=c(0xB564);w(0x1B0,f7);
 f0=c(0xB5E0);h(0x1BC,900);f11=c(0xB5F0);h(0x1BE,-800);w(0xE8,f8);f12=gabi::fmuls_ppc(f12,f10);w(0x1A4,f0);w(0x1A0,f11);w(0xE4,f12);
} else {
 w(0x1A8,f9);w(0x1B0,f7);h(0x1BA,0);f0=c(0xB5F4);h(0x1B6,256);f13=c(0xB5BC);
 w(0xE4,f12);h(0x1BE,-800);w(0x1A4,f0);w(0xE8,f8);h(0x1BC,900);w(0x1AC,f7);w(0x1A0,f13);
}
 gabi::call<void>(0x0251D4EC,p);
}
VERIFY(0x0251F8D8,clothVobjMove_0251F8D8);

void clothInit(u32 p) {
 WWHD_FUNC(0x0251CEC8,void,p);
 u32 ny=get<u32>(p,0x9C),speed=get<u32>(p,0xC8),second=get<u32>(p,0xB4),first=get<u32>(p,0xB0),y=0;
 if(s32(ny)>0) {
  u32 nx=get<u32>(p,0x98);f32 zero=get<f32>(0x1004B564);
  do {
   u32 x=0;
   if(s32(nx)>0) {
    f32 negativeY=f32(s32(0-y));
    do {
     f32 fy=f32(s32(ny-1)),fx=f32(s32(x)),fw=f32(s32(nx-1));
     f32 vertical=gabi::fmuls_ppc(f32(f64(negativeY)/f64(fy)),get<f32>(p,0xA4));
     f32 width=get<f32>(p,0xA0),ratio=f32(f64(fx)/f64(fw));
     put<f32>(first,0,zero);f32 horizontal=gabi::fmuls_ppc(ratio,width);
     put<f32>(first,4,vertical);put<f32>(first,8,horizontal);
     u32 nextNy=get<u32>(p,0x9C),nextNx=get<u32>(p,0x98);
     f32 height=get<f32>(p,0xA4),vy=f32(f64(negativeY)/f64(f32(s32(nextNy-1))));
     f32 vx=f32(f64(fx)/f64(f32(s32(nextNx-1)))),otherWidth=get<f32>(p,0xA0);
     f32 otherVertical=gabi::fmuls_ppc(vy,height);put<f32>(second,0,zero);
     f32 otherHorizontal=gabi::fmuls_ppc(vx,otherWidth);put<f32>(second,4,otherVertical);put<f32>(second,8,otherHorizontal);
     put<f32>(speed,4,zero);put<f32>(speed,8,zero);put<f32>(speed,0,zero);
     speed+=12;++x;nx=get<u32>(p,0x98);second+=12;ny=get<u32>(p,0x9C);first+=12;
    } while(s32(x)<s32(nx));
   }
   ++y;
  } while(s32(y)<s32(ny));
 }
 gabi::call<void>(0x0251C968,p);
 u32 a=get<u32>(0x101FFBFC),current=get<u8>(p,0x1C0);put<u32>(p,0xD8,a);
 a=get<u32>(0x101FFC00);ny=get<u32>(p,0x9C);put<u32>(p,0xDC,a);
 a=get<u32>(0x101FFC04);u32 nx=get<u32>(p,0x98);put<u32>(p,0xE0,a);
 a=get<u32>(0x101FFBCC);put<u32>(p,0xCC,a);
 a=get<u32>(0x101FFBD0);put<u32>(p,0xD0,a);
 a=get<u32>(0x101FFBD4);put<u32>(p,0xD4,a);
 gabi::call<void>(0xC00088B8,get<u32>(p+current*4,0xB0),nx*ny*12);
 current=get<u8>(p,0x1C0);nx=get<u32>(p,0x98);ny=get<u32>(p,0x9C);
 gabi::call<void>(0xC00088B8,get<u32>(p+current*4,0xB8),nx*ny*12);
 ny=get<u32>(p,0x9C);nx=get<u32>(p,0x98);current=get<u8>(p,0x1C0);
 gabi::call<void>(0xC00088B8,get<u32>(p+current*4,0xC0),nx*ny*12);
 u32 vtable=get<u32>(p,0xC);put<u32>(p,0xAC,0x0251B560);
 gabi::call_ptr<void>(get<u32>(vtable,0x64),p);
}
VERIFY(0x0251CEC8,clothInit);

void clothMove(u32 p) {
 WWHD_FUNC(0x0251D4EC,void,p);
 gabi::Local<Vec_l> wind,force,temp;
 u32 angle=get<u16>(p,0x1B4);f32 amplitude=get<f32>(p,0xE8);
 u32 wy=get<u32>(p,0xD0);f32 base=get<f32>(p,0xE4);
 f32 sine=get<f32>(0x104A44F8+(angle&~7));u32 wx=get<u32>(p,0xCC),wz=get<u32>(p,0xD4);
 f32 scale=gabi::fmadds(amplitude,sine,base);
 put<u32>(wind.a,0,wx);put<u32>(wind.a,4,wy);put<u32>(wind.a,8,wz);
 gabi::call<void>(0x028E8E64,wind.a,wind.a,scale);
 u32 nx=get<u32>(p,0x98),current=get<u8>(p,0x1C0),ny=get<u32>(p,0x9C);
 f32 width=get<f32>(p,0xA0),horizontalFactor=get<f32>(p,0x1AC);
 u32 positions=get<u32>(p+current*4,0xB0);f32 horizontal=f32(f64(width)/f64(f32(s32(nx-1))));
 u32 normals=get<u32>(p+current*4,0xB8);u32 next=(current^1)&1;put<u8>(p,0x1C0,next);
 horizontal=gabi::fmuls_ppc(horizontal,horizontalFactor);
 f32 height=get<f32>(p,0xA4),vertical=f32(f64(height)/f64(f32(s32(ny-1))));
 f32 verticalFactor=get<f32>(p,0x1B0);vertical=gabi::fmuls_ppc(vertical,verticalFactor);
 f32 verticalSquare=gabi::fmuls_ppc(vertical,vertical);u32 speeds=get<u32>(p,0xC8);
 f32 square=gabi::fmadds(horizontal,horizontal,verticalSquare);u32 destination=get<u32>(p+next*4,0xB0);
 f64 diagonal=gabi::call<f64>(0x028F4384,square);
 ny=get<u32>(p,0x9C);u32 y=0;
 if(s32(ny)>0) {
  nx=get<u32>(p,0x98);
  do {
   u32 x=0;
   if(s32(nx)>0) {
    do {
     gabi::call<void>(0x0251D128,p,force.a,positions,normals,wind.a,x,y,horizontal,vertical,diagonal);
     nx=get<u32>(p,0x98);u32 fx=get<u32>(force.a),fz=get<u32>(force.a,8),fy=get<u32>(force.a,4);
     u32 offset=(x+y*nx)*12;put<u32>(temp.a,0,fx);put<u32>(temp.a,4,fy);put<u32>(temp.a,8,fz);
     gabi::call<void>(0x028E8D88,speeds+offset,temp.a,speeds+offset);
     nx=get<u32>(p,0x98);offset=(x+y*nx)*12;f32 damping=get<f32>(p,0x1A8);
     gabi::call<void>(0x028E8E64,speeds+offset,speeds+offset,damping);
     nx=get<u32>(p,0x98);offset=(x+y*nx)*12;
     put<u32>(destination+offset,0,get<u32>(positions+offset));
     put<u32>(destination+offset,4,get<u32>(positions+offset,4));put<u32>(destination+offset,8,get<u32>(positions+offset,8));
     nx=get<u32>(p,0x98);offset=(x+y*nx)*12;
     gabi::call<void>(0x028E8D88,destination+offset,speeds+offset,destination+offset);
     nx=get<u32>(p,0x98);++x;
    } while(s32(x)<s32(nx));
    ny=get<u32>(p,0x9C);
   }
   ++y;
  } while(s32(y)<s32(ny));
 }
 gabi::call<void>(0x0251C968,p);
 nx=get<u32>(p,0x98);ny=get<u32>(p,0x9C);current=get<u8>(p,0x1C0);
 gabi::call<void>(0xC00088B8,get<u32>(p+current*4,0xB0),nx*ny*12);
 nx=get<u32>(p,0x98);ny=get<u32>(p,0x9C);current=get<u8>(p,0x1C0);
 gabi::call<void>(0xC00088B8,get<u32>(p+current*4,0xB8),nx*ny*12);
 nx=get<u32>(p,0x98);ny=get<u32>(p,0x9C);current=get<u8>(p,0x1C0);
 gabi::call<void>(0xC00088B8,get<u32>(p+current*4,0xC0),nx*ny*12);
}
VERIFY(0x0251D4EC,clothMove);

void clothGetFactor(u32 p,u32 output,u32 positions,u32 normals,u32 wind,u32 x,u32 y,f64 horizontal,f64 vertical,f64 diagonal) {
 WWHD_FUNC(0x0251D128,void,p,output,positions,normals,wind,x,y,horizontal,vertical,diagonal);
 if(gabi::call_ptr<u32>(get<u32>(p,0xAC),p,x,y,horizontal,vertical,diagonal)) {
  if(!output) {output=gabi::call<u32>(0x0273AD10,u32(12));if(!output)return;}
  put<f32>(output,0,get<f32>(0x101FFBA8));put<f32>(output,4,get<f32>(0x101FFBAC));put<f32>(output,8,get<f32>(0x101FFBB0));return;
 }
 gabi::Local<Vec_l> point,normal,force;
 u32 nx=get<u32>(p,0x98),offset=(x+y*nx)*12;
 put<f32>(point.a,0,get<f32>(positions+offset));put<f32>(point.a,4,get<f32>(positions+offset,4));put<f32>(point.a,8,get<f32>(positions+offset,8));
 gabi::call<f64>(0x028E8F44,wind,normals+offset);
 nx=get<u32>(p,0x98);offset=(x+y*nx)*12;
 gabi::call<void>(0x0201AE48,normals+offset,normal.a);
 f32 a=get<f32>(normal.a),gravity=get<f32>(p,0x1A4);put<f32>(force.a,0,a);
 f32 b=get<f32>(normal.a,4),c=get<f32>(normal.a,8),by=gabi::fadds_ppc(b,gravity);
 nx=get<u32>(p,0x98);put<f32>(force.a,8,c);put<f32>(force.a,4,by);
 u32 flags=x?1:0;if(x!=nx-1)flags|=2;u32 ny=get<u32>(p,0x9C);if(y)flags|=4;if(y!=ny-1)flags|=8;
 auto spring=[&](u32 xx,u32 yy,f64 distance,u32 width) {
  u32 neighbor=positions+(xx+yy*width)*12;f32 factor=get<f32>(p,0x1A0);
  gabi::call<void>(0x0251B590,point.a,neighbor,force.a,distance,factor);
 };
 if(flags&1)spring(x-1,y,horizontal,nx);
 if(flags&2)spring(x+1,y,horizontal,get<u32>(p,0x98));
 if(flags&4)spring(x,y-1,vertical,get<u32>(p,0x98));
 if(flags&8)spring(x,y+1,vertical,get<u32>(p,0x98));
 if((flags&5)==5)spring(x-1,y-1,diagonal,get<u32>(p,0x98));
 if((flags&9)==9)spring(x-1,y+1,diagonal,get<u32>(p,0x98));
 if((flags&6)==6)spring(x+1,y-1,diagonal,get<u32>(p,0x98));
 if((flags&10)==10)spring(x+1,y+1,diagonal,get<u32>(p,0x98));
 if(!output){output=gabi::call<u32>(0x0273AD10,u32(12));if(!output)return;}
 put<f32>(output,0,get<f32>(force.a));put<f32>(output,4,get<f32>(force.a,4));put<f32>(output,8,get<f32>(force.a,8));
}
VERIFY(0x0251D128,clothGetFactor);

namespace {struct ClothNormalScratch_l {u8 bytes[156];};}
void clothSetNormals(u32 p) {
 WWHD_FUNC(0x0251C968,void,p);
 gabi::Local<ClothNormalScratch_l> scratch;u32 frame=scratch.a;
 auto v=[frame](u32 original) {return frame+original-8;};
 auto copy=[](u32 dst,u32 src) {u32 a=get<u32>(src),c=get<u32>(src,8),b=get<u32>(src,4);put<u32>(dst,0,a);put<u32>(dst,4,b);put<u32>(dst,8,c);};
 u32 current=get<u8>(p,0x1C0);s32 step=get<s16>(p,0x1B6);u32 ny=get<u32>(p,0x9C);
 s32 angle=get<s16>(p,0x1B4);u32 normals=get<u32>(p+current*4,0xB8);s32 secondAngle=get<s16>(p,0x1B8),secondStep=get<s16>(p,0x1BA);
 u32 positions=get<u32>(p+current*4,0xB0);put<s16>(p,0x1B4,s16(angle+step));put<s16>(p,0x1B8,s16(secondAngle+secondStep));
 u32 y=0;
 if(s32(ny)>0) {
  u32 nx=get<u32>(p,0x98);
  do {
   u32 x=0;
   if(s32(nx)>0) {
    do {
     u32 offset=(x+y*nx)*12,c=get<u32>(0x101FFBB0),b=get<u32>(0x101FFBAC),a=get<u32>(0x101FFBA8);
     put<f32>(v(0x20),0,get<f32>(positions+offset));put<f32>(v(0x20),4,get<f32>(positions+offset,4));put<f32>(v(0x20),8,get<f32>(positions+offset,8));
     put<u32>(v(0x14),0,a);put<u32>(v(0x14),4,b);put<u32>(v(0x14),8,c);
     auto addTriangle=[&](u32 verticalDelta,u32 horizontalDelta,bool upward,bool left) {
      copy(v(0x38),v(verticalDelta));
      gabi::call<void>(0x0201B080,(upward==left)?v(0x38):v(horizontalDelta),v(verticalDelta),(upward==left)?v(horizontalDelta):v(0x38));
      copy(v(8),v(verticalDelta));gabi::call<void>(0x0201B12C,v(8),v(verticalDelta));
      copy(v(8),v(verticalDelta));gabi::call<void>(0x028E8D88,v(0x14),v(8),v(0x14));
     };
     if(x!=0) {
      gabi::call<void>(0x0201ADE0,positions+(x-1+y*nx)*12,v(0x8C),v(0x20));copy(v(0x44),v(0x8C));
      if(y!=0) {nx=get<u32>(p,0x98);gabi::call<void>(0x0201ADE0,positions+(x+(y-1)*nx)*12,v(0x50),v(0x20));addTriangle(0x50,0x44,true,true);}
      ny=get<u32>(p,0x9C);
      if(y!=ny-1) {nx=get<u32>(p,0x98);gabi::call<void>(0x0201ADE0,positions+(x+(y+1)*nx)*12,v(0x5C),v(0x20));addTriangle(0x5C,0x44,false,true);}
      nx=get<u32>(p,0x98);
     }
     if(x!=nx-1) {
      gabi::call<void>(0x0201ADE0,positions+(x+1+y*nx)*12,v(0x98),v(0x20));copy(v(0x44),v(0x98));
      if(y!=0) {nx=get<u32>(p,0x98);gabi::call<void>(0x0201ADE0,positions+(x+(y-1)*nx)*12,v(0x68),v(0x20));addTriangle(0x68,0x44,true,false);}
      ny=get<u32>(p,0x9C);
      if(y!=ny-1) {nx=get<u32>(p,0x98);gabi::call<void>(0x0201ADE0,positions+(x+(y+1)*nx)*12,v(0x74),v(0x20));addTriangle(0x74,0x44,false,false);}
     }
     gabi::call<void>(0x0201B12C,v(0x14),v(0x80));
     s32 phaseStep=get<s16>(p,0x1BC),phase=get<s16>(p,0x1B4),amplitude=get<s16>(p,0x1BE);
     u32 phaseIndex=u16((y+x)*u32(phaseStep)+u32(phase))&~7u;
     f32 sine=get<f32>(0x104A44F8+phaseIndex),rotation=gabi::fmuls_ppc(f32(amplitude),sine);
     u32 by=get<u32>(v(0x80),4),bx=get<u32>(v(0x80)),bz=get<u32>(v(0x80),8);
     put<u32>(v(0x14),0,bx);put<u32>(v(0x14),4,by);put<u32>(v(0x14),8,bz);
     gabi::call<void>(0x025F1884,u32(0x1048D0CC),s32(s16(gabi::ftoi(rotation))));
     gabi::call<void>(0x028E8F64,u32(0x1048D0CC),v(0x14),v(8));gabi::call<void>(0x0201B12C,v(8),v(0x80));
     nx=get<u32>(p,0x98);offset=(x+y*nx)*12;
     put<u32>(normals+offset,0,get<u32>(v(0x80)));put<u32>(normals+offset,4,get<u32>(v(0x80),4));put<u32>(normals+offset,8,get<u32>(v(0x80),8));
     nx=get<u32>(p,0x98);++x;
    } while(s32(x)<s32(nx));
    ny=get<u32>(p,0x9C);
   }
   ++y;
  } while(s32(y)<s32(ny));
 }
 current=get<u8>(p,0x1C0);u32 back=get<u32>(p+current*4,0xC0);y=0;
 if(s32(ny)>0) {
  u32 nx=get<u32>(p,0x98);
  do {
   u32 x=0;
   if(s32(nx)>0) {
    do {
     f32 a=get<f32>(normals),b=get<f32>(normals,4),c=get<f32>(normals,8);
     put<f32>(back,0,-a);put<f32>(back,4,-b);put<f32>(back,8,-c);
     nx=get<u32>(p,0x98);++x;back+=12;normals+=12;
    } while(s32(x)<s32(nx));
    ny=get<u32>(p,0x9C);
   }
   ++y;
  } while(s32(y)<s32(ny));
 }
}
VERIFY(0x0251C968,clothSetNormals);

void clothPacketDtor(u32 p,u32 flags) {
 WWHD_FUNC(0x0251C558,void,p,flags);
 if(!p)return;
 u32 model=get<u32>(p,0x1D0);put<u32>(p,0xC,0x1004B600);
 auto release=[&](u32 object,u32 pointerOffset) {
  u32 heap=gabi::call<u32>(0x02755FEC,get<u32>(0x101F8B4C),get<u32>(object,pointerOffset));
  u32 target=get<u32>(get<u32>(heap,0xC),0x3C);
  gabi::call_ptr<void>(target,heap,get<u32>(object,pointerOffset));
 };
 u32 count=get<u32>(model,0x10);
 for(u32 i=0;i<count;) {
  for(u32 side=0;side<2;++side) {
   u32 baseOffset=side*8,n=get<u32>(model,baseOffset),array=get<u32>(model,baseOffset+4);
   u32 entry=array+(i<n?i*592:0);gabi::call<void>(0x027BF7E8,entry+0x15C);
   n=get<u32>(model,baseOffset);array=get<u32>(model,baseOffset+4);entry=array+(i<n?i*592:0);
   if(get<u32>(entry,4)) { (void)get<u32>(entry);release(entry,4);put<u32>(entry,0,0);put<u32>(entry,4,0); }
  }
  count=get<u32>(model,0x10);++i;
 }
 for(u32 side=0;side<2;++side) {
  u32 baseOffset=side*8,array=get<u32>(model,baseOffset+4);
  if(array) {
   u32 n=get<u32>(model,baseOffset),i=0,offset=0;
   while(s32(i)<s32(n)) {
    u32 entry=array+offset;
    if(entry) {gabi::call<void>(0x027BF880,entry+0x15C,u32(2));gabi::call<void>(0x027B5CBC,entry+8,u32(2));n=get<u32>(model,baseOffset);array=get<u32>(model,baseOffset+4);}
    ++i;offset+=592;
   }
   release(model,baseOffset+4);put<u32>(model,baseOffset,0);put<u32>(model,baseOffset+4,0);
  }
 }
 put<u32>(model,0x10,0);put<u32>(model,0x14,0);put<u8>(model,0x19,0);
 gabi::call<void>(0x027FEB60,get<u32>(p,0x1D0),u32(3));
 put<u32>(p,0x1D0,0);gabi::call<void>(0x027BE2B0,p+0x788,u32(2));gabi::call<void>(0x027BE2B0,p+0x5F0,u32(2));
 gabi::call<void>(0x027FB528,p+0x288,u32(0));gabi::call<void>(0x027FB528,p+0x1E0,u32(0));gabi::call<void>(0x027FD764,p+0x1D4,u32(2));
 u32 array=get<u32>(p,0x1CC);
 if(array) {
  u32 n=get<u32>(p,0x1C8),i=0,offset=0;
  while(s32(i)<s32(n)) {
   u32 entry=array+offset;
   if(entry) {
    u32 buffer=get<u32>(entry,8);put<u32>(entry,0,0);
    for(u32 side=0;side<2;++side) {
     u32 countOffset=4+side*8,pointerOffset=countOffset+4;
     if(side)buffer=get<u32>(entry,pointerOffset);
     if(buffer) {
      u32 vertices=get<u32>(entry,countOffset),j=0,vertexOffset=0;
      while(s32(j)<s32(vertices)) {
       u32 object=buffer+vertexOffset,target=get<u32>(get<u32>(object,0xF0),0xC);
       gabi::call_ptr<void>(target,object,u32(2));vertices=get<u32>(entry,countOffset);++j;vertexOffset+=244;buffer=get<u32>(entry,pointerOffset);
      }
      release(entry,pointerOffset);put<u32>(entry,countOffset,0);put<u32>(entry,pointerOffset,0);
     }
    }
    n=get<u32>(p,0x1C8);array=get<u32>(p,0x1CC);
   }
   ++i;offset+=20;
  }
  release(p,0x1CC);put<u32>(p,0x1C8,0);put<u32>(p,0x1CC,0);
 }
 gabi::call<void>(0x027F13DC,p,u32(0));if(flags&1)gabi::call<void>(0x0273AF40,p);
}
VERIFY(0x0251C558,clothPacketDtor);

namespace { struct ColorFloat_l {be<f32> values[4];}; }
void clothPrepareRender(u32 p) {
 WWHD_FUNC(0x0251DDE8,void,p);
 gabi::Local<Mtx_l> view,model;gabi::Local<ColorFloat_l> color,first,second;
 gabi::call<void>(0x0251D864,p);
 gabi::call<void>(0x0255F8F4,get<u32>(p,0xA8));gabi::call<void>(0x0255FE90,get<u32>(p,0xA8));
 gabi::call<void>(0x0251D7C4,view.a,u32(0x104B45F8));
 u32 resource=get<u32>(0x104B4708);
 gabi::call<void>(0x027FDA54,p+0x1D4,u32(0),view.a,u32(0x104B470C),resource+0x240);
 u32 env=get<u32>(p,0xA8),uniform=get<u32>(p,0x1D8);
 gabi::call<void>(0x0251DD24,color.a,env+0x90);
 env=get<u32>(p,0xA8);gabi::call<void>(0x0274D458,first.a,color.a,get<f32>(env,0x28));
 for(u32 i=0;i<4;++i)put<u32>(uniform,0x1C4+i*4,get<u32>(first.a,i*4));
 env=get<u32>(p,0xA8);uniform=get<u32>(p,0x1D8);gabi::call<void>(0x0251DD24,color.a,env+0x160);
 env=get<u32>(p,0xA8);gabi::call<void>(0x0274D458,second.a,color.a,get<f32>(env,0x16C));
 put<u32>(uniform,0x1D4,get<u32>(second.a));put<u32>(uniform,0x1D8,get<u32>(second.a,4));put<u32>(uniform,0x1DC,get<u32>(second.a,8));put<u32>(uniform,0x1E0,get<u32>(second.a,12));
 put<u32>(p,0x284,0);gabi::call<void>(0x028E90D4,p+0x170,u32(0x1048D0CC));
 f32 tx=get<f32>(p,0xD8),ty=get<f32>(p,0xDC),tz=get<f32>(p,0xE0);
 gabi::call<void>(0x025F2518,u32(0x1048D0CC),tx,ty,tz);
 for(u32 i=0;i<12;++i)put<f32>(model.a,i*4,get<f32>(0x1048D0CC,i*4));
 gabi::call<void>(0x028E90D4,model.a,p+0x254);
 env=get<u32>(p,0xA8);s16 a=get<s16>(env,0x90),b=get<s16>(env,0x92),c=get<s16>(env,0x94),d=get<s16>(env,0x96);
 f32 divisor=get<f32>(0x1004B584),af=f32(f64(a)/f64(divisor)),bf=f32(f64(b)/f64(divisor)),cf=f32(f64(c)/f64(divisor)),df=f32(f64(d)/f64(divisor));
 put<f32>(p,0x340,bf);put<f32>(p,0x33C,af);put<f32>(p,0x344,cf);put<f32>(p,0x348,df);
 u8 ba=get<u8>(env,0x98),bb=get<u8>(env,0x99),bc=get<u8>(env,0x9A),bd=get<u8>(env,0x9B);
 af=f32(f64(ba)/f64(divisor));bf=f32(f64(bb)/f64(divisor));cf=f32(f64(bc)/f64(divisor));df=f32(f64(bd)/f64(divisor));
 put<f32>(p,0x350,bf);put<f32>(p,0x34C,af);put<f32>(p,0x354,cf);put<f32>(p,0x358,df);
 gabi::call<void>(0x0274D2AC,p+0x34C,get<f32>(env,0x24));
 bd=get<u8>(env,0x9F);
 if(bd) {
  ba=get<u8>(env,0x9C);bc=get<u8>(env,0x9E);bb=get<u8>(env,0x9D);
  af=f32(f64(ba)/f64(divisor));bf=f32(f64(bb)/f64(divisor));cf=f32(f64(bc)/f64(divisor));df=f32(f64(bd)/f64(divisor));
  put<f32>(p,0x360,bf);put<f32>(p,0x364,cf);put<f32>(p,0x35C,af);put<f32>(p,0x368,df);
 } else {
  f32 zero=get<f32>(0x1004B564);put<f32>(p,0x360,zero);put<f32>(p,0x364,zero);put<f32>(p,0x35C,zero);put<f32>(p,0x368,zero);
 }
 gabi::call<void>(0x027FE0DC,p+0x1D4,u32(0));
}
VERIFY(0x0251DDE8,clothPrepareRender);

void clothUpdateVertexBuffers(u32 p) {
 WWHD_FUNC(0x0251D864,void,p);
 u32 nx=get<u32>(p,0x98),ny=get<u32>(p,0x9C),columns=nx-1,current=get<u8>(p,0x1C0);
 f32 unit=get<f32>(0x1004B580),du=f32(f64(unit)/f64(f32(s32(columns)))),dv=f32(f64(unit)/f64(f32(s32(ny-1)))),zero=get<f32>(0x1004B564);
 u32 positions=get<u32>(p+current*4,0xB0),normal=get<u32>(p+current*4,0xB8),model=get<u32>(p,0x1D0);
 for(u32 side=0;side<2;++side) {
  f32 u=zero;u32 x=0;
  if(side){current=get<u8>(p,0x1C0);normal=get<u32>(p+current*4,0xC0);}
  while(s32(x)<s32(columns)) {
   u32 selector=get<u32>(model,0x1C),table=model+selector*8,poolCount=get<u32>(table),pool=get<u32>(table,4),row=side?columns+x:x;
   u32 entry=pool+(row<poolCount?row*592:0),vertex=0,y=0;f32 nextU=gabi::fadds_ppc(u,du),v=zero;
   auto destination=[&](u32 index){u32 count=get<u32>(entry),base=get<u32>(entry,4);return base+(index<count?index*152:0);};
   auto copy=[](u32 dest,u32 src,u32 offset){f32 c=get<f32>(src,8),a=get<f32>(src),b=get<f32>(src,4);put<f32>(dest,offset,a);put<f32>(dest,offset+4,b);put<f32>(dest,offset+8,c);};
   if(s32(ny)>0) {
    do {
     u32 dst=destination(vertex);copy(dst,positions+(x+y*nx)*12,0);
     dst=destination(vertex);nx=get<u32>(p,0x98);copy(dst,normal+(x+y*nx)*12,12);
     dst=destination(vertex);
     if(!side){put<f32>(dst,0x3C,v);put<f32>(dst,0x38,u);}else{put<f32>(dst,0x38,u);put<f32>(dst,0x3C,v);}
     ++vertex;dst=destination(vertex);nx=get<u32>(p,0x98);copy(dst,positions+(x+1+y*nx)*12,0);
     dst=destination(vertex);nx=get<u32>(p,0x98);copy(dst,normal+(x+1+y*nx)*12,12);
     dst=destination(vertex);put<f32>(dst,0x3C,v);put<f32>(dst,0x38,nextU);
     ++y;ny=get<u32>(p,0x9C);++vertex;v=gabi::fadds_ppc(v,dv);nx=get<u32>(p,0x98);
    }while(s32(y)<s32(ny));
    model=get<u32>(p,0x1D0);columns=nx-1;
   }
   u=nextU;++x;
  }
 }
 gabi::call<void>(0x027FF1D8,model,u32(0),get<u32>(model,0x10));
}
VERIFY(0x0251D864,clothUpdateVertexBuffers);

u32 clothPacketCtor(u32 p,u32 image,u32 nx,u32 ny,u32 lighting,u32 supplied,f64 width,f64 height) {
 WWHD_FUNC(0x0251B648,u32,p,image,nx,ny,lighting,supplied,width,height);
 if(!p){p=gabi::call<u32>(0x0273AD10,u32(0xA40));if(!p)return 0;}
 gabi::call<void>(0x027F1278,p);f32 zero=get<f32>(0x1004B564);
 put<u32>(p,0x9C,0);put<u32>(p,0xA8,0);put<u32>(p,0xC,0x1004B600);put<u32>(p,0xAC,0);put<u32>(p,0x98,0);
 put<f32>(p,0xA0,zero);put<f32>(p,0xA4,zero);
 gabi::call<void>(0x028F521C,p+0xB0,u32(8));gabi::call<void>(0x028F521C,p+0xB8,u32(8));gabi::call<void>(0x028F521C,p+0xC0,u32(8));
 put<u32>(p,0xC8,0);put<f32>(p,0xE8,zero);put<u32>(p,0xEC,0);put<f32>(p,0xE4,zero);
 gabi::call<void>(0x028F521C,p+0xF0,u32(64));gabi::call<void>(0x028F521C,p+0x130,u32(64));gabi::call<void>(0x028F521C,p+0x170,u32(48));
 put<f32>(p,0x1A0,zero);put<f32>(p,0x1A4,zero);put<u16>(p,0x1B4,0);put<u16>(p,0x1BC,0);put<f32>(p,0x1B0,zero);
 put<u16>(p,0x1BA,0);put<u16>(p,0x1B6,0);put<u32>(p,0x1C4,0);put<f32>(p,0x1AC,zero);put<f32>(p,0x1A8,zero);put<u16>(p,0x1B8,0);put<u8>(p,0x1C0,0);put<u16>(p,0x1BE,0);
 u32 inlineArray=p+0x1C8;if(!inlineArray)inlineArray=gabi::call<u32>(0x0273AD10,u32(8));
 if(inlineArray){put<u32>(inlineArray,4,0);put<u32>(inlineArray,0,0);}
 gabi::call<void>(0x027FD6F4,p+0x1D4);gabi::call<void>(0x027FB40C,p+0x1E0);
 put<u32>(p,0x1EC,0x1016EF84);gabi::call<void>(0x028F521C,p+0x254,u32(52));
 if(p+0x254==0)gabi::call<u32>(0x0273AD10,u32(48));
 gabi::call<void>(0x027FB40C,p+0x288);put<u32>(p,0x294,0x1016EFB4);gabi::call<void>(0x028F521C,p+0x2FC,u32(0x2F0));
 f32 z=get<f32>(0x10145180);put<f32>(p,0x2FC,z);put<f32>(p,0x300,z);put<f32>(p,0x304,z);f32 one=get<f32>(0x1014517C);
 put<f32>(p,0x30C,z);
 put<f32>(p,0x328,one);
 put<f32>(p,0x334,z);
 put<f32>(p,0x318,one);
 put<f32>(p,0x314,z);
 put<f32>(p,0x364,z);
 put<f32>(p,0x384,z);
 put<f32>(p,0x354,z);
 put<f32>(p,0x378,one);
 put<f32>(p,0x32C,z);
 put<f32>(p,0x350,z);
 put<f32>(p,0x380,z);
 put<f32>(p,0x370,z);
 put<f32>(p,0x340,z);
 put<f32>(p,0x31C,z);
 put<f32>(p,0x330,z);
 put<f32>(p,0x338,one);
 put<f32>(p,0x320,z);
 put<f32>(p,0x344,z);
 put<f32>(p,0x310,z);
 put<f32>(p,0x36C,z);
 put<f32>(p,0x374,z);
 put<f32>(p,0x388,one);
 put<f32>(p,0x358,one);
 put<f32>(p,0x368,one);
 put<f32>(p,0x360,z);
 put<f32>(p,0x34C,z);
 put<f32>(p,0x37C,z);
 put<f32>(p,0x324,z);
 put<f32>(p,0x35C,z);
 put<f32>(p,0x308,one);
 put<f32>(p,0x33C,z);
 put<f32>(p,0x348,one);
 put<f32>(p,0x38C,z);
 put<f32>(p,0x390,z);
 put<f32>(p,0x394,z);
 put<f32>(p,0x398,one);
 put<f32>(p,0x39C,z);
 put<f32>(p,0x3A0,z);
 put<f32>(p,0x3A4,z);
 put<f32>(p,0x3A8,one);
 gabi::call<void>(0x028EFFD0,p+0x3AC,u32(2),u32(16),u32(0x0251FA94));
 gabi::call<void>(0x028EFFD0,p+0x3CC,u32(2),u32(16),u32(0x0251FA94));gabi::call<void>(0x028EFFD0,p+0x3EC,u32(2),u32(16),u32(0x0251FA94));
 for(u32 offset: {0x40Cu,0x43Cu,0x46Cu,0x49Cu,0x4CCu,0x4FCu,0x52Cu,0x55Cu})if(p+offset==0)gabi::call<u32>(0x0273AD10,u32(48));
 for(u32 offset: {0x58Cu,0x59Cu,0x5ACu,0x5BCu,0x5CCu,0x5DCu})if(p+offset==0)gabi::call<u32>(0x0273AD10,u32(16));
 put<u8>(p,0x5EC,0);gabi::call<void>(0x027BDF7C,p+0x5F0);gabi::call<void>(0x027BDF7C,p+0x788);
 gabi::call<void>(0x027BE6B8,p+0x920);gabi::call<void>(0x027BE6B8,p+0x9B0);
 put<u32>(p,0x9C,ny);put<f32>(p,0xA0,f32(width));put<u32>(p,0xA8,lighting);put<f32>(p,0xA4,f32(height));put<u32>(p,0x98,nx);
 gabi::call<void>(0x0251B638,p,u32(0x101F48F0));nx=get<u32>(p,0x98);ny=get<u32>(p,0x9C);u32 count=nx*ny;
 if(!supplied){put<u32>(p,0xB0,gabi::call<u32>(0x028EFFD0,u32(0),count,u32(12),u32(0)));put<u32>(p,0xB4,gabi::call<u32>(0x028EFFD0,u32(0),count,u32(12),u32(0)));}
 else {put<u32>(p,0xB0,get<u32>(supplied));put<u32>(p,0xB4,get<u32>(supplied,4));}
 for(u32 offset: {0xB8u,0xBCu,0xC0u,0xC4u,0xC8u})put<u32>(p,offset,gabi::call<u32>(0x028EFFD0,u32(0),count,u32(12),u32(0)));
 u32 model=gabi::call<u32>(0x027FE64C,u32(0));nx=get<u32>(p,0x98);ny=get<u32>(p,0x9C);put<u32>(p,0x1D0,model);
 gabi::call<void>(0x027FE6BC,model,nx*2-2,ny*2,u32(0));model=get<u32>(p,0x1D0);
 gabi::call<void>(0x027FF078,model,u32(0),get<u32>(model,0x10),u32(0));gabi::call<void>(0x027FE084,p+0x1D4,u32(1),u32(0));
 gabi::call<void>(0x0274FBF8,get<u32>(0x101F8B18));gabi::call<void>(0x02773798,p+0x9B0,get<u32>(image,0x20));
 bool mismatch=false;for(u32 offset: {4u,8u,12u,16u,20u,24u,56u,52u,28u})if(get<u32>(p+0x788,offset)!=get<u32>(p+0x9B0,offset)){mismatch=true;break;}
 if(mismatch)gabi::call<void>(0x027BDEB4,p+0x788,p+0x9B0);
 else{u32 a=get<u32>(p,0x9D8),b=get<u32>(p,0x9E0);put<u32>(p,0x7B0,a);put<u32>(p,0x864,b);put<u32>(p,0x85C,a);put<u32>(p,0x7B8,b);}
 gabi::call<void>(0x0274FCCC,get<u32>(0x101F8B18));return p;
}
VERIFY(0x0251B648,clothPacketCtor);

namespace {struct ClothRenderState_l {u8 bytes[0x120];};}
void clothMaterialDraw(u32 p,u32 state) {
 WWHD_FUNC(0x0251E1D8,void,p,state);
 u32 index=get<u32>(state,0xC),material=0;
 if(s32(index)<4){u32 count=get<u32>(p,0x1C8),array=get<u32>(p,0x1CC);material=get<u32>(array+(index<count?index*20:0));}
 u32 cache=gabi::call<u32>(0x027F29D4,u32(0x104B45C0)),shader=get<u32>(material),last=get<u32>(cache,4);
 if(shader!=last) {
  u8 flags=get<u8>(shader);u32 previous=get<u32>(cache);
  if(flags&2){put<u8>(shader,0,flags&~2);gabi::call<void>(0x027BB9E0,shader,u32(0));}
  u32 shaderResource=get<u32>(shader,0x7C),program=get<u32>(shaderResource,0x28);
  if(previous!=program)gabi::call<void>(0x027B9F68,program);
  u32 size=get<u32>(shader,0xC);
  if(size){gabi::call<void>(0xC00060E0,get<u32>(shader,4),size);put<u32>(cache,0,program);put<u32>(cache,4,shader);}
  else{gabi::call<void>(0x027BB7CC,shader);put<u32>(cache,4,shader);put<u32>(cache,0,program);}
 }
 index=get<u32>(state,0xC);
 auto bindUniforms=[&](u32 uniform) {
  u32 fieldIndex=get<u32>(uniform,0x4C),present=get<u32>(material,0xC),description=present?get<u32>(material,0x10):0;
  u32 block=uniform+0x10+fieldIndex*28; s32 vertex=get<s16>(description,0xC);u32 buffer=get<u32>(block,4),size=get<u32>(block,0xC);
  s32 pixel=get<s16>(description,0xE),geometry=get<s16>(description,0x10);
  if(pixel!=-1)gabi::call<void>(0xC0006900,pixel,size,buffer);
  if(vertex!=-1)gabi::call<void>(0xC0006A38,vertex,size,buffer);
  if(geometry!=-1)gabi::call<void>(0xC00068A8,geometry,size,buffer);
  return geometry!=-1;
 };
 auto bindMaterial=[&](u32 offset){u32 target=get<u32>(get<u32>(p,offset+0xC),0x2C);gabi::call_ptr<void>(target,p+offset,material);};
 if(index==0) {
  bindMaterial(0x1E0);u32 extra=get<u32>(state,0x14);
  if(extra){u32 uniform=get<u32>(extra,4);bindUniforms(uniform);}
 } else if(index==1 || index==2) {
  bindUniforms(get<u32>(p,0x1D8));bindMaterial(0x288);bindMaterial(0x1E0);
  if(index==2){u32 extra=get<u32>(state,0x30);if(extra)gabi::call_ptr<void>(get<u32>(get<u32>(extra,0xC),0x2C),extra,material);gabi::call<void>(0x027FFE54,state,material);}
 }
 u32 count=get<u32>(material,0x14),textures=count?get<u32>(material,0x18):0;
 gabi::call<void>(0x027BE53C,p+0x5F0,textures+4,u32(0),u32(0));
 count=get<u32>(material,0x14);textures=count>1?get<u32>(material,0x18)+20:0;
 gabi::call<void>(0x027BE53C,p+0x788,textures+4,u32(1),u32(0));
 gabi::Local<ClothRenderState_l> render;
 gabi::call<void>(0x02750250,render.a);
 u32 flags=get<u32>(render.a,0xEC);put<u32>(render.a,0xC,2);put<u32>(render.a,8,0);put<u8>(render.a,0xE0,1);
 flags=((flags&~15u)+7)&0xFFFFFF0F;put<u32>(render.a,0xEC,flags+16);
 gabi::call_ptr<void>(get<u32>(get<u32>(p,0xC),0x5C),p,render.a);
 gabi::call<void>(0x0280037C,get<u32>(state,0xC),render.a);gabi::call<void>(0x02750370,render.a);
 auto draw=[&](u32 row){
  u32 idx=get<u32>(state,0xC),limit=get<u32>(p,0x1C8),model=get<u32>(p,0x1D0),array=get<u32>(p,0x1CC),selector=get<u32>(model,0x1C);
  u32 entry=array+(idx<limit?idx*20:0),block=entry+(selector==0?8:0),n=get<u32>(block,4),buffer=get<u32>(block,8);
  gabi::call<void>(0x027BFE5C,buffer+(row<n?row*244:0));u32 vertices=get<u32>(p,0x9C)*2;
  if(vertices)gabi::call<void>(0xC0006178,get<u32>(0x104B4A88),vertices,get<u32>(0x104B4A84),get<u32>(0x104B4A8C),u32(0),u32(1));
 };
 u32 nx=get<u32>(p,0x98),x=0;
 while(s32(x)<s32(nx-1)){draw(x);nx=get<u32>(p,0x98);++x;}
 put<u32>(render.a,8,1);gabi::call<void>(0x02750684,render.a);nx=get<u32>(p,0x98);x=0;
 while(s32(x)<s32(nx-1)){draw(nx-1+x);nx=get<u32>(p,0x98);++x;}
 gabi::call<void>(0x02750370,u32(0x104B474C));
}
VERIFY(0x0251E1D8,clothMaterialDraw);
