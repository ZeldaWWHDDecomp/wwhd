// Player NPC isolated reference implementation.
// Includes inferred per-TU initializer; excludes Pet and shared virtual targets.
// HD: virtual stub signatures retain only the arguments observed by the unit specs;
// return widths and empty bodies are preserved even where the GC API uses pointers.
#include "d/actor/d_a_player_npc.h"
#include <cmath>

static f32 l_zero = 0.0f;
static f32 l_noGround = -1000000000.0f;
static f32 l_homeOffset = 100.0f;
static f32 l_fogSpan = 200.0f;
static f32 l_searchRadius = 300.0f;
static f32 l_minBlockSpeed = 0.0010000000474974513f;
static f32 l_blockDotLimit = -0.707099974155426f;
static f32 l_minusPi = -3.1415927410125732f;
static f32 l_pi = 3.1415927410125732f;
static f32 l_largeRadius = 50000.0f;
static f32 l_smallRadius = 10000.0f;
static u32 l_npcVtable = 0x10037F50;
static u32 l_grpVtable = 0x10037E58;
static u32 l_chkVtable = 0x10037E48;
static u32 l_polyVtable = 0x10037E68;
static u32 l_offsetGuard;
static PlayerNpcVector l_offsetPos = {0, 0, 0};
static u32 l_headerWords[4] = {};
static f32 l_angleMin;
static f32 l_angleMax;
static f32 l_radiusMaxA;
static f32 l_radiusMaxB;
static f32 l_radiusMinA;
static f32 l_radiusMinB;
static u8 l_headerFlag;
static u8 l_headerByte;
static u8 l_headerDtor[12] = {};
static u8 l_flagDtor[12] = {};
static u8 l_byteDtor[12] = {};
static u32 l_partnerManager;
static u32 l_timer;


static void npcCopyWords(u32 dst, u32 src) {
    npcVector(dst)->x = static_cast<u32>(npcVector(src)->x);
    npcVector(dst)->y = static_cast<u32>(npcVector(src)->y);
    npcVector(dst)->z = static_cast<u32>(npcVector(src)->z);
}

/* 02444F74 */
void* daPy_npc_JudgeForPNameAndDistance(void* actor, judge_parm_c* parameter) {
    const u32 a = gabi::ea(actor), p = gabi::ea(parameter);
    const s16 required = static_cast<s16>(npcJudge(p)->mProcName);
    const s16 name = a ? static_cast<s16>(npcActor(a)->mProcName) : 0x7FFF;
    if (name == required) {
        gabi::Local<PlayerNpcVector> relative;
        const u32 v = gabi::ea(relative.get());
        npcVectorSubtract(gabi::ea(&npcActor(a)->current.pos.x), v, gabi::ea(&npcJudge(p)->mAreaCenter));
        const f64 square = npcVectorSquareMag(v);
        const f64 distance = npcSqrt(square);
        if (distance < static_cast<f32>(npcJudge(p)->mAreaRadius) && distance < static_cast<f32>(npcJudge(p)->mMinDist)) {
            npcJudge(p)->mpActor = a;
            npcJudge(p)->mMinDist = static_cast<f32>(distance);
            npcCopyWords(gabi::ea(&npcJudge(p)->mActorRelPos), v);
        }
    }
    return nullptr;
}

/* 02445018 */
void* daPy_npc_SearchAreaByName(void* actor, s16 processName, f32 radius,
                               PlayerNpcVector* output) {
    const u32 a = gabi::ea(actor), out = gabi::ea(output);
    gabi::Local<judge_parm_c> parameter;
    const u32 p = gabi::ea(parameter.get());
    const u32 y = static_cast<u32>(npcWord<u32>(npcActor(a)->current.pos.y)), x = static_cast<u32>(npcWord<u32>(npcActor(a)->current.pos.x)), z = static_cast<u32>(npcWord<u32>(npcActor(a)->current.pos.z));
    const f32 zero = l_zero;
    npcJudge(p)->mAreaRadius = radius;
    npcJudge(p)->mAreaCenter.z = z;
    npcWord<f32>(npcJudge(p)->mActorRelPos.x) = zero;
    npcJudge(p)->mMinDist = radius;
    npcWord<f32>(npcJudge(p)->mActorRelPos.z) = zero;
    npcJudge(p)->mProcName = processName;
    npcJudge(p)->mpActor = 0;
    npcWord<f32>(npcJudge(p)->mActorRelPos.y) = zero;
    npcJudge(p)->mAreaCenter.x = x;
    npcJudge(p)->mAreaCenter.y = y;
    npcActorJudge(gabi::guest_fn((const void*)&daPy_npc_JudgeForPNameAndDistance), p);
    const u32 found = static_cast<u32>(npcJudge(p)->mpActor);
    if (out) {
        const u32 yOut = static_cast<u32>(npcJudge(p)->mActorRelPos.y), xOut = static_cast<u32>(npcJudge(p)->mActorRelPos.x), zOut = static_cast<u32>(npcJudge(p)->mActorRelPos.z);
        npcVector(out)->x = xOut; npcVector(out)->z = zOut; npcVector(out)->y = yOut;
    }
    return npcGuestPointer(found);
}

/* 024450B4 */
daPy_npc_c* daPy_npc_Construct(daPy_npc_c* actor) {
    u32 a = gabi::ea(actor);
    if (!a) { a = npcAllocate(0x608u); if (!a) return nullptr; }
    npcPlayerConstruct(a);
    npcActor(a)->__vtbl = l_npcVtable;
    npcAcchConstruct(gabi::ea(&npcActor(a)->mAcch));
    npcWord<u32>(npcActor(a)->mAcch.dBgS_GrpPassChk::__vtbl) = l_grpVtable;
    npcWord<u32>(npcActor(a)->mAcch.cBgS_Chk::__vtbl) = l_chkVtable;
    npcWord<u32>(npcActor(a)->mAcch.dBgS_PolyPassChk::__vtbl) = l_polyVtable;
    npcWord<u8>(npcActor(a)->mAcch.mbObjThrough) = 1;
    return npcActor(a);
}

/* 0244513C */
void daPy_npc_Destruct(daPy_npc_c* actor, s32 flags) {
    const u32 a = gabi::ea(actor);
    if (!a) return;
    npcWord<u32>(npcActor(a)->mAcch.dBgS_GrpPassChk::__vtbl) = l_grpVtable;
    npcWord<u32>(npcActor(a)->mAcch.dBgS_PolyPassChk::__vtbl) = l_polyVtable;
    npcAcchDestroy(gabi::ea(&npcActor(a)->mAcch), 0u);
    npcPlayerDestroy(a, 0u);
    if (static_cast<u32>(flags) & 1) npcDelete(a);
}

/* 024451B4 */
s32 daPy_npc_c::check_initialRoom() {
    const u32 a = gabi::ea(this);
    if (home.roomNo >= 0) return 1;
    u32 game = dComIfGp_ea();
    npcAcchCrrPos(gabi::ea(&mAcch), gabi::ea(&npcGame(game)->play.mBgS));
    const f32 ground = mAcch.m_ground_h, invalid = l_noGround;
    if (ground == invalid) return 0;
    game = dComIfGp_ea();
    if (npcGroundCode(gabi::ea(&npcGame(game)->play.mBgS), gabi::ea(static_cast<cBgS_PolyInfo*>(&npcActor(a)->mAcch.m_gnd))) == 4) return 0;
    game = dComIfGp_ea();
    const s32 room = npcGroundRoom(gabi::ea(&npcGame(game)->play.mBgS), gabi::ea(static_cast<cBgS_PolyInfo*>(&npcActor(a)->mAcch.m_gnd)));
    if (room < 0) return 0;
    dComIfGp_ea(); // Native accessor still occurs although status storage is a fixed global.
    const u32 statusAddress = gabi::ea(npcRoomStatusAt(room));
    if (!(static_cast<u8>(npcRoomStatus(statusAddress)->mFlags) & 0x10)) return 0;
    npcWord<u8>(home.roomNo) = static_cast<u8>(room);
    npcWord<u8>(current.roomNo) = static_cast<u8>(room);
    return -1;
}

/* 024452A0 */
s32 daPy_npc_c::check_moveStop() {
    const s32 room = current.roomNo;
    dComIfGp_ea();
    const bool loaded = (static_cast<u8>(npcRoomStatusAt(room)->mFlags) & 0x10) != 0;
    if (loaded && room >= 0) return 0;
    if (!loaded || m4E8 >= 30) {
        const u16 yaw = static_cast<u16>(npcWord<u16>(home.angle.y));
        const u32 y = static_cast<u32>(npcWord<u32>(home.pos.y));
        npcWord<u16>(shape_angle.y) = yaw;
        npcWord<u32>(current.pos.y) = y;
        const u32 z = static_cast<u32>(npcWord<u32>(home.pos.z)), x = static_cast<u32>(npcWord<u32>(home.pos.x));
        const f32 zero = l_zero;
        npcWord<u32>(current.pos.x) = x; speedF = zero; m4E8 = 0;
        const u32 xyAngle = static_cast<u32>(npcWord<u32>(home.angle.x)); const u16 pitch = static_cast<u16>(npcWord<u16>(home.angle.x));
        npcWord<u32>(current.angle.x) = xyAngle; const u16 roll = static_cast<u16>(npcWord<u16>(home.angle.z));
        npcWord<u32>(current.pos.z) = z;
        npcWord<u16>(shape_angle.z) = roll;
        const u32 roomWord = static_cast<u32>(npcWord<u32>(home.angle.z));
        npcWord<u16>(shape_angle.x) = pitch;
        npcWord<u32>(current.angle.z) = roomWord;
    }
    const u32 player = static_cast<u32>(npcWord<u32>(npcPlay()->play.mPlayerInfo[0].mpPlayer));
    if (static_cast<u16>(npcWord<u16>(npcActor(player)->eventInfo.mCommand)) != dEvtCmd_INDOOR_e) mNoResetFlg1 = mNoResetFlg1 & ~static_cast<u32>(daPyFlg1_NPC_CALL_COMMAND);
    return 1;
}

/* 02445398 */
void daPy_npc_c::unconditionalSetRestart(s8 option) {
    npcSetRestartOption(gabi::ea(&npcSave()->mRestart), option);
    u32 state = dComIfGs_ea();
    const s8 room = static_cast<s8>(npcWord<s8>(npcSaveView(state)->mRestart.mOptionRoomNo));
    const s16 yaw = static_cast<s16>(npcSaveView(state)->mRestart.mOptionRoomAngleY);
    npcSetPlayerPriest(gabi::ea(&npcSaveView(state)->mSavedata.mPlayer.mPriest), static_cast<u32>(static_cast<u8>(option)), gabi::ea(&npcSaveView(state)->mRestart.mOptionRoomPos), yaw, room);
    state = dComIfGs_ea(); npcCopyWords(gabi::ea(&home.pos.x), gabi::ea(&npcSaveView(state)->mRestart.mOptionRoomPos));
    state = dComIfGs_ea(); home.angle.y = static_cast<s16>(npcSaveView(state)->mRestart.mOptionRoomAngleY);
    state = dComIfGs_ea();
    npcWord<u8>(home.roomNo) = static_cast<u8>(npcWord<u8>(npcSaveView(state)->mRestart.mOptionRoomNo));
}

/* 02445438 */
void daPy_npc_c::setRestart(s8 option) {
    const u32 a = gabi::ea(this), initial = dComIfGs_ea();
    if (static_cast<s8>(npcWord<s8>(npcSaveView(initial)->mRestart.mOption)) != option) return;
    const u32 player = static_cast<u32>(npcWord<u32>(npcPlay()->play.mPlayerInfo[0].mpPlayer)), state = dComIfGs_ea();
    const u16 command = static_cast<u16>(npcWord<u16>(npcActor(player)->eventInfo.mCommand));
    const s8 room = current.roomNo, restartRoom = static_cast<s8>(npcWord<s8>(npcSaveView(state)->mRestart.mOptionRoomNo));
    if (command != dEvtCmd_INDOOR_e && room != restartRoom) npcUnconditionalRestartSlice(a, option);
}

/* 024454C8 */
void daPy_npc_c::setOffsetHomePos() {
    if (!static_cast<u32>(l_offsetGuard)) {
        l_offsetGuard = 1;
        const f32 zero = l_zero, offset = l_homeOffset;
        npcWord<f32>(l_offsetPos.z) = zero;
        npcWord<f32>(l_offsetPos.x) = offset;
        npcWord<f32>(l_offsetPos.y) = zero;
    }
    const s16 yaw = home.angle.y;
    npcOffsetPos(gabi::ea(&home.pos.x), gabi::ea(&home.pos.x), yaw, gabi::ea(&l_offsetPos));
}

/* 02445518 */
void daPy_npc_c::setPointRestart(s16 point, s8 option) {
    const u32 a = gabi::ea(this);
    if (!npcStage()->getPlayer()) npcAssert(0xAD, "dComIfGp_getStagePlayer() != NULL");
    u32 scene = npcStage()->getSclsInfo();
    if (!scene) npcAssert(0xAF, "sclsinfo != NULL");
    if (point < 0 || static_cast<s32>(point) >= static_cast<s32>(npcWord<s32>(npcScene(scene)->num))) npcAssert(0xB0, "0 <= i_point && i_point < sclsinfo->num");
    scene = static_cast<u32>(npcScene(scene)->entries);
    if (!scene) npcAssert(0xB3, "scls_data != NULL");
    const u32 playerInfo = npcStage()->getPlayer();
    u32 entry = static_cast<u32>(npcPlayerInfo(playerInfo)->entries);
    const u32 sceneEntry = gabi::ea(npcSceneEntry(scene) + static_cast<u32>(static_cast<s32>(point)));
    const u8 startCode = static_cast<u8>(npcWord<u8>(npcSceneEntry(sceneEntry)->mStart));
    u32 i = 0;
    s32 count = static_cast<s32>(npcStage()->getPlayerNum());
    while (static_cast<s32>(i) < count) {
        if (static_cast<u8>(npcWord<u8>(npcPlayerEntry(entry)->startCode())) == startCode) break;
        entry = gabi::ea(npcPlayerEntry(entry) + 1); ++i;
        count = static_cast<s32>(npcStage()->getPlayerNum());
    }
    if (i == npcStage()->getPlayerNum()) npcAssert(0xBE, "i != dComIfGp_getStagePlayerNum()");
    npcCopyWords(gabi::ea(&home.pos.x), gabi::ea(&npcPlayerEntry(entry)->position));
    const s16 entryYaw = static_cast<s16>(npcWord<s16>(npcPlayerEntry(entry)->angle.y));
    npcWord<u8>(home.roomNo) = 0xFF; home.angle.y = entryYaw;
    npcOffsetHomeSlice(a);
    // HD: the native captures different words around stores; avoid a struct assignment.
    const u32 angleXY = static_cast<u32>(npcWord<u32>(home.angle.x)); const u16 pitch = static_cast<u16>(npcWord<u16>(home.angle.x));
    const u32 roomWord = static_cast<u32>(npcWord<u32>(home.angle.z)), y = static_cast<u32>(npcWord<u32>(home.pos.y));
    npcWord<u16>(shape_angle.x) = pitch; const u16 roll = static_cast<u16>(npcWord<u16>(home.angle.z));
    npcWord<u32>(old.pos.y) = y; const s16 yaw = home.angle.y;
    npcWord<u16>(shape_angle.z) = roll; shape_angle.y = yaw;
        npcWord<u32>(old.angle.x) = angleXY;
    const u32 z = static_cast<u32>(npcWord<u32>(home.pos.z));
    npcWord<u32>(current.pos.y) = y;
        npcWord<u32>(current.pos.z) = z;
    const u32 x = static_cast<u32>(npcWord<u32>(home.pos.x));
    npcWord<u32>(old.pos.z) = z;
        npcWord<u32>(current.pos.x) = x;
        npcWord<u32>(current.angle.z) = roomWord;
    npcWord<u32>(old.pos.x) = x;
        npcWord<u32>(current.angle.x) = angleXY;
        npcWord<u32>(old.angle.z) = roomWord;
    npcSaveRestart(gabi::ea(&npcSave()->mRestart), option, gabi::ea(&home.pos.x), yaw, -1);
    npcSetPlayerPriest(gabi::ea(&npcSave()->mSavedata.mPlayer.mPriest), static_cast<u32>(static_cast<u8>(option)), gabi::ea(&home.pos.x), yaw, -1);
    const u32 state = dComIfGs_ea();
    const s8 room = static_cast<s8>(npcWord<s8>(npcSaveView(state)->mRestart.mOptionRoomNo)); const s16 savedYaw = static_cast<s16>(npcSaveView(state)->mRestart.mOptionRoomAngleY);
    npcSetPlayerPriest(gabi::ea(&npcSaveView(state)->mSavedata.mPlayer.mPriest), static_cast<u32>(static_cast<u8>(option)), gabi::ea(&npcSaveView(state)->mRestart.mOptionRoomPos), savedYaw, room);
    npcSetStageLayer(a);
}

/* 02445784 */
s32 daPy_npc_c::checkRestart(s8 option) {
    const u32 a = gabi::ea(this), initial = dComIfGs_ea();
    if (static_cast<s8>(npcWord<s8>(npcSaveView(initial)->mRestart.mOption)) != option) return 0;
    const s16 point = static_cast<s16>(npcSaveView(initial)->mRestart.mOptionPoint);
    if (point >= 0) { npcPointRestartSlice(a, point, static_cast<s8>(1)); return 1; }
    const u32 x = static_cast<u32>(npcWord<u32>(npcSaveView(initial)->mRestart.mOptionRoomPos.x));
        npcWord<u32>(home.pos.x) = x;
    const u32 y = static_cast<u32>(npcWord<u32>(npcSaveView(initial)->mRestart.mOptionRoomPos.y));
        npcWord<u32>(home.pos.y) = y;
    const u32 z = static_cast<u32>(npcWord<u32>(npcSaveView(initial)->mRestart.mOptionRoomPos.z));
        npcWord<u32>(home.pos.z) = z;
    u32 state = dComIfGs_ea(); const s16 yaw = static_cast<s16>(npcSaveView(state)->mRestart.mOptionRoomAngleY); const u16 pitch = static_cast<u16>(npcWord<u16>(home.angle.x));
    home.angle.y = yaw;
    state = dComIfGs_ea(); const u32 angleXY = static_cast<u32>(npcWord<u32>(home.angle.x)); const u8 room = static_cast<u8>(npcWord<u8>(npcSaveView(state)->mRestart.mOptionRoomNo));
    shape_angle.y = yaw;
        npcWord<u8>(home.roomNo) = room;
    const u32 roomWord = static_cast<u32>(npcWord<u32>(home.angle.z)); const u16 roll = static_cast<u16>(npcWord<u16>(home.angle.z));
    npcWord<u32>(current.pos.x) = x;
        npcWord<u16>(shape_angle.z) = roll;
        npcWord<u32>(old.pos.x) = x;
    npcWord<u32>(old.angle.x) = angleXY;
        npcWord<u16>(shape_angle.x) = pitch;
        npcWord<u32>(current.pos.y) = y;
    npcWord<u32>(old.pos.z) = z;
        npcWord<u32>(current.angle.z) = roomWord;
        npcWord<u32>(old.pos.y) = y;
    npcWord<u32>(old.angle.z) = roomWord;
        npcWord<u32>(current.pos.z) = z;
        npcWord<u32>(current.angle.x) = angleXY;
    return 1;
}

/* 0244586C */
s32 daPy_npc_c::initialRestartOption(s8 option, s32 shouldSave) {
    const u32 a = gabi::ea(this);
    if (static_cast<u32>(npcWord<u32>(npcPlay()->play.mpPlayerPtr[1]))) return 0;
    npcWord<u32>(npcPlay()->play.mpPlayerPtr[1]) = a;
    if (shouldSave) {
        const u32 initial = dComIfGs_ea();
        if (static_cast<s8>(npcWord<s8>(npcSaveView(initial)->mRestart.mOption)) != option) {
            const s8 room = current.roomNo; const s16 yaw = home.angle.y;
            npcSaveRestart(gabi::ea(&npcSaveView(initial)->mRestart), option, gabi::ea(&home.pos.x), yaw, room);
            npcSetPlayerPriest(gabi::ea(&npcSave()->mSavedata.mPlayer.mPriest), static_cast<u32>(static_cast<u8>(option)), gabi::ea(&home.pos.x), yaw, room);
        }
    }
    // HD: additional nonnull callback after registering the partner.
    const u32 manager = static_cast<u32>(l_partnerManager);
    if (manager) { const u32 object = static_cast<u32>(npcPartnerManager(manager)->mpPartner); if (object) npcNotifyPartner(object); }
    return 1;
}

/* 02445950 */
s32 daPy_npc_c::checkNowPosMove(const char* name) {
    const u32 n = gabi::ea(name);
    if (!static_cast<u8>(npcWord<u8>(npcPlay()->play.mEvtCtrl.mMode))) return 1;
    if (static_cast<u16>(npcWord<u16>(eventInfo.mCommand)) == dEvtCmd_INTALK_e) return 1;
    const u32 flags = m4E4;
    if ((flags & 2) && !(flags & 1)) return 1;
    const u32 game = dComIfGp_ea();
    if (npcMyStaffId(gabi::ea(&npcGame(game)->play.mEvtManager), n, 0u, 0u) != -1) return 1;
    return (actor_status & 0x800) ? 1 : 0;
}

/* 02445A00 */
void daPy_npc_c::drawDamageFog() {
    if (!mDamageFogTimer) return;
    gabi::Local<PlayerNpcVector> camera;
    const u32 v = gabi::ea(camera.get());
    npcPosToCamera(gabi::ea(&current.pos.x), v);
    const u32 timer = static_cast<u32>(l_timer);
    const f32 cameraZ = static_cast<f32>(npcWord<f32>(npcVector(v)->z)), span = l_fogSpan, endOffset = l_searchRadius;
    const u32 sineIndex = ((timer << 11) & 0xFFFFu) >> 3;
    const f32 base = static_cast<f32>(-static_cast<f64>(cameraZ) - static_cast<f64>(span));
    const f32 adjustment = static_cast<f32>(std::fabs(static_cast<f64>(static_cast<f32>(npcSine(sineIndex)->sin))));
    // HD: color channels are three halfwords, not GC's three bytes.
    npcWord<u16>(tevStr.mFogColor.b) = 60;
        npcWord<u16>(tevStr.mFogColor.g) = 60;
    const f32 start = gabi::fmadds(span, adjustment, base);
    npcWord<u16>(tevStr.mFogColor.r) = 255;
    const f32 end = static_cast<f32>(start + endOffset);
    tevStr.mFogStartZ = start;
    tevStr.mFogEndZ = end;
}

/* 02445AA4 */
s32 daPy_npc_c::chkMoveBlock(PlayerNpcVector* output) {
    const u32 a = gabi::ea(this), out = gabi::ea(output);
    gabi::Local<PlayerNpcVector> relative, velocity, normalized, cross;
    const u32 r = gabi::ea(relative.get()), v = gabi::ea(velocity.get());
    const u32 n = gabi::ea(normalized.get()), c = gabi::ea(cross.get());
    const f32 radius = l_searchRadius;
    const u32 block = npcSearchAreaSlice(a, static_cast<s16>(fpcNm_Obj_Movebox_e), radius, r);
    if (!block) return 0;
    npcVectorSubtract(gabi::ea(&npcActor(block)->current.pos), v, gabi::ea(&npcActor(block)->old.pos));
    const f64 square = npcVectorSquareMag(v);
    const f64 magnitude = npcSqrt(square);
    if (!(magnitude > l_minBlockSpeed)) return 0; // HD: native ble: !GT, including unordered.
    if (out) {
        const u32 y = static_cast<u32>(npcVector(v)->y), x = static_cast<u32>(npcVector(v)->x), z = static_cast<u32>(npcVector(v)->z);
        npcVector(out)->x = x; npcVector(out)->z = z; npcVector(out)->y = y;
    }
    // HD: both normalization calls write the same scratch vector.
    npcNormalize(r, n);
    npcNormalize(v, n);
    const f64 dot = npcDot(r, v);
    const f32 zero = l_zero;
    npcWord<f32>(npcVector(v)->y) = zero;
        npcWord<f32>(npcVector(r)->y) = zero;
    npcCross(r, c, v);
    if (!(dot < l_blockDotLimit)) {
        const f32 selfY = static_cast<f32>(npcWord<f32>(current.pos.y)), offset = l_homeOffset;
        const f32 limit = static_cast<f32>(selfY + offset);
        if (!(static_cast<f32>(npcWord<f32>(npcActor(block)->current.pos.y)) > limit)) return 0;
    }
    // HD: native bge is !LT: unordered follows the +1 return path.
    return static_cast<f32>(npcWord<f32>(npcVector(c)->y)) < zero ? -1 : 1;
}

/* 02445CF4 */
s32 daPy_npc_c::getTactMusic() {
    return -1;
}

/* 02445CFC */
s32 daPy_npc_c::getTactTimerCancel() {
    return 0;
}

/* 02445D04 */
s32 daPy_npc_c::checkPlayerGuard() {
    return 0;
}

/* 02445D0C */
s32 daPy_npc_c::getGrabMissActor() {
    return 0;
}

/* 02445D14 */
s32 daPy_npc_c::checkPlayerFly() {
    return 0;
}

/* 02445D1C */
s32 daPy_npc_c::checkFrontRoll() {
    return 0;
}

/* 02445D24 */
s32 daPy_npc_c::checkBottleSwing() {
    return 0;
}

/* 02445D2C */
s32 daPy_npc_c::checkCutCharge() {
    return 0;
}

/* 02445D34 */
s32 daPy_npc_c::getBokoFlamePos() {
    return 0;
}

/* 02445D3C */
s32 daPy_npc_c::checkTactWait() {
    return 0;
}

/* 02445D44 */
void daPy_npc_c::setTactZev() {
    // Native blr leaves the integer return register untouched.
}

/* 02445D48 */
s32 daPy_npc_c::checkComboCutTurn() {
    return 0;
}

/* 02445D50 */
s32 daPy_npc_c::getItemID() {
    return -1;
}

/* 02445D58 */
s32 daPy_npc_c::getThrowBoomerangID() {
    return -1;
}

/* 02445D60 */
s32 daPy_npc_c::getGrabActorID() {
    return -1;
}

/* 02445D68 */
s32 daPy_npc_c::checkGrabBarrel() {
    return 0;
}

/* 02445D70 */
s32 daPy_npc_c::checkPlayerNoDraw() {
    return 0;
}

/* 02445D78 */
s32 daPy_npc_c::checkRopeTag() {
    return 0;
}

/* 02445D80 */
s32 daPy_npc_c::checkRopeReadyAnime() {
    return 0;
}

/* 02445D88 */
void daPy_npc_c::voiceStart() {
    // Native blr leaves the integer return register untouched.
}

/* 02445D8C */
void daPy_npc_c::setOutPower() {
    // Native blr leaves the integer return register untouched.
}

/* 02445D90 */
void daPy_npc_c::onFrollCrashFlg() {
    // Native blr leaves the integer return register untouched.
}

/* 02445D94 */
s32 daPy_npc_c::setHookshotCarryOffset() {
    return 0;
}

/* 02445D9C */
void daPy_npc_c::setPlayerPosAndAnglePosition() {
    // Native blr leaves the integer return register untouched.
}

/* 02445DA0 */
void daPy_npc_c::setPlayerPosAndAngleRotation() {
    // Native blr leaves the integer return register untouched.
}

/* 02445DA4 */
void daPy_npc_c::setPlayerPosAndAngleMatrix() {
    // Native blr leaves the integer return register untouched.
}

/* 02445DA8 */
s32 daPy_npc_c::setThrowDamage() {
    return 0;
}

/* 02445DB0 */
s32 daPy_npc_c::isTagCheckOK() {
    return 1;
}

/* 02445DB8 */
void daPy_npc_c::setMessageAnimation() {
    // Native blr leaves the integer return register untouched.
}

// HD: compiler-generated and template code of this translation unit.
// Per-TU attribution inferred from adjacent NPC constants/vtable and distinct
// header-static descriptors; prior unresolved ownership proposal remains archived.
/* 02445C30 */
void daPy_npc_sinit() {
    npcWord<u32>(l_headerWords[3]) = 0;
        npcWord<u32>(l_headerWords[2]) = 0;
    npcWord<u32>(l_headerWords[1]) = 0;
        npcWord<u32>(l_headerWords[0]) = 0;
    npcRegisterGlobal(gabi::ea(&l_headerDtor[0]));
    const f32 minusPi = l_minusPi, pi = l_pi;
    l_angleMin = minusPi; l_angleMax = pi;
    npcInitHeaderFlag(gabi::ea(&l_headerFlag));
    npcRegisterGlobal(gabi::ea(&l_flagDtor[0]));
    npcInitHeaderByte(gabi::ea(&l_headerByte));
    npcRegisterGlobal(gabi::ea(&l_byteDtor[0]));
    const f32 large = l_largeRadius, small = l_smallRadius;
    l_radiusMaxA = large; l_radiusMinA = small;
    l_radiusMaxB = large; l_radiusMinB = small;
}

/* WWHD glue
data l_zero 10037E30
data l_noGround 10037E78
data l_homeOffset 10037E7C
data l_fogSpan 10037F28
data l_searchRadius 10037F2C
data l_minBlockSpeed 10037F30
data l_blockDotLimit 10037F34
data l_minusPi 10037F3C
data l_pi 10037F40
data l_largeRadius 10037F44
data l_smallRadius 10037F48
data l_offsetGuard 1046D330 nocheck
data l_offsetPos 1046D324 nocheck
data l_headerWords 1046D314 nocheck
data l_angleMin 1046D2F8 nocheck
data l_angleMax 1046D2FC nocheck
data l_radiusMaxA 1046D300 nocheck
data l_radiusMaxB 1046D304 nocheck
data l_radiusMinA 1046D308 nocheck
data l_radiusMinB 1046D30C nocheck
data l_headerFlag 1046D310 nocheck
data l_headerByte 1046D311 nocheck
data l_headerDtor 101CEF50 nocheck
data l_flagDtor 101CEF5C nocheck
data l_byteDtor 101CEF68 nocheck
data l_partnerManager 101F8344 nocheck
data l_timer 101FF560 nocheck
ext 0273AA24 void npcAssertion(const char*, u32, const char*)
ext 0201ADE0 void npcVectorSubtract(u32, u32, u32)
ext 028E8DD0 f64 npcVectorSquareMag(u32)
ext 028F4384 f64 npcSqrt(f64)
ext 025D5218 void npcActorJudge(u32, u32)
ext 0273AD10 u32 npcAllocate(u32)
ext 023D47E8 void npcPlayerConstruct(u32)
ext 024F0474 void npcAcchConstruct(u32)
ext 024EFD9C void npcAcchDestroy(u32, u32)
ext 023D483C void npcPlayerDestroy(u32, u32)
ext 0273AF40 void npcDelete(u32)
ext 024F08A8 void npcAcchCrrPos(u32, u32)
ext 024EF0BC s32 npcGroundCode(u32, u32)
ext 024EF130 s32 npcGroundRoom(u32, u32)
ext 025B9864 void npcSetRestartOption(u32, s8)
ext 025B8880 void npcSetPlayerPriest(u32, u32, u32, s16, s32)
ext 02445398 void npcUnconditionalRestartSlice(u32, s8)
ext 0200F9D4 void npcOffsetPos(u32, u32, s16, u32)
ext 024454C8 void npcOffsetHomeSlice(u32)
ext 025B9834 void npcSaveRestart(u32, s8, u32, s16, s32)
ext 025D537C void npcSetStageLayer(u32)
ext 02445518 void npcPointRestartSlice(u32, s16, s8)
ext 02688BBC void npcNotifyPartner(u32)
ext 02542D88 s32 npcMyStaffId(u32, u32, u32, u32)
ext 025F1108 void npcPosToCamera(u32, u32)
ext 02445018 u32 npcSearchAreaSlice(u32, s16, f32, u32)
ext 0201B3C0 void npcNormalize(u32, u32)
ext 028E8F44 f64 npcDot(u32, u32)
ext 0201B080 void npcCross(u32, u32, u32)
ext 028F026C void npcRegisterGlobal(u32)
ext 028ED6F8 void npcInitHeaderFlag(u32)
ext 028EAB2C void npcInitHeaderByte(u32)
str 10037EF0 "dComIfGp_getStagePlayer() != NULL"
str 10037E80 "sclsinfo != NULL"
str 10037E90 "0 <= i_point && i_point < sclsinfo->num"
str 10037F14 "scls_data != NULL"
str 10037EB8 "i != dComIfGp_getStagePlayerNum()"
str 10037EDC "d_a_player_npc.cpp"
*/
