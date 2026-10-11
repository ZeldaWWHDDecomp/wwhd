/* WWHD m_Do_audio full HD cluster15CC..2113: timer, frame/tact/scene/wave,
 * audio compatibility entry points, global-object initializer. Most header wrappers
 * mapped to legacy JAIZelBasic names are actually singleton shims, not methods.
 * Reconstruction retains every callee argument read, including passthrough ABI slots. */
#include "m_Do/m_Do_audio.h"
namespace m_Do_audio_cpp {
static u8 mLoadTimer;
static u8 mResetFlag;
static jaizel12::JAIZelBasic* zel_basic;
static JAIZelInst mTact;
static jaizel12::JAIZelBasic g_mDoAud_zelAudio;
static InstHeaderStatics l_header;
static AudioRegistration l_registration;
static const f32 l_tactThreshold;
static const f32 l_soundOne;
static const f32 l_soundNegative;
// HD: lbzu leaves the timer address in r3 when the counter is at most one.
/* 025E15CC */
static u32 calcLoadTimer() {
    if (mLoadTimer <= 1)
        return gabi::ea(&mLoadTimer);
    return audioApi_cLib_calcTimer_u8_0(gabi::ea(&mLoadTimer));
}
/* 025E15E0 */
static void mDoAud_Execute() {
    if (gabi::ea(zel_basic) != 0) {
        audioApi_calcLoadTimer_0();
        u32 play = dComIfGs_ea();
        audioApi_JAIZelBasic_setEventBit_0(gabi::ea(zel_basic), play + SAVE_OFF(mSavedata.mEvent));
    }
}
/* 025E162C */
static s32 mDoAud_getTactDirection(s32 stick, s32 previous) {
    // HD: no digital-pad fallback; keep the double comparison and full signed angle.
    f64 magnitude;
    s32 angle;
    if (stick != 0) {
        magnitude = audioApi_CPad_GET_STICK_VALUE_0(0);
        angle = audioApi_CPad_GET_STICK_ANGLE_0(0);
    } else {
        magnitude = audioApi_CPad_GET_SUBSTICK_VALUE_0(0);
        angle = audioApi_CPad_GET_SUBSTICK_ANGLE_0(0);
    }
    if (magnitude < l_tactThreshold)
        return 0;
    s32 absolute = angle < 0 ? (s32)(0u - (u32)angle) : angle;
    if (previous == 0) {
        if (absolute > 0x6000)
            return 1;
        if (angle >= 0x2000)
            return 2;
        if (angle <= -0x2000)
            return 4;
        return 3;
    }
    if (absolute > 0x7000)
        return 1;
    if ((u32)angle - 0x3000u < 0x2001u)
        return 2;
    if ((u32)angle + 0x5000u < 0x2001u)
        return 4;
    if (absolute < 0x1000)
        return 3;
    return previous;
}
/* 025E17CC */
static void mDoAud_setSceneName(u32 name, s32 room, s32 layer) {
    if (mLoadTimer == 0) {
        audioApi_JAIZelBasic_setSceneName_0(gabi::ea(zel_basic), name, room, layer);
        mLoadTimer = SceneLoadFrames;
    }
}
/* 025E1828 */
static s32 mDoAud_load1stDynamicWave() {
    u8 timer = mLoadTimer;
    if (timer == 0)
        return 1;
    if (timer <= 1) {
        audioApi_JAIZelBasic_load1stDynamicWave_0(gabi::ea(zel_basic));
        mLoadTimer = 0;
        return 1;
    }
    return 0;
}
/* 025E1988 */
static u32 mDoAud_seStartDefault(u32 sound) {
    if (sound == ArmBgmSound)
        return audioApi_hdArmBgmSe_0();
    f32 one = l_soundOne;
    f32 negative = l_soundNegative;
    return audioApi_JAIZelBasic_seStart_0(gabi::ea(zel_basic), sound, 0, 0, 0, 0, one, one,
                                          negative, negative);
}
/* 025E1D40 */
static void mDoAud_resetProcess() {
    if (mResetFlag != 0) {
        audioApi_JAIZelBasic_resetProcess_0(gabi::ea(zel_basic));
        audioApi_hdDisarmBgmSe_0();
        mResetFlag = 1;
    }
}
/* 025E1D8C */
static void mDoAud_resetRecover() {
    if (mResetFlag != 0) {
        audioApi_JAIZelBasic_resetRecover_0(gabi::ea(zel_basic));
        audioApi_hdDisarmBgmSe_0();
        mResetFlag = 0;
    }
}
// HD: initializes the reduced tact object and the expanded audio singleton.
/* 025E202C */
static void __sinit_m_Do_audio_cpp() {
    sinit_header_statics(gabi::ea(&l_header), gabi::ea(&l_registration));
    audioApi_JAIZelInst_ct_0(gabi::ea(&mTact));
    audioApi_createJAIZelBasic_0(gabi::ea(&g_mDoAud_zelAudio));
    g_mDoAud_zelAudio.__vtbl = gabi::at<u8>(AudioVtableAddress);
}

/* 025E18AC */
static u32 mDoAud_checkFirstWaves() { return audioApi_JAIZelBasic_checkFirstWaves_0(); }

// HD: emitted wrappers retain unused register arguments read by the verification harness.
/* 025E18B8 */
static u32 mDoAud_check1stDynamicWave(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f64 f1,
                                      f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_check1stDynamicWave_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, f1,
                                                      f2, f3, f4, f5, f6, f7, f8);
}

/* 025E18C4 */
static u32 mDoAud_load2ndDynamicWave(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                     f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7,
                                     f64 f8) {
    return audioApi_JAIZelBasic_load2ndDynamicWave_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, a6,
                                                     a7, f1, f2, f3, f4, f5, f6, f7, f8);
}

// HD: emitted compatibility stub; its GameCube counterpart is unconfirmed.
/* 025E18D0 */
static u32 mDoAud_emptyAudioCompatibility() { return audioApi_emptyAudioCompatibility_0(); }

/* 025E18D4 */
static u32 mDoAud_monsSeInit() { return audioApi_JAIZelBasic_monsSeInit_0(gabi::ea(zel_basic)); }

/* 025E18E0 */
static u32 mDoAud_sceneBgmStart(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1,
                                f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_sceneBgmStart_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, a6, f1, f2,
                                                f3, f4, f5, f6, f7, f8);
}

/* 025E18EC */
static u32 mDoAud_bgmStart(u32 a0, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_bgmStart_0(gabi::ea(zel_basic), a0, 0x00000000u, 0x00000000u, f1,
                                           f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1904 */
static u32 mDoAud_bgmStop(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1,
                          f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_bgmStop_0(gabi::ea(zel_basic), a0, 0x00000000u, a3, a4, a5, a6, a7,
                                          f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1918 */
static u32 mDoAud_subBgmStart(u32 a0, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7,
                              f64 f8) {
    return audioApi_JAIZelBasic_subBgmStart_0(gabi::ea(zel_basic), a0, f1, f2, f3, f4, f5, f6, f7,
                                              f8);
}

/* 025E1928 */
static u32 mDoAud_subBgmStop() { return audioApi_JAIZelBasic_subBgmStop_0(gabi::ea(zel_basic)); }

/* 025E1934 */
static u32 mDoAud_bgmStreamPrepare(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                   f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_bgmStreamPrepare_0(gabi::ea(zel_basic), a0, a2, a3, a4, a5, a6, a7,
                                                   f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1944 */
static u32 mDoAud_bgmStreamPlay(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_bgmStreamPlay_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, a6, a7, f1,
                                                f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1950 */
static u32 mDoAud_changeBgmStatus(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f64 f1, f64 f2,
                                  f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_changeBgmStatus_0(gabi::ea(zel_basic), a0, a2, a3, a4, a5, f1, f2,
                                                  f3, f4, f5, f6, f7, f8);
}

/* 025E1960 */
static u32 mDoAud_bgmAllMute(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1,
                             f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_bgmAllMute_0(gabi::ea(zel_basic), a0, a2, a3, a4, a5, a6, a7, f1,
                                             f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1970 */
static u32 mDoAud_stWaterLevelUp(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_stWaterLevelUp_0(gabi::ea(zel_basic), f1, f2, f3, f4, f5, f6, f7,
                                                 f8);
}

/* 025E197C */
static u32 mDoAud_stWaterLevelDown(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_stWaterLevelDown_0(gabi::ea(zel_basic), f1, f2, f3, f4, f5, f6, f7,
                                                   f8);
}

/* 025E19CC */
static u32 mDoAud_seStartPosition(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                  f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_seStart_1(gabi::ea(zel_basic), a0, a1, 0x00000000u, 0x00000000u,
                                          0x00000000u, a6, a7, l_soundOne, l_soundOne,
                                          l_soundNegative, l_soundNegative, f5, f6, f7, f8);
}

/* 025E1A04 */
static u32 mDoAud_seStartCompatibility2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6,
                                        f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7,
                                        f64 f8) {
    return audioApi_JAIZelBasic_seStart_1(gabi::ea(zel_basic), a0, a1, a2, 0x00000000u, 0x00000000u,
                                          a6, SoundConstantPage, l_soundOne, l_soundOne,
                                          l_soundNegative, l_soundNegative, f5, f6, f7, f8);
}

/* 025E1A40 */
static u32 mDoAud_seStartCompatibility3(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6,
                                        f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7,
                                        f64 f8) {
    return audioApi_JAIZelBasic_seStart_1(gabi::ea(zel_basic), a0, a1, a2, a3, 0x00000000u, a6,
                                          SoundConstantPage, l_soundOne, l_soundOne,
                                          l_soundNegative, l_soundNegative, f5, f6, f7, f8);
}

/* 025E1A7C */
static u32 mDoAud_monsSeStart(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1,
                              f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_monsSeStart_0(gabi::ea(zel_basic), a0, a1, a2, a3, 0x00000000u, a6,
                                              f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1AA4 */
static u32 mDoAud_monsSeStartCompatibility2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6,
                                            f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7,
                                            f64 f8) {
    return audioApi_JAIZelBasic_monsSeStart_0(gabi::ea(zel_basic), a0, a1, a2, a3, a4, a6, f1, f2,
                                              f3, f4, f5, f6, f7, f8);
}

/* 025E1ACC */
static u32 mDoAud_seStop(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1,
                         f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_seStop_0(gabi::ea(zel_basic), a0, 0x00000000u, a3, a4, a5, a6, a7,
                                         f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1AE0 */
static u32 mDoAud_seStopCompatibility2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6,
                                       u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6,
                                       f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_seStop_0(gabi::ea(zel_basic), a0, a1, a3, a4, a5, a6, a7, f1, f2,
                                         f3, f4, f5, f6, f7, f8);
}

/* 025E1AF8 */
static u32 mDoAud_seStopActor(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                              f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_seStopActor_0(gabi::ea(zel_basic), a0, 0xFFFFFFFFu, a3, a4, a5, a6,
                                              a7, f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1B0C */
static u32 mDoAud_seStopActorCompatibility2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6,
                                            u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6,
                                            f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_seStopActor_0(gabi::ea(zel_basic), a0, a1, a3, a4, a5, a6, a7, f1,
                                              f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1B24 */
static u32 mDoAud_checkSePlaying(u32 a0) {
    return audioApi_JAIZelBasic_checkSePlaying_0(gabi::ea(zel_basic), a0);
}

/* 025E1B34 */
static u32 mDoAud_seDeleteObjectCompatibility(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5,
                                              u32 a6, u32 a7, f64 f1, f64 f2, f64 f3, f64 f4,
                                              f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_seDeleteObject_0(gabi::ea(zel_basic), a0, a2, a3, a4, a5, a6, a7,
                                                 f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1B44 */
static u32 mDoAud_getCameraInfo(u32 a0, u32 a1, u32 a2) {
    return audioApi_JAIZelBasic_getCameraInfo_0(gabi::ea(zel_basic), a0, a1, a2);
}

/* 025E1B60 */
static u32 mDoAud_getCameraMapInfo(u32 a0) {
    return audioApi_JAIZelBasic_getCameraMapInfo_0(gabi::ea(zel_basic), a0);
}

/* 025E1B70 */
static u32 mDoAud_initSeaEnvPos() {
    return audioApi_JAIZelBasic_initSeaEnvPos_0(gabi::ea(zel_basic));
}

/* 025E1B7C */
static u32 mDoAud_registSeaEnvPos(u32 a0) {
    return audioApi_JAIZelBasic_registSeaEnvPos_0(gabi::ea(zel_basic), a0);
}

/* 025E1B8C */
static u32 mDoAud_seaEnvSePlay(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1,
                               f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_seaEnvSePlay_0(gabi::ea(zel_basic), a0, 0x00000000u, a3, a4, a5, a6,
                                               f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1BA0 */
static u32 mDoAud_seaShoreSE(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1,
                             f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_seaShoreSE_0(gabi::ea(zel_basic), a1, a0, 0x00000000u, 0x00000000u,
                                             a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1BB8 */
static u32 mDoAud_seaShoreSECompatibility2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6,
                                           u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6,
                                           f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_seaShoreSE_0(gabi::ea(zel_basic), a1, a0, 0x00000001u, 0x00000000u,
                                             a5, a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1BD0 */
static u32 mDoAud_registRiverPos(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                 f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_registRiverPos_0(gabi::ea(zel_basic), a0, a2, a3, a4, a5, a6, a7,
                                                 f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1BE0 */
static u32 mDoAud_riverSePlay(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                              f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_riverSePlay_0(gabi::ea(zel_basic), a0, a1, a3, a4, a5, a6, a7, f1,
                                              f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1BF8 */
static u32 mDoAud_initRiverPos() {
    return audioApi_JAIZelBasic_initRiverPos_0(gabi::ea(zel_basic));
}

/* 025E1C04 */
static u32 mDoAud_waterfallSePlay(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                  f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_waterfallSePlay_0(gabi::ea(zel_basic), a0, a1, a2, a4, a5, a6, a7,
                                                  f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1C20 */
static u32 mDoAud_initWindowPos() {
    return audioApi_JAIZelBasic_initWindowPos_0(gabi::ea(zel_basic));
}

/* 025E1C2C */
static u32 mDoAud_registWindowPos(u32 a0) {
    return audioApi_JAIZelBasic_registWindowPos_0(gabi::ea(zel_basic), a0);
}

/* 025E1C3C */
static u32 mDoAud_messageSePlay(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1,
                                f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_messageSePlay_0(gabi::ea(zel_basic), a0, a1, a2, a4, a5, a6, f1, f2,
                                                f3, f4, f5, f6, f7, f8);
}

/* 025E1C58 */
static u32 mDoAud_talkIn(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1,
                         f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_talkIn_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, a6, a7, f1, f2,
                                         f3, f4, f5, f6, f7, f8);
}

/* 025E1C64 */
static u32 mDoAud_talkOut(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_talkOut_0(gabi::ea(zel_basic), 0x0000001Eu, f1, f2, f3, f4, f5, f6,
                                          f7, f8);
}

/* 025E1C74 */
static u32 mDoAud_basicFinalizeStub() { return audioApi_basicFinalizeStub_0(gabi::ea(zel_basic)); }

/* 025E1C84 */
static u32 mDoAud_heartGaugeOn() {
    return audioApi_JAIZelBasic_heartGaugeOn_0(gabi::ea(zel_basic));
}

/* 025E1C90 */
static u32 mDoAud_linkVoiceStart(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f64 f1, f64 f2,
                                 f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_linkVoiceStart_0(gabi::ea(zel_basic), a0, a1, a2, a3, a5, f1, f2,
                                                 f3, f4, f5, f6, f7, f8);
}

/* 025E1CB4 */
static u32 mDoAud_getLinkVoiceVowel(u32 a0) {
    return audioApi_JAIZelBasic_getLinkVoiceVowel_0(gabi::ea(zel_basic), a0);
}

/* 025E1CC4 */
static u32 mDoAud_shipCruiseSePlay(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                   f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7) {
    return audioApi_JAIZelBasic_shipCruiseSePlay_0(gabi::ea(zel_basic), a0, a2, a3, a4, a5, a6, a7,
                                                   f1, f2, f3, f4, f5, f6, f7);
}

/* 025E1CD4 */
static u32 mDoAud_setShipSailState(u32 a0) {
    return audioApi_JAIZelBasic_setShipSailState_0(gabi::ea(zel_basic), a0);
}

/* 025E1CE4 */
static u32 mDoAud_setLinkHp(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2,
                            f64 f3, f64 f4, f64 f5, f64 f6, f64 f7) {
    return audioApi_JAIZelBasic_setLinkHp_0(gabi::ea(zel_basic), a0, a1, a3, a4, a5, a6, f1, f2, f3,
                                            f4, f5, f6, f7);
}

/* 025E1CFC */
static u32 mDoAud_bgmNowKaitengiri() {
    return audioApi_JAIZelBasic_bgmNowKaitengiri_0(gabi::ea(zel_basic));
}

/* 025E1D08 */
static u32 mDoAud_bgmSetSwordUsing(u32 a0) {
    return audioApi_JAIZelBasic_bgmSetSwordUsing_0(gabi::ea(zel_basic), a0);
}

/* 025E1D18 */
static u32 mDoAud_bgmMuteMtDragon() {
    return audioApi_JAIZelBasic_bgmMuteMtDragon_0(gabi::ea(zel_basic));
}

/* 025E1D24 */
static u32 mDoAud_stSkyCloisters(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_stSkyCloisters_0(gabi::ea(zel_basic), f1, f2, f3, f4, f5, f6, f7,
                                                 f8);
}

/* 025E1D30 */
static u32 mDoAud_bgmHitSound(u32 a0) {
    return audioApi_JAIZelBasic_bgmHitSound_0(gabi::ea(zel_basic), a0);
}

/* 025E1DD8 */
static u32 mDoAud_bgmNowBattle(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                               f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_bgmNowBattle_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, a6, a7, f1,
                                               f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1DE4 */
static u32 mDoAud_setCameraGroupInfo(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1,
                                     f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_setCameraGroupInfo_0(gabi::ea(zel_basic), a0, a2, a3, a4, a5, a6,
                                                     f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1DF4 */
static u32 mDoAud_setLinkGroupInfo(u32 a0) {
    return audioApi_JAIZelBasic_setLinkGroupInfo_0(gabi::ea(zel_basic), a0);
}

/* 025E1E04 */
static u32 mDoAud_setLinkBootsType(u32 a0) {
    return audioApi_JAIZelBasic_setLinkBootsType_0(gabi::ea(zel_basic), a0);
}

/* 025E1E14 */
static u32 mDoAud_setLinkSwordType(u32 a0, u32 a1) {
    return audioApi_JAIZelBasic_setLinkSwordType_0(gabi::ea(zel_basic), a0, a1);
}

/* 025E1E2C */
static u32 mDoAud_setLinkShieldType(u32 a0, u32 a1) {
    return audioApi_JAIZelBasic_setLinkShieldType_0(gabi::ea(zel_basic), a0, a1);
}

/* 025E1E44 */
static u32 mDoAud_setLinkOnBoard(u32 a0) {
    return audioApi_JAIZelBasic_setLinkOnBoard_0(gabi::ea(zel_basic), a0);
}

/* 025E1E54 */
static u32 mDoAud_checkPlayingMainBgmFlag(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6,
                                          u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6,
                                          f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_checkPlayingMainBgmFlag_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5,
                                                          a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1E60 */
static u32 mDoAud_checkPlayingSubBgmFlag(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6,
                                         u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6,
                                         f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_checkPlayingSubBgmFlag_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5,
                                                         a6, a7, f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1E6C */
static u32 mDoAud_changeSubBgmStatus(u32 a0) {
    return audioApi_JAIZelBasic_changeSubBgmStatus_0(gabi::ea(zel_basic), a0);
}

/* 025E1E7C */
static u32 mDoAud_taktModeMute(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                               f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_taktModeMute_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, a6, a7, f1,
                                               f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1E88 */
static u32 mDoAud_taktModeMuteOff(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                  f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_taktModeMuteOff_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, a6, a7,
                                                  f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1E94 */
static u32 mDoAud_tact_reset(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, f64 f1, f64 f2,
                             f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelInst_reset_0(gabi::ea(&mTact), a1, a2, a3, a4, a5, a6, f1, f2, f3, f4, f5,
                                       f6, f7, f8);
}

/* 025E1EA0 */
static u32 mDoAud_tact_setStickPos() { return audioApi_JAIZelInst_setStickPos_0(); }

/* 025E1EB4 */
static u32 mDoAud_Inst_headerObject_ct() {
    return audioApi_Inst_headerObject_ct_0(gabi::ea(&mTact));
}

/* 025E1EC0 */
static u32 mDoAud_tact_emptyMember() { return audioApi_JAIZelInst_emptyMember_0(gabi::ea(&mTact)); }

/* 025E1ED4 */
static u32 mDoAud_Inst_headerObject_dt() {
    return audioApi_Inst_headerObject_dt_0(gabi::ea(&mTact));
}

/* 025E1EE0 */
static u32 mDoAud_tact_setBeat(u32 a0) {
    return audioApi_JAIZelInst_setBeat_0(gabi::ea(&mTact), a0);
}

/* 025E1EF0 */
static u32 mDoAud_tact_setVolume(f64 f1) {
    return audioApi_JAIZelInst_setVolume_0(gabi::ea(&mTact), f1);
}

/* 025E1EFC */
static u32 mDoAud_tact_getBeat() { return mTact.mBeat; }

// HD: the conducting beat length is returned in f1, preserving the float ABI.
/* 025E1F08 */
static f32 mDoAud_tact_getBeatFrames() { return mTact.mBeatFrames; }

/* 025E1F14 */
static u32 mDoAud_tact_ambientPlay(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelInst_ambientPlay_0(gabi::ea(&mTact), f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1F20 */
static u32 mDoAud_tact_metronomePlay(u32 a0, u32 a1, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6,
                                     f64 f7, f64 f8) {
    return audioApi_JAIZelInst_metronomePlay_0(gabi::ea(&mTact), a0, a1, f1, f2, f3, f4, f5, f6, f7,
                                               f8);
}

/* 025E1F34 */
static u32 mDoAud_tact_judge(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, f64 f1,
                             f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelInst_judge_0(gabi::ea(&mTact), a0, a1, a3, a4, a5, a6, a7, f1, f2, f3, f4,
                                       f5, f6, f7, f8);
}

/* 025E1F48 */
static u32 mDoAud_tact_armSoundPlay(u32 a0) {
    return audioApi_JAIZelInst_armSoundPlay_0(gabi::ea(&mTact), a0);
}

/* 025E1F58 */
static u32 mDoAud_tact_melodyPlay(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                  f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelInst_melodyPlay_0(gabi::ea(&mTact), a0, a2, a3, a4, a5, a6, a7, f1, f2,
                                            f3, f4, f5, f6, f7, f8);
}

// HD: melody lookup returns f32 in f1 and forwards all three integer arguments.
/* 025E1F68 */
static f32 mDoAud_tact_getMelodyPattern(s32 n, s32 step, u32 out) {
    return audioApi_JAIZelInst_getMelodyPattern_0(gabi::ea(&mTact), n, step, out);
}

/* 025E1F80 */
static u32 mDoAud_setHour(u32 a0) {
    zel_basic->mHour = a0;
    return a0;
}

/* 025E1F90 */
static u32 mDoAud_setMinute(u32 a0) {
    zel_basic->mMinute = a0;
    return a0;
}

/* 025E1FA0 */
static u32 mDoAud_setWeekday(u32 a0) {
    zel_basic->mWeekday = a0;
    return a0;
}

/* 025E1FB0 */
static u32 mDoAud_cbPracticePlay(u32 a0, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7,
                                 f64 f8) {
    return audioApi_JAIZelBasic_cbPracticePlay_0(gabi::ea(zel_basic), a0, f1, f2, f3, f4, f5, f6,
                                                 f7, f8);
}

/* 025E1FC0 */
static u32 mDoAud_cbPracticeStop(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                 f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_cbPracticeStop_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, a6, a7,
                                                 f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1FCC */
static u32 mDoAud_checkCbPracticePlay(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6,
                                      u32 a7, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6,
                                      f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_checkCbPracticePlay_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, a6,
                                                      a7, f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1FD8 */
static u32 mDoAud_onEnemyDamage(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f64 f1, f64 f2,
                                f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_onEnemyDamage_0(gabi::ea(zel_basic), a1, a2, a3, a4, a5, f1, f2, f3,
                                                f4, f5, f6, f7, f8);
}

/* 025E1FE4 */
static u32 mDoAud_prepareLandingDemo(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                                     f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7,
                                     f64 f8) {
    return audioApi_JAIZelBasic_prepareLandingDemo_0(gabi::ea(zel_basic), a0, a2, a3, a4, a5, a6,
                                                     a7, f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E1FF4 */
static u32 mDoAud_startLandingDemo(f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_startLandingDemo_0(gabi::ea(zel_basic), f1, f2, f3, f4, f5, f6, f7,
                                                   f8);
}

/* 025E2000 */
static u32 mDoAud_endLandingDemo() {
    return audioApi_JAIZelBasic_endLandingDemo_0(gabi::ea(zel_basic));
}

/* 025E200C */
static u32 mDoAud_rainPlay(u32 a0, f64 f1, f64 f2, f64 f3, f64 f4, f64 f5, f64 f6, f64 f7, f64 f8) {
    return audioApi_JAIZelBasic_rainPlay_0(gabi::ea(zel_basic), a0, f1, f2, f3, f4, f5, f6, f7, f8);
}

/* 025E201C */
static u32 mDoAud_setSpecialBgmFlag(u32 a0) {
    zel_basic->field_0x00bf = a0;
    return a0;
}

// HD: compiler-generated and template code of this translation unit.
/* 025E20F0 */
static u32 audioHeaderResultStub() { return 0; }
/* 025E20F8 */
static u32 audioHeaderResultStubCopy() { return 0; }
/* 025E2100 */
static void destroyAudioObject(u32 object, u32 flags) {
    if (object == 0)
        return;
    if ((flags & 1) == 0)
        return;
    audioApi_JKRHeap_operator_delete_0(object);
}
} // namespace m_Do_audio_cpp

using namespace m_Do_audio_cpp;
/* WWHD glue
data l_header 1048ABBC
data l_registration 101F46E0
data mLoadTimer 101F4707
data mResetFlag 101F4705
data zel_basic 101FFC78
data mTact 1048CDD0
data g_mDoAud_zelAudio 1048ABD8
data l_tactThreshold 100584FC
data l_soundOne 10058500
data l_soundNegative 10058504
ext 0207A9A0 u32 audioApi_cLib_calcTimer_u8_0(u32)
ext 025E15CC void audioApi_calcLoadTimer_0()
ext 020277A4 void audioApi_JAIZelBasic_setEventBit_0(u32, u32)
ext 020079B4 f64 audioApi_CPad_GET_STICK_VALUE_0(u32)
ext 02007A1C s32 audioApi_CPad_GET_STICK_ANGLE_0(u32)
ext 02007B44 f64 audioApi_CPad_GET_SUBSTICK_VALUE_0(u32)
ext 02007BA0 s32 audioApi_CPad_GET_SUBSTICK_ANGLE_0(u32)
ext 02027814 void audioApi_JAIZelBasic_setSceneName_0(u32, u32, u32, u32)
ext 02026E04 void audioApi_JAIZelBasic_load1stDynamicWave_0(u32)
ext 020311A0 u32 audioApi_hdArmBgmSe_0()
ext 0201EBA0 u32 audioApi_JAIZelBasic_seStart_0(u32, u32, u32, u32, u32, u32, f32, f32, f32, f32)
ext 020263C0 void audioApi_JAIZelBasic_resetProcess_0(u32)
ext 02030C10 void audioApi_hdDisarmBgmSe_0()
ext 0202736C void audioApi_JAIZelBasic_resetRecover_0(u32)
ext 02029E5C void audioApi_JAIZelInst_ct_0(u32)
ext 02025B44 void audioApi_createJAIZelBasic_0(u32)
ext 02027AE4 u32 audioApi_JAIZelBasic_checkFirstWaves_0()
ext 0202796C u32 audioApi_JAIZelBasic_check1stDynamicWave_0(u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 020271E4 u32 audioApi_JAIZelBasic_load2ndDynamicWave_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 028024E0 u32 audioApi_emptyAudioCompatibility_0()
ext 0202573C u32 audioApi_JAIZelBasic_monsSeInit_0(u32)
ext 020234A4 u32 audioApi_JAIZelBasic_sceneBgmStart_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0202204C u32 audioApi_JAIZelBasic_bgmStart_0(u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02021F28 u32 audioApi_JAIZelBasic_bgmStop_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201DE00 u32 audioApi_JAIZelBasic_subBgmStart_0(u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201D8BC u32 audioApi_JAIZelBasic_subBgmStop_0(u32)
ext 02027168 u32 audioApi_JAIZelBasic_bgmStreamPrepare_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02027F04 u32 audioApi_JAIZelBasic_bgmStreamPlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02027FD4 u32 audioApi_JAIZelBasic_changeBgmStatus_0(u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0202844C u32 audioApi_JAIZelBasic_bgmAllMute_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 020284B0 u32 audioApi_JAIZelBasic_stWaterLevelUp_0(u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 020284E4 u32 audioApi_JAIZelBasic_stWaterLevelDown_0(u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201EBA0 u32 audioApi_JAIZelBasic_seStart_1(u32, u32, u32, u32, u32, u32, u32, u32, f32, f32, f32, f32, f64, f64, f64, f64)
ext 020299D8 u32 audioApi_JAIZelBasic_monsSeStart_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201E538 u32 audioApi_JAIZelBasic_seStop_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02029044 u32 audioApi_JAIZelBasic_seStopActor_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201D62C u32 audioApi_JAIZelBasic_checkSePlaying_0(u32, u32)
ext 02029070 u32 audioApi_JAIZelBasic_seDeleteObject_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02027558 u32 audioApi_JAIZelBasic_getCameraInfo_0(u32, u32, u32, u32)
ext 02027668 u32 audioApi_JAIZelBasic_getCameraMapInfo_0(u32, u32)
ext 0201C4B8 u32 audioApi_JAIZelBasic_initSeaEnvPos_0(u32)
ext 0201C4C4 u32 audioApi_JAIZelBasic_registSeaEnvPos_0(u32, u32)
ext 0201C524 u32 audioApi_JAIZelBasic_seaEnvSePlay_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201CA10 u32 audioApi_JAIZelBasic_seaShoreSE_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201CC50 u32 audioApi_JAIZelBasic_registRiverPos_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201CDA0 u32 audioApi_JAIZelBasic_riverSePlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201CC44 u32 audioApi_JAIZelBasic_initRiverPos_0(u32)
ext 0201CF54 u32 audioApi_JAIZelBasic_waterfallSePlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201D140 u32 audioApi_JAIZelBasic_initWindowPos_0(u32)
ext 0201D14C u32 audioApi_JAIZelBasic_registWindowPos_0(u32, u32)
ext 0202914C u32 audioApi_JAIZelBasic_messageSePlay_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02027D4C u32 audioApi_JAIZelBasic_talkIn_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201DB50 u32 audioApi_JAIZelBasic_talkOut_0(u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02029C98 u32 audioApi_basicFinalizeStub_0(u32)
ext 02027D40 u32 audioApi_JAIZelBasic_heartGaugeOn_0(u32)
ext 020293CC u32 audioApi_JAIZelBasic_linkVoiceStart_0(u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 020292B4 u32 audioApi_JAIZelBasic_getLinkVoiceVowel_0(u32, u32)
ext 02029638 u32 audioApi_JAIZelBasic_shipCruiseSePlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64)
ext 020299D0 u32 audioApi_JAIZelBasic_setShipSailState_0(u32, u32)
ext 02027B0C u32 audioApi_JAIZelBasic_setLinkHp_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64)
ext 02028D6C u32 audioApi_JAIZelBasic_bgmNowKaitengiri_0(u32)
ext 02028FE0 u32 audioApi_JAIZelBasic_bgmSetSwordUsing_0(u32, u32)
ext 020283D8 u32 audioApi_JAIZelBasic_bgmMuteMtDragon_0(u32)
ext 020283E4 u32 audioApi_JAIZelBasic_stSkyCloisters_0(u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02028E54 u32 audioApi_JAIZelBasic_bgmHitSound_0(u32, u32)
ext 0202879C u32 audioApi_JAIZelBasic_bgmNowBattle_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02027670 u32 audioApi_JAIZelBasic_setCameraGroupInfo_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02027778 u32 audioApi_JAIZelBasic_setLinkGroupInfo_0(u32, u32)
ext 02027CEC u32 audioApi_JAIZelBasic_setLinkBootsType_0(u32, u32)
ext 02027C50 u32 audioApi_JAIZelBasic_setLinkSwordType_0(u32, u32, u32)
ext 02027C9C u32 audioApi_JAIZelBasic_setLinkShieldType_0(u32, u32, u32)
ext 02027CF4 u32 audioApi_JAIZelBasic_setLinkOnBoard_0(u32, u32)
ext 0202348C u32 audioApi_JAIZelBasic_checkPlayingMainBgmFlag_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0202149C u32 audioApi_JAIZelBasic_checkPlayingSubBgmFlag_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02027FF0 u32 audioApi_JAIZelBasic_changeSubBgmStatus_0(u32, u32)
ext 02028518 u32 audioApi_JAIZelBasic_taktModeMute_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0202862C u32 audioApi_JAIZelBasic_taktModeMuteOff_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02029F10 u32 audioApi_JAIZelInst_reset_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0202A038 u32 audioApi_JAIZelInst_setStickPos_0()
ext 0202ABA0 u32 audioApi_Inst_headerObject_ct_0(u32)
ext 0202A034 u32 audioApi_JAIZelInst_emptyMember_0(u32)
ext 0202AB9C u32 audioApi_Inst_headerObject_dt_0(u32)
ext 0202A040 u32 audioApi_JAIZelInst_setBeat_0(u32, u32)
ext 0202A14C u32 audioApi_JAIZelInst_setVolume_0(u32, f64)
ext 0202A77C u32 audioApi_JAIZelInst_ambientPlay_0(u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0202A1B8 u32 audioApi_JAIZelInst_metronomePlay_0(u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0202A3B4 u32 audioApi_JAIZelInst_judge_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0202A87C u32 audioApi_JAIZelInst_armSoundPlay_0(u32, u32)
ext 0202A8F0 u32 audioApi_JAIZelInst_melodyPlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0202AA5C f32 audioApi_JAIZelInst_getMelodyPattern_0(u32, u32, u32, u32)
ext 02028038 u32 audioApi_JAIZelBasic_cbPracticePlay_0(u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 020282F8 u32 audioApi_JAIZelBasic_cbPracticeStop_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 020214B4 u32 audioApi_JAIZelBasic_checkCbPracticePlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0201E4AC u32 audioApi_JAIZelBasic_onEnemyDamage_0(u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02028FE8 u32 audioApi_JAIZelBasic_prepareLandingDemo_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02028FF8 u32 audioApi_JAIZelBasic_startLandingDemo_0(u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 02029038 u32 audioApi_JAIZelBasic_endLandingDemo_0(u32)
ext 0201D1AC u32 audioApi_JAIZelBasic_rainPlay_0(u32, u32, f64, f64, f64, f64, f64, f64, f64, f64)
ext 0273AF40 void audioApi_JKRHeap_operator_delete_0(u32)
*/
