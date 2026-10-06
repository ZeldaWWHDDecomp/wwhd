// Molgera action dispatch and HD-inlined actions.
#include "d/actor/d_a_bwd.h"

static void bd_seStartCurrent(fopAc_ac_c* a,u32 id,u32 param){if(gabi::ea(&a->current.pos)) {s32 reverb=dComIfGp_getReverb(fopAcM_GetRoomNo(a));mDoAud_seStart(id,&a->current.pos,param,reverb);}}
static f32 bd_horizontalMag(cXyz* p){f32 z=p->z,x=p->x;return std_sqrtf(gabi::fmadds(x,x,z*z));}
static f32 bd_square(f32 x){return x*x;}
static void bd_sub(cXyz* out,const cXyz* a,const cXyz* b){gabi::call(0x0201ADE0,a,out,b);}
static void dComIfGp_getVibration_StopQuake(s32 n){auto* v=dComIfGp_getVibration();gabi::call(0x025CB610,v,n);}
static void mDoAud_bgmStop(u32 n){gabi::call(0x025E1904,n);}
static void mDoAud_bgmStart(u32 n){gabi::call(0x025E18EC,n);}
static void bd_monsSeStart(fopAc_ac_c* a,u32 id,u32 param){if(!a || !gabi::ea(&a->eyePos)) return;u32 actorId=fopAcM_GetID(a);s32 reverb=dComIfGp_getReverb(fopAcM_GetRoomNo(a));gabi::call(0x025E1AA4,id,&a->eyePos,actorId,param,reverb);}
static s32 bd_throwDamage(fopAc_ac_c* p,cXyz* pos,s16 yaw,f32 x,f32 y,s32 mode){u32 vt=gabi::load<u32>(gabi::ea(p)+0xB4);u32 fn=gabi::load<u32>(vt+0x12C);return gabi::call<s32>(fn,p,pos,yaw,x,y,mode);}
static void bd_setPlayerPos(fopAc_ac_c* p,cXyz* pos,s16 yaw){u32 vt=gabi::load<u32>(gabi::ea(p)+0xB4);u32 fn=gabi::load<u32>(vt+0x114);gabi::call(fn,p,pos,yaw);}

static void start(bwd_class* i_this) {
    fopAc_ac_c* actor = i_this;

    fopAc_ac_c* player = (fopAc_ac_c*)dComIfGp_getPlayer(0);
    switch (i_this->m18B0) {
    case 0:
        i_this->m17C4 = 0xC9;
        if (std_sqrtf(PSVECSquareMag(&player->current.pos)) < 3200.0f) {
            i_this->m18B0 = 1;
            i_this->m18AC = 0;
            i_this->m3C1E = 0x32;
        }
        break;
    case 1:
        if (i_this->m18AC == 0x3C) {
            i_this->m17E4[0] = 1;
            for (int i = 0; i < 3; i++) {
                mDoAud_seStart(0x69E1, &suna_gr_pos()[i], 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            }
        }
        if (i_this->m18AC == 200) {
            i_this->m17D8 = REG0_S(4) + 500;
        }
        if (i_this->m18AC >= 0xDC) {
            cLib_addCalc2(&i_this->m17E0, 900.0f, 0.1f, REG0_F(13) + 0.9f);
        }
        if (i_this->m18AC == 0x28f) {
            i_this->m1865 = 1;
        }
        if (i_this->m18AC >= 600) {
            i_this->m394C = 1;
            if (i_this->m18AC == 0x294) {
                i_this->m18AE = 0xB;
                i_this->m18B0 = 0;
                actor->current.pos.x = 0.0f;
                actor->current.pos.y = ((l_HIO.m28 + -2000.0f) - 200.0f) + REG0_F(11);
                actor->current.pos.z = 0.0f;
                i_this->m18CC[1] = 0x28;
                bwd_g_eff_on(i_this);
                dComIfGp_getVibration_StopQuake(-1);
            }
        }
        break;
    }
    i_this->m1BB6 = 1;
}

/* 000018E8-00001AEC       .text wait__FP9bwd_class */
static void wait(bwd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = (fopAc_ac_c*)dComIfGp_getPlayer(0);
    i_this->m394C = 1;
    i_this->m17C8.copy(actor->current.pos);
    i_this->m17C4 = 1;
    actor->current.angle.z = 0;
    actor->current.angle.x = 0;
    if (i_this->m1BB8 > 80.0f) {
        cLib_addCalcAngleS2(&actor->current.angle.y, fopAcM_searchPlayerAngleY(actor), 10, 0x800);
    }
    switch (i_this->m18B0) {
    case -10:
        if (!(player->current.pos.y < 900.0f)) {
            break;
        }
        i_this->m18CC[0] = 0x7a;
        i_this->m18D0 = 0xde;
        i_this->m18B0 = 1;
        break;
    case 0:
        i_this->m18B0++;
    case 1:
        if (i_this->m18CC[0] == 0) {
            bwd_anm_init(i_this, 0x17, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            i_this->m18B0++;
            mDoAud_seStart(0x5913, &i_this->m3954, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        } else {
            if (i_this->m18CC[0] == 0x78) {
                bwd_g_eff_on(i_this);
            }
            break;
        }
    case 2:
        cLib_addCalc2(&actor->current.pos.y, l_HIO.m0C + l_HIO.m28, 0.1f, 20.0f);
        if (i_this->m18D0 == 0) {
            if (i_this->m1BB8 < l_HIO.m18) {
                i_this->m18AE = 5;
                i_this->m18B0 = 0;
            } else {
                if (i_this->m1BB8 > l_HIO.m1C) {
                    i_this->m18AE = 2;
                    i_this->m18B0 = 0;
                }
            }
        }
        break;
    }
}

/* 00001AEC-00001CBC       .text reset__FP9bwd_class */
static void reset(bwd_class* i_this) {
    fopAc_ac_c* actor = i_this;

    fopAc_ac_c* player = (fopAc_ac_c*)dComIfGp_getPlayer(0);
    switch (i_this->m18B0) {
    case 0:
        i_this->m18B0++;
        i_this->m18CC[0] = 0x96;
        // fallthrough
    case 1:
        cLib_addCalc2(&actor->current.pos.y, REG0_F(17) + -1500.0f + l_HIO.m28, 1.0f, 10.0f);
        if (i_this->m18CC[0] == 100) {
            bwd_g_eff_off(i_this);
        }
        if (i_this->m18CC[0] == 0) {
            i_this->m18AE = 1;
            i_this->m18B0 = 0;
            i_this->m18D0 = 0xde;
            i_this->m18CC[0] = 0x7a;
            { f32 randomOffset = cM_rndFX(300.0f); actor->current.pos.x = player->current.pos.x + randomOffset; }
            actor->current.pos.y = l_HIO.m28 + -1500.0f;
            { f32 randomOffset = cM_rndFX(300.0f); actor->current.pos.z = player->current.pos.z + randomOffset; }
            while (true) {
                if (bd_horizontalMag(&actor->current.pos) < REG0_F(18) + 3000.0f) {
                    break;
                }
                actor->current.pos.x *= 0.9f;
                actor->current.pos.z *= 0.9f;
            }
        }
        break;
    }
}

/* 00001CBC-000026A4       .text sita_hit__FP9bwd_class */
static void sita_hit(bwd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<csXyz> local_40;
    gabi::Local<cXyz> local_2c;
    gabi::Local<cXyz> local_38;

    fopAc_ac_c* player = (fopAc_ac_c*)dComIfGp_getPlayer(0);
    int frame = gabi::ftoi(i_this->mpHeadMorf->getFrame());
    i_this->m17C4 = 200;
    switch (i_this->m18B0) {
    case 0:
        bwd_anm_init(i_this, 0x13, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->m18B0++;
        i_this->m1BAC = 1.0f;
        // fallthrough
    case 1:
        if (!i_this->mpHeadMorf->isStop()) {
            break;
        }
        bwd_anm_init(i_this, 0x14, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        i_this->m18B0++;
        break;
    case 2:
        if (i_this->m1BB1 == 0) {
            i_this->m18AE = 1;
            i_this->m18B0 = 0;
            i_this->m18CC[0] = 0;
            break;
        }
        if (i_this->m1BB4 < 3) {
            break;
        }
        i_this->m1BB5++;
        dComIfGp_particle_set(0x0010, &i_this->mTongueSegments[29].m04);
        local_38->x = local_38->y = local_38->z = 2.0f;
        local_40->z = 0;
        local_40->x = 0;
        local_40->y = fopAcM_searchPlayerAngleY(actor);
        dComIfGp_particle_set(0x000D, &i_this->mTongueSegments[29].m04, local_40.get(), local_38.get());
        if ((i_this->m1BB5 >= 4) || (l_HIO.m06 != 0)) {
            gabi::store<f32>(dComIfGp_ea()+0x5B44,0.0f); // HD clears this play-state value on the death transition.
            bd_monsSeStart(actor, 0x48E1, 0);
            i_this->m18AE = 0xc;
            i_this->m18B0 = 0;
            i_this->m1BB2 = 0x32;
            gabi::store<u8>(0x101EACB7,8);
            mDoAud_bgmStop(30);
            i_this->m1710 = 0;
            break;
        }
        gabi::store<u8>(0x101EACB7,4);
        bwd_anm_init(i_this, 0xA, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->m18B0++;
        i_this->m1BB1 = 0;
        i_this->m1BB4 = 0;
        i_this->m1BB2 = 0x32;
        bd_monsSeStart(actor, 0x48DF, 0);
        break;
    case 3:
        if (!i_this->mpHeadMorf->isStop()) {
            break;
        }
        bwd_anm_init(i_this, 0xB, 1.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->m18B0++;
        i_this->m18CC[0] = 300;
        i_this->m3954.x = actor->current.pos.x;
        i_this->m3954.z = actor->current.pos.z;
        dComIfGp_particle_set(0x8253, &i_this->m3954, &actor->shape_angle);
        i_this->m3964 = dComIfGp_particle_set(0x8252, &i_this->m3954);
        dComIfGp_particle_set(0x8254, &i_this->m3954, &actor->shape_angle);
        dComIfGp_particle_set(0x8255, &i_this->m3954, &actor->shape_angle);
        if (i_this->m3AE4 == 0) {
            i_this->m3AE4 = 0x4d;
            i_this->m3954.x = actor->current.pos.x;
            i_this->m3954.z = actor->current.pos.z;
            dComIfGp_particle_setToon(
                0xA256, &i_this->m3954, &actor->shape_angle, NULL, gabi::load<u8>(eff_col+3), &i_this->m3978[2], (s8)actor->current.roomNo
            );
            smoke_setColor(&i_this->m3978[2],eff_col);
        }
        // fallthrough
    case 4:
        if (i_this->m3964 != NULL) {
            if (i_this->mpHeadMorf->isStop()) {
                JPA_becomeInvalidEmitter(i_this->m3964);
                i_this->m3964 = NULL;
            } else {
                JPA_setGlobalRTMatrix(i_this->m3964,bd_getAnmMtx(i_this->mpHeadMorf->getModel(),0xE));
            }
        }
        i_this->m1BB6 = 1;
        int r0 = gabi::ftoi(i_this->mpHeadMorf->getFrame());
        if (r0 == 0x32) {
            bwd_g_eff_off(i_this);
        }
        if (frame == 0x30) {
            mDoAud_seStart(0x591C, &i_this->m3954, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
        if (((((frame == 0x30) && (l_HIO.m24 >= 1)) || ((frame == 0x31) && (l_HIO.m24 >= 2)) || (frame == 0x32 && (l_HIO.m24 >= 3))) ||
             (frame == 0x33 && (l_HIO.m24 >= 4))) &&
            (ko_count < l_HIO.m26))
        {
            local_2c->copy(i_this->mTongueSegments[0].m04);
            local_2c->y = REG0_F(9) + -600.0f + l_HIO.m28;
            fopAcM_create(0xDA,0x23,local_2c.get(),fopAcM_GetRoomNo(actor),nullptr,nullptr,-1,0);
            if (i_this->m1710 == 0) {
                if (dComIfGp_getStartStageName0() == 'X') {
                    mDoAud_bgmStart(0x80000152);
                } else {
                    mDoAud_bgmStart(0x80000124);
                }
                i_this->m1710 = 10;
            }
        }
        if (i_this->m18CC[0] == 0) {
            if ((i_this->m1BB5 >= 2) || (l_HIO.m07 != 0)) {
                i_this->m18AE = 10;
                i_this->m18B0 = 0;
                { f32 randomOffset = cM_rndFX(300.0f); actor->current.pos.x = player->current.pos.x + randomOffset; }
                actor->current.pos.y = l_HIO.m28 + -2500.0f;
                { f32 randomOffset = cM_rndFX(300.0f); actor->current.pos.z = player->current.pos.z + randomOffset; }
                while (true) {
                    if (bd_horizontalMag(&actor->current.pos) < REG0_F(18) + 3000.0f) {
                        break;
                    }
                    actor->current.pos.x *= 0.9f;
                    actor->current.pos.z *= 0.9f;
                }
                i_this->m18B4.x = actor->current.pos.x;
                i_this->m3954.x = actor->current.pos.x;
                i_this->m18B4.y = (l_HIO.m28 + 3000.0f) + REG10_F(3);
                i_this->m3954.z = actor->current.pos.z;
                actor->current.angle.x = -0x4000;
                i_this->m18B4.z = actor->current.pos.z;
                i_this->m18CC[1] = 0x28;
                for (int i = 0; i < 256; i++) {
                    i_this->m0508[i] = actor->current.pos;
                }
                bwd_g_eff_on(i_this);
            } else {
                i_this->m18AE = 1;
                i_this->m18B0 = 0;
                i_this->m18D0 = 0xde;
                i_this->m18CC[0] = 0x7a;
                { f32 randomOffset = cM_rndFX(300.0f); actor->current.pos.x = player->current.pos.x + randomOffset; }
                actor->current.pos.y = l_HIO.m28 + -1500.0f;
                { f32 randomOffset = cM_rndFX(300.0f); actor->current.pos.z = player->current.pos.z + randomOffset; }
                while (true) {
                    if (bd_horizontalMag(&actor->current.pos) < REG0_F(18) + 3000.0f) {
                        break;
                    }
                    actor->current.pos.x *= 0.9f;
                    actor->current.pos.z *= 0.9f;
                }
            }
        }
        break;
    }
}

/* 000026A4-00002F30       .text eat_attack__FP9bwd_class */
static void eat_attack(bwd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    s8 bVar2;
    gabi::Local<cXyz> local_38;

    fopAc_ac_c* player_actor = dComIfGp_getPlayer(0);
    fopAc_ac_c* player = (fopAc_ac_c*)player_actor;
    s8 cVar10 = 0;
    i_this->m17C8.copy(actor->current.pos);
    i_this->m17C4 = 1;
    if ((i_this->m3C1E == 0) && (i_this->m1BB8 > 80.0f)) {
        cLib_addCalcAngleS2(&actor->current.angle.y, fopAcM_searchPlayerAngleY(actor), 10, 0x800);
    }
    i_this->m18D6 = 5;
    i_this->m1BB6 = 1;
    bVar2 = false;
    switch (i_this->m18B0) {
    case 0:
        bwd_anm_init(i_this, 0x7, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        i_this->m18B0++;
        i_this->m3954.x = actor->current.pos.x;
        i_this->m3954.z = actor->current.pos.z;
        dComIfGp_particle_set(0x824E, &i_this->m3954, &actor->shape_angle);
        bd_seStart(actor, 0x48E0, 0);
        break;
    case 1:
        if (!(i_this->mpHeadMorf->getFrame() < 27.0f)) {
            if ((gabi::ftoi(i_this->mpHeadMorf->getFrame()) == 0x1b) && (i_this->m3AE6 == 0)) {
                i_this->m3AE6 = REG0_S(4) + 10;
                gabi::call(0x025A5F88,&i_this->m3978[3]);
                i_this->m3954.x = actor->current.pos.x;
                i_this->m3954.z = actor->current.pos.z;
                dComIfGp_particle_setToon(
                    0xA24F, &i_this->m3954, &actor->shape_angle, NULL, gabi::load<u8>(eff_col+3), &i_this->m3978[3], (s8)actor->current.roomNo
                );
                smoke_setColor(&i_this->m3978[3],eff_col);
            }
            for (int i = 0; i < 9; i++) {
                if (i_this->mBodySph[i].ChkAtHit()) {
                    bVar2 = true;
                    break;
                }
            }
            if (i_this->m1BB8 < REG0_F(2) + 150.0f) {
                bVar2 = true;
            }
        }
        if (bVar2) {
            bwd_anm_init(i_this, 0x8, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            i_this->m18B0 = 5;
            i_this->m18CC[0] = 0x96;
            i_this->m3C1E++;
            i_this->m3954.x = actor->current.pos.x;
            i_this->m3954.z = actor->current.pos.z;
            dComIfGp_particle_set(0x8250, &i_this->m3954, &actor->shape_angle);
            if (i_this->m3AE8 == 0) {
                i_this->m3AE8 = 0x3c;
                gabi::call(0x025A5F88,&i_this->m3978[4]);
                i_this->m3954.x = actor->current.pos.x;
                i_this->m3954.z = actor->current.pos.z;
                dComIfGp_particle_setToon(
                    0xA251, &i_this->m3954, &actor->shape_angle, NULL, gabi::load<u8>(eff_col+3), &i_this->m3978[4], (s8)actor->current.roomNo
                );
                smoke_setColor(&i_this->m3978[4],eff_col);
            }
            bd_seStart(actor, 0x5915, 0);
        } else {
            if (i_this->mpHeadMorf->isStop()) {
                bwd_anm_init(i_this, 0x9, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
                bd_seStart(actor, 0x5915, 0);
                i_this->m18B0++;
            }
        }
        break;
    case 2:
        if (i_this->mpHeadMorf->isStop()) {
            cVar10 = 1;
        }
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        if (gabi::ftoi(i_this->mpHeadMorf->getFrame()) == 10) {
            bd_seStart(actor, 0x5916, 0);
        }
        if (gabi::ftoi(i_this->mpHeadMorf->getFrame()) == 30) {
            bd_seStart(actor, 0x5914, 0);
        }
        if (gabi::ftoi(i_this->mpHeadMorf->getFrame()) == 50) {
            bwd_g_eff_off(i_this);
        }
        if (i_this->m18CC[0] == 0x3c) {
            local_38->copy(i_this->mTongueSegments[0].m04);
            local_38->y = i_this->m3954.y + REG0_F(9);
            if (bd_throwDamage(player,local_38.get(),actor->shape_angle.y,REG0_F(7)+60.0f,REG0_F(8)+130.0f,0) == 1) {
                i_this->m3C1E++;
                i_this->m3C20 = 0;
                if (i_this->m3AEA == 0) {
                    i_this->m3AEA = 0x14;
                    gabi::call(0x025A5F88,&i_this->m3978[5]);
                    i_this->m3954.x = actor->current.pos.x;
                    i_this->m3954.z = actor->current.pos.z;
                    dComIfGp_particle_setToon(
                        0xA24A, &i_this->m3954, NULL, NULL, gabi::load<u8>(eff_col+3), &i_this->m3978[5], (s8)actor->current.roomNo
                    );
                    smoke_setColor(&i_this->m3978[5],eff_col);
                }
                dComIfGp_particle_set(0x8249, &player_actor->current.pos);
                mDoAud_seStart(0x5917, &i_this->m3954, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            } else {
                local_38->copy(i_this->mTongueSegments[0].m04);
                local_38->y = REG0_F(9) + -450.0f + l_HIO.m28;
                bd_setPlayerPos(player,local_38.get(),player_actor->shape_angle.y);
                i_this->m3C1E = 0x96;
            }
        }
        if (i_this->m18CC[0] == 0) {
            cVar10 = 2;
        }
        break;
    }
    if (cVar10 != 0) {
        i_this->m18AE = 1;
        i_this->m18B0 = 0;
        if (cVar10 == 1) {
            i_this->m18D0 = 100;
        } else {
            i_this->m18CC[0] = 0x7a;
            i_this->m18D0 = 0xde;
        }
    }
}

/* 00002F30-00003694       .text fly__FP9bwd_class */
static void fly(bwd_class* i_this) {
    fopAc_ac_c* actor = i_this;
    f32 dVar7;
    f32 dVar8;
    f32 fVar9;
    gabi::Local<cXyz> local_48;

    fopAc_ac_c* player = (fopAc_ac_c*)dComIfGp_getPlayer(0);
    i_this->m18D6=10; // HD refreshes this timer while flying.
    if (actor->current.pos.y > 1000.0f) {
        i_this->m3C50 = 2;
    }
    switch (i_this->m18B0) {
    case 0:
        if (i_this->m18CC[1] == 0) {
            bwd_anm_init(i_this, 0xF, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            i_this->m18B0 = 1;
            i_this->m18CC[0] = REG_S(10,1)+50;
            i_this->m18CC[1] = (s16)gabi::ftoi((cM_rndF(50.0f) + 200.0f) + REG0_F(18));
            i_this->m1904 = REG0_S(0) + 5;
            i_this->m18D0 = REG0_S(4) + 0xb4;
        } else {
            i_this->m17C8.copy(i_this->m3954);
            i_this->m17C4 = 0xff;
            break;
        }
    case 1:
        if (actor->current.pos.y > i_this->m3954.y) {
            bd_seStart(actor, 0x5110, 0);
            if (gabi::ftoi(i_this->mpHeadMorf->getFrame()) == 2) {
                bd_seStartCurrent(actor, 0x5911, 0);
            }
            if (gabi::ftoi(i_this->mpHeadMorf->getFrame()) == 0x14) {
                bd_seStartCurrent(actor, 0x48DB, 0);
            }
        }
        if (i_this->m18D0 == 0x50) {
            bwd_g_eff_off(i_this);
        }
        if (actor->current.pos.y < i_this->m3954.y) {
            i_this->m17C8.copy(i_this->m3954);
            i_this->m17C4 = 0xff;
        } else {
            if (i_this->m18D0 == 0) {
                i_this->m17C4 = 200;
            }
        }
        bd_sub(local_48.get(),&i_this->m18B4,&actor->current.pos);
        if ((std_sqrtf(PSVECSquareMag(local_48.get())) < REG0_F(14) + 500.0f) || (i_this->m18CC[0] == 0)) {
            i_this->m18CC[0] = 100;
            i_this->m18B4.x = cM_rndFX(REG0_F(16) + 1500.0f);
            f32 targetYBase = REG0_F(15) + 2000.0f;
            f32 targetYRandom = cM_rndFX(800.0f);
            i_this->m18B4.y = (targetYBase + targetYRandom) + l_HIO.m28;
            i_this->m18B4.z = cM_rndFX(REG0_F(16) + 1500.0f);
            i_this->m18C8 = 0.0f;
        }
        i_this->m18C4 = REG0_F(18) + 500.0f;
        actor->speedF = REG0_F(17) + 40.0f;
        if (i_this->m18CC[1] == 0) {
            i_this->m18B0 = 2;
            i_this->m18CC[0] = 0x96;
            bwd_anm_init(i_this, 0x10, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            bd_seStart(actor, 0x48DC, 0);
        }
        break;
    case 2:
        bd_seStart(actor, 0x5110, 0);
        if (i_this->mpHeadMorf->isStop()) {
            bwd_anm_init(i_this, 0x11, 2.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        }
        i_this->m18B4.copy(player->eyePos);
        bd_sub(local_48.get(),&i_this->m18B4,&actor->current.pos);
        if ((std_sqrtf(PSVECSquareMag(local_48.get())) < REG0_F(14) + 1000.0f) || (i_this->m18CC[0] == 0)) {
            i_this->m18B0 = 3;
            i_this->m18B4.y -= 10000.0f;
            i_this->m18CC[0] = 200;
            bwd_anm_init(i_this, 0x12, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        }
        cLib_addCalc2(&i_this->m18C4, REG0_F(15) + 800.0f, 1.0f, 10.0f);
        break;
    case 3:
        bd_seStart(actor, 0x5110, 0);
        cLib_addCalc2(&i_this->m18C4, REG0_F(12) + 1200.0f, 1.0f, 20.0f);
        if (i_this->m18CC[0] == 0) {
            i_this->m18AE = 1;
            i_this->m18B0 = 0;
            i_this->m18CC[0] = 0x7a;
            i_this->m18D0 = 0xde;
            actor->current.pos.set(cM_rndFX(1500.0f), REG0_F(17) + -1500.0f + l_HIO.m28, cM_rndFX(1500.0f));
        }
        break;
    }

    bwd_fly_pos_move(i_this, 0, 0);
    i_this->m170C = 1;
    i_this->m1BB6 = 2;
}

/* 00003694-00003D3C       .text s_fly__FP9bwd_class */
static void s_fly(bwd_class* i_this) {
    dComIfGp_get(); // HD retains the singleton accessor even though its result is unused.
    fopAc_ac_c* actor = i_this;
    f32 fVar6;
    gabi::Local<cXyz> local_28;

    switch (i_this->m18B0) {
    case 0:
        if (i_this->m18CC[1] == 0) {
            bwd_anm_init(i_this, 0xF, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
            i_this->m18B0 = 1;
            i_this->m18B4.x = 0.0f;
            i_this->m18B4.y = l_HIO.m28 + 3000.0f;
            i_this->m18B4.z = 0.0f;
            actor->current.angle.x = -0x4000;
            i_this->m18D0 = REG0_S(4) + 0xb4;
            i_this->m1904 = REG0_S(0) + 5;
        } else {
            i_this->m17C8.copy(i_this->m3954);
            i_this->m17C4 = 0xff;
            break;
        }
    case 1:
        if (actor->current.pos.y > i_this->m3954.y) {
            bd_seStart(actor, 0x5110, 0);
            if (gabi::ftoi(i_this->mpHeadMorf->getFrame()) == 2) {
                bd_seStartCurrent(actor, 0x5911, 0);
            }
            if (gabi::ftoi(i_this->mpHeadMorf->getFrame()) == 20) {
                bd_seStartCurrent(actor, 0x48DB, 0);
            }
        }
        if (i_this->m18D0 == 0x50) {
            bwd_g_eff_off(i_this);
        }
        if (actor->current.pos.y < i_this->m3954.y) {
            i_this->m17C8.copy(i_this->m3954);
            i_this->m17C4 = 0xff;
        } else {
            if (i_this->m18D0 == 0) {
                i_this->m17C4 = 200;
            }
        }
        bd_sub(local_28.get(),&i_this->m18B4,&actor->current.pos);
        if (std_sqrtf(PSVECSquareMag(local_28.get())) < REG0_F(14) + 500.0f) {
            if (i_this->m3C24 == 3) {
                i_this->m18B0 = 2;
                i_this->m18CC[0] = 0x96;
                bwd_anm_init(i_this, 0x10, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
                bd_seStart(actor, 0x48DC, 0);
            } else {
                i_this->m18B4.x = gabi::load<f32>(0x10192F90 + (u32)(s32)i_this->m3C24*4);
                i_this->m18B4.y = gabi::load<f32>(0x10192F9C + (u32)(s32)i_this->m3C24*4);
                i_this->m18B4.z = gabi::load<f32>(0x10192FA8 + (u32)(s32)i_this->m3C24*4);
                i_this->m18C8 = 0.0f;
                i_this->m3C24++;
            }
        }
        i_this->m18C4 = REG0_F(18) + 500.0f;
        actor->speedF = REG0_F(17) + 40.0f;
        break;
    case 2:
        if (actor != nullptr) bd_seStart(actor, 0x5110, 0);
        if (i_this->mpHeadMorf->isStop()) {
            bwd_anm_init(i_this, 0x11, 2.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        }
        i_this->m18B4.x = 0.0f;
        i_this->m18B4.y = 800.0f;
        i_this->m18B4.z = 0.0f;
        bd_sub(local_28.get(),&i_this->m18B4,&actor->current.pos);
        if (std_sqrtf(PSVECSquareMag(local_28.get())) < REG0_F(14) + 1000.0f) {
            i_this->m18B0 = 3;
            i_this->m18B4.y -= 10000.0f;
            i_this->m18CC[0] = 200;
            bwd_anm_init(i_this, 0x12, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        }
        cLib_addCalc2(&i_this->m18C4, REG0_F(15) + 800.0f, 1.0f, 10.0f);
        break;
    case 3:
        if (actor != nullptr) bd_seStart(actor, 0x5110, 0);
        cLib_addCalc2(&i_this->m18C4, REG0_F(12) + 1200.0f, 1.0f, 20.0f);
        if (i_this->m18CC[0] == 0) {
            i_this->m18AE = 1;
            i_this->m18B0 = 0;
            i_this->m18CC[0] = 0xac;
            i_this->m18D0 = 0x12e;
            fVar6 = REG0_F(17) + -1500.0f + l_HIO.m28;
            actor->current.pos.x = 0.0f;
            actor->current.pos.y = fVar6;
            actor->current.pos.z = 0.0f;
        }
        break;
    }
    bwd_fly_pos_move(i_this, 0, 0);
    i_this->m170C = 1;
    i_this->m1BB6 = 2;
}


static void bwd_move(bwd_class* i_this) {
    WWHD_FUNC(0x020FF80C,void,i_this);
    fopAc_ac_c* actor = i_this;
    attn_flags(actor) = 4;
    switch (i_this->m18AE) {
    case 0:
        start(i_this);
        break;
    case 1:
        wait(i_this);
        break;
    case 2:
        reset(i_this);
        break;
    case 3:
        sita_hit(i_this);
        break;
    case 5:
        eat_attack(i_this);
        break;
    case 10:
        fly(i_this);
        break;
    case 11:
        s_fly(i_this);
        break;
    case 12:
        gabi::call(0x020FEA30,i_this);
    case 13:
        break;
    }
}


VERIFY(0x020FF80C,bwd_move);
