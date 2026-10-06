/* Local WWHD menu scene reconstruction. */
#include "wwhd.h"
// Explicit native blocks retain all debug menu string comparisons and callback boundaries.
// The 176-byte frame contains the native eight-byte SafeString temporaries and conversion slots.
s32 menu_execute(void* self) {
 WWHD_FUNC(0x025AC798,s32,self);
 gabi::Local<u8[176]> frame;
 u32 v0=0,v1=gabi::ea(frame.get()),v3=gabi::ea(self),v4=0,v5=0,v6=0,v7=0,v8=0,v9=0,v10=0,v11=0,v12=0,v21=0,v22=0,v23=0,v24=0,v25=0,v26=0,v27=0,v28=0,v29=0,v30=0,v31=0;
 f64 q0=0,q1=0,q6=0,q7=0,q8=0,q9=0,q10=0,q11=0,q12=0,q13=0,q30=0,q31=0;
 bool less=false,greater=false,equal=false; u32 count=0; u8 carry=0;
 v29 = v3;
 v3 = 0u + 0x00000000u;
 v26 = gabi::load<u32>(v29 + 0x000001D8u);
 v3=gabi::call<u32>(0x020075E0 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 v27 = 0u + 0x101F0000u;
 if (!equal) { goto L_025AC7EC; }
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x0200760C ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (equal) { goto L_025AC87C; }
 L_025AC7EC: ;
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x02007764 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (!equal) { goto L_025AC80C; }
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x02007790 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (equal) { goto L_025AC818; }
 L_025AC80C: ;
 v9 = 0u + 0x00000014u;
 gabi::store<u8>(v29 + 0x000001E5u, v9);
 goto L_025AC830;
 L_025AC818: ;
 v3 = v29 + 0x000001E5u;
 v3=gabi::call<u32>(0x0207A9A0 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (!equal) { goto L_025AC87C; }
 v9 = 0u + 0x00000004u;
 gabi::store<u8>(v29 + 0x000001E5u, v9);
 L_025AC830: ;
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x020075E0 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 v8 = gabi::load<u32>(v27 + 0xFFFFA6C4u);
 if (equal) { goto L_025AC860; }
 { uint64_t t = (uint64_t)v8 + 0xFFFFFFFFu; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); } less=(s32)v0<0; greater=(s32)v0>0; equal=v0==0;
 gabi::store<u32>(v27 + 0xFFFFA6C4u, v0);
 if (!less) { goto L_025AC87C; }
 v7 = gabi::load<u8>(v26 + 0x00000000u);
 v12 = v7 + 0xFFFFFFFFu;
 gabi::store<u32>(v27 + 0xFFFFA6C4u, v12);
 goto L_025AC87C;
 L_025AC860: ;
 v8 = v8 + 0x00000001u;
 gabi::store<u32>(v27 + 0xFFFFA6C4u, v8);
 v0 = gabi::load<u8>(v26 + 0x00000000u);
 less=((int32_t)v8)<((int32_t)v0); greater=((int32_t)v8)>((int32_t)v0); equal=((int32_t)v8)==((int32_t)v0);
 if (less) { goto L_025AC87C; }
 v12 = 0u + 0x00000000u;
 gabi::store<u32>(v27 + 0xFFFFA6C4u, v12);
 L_025AC87C: ;
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x020076E0 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (!equal) { goto L_025AC89C; }
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x020076BC ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (equal) { goto L_025AC974; }
 L_025AC89C: ;
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x020078BC ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (!equal) { goto L_025AC8BC; }
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x02007898 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (equal) { goto L_025AC8C8; }
 L_025AC8BC: ;
 v10 = 0u + 0x00000014u;
 gabi::store<u8>(v29 + 0x000001E6u, v10);
 goto L_025AC8E0;
 L_025AC8C8: ;
 v3 = v29 + 0x000001E6u;
 v3=gabi::call<u32>(0x0207A9A0 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (!equal) { goto L_025AC974; }
 v10 = 0u + 0x00000004u;
 gabi::store<u8>(v29 + 0x000001E6u, v10);
 L_025AC8E0: ;
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x020076E0 ,v3);
 v10 = 0u + 0x101F0000u;
 v5 = gabi::load<u32>(v27 + 0xFFFFA6C4u);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 v10 = gabi::load<u32>(v10 + 0xFFFFA6CCu);
 if (equal) { goto L_025AC938; }
 v6 = gabi::load<u8>(v10 + v5);
 v6 = v6 + 0xFFFFFFFFu;
 v6 = (uint32_t)(int32_t)(int8_t)v6; less=(s32)v6<0; greater=(s32)v6>0; equal=v6==0;
 gabi::store<u8>(v10 + v5, v6);
 if (!less) { goto L_025AC974; }
 v5 = gabi::load<u32>(v27 + 0xFFFFA6C4u);
 v4 = ((u32)v5 * 40u);
 v8 = gabi::load<u32>(v26 + 0x00000004u);
 v6 = v8 + v4;
 v9 = gabi::load<u8>(v6 + 0x00000021u);
 v4 = v9 + 0xFFFFFFFFu;
 v9 = 0u + 0x101F0000u;
 v9 = gabi::load<u32>(v9 + 0xFFFFA6CCu);
 gabi::store<u8>(v9 + v5, v4);
 goto L_025AC974;
 L_025AC938: ;
 v7 = gabi::load<u8>(v10 + v5);
 v7 = v7 + 0x00000001u;
 v7 = (uint32_t)(int32_t)(int8_t)v7;
 gabi::store<u8>(v10 + v5, v7);
 v5 = gabi::load<u32>(v27 + 0xFFFFA6C4u);
 v0 = ((u32)v5 * 40u);
 v12 = gabi::load<u32>(v26 + 0x00000004u);
 v11 = v12 + v0;
 v4 = gabi::load<u8>(v11 + 0x00000021u);
 less=((int32_t)v7)<((int32_t)v4); greater=((int32_t)v7)>((int32_t)v4); equal=((int32_t)v7)==((int32_t)v4);
 if (less) { goto L_025AC974; }
 v9 = 0u + 0x101F0000u;
 v9 = gabi::load<u32>(v9 + 0xFFFFA6CCu);
 v4 = 0u + 0x00000000u;
 gabi::store<u8>(v9 + v5, v4);
 L_025AC974: ;
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x02007940 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 v28 = 0u + 0x10200000u;
 if (equal) { goto L_025AD254; }
 v25 = 0u + 0x10050000u;
 v31 = 0u + 0x025B0000u;
 v6 = 0u + 0x10050000u;
 v25 = v25 + 0x000028C4u;
 v31 = v31 + 0xFFFFD9B4u;
 v6 = v6 + 0x0000291Cu;
 v0 = gabi::load<u32>(v27 + 0xFFFFA6C4u);
 count = v31;
 gabi::store<u32>(v1 + 0x0000004Cu, v25);
 v7 = ((u32)v0 * 40u);
 gabi::store<u32>(v1 + 0x00000048u, v6);
 v9 = gabi::load<u32>(v26 + 0x00000004u);
 gabi::store<u32>(v1 + 0x00000054u, v25);
 v0 = v9 + v7;
 v3 = v1 + 0x00000048u;
 gabi::store<u32>(v1 + 0x00000050u, v0);
 v3=gabi::call_ptr<u32>(count,v3);
 v10 = gabi::load<u32>(v1 + 0x0000004Cu);
 v11 = gabi::load<u32>(v10 + 0x00000014u);
 count = v11;
 v3 = v1 + 0x00000048u;
 v3=gabi::call_ptr<u32>(count,v3);
 v12 = gabi::load<u32>(v1 + 0x00000054u);
 v0 = gabi::load<u32>(v12 + 0x00000014u);
 count = v0;
 v30 = gabi::load<u32>(v1 + 0x00000048u);
 v3 = v1 + 0x00000050u;
 v3=gabi::call_ptr<u32>(count,v3);
 v4 = gabi::load<u32>(v1 + 0x00000050u);
 less=(v30)<(v4); greater=(v30)>(v4); equal=(v30)==(v4);
 if (equal) { goto L_025ACB0C; }
 v11 = 0u + 0x00040000u;
 v10 = gabi::load<u32>(v1 + 0x00000048u);
 v11 = v11 + 0x00000001u;
 v12 = gabi::load<u32>(v1 + 0x00000050u);
 count = v11;
 v12 = v12 + 0xFFFFFFFFu;
 L_025ACA1C: ;
 v0 = gabi::load<u8>(v10 + 0x00000000u);
 { uint32_t ea = v12 + 0x00000001u; v5 = gabi::load<u8>(ea); v12 = ea; }
 less=(v0)<(v5); greater=(v0)>(v5); equal=(v0)==(v5);
 if (!equal) { goto L_025AD188; }
 less=((int32_t)v0)<(0); greater=((int32_t)v0)>(0); equal=((int32_t)v0)==(0);
 if (equal) { goto L_025ACB0C; }
 v10 = v10 + 0x00000001u;
 count--; if (count != 0) { goto L_025ACA1C; }
 v5 = gabi::load<u32>(v27 + 0xFFFFA6C4u);
 v8 = 0u + 0x101F0000u;
 v8 = gabi::load<u32>(v8 + 0xFFFFA6CCu);
 v12 = ((u32)v5 * 40u);
 v8 = gabi::load<u8>(v8 + v5);
 v5 = gabi::load<u32>(v26 + 0x00000004u);
 v8 = (uint32_t)(int32_t)(int8_t)v8;
 v4 = v5 + v12;
 v7 = ((u32)v8 * 44u);
 v10 = gabi::load<u32>(v4 + 0x00000024u);
 v31 = v10 + v7;
 v3=gabi::call<u32>(0x025200D4);
 v5 = 0u + 0x00000000u;
 gabi::store<u8>(v3 + 0x0000514Cu, v5);
 v11 = gabi::load<u8>(v29 + 0x000001E4u);
 less=((int32_t)v11)<(0); greater=((int32_t)v11)>(0); equal=((int32_t)v11)==(0);
 if (!equal) { goto L_025AD1CC; }
 L_025ACA80: ;
 v5 = gabi::load<u8>(v31 + 0x00000029u);
 v7 = 0u + 0x10050000u;
 v0 = gabi::load<u8>(v31 + 0x0000002Au);
 v5 = (uint32_t)(int32_t)(int8_t)v5;
 q1=gabi::load<f32>(v7 + 0x000028DCu);
 v7 = 0u + 0x00000000u;
 v3 = v31 + 0x00000021u;
 v6 = gabi::load<u8>(v31 + 0x0000002Bu);
 v8 = 0u + 0x00000001u;
 v9 = v7;
 v4 = v0;
 v6 = (uint32_t)(int32_t)(int8_t)v6;
 v3=gabi::call<u32>(0x0252012C ,v3,v4,v5,v6,v7,v8,v9,q1);
 v5 = 0u + 0x00000000u;
 v4 = 0u + 0x00000007u;
 v3 = v29;
 v7 = v5;
 v6 = 0u + 0x00000005u;
 v3=gabi::call<u32>(0x025DC86C ,v3,v4,v5,v6,v7);
 v12 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 v4 = 0u + 0x00000000u;
 gabi::store<u32>(v12 + 0x0000116Cu, v4);
 v3=gabi::call<u32>(0x025200D4);
 v30 = v3 + 0x00005140u;
 v3=gabi::call<u32>(0x025200D4);
 v31 = gabi::load<u8>(v3 + 0x0000514Au);
 v31 = (uint32_t)(int32_t)(int8_t)v31;
 v3=gabi::call<u32>(0x025200D4);
 v7 = v3 + 0x000012A0u;
 v5 = gabi::load<u8>(v7 + 0x00003EABu);
 v4 = v31;
 v3 = v30;
 v5 = (uint32_t)(int32_t)(int8_t)v5;
 v3=gabi::call<u32>(0x025E17CC ,v3,v4,v5);
 goto L_025AD254;
 L_025ACB0C: ;
 v5 = gabi::load<u32>(v27 + 0xFFFFA6C4u);
 v10 = 0u + 0x101F0000u;
 v10 = gabi::load<u32>(v10 + 0xFFFFA6CCu);
 v6 = ((u32)v5 * 40u);
 v0 = gabi::load<u8>(v10 + v5);
 v8 = gabi::load<u32>(v26 + 0x00000004u);
 v0 = (uint32_t)(int32_t)(int8_t)v0;
 v7 = v8 + v6;
 v9 = ((u32)v0 * 44u);
 v12 = gabi::load<u32>(v7 + 0x00000024u);
 gabi::store<u32>(v1 + 0x00000034u, v25);
 v12 = v12 + v9;
 gabi::store<u32>(v1 + 0x00000030u, v12);
 v4 = gabi::load<u8>(v10 + v5);
 v4 = (uint32_t)(int32_t)(int8_t)v4;
 v24 = 0u + 0x00000001u;
 less=((int32_t)v4)<(23); greater=((int32_t)v4)>(23); equal=((int32_t)v4)==(23);
 v0 = 0u + 0x00000000u;
 if (!greater) { goto L_025ACFBC; }
 count = v31;
 v5 = 0u + 0x10050000u;
 gabi::store<u32>(v1 + 0x0000003Cu, v25);
 v5 = v5 + 0x00002924u;
 v3 = v1 + 0x00000030u;
 gabi::store<u32>(v1 + 0x00000038u, v5);
 v0 = 0u + 0x00000001u;
 v3=gabi::call_ptr<u32>(count,v3);
 v7 = gabi::load<u32>(v1 + 0x00000030u);
 v6 = gabi::load<u8>(v7 + 0x00000000u);
 v31 = 0u + 0x00000000u;
 less=((int32_t)v6)<(0); greater=((int32_t)v6)>(0); equal=((int32_t)v6)==(0);
 v0 = 0u + 0x00040000u;
 if (equal) { goto L_025ACBB4; }
 L_025ACB90: ;
 v31 = v31 + 0x00000001u;
 less=((int32_t)v31)<((int32_t)v0); greater=((int32_t)v31)>((int32_t)v0); equal=((int32_t)v31)==((int32_t)v0);
 v7 = v7 + 0x00000001u;
 if (greater) { goto L_025ACBB0; }
 v8 = gabi::load<u8>(v7 + 0x00000000u);
 less=((int32_t)v8)<(0); greater=((int32_t)v8)>(0); equal=((int32_t)v8)==(0);
 if (!equal) { goto L_025ACB90; }
 goto L_025ACBB4;
 L_025ACBB0: ;
 v31 = 0u + 0x00000000u;
 L_025ACBB4: ;
 v9 = gabi::load<u32>(v1 + 0x0000003Cu);
 v10 = gabi::load<u32>(v9 + 0x00000014u);
 count = v10;
 v3 = v1 + 0x00000038u;
 v3=gabi::call_ptr<u32>(count,v3);
 v5 = gabi::load<u32>(v1 + 0x00000038u);
 v11 = gabi::load<u8>(v5 + 0x00000000u);
 v30 = 0u + 0x00000000u;
 less=((int32_t)v11)<(0); greater=((int32_t)v11)>(0); equal=((int32_t)v11)==(0);
 v6 = 0u + 0x00040000u;
 if (equal) { goto L_025ACC04; }
 L_025ACBE0: ;
 v30 = v30 + 0x00000001u;
 less=((int32_t)v30)<((int32_t)v6); greater=((int32_t)v30)>((int32_t)v6); equal=((int32_t)v30)==((int32_t)v6);
 v5 = v5 + 0x00000001u;
 if (greater) { goto L_025ACC00; }
 v12 = gabi::load<u8>(v5 + 0x00000000u);
 less=((int32_t)v12)<(0); greater=((int32_t)v12)>(0); equal=((int32_t)v12)==(0);
 if (!equal) { goto L_025ACBE0; }
 goto L_025ACC04;
 L_025ACC00: ;
 v30 = 0u + 0x00000000u;
 L_025ACC04: ;
 v31 = v31 - v30; less=(s32)v31<0; greater=(s32)v31>0; equal=v31==0;
 v23 = 0u + 0x00000000u;
 if (less) { goto L_025ACCC8; }
 L_025ACC10: ;
 v4 = gabi::load<u32>(v1 + 0x00000030u);
 v22 = v30;
 gabi::store<u32>(v1 + 0x00000014u, v25);
 v0 = v4 + v23;
 v3 = v1 + 0x00000010u;
 gabi::store<u32>(v1 + 0x00000010u, v0);
 v3=gabi::call<u32>(0x025AD9B4);
 v5 = gabi::load<u32>(v1 + 0x00000014u);
 v6 = gabi::load<u32>(v5 + 0x00000014u);
 count = v6;
 v3 = v1 + 0x00000010u;
 v3=gabi::call_ptr<u32>(count,v3);
 v7 = gabi::load<u32>(v1 + 0x0000003Cu);
 v8 = gabi::load<u32>(v7 + 0x00000014u);
 count = v8;
 v21 = gabi::load<u32>(v1 + 0x00000010u);
 v3 = v1 + 0x00000038u;
 v3=gabi::call_ptr<u32>(count,v3);
 v6 = gabi::load<u32>(v1 + 0x00000038u);
 less=(v21)<(v6); greater=(v21)>(v6); equal=(v21)==(v6);
 if (equal) { goto L_025ACCB0; }
 less=((int32_t)v22)<(0); greater=((int32_t)v22)>(0); equal=((int32_t)v22)==(0);
 if (!greater) { goto L_025ACCB0; }
 count = v22;
 v11 = gabi::load<u32>(v1 + 0x00000010u);
 L_025ACC74: ;
 v9 = gabi::load<u8>(v11 + 0x00000000u);
 less=((int32_t)v9)<(0); greater=((int32_t)v9)>(0); equal=((int32_t)v9)==(0);
 if (!equal) { goto L_025ACC90; }
 v9 = gabi::load<u8>(v6 + 0x00000000u);
 less=((int32_t)v9)<(0); greater=((int32_t)v9)>(0); equal=((int32_t)v9)==(0);
 if (equal) { goto L_025ACCB0; }
 goto L_025ACCBC;
 L_025ACC90: ;
 v8 = gabi::load<u8>(v6 + 0x00000000u);
 less=((int32_t)v8)<(0); greater=((int32_t)v8)>(0); equal=((int32_t)v8)==(0);
 if (equal) { goto L_025ACCBC; }
 less=(v8)<(v9); greater=(v8)>(v9); equal=(v8)==(v9);
 if (!equal) { goto L_025ACCBC; }
 v6 = v6 + 0x00000001u;
 v11 = v11 + 0x00000001u;
 count--; if (count != 0) { goto L_025ACC74; }
 L_025ACCB0: ;
 less=((int32_t)v23)<(-1); greater=((int32_t)v23)>(-1); equal=((int32_t)v23)==(-1);
 if (equal) { goto L_025ACCC8; }
 goto L_025ACFBC;
 L_025ACCBC: ;
 v23 = v23 + 0x00000001u;
 less=((int32_t)v23)<((int32_t)v31); greater=((int32_t)v23)>((int32_t)v31); equal=((int32_t)v23)==((int32_t)v31);
 if (!greater) { goto L_025ACC10; }
 L_025ACCC8: ;
 v0 = 0u + 0x10050000u;
 gabi::store<u32>(v1 + 0x00000044u, v25);
 { uint64_t t = (uint64_t)v0 + 0x00002930u; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
 v11 = gabi::load<u32>(v1 + 0x00000034u);
 gabi::store<u32>(v1 + 0x00000040u, v0);
 v12 = gabi::load<u32>(v11 + 0x00000014u);
 count = v12;
 v0 = 0u + 0x00000001u;
 v3 = v1 + 0x00000030u;
 v3=gabi::call_ptr<u32>(count,v3);
 v9 = gabi::load<u32>(v1 + 0x00000030u);
 v4 = gabi::load<u8>(v9 + 0x00000000u);
 v30 = 0u + 0x00000000u;
 less=((int32_t)v4)<(0); greater=((int32_t)v4)>(0); equal=((int32_t)v4)==(0);
 v0 = 0u + 0x00040000u;
 if (equal) { goto L_025ACD2C; }
 L_025ACD08: ;
 v30 = v30 + 0x00000001u;
 less=((int32_t)v30)<((int32_t)v0); greater=((int32_t)v30)>((int32_t)v0); equal=((int32_t)v30)==((int32_t)v0);
 v9 = v9 + 0x00000001u;
 if (greater) { goto L_025ACD28; }
 v5 = gabi::load<u8>(v9 + 0x00000000u);
 less=((int32_t)v5)<(0); greater=((int32_t)v5)>(0); equal=((int32_t)v5)==(0);
 if (!equal) { goto L_025ACD08; }
 goto L_025ACD2C;
 L_025ACD28: ;
 v30 = 0u + 0x00000000u;
 L_025ACD2C: ;
 v6 = gabi::load<u32>(v1 + 0x00000044u);
 v7 = gabi::load<u32>(v6 + 0x00000014u);
 count = v7;
 v3 = v1 + 0x00000040u;
 v3=gabi::call_ptr<u32>(count,v3);
 v7 = gabi::load<u32>(v1 + 0x00000040u);
 v8 = gabi::load<u8>(v7 + 0x00000000u);
 v31 = 0u + 0x00000000u;
 less=((int32_t)v8)<(0); greater=((int32_t)v8)>(0); equal=((int32_t)v8)==(0);
 v8 = 0u + 0x00040000u;
 if (equal) { goto L_025ACD7C; }
 L_025ACD58: ;
 v31 = v31 + 0x00000001u;
 less=((int32_t)v31)<((int32_t)v8); greater=((int32_t)v31)>((int32_t)v8); equal=((int32_t)v31)==((int32_t)v8);
 v7 = v7 + 0x00000001u;
 if (greater) { goto L_025ACD78; }
 v9 = gabi::load<u8>(v7 + 0x00000000u);
 less=((int32_t)v9)<(0); greater=((int32_t)v9)>(0); equal=((int32_t)v9)==(0);
 if (!equal) { goto L_025ACD58; }
 goto L_025ACD7C;
 L_025ACD78: ;
 v31 = 0u + 0x00000000u;
 L_025ACD7C: ;
 v30 = v30 - v31; less=(s32)v30<0; greater=(s32)v30>0; equal=v30==0;
 v23 = 0u + 0x00000000u;
 if (less) { goto L_025ACE40; }
 L_025ACD88: ;
 v11 = gabi::load<u32>(v1 + 0x00000030u);
 v22 = v31;
 gabi::store<u32>(v1 + 0x0000001Cu, v25);
 v0 = v11 + v23;
 v3 = v1 + 0x00000018u;
 gabi::store<u32>(v1 + 0x00000018u, v0);
 v3=gabi::call<u32>(0x025AD9B4);
 v12 = gabi::load<u32>(v1 + 0x0000001Cu);
 v4 = gabi::load<u32>(v12 + 0x00000014u);
 count = v4;
 v3 = v1 + 0x00000018u;
 v3=gabi::call_ptr<u32>(count,v3);
 v5 = gabi::load<u32>(v1 + 0x00000044u);
 v6 = gabi::load<u32>(v5 + 0x00000014u);
 count = v6;
 v21 = gabi::load<u32>(v1 + 0x00000018u);
 v3 = v1 + 0x00000040u;
 v3=gabi::call_ptr<u32>(count,v3);
 v5 = gabi::load<u32>(v1 + 0x00000040u);
 less=(v21)<(v5); greater=(v21)>(v5); equal=(v21)==(v5);
 if (equal) { goto L_025ACE28; }
 less=((int32_t)v22)<(0); greater=((int32_t)v22)>(0); equal=((int32_t)v22)==(0);
 if (!greater) { goto L_025ACE28; }
 count = v22;
 v6 = gabi::load<u32>(v1 + 0x00000018u);
 L_025ACDEC: ;
 v11 = gabi::load<u8>(v6 + 0x00000000u);
 less=((int32_t)v11)<(0); greater=((int32_t)v11)>(0); equal=((int32_t)v11)==(0);
 if (!equal) { goto L_025ACE08; }
 v7 = gabi::load<u8>(v5 + 0x00000000u);
 less=((int32_t)v7)<(0); greater=((int32_t)v7)>(0); equal=((int32_t)v7)==(0);
 if (equal) { goto L_025ACE28; }
 goto L_025ACE34;
 L_025ACE08: ;
 v10 = gabi::load<u8>(v5 + 0x00000000u);
 less=((int32_t)v10)<(0); greater=((int32_t)v10)>(0); equal=((int32_t)v10)==(0);
 if (equal) { goto L_025ACE34; }
 less=(v10)<(v11); greater=(v10)>(v11); equal=(v10)==(v11);
 if (!equal) { goto L_025ACE34; }
 v5 = v5 + 0x00000001u;
 v6 = v6 + 0x00000001u;
 count--; if (count != 0) { goto L_025ACDEC; }
 L_025ACE28: ;
 less=((int32_t)v23)<(-1); greater=((int32_t)v23)>(-1); equal=((int32_t)v23)==(-1);
 if (equal) { goto L_025ACE40; }
 goto L_025ACFBC;
 L_025ACE34: ;
 v23 = v23 + 0x00000001u;
 less=((int32_t)v23)<((int32_t)v30); greater=((int32_t)v23)>((int32_t)v30); equal=((int32_t)v23)==((int32_t)v30);
 if (!greater) { goto L_025ACD88; }
 L_025ACE40: ;
 v0 = 0u + 0x10050000u;
 gabi::store<u32>(v1 + 0x0000000Cu, v25);
 { uint64_t t = (uint64_t)v0 + 0x0000293Cu; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
 v9 = gabi::load<u32>(v1 + 0x00000034u);
 gabi::store<u32>(v1 + 0x00000008u, v0);
 v10 = gabi::load<u32>(v9 + 0x00000014u);
 count = v10;
 v0 = 0u + 0x00000001u;
 v3 = v1 + 0x00000030u;
 v3=gabi::call_ptr<u32>(count,v3);
 v12 = gabi::load<u32>(v1 + 0x00000030u);
 v11 = gabi::load<u8>(v12 + 0x00000000u);
 v30 = 0u + 0x00000000u;
 less=((int32_t)v11)<(0); greater=((int32_t)v11)>(0); equal=((int32_t)v11)==(0);
 v4 = 0u + 0x00040000u;
 if (equal) { goto L_025ACEA4; }
 L_025ACE80: ;
 v30 = v30 + 0x00000001u;
 less=((int32_t)v30)<((int32_t)v4); greater=((int32_t)v30)>((int32_t)v4); equal=((int32_t)v30)==((int32_t)v4);
 v12 = v12 + 0x00000001u;
 if (greater) { goto L_025ACEA0; }
 v0 = gabi::load<u8>(v12 + 0x00000000u);
 less=((int32_t)v0)<(0); greater=((int32_t)v0)>(0); equal=((int32_t)v0)==(0);
 if (!equal) { goto L_025ACE80; }
 goto L_025ACEA4;
 L_025ACEA0: ;
 v30 = 0u + 0x00000000u;
 L_025ACEA4: ;
 v5 = gabi::load<u32>(v1 + 0x0000000Cu);
 v6 = gabi::load<u32>(v5 + 0x00000014u);
 count = v6;
 v3 = v1 + 0x00000008u;
 v3=gabi::call_ptr<u32>(count,v3);
 v9 = gabi::load<u32>(v1 + 0x00000008u);
 v7 = gabi::load<u8>(v9 + 0x00000000u);
 v31 = 0u + 0x00000000u;
 less=((int32_t)v7)<(0); greater=((int32_t)v7)>(0); equal=((int32_t)v7)==(0);
 v0 = 0u + 0x00040000u;
 if (equal) { goto L_025ACEF4; }
 L_025ACED0: ;
 v31 = v31 + 0x00000001u;
 less=((int32_t)v31)<((int32_t)v0); greater=((int32_t)v31)>((int32_t)v0); equal=((int32_t)v31)==((int32_t)v0);
 v9 = v9 + 0x00000001u;
 if (greater) { goto L_025ACEF0; }
 v8 = gabi::load<u8>(v9 + 0x00000000u);
 less=((int32_t)v8)<(0); greater=((int32_t)v8)>(0); equal=((int32_t)v8)==(0);
 if (!equal) { goto L_025ACED0; }
 goto L_025ACEF4;
 L_025ACEF0: ;
 v31 = 0u + 0x00000000u;
 L_025ACEF4: ;
 v30 = v30 - v31; less=(s32)v30<0; greater=(s32)v30>0; equal=v30==0;
 v23 = 0u + 0x00000000u;
 if (less) { goto L_025ACFB8; }
 L_025ACF00: ;
 v11 = gabi::load<u32>(v1 + 0x00000030u);
 v22 = v31;
 gabi::store<u32>(v1 + 0x00000024u, v25);
 v0 = v11 + v23;
 v3 = v1 + 0x00000020u;
 gabi::store<u32>(v1 + 0x00000020u, v0);
 v3=gabi::call<u32>(0x025AD9B4);
 v12 = gabi::load<u32>(v1 + 0x00000024u);
 v4 = gabi::load<u32>(v12 + 0x00000014u);
 count = v4;
 v3 = v1 + 0x00000020u;
 v3=gabi::call_ptr<u32>(count,v3);
 v5 = gabi::load<u32>(v1 + 0x0000000Cu);
 v6 = gabi::load<u32>(v5 + 0x00000014u);
 count = v6;
 v21 = gabi::load<u32>(v1 + 0x00000020u);
 v3 = v1 + 0x00000008u;
 v3=gabi::call_ptr<u32>(count,v3);
 v4 = gabi::load<u32>(v1 + 0x00000008u);
 less=(v21)<(v4); greater=(v21)>(v4); equal=(v21)==(v4);
 if (equal) { goto L_025ACFA0; }
 less=((int32_t)v22)<(0); greater=((int32_t)v22)>(0); equal=((int32_t)v22)==(0);
 if (!greater) { goto L_025ACFA0; }
 count = v22;
 v6 = gabi::load<u32>(v1 + 0x00000020u);
 L_025ACF64: ;
 v5 = gabi::load<u8>(v6 + 0x00000000u);
 less=((int32_t)v5)<(0); greater=((int32_t)v5)>(0); equal=((int32_t)v5)==(0);
 if (!equal) { goto L_025ACF80; }
 v7 = gabi::load<u8>(v4 + 0x00000000u);
 less=((int32_t)v7)<(0); greater=((int32_t)v7)>(0); equal=((int32_t)v7)==(0);
 if (equal) { goto L_025ACFA0; }
 goto L_025ACFAC;
 L_025ACF80: ;
 v0 = gabi::load<u8>(v4 + 0x00000000u);
 less=((int32_t)v0)<(0); greater=((int32_t)v0)>(0); equal=((int32_t)v0)==(0);
 if (equal) { goto L_025ACFAC; }
 less=(v0)<(v5); greater=(v0)>(v5); equal=(v0)==(v5);
 if (!equal) { goto L_025ACFAC; }
 v4 = v4 + 0x00000001u;
 v6 = v6 + 0x00000001u;
 count--; if (count != 0) { goto L_025ACF64; }
 L_025ACFA0: ;
 less=((int32_t)v23)<(-1); greater=((int32_t)v23)>(-1); equal=((int32_t)v23)==(-1);
 if (equal) { goto L_025ACFB8; }
 goto L_025ACFBC;
 L_025ACFAC: ;
 v23 = v23 + 0x00000001u;
 less=((int32_t)v23)<((int32_t)v30); greater=((int32_t)v23)>((int32_t)v30); equal=((int32_t)v23)==((int32_t)v30);
 if (!greater) { goto L_025ACF00; }
 L_025ACFB8: ;
 v24 = 0u + 0x00000000u;
 L_025ACFBC: ;
 v11 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 v4 = 0u + 0x00002D01u;
 less=((int32_t)v24)<(0); greater=((int32_t)v24)>(0); equal=((int32_t)v24)==(0);
 v3 = v11 + 0x00000644u;
 if (equal) { goto L_025ACFE8; }
 v3=gabi::call<u32>(0x025B8B7C ,v3,v4);
 v0 = 0u + 0x10050000u;
 gabi::store<u32>(v1 + 0x0000000Cu, v25);
 { uint64_t t = (uint64_t)v0 + 0x00002948u; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
 gabi::store<u32>(v1 + 0x00000008u, v0);
 goto L_025ACFFC;
 L_025ACFE8: ;
 v3=gabi::call<u32>(0x025B8B68 ,v3,v4);
 v0 = 0u + 0x10050000u;
 gabi::store<u32>(v1 + 0x0000000Cu, v25);
 { uint64_t t = (uint64_t)v0 + 0x00002948u; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
 gabi::store<u32>(v1 + 0x00000008u, v0);
 L_025ACFFC: ;
 v6 = gabi::load<u32>(v1 + 0x00000034u);
 v7 = gabi::load<u32>(v6 + 0x00000014u);
 count = v7;
 v3 = v1 + 0x00000030u;
 v3=gabi::call_ptr<u32>(count,v3);
 v11 = gabi::load<u32>(v1 + 0x00000030u);
 v8 = gabi::load<u8>(v11 + 0x00000000u);
 v31 = 0u + 0x00000000u;
 less=((int32_t)v8)<(0); greater=((int32_t)v8)>(0); equal=((int32_t)v8)==(0);
 v0 = 0u + 0x00040000u;
 if (equal) { goto L_025AD04C; }
 L_025AD028: ;
 v31 = v31 + 0x00000001u;
 less=((int32_t)v31)<((int32_t)v0); greater=((int32_t)v31)>((int32_t)v0); equal=((int32_t)v31)==((int32_t)v0);
 v11 = v11 + 0x00000001u;
 if (greater) { goto L_025AD048; }
 v9 = gabi::load<u8>(v11 + 0x00000000u);
 less=((int32_t)v9)<(0); greater=((int32_t)v9)>(0); equal=((int32_t)v9)==(0);
 if (!equal) { goto L_025AD028; }
 goto L_025AD04C;
 L_025AD048: ;
 v31 = 0u + 0x00000000u;
 L_025AD04C: ;
 v10 = gabi::load<u32>(v1 + 0x0000000Cu);
 v0 = gabi::load<u32>(v10 + 0x00000014u);
 count = v0;
 v3 = v1 + 0x00000008u;
 v3=gabi::call_ptr<u32>(count,v3);
 v8 = gabi::load<u32>(v1 + 0x00000008u);
 v4 = gabi::load<u8>(v8 + 0x00000000u);
 v30 = 0u + 0x00000000u;
 less=((int32_t)v4)<(0); greater=((int32_t)v4)>(0); equal=((int32_t)v4)==(0);
 v9 = 0u + 0x00040000u;
 if (equal) { goto L_025AD09C; }
 L_025AD078: ;
 v30 = v30 + 0x00000001u;
 less=((int32_t)v30)<((int32_t)v9); greater=((int32_t)v30)>((int32_t)v9); equal=((int32_t)v30)==((int32_t)v9);
 v8 = v8 + 0x00000001u;
 if (greater) { goto L_025AD098; }
 v5 = gabi::load<u8>(v8 + 0x00000000u);
 less=((int32_t)v5)<(0); greater=((int32_t)v5)>(0); equal=((int32_t)v5)==(0);
 if (!equal) { goto L_025AD078; }
 goto L_025AD09C;
 L_025AD098: ;
 v30 = 0u + 0x00000000u;
 L_025AD09C: ;
 v31 = v31 - v30; less=(s32)v31<0; greater=(s32)v31>0; equal=v31==0;
 v24 = 0u + 0x00000000u;
 if (less) { goto L_025AD168; }
 L_025AD0A8: ;
 v7 = gabi::load<u32>(v1 + 0x00000030u);
 gabi::store<u32>(v1 + 0x0000002Cu, v25);
 v6 = v7 + v24;
 v3 = v1 + 0x00000028u;
 gabi::store<u32>(v1 + 0x00000028u, v6);
 v3=gabi::call<u32>(0x025AD9B4);
 v8 = gabi::load<u32>(v1 + 0x0000002Cu);
 v9 = gabi::load<u32>(v8 + 0x00000014u);
 count = v9;
 v3 = v1 + 0x00000028u;
 v3=gabi::call_ptr<u32>(count,v3);
 v10 = gabi::load<u32>(v1 + 0x0000000Cu);
 v11 = gabi::load<u32>(v10 + 0x00000014u);
 count = v11;
 v23 = gabi::load<u32>(v1 + 0x00000028u);
 v3 = v1 + 0x00000008u;
 v3=gabi::call_ptr<u32>(count,v3);
 v0 = gabi::load<u32>(v1 + 0x00000008u);
 less=(v23)<(v0); greater=(v23)>(v0); equal=(v23)==(v0);
 if (equal) { goto L_025AD148; }
 v0 = v30; less=(s32)v0<0; greater=(s32)v0>0; equal=v0==0;
 if (!greater) { goto L_025AD148; }
 v12 = gabi::load<u32>(v1 + 0x00000008u);
 count = v0;
 v4 = gabi::load<u32>(v1 + 0x00000028u);
 L_025AD10C: ;
 v8 = gabi::load<u8>(v4 + 0x00000000u);
 less=((int32_t)v8)<(0); greater=((int32_t)v8)>(0); equal=((int32_t)v8)==(0);
 if (!equal) { goto L_025AD128; }
 v5 = gabi::load<u8>(v12 + 0x00000000u);
 less=((int32_t)v5)<(0); greater=((int32_t)v5)>(0); equal=((int32_t)v5)==(0);
 if (equal) { goto L_025AD148; }
 goto L_025AD15C;
 L_025AD128: ;
 v6 = gabi::load<u8>(v12 + 0x00000000u);
 less=((int32_t)v6)<(0); greater=((int32_t)v6)>(0); equal=((int32_t)v6)==(0);
 if (equal) { goto L_025AD15C; }
 less=(v6)<(v8); greater=(v6)>(v8); equal=(v6)==(v8);
 if (!equal) { goto L_025AD15C; }
 v12 = v12 + 0x00000001u;
 v4 = v4 + 0x00000001u;
 count--; if (count != 0) { goto L_025AD10C; }
 L_025AD148: ;
 v6 = ~(v24 | v24);
 { uint64_t t = (uint64_t)v6 + 0xFFFFFFFFu; v7 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
 { uint64_t t = (uint64_t)(uint32_t)~v7 + v6 + carry; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); } less=(s32)v0<0; greater=(s32)v0>0; equal=v0==0;
 if (equal) { goto L_025AD188; }
 goto L_025AD178;
 L_025AD15C: ;
 v24 = v24 + 0x00000001u;
 less=((int32_t)v24)<((int32_t)v31); greater=((int32_t)v24)>((int32_t)v31); equal=((int32_t)v24)==((int32_t)v31);
 if (!greater) { goto L_025AD0A8; }
 L_025AD168: ;
 v6 = 0u + 0x00000000u;
 { uint64_t t = (uint64_t)v6 + 0xFFFFFFFFu; v7 = (uint32_t)t; carry = (uint8_t)(t >> 32); }
 { uint64_t t = (uint64_t)(uint32_t)~v7 + carry; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); } less=(s32)v0<0; greater=(s32)v0>0; equal=v0==0;
 if (equal) { goto L_025AD188; }
 L_025AD178: ;
 v10 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 v4 = 0u + 0x00001E40u;
 v3 = v10 + 0x00000644u;
 v3=gabi::call<u32>(0x025B8B68 ,v3,v4);
 L_025AD188: ;
 v5 = gabi::load<u32>(v27 + 0xFFFFA6C4u);
 v8 = 0u + 0x101F0000u;
 v8 = gabi::load<u32>(v8 + 0xFFFFA6CCu);
 v12 = ((u32)v5 * 40u);
 v8 = gabi::load<u8>(v8 + v5);
 v5 = gabi::load<u32>(v26 + 0x00000004u);
 v8 = (uint32_t)(int32_t)(int8_t)v8;
 v4 = v5 + v12;
 v7 = ((u32)v8 * 44u);
 v10 = gabi::load<u32>(v4 + 0x00000024u);
 v31 = v10 + v7;
 v3=gabi::call<u32>(0x025200D4);
 v5 = 0u + 0x00000000u;
 gabi::store<u8>(v3 + 0x0000514Cu, v5);
 v11 = gabi::load<u8>(v29 + 0x000001E4u);
 less=((int32_t)v11)<(0); greater=((int32_t)v11)>(0); equal=((int32_t)v11)==(0);
 if (equal) { goto L_025ACA80; }
 L_025AD1CC: ;
 v5 = gabi::load<u8>(v31 + 0x00000029u);
 v7 = 0u + 0x10050000u;
 v3 = v31 + 0x00000021u;
 v0 = v11 + 0xFFFFFFFFu;
 q1=gabi::load<f32>(v7 + 0x000028DCu);
 v7 = 0u + 0x00000000u;
 v5 = (uint32_t)(int32_t)(int8_t)v5;
 v6 = gabi::load<u8>(v31 + 0x0000002Bu);
 v8 = 0u + 0x00000001u;
 v9 = v7;
 v4 = (uint32_t)(int32_t)(int16_t)v0;
 v6 = (uint32_t)(int32_t)(int8_t)v6;
 v3=gabi::call<u32>(0x0252012C ,v3,v4,v5,v6,v7,v8,v9,q1);
 v5 = 0u + 0x00000000u;
 v4 = 0u + 0x00000007u;
 v3 = v29;
 v7 = v5;
 v6 = 0u + 0x00000005u;
 v3=gabi::call<u32>(0x025DC86C ,v3,v4,v5,v6,v7);
 v12 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 v4 = 0u + 0x00000000u;
 gabi::store<u32>(v12 + 0x0000116Cu, v4);
 v3=gabi::call<u32>(0x025200D4);
 v30 = v3 + 0x00005140u;
 v3=gabi::call<u32>(0x025200D4);
 v31 = gabi::load<u8>(v3 + 0x0000514Au);
 v31 = (uint32_t)(int32_t)(int8_t)v31;
 v3=gabi::call<u32>(0x025200D4);
 v7 = v3 + 0x000012A0u;
 v5 = gabi::load<u8>(v7 + 0x00003EABu);
 v4 = v31;
 v3 = v30;
 v5 = (uint32_t)(int32_t)(int8_t)v5;
 v3=gabi::call<u32>(0x025E17CC ,v3,v4,v5);
 L_025AD254: ;
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x02007914 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 v30 = 0u + 0x101F0000u;
 if (equal) { goto L_025AD280; }
 v8 = gabi::load<u32>(v30 + 0xFFFFA6C8u);
 { uint64_t t = (uint64_t)v8 + 0xFFFFFFFFu; v0 = (uint32_t)t; carry = (uint8_t)(t >> 32); } less=(s32)v0<0; greater=(s32)v0>0; equal=v0==0;
 if (!less) { goto L_025AD2A0; }
 v8 = 0u + 0x0000001Du;
 gabi::store<u32>(v30 + 0xFFFFA6C8u, v8);
 goto L_025AD2C8;
 L_025AD280: ;
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x020078E8 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (equal) { goto L_025AD2C8; }
 v11 = gabi::load<u32>(v30 + 0xFFFFA6C8u);
 v0 = v11 + 0x00000001u;
 less=((int32_t)v0)<(29); greater=((int32_t)v0)>(29); equal=((int32_t)v0)==(29);
 if (greater) { goto L_025AD2C0; }
 L_025AD2A0: ;
 v3 = 0u + 0x00000000u;
 gabi::store<u32>(v30 + 0xFFFFA6C8u, v0);
 v3=gabi::call<u32>(0x020077BC ,v3);
 v31 = 0u + 0x101F0000u;
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 v31 = v31 + 0xFFFFA6D0u;
 if (equal) { goto L_025AD2FC; }
 goto L_025AD2E0;
 L_025AD2C0: ;
 v8 = 0u + 0x00000000u;
 gabi::store<u32>(v30 + 0xFFFFA6C8u, v8);
 L_025AD2C8: ;
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x020077BC ,v3);
 v31 = 0u + 0x101F0000u;
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 v31 = v31 + 0xFFFFA6D0u;
 if (equal) { goto L_025AD2FC; }
 L_025AD2E0: ;
 v4 = (uint32_t)(int32_t)(int16_t)gabi::load<u16>(v31 + 0x00000000u);
 v0 = v4 + 0xFFFFFFFFu;
 v0 = (uint32_t)(int32_t)(int16_t)v0; less=(s32)v0<0; greater=(s32)v0>0; equal=v0==0;
 if (!less) { goto L_025AD354; }
 v0 = 0u + 0x00000006u;
 gabi::store<u16>(v31 + 0x00000000u, v0);
 goto L_025AD390;
 L_025AD2FC: ;
 v3 = 0u + 0x00000000u;
 v3=gabi::call<u32>(0x020077E8 ,v3);
 less=((int32_t)v3)<(0); greater=((int32_t)v3)>(0); equal=((int32_t)v3)==(0);
 if (!equal) { goto L_025AD340; }
 v0 = (uint32_t)(int32_t)(int16_t)gabi::load<u16>(v31 + 0x00000000u);
 v7 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 gabi::store<u16>(v7 + 0x00000048u, v0);
 v3=gabi::call<u32>(0x02555D0C);
 v12 = 0u + 0x10050000u;
 v11 = 0u + 0x10050000u;
 q0=gabi::load<f32>(v12 + 0x000028DCu);
 q31=gabi::load<f32>(v11 + 0x000028E0u);
 { uint32_t ea = v3 + 0x00001028u; gabi::store<f32>(ea, q0); }
 v11 = gabi::load<u32>(v30 + 0xFFFFA6C8u);
 less=((int32_t)v11)<(6); greater=((int32_t)v11)>(6); equal=((int32_t)v11)==(6);
 if (less) { goto L_025AD404; }
 goto L_025AD3BC;
 L_025AD340: ;
 v6 = (uint32_t)(int32_t)(int16_t)gabi::load<u16>(v31 + 0x00000000u);
 v0 = v6 + 0x00000001u;
 v0 = (uint32_t)(int32_t)(int16_t)v0;
 less=((int32_t)v0)<(7); greater=((int32_t)v0)>(7); equal=((int32_t)v0)==(7);
 if (!less) { goto L_025AD388; }
 L_025AD354: ;
 v7 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 gabi::store<u16>(v31 + 0x00000000u, v0);
 gabi::store<u16>(v7 + 0x00000048u, v0);
 v3=gabi::call<u32>(0x02555D0C);
 v12 = 0u + 0x10050000u;
 v11 = 0u + 0x10050000u;
 q0=gabi::load<f32>(v12 + 0x000028DCu);
 q31=gabi::load<f32>(v11 + 0x000028E0u);
 { uint32_t ea = v3 + 0x00001028u; gabi::store<f32>(ea, q0); }
 v11 = gabi::load<u32>(v30 + 0xFFFFA6C8u);
 less=((int32_t)v11)<(6); greater=((int32_t)v11)>(6); equal=((int32_t)v11)==(6);
 if (less) { goto L_025AD404; }
 goto L_025AD3BC;
 L_025AD388: ;
 v0 = 0u + 0x00000000u;
 gabi::store<u16>(v31 + 0x00000000u, v0);
 L_025AD390: ;
 v7 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 gabi::store<u16>(v7 + 0x00000048u, v0);
 v3=gabi::call<u32>(0x02555D0C);
 v12 = 0u + 0x10050000u;
 v11 = 0u + 0x10050000u;
 q0=gabi::load<f32>(v12 + 0x000028DCu);
 q31=gabi::load<f32>(v11 + 0x000028E0u);
 { uint32_t ea = v3 + 0x00001028u; gabi::store<f32>(ea, q0); }
 v11 = gabi::load<u32>(v30 + 0xFFFFA6C8u);
 less=((int32_t)v11)<(6); greater=((int32_t)v11)>(6); equal=((int32_t)v11)==(6);
 if (less) { goto L_025AD404; }
 L_025AD3BC: ;
 v9 = 0u + 0x10050000u;
 v12 = v11 + 0xFFFFFFFAu;
 v7 = 0u + 0x43300000u;
 { uint32_t ea = v9 + 0x000028E8u; q11 = gabi::load<f64>(ea); }
 v9 = v12 ^ 0x80000000u;
 gabi::store<u32>(v1 + 0x00000058u, v7);
 gabi::store<u32>(v1 + 0x0000005Cu, v9);
 { uint32_t ea = v1 + 0x00000058u; q7 = gabi::load<f64>(ea); }
 q13 = q7 - q11;
 v6 = 0u + 0x10050000u;
 q6=to_single(q13);
 q9=gabi::load<f32>(v6 + 0x000028F0u);
 q0=to_single(q6 * round25(q9));
 v12 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 { uint32_t ea = v12 + 0x00000044u; gabi::store<f32>(ea, q0); }
 v3=gabi::call<u32>(0x02555D0C);
 { uint32_t ea = v3 + 0x00001028u; gabi::store<f32>(ea, q31); }
 goto L_025AD524;
 L_025AD404: ;
 less=(v11)<(0x0003u); greater=(v11)>(0x0003u); equal=(v11)==(0x0003u);
 if (!less) { goto L_025AD440; }
 less=(v11)<(0x0001u); greater=(v11)>(0x0001u); equal=(v11)==(0x0001u);
 if (less) { goto L_025AD45C; }
 if (equal) { goto L_025AD480; }
 v31 = 0u + 0x10050000u;
 v8 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 q10=gabi::load<f32>(v31 + 0x00002900u);
 { uint32_t ea = v8 + 0x00000044u; gabi::store<f32>(ea, q10); }
 v3=gabi::call<u32>(0x02555D0C);
 q8=gabi::load<f32>(v31 + 0x00002900u);
 { uint32_t ea = v3 + 0x00001020u; gabi::store<f32>(ea, q8); }
 v3=gabi::call<u32>(0x02555D0C);
 { uint32_t ea = v3 + 0x00001028u; gabi::store<f32>(ea, q31); }
 goto L_025AD524;
 L_025AD440: ;
 less=(v11)<(0x0004u); greater=(v11)>(0x0004u); equal=(v11)==(0x0004u);
 if (less) { goto L_025AD4A4; }
 if (equal) { goto L_025AD4D0; }
 less=(v11)<(0x0005u); greater=(v11)>(0x0005u); equal=(v11)==(0x0005u);
 if (equal) { goto L_025AD4FC; }
 v3 = 0u + 0x00000001u;
 goto L_025AD528;
 L_025AD45C: ;
 v4 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 v9 = 0u + 0x10050000u;
 q12=gabi::load<f32>(v9 + 0x000028F4u);
 v8 = 0u + 0x10050000u;
 q31=gabi::load<f32>(v8 + 0x000028F8u);
 { uint32_t ea = v4 + 0x00000044u; gabi::store<f32>(ea, q12); }
 v3=gabi::call<u32>(0x02555D0C);
 { uint32_t ea = v3 + 0x00001028u; gabi::store<f32>(ea, q31); }
 goto L_025AD524;
 L_025AD480: ;
 v5 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 v11 = 0u + 0x10050000u;
 q13=gabi::load<f32>(v11 + 0x000028F4u);
 v10 = 0u + 0x10050000u;
 q31=gabi::load<f32>(v10 + 0x000028FCu);
 { uint32_t ea = v5 + 0x00000044u; gabi::store<f32>(ea, q13); }
 v3=gabi::call<u32>(0x02555D0C);
 { uint32_t ea = v3 + 0x00001028u; gabi::store<f32>(ea, q31); }
 goto L_025AD524;
 L_025AD4A4: ;
 v11 = 0u + 0x10050000u;
 v10 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 q30=gabi::load<f32>(v11 + 0x00002904u);
 { uint32_t ea = v10 + 0x00000044u; gabi::store<f32>(ea, q30); }
 v3=gabi::call<u32>(0x02555D0C);
 v10 = 0u + 0x10050000u;
 q31=gabi::load<f32>(v10 + 0x00002908u);
 { uint32_t ea = v3 + 0x00001020u; gabi::store<f32>(ea, q30); }
 v3=gabi::call<u32>(0x02555D0C);
 { uint32_t ea = v3 + 0x00001028u; gabi::store<f32>(ea, q31); }
 goto L_025AD524;
 L_025AD4D0: ;
 v5 = 0u + 0x10050000u;
 v11 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 q30=gabi::load<f32>(v5 + 0x0000290Cu);
 { uint32_t ea = v11 + 0x00000044u; gabi::store<f32>(ea, q30); }
 v3=gabi::call<u32>(0x02555D0C);
 v4 = 0u + 0x10050000u;
 q31=gabi::load<f32>(v4 + 0x00002910u);
 { uint32_t ea = v3 + 0x00001020u; gabi::store<f32>(ea, q30); }
 v3=gabi::call<u32>(0x02555D0C);
 { uint32_t ea = v3 + 0x00001028u; gabi::store<f32>(ea, q31); }
 goto L_025AD524;
 L_025AD4FC: ;
 v7 = 0u + 0x10050000u;
 v12 = gabi::load<u32>(v28 + 0xFFFF84DCu);
 q30=gabi::load<f32>(v7 + 0x00002914u);
 { uint32_t ea = v12 + 0x00000044u; gabi::store<f32>(ea, q30); }
 v3=gabi::call<u32>(0x02555D0C);
 v6 = 0u + 0x10050000u;
 q31=gabi::load<f32>(v6 + 0x00002918u);
 { uint32_t ea = v3 + 0x00001020u; gabi::store<f32>(ea, q30); }
 v3=gabi::call<u32>(0x02555D0C);
 { uint32_t ea = v3 + 0x00001028u; gabi::store<f32>(ea, q31); }
 L_025AD524: ;
 v3 = 0u + 0x00000001u;
 L_025AD528: ;
 return (s32)v3;
}
VERIFY(0x025AC798,menu_execute);
s32 menu_draw(void* self) { WWHD_FUNC(0x025AC790,s32,self); return 1; }
VERIFY(0x025AC790,menu_draw);
s32 menu_isDelete(void* self) { WWHD_FUNC(0x025AD550,s32,self); return 1; }
VERIFY(0x025AD550,menu_isDelete);
s32 menu_delete(void* self) {
 WWHD_FUNC(0x025AD558,s32,self);
 u32 addr=gabi::ea(self),font=gabi::load<u32>(addr+0x1E0);
 if(font) {
  u32 table=gabi::load<u32>(font+0x18),target=gabi::load<u32>(table+0xC);
  gabi::call_ptr(target,gabi::at<void>(font),3);
 }
 gabi::call(0x027EC0B0,gabi::at<void>(gabi::load<u32>(addr+0x1D8)));
 gabi::call(0x027EC0B0,gabi::at<void>(gabi::load<u32>(addr+0x1DC)));
 u8 flag=gabi::load<u8>(0x104873CF)&0xFD;
 u32 save=gabi::load<u32>(0x101F84DC);
 gabi::store<u8>(0x104873CF,flag);
 gabi::call(0x025B9864,gabi::at<void>(save+0x1148),0);
 gabi::call(0x0259170C,0);
 return 1;
}
VERIFY(0x025AD558,menu_delete);
s32 menu_phase1(void* self) {
 WWHD_FUNC(0x025AD5E0,s32,self);
 u32 addr=gabi::ea(self);
 u32 command=gabi::call<u32>(0x025E255C,gabi::at<void>(0x10052954),0,0);
 gabi::store<u32>(addr+0x1D0,command);
 if(!command) gabi::call(0x0273AA24,gabi::at<void>(0x100529A4),0x308,gabi::at<void>(0x100529B4));
 u32 font=gabi::call<u32>(0x025E255C,gabi::at<void>(0x10052978),0,0);
 gabi::store<u32>(addr+0x1D4,font);
 if(!font) gabi::call(0x0273AA24,gabi::at<void>(0x100529A4),0x30B,gabi::at<void>(0x100529CC));
 return 2;
}
VERIFY(0x025AD5E0,menu_phase1);
s32 menu_phase2(void* self) {
 WWHD_FUNC(0x025AD680,s32,self);
 u32 addr=gabi::ea(self),command=gabi::load<u32>(addr+0x1D0);
 if(!gabi::load<u8>(command+0xC)) return 0;
 u32 fontCommand=gabi::load<u32>(addr+0x1D4);
 if(!gabi::load<u8>(fontCommand+0xC)) return 0;
 u32 info=gabi::load<u32>(command+0x118);
 gabi::store<u32>(addr+0x1D8,info);
 if(!info) gabi::call(0x0273AA24,gabi::at<void>(0x10052A10),0x338,gabi::at<void>(0x100529E8));
 command=gabi::load<u32>(addr+0x1D0);
 if(command) {
  u32 table=gabi::load<u32>(command+0x10);
  gabi::call_ptr(gabi::load<u32>(table+0xC),gabi::at<void>(command),3);
 }
 u32 endian=gabi::load<u32>(0x104A0CD4),slot=0x10147620+endian*4;
 info=gabi::load<u32>(addr+0x1D8);
 u32 offset=gabi::load<u32>(info+4);
 u32 decoded=gabi::call_ptr<u32>(gabi::load<u32>(slot),offset);
 u32 num=gabi::load<u8>(info);
 u32 stages=decoded+info;
 gabi::store<u32>(info+4,stages);
 if(num>0) {
  u32 target=gabi::load<u32>(slot),i=0;
  do {
   u32 room=gabi::load<u32>(stages+i*40+0x24);
   room=gabi::call_ptr<u32>(target,room);
   stages=gabi::load<u32>(info+4);
   gabi::store<u32>(stages+i*40+0x24,room+info);
   num=gabi::load<u8>(info);
   i++;
   if(i>=num) break;
   stages=gabi::load<u32>(info+4);
  } while(true);
 }
 if(!gabi::load<u32>(0x101EA6CC)) {
  u32 points=gabi::call<u32>(0x0273ADAC,num);
  gabi::store<u32>(0x101EA6CC,points);
  if(!points) gabi::call(0x0273AA24,gabi::at<void>(0x10052A10),0x353,gabi::at<void>(0x100529FC));
  for(u32 i=0;i<gabi::load<u8>(info);i++) {
   u32 p=gabi::load<u32>(0x101EA6CC);
   gabi::store<u8>(p+i,0);
  }
 }
 fontCommand=gabi::load<u32>(addr+0x1D4);
 u32 font=gabi::load<u32>(fontCommand+0x118);
 gabi::store<u32>(addr+0x1DC,font);
 if(fontCommand) {
  u32 table=gabi::load<u32>(fontCommand+0x10);
  gabi::call_ptr(gabi::load<u32>(table+0xC),gabi::at<void>(fontCommand),3);
  font=gabi::load<u32>(addr+0x1DC);
 }
 if(font) {
  u32 instance=gabi::load<u32>(addr+0x1E0);
  if(instance) gabi::call(0x027ECCC8,gabi::at<void>(gabi::load<u32>(0x101F97D4)),gabi::at<void>(instance));
 }
 u32 system=gabi::call<u32>(0xC0009C80u);
 u32 clock=gabi::load<u32>(system);
 u32 high=((u64)0x88888889u*(clock>>2))>>32;
 gabi::call(0x025F096C,high>>5);
 gabi::store<u32>(0x101F4818,gabi::load<u32>(0x101D5E90));
 u8 flags=gabi::load<u8>(0x104873CF);
 gabi::store<u8>(0x104873CF,flags|2);
 return 4;
}
VERIFY(0x025AD680,menu_phase2);
s32 menu_create(void* self) {
 WWHD_FUNC(0x025AD8AC,s32,self);
 u32 save=gabi::load<u32>(0x101F84DC);
 u32 option=gabi::call<u32>(0x027200D0,gabi::at<void>(save+0x12C0));
 gabi::store<u8>(0x101EA6D2,gabi::load<u8>(option+4));
 gabi::call(0x0259170C,1);
 return gabi::call<s32>(0x02525FE4,gabi::at<void>(gabi::ea(self)+0x1C8),gabi::at<void>(0x101EA6E8),self);
}
VERIFY(0x025AD8AC,menu_create);
void menu_initializer() {
 WWHD_FUNC(0x025AD90C,void);
 gabi::store<u32>(0x1047B3AC,0); gabi::store<u32>(0x1047B3A4,0);
 gabi::store<u32>(0x1047B3B0,0); gabi::store<u32>(0x1047B3A8,0);
 gabi::call(0x028F026C,gabi::at<void>(0x101EA6F0));
 f32 zero=gabi::load<f32>(0x10052A24),one=gabi::load<f32>(0x10052A28);
 gabi::store<f32>(0x1047B398,zero); gabi::store<f32>(0x1047B39C,one);
 gabi::call(0x028ED6F8,gabi::at<void>(0x1047B3A0));
 gabi::call(0x028F026C,gabi::at<void>(0x101EA6FC));
 gabi::call(0x028EAB2C,gabi::at<void>(0x1047B3A1));
 gabi::call(0x028F026C,gabi::at<void>(0x101EA708));
}
VERIFY(0x025AD90C,menu_initializer);

/* 025AD9A0 this TU's sead::SafeString copy (vtable 100528C4): deleting destructor of a trivially
 * destructible class: operator delete when this != NULL and bit 0 of the flags is set */
static void SafeString_deletingDtor_d_s_menu(u32 p, u32 flags) {
    WWHD_FUNC(0x025AD9A0, void, p, flags);
    if (p == 0) return;
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x025AD9A0, SafeString_deletingDtor_d_s_menu);

/* 025AD9B4 this TU's sead::SafeString copy assureTerminationImpl_ (vtable 100528C4; also called directly by dScnMenu_Execute): empty function */
static void SafeString_assureTerminationImpl_d_s_menu(u32 p) {
    WWHD_FUNC(0x025AD9B4, void, p);
}
VERIFY(0x025AD9B4, SafeString_assureTerminationImpl_d_s_menu);
