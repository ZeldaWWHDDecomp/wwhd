/* d_kankyo_rain, WWHD: weather-vector and camera helpers.
 * Full TU inventory and remaining rendering/weather bodies are tracked separately.
 */
#include "bindings.h"
#include <bit>
namespace d_kankyo_rain_cpp {
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline f32 lf(u32 a) { return gabi::load<f32>(a); }
static inline f64 ldbl(u32 a) { return gabi::load<f64>(a); }
static inline void sf(u32 a, f32 v) { gabi::store<f32>(a,v); }
struct Vec_l { be<f32> x,y,z; };
struct DoublePos_l { be<f64> x,y,z; };
WWHD_SIZE(Vec_l,12);
WWHD_SIZE(DoublePos_l,24);

static void vectle_calc(u32 pos, u32 out) {
    WWHD_FUNC(0x02563E48,void,pos,out);
    f64 y=ldbl(pos+8), x=ldbl(pos), z=ldbl(pos+16);
    f64 sum=gabi::fmadd(z,z,gabi::fmadd(x,x,y*y));
    f64 magnitude=gabi::call<f64>(0x028F4384,(f64)(f32)sum);
    if (std::fabs((f32)magnitude)<lf(0x100030B8)) {
        f32 zero=lf(0x1004F528);
        sf(out+4,zero); sf(out,zero); sf(out+8,zero);
    } else {
        f64 reciprocal=ldbl(0x1004F520)/(f64)magnitude;
        sf(out,(f32)(ldbl(pos)*reciprocal));
        sf(out+4,(f32)(ldbl(pos+8)*reciprocal));
        sf(out+8,(f32)(ldbl(pos+16)*reciprocal));
    }
}
VERIFY(0x02563E48,vectle_calc);

static f32 subtract_fpr(f32 a,f32 b) {
    volatile f64 left=(f64)a,right=(f64)b;
    return (f32)(left-right);
}
static void get_vectle_calc(u32 eye,u32 center,u32 out) {
    WWHD_FUNC(0x02563F0C,void,eye,center,out);
    gabi::Local<DoublePos_l> delta;
    f32 x=subtract_fpr(lf(center),lf(eye)), y=subtract_fpr(lf(center+4),lf(eye+4));
    f32 z=subtract_fpr(lf(center+8),lf(eye+8));
    delta->x=(f64)x; delta->y=(f64)y; delta->z=(f64)z;
    gabi::call(0x02563E48,gabi::ea(delta.get()),out);
}
VERIFY(0x02563F0C,get_vectle_calc);

static void dKyr_get_vectle_calc(u32 eye,u32 center,u32 out) {
    WWHD_FUNC(0x02563F64,void,eye,center,out);
    gabi::call(0x02563F0C,eye,center,out);
}
VERIFY(0x02563F64,dKyr_get_vectle_calc);

static void dKy_set_eyevect_calc(u32 camera,u32 out,f32 distance,f32 height) {
    WWHD_FUNC(0x02563F68,void,camera,out,distance,height);
    gabi::Local<Vec_l> direction;
    gabi::call(0x02563F0C,camera+0xDC,camera+0xE8,gabi::ea(direction.get()));
    sf(out,gabi::fmadds(lf(gabi::ea(direction.get())),distance,lf(camera+0xDC)));
    f32 y=gabi::fmadds(lf(gabi::ea(direction.get())+4),height,lf(camera+0xE0));
    sf(out+4,y-lf(0x1004F52C));
    sf(out+8,gabi::fmadds(lf(gabi::ea(direction.get())+8),distance,lf(camera+0xE4)));
}
VERIFY(0x02563F68,dKy_set_eyevect_calc);

static void dKy_set_eyevect_calc2(u32 camera,u32 out,f32 distance,f32 height) {
    WWHD_FUNC(0x0256401C,void,camera,out,distance,height);
    gabi::Local<Vec_l> direction;
    gabi::Local<DoublePos_l> delta;
    f32 zero=lf(0x1004F528);
    f32 x=lf(camera+0xE8)-lf(camera+0xDC);
    f32 centerZ=lf(camera+0xF0);
    delta->x=(f64)x;
    if(height==zero) {
        delta->y=ldbl(0x1004F530);
        delta->z=(f64)(centerZ-lf(camera+0xE4));
    } else {
        delta->y=(f64)(lf(camera+0xEC)-lf(camera+0xE0));
        delta->z=(f64)(centerZ-lf(camera+0xE4));
    }
    gabi::call(0x02563E48,gabi::ea(delta.get()),gabi::ea(direction.get()));
    sf(out,gabi::fmadds(lf(gabi::ea(direction.get())),distance,lf(camera+0xDC)));
    sf(out+4,gabi::fmadds(lf(gabi::ea(direction.get())+4),height,lf(camera+0xE0)));
    sf(out+8,gabi::fmadds(lf(gabi::ea(direction.get())+8),distance,lf(camera+0xE4)));
    if(height==zero) sf(out+4,zero);
}
VERIFY(0x0256401C,dKy_set_eyevect_calc2);

static inline u32 environment() { return gabi::call<u32>(0x02555D0C); }
static inline u32 gameInfo() { return gabi::call<u32>(0x025200D4); }
static inline u32 windPacket() { return ld(environment()+0xAAC); }
static void dKyr_wind_init() {
    WWHD_FUNC(0x02564184,void);
    u32 camera=ld(gameInfo()+0x5AF8);
    gabi::store<u8>(windPacket()+0x764,0);
    gabi::store<u8>(windPacket()+0x765,0);
    u32 packet=windPacket();
    f32 randomScale=lf(0x1004F538),zero=lf(0x1004F528);
    gabi::store<u32>(packet+0x758,ld(camera+0xDC));
    gabi::store<u32>(packet+0x75C,ld(camera+0xE0));
    gabi::store<u32>(packet+0x760,ld(camera+0xE4));
    for(u32 offset=0;offset<0x40;offset+=0x20) {
        gabi::store<u8>(windPacket()+offset+0x636,0);
        sf(windPacket()+offset+0x630,zero);
        u32 particle=windPacket()+offset;
        f32 angle=gabi::call<f32>(0x020198D8,randomScale);
        gabi::store<u16>(particle+0x634,(u16)gabi::ftoi(angle));
        gabi::store<u32>(windPacket()+offset+0x618,0);
    }
}
VERIFY(0x02564184,dKyr_wind_init);
static u32 dKyr_moon_arrival_check() {
    WWHD_FUNC(0x02565DAC,u32);
    if(lf(environment()+0x1020)>lf(0x1004F60C)) return 1;
    return lf(environment()+0x1020)<lf(0x1004F610);
}
VERIFY(0x02565DAC,dKyr_moon_arrival_check);
static void rain_bg_chk(u32 packet,s32 index) {
    WWHD_FUNC(0x0256691C,void,packet,index);
    u32 camera=ld(gameInfo()+0x5AF8);
    sf(packet+(u32)index*56+0xD0,gabi::fadds_ppc(lf(camera+0xEC),lf(0x1004F6B4)));
}
VERIFY(0x0256691C,rain_bg_chk);
static u32 dKyr_poison_live_check() {
    WWHD_FUNC(0x0256C8BC,u32);
    return ld(environment()+0xA68)!=0;
}
VERIFY(0x0256C8BC,dKyr_poison_live_check);
static void dKyr_thunder_init() {
    WWHD_FUNC(0x02577110,void);
    gabi::store<u8>(environment()+0xAB1,0);
}
VERIFY(0x02577110,dKyr_thunder_init);

struct ProjectParams_l { u8 bytes[40]; };
WWHD_SIZE(ProjectParams_l,40);
// 0x02565B70 dKyr_lenzflare_move: body lives in d_kankyo_rain_movement_qa.cpp.

struct SafeString_l { be<u32> text,vtable; };
WWHD_SIZE(SafeString_l,8);
static inline u32 rainPacket() { return ld(environment()+0xA44); }
static void dKyr_rain_init() {
    WWHD_FUNC(0x0256670C,void);
    u32 camera=ld(gameInfo()+0x5AF8);
    gabi::Local<SafeString_l> archiveName,resourceName;
    archiveName->text=0x1004F67C; archiveName->vtable=0x1004F3AC;
    resourceName->text=0x1004F6A4; resourceName->vtable=0x1004F3AC;
    u32 resource=gabi::call<u32>(0x026124B0,ld(0x101F4F7C),archiveName.get(),resourceName.get(),0);
    u32 shader=gabi::call<u32>(0x027E2DC0,resource);
    u32 relative=ld(shader+0x24),array=relative?shader+0x24+relative:0;
    u32 uniform=gabi::call<u32>(0x027DFA24,array,0x1004F684u);
    gabi::store<u32>(rainPacket()+0x98,uniform);
    relative=ld(shader+0x24); array=relative?shader+0x24+relative:0;
    uniform=gabi::call<u32>(0x027DFA24,array,0x1004F690u);
    gabi::store<u32>(rainPacket()+0x9C,uniform);
    gabi::call(0x0257F490,rainPacket());
    u32 packet=rainPacket();
    for(u32 component=0;component<12;component+=4)
        gabi::store<u32>(packet+0x3760+component,ld(camera+0xDC+component));
    packet=rainPacket();
    for(u32 component=0;component<12;component+=4)
        gabi::store<u32>(packet+0x376C+component,ld(camera+0xE8+component));
    f32 zero=lf(0x1004F528);
    for(u32 offset=0x3784;offset<=0x3798;offset+=4) sf(rainPacket()+offset,zero);
    gabi::store<u8>(rainPacket()+0x379C,0);
    for(u32 offset=0x3778;offset<=0x3780;offset+=4) sf(rainPacket()+offset,zero);
    for(u32 i=0;i<250;i++) gabi::store<u8>(rainPacket()+0xA0+i*56,0);
    gabi::store<u16>(rainPacket()+0x3758,0);
}
VERIFY(0x0256670C,dKyr_rain_init);
static void dKyr_star_init() {
    WWHD_FUNC(0x0256A25C,void);
    // The native scoped heap guard constructor allocates four bytes when needed.
    gabi::Local<be<u32>> heapGuard;
    gabi::call(0x025F01D8,heapGuard.get(),0x1004F828u,0x49AED0u,0);
    u32 packet=gabi::call<u32>(0x0273AE48,0x49ADD8u,32);
    if(packet) packet=gabi::call<u32>(0x02580ED8,packet);
    gabi::store<u32>(environment()+0xA60,packet);
    if(ld(environment()+0xA60)) {
        gabi::Local<SafeString_l> name;
        name->text=0x1004F820; name->vtable=0x1004F3AC;
        u32 resource=gabi::call<u32>(0x026066C4,ld(0x101F4F28),name.get(),0x81);
        gabi::store<u32>(ld(environment()+0xA60)+0x98,resource);
        sf(ld(environment()+0xA60)+0xC0,lf(0x1004F550));
        gabi::store<u16>(ld(environment()+0xA60)+0xCA,0);
        gabi::store<u16>(ld(environment()+0xA60)+0xCC,0);
        gabi::store<u32>(ld(environment()+0xA60)+0xD0,0);
    } else {
        gabi::call(0x025F02D0,heapGuard.get());
        gabi::store<u32>(ld(environment()+0xA60)+0xD0,0);
    }
    gabi::call(0x025F0270,heapGuard.get(),2);
}
VERIFY(0x0256A25C,dKyr_star_init);
static void dKy_wave_chan_init() {
    WWHD_FUNC(0x0256EF7C,void);
    gabi::store<u16>(environment()+0x9F8,0);
    u32 env=environment();
    f32 zero=lf(0x1004F528);
    sf(env+0x9CC,lf(0x1004FAEC));
    sf(environment()+0x9D0,zero);
    sf(environment()+0x9D4,zero);
    sf(environment()+0x9D8,lf(0x1004F584));
    sf(environment()+0x9DC,lf(0x1004F7E8));
    sf(environment()+0x9E0,lf(0x1004FB6C));
    gabi::store<u8>(environment()+0x9FA,0);
    sf(environment()+0x9E4,lf(0x1004F54C));
    sf(environment()+0x9F0,lf(0x1004F730));
    sf(environment()+0x9E8,lf(0x1004FB70));
    sf(environment()+0x9EC,lf(0x1004F5B4));
    gabi::store<u8>(environment()+0x9FB,0);
    sf(environment()+0x9D8,lf(0x1004F588));
    sf(environment()+0x9F4,zero);
}
VERIFY(0x0256EF7C,dKy_wave_chan_init);

static void color_to_float(u32 out,u32 color) {
    WWHD_FUNC(0x0257015C,void,out,color);
    f32 divisor=lf(0x1004F5AC);
    f32 red=(f32)gabi::load<u8>(color)/divisor;
    f32 green=(f32)gabi::load<u8>(color+1)/divisor;
    f32 blue=(f32)gabi::load<u8>(color+2)/divisor;
    f32 alpha=(f32)gabi::load<u8>(color+3)/divisor;
    sf(out,red);sf(out+4,green);sf(out+8,blue);sf(out+12,alpha);
}
VERIFY(0x0257015C,color_to_float);
static f64 color_gamma_component(u32 color) {
    WWHD_FUNC(0x02572E9C,f64,color);
    f32 normalized=(f32)color/lf(0x1004F5AC);
    f64 result=gabi::call<f64>(0x028F4560,(f64)normalized,(f64)lf(0x1004FC90));
    f32 zero=lf(0x1004F528);
    if(result<zero) return zero;
    f32 one=lf(0x1004F550);
    if(result>one) return one;
    return result;
}
VERIFY(0x02572E9C,color_gamma_component);
static void color_gamma(u32 color) {
    WWHD_FUNC(0x02572F1C,void,color);
    f32 scale=lf(0x1004F5AC);
    u32 component=gabi::load<u8>(color);
    for(u32 i=0;i<3;i++) {
        f64 converted=gabi::call<f64>(0x02572E9C,component);
        f32 scaled=(f32)(converted*(f64)scale);
        s32 value=gabi::ftoi(scaled);
        // GHS loads the next byte before storing the converted current component.
        if(i<2) component=gabi::load<u8>(color+i+1);
        gabi::store<u8>(color+i,(u8)value);
    }
}
VERIFY(0x02572F1C,color_gamma);
static void matrix_float_copy(u32 out,u32 source) {
    WWHD_FUNC(0x02575ACC,void,out,source);
    f32 matrix[12];
    for(u32 i=0;i<12;i++) matrix[i]=lf(source+4*i);
    for(u32 i=0;i<12;i++) sf(out+4*i,matrix[i]);
}
VERIFY(0x02575ACC,matrix_float_copy);
struct MassCollisionResult_l { u8 bytes[20]; };
WWHD_SIZE(MassCollisionResult_l,20);
static void dKyr_poison_light_colision() {
    WWHD_FUNC(0x0256C8FC,void);
    u32 packet=ld(environment()+0xA6C);
    if(!gabi::call<u32>(0x0256C8BC)) return;
    u32 game=gameInfo(), status=game+0x26A4,manager=game+0x4EF8;
    gabi::call(0x020184DC,manager+0x110,lf(0x1004F918));
    gabi::call(0x02018428,manager+0x110,lf(0x1004F91C));
    gabi::store<u8>(status+0x297C,11);gabi::store<u8>(status+0x297D,3);
    s32 count=(s32)ld(environment()+0xA68);
    if(count<=0) return;
    f32 verticalOffset=lf(0x1004F920);
    gabi::Local<Vec_l> summed,pos;
    gabi::Local<be<u32>> mask;
    gabi::Local<MassCollisionResult_l> result;
    for(s32 i=0;i<count;i++) {
        u32 particle=packet+0x98+(u32)i*48;
        gabi::call(0x0201AD78,packet+0xBC18,summed.get(),particle+4);
        pos->x=(f32)summed->x;
        pos->y=gabi::fsubs_ppc((f32)summed->y,verticalOffset);
        pos->z=(f32)summed->z;
        gabi::call(0x0251694C,result.get());
        u32 hit=gabi::call<u32>(0x025170D8,gameInfo()+0x4EF8,pos.get(),mask.get(),result.get());
        u32 object=ld(gabi::ea(result.get())+4);
        if((hit&1)&&object&&(ld(object+0x10)&0x800000)&&gabi::load<u8>(particle)==1) {
            gabi::store<u16>(particle+0x2E,60);
            gabi::store<u8>(particle,2);
        }
        count=(s32)ld(environment()+0xA68);
    }
}
VERIFY(0x0256C8FC,dKyr_poison_light_colision);

static void rain_static_initializer() {
    WWHD_FUNC(0x02577664,void);
    gabi::store<u32>(0x104774B8,0);gabi::store<u32>(0x104774B0,0);
    gabi::store<u32>(0x104774BC,0);gabi::store<u32>(0x104774B4,0);
    gabi::call(0x028F026C,0x101E9A28u);
    sf(0x10477410,lf(0x1004FD28));sf(0x10477414,lf(0x1004FD2C));
    gabi::call(0x028ED6F8,0x1047741Cu);
    gabi::call(0x028F026C,0x101E9A34u);
    gabi::call(0x028EAB2C,0x1047741Du);
    gabi::call(0x028F026C,0x101E9A40u);
}
VERIFY(0x02577664,rain_static_initializer);

static inline u32 poisonPacket() { return ld(environment()+0xA6C); }
static void poison_init() {
    WWHD_FUNC(0x0256DC04,void);
    gabi::Local<be<u32>> heapGuard;
    gabi::call(0x025F01D8,heapGuard.get(),0x1004FA98u,0x4FAA10u,0);
    u32 packet=gabi::call<u32>(0x0273AE48,0x20BCA8u,32);
    if(packet) packet=gabi::call<u32>(0x025812EC,packet);
    gabi::store<u32>(environment()+0xA6C,packet);
    if(!poisonPacket()) {
        gabi::call(0x025F02D0,heapGuard.get());
        gabi::call(0x025F0270,heapGuard.get(),2);
        return;
    }
    f32 zero=lf(0x1004F528);
    sf(poisonPacket()+0xBC24,zero);
    sf(poisonPacket()+0xBC28,zero);
    sf(poisonPacket()+0xBC2C,zero);
    gabi::store<u32>(poisonPacket()+0xBC30,0);
    packet=poisonPacket();
    sf(packet+0xBC38,zero);
    gabi::Local<SafeString_l> archiveName,resourceName;
    archiveName->text=0x1004FA90;archiveName->vtable=0x1004F3AC;
    resourceName->text=0x1004FAB8;resourceName->vtable=0x1004F3AC;
    u32 resource=gabi::call<u32>(0x026124B0,ld(0x101F4F7C),archiveName.get(),resourceName.get(),0);
    u32 shader=gabi::call<u32>(0x027E2DC0,resource);
    u32 relative=ld(shader+0x24);
    u32 uniform=gabi::call<u32>(0x027DFA24,relative?shader+0x24+relative:0,0x1004FAC8u);
    gabi::store<u32>(poisonPacket()+0xBC34,uniform);
    gabi::call(0x02581518,poisonPacket());
    for(u32 i=0;i<1000;i++) gabi::store<u8>(poisonPacket()+0x98+48*i,0);
    gabi::call(0x0256CA54);
    gabi::store<s32>(0x101E99F0,(s8)gabi::load<u8>(0x1047E6C8));
    gabi::call(0x025F0270,heapGuard.get(),2);
}
VERIFY(0x0256DC04,poison_init);

struct SnapEntry_l { u8 bytes[52]; };
WWHD_SIZE(SnapEntry_l,52);
static void snap_sunmoon_proc(u32 pos,s32 type) {
    WWHD_FUNC(0x0256F084,void,pos,type);
    gabi::Local<SnapEntry_l> entry;
    gabi::call(0x025BD71C,entry.get());
    u32 camera=ld(gameInfo()+0x5AF8);
    if(!(ld(gameInfo()+0x5CDC)&8)) return;
    gabi::Local<Vec_l> scaledPos;
    f32 scale=lf(0x1004F6B8);
    f32 x=gabi::fsubs_ppc(lf(pos),lf(camera+0xDC));
    f32 y=gabi::fsubs_ppc(lf(pos+4),lf(camera+0xE0));
    f32 z=gabi::fsubs_ppc(lf(pos+8),lf(camera+0xE4));
    x=gabi::fmuls_ppc(x,scale);y=gabi::fmuls_ppc(y,scale);z=gabi::fmuls_ppc(z,scale);
    scaledPos->x=gabi::fadds_ppc(x,lf(camera+0xDC));
    scaledPos->y=gabi::fadds_ppc(y,lf(camera+0xE0));
    scaledPos->z=gabi::fadds_ppc(z,lf(camera+0xE4));
    gabi::call(0x025BEF3C,entry.get(),scaledPos.get(),lf(0x1004F614));
    u32 entryType=type==9?9:type==0?7:8;
    gabi::call(0x025BEB90,entry.get(),entryType,0,255,4,0x7FFF);
    gabi::call(0x025BEB4C,entry.get());
}
VERIFY(0x0256F084,snap_sunmoon_proc);

static void dKyr_star_move() {
    WWHD_FUNC(0x0256A388,void);
    environment();
    u32 packet=ld(environment()+0xA60);
    gameInfo();gameInfo();
    s16 count=gabi::load<s16>(environment()+0xA5A);
    gabi::store<u16>(packet+0xCC,(u16)count);
    if(!count) return;
    u32 counter=ld(packet+0xD0)+1;
    u32 angle=gabi::load<u16>(packet+0xCA);
    if(counter>719) counter=0;
    gabi::store<u32>(packet+0xD0,counter);
    f32 sine=std::fabs(lf(0x104A44F8+(angle&0xFFF8)));
    sf(packet+0xC0,gabi::fmadds(lf(0x1004F844),sine,lf(0x1004F640)));
    f32 random=gabi::call<f32>(0x02019918,lf(0x1004F5D0));
    s32 oldAngle=gabi::load<s16>(packet+0xCA);
    gabi::store<u16>(packet+0xCA,(u16)((u32)oldAngle+(u32)gabi::ftoi(random)+200));
}
VERIFY(0x0256A388,dKyr_star_move);

struct GroundCheck_l { u8 bytes[84]; };
struct RoofCheck_l { u8 bytes[80]; };
WWHD_SIZE(GroundCheck_l,84);
WWHD_SIZE(RoofCheck_l,80);
static f32 forward_fmadds(f32 a,f32 c,f32 b) {
    // Native FMA adds the camera operand after the product; retain its NaN payload.
    if(b!=b) return gabi::ppc_qnan(b);
    if(a!=a) return gabi::ppc_qnan(a);
    if(c!=c) return gabi::ppc_qnan(c);
    return gabi::fmadds(a,c,b);
}
static u32 forward_overhead_bg_chk(u32 out,f32 distance) {
    WWHD_FUNC(0x02566974,u32,out,distance);
    u32 camera=ld(gameInfo()+0x5AF8);gameInfo();
    gabi::Local<GroundCheck_l> groundQuery;
    gabi::Local<RoofCheck_l> roofQuery;
    gabi::Local<Vec_l> direction;
    u32 r=gabi::ea(groundQuery.get()),g=gabi::ea(roofQuery.get());
    gabi::call(0x02008E0C,groundQuery.get());
    gabi::store<u32>(r,r+0x40);gabi::store<u32>(r+4,r+0x4C);
    gabi::store<u8>(r+0x45,0);gabi::store<u8>(r+0x46,0);
    gabi::store<u32>(r+0x4C,0x1004F494);
    gabi::store<u8>(r+0x4A,0);gabi::store<u8>(r+0x49,0);
    gabi::store<u32>(r+0x10,0x1004F474);gabi::store<u32>(r+0x50,15);
    gabi::store<u32>(r+0x20,0x1004F484);
    gabi::store<u8>(r+0x47,0);gabi::store<u8>(r+0x48,0);
    gabi::store<u8>(r+0x44,1);gabi::store<u32>(r+0x40,0x1004F4A4);
    gabi::call(0x024EE7AC,roofQuery.get());
    gabi::call(0x02563F64,camera+0xDC,camera+0xE8,direction.get());
    f32 y=gabi::fadds_ppc(lf(camera+0xE0),lf(0x1004F564));
    f32 x=forward_fmadds(distance,(f32)direction->x,lf(camera+0xDC));
    f32 z=forward_fmadds(distance,(f32)direction->z,lf(camera+0xE4));
    sf(out+4,y);sf(out,x);sf(out+8,z);
    sf(g+0x3C,y);sf(g+0x40,z);sf(g+0x38,x);
    f32 roofHeight=gabi::call<f32>(0x024EF6E8,gameInfo()+0x12A0,roofQuery.get());
    u32 result=roofHeight!=lf(0x1004F630);
    sf(r+0x24,x);sf(r+0x2C,z);sf(r+0x28,gabi::fadds_ppc(y,lf(0x1004F594)));
    f32 floorHeight=gabi::call<f32>(0x02008974,gameInfo()+0x12A0,groundQuery.get());
    if(floorHeight>gabi::fadds_ppc(lf(camera+0xE0),lf(0x1004F564))) result=1;
    gabi::store<u32>(g+0x30,0x1004F3E4);
    gabi::store<u32>(g+0x20,0x1004F4B4);gabi::store<u32>(g+0x24,0x1004F4D4);
    gabi::call(0x02008B4C,g+0x10,0);
    gabi::store<u32>(r+0x40,0x1004F424);gabi::store<u32>(r+0x4C,0x1004F3E4);
    gabi::store<u32>(r+0x20,0x1004F404);
    gabi::call(0x02008DAC,groundQuery.get(),0);
    return result;
}
VERIFY(0x02566974,forward_overhead_bg_chk);

static bool equal_terminated_strings(u32 left,u32 right) {
    if(left==right) return true;
    for(u32 i=0;i<0x40001;i++) {
        u8 a=gabi::load<u8>(left+i),b=gabi::load<u8>(right+i);
        if(a!=b) return false;
        if(!a) return true;
    }
    return false;
}
static void safe_string_callback(SafeString_l* string) {
    gabi::call_ptr(ld((u32)string->vtable+0x14),string);
}
static inline u32 snowPacket() { return ld(environment()+0xA50); }
// 0x025688C4 dKyr_snow_init: body lives in d_kankyo_rain_movement_qa.cpp.

static void thunder_color(u32 target,u32 thunder,u32 scale,u32 red=90,u32 green=160,u32 blue=245) {
    gabi::call(target,red,green,blue,gabi::fmuls_ppc(lf(thunder+8),lf(scale)));
}
// 0x02577138 dKyr_thunder_move: body lives in d_kankyo_rain_movement_qa.cpp.

struct Matrix_l { be<f32> cells[12]; };
struct ColorF_l { be<f32> channels[4]; };
struct Quad_l { Vec_l vertices[4]; };
WWHD_SIZE(Matrix_l,48);
WWHD_SIZE(ColorF_l,16);
WWHD_SIZE(Quad_l,48);




static f32 wave_multiply_double(f32 a,f64 c) {
    u64 bits=std::bit_cast<u64>(c);
    bits=(bits&0xFFFFFFFFF8000000ull)+(bits&0x08000000ull);
    return (f32)((f64)a*std::bit_cast<f64>(bits));
}





}
