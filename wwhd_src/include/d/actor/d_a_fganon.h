/* fganon_class (Phantom Ganon), WWHD layout. */
#ifndef WWHD_D_A_FGANON_H
#define WWHD_D_A_FGANON_H
#include "bindings.h"

// Phantom Ganon's HD actor storage. Named members are added as their layout is checked.
struct fganon_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs1;
    /* 0x3D0 */ request_of_phase_process_class mPhs2;
    /* 0x3D8 */ be<u8> m2BC;
    /* 0x3D9 */ be<u8> m2BD;
    /* 0x3DA */ be<u8> mSwitchNo;
    /* 0x3DB */ be<u8> m2BF;
    /* 0x3DC */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3E0 */ gptr<mDoExt_baseAnm> mpBrkAnm1;
    /* 0x3E4 */ gptr<J3DModel> mpKenModel;
    /* 0x3E8 */ gptr<mDoExt_baseAnm> mpBrkAnm2;
    /* 0x3EC */ be<s8> m2D0;
    /* 0x3ED */ be<u8> m2D1[0x2D4 - 0x2D1];
    /* 0x3F0 */ dKy_tevstr_c mKenTevStr;
    /* 0x5B8 */ be<s16> m384;
    /* 0x5BA */ be<s16> mAction;
    /* 0x5BC */ be<s16> mMode;
    /* 0x5BE */ be<s16> m38A;
    /* 0x5C0 */ cXyz m38C;
    /* 0x5CC */ be<s16> m398;
    /* 0x5CE */ be<s16> m39A;
    /* 0x5D0 */ be<f32> m39C;
    /* 0x5D4 */ be<f32> m3A0;
    /* 0x5D8 */ be<s16> m3A4[5];
    /* 0x5E2 */ be<s16> m3AE;
    /* 0x5E4 */ be<f32> m3B0;
    /* 0x5E8 */ be<s16> m3B4;
    /* 0x5EA */ be<s16> m3B6;
    /* 0x5EC */ be<s16> m3B8;
    /* 0x5EE */ be<u8> m3BA[0x3BC - 0x3BA];
    /* 0x5F0 */ be<f32> m3BC;
    /* 0x5F4 */ be<f32> m3C0;
    /* 0x5F8 */ gptr<void> mEmitters1[2];
    /* 0x600 */ gptr<void> mEmitters2[2];
    /* 0x608 */ gptr<void> mEmitters3[2];
    /* 0x610 */ be<u32> m3DC;
    /* 0x614 */ cXyz m3E0;
    /* 0x620 */ cXyz m3EC;
    /* 0x62C */ cXyz m3F8;
    /* 0x638 */ be<f32> m404;
    /* 0x63C */ be<s8> m408;
    /* 0x63D */ be<s8> m409;
    /* 0x63E */ be<s8> m40A;
    /* 0x63F */ be<s8> m40B;
    /* 0x640 */ dCcD_Sph mBallTgSph;
    /* 0x76C */ dCcD_Sph mBallAtSph;
    /* 0x898 */ cXyz m664;
    /* 0x8A4 */ be<s8> m670;
    /* 0x8A5 */ be<s8> m671;
    /* 0x8A6 */ be<u8> m672;
    /* 0x8A7 */ be<s8> m673;
    /* 0x8A8 */ gptr<J3DModel> mpEnergySphereModel;
    /* 0x8AC */ gptr<mDoExt_baseAnm> mpBtkAnm;
    /* 0x8B0 */ gptr<mDoExt_baseAnm> mpBrkAnm3;
    /* 0x8B4 */ be<f32> m680;
    /* 0x8B8 */ be<s8> m684;
    /* 0x8B9 */ be<s8> m685;
    /* 0x8BA */ be<s8> m686;
    /* 0x8BB */ be<s8> m687;
    /* 0x8BC */ be<s8> m688;
    /* 0x8BD */ be<s8> m689;
    /* 0x8BE */ be<s8> m68A;
    /* 0x8BF */ be<s8> m68B;
    /* 0x8C0 */ be<s8> m68C;
    /* 0x8C1 */ be<s8> mbIsMaterialized;
    /* 0x8C2 */ be<s8> m68E;
    /* 0x8C3 */ be<u8> m68F;
    /* 0x8C4 */ be<s8> m690;
    /* 0x8C5 */ be<u8> m691[0x694 - 0x691];
    /* 0x8C8 */ be<f32> m694;
    /* 0x8CC */ be<f32> m698;
    /* 0x8D0 */ be<f32> m69C;
    /* 0x8D4 */ be<s16> m6A0;
    /* 0x8D6 */ be<s16> m6A2;
    /* 0x8D8 */ be<s16> m6A4;
    /* 0x8DA */ be<s16> m6A6;
    /* 0x8DC */ be<u32> m6A8;
    /* 0x8E0 */ be<s8> m6AC;
    /* 0x8E1 */ be<u8> m6AD[0x6B0 - 0x6AD];
    /* 0x8E4 */ be<u32> mBokoID;
    /* 0x8E8 */ be<u32> mCapeID;
    /* 0x8EC */ dBgS_AcchCir mAcchCir;
    /* 0x92C */ dBgS_ObjAcch mAcch;
    /* 0xAF0 */ dCcD_Stts mStts;
    /* 0xB2C */ dCcD_Cyl mCyl;
    /* 0xC5C */ dCcD_Sph mWeponSph;
    /* 0xD88 */ be<s16> mB54;
    /* 0xD8A */ be<s16> mB56;
    /* 0xD8C */ be<u8> mB58[0xB5C - 0xB58];
    /* 0xD90 */ cXyz mB5C;
    /* 0xD9C */ cXyz mB68;
    /* 0xDA8 */ be<s16> mB74;
    /* 0xDAA */ be<s16> mB76;
    /* 0xDAC */ be<u8> mB78[0xB80 - 0xB78];
    /* 0xDB4 */ be<f32> mB80;
    /* 0xDB8 */ be<f32> mB84;
    /* 0xDBC */ be<u8> mB88;
    /* 0xDBD */ be<s8> mB89;
    /* 0xDBE */ be<u8> mB8A;
    /* 0xDBF */ be<u8> mB8B;
    u8 hdTail[0x10];
};
WWHD_SIZE(fganon_class,0xDD0);
WWHD_OFFSET(fganon_class,mpMorf,0x3DC);
WWHD_OFFSET(fganon_class,mMode,0x5BC);
WWHD_OFFSET(fganon_class,mWeponSph,0xC5C);

namespace fganon {
template<class T> inline T* member(fganon_class* actor, u32 offset) {
    return gabi::at<T>(gabi::ea(actor) + offset);
}
template<class T> inline T read(fganon_class* actor, u32 offset) {
    return gabi::load<T>(gabi::ea(actor) + offset);
}
template<class T> inline void write(fganon_class* actor, u32 offset, T value) {
    gabi::store<T>(gabi::ea(actor) + offset, value);
}
inline u32 morfModel(fganon_class* actor) {
    return gabi::load<u32>(read<u32>(actor, 0x3DC) + 0x90);
}
inline void sound(fganon_class* actor, u32 id, u32 volume=0) {
    u32 p=gabi::ea(actor);
    if (p && p+0x37C) {
        s32 reverb=gabi::call<s32>(0x02520540, read<s8>(actor,0x326));
        gabi::call(0x025E1A40,id,member<cXyz>(actor,0x37C),volume,reverb);
    }
}
}
#endif
