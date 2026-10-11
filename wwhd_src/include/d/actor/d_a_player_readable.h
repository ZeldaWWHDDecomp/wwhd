// Unit-private HD views and method declarations; shared player layouts stay
// canonical.
#pragma once
#include "d/actor/d_a_player.h"
#include "d/d_cc_d.h"
#include "f_op/f_op_camera.h"
#include "gc_api/d/d_com_inf_game.h"
namespace player_detail {
enum { AT_TYPE_WIND = 0x200000 };
struct Emitter {
    u8 beforeMaxFrame[0x5C];
    be<s32> mMaxFrame;
    u8 beforeCallback[0x1E4 - 0x60];
    gptr<void> mpCallBack;
    u8 beforeRotation[8];
    Mtx34 mGlobalRotation;
    u8 beforeTranslation[0xC];
    cXyz mGlobalTranslation;
    u8 beforeFlags[0x254 - 0x238];
    be<u32> mFlags;
};
WWHD_OFFSET(Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22C);
struct Camera {
    u8 beforePlayer[0x370];
    gptr<fopAc_ac_c> mpPlayerActor;
};
struct Attention {
    u8 beforeFlags[0x20];
    be<u32> mFlags;
};
inline dComIfG_inf_c *player_play() {
    return reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
}
struct daPy_mtxFollowEcallBack_c {
    be<u32> mVtable;
    gptr<Emitter> mpEmitter;
    gptr<void> mpMtx;
    void executeAfter(Emitter *emitter);
    void draw(Emitter *emitter);
    void endHook();
    void execute(Emitter *emitter);
    void end();
    void *makeEmitter(u32 particle, void *matrix, cXyz *position, cXyz *scale);
    void *makeEmitterColor(u32 particle, void *matrix, cXyz *position, void *primary,
                           void *environment);
    void setup(Emitter *emitter);
};
struct daPy_py_c : ::daPy_py_c {
    void onDekuSpReturnFlg(u8 mode);
    void voiceStart(u32 sound);
    void setOutPower(f32 power, s16 angle, int type);
    void onFrollCrashFlg(u32 flags);
    void setPlayerPosAndAngle(cXyz *pos, s16 angle);
    void setPlayerPosAndAngleAngles(cXyz *pos, csXyz *angle);
    void setPlayerPosAndAngleMatrix(Mtx34 *matrix);
    s32 getTactTimerCancel();
    s32 checkPlayerGuard();
    fopAc_ac_c *getGrabMissActor();
    s32 checkPlayerFly();
    s32 checkFrontRoll();
    s32 checkBottleSwing();
    s32 checkCutCharge();
    s32 getBokoFlamePos(cXyz *position);
    s32 checkTactWait();
    s32 checkComboCutTurn();
    s32 checkGrabBarrel();
    s32 checkPlayerNoDraw();
    s32 checkRopeTag();
    s32 checkRopeReadyAnime();
    s32 setHookshotCarryOffset(u32 actorID, const cXyz *offset);
    s32 setThrowDamage(cXyz *position, s16 angle, f32 speed, f32 speedY, int type);
    s32 getTactMusic();
    s32 getItemID();
    s32 getThrowBoomerangID();
    s32 getGrabActorID();
    void changePlayer(fopAc_ac_c *replacement);
    void setDoButtonQuake();
    void stopDoButtonQuake(s32 release);
    void objWindHitCheck(dCcD_Cyl *cylinder);
};
WWHD_SIZE(daPy_py_c, 0x43C);
} // namespace player_detail
void JPASetRMtxTVecfromMtx(void *, void *, void *);
void *player_particle_set(void *, u32, u32, cXyz *, u32, cXyz *, u32, void *, s32, void *, void *,
                          u32);
void player_StartQuake(void *, void *, s32, s32, cXyz *);
void player_StopQuake(void *, s32);
void player_actor_destructor(fopAc_ac_c *, u32);
void cLib_addCalc(be<f32> *, f32, f32, f32, f32);
void player_firstStaticConstructor(void *);
void player_secondStaticConstructor(void *);
