/**
 * d_a_bgn_create.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 1): daBgn_Create.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bgn.h"

BOOL daBgn_Execute(bgn_class* i_this);

/* dKy_tevstr_c assignment (HD 0x1C8): the members are copied, the three 0x40-byte blocks after each light
 * block (+0x44, +0x104, +0x184: HD caches) and the padding at +0xBD are not */
static inline void bgn_tevstr_assign(u32 dst, u32 src) {
    static const u16 ranges[4][2] = {{0x000, 0x044}, {0x084, 0x0BD}, {0x0C0, 0x104}, {0x144, 0x188}};
    for (int r = 0; r < 4; r++)
        for (u32 o = ranges[r][0]; o < ranges[r][1]; o++) gabi::store<u8>(dst + o, gabi::load<u8>(src + o));
}

/* the collision sphere of a part: cc_sph_src, the actor's Stts, no attack yet, the actor's tev settings */
static inline void bgn_part_sph(bgn_class* i_this, part_s* p) {
    p->mPartSph.Set(gabi::at<dCcD_SrcSph>(0x10190F68) /* cc_sph_src */);
    p->mPartSph.SetStts(&i_this->mStts);
    OffAtSetBit(&p->mPartSph);
}

/* 02080BA4 daBgn_Create. HD: the parts copy the actor's tev settings (as the GameCube release);
 * the static `bgn` pointer is set here. */
cPhs_State daBgn_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02080BA4, cPhs_State, a_this);
    bgn_class* i_this = (bgn_class*)a_this;
    /* fopAcM_ct */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            gabi::call(0x0208064C /* bgn_class::bgn_class */, a_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State res = dComIfG_resLoad(&i_this->mPhase, STR(0x10008C90) /* "Bgn" */);
    if (res == cPhs_COMPLEATE_e) {
        gabi::store<u8>(gabi::ea(i_this->mPunchSmokeCb[0]) + 0x12, 1); /* setFollowOff */
        gabi::store<u8>(gabi::ea(i_this->mPunchSmokeCb[1]) + 0x12, 1);
        bgn_g() = gabi::ea(i_this);
        bgn3_g() = 0;
        bgn2_g() = 0;
        i_this->m02B4 = (u8)fopAcM_GetParam(a_this);
        if (!fopAcM_entrySolidHeap(a_this, 0x0207FE14 /* useHeapInit */, 0x96000))
            return cPhs_ERROR_e;
        if (i_this->m02B4 != 0xFF) {
            if (hio_set() == 0) {
                i_this->mCC91 = 1;
                hio_set() = 1;
                s8 no = mDoHIO_createChild(STR(0x10008C94) /* "Ｇ（クグツ）" */, &l_HIO());
                BGN_HAND_MAX() = l_HIO().m0F0;
                BGN_TAIL_MAX() = l_HIO().m0F2;
                l_HIO().mNo = no;
            }
            i_this->mStts.Init(0xFF, 0xFF, a_this);
            i_this->mC7FC.Set(gabi::at<dCcD_SrcSph>(0x10190F68) /* cc_sph_src */);
            i_this->mC7FC.SetStts(&i_this->mStts);
            OffAtSetBit(&i_this->mC7FC);
            i_this->mCoreSph.Set(gabi::at<dCcD_SrcSph>(0x10190FA8) /* core_sph_src */);
            i_this->mCoreSph.SetStts(&i_this->mStts);
            u32 tev = gabi::ea(a_this) + 0x110;
            bgn_part_sph(i_this, &i_this->mHeadParts[0]);
            bgn_tevstr_assign(gabi::ea(&i_this->mHeadParts[0].mPartTevStr), tev);
            bgn_part_sph(i_this, &i_this->mPelvisParts[0]);
            bgn_tevstr_assign(gabi::ea(&i_this->mPelvisParts[0].mPartTevStr), tev);
            for (s32 i = 0; i < 20; i++) {
                bgn_part_sph(i_this, &i_this->mLeftArmParts[i]);
                bgn_part_sph(i_this, &i_this->mRightArmParts[i]);
                bgn_tevstr_assign(gabi::ea(&i_this->mLeftArmParts[i].mPartTevStr), tev);
                bgn_tevstr_assign(gabi::ea(&i_this->mRightArmParts[i].mPartTevStr), tev);
            }
            for (s32 i = 0; i < 3; i++) {
                bgn_part_sph(i_this, &i_this->mLeftLegParts[i]);
                bgn_part_sph(i_this, &i_this->mRightLegParts[i]);
                bgn_tevstr_assign(gabi::ea(&i_this->mLeftLegParts[i].mPartTevStr), tev);
                bgn_tevstr_assign(gabi::ea(&i_this->mRightLegParts[i].mPartTevStr), tev);
            }
            for (s32 i = 0; i < 20; i++) {
                bgn_part_sph(i_this, &i_this->mTailParts[i]);
                bgn_tevstr_assign(gabi::ea(&i_this->mTailParts[i].mPartTevStr), tev);
            }
            for (s32 i = 0; i < 8; i++) {
                i_this->mAAA8[i].m2FA = (s16)gabi::ftoi(cM_rndF(32768.0f));
                i_this->mAAA8[i].m2FC = (s16)gabi::ftoi(cM_rndF(32768.0f));
            }
            gabi::Local<cXyz> local_50;
            if (dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), 0x3F10)) {
                i_this->m02B4 = 0xFF;
                local_50->y = gabi::fadds_ppc(REG_F(8, 4), 4441.46f);
                local_50->x = 375.17f;
                local_50->z = gabi::fadds_ppc(REG_F(8, 3), -15.0f);
                fopAcM_create(0x1BF /* fpcNm_HIMO3_e */, 0xF, local_50, fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
                f32 y = gabi::fadds_ppc(REG_F(8, 4), 4453.96f);
                local_50->x = 375.17f;
                local_50->z = 0.0f;
                local_50->y = y;
                gabi::Local<csXyz> cStack_58;
                csXyz_ct(cStack_58, 0, 0x4000, 0);
                fopAcM_create(0xFA /* fpcNm_KUI_e */, 0xFFFF0400, local_50, fopAcM_GetRoomNo(a_this), cStack_58, nullptr, -1, 0);
            } else {
                for (s32 i = 0; i < 8; i++) {
                    fopAcM_create(0x1C0 /* fpcNm_ATT_e */, i, &a_this->current.pos, fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
                    i_this->mAAA8[i].m308 = 3;
                }
                fopAcM_create(0xF4 /* fpcNm_BGN2_e */, 0, &a_this->current.pos, fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
                fopAcM_create(0xF5 /* fpcNm_BGN3_e */, 0, &a_this->current.pos, fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
                mDoAud_bgmStart(0x80000059 /* JA_BGM_BGN_KUGUTSU */);
                a_this->health = 3;
                a_this->max_health = 3;
            }
            i_this->mCC80 = 1.0f;
            i_this->mC748 = 7;
            i_this->mC7AC[0] = 60;
            bgn_tevstr_assign(bg_tevstr(), tev);
            bgn_tevstr_assign(gabi::ea(&i_this->mWaterTevStr), tev);
            bgn_tevstr_assign(gabi::ea(&i_this->mRoomTevStr), tev);
            daBgn_Execute(i_this);
        }
    }
    return res;
}
VERIFY(0x02080BA4, daBgn_Create);
