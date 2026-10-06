// Molgera camera sequence;
#include "d/actor/d_a_bwd.h"

static u32 bc_camera(s32 id){return gabi::load<u32>(dComIfGp_ea()+0x5AF8+(u32)id*0x34);}
static s32 bc_cameraId(){return gabi::load<s8>(dComIfGp_ea()+0x5B30);}
static void bc_trim(u32 c,s32 n){gabi::call(0x02515280,c+0x248,n);}
static cXyz* bc_set_pos(){return gabi::at<cXyz>(0x10462AE8);}
static void bc_setPlayerPos(fopAc_ac_c* p,cXyz* pos,s16 yaw){u32 vt=gabi::load<u32>(gabi::ea(p)+0xB4);gabi::call(gabi::load<u32>(vt+0x114),p,pos,yaw);}
static void bc_originalDemo(fopAc_ac_c* p){dComIfGp_get();gabi::store<u16>(gabi::ea(p)+0x420,3);gabi::store<u32>(gabi::ea(p)+0x428,0);}
static void bc_demoMode(fopAc_ac_c* p,u32 n){gabi::store<u32>(gabi::ea(p)+0x430,n);}
static void bc_bgmStart(u32 n){gabi::call(0x025E18EC,n);}
static void bc_startQuake(){gabi::Local<cXyz> dir;dir->set(0,1,0);auto* vib=dComIfGp_getVibration();gabi::call(0x025CB408,vib,REG0_S(5)+3,-0x21,dir.get());}
static u32 bc_item(cXyz* p,s32 n,s32 room,csXyz* angle,cXyz* scale,s32 mode){return gabi::call<u32>(0x025D8A5C,p,n,room,angle,scale,mode);}
static void bc_warp(cXyz* p,csXyz* angle,s32 room,s32 n){gabi::call(0x025D9874,p,angle,room,n);}
static void bwd_demo_camera(bwd_class* i_this) {
    WWHD_FUNC(0x02101F30,void,i_this);
    fopAc_ac_c* actor = i_this;
    f32 fVar1;
    f32 fVar2;
    s16 sVar3;
    u32  camera2;
    fopAc_ac_c* pfVar7;
    gabi::Local<cXyz> local_98;
    gabi::Local<cXyz> local_a4;
    gabi::Local<cXyz> local_b0;
    gabi::Local<cXyz> local_d4;
    gabi::Local<cXyz> local_e0;

    fopAc_ac_c* player = (fopAc_ac_c*)dComIfGp_getPlayer(0);
    fopAc_ac_c* player2 = (fopAc_ac_c*)player;
    u32  camera = bc_camera(bc_cameraId());
    switch (i_this->m3C1E) {
    case 0:
        break;
    case 1:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            gabi::call(0x025D7B24,actor,2,0xFFFF,0);
            eventInfo_onCondition(actor,2);
            return;
        }
        i_this->m3C1E++;
        camera2 = bc_camera(0);
        i_this->m3C28.copy(*gabi::at<cXyz>(camera2+0xDC));
        i_this->m3C34.copy(*gabi::at<cXyz>(camera2+0xE8));
        gabi::call(0x02514F2C,camera+0x248);
        bc_trim(camera,2);
        i_this->m3C20 = 0;
        i_this->m3C22 = 0;
        i_this->m3C48 = 55.0f;
        i_this->m3C44 = 0.0f;
        i_this->m3C1C = 0x96;
        // fallthrough
    case 2:
        local_98->x = 0.0f;
        local_98->y = 5000.0f;
        local_98->z = 0.0f;
        bc_setPlayerPos(player2,local_98.get(), (int)player->shape_angle.y);
        cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
        local_98->x = 0.0f;
        local_98->y = REG0_F(6) + 500.0f;
        local_98->z = REG0_F(7) + 1500.0f;
        MtxPosition(local_98.get(), local_a4.get());
        gabi::call(0x028E8D88,local_a4.get(),&actor->current.pos,local_a4.get());
        cLib_addCalc2(&i_this->m3C28.x, local_a4->x, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.y, local_a4->y, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.z, local_a4->z, 0.1f, 50.0f);
        local_98->x = 0.0f;
        local_98->y = REG0_F(8) + 400.0f;
        local_98->z = REG0_F(9);
        MtxPosition(local_98.get(), local_a4.get());
        gabi::call(0x028E8D88,local_a4.get(),&actor->current.pos,local_a4.get());
        cLib_addCalc2(&i_this->m3C34.x, local_a4->x, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C34.y, local_a4->y, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C34.z, local_a4->z, 0.1f, 50.0f);
        break;
    case 3:
        local_a4->copy(player->current.pos);
        local_a4->y += 100.0f;
        cLib_addCalc2(&i_this->m3C34.x, local_a4->x, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C34.y, local_a4->y, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C34.z, local_a4->z, 0.1f, 100.0f);
        if (i_this->m3C20 > 0x3c) {
            i_this->m3C1E = 0x96;
        }
        break;
    case 0x32:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            gabi::call(0x025D7B24,actor,2,0xFFFF,0);
            eventInfo_onCondition(actor,2);
            return;
        }
        gabi::call(0x02514F2C,camera+0x248);
        bc_trim(camera,2);
        i_this->m3C1E = 0x33;
        i_this->m3C20 = 0;
        i_this->m3C22 = 0;
        i_this->m3C48 = 55.0f;
        dComIfGp_get();
        i_this->m3C44 = 0.0f;
        gabi::store<u16>(gabi::ea(player2)+0x420,3);
        gabi::store<u32>(gabi::ea(player2)+0x428,0);
        mDoAud_bgmStreamPlay();
        i_this->m3C1C = 100;
    case 0x33:
        i_this->m3C34.set(REG10_F(0)+50.0f, REG10_F(1)+500.0f, REG10_F(2)+1000.0f);
        i_this->m3C28.set(REG10_F(3)+200.0f, REG10_F(4)+488.0f, REG10_F(4)+3575.0f);
        local_98->set(0.0f, player->current.pos.y, REG0_F(4) + 3000.0f);
        bc_setPlayerPos(player2,local_98.get(), -0x8000);
        if (i_this->m3C20 >= 0x1E) {
            if (i_this->m3C20 == 0x1E) {
                bc_startQuake();
            }
            i_this->m3C4C = REG10_F(5) + 5.0f;
            if (i_this->m3C20 == 0x32) {
                bc_demoMode(player2,28);
            }
        }
        if ((int)i_this->m3C22 != REG0_S(3) + 0xaa) {
            break;
        }
        i_this->m3C1E = 0x34;
        i_this->m3C20 = 0;
        i_this->m3C15 = 1;
    case 0x34:
        i_this->m3C4C = REG10_F(6) + 1.5f;
        i_this->m1864 = 10;
        if (i_this->m3C20 >= 0x96) {
            if (i_this->m3C20 == 0x96) {
                i_this->m3C34.set(25.0f, REG0_F(8) + 597.0f, 1578.0f);
            }
            i_this->m3C34.x -= (REG0_F(4) + 0.5f);
            i_this->m3C28.x = -156.0f;
            i_this->m3C28.y = 767.0f;
            i_this->m3C28.z = 2148.0f;
        } else {
            if (i_this->m3C20 == 0x14) {
                i_this->m17E4[1] = 1;
                for (int i = 3; i < 6; i++) {
                    mDoAud_seStart(0x69E1, &suna_gr_pos()[i], 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                }
            }
            i_this->m3C34.x = -1110.0f;
            i_this->m3C34.y = 1598.0f;
            i_this->m3C34.z = -45.0f;
            i_this->m3C28.x = -4426.0f;
            i_this->m3C28.y = 2574.0f;
            i_this->m3C28.z = 888.0f;
            // fallthrough
        }
        if ((int)i_this->m3C22 != REG0_S(5) + 0x280) {
            break;
        }
        i_this->m3C1E = 0x35;
        i_this->m3C20 = 0;
        i_this->m3C48 = 45.0f;
        i_this->m17E0 = 900.0f;
        local_98->set(player->current.pos.x, player->current.pos.y + 400.0f, player->current.pos.z);
        bc_setPlayerPos(player2,local_98.get(), -0x8000);
        // fallthrough
    case 0x35:
        if ((i_this->m3C20 >= 0x46) && (i_this->m3C20 < (s16)(REG0_S(8) + 0x5a))) {
            if (i_this->m3C20 == 0x53) {
                bc_demoMode(player2,49);
            }
            i_this->m3C34.set(player->current.pos.x, (player->eyePos.y + REG0_F(6)) - 30.0f, player->current.pos.z);
            i_this->m3C28.set(player->current.pos.x + REG0_F(7), (player->eyePos.y + REG0_F(8)) - 30.0f, (player->current.pos.z - 200.0f) + REG0_F(9));
        } else {
            i_this->m3C34.x = 6.0f;
            i_this->m3C34.y = 1166.0f;
            i_this->m3C34.z = -319.0f;
            i_this->m3C28.x = 180.0f;
            i_this->m3C28.y = 1362.0f;
            i_this->m3C28.z = 3253.0f;
            cLib_addCalc2(&i_this->m3C48, 55.0f, 0.1f, 0.1f);
        }
        if (i_this->m3C22 < 0x2ee) {
            i_this->m3C4C = REG10_F(7) + 2.0f;
        }
        if ((int)i_this->m3C22 != REG0_S(3) + 0x2f8) {
            break;
        }
        i_this->m3C1E = 0x3c;
        i_this->m3C20 = 0;
        gabi::call(0x025CB610,dComIfGp_getVibration(),-1);
        i_this->m3C1C = 200;
        // fallthrough
    case 0x3C:
        if (i_this->m3C1C > 2) {
            cLib_addCalcAngleS2(&i_this->m3C1C, 1, 1, 1);
        }
        cLib_addCalc2(&i_this->m3C34.x, actor->current.pos.x, 0.1f, 100.0f);
        local_98->y = i_this->mTongueSegments[0].m04.y + REG0_F(10) + 200.0f;
        fVar2 = (REG0_F(12) + 100.0f) + l_HIO.m28;
        if (local_98->y < fVar2) {
            local_98->y = fVar2;
        }
        cLib_addCalc2(&i_this->m3C34.y, local_98->y, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C34.z, actor->current.pos.z, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C28.x, actor->current.pos.x * 0.5f, 0.05f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.y, l_HIO.m28 + 100.0f + REG0_F(15) + 300.0f, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.z, (actor->current.pos.z + 2000.0f) + REG0_F(9), 0.05f, 50.0f);
        if (i_this->m3C20 > 550) {
            cLib_addCalc2(&i_this->m3C48,65.0f,0.05f,0.1f);
            gabi::call(0x027EC9E8,30,360,0x1000BE7C,(f32)i_this->m3C48);
        }
        if ((int)i_this->m3C22 == REG0_S(4) + 0x5b4) {
            i_this->m3C1E = 0x96;
            if (dComIfGp_getStartStageName0() == 'X') {
                bc_bgmStart(0x8000004C);
            } else {
                bc_bgmStart(0x80000029);
            }
            i_this->m3C50 = 1;
            gabi::call(0x025B9098,gabi::load<u32>(0x101F84DC)+0x798,5);
        }
        break;
    case 0x64:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            gabi::call(0x025D7B24,actor,2,0xFFFF,0);
            eventInfo_onCondition(actor,2);
            return;
        }
        gabi::call(0x02514F2C,camera+0x248);
        bc_trim(camera,2);
        i_this->m3C1E = 0x65;
        i_this->m3C20 = 0;
        i_this->m3C22 = 0;
        i_this->m3C48 = 55.0f;
        camera2 = bc_camera(0);
        i_this->m3C28.copy(*gabi::at<cXyz>(camera2+0xDC));
        i_this->m3C34.copy(*gabi::at<cXyz>(camera2+0xE8));
        i_this->m3C44 = 0.0f;
        gabi::store<u16>(gabi::ea(player2)+0x420,3);
        gabi::store<u32>(gabi::ea(player2)+0x428,0);
        bc_set_pos()->x = actor->current.pos.x - 100.0f;
        bc_set_pos()->y = player->current.pos.y;
        bc_set_pos()->z = actor->current.pos.z + 777.0f;
        i_this->m3C1C = 0x96;
        // fallthrough
    case 0x65: {
        gabi::Local<dBgS_GndChk> gndChk;
        dBgS_GndChk_ct(gndChk.get(),BD_GNDCHK_VT,false);
        auto* pos=gabi::at<cXyz>(gabi::ea(gndChk.get())+0x24);
        pos->copy(*bc_set_pos());
        pos->y += 1000.0f;
        fVar1 = cBgS_GroundCross(dComIfG_Bgsp(),gndChk.get());
        if (fVar1 != -1.0e9f) {
            bc_set_pos()->y = fVar1;
        } else {
            bc_set_pos()->y = 3000.0f;
        }
        bc_setPlayerPos(player2,bc_set_pos(), -0x8000);
        bd_GndChk_dt(gndChk.get());
    }
        cLib_addCalc2(&i_this->m3C34.x, actor->current.pos.x, 0.1f, 100.0f);
        local_98->y = i_this->mTongueSegments[0].m04.y + REG0_F(10) + 200.0f;
        fVar2 = (REG0_F(12) + 100.0f) + l_HIO.m28;
        if (local_98->y < fVar2) {
            local_98->y = fVar2;
        }
        cLib_addCalc2(&i_this->m3C34.y, local_98->y, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C34.z, actor->current.pos.z, 0.1f, 100.0f);
        cLib_addCalc2(&i_this->m3C28.x, actor->current.pos.x, 0.05f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.y, l_HIO.m28 + 100.0f + REG0_F(15) + 300.0f, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m3C28.z, (actor->current.pos.z + 2200.0f) + REG0_F(9), 0.05f, 50.0f);
        cLib_addCalcAngleS2(&i_this->m3C1C, 1, 1, 1);
        break;
    case 0x66:
        cLib_addCalc2(&i_this->m3C34.x, i_this->m0418[i_this->m3C24].x, 0.1f, i_this->m3C44 * 200.0f);
        cLib_addCalc2(&i_this->m3C34.y, i_this->m0418[i_this->m3C24].y, 0.1f, i_this->m3C44 * 200.0f);
        cLib_addCalc2(&i_this->m3C34.z, i_this->m0418[i_this->m3C24].z, 0.1f, i_this->m3C44 * 200.0f);
        cLib_addCalc2(&i_this->m3C44, 1.0f, 1.0f, 0.02f);
        if (i_this->m3C20 == 0x32) {
            bc_demoMode(player2,26);
        }
        break;
    case 0x68:
        i_this->m3C48 = REG0_F(6) + 55.0f;
        if (i_this->m3C20 == 0x14) {
            local_b0->x = 1.0f;
            local_b0->y = 1.0f;
            local_b0->z = 1.0f;
            i_this->m18E0 = bc_item(&i_this->m18E4, 0, fopAcM_GetRoomNo(actor), NULL, local_b0.get(), 1);
        }
        if (i_this->m3C20 >= 0x2C) {
            i_this->m3C1E = 0x69;
            i_this->m3C20 = 0;
        }
        break;
    case 0x69:
        i_this->m3C28.x = (player->current.pos.x - 300.0f) + REG0_F(8);
        i_this->m3C28.y = ((player->current.pos.y - 50.0f) + REG0_F(9)) + 200.0f;
        i_this->m3C28.z = (player->current.pos.z - 600.0f) + REG0_F(10);
        i_this->m3C34.x = player->current.pos.x + REG0_F(11);
        i_this->m3C34.y = player->current.pos.y + REG0_F(12);
        i_this->m3C34.z = (player->current.pos.z - 350.0f) + REG0_F(13);
        pfVar7 = fopAcM_SearchByID(i_this->m18E0);
        if ((i_this->m3C20 < 10) && (pfVar7 != NULL)) {
            pfVar7->current.pos.set(player->current.pos.x, player->current.pos.y + 300.0f, player->current.pos.z - 350.0f);
            pfVar7->speed.set(0,0,0);
        }
        if ((s32)i_this->m3C20 == REG_S(10,2)+15) {
            bc_demoMode(player2,50);
        }
        if ((s32)i_this->m3C20 == REG_S(10,3)+53) {
            bc_demoMode(player2,29);
        }
        if ((i_this->m3C20 >= 0x78) && (REG0_S(3) == 0)) {
            i_this->m3C1E = 0x6a;
            i_this->m3C20 = 0;
            actor->current.angle.y = 0;
            i_this->m17C8.x = 0.0f;
            i_this->m17C8.z = 0.0f;
            i_this->m17C4 = 0xca;
            i_this->m1865 = 0;
            i_this->m3C15 = 5;
            i_this->m3C1C = 100;
        }
        break;
    case 0x6A:
        if (i_this->m3C20 == 0x32) {
            i_this->m17E4[0]++;
        }
        if (i_this->m3C20 == 0x6e) {
            i_this->m17E4[1]++;
        }
        cLib_addCalc2(&i_this->m17E0, REG0_F(7) + 400.0f, 0.1f, 1.0f);
        i_this->m3C34.x = 0.0f;
        i_this->m3C34.z = 0.0f;
        i_this->m3C34.y = REG0_F(12) + 200.0f;
        actor->current.angle.y = actor->current.angle.y + gabi::ftoi(i_this->m3C44);
        if ((int)i_this->m3C20 < REG0_S(6) + 400) {
            cLib_addCalc2(&i_this->m3C44, 40.0f, 1.0f, 0.5f);
        }
        if ((int)i_this->m3C20 > REG0_S(6) + 600) {
            cLib_addCalc2(&i_this->m3C44, 0.0f, 1.0f, 0.5f);
        }
        cMtx_YrotS(calc_mtx(), actor->current.angle.y);
        local_98->set(0.0f, REG0_F(13) + 1200.0f, REG0_F(14) + 2600.0f);
        MtxPosition(local_98.get(), &i_this->m3C28);
        if ((int)i_this->m3C20 == REG0_S(6) + 0x208) {
            i_this->m17C4 = 200;
            gabi::call(0x025B9098,gabi::load<u32>(0x101F84DC)+0x798,3);
            local_98->x = 0.0f;
            local_98->y = 300.0f;
            local_98->z = 0.0f;
            bc_warp(local_98.get(), 0, fopAcM_GetRoomNo(actor), 0);
            i_this->m3C50 = 0;
        }
        if ((int)i_this->m3C20 != REG0_S(6) + 0x2a8) {
            break;
        }
        i_this->m3C1E = 0x96;
    case 0x96:
        dComIfGp_event_reset();
        i_this->m3C1E = 0;
        { gabi::Local<cXyz> resetCenter; gabi::Local<cXyz> resetEye;
          resetCenter->copy(i_this->m3C34); resetEye->copy(i_this->m3C28);
          gabi::call(0x0251510C,camera+0x248,resetCenter.get(),resetEye.get()); }
        gabi::call(0x02514F38,camera+0x248);
        bc_trim(camera,0);
        i_this->m3C1C = 1;
        break;
    default:
        break;
    }
    if (i_this->m3C1E != 0) {
        s16 shakeTimer = i_this->m3C20;
        f32 shakeAmount = i_this->m3C4C;
        f32 fx = cM_ssin(shakeTimer * 0x3300) * shakeAmount;
        f32 fy = cM_scos(shakeTimer * 0x3000) * shakeAmount;
        f32 fz = cM_ssin(shakeTimer * 0x3700) * shakeAmount;
        f32 eyeX = i_this->m3C28.x + fx;
        f32 eyeY = i_this->m3C28.y + fy;
        f32 eyeZ = i_this->m3C28.z + fz;
        f32 centerX = i_this->m3C34.x - fx;
        f32 centerY = i_this->m3C34.y - fy;
        f32 centerZ = i_this->m3C34.z - fz;
        sVar3 = 0;
        if (shakeAmount > 0.1f) {
            f32 cosine = cM_scos(shakeTimer * 0x1C00);
            f32 randomBank = cM_rndF(20.0f);
            sVar3 = (s16)gabi::ftoi(cosine * ((REG10_F(8)+50.0f)+randomBank));
        }
        local_e0->set(centerX,centerY,centerZ);
        local_d4->set(eyeX,eyeY,eyeZ);
        gabi::call(0x02514FE8,camera+0x248,local_e0.get(),local_d4.get(),sVar3,(f32)i_this->m3C48);
        cLib_addCalc0(&i_this->m3C4C, 1.0f, REG0_F(16) + 2.0f);
        gabi::call(0x027EC9E8,30,390,0x1000BE88,(s32)i_this->m3C1E);
        gabi::call(0x027EC9E8,30,410,0x1000BE9C,(s32)i_this->m3C22);
        gabi::call(0x027EC9E8,30,430,0x1000BEB0,(s32)i_this->m3C20);
        i_this->m3C20++;
        i_this->m3C22++;
    }
    if ((i_this->m3C1E >= 0x32) && (i_this->m3C1E < 100)) {
        if ((i_this->m3C22 > 0x28) && (i_this->m3C22 < 0x2da)) {
            mDoAud_seStart(0x105E, NULL, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
        if ((i_this->m3C22 > 0x154) && (i_this->m3C22 < 700)) {
            mDoAud_seStart(0x1077, center_pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        }
    }
    if (((i_this->m3C1E >= 100) && (i_this->m3C22 >= 700)) && (i_this->m3C22 <= 0x4b0)) {
        mDoAud_seStart(0x1076, center_pos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
    }
}


VERIFY(0x02101F30,bwd_demo_camera);
