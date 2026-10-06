/* Bomb static utilities reconstructed from native HD and GC TU.
 * SOURCE-ONLY DRAFT: never built or verified. Initializer ownership structurally accepted by parent. */
#include "d/actor/d_a_bomb_static.h"
namespace BombStatic {
template<class T> static T rd(const void* self,u32 offset) { return gabi::load<T>(gabi::ea(self)+offset); }
template<class T> static void wr(void* self,u32 offset,T value) { gabi::store<T>(gabi::ea(self)+offset,value); }
static u32 shiftLeft(u32 value,u32 shift) { shift &= 63; return shift<32 ? value<<shift : 0; }
static u32 shiftRight(u32 value,u32 shift) { shift &= 63; return shift<32 ? value>>shift : 0; }

// GC 800683D0; native 020CB5FC.
u32 getVersion(const void* self) {
    WWHD_FUNC(0x020CB5FC, u32, self);
    return abstractParam(self, 1, 31);
}
VERIFY(0x020CB5FC, getVersion);

// GC 800683F8; native 020CB608.
void checkVersion(const void* self) {
    WWHD_FUNC(0x020CB608, void, self);
    if (getVersion(self) != 1) {
        gabi::call(0x0273AA24, gabi::at<const char>(0x1000AD00),
                   u32(0xCE), gabi::at<const char>(0x1000ACE8));
    }
}
VERIFY(0x020CB608, checkVersion);

// GC 80067FA0; native 020CB648.
s16 getRestTime(const void* self) {
    WWHD_FUNC(0x020CB648, s16, self);
    checkVersion(self);
    return rd<s16>(self, 0xA7C);
}
VERIFY(0x020CB648, getRestTime);

// GC 80067FD0; native 020CB678.
u8 getCheckFlag(const void* self) {
    WWHD_FUNC(0x020CB678, u8, self);
    checkVersion(self);
    return rd<u8>(self, 0xA71);
}
VERIFY(0x020CB678, getCheckFlag);

// GC 80068000; native 020CB6A8.
void setCheckFlag(void* self) {
    WWHD_FUNC(0x020CB6A8, void, self);
    checkVersion(self);
    wr<u8>(self, 0xA71, 1);
}
VERIFY(0x020CB6A8, setCheckFlag);

// GC 80068034; native 020CB6DC.
void setFire(void* self) {
    WWHD_FUNC(0x020CB6DC, void, self);
    checkVersion(self);
    wr<u8>(self, 0xA72, 1);
}
VERIFY(0x020CB6DC, setFire);

// GC 80068068; native 020CB710.
void setNoHit(void* self) {
    WWHD_FUNC(0x020CB710, void, self);
    checkVersion(self);
    u32 tg = rd<u32>(self, 0x920);
    u32 co = rd<u32>(self, 0x938);
    u32 at = rd<u32>(self, 0x94C);
    wr<u32>(self,0x920,tg&~1u);
    wr<u32>(self,0x938,co&~1u);
    wr<u8>(self,0xA73,1);
    wr<u32>(self,0x94C,at&~1u);
}
VERIFY(0x020CB710, setNoHit);

// GC 800680CC; native 020CB768.
void disableCo(void* self) {
    WWHD_FUNC(0x020CB768, void, self);
    checkVersion(self);
    wr<u32>(self,0x94C,rd<u32>(self,0x94C)&~1u);
}
VERIFY(0x020CB768, disableCo);

// GC 80068104; native 020CB7A0.
void enableCo(void* self) {
    WWHD_FUNC(0x020CB7A0, void, self);
    checkVersion(self);
    wr<u32>(self,0x94C,rd<u32>(self,0x94C)|1u);
}
VERIFY(0x020CB7A0, enableCo);

// GC 8006813C; native 020CB7D8.
void removeEffects(void* self) {
    WWHD_FUNC(0x020CB7D8, void, self);
    checkVersion(self);
    u32 first = rd<u32>(self, 0xA60);
    if (first) {
        // Reload after the first store: readable local alias cases can change this slot.
        gabi::store<u32>(first + 0x1E4, 0);
        first = rd<u32>(self, 0xA60);
        u32 flags = gabi::load<u32>(first + 0x254);
        gabi::store<u32>(first + 0x5C, ~0u);
        gabi::store<u32>(first + 0x254, flags | 1u);
    }
    // Native order captures the second pointer before clearing the first slot.
    u32 second = rd<u32>(self, 0xA6C);
    wr<u32>(self, 0xA60, 0);
    if (second) {
        gabi::store<u32>(second + 0x1E4, 0);
        second = rd<u32>(self, 0xA6C);
        u32 flags = gabi::load<u32>(second + 0x254);
        gabi::store<u32>(second + 0x5C, ~0u);
        gabi::store<u32>(second + 0x254, flags | 1u);
    }
    wr<u32>(self, 0xA6C, 0);
}
VERIFY(0x020CB7D8, removeEffects);

// GC 800681CC; native 020CB860.
void setRestTime(void* self, s16 time) {
    WWHD_FUNC(0x020CB860, void, self, time);
    checkVersion(self);
    wr<s16>(self,0xA7C,time);
}
VERIFY(0x020CB860, setRestTime);

// GC 80068208; native 020CB89C.
void setNoGravityTime(void* self, s16 time) {
    WWHD_FUNC(0x020CB89C, void, self, time);
    checkVersion(self);
    wr<s16>(self,0xA80,time);
}
VERIFY(0x020CB89C, setNoGravityTime);

// GC 80068244; native 020CB8D8.
u32 makeParam(u32 state, u32 cheap, u32 zeroAngle) {
    WWHD_FUNC(0x020CB8D8, u32, state, cheap, zeroAngle);
    return state | ((cheap & 0xFFFFu)<<16) | ((zeroAngle & 0x7FFFu)<<17) | 0x80000000u;
}
VERIFY(0x020CB8D8, makeParam);

// GC 800682F0; native 020CB8F0.
u32 getState(const void* self) {
    WWHD_FUNC(0x020CB8F0, u32, self);
    checkVersion(self);
    return abstractParam(self,8,0);
}
VERIFY(0x020CB8F0, getState);

// GC 80068274; native 020CB92C.
bool checkState(const void* self, u32 state) {
    WWHD_FUNC(0x020CB92C, bool, self, state);
    checkVersion(self);
    return getState(self)==state;
}
VERIFY(0x020CB92C, checkState);

// GC 800682C0; native 020CB978.
void changeState(void* self, u32 state) {
    WWHD_FUNC(0x020CB978, void, self, state);
    // Native020CB984 stores before the tail call at020CB988.
    wr<u32>(self,0xB0,(rd<u32>(self,0xB0)&0xFFFFFF00u)|state);
    checkVersion(self);
}
VERIFY(0x020CB978, changeState);

// GC 8006832C; native 020CB98C.
bool getInstantExplosion(const void* self) {
    WWHD_FUNC(0x020CB98C, bool, self);
    return rd<u32>(self,0xB08)==1;
}
VERIFY(0x020CB98C, getInstantExplosion);

// GC 80068340; native 020CB9A0.
bool getCheapEffect(const void* self) {
    WWHD_FUNC(0x020CB9A0, bool, self);
    checkVersion(self);
    return abstractParam(self,1,16)!=0;
}
VERIFY(0x020CB9A0, getCheapEffect);

// GC 80068388; native 020CB9E4.
bool getZeroAngle(const void* self) {
    WWHD_FUNC(0x020CB9E4, bool, self);
    checkVersion(self);
    return abstractParam(self,1,17)!=0;
}
VERIFY(0x020CB9E4, getZeroAngle);

// GC 80068450; native 020CBA28.
void flowerRemoveEffects(void* self) {
    WWHD_FUNC(0x020CBA28, void, self);
    u32 base=gabi::ea(self);
    gabi::call(0x020C852C,gabi::at<void>(base+0x894));
    gabi::call(0x020C8918,gabi::at<void>(base+0x8AC));
}
VERIFY(0x020CBA28, flowerRemoveEffects);

// GC 80068488; native 020CBA60.
void flowerSetTime(void* self, s32 time) {
    WWHD_FUNC(0x020CBA60, void, self, time);
    wr<s32>(self,0x934,time);
}
VERIFY(0x020CBA60, flowerSetTime);

// GC 80068490; native 020CBA68.
s32 flowerGetTime(const void* self) {
    WWHD_FUNC(0x020CBA68, s32, self);
    return rd<s32>(self,0x934);
}
VERIFY(0x020CBA68, flowerGetTime);

// GC 80068498; native 020CBA70.
u8 flowerCheckEat(const void* self) {
    WWHD_FUNC(0x020CBA70, u8, self);
    return rd<u8>(self,0x941);
}
VERIFY(0x020CBA70, flowerCheckEat);

// GC 800684A0; native 020CBA78.
void flowerSetEat(void* self) {
    WWHD_FUNC(0x020CBA78, void, self);
    wr<u8>(self,0x941,1);
}
VERIFY(0x020CBA78, flowerSetEat);

// GC 800684AC; native 020CBA84.
void flowerSetNoHit(void* self) {
    WWHD_FUNC(0x020CBA84, void, self);
    u32 tg = rd<u32>(self, 0x778);
    u32 co = rd<u32>(self, 0x78C);
    u32 at = rd<u32>(self, 0x760);
    wr<u32>(self,0x778,tg&~1u);
    wr<u32>(self,0x78C,co&~1u);
    wr<u32>(self,0x760,at&~1u);
}
VERIFY(0x020CBA84, flowerSetNoHit);

// GC 800684E0; native 020CBAAC.
bool flowerCheckExplosion(const void* self) {
    WWHD_FUNC(0x020CBAAC, bool, self);
    return rd<u32>(self,0x88C)==2;
}
VERIFY(0x020CBAAC, flowerCheckExplosion);

// GC 800684F4; native 020CBB54.
u32 abstractParam(const void* self, u32 width, u32 shift) {
    WWHD_FUNC(0x020CBB54, u32, self, width, shift);
    u32 param=rd<u32>(self,0xB0);
    u32 left = shiftLeft(1, width);
    u32 right = shiftRight(param, shift);
    return right & (left-1u);
}
VERIFY(0x020CBB54, abstractParam);
// HD per-TU header initializer; structural ownership accepted, literal symbol unavailable.
// This helper models disjoint local static storage; external SDK callees remain mocked.
void initializeHeaderStatics() {
    WWHD_FUNC(0x020CBAC0, void);
    sinit_header_statics(0x104626EC, 0x101923D8);
}
VERIFY(0x020CBAC0, initializeHeaderStatics);

}
