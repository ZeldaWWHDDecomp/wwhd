/**
 * d_a_bgn_create.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 1): daBgn_Create.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bgn_create.h"

BOOL daBgn_Execute(bgn_class* i_this);

static BgnCreateActors actors;
static BgnCreateSavePointer save;
static BgnCreateTev bgTevStr;
static dCcD_SrcSph cc_sph_src;
static dCcD_SrcSph core_sph_src;



template <unsigned N>
static void copyTevBytes(be<u8> (&dst)[N], const be<u8> (&src)[N]) {
    for (unsigned i = 0; i < N; ++i)
        dst[i] = u8(src[i]);
}
static void bgn_tevstr_assign(BgnCreateTev* dst, const BgnCreateTev* src) {
    copyTevBytes(dst->mLight0, src->mLight0);
    copyTevBytes(dst->mParameters, src->mParameters);
    copyTevBytes(dst->mLight1, src->mLight1);
    copyTevBytes(dst->mLight2, src->mLight2);
}

/* the collision sphere of a part: cc_sph_src, the actor's Stts, no attack yet, the actor's tev settings */
static inline void bgn_part_sph(bgn_class* i_this, part_s* p) {
    p->mPartSph.Set(&cc_sph_src);
    p->mPartSph.SetStts(&i_this->mStts);
    OffAtSetBit(&p->mPartSph);
}

// HD: parts copy actor tev settings; the static bgn pointer is set here.
/* 02080BA4 */
cPhs_State daBgn_Create(fopAc_ac_c* a_this) {
    bgn_class* i_this = (bgn_class*)a_this;
    /* fopAcM_ct */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            BgnCreate_construct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State res = dComIfG_resLoad(&i_this->mPhase, "Bgn");
    if (res == cPhs_COMPLEATE_e) {
        reinterpret_cast<BgnCreateSmoke*>(i_this->mPunchSmokeCb[0])->setFollowOff();
        reinterpret_cast<BgnCreateSmoke*>(i_this->mPunchSmokeCb[1])->setFollowOff();
        actors.bgn = i_this;
        actors.bgn3 = nullptr;
        actors.bgn2 = nullptr;
        i_this->m02B4 = (u8)fopAcM_GetParam(a_this);
        if (!fopAcM_entrySolidHeap(a_this, useHeapInit, 0x96000))
            return cPhs_ERROR_e;
        if (i_this->m02B4 != 0xFF) {
            if (hio_set() == 0) {
                i_this->mCC91 = 1;
                hio_set() = 1;
                s8 no = mDoHIO_createChild("G (Puppet)" /* HD: translated HIO label; original guest string retained. */, &l_HIO());
                BGN_HAND_MAX() = l_HIO().m0F0;
                BGN_TAIL_MAX() = l_HIO().m0F2;
                l_HIO().mNo = no;
            }
            i_this->mStts.Init(0xFF, 0xFF, a_this);
            i_this->mC7FC.Set(&cc_sph_src);
            i_this->mC7FC.SetStts(&i_this->mStts);
            OffAtSetBit(&i_this->mC7FC);
            i_this->mCoreSph.Set(&core_sph_src);
            i_this->mCoreSph.SetStts(&i_this->mStts);
            const BgnCreateTev* tev = reinterpret_cast<const BgnCreateTev*>(&a_this->tevStr);
            bgn_part_sph(i_this, &i_this->mHeadParts[0]);
            bgn_tevstr_assign(reinterpret_cast<BgnCreateTev*>(&i_this->mHeadParts[0].mPartTevStr), tev);
            bgn_part_sph(i_this, &i_this->mPelvisParts[0]);
            bgn_tevstr_assign(reinterpret_cast<BgnCreateTev*>(&i_this->mPelvisParts[0].mPartTevStr), tev);
            for (s32 i = 0; i < 20; i++) {
                bgn_part_sph(i_this, &i_this->mLeftArmParts[i]);
                bgn_part_sph(i_this, &i_this->mRightArmParts[i]);
                bgn_tevstr_assign(reinterpret_cast<BgnCreateTev*>(&i_this->mLeftArmParts[i].mPartTevStr), tev);
                bgn_tevstr_assign(reinterpret_cast<BgnCreateTev*>(&i_this->mRightArmParts[i].mPartTevStr), tev);
            }
            for (s32 i = 0; i < 3; i++) {
                bgn_part_sph(i_this, &i_this->mLeftLegParts[i]);
                bgn_part_sph(i_this, &i_this->mRightLegParts[i]);
                bgn_tevstr_assign(reinterpret_cast<BgnCreateTev*>(&i_this->mLeftLegParts[i].mPartTevStr), tev);
                bgn_tevstr_assign(reinterpret_cast<BgnCreateTev*>(&i_this->mRightLegParts[i].mPartTevStr), tev);
            }
            for (s32 i = 0; i < 20; i++) {
                bgn_part_sph(i_this, &i_this->mTailParts[i]);
                bgn_tevstr_assign(reinterpret_cast<BgnCreateTev*>(&i_this->mTailParts[i].mPartTevStr), tev);
            }
            for (s32 i = 0; i < 8; i++) {
                i_this->mAAA8[i].m2FA = (s16)gabi::ftoi(cM_rndF(32768.0f));
                i_this->mAAA8[i].m2FC = (s16)gabi::ftoi(cM_rndF(32768.0f));
            }
            gabi::Local<cXyz> local_50;
            if (save.info->mSave.mSavedata.mEvent.isEventBit(0x3F10)) {
                i_this->m02B4 = 0xFF;
                local_50->y = gabi::fadds_ppc(REG8_F(4), 4441.46f);
                local_50->x = 375.17f;
                local_50->z = gabi::fadds_ppc(REG8_F(3), -15.0f);
                fopAcM_create(fpcNm_HIMO3_e, 0xF, local_50, fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
                f32 y = gabi::fadds_ppc(REG8_F(4), 4453.96f);
                local_50->x = 375.17f;
                local_50->z = 0.0f;
                local_50->y = y;
                gabi::Local<csXyz> cStack_58;
                csXyz_ct(cStack_58, 0, 0x4000, 0);
                fopAcM_create(fpcNm_KUI_e, 0xFFFF0400, local_50, fopAcM_GetRoomNo(a_this), cStack_58, nullptr, -1, 0);
            } else {
                for (s32 i = 0; i < 8; i++) {
                    fopAcM_create(fpcNm_ATT_e, i, &a_this->current.pos, fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
                    i_this->mAAA8[i].m308 = 3;
                }
                fopAcM_create(fpcNm_BGN2_e, 0, &a_this->current.pos, fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
                fopAcM_create(fpcNm_BGN3_e, 0, &a_this->current.pos, fopAcM_GetRoomNo(a_this), nullptr, nullptr, -1, 0);
                mDoAud_bgmStart(JA_BGM_BGN_KUGUTSU);
                a_this->health = 3;
                a_this->max_health = 3;
            }
            i_this->mCC80 = 1.0f;
            i_this->mC748 = 7;
            i_this->mC7AC[0] = 60;
            bgn_tevstr_assign(&bgTevStr, tev);
            bgn_tevstr_assign(reinterpret_cast<BgnCreateTev*>(&i_this->mWaterTevStr), tev);
            bgn_tevstr_assign(reinterpret_cast<BgnCreateTev*>(&i_this->mRoomTevStr), tev);
            daBgn_Execute(i_this);
        }
    }
    return res;
}

/* WWHD glue
 data actors 10461A34
 data save 101F84DC
 data bgTevStr 10461E58
 data cc_sph_src 10190F68
 data core_sph_src 10190FA8
 ext 0208064C bgn_class* BgnCreate_construct(bgn_class*)
 str 10008C90 "Bgn" in 02080BA4
 str 10008C94 "G (Puppet)" renamed in 02080BA4
*/
