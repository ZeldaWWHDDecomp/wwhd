/* Retained HD J3DDrawBuffer group (027F093C..027F1274).
 * HD storage: count+0, buckets+4, draw mode+8, near/far/ratio+10/14/18,
 * matrix+1C, callback+20; packets retain next+10 and bucket backlink+94.
 * Reload bucket metadata between stores: backlink or packet memory can alias it.
 * The native remove branch at 027F1034 loops when head.next == packet;
 * intentionally preserved. Verification excludes that nonterminating case.
 * Heap callbacks below receive heap/size/alignment or heap/storage; packet draw
 * receives packet and drawing context. PTMF dispatch forwards both arguments.
 */
#include "wwhd.h"
u32 buffer_027F093C(u32 a3) {
WWHD_FUNC(0x027F093C,u32,a3);
gabi::Local<u8[24]> frame;
u32 v0=0,v3=a3,v4=0,v5=0,v8=0,v9=0,v10=0,v11=0,v28=0,v29=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;
v28 = v3;
v0 = gabi::load<u32>(v28 + 0x00000000u);
v31 = gabi::load<u32>(v28 + 0x00000004u);
v11 = rotl32(v0, 2) & 0xFFFFFFFCu;
v10 = v31 + v11;
less=(v31)<(v10);greater=(v31)>(v10);equal=(v31)==(v10);
if (equal) { goto L_027F09E4; }
v29 = 0u + 0x00000000u;
L_027F0978: ;
v30 = gabi::load<u32>(v31 + 0x00000000u);
less=((int32_t)v30)<(0);greater=((int32_t)v30)>(0);equal=((int32_t)v30)==(0);
if (equal) { goto L_027F09D8; }
v29 = 0u + 0x00000000u;
L_027F0988: ;
v0 = gabi::load<u32>(v30 + 0x00000094u);
less=(v0)<(v31);greater=(v0)>(v31);equal=(v0)==(v31);
if (equal) { goto L_027F09AC; }
v3 = 0u + 0x10170000u;
v5 = 0u + 0x10170000u;
v4 = 0u + 0x0000008Fu;
v3 = v3 + 0xFFFFE100u;
v5 = v5 + 0xFFFFE114u;
v3=gabi::call<u32>(0x0273AA24,v3,v4,v5);
L_027F09AC: ;
gabi::store<u32>(v30 + 0x00000094u, v29);
v3 = v30;
v30 = gabi::load<u32>(v30 + 0x00000010u);
v3=gabi::call<u32>(0x027F1508,v3);
less=((int32_t)v30)<(0);greater=((int32_t)v30)>(0);equal=((int32_t)v30)==(0);
if (!equal) { goto L_027F0988; }
gabi::store<u32>(v31 + 0x00000000u, v29);
v0 = gabi::load<u32>(v28 + 0x00000000u);
v9 = gabi::load<u32>(v28 + 0x00000004u);
v11 = rotl32(v0, 2) & 0xFFFFFFFCu;
v10 = v9 + v11;
L_027F09D8: ;
v31 = v31 + 0x00000004u;
less=(v31)<(v10);greater=(v31)>(v10);equal=(v31)==(v10);
if (!equal) { goto L_027F0978; }
L_027F09E4: ;
v8 = 0u + 0x00000000u;
gabi::store<u32>(v28 + 0x00000020u, v8);
return v3;
return v3;
}
VERIFY(0x027F093C,buffer_027F093C);
u32 buffer_027F0A0C(u32 a3,u32 a4) {
WWHD_FUNC(0x027F0A0C,u32,a3,a4);
gabi::Local<u8[16]> frame;
u32 v0=0,v3=a3,v4=a4,v9=0,v10=0,v11=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;u32 count=0;
v31 = v4;
v30 = v3; less=(s32)v30<0;greater=(s32)v30>0;equal=v30==0;
if (equal) { goto L_027F0A84; }
v3 = v30;
v3=gabi::call<u32>(0x027F093C,v3);
v4 = gabi::load<u32>(v30 + 0x00000004u);
less=((int32_t)v4)<(0);greater=((int32_t)v4)>(0);equal=((int32_t)v4)==(0);
if (equal) { goto L_027F0A74; }
v0 = gabi::load<u32>(v30 + 0x00000000u);
less=((int32_t)v0)<(0);greater=((int32_t)v0)>(0);equal=((int32_t)v0)==(0);
v3 = 0u + 0x10200000u;
v3 = gabi::load<u32>(v3 + 0xFFFF8B4Cu);
v3=gabi::call<u32>(0x02755FEC,v3,v4);
v9 = gabi::load<u32>(v3 + 0x0000000Cu);
v10 = gabi::load<u32>(v9 + 0x0000003Cu);
count = v10;
v4 = gabi::load<u32>(v30 + 0x00000004u);
v3=gabi::call_ptr<u32>(count,v3,v4);
v11 = 0u + 0x00000000u;
gabi::store<u32>(v30 + 0x00000000u, v11);
gabi::store<u32>(v30 + 0x00000004u, v11);
L_027F0A74: ;
v0 = v31 & 0x00000001u; less=(s32)v0<0;greater=(s32)v0>0;equal=v0==0;
if (equal) { goto L_027F0A84; }
v3 = v30;
v3=gabi::call<u32>(0x0273AF40,v3);
L_027F0A84: ;
return v3;
return v3;
}
VERIFY(0x027F0A0C,buffer_027F0A0C);
u32 buffer_027F0A9C(u32 a3) {
WWHD_FUNC(0x027F0A9C,u32,a3);
gabi::Local<u8[24]> frame;
u32 v0=0,v1=gabi::ea(frame.get()),v3=a3,v11=0,v12=0;
f64 q0=0,q11=0,q12=0,q13=0;
bool less=false,greater=false,equal=false;
v12 = 0u + 0x10170000u;
{ uint32_t ea = v12 + 0xFFFFE138u; q0 = gabi::load<f64>(ea); }
v11 = 0u + 0x10170000u;
v12 = gabi::load<u32>(v3 + 0x00000000u);
q12=gabi::load<f32>(v11 + 0xFFFFE130u);
v0 = 0u + 0x43300000u;
gabi::store<u32>(v1 + 0x0000000Cu, v12);
v11 = 0u + 0x10170000u;
gabi::store<u32>(v1 + 0x00000008u, v0);
{ uint32_t ea = v1 + 0x00000008u; q11 = gabi::load<f64>(ea); }
v12 = 0u + 0x00000000u;
q13=gabi::load<f32>(v11 + 0xFFFFE134u);
q11 = q11 - q0;
gabi::store<u32>(v3 + 0x00000008u, v12);
{ double v = to_single(q13 - q12); q0 = v; q0 = v; }
gabi::store<u32>(v3 + 0x0000000Cu, v12);
{ double v = to_single(q11); q11 = v; q11 = v; }
gabi::store<u32>(v3 + 0x0000001Cu, v12);
gabi::store<u32>(v3 + 0x00000020u, v12);
{ double v = to_single(q0 / q11); q0 = v; q0 = v; }
{ uint32_t ea = v3 + 0x00000010u; gabi::store<f32>(ea, q12); }
{ uint32_t ea = v3 + 0x00000014u; gabi::store<f32>(ea, q13); }
{ uint32_t ea = v3 + 0x00000018u; gabi::store<f32>(ea, q0); }
return v3;
return v3;
}
VERIFY(0x027F0A9C,buffer_027F0A9C);
u32 buffer_027F0B04(u32 a3,u32 a4) {
WWHD_FUNC(0x027F0B04,u32,a3,a4);
gabi::Local<u8[24]> frame;
u32 v0=0,v1=gabi::ea(frame.get()),v3=a3,v4=a4,v5=0,v8=0,v9=0,v10=0,v11=0,v12=0,v30=0,v31=0;
f64 q0=0,q10=0,q11=0,q12=0,q13=0;
bool less=false,greater=false,equal=false;u32 count=0;
v31 = v4; less=(s32)v31<0;greater=(s32)v31>0;equal=v31==0;
v30 = v3;
if (!greater) { goto L_027F0B60; }
v3 = 0u + 0x10200000u;
v3 = gabi::load<u32>(v3 + 0xFFFF8B4Cu);
v3=gabi::call<u32>(0x02756140);
v8 = gabi::load<u32>(v3 + 0x0000000Cu);
v12 = gabi::load<u32>(v8 + 0x00000034u);
count = v12;
v5 = 0u + 0x00000004u;
v4 = rotl32(v31, 2) & 0xFFFFFFFCu;
v3=gabi::call_ptr<u32>(count,v3,v4,v5);
less=((int32_t)v31)<(0);greater=((int32_t)v31)>(0);equal=((int32_t)v31)==(0);
v0 = v31;
if (greater) { goto L_027F0B58; }
v0 = 0u + 0x00000001u;
L_027F0B58: ;
less=((int32_t)v3)<(0);greater=((int32_t)v3)>(0);equal=((int32_t)v3)==(0);
if (!equal) { goto L_027F0B7C; }
L_027F0B60: ;
v9 = gabi::load<u32>(v30 + 0x00000000u);
v12 = gabi::load<u32>(v30 + 0x00000004u);
v8 = rotl32(v9, 2) & 0xFFFFFFFCu;
v0 = v12 + v8;
less=(v12)<(v0);greater=(v12)>(v0);equal=(v12)==(v0);
if (equal) { goto L_027F0BBC; }
goto L_027F0B98;
L_027F0B7C: ;
v8 = rotl32(v31, 2) & 0xFFFFFFFCu;
v12 = v3;
v0 = v3 + v8;
gabi::store<u32>(v30 + 0x00000000u, v31);
less=(v12)<(v0);greater=(v12)>(v0);equal=(v12)==(v0);
gabi::store<u32>(v30 + 0x00000004u, v3);
if (equal) { goto L_027F0BBC; }
L_027F0B98: ;
v0 = 0u + 0x00000000u;
L_027F0B9C: ;
gabi::store<u32>(v12 + 0x00000000u, v0);
v11 = gabi::load<u32>(v30 + 0x00000000u);
v3 = gabi::load<u32>(v30 + 0x00000004u);
v10 = rotl32(v11, 2) & 0xFFFFFFFCu;
v12 = v12 + 0x00000004u;
v11 = v3 + v10;
less=(v12)<(v11);greater=(v12)>(v11);equal=(v12)==(v11);
if (!equal) { goto L_027F0B9C; }
L_027F0BBC: ;
v9 = gabi::load<u32>(v30 + 0x00000000u);
q12=gabi::load<f32>(v30 + 0x00000014u);
v10 = 0u + 0x43300000u;
gabi::store<u32>(v1 + 0x0000000Cu, v9);
v9 = 0u + 0x10170000u;
gabi::store<u32>(v1 + 0x00000008u, v10);
{ uint32_t ea = v1 + 0x00000008u; q13 = gabi::load<f64>(ea); }
{ uint32_t ea = v9 + 0xFFFFE138u; q0 = gabi::load<f64>(ea); }
q10=gabi::load<f32>(v30 + 0x00000010u);
q11 = q13 - q0;
{ double v = to_single(q12 - q10); q10 = v; q10 = v; }
{ double v = to_single(q11); q13 = v; q13 = v; }
v3 = 0u + 0x00000000u;
{ double v = to_single(q10 / q13); q12 = v; q12 = v; }
gabi::store<u32>(v30 + 0x00000020u, v3);
{ uint32_t ea = v30 + 0x00000018u; gabi::store<f32>(ea, q12); }
return v3;
return v3;
}
VERIFY(0x027F0B04,buffer_027F0B04);
u32 buffer_027F0C14(u32 a3,u32 a4) {
WWHD_FUNC(0x027F0C14,u32,a3,a4);
gabi::Local<u8[56]> frame;
u32 v0=0,v1=gabi::ea(frame.get()),v3=a3,v4=a4,v7=0,v8=0,v9=0,v10=0,v11=0,v12=0,v30=0,v31=0;
f64 q0=0,q1=0,q12=0,q13=0;
bool less=false,greater=false,equal=false;
v31 = v3;
v30 = v4;
v3 = v30;
v3=gabi::call<u32>(0x027F1508,v3);
v10 = gabi::load<u32>(v31 + 0x0000001Cu);
q13=gabi::load<f32>(v10 + 0x0000000Cu);
{ uint32_t ea = v1 + 0x00000008u; gabi::store<f32>(ea, q13); }
q13=gabi::load<f32>(v10 + 0x0000001Cu);
v7 = gabi::load<u32>(v1 + 0x00000008u);
{ uint32_t ea = v1 + 0x0000000Cu; gabi::store<f32>(ea, q13); }
q13=gabi::load<f32>(v10 + 0x0000002Cu);
v8 = gabi::load<u32>(v1 + 0x0000000Cu);
{ uint32_t ea = v1 + 0x00000010u; gabi::store<f32>(ea, q13); }
v0 = gabi::load<u32>(v1 + 0x00000010u);
v4 = v1 + 0x00000020u;
gabi::store<u32>(v1 + 0x00000020u, v7);
v3 = 0u + 0x104B0000u;
gabi::store<u32>(v1 + 0x00000024u, v8);
v3 = v3 + 0x000045F8u;
gabi::store<u32>(v1 + 0x00000028u, v0);
q1=gabi::call<f64>(0x027F2B68,v3,v4);
q0=gabi::load<f32>(v31 + 0x00000010u);
q13=gabi::load<f32>(v31 + 0x00000018u);
q12 = -q1;
{ double v = to_single(q0 + q13); q0 = v; q0 = v; }
less=(q0)<(q12);greater=(q0)>(q12);equal=(q0)==(q12);
if (!less) { goto L_027F0D30; }
q0=gabi::load<f32>(v31 + 0x00000014u);
{ double v = to_single(q0 - q13); q0 = v; q0 = v; }
less=(q0)<(q12);greater=(q0)>(q12);equal=(q0)==(q12);
if (!greater) { goto L_027F0D18; }
{ double v = to_single(q12 / q13); q13 = v; q13 = v; }
v12 = 0u + 0x10170000u;
q0=gabi::load<f32>(v12 + 0xFFFFE140u);
less=(q13)<(q0);greater=(q13)>(q0);equal=(q13)==(q0);
if (!less) { goto L_027F0CE4; }
q0 = u64_as_f64(ppc_fctiwz(q13));
v0 = v1 + 0x00000018u;
gabi::store<u32>(0u + v0, (uint32_t)f64_as_u64(q0));
v12 = gabi::load<u32>(v31 + 0x00000000u);
v0 = gabi::load<u32>(v1 + 0x00000018u);
v7 = v12 + 0xFFFFFFFFu;
v0 = v7 - v0;
less=(v0)<(v12);greater=(v0)>(v12);equal=(v0)==(v12);
v12 = gabi::load<u32>(v31 + 0x00000004u);
if (less) { goto L_027F0D44; }
L_027F0CE0: ;
goto L_027F0D4C;
L_027F0CE4: ;
{ double v = to_single(q13 - q0); q13 = v; q13 = v; }
q0 = u64_as_f64(ppc_fctiwz(q13));
v8 = v1 + 0x00000018u;
gabi::store<u32>(0u + v8, (uint32_t)f64_as_u64(q0));
v11 = gabi::load<u32>(v1 + 0x00000018u);
v12 = gabi::load<u32>(v31 + 0x00000000u);
v0 = v11 + 0x80000000u;
v7 = v12 + 0xFFFFFFFFu;
v0 = v7 - v0;
less=(v0)<(v12);greater=(v0)>(v12);equal=(v0)==(v12);
v12 = gabi::load<u32>(v31 + 0x00000004u);
if (!less) { goto L_027F0CE0; }
goto L_027F0D44;
L_027F0D18: ;
v12 = gabi::load<u32>(v31 + 0x00000000u);
v0 = 0u + 0x00000000u;
less=(v0)<(v12);greater=(v0)>(v12);equal=(v0)==(v12);
v12 = gabi::load<u32>(v31 + 0x00000004u);
if (!less) { goto L_027F0CE0; }
goto L_027F0D44;
L_027F0D30: ;
v12 = gabi::load<u32>(v31 + 0x00000000u);
v0 = v12 + 0xFFFFFFFFu;
less=(v0)<(v12);greater=(v0)>(v12);equal=(v0)==(v12);
v12 = gabi::load<u32>(v31 + 0x00000004u);
if (!less) { goto L_027F0CE0; }
L_027F0D44: ;
v8 = rotl32(v0, 2) & 0xFFFFFFFCu;
v12 = v12 + v8;
L_027F0D4C: ;
gabi::store<u32>(v30 + 0x00000094u, v12);
v9 = gabi::load<u32>(v31 + 0x00000000u);
less=(v0)<(v9);greater=(v0)>(v9);equal=(v0)==(v9);
v11 = gabi::load<u32>(v31 + 0x00000004u);
if (!less) { goto L_027F0D68; }
v10 = rotl32(v0, 2) & 0xFFFFFFFCu;
v11 = v11 + v10;
L_027F0D68: ;
v12 = gabi::load<u32>(v11 + 0x00000000u);
gabi::store<u32>(v30 + 0x00000010u, v12);
v11 = gabi::load<u32>(v31 + 0x00000000u);
less=(v0)<(v11);greater=(v0)>(v11);equal=(v0)==(v11);
v10 = gabi::load<u32>(v31 + 0x00000004u);
if (!less) { goto L_027F0D88; }
v7 = rotl32(v0, 2) & 0xFFFFFFFCu;
v10 = v10 + v7;
L_027F0D88: ;
gabi::store<u32>(v10 + 0x00000000u, v30);
v3 = 0u + 0x00000001u;
return v3;
return v3;
}
VERIFY(0x027F0C14,buffer_027F0C14);
u32 buffer_027F0DA8(u32 a3,u32 a4) {
WWHD_FUNC(0x027F0DA8,u32,a3,a4);
gabi::Local<u8[16]> frame;
u32 v0=0,v3=a3,v4=a4,v9=0,v11=0,v12=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;
v31 = v4;
v30 = v3;
v3 = v31;
v3=gabi::call<u32>(0x027F1508,v3);
v0 = gabi::load<u32>(v30 + 0x00000004u);
gabi::store<u32>(v31 + 0x00000094u, v0);
v11 = gabi::load<u32>(v30 + 0x00000004u);
v9 = gabi::load<u32>(v11 + 0x00000000u);
gabi::store<u32>(v31 + 0x00000010u, v9);
v12 = gabi::load<u32>(v30 + 0x00000004u);
v3 = 0u + 0x00000001u;
gabi::store<u32>(v12 + 0x00000000u, v31);
return v3;
return v3;
}
VERIFY(0x027F0DA8,buffer_027F0DA8);
u32 buffer_027F0E04(u32 a3,u32 a4,u32 a5) {
WWHD_FUNC(0x027F0E04,u32,a3,a4,a5);
gabi::Local<u8[24]> frame;
u32 v0=0,v3=a3,v4=a4,v5=a5,v10=0,v11=0,v12=0,v29=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;
v31 = v5;
v30 = v4;
v29 = v3;
v12 = gabi::load<u32>(v30 + 0x00000094u);
less=((int32_t)v12)<(0);greater=((int32_t)v12)>(0);equal=((int32_t)v12)==(0);
if (equal) { goto L_027F0E4C; }
v3 = 0u + 0x10170000u;
v5 = 0u + 0x10170000u;
v4 = 0u + 0x000000F3u;
v3 = v3 + 0xFFFFE160u;
v5 = v5 + 0xFFFFE144u;
v3=gabi::call<u32>(0x0273AA24,v3,v4,v5);
L_027F0E4C: ;
v0 = gabi::load<u32>(v29 + 0x00000000u);
v10 = v31;
less=(v10)<(v0);greater=(v10)>(v0);equal=(v10)==(v0);
v12 = gabi::load<u32>(v29 + 0x00000004u);
if (!less) { goto L_027F0E68; }
v0 = rotl32(v10, 2) & 0xFFFFFFFCu;
v12 = v12 + v0;
L_027F0E68: ;
gabi::store<u32>(v30 + 0x00000094u, v12);
v10 = gabi::load<u32>(v29 + 0x00000000u);
v11 = v31;
less=(v11)<(v10);greater=(v11)>(v10);equal=(v11)==(v10);
v10 = gabi::load<u32>(v29 + 0x00000004u);
if (!less) { goto L_027F0E88; }
v11 = rotl32(v11, 2) & 0xFFFFFFFCu;
v10 = v10 + v11;
L_027F0E88: ;
v12 = gabi::load<u32>(v10 + 0x00000000u);
gabi::store<u32>(v30 + 0x00000010u, v12);
v0 = gabi::load<u32>(v29 + 0x00000000u);
less=(v31)<(v0);greater=(v31)>(v0);equal=(v31)==(v0);
v11 = gabi::load<u32>(v29 + 0x00000004u);
if (!less) { goto L_027F0EA8; }
v0 = rotl32(v31, 2) & 0xFFFFFFFCu;
v11 = v11 + v0;
L_027F0EA8: ;
gabi::store<u32>(v11 + 0x00000000u, v30);
v3 = 0u + 0x00000001u;
return v3;
return v3;
}
VERIFY(0x027F0E04,buffer_027F0E04);
u32 buffer_027F0ECC(u32 a3,u32 a4,u32 a5) {
WWHD_FUNC(0x027F0ECC,u32,a3,a4,a5);
gabi::Local<u8[40]> frame;
u32 v0=0,v1=gabi::ea(frame.get()),v3=a3,v4=a4,v5=a5,v6=0,v7=0,v8=0,v9=0,v10=0,v12=0,v25=0,v26=0,v27=0,v28=0,v29=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;
v26 = v3;
v30 = v4; less=(s32)v30<0;greater=(s32)v30>0;equal=v30==0;
v31 = v5;
v29 = 0u + 0x00000000u;
if (equal) { goto L_027F0F94; }
v27 = gabi::load<u32>(v30 + 0x00000094u);
less=((int32_t)v27)<(0);greater=((int32_t)v27)>(0);equal=((int32_t)v27)==(0);
v28 = 0u + 0x10170000u;
if (equal) { goto L_027F0F44; }
v10 = gabi::load<u32>(v26 + 0x00000000u);
v8 = v31;
less=(v8)<(v10);greater=(v8)>(v10);equal=(v8)==(v10);
v12 = gabi::load<u32>(v26 + 0x00000004u);
if (!less) { goto L_027F0F28; }
v6 = rotl32(v8, 2) & 0xFFFFFFFCu;
v0 = v12 + v6;
less=(v27)<(v0);greater=(v27)>(v0);equal=(v27)==(v0);
if (!equal) { goto L_027F0F30; }
goto L_027F0F4C;
L_027F0F28: ;
less=(v27)<(v12);greater=(v27)>(v12);equal=(v27)==(v12);
if (equal) { goto L_027F0F4C; }
L_027F0F30: ;
v5 = 0u + 0x10170000u;
v4 = 0u + 0x00000110u;
v5 = v5 + 0xFFFFE1B8u;
v3 = v28 + 0xFFFFE184u;
v3=gabi::call<u32>(0x0273AA24,v3,v4,v5);
L_027F0F44: ;
v10 = gabi::load<u32>(v26 + 0x00000000u);
v12 = gabi::load<u32>(v26 + 0x00000004u);
L_027F0F4C: ;
v0 = v31;
less=(v0)<(v10);greater=(v0)>(v10);equal=(v0)==(v10);
if (!less) { goto L_027F0F6C; }
v8 = rotl32(v0, 2) & 0xFFFFFFFCu;
v25 = gabi::load<u32>(v12 + v8);
less=((int32_t)v25)<(0);greater=((int32_t)v25)>(0);equal=((int32_t)v25)==(0);
if (!equal) { goto L_027F0FAC; }
goto L_027F0F78;
L_027F0F6C: ;
v25 = gabi::load<u32>(v12 + 0x00000000u);
less=((int32_t)v25)<(0);greater=((int32_t)v25)>(0);equal=((int32_t)v25)==(0);
if (!equal) { goto L_027F0FAC; }
L_027F0F78: ;
less=((int32_t)v27)<(0);greater=((int32_t)v27)>(0);equal=((int32_t)v27)==(0);
if (equal) { goto L_027F0F94; }
v5 = 0u + 0x10170000u;
v4 = 0u + 0x00000116u;
v5 = v5 + 0xFFFFE174u;
v3 = v28 + 0xFFFFE184u;
v3=gabi::call<u32>(0x0273AA24,v3,v4,v5);
L_027F0F94: ;
v0 = gabi::load<u32>(v1 + 0x0000002Cu);
v3 = 0u + 0x00000000u;
return v3;
L_027F0FAC: ;
v5 = 0u + 0x10170000u;
less=(v25)<(v30);greater=(v25)>(v30);equal=(v25)==(v30);
v5 = v5 + 0xFFFFE198u;
if (!equal) { goto L_027F1030; }
v0 = v31;
less=(v0)<(v10);greater=(v0)>(v10);equal=(v0)==(v10);
if (!less) { goto L_027F0FDC; }
v9 = rotl32(v0, 2) & 0xFFFFFFFCu;
v7 = v12 + v9;
less=(v27)<(v7);greater=(v27)>(v7);equal=(v27)==(v7);
if (!equal) { goto L_027F0FE4; }
goto L_027F0FF8;
L_027F0FDC: ;
less=(v27)<(v12);greater=(v27)>(v12);equal=(v27)==(v12);
if (equal) { goto L_027F0FF8; }
L_027F0FE4: ;
v4 = 0u + 0x0000011Bu;
v3 = v28 + 0xFFFFE184u;
v3=gabi::call<u32>(0x0273AA24,v3,v4,v5);
v10 = gabi::load<u32>(v26 + 0x00000000u);
v12 = gabi::load<u32>(v26 + 0x00000004u);
L_027F0FF8: ;
less=(v31)<(v10);greater=(v31)>(v10);equal=(v31)==(v10);
v7 = gabi::load<u32>(v30 + 0x00000010u);
if (!less) { goto L_027F100C; }
v10 = rotl32(v31, 2) & 0xFFFFFFFCu;
v12 = v12 + v10;
L_027F100C: ;
gabi::store<u32>(v12 + 0x00000000u, v7);
gabi::store<u32>(v30 + 0x00000094u, v29);
gabi::store<u32>(v30 + 0x00000010u, v29);
v0 = gabi::load<u32>(v1 + 0x0000002Cu);
v3 = 0u + 0x00000001u;
return v3;
L_027F1030: ;
v0 = gabi::load<u32>(v25 + 0x00000010u);
L_027F1034: ;
less=(v0)<(v30);greater=(v0)>(v30);equal=(v0)==(v30);
if (equal) { goto L_027F1034; }
less=(v31)<(v10);greater=(v31)>(v10);equal=(v31)==(v10);
if (!less) { goto L_027F104C; }
v0 = rotl32(v31, 2) & 0xFFFFFFFCu;
v12 = v12 + v0;
L_027F104C: ;
less=(v27)<(v12);greater=(v27)>(v12);equal=(v27)==(v12);
if (equal) { goto L_027F1060; }
v4 = 0u + 0x00000130u;
v3 = v28 + 0xFFFFE184u;
v3=gabi::call<u32>(0x0273AA24,v3,v4,v5);
L_027F1060: ;
v8 = gabi::load<u32>(v30 + 0x00000010u);
gabi::store<u32>(v25 + 0x00000010u, v8);
gabi::store<u32>(v30 + 0x00000094u, v29);
v3 = 0u + 0x00000001u;
gabi::store<u32>(v30 + 0x00000010u, v29);
return v3;
return v3;
}
VERIFY(0x027F0ECC,buffer_027F0ECC);
u32 buffer_027F1088(u32 a3,u32 a4) {
WWHD_FUNC(0x027F1088,u32,a3,a4);
u32 v0=0,v3=a3,v4=a4,v5=0,v6=0,v7=0,v8=0,v9=0,v10=0,v11=0,v12=0;

bool less=false,greater=false,equal=false;u32 count=0;
v9 = gabi::load<u32>(v3 + 0x00000008u);
v8 = rotl32(v9, 3) & 0xFFFFFFF8u;
v10 = v8 + 0x10200000u;
v7 = v10 + 0xFFFF98C8u;
v5 = (uint32_t)(int32_t)(int16_t)gabi::load<u16>(v10 + 0xFFFF98C8u);
v11 = (uint32_t)(int32_t)(int16_t)gabi::load<u16>(v7 + 0x00000002u);
v0 = gabi::load<u32>(v7 + 0x00000004u);
less=((int32_t)v11)<(0);greater=((int32_t)v11)>(0);equal=((int32_t)v11)==(0);
v3 = v3 + v5;
if (less) { goto L_027F10C4; }
v0 = (uint32_t)(int32_t)(int16_t)v0;
v12 = gabi::load<u32>(v3 + v0);
v11 = rotl32(v11, 3) & 0xFFFFFFF8u;
v6 = v12 + v11;
v0 = gabi::load<u32>(v6 + 0x00000004u);
L_027F10C4: ;
count = v0;
v3=gabi::call_ptr<u32>(count,v3,v4);
return v3;
return v3;
}
VERIFY(0x027F1088,buffer_027F1088);
u32 buffer_027F10CC(u32 a3,u32 a4) {
WWHD_FUNC(0x027F10CC,u32,a3,a4);
gabi::Local<u8[24]> frame;
u32 v0=0,v3=a3,v4=a4,v9=0,v10=0,v11=0,v12=0,v28=0,v29=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;u32 count=0;
v28 = v3;
v29 = v4;
v0 = gabi::load<u32>(v28 + 0x00000000u);
v31 = gabi::load<u32>(v28 + 0x00000004u);
v0 = rotl32(v0, 2) & 0xFFFFFFFCu;
v12 = v31 + v0;
less=(v31)<(v12);greater=(v31)>(v12);equal=(v31)==(v12);
if (equal) { goto L_027F1154; }
L_027F1108: ;
v30 = gabi::load<u32>(v31 + 0x00000000u);
less=((int32_t)v30)<(0);greater=((int32_t)v30)>(0);equal=((int32_t)v30)==(0);
if (equal) { goto L_027F1148; }
L_027F1114: ;
v9 = gabi::load<u32>(v30 + 0x0000000Cu);
v10 = gabi::load<u32>(v9 + 0x0000002Cu);
count = v10;
v3 = v30;
v4 = v29;
v3=gabi::call_ptr<u32>(count,v3,v4);
v30 = gabi::load<u32>(v30 + 0x00000010u);
less=((int32_t)v30)<(0);greater=((int32_t)v30)>(0);equal=((int32_t)v30)==(0);
if (!equal) { goto L_027F1114; }
v0 = gabi::load<u32>(v28 + 0x00000000u);
v11 = gabi::load<u32>(v28 + 0x00000004u);
v0 = rotl32(v0, 2) & 0xFFFFFFFCu;
v12 = v11 + v0;
L_027F1148: ;
v31 = v31 + 0x00000004u;
less=(v31)<(v12);greater=(v31)>(v12);equal=(v31)==(v12);
if (!equal) { goto L_027F1108; }
L_027F1154: ;
return v3;
return v3;
}
VERIFY(0x027F10CC,buffer_027F10CC);
u32 buffer_027F1174(u32 a3,u32 a4) {
WWHD_FUNC(0x027F1174,u32,a3,a4);
gabi::Local<u8[24]> frame;
u32 v0=0,v3=a3,v4=a4,v8=0,v9=0,v10=0,v12=0,v28=0,v29=0,v30=0,v31=0;

bool less=false,greater=false,equal=false;u32 count=0;
v30 = v3;
v31 = v4;
v0 = gabi::load<u32>(v30 + 0x00000000u);
less=((int32_t)v0)<(0);greater=((int32_t)v0)>(0);equal=((int32_t)v0)==(0);
if (equal) { goto L_027F11F0; }
v9 = gabi::load<u32>(v30 + 0x00000000u);
v12 = gabi::load<u32>(v30 + 0x00000004u);
v8 = rotl32(v9, 2) & 0xFFFFFFFCu;
v29 = v12 + v8;
L_027F11B4: ;
{ uint32_t ea = v29 + 0xFFFFFFFCu; v28 = gabi::load<u32>(ea); v29 = ea; }
less=((int32_t)v28)<(0);greater=((int32_t)v28)>(0);equal=((int32_t)v28)==(0);
if (equal) { goto L_027F11E8; }
L_027F11C0: ;
v10 = gabi::load<u32>(v28 + 0x0000000Cu);
v0 = gabi::load<u32>(v10 + 0x0000002Cu);
count = v0;
v3 = v28;
v4 = v31;
v3=gabi::call_ptr<u32>(count,v3,v4);
v28 = gabi::load<u32>(v28 + 0x00000010u);
less=((int32_t)v28)<(0);greater=((int32_t)v28)>(0);equal=((int32_t)v28)==(0);
if (!equal) { goto L_027F11C0; }
v12 = gabi::load<u32>(v30 + 0x00000004u);
L_027F11E8: ;
less=(v29)<(v12);greater=(v29)>(v12);equal=(v29)==(v12);
if (!equal) { goto L_027F11B4; }
L_027F11F0: ;
return v3;
return v3;
}
VERIFY(0x027F1174,buffer_027F1174);
u32 buffer_027F1210(u32 a3) {
WWHD_FUNC(0x027F1210,u32,a3);
u32 v0=0,v3=a3,v11=0,v12=0;

bool less=false,greater=false,equal=false;
v0 = gabi::load<u32>(v3 + 0x00000000u);
v12 = gabi::load<u32>(v3 + 0x00000004u);
v11 = rotl32(v0, 2) & 0xFFFFFFFCu;
v11 = v12 + v11;
less=(v12)<(v11);greater=(v12)>(v11);equal=(v12)==(v11);
if (equal) { goto L_027F1248; }
L_027F1228: ;
v0 = gabi::load<u32>(v12 + 0x00000000u);
less=((int32_t)v0)<(0);greater=((int32_t)v0)>(0);equal=((int32_t)v0)==(0);
if (equal) { goto L_027F123C; }
v3 = 0u + 0x00000000u;
return v3;
L_027F123C: ;
v12 = v12 + 0x00000004u;
less=(v12)<(v11);greater=(v12)>(v11);equal=(v12)==(v11);
if (!equal) { goto L_027F1228; }
L_027F1248: ;
v3 = 0u + 0x00000001u;
return v3;
return v3;
}
VERIFY(0x027F1210,buffer_027F1210);
u32 buffer_027F1250() {
WWHD_FUNC(0x027F1250,u32);
u32 v0=0,v3=0,v12=0;

bool less=false,greater=false,equal=false;
v12 = 0u + 0x104B0000u;
v0 = 0u + 0x00000000u;
v12 = v12 + 0x00004588u;
gabi::store<u32>(v12 + 0x00000008u, v0);
gabi::store<u32>(v12 + 0x00000000u, v0);
v3 = 0u + 0x10200000u;
gabi::store<u32>(v12 + 0x0000000Cu, v0);
v3 = v3 + 0xFFFF988Cu;
gabi::store<u32>(v12 + 0x00000004u, v0);
v3=gabi::call<u32>(0x028F026C,v3);
return v3;
return v3;
}
VERIFY(0x027F1250,buffer_027F1250);
