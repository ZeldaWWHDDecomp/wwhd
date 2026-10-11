/** JAIZelBasic part6: per-frame sound timers, BGM transitions and scene ambience.
 * Private WWHD-derived source, verified against the original.
 * GameCube has a nonmatching stub: retain its field/function names and the verified HD body. */
#include "JAZelAudio/JAIZelBasic_6.h"
#include "gc_api/d/d_com_inf_game.h"
using namespace jaizel6;
static IsleInfo l_isleInfo;
using GuestFloat = be<f32>;
static GuestFloat l_highWindVolume;
static GuestFloat l_ambientWindVolume;
static u8 decrementTimer(const be<u8> *timer) {
    u8 value = *timer;
    return value != 0 ? value - 1 : 0;
}
void JAIZelBasic::sceneSE(u32 id, f32 volume) {
    seStart(id, 0, 0, 0, 1.0f, volume, -1.0f, -1.0f, 0);
}

enum : u32 {
    dIsleRoom_WindfallIsland_e = 11,
    dIsleRoom_DragonRoostIsland_e = 13,
    dIsleRoom_ForestHaven_e = 41,
    dIsleRoom_OutsetIsland_e = 44,
    JA_SE_ATM_WIND_1 = 0x105A,
    JA_SE_ATM_WIND_I_LINK = 0x1060,
    JA_SE_ATM_WIND_MJT = 0x1064,
    JA_SE_ATM_WIND_KINDAN = 0x1065,
    JA_SE_ATM_BWD_WIND = 0x1075,
    JA_SE_ATM_JB_WAVE = 0x1087,
    JA_SE_ATM_WIND_R_DUN_MAE = 0x10A5,
    JA_SE_ATM_D24_WIND = 0x10A6,
    JA_SE_ATM_PRT_SHIP_INNER = 0x7019,
    JA_SE_ATM_PRT_SHIP_WAVE = 0x701A,
    JA_SE_ATM_FILE_SEL_SEA = 0x703B,
    JA_SE_ATM_VS_GANON_WFALL = 0x706C,
    JA_BGM_ITEM_GET = 0x80000002,
    JA_BGM_D_RYU_MT = 0x80000006,
    JA_BGM_I_MAJU = 0x80000007,
    JA_BGM_DIE_LINK = 0x8000000A,
    JA_BGM_I_MAJU_JAIL = 0x8000000F,
    JA_BGM_BK_FLY_DOWN = 0x80000013,
    JA_BGM_I_LINK_FOREST = 0x80000017,
    JA_BGM_MBOSS_S = 0x8000001A,
    JA_BGM_SELECT = 0x8000001E,
    JA_BGM_GET_HEART = 0x80000024,
    JA_BGM_ITEM_GET_S = 0x80000025,
    JA_BGM_GET_SONG = 0x80000027,
    JA_BGM_SEA = 0x8000002E,
    JA_BGM_TAKT_CHUYA = 0x80000030,
    JA_BGM_TAKT_SHIPPU = 0x80000032,
    JA_BGM_SEA_STORM = 0x8000003C,
    JA_BGM_I_WOOD_INNER = 0x8000003F,
    JA_BGM_BAACHAN = 0x80000044,
    JA_BGM_DEKU_PINCH = 0x80000045,
    JA_BGM_DIOCTA_2 = 0x80000046,
    JA_BGM_P_GANON_2 = 0x80000047,
    JA_BGM_GET_PEARL = 0x8000004F,
    JA_BGM_ISLAND_LINK_3 = 0x80000055,
    JA_BGM_BGN_GET_BOX = 0x8000005D,
    JA_BGM_TRIFORCE_DEMO = 0x80000061,
};
/* the main-bgm volume with all factors read */
f32 JAIZelBasic::calcMainBgmVol() {
    f32 v = gabi::fmuls_ppc(field_0x0080, field_0x0084);
    v = gabi::fmuls_ppc(v, field_0x0088);
    v = gabi::fmuls_ppc(v, field_0x008c);
    v = gabi::fmuls_ppc(v, field_0x0090);
    v = gabi::fmuls_ppc(v, field_0x0094);
    v = gabi::fmuls_ppc(v, field_0x0098);
    v = gabi::fmuls_ppc(v, field_0x009c);
    return gabi::fmuls_ppc(v, field_0x00ac);
}

// HD: GameCube has no matching body; retain the HD timer and call sequence.
/* 02024478 */
void JAIZelBasic::zeldaGFrameWork() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* frame counter, timers */
    if (field_0x022d != 0) {
        u32 c = field_0x0028;
        field_0x0028 = c < 0xFFFFFFFF ? c + 1 : 0xFFFFFFFF;
    } else {
        field_0x0028 = 0;
    }
    processLevObjSE();
    cbPracticeProcess();
    u8 timerNext = decrementTimer(&field_0x01fa);
    field_0x1f34 = 0;
    field_0x01f9 = 0;
    field_0x01fc = 0;
    field_0x01fb = 0;
    field_0x01fa = timerNext;
    field_0x0202[0] = decrementTimer(&field_0x0202[0]);
    field_0x0202[1] = decrementTimer(&field_0x0202[1]);
    field_0x0207 = decrementTimer(&field_0x0207);
    field_0x01fd = decrementTimer(&field_0x01fd);
    field_0x01fe = decrementTimer(&field_0x01fe);
    field_0x01ff = decrementTimer(&field_0x01ff);
    field_0x0200 = decrementTimer(&field_0x0200);
    field_0x1e98 = 0;
    field_0x203d = 0;
    field_0x203c = 0;
    if (dComIfGp_event_runCheck())
        field_0x0061 = 5;
    else
        field_0x0061 = decrementTimer(&field_0x0061);
    field_0x00ce = decrementTimer(&field_0x00ce);
    processDemoFanfareMute();
    if (mMainBgmNum == JA_BGM_I_MAJU_JAIL && checkBgmPlaying() == 0) {
        bgmStop(0, 0);
        bgmStart(JA_BGM_I_MAJU, 0, 0);
    }
    /* restore the sub bgm after it was interrupted */
    u32 subBgm = mSubBgmNum;
    bool restore;
    if (subBgm >= JA_BGM_TAKT_CHUYA)
        restore = subBgm == JA_BGM_TRIFORCE_DEMO || subBgm == JA_BGM_BGN_GET_BOX ||
                  subBgm == JA_BGM_GET_PEARL || subBgm <= JA_BGM_TAKT_SHIPPU;
    else
        restore = subBgm == JA_BGM_GET_SONG ||
                  (subBgm <= JA_BGM_ITEM_GET_S &&
                   (subBgm >= JA_BGM_GET_HEART || subBgm == JA_BGM_ITEM_GET));
    if (restore && checkSubBgmPlaying() == 0) {
        f32 b4 = field_0x00a4;
        field_0x0206 = 0;
        if (b4 != 0.0f && field_0x00bb != 0) {
            u32 c6 = mLastMinibossSubBGMType;
            if (c6 == 1)
                subBgmStart(JA_BGM_MBOSS_S);
            else if (c6 == 2)
                subBgmStart(JA_BGM_DIOCTA_2);
            else if (c6 == 3)
                subBgmStart(JA_BGM_P_GANON_2);
            else {
                field_0x00b0 = (u32)-2;
                subBgmStopInner();
            }
            field_0x00bb = 0;
        }
    }
    u32 mainBgm = mMainBgmNum;
    if (mainBgm == JA_BGM_DEKU_PINCH) {
        if (checkBgmPlaying() == 0)
            bgmStart(JA_BGM_I_WOOD_INNER, 0, 1);
    } else if (mainBgm == JA_BGM_BK_FLY_DOWN) {
        if (checkBgmPlaying() == 0)
            bgmStart(JA_BGM_I_LINK_FOREST, 0, 1);
    }
    // Sub-BGM fade-out countdown.
    s32 c0 = field_0x00b0;
    if (c0 != -1) {
        if (c0 > 0) {
            field_0x00b0 = c0 - 1;
        } else {
            subBgmStopInner();
            field_0x00b0 = (u32)-1;
        }
    }
    if (mMainBgmNum == JA_BGM_D_RYU_MT) {
        if (field_0x0065 == 1) {
            auto *m = mpMainBgmSound.get();
            field_0x0090 = 0.0f;
            if (m != 0) {
                f32 v = gabi::fmuls_ppc(field_0x0080, field_0x0084);
                v = gabi::fmuls_ppc(v, field_0x0088);
                v = gabi::fmuls_ppc(v, field_0x008c);
                v = gabi::fmuls_ppc(v, 0.0f);
                v = gabi::fmuls_ppc(v, field_0x0094);
                v = gabi::fmuls_ppc(v, field_0x0098);
                v = gabi::fmuls_ppc(v, field_0x009c);
                v = gabi::fmuls_ppc(v, field_0x00ac);
                (m)->setVolume(v, 0x1E, 0);
            }
            field_0x0065 = 0;
        } else if (field_0x0090 == 0.0f) {
            auto *m = mpMainBgmSound.get();
            field_0x0090 = 1.0f;
            if (m != 0) {
                f32 v = gabi::fmuls_ppc(field_0x0080, field_0x0084);
                v = gabi::fmuls_ppc(v, field_0x0088);
                v = gabi::fmuls_ppc(v, field_0x008c);
                v = gabi::fmuls_ppc(v, field_0x0094);
                v = gabi::fmuls_ppc(v, field_0x0098);
                v = gabi::fmuls_ppc(v, field_0x009c);
                v = gabi::fmuls_ppc(v, field_0x00ac);
                (m)->setVolume(v, 0x1E, 0);
            }
        }
    }
    if (isDemo() == 1 && checkSeqIDDemoPlaying(JA_BGM_BAACHAN) != 0)
        bgmStart(JA_BGM_BAACHAN, 0, 0);

    /* scene ambience and the volume of the main bgm by the camera position */
    s32 scene = field_0x0224;
    bool skipSceneVolume = false;
    if (scene == 0x21) {
        skipSceneVolume = true;
        if (field_0x022d == 1) {
            u32 cp = cameraPositionEA();
            if (cp != 0) {
                f32 y = cameraY(cp);
                if (!(y < 2400.0f)) {
                    f32 v;
                    bool go = true;
                    if (y < 3200.0f) {
                        v = (y - 2400.0f) / 800.0f;
                        if (v == 0.0f)
                            go = false;
                    } else {
                        v = 1.0f;
                    }
                    if (go) {
                        startSoundVec(JA_SE_ATM_WIND_I_LINK, &field_0x1f38, 0, 0, 0, 4);
                        if (field_0x1f38.get() != 0)
                            (field_0x1f38.get())->setVolume(v, 0, 0);
                        scene = field_0x0224;
                        skipSceneVolume = false;
                    }
                }
            }
        }
    }
    if (!skipSceneVolume) {
        if (scene == 0x12 || scene == 0x55 || scene == 0x13) {
            f32 y = 0.0f;
            f32 windVolume = 0.0f;
            u32 room;
            bool skipIslandVolume = false;
            bool roomLoaded = false;
            if (field_0x022d == 1) {
                u32 cp = cameraPositionEA();
                if (cp != 0)
                    y = cameraY(cp);
                if (cp != 0 && !(y < 2400.0f)) {
                    if (!(y < 3200.0f)) {
                        room = mIslandRoomNo;
                        windVolume = 1.0f;
                    } else {
                        room = mIslandRoomNo;
                        windVolume = (y - 2400.0f) / 800.0f;
                    }
                } else {
                    room = mIslandRoomNo;
                    windVolume = 0.0f;
                }
                if (room == dIsleRoom_WindfallIsland_e && isDemo() == 1)
                    skipIslandVolume = true;
                if (!skipIslandVolume) {
                    if (windVolume == 0.0f) {
                        roomLoaded = true;
                    } else if (checkPlayingStreamBgmFlag(0xC0000036) == 0) {
                        startSoundVec(JA_SE_ATM_WIND_I_LINK, &field_0x1f38, 0, 0, 0, 4);
                        if (field_0x1f38.get() != 0)
                            (field_0x1f38.get())->setVolume(windVolume, 0, 0);
                    }
                }
            }
            if (!skipIslandVolume) {
                if (!roomLoaded)
                    room = mIslandRoomNo;
                bool setVol = false;
                if (room == dIsleRoom_OutsetIsland_e) {
                    if (checkEventBit(1) == 1) {
                        f32 v;
                        if (mMainBgmNum == JA_BGM_DIE_LINK || y < 1800.0f)
                            v = 1.0f;
                        else if (!(y < 3000.0f))
                            v = 0.0f;
                        else
                            v = 1.0f - (y - 1800.0f) / 1200.0f;
                        auto *m = mpMainBgmSound.get();
                        field_0x00ac = v;
                        setVol = m != 0;
                    }
                } else if (room == dIsleRoom_DragonRoostIsland_e) {
                    f32 v;
                    if (mMainBgmNum == JA_BGM_DIE_LINK || mbLandingDemoStarted == 2 || y < 1800.0f)
                        v = 1.0f;
                    else if (!(y < 3000.0f))
                        v = f32(l_highWindVolume); /* 0.3 */
                    else
                        v = gabi::fmadds(1.0f - (y - 1800.0f) / 1200.0f, 0.7f, 0.3f);
                    auto *m = mpMainBgmSound.get();
                    field_0x00ac = v;
                    setVol = m != 0;
                } else if (room == dIsleRoom_ForestHaven_e) {
                    if (mbLandingDemoStarted != 3) {
                        if (!dComIfGp_event_runCheck()) {
                            u32 cp = cameraPositionEA();
                            f32 dz = cameraZ(cp) - 198000.0f;
                            f32 dx = cameraX(cp) - 195000.0f;
                            f32 d = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
                            f32 v = 1.0f;
                            if (d > 8000.0f) {
                            } else if (d > 5000.0f) {
                                v = (d - 5000.0f) / 3000.0f;
                            } else {
                                v = 0.0f;
                            }
                            u32 n = mMainBgmNum;
                            u32 b275 = field_0x0205;
                            if (n == JA_BGM_DIE_LINK)
                                v = 1.0f;
                            if (b275 == 0)
                                field_0x0090 = v;
                            setVol = mpMainBgmSound.get() != 0;
                        }
                    }
                }
                if (setVol)
                    (mpMainBgmSound.get())->setVolume(calcMainBgmVol(), 0, 0);
                scene = field_0x0224;
            }
        }
        if (scene == 0x19) {
            u32 cp = cameraPositionEA();
            u32 n = mMainBgmNum;
            f32 y = cp != 0 ? cameraY(cp) : 0.0f;
            f32 v;
            if (n == JA_BGM_DIE_LINK || y < 2500.0f)
                v = 1.0f;
            else if (y < 5000.0f)
                v = 1.0f - (y - 2500.0f) / 2500.0f;
            else
                v = 0.0f;
            auto *m = mpMainBgmSound.get();
            field_0x00ac = v;
            if (m != 0) {
                f32 t = gabi::fmuls_ppc(field_0x0080, field_0x0084);
                t = gabi::fmuls_ppc(t, field_0x0088);
                t = gabi::fmuls_ppc(t, field_0x008c);
                t = gabi::fmuls_ppc(t, field_0x0090);
                t = gabi::fmuls_ppc(t, field_0x0094);
                t = gabi::fmuls_ppc(t, field_0x0098);
                t = gabi::fmuls_ppc(t, field_0x009c);
                t = gabi::fmuls_ppc(t, v);
                (m)->setVolume(t, 0, 0);
            }
            scene = field_0x0224;
        }
        if (scene == 0x35) {
            u32 cp = cameraPositionEA();
            f32 z = cameraZ(cp), x = cameraX(cp);
            f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
            f32 v = 1.0f;
            if (d > 8500.0f) {
            } else if (d > 6500.0f) {
                v = (d - 6500.0f) / 2000.0f;
            } else {
                v = 0.0f;
            }
            u32 n = mMainBgmNum;
            auto *m = mpMainBgmSound.get();
            if (n == JA_BGM_DIE_LINK)
                v = 1.0f;
            field_0x0090 = v;
            if (m != 0) {
                f32 t = gabi::fmuls_ppc(field_0x0080, field_0x0084);
                t = gabi::fmuls_ppc(t, field_0x0088);
                t = gabi::fmuls_ppc(t, field_0x008c);
                t = gabi::fmuls_ppc(t, v);
                t = gabi::fmuls_ppc(t, field_0x0094);
                t = gabi::fmuls_ppc(t, field_0x0098);
                t = gabi::fmuls_ppc(t, field_0x009c);
                t = gabi::fmuls_ppc(t, field_0x00ac);
                (m)->setVolume(t, 0, 0);
            }
        }
    }
    if (mMainBgmNum == JA_BGM_SELECT) {
        u32 s = field_0x0224;
        if (s < 0x4E || (s > 0x52 && s != 0x5D))
            sceneSE(JA_SE_ATM_FILE_SEL_SEA, 1.0f);
    }
    // Start queued BGM once nothing plays.
    u32 cc = field_0x00bc;
    if (cc != 0 && checkBgmPlaying() == 0) {
        if (cc == 1) {
            if (mpMainBgmSound.get() == 0 ||
                (mMainBgmNum != JA_BGM_SEA && mMainBgmNum != JA_BGM_SEA_STORM))
                bgmStart(checkSeaBgmID(), 0x5A, 0);
            field_0x021e = 0;
            field_0x00bc = 0;
        } else if (cc == 2) {
            sceneBgmStart();
            field_0x021e = 0;
            field_0x00bc = 0;
        } else {
            if (cc == 3) {
                u32 room = mIslandRoomNo;
                if (room == dIsleRoom_OutsetIsland_e && checkEventBit(0xE20) == 1)
                    bgmStart(JA_BGM_ISLAND_LINK_3, 0, 0);
                else
                    bgmStart(expandSceneBgmNum(u16(l_isleInfo.entries[room].bgmNum)), 0, 0);
            }
            field_0x00bc = 0;
        }
    }
    processTime();
    changeSeaBgm();
    bgmBattleGFrame();
    mbossBgmMuteProcess();
    enemyNearByGFrame();
    stSkyCloistersProcess();
    /* scene ambience loops */
    if (field_0x022d == 1) {
        u32 s = field_0x0224;
        switch (s) {
        case 7:
            sceneSE(JA_SE_ATM_WIND_KINDAN, 1.0f);
            break;
        case 0xC:
        case 0x54:
        case 0x58:
            sceneSE(JA_SE_ATM_PRT_SHIP_INNER, 1.0f);
            break;
        case 0x11:
            sceneSE(JA_SE_ATM_PRT_SHIP_WAVE, 1.0f);
            break;
        case 0x18:
            sceneSE(JA_SE_ATM_BWD_WIND, 1.0f);
            break;
        case 0x21:
            sceneSE(JA_SE_ATM_WIND_R_DUN_MAE, 1.0f);
            break;
        case 0x23:
            if (checkSubBgmPlaying() == 1)
                sceneSE(JA_SE_ATM_WIND_1, 0.3f);
            else
                sceneSE(JA_SE_ATM_WIND_1, f32(l_ambientWindVolume)); /* 0.9 */
            break;
        case 0x29:
            sceneSE(JA_SE_ATM_JB_WAVE, 1.0f);
            break;
        case 0x5A:
            sceneSE(JA_SE_ATM_D24_WIND, 1.0f);
            break;
        case 0x5C: {
            u32 r = field_0x0239;
            if (r != 8 && r != 9 && r != 0xA && r != 0xB)
                sceneSE(JA_SE_ATM_VS_GANON_WFALL, 1.0f);
            break;
        }
        case 9:
        case 0x40: {
            if (checkStreamPlaying(0xC0000004) != 0)
                break;
            u32 cp = cameraPositionEA();
            f32 y = cp != 0 ? cameraY(cp) : 0.0f;
            if (y < 9000.0f)
                break;
            f32 v;
            if (y < 11000.0f) {
                v = (y - 9000.0f) / 2000.0f;
                if (v == 0.0f)
                    break;
            } else {
                v = 1.0f;
            }
            startSoundVec(JA_SE_ATM_WIND_MJT, &field_0x1f38, 0, 0, 0, 4);
            if (field_0x1f38.get() != 0)
                (field_0x1f38.get())->setVolume(v, 0, 0);
            break;
        }
        }
    }
    processHeartGaugeSound();
    if (field_0x2060.get() == 0)
        mResetIndexHD = 0xFFFF; // HD: only the first halfword is reset.
}

// clang-format off
/* WWHD glue:
data l_isleInfo 1018E60C
data l_highWindVolume 10003910
data l_ambientWindVolume 100039BC
ext 020212EC void jaizel6::JAIZelBasic::processLevObjSE()
ext 02021534 void jaizel6::JAIZelBasic::cbPracticeProcess()
ext 02021DC4 void jaizel6::JAIZelBasic::processDemoFanfareMute()
ext 02021E30 s32 jaizel6::JAIZelBasic::checkBgmPlaying()
ext 02021F28 void jaizel6::JAIZelBasic::bgmStop(u32, u32)
ext 0202204C void jaizel6::JAIZelBasic::bgmStart(u32, u32, u32)
ext 0202148C s32 jaizel6::JAIZelBasic::checkSubBgmPlaying()
ext 0201DE00 void jaizel6::JAIZelBasic::subBgmStart(u32)
ext 0201D9C8 void jaizel6::JAIZelBasic::subBgmStopInner()
ext 0201E5CC s32 jaizel6::JAIZelBasic::isDemo()
ext 020217A4 s32 jaizel6::JAIZelBasic::checkSeqIDDemoPlaying(u32)
ext 020233C8 s32 jaizel6::JAIZelBasic::checkPlayingStreamBgmFlag(u32)
ext 0201E480 s32 jaizel6::JAIZelBasic::checkEventBit(u32)
ext 02023410 u32 jaizel6::JAIZelBasic::checkSeaBgmID()
ext 020234A4 void jaizel6::JAIZelBasic::sceneBgmStart()
ext 02023608 u32 jaizel6::JAIZelBasic::expandSceneBgmNum(u32)
ext 02024334 void jaizel6::JAIZelBasic::processHeartGaugeSound()
ext 020239C8 void jaizel6::JAIZelBasic::processTime()
ext 02023A94 void jaizel6::JAIZelBasic::changeSeaBgm()
ext 02023E0C void jaizel6::JAIZelBasic::bgmBattleGFrame()
ext 02024018 void jaizel6::JAIZelBasic::mbossBgmMuteProcess()
ext 02024190 void jaizel6::JAIZelBasic::enemyNearByGFrame()
ext 02024278 void jaizel6::JAIZelBasic::stSkyCloistersProcess()
ext 0201DB08 s32 jaizel6::JAIZelBasic::checkStreamPlaying(u32)
ext 02802848 void jaizel6::JAIZelBasic::startSoundVec(u32, gptr<Sound>*, u32, u32, u32, u32)
ext 0201EBA0 u32 jaizel6::JAIZelBasic::seStart(u32, u32, u32, s32, f32, f32, f32, f32, u32)
ext 0280B68C void jaizel6::Sound::setVolume(f32, u32, u32)
*/
