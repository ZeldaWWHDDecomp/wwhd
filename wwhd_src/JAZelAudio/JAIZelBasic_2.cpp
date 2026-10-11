/** Zelda audio BGM and sound-effect control. Verified against the HD image; private derived source.
 */
#include "JAZelAudio/JAIZelBasic.h"
using namespace jaizel12;

namespace jaizel12 {
// HD: level-object mixing stores15 sound kinds with20 instances each.
/* 0201E74C */
void JAIZelBasic::setLevObjSE(u32 id, cXyz *pos, u32 reverb) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    AudioCamera *cam = camera();
    Mtx34 *mtx = cameraMatrix(cam);
    gabi::Local<VectorWords> vv;
    VectorWords *v = vv.get();
    if (pos == 0) {
        /* default (0, 0, -50) unless the camera has a position */
        v->w[0] = l_defaultLevelPosition.w[0].get();
        v->w[1] = l_defaultLevelPosition.w[1].get();
        v->w[2] = l_defaultLevelPosition.w[2].get();
        cXyz *cp = cameraPosition(cam);
        if (cp != 0) {
            v->w[0] = reinterpret_cast<VectorWords *>(cp)->w[0].get();
            v->w[1] = reinterpret_cast<VectorWords *>(cp)->w[1].get();
            v->w[2] = reinterpret_cast<VectorWords *>(cp)->w[2].get();
        }
    } else {
        v->w[0] = reinterpret_cast<VectorWords *>(pos)->w[0].get();
        v->w[2] = reinterpret_cast<VectorWords *>(pos)->w[2].get();
        v->w[1] = reinterpret_cast<VectorWords *>(pos)->w[1].get();
    }
    if ((s32)id == JA_SE_FIREBLAST_NOISE) {
        /* not above the camera */
        f32 cy = cameraHeight(cam);
        if (reinterpret_cast<cXyz *>(v)->y.get() > cy)
            reinterpret_cast<cXyz *>(v)->y = cy;
    }
    if (mtx != 0)
        transformSoundVector(mtx, v, v);
    f32 scale = 1.0f;
    if (id < JA_SE_CM_BKM_ATKVINE_DIG) {
        if (id == JA_SE_ATM_RAKUBAN)
            scale = 4.0f;
        else if (id < JA_SE_ATM_RIVER_M)
            scale = 1.0f;
        else if (id <= JA_SE_ATM_RIVER_S)
            scale = 3.0f;
        else if (id == JA_SE_CM_BTD_HIFUKI_BEF)
            scale = 3.0f;
        else if (id == JA_SE_CM_BTD_HIFUKI)
            scale = 4.0f;
    } else if (id >= JA_SE_FIREBLAST_NOISE) {
        if (id == JA_SE_FIREBLAST_NOISE)
            scale = 3.0f;
        else if (id == JA_SE_ATM_WATER_GATE)
            scale = 6.0f;
        else if (id == JA_SE_CM_BST_HORI)
            scale = 0.5f;
    } else if (id <= JA_SE_CM_BKM_ATKVINE_L_ATK) {
        scale = 4.0f;
    } else if (id == JA_SE_OBJ_TORCH_BURNING) {
        scale = 0.5f;
    } else if (id == JA_SE_OBJ_STEAM) {
        scale = 0.4f;
    }
    f32 vol = calcPosVolume(v, scale);
    f32 lr = calcPosPanLR(v);
    f32 sr = calcPosPanSR(v, 1.0f);
    if (vol == 0.0f)
        return;
    u32 n = mLevelObjectSoundCountHD.get();
    u32 i = 0;
    for (; i < n; i++)
        if (indexed(mLevelObjectSoundsHD, i)->id.get() == id)
            break;
    LevelObjectSound *e = indexed(mLevelObjectSoundsHD, i);
    if (i == n) {
        if ((s32)n == 0xF)
            return;
        e->id = id;
        mLevelObjectSoundCountHD = mLevelObjectSoundCountHD.get() + 1;
    }
    s32 c = e->count;
    if (c == 0x14)
        return;
    LevelObjectInstance *slot = indexed(e->instances, u32(c));
    slot->volume = vol;
    slot->pan = lr;
    slot->surround = sr;
    slot->reverb = reverb;
    e->count = e->count.get() + 1;
}

// The sub-BGM fade factor was just set to1; preserve the remaining multiply order.
static inline f32 calcMainBgmVolumeWithoutSubFade(JAIZelBasic *basic) {
    f32 v = mul(basic->field_0x0080.get(), basic->field_0x0084.get());
    v = mul(v, basic->field_0x0088.get());
    v = mul(v, basic->field_0x0090.get());
    v = mul(v, basic->field_0x0094.get());
    v = mul(v, basic->field_0x0098.get());
    v = mul(v, basic->field_0x009c.get());
    return mul(v, basic->field_0x00ac.get());
}

static inline f32 soundPositionDistance(VectorWords *v) {
    f32 y = reinterpret_cast<cXyz *>(v)->y.get(), x = reinterpret_cast<cXyz *>(v)->x.get(),
        z = reinterpret_cast<cXyz *>(v)->z.get();
    return std_sqrtf(gabi::fmadds(z, z, gabi::fmadds(x, x, y * y)));
}

/* 0201EBA0 */
gptr<Sound> *JAIZelBasic::seStart(u32 id, cXyz *pos, u32 info, u32 reverb, f32 pitch, f32 volume,
                                  f32 pan, f32 dolby, u32 flag) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (pos != 0)
        checkSoundPointer(pos);
    if (field_0x0201.get() == 1)
        return 0;
    bool kurobo = id - JA_SE_CM_KS_MOVE < 3 || id - JA_SE_CM_PT_JUMP < 3;
    if (!kurobo && (s32)field_0x0224.get() == 0x2D) {
        if ((s32)id == JA_SE_CHR_PW_MOVE)
            return 0;
        kurobo = id == JA_SE_FT_PW_RUN_AWAY || id == JA_SE_CM_PW_BECOME_SOLID ||
                 (id >= JA_SE_CM_PW_BECOME_CLEAR && id <= JA_SE_CM_PW_GO_HEAVEN) ||
                 id == JA_SE_CM_PW_SOLID_END;
    }
    if (kurobo) {
        kuroboMotionPlay(id, pos, info, reverb);
        return 0;
    }
    if (id - JA_SE_CV_KS_ATTACK < 3)
        return 0;
    if (mMainBgmNum.get() == JA_BGM_TETRA_MEET &&
        ((s32)id == JA_SE_CM_INOCHIDAMA_BLINK || (s32)id == JA_SE_CM_INOCHIDAMA_BREAK))
        return 0;
    if (field_0x0206.get() != 0) {
        if (!(id - JA_SE_OBJ_TBOX_UNLOCK < 9 || (id & 0xFFFFF000) == 0 ||
              (s32)id == JA_SE_CM_SO_DIVE || (s32)id == JA_SE_CV_G_SHIP_SCREAM ||
              (s32)id == JA_SE_CV_G_SHIP_LAUGH))
            return 0;
    }
    if (field_0x0207.get() != 0 && (id & 0xFFFFF000) != 0) {
        bool ok;
        if (id >= JA_SE_WTAKT_AUFTAKT_4)
            ok = id <= JA_SE_WTAKT_AUFTAKT_6 || id == JA_SE_CV_TT_DANCE_FIN ||
                 (id >= JA_SE_CV_TT_DANCE_1 && id <= JA_SE_CV_TT_DANCE_2);
        else
            ok = id == JA_SE_WTAKT_AMBIENT || id == JA_SE_WTAKT_AUFTAKT;
        if (!ok)
            return 0;
    }
    /* one-shot sounds per scene and sounds that must not overlap */
    switch ((s32)id) {
    case JA_SE_FT_ADD_GRASS: {
        if (field_0x01f9.get() == 1)
            return 0;
        s32 scene = field_0x0224.get();
        field_0x01f9 = 1;
        if (scene == 0x12 && mIslandRoomNo.get() == 0x2C && field_0x0239.get() == 9 &&
            field_0x0028.get() < 30)
            return 0;
        break;
    }
    case JA_SE_OBJ_COL_BRK_WRAILING:
        if (field_0x01fc.get() == 1)
            return 0;
        field_0x01fc = 1;
        break;
    case JA_SE_OBJ_BRK_WRAIL_K:
        if (field_0x01fb.get() == 1)
            return 0;
        field_0x01fb = 1;
        break;
    case JA_SE_LK_CUT_GRASS:
        if (field_0x01fa.get() != 0)
            return 0;
        field_0x01fa = 4;
        break;
    case JA_SE_OBJ_TREE_SWING:
    case JA_SE_OBJ_TREE_SWING_S:
        if (field_0x0202[0].get() != 0)
            return 0;
        field_0x0202[0] = 2;
        break;
    case JA_SE_OBJ_COL_SWC_CHNS:
        if (field_0x0202[1].get() != 0)
            return 0;
        field_0x0202[1] = 2;
        break;
    case JA_SE_CM_BGN_STRING_RIPPLE:
        if (field_0x01fd.get() != 0)
            return 0;
        field_0x01fd = 0x14;
        break;
    case JA_SE_CM_BGN_BODY_RIPPLE:
        if (field_0x01fe.get() != 0)
            return 0;
        field_0x01fe = 0x14;
        break;
    case JA_SE_CM_BGN_M_HEAD_RIPPLE:
        if (field_0x01ff.get() != 0)
            return 0;
        field_0x01ff = 0x14;
        break;
    case JA_SE_CM_BGN_M_TAIL_RIPPL:
        if (field_0x0200.get() != 0)
            return 0;
        field_0x0200 = 0x14;
        break;
    case JA_SE_OBJ_OTOSHIANA_OPEN:
        if (checkSePlaying(0x6934) != 0)
            return 0;
        break;
    case JA_SE_LK_WARP_TO_G_SHIP:
        if (checkSePlaying(0x2889) != 0)
            return 0;
        break;
    case JA_SE_OBJ_PIRATE_CRUISE:
        if ((s32)field_0x0224.get() == 0x11)
            id = JA_SE_OBJ_PIRATE_CRUISE_LPF;
        break;
    }
    /* instance limits: {distance, priority, count} */
    f32 hundred = 100.0f;
    int lim = 0; /* A..H = 1..8 */
    if (id < JA_SE_CM_AM_JUMP_L) {
        if (id < JA_SE_CV_WZ_ATTACK) {
            if (id < JA_SE_CV_AM_JITABATA) {
                if (id == JA_SE_LK_LAST_HIT)
                    lim = 3;
                else if (id == JA_SE_CV_KS_DAMAGE)
                    lim = 1;
                else if (id >= JA_SE_CV_AM_JUMP && id <= JA_SE_CV_AM_OPEN_MOUTH)
                    lim = 5;
            } else if (id == JA_SE_CV_AM_JITABATA)
                lim = 5;
            else if (id >= JA_SE_CV_AM2_JUMP && id <= JA_SE_CV_AM2_DAMAGE)
                lim = 7;
            else if (id == JA_SE_CV_WZ_LAUGH)
                lim = 8;
        } else if (id < JA_SE_CM_MONS_EXPLODE) {
            if (id == JA_SE_CV_WZ_ATTACK)
                lim = 8;
            else if (id >= JA_SE_CV_PG_EBALL_FIRE_S && id <= JA_SE_CV_PG_EBALL_FIRE_L)
                lim = 6;
            else if (id == JA_SE_CV_PG_DISAPPEAR)
                lim = 6;
        } else if (id == JA_SE_CM_MONS_EXPLODE)
            lim = 2;
        else if (id == JA_SE_CM_KS_DIE)
            lim = 1;
        else if (id >= JA_SE_CM_KS_DIE && id <= JA_SE_CM_AM_NEEDLE_IN)
            lim = 5;
        else if (id == JA_SE_CM_AM_JUMP_S)
            lim = 5;
    } else if (id < JA_SE_OBJ_BOMB_EXPLODE) {
        if (id < JA_SE_CM_WZ_APPEAR) {
            if (id == JA_SE_CM_AM_JUMP_L)
                lim = 5;
            else if (id == JA_SE_CM_AM2_LANDING || id == JA_SE_CM_AM2_JUMP2)
                lim = 7;
            else if (id == JA_SE_CM_BST_BEAM_FIRE)
                lim = 4;
        } else if (id <= JA_SE_CM_WZ_PRE_SUMMONBALL)
            lim = 8;
        else if (id >= JA_SE_CM_PG_APPEAR && id <= JA_SE_CM_PG_DISAPPEAR)
            lim = 6;
        else if (id == JA_SE_CM_PG_SWING_L)
            lim = 6;
        else if (id >= JA_SE_OBJ_COL_SWC_CHNS && id <= JA_SE_OBJ_COL_SWC_CHNL)
            lim = 3;
    } else if (id < JA_SE_CM_BST_BEAM_BOMB) {
        if (id == JA_SE_OBJ_BOMB_EXPLODE || id == JA_SE_OBJ_RACK_BREAK ||
            id == JA_SE_OBJ_BOMB_WATER)
            lim = 1;
        else if (id == JA_SE_OBJ_BREAK_WATER_POT)
            lim = 3;
    } else if (id == JA_SE_CM_BST_BEAM_BOMB)
        lim = 4;
    else if (id >= JA_SE_OBJ_WZ_FIRE_B_IMP && id <= JA_SE_OBJ_WZ_SUMMON_B_SMN)
        lim = 8;
    else if (id == JA_SE_OBJ_LUPY_BOUND)
        lim = 1;
    else if (id == JA_SE_OBJ_PG_EBALL_EXP_L)
        lim = 5;
    s32 blocked = 0;
    switch (lim) {
    case 1:
        blocked = seLimitInstances(id, pos, 0xA, 4, 160.0f);
        break;
    case 2:
        blocked = seLimitInstances(0x5801, pos, 0xA, 3, 200.0f);
        break;
    case 3:
        blocked = seLimitInstances(id, pos, 0xA, 3, hundred);
        break;
    case 4:
        blocked = seLimitInstances(id, pos, 0xA, 2, hundred);
        break;
    case 5:
        blocked = seLimitInstances(id, pos, 0xF, 4, 320.0f);
        break;
    case 6:
        blocked = seLimitInstances(id, pos, 0x14, 2, 400.0f);
        break;
    case 7:
        blocked = seLimitInstances(id, pos, 0xF, 3, 320.0f);
        break;
    case 8:
        blocked = seLimitInstances(id, pos, 0x14, 3, 400.0f);
        break;
    }
    if (blocked != 0)
        return 0;
    if (id == 0xFFFFFFFF)
        return 0;
    if ((id & 0xFFFFFF00) == JA_SE_LK_V_01_01 && (id & 0xFF) < 0x5A) {
        /* system sounds 0x1800..0x1859 use their own handle */
        startSoundVec(id, &mSystemSoundHD, pos, 0, 0, 4);
        if (mSystemSoundHD.get() == 0)
            return 0;
        mSystemSoundHD.get()->setPortData(9, reverb & 0xFFFF);
        return 0;
    }
    /* the sound position (or the camera position) for the distance-dependent sounds */
    Mtx34 *mtx = cameraMatrix(camera());
    gabi::Local<VectorWords> vv;
    VectorWords *v = vv.get();
    v->w[0] = l_defaultSePosition.w[0].get();
    v->w[1] = l_defaultSePosition.w[1].get();
    v->w[2] = l_defaultSePosition.w[2].get();
    if (pos == 0) {
        cXyz *cp = cameraPosition(camera());
        if (cp != 0) {
            v->w[0] = reinterpret_cast<VectorWords *>(cp)->w[0].get();
            v->w[1] = reinterpret_cast<VectorWords *>(cp)->w[1].get();
            v->w[2] = reinterpret_cast<VectorWords *>(cp)->w[2].get();
        }
    } else {
        v->w[0] = reinterpret_cast<VectorWords *>(pos)->w[0].get();
        v->w[1] = reinterpret_cast<VectorWords *>(pos)->w[1].get();
        v->w[2] = reinterpret_cast<VectorWords *>(pos)->w[2].get();
    }
    const u32 STREAM5 = JA_STRM_DEMO_GO_MAJU;
    f32 f; /* (f32)info */
    switch (id) {
    case JA_SE_TELESCOPE_ZOOM: {
        if (info > 0x7FFF)
            info = 0x7FFF;
        f32 d = (f32)info / 32767.0f * 20.0f;
        if (d > 1.0f)
            d = 1.0f;
        startSoundVec(id, &field_0x0214, 0, 0, 0, 4);
        if (field_0x0214.get() == 0)
            return 0;
        field_0x0214.get()->setPitch(gabi::fmadds(0.2f, d, 0.8f), 0, 0);
        field_0x0214.get()->setVolume(d, 2, 0);
        return 0;
    }
    case JA_SE_CAMERA_C_MOVE:
    case JA_SE_LK_SHIP_CANNON_FLY:
    case JA_SE_CM_BKM_VINE_SET:
    case JA_SE_OBJ_BS_MELODY_1:
    case JA_SE_OBJ_BS_MELODY_2:
        return 0;
    case JA_SE_L_FOCUS_SET:
    case JA_SE_CAMERA_L_MOVE:
        seStop(JA_SE_L_FOCUS_RESET, 0);
        seStop(JA_SE_CAMERA_L_CANCEL, 0);
        break;
    case JA_SE_L_FOCUS_RESET:
    case JA_SE_CAMERA_L_CANCEL:
        seStop(JA_SE_L_FOCUS_SET, 0);
        seStop(JA_SE_CAMERA_L_MOVE, 0);
        break;
    case JA_SE_TALK_START:
    case JA_SE_TALK_WIN_OPEN:
    case JA_SE_TALK_END:
        if (checkStreamPlaying(STREAM5) == 1)
            return 0;
        if (isDemo() == 1)
            return 0;
        if (field_0x0207.get() != 0)
            return 0;
        if (field_0x00be.get() != 0)
            return 0;
        break;
    case JA_SE_TALK_NEXT:
        if (checkStreamPlaying(STREAM5) == 1)
            return 0;
        if (isDemo() != 1 && field_0x0207.get() == 0) {
            if (field_0x00be.get() != 0)
                return 0;
            break;
        }
        id = JA_SE_DEMO_MSG_NEXT;
        if (field_0x00be.get() != 0)
            return 0;
        break;
    case JA_SE_TALK_WIN_CLOSE:
        if (checkStreamPlaying(STREAM5) == 1)
            return 0;
        if (isDemo() == 1)
            id = JA_SE_DEMO_MSG_END;
        if (checkSePlaying(0x871) == 1)
            return 0;
        if (field_0x00be.get() != 0)
            return 0;
        break;
    case JA_SE_ITM_MENU_IN:
        menuIn();
        break;
    case JA_SE_ITM_MENU_OUT:
    case JA_SE_QUIT_GAME:
        menuOut();
        break;
    case JA_SE_OP_ENTER_GAME:
        seStop(JA_SE_TITLE_WIND, 0x3C);
        break;
    case JA_SE_ITM_OMAMORI_TETLA: {
        u32 scene = field_0x0224.get();
        if (scene == 8 || scene == 0xC || scene == 0x13)
            break;
        id = JA_SE_ITM_OMAMORI_SHIP;
        break;
    }
    case JA_SE_TAKT_WIND_DEMO:
        subBgmStart(JA_BGM_TAKT_KAZE);
        break;
    case JA_SE_MP_GAUGE_INC:
        if (info != 0)
            info -= 1;
        pitch = gabi::fmadds((f32)info, l_pitchIncrement.get(), 1.0f);
        break;
    case JA_SE_PRE_TAKT:
    case JA_SE_LK_WAPR_EFF_WARP:
    case JA_SE_CM_BWD_LAST_EXPLODE:
    case JA_SE_OBJ_LUPY_IN_POT:
        pos = 0;
        break;
    case JA_SE_PLAYSPOT_END_S:
        seStop(JA_SE_PLAYSPOT_DESTROY, 0);
        break;
    case JA_SE_FORCE_BACK:
        field_0x0205 = 1;
        stopBattleBgm();
        {
            Sound *main = mpMainBgmSound.get();
            field_0x008c = 1.0f;
            if (main != 0)
                main->setVolume(calcMainBgmVolumeWithoutSubFade(this), 0x2D, 0);
        }
        stopBattleBgm();
        field_0x00c1 = 0;
        field_0x00c0 = 0;
        field_0x00c4 = 0;
        subBgmStop();
        break;
    case JA_SE_CHUYA_SWITCH:
        subBgmStart(JA_BGM_TAKT_CHUYA);
        field_0x0033 = 1;
        return 0;
    case JA_SE_AUC_BID_GAUGE:
        if (info >= 100)
            info = 0x63;
        else if (info != 0)
            info -= 1;
        pitch = gabi::fmadds((f32)info, 0.01f, 1.0f);
        break;
    case JA_SE_ME_ITEM_GET_S: {
        u32 c6 = mLastMinibossSubBGMType.get();
        field_0x0206 = 0;
        field_0x00bb = 0;
        if (c6 != 0)
            break;
        u32 n = mSubBgmNum.get();
        if (n == JA_BGM_CB_NEXTYEAR || n == JA_BGM_CB_NEXTYEAR_W)
            break;
        field_0x00b0 = (u32)-2;
        subBgmStopInner();
        break;
    }
    case JA_SE_ATM_RAIN_1:
        if ((s32)field_0x0224.get() == 0x5C && field_0x0239.get() < 8)
            id = JA_SE_ATM_VS_GANON_RAIN;
        break;
    case JA_SE_ATM_SWIRL:
    case JA_SE_OBJ_KASSHA_LIFT_MOVE:
    case JA_SE_CM_LANCE_BOUND:
    case JA_SE_CM_BOKOBOU_BOUND:
    case JA_SE_CM_NATA_BOUND:
    case JA_SE_CM_MAGBALL_BOUND:
    case JA_SE_CM_BL_BOUND:
    case JA_SE_CM_BGN_D_KAZEKIRI:
    case JA_SE_OBJ_LADDER_FALL_2:
    case JA_SE_CM_BST_ARROW_BOUND:
    case JA_SE_CM_BST_BOMB_BOUND:
    case JA_SE_CM_BST_HEART_BOUND:
    case JA_SE_OBJ_TN_SWORD_FALL:
    case JA_SE_OBJ_ST_CLUB_FALL:
    case JA_SE_OBJ_CUP_SWING:
    case JA_SE_OBJ_DISH_SWING:
    case JA_SE_OBJ_BOKKURI_BOUND:
    case JA_SE_OBJ_LUPY_BOUND:
    case JA_SE_OBJ_M_POT_BOUND:
    case JA_SE_OBJ_H_PENDANT_BOUND:
    case JA_SE_OBJ_S_NECKLACE_BOUND:
    case JA_SE_OBJ_BOKO_SEED_BOUND:
    case JA_SE_OBJ_W_EMBLEM_BOUND:
    case JA_SE_OBJ_V_BAIT_BOUND:
    case JA_SE_OBJ_HYOI_BOUND:
        if (info >= 100) {
            info = 10000;
            volume = (f32)info / 10000.0f;
        } else {
            if (info == 0)
                return 0;
            info = info * info;
            volume = (f32)info / 10000.0f;
        }
        break;
    case JA_SE_ATM_WIND_VAR:
        if (info >= 100)
            info = 100;
        f = (f32)info;
        {
            f32 sq = f * f;
            f32 lin = f / hundred;
            f32 p = sq / 40000.0f;
            volume = gabi::fmadds(lin, 0.85f, 0.15f);
            pitch = p + 0.75f;
        }
        break;
    case JA_SE_ATM_ICEBERG_WIND: {
        if (info >= 100)
            info = 100;
        f = (f32)info;
        f32 sq = f * f;
        f32 lin = f / hundred;
        f32 p = sq / 20000.0f;
        volume = gabi::fmadds(lin, 0.6f, 0.4f);
        pitch = p + 0.75f;
        if (mtx != 0)
            transformSoundVector(mtx, v, v);
        f32 d = soundPositionDistance(v);
        if (!(d < 4500.0f))
            break;
        pan = 0.5f;
        dolby = 0.5f;
        volume = volume * (d / 4500.0f * 0.7f);
        pos = 0;
        break;
    }
    case JA_SE_OBJ_FER_WHEEL_ROUND:
        if (info >= 100)
            info = 100;
        f = (f32)info;
        volume = f / hundred;
        if (volume != 0.0f)
            volume = gabi::fmadds(volume, 0.6f, 0.4f);
        pitch = f / 200.0f + 0.5f;
        break;
    case JA_SE_OBJ_HL_WAPR_EFF:
        if ((s32)field_0x0224.get() == 0x55) {
            u32 r = field_0x0239.get();
            if (r == 9 || r == 0xA)
                return 0;
        }
        break;
    case JA_SE_OBJ_SALVAGE_TAG_NEAR:
    case JA_SE_OBJ_BS_BOAT_CRUISE:
    case JA_SE_CM_GY_CRUISING:
    case JA_SE_CM_SH_CRUISING:
        if (info >= 100) {
            info = 100;
        } else if (info == 0) {
            return 0;
        }
        volume = (f32)info / hundred;
        break;
    case JA_SE_ATM_MAGMA_CHILL:
        field_0x0204 = 1;
        break;
    case JA_SE_OBJ_TORNADE_WARP:
        subBgmStart(JA_BGM_TAKT_SHIPPU);
        break;
    case JA_SE_LK_SLIP_SUS:
        if (info == 7)
            id = JA_SE_LK_SLIP_SUS_STONE;
        else if (info < 9)
            break;
        else if (info <= 0xE)
            id = attackSoundId(info)->get();
        else if (info >= 0x11 && info <= 0x12)
            id = JA_SE_LK_SLIP_SUS_STONE;
        break;
    case JA_SE_WTAKT_AMBIENT:
        field_0x0207 = 2;
        break;
    case JA_SE_LK_DAMAGE_NORMAL:
    case JA_SE_LK_DAMAGE_LARGE:
        onEnemyDamage();
        break;
    case JA_SE_SHIP_SAIL_OUT: {
        if (mpMainBgmSound.get() == 0)
            break;
        if (mMainBgmNum.get() != JA_BGM_SEA_GAME)
            break;
        Sound *sub = mpSubBgmSound.get();
        f32 d = VOL_BGM_DEFAULT.get();
        field_0x0090 = d;
        if (sub != 0)
            break;
        f32 vol = mul(field_0x0080.get(), field_0x0084.get());
        vol = mul(vol, field_0x0088.get());
        vol = mul(vol, field_0x008c.get());
        vol = mul(vol, d);
        vol = mul(vol, field_0x0094.get());
        vol = mul(vol, field_0x0098.get());
        vol = mul(vol, field_0x009c.get());
        vol = mul(vol, field_0x00ac.get());
        mpMainBgmSound.get()->setVolume(vol, 2, 0);
        break;
    }
    case JA_SE_CM_PG_FLYING:
    case JA_SE_CHR_PW_MOVE: {
        if (info >= 100)
            info = 100;
        f = (f32)info;
        f32 sq = f * f;
        f32 lin = f / hundred;
        f32 p = sq / 20000.0f;
        volume = gabi::fmadds(lin, 0.7f, 0.3f);
        pitch = p + 0.75f;
        break;
    }
    case JA_SE_LK_INTO_WATER:
        if (checkStreamPlaying(STREAM5) == 1)
            return 0;
        if (mSystemSoundHD.get() != 0)
            mSystemSoundHD.get()->stop(1);
        break;
    case JA_SE_FIREBLAST_BLOW: {
        if (pos == 0)
            break;
        VectorWords *tab = field_0x1ed4;
        VectorWords *e = indexed(tab, field_0x1f34);
        e->w[0] = reinterpret_cast<VectorWords *>(pos)->w[0].get();
        e->w[1] = reinterpret_cast<VectorWords *>(pos)->w[1].get();
        e->w[2] = reinterpret_cast<VectorWords *>(pos)->w[2].get();
        u32 n = field_0x1f34.get();
        f32 cy = cameraHeight(camera());
        e = indexed(tab, n);
        if (reinterpret_cast<cXyz *>(e)->y.get() > cy) {
            reinterpret_cast<cXyz *>(e)->y = cy;
            n = field_0x1f34.get();
            e = indexed(tab, n);
        }
        field_0x1f34 = n + 1;
        pos = reinterpret_cast<cXyz *>(e); // HD: follow the stored height-clamped copy.
        break;
    }
    case JA_SE_FT_LADDER_CLIMB:
    case JA_SE_FT_LADDER_CLIMB_D:
        if (field_0x0046.get() == 1) {
            id = JA_SE_LK_WALK_HEAVY;
            if ((s32)info != 0xD)
                info = 9;
        }
        if (field_0x0045.get() != 1)
            break;
        seStart(JA_SE_LK_FT_SW_SHIELD, pos, 0, reverb, 1.0f, 1.0f, -1.0f, -1.0f, 0);
        break;
    case JA_SE_OBJ_PIRATE_SAIL:
        if ((s32)field_0x0224.get() == 0x12 && mIslandRoomNo.get() == 0x2C &&
            field_0x0239.get() == 0xA) {
            if (checkStreamPlaying(STREAM5 + 0x17) == 0 && field_0x0208.get() == 0)
                return 0;
            field_0x0208 = 1;
        }
        break;
    case JA_SE_CM_PRAPELLO_ROLLING:
    case JA_SE_CM_ST_CLUB_SWING_L:
        if (info >= 100)
            info = 100;
        f = (f32)info;
        pitch = f / 300.0f + 1.0f;
        volume = f / hundred;
        break;
    case JA_SE_CM_SH_PROPELLER:
        if (info >= 100)
            info = 100;
        else if (info == 0)
            return 0;
        f = (f32)info;
        {
            f32 sq = f * f;
            pitch = sq / 20000.0f + 0.5f;
            volume = sq / 10000.0f;
        }
        break;
    case JA_SE_CM_INOCHIDAMA_BLINK:
    case JA_SE_CM_MAGTAIL_MOVE: {
        if (mtx != 0)
            transformSoundVector(mtx, v, v);
        f32 d = soundPositionDistance(v);
        if (d > getParamDistanceMax())
            return 0;
        break;
    }
    case JA_SE_CM_BTD_ENT_BUBBLE:
        mSomeSpecialBGMFlag = 1;
        break;
    case JA_SE_CM_BKM_ATKVINE_ENTER:
        seStart(JA_SE_CM_BKM_ATKVINE_BOOM, 0, 0, 0, 1.0f, 1.0f, -1.0f, -1.0f, 0);
        break;
    case JA_SE_CM_DK_BREAK_MASK:
        field_0x00cc = 1;
        break;
    case JA_SE_OBJ_KM_WINDMILL:
        if (info >= 100) {
            info = 100;
            pitch = 100.0f / hundred + 1.0f;
            info = 30;
            f = 30.0f;
        } else {
            if (info == 0)
                return 0;
            f = (f32)info;
            pitch = f / hundred + 1.0f;
            if (info >= 30) {
                info = 30;
                f = 30.0f;
            }
        }
        volume = f / 30.0f;
        break;
    case JA_SE_OBJ_SPRING:
        if ((s32)field_0x0224.get() == 0x35 && checkEventBit(0x2D04) == 0)
            return 0;
        break;
    case JA_SE_OBJ_STN_DOOR_STL_BAR:
        seStop(JA_SE_OBJ_STN_DOOR_STL_BAR, 0);
        break;
    case JA_SE_OBJ_STN_DOOR_MOVE_U:
    case JA_SE_OBJ_STN_DOOR_MOVE_D:
    case JA_SE_OBJ_STN_DOOR_STOP_U:
    case JA_SE_OBJ_STN_DOOR_STOP_D:
        seStop(JA_SE_OBJ_STN_DOOR_MOVE_U, 0);
        seStop(JA_SE_OBJ_STN_DOOR_MOVE_D, 0);
        break;
    case JA_SE_OBJ_OTOSHIANA_OPEN: {
        field_0x00ca = 1;
        stopBattleBgm();
        Sound *main = mpMainBgmSound.get();
        field_0x008c = 1.0f;
        if (main != 0)
            main->setVolume(calcMainBgmVolumeWithoutSubFade(this), 0x2D, 0);
        stopBattleBgm();
        field_0x00c1 = 0;
        pos = 0;
        field_0x00c0 = 0;
        field_0x00c4 = 0;
        break;
    }
    case JA_SE_OBJ_BTD_FIRE_DROP: {
        if (mtx != 0)
            transformSoundVector(mtx, v, v);
        f32 d = soundPositionDistance(v);
        if (d > getParamDistanceMax() * 0.5f)
            return 0;
        break;
    }
    case JA_SE_OBJ_L_BRIDGE_ON:
        seStop(JA_SE_OBJ_L_BRIDGE_OFF, 0);
        break;
    case JA_SE_OBJ_L_BRIDGE_OFF:
        seStop(JA_SE_OBJ_L_BRIDGE_ON, 0);
        break;
    case JA_SE_OBJ_WIND_TAG:
    case JA_SE_OBJ_WIND_TAG_L:
    case JA_SE_CM_BGN_MECHA_ROTATE:
    case JA_SE_CM_BGN_MECHA_ROPE:
        if (info >= 100)
            info = 100;
        else if (info == 0)
            return 0;
        f = (f32)info;
        {
            f32 sq = f * f;
            pitch = sq / 40000.0f + 0.75f;
            volume = sq / 10000.0f;
        }
        break;
    case JA_SE_ATM_MJ_WATER_UP: {
        if (mtx != 0)
            transformSoundVector(mtx, v, v);
        f32 a = std::fabs(reinterpret_cast<cXyz *>(v)->y.get());
        f32 max = getParamDistanceMax();
        volume = 1.0f - a / (max + max);
        if (volume < 0.0f) {
            pos = 0;
            volume = 0.0f;
            break;
        }
        if (volume > 1.0f)
            volume = 1.0f;
        pos = 0;
        break;
    }
    case JA_SE_CM_BST_HAND_WORKING:
    case JA_SE_CM_BST_HAND_SLEEPING:
    case JA_SE_CM_BST_HEAD_WORKING:
    case JA_SE_CM_BST_HEAD_SLEEPING:
        if (info >= 100)
            info = 100;
        else if (info == 0)
            return 0;
        f = (f32)info;
        {
            f32 sq = f * f;
            pitch = sq / 80000.0f + 0.95f;
            volume = sq / 10000.0f;
        }
        break;
    case JA_SE_CM_BST_GOKOU:
        field_0x00be = 1;
        break;
    case JA_SE_CM_BGN_T_ROUND:
        if (info >= 100)
            info = 100;
        else if (info == 0)
            return 0;
        f = (f32)info;
        pitch = f / 80.0f + 0.2f;
        volume = f * f / 10000.0f;
        if (!(pitch > 0.5f))
            pitch = 0.5f;
        break;
    }
    /* level-object sounds are mixed by processLevObjSE */
    if (flag != 1) {
        switch (id) {
        case JA_SE_ATM_RAKUBAN:
        case JA_SE_OBJ_OTOSHIANA_SUS:
        case JA_SE_ATM_RIVER_M:
        case JA_SE_ATM_RIVER_S:
        case JA_SE_ATM_RIVER_S_ISLE:
        case JA_SE_CM_BTD_HIFUKI_BEF:
        case JA_SE_CM_BTD_HIFUKI:
        case JA_SE_CM_BKM_ATKVINE_DIG:
        case JA_SE_CM_BKM_ATKVINE_L_ATK:
        case JA_SE_OBJ_TORCH_BURNING:
        case JA_SE_OBJ_STEAM:
        case JA_SE_OBJ_SEARCH_LIGHT:
        case JA_SE_OBJ_BAR_FRAME_BURN:
        case JA_SE_CM_BST_HORI:
            setLevObjSE(id, pos, reverb);
            return 0;
        }
    }
    gptr<Sound> *snd = mpSeSound;
    if ((id & 0x800) == 0) {
        /* restart the instance already playing for this position */
        for (u32 i = 0; i < 32; i++) {
            if (mSeNum[i].get() != id)
                continue;
            if (indexed(snd, i)->get() == 0)
                continue;
            if (field_0x0194[i].get() != gabi::ea(pos))
                continue;
            gptr<Sound> *h = indexed(snd, i);
            startSoundVec(id, h, pos, 0, info, 4);
            if (h->get() != 0) {
                h->get()->setPortData(9, reverb & 0xFFFF);
                if (pitch != 1.0f)
                    h->get()->setPitch(pitch, 0, 0);
                if (volume != 1.0f)
                    h->get()->setVolume(volume, 0, 0);
                if (pan != -1.0f)
                    h->get()->setPan(pan, 0, 0);
                if (dolby != -1.0f)
                    h->get()->setDolby(dolby, 0, 0);
            }
            return h;
        }
    }
    // HD: search32 SE handles from the rotating index; reload after sound calls.
    s32 idx = field_0x01f4.get();
    gptr<Sound> *h = indexed(snd, u32(idx));
    if (h->get() != 0) {
        s32 start = idx;
        if (indexed(snd, idx)->get() != 0) {
            for (;;) {
                idx = (idx + 1) % 32;
                if ((u32)idx == (u32)start)
                    return 0;
                if (indexed(snd, idx)->get() == 0)
                    break;
            }
        }
        h = indexed(snd, idx);
        field_0x01f4 = idx;
    }
    *h = nullptr;
    startSoundVec(id, indexed(snd, field_0x01f4.get()), pos, 0, info, 4);
    if ((s32)id == JA_SE_PLAYSPOT_END_S) {
        if (pos != 0)
            deleteObject(pos);
        pos = 0;
    }
    u32 i2 = field_0x01f4.get();
    h = indexed(snd, i2);
    if (h->get() != 0) {
        h->get()->setPortData(9, reverb & 0xFFFF);
        if (pitch != 1.0f)
            indexed(snd, field_0x01f4.get())->get()->setPitch(pitch, 0, 0);
        if (volume != 1.0f)
            indexed(snd, field_0x01f4.get())->get()->setVolume(volume, 0, 0);
        if (pan != -1.0f)
            indexed(snd, field_0x01f4.get())->get()->setPan(pan, 0, 0);
        if (dolby != -1.0f)
            indexed(snd, field_0x01f4.get())->get()->setDolby(dolby, 0, 0);
        if (id == JA_SE_WTAKT_METRONOME || (id >= JA_SE_WTAKT_PULSE_3 && id <= JA_SE_WTAKT_PULSE_6))
            indexed(snd, field_0x01f4.get())->get()->setPortData(8, info & 0xFFFF);
        if ((s32)id == JA_SE_WTAKT_ARM_SWING || (s32)id == JA_SE_LK_WTAKT_SWING) {
            indexed(snd, field_0x01f4.get())->get()->setPortData(7, info >> 16);
            indexed(snd, field_0x01f4.get())->get()->setPortData(8, info & 0xFFFF);
        }
        i2 = field_0x01f4.get();
        h = indexed(snd, i2);
    }
    *indexed(mSeNum, i2) = id;
    *indexed(field_0x0194, field_0x01f4) = gabi::ea(pos);
    s32 nx = field_0x01f4.get() + 1;
    field_0x01f4 = nx % 32;
    return h;
}

} // namespace jaizel12

// clang-format off
/* WWHD glue:
*/
