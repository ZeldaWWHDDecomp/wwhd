#include "gabi.h"
using namespace gabi;
// Linked native d_mesg initializer and compiler-emitted companions, not a GC TU identity claim.
void mesgInitializeGlobals() {WWHD_FUNC(0x025F8EC4,void);store<u32>(0x1048D730,0);store<u32>(0x1048D728,0);store<u32>(0x1048D734,0);store<u32>(0x1048D72C,0);call<void>(0x028F026C,0x101F4B38u);const f32 lo=load<f32>(0x100E0CA8),hi=load<f32>(0x100E0CAC);store<f32>(0x1048D71C,lo);store<f32>(0x1048D720,hi);call<void>(0x028ED6F8,0x1048D724u);call<void>(0x028F026C,0x101F4B44u);call<void>(0x028EAB2C,0x1048D725u);call<void>(0x028F026C,0x101F4B50u);}
VERIFY(0x025F8EC4,mesgInitializeGlobals);
void mesgDelete025F8F58(u32 p,u32 flags) {WWHD_FUNC(0x025F8F58,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F8F58,mesgDelete025F8F58);
void mesgDelete025F8F6C(u32 p,u32 flags) {WWHD_FUNC(0x025F8F6C,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F8F6C,mesgDelete025F8F6C);
void mesgDelete025F8F9C(u32 p,u32 flags) {WWHD_FUNC(0x025F8F9C,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F8F9C,mesgDelete025F8F9C);
void mesgDelete025F8FD0(u32 p,u32 flags) {WWHD_FUNC(0x025F8FD0,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F8FD0,mesgDelete025F8FD0);
void mesgDelete025F8FE4(u32 p,u32 flags) {WWHD_FUNC(0x025F8FE4,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F8FE4,mesgDelete025F8FE4);
void mesgDelete025F8FF8(u32 p,u32 flags) {WWHD_FUNC(0x025F8FF8,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F8FF8,mesgDelete025F8FF8);
void mesgDelete025F900C(u32 p,u32 flags) {WWHD_FUNC(0x025F900C,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F900C,mesgDelete025F900C);
void mesgDelete025F9020(u32 p,u32 flags) {WWHD_FUNC(0x025F9020,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F9020,mesgDelete025F9020);
void mesgDelete025F9034(u32 p,u32 flags) {WWHD_FUNC(0x025F9034,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F9034,mesgDelete025F9034);
void mesgDelete025F9048(u32 p,u32 flags) {WWHD_FUNC(0x025F9048,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F9048,mesgDelete025F9048);
void mesgDelete025F905C(u32 p,u32 flags) {WWHD_FUNC(0x025F905C,void,p,flags);if(p&&(flags&1))call<void>(0x0273AF40,p);}
VERIFY(0x025F905C,mesgDelete025F905C);
void mesgNoop025F8F80() {WWHD_FUNC(0x025F8F80,void);}
VERIFY(0x025F8F80,mesgNoop025F8F80);
void mesgNoop025F8FB0() {WWHD_FUNC(0x025F8FB0,void);}
VERIFY(0x025F8FB0,mesgNoop025F8FB0);
void mesgTerminateBytes(u32 p) {WWHD_FUNC(0x025F8F84,void,p);const u32 data=load<u32>(p),size=load<u32>(p+8);store<u8>(data+size-1,0);}
VERIFY(0x025F8F84,mesgTerminateBytes);
void mesgTerminateWords(u32 p) {WWHD_FUNC(0x025F8FB4,void,p);const u32 size=load<u32>(p+8),data=load<u32>(p);store<u16>(data+2*size-2,0);}
VERIFY(0x025F8FB4,mesgTerminateWords);
