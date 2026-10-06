// Molgera execute sequence;
#include "d/actor/d_a_bwd.h"

static BOOL bx_hookReturn(fopAc_ac_c*){u32 link=gabi::load<u32>(dComIfGp_ea()+0x5B34);return gabi::call<BOOL>(0x024435FC,link);}
static void bx_bgmStart(u32 n){gabi::call(0x025E18EC,n);}
static void bx_monsSeStart(fopAc_ac_c* a,u32 id,u32 param){if(!gabi::ea(&a->eyePos))return;u32 actorID=fopAcM_GetID(a);s32 rev=dComIfGp_getReverb(fopAcM_GetRoomNo(a));gabi::call(0x025E1AA4,id,&a->eyePos,actorID,param,rev);}
static void bx_cameraType(const char* key,void* a){gabi::call(0x02514EE4,gabi::call<u32>(0x024F8044),key,a);}
static void bx_color(u8 a,u8 b,f32 f){gabi::call(0x0255FDA4,a,b,f);}

static void bx_stackX(s16 a){cMtx_XrotM(mDoMtx_stack_c::get(),a);}
static void bx_stackZ(s16 a){cMtx_ZrotM(mDoMtx_stack_c::get(),a);}
static void bx_addCalcPos2(cXyz* a,cXyz* b,f32 rate,f32 step){gabi::call(0x0200F164,a,b,rate,step);}
static csXyz* bx_historyAngles(bwd_class* a,u32 i){return gabi::at<csXyz>(gabi::ea(a)+0x1274+6u*i);}
static cXyz* bx_historyPosition(bwd_class* a,u32 i){return gabi::at<cXyz>(gabi::ea(a)+0x674+12u*i);}
static void damage_check(bwd_class* i_this) {
    fopAc_ac_c* actor=i_this;
    dComIfGp_get();
    bool scanBody = i_this->m18D6==0;
    if (i_this->m18D6==0) {
        gabi::Local<CcAtInfo_bd> atInfo;
        atInfo->pParticlePos=0;
        if (auto* tracked=fopAcM_SearchByID(i_this->m1BC0)) tracked->current.pos.copy(actor->eyePos);
        i_this->mTongueSph.SetTgType(i_this->m1BB1!=0?0xFF1DFEFFu:0x8000u);
        if (i_this->mTongueSph.ChkTgHit()) {
            i_this->m18D6=6;
            atInfo->mpObj=gabi::ea(i_this->mTongueSph.GetTgHitObj());
            if (atInfo->mpObj!=0) {
                atInfo->pParticlePos=gabi::ea(i_this->mTongueSph.GetTgHitPosP());
                if (gabi::load<u32>((u32)atInfo->mpObj+0x10)&0x8000) {
                    i_this->m1BB0=1;
                    u32 stts=gabi::load<u32>((u32)atInfo->mpObj+0x44);
                    u32 owner=stts?gabi::load<u32>(stts+0x0C):0;
                    atInfo->mpActor=owner;
                    i_this->m1BBC=owner?gabi::load<u32>(owner+4):0xFFFFFFFFu;
                    i_this->m18AE=3; i_this->m18B0=0;
                    if (actor!=nullptr) bd_seStart(actor,0x286F,0x20);
                } else if (i_this->m1BB1!=0) {
                    s8 saved=actor->health; actor->health=20;
                    cc_at_check(actor,atInfo.get()); actor->health=saved-1;
                    bx_monsSeStart(actor,0x48DE,0);
                    i_this->m1BB4++; i_this->m1BB3=20;
                }
            }
        }
    }
    if (scanBody) {
        for (int i=0;i<9;i++) if (i_this->mBodySph[i].ChkTgHit()) {
            def_se_set(actor,i_this->mBodySph[i].GetTgHitObj(),0x40); return;
        }
    }
}

static void control1(bwd_class* i_this, dBgS_GndChk* sharedGround) {
    fopAc_ac_c* actor = i_this;
    f32 dVar8;
    f32 dVar10;
    gabi::Local<cXyz> local_104;
    gabi::Local<cXyz> local_110;
    gabi::Local<cXyz> local_11c;
    gabi::Local<cXyz> local_128;

    PSMTXCopy(bd_getAnmMtx(i_this->mpHeadMorf->getModel(),0x1D), calc_mtx());
    local_104->set(REG0_F(2) + -50.0f, REG0_F(3), REG0_F(4));
    MtxPosition(local_104.get(), &i_this->mTongueSegments[0].m04);
    int i = 1;
    sita_s* sita_i = &i_this->mTongueSegments[1];
    cMtx_YrotS(calc_mtx(), actor->current.angle.y);
    cMtx_XrotM(calc_mtx(), actor->current.angle.x);
    local_104->x = 0.0f;
    local_104->y = 0.0f;
    local_104->z = i_this->m1BA4;
    MtxPosition(local_104.get(), local_11c.get());
    local_104->z = i_this->m1BA0;
    f32 dVar9 = i_this->m1BA8;
    local_128->set(0,0,0);
    dBgS_GndChk_ct(sharedGround,BD_GNDCHK_VT,false);
    for (i = 1; i < 0x1d; i++, ++sita_i) {
        if (i_this->m1BB0 == 0) {
            if (i_this->m1BB3 != 0) {
                dVar9 = i_this->m1BB3 * (REG0_F(3) + 4.5f);
                local_110->x = dVar9 * cM_ssin(i_this->m18AC * (REG0_S(5) + 0x1fa4) - i * (REG0_S(6) + 0xa28));
                local_110->y = (dVar9 * cM_scos(i_this->m18AC * (REG0_S(5) + 0x2198) - i * (REG0_S(6) + 0x898)));
                local_110->z = dVar9 * cM_scos(i_this->m18AC * (REG0_S(5) + 0x206c) - i * (REG0_S(6) + 0x960));
            } else {
                local_110->x = dVar9 * cM_ssin(i_this->m18AC * (REG0_S(5) + 0x44c) - i * (REG0_S(6) + 2000));
                if (i_this->m1BB1 != 0) {
                    local_110->y = -5.0f;
                } else {
                    local_110->y = REG0_F(5) + 5.0f;
                }
                local_110->z = dVar9 * cM_scos(i_this->m18AC * (REG0_S(7) + 900) - i * (REG0_S(8) + 2000));
            }
            MtxPosition(local_110.get(), local_128.get());
        }
        dVar10 = (local_128->y + (sita_i->m04.y + local_11c->y));
        if (local_110->y < 0.0f) {
            gabi::Local<cXyz> pos;
            pos->copy(sita_i->m04);
            pos->y += 200.0f;
            gabi::at<cXyz>(gabi::ea(sharedGround)+0x24)->copy(*pos.get());
            f32 ground = cBgS_GroundCross(dComIfG_Bgsp(),sharedGround);
            dVar8 = (ground + REG0_F(16)) + 80.0f;
            if (!(dVar10 > dVar8)) {
                dVar10 = dVar8;
            }
        }
        f32 x = ((sita_i->m04.x - sita_i[-1].m04.x) + local_11c->x) + local_128->x;
        f32 y = (dVar10 - sita_i[-1].m04.y);
        f32 z = ((sita_i->m04.z - sita_i[-1].m04.z) + local_11c->z) + local_128->z;
        s16 iVar3;
        int iVar2 = cM_atan2s(x, z);
        iVar3 = -cM_atan2s(y, std_sqrtf(gabi::fmadds(x,x,z*z)));
        gabi::call(0x0200FCF0);
        cMtx_YrotS(calc_mtx(), iVar2);
        cMtx_XrotM(calc_mtx(), iVar3);
        MtxPosition(local_104.get(), local_110.get());
        gabi::call(0x0200FD38);
        { gabi::Local<cXyz> added; gabi::call(0x0201AD78,&sita_i[-1].m04,added.get(),local_110.get()); sita_i->m04.copy(*added.get()); }
    }
    bd_GndChk_dt(sharedGround);
}

/* 0000527C-00005438       .text control2__FP9bwd_class */
static void control2(bwd_class* i_this) {
    gabi::Local<cXyz> rel_offset; gabi::Local<cXyz> abs_offset;
    rel_offset->x = 0;
    rel_offset->y = 0;
    rel_offset->z = i_this->m1BA0;
    i_this->mTongueSegments[29].m04.copy(i_this->m1B88);

    int i = 28;
    s16 Yangle;
    int XZangle;
    sita_s* sita_i = &i_this->mTongueSegments[28];
    for (i = 28; i >= 1; i--, sita_i--) {
        f32 delta_pos_x = sita_i->m04.x - sita_i[1].m04.x;
        f32 delta_pos_y = sita_i->m04.y - sita_i[1].m04.y;
        f32 delta_pos_z = sita_i->m04.z - sita_i[1].m04.z;
        XZangle = cM_atan2s(delta_pos_x, delta_pos_z);
        Yangle = -cM_atan2s(delta_pos_y, std_sqrtf(gabi::fmadds(delta_pos_x,delta_pos_x,delta_pos_z*delta_pos_z)));
        cMtx_YrotS(calc_mtx(), XZangle);
        cMtx_XrotM(calc_mtx(), Yangle);
        MtxPosition(rel_offset.get(), abs_offset.get());
        { gabi::Local<cXyz> added; gabi::call(0x0201AD78,&sita_i[1].m04,added.get(),abs_offset.get()); sita_i->m04.copy(*added.get()); }
    }
}

/* 00005438-00005D78       .text sita_move__FP9bwd_class */
static void sita_move(bwd_class* i_this,fopAc_ac_c* player) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* pfVar2;
    f32 dVar7;
    f32 dVar8;
    f32 dVar9;
    f32 fVar10;
    gabi::Local<cXyz> local_c8;
    gabi::Local<cXyz> local_d4;
    gabi::Local<cXyz> local_f8;
    gabi::Local<dBgS_GndChk> sharedGround;

    if (i_this->m1BB3 != 0) {
        i_this->m1BB3--;
    }
    local_c8->x = (REG0_F(13) + 100.0f) * cM_ssin(i_this->m18AC * (REG0_S(5) + 0x44c));
    local_c8->y = gabi::fmadds(cM_ssin(i_this->m18AC * (REG0_S(4)+700)),REG0_F(14)+50.0f,REG0_F(0)+600.0f);
    local_c8->z = (REG0_F(13) + 100.0f) * cM_ssin(i_this->m18AC * (REG0_S(7) + 900));
    gabi::call(0x0201AD78,&i_this->mTongueSegments[0].m04,local_f8.get(),local_c8.get());
    if (i_this->m1B88.y < i_this->m3954.y) {
        attn_flags(actor) = 0;
    }
    dVar8 = 0.1f;
    dVar9 = ((REG0_F(7) + 100.0f) * i_this->m1BAC);
    if (i_this->m1BB0 != 0) {
        pfVar2 = fopAcM_SearchByID(i_this->m1BBC);
        if ((pfVar2 == NULL) || (!bx_hookReturn(player))) {
            i_this->m1BB0 = 0;
        } else {
            i_this->m18D6 = 5;
            local_f8->copy(pfVar2->current.pos);
            dVar8 = 1.0f;
            dVar9 = 200.0f;
            if (i_this->m1BA0 > REG0_F(2) + 50.0f) {
                i_this->m1BB0 = 0;
            }
        }
        i_this->m1BAC = 0.0f;
        i_this->m1BB1 = (s8)l_HIO.m14;
        i_this->m1B94.copy(i_this->m1B88);
    } else {
        if (i_this->m1BB1 != 0) {
            i_this->m1BB1--;
            f32 dx=actor->current.pos.x-i_this->m1B94.x;
            f32 dz=actor->current.pos.z-i_this->m1B94.z;
            f32 reach=std_sqrtf(gabi::fmadds(dx,dx,dz*dz));
            f32 maxReach=gabi::load<f32>(0x1047BBD8)+1200.0f;
            if(reach>maxReach){
                gabi::Local<cXyz> tether; tether->copy(actor->current.pos); tether->y=i_this->m1B94.y;
                bx_addCalcPos2(&i_this->m1B94,tether.get(),1.0f,(reach-maxReach)*0.05f);
                i_this->m1B88.copy(i_this->m1B94);
            }
            dBgS_GndChk_ct(sharedGround.get(),BD_GNDCHK_VT,false);
            gabi::Local<cXyz> pos;
            pos->copy(i_this->m1B88);
            pos->y += 200.0f;
            gabi::at<cXyz>(gabi::ea(sharedGround.get())+0x24)->copy(*pos.get());
            f32 tongueGround=cBgS_GroundCross(dComIfG_Bgsp(),sharedGround.get());
            fVar10 = (tongueGround + REG0_F(16)) + 30.0f;
            if (fVar10 != -1.0e9f) {
                cLib_addCalc2(&i_this->m1B88.y, fVar10, 0.1f, REG0_F(11) + 2.0f);
            }
            if (i_this->m1BB3 != 0) {
                f32 f1 = i_this->m1BB3 * (REG0_F(3) + 3.0f);
                local_d4->x = (f1 * cM_ssin(i_this->m18AC * (REG0_S(5) + 0x1fa4)));
                local_d4->y = (f1 * cM_ssin(i_this->m18AC * (REG0_S(5) + 0x2260)));
                local_d4->z = (f1 * cM_ssin(i_this->m18AC * (REG0_S(5) + 0x206c)));
                dVar8 = 1.0f;
                dVar9 = f1;
                gabi::call(0x0201AD78,&i_this->m1B94,local_f8.get(),local_d4.get());
            } else {
                local_f8->copy(i_this->m1B94);
            }
            bd_GndChk_dt(sharedGround.get());
        } else {
            cLib_addCalc2(&i_this->m1BAC, 1.0f, 1.0f, 0.01f);
        }
        if (i_this->m1BB2 != 0) {
            i_this->m1BB2--;
            local_c8->x = (REG0_F(17) + 400.0f) * cM_ssin((int)i_this->m18AC * (REG0_S(5) + 0xc1c));
            local_c8->y = 400.0f;
            local_c8->z = (REG0_F(17) + 400.0f) * cM_ssin((int)i_this->m18AC * (REG0_S(7) + 0x1324));
            gabi::call(0x0201AD78,&i_this->mTongueSegments[0].m04,local_f8.get(),local_c8.get());
            dVar8 = 1.0f;
            dVar9 = 200.0f;
        }
    }
    gabi::call(0x0201ADE0,&i_this->m1B88,local_c8.get(),&i_this->mTongueSegments[0].m04);
    fVar10 = std_sqrtf(PSVECSquareMag(local_c8.get()));
    dVar7 = gabi::fmuls_ppc(fVar10, REG0_F(2) + 0.035f); /* fmuls f22,f1,f8: the length is frA (NaN payload order) */
    if (i_this->m1BB6 != 0) {
        local_f8->copy(i_this->mTongueSegments[0].m04);
        dVar8 = 1.0f;
        dVar9 = 50.0f;
        if (i_this->m1BB6 != 2) {
                attn_flags(actor) = 0;
        }
        if (dVar7 < (REG0_F(1) + 20.0f)) {
            dVar7 = (REG0_F(1) + 20.0f);
        }
    }
    cLib_addCalc2(&i_this->m1B88.x, local_f8->x, dVar8, dVar9);
    cLib_addCalc2(&i_this->m1B88.y, local_f8->y, dVar8, dVar9);
    cLib_addCalc2(&i_this->m1B88.z, local_f8->z, dVar8, dVar9);
    cLib_addCalc2(&i_this->m1BA0, dVar7, 1.0f, 2.0f);
    i_this->m1BA8 = REG0_F(3) + 10.0f;
    control1(i_this,sharedGround.get());
    control2(i_this);
    sita_s* sita_i = i_this->mTongueSegments;
    for (int i = 0; i < 30; i++, ++sita_i) {
        MtxTrans(sita_i->m04.x, sita_i->m04.y, sita_i->m04.z, false);
        fVar10 = gabi::fmadds(cM_scos((int)i_this->m18AC * (REG0_S(7) + 1000) + i * (REG0_S(8) + 2000)), (0.2f + REG0_F(9)), (1.0f + REG0_F(9)));
        MtxScale(fVar10, fVar10, fVar10, true);
        J3DModel_setBaseTRMtx(sita_i->m00,calc_mtx());
        if (i == 29) {
            actor->eyePos = sita_i->m04;
            gabi::at<cXyz>(gabi::ea(actor)+0x390)->copy(actor->eyePos);
            (*gabi::at<cXyz>(gabi::ea(actor)+0x390)).y += 20.0f;
            i_this->mTongueSph.SetC(&sita_i->m04);
            i_this->mTongueSph.SetR(REG10_F(15)+100.0f);
            gabi::call(0x0200E240,dComIfG_Ccsp(),&i_this->mTongueSph);
        } else if ((((i == 0) || (i == 6)) || (i == 12)) || (i == 18 || (i == 24))) {
            int r0 = i / 6;
            i_this->mTongueCoSph[r0].SetC(&sita_i->m04);
            gabi::call(0x0200E240,dComIfG_Ccsp(),&i_this->mTongueCoSph[r0]);
        }
    }
}

static void bwd_kankyo(bwd_class* i_this) {
    if (i_this->m3C14 != 0) {
        i_this->m3C14--;
    }
    switch (i_this->m3C15) {
    case 0:
        bx_color(0, 1, 0.0f);
        break;
    case 1:
        bx_color(0, 1, i_this->m3C18);
        cLib_addCalc2(&i_this->m3C18, 1.0f, 1.0f, 0.01f);
        break;
    case 2:
        bx_color(1, 2, i_this->m3C18);
        cLib_addCalc2(&i_this->m3C18, 1.0f, 1.0f, 0.05f);
        if (i_this->m3C14 == 0) {
            if (i_this->m18AE == 0xc) {
                i_this->m3C15 = 4;
            } else {
                i_this->m3C15 = 3;
            }
        }
        break;
    case 3:
        bx_color(1, 2, i_this->m3C18);
        cLib_addCalc0(&i_this->m3C18, 1.0f, 0.02f);
        break;
    case 4:
        bx_color(3, 2, i_this->m3C18);
        cLib_addCalc0(&i_this->m3C18, 1.0f, 0.02f);
        break;
    case 5:
        bx_color(3, 4, i_this->m3C18);
        cLib_addCalc2(&i_this->m3C18, 1.0f, 1.0f, 0.02f);
        break;
    }
}

/* 000074A4-00008C24       .text daBwd_Execute__FP9bwd_class */
static BOOL bwd_execute(bwd_class* i_this) {
    WWHD_FUNC(0x020FA614,BOOL,i_this);
    int i;
    int j;
    fopAc_ac_c* actor = i_this;
    J3DAnmTextureSRTKey* pBtk;
    J3DModel* model;
    f32 dVar24;
    f32 fVar26;
    gabi::Local<cXyz> local_110;
    gabi::Local<cXyz> cStack_11c;
    gabi::Local<cXyz> local_128;

    fopAc_ac_c* player = (fopAc_ac_c*)dComIfGp_getPlayer(0);
    if (auto* tracked=fopAcM_SearchByID(i_this->m1BC0)) tracked->current.pos.y=10000.0f;
    if (dComIfGp_getStartStageName0() == 'X') {
        i_this->m3C1C = 0x32;
    }
    gabi::store<u32>(0x1018C7B0,0x101FF5F0);
    if (i_this->m3C50 != 0) {
        if (i_this->m3C50 == 2) {
            bx_cameraType(gabi::at<char>(0x1000BF90), nullptr);
            i_this->m3C50 = 1;
        } else {
            bx_cameraType(gabi::at<char>(0x1000BF98), nullptr);
        }
    }
    gabi::Local<dBgS_GndChk> gndChk; dBgS_GndChk_ct(gndChk.get(),BD_GNDCHK_VT,false);
    gabi::Local<cXyz> pos;
    pos->copy(actor->current.pos);
    pos->y += REG0_F(13) + 5500.0f;
    gabi::at<cXyz>(gabi::ea(gndChk.get())+0x24)->copy(*pos.get());
    fVar26 = cBgS_GroundCross(dComIfG_Bgsp(),gndChk.get());
    if ((fVar26 < 850.0f) && (fVar26 != -1.0e9f)) {
        i_this->m3954.y = fVar26;
    }
    i_this->m1BB6 = 0;
    ko_count = 0;
    for (int i = 0; i < l_HIO.m26; i++) {
        ko_ac()[i] = 0;
    }
    gabi::call(0x025DE508,0x020F9A28,i_this);
    if (i_this->m1710 > 1) {
        i_this->m1710--;
    }
    if ((ko_count == 0) && (i_this->m1710 == 1)) {
        if (dComIfGp_getStartStageName0() == 'X') {
            bx_bgmStart(0x8000004C);
        } else {
            bx_bgmStart(0x80000029);
        }
        i_this->m1710 = 0;
    }
    gabi::call(0x0201ADE0,&player->current.pos,local_110.get(),&actor->current.pos);
    local_110->y = 0.0f;
    i_this->m1BB8 = std_sqrtf(PSVECSquareMag(local_110.get()));
    i_this->m170C = 0;
    i_this->m18AC++;
    for (int i = 0; i < 5; i++) {
        auto* timer=gabi::at<be<s16>>(gabi::ea(i_this)+0x1B50+(u32)i*2);
        if (*timer != 0) {
            *timer = (s16)*timer-1;
        }
    }
    if (i_this->m18D6 != 0) {
        i_this->m18D6--;
    }
    if (i_this->m18FC != 0) {
        i_this->m18FC--;
    }
    if (l_HIO.m05 == 0) {
        gabi::call(0x020FF80C,i_this);
        i_this->mpHeadMorf->play(&actor->eyePos, 0, 0);
        local_110->x = 0.0f;
        local_110->y = 0.0f;
        local_110->z = gabi::load<f32>(0x1047BBFC) - 270.0f;
        cMtx_YrotS(calc_mtx(), actor->current.angle.y);
        cMtx_XrotM(calc_mtx(), actor->current.angle.x);
        MtxPosition(local_110.get(), cStack_11c.get());
        bx_historyAngles(i_this,(u32)i_this->m1708)->y = actor->current.angle.y;
        bx_historyAngles(i_this,(u32)i_this->m1708)->x = actor->current.angle.x;
        f32 rollAmount=gabi::load<f32>(0x1047BBEC)+4000.0f;
        s16 phase=i_this->m1902;
        s16 rollOffset=(s16)gabi::ftoi(cM_ssin(phase)*rollAmount);
        i_this->m1902=phase+REG0_S(7)+1000;
        bx_historyAngles(i_this,(u32)i_this->m1708)->z=actor->current.angle.z+rollOffset;
        gabi::call(0x0201AD78,&actor->current.pos,local_110.get(),cStack_11c.get());
        bx_historyPosition(i_this,(u32)i_this->m1708)->copy(*local_110.get());

        if (i_this->m170C != 0) {
            s32 angleLag=gabi::load<s16>(0x1047BC36)+10;
            for (int i = 0; i < 20; i++) {
                if (i_this->mpBodyModel[i] != 0) {
                    u32 uVar18 = (u32)i_this->m1708 - (u32)(i + 1) * (u32)i_this->m1904;
                    uVar18 = uVar18 & 0xFF;
                    int angleIndex=(uVar18+angleLag+gabi::load<s16>(0x1047BC34)-8)&0xFF;
                    if(angleLag!=0) angleLag-=gabi::load<s16>(0x1047BC38)+2;
                    mDoMtx_stack_c::transS(bx_historyPosition(i_this,uVar18)->x, bx_historyPosition(i_this,uVar18)->y, bx_historyPosition(i_this,uVar18)->z);
                    mDoMtx_stack_c::YrotM(bx_historyAngles(i_this,(u32)angleIndex)->y);
                    bx_stackX(bx_historyAngles(i_this,(u32)angleIndex)->x);
                    bx_stackZ(bx_historyAngles(i_this,(u32)angleIndex)->z);
                    fVar26 = gabi::fmadds(-(f32)i,0.02f,REG0_F(2)+1.1f);
                    fVar26 *= gabi::load<f32>(gabi::ea(i_this)+0x530+4*i)+1.0f;
                    mDoMtx_stack_c::scaleM(fVar26, fVar26, 1.0f);
                    J3DModel_setBaseTRMtx(i_this->mpBodyModel[i],mDoMtx_stack_c::get());
                    PSMTXCopy(mDoMtx_stack_c::get(), calc_mtx());
                    local_110->set(REG0_F(13), REG0_F(14), REG0_F(15));
                    MtxPosition(local_110.get(), &i_this->m0418[i]);
                    if (((i & 1) == 0) && gabi::load<s16>(0x1047BC2A)==0 && (i_this->m03C4[i] < 0.1f)) {
                        dComIfGp_particle_setSimple(0x8241, &i_this->m0418[i], 0xff, gabi::at<GXColor>(0x101D5E98), gabi::at<GXColor>(0x101D5E98), 0);
                    }
                    gabi::store<f32>(gabi::ea(i_this->mpBodyMorf[i]),i_this->m03C4[i]);
                    mDoExt_baseAnm_play(i_this->mpBodyMorf[i]);
                }
            }
        }
        if (actor->speedF > 1.0f) {
            i_this->m1708=(u32)i_this->m1708+1u;
            i_this->m1708 = i_this->m1708 & 0xFF;
        }
    }
    actor->shape_angle.x = actor->current.angle.x;
    actor->shape_angle.y = actor->current.angle.y;
    actor->shape_angle.z = actor->current.angle.z;
    model = i_this->mpHeadMorf->getModel();
    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);
    mDoMtx_stack_c::YrotM(actor->shape_angle.y);
    bx_stackX(actor->shape_angle.x);
    bx_stackZ(actor->shape_angle.z);
    if (((i_this->m18AE == 10) || (i_this->m18AE == 0xb)) || ((i_this->m18AE == 0xc) && (i_this->m18B0 >= 2))) {
        fVar26 = 1.0f;
    } else {
        fVar26 = l_HIO.m20;
    }
    mDoMtx_stack_c::scaleM(fVar26, fVar26, fVar26);
    J3DModel_setBaseTRMtx(model,mDoMtx_stack_c::get());
    i_this->mpHeadMorf->calc();
    sita_move(i_this,player);
    damage_check(i_this);
    if (gabi::load<u32>(0x10462C00) == 0) {
        for (int k = 0; k < 9; ++k) {
            gabi::at<cXyz>(0x10462B94 + 12*k)->set(k<6?100.0f:0.0f,0.0f,k<3?100.0f:(k<6?-100.0f:0.0f));
        }
        gabi::store<u32>(0x10462C00,1);
    }
    for (int i = 0; i < 9; i++) {
        PSMTXCopy(bd_getAnmMtx(i_this->mpHeadMorf->getModel(),gabi::load<u32>(0x10192F48+4*i)), calc_mtx());
        local_128->copy(*gabi::at<cXyz>(0x10462B94+12*i));
        MtxPosition(local_128.get(), cStack_11c.get());
        i_this->mBodySph[i].SetC(cStack_11c.get());
        f32 radius = gabi::load<f32>(0x10192F6C+4*i);
        if (i_this->m18AE == 10) {
            i_this->mBodySph[i].SetR(radius);
        } else {
            i_this->mBodySph[i].SetR(l_HIO.m20 * radius);
        }
        gabi::call(0x0200E240,dComIfG_Ccsp(),&i_this->mBodySph[i]);
    }
    gabi::call(0x02101F30,i_this);
    if ((i_this->m394C != 0) && (player->current.pos.y < 900.0f)) {
        gabi::call(0x0201ADE0,&i_this->m17C8,local_110.get(),&player->current.pos);
        local_110->y = 0.0f;
        f32 powerLength=std_sqrtf(PSVECSquareMag(local_110.get()));
        dVar24 = gabi::fmadds(-powerLength,gabi::load<f32>(0x1047B620)+0.00025f,1.0f);
        if (!(dVar24 >= 0.0f)) {
            dVar24 = 0.0f;
        }
        u32 powerVtable = gabi::load<u32>(gabi::ea(player)+0xB4);
        s16 powerAngle = cM_atan2s(local_110->x, local_110->z);
        u32 powerTarget = gabi::load<u32>(powerVtable+0xEC);
        gabi::call(powerTarget,player,l_HIO.m10 * dVar24,powerAngle,0);
        i_this->m394C = 0;
    }
    gabi::store<f32>(gabi::ea(i_this->mpHeadBrkAnm),i_this->m02C8);
    mDoExt_baseAnm_play(i_this->mpHeadBrkAnm);
    model = i_this->mpTriforcePlatformModel;
    if (i_this->m1864 != 0) {
        i_this->m1864--;
        i_this->m18A0.x = cM_rndFX(REG0_F(18) + 4.0f);
        i_this->m18A0.x = cM_rndFX(REG0_F(18) + 4.0f);
    }
    mDoMtx_stack_c::transS(i_this->m18A0.x, l_HIO.m28 + (i_this->m18A0.y + l_HIO.m2C), i_this->m18A0.z);
    if (i_this->m1865 != 0) {
        mDoMtx_stack_c::transM(0.0f, -20000.0f, 0.0f);
    }
    J3DModel_setBaseTRMtx(model,mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &i_this->mBgwMtx2);
    dBgW_Move(i_this->mpBgW2);
    for (int i = 0; i < 2; i++) {
        if (i_this->m17E4[i] != 0) {
            dBgW_Move(i_this->mpBgW1[i]);
            fVar26 = i_this->m17E0 + 50.0f + REG0_F(8);
            i_this->m3B3C = 0.0f;
            i_this->m3B40 = fVar26;
            i_this->m3B44 = 0.0f;
            i_this->m3B48[i].y = REG0_S(i + 0x6) + gabi::load<s16>(0x10192F20+2*i);
            switch (i_this->m17E4[i]) {
            case 3:
                break;
            case 1:
                i_this->m17E6[i] = 0;
                i_this->m17E4[i] = 2;
                // fallthrough
            case 2:
                if (i_this->m17E6[i] == 0x32) {
                    for (int j = 0; j < 3; j++) {
                        int idx = j + (i * 3);
                        suna_gr_ang()[idx].x = suna_gr_ang()[idx].z = 0;
                        suna_gr_ang()[idx].y = cM_atan2s(-suna_gr_pos()[idx].x, -suna_gr_pos()[idx].z);
                        dComIfGp_particle_setToon(
                            0xA260,
                            &suna_gr_pos()[idx],
                            &suna_gr_ang()[idx],
                            NULL, gabi::load<u8>(eff_col+3),
                            &i_this->m3B54[idx],
                            (s8)actor->current.roomNo
                        );
                        smoke_setColor(&i_this->m3B54[idx],eff_col);
                    }
                }
                if (i_this->m17E6[i] == 0xb4) {
                    model=i_this->m17EC[i];
                    pBtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(gabi::at<char>(0x1000BF8C),0x35,SAFESTRING_VTBL);
                    i_this->m17F4[i]->init(J3DModel_getModelData(model), pBtk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, true, 0);
                    i_this->m17E4[i] = 3;
                }
                break;
            case 4:
                model=i_this->m17EC[i];
                pBtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(gabi::at<char>(0x1000BF8C),0x33,SAFESTRING_VTBL);
                if (i_this->m17F4[i]->init(J3DModel_getModelData(model), pBtk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, true, 0) == 1) {
                    i_this->m17E4[i] = 5;
                    i_this->m17E6[i] = 0;
                }
                break;
            case 5:
                if (i_this->m17E6[i] >= 0x3C) {
                    i_this->m17E4[i] = 0;
                    for (int j = 0; j < 3; j++) {
                        gabi::call(0x025A5F88,&i_this->m3B54[j + (i * 3)]);
                    }
                }
                break;
            }
            i_this->m17E6[i]++;
            i_this->m17F4[i]->play();
            model = i_this->m17EC[i];
            mDoMtx_stack_c::transS(0.0f, REG0_F(3) + -500.0f + l_HIO.m28, 0.0f);
            mDoMtx_stack_c::YrotM(i_this->m3B48[i].y);
            J3DModel_setBaseTRMtx(model,mDoMtx_stack_c::get());
            PSMTXCopy(mDoMtx_stack_c::get(), &i_this->mBgwMtx1[i]);
            if (((i_this->m17E4[i] == 2) && (i_this->m17E6[i] >= 0x32)) || (i_this->m17E4[i] == 3)) {
                model = NULL;
                for (j = 0; j < 3; j++) {
                    mDoAud_seStart(0x7049, &suna_gr_pos()[j + (i * 3)], 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                }
            }
        }
    }
    for (i = 0; i < 6; i++) {
        pos->copy(suna_gr_pos()[i]);
        pos->y += (f32)(1000.0f + REG0_F(13));
        gabi::at<cXyz>(gabi::ea(gndChk.get())+0x24)->copy(*pos.get());
        fVar26 = cBgS_GroundCross(dComIfG_Bgsp(),gndChk.get());
        if (fVar26 != -1.0e9f) {
            suna_gr_pos()[i].y = fVar26;
        }
    }
    if (i_this->m3960 != 0) {
        if (i_this->m3960 == 1) {
            i_this->m3960++;
            gabi::call(0x025A5AC8,&i_this->m3AF4);
            dComIfGp_particle_set(
                0x824D, &i_this->m3954, &actor->shape_angle, NULL, 0xff, (dPa_levelEcallBack*)&i_this->m3AF4, (s8)actor->current.roomNo
            );
        } else if (i_this->m3960 < 0) {
            i_this->m3960 = 0;
            gabi::call(0x025A5AC8,&i_this->m3AF4);
        } else {
            mDoAud_seStart(0x510E, &i_this->m3954, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
    }
    for (i = 0; i < 10; i++) {
        // HD walks ten contiguous timer words, including the named fields after m3AE0[2].
        auto& effectTimer = *gabi::at<be<s16>>(gabi::ea(i_this) + 0x3D58u + (u32)i * 2u);
        if (effectTimer != 0) {
            effectTimer--;
            if (effectTimer == 0) {
                gabi::call(0x025A5F88,&i_this->m3978[i]);
                if (i < 2) {
                    gabi::call(0x025A5AC8,&i_this->m3AB8[i]);
                }
            }
        }
    }
    for (i = 0; i < 2; i++) {
        if (i_this->m3B08[i] != 0) {
            fVar26 = i_this->m3B08[i] - 1.0f;
            i_this->mpGspMorf[i]->setFrame(fVar26);
            gabi::store<f32>(gabi::ea(i_this->mpGspBtkAnm[i])+4,fVar26);
            gabi::store<f32>(gabi::ea(i_this->mpGspBrkAnm[i])+4,fVar26);
            mDoMtx_stack_c::transS(i_this->m3B0C[i].x, i_this->m3B0C[i].y, i_this->m3B0C[i].z);
            mDoMtx_stack_c::scaleM(l_HIO.m30, l_HIO.m34, l_HIO.m30);
            J3DModel_setBaseTRMtx(i_this->mpGspMorf[i]->getModel(),mDoMtx_stack_c::get());
            i_this->m3B08[i]++;
            if (i_this->m3B08[i] > 0x3c) {
                i_this->m3B08[i] = 0;
            }
        }
    }
    if (i_this->m18D8 == 0) {
        i_this->m18D8 = 1;
        fopAcM_create(0xDB, 0, &actor->home.pos, fopAcM_GetRoomNo(actor), NULL, NULL, 0xff, 0);
    }
    if (i_this->m17C4 != 0) {
        if (i_this->m17C4 == 0xFF) {
            cLib_addCalc2(&i_this->m17D4, REG0_F(5) + 0.35f, 0.2f, REG0_F(6) + 0.1f);
            cLib_addCalcAngleS2(&i_this->m17D8, REG0_S(9) + -0x5dc, 1, 0x1e);
            cLib_addCalc2(&i_this->m17DC, 1.0f, 1.0f, 0.1f);
        } else if (i_this->m17C4 == 200) {
            cLib_addCalcAngleS2(&i_this->m17D8, 0, 1, 0x1e);
        } else if (i_this->m17C4 == 0xC9) {
            i_this->m17D4 = REG0_F(9) + -0.3f;
        } else if (i_this->m17C4 == 0xCA) {
            cLib_addCalc2(&i_this->m17D4, REG0_F(4) + -0.7f, 0.1f, 0.005f);
            cLib_addCalcAngleS2(&i_this->m17D8, REG0_S(9) + -500, 1, 0x1e);
            cLib_addCalc2(&i_this->m17DC, 1.0f, 1.0f, 0.005f);
        } else {
            cLib_addCalc2(&i_this->m17D4, -0.75f, 0.1f, 0.01f);
            cLib_addCalcAngleS2(&i_this->m17D8, REG0_S(7) + 1000, 1, 0x1e);
            cLib_addCalc2(&i_this->m17DC, 1.0f, 1.0f, 0.01f);
            i_this->m17C4--;
        }
    } else {
        cLib_addCalc0(&i_this->m17D4, 0.1f, 0.01f);
        cLib_addCalcAngleS2(&i_this->m17D8, 0, 1, 0x14);
        cLib_addCalc0(&i_this->m17DC, 1.0f, 0.01f);
    }
    bwd_kankyo(i_this);
    bd_GndChk_dt(gndChk.get());
    return TRUE;
}


VERIFY(0x020FA614,bwd_execute);
