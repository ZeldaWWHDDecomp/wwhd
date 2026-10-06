#include "gabi.h"
struct GrassVector { u8 data[12]; };
struct GrassColor { u8 data[16]; };
struct GrassMatrix { u8 data[48]; };
struct GrassGround { u8 data[84]; };
u32 Grass_DataConstruct(u32 self) {
    WWHD_FUNC(0x0254A0DC, u32, self);
    if (!self) { self = gabi::call<u32>(0x0273AD10, 0x44); if (!self) return 0; }
    gmem_st8(self + 3, 0); gmem_st8(self + 2, 0); gmem_st8(self, 0); gmem_st8(self + 1, 0);
    gabi::call<void>(0x028F521C, self + 16, 48);
    gmem_st32(self + 0x40, 0); gmem_st8(self, 0);
    return self;
}
VERIFY(0x0254A0DC, Grass_DataConstruct);
s32 Grass_NewAnm(u32 self) {
    WWHD_FUNC(0x0254A14C, s32, self);
    u32 data = self + 0x190CC;
    for (s32 i = 8; i < 104; ++i, data += 0x38) {
        if (!gmem_ld8(data)) { gmem_st8(data, 1); gmem_st16(data + 4, 0); gmem_st16(data + 2, 0); return i; }
    }
    return -1;
}
VERIFY(0x0254A14C, Grass_NewAnm);
void Grass_RoomNewData(u32 self, u32 data) {
    WWHD_FUNC(0x0254AC40, void, self, data);
    gmem_st32(data + 0x40, gmem_ld32(self)); gmem_st32(self, data);
}
VERIFY(0x0254AC40, Grass_RoomNewData);
void Grass_RoomDeleteData(u32 self) {
    WWHD_FUNC(0x0254AC50, void, self);
    u32 data = gmem_ld32(self);
    while (data) {
        gmem_st8(data, 0);
        u32 current = gmem_ld32(self);
        gabi::call<void>(0x025E1B34, current + 4);
        current = gmem_ld32(self);
        data = gmem_ld32(current + 0x40); gmem_st32(self, data);
    }
}
VERIFY(0x0254AC50, Grass_RoomDeleteData);
void Grass_SetAnm(u32 self, u32 index, s32 angle) {
    WWHD_FUNC(0x0254ACB4, void, self, index, angle);
    u32 data = self + 0x18F0C + index * 0x38;
    gmem_st8(data, 1); gmem_st16(data + 4, 0); gmem_st16(data + 2, angle);
}
VERIFY(0x0254ACB4, Grass_SetAnm);
void Grass_MatrixCopy(u32 dest, u32 source) {
    WWHD_FUNC(0x0254C934, void, dest, source);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i) values[i] = gabi::load<f32>(source + i * 4);
    for (u32 i = 0; i < 12; ++i) gabi::store<f32>(dest + i * 4, values[i]);
}
VERIFY(0x0254C934, Grass_MatrixCopy);
void Grass_ColorToFloat(u32 dest, u32 source) {
    WWHD_FUNC(0x0254C9D4, void, dest, source);
    f32 r=(f32)gabi::load<s16>(source), b=(f32)gabi::load<s16>(source+4);
    f32 g=(f32)gabi::load<s16>(source+2), a=(f32)gabi::load<s16>(source+6);
    f32 denominator=gabi::load<f32>(0x1004E344);
    gabi::store<f32>(dest,r/denominator);gabi::store<f32>(dest+4,g/denominator);
    gabi::store<f32>(dest+8,b/denominator);gabi::store<f32>(dest+12,a/denominator);
}
VERIFY(0x0254C9D4, Grass_ColorToFloat);
void Grass_StaticInit() {
    WWHD_FUNC(0x0254D3F4, void);
    gmem_st32(0x104759A4,0);gmem_st32(0x1047599C,0);gmem_st32(0x104759A8,0);gmem_st32(0x104759A0,0);
    gabi::call<void>(0x028F026C,0x101E3A50);
    f32 lower=gabi::load<f32>(0x1004E380),upper=gabi::load<f32>(0x1004E384);
    gabi::store<f32>(0x10475988,lower);gabi::store<f32>(0x1047598C,upper);
    gabi::call<void>(0x028ED6F8,0x10475998);gabi::call<void>(0x028F026C,0x101E3A5C);
    gabi::call<void>(0x028EAB2C,0x10475999);gabi::call<void>(0x028F026C,0x101E3A68);
}
VERIFY(0x0254D3F4, Grass_StaticInit);
void grass_InlineDelete(u32 self, u32 flags) {
    WWHD_FUNC(0x0254D488, void, self, flags);
    if (self && (flags & 1)) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0254D488, grass_InlineDelete);

u32 grass_InlinePacketCtor(u32 self) {
    WWHD_FUNC(0x0254D570, u32, self);
    if (!self) self = gabi::call<u32>(0x0273AD10, 0xA8);
    if (self) {
        gabi::call<void>(0x027FB40C, self);
        gmem_st32(self + 12, 0x1016EF84);
        gabi::call<void>(0x028F521C, self + 0x74, 0x34);
        if (self + 0x74 == 0) gabi::call<u32>(0x0273AD10, 0x30);
    }
    return self;
}
VERIFY(0x0254D570, grass_InlinePacketCtor);

u32 grass_InlineColorCtor(u32 self) {
    WWHD_FUNC(0x0254D5E0, u32, self);
    return self ? self : gabi::call<u32>(0x0273AD10, 0x10);
}
VERIFY(0x0254D5E0, grass_InlineColorCtor);

u32 grass_InlineResourceCtor(u32 self) {
    WWHD_FUNC(0x0254D514, u32, self);
    if (!self) self = gabi::call<u32>(0x0273AD10, 0x254);
    if (self) {
        gabi::call<void>(0x027B5BD8, self + 4);
        gabi::call<void>(0x027BF734, self + 0x158);
        gmem_st32(self + 0x250, 0);
        gmem_st32(self + 0x24C, 0);
    }
    return self;
}
VERIFY(0x0254D514, grass_InlineResourceCtor);

void grass_InlineResourceDtor(u32 self, u32 flags) {
    WWHD_FUNC(0x0254D93C, void, self, flags);
    if (self) {
        gabi::call<void>(0x027BF880, self + 0x158, 2);
        gabi::call<void>(0x027B5CBC, self + 4, 2);
        if (flags & 1) gabi::call<void>(0x0273AF40, self);
    }
}
VERIFY(0x0254D93C, grass_InlineResourceDtor);

void grass_InlinePacketDtor(u32 self, u32 flags) {
    WWHD_FUNC(0x0254D894, void, self, flags);
    if (self) {
        gabi::call<void>(0x027FB528, self, 0);
        if (flags & 1) gabi::call<void>(0x0273AF40, self);
    }
}
VERIFY(0x0254D894, grass_InlinePacketDtor);

void grass_InlineEmpty0(u32 self) { WWHD_FUNC(0x0254D99C, void, self); }
VERIFY(0x0254D99C, grass_InlineEmpty0);
void grass_InlineEmpty1(u32 self) { WWHD_FUNC(0x0254D9A0, void, self); }
VERIFY(0x0254D9A0, grass_InlineEmpty1);
void grass_ShaderPacketDtor(u32 self, u32 flags) {
    WWHD_FUNC(0x0254D8E8, void, self, flags);
    if (self) {
        gabi::call<void>(0x027FB528, self, 0);
        if (flags & 1) gabi::call<void>(0x0273AF40, self);
    }
}
VERIFY(0x0254D8E8, grass_ShaderPacketDtor);

u32 Grass_AnmConstruct(u32 self) {
    WWHD_FUNC(0x0254D49C, u32, self);
    if (!self) self = gabi::call<u32>(0x0273AD10, 56);
    if (self) gmem_st8(self, 0);
    return self;
}
VERIFY(0x0254D49C, Grass_AnmConstruct);
u32 Grass_RoomConstruct(u32 self) {
    WWHD_FUNC(0x0254D4D8, u32, self);
    if (!self) self = gabi::call<u32>(0x0273AD10, 4);
    if (self) gmem_st32(self, 0);
    return self;
}
VERIFY(0x0254D4D8, Grass_RoomConstruct);
u32 Grass_ShaderPacketConstruct(u32 self) {
    WWHD_FUNC(0x0254D60C, u32, self);
    if (!self) { self = gabi::call<u32>(0x0273AD10, 0x364); if (!self) return 0; }
    gabi::call<void>(0x027FB40C, self);
    gmem_st32(self + 12, 0x1016EFB4);
    gabi::call<void>(0x028F521C, self + 0x74, 0x2F0);
    f32 zero=gabi::load<f32>(0x10145180),one=gabi::load<f32>(0x1014517C);
    gabi::store<f32>(self + 0x74, zero);
    gabi::store<f32>(self + 0x78, zero);
    gabi::store<f32>(self + 0x7c, zero);
    gabi::store<f32>(self + 0x84, zero);
    gabi::store<f32>(self + 0xa0, one);
    gabi::store<f32>(self + 0xac, zero);
    gabi::store<f32>(self + 0x90, one);
    gabi::store<f32>(self + 0x8c, zero);
    gabi::store<f32>(self + 0xdc, zero);
    gabi::store<f32>(self + 0xfc, zero);
    gabi::store<f32>(self + 0xcc, zero);
    gabi::store<f32>(self + 0xf0, one);
    gabi::store<f32>(self + 0xa4, zero);
    gabi::store<f32>(self + 0xc8, zero);
    gabi::store<f32>(self + 0xf8, zero);
    gabi::store<f32>(self + 0xe8, zero);
    gabi::store<f32>(self + 0xb8, zero);
    gabi::store<f32>(self + 0x94, zero);
    gabi::store<f32>(self + 0xa8, zero);
    gabi::store<f32>(self + 0xb0, one);
    gabi::store<f32>(self + 0x98, zero);
    gabi::store<f32>(self + 0xbc, zero);
    gabi::store<f32>(self + 0x88, zero);
    gabi::store<f32>(self + 0xe4, zero);
    gabi::store<f32>(self + 0xec, zero);
    gabi::store<f32>(self + 0x100, one);
    gabi::store<f32>(self + 0xd0, one);
    gabi::store<f32>(self + 0xe0, one);
    gabi::store<f32>(self + 0xd8, zero);
    gabi::store<f32>(self + 0xc4, zero);
    gabi::store<f32>(self + 0xf4, zero);
    gabi::store<f32>(self + 0x9c, zero);
    gabi::store<f32>(self + 0xd4, zero);
    gabi::store<f32>(self + 0x80, one);
    gabi::store<f32>(self + 0xb4, zero);
    gabi::store<f32>(self + 0xc0, one);
    gabi::store<f32>(self + 0x104, zero);
    gabi::store<f32>(self + 0x108, zero);
    gabi::store<f32>(self + 0x10c, zero);
    gabi::store<f32>(self + 0x110, one);
    gabi::store<f32>(self + 0x114, zero);
    gabi::store<f32>(self + 0x118, zero);
    gabi::store<f32>(self + 0x11c, zero);
    gabi::store<f32>(self + 0x120, one);
    for (u32 off : {0x124u,0x144u,0x164u}) gabi::call<void>(0x028EFFD0,self+off,2,16,0x0254D5E0);
    for (u32 off=0x184;off<=0x2D4;off+=0x30) if (!(self+off)) gabi::call<void>(0x0273AD10,0x30);
    for (u32 off=0x304;off<=0x354;off+=0x10) if (!(self+off)) gabi::call<void>(0x0273AD10,0x10);
    return self;
}
VERIFY(0x0254D60C, Grass_ShaderPacketConstruct);

struct GrassString { u8 data[8]; };
static void grass_StringValidate(GrassString* string) {
    u32 table=gmem_ld32(gabi::ea(string)+4);
    gabi::call_ptr<void>(gmem_ld32(table+20), string);
}
void Grass_SetBatta(u32 pos,u32 color) {
    WWHD_FUNC(0x02549D38,void,pos,color);
    gabi::Local<GrassString> stage,prefix,boss,other;
    bool spawn=false;
    if (gabi::call<u32>(0x0256019C)) return;
    u32 play=gabi::call<u32>(0x025200D4);
    if (gmem_ld8(play+0x5292)) return;
    play=gabi::call<u32>(0x025200D4);
    gmem_st32(stage.a+4,0x1004E1F4);gmem_st32(prefix.a+4,0x1004E1F4);
    gmem_st32(stage.a,play+0x5134);gmem_st32(prefix.a,0x1004E260);
    gabi::call<void>(0x0254D9A0,stage.get());
    grass_StringValidate(stage.get());
    u32 left=gmem_ld32(stage.a);
    grass_StringValidate(prefix.get());
    u32 right=gmem_ld32(prefix.a);
    if (left==right) return;
    bool different=false;
    for (u32 i=0;i<3;++i) { /* "kin" stage prefix: the original advances both cursors (r9 stage, r12 "kin" 0x1004E260; ctr=3) */
        u8 a=gmem_ld8(left),b=gmem_ld8(right);
        if (!a) { if (!b) return; different=true;break; }
        if (!b || a!=b) { different=true;break; }
        ++left;++right;
    }
    if (!different) return;
    gmem_st32(boss.a+4,0x1004E1F4);gmem_st32(boss.a,0x1004E264);
    play=gabi::call<u32>(0x025200D4);
    gmem_st32(other.a+4,0x1004E1F4);gmem_st32(other.a,play+0x5134);
    grass_StringValidate(boss.get());grass_StringValidate(boss.get());
    left=gmem_ld32(boss.a);
    grass_StringValidate(other.get());right=gmem_ld32(other.a);
    if (left==right) return;
    left=gmem_ld32(boss.a);right=gmem_ld32(other.a);
    for (u32 i=0;i<0x40001;++i) {
        u8 a=gmem_ld8(left),b=gmem_ld8(right);
        if (a!=b) break;
        if (!a) return;
        ++left;++right;
    }
    f32 random=gabi::call<f32>(0x02019788);
    f32 threshold=gabi::load<f32>(0x1004E25C);
    if (random>threshold) spawn=true;
    if (!spawn) return;
    play=gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x025A847C,gmem_ld32(play+0x5AB0),0,0x453,pos,0,0,255,0,-1,color,color,0);
}
VERIFY(0x02549D38,Grass_SetBatta);
f32 Grass_CheckGroundY(u32 pos) {
    WWHD_FUNC(0x02549F4C,f32,pos);
    gabi::Local<GrassGround> ground;
    gabi::call<void>(0x02008E0C,ground.get());
    f32 y=gabi::load<f32>(pos+4),x=gabi::load<f32>(pos);
    gmem_st8(ground.a+0x44,0);gmem_st32(ground.a+0x4C,0x1004E23C);
    f32 lift=gabi::load<f32>(0x1004E26C);
    gmem_st32(ground.a+0x20,0x1004E22C);gmem_st8(ground.a+0x49,0);
    y+=lift;gmem_st32(ground.a+0x50,1);gmem_st8(ground.a+0x45,0);
    gabi::store<f32>(ground.a+0x28,y);gabi::store<f32>(pos+4,y);
    gmem_st32(ground.a+0x10,0x1004E21C);gabi::store<f32>(ground.a+0x24,x);
    gmem_st8(ground.a+0x4A,0);gmem_st32(ground.a+4,ground.a+0x4C);
    gmem_st8(ground.a+0x47,0);gmem_st8(ground.a+0x48,0);
    gmem_st32(ground.a+0x40,0x1004E24C);
    f32 z=gabi::load<f32>(pos+8);
    gmem_st8(ground.a+0x46,0);gmem_st32(ground.a,ground.a+0x40);
    gabi::store<f32>(ground.a+0x2C,z);
    u32 play=gabi::call<u32>(0x025200D4);
    f32 height=gabi::call<f32>(0x02008974,play+0x12A0,ground.get());
    y=gabi::load<f32>(pos+4)-lift;
    gmem_st32(ground.a+0x40,0x1004E24C);gmem_st32(ground.a+0x20,0x1004E22C);
    f32 limit=gabi::load<f32>(0x1004E270);
    gmem_st32(ground.a+0x4C,0x1004E20C);gabi::store<f32>(pos+4,y);
    bool valid=height>limit;
    gabi::call<void>(0x02008DAC,ground.get(),0);
    return valid?height:y;
}
VERIFY(0x02549F4C,Grass_CheckGroundY);

void Grass_SetData(u32 self,u32 data,u32 index,u32 pos,u32 room,u32 item) {
    WWHD_FUNC(0x0254D19C,void,self,data,index,pos,room,item);
    bool immediate=gabi::call<u32>(0x025DBE00)!=0;
    f32 height;
    if (immediate) { height=gabi::call<f32>(0x02549F4C,pos);gmem_st8(data,2); }
    else { height=gabi::load<f32>(pos+4);gmem_st8(data,1); }
    f32 maximum=gabi::load<f32>(0x1004E350);
    gmem_st8(data+1,2);
    f32 random=gabi::call<f32>(0x020198D8,maximum);
    gmem_st8(data+2,(u8)gabi::ftoi(random));
    f32 x=gabi::load<f32>(pos),z=gabi::load<f32>(pos+8);
    gabi::store<f32>(data+4,x);gabi::store<f32>(data+8,height);
    gmem_st8(data+3,item);gabi::store<f32>(data+12,z);
    gabi::call<void>(0x0254AC40,self+0x1A5CC+room*4,data);
    gmem_st16(self+0x98,index);
}
VERIFY(0x0254D19C,Grass_SetData);
u32 Grass_NewData(u32 self,u32 pos,u32 room,u32 item) {
    WWHD_FUNC(0x0254D2D4,u32,self,pos,room,item);
    if (room>=64) gabi::call<void>(0x0273AA24,0x1004E354,0xB00,0x1004E360);
    u32 start=gmem_ld16(self+0x98),data=self+0x9C+start*0x44;
    for (u32 index=start;index<1500;++index,data+=0x44) {
        if (!gmem_ld8(data)) {
            gabi::call<void>(0x0254D19C,self,data,index,pos,room,item);return data;
        }
    }
    data=self+0x9C;
    for (u32 index=0;index<start;++index,data+=0x44) {
        if (!gmem_ld8(data)) {
            gabi::call<void>(0x0254D19C,self,data,index,pos,room,item);return data;
        }
    }
    return 0;
}
VERIFY(0x0254D2D4,Grass_NewData);
struct GrassParticleParams { u8 data[28]; };
void Grass_WorkCo(u32 self,u32 actor,u32 mode,s32 room) {
    WWHD_FUNC(0x0254A198,void,self,actor,mode,room);
    gabi::Local<GrassVector> delta,position;
    gabi::Local<GrassParticleParams> parameters;
    f32 x=gabi::load<f32>(self+4)-gabi::load<f32>(actor+0x314);
    f32 z=gabi::load<f32>(self+12)-gabi::load<f32>(actor+0x31C);
    f32 zero=gabi::load<f32>(0x1004E274);
    gabi::store<f32>(delta.a+4,zero);gabi::store<f32>(delta.a,x);gabi::store<f32>(delta.a+8,z);
    f32 squared=gabi::call<f32>(0x028E8DD0,delta.get());
    if (squared>gabi::load<f32>(0x1004E278)) return;
    u32 angle=gabi::call<u32>(0x020195B0,x,z);
    f32 distance=gabi::call<f32>(0x028F4384,squared);
    s32 index=gabi::load<s8>(self+2);
    if (index<8) {
        f32 speed=gabi::load<f32>(actor+0x370),threshold=gabi::load<f32>(0x1004E27C);
        if (speed>threshold) {
            f32 y=gabi::load<f32>(self+8),pz=gabi::load<f32>(self+12);
            f32 lift=gabi::load<f32>(0x1004E280),px=gabi::load<f32>(self+4);
            gabi::store<f32>(position.a+8,pz);gabi::store<f32>(position.a,px);gabi::store<f32>(position.a+4,y+lift);
            gabi::call<void>(0x025200D4);
            u32 color=0x1047E6CC+(u32)room*0x22C+0xEC;
            u32 play=gabi::call<u32>(0x025200D4);
            u32 packet=gmem_ld32(play+0x5AB8);
            u32 particle=gmem_ld16(packet+0x1A6CC);
            play=gabi::call<u32>(0x025200D4);
            gabi::call<void>(0x025A8D40,gmem_ld32(play+0x5AB0),particle,position.get(),255,color,color,1);
            play=gabi::call<u32>(0x025200D4);
            u32 background=play+0x12A0;
            play=gabi::call<u32>(0x025200D4);
            packet=gmem_ld32(play+0x5AB8);
            u32 emitter=gabi::call<u32>(0x025A8D10,gmem_ld32(background+0x4810),gmem_ld16(packet+0x1A6CC));
            if (emitter) {
                u32 target=gmem_ld32(emitter+4);
                if (target) {
                    gmem_st32(parameters.a,0x10000160);gmem_st32(parameters.a+4,0x1004E1F4);
                    gmem_st8(parameters.a+24,1);gmem_st8(parameters.a+26,0);
                    f32 one=gabi::load<f32>(0x1004E284);
                    gmem_st8(parameters.a+25,0);gabi::store<f32>(parameters.a+8,one);
                    gmem_st8(parameters.a+27,0);gabi::store<f32>(parameters.a+20,one);
                    gabi::store<f32>(parameters.a+12,one);gabi::store<f32>(parameters.a+16,one);
                    play=gabi::call<u32>(0x025200D4);
                    u32 control=gmem_ld32(play+0x5AB0);
                    for (u32 i=0;i<4;++i) gmem_st32(parameters.a+8+i*4,gmem_ld32(control+8+i*4));
                    gabi::call<void>(0x0281E5A8,target,parameters.get());
                }
            }
            gabi::call<void>(0x02549D38,self+4,color);
        }
        u32 play=gabi::call<u32>(0x025200D4);
        s32 fresh=gabi::call<s32>(0x0254A14C,gmem_ld32(play+0x5AB8));
        if (fresh<0) return;
        gmem_st8(self+2,fresh);
    }
    u32 play=gabi::call<u32>(0x025200D4);
    index=gabi::load<s8>(self+2);
    u32 packet=gmem_ld32(play+0x5AB8);
    f32 radius=gabi::load<f32>(0x1004E288);
    u32 animation=packet+0x18F0C+(u32)index*0x38;
    gmem_st16(animation+2,angle);
    u32 bend=gabi::call<u32>(0x020195B0,radius-distance,radius);
    gmem_st16(animation+4,bend);gmem_st8(animation,2);
}
VERIFY(0x0254A198,Grass_WorkCo);
void Grass_WorkAtNoCut(u32 self,u32 actor,u32 mode,s32 room,u32 hit,u32 object) {
    WWHD_FUNC(0x0254A458,void,self,actor,mode,room,hit,object);
    gabi::Local<GrassVector> velocity,horizontal;
    u32 info=gabi::call<u32>(0x025157AC,object);
    f32 x=gabi::load<f32>(info+0x7C);gabi::store<f32>(velocity.a,x);
    f32 y=gabi::load<f32>(info+0x80);gabi::store<f32>(velocity.a+4,y);
    f32 z=gabi::load<f32>(info+0x84),zero=gabi::load<f32>(0x1004E274);
    gabi::store<f32>(horizontal.a+4,zero);gabi::store<f32>(horizontal.a+8,z);
    gabi::store<f32>(horizontal.a,x);gabi::store<f32>(velocity.a+8,z);
    f32 magnitude=gabi::call<f32>(0x028E8DD0,horizontal.get());
    f32 epsilon=gabi::load<f32>(0x100030B8);
    if (fabsf(magnitude)<epsilon) {
        u32 table=gmem_ld32(object+0x3C);
        u32 shape=gabi::call_ptr<u32>(gmem_ld32(table+0x2C),object);
        table=gmem_ld32(shape+0x1C);
        if (gabi::call_ptr<u32>(gmem_ld32(table+0x9C),shape,self+4,velocity.get())) {
            f32 scale=gabi::load<f32>(0x1004E28C);
            gabi::call<void>(0x028E8E64,velocity.get(),velocity.get(),scale);
            x=gabi::load<f32>(velocity.a);z=gabi::load<f32>(velocity.a+8);
            gabi::store<f32>(horizontal.a,x);gabi::store<f32>(horizontal.a+4,zero);gabi::store<f32>(horizontal.a+8,z);
        } else {
            f32 ax=gabi::load<f32>(actor+0x314),px=gabi::load<f32>(self+4),az=gabi::load<f32>(actor+0x31C);
            x=px-ax;y=gabi::load<f32>(self+8)-gabi::load<f32>(actor+0x318);
            z=gabi::load<f32>(self+12)-az;
            gabi::store<f32>(horizontal.a,x);gabi::store<f32>(horizontal.a+4,zero);
            gabi::store<f32>(velocity.a+4,y);gabi::store<f32>(horizontal.a+8,z);
            gabi::store<f32>(velocity.a,x);gabi::store<f32>(velocity.a+8,z);
        }
        magnitude=gabi::call<f32>(0x028E8DD0,horizontal.get());
        if (fabsf(magnitude)<epsilon) return;
    }
    s32 angle=gabi::call<s32>(0x020195B0,gabi::load<f32>(velocity.a),gabi::load<f32>(velocity.a+8));
    s32 index=gabi::load<s8>(self+2);
    u32 play=gabi::call<u32>(0x025200D4),packet=gmem_ld32(play+0x5AB8);
    if (index<8) {
        s32 fresh=gabi::call<s32>(0x0254A14C,packet);
        if (fresh<0) return;
        gmem_st8(self+2,fresh);
        play=gabi::call<u32>(0x025200D4);
        packet=gmem_ld32(play+0x5AB8);
    }
    index=gabi::load<s8>(self+2);
    f32 weight=gabi::load<f32>(0x1004E290),bias=gabi::load<f32>(0x1004E294);
    u32 animation=packet+0x18F0C+(u32)index*0x38;
    f32 random=gabi::call<f32>(0x02019788);
    f32 factor=gabi::fmadds(random,weight,bias);
    u32 horizontalAngle=gabi::ftoi(gabi::fmuls_ppc((f32)angle,factor));
    f32 radius=gabi::load<f32>(0x1004E288);
    gmem_st16(animation+2,horizontalAngle);
    s32 bend=gabi::call<s32>(0x020195B0,magnitude,radius);
    random=gabi::call<f32>(0x02019788);
    factor=gabi::fmadds(random,weight,bias);
    u32 verticalAngle=gabi::ftoi(gabi::fmuls_ppc((f32)bend,factor));
    gmem_st8(animation,2);gmem_st16(animation+4,verticalAngle);
}
VERIFY(0x0254A458,Grass_WorkAtNoCut);

void Grass_WorkAt(u32 self,u32 actor,u32 mode,s32 room,u32 hit) {
    WWHD_FUNC(0x0254A7C8,void,self,actor,mode,room,hit);
    gabi::Local<GrassVector> position;
    gabi::Local<GrassParticleParams> parameters;
    u32 object=gmem_ld32(hit+4);
    if (object && (gmem_ld32(object+16)&0x3CC000)) {
        gabi::call<void>(0x0254A458,self,actor,mode,room,hit,object);return;
    }
    s32 index=gabi::load<s8>(self+2);
    if (index>=8) {
        u32 play=gabi::call<u32>(0x025200D4);
        index=gabi::load<s8>(self+2);
        u32 packet=gmem_ld32(play+0x5AB8);
        gmem_st8(packet+0x18F0C+(u32)index*0x38,0);
    }
    gmem_st8(self+2,255);
    if (!gmem_ld32(0x104759AC)) {
        gmem_st32(0x104759AC,1);
        gabi::call<void>(0x0201A478,0x10475990,0,0,0);
    }
    f32 x=gabi::load<f32>(self+4),y=gabi::load<f32>(self+8),lift=gabi::load<f32>(0x1004E2A0);
    gabi::store<f32>(position.a,x);
    f32 z=gabi::load<f32>(self+12);
    gabi::store<f32>(position.a+8,z);gabi::store<f32>(position.a+4,y+lift);
    gabi::call<void>(0x025200D4);
    u32 color=0x1047E6CC+(u32)room*0x22C+0xEC;
    u32 play=gabi::call<u32>(0x025200D4),packet=gmem_ld32(play+0x5AB8);
    u32 particle=gmem_ld16(packet+0x1A6CE);
    play=gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x025A8D40,gmem_ld32(play+0x5AB0),particle,position.get(),255,color,color,1);
    play=gabi::call<u32>(0x025200D4);
    u32 background=play+0x12A0;
    play=gabi::call<u32>(0x025200D4);packet=gmem_ld32(play+0x5AB8);
    u32 emitter=gabi::call<u32>(0x025A8D10,gmem_ld32(background+0x4810),gmem_ld16(packet+0x1A6CE));
    if (emitter) {
        u32 target=gmem_ld32(emitter+4);
        if (target) {
            gmem_st8(parameters.a+25,0);gmem_st8(parameters.a+27,0);
            f32 one=gabi::load<f32>(0x1004E284);
            gmem_st8(parameters.a+26,0);gabi::store<f32>(parameters.a+8,one);
            gabi::store<f32>(parameters.a+12,one);
            gmem_st32(parameters.a,0x10000160);gmem_st32(parameters.a+4,0x1004E1F4);
            gabi::store<f32>(parameters.a+16,one);gabi::store<f32>(parameters.a+20,one);gmem_st8(parameters.a+24,1);
            play=gabi::call<u32>(0x025200D4);
            u32 control=gmem_ld32(play+0x5AB0);
            for (u32 i=0;i<4;++i) gmem_st32(parameters.a+8+i*4,gmem_ld32(control+8+i*4));
            gabi::call<void>(0x0281E5A8,target,parameters.get());
        }
    }
    gabi::call<void>(0x02549D38,self+4,color);
    s32 item=gabi::load<s8>(self+3);
    if (item>=0) gabi::call<void>(0x025D8120,self+4,item,-1,room,0,0,1,0);
    if (!gmem_ld8(0x101E1D60)) {
        gmem_st8(0x101E1D60,1);
        u32 reverb=gabi::call<u32>(0x02520540,room);
        gabi::call<void>(0x025E1A40,0x282C,self+4,0,reverb);
    }
}
VERIFY(0x0254A7C8,Grass_WorkAt);
struct GrassHitInfo { u8 data[20]; };
void Grass_HitCheck(u32 self,s32 room) {
    WWHD_FUNC(0x0254AA3C,void,self,room);
    gabi::Local<GrassHitInfo> hit;
    gabi::Local<u32> actor;
    gabi::call<void>(0x0251694C,hit.get());
    u32 play=gabi::call<u32>(0x025200D4);
    u32 result=gabi::call<u32>(0x025170D8,play+0x4EF8,self+4,actor.get(),hit.get());
    bool attack=false;
    if (result&1) {
        u32 other=gmem_ld32(actor.a);
        if (other && gabi::load<s16>(other+8)!=0x1C5 && gabi::load<s16>(other+8)!=0x1C6) attack=true;
    }
    u32 contact=result&2;
    if (!contact && !attack) {
        s32 index=gabi::load<s8>(self+2);
        if (index<8) return;
        play=gabi::call<u32>(0x025200D4);
        index=gabi::load<s8>(self+2);
        u32 packet=gmem_ld32(play+0x5AB8),animation=packet+0x18F0C+(u32)index*0x38;
        u32 rotation=gmem_ld16(animation+2);
        s32 target=(s16)(rotation&0xE000);
        u32 baseIndex=rotation>>13;
        play=gabi::call<u32>(0x025200D4);
        packet=gmem_ld32(play+0x5AB8);
        u32 base=packet+0x18F0C+baseIndex*0x38;
        if (gmem_ld8(animation)==2) {
            u32 reverb=gabi::call<u32>(0x02520540,room);
            gabi::call<void>(0x025E1A40,0x3814,self+4,0,reverb);
            gmem_st8(animation,1);
        }
        if (gabi::call<u32>(0x0200F378,animation+4,(s32)gabi::load<s16>(base+4),16,4000,100)) return;
        if (!gabi::call<u32>(0x0200F8D0,animation+2,target,800)) return;
        play=gabi::call<u32>(0x025200D4);
        index=gabi::load<s8>(self+2);packet=gmem_ld32(play+0x5AB8);
        gmem_st8(packet+0x18F0C+(u32)index*0x38,0);
        gmem_st8(self+2,gmem_ld16(animation+2)>>13);
    } else {
        if (!gmem_ld32(actor.a)) gabi::call<void>(0x0273AA24,0x1004E2A4,0x80F,0x1004E2B0);
        if (contact) gabi::call<void>(0x0254A198,self,gmem_ld32(actor.a),result,room);
        if (attack) gabi::call<void>(0x0254A7C8,self,gmem_ld32(actor.a),result,room,hit.get());
    }
}
VERIFY(0x0254AA3C,Grass_HitCheck);

void Grass_Calc(u32 self) {
    WWHD_FUNC(0x0254C6C4, void, self);
    float wind=gabi::load<float>(0x1004E274);
    bool enabled=true;
    if(gmem_ld8(0x101F4829)) {
        gabi::Local<GrassString> named,stage;
        gmem_st32(named.a,0x1004E33C);gmem_st32(named.a+4,0x1004E1F4);
        u32 play=gabi::call<u32>(0x025200D4);
        gmem_st32(stage.a,play+0x5134);gmem_st32(stage.a+4,0x1004E1F4);
        grass_StringValidate(named.get());grass_StringValidate(named.get());
        u32 left=gmem_ld32(named.a);
        grass_StringValidate(stage.get());u32 right=gmem_ld32(stage.a);
        if(left==right) enabled=false;
        else {
            left=gmem_ld32(named.a);
            for(u32 count=0;count<0x40001;++count,++left,++right) {
                u32 a=gmem_ld8(left),b=gmem_ld8(right);
                if(a!=b) break;
                if(!a) {enabled=false;break;}
            }
        }
    }
    if(enabled) {
        wind=gabi::load<float>(0x1004E324);
        float power=gabi::call<float>(0x02578348);
        wind=gabi::fmadds(wind,power,wind);
        float maximum=gabi::load<float>(0x1004E328);
        if(wind-maximum>=0) wind=maximum;
    }
    for(u32 i=0;i<8;++i) {
        u32 time=gmem_ld32(0x101FF560)+i*250;
        u16 angle=(u16)gabi::ftoi(gabi::fmuls_ppc((float)time,wind));
        float cosine=gabi::load<float>(0x104A44F8+((u32)angle>>3)*8+4);
        gmem_st16(self+0x18F0C+i*0x38+4,(u16)gabi::ftoi(gabi::fmadds(wind,cosine,wind)));
    }
    s32 room=gabi::load<s8>(0x1047E6C8);
    u32 data=gmem_ld32(self+0x1A5CC+(u32)room*4);
    if(!data) return;
    gmem_st8(0x101E1D60,0);
    u32 play=gabi::call<u32>(0x025200D4),attribute=play+0x26A4,mass=play+0x4EF8;
    gabi::call<void>(0x020184DC,play+0x5008,gabi::load<float>(0x1004E288));
    gabi::call<void>(0x02018428,mass+0x110,gabi::load<float>(0x1004E338));
    gmem_st8(attribute+0x297C,11);gmem_st8(attribute+0x297D,0);
    do {
        if(!(gmem_ld8(data+1)&2) && gabi::load<s8>(data+2)>=0) gabi::call<void>(0x0254AA3C,data,room);
        data=gmem_ld32(data+0x40);
    } while(data);
}
VERIFY(0x0254C6C4,Grass_Calc);

void Grass_Update(u32 self) {
    WWHD_FUNC(0x0254CF78,void,self);
    const u32 matrix=0x1048D0CC;
    for(u32 i=0;i<104;++i) {
        u32 animation=self+0x18F0C+i*0x38;
        gabi::call<void>(0x025F1884,matrix,(s32)gabi::load<s16>(animation+2));
        gabi::call<void>(0x025F1BF4,matrix,(s32)gabi::load<s16>(animation+4));
        gabi::call<void>(0x025F1C28,matrix,(s32)(s16)-gabi::load<s16>(animation+2));
        gabi::call<void>(0x028E90D4,matrix,animation+8);
    }
    const u32 clipper=0x1048CFF0;
    float scale=gabi::load<float>(0x1048D04C);
    gabi::store<float>(clipper+0x54,gabi::fmuls_ppc(scale,gabi::load<float>(0x1004E348)));
    gabi::call<void>(0x0283801C,clipper);
    float radius=gabi::load<float>(0x1004E34C);
    u32 corrected=0;
    for(u32 i=0;i<1500;++i) {
        u32 data=self+0x9C+i*0x44;
        u32 state=gmem_ld8(data);
        if(!state) continue;
        if(state==1 && corrected<30) {
            float height=gabi::call<float>(0x02549F4C,data+4);
            gabi::store<float>(data+8,height);++corrected;gmem_st8(data,2);
        }
        gabi::Local<GrassVector> position;
        for(u32 j=0;j<3;++j) gmem_st32(position.a+j*4,gmem_ld32(data+4+j*4));
        u32 clipped=gabi::call<u32>(0x02838148,clipper,0x104B45F8,position.get(),radius);
        u32 flags=gmem_ld8(data+1);
        if(clipped) {gmem_st8(data+1,flags|2);continue;}
        s32 index=gabi::load<s8>(data+2);
        gmem_st8(data+1,flags&0xFD);
        float x=gabi::load<float>(data+4);
        if(index>=0) {
            u32 animation=self+0x18F0C+(u32)index*0x38;
            gabi::store<float>(animation+0x14,x);
            gmem_st32(animation+0x24,gmem_ld32(data+8));
            gmem_st32(animation+0x34,gmem_ld32(data+12));
            gabi::call<void>(0x028E90D4,animation+8,data+16);
        } else {
            gabi::call<void>(0x028E93CC,data+16,x,gabi::load<float>(data+8),gabi::load<float>(data+12));
            gabi::call<void>(0x025F1C28,data+16,(s32)(s16)(i*0xDCF));
        }
    }
    // The PPC load/store restores the original bits, including signaling NaN payloads.
    gmem_st32(clipper+0x54,gmem_ld32(0x1048D04C));
    gabi::call<void>(0x0283801C,clipper);
    gabi::call<void>(0x027F0E04,gmem_ld32(0x104B4634),self,0);
    gabi::call<void>(0x0254CA98,self);
}
VERIFY(0x0254CF78,Grass_Update);

static void grass_ReleaseAllocation(u32 owner,u32 offset) {
    u32 allocation=gmem_ld32(owner+offset);
    u32 heap=gabi::call<u32>(0x02755FEC,gmem_ld32(0x101F8B4C),allocation);
    u32 table=gmem_ld32(heap+12);
    gabi::call_ptr<void>(gmem_ld32(table+0x3C),heap,gmem_ld32(owner+offset));
}
static void grass_ReleaseResources(u32 base) {
    for(u32 i=0;i<2;++i) {
        u32 resource=base+i*0x4A8;
        gabi::call<void>(0x027BF7E8,resource+0x158);
        gmem_st32(resource,0);
        if(gmem_ld32(resource+0x250)) {
            grass_ReleaseAllocation(resource,0x250);
            gmem_st32(resource+0x24C,0);gmem_st32(resource+0x250,0);
        }
        gabi::call<void>(0x027BF7E8,resource+0x3AC);
        gmem_st32(resource+0x254,0);
        if(gmem_ld32(resource+0x4A4)) {
            grass_ReleaseAllocation(resource,0x4A4);
            gmem_st32(resource+0x4A0,0);gmem_st32(resource+0x4A4,0);
        }
    }
    gmem_st32(base+0x960,0);
}
void Grass_PacketDtor(u32 self,u32 flags) {
    WWHD_FUNC(0x0254BD34,void,self,flags);
    if(!self) return;
    u32 resources=self+0x1A6DC,collection=self+0x1A6D0;
    gmem_st32(self+12,0x1004E398);
    grass_ReleaseResources(resources);
    gabi::call<void>(0x027BE2B0,self+0x661E0,2);
    gabi::call<void>(0x027B54A0,self+0x661C8,2);
    gabi::call<void>(0x027B54A0,self+0x661B0,2);
    gabi::call<void>(0x028F0164,self+0x588B0,64,0x364,0x0254D894,0,0);
    gabi::call<void>(0x028F0164,self+0x1B050,1500,0xA8,0x0254D8E8,0,0);
    gabi::call<void>(0x027FD764,self+0x1B044,2);
    if(resources) {
        grass_ReleaseResources(resources);
        gabi::call<void>(0x028F0164,resources,4,0x254,0x0254D93C,0,0);
    }
    u32 array=gmem_ld32(collection+8);
    if(array) {
        s32 count=gabi::load<s32>(collection+4);
        for(s32 i=0;i<count;++i) {
            u32 item=array+(u32)i*20;
            if(item) {
                gmem_st32(item,0);
                for(u32 offset=8;offset<=16;offset+=8) {
                    u32 children=gmem_ld32(item+offset);
                    if(!children) continue;
                    s32 size=gabi::load<s32>(item+offset-4);
                    for(s32 j=0;j<size;++j) {
                        u32 child=children+(u32)j*0xF4;
                        u32 table=gmem_ld32(child+0xF0);
                        gabi::call_ptr<void>(gmem_ld32(table+12),child,2);
                        size=gabi::load<s32>(item+offset-4);
                        children=gmem_ld32(item+offset);
                    }
                    grass_ReleaseAllocation(item,offset);
                    gmem_st32(item+offset-4,0);gmem_st32(item+offset,0);
                }
                count=gabi::load<s32>(collection+4);array=gmem_ld32(collection+8);
            }
        }
        grass_ReleaseAllocation(collection,8);
        gmem_st32(collection+4,0);gmem_st32(collection+8,0);
    }
    gabi::call<void>(0x027F13DC,self,0);
    if(flags&1) gabi::call<void>(0x0273AF40,self);
}
VERIFY(0x0254BD34,Grass_PacketDtor);

// HD dKy_tevstr_c is 0x1C8 bytes: dKy_tevstr_init (0255FFF4) first clears 0x72 words of it.
struct GrassTev { u8 data[0x1C8]; };
// The original 0x2A8-byte frame (offsets from the frame base r1, entry SP - 0x2A8): PSMTXCopy matrix at +0x10,
// light state at +0x40, colours at +0x208/+0x218/+0x228, view matrix at +0x238. Same layout here, so
// recorded callee outputs land in the same objects. The struct starts at r1+8 (entry SP - 0x2A0).
struct GrassShaderFrame {
    u8 conversion[8];      // r1+0x08: int-to-float scratch (unused here)
    GrassMatrix matrix;    // r1+0x10
    GrassTev tev;          // r1+0x40
    GrassColor color;      // r1+0x208
    GrassColor scaled0;    // r1+0x218
    GrassColor scaled1;    // r1+0x228
    GrassMatrix view;      // r1+0x238
    u8 pad[0x2A0 - 0x260]; // to the frame top (saved registers)
};
static_assert(sizeof(GrassShaderFrame)==0x2A0,"Grass_ShaderBuild frame");
void Grass_ShaderBuild(u32 self) {
    WWHD_FUNC(0x0254CA98,void,self);
    gabi::Local<GrassShaderFrame> frame;
    struct Slot {u32 a; u32 get() const {return a;}};
    const u32 base=frame.a-8;   // the original r1
    Slot view{base+0x238},matrix{base+0x10},tev{base+0x40},color{base+0x208},scaled0{base+0x218},scaled1{base+0x228};
    s32 room=(s8)gmem_ld8(0x1047E6C8);
    gabi::call<void>(0x0254C934,view.get(),0x104B45F8);
    // Light template 0x1016E414 (0x44 bytes) into the state and its two light objects (+0xC0, +0x144), all fields.
    for(u32 i=0;i<=0x40;i+=4) gmem_st32(tev.a+i,gmem_ld32(0x1016E414+i));
    for(u32 i=0;i<=0x40;i+=4) {
        gmem_st32(tev.a+0xC0+i,gmem_ld32(tev.a+i));
        gmem_st32(tev.a+0x144+i,gmem_ld32(tev.a+i));
    }
    u32 environment=gabi::call<u32>(0x02555D0C);
    if(gmem_ld8(environment+0x10A3)!=255) {
        environment=gabi::call<u32>(0x02555D0C);room=(s8)gmem_ld8(environment+0x10A3);
    }
    gabi::call<void>(0x0255FFF4,tev.get(),room,255);
    environment=gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x025626A4,environment,1,0,tev.get());
    gabi::call<void>(0x0255FAF0,tev.get());
    u32 shared=self+0x1B044;
    u32 model=gmem_ld32(0x104B4708);
    gabi::call<void>(0x027FDA54,shared,0,view.get(),0x104B470C,model+0x240);
    u32 renderer=gmem_ld32(shared+4);
    gabi::call<void>(0x0254C9D4,color.get(),tev.a+0x90);
    gabi::call<void>(0x0274D458,scaled0.get(),color.get(),gabi::load<float>(tev.a+0x28));
    for(u32 i=0;i<4;++i) gmem_st32(renderer+0x1C4+i*4,gmem_ld32(scaled0.a+i*4));
    renderer=gmem_ld32(shared+4);
    gabi::call<void>(0x0254C9D4,color.get(),tev.a+0x160);
    gabi::call<void>(0x0274D458,scaled1.get(),color.get(),gabi::load<float>(tev.a+0x16C));
    for(u32 i=0;i<4;++i) gmem_st32(renderer+0x1D4+i*4,gmem_ld32(scaled1.a+i*4));
    gabi::call<void>(0x027FE010,shared);
    float denominator=gabi::load<float>(0x1004E344);
    u32 shaderOffset=0,packetOffset=0;
    for(u32 roomIndex=0;roomIndex<64;++roomIndex,shaderOffset+=0x364) {
        u32 data=gmem_ld32(self+0x1A5CC+roomIndex*4);
        if(!data) continue;
        u32 shader=self+0x588B0+shaderOffset;
        for(u32 j=0;j<4;++j) gabi::store<float>(shader+0xB4+j*4,(float)gabi::load<s16>(tev.a+0x90+j*2)/denominator);
        for(u32 j=0;j<4;++j) gabi::store<float>(shader+0xC4+j*4,(float)gmem_ld8(tev.a+0x98+j)/denominator);
        gabi::call<void>(0x0274D2AC,shader+0xC4,gabi::load<float>(tev.a+0x24));
        renderer=gmem_ld32(shared+4);
        for(u32 j=0;j<4;++j) gmem_st32(shader+0x94+j*4,gmem_ld32(renderer+0x1C4+j*4));
        gabi::call<void>(0x027FB678,shader);
        do {
            if(!(gmem_ld8(data+1)&2)) {
                for(u32 j=0;j<12;++j) gmem_st32(matrix.a+j*4,gmem_ld32(data+16+j*4));
                u32 packet=self+0x1B050+packetOffset;
                gabi::call<void>(0x028E90D4,matrix.get(),packet+0x74);
                gabi::call<void>(0x027FB678,packet);
                packetOffset+=0xA8;
            }
            data=gmem_ld32(data+0x40);
        } while(data);
    }
}
VERIFY(0x0254CA98,Grass_ShaderBuild);

struct GrassRenderState {u8 data[0x11C];};
static u32 grass_Collection(u32 self,u32 pass) {
    u32 collection=gmem_ld32(self+0x1A6D8);
    if(pass<gmem_ld32(self+0x1A6D4)) collection+=pass*20;
    return collection;
}
void Grass_Draw(u32 self,u32 state) {
    WWHD_FUNC(0x0254C120,void,self,state);
    u32 pass=gmem_ld32(state+12),material=0;
    if((s32)pass<4) material=gmem_ld32(grass_Collection(self,pass));
    u32 cache=gabi::call<u32>(0x027F29D4,0x104B45C0);
    u32 model=gmem_ld32(material);
    if(model!=gmem_ld32(cache+4)) {
        u32 flag=gmem_ld8(model),previous=gmem_ld32(cache);
        if(flag&2) {gmem_st8(model,flag&~2u);gabi::call<void>(0x027BB9E0,model,0);}
        u32 shader=gmem_ld32(gmem_ld32(model+0x7C)+0x28);
        if(previous!=shader) gabi::call<void>(0x027B9F68,shader);
        u32 size=gmem_ld32(model+12);
        if(size) gabi::call<void>(0xC00060E0,gmem_ld32(model+4),size);
        else gabi::call<void>(0x027BB7CC,model);
        gmem_st32(cache,shader);gmem_st32(cache+4,model);
    }
    pass=gmem_ld32(state+12);
    bool bind=pass==0?gmem_ld32(state+20)!=0:(pass==1 || pass==2);
    if(bind) {
        u32 data=pass==0?gmem_ld32(gmem_ld32(state+20)+4):gmem_ld32(self+0x1B048);
        u32 block=data+16+gmem_ld32(data+0x4C)*28;
        u32 info=gmem_ld32(material+12)?gmem_ld32(material+16):0;
        s32 vertex=gabi::load<s16>(info+12),pixel=gabi::load<s16>(info+14),geometry=gabi::load<s16>(info+16);
        u32 buffer=gmem_ld32(block+12),size=gmem_ld32(block+4);
        if(vertex!=-1 || pixel!=-1 || geometry!=-1) {
            if(pixel!=-1) gabi::call<void>(0xC0006900,pixel,buffer,size);
            if(vertex!=-1) gabi::call<void>(0xC0006A38,vertex,buffer,size);
            if(geometry!=-1) gabi::call<void>(0xC00068A8,geometry,buffer,size);
        }
    }
    if(pass==2) {
        u32 object=gmem_ld32(state+0x30);
        if(object) gabi::call_ptr<void>(gmem_ld32(gmem_ld32(object+12)+0x2C),object,material);
        gabi::call<void>(0x027FFE54,state,material);
    }
    u32 attribute=gmem_ld32(material+20)?gmem_ld32(material+24):0;
    gabi::call<void>(0x027BE53C,self+0x661E0,attribute+4,0,0);
    gabi::Local<GrassRenderState> render;
    gabi::call<void>(0x02750250,render.get());
    u32 mode=gmem_ld32(render.a+0xEC);
    gabi::store<float>(render.a+0xE8,gabi::load<float>(0x1004E320));
    gmem_st32(render.a+12,2);gmem_st8(render.a+0xE0,1);gmem_st32(render.a+0xE4,4);
    gmem_st32(render.a+0xEC,(((mode&0xFFFFFFF0)+7)&0xFFFFFF0F)+16);
    gabi::call<void>(0x0280037C,gmem_ld32(state+12),render.get());
    gabi::call<void>(0x02750370,render.get());
    u32 packetOffset=0;
    for(u32 room=0;room<64;++room) {
        u32 data=gmem_ld32(self+0x1A5CC+room*4);
        if(data && gmem_ld32(state+12)) {
            u32 packet=self+0x588B0+room*0x364;
            gabi::call_ptr<void>(gmem_ld32(gmem_ld32(packet+12)+0x2C),packet,material);
        }
        s32 kind=-1;
        while(data) {
            if(!(gmem_ld8(data+1)&2)) {
                u32 packet=self+0x1B050+packetOffset;
                gabi::call_ptr<void>(gmem_ld32(gmem_ld32(packet+12)+0x2C),packet,material);
                packetOffset+=0xA8;
                bool animated=gabi::load<s8>(data+2)>=0;
                s32 nextKind=animated?0:1;
                if(kind!=nextKind) {
                    u32 collection=grass_Collection(self,gmem_ld32(state+12));
                    if(!gmem_ld32(self+0x1B02C)) collection+=8;
                    u32 shape=gmem_ld32(collection+8);
                    if(!animated && gmem_ld32(collection+4)>1) shape+=0xF4;
                    gabi::call<void>(0x027BFE5C,shape);kind=nextKind;
                }
                u32 indices=self+0x661B0+(animated?0:24);
                u32 count=gmem_ld32(indices+12);
                if(count) gabi::call<void>(0xC0006178,gmem_ld32(indices+4),count,gmem_ld32(indices),gmem_ld32(indices+8),0,1);
            }
            data=gmem_ld32(data+0x40);
        }
    }
    gabi::call<void>(0x02750370,0x104B474C);
}
VERIFY(0x0254C120,Grass_Draw);
static u32 grass_HeapAlloc(u32 size,u32 alignment) {
    u32 heap=gabi::call<u32>(0x02756140,gmem_ld32(0x101F8B4C));
    return gabi::call_ptr<u32>(gmem_ld32(gmem_ld32(heap+12)+0x34),heap,size,alignment);
}
static void grass_ClearShapes(u32 item) {
    gmem_st32(item,0);
    for(u32 offset=8;offset<=16;offset+=8) {
        u32 array=gmem_ld32(item+offset);
        if(!array) continue;
        for(s32 i=0;i<gabi::load<s32>(item+offset-4);++i) {
            u32 child=array+(u32)i*0xF4;
            gabi::call_ptr<void>(gmem_ld32(gmem_ld32(child+0xF0)+12),child,2);
            array=gmem_ld32(item+offset);
        }
        grass_ReleaseAllocation(item,offset);
        gmem_st32(item+offset-4,0);gmem_st32(item+offset,0);
    }
}
static void grass_ClearVertexBuffer(u32 buffer) {
    u32 end=buffer+0xC00;
    for(u32 address=buffer;address<end;address+=32) {
        for(u32 i=0;i<32;i+=4) gmem_st32((address&~31u)+i,0);
    }
}
static void grass_PopulateVertices(u32 buffer,u32 countAddress,u32 positions,u32 normals,u32 colors,u32 texcoords) {
    for(u32 i=0;i<gmem_ld32(countAddress);++i) {
        for(u32 j=0;j<3;++j) gmem_st32(buffer+i*48+j*4,gmem_ld32(positions+(i+1)*12+j*4));
        for(u32 j=0;j<3;++j) gmem_st32(buffer+i*48+12+j*4,gmem_ld32(normals+(i+1)*12+j*4));
        for(u32 j=0;j<4;++j) gmem_st32(buffer+i*48+32+j*4,gmem_ld32(colors+(i+1)*16+j*4));
        for(u32 j=0;j<2;++j) gmem_st32(buffer+i*48+24+j*4,gmem_ld32(texcoords+(i+1)*8+j*4));
    }
}
u32 Grass_PacketCtor(u32 self) {
    WWHD_FUNC(0x0254ACD8,u32,self);
    if(!self) {self=gabi::call<u32>(0x0273AD10,0x66408);if(!self)return 0;}
    gabi::call<void>(0x027F1278,self);
    gmem_st32(self+12,0x1004E398);
    gabi::call<void>(0x028EFFD0,self+0x9C,1500,0x44,0x0254A0DC);
    gabi::call<void>(0x028EFFD0,self+0x18F0C,104,0x38,0x0254D49C);
    gabi::call<void>(0x028EFFD0,self+0x1A5CC,64,4,0x0254D4D8);
    u32 collection=self+0x1A6D0,resources=self+0x1A6DC;
    gmem_st32(collection,0);
    u32 pair=collection+4;
    if(!pair) pair=gabi::call<u32>(0x0273AD10,8);
    if(pair) {gmem_st32(pair+4,0);gmem_st32(pair,0);}
    u32 resourceStorage=resources;
    if(!resourceStorage) resourceStorage=gabi::call<u32>(0x0273AD10,0x968);
    if(resourceStorage) {
        gabi::call<void>(0x028EFFD0,resourceStorage,4,0x254,0x0254D514);
        gmem_st32(resourceStorage+0x950,0);gmem_st32(resourceStorage+0x960,0);
        gmem_st32(resourceStorage+0x958,48);gmem_st8(resourceStorage+0x964,0);gmem_st32(resourceStorage+0x954,0);
        for(u32 i=0;i<2;++i) for(u32 j=0;j<2;++j) gmem_st32(resourceStorage+i*0x254+j*0x4A8,0);
    }
    gabi::call<void>(0x027FD6F4,self+0x1B044);
    gabi::call<void>(0x028EFFD0,self+0x1B050,1500,0xA8,0x0254D570);
    gabi::call<void>(0x028EFFD0,self+0x588B0,64,0x364,0x0254D60C);
    gabi::call<void>(0x027B5430,self+0x661B0);
    gabi::call<void>(0x027B5430,self+0x661C8);
    gabi::call<void>(0x027BDF7C,self+0x661E0);
    gabi::call<void>(0x027BE6B8,self+0x66378);
    for(u32 i=0;i<1500;++i) gmem_st8(self+0x9C+i*0x44,0);
    gmem_st16(self+0x98,0);
    for(u32 i=0;i<104;++i) gmem_st8(self+0x18F0C+i*0x38,0);
    for(u32 i=0;i<8;++i) gabi::call<void>(0x0254ACB4,self,i,(s32)(s16)(i*0x2000));
    gabi::Local<GrassString> name,archiveName,resourceName,stage,prefix,boss,other;
    gmem_st32(name.a,0x1004E2EC);gmem_st32(name.a+4,0x1004E1F4);
    u32 archive=gabi::call<u32>(0x027FFCBC);
    s32 index=gabi::call<s32>(0x027B90AC,gmem_ld32(archive+4),name.get());
    u32 entry=0;
    if(index>=0) {
        u32 count=gmem_ld32(archive+8),entries=gmem_ld32(archive+12);
        u32 record=entries+((u32)index<count?(u32)index*36:0);
        if(!gmem_ld8(record+32)) {
            u32 directory=gmem_ld32(archive+4),source=0;
            if((u32)index<gmem_ld32(directory+28)) source=gmem_ld32(directory+32)+(u32)index*0x84;
            gabi::call<void>(0x02800B0C,record,source,0);
            count=gmem_ld32(archive+8);entries=gmem_ld32(archive+12);
        }
        entry=entries+((u32)index<count?(u32)index*36:0);
    }
    gabi::call<void>(0x0280068C,collection,entry,0);
    gmem_st32(resources+0x954,0x1013);gmem_st32(resources+0x95C,0x1004E388);
    for(u32 outer=0;outer<2;++outer) {
        for(u32 inner=0;inner<2;++inner) {
            u32 resource=resources+outer*0x254+inner*0x4A8;
            u32 buffer=gmem_ld32(resource);
            if(!buffer) {
                u32 allocated=grass_HeapAlloc(0xC00,64);
                if(allocated) {gmem_st32(resource+0x250,allocated);gmem_st32(resource+0x24C,64);}
                buffer=gmem_ld32(resource+0x250);gmem_st32(resource,buffer);
            }
            gabi::call<void>(0x027FF478,resource+4,buffer,64,resources+0x954);
        }
    }
    gmem_st32(resources+0x960,0);gmem_st8(resources+0x964,1);
    for(u32 i=0;i<gmem_ld32(collection);++i) {
        u32 item=gmem_ld32(collection+8);
        if(i<gmem_ld32(collection+4)) item+=i*20;
        u32 material=gmem_ld32(item);
        grass_ClearShapes(item);gmem_st32(item,material);
        for(u32 offset=8;offset<=16;offset+=8) {
            u32 shapes=grass_HeapAlloc(0x1E8,4);
            for(u32 j=0;j<2;++j) {u32 shape=shapes+j*0xF4;if(shape)gabi::call<void>(0x027BF734,shape);}
            if(shapes) {gmem_st32(item+offset,shapes);gmem_st32(item+offset-4,2);}
        }
        for(u32 outer=0;outer<2;++outer) for(u32 inner=0;inner<2;++inner) {
            u32 owner=item+4+outer*8;
            u32 shape=gmem_ld32(owner+4);
            if(inner<gmem_ld32(owner)) shape+=inner*0xF4;
            gabi::call<void>(0x027FF530,material,shape,resources+4+outer*0x254+inner*0x4A8,resources+0x954,0);
        }
    }
    gabi::call<void>(0x027FD838,self+0x1B044,1,0);
    for(u32 i=0;i<64;++i) gabi::call<void>(0x027FB5D4,self+0x588B0+i*0x364,0);
    for(u32 i=0;i<1500;++i) gabi::call<void>(0x027FB5D4,self+0x1B050+i*0xA8,0);
    gmem_st32(archiveName.a,0x1004E2F8);gmem_st32(archiveName.a+4,0x1004E1F4);
    gmem_st32(resourceName.a,0x1004E308);gmem_st32(resourceName.a+4,0x1004E1F4);
    u32 texture=gabi::call<u32>(0x026124B0,gmem_ld32(0x101F4F7C),archiveName.get(),resourceName.get(),0);
    u32 play=gabi::call<u32>(0x025200D4);
    gmem_st32(stage.a,play+0x5134);gmem_st32(stage.a+4,0x1004E1F4);
    gmem_st32(prefix.a,0x1004E2C4);gmem_st32(prefix.a+4,0x1004E1F4);
    gabi::call<void>(0x0254D9A0,stage.get());
    grass_StringValidate(stage.get());u32 left=gmem_ld32(stage.a);
    grass_StringValidate(prefix.get());u32 right=gmem_ld32(prefix.a);
    bool alternate=left==right;
    if(!alternate) {
        alternate=true;left=gmem_ld32(stage.a);
        for(u32 i=0;i<3;++i,++left,++right) {
            u32 a=gmem_ld8(left),b=gmem_ld8(right);
            if(!a || !b) {alternate=a==b;break;}
            if(a!=b) {alternate=false;break;}
        }
    }
    if(!alternate) {
        gmem_st32(boss.a,0x1004E2C8);gmem_st32(boss.a+4,0x1004E1F4);
        play=gabi::call<u32>(0x025200D4);
        gmem_st32(other.a,play+0x5134);gmem_st32(other.a+4,0x1004E1F4);
        grass_StringValidate(boss.get());grass_StringValidate(boss.get());left=gmem_ld32(boss.a);
        grass_StringValidate(other.get());right=gmem_ld32(other.a);
        alternate=left==right;
        if(!alternate) {
            left=gmem_ld32(boss.a);alternate=true;
            for(u32 i=0;i<0x40001;++i,++left,++right) {
                u32 a=gmem_ld8(left),b=gmem_ld8(right);
                if(a!=b) {alternate=false;break;}
                if(!a) break;
                if(i==0x40000) alternate=false;
            }
        }
    }
    u32 indices=self+0x661B0;
    gabi::call<void>(0x027B54E0,indices,alternate?0x101E31F8:0x101E2214,4,gmem_ld32(alternate?0x101E1D58:0x101E1D48));
    gmem_st32(indices+4,4);
    for(u32 kind=0;kind<2;++kind) {
        u32 selected=resources+gmem_ld32(resources+0x950)*0x254;
        u32 buffer=gmem_ld32(selected+kind*0x4A8);
        grass_ClearVertexBuffer(buffer);
        selected=resources+gmem_ld32(resources+0x950)*0x254;
        buffer=gmem_ld32(selected+kind*0x4A8);
        if(alternate) {
            if(!kind) grass_PopulateVertices(buffer,0x101E1D54,0x101E264C,0x101E2934,0x101E2C18,0x101E3000);
            else grass_PopulateVertices(buffer,0x101E1D5C,0x101E32C4,0x101E34A4,0x101E3680,0x101E3908);
        } else {
            if(!kind) grass_PopulateVertices(buffer,0x101E1D44,0x101E1D58,0x101E1E84,0x101E1FAC,0x101E2144);
            else grass_PopulateVertices(buffer,0x101E1D4C,0x101E22E0,0x101E23A0,0x101E245C,0x101E2564);
        }
    }
    u32 original=gmem_ld32(resources+0x950),destination=resources+(!original)*0x254;
    for(u32 kind=0;kind<2;++kind,destination+=0x4A8) {
        u32 selected=resources+(original+kind*2)*0x254;
        for(u32 i=0;i<64;++i) {
            u32 source=gmem_ld32(selected),target=gmem_ld32(destination);
            for(u32 j=0;j<12;++j) gmem_st32(target+i*48+j*4,gmem_ld32(source+i*48+j*4));
        }
        original=gmem_ld32(resources+0x950);
    }
    u32 selected=resources+gmem_ld32(resources+0x950)*0x254+4;
    for(u32 i=0;i<2;++i) gabi::call<void>(0x027B5E94,selected+i*0x4A8,0,gmem_ld32(selected+i*0x4A8+0x14C));
    gmem_st32(resources+0x950,!gmem_ld32(resources+0x950));
    gabi::call<void>(0x0274FBF8,gmem_ld32(0x101F8B18));
    u32 textureInfo=self+0x66378;
    gabi::call<void>(0x02773870,textureInfo,texture,alternate?0x1004E2D0:0x1004E2DC);
    gabi::call<void>(0x0274FCCC,gmem_ld32(0x101F8B18));
    gmem_st16(self+0x1A6CC,alternate?0x8222:0x3DB);gmem_st16(self+0x1A6CE,alternate?0x8221:0x3DA);
    gabi::call<void>(0x027B54E0,self+0x661C8,0x101E25EC,4,gmem_ld32(0x101E1D50));
    gmem_st32(self+0x661CC,4);
    u32 target=self+0x661E0;
    bool equal=true;
    const u32 fields[]={4,8,12,16,20,24,56,52,28};
    for(u32 offset:fields) if(gmem_ld32(target+offset)!=gmem_ld32(textureInfo+offset)) {equal=false;break;}
    if(!equal) gabi::call<void>(0x027BDEB4,target,textureInfo);
    else {
        u32 first=gmem_ld32(textureInfo+40),second=gmem_ld32(textureInfo+48);
        gmem_st32(target+40,first);gmem_st8(target+0x190,gmem_ld8(target+0x190)|2);
        gmem_st32(target+0xDC,second);gmem_st32(target+48,second);
        gmem_st32(target+0xD4,first);
    }
    gmem_st32(target+0x160,1);gmem_st32(target+0x15C,1);gmem_st32(target+0x164,1);
    if(!equal)gmem_st8(target+0x190,gmem_ld8(target+0x190)|2);
    return self;
}
VERIFY(0x0254ACD8,Grass_PacketCtor);
