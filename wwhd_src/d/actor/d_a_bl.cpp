/* Bubble enemy. Verified HD implementation.
 * HD: the GameCube body is not reconstructed; state names describe the audited HD layout. */
#include "d/actor/d_a_bl.h"

const gc_tu_data gc_tu = {
    .safeString = 0x100098AC,
    .objAcch = {0x100098F4, 0x10009914, 0x10009904},
    .objAcchDt = {0x10009904, 0x10009914},
    .headerStatics = {0x10462478, 0x10191958, 0x10462484},
};

namespace bl_data {
constexpr u32 matrixWork = 0x1048D0CC;
constexpr u32 matrixStack = 0x1018C7B0;
constexpr u32 saveRoot = 0x101F84DC;
constexpr u32 roomNo = 0x1047E6C8;
constexpr u32 resourceControl = 0x101F4F28;
constexpr u32 attackSpeedTuning = 0x1047BAB8;
constexpr u32 retreatSpeedTuning = 0x1047BABC;
constexpr u32 smokeHeightTuning = 0x1047BA98;
constexpr u32 attackDistanceTuning = 0x1047BA9C;
constexpr u32 vanishRadiusTuning = 0x1047BAAC;
constexpr u32 debugFlying = 0x1047BB18;
constexpr u32 debugVariant = 0x1047BB1A;
constexpr u32 staticVectorZ = 0x1046248C;
constexpr u32 staticVectorX = 0x10462484;
constexpr u32 staticVectorExtra = 0x10462490;
constexpr u32 staticVectorY = 0x10462488;
constexpr u32 negativePi = 0x10462478;
constexpr u32 positivePi = 0x1046247C;
constexpr u32 safeStringVtable = 0x100098AC;
constexpr u32 lineBaseVtable = 0x10009924;
constexpr u32 lineCheckVtable = 0x10009934;
constexpr u32 groupPassVtable = 0x10009954;
constexpr u32 objectPassVtable = 0x10009944;
constexpr u32 objectPassBaseVtable = 0x100098E4;
constexpr u32 lineBaseDestructorVtable = 0x100098D4;
constexpr u32 actorVtable = 0x10009964;
constexpr u32 groundBaseVtable = 0x100098F4;
constexpr u32 groundObjectVtable = 0x10009914;
constexpr u32 groundActorVtable = 0x10009904;
constexpr u32 collisionStatusVtable = 0x1004AE88;
constexpr u32 globalCollisionStatusVtable = 0x1004AEC0;
constexpr u32 cylinderBaseVtable = 0x100015A8;
constexpr u32 cylinderShapeVtable = 0x100098C4;
constexpr u32 collisionObjectVtable = 0x1004B108;
constexpr u32 collisionCylinderVtable = 0x1004B150;
constexpr u32 cylinderVtable = 0x1004B160;
constexpr u32 angleDestructorRecord = 0x10191958;
constexpr u32 fixedVectorDestructorRecord = 0x10191964;
constexpr u32 vectorDestructorRecord = 0x10191970;
constexpr u32 fixedVectorStatic = 0x10462480;
constexpr u32 vectorStatic = 0x10462481;
} // namespace bl_data

u8* bl_resourceControl() {
    return reinterpret_cast<u8*>(gabi::object_at<gptr<dRes_control_c>>(bl_data::resourceControl).get());
}

// HD: original collision and fire-helper initialization data.
static u32 body_co_sph_src[16] = {0x00000000, 0x00000200, 0x01000000, 0x00000005, 0xFF3DFEFF, 0x00000003,
                                  0x00000075, 0x04000000, 0x00000000, 0x00000000, 0x00000006, 0x00000000,
                                  0x00000000, 0x00000000, 0x00000000, 0x41700000};
static f32 fireFloatParameters[10] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static u8 fireByteParameters[10] = {0, 1, 255, 255, 255, 255, 255, 255, 255, 255};

Mtx34* matrixStack() { return gabi::object_at<gptr<Mtx34>>(bl_data::matrixStack).get(); }

/* 020A8A9C */
void draw_SUB(bl_class* actor) {
    BubbleModel* m = bl_getModel(actor);
    f32 x = actor->scale.x, z = actor->scale.z, y = actor->scale.y;
    m->scaleX = x;
    m->scaleZ = z;
    m->scaleY = y;
    y = (f32)actor->current.pos.y + (f32)actor->modelYOffset;
    x = actor->current.pos.x;
    z = actor->current.pos.z;
    bl_MtxTrans(0, x, y, z);
    bl_mDoMtx_YrotM(reinterpret_cast<u8*>(matrixStack()), (s16)actor->shape_angle.y);
    bl_mDoMtx_XrotM(reinterpret_cast<u8*>(matrixStack()), (s16)actor->shape_angle.x);
    bl_mDoMtx_ZrotM(reinterpret_cast<u8*>(matrixStack()), (s16)actor->shape_angle.z);
    y = -(f32)actor->modelYOffset;
    bl_MtxTrans(1, 0.0f, y, 0.0f);
    mtx_copy(&m->baseMatrix, matrixStack());
    bl_mDoExt_McaMorf_calc((u8*)actor->morph);
    bl_enemy_fire(reinterpret_cast<u8*>(&actor->fireHelper));
    dScnKy_env_light_c* environment = bl_getEnvLight();
    bl_dScnKy_env_light_c_settingTevStruct(reinterpret_cast<u8*>(environment), 0, &actor->current.pos,
                                           reinterpret_cast<u8*>(&actor->tevStr));
}

enum class NameMatch { Different, Equal, Limit };
NameMatch stageMatch(SafeString* name, SafeString* stage) {
    u32 table = name->__vtbl;
    gabi::call((u32)gabi::object_at<BubbleStringVtable>(table).assureTermination, name);
    table = name->__vtbl;
    gabi::call((u32)gabi::object_at<BubbleStringVtable>(table).assureTermination, name);
    table = stage->__vtbl;
    u32 left = name->mStringTop;
    gabi::call((u32)gabi::object_at<BubbleStringVtable>(table).assureTermination, stage);
    u32 right = stage->mStringTop;
    if (left == right)
        return NameMatch::Equal;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = (u8)gabi::object_at<be<u8>>(left + i), b = (u8)gabi::object_at<be<u8>>(right + i);
        if (a != b)
            return NameMatch::Different;
        if (a == 0)
            return NameMatch::Equal;
    }
    return NameMatch::Limit;
}
/* 020A8BDC */
BOOL daBL_Draw(bl_class* actor) {
    BubbleModel* m = bl_getModel(actor);
    dScnKy_env_light_c* environment = bl_getEnvLight();
    bl_setLightTevColorType(reinterpret_cast<u8*>(environment), reinterpret_cast<u8*>(m),
                            reinterpret_cast<u8*>(&actor->tevStr));
    u8 switchNo = actor->switchNo;
    if (switchNo != 0xFF) {
        dSv_info_c* save = bl_saveInfo((u32)gabi::object_at<be<u32>>(bl_data::saveRoot));
        s8 room = ((s8)gabi::object_at<be<s8>>(bl_data::roomNo));
        if (!bl_dSv_info_c_isSwitch(reinterpret_cast<u8*>(save), switchNo, room))
            return TRUE;
    }
    gabi::Local<SafeString> first, stage;
    first->mStringTop = gabi::guest_addr("M_DaiB");
    first->__vtbl = gc_tu.safeString;
    u32 play = bl_getGameInfo();
    stage->__vtbl = gc_tu.safeString;
    stage->mStringTop = play + 0x5134;
    NameMatch match = stageMatch(first.get(), stage.get());
    bool skipFigure = match == NameMatch::Equal && actor->home.roomNo == 0;
    if (!skipFigure) {
        gabi::Local<SafeString> second, otherStage;
        second->__vtbl = gc_tu.safeString;
        second->mStringTop = gabi::guest_addr("Xboss2");
        play = bl_getGameInfo();
        otherStage->mStringTop = play + 0x5134;
        otherStage->__vtbl = gc_tu.safeString;
        NameMatch secondMatch = stageMatch(second.get(), otherStage.get());
        skipFigure = secondMatch == NameMatch::Equal && actor->home.roomNo == 0;
    }
    if (!skipFigure)
        bl_dSnap_RegistFig(0xB4, actor, 1.0f, 1.0f, 1.0f);
    if (actor->iceState.thawTimer > 20) {
        bl_dMat_ice_c_entryDL((u8*)actor->morph, -1, reinterpret_cast<u8*>(&actor->lightInfo));
        return TRUE;
    }
    u8 state = actor->burning;
    u32 data = m->modelData;
    u32 animation = state ? gabi::ea((u8*)actor->textureAnimation) : gabi::ea((u8*)actor->colorAnimation);
    f32 frame = gabi::object_at<BubbleAnimation>(animation).frame;
    bl_mDoExt_btkAnm_entry(gabi::at<u8>(animation), gabi::at<u8>(data), frame);
    bl_touchGameInfo();
    bl_mDoExt_McaMorf_entryDL((u8*)actor->morph);
    data = m->modelData;
    gabi::object_at<BubbleModelData>(data).materialAnimation = 0;
    return TRUE;
}

/* 020A8EE0 */
void smoke_set(bl_class* actor) {
    u32 emitter = actor->smokeCallback.emitter;
    if (!emitter) {
        s8 room = actor->current.roomNo;
        u32 play = bl_getGameInfo();
        dPa_control_c* particles = gabi::object_at<dComIfG_inf_c>(play).play.mParticle.get();
        bl_dPa_control_c_set(reinterpret_cast<u8*>(particles), 2, 0x2027,
                             reinterpret_cast<cXyz*>(&actor->smokePosition), &actor->shape_angle, 0, 0xB9,
                             reinterpret_cast<u8*>(&actor->smokeCallback), room, 0, 0, 0);
        emitter = actor->smokeCallback.emitter;
        if (!emitter)
            return;
    }
    gabi::object_at<BubbleEmitter>(emitter).scaleZ = 1.6f;
    gabi::object_at<BubbleEmitter>(emitter).scaleY = 1.6f;
    gabi::object_at<BubbleEmitter>(emitter).scaleX = 1.6f;
    emitter = actor->smokeCallback.emitter;
    gabi::object_at<BubbleEmitter>(emitter).emissionRate = 10.0f;
    emitter = actor->smokeCallback.emitter;
    gabi::object_at<BubbleEmitter>(emitter).maximumParticles = 1;
    emitter = actor->smokeCallback.emitter;
    gabi::object_at<BubbleEmitter>(emitter).initialVelocity = 8.0f;
    emitter = actor->smokeCallback.emitter;
    gabi::object_at<BubbleEmitter>(emitter).flags = (u32)gabi::object_at<BubbleEmitter>(emitter).flags | 0x40;
    actor->smokeCallback.color = 0xA0A080B4;
}

/* 020A8FE8 */
void fire_move_set(bl_class* actor) {
    u32 effect = actor->variant ? 0x8123 : 0x8124;
    if (gabi::ea(actor->fireCallback.mpEmitter.get()))
        return;
    s8 room = actor->current.roomNo;
    u32 play = bl_getGameInfo();
    dPa_control_c* particles = gabi::object_at<dComIfG_inf_c>(play).play.mParticle.get();
    bl_dPa_control_c_set_2(reinterpret_cast<u8*>(particles), 0, effect, &actor->current.pos, 0, 0, 0xFF,
                           reinterpret_cast<u8*>(&actor->fireCallback), room, 0, 0, 0);
    if (actor->variant == 1) {
        u32 flags = actor->bodyCollider.targetLinkFlags;
        actor->bodyCollider.targetState = 5;
        actor->bodyCollider.targetMaterial = 12;
        actor->bodyCollider.targetLinkFlags = flags | 1;
    }
}

/* 020A90AC */
void fire_emitter_clr(bl_class* actor) {
    if (gabi::ea(actor->fireCallback.mpEmitter.get()) && actor->timers[2] == 0) {
        if (actor->variant == 0) {
            u32 play = bl_getGameInfo();
            u32 player = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get());
            s16 angle = (s16)(gabi::object_at<fopAc_ac_c>(player).shape_angle.y + 0x8000);
            u32 emitter = gabi::ea(actor->fireCallback.mpEmitter.get());
            bl_JPAGetXYZRotateMtx(0, angle, 0,
                                  reinterpret_cast<u8*>(&gabi::object_at<BubbleEmitter>(emitter).rotation));
            emitter = gabi::ea(actor->fireCallback.mpEmitter.get());
            gabi::object_at<BubbleEmitter>(emitter).emissionRate = 3.0f;
            emitter = gabi::ea(actor->fireCallback.mpEmitter.get());
            gabi::object_at<BubbleEmitter>(emitter).lifetime = 10;
            emitter = gabi::ea(actor->fireCallback.mpEmitter.get());
            gabi::object_at<BubbleEmitter>(emitter).spread = 50.0f;
            actor->timers[2] = 21;
        } else
            actor->timers[2] = 1;
    }
    bl_dPa_followEcallBack_end(reinterpret_cast<u8*>(&actor->appearanceCallback));
}

/* 020A916C */
void fire_kaiten_keisan(bl_class* actor) {
    u32 emitter = gabi::ea(actor->fireCallback.mpEmitter.get());
    if (!emitter)
        return;
    if (actor->variant == 0) {
        BubbleModel* m = bl_getModel(actor);
        u32 joint = m->jointController;
        u16 flags = gabi::object_at<BubbleJointController>(joint).flags;
        Mtx34* matrices = gabi::object_at<BubbleJointController>(joint).matrices;
        gabi::object_at<BubbleJointController>(joint).flags = flags | 0x10;
        bl_JPASetRMtxTVecfromMtx(reinterpret_cast<u8*>(matrices),
                                 reinterpret_cast<u8*>(&gabi::object_at<BubbleEmitter>(emitter).rotation),
                                 reinterpret_cast<u8*>(&gabi::object_at<BubbleEmitter>(emitter).translation));
        if (actor && gabi::ea(&actor->eyePos)) {
            s32 reverb = bl_dComIfGp_getReverb((s8)actor->current.roomNo);
            bl_seStartFull_1(0x7023, reinterpret_cast<cXyz*>(&actor->eyePos), 0, reverb);
        }
        return;
    }
    if (actor && gabi::ea(&actor->eyePos)) {
        s32 reverb = bl_dComIfGp_getReverb((s8)actor->current.roomNo);
        bl_seStartFull_1(0x7024, reinterpret_cast<cXyz*>(&actor->eyePos), 0, reverb);
    }
    u32 play = bl_getGameInfo();
    s8 camera = gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mCameraID;
    play = bl_getGameInfo();
    BubbleModel* m = bl_getModel(actor);
    u32 joint = m->jointController;
    u16 flags = gabi::object_at<BubbleJointController>(joint).flags;
    u32 cameraObject = (u32)gabi::object_at<dComIfG_inf_c>(play).play.mCameraInfo[camera].mpCamera;
    Mtx34* matrices = gabi::object_at<BubbleJointController>(joint).matrices;
    gabi::object_at<BubbleJointController>(joint).flags = flags | 0x10;
    bl_PSMTXCopy(reinterpret_cast<u8*>(matrices), reinterpret_cast<u8*>(matrixStack()));
    s16 yaw = actor->shape_angle.y;
    s16 relative = (s16)(gabi::object_at<BubbleCamera>(cameraObject).yaw - yaw);
    bl_mDoMtx_YrotM(reinterpret_cast<u8*>(matrixStack()), relative);
    emitter = gabi::ea(actor->fireCallback.mpEmitter.get());
    bl_JPASetRMtxTVecfromMtx(reinterpret_cast<u8*>(matrixStack()),
                             reinterpret_cast<u8*>(&gabi::object_at<BubbleEmitter>(emitter).rotation),
                             reinterpret_cast<u8*>(&gabi::object_at<BubbleEmitter>(emitter).translation));
}

/* 020A92D0 */
BOOL shock_damage_check(bl_class* actor) {
    u32 play = bl_getGameInfo();
    u8 type = actor->variant;
    u32 player = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get());
    if (!(type & 0x80) && actor->burning)
        return FALSE;
    if (!(gabi::object_at<daPy_py_c>(player).mResetFlg0 & 0x20000))
        return FALSE;
    f32 dz = gabi::object_at<daPy_py_c>(player).mSwordTopPos.z - (f32)actor->current.pos.z;
    f32 dx = gabi::object_at<daPy_py_c>(player).mSwordTopPos.x - (f32)actor->current.pos.x;
    f32 distance = bl_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    if (!(distance < 1000.0f))
        return FALSE;
    actor->timers[2] = 0;
    fire_emitter_clr(actor);
    return TRUE;
}

/* 020A9384 */
void anm_init(bl_class* actor, s32 animation, f32 morph, u8 mode, f32 rate, s32 sound) {
    actor->animationId = animation;
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::guest_addr("Bl");
    name->__vtbl = gc_tu.safeString;
    u8* resources = bl_resourceControl();
    auto* resource = bl_dRes_control_c_getRes(resources, name.get(), animation);
    u8* soundResource = nullptr;
    if (sound >= 0) {
        gabi::Local<SafeString> soundName;
        soundName->mStringTop = gabi::guest_addr("Bl");
        soundName->__vtbl = gc_tu.safeString;
        resources = bl_resourceControl();
        soundResource = bl_dRes_control_c_getRes(resources, soundName.get(), sound);
    }
    bl_mDoExt_McaMorf_setAnm((u8*)actor->morph, resource, mode, soundResource, morph, rate, 0.0f, -1.0f);
}

/* 020A94B0 */
BOOL skull_atari_check(bl_class* actor) {
    u32 play = bl_getGameInfo();
    u32 player = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get());
    bl_dCcD_GStts_Move(reinterpret_cast<u8*>(&actor->bodyCollisionStatus.globalVtable));
    if ((s8)actor->health == 0)
        return TRUE;
    if (shock_damage_check(actor)) {
        actor->action = BL_ACTION_KAZE_MOVE;
        actor->actionStep = 34;
        return TRUE;
    }
    if (!bl_dCcD_GObjInf_ChkTgHit(reinterpret_cast<u8*>(&actor->bodyCollider)))
        return FALSE;
    u32 hit = bl_dCcD_GObjInf_GetTgHitObj(reinterpret_cast<u8*>(&actor->bodyCollider));
    if (!hit || actor->timers[4])
        return FALSE;
    play = bl_getGameInfo();
    fopAc_ac_c* opponent = gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get();
    s32 angle = bl_fopAcM_searchActorAngleY(actor, reinterpret_cast<u8*>(opponent));
    actor->damageKind = 0;
    actor->timers[4] = 8;
    actor->current.angle.y = (s16)((u32)angle + 0x8000u);
    u32 attack = gabi::object_at<BubbleAttackHit>(hit).attackType;
    switch (attack) {
    case 2:
        actor->damageKind = 6;
        break;
    case 0x200000:
        actor->damageKind = 1;
        actor->action = BL_ACTION_KAZE_MOVE;
        actor->actionStep = 36;
        return TRUE;
    case 0x10000:
        actor->damageKind = 4;
        if (bl_playerCutType(gabi::at<daPy_py_c>(player)) == 17) {
            s16 yaw = gabi::object_at<fopAc_ac_c>(player).shape_angle.y;
            actor->speed.y = 30.0f;
            actor->current.angle.y = (s16)(yaw - 0x4000);
            actor->damageKind = 5;
            actor->speedF = 20.0f;
        }
        break;
    case 0x40:
    case 0x80:
        actor->damageKind = 3;
        break;
    case 0x20:
        actor->speed.y = 30.0f;
        [[fallthrough]];
    case 0x200:
        actor->fireHelper.bounceCount = 1000;
        break;
    case 0x8000:
        actor->damageKind = 7;
        actor->action = BL_ACTION_HOOK;
        actor->actionStep = 60;
        return TRUE;
    default:
        break;
    }
    if (gabi::ea(&actor->eyePos)) {
        s32 reverb = bl_dComIfGp_getReverb((s8)actor->current.roomNo);
        bl_seStartFull_1(0x692B, reinterpret_cast<cXyz*>(&actor->eyePos), 0, reverb);
    }
    actor->health = 0;
    actor->attention_info.flags = 0;
    actor->action = BL_ACTION_SKULL;
    actor->actionStep = 103;
    return TRUE;
}

void hitSound(bl_class* actor, u32 sound, u32 volume) {
    if (gabi::ea(&actor->eyePos)) {
        s32 reverb = bl_dComIfGp_getReverb((s8)actor->current.roomNo);
        bl_seStartFull_2(sound, reinterpret_cast<cXyz*>(&actor->eyePos), volume, reverb);
    }
}
void stopFireSound(bl_class* actor) {
    if (gabi::ea(&actor->eyePos)) {
        hitSound(actor, 0x588B, 0);
        s8 room = actor->current.roomNo;
        u32 id = actor ? actor->mBsPcId : 0xFFFFFFFF;
        s32 reverb = bl_dComIfGp_getReverb(room);
        bl_seStartLevelFull_1(0x4873, reinterpret_cast<cXyz*>(&actor->eyePos), id, 0, reverb);
    }
}
void collisionParticle(bl_class* actor, u32 effect, cXyz* position, void* rotation = nullptr,
                       cXyz* scale = nullptr) {
    u32 play = bl_getGameInfo();
    dPa_control_c* particles = gabi::object_at<dComIfG_inf_c>(play).play.mParticle.get();
    bl_dPa_control_c_set_3(reinterpret_cast<u8*>(particles), 0, effect, position, rotation, scale, 0xFF, 0,
                           -1, 0, 0, 0);
}
void checkAttack(bl_class* actor, AttackInfo* info) {
    u32 hit = bl_dCcD_GObjInf_GetTgHitObj(reinterpret_cast<u8*>(&actor->bodyCollider));
    info->hitObject = hit;
    info->attackPower = 0;
    bl_cc_at_check(actor, info);
}
/* 020A9744 */
BOOL blue_body_atari_check(bl_class* actor) {
    u32 play = bl_getGameInfo();
    u32 player = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get());
    bl_dCcD_GStts_Move(reinterpret_cast<u8*>(&actor->bodyCollisionStatus.globalVtable));
    if (shock_damage_check(actor)) {
        actor->action = BL_ACTION_KAZE_MOVE;
        actor->actionStep = 34;
        return TRUE;
    }
    if (!bl_dCcD_GObjInf_ChkTgHit(reinterpret_cast<u8*>(&actor->bodyCollider)))
        return FALSE;
    u32 hit = bl_dCcD_GObjInf_GetTgHitObj(reinterpret_cast<u8*>(&actor->bodyCollider));
    if (!hit || actor->timers[4])
        return FALSE;
    play = bl_getGameInfo();
    fopAc_ac_c* opponent = gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get();
    s32 angle = bl_fopAcM_searchActorAngleY(actor, reinterpret_cast<u8*>(opponent));
    actor->damageKind = 0;
    actor->current.angle.y = (s16)((u32)angle + 0x8000u);
    actor->timers[4] = 8;
    u32 attack = gabi::object_at<BubbleAttackHit>(hit).attackType;
    bool suppressDamage = false;
    gabi::Local<AttackInfo> info;
    switch (attack) {
    case 0x08000000:
        if (actor->burning) {
            s8 count = actor->stolenItemCount;
            actor->stealItemLeft = count;
            if (count > 0) {
                s8 health = actor->health;
                actor->health = 10;
                checkAttack(actor, info.get());
                actor->health = health;
            }
            hitSound(actor, 0x2834, 0x42);
            count = actor->stolenItemCount;
            if (count > 0)
                actor->stolenItemCount = count - 1;
            collisionParticle(actor, 0x27B, reinterpret_cast<cXyz*>(&actor->attention_info.position));
            suppressDamage = true;
        } else {
            actor->stealItemLeft = 0;
            hitSound(actor, 0x2834, 0x33);
        }
        break;
    case 2: {
        u32 volume = actor->burning ? 0x42 : 0x33;
        hitSound(actor, 0x2803, volume);
        u8 action = bl_playerCutType(gabi::at<daPy_py_c>(player));
        if ((action >= 5 && action <= 10) || action == 12 || (action >= 14 && action <= 16) || action == 21 ||
            action == 23 || (action >= 25 && action <= 27) || (action >= 30 && action <= 31))
            actor->damageKind = 6;
        break;
    }
    case 0x20:
        if (!actor->burning) {
            actor->damageKind = 2;
            break;
        }
        [[fallthrough]];
    case 0x200000: {
        u32 ground = actor->groundCollision.flags;
        actor->action = BL_ACTION_KAZE_MOVE;
        actor->actionStep = 36;
        suppressDamage = true;
        actor->damageKind = 1;
        if (!(ground & 0x20) && (f32)actor->gravity > -2.0f)
            actor->actionStep = 30;
        break;
    }
    case 0x10000: {
        u32 volume = actor->burning ? 0x42 : 0x33;
        hitSound(actor, 0x2855, volume);
        actor->damageKind = 4;
        u8 action = bl_playerCutType(gabi::at<daPy_py_c>(player));
        u8 burning = actor->burning;
        if (action == 17) {
            s16 yaw = gabi::object_at<fopAc_ac_c>(player).shape_angle.y;
            actor->damageKind = 5;
            actor->current.angle.y = (s16)(yaw - 0x4000);
        }
        if (!burning)
            actor->health = 0;
        break;
    }
    case 0x40:
    case 0x80: {
        suppressDamage = true;
        u32 volume = actor->burning ? 0x42 : 0x33;
        hitSound(actor, 0x2833, volume);
        actor->damageKind = 3;
        break;
    }
    case 0x200:
    case 0x40000:
        if (!actor->burning) {
            actor->fireHelper.bounceCount = 100;
            stopFireSound(actor);
            u32 flags = actor->bodyCollider.attackFlags;
            actor->timers[2] = 0;
            actor->attention_info.flags = 0;
            actor->bodyCollider.attackFlags = flags & ~1u;
            fire_emitter_clr(actor);
        }
        break;
    case 0x100000: {
        actor->fireCylinder.scale = 1.0f;
        actor->iceState.speed = 40.0f;
        actor->iceState.state = 1;
        stopFireSound(actor);
        u32 flags = actor->bodyCollider.attackFlags;
        actor->gravity = 0.0f;
        actor->burning = 0;
        actor->shape_angle.z = 0;
        actor->speed.y = 0.0f;
        actor->shape_angle.x = 0;
        actor->current.angle.x = 0;
        actor->timers[0] = 0;
        actor->attention_info.flags = 0;
        actor->speedF = 0.0f;
        actor->timers[2] = 0;
        actor->speed.z = 0.0f;
        actor->speed.x = 0.0f;
        actor->current.angle.z = 0;
        actor->bodyCollider.attackFlags = flags & ~1u;
        fire_emitter_clr(actor);
        break;
    }
    case 0x80000:
        suppressDamage = true;
        if (!actor->burning) {
            u32 flags = actor->bodyCollider.attackFlags;
            actor->current.angle.z = 0;
            actor->iceState.freezeTimer = 200;
            actor->bodyCollider.attackFlags = flags & ~1u;
            actor->burning = 0;
            actor->timers[0] = 0;
            actor->gravity = 0.0f;
            actor->shape_angle.z = 0;
            actor->speedF = 0.0f;
            actor->speed.x = 0.0f;
            actor->speed.z = 0.0f;
            actor->current.angle.x = 0;
            actor->shape_angle.x = 0;
            actor->speed.y = 0.0f;
            bl_enemy_fire_remove(reinterpret_cast<u8*>(&actor->fireHelper));
            actor->attention_info.flags = 0;
        } else {
            actor->action = BL_ACTION_KAZE_MOVE;
            actor->damageKind = 1;
            actor->actionStep = 30;
        }
        break;
    case 0x8000: {
        suppressDamage = true;
        u32 volume = actor->burning ? 0x42 : 0x33;
        hitSound(actor, 0x2834, volume);
        actor->damageKind = 7;
        actor->actionStep = 60;
        actor->action = BL_ACTION_HOOK;
        break;
    }
    default: {
        u32 volume = actor->burning ? 0x42 : 0x33;
        hitSound(actor, 0x2834, volume);
        break;
    }
    }
    u8 burning = actor->burning;
    gabi::Local<cXyz> impact, scale;
    impact->x = actor->bodyCollider.targetHitPosition.x;
    impact->y = actor->bodyCollider.targetHitPosition.y;
    impact->z = actor->bodyCollider.targetHitPosition.z;
    if (burning) {
        u8 kind = actor->damageKind;
        if (kind != 7 && kind != 1) {
            collisionParticle(actor, 12, impact.get());
            actor->action = BL_ACTION_SAGARIMASU;
            actor->actionStep = 20;
            return TRUE;
        }
    }
    if (!suppressDamage) {
        checkAttack(actor, info.get());
        u8 kind = actor->damageKind;
        if ((u32)(kind - 4) <= 2 || (s8)actor->health <= 0) {
            collisionParticle(actor, 16, impact.get());
            scale->x = 2.0f;
            scale->z = 2.0f;
            scale->y = 2.0f;
            collisionParticle(actor, 15, impact.get(),
                              reinterpret_cast<u8*>(&gabi::object_at<fopAc_ac_c>(player).shape_angle),
                              scale.get());
        } else
            collisionParticle(actor, 13, impact.get(),
                              reinterpret_cast<u8*>(&gabi::object_at<fopAc_ac_c>(player).shape_angle));
        actor->action = BL_ACTION_ITAIYO;
        actor->actionStep = 40;
    }
    return TRUE;
}

/* 020AA068 */
BOOL red_body_atari_check(bl_class* actor) {
    u32 play = bl_getGameInfo();
    u32 player = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get());
    bl_dCcD_GStts_Move(reinterpret_cast<u8*>(&actor->bodyCollisionStatus.globalVtable));
    if (shock_damage_check(actor)) {
        actor->action = BL_ACTION_KAZE_MOVE;
        actor->actionStep = 34;
        return TRUE;
    }
    if (!bl_dCcD_GObjInf_ChkTgHit(reinterpret_cast<u8*>(&actor->bodyCollider)))
        return FALSE;
    u32 hit = bl_dCcD_GObjInf_GetTgHitObj(reinterpret_cast<u8*>(&actor->bodyCollider));
    if (!hit || actor->timers[4])
        return FALSE;
    play = bl_getGameInfo();
    fopAc_ac_c* opponent = gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get();
    s32 angle = bl_fopAcM_searchActorAngleY(actor, reinterpret_cast<u8*>(opponent));
    actor->current.angle.y = (s16)((u32)angle + 0x8000u);
    gabi::Local<cXyz> impact, scale;
    impact->x = actor->bodyCollider.targetHitPosition.x;
    impact->z = actor->bodyCollider.targetHitPosition.z;
    impact->y = actor->bodyCollider.targetHitPosition.y;
    actor->timers[4] = 8;
    actor->damageKind = 0;
    u32 attack = gabi::object_at<BubbleAttackHit>(hit).attackType;
    gabi::Local<AttackInfo> info;
    switch (attack) {
    case 0x08000000:
        if (actor->burning) {
            s8 count = actor->stolenItemCount;
            actor->stealItemLeft = count;
            if (count > 0) {
                s8 health = actor->health;
                actor->health = 10;
                checkAttack(actor, info.get());
                actor->health = health;
            }
            hitSound(actor, 0x2834, 0x42);
            count = actor->stolenItemCount;
            if (count > 0)
                actor->stolenItemCount = count - 1;
            collisionParticle(actor, 0x27B, reinterpret_cast<cXyz*>(&actor->attention_info.position));
            return TRUE;
        }
        actor->stealItemLeft = 0;
        hitSound(actor, 0x2834, 0x33);
        break;
    case 2: {
        actor->damageKind = 8;
        hitSound(actor, 0x2803, 0x33);
        u8 action = bl_playerCutType(gabi::at<daPy_py_c>(player));
        if ((action >= 5 && action <= 10) || action == 12 || (action >= 14 && action <= 16) || action == 21 ||
            action == 23 || (action >= 25 && action <= 27) || (action >= 30 && action <= 31))
            actor->damageKind = 6;
        break;
    }
    case 0x20:
        if (!actor->burning) {
            actor->damageKind = 2;
            break;
        }
        [[fallthrough]];
    case 0x200000: {
        u32 ground = actor->groundCollision.flags;
        actor->damageKind = 1;
        actor->action = BL_ACTION_KAZE_MOVE;
        actor->actionStep = 36;
        if (!(ground & 0x20) && (f32)actor->gravity > -2.0f)
            actor->actionStep = 30;
        return TRUE;
    }
    case 0x10000:
        hitSound(actor, 0x2855, 0x33);
        actor->damageKind = 4;
        if (bl_playerCutType(gabi::at<daPy_py_c>(player)) == 17) {
            s16 yaw = gabi::object_at<fopAc_ac_c>(player).shape_angle.y;
            actor->damageKind = 5;
            actor->current.angle.y = (s16)(yaw - 0x4000);
        }
        actor->health = 0;
        break;
    case 0x40:
    case 0x80: {
        u32 volume = actor->burning ? 0x42 : 0x33;
        hitSound(actor, 0x2833, volume);
        actor->damageKind = 3;
        collisionParticle(actor, 12, impact.get());
        return TRUE;
    }
    case 0x200:
    case 0x4000:
    case 0x40000: {
        u8 burning = actor->burning;
        if (burning && (attack & 0x200))
            break;
        if (!burning && !(attack & 0x4000))
            actor->fireHelper.bounceCount = 100;
        stopFireSound(actor);
        u32 flags = actor->bodyCollider.attackFlags;
        actor->timers[2] = 0;
        actor->attention_info.flags = 0;
        actor->bodyCollider.attackFlags = flags & ~1u;
        fire_emitter_clr(actor);
        break;
    }
    case 0x100000: {
        actor->fireCylinder.scale = 1.0f;
        actor->iceState.state = 1;
        actor->iceState.speed = 40.0f;
        stopFireSound(actor);
        u32 flags = actor->bodyCollider.attackFlags;
        actor->gravity = 0.0f;
        actor->burning = 0;
        actor->shape_angle.z = 0;
        actor->speed.y = 0.0f;
        actor->shape_angle.x = 0;
        actor->current.angle.x = 0;
        actor->timers[0] = 0;
        actor->attention_info.flags = 0;
        actor->speedF = 0.0f;
        actor->timers[2] = 0;
        actor->speed.z = 0.0f;
        actor->speed.x = 0.0f;
        actor->current.angle.z = 0;
        actor->bodyCollider.attackFlags = flags & ~1u;
        fire_emitter_clr(actor);
        break;
    }
    case 0x80000:
        actor->timers[2] = 0;
        actor->speedF = 0.0f;
        actor->speed.y = 0.0f;
        actor->burning = 0;
        actor->shape_angle.z = 0;
        actor->current.angle.x = 0;
        actor->speed.x = 0.0f;
        actor->gravity = 0.0f;
        actor->current.angle.z = 0;
        actor->shape_angle.x = 0;
        actor->speed.z = 0.0f;
        actor->timers[0] = 0;
        fire_emitter_clr(actor);
        actor->iceState.freezeTimer = 200;
        actor->bodyCollider.attackFlags = (u32)actor->bodyCollider.attackFlags & ~1u;
        bl_enemy_fire_remove(reinterpret_cast<u8*>(&actor->fireHelper));
        actor->attention_info.flags = 0;
        return TRUE;
    case 0x8000: {
        u32 volume = actor->burning ? 0x42 : 0x33;
        hitSound(actor, 0x2834, volume);
        actor->damageKind = 7;
        actor->action = BL_ACTION_HOOK;
        actor->actionStep = 60;
        return TRUE;
    }
    default: {
        u32 volume = actor->burning ? 0x42 : 0x33;
        hitSound(actor, 0x2834, volume);
        collisionParticle(actor, 12, impact.get());
        return TRUE;
    }
    }
    checkAttack(actor, info.get());
    u8 kind = actor->damageKind;
    if ((u32)(kind - 4) <= 2 || (s8)actor->health <= 0) {
        collisionParticle(actor, 16, impact.get());
        scale->x = 2.0f;
        scale->z = 2.0f;
        scale->y = 2.0f;
        collisionParticle(actor, 15, impact.get(),
                          reinterpret_cast<u8*>(&gabi::object_at<fopAc_ac_c>(player).shape_angle),
                          scale.get());
    } else
        collisionParticle(actor, 13, impact.get(),
                          reinterpret_cast<u8*>(&gabi::object_at<fopAc_ac_c>(player).shape_angle));
    if (actor->burning) {
        anm_init(actor, 23, 1.0f, 0, 1.0f, -1);
        actor->speedF = ((f32)gabi::object_at<be<f32>>(bl_data::attackSpeedTuning)) + 60.0f;
        if ((s8)actor->health <= 0) {
            u8 kind = actor->damageKind;
            actor->speedF = ((f32)gabi::object_at<be<f32>>(bl_data::retreatSpeedTuning)) + 60.0f;
            if (kind == 5)
                actor->current.angle.y = (s16)(gabi::object_at<fopAc_ac_c>(player).shape_angle.y - 0x4000);
        }
        if (gabi::ea(&actor->eyePos)) {
            u32 id = actor ? actor->mBsPcId : 0xFFFFFFFF;
            s32 reverb = bl_dComIfGp_getReverb((s8)actor->current.roomNo);
            bl_seStartLevelFull_1(0x4873, reinterpret_cast<cXyz*>(&actor->eyePos), id, 0, reverb);
        }
        actor->action = BL_ACTION_SAGARIMASU;
        actor->actionStep = 21;
    } else {
        actor->action = BL_ACTION_ITAIYO;
        actor->actionStep = 40;
    }
    return TRUE;
}

/* 020AAB58 */
void bound_sound_set(bl_class* actor) {
    f32 strength = (f32)actor->speed.y * 3.3f;
    u32 volume = strength < 2147483648.0f ? (u32)gabi::ftoi(strength)
                                          : (u32)gabi::ftoi(strength - 2147483648.0f) + 0x80000000u;
    if (volume > 100)
        volume = 100;
    if (gabi::ea(&actor->eyePos)) {
        s32 reverb = bl_dComIfGp_getReverb((s8)actor->current.roomNo);
        bl_seStartFull_3(0x588C, reinterpret_cast<cXyz*>(&actor->eyePos), volume, reverb);
    }
}

/* 020AAC08 */
void fuwafuwa_keisan(bl_class* actor) {
    f32 target;
    if (actor->action == BL_ACTION_KOUGEKI) {
        u32 play = bl_getGameInfo();
        s16 phase = (s16)(actor->flightPhase + 1000);
        actor->flightPhase = phase;
        u32 player = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get());
        target = gabi::object_at<fopAc_ac_c>(player).current.pos.y + 80.0f;
        actor->targetHeight = target;
        f32 sine = cM_ssin(phase);
        target = gabi::fmadds(sine, 10.0f, target);
        actor->targetHeight = target;
        bl_cLib_addCalc2(&actor->current.pos.y, target, 1.0f, 6.0f);
    } else {
        s16 phase = (s16)(actor->flightPhase + 500);
        target = (f32)actor->homeGroundHeight + 100.0f;
        actor->flightPhase = phase;
        actor->targetHeight = target;
        f32 sine = cM_ssin(phase);
        target = gabi::fmadds(sine, 40.0f, target);
        actor->targetHeight = target;
        bl_cLib_addCalc2(&actor->current.pos.y, target, 1.0f, 3.0f);
    }
}

/* 020AAD08 */
void BG_check(bl_class* actor) {
    if (actor->actionStep == 2)
        return;
    bl_dBgS_AcchCir_SetWall(reinterpret_cast<u8*>(&actor->groundCircle), 40.0f, 40.0f);
    u32 play = bl_getGameInfo();
    bl_dBgS_Acch_CrrPos(reinterpret_cast<u8*>(&actor->groundCollision),
                        reinterpret_cast<u8*>(&gabi::object_at<dComIfG_inf_c>(play).play.mBgS));
    actor->groundCollision.flags = (u32)actor->groundCollision.flags | 0x2000;
    if (actor->flying == 0) {
        f32 ground = actor->groundCollision.groundHeight;
        if (ground == -1000000000.0f)
            actor->homeGroundHeight = ground;
    }
}

/* 020AAD90 */
BOOL roll_check(bl_class* actor) {
    bl_cLib_addCalcAngleS2(&actor->shape_angle.x, 0, 1, 0x1000);
    bl_cLib_addCalcAngleS2(&actor->shape_angle.z, 0, 1, 0x1000);
    s32 x = actor->shape_angle.x;
    actor->spinX = 0;
    actor->spinZ = 0;
    if (x < 0)
        x = -x;
    if (x >= 0x200)
        return FALSE;
    s32 z = actor->shape_angle.z;
    if (z < 0)
        z = -z;
    return z < 0x200;
}

/* 020AAE38 */
s16 way_check(bl_class* actor, s16 angle) {
    gabi::Local<LineCheck> line;
    bl_cBgS_LinChk_ct(line.get());
    line->groupPass = gabi::ea(&line->groupVtable);
    line->baseVtable = bl_data::lineBaseVtable;
    line->groupFlags[6] = 0;
    line->groupFlags[1] = 0;
    line->groupFlags[5] = 0;
    line->lineVtable = bl_data::lineCheckVtable;
    line->groupFlags[4] = 0;
    line->objectFlags = 1;
    line->objectPass = gabi::ea(&line->objectVtable);
    line->groupFlags[2] = 0;
    line->groupVtable = bl_data::groupPassVtable;
    line->objectVtable = bl_data::objectPassVtable;
    line->groupFlags[0] = 0;
    line->groupFlags[3] = 0;
    s32 step = bl_cM_rndFX(1.0f) < 0.5f ? -0x2000 : 0x2000;
    s16 direction = angle;
    gabi::Local<cXyz> forward, destination;
    for (u32 attempts = 0; attempts < 8; ++attempts) {
        bl_mDoMtx_YrotS(reinterpret_cast<u8*>(matrixStack()), direction);
        forward->x = 0.0f;
        forward->y = 0.0f;
        forward->z = 300.0f;
        bl_MtxPosition(forward.get(), destination.get());
        bl_PSVECAdd(destination.get(), &actor->current.pos, destination.get());
        bl_dBgS_LinChk_Set(line.get(), &actor->current.pos, destination.get(), actor);
        u32 play = bl_getGameInfo();
        BOOL blocked = bl_cBgS_LineCross(
            reinterpret_cast<u8*>(&gabi::object_at<dComIfG_inf_c>(play).play.mBgS), line.get());
        if (!blocked) {
            line->groupVtable = bl_data::groupPassVtable;
            line->objectVtable = bl_data::objectPassBaseVtable;
            line->lineVtable = bl_data::lineBaseDestructorVtable;
            bl_lineCheckDestructor(line.get(), 0);
            return direction;
        }
        direction = (s16)(direction + step);
    }
    line->groupVtable = bl_data::groupPassVtable;
    line->objectVtable = bl_data::objectPassBaseVtable;
    line->lineVtable = bl_data::lineBaseDestructorVtable;
    bl_lineCheckDestructor(line.get(), 0);
    return angle;
}

// The HD compiler inlines these action states into daBL_Execute.
/* inlined into daBL_Execute */
bool hasPath(bl_class* a) { return a->pathNo != 0xFF && a->path != 0; }
/* inlined into daBL_Execute */
fopAc_ac_c* opponent() {
    u32 play = bl_getGameInfo();
    return gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get();
}
/* inlined into daBL_Execute */
bool animationStopped(bl_class* a) {
    u32 m = gabi::ea((u8*)a->morph);
    return (gabi::object_at<BubbleMorph>(m).flags & 1) || gabi::object_at<BubbleMorph>(m).frameRate == 0.0f;
}
/* inlined into daBL_Execute */
void updateAppearanceEmitter(bl_class* a, u32 emitter) {
    u32 joint = bl_getModel(a)->jointController;
    u16 flags = gabi::object_at<BubbleJointController>(joint).flags;
    Mtx34* matrices = gabi::object_at<BubbleJointController>(joint).matrices;
    gabi::object_at<BubbleJointController>(joint).flags = flags | 0x10;
    bl_JPASetRMtxTVecfromMtx(reinterpret_cast<u8*>(matrices),
                             reinterpret_cast<u8*>(&gabi::object_at<BubbleEmitter>(emitter).rotation),
                             reinterpret_cast<u8*>(&gabi::object_at<BubbleEmitter>(emitter).translation));
}
/* inlined into daBL_Execute */
void actorSound(bl_class* a, u32 sound) {
    if (gabi::ea(&a->eyePos) == 0)
        return;
    s8 room = a->current.roomNo;
    u32 id = a ? (u32)a->mBsPcId : 0xFFFFFFFF;
    s32 reverb = bl_dComIfGp_getReverb(room);
    bl_seStartLevelFull_2(sound, reinterpret_cast<u8*>(&a->eyePos), id, 0, reverb);
}
/* inlined into daBL_Execute */
bool collisionBody(bl_class* a) {
    return a->variant == 1 ? blue_body_atari_check(a) : red_body_atari_check(a);
}
/* inlined into daBL_Execute */
void pathDelta(bl_class* a, f32& x, f32& z) {
    u32 path = a->path;
    u32 points = gabi::ea(gabi::object_at<path_layout::dPath>(path).m_points.get());
    s8 index = a->pathPoint;
    u32 point = points + (s32)index * 16;
    x = gabi::object_at<path_layout::dPnt>(point).m_position.x - (f32)a->current.pos.x;
    z = gabi::object_at<path_layout::dPnt>(point).m_position.z - (f32)a->current.pos.z;
}
/* inlined into daBL_Execute */
void action_dousa(bl_class* a, bool& forcePlay) {
    s16 state = a->actionStep;
    switch (state) {
    case 0: {
        a->flightPhase = 0;
        if (bl_fopAcM_searchActorDistance(a, reinterpret_cast<u8*>(opponent())) > 1000.0f)
            break;
        a->attention_info.flags = 4;
        a->actor_status = (u32)a->actor_status | 0x20;
        anm_init(a, 21, 1.0f, 0, 1.0f, -1);
        a->actionStep = (s16)((s16)a->actionStep + 1);
        break;
    }
    case 1: {
        a->targetScale = 1.5f;
        u32 emitter = gabi::ea(a->appearanceCallback.mpEmitter.get());
        if (!emitter) {
            u32 play = bl_getGameInfo(),
                particles = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mParticle.get());
            bl_dPa_control_c_set_4(reinterpret_cast<u8*>(particles), 0, 0x8122, &a->current.pos, 0, 0, 0xFF,
                                   reinterpret_cast<u8*>(&a->appearanceCallback), -1, 0, 0, 0);
            if (a && gabi::ea(&a->eyePos))
                hitSound(a, 0x5888, 0);
            emitter = gabi::ea(a->appearanceCallback.mpEmitter.get());
        }
        if (emitter) {
            bool red = a->variant == 0;
            gabi::object_at<BubbleEmitter>(emitter).red = red ? 0x55 : 0xD;
            gabi::object_at<BubbleEmitter>(emitter).green = red ? 0x1A : 0x20;
            gabi::object_at<BubbleEmitter>(emitter).blue = red ? 0xD : 0x41;
        }
        if (!animationStopped(a))
            break;
        a->actionStep = (s16)((s16)a->actionStep + 1);
        a->gravity = 0;
        a->speed.y = 0;
        [[fallthrough]];
    }
    case 2: {
        u32 emitter = gabi::ea(a->appearanceCallback.mpEmitter.get());
        if (emitter)
            updateAppearanceEmitter(a, emitter);
        bl_cLib_addCalc2_2(reinterpret_cast<u8*>(&a->speed.y), 10.0f, 0.8f, 1.0f);
        if ((f32)a->current.pos.y < (f32)a->homeGroundHeight + 100.0f)
            break;
        fire_move_set(a);
        a->speed.y = 0;
        hitSound(a, 0x5889, 0);
        actorSound(a, 0x4871);
        anm_init(a, 22, 1.0f, 0, 1.0f, -1);
        a->actionStep = (s16)((s16)a->actionStep + 1);
        break;
    }
    case 3:
        if (bl_J3DFrameCtrl_checkPass(
                reinterpret_cast<u8*>(&gabi::object_at<BubbleMorph>(gabi::ea((u8*)a->morph)).frameRate),
                10.0f))
            bl_dPa_followEcallBack_end(reinterpret_cast<u8*>(&a->appearanceCallback));
        if (animationStopped(a))
            a->actionStep = 5;
        break;
    case 4:
        fire_move_set(a);
        a->actor_status = (u32)a->actor_status | 0x20;
        a->timers[3] = 5;
        a->actionStep = 5;
        [[fallthrough]];
    case 5: {
        a->actor_status = (u32)a->actor_status | 0x20;
        if (a->animationId != 17) {
            anm_init(a, 17, 1.0f, 2, 1.0f, -1);
            u32 flags = a->bodyCollider.attackFlags;
            a->burning = 1;
            a->bodyCollisionStatus.weight = 2;
            a->bodyCollider.attackFlags = flags | 1;
            a->bodyCollider.attackType = 1;
        }
        a->attention_info.flags = 4;
        s16 heading;
        if (hasPath(a)) {
            f32 x, z;
            pathDelta(a, x, z);
            heading = bl_cM_atan2s(x, z);
            a->speedF = a->pathSpeed;
        } else {
            a->timers[0] = (s16)gabi::ftoi(bl_cM_rndFX(50.0f) + 100.0f);
            heading = (s16)gabi::ftoi(bl_cM_rndFX(32767.0f));
            if ((f32)a->speedF == 0.0f)
                a->speedF = bl_cM_rndF(2.0f) + 4.0f;
        }
        a->targetYaw = way_check(a, heading);
        a->actionStep = (s16)((s16)a->actionStep + 1);
        [[fallthrough]];
    }
    case 6: {
        if (hasPath(a)) {
            f32 x, z;
            pathDelta(a, x, z);
            s32 heading = bl_cM_atan2s_2(x, z);
            f32 distance = bl_sqrtf(gabi::fmadds(x, x, z * z));
            f32 threshold = ((f32)gabi::object_at<be<f32>>(bl_data::attackDistanceTuning)) + 80.0f;
            u32 savedY = bl_words(&a->current.pos).y, savedX = bl_words(&a->current.pos).x;
            if (distance < threshold) {
                s8 index = (s8)((s8)a->pathPoint + 1);
                a->pathPoint = index;
                if (index >= gabi::object_at<path_layout::dPath>((u32)a->path).m_num)
                    a->pathPoint = 0;
            }
            u32 savedZ = bl_words(&a->current.pos).z;
            bl_words(&a->roamingOrigin).x = savedX;
            bl_words(&a->roamingOrigin).y = savedY;
            bl_words(&a->roamingOrigin).z = savedZ;
            if (a->timers[0] == 0) {
                s16 next = way_check(a, (s16)heading);
                a->targetYaw = next;
                if (next != heading)
                    a->timers[0] = (s16)gabi::ftoi(bl_cM_rndF(10.0f) + 10.0f);
            }
            break;
        }
        f32 x = (f32)a->current.pos.x - (f32)a->roamingOrigin.x,
            z = (f32)a->current.pos.z - (f32)a->roamingOrigin.z;
        if (bl_sqrtf(gabi::fmadds(x, x, z * z)) < 250.0f) {
            if (a->timers[0] == 0)
                a->actionStep = 5;
            break;
        }
        a->actionStep = 7;
        [[fallthrough]];
    }
    case 7: {
        f32 x = (f32)a->roamingOrigin.x - (f32)a->current.pos.x,
            z = (f32)a->roamingOrigin.z - (f32)a->current.pos.z;
        a->targetYaw = bl_cM_atan2s(x, z);
        f32 radius = hasPath(a) ? 80.0f : 10.0f;
        if (bl_sqrtf(gabi::fmadds(x, x, z * z)) < radius)
            a->actionStep = 5;
        break;
    }
    }
    s16 step = a->actionStep;
    if ((u32)(step - 1) < 3) {
        u32 emitter = gabi::ea(a->appearanceCallback.mpEmitter.get());
        if (emitter)
            updateAppearanceEmitter(a, emitter);
    }
    if (a->timers[3] == 0)
        fire_kaiten_keisan(a);
    bool path = hasPath(a);
    bl_cLib_addCalcAngleS2_2(&a->current.angle.y, (s16)a->targetYaw, 1, path ? 0x1000 : 0x120);
    bl_cLib_addCalcAngleS2_2(&a->shape_angle.y, (s16)a->current.angle.y, 1, path ? 0x1000 : 0x400);
    if ((s16)a->actionStep >= 3) {
        a->targetScale = 1.5f;
        fuwafuwa_keisan(a);
    }
    if (collisionBody(a))
        return;
    if ((s16)a->actionStep < 5)
        return;
    if ((u32)a->bodyCollider.contactFlags & 1) {
        if (hasPath(a) && a->actionStep == 7) {
            forcePlay = true;
            return;
        }
        s32 angle = bl_fopAcM_searchActorAngleY(a, reinterpret_cast<u8*>(opponent()));
        a->action = BL_ACTION_KOUGEKI;
        a->current.angle.y = (s16)(angle + 0x8000);
        a->speedF = 50;
        a->actionStep = 10;
        forcePlay = true;
        return;
    }
    if (!a->burning) {
        forcePlay = true;
        return;
    }
    if (!(bl_fopAcM_searchActorDistance(a, reinterpret_cast<u8*>(opponent())) < 600.0f))
        return;
    u32 play = bl_getGameInfo(),
        player = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get());
    gabi::Local<cXyz> target, origin;
    target->x = gabi::object_at<fopAc_ac_c>(player).current.pos.x;
    target->y = gabi::object_at<fopAc_ac_c>(player).current.pos.y;
    target->z = gabi::object_at<fopAc_ac_c>(player).current.pos.z;
    gabi::Local<LineCheck> line;
    bl_cBgS_LinChk_ct(line.get());
    line->objectFlags = 1;
    line->groupFlags[2] = 0;
    line->baseVtable = bl_data::lineBaseVtable;
    line->lineVtable = bl_data::lineCheckVtable;
    line->groupFlags[4] = 0;
    line->groupFlags[1] = 0;
    line->groupFlags[0] = 0;
    line->groupFlags[3] = 0;
    line->groupPass = gabi::ea(&line->groupVtable);
    line->objectPass = gabi::ea(&line->objectVtable);
    line->groupVtable = bl_data::groupPassVtable;
    line->objectVtable = bl_data::objectPassVtable;
    line->groupFlags[6] = 0;
    line->groupFlags[5] = 0;
    origin->x = a->current.pos.x;
    origin->y = a->current.pos.y;
    origin->z = a->current.pos.z;
    target->y = (f32)target->y + 100.0f;
    origin->y = (f32)origin->y + 100.0f;
    bl_dBgS_LinChk_Set(line.get(), origin.get(), target.get(), a);
    play = bl_getGameInfo();
    BOOL blocked =
        bl_cBgS_LineCross(reinterpret_cast<u8*>(&gabi::object_at<dComIfG_inf_c>(play).play.mBgS), line.get());
    line->groupVtable = bl_data::groupPassVtable;
    line->objectVtable = bl_data::objectPassBaseVtable;
    line->lineVtable = bl_data::lineBaseDestructorVtable;
    bl_lineCheckDestructor(line.get(), 0);
    if (blocked)
        return;
    if (!(hasPath(a) && a->actionStep == 7)) {
        a->action = BL_ACTION_KOUGEKI;
        a->actionStep = 10;
    }
    forcePlay = true;
}
/* inlined into daBL_Execute */
void action_kougeki(bl_class* a, bool& forcePlay) {
    u32 play = bl_getGameInfo(),
        player = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get());
    u32 morph = 0;
    bool haveMorph = false;
    switch ((s16)a->actionStep) {
    case 10:
        a->spinX = 0;
        a->spinZ = 0;
        a->bounceOrSmokeFade = 0;
        a->skullSmokeFade = 0;
        a->bodyCollider.attackFlags = (u32)a->bodyCollider.attackFlags | 1;
        a->bodyCollider.attackType = 1;
        a->flightPhase = 0;
        anm_init(a, 18, 1.0f, 0, 1.0f, -1);
        if (a)
            actorSound(a, 0x4870);
        a->actionStep = (s16)((s16)a->actionStep + 1);
        a->speedF = 0;
        break;
    case 11:
        morph = gabi::ea((u8*)a->morph);
        haveMorph = true;
        if (!(gabi::object_at<BubbleMorph>(morph).flags & 1) &&
            gabi::object_at<BubbleMorph>(morph).frameRate != 0.0f)
            break;
        a->actionStep = (s16)((s16)a->actionStep + 1);
        [[fallthrough]];
    case 12: {
        anm_init(a, 16, 1.0f, 2, 1.0f, 6);
        a->speedF = 10;
        u32 id = a ? (u32)a->mBsPcId : 0xFFFFFFFF;
        f32 random = bl_cM_rndFX(768.0f);
        a->attackYawOffset = (s16)gabi::ftoi(random * (f32)(id & 3));
        a->actionStep = (s16)((s16)a->actionStep + 1);
        haveMorph = false;
        [[fallthrough]];
    }
    case 13:
        if (hasPath(a)) {
            f32 z = (f32)a->current.pos.z - (f32)a->roamingOrigin.z,
                x = (f32)a->current.pos.x - (f32)a->roamingOrigin.x;
            if (!(bl_sqrtf(gabi::fmadds(x, x, z * z)) > 700.0f))
                break;
            anm_init(a, 17, 1.0f, 2, 1.0f, -1);
            a->speedF = bl_cM_rndF(2.0f) + 4.0f;
            a->actionStep = 7;
        } else {
            if (!(bl_fopAcM_searchActorDistance(a, reinterpret_cast<u8*>(opponent())) > 700.0f))
                break;
            a->speedF = bl_cM_rndF(2.0f) + 4.0f;
            a->actionStep = 5;
        }
        a->action = BL_ACTION_DOUSA;
        forcePlay = true;
        return;
    case 14:
        bl_cLib_addCalc0(&a->speedF, 0.5f, 1.0f);
        if (!(std::fabs((f32)a->speedF) < 0.2f))
            break;
        a->speedF = 0;
        anm_init(a, 24, 1.0f, 0, 1.0f, -1);
        actorSound(a, 0x4870);
        a->actionStep = 15;
        break;
    case 15:
        morph = gabi::ea((u8*)a->morph);
        haveMorph = true;
        if ((gabi::object_at<BubbleMorph>(morph).flags & 1) ||
            gabi::object_at<BubbleMorph>(morph).frameRate == 0.0f)
            a->actionStep = 12;
        break;
    }
    if (!haveMorph)
        morph = gabi::ea((u8*)a->morph);
    gabi::object_at<BubbleMorph>(morph).frameRate = 1.0f;
    if (a->actionStep == 13 && bl_fopAcM_searchActorDistance(a, reinterpret_cast<u8*>(opponent())) < 230.0f)
        gabi::object_at<BubbleMorph>(gabi::ea((u8*)a->morph)).frameRate = 2.0f;
    if ((u32)a->bodyCollider.contactFlags & 1) {
        if (a->actionStep != 14) {
            a->action = BL_ACTION_KOUGEKI;
            a->speedF = -20;
            a->actionStep = 14;
        }
    } else if (bl_dCcD_GObjInf_ChkAtHit(reinterpret_cast<u8*>(&a->bodyCollider))) {
        u32 hit = bl_dCcD_GAtTgCoCommonBase_GetAc(reinterpret_cast<u8*>(&a->bodyCollider.contactObject));
        if (hit && hit == player && a->actionStep != 15) {
            anm_init(a, 20, 1.0f, 0, 1.0f, -1);
            if (a)
                actorSound(a, 0x4872);
            a->speedF = 0;
            a->spinX = 0;
            a->actionStep = 15;
        }
    }
    fire_kaiten_keisan(a);
    s32 heading = bl_fopAcM_searchActorAngleY(a, reinterpret_cast<u8*>(opponent()));
    s16 target = (s16)(heading + (s16)a->attackYawOffset);
    a->targetYaw = target;
    bl_cLib_addCalcAngleS2_2(&a->current.angle.y, target, 1, 0x1000);
    bl_cLib_addCalcAngleS2_2(&a->shape_angle.y, (s16)a->current.angle.y, 1, 0x400);
    if ((s16)a->actionStep >= 12)
        fuwafuwa_keisan(a);
    collisionBody(a);
}
/* inlined into daBL_Execute */
void action_sagarimasu(bl_class* a) {
    u32 play = bl_getGameInfo(),
        player = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get());
    s16 state = a->actionStep;
    if (state == 20) {
        anm_init(a, 23, 1.0f, 0, 1.0f, -1);
        a->speedF = ((f32)gabi::object_at<be<f32>>(bl_data::attackSpeedTuning)) + 20.0f;
        u8 damage = a->damageKind;
        if (damage == 4 || damage == 5) {
            a->speedF = ((f32)gabi::object_at<be<f32>>(bl_data::retreatSpeedTuning)) + 60.0f;
            if (a->damageKind == 5)
                a->current.angle.y = (s16)(gabi::object_at<fopAc_ac_c>(player).shape_angle.y - 0x4000);
        }
        a->actionStep = (s16)((s16)a->actionStep + 1);
        state = 21;
    }
    if (state == 21) {
        bl_cLib_addCalc0(&a->speedF, 0.8f, 5.0f);
        if ((f32)a->speedF < 0.1f) {
            a->action = BL_ACTION_KOUGEKI;
            a->actionStep = 10;
            if (a->burning && a->variant == 0 && a->health <= 0) {
                gabi::Local<cXyz> position;
                position->x = a->current.pos.x;
                position->y = a->current.pos.y;
                position->z = a->current.pos.z;
                position->y = (f32)position->y + 30.0f;
                bl_fopAcM_createDisappear(a, position.get(), 5, 0, 0xFF);
                s8 room = a->home.roomNo;
                a->attention_info.flags = 0;
                dSv_info_c* save = bl_saveInfo((u32)gabi::object_at<be<u32>>(bl_data::saveRoot));
                u16 id = a->setID;
                bl_dSv_info_c_onActor(reinterpret_cast<u8*>(save), id, room);
                bl_fopAcM_delete(a);
            }
        }
    }
    fire_kaiten_keisan(a);
    collisionBody(a);
}
/* inlined into daBL_Execute */
void action_kaze_move(bl_class* a) {
    bool followed = false;
    switch ((s16)a->actionStep) {
    case 30: {
        a->spinX = 0;
        a->spinZ = 0;
        a->bounceOrSmokeFade = 0;
        a->skullSmokeFade = 0;
        a->bodyCollider.targetState = 0;
        a->bodyCollider.targetMaterial = 0;
        u32 flags = a->bodyCollider.attackFlags;
        a->attention_info.flags = 4;
        u32 hit = a->bodyCollider.targetLinkFlags;
        a->bounceSpeed = 7;
        a->speedF = 15;
        a->bodyCollider.targetLinkFlags = hit & ~1u;
        a->bodyCollider.attackFlags = flags & ~1u;
        a->spinX = (s16)gabi::ftoi(bl_cM_rndF(500.0f) + 512.0f);
        if (bl_cM_rnd() < 0.5f)
            a->spinX = (s16)(-(s16)a->spinX);
        if (a->animationId != 23)
            anm_init(a, 23, 1.0f, 0, 1.0f, -1);
        a->timers[2] = 0;
        fire_emitter_clr(a);
        stopFireSound(a);
        a->actionStep = (s16)((s16)a->actionStep + 1);
        [[fallthrough]];
    }
    case 31:
        a->shape_angle.z = (s16)((s16)a->shape_angle.z + (s16)a->spinX);
        bl_cLib_addCalc2_2(reinterpret_cast<u8*>(&a->speed.y), (f32)a->bounceSpeed, 0.8f, 2.0f);
        bl_cLib_addCalc0(&a->speedF, 0.8f, 3.0f);
        if ((f32)a->speedF > 0.1f)
            break;
        a->actionStep = (s16)((s16)a->actionStep + 1);
        [[fallthrough]];
    case 32:
        a->burning = 0;
        a->actionStep = (s16)((s16)a->actionStep + 1);
        a->speedF = 0;
        [[fallthrough]];
    case 33:
        bl_cLib_addCalc2(&a->gravity, -3.0f, 0.8f, 0.1f);
        bl_cLib_addCalcAngleS2(&a->shape_angle.x, -0x7FFF, 1, 0x120);
        if (!((u32)a->groundCollision.flags & 0x20))
            break;
        a->actionStep = (s16)((s16)a->actionStep + 1);
        [[fallthrough]];
    case 34:
        a->speed.y = 15;
        a->gravity = -3;
        bound_sound_set(a);
        a->actionStep = (s16)((s16)a->actionStep + 1);
        [[fallthrough]];
    case 35:
        bl_cLib_addCalcAngleS2(&a->shape_angle.x, 0, 1, 0x400);
        bl_cLib_addCalcAngleS2(&a->shape_angle.z, 0, 1, 0x400);
        if (std::abs((s16)a->shape_angle.x) >= 0x100 || std::abs((s16)a->shape_angle.z) >= 0x100)
            break;
        a->shape_angle.x = 0;
        a->shape_angle.z = 0;
        if ((u8)a->variant & 0x80) {
            a->action = BL_ACTION_SKULL;
            a->actionStep = 100;
        } else
            a->actionStep = 38;
        bl_dPa_followEcallBack_end(reinterpret_cast<u8*>(&a->appearanceCallback));
        followed = true;
        a->targetScale = 1;
        break;
    case 36: {
        a->spinX = 0;
        a->spinZ = 0;
        a->bounceOrSmokeFade = 0;
        a->skullSmokeFade = 0;
        a->speed.y = 15;
        a->gravity = -3;
        bound_sound_set(a);
        if (!((u8)a->variant & 0x80))
            a->attention_info.flags = 4;
        u32 flags = a->bodyCollider.attackFlags;
        a->speedF = 30;
        gabi::Local<cXyz> forward, direction;
        forward->x = 0;
        forward->z = 5000;
        forward->y = 0;
        a->bodyCollider.attackFlags = flags & ~1u;
        Mtx34* stack = matrixStack();
        s32 heading = bl_fopAcM_searchActorAngleY(a, reinterpret_cast<u8*>(opponent()));
        bl_mDoMtx_YrotS(reinterpret_cast<u8*>(stack), (s16)(heading + 0x8000));
        bl_MtxPosition(forward.get(), direction.get());
        f32 x = -(f32)direction->x;
        a->bodyCollisionStatus.weight = 0x50;
        f32 z = direction->z;
        a->spinX = (s16)gabi::ftoi(x);
        a->spinZ = (s16)gabi::ftoi(z);
        a->actionStep = (s16)((s16)a->actionStep + 1);
        [[fallthrough]];
    }
    case 37:
        a->shape_angle.x = (s16)((s16)a->shape_angle.x + (s16)a->spinX);
        a->shape_angle.z = (s16)((s16)a->shape_angle.z + (s16)a->spinZ);
        if ((f32)a->speedF < 5.0f) {
            if (roll_check(a)) {
                if ((u8)a->variant & 0x80) {
                    a->actionStep = 100;
                    a->action = BL_ACTION_SKULL;
                } else
                    a->actionStep = 38;
            } else if (a->bounceOrSmokeFade == 0)
                break;
        } else if ((u32)a->groundCollision.flags & 0x20) {
            a->speed.y = 10;
            a->bounceOrSmokeFade = 1;
            bound_sound_set(a);
        }
        if (a->bounceOrSmokeFade == 0)
            break;
        bl_cLib_addCalc0(&a->speedF, 0.8f, 1.0f);
        bl_dPa_followEcallBack_end(reinterpret_cast<u8*>(&a->appearanceCallback));
        followed = true;
        a->targetScale = 1;
        break;
    case 38:
        a->timers[1] = (s16)gabi::ftoi(bl_cM_rndF(35.0f) + 70.0f);
        a->speedF = 0;
        a->actionStep = (s16)((s16)a->actionStep + 1);
        [[fallthrough]];
    case 39:
        if (a->timers[1] != 0)
            break;
        anm_init(a, 21, 1.0f, 0, 1.0f, -1);
        a->speedF = 0;
        a->current.angle.y = a->shape_angle.y;
        a->action = BL_ACTION_DOUSA;
        a->actionStep = 1;
        break;
    }
    if (!followed) {
        bl_dPa_followEcallBack_end(reinterpret_cast<u8*>(&a->appearanceCallback));
        a->targetScale = 1;
    }
    if ((u8)a->variant & 0x80)
        skull_atari_check(a);
    else
        collisionBody(a);
}
/* inlined into daBL_Execute */
void recordActorDeath(bl_class* a) {
    s8 room = a->home.roomNo;
    dSv_info_c* save = bl_saveInfo((u32)gabi::object_at<be<u32>>(bl_data::saveRoot));
    u16 id = a->setID;
    bl_dSv_info_c_onActor(reinterpret_cast<u8*>(save), id, room);
    bl_fopAcM_delete(a);
}
/* inlined into daBL_Execute */
void action_itaiyo_ne_san(bl_class* a) {
    u32 play = bl_getGameInfo(),
        player = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mPlayerInfo[0].mpPlayer.get());
    switch ((s16)a->actionStep) {
    case 40: {
        a->targetScale = 1;
        a->spinX = 0;
        a->spinZ = 0;
        a->bounceOrSmokeFade = 0;
        a->skullSmokeFade = 0;
        s32 animation = a->animationId;
        a->gravity = -3;
        a->attention_info.flags = 4;
        a->bodyCollider.attackFlags = (u32)a->bodyCollider.attackFlags & ~1u;
        if (animation != 23)
            anm_init(a, 23, 1.0f, 0, 1.0f, -1);
        a->timers[2] = 0;
        fire_emitter_clr(a);
        gabi::Local<cXyz> forward, direction;
        forward->x = 0;
        forward->y = 0;
        if (a->health <= 0) {
            if (a->damageKind == 4) {
                a->timers[0] = 0;
                a->attention_info.flags = 0;
                a->actionStep = 42;
                // HD deliberately leaves forward.z as the original stack contents.
            } else {
                a->timers[0] = 50;
                u8 damage = a->damageKind;
                a->speed.y = 30;
                a->speedF = 20;
                a->bounceSpeed = 30;
                forward->z = 5000;
                if (damage == 5)
                    a->current.angle.y = (s16)(gabi::object_at<fopAc_ac_c>(player).shape_angle.y - 0x4000);
                bound_sound_set(a);
                a->attention_info.flags = 0;
                a->actionStep = 42;
            }
        } else {
            a->actionStep = 41;
            a->speed.y = 15;
            a->speedF = 10;
            forward->z = 5000;
            bound_sound_set(a);
        }
        Mtx34* stack = matrixStack();
        s32 angle = bl_fopAcM_searchActorAngleY(a, reinterpret_cast<u8*>(opponent()));
        bl_mDoMtx_YrotS(reinterpret_cast<u8*>(stack), (s16)(angle + 0x8000));
        bl_MtxPosition(forward.get(), direction.get());
        a->spinX = (s16)gabi::ftoi(-(f32)direction->x);
        a->spinZ = (s16)gabi::ftoi((f32)direction->z);
        break;
    }
    case 41:
        if ((f32)a->speedF < 0.1f && roll_check(a)) {
            a->action = BL_ACTION_KAZE_MOVE;
            a->actionStep = 38;
        }
        bl_cLib_addCalc0(&a->speedF, 0.8f, 1.0f);
        collisionBody(a);
        break;
    case 42:
        if (!((u32)a->groundCollision.flags & 0x20))
            break;
        {
            f32 bounce = (f32)a->bounceSpeed * 0.5f;
            a->bounceSpeed = bounce;
            if (bounce < 10.0f) {
                bounce = 10;
                a->bounceSpeed = 10;
            }
            a->speed.y = bounce;
            bound_sound_set(a);
        }
        if (a->timers[0] != 0)
            break;
        {
            gabi::Local<cXyz> vanish;
            vanish->x = a->current.pos.x;
            vanish->y = a->current.pos.y;
            vanish->z = a->current.pos.z;
            vanish->y = (f32)vanish->y + 30;
            bl_fopAcM_createDisappear(a, vanish.get(), 5, 0, 0xFF);
        }
        if (a->damageKind != 4) {
            recordActorDeath(a);
            break;
        }
        {
            f32 height = ((f32)gabi::object_at<be<f32>>(bl_data::smokeHeightTuning)) + 25;
            gabi::Local<cXyz> smoke;
            smoke->x = a->current.pos.x;
            smoke->y = (f32)a->current.pos.y + height;
            smoke->z = a->current.pos.z;
            s8 room = a->current.roomNo;
            play = bl_getGameInfo();
            dPa_control_c* particles = gabi::object_at<dComIfG_inf_c>(play).play.mParticle.get();
            u32 emitter = bl_dPa_control_c_set_5(reinterpret_cast<u8*>(particles), 0, 0x3E8, smoke.get(),
                                                 &a->current.angle, &a->scale, 0xFF, 0, room,
                                                 reinterpret_cast<u8*>(&a->tevStr.mColorK0),
                                                 reinterpret_cast<u8*>(&a->tevStr.mColorK0), 0);
            if (emitter) {
                gabi::object_at<BubbleEmitter>(emitter).velocityRandomness = 30;
                gabi::object_at<BubbleEmitter>(emitter).spread = 20;
            }
            bl_words(&a->smokePosition).x = bl_words(smoke.get()).x;
            bl_words(&a->smokePosition).y = bl_words(smoke.get()).y;
            bl_words(&a->smokePosition).z = bl_words(smoke.get()).z;
            smoke_set(a);
            a->bodyCollider.targetFlags = (u32)a->bodyCollider.targetFlags & ~1u;
            a->bodyCollider.correctionFlags = (u32)a->bodyCollider.correctionFlags & ~1u;
            bl_dCcD_GObjInf_ClrTgHit(reinterpret_cast<u8*>(&a->bodyCollider));
            a->scale.x = 0;
            a->scale.y = 0;
            a->scale.z = 0;
            a->bounceOrSmokeFade = 180;
            a->targetScale = 0;
            a->timers[1] = 10;
            a->actionStep = (s16)((s16)a->actionStep + 1);
        }
        break;
    case 43: {
        u32 emitter = a->smokeCallback.emitter;
        if (!emitter) {
            recordActorDeath(a);
            break;
        }
        if (a->timers[1] != 0)
            break;
        gabi::object_at<BubbleEmitter>(emitter).alpha = (u8)bl_bytes(&a->bounceOrSmokeFade).low;
        s16 fade = (s16)((s16)a->bounceOrSmokeFade - 4);
        a->bounceOrSmokeFade = fade;
        if (fade < 0)
            bl_dPa_smokeEcallBack_end(reinterpret_cast<u8*>(&a->smokeCallback));
        break;
    }
    }
    a->shape_angle.x = (s16)((s16)a->shape_angle.x + (s16)a->spinX);
    a->shape_angle.z = (s16)((s16)a->shape_angle.z + (s16)a->spinZ);
}
/* inlined into daBL_Execute */
void action_hook_atari(bl_class* a) {
    bl_touchGameInfo();
    if (a->actionStep == 60) {
        a->spinX = 0;
        a->spinZ = 0;
        a->bounceOrSmokeFade = 0;
        a->skullSmokeFade = 0;
        if (gabi::ea(a->fireCallback.mpEmitter.get())) {
            a->timers[2] = 0;
            fire_emitter_clr(a);
            anm_init(a, 23, 1.0f, 0, 1.0f, -1);
            a->speedF = 0;
            a->bodyCollider.attackFlags = (u32)a->bodyCollider.attackFlags & ~1u;
            stopFireSound(a);
        }
        u32 flags = a->bodyCollider.targetLinkFlags, status = a->actor_status;
        a->bodyCollider.targetState = 0;
        a->bodyCollider.targetMaterial = 0;
        a->bodyCollider.targetLinkFlags = flags & ~1u;
        a->actor_status = status | 0x20;
        a->attention_info.flags = 4;
        a->actionStep = (s16)((s16)a->actionStep + 1);
    } else if (a->actionStep == 61 && !((u32)a->actor_status & 0x100000)) {
        a->action = BL_ACTION_KAZE_MOVE;
        a->actionStep = 32;
    }
    bl_dPa_followEcallBack_end(reinterpret_cast<u8*>(&a->appearanceCallback));
}
/* inlined into daBL_Execute */
void action_come_wait(bl_class* a, bool& forcePlay) {
    bl_touchGameInfo();
    if (a->actionStep != 70)
        return;
    u8 switchNo = a->switchNo;
    if (switchNo == 0xFF) {
        forcePlay = true;
        return;
    }
    dSv_info_c* save = bl_saveInfo((u32)gabi::object_at<be<u32>>(bl_data::saveRoot));
    s8 room = ((s8)gabi::object_at<be<s8>>(bl_data::roomNo));
    if (!bl_dSv_info_c_isSwitch(reinterpret_cast<u8*>(save), switchNo, room))
        return;
    a->action = BL_ACTION_KAZE_MOVE;
    a->actor_status = (u32)a->actor_status | 0x20;
    a->actionStep = 38;
    forcePlay = true;
}
/* inlined into daBL_Execute */
void action_normal_skull(bl_class* a) {
    bl_touchGameInfo();
    gabi::Local<cXyz> position;
    position->x = a->current.pos.x;
    position->y = a->current.pos.y;
    position->z = a->current.pos.z;
    switch ((s16)a->actionStep) {
    case 100: {
        f32 radius = ((f32)gabi::object_at<be<f32>>(bl_data::vanishRadiusTuning)) + 30;
        u32 status = a->actor_status;
        a->gravity = -3;
        a->collisionRadius = radius;
        if (!(status & 0x2000))
            break;
        a->bodyCollider.correctionFlags = (u32)a->bodyCollider.correctionFlags & ~1u;
        a->gravity = 0;
        a->actionStep = (s16)((s16)a->actionStep + 1);
    }
        [[fallthrough]];
    case 101:
        if ((u32)a->actor_status & 0x2000)
            break;
        a->bodyCollider.correctionFlags = (u32)a->bodyCollider.correctionFlags | 1;
        a->gravity = -3;
        if ((f32)a->speedF > 0.0f) {
            a->actionStep = 102;
            a->speed.y = 25;
            a->speedF = 35;
        } else {
            a->speedF = 0;
            a->speed.x = 0;
            a->speed.y = 0;
            a->speed.z = 0;
            a->gravity = -3;
            a->actionStep = 100;
        }
        break;
    case 102:
        if (!((u32)a->groundCollision.flags & 0x30))
            break;
        {
            s16 bounce = a->fireHelper.bounceCount;
            a->speed.x = 0;
            a->speed.y = 0;
            a->speed.z = 0;
            if (bounce != 0)
                break;
            a->speedF = 0;
            a->actionStep = 104;
            a->attention_info.flags = 0;
            a->health = 0;
        }
        break;
    case 103:
        bound_sound_set(a);
        a->actionStep = 102;
        break;
    case 104: {
        f32 height = ((f32)gabi::object_at<be<f32>>(bl_data::smokeHeightTuning)) + 25;
        s8 room = a->current.roomNo;
        position->y = (f32)position->y + height;
        u32 play = bl_getGameInfo(),
            particles = gabi::ea(gabi::object_at<dComIfG_inf_c>(play).play.mParticle.get());
        u32 emitter = bl_dPa_control_c_set_5(
            reinterpret_cast<u8*>(particles), 0, 0x3E8, position.get(), &a->current.angle, &a->scale, 0xFF, 0,
            room, reinterpret_cast<u8*>(&a->tevStr.mColorK0), reinterpret_cast<u8*>(&a->tevStr.mColorK0), 0);
        if (emitter) {
            gabi::object_at<BubbleEmitter>(emitter).velocityRandomness = 30;
            gabi::object_at<BubbleEmitter>(emitter).spread = 20;
        }
        bl_words(&a->smokePosition).x = bl_words(position.get()).x;
        bl_words(&a->smokePosition).y = bl_words(position.get()).y;
        bl_words(&a->smokePosition).z = bl_words(position.get()).z;
        smoke_set(a);
        s32 items = 0;
        while (items < gabi::ftoi(bl_cM_rndF(1.99f))) {
            bl_fopAcM_createItem(&a->current.pos, 0, -1, -1, 0, 0, 4, 0);
            ++items;
        }
        a->bodyCollider.targetFlags = (u32)a->bodyCollider.targetFlags & ~1u;
        a->bodyCollider.correctionFlags = (u32)a->bodyCollider.correctionFlags & ~1u;
        bl_dCcD_GObjInf_ClrTgHit(reinterpret_cast<u8*>(&a->bodyCollider));
        a->scale.x = 0;
        a->scale.y = 0;
        a->scale.z = 0;
        a->targetScale = 0;
        a->skullSmokeFade = 180;
        a->timers[1] = 10;
        a->actionStep = (s16)((s16)a->actionStep + 1);
        break;
    }
    case 105: {
        u32 emitter = a->smokeCallback.emitter;
        if (!emitter) {
            bl_fopAcM_delete(a);
            break;
        }
        if (a->timers[1] != 0)
            break;
        gabi::object_at<BubbleEmitter>(emitter).alpha = (u8)bl_bytes(&a->skullSmokeFade).low;
        s16 fade = (s16)((s16)a->skullSmokeFade - 4);
        a->skullSmokeFade = fade;
        if (fade < 0)
            bl_dPa_smokeEcallBack_end(reinterpret_cast<u8*>(&a->smokeCallback));
        break;
    }
    }
    a->modelYOffset = 24;
    skull_atari_check(a);
}
/* 020AB020 */
BOOL daBL_Execute(bl_class* a) {
    if (a->timers[2] == 1)
        bl_dPa_followEcallBack_end(reinterpret_cast<u8*>(&a->fireCallback));
    s16 immunity = a->timers[5];
    u32 x = bl_words(&a->current.pos).x;
    if (immunity == 1)
        a->actor_status = (u32)a->actor_status & ~0x4000u;
    bl_words(&a->eyePos).x = x;
    u32 y = bl_words(&a->current.pos).y, z = bl_words(&a->current.pos).z;
    bl_words(&a->eyePos).y = y;
    f32 eyeY = a->eyePos.y;
    bl_words(&a->eyePos).z = z;
    bl_words(&a->attention_info.position).x = x;
    bl_words(&a->attention_info.position).y = y;
    f32 attentionY = a->attention_info.position.y;
    bl_words(&a->attention_info.position).z = z;
    a->eyePos.y = eyeY + 40;
    a->attention_info.position.y = attentionY + 80;
    if (!((u8)a->variant & 0x80))
        bl_mDoExt_baseAnm_play(a->burning ? (u8*)a->textureAnimation : (u8*)a->colorAnimation);
    if (bl_enemy_ice(reinterpret_cast<u8*>(&a->iceState))) {
        mtx_copy(&bl_getModel(a)->baseMatrix, &gabi::object_at<Mtx34>(bl_data::matrixWork));
        bl_mDoExt_McaMorf_calc((u8*)a->morph);
        s16 timer = a->timers[2];
        if (timer)
            a->timers[2] = (s16)(timer - 1);
        return TRUE;
    }
    for (u32 i = 0; i < 6; ++i) {
        s16 timer = a->timers[i];
        if (timer)
            a->timers[i] = (s16)(timer - 1);
    }
    bool forcePlay = false;
    switch ((u8)a->action) {
    case BL_ACTION_DOUSA:
        action_dousa(a, forcePlay);
        break;
    case BL_ACTION_KOUGEKI:
        action_kougeki(a, forcePlay);
        break;
    case BL_ACTION_SAGARIMASU:
        action_sagarimasu(a);
        break;
    case BL_ACTION_KAZE_MOVE:
        action_kaze_move(a);
        break;
    case BL_ACTION_ITAIYO:
        action_itaiyo_ne_san(a);
        break;
    case BL_ACTION_HOOK:
        action_hook_atari(a);
        break;
    case BL_ACTION_COME_WAIT:
        action_come_wait(a, forcePlay);
        break;
    case BL_ACTION_SKULL:
        action_normal_skull(a);
        break;
    }
    if (forcePlay || a->actionStep != 0) {
        u32 material = 0;
        if ((u32)a->groundCollision.flags & 0x20) {
            u32 play = bl_getGameInfo();
            material =
                bl_dBgS_GetMtrlSndId(reinterpret_cast<u8*>(&gabi::object_at<dComIfG_inf_c>(play).play.mBgS),
                                     reinterpret_cast<u8*>(&a->groundCollision.groundPolygon));
        }
        s32 reverb = bl_dComIfGp_getReverb((s8)a->current.roomNo);
        bl_mDoExt_McaMorf_play((u8*)a->morph, reinterpret_cast<u8*>(&a->eyePos), material, reverb);
    }
    bl_mDoMtx_YrotS(reinterpret_cast<u8*>(matrixStack()), (s16)a->current.angle.y);
    gabi::Local<cXyz> forward, direction;
    forward->x = 0;
    forward->y = 0;
    forward->z = a->speedF;
    bl_MtxPosition(forward.get(), direction.get());
    f32 velocity = a->speed.y, dx = direction->x, gravity = a->gravity;
    a->speed.x = dx;
    velocity = velocity + gravity;
    a->speed.z = direction->z;
    if (velocity < -55.0f)
        velocity = -55.0f;
    a->speed.y = velocity;
    bl_cLib_addCalc2(&a->scale.x, (f32)a->targetScale, 0.3f, 0.3f);
    f32 scale = a->scale.x;
    a->scale.z = scale;
    a->scale.y = scale;
    gabi::Local<cXyz> center;
    center->x = a->current.pos.x;
    center->y = a->current.pos.y;
    center->z = a->current.pos.z;
    center->y = (f32)center->y + 20;
    bl_cM3dGSph_SetC(reinterpret_cast<u8*>(&a->bodyCollider.sphere), center.get());
    bl_cM3dGSph_SetR(reinterpret_cast<u8*>(&a->bodyCollider.sphere), (f32)a->collisionRadius);
    u32 play = bl_getGameInfo();
    bl_cCcS_Set(reinterpret_cast<u8*>(&gabi::object_at<dComIfG_inf_c>(play).play.mCcS),
                reinterpret_cast<u8*>(&a->bodyCollider));
    bl_fopAcM_posMove(a, ((u32)a->bodyCollider.correctionFlags & 1)
                             ? reinterpret_cast<u8*>(&a->bodyCollisionStatus)
                             : nullptr);
    BG_check(a);
    if (!(a->flying && (f32)a->gravity == 0.0f) && (f32)a->groundCollision.groundHeight != -1000000000.0f) {
        play = bl_getGameInfo();
        if (bl_cBgS_ChkPolySafe(reinterpret_cast<u8*>(&gabi::object_at<dComIfG_inf_c>(play).play.mBgS),
                                reinterpret_cast<u8*>(&a->groundCollision.groundPolygon))) {
            play = bl_getGameInfo();
            if (bl_dBgS_GetGroundCode(reinterpret_cast<u8*>(&gabi::object_at<dComIfG_inf_c>(play).play.mBgS),
                                      reinterpret_cast<u8*>(&a->groundCollision.groundPolygon)) == 4 &&
                (f32)a->current.pos.y < (f32)a->homeGroundHeight - 500.0f) {
                a->speedF = 0;
                a->speed.x = 0;
                a->speed.y = 0;
                a->speed.z = 0;
                a->gravity = 0;
                recordActorDeath(a);
            }
        }
    }
    draw_SUB(a);
    return TRUE;
}

/* 020AD594 */
BOOL daBL_IsDelete(bl_class* actor) { return TRUE; }
/* 020AD59C */
BOOL daBL_Delete(bl_class* actor) {
    for (u8* callback :
         {reinterpret_cast<u8*>(&actor->appearanceCallback), reinterpret_cast<u8*>(&actor->fireCallback),
          reinterpret_cast<u8*>(&actor->smokeCallback)}) {
        u32 table = gabi::object_at<BubbleCallbackRoot>(gabi::ea(callback)).vtable;
        u32 target = gabi::object_at<BubbleCallbackVtable>(table).end;
        gabi::call(target, gabi::at<u8>(gabi::ea(callback)));
    }
    bl_enemy_fire_remove(reinterpret_cast<u8*>(&actor->fireHelper));
    bl_dComIfG_resDelete(&actor->phase, "Bl");
    return TRUE;
}

/* 020AD61C */
BOOL useHeapInit(bl_class* actor) {
    gabi::Local<SafeString> modelName, animationName;
    modelName->__vtbl = gc_tu.safeString;
    modelName->mStringTop = gabi::guest_addr("Bl");
    u8* resources = bl_resourceControl();
    u8* modelResource = bl_dRes_control_c_getRes(resources, modelName.get(), 0x1B);
    animationName->mStringTop = gabi::guest_addr("Bl");
    animationName->__vtbl = gc_tu.safeString;
    resources = bl_resourceControl();
    u8* animationResource = bl_dRes_control_c_getRes(resources, animationName.get(), 0x15);
    u8* morph = bl_mDoExt_McaMorf_mDoExt_McaMorf(0, modelResource, 0, 0, animationResource, 1, 0, -1, 1, 0,
                                                 0x80000, 0x37441422, 0.0f);
    actor->morph = morph;
    if (!morph || !bl_getMorphModel(morph))
        return FALSE;
    BubbleModel* m = bl_getMorphModel(morph);
    for (bool texture : {true, false}) {
        gptr<u8>& animationSlot = texture ? actor->textureAnimation : actor->colorAnimation;
        u8* animation = bl_allocateBytes(0x74);
        if (animation)
            animation = bl_mDoExt_btkAnm_init(animation);
        animationSlot = animation;
        if (!animation)
            return FALSE;
        gabi::Local<SafeString> name;
        name->__vtbl = gc_tu.safeString;
        name->mStringTop = gabi::guest_addr("Bl");
        resources = bl_resourceControl();
        u8* resource = bl_dRes_control_c_getRes(resources, name.get(), texture ? 0x1F : 0x1E);
        animation = animationSlot;
        u32 data = m->modelData;
        if (!bl_textureAnimationInit(animation, gabi::at<u8>(data), resource, 1, 0, 0, -1, 0, 0, 1.0f))
            return FALSE;
        if (!animationSlot)
            return FALSE;
    }
    m = bl_getModel(actor);
    m->userArea = gabi::ea(actor);
    m = bl_getModel(actor);
    return bl_invisibleModelCreate(reinterpret_cast<u8*>(&actor->lightInfo), reinterpret_cast<u8*>(m)) != 0;
}

/* 020ADA5C */
s32 daBL_Create(bl_class* actor) {
    u32 flags = actor->actor_condition;
    if (!(flags & 8)) {
        if (actor) {
            constructBubble(actor);
            flags = actor->actor_condition;
        }
        actor->actor_condition = flags | 8;
    }
    s32 phase = bl_dComIfG_resLoad(&actor->phase, "Bl");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    u32 params = actor->mParameters;
    s16 pitch = actor->current.angle.z;
    actor->variant = params;
    actor->flying = params >> 24;
    actor->pathNo = params >> 16;
    actor->switchNo = params >> 8;
    u8 type = actor->variant;
    actor->current.angle.z = 0;
    actor->shape_angle.z = 0;
    actor->pathSpeed = (f32)pitch;
    if (type == 0xFF)
        actor->variant = 0;
    if (params >> 24 == 0xFF)
        actor->flying = 0;
    if (((s16)gabi::object_at<be<s16>>(bl_data::debugFlying)))
        actor->flying = 1;
    s16 debugType = ((s16)gabi::object_at<be<s16>>(bl_data::debugVariant));
    if (debugType)
        actor->variant = debugType - 1;
    if (!bl_fopAcM_entrySolidHeap(actor, useHeapInit, 0x14E0))
        return cPhs_ERROR_e;
    type = actor->variant;
    if (type & 2) {
        u8 pathNo = actor->pathNo;
        actor->variant = type ^ 2;
        if (pathNo != 0xFF) {
            u8* path = bl_dPath_GetRoomPath(pathNo, (s8)actor->current.roomNo);
            f32 length = actor->pathSpeed;
            actor->path = gabi::ea(path);
            if (length < 4.0f)
                actor->pathSpeed = bl_cM_rndF(2.0f) + 4.0f;
        }
        if (((s16)gabi::object_at<be<s16>>(bl_data::debugVariant))) {
            s8 room = actor->current.roomNo;
            actor->pathNo = 0;
            actor->path = gabi::ea(bl_dPath_GetRoomPath_2(0, room));
        }
    }
    BubbleModel* m = bl_getModel(actor);
    actor->cullMtx = m ? gabi::ea(&m->baseMatrix) : 0;
    actor->attention_info.flags = 0;
    bl_dBgS_Acch_Set(reinterpret_cast<u8*>(&actor->groundCollision), &actor->current.pos,
                     reinterpret_cast<cXyz*>(&actor->old.pos), actor, 1,
                     reinterpret_cast<u8*>(&actor->groundCircle), reinterpret_cast<cXyz*>(&actor->speed), 0,
                     0);
    bl_dCcD_Stts_Init(reinterpret_cast<u8*>(&actor->bodyCollisionStatus), 0x50, 1, actor);
    actor->iceState.owner = gabi::ea(actor);
    actor->max_health = 2;
    u32 morph = gabi::ea((u8*)actor->morph);
    actor->health = 2;
    actor->fireHelper.morph = morph;
    actor->fireCylinder.height = 50.0f;
    actor->fireHelper.owner = gabi::ea(actor);
    actor->fireCylinder.radius = 50.0f;
    for (u32 i = 0; i < 10; ++i) {
        actor->fireHelper.byteParameters[i] = fireByteParameters[i];
        actor->fireHelper.floatParameters[i] = fireFloatParameters[i];
    }
    s16 yaw = actor->shape_angle.y;
    actor->collisionRadius = 50.0f;
    actor->targetYaw = yaw;
    bl_dCcD_Sph_Set(reinterpret_cast<u8*>(&actor->bodyCollider), reinterpret_cast<u8*>(body_co_sph_src));
    u32 sphereFlags = actor->bodyCollider.attackFlags;
    actor->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 0x29;
    actor->bodyCollider.status = gabi::ea(actor) + 0x64C;
    actor->bodyCollider.attackFlags = sphereFlags & ~1u;
    u32 play = bl_getGameInfo();
    s32 item =
        bl_cDT_NamePTbl_GetIndex(bl_itemNameTable(&gabi::object_at<dComIfG_inf_c>(play).play), "bable_r", 0);
    type = actor->variant;
    actor->itemTableIdx = item;
    actor->gbaName = 14;
    if (type == 1) {
        actor->bodyCollider.targetHitType = 3;
        actor->bodyCollider.attackAttributes = 0x800;
        play = bl_getGameInfo();
        item = bl_cDT_NamePTbl_GetIndex(bl_itemNameTable(&gabi::object_at<dComIfG_inf_c>(play).play), "bable",
                                        0);
        actor->itemTableIdx = item;
        actor->gbaName = 15;
    }
    f32 velocity = (f32)actor->speed.y - 3.0f;
    actor->gravity = -3.0f;
    if (velocity < -55.0f)
        velocity = -55.0f;
    u32 z = bl_words(&actor->current.pos).z, x = bl_words(&actor->current.pos).x;
    actor->speed.y = velocity;
    bl_words(&actor->roamingOrigin).x = x;
    u32 y = bl_words(&actor->current.pos).y;
    actor->targetScale = 1.0f;
    bl_words(&actor->roamingOrigin).y = y;
    bl_words(&actor->roamingOrigin).z = z;
    actor->modelYOffset = 24.0f;
    bl_fopAcM_posMove(actor, reinterpret_cast<u8*>(&actor->bodyCollisionStatus));
    BG_check(actor);
    u8 flying = actor->flying;
    f32 height = actor->current.pos.y;
    u32 collisionFlags = actor->groundCollision.flags;
    actor->homeGroundHeight = height;
    if (!flying)
        actor->homeGroundHeight = (f32)actor->groundCollision.groundHeight;
    actor->action = BL_ACTION_DOUSA;
    u8 switchNo = actor->switchNo;
    if (!(collisionFlags & 0x20)) {
        height = actor->homeGroundHeight;
        actor->scale.x = 0.0f;
        actor->scale.y = 0.0f;
        actor->scale.z = 0.0f;
        actor->gravity = 0.0f;
        actor->speed.y = 0.0f;
        actor->actionStep = 4;
        actor->current.pos.y = height + 100.0f;
    } else
        actor->actionStep = 0;
    if (switchNo != 0xFF) {
        dSv_info_c* save = bl_saveInfo((u32)gabi::object_at<be<u32>>(bl_data::saveRoot));
        s8 room = ((s8)gabi::object_at<be<s8>>(bl_data::roomNo));
        if (!bl_dSv_info_c_isSwitch(reinterpret_cast<u8*>(save), switchNo, room)) {
            u32 status = actor->actor_status;
            actor->action = BL_ACTION_COME_WAIT;
            actor->actionStep = 70;
            actor->actor_status = status & ~0x20u;
        }
    }
    if (actor->variant & 0x80) {
        u32 sphere = actor->bodyCollider.correctionFlags;
        actor->max_health = 1;
        actor->burning = 1;
        actor->bodyCollider.correctionFlags = sphere | 1;
        actor->health = 1;
        s16 heading = (s16)gabi::ftoi(bl_cM_rndFX(32767.0f));
        u32 status = actor->actor_status;
        actor->action = BL_ACTION_SKULL;
        actor->current.angle.y = heading;
        actor->shape_angle.y = heading;
        u32 attention = actor->attention_info.flags;
        actor->timers[5] = 4;
        actor->actionStep = 100;
        actor->actor_status = (status | 0x4000) & ~0x20u;
        actor->attention_info.flags = attention | 0x10;
    } else {
        reinterpret_cast<be<u8>&>(actor->stolenItemCount) = 1;
        actor->stealItemLeft = 1;
    }
    draw_SUB(actor);
    return phase;
}

/* 020AD850 */
u8* constructFire(u8* fire) {
    if (!fire) {
        fire = bl_allocateBytes(0x22C);
        if (!fire)
            return nullptr;
    }
    if (gabi::ea(&gabi::object_at<BubbleFireHelper>(gabi::ea(fire)).position) == 0)
        bl_allocateVectorSubobject(12);
    bl_collisionStatusConstructor(
        reinterpret_cast<u8*>(&gabi::object_at<BubbleFireHelper>(gabi::ea(fire)).status));
    bl_dCcD_GStts_dCcD_GStts(
        reinterpret_cast<u8*>(&gabi::object_at<BubbleFireHelper>(gabi::ea(fire)).status.globalVtable));
    gabi::object_at<BubbleFireHelper>(gabi::ea(fire)).status.baseVtable = bl_data::collisionStatusVtable;
    gabi::object_at<BubbleFireHelper>(gabi::ea(fire)).status.globalVtable =
        bl_data::globalCollisionStatusVtable;
    bl_collisionSphereConstructor(
        reinterpret_cast<u8*>(&gabi::object_at<BubbleFireHelper>(gabi::ea(fire)).sphere));
    gabi::object_at<BubbleFireHelper>(gabi::ea(fire)).sphere.scale = 1.0f;
    return fire;
}

/* 020AD8DC */
bl_class* constructBubble(bl_class* actor) {
    if (!actor) {
        actor = bl_allocateActor(0xE18);
        if (!actor)
            return nullptr;
    }
    bl_fopAc_ac_c_fopAc_ac_c(actor);
    actor->__vtbl = bl_data::actorVtable;
    bl_dBgS_AcchCir_dBgS_AcchCir(reinterpret_cast<u8*>(&actor->groundCircle));
    bl_dBgS_Acch_dBgS_Acch(reinterpret_cast<u8*>(&actor->groundCollision));
    actor->groundCollision.baseVtable = bl_data::groundBaseVtable;
    actor->groundCollision.constructed = 1;
    actor->groundCollision.actorVtable = bl_data::groundActorVtable;
    actor->groundCollision.objectVtable = bl_data::groundObjectVtable;
    bl_collisionStatusConstructor(reinterpret_cast<u8*>(&actor->bodyCollisionStatus));
    bl_dCcD_GStts_dCcD_GStts(reinterpret_cast<u8*>(&actor->bodyCollisionStatus.globalVtable));
    actor->bodyCollisionStatus.baseVtable = bl_data::collisionStatusVtable;
    actor->bodyCollisionStatus.globalVtable = bl_data::globalCollisionStatusVtable;
    bl_collisionSphereConstructor(reinterpret_cast<u8*>(&actor->bodyCollider));
    bl_dPa_smokeEcallBack_dPa_smokeEcallBack(reinterpret_cast<u8*>(&actor->smokeCallback), 1);
    bl_dPa_followEcallBack_dPa_followEcallBack(reinterpret_cast<u8*>(&actor->appearanceCallback), 0, 0);
    bl_dPa_followEcallBack_dPa_followEcallBack(reinterpret_cast<u8*>(&actor->fireCallback), 0, 0);
    bl_collisionStatusConstructor(reinterpret_cast<u8*>(&actor->fireCollisionStatus));
    bl_dCcD_GStts_dCcD_GStts(reinterpret_cast<u8*>(&actor->fireCollisionStatus.globalVtable));
    actor->fireCollisionStatus.baseVtable = bl_data::collisionStatusVtable;
    actor->fireCollisionStatus.globalVtable = bl_data::globalCollisionStatusVtable;
    bl_dCcD_GObjInf_dCcD_GObjInf(reinterpret_cast<u8*>(&actor->fireCylinder));
    actor->fireCylinder.cylinderVtable = bl_data::cylinderBaseVtable;
    actor->fireCylinder.shapeVtable = bl_data::cylinderShapeVtable;
    bl_cylinderConstructor(reinterpret_cast<u8*>(&actor->fireCylinder.cylinder));
    actor->fireCylinder.correctionFlags = bl_data::collisionObjectVtable;
    actor->fireCylinder.collisionVtable = bl_data::collisionCylinderVtable;
    actor->fireCylinder.cylinderVtable = bl_data::cylinderVtable;
    bl_dBgS_AcchCir_dBgS_AcchCir(reinterpret_cast<u8*>(&actor->fireGroundCircle));
    bl_dBgS_Acch_dBgS_Acch(reinterpret_cast<u8*>(&actor->fireGroundCollision));
    actor->fireGroundCollision.baseVtable = bl_data::groundBaseVtable;
    actor->fireGroundCollision.objectVtable = bl_data::groundObjectVtable;
    actor->fireGroundCollision.actorVtable = bl_data::groundActorVtable;
    actor->fireGroundCollision.constructed = 1;
    constructFire(reinterpret_cast<u8*>(&actor->fireHelper));
    bl_invisibleModelConstructor(reinterpret_cast<u8*>(&actor->lightInfo));
    return actor;
}

// HD: compiler-generated constructors, deleting destructors and TU initialization.
/* 020ADF84 */
void initializeStatics() {
    gabi::object_at<be<u32>>(bl_data::staticVectorZ) = 0;
    gabi::object_at<be<u32>>(bl_data::staticVectorX) = 0;
    gabi::object_at<be<u32>>(bl_data::staticVectorExtra) = 0;
    gabi::object_at<be<u32>>(bl_data::staticVectorY) = 0;
    bl_register_global_object(gabi::at<u8>(bl_data::angleDestructorRecord));
    gabi::object_at<be<f32>>(bl_data::negativePi) = -3.1415927410125732f;
    gabi::object_at<be<f32>>(bl_data::positivePi) = 3.1415927410125732f;
    bl_fixedVectorConstructor(gabi::at<u8>(bl_data::fixedVectorStatic));
    bl_register_global_object(gabi::at<u8>(bl_data::fixedVectorDestructorRecord));
    bl_vectorConstructor(gabi::at<u8>(bl_data::vectorStatic));
    bl_register_global_object(gabi::at<u8>(bl_data::vectorDestructorRecord));
}
/* 020AE018 */
void deleteStatic(u8* object, u32 flags) {
    if (object && (flags & 1))
        bl_dl(object);
}
/* 020AE02C */
void destructBubble(bl_class* actor, u32 flags) {
    if (!actor)
        return;
    bl_invisibleModelDestructor(reinterpret_cast<u8*>(&actor->lightInfo), 2);
    bl_collisionSphereDestructor(reinterpret_cast<u8*>(&actor->fireHelper.sphere), 2);
    bl_collisionStatusDestructor(reinterpret_cast<u8*>(&actor->fireHelper.status), 2);
    actor->fireGroundCollision.actorVtable = bl_data::groundActorVtable;
    actor->fireGroundCollision.objectVtable = bl_data::groundObjectVtable;
    bl_groundCollisionDestructor(reinterpret_cast<u8*>(&actor->fireGroundCollision), 0);
    bl_groundCircleDestructor(reinterpret_cast<u8*>(&actor->fireGroundCircle.circle), 2);
    bl_dBgS_Acch_dBgS_Acch_2(reinterpret_cast<u8*>(&actor->fireCylinder), 2);
    bl_collisionStatusDestructor(reinterpret_cast<u8*>(&actor->fireCollisionStatus), 2);
    bl_collisionSphereDestructor(reinterpret_cast<u8*>(&actor->bodyCollider), 2);
    bl_collisionStatusDestructor(reinterpret_cast<u8*>(&actor->bodyCollisionStatus), 2);
    actor->groundCollision.actorVtable = bl_data::groundActorVtable;
    actor->groundCollision.objectVtable = bl_data::groundObjectVtable;
    bl_groundCollisionDestructor(reinterpret_cast<u8*>(&actor->groundCollision), 0);
    bl_groundCircleDestructor(reinterpret_cast<u8*>(&actor->groundCircle.circle), 2);
    bl_fopAc_ac_c_fopAc_ac_c_2(actor, 0);
    if (flags & 1)
        bl_dl_2(actor);
}
/* 020AE134 */
void emptyVirtual(bl_class* actor) {}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data body_co_sph_src 101918E4
data fireFloatParameters 10191924
data fireByteParameters 1019194C
str 10009A70 in 020ADA5C
str 10009A68 in 020ADA5C
str 10009A78 in 020ADA5C
str 10009A54 in 020AD59C
str 10009A57 in 020AD61C
str 100099A8 in 020A9384
str 10009984 in 020A8BDC
str 1000997C in 020A8BDC
ext 0200FAD8 void bl_MtxTrans(int, float, float, float)
ext 025F1C28 void bl_mDoMtx_YrotM(unsigned char *, short)
ext 025F1BF4 void bl_mDoMtx_XrotM(unsigned char *, short)
ext 025F1C5C void bl_mDoMtx_ZrotM(unsigned char *, short)
ext 025E55A0 void bl_mDoExt_McaMorf_calc(unsigned char *)
ext 02041570 void bl_enemy_fire(unsigned char *)
ext 02555D0C dScnKy_env_light_c* bl_getEnvLight()
ext 025626A4 void bl_dScnKy_env_light_c_settingTevStruct(unsigned char *, int, cXyz *, unsigned char *)
ext 02562F5C void bl_setLightTevColorType(unsigned char *, unsigned char *, unsigned char *)
ext 025BA0C0 int bl_dSv_info_c_isSwitch(unsigned char *, unsigned char, signed char)
ext 025200D4 unsigned int bl_getGameInfo()
ext 025BED80 void bl_dSnap_RegistFig(int, bl_class *, float, float, float)
ext 0259138C void bl_dMat_ice_c_entryDL(unsigned char *, int, unsigned char *)
ext 025E7FC4 void bl_mDoExt_btkAnm_entry(unsigned char *, unsigned char *, float)
ext 025200D4 void bl_touchGameInfo()
ext 025E5590 void bl_mDoExt_McaMorf_entryDL(unsigned char *)
ext 025A847C void bl_dPa_control_c_set(unsigned char *, int, int, cXyz *, csXyz *, int, int, unsigned char *, signed char, int, int, int)
ext 025A847C void bl_dPa_control_c_set_2(unsigned char *, int, unsigned int, cXyz *, int, int, int, unsigned char *, signed char, int, int, int)
ext 028245AC void bl_JPAGetXYZRotateMtx(int, short, int, unsigned char *)
ext 025A5AC8 void bl_dPa_followEcallBack_end(unsigned char *)
ext 028249B0 void bl_JPASetRMtxTVecfromMtx(unsigned char *, unsigned char *, unsigned char *)
ext 02520540 int bl_dComIfGp_getReverb(signed char)
ext 025E1A40 void bl_seStartFull_1(int, cXyz *, int, int)
ext 028E90D4 void bl_PSMTXCopy(unsigned char *, unsigned char *)
ext 028F4384 float bl_sqrtf(float)
ext 026066C4 unsigned char * bl_dRes_control_c_getRes(unsigned char *, SafeString *, int)
ext 025E4A98 void bl_mDoExt_McaMorf_setAnm(unsigned char *, unsigned char *, unsigned char, unsigned char *, float, float, float, float)
ext 02515E50 void bl_dCcD_GStts_Move(unsigned char *)
ext 025162A4 int bl_dCcD_GObjInf_ChkTgHit(unsigned char *)
ext 02516300 unsigned int bl_dCcD_GObjInf_GetTgHitObj(unsigned char *)
ext 025D6894 int bl_fopAcM_searchActorAngleY(bl_class *, unsigned char *)
ext 025E1A40 void bl_seStartFull_2(unsigned int, cXyz *, unsigned int, int)
ext 025E1AA4 void bl_seStartLevelFull_1(int, cXyz *, unsigned int, int, int)
ext 025A847C void bl_dPa_control_c_set_3(unsigned char *, int, unsigned int, cXyz *, void *, cXyz *, int, int, int, int, int, int)
ext 025192A8 void bl_cc_at_check(bl_class *, AttackInfo *)
ext 02041C30 void bl_enemy_fire_remove(unsigned char *)
ext 025E1A40 void bl_seStartFull_3(int, cXyz *, unsigned int, int)
ext 0200ED84 void bl_cLib_addCalc2(gabi::be<float> *, float, float, float)
ext 024EFF44 void bl_dBgS_AcchCir_SetWall(unsigned char *, float, float)
ext 024F08A8 void bl_dBgS_Acch_CrrPos(unsigned char *, unsigned char *)
ext 0200F428 void bl_cLib_addCalcAngleS2(gabi::be<short> *, int, int, int)
ext 02008FEC void bl_cBgS_LinChk_ct(LineCheck *)
ext 02019918 float bl_cM_rndFX(float)
ext 025F1884 void bl_mDoMtx_YrotS(unsigned char *, short)
ext 0200FCD8 void bl_MtxPosition(cXyz *, cXyz *)
ext 028E8D88 void bl_PSVECAdd(cXyz *, cXyz *, cXyz *)
ext 024F1AFC void bl_dBgS_LinChk_Set(LineCheck *, cXyz *, cXyz *, bl_class *)
ext 02008860 int bl_cBgS_LineCross(unsigned char *, LineCheck *)
ext 02008B4C void bl_lineCheckDestructor(LineCheck *, int)
ext 025E1AA4 void bl_seStartLevelFull_2(unsigned int, unsigned char *, unsigned int, int, int)
ext 025D68EC float bl_fopAcM_searchActorDistance(bl_class *, unsigned char *)
ext 025A847C void bl_dPa_control_c_set_4(unsigned char *, int, int, cXyz *, int, int, int, unsigned char *, int, int, int, int)
ext 0200ED84 void bl_cLib_addCalc2_2(unsigned char *, float, float, float)
ext 027F2BF8 int bl_J3DFrameCtrl_checkPass(unsigned char *, float)
ext 020195B0 short bl_cM_atan2s(float, float)
ext 020198D8 float bl_cM_rndF(float)
ext 020195B0 int bl_cM_atan2s_2(float, float)
ext 0200F428 void bl_cLib_addCalcAngleS2_2(gabi::be<short> *, short, int, int)
ext 0200EDC8 void bl_cLib_addCalc0(gabi::be<float> *, float, float)
ext 025160DC int bl_dCcD_GObjInf_ChkAtHit(unsigned char *)
ext 02515BBC unsigned int bl_dCcD_GAtTgCoCommonBase_GetAc(unsigned char *)
ext 025D99E8 void bl_fopAcM_createDisappear(bl_class *, cXyz *, int, int, int)
ext 025BA5D4 void bl_dSv_info_c_onActor(unsigned char *, unsigned short, signed char)
ext 025D57E0 void bl_fopAcM_delete(bl_class *)
ext 02019788 float bl_cM_rnd()
ext 025A847C unsigned int bl_dPa_control_c_set_5(unsigned char *, int, int, cXyz *, csXyz *, cXyz *, int, int, signed char, unsigned char *, unsigned char *, int)
ext 0251621C void bl_dCcD_GObjInf_ClrTgHit(unsigned char *)
ext 025A5F88 void bl_dPa_smokeEcallBack_end(unsigned char *)
ext 025D8870 void bl_fopAcM_createItem(cXyz *, int, int, int, int, int, int, int)
ext 025E742C void bl_mDoExt_baseAnm_play(unsigned char *)
ext 020402C8 int bl_enemy_ice(unsigned char *)
ext 024EECAC unsigned int bl_dBgS_GetMtrlSndId(unsigned char *, unsigned char *)
ext 025E535C void bl_mDoExt_McaMorf_play(unsigned char *, unsigned char *, unsigned int, int)
ext 02018D40 void bl_cM3dGSph_SetC(unsigned char *, cXyz *)
ext 02018C8C void bl_cM3dGSph_SetR(unsigned char *, float)
ext 0200E240 void bl_cCcS_Set(unsigned char *, unsigned char *)
ext 025D6800 void bl_fopAcM_posMove(bl_class *, unsigned char *)
ext 02008254 int bl_cBgS_ChkPolySafe(unsigned char *, unsigned char *)
ext 024EF0BC int bl_dBgS_GetGroundCode(unsigned char *, unsigned char *)
ext 025E4F64 unsigned char * bl_mDoExt_McaMorf_mDoExt_McaMorf(int, unsigned char *, int, int, unsigned char *, int, int, int, int, int, int, int, float)
ext 0273AD10 unsigned char * bl_allocateBytes(int)
ext 025E7C6C unsigned char * bl_mDoExt_btkAnm_init(unsigned char *)
ext 025E7CE0 int bl_textureAnimationInit(unsigned char *, unsigned char *, unsigned char *, int, int, int, int, int, int, float)
ext 025E8A48 int bl_invisibleModelCreate(unsigned char *, unsigned char *)
ext 025204C8 void bl_dComIfG_resDelete(request_of_phase_process_class *, const char *)
ext 0273AD10 void bl_allocateVectorSubobject(int)
ext 0200BD2C void bl_collisionStatusConstructor(unsigned char *)
ext 02515DA0 void bl_dCcD_GStts_dCcD_GStts(unsigned char *)
ext 025166F0 void bl_collisionSphereConstructor(unsigned char *)
ext 0273AD10 bl_class * bl_allocateActor(int)
ext 025D4ED0 void bl_fopAc_ac_c_fopAc_ac_c(bl_class *)
ext 024EFE94 void bl_dBgS_AcchCir_dBgS_AcchCir(unsigned char *)
ext 024F0474 void bl_dBgS_Acch_dBgS_Acch(unsigned char *)
ext 025A5B18 void bl_dPa_smokeEcallBack_dPa_smokeEcallBack(unsigned char *, int)
ext 025A5894 void bl_dPa_followEcallBack_dPa_followEcallBack(unsigned char *, int, int)
ext 02515FB8 void bl_dCcD_GObjInf_dCcD_GObjInf(unsigned char *)
ext 02018590 void bl_cylinderConstructor(unsigned char *)
ext 025E895C void bl_invisibleModelConstructor(unsigned char *)
ext 02520460 int bl_dComIfG_resLoad(request_of_phase_process_class *, const char *)
ext 025D63E8 int bl_fopAcM_entrySolidHeap(bl_class *, BubbleHeapCallback, int)
ext 025AAF88 unsigned char * bl_dPath_GetRoomPath(unsigned char, signed char)
ext 025AAF88 unsigned char * bl_dPath_GetRoomPath_2(int, signed char)
ext 024F06B4 void bl_dBgS_Acch_Set(unsigned char *, cXyz *, cXyz *, bl_class *, int, unsigned char *, cXyz *, int, int)
ext 02515F14 void bl_dCcD_Stts_Init(unsigned char *, int, int, bl_class *)
ext 0251677C void bl_dCcD_Sph_Set(unsigned char *, unsigned char *)
ext 0200E814 int bl_cDT_NamePTbl_GetIndex(unsigned char *, const char *, int)
ext 028F026C void bl_register_global_object(unsigned char *)
ext 028ED6F8 void bl_fixedVectorConstructor(unsigned char *)
ext 028EAB2C void bl_vectorConstructor(unsigned char *)
ext 0273AF40 void bl_dl(unsigned char *)
ext 025E89F8 void bl_invisibleModelDestructor(unsigned char *, int)
ext 02515AE8 void bl_collisionSphereDestructor(unsigned char *, int)
ext 02515860 void bl_collisionStatusDestructor(unsigned char *, int)
ext 024EFD9C void bl_groundCollisionDestructor(unsigned char *, int)
ext 02018034 void bl_groundCircleDestructor(unsigned char *, int)
ext 02515A70 void bl_dBgS_Acch_dBgS_Acch_2(unsigned char *, int)
ext 025D50BC void bl_fopAc_ac_c_fopAc_ac_c_2(bl_class *, int)
ext 0273AF40 void bl_dl_2(bl_class *)
*/
// clang-format on
