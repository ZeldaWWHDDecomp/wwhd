/**
 * d_a_bridge_draw.cpp (WWHD)
 * Rope bridge: daBridge_Draw.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bridge.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bridge.h"

// HD: Rope and collision helpers preserve the original caller workspaces.
/* 025ED1BC mDoExt_3DlineMat1_c::update(int segs, GXColor& color, dKy_tevstr_c*) (as d_a_sss) */

/* 025EC62C mDoExt_3DlineMat1_c::update(u16 segs, f32 width, GXColor& color, u16, dKy_tevstr_c*) */

/* dComIfGd_set3DlineMat (HD): the sort packet play+0x5FB4 + getMaterialID() * 0x9C (as d_a_sss) */

/* mDoExt_3DlineMat1_c::getPos(i) / getSize(i): mpLines (+0x184) is an array of {cXyz* pos; u8* size; ...}
 * (0x10) */
static inline u32 lineMat_pos(BridgeLineMaterial *l, int i) {
    return gabi::ea(l->mpLines[i].mpSegments.get());
}
static inline u32 lineMat_size(BridgeLineMaterial *l, int i) { return gabi::ea(l->mpLines[i].mpSize.get()); }
static inline void copy_words(u32 dst, u32 src) {
    gabi::at<BridgeHD::VectorWords>(dst)->mWords[0] = (u32)(gabi::at<BridgeHD::VectorWords>(src)->mWords[0]);
    gabi::at<BridgeHD::VectorWords>(dst)->mWords[1] = (u32)(gabi::at<BridgeHD::VectorWords>(src)->mWords[1]);
    gabi::at<BridgeHD::VectorWords>(dst)->mWords[2] = (u32)(gabi::at<BridgeHD::VectorWords>(src)->mWords[2]);
}

/* one plank's rope (pBr->mLineMat1 lines `line` and `line + 2`): GameCube's two copies of this loop */
static inline void draw_plank_rope(bridge_class *i_this, br_s *pBr, int line, u8 flag, u8 uVar16, f32 dx,
                                   f32 dy, f32 dz, cXyz *sp78, cXyz *sp84) {
    u32 lines = (u32)(*reinterpret_cast<be<u32> *>(
        &gabi::at<BridgeLineMaterial>(gabi::ea(&pBr->mLineMat1))->mpLines));
    u32 size0 = gabi::ea(gabi::at<BridgeLine>(lines)[line].mpSize.get());
    u32 size1 = gabi::ea(gabi::at<BridgeLine>(lines)[(line + 2)].mpSize.get());
    if ((pBr->m408 & flag) == 0) {
        for (int j = 0; j < 5; j++) {
            *gabi::at<be<u8>>(size1++) = uVar16;
            *gabi::at<be<u8>>(size0++) = uVar16;
        }
        return;
    }
    u32 segment0 = gabi::ea(gabi::at<BridgeLine>(lines)[line].mpSegments.get());
    u32 segment1 = gabi::ea(gabi::at<BridgeLine>(lines)[(line + 2)].mpSegments.get());
    cXyz *ropeEnd = line == 0 ? pBr->m11C : pBr->m0F8;

    dx = dx * 0.25f; /* GameCube: /= 4.0f */
    dy = dy * 0.25f;
    dz = dz * 0.25f;
    f32 fVar3 = cM_ssin(i_this->m0300 * 5) * (f32)pBr->m3A0[line];

    gabi::Local<cXyz> sum;
    for (int j = 0; j < 5; j++, segment0 += 0xC, segment1 += 0xC, size0++, size1++) {
        *gabi::at<be<u8>>(size0) = uVar16;
        f32 fVar2;
        if (j == 2) {
            fVar2 = 1.0f;
            s8 n = line == 0 ? (s8)pBr->m3A4 : (s8)pBr->m3A5;
            if (n <= 1) {
                *gabi::at<be<u8>>(size0) = 0;
            } else if (n == 2) {
                *gabi::at<be<u8>>(size0) = 1;
            }
            copy_words(gabi::ea(&pBr->m3A8[line]), segment0);
        } else if (j == 1 || j == 3) {
            fVar2 = 0.7f;
        } else {
            fVar2 = 0.0f;
        }
        f32 fj = (f32)j;
        sp84->x = gabi::fmadds(dx, fj, fVar2 * sp78->x * fVar3);
        sp84->y = dy * fj;
        sp84->z = gabi::fmadds(dz, fj, fVar2 * sp78->z * fVar3);
        cXyz_pl(&ropeEnd[1], sum, sp84);
        copy_words(segment0, gabi::ea(sum.get()));
        copy_words(segment1, gabi::ea(&ropeEnd[0]));
        *gabi::at<be<u8>>(size1) = 0;
    }
}

/* one chain model (type 1 bridges): rope 0 hangs from m11C, rope 1 from m0F8 */
static inline void draw_chain(bridge_class *i_this, br_s *pBr, int side, f32 dx, f32 dy, f32 dz) {
    s16 atan = (s16)-cM_atan2s(dy, dz);
    s16 atan2 = cM_atan2s(dx, std_sqrtf(gabi::fmadds(dy, dy, dz * dz)));
    cXyz *top = side == 0 ? &pBr->m11C[1] : &pBr->m0F8[1];
    MtxTrans(top->x, top->y, top->z, 0);
    s16 m3A0 = pBr->m3A0[side];
    s16 sVar8;
    if (m3A0 != 0) {
        s16 sVar7 = i_this->m0300;
        sVar8 = (s16)gabi::ftoi(cM_ssin(sVar7 * 6) * (f32)m3A0 * 100.0f);
        cMtx_YrotM(calc_mtx(), sVar7);
    } else {
        sVar8 = 0;
        cMtx_YrotM(calc_mtx(), 0);
    }
    cMtx_XrotM(calc_mtx(), (s16)(atan + sVar8));
    cMtx_YrotM(calc_mtx(), atan2);
    J3DModel *rope = side == 0 ? pBr->mpModelRope0.get() : pBr->mpModelRope1.get();
    J3DModel_setBaseTRMtx(rope, calc_mtx());
    dScnKy_env_light_c *env = dKy_getEnvlight();
    setLightTevColorType(env, side == 0 ? pBr->mpModelRope0.get() : pBr->mpModelRope1.get(), &i_this->tevStr);
    mDoExt_modelUpdateDL(side == 0 ? pBr->mpModelRope0.get() : pBr->mpModelRope1.get());
}

/* the side ropes (mLineMat line `line`): from home.pos to the far end (or the partner bridge) */
static inline void side_rope_end(bridge_class *i_this, int line, cXyz *sp54, cXyz *sp48) {
    u32 segment1 = lineMat_pos(&i_this->mLineMat, line) + i_this->m030C * 0xC + 0xC;
    if ((i_this->mTypeBits & 2) != 0) {
        bridge_class *aite = i_this->mpAite;
        if (aite != nullptr) {
            copy_words(segment1, gabi::ea(line == 0 ? &aite->m032C : &aite->m0320));
        }
    } else {
        MtxPosition(sp54, sp48);
        gabi::at<cXyz>(segment1)->x = i_this->mEndPos.x + sp48->x;
        gabi::at<cXyz>(segment1)->y = i_this->mEndPos.y + sp48->y;
        gabi::at<cXyz>(segment1)->z = i_this->mEndPos.z + sp48->z;
    }
}

/* 020DFF60 */
BOOL daBridge_Draw(bridge_class *i_this) {
    /* HD: no mbStopDraw */
    br_s *pBr = &i_this->mBr[0];
    gabi::Local<cXyz> sp84;
    gabi::Local<cXyz> sp78;
    gabi::Local<cXyz> diff;
    for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
        dScnKy_env_light_c *env = dKy_getEnvlight();
        setLightTevColorType(env, pBr->mpModel, &i_this->tevStr);
        dComIfGd_setListBG();
        mDoExt_modelUpdateDL(pBr->mpModel);
        dComIfGd_setList();

        if ((pBr->m408 & 4) == 0) {
            continue;
        }

        cMtx_YrotS(calc_mtx(), i_this->m0300);
        sp84->x = 0.0f;
        sp84->y = 0.0f;
        sp84->z = 1.0f;
        MtxPosition(sp84, sp78);
        cXyz_mi(&pBr->m11C[0], diff, &pBr->m11C[1]);
        f32 sp6Cx = diff->x, sp6Cy = diff->y, sp6Cz = diff->z;
        cXyz_mi(&pBr->m0F8[0], diff, &pBr->m0F8[1]);
        f32 sp60x = diff->x, sp60y = diff->y, sp60z = diff->z;

        if ((i_this->mTypeBits & 1) == 0) {
            u8 uVar16 = (i_this->mTypeBits & 8) ? 5 : 3;
            draw_plank_rope(i_this, pBr, 0, 1, uVar16, sp6Cx, sp6Cy, sp6Cz, sp78, sp84);
            draw_plank_rope(i_this, pBr, 1, 2, uVar16, sp60x, sp60y, sp60z, sp78, sp84);
            BridgeHD::SDK::mDoExt_3DlineMat1_update(
                &pBr->mLineMat1, 5, BridgeHD::plankRopeColor() /* {150, 150, 150, 255} */, &i_this->tevStr);
            BridgeHD::SDK::dComIfGd_set3DlineMat(&pBr->mLineMat1);
            continue;
        }

        if ((pBr->m408 & 1) != 0) {
            draw_chain(i_this, pBr, 0, sp6Cx, sp6Cy, sp6Cz);
        }
        if ((pBr->m408 & 2) != 0) {
            draw_chain(i_this, pBr, 1, sp60x, sp60y, sp60z);
        }
    }

    if ((i_this->mTypeBits & 5) == 0) {
        gabi::Local<cXyz> sp54;
        gabi::Local<cXyz> sp48;
        sp54->x = -120.0f;
        sp54->y = 350.0f;
        sp54->z = -40.0f;
        cMtx_YrotS(calc_mtx(), i_this->home.angle.y);
        MtxPosition(sp54, sp48);

        u32 segment1 = lineMat_pos(&i_this->mLineMat, 0);
        gabi::at<cXyz>(segment1)->x = i_this->home.pos.x + sp48->x;
        gabi::at<cXyz>(segment1)->y = i_this->home.pos.y + sp48->y;
        gabi::at<cXyz>(segment1)->z = i_this->home.pos.z + sp48->z;
        sp54->z = -sp54->z; /* GameCube: *= -1.0f (fneg) */
        side_rope_end(i_this, 0, sp54, sp48);

        sp54->x = -sp54->x;
        sp54->z = -sp54->z;
        MtxPosition(sp54, sp48);
        segment1 = lineMat_pos(&i_this->mLineMat, 1);
        gabi::at<cXyz>(segment1)->x = i_this->home.pos.x + sp48->x;
        gabi::at<cXyz>(segment1)->y = i_this->home.pos.y + sp48->y;
        gabi::at<cXyz>(segment1)->z = i_this->home.pos.z + sp48->z;
        sp54->z = -sp54->z;
        side_rope_end(i_this, 1, sp54, sp48);

        f32 tmp = (i_this->mTypeBits & 8) != 0 ? 6.5f : 4.0f;
        /* HD: one segment fewer for a bridge joined to a partner (type 2) that faces a positive angle */
        s32 extra = 2;
        if ((i_this->mTypeBits & 2) != 0 && i_this->home.angle.y > 0) {
            extra = 1;
        }
        BridgeHD::SDK::mDoExt_3DlineMat1_update_w(&i_this->mLineMat, (u16)(i_this->m030C + extra), tmp,
                                                  BridgeHD::sideRopeColor() /* {150, 150, 150, 255} */, 0,
                                                  &i_this->tevStr);
        BridgeHD::SDK::dComIfGd_set3DlineMat(&i_this->mLineMat);
    }
    return TRUE;
}

/* WWHD glue
 */
