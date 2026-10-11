/** Audio frame, scene, sound and conducting wrappers. Private game-derived source.
 * HD: singleton shims retain full GHS forwarding signatures, including unused ABI arguments.
 */
#pragma once
#include "JAZelAudio/JAIZelBasic.h"
#include "JAZelAudio/JAIZelInst.h"
#include "gc_api/d/d_com_inf_game.h"
u32 audioApi_cLib_calcTimer_u8_0(u32);
void audioApi_calcLoadTimer_0();
void audioApi_JAIZelBasic_setEventBit_0(u32, u32);
f64 audioApi_CPad_GET_STICK_VALUE_0(u32);
s32 audioApi_CPad_GET_STICK_ANGLE_0(u32);
f64 audioApi_CPad_GET_SUBSTICK_VALUE_0(u32);
s32 audioApi_CPad_GET_SUBSTICK_ANGLE_0(u32);
void audioApi_JAIZelBasic_setSceneName_0(u32, u32, u32, u32);
void audioApi_JAIZelBasic_load1stDynamicWave_0(u32);
u32 audioApi_hdArmBgmSe_0();
u32 audioApi_JAIZelBasic_seStart_0(u32, u32, u32, u32, u32, u32, f32, f32, f32, f32);
void audioApi_JAIZelBasic_resetProcess_0(u32);
void audioApi_hdDisarmBgmSe_0();
void audioApi_JAIZelBasic_resetRecover_0(u32);
void audioApi_JAIZelInst_ct_0(u32);
void audioApi_createJAIZelBasic_0(u32);
u32 audioApi_JAIZelBasic_checkFirstWaves_0();
u32 audioApi_JAIZelBasic_check1stDynamicWave_0(u32, u32, u32, u32, u32, u32, f64, f64, f64, f64,
                                               f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_load2ndDynamicWave_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64,
                                              f64, f64, f64, f64, f64);
u32 audioApi_emptyAudioCompatibility_0();
u32 audioApi_JAIZelBasic_monsSeInit_0(u32);
u32 audioApi_JAIZelBasic_sceneBgmStart_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                         f64, f64, f64);
u32 audioApi_JAIZelBasic_bgmStart_0(u32, u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_bgmStop_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                   f64, f64, f64);
u32 audioApi_JAIZelBasic_subBgmStart_0(u32, u32, f64, f64, f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_subBgmStop_0(u32);
u32 audioApi_JAIZelBasic_bgmStreamPrepare_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64,
                                            f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_bgmStreamPlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64,
                                         f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_changeBgmStatus_0(u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                           f64, f64, f64);
u32 audioApi_JAIZelBasic_bgmAllMute_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64,
                                      f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_stWaterLevelUp_0(u32, f64, f64, f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_stWaterLevelDown_0(u32, f64, f64, f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_seStart_1(u32, u32, u32, u32, u32, u32, u32, u32, f32, f32, f32, f32, f64,
                                   f64, f64, f64);
u32 audioApi_JAIZelBasic_monsSeStart_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                       f64, f64, f64);
u32 audioApi_JAIZelBasic_seStop_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                  f64, f64, f64);
u32 audioApi_JAIZelBasic_seStopActor_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64,
                                       f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_checkSePlaying_0(u32, u32);
u32 audioApi_JAIZelBasic_seDeleteObject_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64,
                                          f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_getCameraInfo_0(u32, u32, u32, u32);
u32 audioApi_JAIZelBasic_getCameraMapInfo_0(u32, u32);
u32 audioApi_JAIZelBasic_initSeaEnvPos_0(u32);
u32 audioApi_JAIZelBasic_registSeaEnvPos_0(u32, u32);
u32 audioApi_JAIZelBasic_seaEnvSePlay_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                        f64, f64, f64);
u32 audioApi_JAIZelBasic_seaShoreSE_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64,
                                      f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_registRiverPos_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64,
                                          f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_riverSePlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64,
                                       f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_initRiverPos_0(u32);
u32 audioApi_JAIZelBasic_waterfallSePlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64,
                                           f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_initWindowPos_0(u32);
u32 audioApi_JAIZelBasic_registWindowPos_0(u32, u32);
u32 audioApi_JAIZelBasic_messageSePlay_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                         f64, f64, f64);
u32 audioApi_JAIZelBasic_talkIn_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                  f64, f64, f64);
u32 audioApi_JAIZelBasic_talkOut_0(u32, u32, f64, f64, f64, f64, f64, f64, f64, f64);
u32 audioApi_basicFinalizeStub_0(u32);
u32 audioApi_JAIZelBasic_heartGaugeOn_0(u32);
u32 audioApi_JAIZelBasic_linkVoiceStart_0(u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                          f64, f64, f64);
u32 audioApi_JAIZelBasic_getLinkVoiceVowel_0(u32, u32);
u32 audioApi_JAIZelBasic_shipCruiseSePlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64,
                                            f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_setShipSailState_0(u32, u32);
u32 audioApi_JAIZelBasic_setLinkHp_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                     f64, f64);
u32 audioApi_JAIZelBasic_bgmNowKaitengiri_0(u32);
u32 audioApi_JAIZelBasic_bgmSetSwordUsing_0(u32, u32);
u32 audioApi_JAIZelBasic_bgmMuteMtDragon_0(u32);
u32 audioApi_JAIZelBasic_stSkyCloisters_0(u32, f64, f64, f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_bgmHitSound_0(u32, u32);
u32 audioApi_JAIZelBasic_bgmNowBattle_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64,
                                        f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_setCameraGroupInfo_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64,
                                              f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_setLinkGroupInfo_0(u32, u32);
u32 audioApi_JAIZelBasic_setLinkBootsType_0(u32, u32);
u32 audioApi_JAIZelBasic_setLinkSwordType_0(u32, u32, u32);
u32 audioApi_JAIZelBasic_setLinkShieldType_0(u32, u32, u32);
u32 audioApi_JAIZelBasic_setLinkOnBoard_0(u32, u32);
u32 audioApi_JAIZelBasic_checkPlayingMainBgmFlag_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64,
                                                   f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_checkPlayingSubBgmFlag_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64,
                                                  f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_changeSubBgmStatus_0(u32, u32);
u32 audioApi_JAIZelBasic_taktModeMute_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64,
                                        f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_taktModeMuteOff_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64,
                                           f64, f64, f64, f64, f64);
u32 audioApi_JAIZelInst_reset_0(u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64,
                                f64, f64);
u32 audioApi_JAIZelInst_setStickPos_0();
u32 audioApi_Inst_headerObject_ct_0(u32);
u32 audioApi_JAIZelInst_emptyMember_0(u32);
u32 audioApi_Inst_headerObject_dt_0(u32);
u32 audioApi_JAIZelInst_setBeat_0(u32, u32);
u32 audioApi_JAIZelInst_setVolume_0(u32, f64);
u32 audioApi_JAIZelInst_ambientPlay_0(u32, f64, f64, f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelInst_metronomePlay_0(u32, u32, u32, f64, f64, f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelInst_judge_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64,
                                f64, f64, f64);
u32 audioApi_JAIZelInst_armSoundPlay_0(u32, u32);
u32 audioApi_JAIZelInst_melodyPlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64, f64,
                                     f64, f64, f64, f64);
f32 audioApi_JAIZelInst_getMelodyPattern_0(u32, u32, u32, u32);
u32 audioApi_JAIZelBasic_cbPracticePlay_0(u32, u32, f64, f64, f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_cbPracticeStop_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64,
                                          f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_checkCbPracticePlay_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64,
                                               f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_onEnemyDamage_0(u32, u32, u32, u32, u32, u32, f64, f64, f64, f64, f64, f64,
                                         f64, f64);
u32 audioApi_JAIZelBasic_prepareLandingDemo_0(u32, u32, u32, u32, u32, u32, u32, u32, f64, f64, f64,
                                              f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_startLandingDemo_0(u32, f64, f64, f64, f64, f64, f64, f64, f64);
u32 audioApi_JAIZelBasic_endLandingDemo_0(u32);
u32 audioApi_JAIZelBasic_rainPlay_0(u32, u32, f64, f64, f64, f64, f64, f64, f64, f64);
void audioApi_JKRHeap_operator_delete_0(u32);

namespace m_Do_audio_cpp {
struct AudioRegistration {
    u8 records[36];
};
WWHD_SIZE(AudioRegistration, 36);
enum {
    SceneLoadFrames = 36,
    ArmBgmSound = 0x806,
    AudioVtableAddress = 0x100584B4,
    SoundConstantPage = 0x10060000
};
static_assert(SAVE_OFF(mSavedata.mEvent) == 0x644);
} // namespace m_Do_audio_cpp
