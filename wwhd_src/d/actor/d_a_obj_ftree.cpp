/* Forest trees. Verified WWHD implementation with GameCube actions and explicit HD rendering. */
#include "d/actor/d_a_obj_ftree.h"
#include "bindings.h"
using daObjFtree::Act_c;
using enum daObjFtree::Mode;

// HD: verified collision and eight-byte GHS action records.
static u32 ftree_M_cyl_srcW[17] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000100, 0x0000000F,
                                   0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
                                   0x00000000, 0x00000000, 0x00000000, 0x42C80000, 0x43480000};
static u32 ftree_M_cyl_src[17] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFF1DFEFF, 0x0000000F,
                                  0x00000079, 0x00000000, 0x00000000, 0x00000000, 0x00000003, 0x00000000,
                                  0x00000000, 0x00000000, 0x00000000, 0x42C80000, 0x43480000};
static u32 M_action_init[26] = {0x0000FFFF, 0x023485C0, 0x0000FFFF, 0x023485D4, 0x0000FFFF, 0x02348884,
                                0x0000FFFF, 0x02348BBC, 0x0000FFFF, 0x02348CF8, 0x0000FFFF, 0x0234915C,
                                0x0000FFFF, 0x02349574, 0x0000FFFF, 0x02348F20, 0x0000FFFF, 0x02348E58,
                                0x0000FFFF, 0x0234903C, 0x0000FFFF, 0x0234988C, 0x0000FFFF, 0x023499CC,
                                0x0000FFFF, 0x02349AFC};
static u32 M_action_main[26] = {0x0000FFFF, 0x02349DF0, 0x0000FFFF, 0x02348718, 0x0000FFFF, 0x02348A38,
                                0x0000FFFF, 0x02348BF8, 0x0000FFFF, 0x023490FC, 0x0000FFFF, 0x02349238,
                                0x0000FFFF, 0x023495C0, 0x0000FFFF, 0x02349098, 0x0000FFFF, 0x02348EC0,
                                0x0000FFFF, 0x023490F8, 0x0000FFFF, 0x0234991C, 0x0000FFFF, 0x02349A20,
                                0x0000FFFF, 0x02349B50};
// The recompiled fmul selects the first NaN operand's payload before rounding.
static f32 scaledComponent(f32 component, f32 scale) {
    if (std::isnan(component))
        return component;
    if (std::isnan(scale))
        return scale;
    return f32(component * scale);
}
static void initTreeMap() {
    if (!static_cast<u32>(ftree_treeMapInitialized())) {
        ftree_treeMapInitialized() = 1;
        ftree_treeMapSize() = 10;
    }
}

/* 02347B7C */
u32 daObjFtree::Act_c::launch_heart_part() {

    gabi::Local<cXyz> position, scale;
    gabi::Local<be<u32>[8]> positionLinkage;
    gabi::Local<csXyz> angles;
    gabi::Local<be<u32>[8]> anglesLinkage;
    position->y = static_cast<f32>(this->current.pos.y);
    position->x = static_cast<f32>(this->current.pos.x);
    scale->y = 1.0f;
    angles->x = static_cast<u16>(this->shape_angle.x);
    angles->z = static_cast<u16>(this->shape_angle.z);
    angles->y = static_cast<u16>(this->shape_angle.y);
    scale->x = 1.0f;
    scale->z = 1.0f;
    position->z = static_cast<f32>(this->current.pos.z);
    ftree_getPlay_2();
    angles->y = s16(u16(s16(angles->y)) + 9000);
    position->y = f32(f32(position->y) + 860.0f);
    u32 item =
        ftree_fopAcM_fastCreateItem(position.get(), 7, static_cast<s8>(this->current.roomNo), angles.get(),
                                    scale.get(), -1, 0, 1.75f, 30.0f, -2.0999999046325684f);
    if (!item)
        return 0;
    ftree_baseActor(item).parentActorID = static_cast<u32>(this->mBsPcId);
    initTreeMap();
    s32 tree = static_cast<s32>(this->param_get_tree_idx(4, 0));
    u32 id = tree < 10 ? static_cast<u8>(ftree_treeId(u32(tree))) : 15;
    ftree_dSv_event_c__setEventReg_2(static_cast<void*>(dComIfGs_getEvent()), 0x9B07, u8(id));
    ftree_baseActor(item).actor_status = 0x4040;
    this->heartPlaced = 1;
    return 1;
}

/* 02347924 */
u32 daObjFtree::Act_c::place_heart_part() {
    if (static_cast<s16>(this->m356) != -1)
        return 0;
    if (ftree_dSv_event_c__isEventBit_2(static_cast<void*>(dComIfGs_getEvent()), 0x102) != 1)
        return 0;
    if (ftree_dSv_event_c__isEventBit_2(static_cast<void*>(dComIfGs_getEvent()), 0x2E20))
        return 0;
    initTreeMap();
    s32 tree = static_cast<s32>(this->param_get_tree_idx(4, 0));
    u32 id = tree < 10 ? static_cast<u8>(ftree_treeId(u32(tree))) : 15;
    if (ftree_dSv_event_c__getEventReg_2(static_cast<void*>(dComIfGs_getEvent()), 0x9B07) != id)
        return 0;

    gabi::Local<cXyz> source, position, scale;
    gabi::Local<be<u32>[8]> sourceLinkage;
    source->x = 0.0f;
    s16 angle = s16(u16(static_cast<s16>(this->shape_angle.y)) + 9000);
    f32 z = static_cast<f32>(this->current.pos.z), x = static_cast<f32>(this->current.pos.x),
        y = static_cast<f32>(this->current.pos.y);
    source->z = 310.0f;
    source->y = 1.0f;
    ftree_PSMTXTrans(static_cast<void*>(&ftree_workMatrix()), x, y, z);
    ftree_mDoMtx_YrotM(static_cast<void*>(&ftree_workMatrix()), angle);
    gabi::Local<Mtx34> matrix;
    gabi::Local<be<u32>[8]> matrixLinkage;
    ftree_PSMTXCopy_2(static_cast<void*>(&ftree_workMatrix()), matrix.get());
    ftree_PSMTXMultVec(matrix.get(), source.get(), position.get());
    scale->y = 1.0f;
    scale->x = 1.0f;
    scale->z = 1.0f;
    gabi::Local<csXyz> angles;
    gabi::Local<be<u32>[8]> anglesLinkage;
    ftree_csXyz__csXyz_3(angles.get(), 0, 0, 0);
    u32 item =
        ftree_fopAcM_fastCreateItem(position.get(), 7, static_cast<s8>(this->current.roomNo), angles.get(),
                                    scale.get(), -1, 0, 0.0f, 0.0f, -2.0999999046325684f);
    if (!item)
        return 0;
    ftree_baseActor(item).parentActorID = static_cast<u32>(this->mBsPcId);
    initTreeMap();
    tree = static_cast<s32>(this->param_get_tree_idx(4, 0));
    id = tree < 10 ? static_cast<u8>(ftree_treeId(u32(tree))) : 15;
    ftree_dSv_event_c__setEventReg_2(static_cast<void*>(dComIfGs_getEvent()), 0x9B07, u8(id));
    return 1;
}

/* 02346A40 */
u32 daObjFtree::Act_c::estimate_water() {

    ftree_getPlay_2();
    gabi::Local<cXyz> source, result;
    gabi::Local<be<u32>[8]> sourceLinkage;
    source->x = 0.0f;
    source->z = 50.0f;
    source->y = 0.0f;
    u32 play = ftree_getPlay(), player = static_cast<u32>(ftree_play(play).player);
    f32 playerX = static_cast<f32>(ftree_baseActor(player).current.pos.x),
        actorX = static_cast<f32>(this->current.pos.x), actorZ = static_cast<f32>(this->current.pos.z);
    f32 playerZ = static_cast<f32>(ftree_baseActor(player).current.pos.z);
    s32 angle = ftree_cM_atan2s(f32(actorX - playerX), f32(actorZ - playerZ));
    f32 y = static_cast<f32>(ftree_baseActor(player).current.pos.y),
        z = static_cast<f32>(ftree_baseActor(player).current.pos.z),
        x = static_cast<f32>(ftree_baseActor(player).current.pos.x);
    ftree_PSMTXTrans(static_cast<void*>(&ftree_workMatrix()), x, y, z);
    ftree_mDoMtx_YrotM(static_cast<void*>(&ftree_workMatrix()), angle);
    gabi::Local<Mtx34> matrix;
    gabi::Local<be<u32>[8]> matrixLinkage;
    ftree_PSMTXCopy_2(static_cast<void*>(&ftree_workMatrix()), matrix.get());
    ftree_PSMTXMultVec(matrix.get(), source.get(), result.get());
    f32 distance = ftree_PSVECSquareDistance(result.get(), static_cast<void*>(&this->current.pos));
    f32 radius = static_cast<f32>(this->m4C8) + 20.0f;
    return !(distance > f32(radius * radius));
}

/* 02346C18 */
s32 daObjFtree::Act_c::XyEventCallBack(s32 index) {
    u32 play = ftree_getPlay();
    if (static_cast<u8>(ftree_play(play).eventState[u32(index)]) == 0x59 &&
        !static_cast<s32>(this->is_brought()) && static_cast<s32>(this->estimate_water())) {
        s32 last = static_cast<s32>(this->iam_last());
        if (static_cast<s32>(
                this->XyEvent_set(static_cast<s16>(last ? this->mEventCam2Idx : this->mEventCam1Idx)))) {
            if (last)
                ftree_dSv_event_c__onEventBit(static_cast<void*>(dComIfGs_getEvent()), 0x102);
            this->eventTimer = 140;
            return static_cast<s16>(last ? this->mEventCam2Idx : this->mEventCam1Idx);
        }
    }
    s32 result = -1;
    if (static_cast<s32>(this->XyEvent_set(static_cast<s16>(this->mEventCam0Idx))))
        result = static_cast<s16>(this->mEventCam0Idx);
    return result;
}

/* 023469BC */
void daObjFtree::Act_c::XyEvent_init() {
    this->m356 = -1;
    this->m358 = 0;
}

/* 02346A10 */
u32 daObjFtree::Act_c::XyEvent_set(s32 event) {
    if (static_cast<s16>(this->m358) != 0 || event == -1)
        return 0;
    this->m356 = s16(event);
    this->m358 = 1;
    return 1;
}

/* 023477AC */
void daObjFtree::Act_c::XyEvent_exe() {
    s16 state = static_cast<s16>(this->m358);
    if (state == 1) {
        if (static_cast<u16>(this->eventInfo.mCommand) != 1)
            return;
        this->m358 = 2;
        u32 play = ftree_getPlay(), player = static_cast<u32>(ftree_play(play).player);

        gabi::Local<cXyz> source, result;
        gabi::Local<be<u32>[8]> sourceLinkage;
        source->x = 0.0f;
        s16 angle = s16(u16(static_cast<s16>(this->shape_angle.y)) + 9000);
        f32 z = static_cast<f32>(this->current.pos.z), x = static_cast<f32>(this->current.pos.x),
            y = static_cast<f32>(this->current.pos.y);
        source->y = 1.0f;
        source->z = 108.0f;
        ftree_PSMTXTrans(static_cast<void*>(&ftree_workMatrix()), x, y, z);
        ftree_mDoMtx_YrotM(static_cast<void*>(&ftree_workMatrix()), angle);
        gabi::Local<Mtx34> matrix;
        gabi::Local<be<u32>[8]> matrixLinkage;
        ftree_PSMTXCopy_2(static_cast<void*>(&ftree_workMatrix()), matrix.get());
        ftree_PSMTXMultVec(matrix.get(), source.get(), result.get());
        u32 target = static_cast<u32>(
            gabi::object_at<FtreePlayerVtable>(static_cast<u32>(ftree_baseActor(player).__vtbl)).prepareTalk);
        gabi::call_ptr<void>(target, static_cast<void*>(&ftree_baseActor(player)), result.get(),
                             s16(u16(angle) + 0x8000));
    } else if (state == 2) {
        s16 event = static_cast<s16>(this->m356);
        u32 play = ftree_getPlay();
        if (ftree_dEvent_manager_c__endCheck(ftree_eventManager(play), event)) {
            play = ftree_getPlay();
            u16 flags = static_cast<u16>(ftree_play(play).eventFlags);
            ftree_play(play).eventFlags = flags | 8;
            this->XyEvent_init();
        }
    }
}

/* 02349DFC */
u32 daObjFtree::Act_c::param_get_tree_idx(u32 width, u32 shift) {
    u32 value = static_cast<u32>(this->mParameters);
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    return ((shift & 32) ? 0 : value >> (shift & 31)) & (mask - 1);
}

/* 02348394 */
u32 daObjFtree::Act_c::SetJointAnimation(s32 animation, f32 rate, f32 morph, s32 endOffset) {

    gabi::Local<SafeString> name;
    gabi::Local<be<u32>[8]> nameLinkage;
    name->mStringTop = ftree_tu::archiveName;
    name->__vtbl = ftree_tu::safeString;
    u32 resource = ftree_dRes_control_c__getRes(ftree_resourceManager(), name.get(), animation);
    if (endOffset > 0)
        endOffset = 0;
    if (!resource)
        return 0;
    f32 end = -1.0f;
    if (endOffset) {
        u32 target = static_cast<u32>(gabi::object_at<FtreeResourceVtable>(
                                          static_cast<u32>(gabi::object_at<FtreeResource>(resource).vtable))
                                          .getEndFrame);
        s32 frames =
            gabi::call_ptr<s32>(target, static_cast<void*>(&gabi::object_at<FtreeResource>(resource)));
        end = f32(f32(frames) + f32(endOffset));
    }
    ftree_mDoExt_McaMorf__setAnm(static_cast<void*>(&ftree_morph(static_cast<u32>(this->mpMorf))),
                                 static_cast<void*>(&gabi::object_at<FtreeResource>(resource)), 0, morph,
                                 rate, 0.0f, end, 0);
    return 1;
}

/* 0234855C */
u32 daObjFtree::Act_c::PlayStopJointAnimation() {
    return ftree_mDoExt_McaMorf__play(static_cast<void*>(&ftree_morph(static_cast<u32>(this->mpMorf))), 0, 0,
                                      0) != 0;
}

/* 02348594 */
u32 daObjFtree::Act_c::PlayStopColorAnimation() {
    return ftree_mDoExt_baseAnm__play(static_cast<void*>(&this->mBrkAnm)) != 0;
}

/* 023465A0 */
void daObjFtree::Act_c::set_first_stat() {
    if (!ftree_dSv_player_collect_c__isSymbol(ftree_saveCollect(), 2)) {
        this->process_init(MODE_NONE, 0);
        return;
    }
    if (ftree_dSv_event_c__isEventBit(static_cast<void*>(dComIfGs_getEvent()), 0x102)) {
        this->process_init(MODE_WAIT_L, 0);
        this->largeScale = 1.0f;
        return;
    }
    s32 brought = static_cast<s32>(this->is_brought());
    f32 size = static_cast<f32>(ftree_minimumScale());
    this->process_init(brought ? MODE_WAIT_M : MODE_WAIT_S, 0);
    this->largeScale = size;
}

/* 023474F4 */
void daObjFtree::Act_c::set_collision() {
    if (u8(this->smallVisible) != 1 && u8(this->largeVisible) != 1)
        return;
    s32 hit = ftree_dCcD_GObjInf__ChkTgHit(static_cast<void*>(&this->m508));
    u8 joint = this->smallVisible;
    if (hit) {
        ftree_daObj__HitSeStart(static_cast<void*>(&this->current.pos), static_cast<s8>(this->current.roomNo),
                                static_cast<void*>(&this->m508), joint == 1 ? 8 : 7);
        if (joint != 1)
            ftree_daObj__HitEff_kikuzu(this, static_cast<void*>(&this->m508));
        gabi::Local<cXyz> position;
        gabi::Local<be<u32>[8]> positionLinkage;
        position->y = static_cast<f32>(this->current.pos.y);
        position->z = static_cast<f32>(this->current.pos.z);
        position->x = static_cast<f32>(this->current.pos.x);
        ftree_dKy_Sound_set(position.get(), 4, static_cast<u32>(this->mBsPcId), 100);
        ftree_dCcD_GObjInf__ClrTgHit(static_cast<void*>(&this->m508));
        return;
    }
    if (joint == 1) {
        gabi::Local<cXyz> center;
        gabi::Local<be<u32>[8]> centerLinkage;
        center->x = static_cast<f32>(this->current.pos.x);
        center->y = f32(static_cast<f32>(this->current.pos.y) - 50.0f);
        center->z = static_cast<f32>(this->current.pos.z);
        ftree_cM3dGCyl__SetR(static_cast<void*>(&this->m398.mCyl), 61.0f);
        ftree_cM3dGCyl__SetH(static_cast<void*>(&this->m398.mCyl), 96.0f);
        ftree_cM3dGCyl__SetC(static_cast<void*>(&this->m398.mCyl), center.get());
        u32 play = ftree_getPlay();
        ftree_cCcS__Set(ftree_collisionServer(play), static_cast<void*>(&this->m398));
        this->m4C8 = 61.0f;
        ftree_cM3dGCyl__SetR(static_cast<void*>(&this->m508.mCyl), 61.0f);
        ftree_cM3dGCyl__SetH(static_cast<void*>(&this->m508.mCyl), 96.0f);
        ftree_cM3dGCyl__SetC_2(static_cast<void*>(&this->m508.mCyl), static_cast<void*>(&this->current.pos));
        play = ftree_getPlay();
        ftree_cCcS__Set(ftree_collisionServer(play), static_cast<void*>(&this->m508));
    } else if (u8(this->largeVisible) == 1) {
        f32 radius = f32(132.0f * static_cast<f32>(this->largeScale));
        this->m4C8 = radius;
        ftree_cM3dGCyl__SetR(static_cast<void*>(&this->m508.mCyl), radius);
        ftree_cM3dGCyl__SetH(static_cast<void*>(&this->m508.mCyl),
                             f32(950.0f * static_cast<f32>(this->largeScale)));
        ftree_cM3dGCyl__SetC_2(static_cast<void*>(&this->m508.mCyl), static_cast<void*>(&this->current.pos));
        u32 play = ftree_getPlay();
        ftree_cCcS__Set(ftree_collisionServer(play), static_cast<void*>(&this->m508));
    }
}

/* 023466E8 */
void daObjFtree::Act_c::talk_ct() {
    f32 x = static_cast<f32>(this->current.pos.x), y = static_cast<f32>(this->current.pos.y),
        z = static_cast<f32>(this->current.pos.z);
    f32 talkY = y, attentionY = y;
    u32 mode = static_cast<u32>(this->mMode);
    this->messageId = 0xFFFFFFFF;
    if (mode < MODE_COUNT) {
        u32 message = static_cast<u32>(ftree_message(mode));
        if (message == 0x149F) {
            talkY = f32(talkY + 80.0f);
            attentionY = f32(attentionY + 105.0f);
            this->attention_info.flags = 0x0800000A;
        } else if (message == 0x14A0) {
            talkY = f32(talkY + 90.0f);
            attentionY = f32(attentionY + 195.0f);
            this->attention_info.flags = 0x0800000A;
        } else if (message == 0x14A1) {
            talkY = f32(talkY + 250.0f);
            attentionY = f32(attentionY + 200.0f);
            this->attention_info.flags = 0x08000008;
        }
    }
    this->attention_info.position.x = x;
    this->eyePos.y = talkY;
    this->eyePos.z = z;
    this->eyePos.x = x;
    this->attention_info.position.z = z;
    this->attention_info.position.y = attentionY;
}
/* 023472BC */
void daObjFtree::Act_c::talk_main() {
    u32 messageObject = static_cast<u32>(ftree_messageObject());
    if (static_cast<s16>(this->m356) == -1 && static_cast<u16>(this->eventInfo.mCommand) == 1) {
        if (static_cast<u32>(this->messageId) == 0xFFFFFFFF) {
            u32 mode = static_cast<u32>(this->mMode);
            u32 message = static_cast<u32>(ftree_message(mode));
            this->messageId = ftree_fopMsgM_messageSet(ftree_messagePointer(messageObject), message,
                                                       static_cast<void*>(&this->eyePos));
            return;
        }
        u32 status = ftree_fopMsgM_SearchByID(ftree_messagePointer(messageObject));
        if (status == 14)
            ftree_setMessageStatus(ftree_messagePointer(messageObject), 16);
        else if (status == 18) {
            ftree_setMessageStatus(ftree_messagePointer(messageObject), 19);
            u32 play = ftree_getPlay();
            ftree_play(play).eventFlags = static_cast<u16>(ftree_play(play).eventFlags) | 8;
            this->talk_ct();
        }
        return;
    }
    u32 mode = static_cast<u32>(this->mMode);
    if (mode >= MODE_COUNT || !static_cast<u32>(ftree_message(mode)))
        return;
    u32 play = ftree_getPlay(), player = static_cast<u32>(ftree_play(play).player);
    f32 distance = ftree_PSVECSquareDistance_2(static_cast<void*>(&this->current.pos),
                                               static_cast<void*>(&ftree_baseActor(player).current.pos));
    f32 radius = f32(static_cast<f32>(this->m4C8) + 130.0f);
    if (distance > f32(radius * radius))
        return;
    this->talk_ct();
    play = ftree_getPlay();
    player = static_cast<u32>(ftree_play(play).player);
    u16 bits = 0x20;
    if (player) {
        gabi::Local<cXyz> difference;
        // HD keeps the result above outgoing linkage (SP+8); aligned Local storage reserves 16 bytes.
        gabi::Local<be<u32>[4]> outgoingLinkage;
        ftree_cXyz____mi(static_cast<void*>(&this->current.pos), difference.get(),
                         static_cast<void*>(&ftree_baseActor(player).current.pos));
        s32 angle = ftree_cM_atan2s(f32(difference->x), f32(difference->z));
        s32 delta = s16(u32(angle) - u16(static_cast<s16>(ftree_baseActor(player).shape_angle.y)));
        if ((delta < 0 ? -delta : delta) <= 0x4000)
            bits = 0x21;
    }
    this->eventInfo.mCondition = static_cast<u16>(this->eventInfo.mCondition) | bits;
}

// HD: material colors come from relative shader-name and color-parameter tables.
/* 02345ED8 */
void daObjFtree::Act_c::get_tev_material0_color(void* modelData, u32 colorSlot, be<s16>* red, be<s16>* green,
                                                be<s16>* blue) {
    u32 model = gabi::ea(modelData),
        material = static_cast<u32>(gabi::object_at<FtreeMaterialList>(
                                        static_cast<u32>(gabi::object_at<FtreeMaterialRoot>(
                                                             static_cast<u32>(ftree_model(model).modelData))
                                                             .listObject))
                                        .material);
    if (!material)
        return;
    u16 materialIndex =
        static_cast<u16>(gabi::object_at<FtreeMaterialDescription>(
                             static_cast<u32>(gabi::object_at<FtreeMaterial>(material).description))
                             .index);
    u32 materialEntry = static_cast<u32>(ftree_model(model).materialEntries) + materialIndex * 60;
    gabi::Local<FtreeColorNameBuffer> buffer;
    // HD places this object at SP+0x10, above outgoing linkage saved by the formatter.
    gabi::Local<be<u32>[4]> linkage;
    buffer->size = 32;
    buffer->characters[31] = 0;
    buffer->characters[0] = 0;
    buffer->string = gabi::ea(&buffer->characters[0]);
    buffer->vtable = ftree_tu::bufferedSafeString;
    ftree_formatColorName(buffer.get(), "tev_color%d", colorSlot);
    u32 target = static_cast<u32>(gabi::object_at<FtreeResourceVtable>(u32(buffer->vtable)).getEndFrame);
    gabi::call_ptr<void>(target, buffer.get());
    u32 entry = static_cast<u32>(gabi::object_at<FtreeMaterialEntry>(materialEntry).resource),
        relative = static_cast<u32>(gabi::object_at<FtreeMaterialResource>(entry).nameOffset);
    u32 table = relative ? entry + 0x38 + relative : 0;
    s32 index =
        ftree_findMaterialParameter(ftree_materialNameTable(table), ftree_formattedName(u32(buffer->string)));
    if (index < 0)
        return;
    entry = static_cast<u32>(gabi::object_at<FtreeMaterialEntry>(materialEntry).resource);
    relative = static_cast<u32>(gabi::object_at<FtreeMaterialResource>(entry).colorOffset);
    table = relative ? entry + 0x34 + relative : 0;
    u32 colors = static_cast<u32>(gabi::object_at<FtreeMaterialEntry>(materialEntry).colors) +
                 static_cast<u16>(gabi::object_at<FtreeColorEntry>(table + u32(index) * 20).offset);
    f32 value = f32(static_cast<f32>(gabi::object_at<FtreeFloatColor>(colors).r) * 255.0f);
    *red = value > 1.0f ? s16(gabi::ftoi(value)) : 1;
    value = f32(static_cast<f32>(gabi::object_at<FtreeFloatColor>(colors).g) * 255.0f);
    *green = value > 1.0f ? s16(gabi::ftoi(value)) : 1;
    value = f32(static_cast<f32>(gabi::object_at<FtreeFloatColor>(colors).b) * 255.0f);
    *blue = value > 1.0f ? s16(gabi::ftoi(value)) : 1;
}

// HD: reload the color block after each virtual call, which can replace material state.
/* 02347E84 */
void daObjFtree::Act_c::set_tev_color(void* tableObject, s32 colorIndex, s32 red, s32 green, s32 blue) {
    u32 material = static_cast<u32>(
        gabi::object_at<FtreeMaterialList>(
            static_cast<u32>(gabi::object_at<FtreeMaterialRoot>(gabi::ea(tableObject)).listObject))
            .material);
    while (material) {
        u32 block = static_cast<u32>(gabi::object_at<FtreeMaterial>(material).colorBlock),
            target = static_cast<u32>(gabi::object_at<FtreeColorBlockVtable>(
                                          static_cast<u32>(gabi::object_at<FtreeColorBlock>(block).vtable))
                                          .getColor);
        u32 color = gabi::call_ptr<u32>(target, static_cast<void*>(&gabi::object_at<FtreeColorBlock>(block)),
                                        colorIndex);
        gabi::object_at<GXColorS10>(color).r = s16(red);
        block = static_cast<u32>(gabi::object_at<FtreeMaterial>(material).colorBlock);
        target = static_cast<u32>(gabi::object_at<FtreeColorBlockVtable>(
                                      static_cast<u32>(gabi::object_at<FtreeColorBlock>(block).vtable))
                                      .getColor);
        color = gabi::call_ptr<u32>(target, static_cast<void*>(&gabi::object_at<FtreeColorBlock>(block)),
                                    colorIndex);
        gabi::object_at<GXColorS10>(color).g = s16(green);
        block = static_cast<u32>(gabi::object_at<FtreeMaterial>(material).colorBlock);
        target = static_cast<u32>(gabi::object_at<FtreeColorBlockVtable>(
                                      static_cast<u32>(gabi::object_at<FtreeColorBlock>(block).vtable))
                                      .getColor);
        color = gabi::call_ptr<u32>(target, static_cast<void*>(&gabi::object_at<FtreeColorBlock>(block)),
                                    colorIndex);
        gabi::object_at<GXColorS10>(color).b = s16(blue);
        block = static_cast<u32>(gabi::object_at<FtreeMaterial>(material).colorBlock);
        target = static_cast<u32>(gabi::object_at<FtreeColorBlockVtable>(
                                      static_cast<u32>(gabi::object_at<FtreeColorBlock>(block).vtable))
                                      .getColor);
        color = gabi::call_ptr<u32>(target, static_cast<void*>(&gabi::object_at<FtreeColorBlock>(block)),
                                    colorIndex);
        block = static_cast<u32>(gabi::object_at<FtreeMaterial>(material).colorBlock);
        target = static_cast<u32>(gabi::object_at<FtreeColorBlockVtable>(
                                      static_cast<u32>(gabi::object_at<FtreeColorBlock>(block).vtable))
                                      .setColor);
        gabi::call_ptr<void>(target, static_cast<void*>(&gabi::object_at<FtreeColorBlock>(block)), colorIndex,
                             static_cast<void*>(&gabi::object_at<GXColorS10>(color)));
        s16 r = static_cast<s16>(gabi::object_at<GXColorS10>(color).r),
            b = static_cast<s16>(gabi::object_at<GXColorS10>(color).b),
            g = static_cast<s16>(gabi::object_at<GXColorS10>(color).g),
            a = static_cast<s16>(gabi::object_at<GXColorS10>(color).a);
        gabi::Local<FtreeFloatColor> floating;
        gabi::Local<be<u32>[8]> floatingLinkage;
        floating->g = f32(f32(g) / 255.0f);
        floating->b = f32(f32(b) / 255.0f);
        floating->a = f32(f32(a) / 255.0f);
        floating->r = f32(f32(r) / 255.0f);
        gabi::Local<FtreeFloatColor> rgb;
        gabi::Local<be<u32>[8]> rgbLinkage;
        ftree_convertMaterialRgb(rgb.get(), floating.get(), 1.0f);
        u32 flags = static_cast<u32>(gabi::object_at<FtreeMaterial>(material).colorFlags),
            shift = u32(colorIndex) + 4;
        u32 mask = (shift & 32) ? 0 : (1u << (shift & 31));
        gabi::object_at<FtreeMaterial>(material).colorFlags = flags | mask;
        u32 output = ftree_getMaterialColorStorage(
            static_cast<void*>(&gabi::object_at<FtreeMaterial>(material).colorFlags), shift);
        a = static_cast<s16>(gabi::object_at<GXColorS10>(color).a);
        f32 x = rgb->r, y = rgb->g, z = rgb->b;
        gabi::object_at<FtreeFloatColor>(output).g = y;
        gabi::object_at<FtreeFloatColor>(output).b = z;
        gabi::object_at<FtreeFloatColor>(output).r = x;
        gabi::object_at<FtreeFloatColor>(output).a = f32(f32(a) / 255.0f);
        material = static_cast<u32>(gabi::object_at<FtreeMaterial>(material).next);
    }
}

/* 02346534 */
u32 daObjFtree::Act_c::is_brought() {
    initTreeMap();
    s32 tree = static_cast<s32>(this->param_get_tree_idx(4, 0));
    u32 id = tree < 10 ? static_cast<u8>(ftree_treeId(u32(tree))) : 15;
    return ftree_daObjFtree__is_broughtID(id);
}

/* 02347128 */
void daObjFtree::Act_c::set_broughtID(s32 id) {
    u32 mask = 1u << (u32(id) & 7);
    u32 bits = ftree_dSv_event_c__getEventReg_2(static_cast<void*>(dComIfGs_getEvent()), 0x9EFF);
    ftree_dSv_event_c__setEventReg_2(static_cast<void*>(dComIfGs_getEvent()), 0x9EFF, u8(bits | mask));
    bits = ftree_dSv_event_c__getEventReg_2(static_cast<void*>(dComIfGs_getEvent()), 0x9AFF);
    ftree_dSv_event_c__setEventReg_2(static_cast<void*>(dComIfGs_getEvent()), 0x9AFF, u8(bits | mask));
    gabi::Local<daObjFtree::_ftree_seach_info_> info;
    gabi::Local<be<u32>[8]> infoLinkage;
    this->get_ftree_info(info.get());
    if (u32(info->brought) == u32(info->total))
        ftree_dSv_event_c__onEventBit(static_cast<void*>(dComIfGs_getEvent()), 0x102);
}

/* 023471EC */
void daObjFtree::Act_c::set_brought() {
    initTreeMap();
    s32 tree = static_cast<s32>(this->param_get_tree_idx(4, 0));
    u32 id = tree < 10 ? static_cast<u8>(ftree_treeId(u32(tree))) : 15;
    this->set_broughtID(id);
    this->brought = 1;
}

/* 0234829C */
void daObjFtree::Act_c::unset_broughtID(s32 id) {
    u32 bits = ftree_dSv_event_c__getEventReg_2(static_cast<void*>(dComIfGs_getEvent()), 0x9EFF);
    ftree_dSv_event_c__setEventReg(static_cast<void*>(dComIfGs_getEvent()), 0x9EFF,
                                   bits & ~(1u << (u32(id) & 7)));
}

/* 02348314 */
void daObjFtree::Act_c::unset_brought() {
    initTreeMap();
    s32 tree = static_cast<s32>(this->param_get_tree_idx(4, 0));
    u32 id = tree < 10 ? static_cast<u8>(ftree_treeId(u32(tree))) : 15;
    this->unset_broughtID(id);
    this->brought = 0;
}

/* 02346B38 */
void daObjFtree::Act_c::get_ftree_info(daObjFtree::_ftree_seach_info_* info) {
    info->brought = 0;
    info->total = 8;
    for (u32 id = 0; id < 8; ++id)
        if (ftree_daObjFtree__is_broughtID_2(id))
            info->brought = u32(info->brought) + 1;
}

/* 02346BA8 */
u32 daObjFtree::Act_c::iam_last() {
    gabi::Local<daObjFtree::_ftree_seach_info_> info;
    gabi::Local<be<u32>[8]> infoLinkage;
    this->get_ftree_info(info.get());
    if (static_cast<s32>(this->is_brought()))
        return 0;
    return s32(u32(info->brought)) >= s32(u32(info->total) - 1);
}

/* 023485C0 */
u32 daObjFtree::Act_c::action_none_init(s32 parameter) {
    this->smallVisible = 0;
    this->largeVisible = 0;
    return 1;
}

/* 023485D4 */
u32 daObjFtree::Act_c::action_waitS_init(s32 parameter) {
    this->smallVisible = 1;
    this->largeVisible = 0;
    this->unset_brought();
    gabi::Local<SafeString> name;
    gabi::Local<be<u32>[8]> nameLinkage;
    name->mStringTop = ftree_tu::archiveName;
    name->__vtbl = ftree_tu::safeString;
    u32 resource = ftree_dRes_control_c__getRes(ftree_resourceManager(), name.get(), 13);
    if (!resource)
        return 0;
    u32 model = static_cast<u32>(ftree_morph(static_cast<u32>(this->mpMorf)).model);
    if (!model)
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x56D, "anmSmodel != (0)");
    f32 rate = 1.0f;
    if (!ftree_initBrkAnimation(
            static_cast<void*>(&this->mBrkAnm),
            static_cast<void*>(&ftree_modelData(static_cast<u32>(ftree_model(model).modelData))),
            static_cast<void*>(&gabi::object_at<FtreeResource>(resource)), 1, 0, 0, -1, 1, rate, 0))
        return 0;
    return this->SetJointAnimation(6, rate, 0.0f, 0);
}

/* 02348718 */
void daObjFtree::Act_c::action_waitS_main() {
    if (static_cast<s16>(this->eventTimer) == 1) {
        s32 next = static_cast<s16>(this->m356) == static_cast<s16>(this->mEventCam2Idx) ? 4 : 8;
        if (this->process_init(next, 0)) {
            this->eventTimer = 0;
            return;
        }
    }
    u32 hit = ftree_dCcD_GObjInf__ChkCoHit(static_cast<void*>(&this->m508));
    if (!static_cast<u32>(this->previousCollision) && hit == 1 && this->process_init(MODE_PIKUPIKU_S, 1)) {
        this->previousCollision = 0;
        return;
    }
    if (gabi::ftoi(ftree_cM_rndF(100.0f)) == 0 && static_cast<u32>(this->mMode) == MODE_WAIT_S) {
        f32 random = ftree_cM_rndF(1.0f);
        s16 repetitions = s16(gabi::ftoi(gabi::fmadds(4.0f, random, 1.0f)));
        this->process_init(MODE_PIKUPIKU_S, repetitions);
    }
    this->previousCollision = hit;
}
/* 02348884 */
u32 daObjFtree::Act_c::action_waitM_init(s32 parameter) {
    this->set_brought();
    this->smallVisible = 1;
    this->largeVisible = 0;
    gabi::Local<SafeString> name;
    gabi::Local<be<u32>[8]> nameLinkage;
    name->mStringTop = ftree_tu::archiveName;
    name->__vtbl = ftree_tu::safeString;
    u32 resource = ftree_dRes_control_c__getRes(ftree_resourceManager(), name.get(), 13);
    if (!resource)
        return 0;
    u32 model = static_cast<u32>(ftree_morph(static_cast<u32>(this->mpMorf)).model);
    if (!model)
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x5A8, "anmSmodel != (0)");
    f32 rate = -1.0f;
    if (!ftree_initBrkAnimation(
            static_cast<void*>(&this->mBrkAnm),
            static_cast<void*>(&ftree_modelData(static_cast<u32>(ftree_model(model).modelData))),
            static_cast<void*>(&gabi::object_at<FtreeResource>(resource)), 1, 0, 0, -1, 1, rate, 0))
        return 0;
    if (!this->SetJointAnimation(5, rate, 0.0f, 0))
        return 0;
    u32 morph = static_cast<u32>(this->mpMorf);
    s16 frame = s16(gabi::ftoi(f32(f32(static_cast<s16>(ftree_morph(morph).endFrame)) - 1.0f)));
    ftree_morph(morph).frame = f32(frame);
    return 1;
}

/* 02348A38 */
void daObjFtree::Act_c::action_waitM_main() {
    if (!ftree_dSv_player_item_record_c__getTimer(ftree_saveItemRecord())) {
        gabi::Local<daObjFtree::_ftree_seach_info_> info;
        gabi::Local<be<u32>[8]> infoLinkage;
        this->get_ftree_info(info.get());
        if (u32(info->total) != u32(info->brought)) {
            this->process_init(MODE_CHANGE_MS, 0);
            return;
        }
    }
    if (static_cast<u32>(this->mMode) != MODE_WAIT_M)
        return;
    u32 hit = ftree_dCcD_GObjInf__ChkCoHit(static_cast<void*>(&this->m508));
    if (ftree_dKy_rain_check()) {
        this->process_init(MODE_PIKUPIKU_M, -1);
        this->previousCollision = hit;
        return;
    }
    if (!static_cast<u32>(this->previousCollision) && hit == 1 && this->process_init(MODE_PIKUPIKU_M, 120)) {
        this->previousCollision = 0;
        return;
    }
    if (gabi::ftoi(ftree_cM_rndF(90.0f)) == 0) {
        f32 random = ftree_cM_rndF(1.0f);
        s16 duration = s16(gabi::ftoi(gabi::fmadds(90.0f, random, 30.0f)));
        this->process_init(MODE_PIKUPIKU_M, duration);
    }
    this->previousCollision = hit;
}

/* 02348BBC */
u32 daObjFtree::Act_c::action_waitL_init(s32 parameter) {
    this->largeVisible = 1;
    this->smallVisible = 0;
    this->set_brought();
    return 1;
}
/* 02348BF8 */
void daObjFtree::Act_c::action_waitL_main() {
    u32 mode = static_cast<u32>(this->mMode);
    if (mode == MODE_WAIT_L) {
        if (ftree_dKy_rain_check()) {
            this->process_init(MODE_PIKUPIKU_L, -1);
            return;
        }
        f32 chance = ftree_cM_rndF(30.0f);
        if (gabi::ftoi(chance) == 0) {
            f32 random = ftree_cM_rndF(1.0f);
            s16 duration = s16(gabi::ftoi(gabi::fmadds(80.0f, random, 100.0f)));
            this->process_init(MODE_PIKUPIKU_L, duration);
            return;
        }
        if (static_cast<s16>(this->m356) != -1)
            this->process_init(MODE_PIKUPIKU_L, -1);
    } else if (mode == MODE_PIKUPIKU_L) {
        if (!ftree_dKy_rain_check() && static_cast<s16>(this->largeDuration) == -1 &&
            static_cast<s16>(this->m356) == -1)
            this->process_init(MODE_WAIT_L, 0);
    }
}

/* 0234988C */
u32 daObjFtree::Act_c::action_pikupikuS_init(s32 repetitions) {
    this->repetitions = repetitions > 0 ? repetitions : 1;
    this->completedRepetitions = 0;
    this->smallVisible = 1;
    this->largeVisible = 0;
    f32 random = ftree_cM_rndF(1.0f);
    f32 rate = gabi::fmadds(static_cast<f32>(ftree_smallIdleRateRange()), random,
                            static_cast<f32>(ftree_smallIdleRateBase()));
    return this->SetJointAnimation(6, rate, 10.0f, 0);
}

/* 0234991C */
void daObjFtree::Act_c::action_pikupikuS_main() {
    if (static_cast<s32>(this->PlayStopJointAnimation())) {
        if (static_cast<s32>(this->completedRepetitions) < static_cast<s32>(this->repetitions)) {
            f32 random = ftree_cM_rndF(1.0f);
            f32 rate = gabi::fmadds(static_cast<f32>(ftree_smallRepeatRateRange()), random,
                                    static_cast<f32>(ftree_smallIdleRateBase()));
            this->SetJointAnimation(6, rate, 10.0f, 0);
        } else
            this->process_init(MODE_WAIT_S, 0);
        this->completedRepetitions = static_cast<u32>(this->completedRepetitions) + 1;
    }
    this->action_waitS_main();
}

/* 023499CC */
u32 daObjFtree::Act_c::action_pikupikuM_init(s32 duration) {
    this->set_brought();
    this->mediumDuration = s16(duration);
    this->mediumElapsed = 0;
    this->mediumPhase = 0;
    this->smallVisible = 1;
    this->largeVisible = 0;
    return 1;
}

/* 02349A20 */
void daObjFtree::Act_c::action_pikupikuM_main() {
    s16 duration = static_cast<s16>(this->mediumDuration), elapsed = static_cast<s16>(this->mediumElapsed);
    if (elapsed < duration || duration == -1) {
        u16 angle = static_cast<u16>(this->mediumPhase);
        u32 table = ftree_tu::sinCosTable + (angle >> 3) * 8;
        this->m696 =
            s16(gabi::ftoi(f32(100.0f * static_cast<f32>(gabi::object_at<FtreeSinCos>(table).sine))));
        this->m698 =
            s16(gabi::ftoi(f32(300.0f * static_cast<f32>(gabi::object_at<FtreeSinCos>(table).sine))));
        this->mediumPhase = u16(angle + 4000);
        elapsed = static_cast<s16>(this->mediumElapsed);
    } else {
        this->process_init(MODE_WAIT_M, 0);
        elapsed = static_cast<s16>(this->mediumElapsed);
    }
    this->mediumElapsed = s16(u16(elapsed) + 1);
    this->action_waitM_main();
}

/* 02349AFC */
u32 daObjFtree::Act_c::action_pikupikuL_init(s32 duration) {
    this->set_brought();
    this->largeDuration = s16(duration);
    this->largeElapsed = 0;
    this->largePhase = 0;
    this->smallVisible = 0;
    this->largeVisible = 1;
    return 1;
}

/* 02349B50 */
void daObjFtree::Act_c::action_pikupikuL_main() {
    s16 duration = static_cast<s16>(this->largeDuration), elapsed = static_cast<s16>(this->largeElapsed);
    if (elapsed < duration || duration == -1) {
        u16 angle = static_cast<u16>(this->largePhase);
        u32 table = ftree_tu::sinCosTable + (angle >> 3) * 8;
        this->m6A0 = s16(gabi::ftoi(f32(30.0f * static_cast<f32>(gabi::object_at<FtreeSinCos>(table).sine))));
        this->m6A2 = s16(gabi::ftoi(f32(36.0f * static_cast<f32>(gabi::object_at<FtreeSinCos>(table).sine))));
        this->largePhase = u16(angle + 3000);
        elapsed = static_cast<s16>(this->largeElapsed);
    } else {
        this->process_init(MODE_WAIT_L, 0);
        elapsed = static_cast<s16>(this->largeElapsed);
    }
    this->largeElapsed = s16(u16(elapsed) + 1);
    this->action_waitL_main();
}

/* 0234915C */
u32 daObjFtree::Act_c::action_changeSL_init(s32 parameter) {
    this->largeVisible = 0;
    this->effectPending = 1;
    this->colorBlend = 0.0f;
    u32 morph = static_cast<u32>(this->mpMorf);
    this->largeScale = static_cast<f32>(ftree_minimumScale());
    this->smallVisible = 1;
    this->colorDelay = 30;
    u32 model = static_cast<u32>(ftree_morph(morph).model);
    if (!model)
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x6B7, "anmSmodel != (0)");
    this->get_tev_material0_color(static_cast<void*>(&ftree_model(model)), 2, &this->cachedSmallColor[0],
                                  &this->cachedSmallColor[1], &this->cachedSmallColor[2]);
    this->m66C[0] = static_cast<u16>(this->cachedSmallColor[0]);
    this->m66C[1] = static_cast<u16>(this->cachedSmallColor[1]);
    this->m66C[2] = static_cast<u16>(this->cachedSmallColor[2]);
    u16 alpha = static_cast<u16>(this->cachedSmallColor[3]);
    this->largePhase = 0;
    this->m66C[3] = alpha;
    return 1;
}

/* 023490FC */
void daObjFtree::Act_c::action_changeSL_main() {
    u32 joint = this->PlayStopJointAnimation();
    u32 color = this->PlayStopColorAnimation();
    if (joint == 1 && color == 1)
        this->process_init(MODE_CHANGE_SL2, 0);
}

/* 02348CF8 */
u32 daObjFtree::Act_c::action_changeSL2_init(s32 parameter) {
    this->smallVisible = 1;
    this->largeVisible = 0;
    gabi::Local<SafeString> name;
    gabi::Local<be<u32>[8]> nameLinkage;
    name->mStringTop = ftree_tu::archiveName;
    name->__vtbl = ftree_tu::safeString;
    u32 resource = ftree_dRes_control_c__getRes(ftree_resourceManager(), name.get(), 13);
    if (!resource)
        return 0;
    u32 model = static_cast<u32>(ftree_morph(static_cast<u32>(this->mpMorf)).model);
    if (!model)
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x68F, "anmSmodel != (0)");
    if (!ftree_initBrkAnimation(
            static_cast<void*>(&this->mBrkAnm),
            static_cast<void*>(&ftree_modelData(static_cast<u32>(ftree_model(model).modelData))),
            static_cast<void*>(&gabi::object_at<FtreeResource>(resource)), 1, 0, 0, -1, 1, 1.0f, 0))
        return 0;
    if (!this->SetJointAnimation(5, 1.0f, 10.0f, 0))
        return 0;
    this->set_brought();
    ftree_playTreeSound(0x6A17, static_cast<void*>(&this->current.pos));
    return 1;
}

/* 02349238 */
void daObjFtree::Act_c::action_changeSL2_main() {
    if (u8(this->smallVisible) != 0) {
        ftree_cLib_addCalc(static_cast<void*>(&this->colorBlend), 1.0f,
                           static_cast<f32>(ftree_minimumScale()), 1.0f,
                           static_cast<f32>(ftree_colorBlendStep()));
        f32 blend = static_cast<f32>(this->colorBlend);
        for (unsigned i = 0; i < 3; ++i) {
            f32 start = f32(static_cast<s16>(this->cachedSmallColor[i]));
            this->m66C[i] = s16(gabi::ftoi(gabi::fmadds(f32(255.0f - start), blend, start)));
        }
        if (blend == 1.0f) {
            s16 timer = static_cast<s16>(this->colorDelay);
            if (timer >= 0) {
                if (timer > 0) {
                    timer = s16(u16(timer) - 1);
                    this->colorDelay = timer;
                }
                if (timer == 0) {
                    this->smallVisible = 0;
                    this->largeVisible = 1;
                }
            }
        }
    }
    if (u8(this->largeVisible) == 0)
        return;
    f32 size = static_cast<f32>(this->largeScale), minimum = static_cast<f32>(ftree_minimumScale()), blend;
    if (!(size > minimum))
        blend = 0.0f;
    else if (!(size < 1.0f))
        blend = 1.0f;
    else
        blend = f32(f32(size - minimum) / static_cast<f32>(ftree_scaleBlendRange()));
    ftree_cLib_addCalc(static_cast<void*>(&this->largeScale), 1.0f,
                       static_cast<f32>(ftree_scaleApproachRate()), 1.0f,
                       static_cast<f32>(ftree_scaleMinimumStep()));
    for (unsigned i = 0; i < 3; ++i) {
        f32 end = f32(static_cast<s16>(this->m664[i]));
        this->m674[i] = s16(gabi::ftoi(gabi::fmadds(f32(end - 255.0f), blend, 255.0f)));
    }
    if (static_cast<f32>(this->largeScale) == 1.0f && this->process_init(MODE_WAIT_L, 0))
        this->heartLaunchTimer = 47;
    u16 phase = static_cast<u16>(this->largePhase);
    u32 table = ftree_tu::sinCosTable + (phase >> 3) * 8;
    this->m6A0 = s16(gabi::ftoi(f32(30.0f * static_cast<f32>(gabi::object_at<FtreeSinCos>(table).sine))));
    s16 next = s16(u16(static_cast<s16>(this->largePhase)) + 3000);
    this->m6A2 = s16(gabi::ftoi(f32(36.0f * static_cast<f32>(gabi::object_at<FtreeSinCos>(table).sine))));
    this->largePhase = next;
}

/* 02349574 */
u32 daObjFtree::Act_c::action_changeLS_init(s32 parameter) {
    this->largeVisible = 1;
    this->smallVisible = 0;
    this->colorBlend = 1.0f;
    this->largeScale = 1.0f;
    this->unset_brought();
    return 1;
}
/* 023495C0 */
void daObjFtree::Act_c::action_changeLS_main() {
    f32 minimum = static_cast<f32>(ftree_minimumScale());
    if (u8(this->largeVisible) != 0) {
        f32 size = static_cast<f32>(this->largeScale), blend;
        if (!(size > minimum))
            blend = 0.0f;
        else if (!(size < 1.0f))
            blend = 1.0f;
        else
            blend = f32(f32(size - minimum) / static_cast<f32>(ftree_scaleBlendRange()));
        ftree_cLib_addCalc(static_cast<void*>(&this->largeScale), minimum,
                           static_cast<f32>(ftree_scaleApproachRate()), 1.0f,
                           static_cast<f32>(ftree_scaleMinimumStep()));
        for (unsigned i = 0; i < 3; ++i) {
            f32 end = f32(static_cast<s16>(this->m664[i]));
            this->m674[i] = s16(gabi::ftoi(gabi::fmadds(f32(end - 255.0f), blend, 255.0f)));
        }
        if (static_cast<f32>(this->largeScale) == minimum) {
            this->smallVisible = 1;
            this->largeVisible = 0;
        }
    }
    if (u8(this->smallVisible) == 0)
        return;
    ftree_cLib_addCalc(static_cast<void*>(&this->colorBlend), 0.0f, minimum, 1.0f,
                       static_cast<f32>(ftree_colorBlendStep()));
    f32 blend = static_cast<f32>(this->colorBlend);
    for (unsigned i = 0; i < 3; ++i) {
        f32 start = f32(static_cast<s16>(this->cachedSmallColor[i]));
        this->m66C[i] = s16(gabi::ftoi(gabi::fmadds(f32(255.0f - start), blend, start)));
    }
    if (blend == 0.0f)
        this->process_init(MODE_CHANGE_LS2, 0);
}

/* 02348F20 */
u32 daObjFtree::Act_c::action_changeLS2_init(s32 parameter) {
    this->smallVisible = 1;
    this->largeVisible = 0;
    gabi::Local<SafeString> name;
    gabi::Local<be<u32>[8]> nameLinkage;
    name->mStringTop = ftree_tu::archiveName;
    name->__vtbl = ftree_tu::safeString;
    u32 resource = ftree_dRes_control_c__getRes(ftree_resourceManager(), name.get(), 13);
    if (!resource)
        return 0;
    u32 model = static_cast<u32>(ftree_morph(static_cast<u32>(this->mpMorf)).model);
    if (!model)
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x736, "anmSmodel != (0)");
    if (!ftree_initBrkAnimation(
            static_cast<void*>(&this->mBrkAnm),
            static_cast<void*>(&ftree_modelData(static_cast<u32>(ftree_model(model).modelData))),
            static_cast<void*>(&gabi::object_at<FtreeResource>(resource)), 1, 0, 0, -1, 1, -1.0f, 0))
        return 0;
    return this->SetJointAnimation(5, static_cast<f32>(ftree_shrinkAnimationRate()), 10.0f, -20);
}

/* 02349098 */
void daObjFtree::Act_c::action_changeLS2_main() {
    u32 joint = this->PlayStopJointAnimation();
    u32 color = this->PlayStopColorAnimation();
    if (joint == 1 && color == 1)
        this->process_init(MODE_WAIT_S, 0);
}

/* 02348E58 */
u32 daObjFtree::Act_c::action_changeSM_init(s32 parameter) {
    this->set_brought();
    if (this->action_changeSL2_init(0) != 1)
        return 0;
    ftree_playTreeSound(0x6A17, static_cast<void*>(&this->current.pos));
    return 1;
}
/* 02348EC0 */
void daObjFtree::Act_c::action_changeSM_main() {
    u32 joint = this->PlayStopJointAnimation();
    u32 color = this->PlayStopColorAnimation();
    if (joint == 1 && color == 1)
        this->process_init(MODE_WAIT_M, 0);
}

/* 0234903C */
u32 daObjFtree::Act_c::action_changeMS_init(s32 parameter) {
    if (!this->action_changeLS2_init(0))
        return 0;
    this->unset_brought();
    return 1;
}
/* 023490F8 */
void daObjFtree::Act_c::action_changeMS_main() { this->action_changeLS2_main(); }

/* 023463E4 */
u32 daObjFtree::Act_c::process_init(u32 mode, s32 parameter) {
    if (mode >= MODE_COUNT)
        return 0;
    const auto& action = gabi::object_at<gabi::ptmf<u32 (Act_c::*)(s32)>>(gabi::ea(M_action_init) + mode * 8);
    if (!(this->*action)(parameter))
        return 0;
    this->mMode = mode;
    this->m696 = 0;
    this->m6A0 = 0;
    this->m698 = 0;
    this->m6A2 = 0;
    return 1;
}

/* 0234749C */
void daObjFtree::Act_c::process_main() {
    u32 mode = static_cast<u32>(this->mMode);
    if (mode >= MODE_COUNT)
        return;
    const auto& action = gabi::object_at<gabi::ptmf<void (Act_c::*)()>>(gabi::ea(M_action_main) + mode * 8);
    (this->*action)();
}

/* 02345ADC */
u32 daObjFtree::Act_c::NodeCallBack_Effect(void* node, s32 phase) {
    if (s16(this->effectPending) == 1 && phase == 0) {

        gabi::Local<cXyz> source, result;
        gabi::Local<be<u32>[8]> sourceLinkage;
        source->x = 0.0f;
        source->y = 0.0f;
        source->z = 0.0f;
        u32 joint = ftree_getJoint(node);
        u16 index = static_cast<u16>(gabi::object_at<FtreeJointView>(joint).index);
        u32 model = static_cast<u32>(ftree_model(static_cast<u32>(ftree_j3dSystem())).matrixBuffer);
        u32 matrices = static_cast<u32>(ftree_matrixBuffer(model).mpAnmMtx);
        u16 flags = static_cast<u16>(ftree_matrixBuffer(model).mFlags);
        ftree_matrixBuffer(model).mFlags = flags | 0x10;
        ftree_PSMTXCopy(static_cast<void*>(&gabi::object_at<Mtx34>(matrices + index * sizeof(Mtx34))),
                        static_cast<void*>(&ftree_workMatrix()));
        gabi::Local<Mtx34> matrix;
        gabi::Local<be<u32>[8]> matrixLinkage;
        ftree_PSMTXCopy_2(static_cast<void*>(&ftree_workMatrix()), matrix.get());
        ftree_PSMTXMultVec(matrix.get(), source.get(), result.get());
        ftree_playTreeSound(0x6A18, static_cast<void*>(&this->current.pos));
        u32 play = ftree_getPlay();
        u32 first = ftree_dPa_control_c__set(static_cast<void*>(&gabi::object_at<dPa_control_c>(
                                                 static_cast<u32>(ftree_play(play).particleControl))),
                                             0, 0x82C4, result.get(), 0, 0, 255, 0, -1, 0, 0, 0);
        play = ftree_getPlay();
        u32 second = ftree_dPa_control_c__set(static_cast<void*>(&gabi::object_at<dPa_control_c>(
                                                  static_cast<u32>(ftree_play(play).particleControl))),
                                              0, 0x82C5, result.get(), 0, 0, 255, 0, -1, 0, 0, 0);
        if (first && second)
            this->effectPending = 0;
    }
    return 1;
}

/* 02346064 */
u32 daObjFtree::Act_c::create_heap() {
    gabi::Local<SafeString> animationName, modelName, largeName, colorName;
    gabi::Local<be<u32>[8]> animationNameLinkage;
    animationName->mStringTop = ftree_tu::archiveName;
    animationName->__vtbl = ftree_tu::safeString;
    u32 animation = ftree_dRes_control_c__getRes(ftree_resourceManager(), animationName.get(), 6);
    if (!animation)
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x861, "tmp_bck_data != 0");
    modelName->__vtbl = ftree_tu::safeString;
    modelName->mStringTop = ftree_tu::archiveName;
    u32 data = ftree_dRes_control_c__getRes(ftree_resourceManager(), modelName.get(), 10);
    if (!data) {
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x867, "mdl_dataS != 0");
        return 0;
    }
    u32 morph = ftree_mDoExt_McaMorf__mDoExt_McaMorf(0, static_cast<void*>(&ftree_modelData(data)), 0, 0,
                                                     ftree_jointAnimation(animation), 0, 0, -1, 1.0f, 1, 0, 0,
                                                     0x11020203);
    this->mpMorf = morph;
    if (!morph)
        return 0;
    u32 model = static_cast<u32>(ftree_morph(morph).model);
    if (!model)
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x87C, "anmSmodel != (0)");
    else
        ftree_model(model).userArea = gabi::ea(this);
    u32 count = static_cast<u32>(ftree_modelData(data).mJointNum),
        nodes = static_cast<u32>(ftree_modelData(data).mJointNodes);
    gabi::object_at<FtreeJointView>(nodes + (count > 2 ? 0x38 : 0)).callback =
        gabi::guest_fn(reinterpret_cast<const void*>(&ftree_nodeEffectCallback));
    count = static_cast<u32>(ftree_modelData(data).mJointNum);
    nodes = static_cast<u32>(ftree_modelData(data).mJointNodes);
    gabi::object_at<FtreeJointView>(nodes + (count > 3 ? 0x54 : 0)).callback =
        gabi::guest_fn(reinterpret_cast<const void*>(&ftree_nodeMediumCallback));
    count = static_cast<u32>(ftree_modelData(data).mJointNum);
    nodes = static_cast<u32>(ftree_modelData(data).mJointNodes);
    gabi::object_at<FtreeJointView>(nodes + (count > 4 ? 0x70 : 0)).callback =
        gabi::guest_fn(reinterpret_cast<const void*>(&ftree_nodeMediumCallback));
    count = static_cast<u32>(ftree_modelData(data).mJointNum);
    nodes = static_cast<u32>(ftree_modelData(data).mJointNodes);
    gabi::object_at<FtreeJointView>(nodes + (count > 5 ? 0x8C : 0)).callback =
        gabi::guest_fn(reinterpret_cast<const void*>(&ftree_nodeMediumCallback));
    largeName->__vtbl = ftree_tu::safeString;
    largeName->mStringTop = ftree_tu::archiveName;
    u32 largeData = ftree_dRes_control_c__getRes(ftree_resourceManager(), largeName.get(), 9);
    if (!largeData) {
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x888, "mdl_dataL != 0");
        return 0;
    }
    u32 large = ftree_mDoExt_J3DModel__create(static_cast<void*>(&ftree_modelData(largeData)), 0, 0x11020203);
    this->mpModel = large;
    if (!large)
        return 0;
    ftree_model(large).userArea = gabi::ea(this);
    count = static_cast<u32>(ftree_modelData(largeData).mJointNum);
    nodes = static_cast<u32>(ftree_modelData(largeData).mJointNodes);
    gabi::object_at<FtreeJointView>(nodes + (count > 1 ? 0x1C : 0)).callback =
        gabi::guest_fn(reinterpret_cast<const void*>(&ftree_nodeLargeCallback));
    colorName->__vtbl = ftree_tu::safeString;
    colorName->mStringTop = ftree_tu::archiveName;
    u32 color = ftree_dRes_control_c__getRes(ftree_resourceManager(), colorName.get(), 13);
    if (!color) {
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x898, "brk_data != 0");
        return 0;
    }
    u32 initialized = ftree_initBrkAnimation(
        static_cast<void*>(&this->mBrkAnm), static_cast<void*>(&ftree_modelData(data)),
        static_cast<void*>(&gabi::object_at<GXColorS10>(color)), 1, 0, 0, -1, 0, 1.0f, 0);
    this->get_tev_material0_color(static_cast<void*>(&ftree_model(model)), 2, &this->m654[0], &this->m654[1],
                                  &this->m654[2]);
    this->m66C[0] = static_cast<u16>(this->m654[0]);
    this->m66C[1] = static_cast<u16>(this->m654[1]);
    this->m66C[2] = static_cast<u16>(this->m654[2]);
    this->m66C[3] = static_cast<u16>(this->m654[3]);
    this->get_tev_material0_color(static_cast<void*>(&ftree_model(static_cast<u32>(this->mpModel))), 2,
                                  &this->m664[0], &this->m664[1], &this->m664[2]);
    this->m674[0] = static_cast<u16>(this->m664[0]);
    this->m674[1] = static_cast<u16>(this->m664[1]);
    this->m674[2] = static_cast<u16>(this->m664[2]);
    this->m674[3] = static_cast<u16>(this->m664[3]);
    return static_cast<u32>(this->mpMorf) && model && initialized;
}

/* 02346D34 */
u32 daObjFtree::Act_c::_create() {
    u32 condition = static_cast<u32>(this->actor_condition);
    if (!(condition & 8)) {
        if (gabi::ea(this) != 0) {
            ftree_fopAc_ac_c__fopAc_ac_c(this);
            this->__vtbl = ftree_tu::actor;
            ftree_mDoExt_brkAnm__init(static_cast<void*>(&this->mBrkAnm));
            ftree_cBgS_GndChk__cBgS_GndChk(static_cast<void*>(&this->mGndChk));
            this->mGndChk.mbCamThrough = 0;
            this->mGndChk.mbBombThrough = 0;
            this->mGndChk.mbObjThrough = 1;
            this->mGndChk.cBgS_Chk::mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(&this->mGndChk));
            this->mGndChk.mbArrowThrough = 0;
            this->mGndChk.mbLinkThrough = 0;
            this->mGndChk.cBgS_Chk::__vtbl = ftree_tu::groundCheck;
            this->mGndChk.mGrp = 1;
            this->mGndChk.mbRopeThrough = 0;
            this->mGndChk.dBgS_PolyPassChk::__vtbl = ftree_tu::polyPassCheck;
            this->mGndChk.cBgS_PolyInfo::__vtbl = ftree_tu::polyInfo;
            this->mGndChk.mbBoomerangThrough = 0;
            this->mGndChk.cBgS_Chk::mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(&this->mGndChk));
            this->mGndChk.dBgS_GrpPassChk::__vtbl = ftree_tu::groupPassCheck;
            ftree_constructCollisionStatus(static_cast<void*>(&this->m35C));
            ftree_dCcD_GStts__dCcD_GStts(static_cast<void*>(&this->m35C.__vtbl_gstts));
            this->m35C.__vtbl = ftree_tu::collisionStatus;
            this->m35C.__vtbl_gstts = ftree_tu::globalCollisionStatus;
            ftree_dCcD_GObjInf__dCcD_GObjInf(static_cast<void*>(&this->m398));
            gabi::object_at<FtreeShapeAttr>(gabi::ea(&this->m398.mShapeAttr)).cylinderVtable =
                ftree_tu::baseCylinder;
            gabi::object_at<FtreeShapeAttr>(gabi::ea(&this->m398.mShapeAttr)).shapeVtable =
                ftree_tu::shapeAttribute;
            ftree_constructCylinder(static_cast<void*>(&this->m398.mCyl));
            this->m398.__vtbl_hitinf = ftree_tu::collisionHitInfo;
            gabi::object_at<FtreeShapeAttr>(gabi::ea(&this->m398.mShapeAttr)).cylinderVtable =
                ftree_tu::collisionCylinder;
            this->m398.mCyl.__vtbl = ftree_tu::cylinder;
            ftree_constructCollisionStatus(static_cast<void*>(&this->m4CC));
            ftree_dCcD_GStts__dCcD_GStts(static_cast<void*>(&this->m4CC.__vtbl_gstts));
            this->m4CC.__vtbl = ftree_tu::collisionStatus;
            this->m4CC.__vtbl_gstts = ftree_tu::globalCollisionStatus;
            ftree_dCcD_GObjInf__dCcD_GObjInf(static_cast<void*>(&this->m508));
            gabi::object_at<FtreeShapeAttr>(gabi::ea(&this->m508.mShapeAttr)).cylinderVtable =
                ftree_tu::baseCylinder;
            gabi::object_at<FtreeShapeAttr>(gabi::ea(&this->m508.mShapeAttr)).shapeVtable =
                ftree_tu::shapeAttribute;
            ftree_constructCylinder(static_cast<void*>(&this->m508.mCyl));
            this->m508.mCyl.__vtbl = ftree_tu::cylinder;
            gabi::object_at<FtreeShapeAttr>(gabi::ea(&this->m508.mShapeAttr)).cylinderVtable =
                ftree_tu::collisionCylinder;
            this->m508.__vtbl_hitinf = ftree_tu::collisionHitInfo;
            condition = static_cast<u32>(this->actor_condition);
        }
        this->actor_condition = condition | 8;
    }
    u32 phase = ftree_dComIfG_resLoad(static_cast<void*>(&this->mPhs), "Vmr");
    if (phase != 4)
        return phase;
    if (!ftree_fopAcM_entrySolidHeap(this, gabi::guest_fn(reinterpret_cast<const void*>(&ftree_heapCallback)),
                                     0))
        return 5;
    initTreeMap();
    s32 tree = static_cast<s32>(this->param_get_tree_idx(4, 0));
    this->treeId = tree < 10 ? static_cast<u8>(ftree_treeId(u32(tree))) : 15;
    this->heartPlaced = 0;
    this->set_first_stat();
    this->talk_ct();
    u32 process = static_cast<u32>(this->mBsPcId);
    f32 y = f32(static_cast<f32>(this->current.pos.y) + 100.0f), z = static_cast<f32>(this->current.pos.z),
        x = static_cast<f32>(this->current.pos.x);
    this->mGndChk.m_pos.z = z;
    this->mGndChk.cBgS_Chk::mActorPid = process;
    this->mGndChk.m_pos.x = x;
    this->mGndChk.m_pos.y = y;
    u32 play = ftree_getPlay();
    this->m34C = ftree_cBgS__GroundCross(ftree_backgroundServer(play), static_cast<void*>(&this->mGndChk));
    this->set_mtx();
    u32 model = static_cast<u32>(ftree_morph(static_cast<u32>(this->mpMorf)).model), matrix = 0;
    if (!model)
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x8DE, "anmSmodel != (0)");
    else
        matrix = model + 0xC8;
    this->cullMtx = matrix;
    ftree_fopAcM_setCullSizeBox(this, -300.0f, 0.0f, -300.0f, 300.0f, 1000.0f, 300.0f);
    ftree_dCcD_Stts__Init(static_cast<void*>(&this->m35C), 255, 255, this);
    ftree_dCcD_Cyl__Set(static_cast<void*>(&this->m398), static_cast<void*>(ftree_M_cyl_srcW));
    this->m398.mStts = &this->m35C;
    u32 first = static_cast<u32>(ftree_targetVectorX()), flags = static_cast<u32>(this->m398.mGObjTg.mSPrm);
    ftree_words(&this->m398.mGObjTg.mVec).x = first;
    ftree_words(&this->m398.mGObjTg.mVec).y = static_cast<u32>(ftree_targetVectorY());
    u32 third = static_cast<u32>(ftree_targetVectorZ());
    this->m398.mGObjTg.mSPrm = flags | 4;
    ftree_words(&this->m398.mGObjTg.mVec).z = third;
    ftree_dCcD_Stts__Init(static_cast<void*>(&this->m4CC), 255, 255, this);
    ftree_dCcD_Cyl__Set(static_cast<void*>(&this->m508), static_cast<void*>(ftree_M_cyl_src));
    this->m508.mStts = &this->m4CC;
    first = static_cast<u32>(ftree_targetVectorX());
    flags = static_cast<u32>(this->m508.mGObjTg.mSPrm);
    ftree_words(&this->m508.mGObjTg.mVec).x = first;
    ftree_words(&this->m508.mGObjTg.mVec).y = static_cast<u32>(ftree_targetVectorY());
    third = static_cast<u32>(ftree_targetVectorZ());
    this->m508.mGObjTg.mSPrm = flags | 4;
    ftree_words(&this->m508.mGObjTg.mVec).z = third;
    this->XyEvent_init();
    this->eventInfo.mpCheckCB = gabi::guest_fn(reinterpret_cast<const void*>(&ftree_checkEvent));
    this->eventInfo.mpEventCB = gabi::guest_fn(reinterpret_cast<const void*>(&ftree_eventCallback));
    play = ftree_getPlay();
    this->mEventCam0Idx =
        s16(ftree_dEvent_manager_c__getEventIdx(ftree_eventManager(play), "morinoki_camera0", 255));
    play = ftree_getPlay();
    this->mEventCam1Idx =
        s16(ftree_dEvent_manager_c__getEventIdx(ftree_eventManager(play), "morinoki_camera1", 255));
    play = ftree_getPlay();
    this->mEventCam2Idx =
        s16(ftree_dEvent_manager_c__getEventIdx(ftree_eventManager(play), "morinoki_camera2", 255));
    return phase;
}

/* 0234726C */
u32 daObjFtree::Act_c::_delete() {
    if (static_cast<u32>(this->brought))
        this->set_brought();
    ftree_dComIfG_resDelete(static_cast<void*>(&this->mPhs), "Vmr");
    return 1;
}

/* 023467DC */
void daObjFtree::Act_c::set_mtx() {
    u32 model = static_cast<u32>(ftree_morph(static_cast<u32>(this->mpMorf)).model);
    if (!model)
        ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x92A, "anmSmodel != (0)");
    f32 x = static_cast<f32>(this->scale.x), y = static_cast<f32>(this->scale.y),
        z = static_cast<f32>(this->scale.z);
    ftree_model(model).baseScale.y = y;
    ftree_model(model).baseScale.x = x;
    ftree_model(model).baseScale.z = z;
    ftree_PSMTXTrans(static_cast<void*>(&ftree_workMatrix()), static_cast<f32>(this->current.pos.x),
                     static_cast<f32>(this->current.pos.y), static_cast<f32>(this->current.pos.z));
    ftree_mDoMtx_ZXYrotM(static_cast<void*>(&ftree_workMatrix()), static_cast<s16>(this->shape_angle.x),
                         static_cast<s16>(this->shape_angle.y), static_cast<s16>(this->shape_angle.z));
    f32 matrix[12];
    for (unsigned i = 0; i < 12; ++i)
        matrix[i] = static_cast<f32>(ftree_workMatrix().m[i / 4][i % 4]);
    const unsigned firstOrder[] = {7, 2, 1, 6, 3, 0, 8, 4, 5, 10, 11, 9};
    for (unsigned i : firstOrder)
        ftree_model(model).baseTransform.m[i / 4][i % 4] = matrix[i];
    ftree_PSMTXCopy(static_cast<void*>(&ftree_workMatrix()), static_cast<void*>(&this->m2A8));
    f32 size = static_cast<f32>(this->largeScale);
    x = static_cast<f32>(this->scale.x);
    y = static_cast<f32>(this->scale.y);
    f32 scaledX = scaledComponent(x, size);
    model = static_cast<u32>(this->mpModel);
    z = static_cast<f32>(this->scale.z);
    ftree_model(model).baseScale.x = scaledX;
    ftree_model(model).baseScale.y = scaledComponent(y, size);
    ftree_model(model).baseScale.z = scaledComponent(z, size);
    ftree_PSMTXTrans(static_cast<void*>(&ftree_workMatrix()), static_cast<f32>(this->current.pos.x),
                     static_cast<f32>(this->current.pos.y), static_cast<f32>(this->current.pos.z));
    ftree_mDoMtx_ZXYrotM(static_cast<void*>(&ftree_workMatrix()), static_cast<s16>(this->shape_angle.x),
                         static_cast<s16>(this->shape_angle.y), static_cast<s16>(this->shape_angle.z));
    for (unsigned i = 0; i < 12; ++i)
        matrix[i] = static_cast<f32>(ftree_workMatrix().m[i / 4][i % 4]);
    model = static_cast<u32>(this->mpModel);
    const unsigned secondOrder[] = {11, 5, 8, 1, 2, 4, 10, 7, 9, 6, 0, 3};
    for (unsigned i : secondOrder)
        ftree_model(model).baseTransform.m[i / 4][i % 4] = matrix[i];
    ftree_PSMTXCopy(static_cast<void*>(&ftree_workMatrix()), static_cast<void*>(&this->m2A8));
    ftree_J3DModel__calc(static_cast<void*>(&ftree_model(static_cast<u32>(this->mpModel))));
}

/* 02347CF0 */
u32 daObjFtree::Act_c::_execute() {
    if (static_cast<u32>(this->mMode) == MODE_NONE)
        return 1;
    this->talk_main();
    this->set_mtx();
    this->process_main();
    this->set_collision();
    ftree_fopAcM_rollPlayerCrash(this, 7, static_cast<f32>(this->m4C8));
    s32 mediumStep = static_cast<u32>(this->mMode) == MODE_PIKUPIKU_M ? 1 : 30;
    ftree_cLib_addCalcAngleS2(static_cast<void*>(&this->m294), static_cast<s16>(this->m696), mediumStep,
                              0x1000);
    ftree_cLib_addCalcAngleS2(static_cast<void*>(&this->m296), static_cast<s16>(this->m698), mediumStep,
                              0x1000);
    u32 mode = static_cast<u32>(this->mMode);
    s32 largeStep = (mode == MODE_PIKUPIKU_L || mode == MODE_CHANGE_SL2) ? 1 : 10;
    ftree_cLib_addCalcAngleS2(static_cast<void*>(&this->m298), static_cast<s16>(this->m6A0), largeStep,
                              0x1000);
    ftree_cLib_addCalcAngleS2(static_cast<void*>(&this->m29A), static_cast<s16>(this->m6A2), largeStep,
                              0x1000);
    this->XyEvent_exe();
    if (static_cast<s16>(this->heartPlaced) == 0)
        this->heartPlaced = s16(static_cast<s32>(this->place_heart_part()));
    if (static_cast<s16>(this->heartLaunchTimer) == 1 && static_cast<s32>(this->launch_heart_part()))
        this->heartLaunchTimer = 0;
    s16 timer = static_cast<s16>(this->eventTimer);
    if (timer > 1)
        this->eventTimer = timer - 1;
    timer = static_cast<s16>(this->heartLaunchTimer);
    if (timer > 1)
        this->heartLaunchTimer = timer - 1;
    return 1;
}

/* 023480C4 */
u32 daObjFtree::Act_c::_draw() {
    if (!static_cast<u32>(this->mMode))
        return 1;
    if (u8(this->smallVisible) == 1) {
        u32 environment = ftree_dScnKy_env_light_c__setLightTevColorType();
        ftree_dScnKy_env_light_c__settingTevStruct(
            static_cast<void*>(&gabi::object_at<dScnKy_env_light_c>(environment)), 0,
            static_cast<void*>(&this->current.pos), static_cast<void*>(&this->tevStr));
        u32 model = static_cast<u32>(ftree_morph(static_cast<u32>(this->mpMorf)).model);
        if (!model)
            ftree_JUTAssertion__getSDevice("d_a_obj_ftree.cpp", 0x9C7, "anmSmodel != (0)");
        environment = ftree_dScnKy_env_light_c__setLightTevColorType();
        ftree_setLightTevColor(static_cast<void*>(&gabi::object_at<dScnKy_env_light_c>(environment)),
                               static_cast<void*>(&ftree_model(model)), static_cast<void*>(&this->tevStr));
        u32 play = ftree_getPlay();
        ftree_opaqueDrawBuffer() = static_cast<u32>(ftree_play(play).opaqueList);
        play = ftree_getPlay();
        ftree_translucentDrawBuffer() = static_cast<u32>(ftree_play(play).translucentList);
        u32 mode = static_cast<u32>(this->mMode);
        if (mode == MODE_CHANGE_SL2 || mode == MODE_CHANGE_LS) {
            gabi::object_at<FtreeModelDataFlags>(static_cast<u32>(ftree_model(model).modelData))
                .materialFlags = 0;
            this->set_tev_color(
                static_cast<void*>(&ftree_modelData(static_cast<u32>(ftree_model(model).modelData))), 2,
                static_cast<s16>(this->m66C[0]), static_cast<s16>(this->m66C[1]),
                static_cast<s16>(this->m66C[2]));
        } else
            ftree_mDoExt_brkAnm__entry(
                static_cast<void*>(&this->mBrkAnm),
                static_cast<void*>(&ftree_modelData(static_cast<u32>(ftree_model(model).modelData))),
                static_cast<f32>(gabi::object_at<be<f32>>(gabi::ea(&this->mBrkAnm[4]))));
        ftree_mDoExt_McaMorf__updateDL(static_cast<void*>(&ftree_morph(static_cast<u32>(this->mpMorf))));
        play = ftree_getPlay();
        ftree_opaqueDrawBuffer() = static_cast<u32>(ftree_play(play).defaultOpaqueList);
        play = ftree_getPlay();
        ftree_translucentDrawBuffer() = static_cast<u32>(ftree_play(play).defaultTranslucentList);
    }
    if (u8(this->largeVisible) == 1) {
        u32 environment = ftree_dScnKy_env_light_c__setLightTevColorType();
        ftree_dScnKy_env_light_c__settingTevStruct(
            static_cast<void*>(&gabi::object_at<dScnKy_env_light_c>(environment)), 1,
            static_cast<void*>(&this->current.pos), static_cast<void*>(&this->tevStr));
        environment = ftree_dScnKy_env_light_c__setLightTevColorType();
        ftree_setLightTevColor(static_cast<void*>(&gabi::object_at<dScnKy_env_light_c>(environment)),
                               static_cast<void*>(&ftree_model(static_cast<u32>(this->mpModel))),
                               static_cast<void*>(&this->tevStr));
        u32 play = ftree_getPlay();
        ftree_opaqueDrawBuffer() = static_cast<u32>(ftree_play(play).opaqueList);
        play = ftree_getPlay();
        ftree_translucentDrawBuffer() = static_cast<u32>(ftree_play(play).translucentList);
        u32 model = static_cast<u32>(this->mpModel);
        this->set_tev_color(
            static_cast<void*>(&ftree_modelData(static_cast<u32>(ftree_model(model).modelData))), 2,
            static_cast<s16>(this->m674[0]), static_cast<s16>(this->m674[1]),
            static_cast<s16>(this->m674[2]));
        ftree_mDoExt_modelUpdateDL(static_cast<void*>(&ftree_model(static_cast<u32>(this->mpModel))), 0);
        play = ftree_getPlay();
        ftree_opaqueDrawBuffer() = static_cast<u32>(ftree_play(play).defaultOpaqueList);
        play = ftree_getPlay();
        ftree_translucentDrawBuffer() = static_cast<u32>(ftree_play(play).defaultTranslucentList);
    }
    return 1;
}

/* 023469D0 */
u32 ftree_checkEvent(Act_c* actor, s32 index) {
    u32 play = ftree_getPlay();
    return static_cast<u8>(ftree_play(play).eventState[u32(index)]) == 0x59;
}

/* 023464B4 */
u32 ftree_isBroughtId(s32 id) {
    s32 bits = ftree_dSv_event_c__getEventReg(static_cast<void*>(dComIfGs_getEvent()), 0x9EFF);
    if ((u32(bits) >> (u32(id) & 7)) & 1)
        return 1;
    return ftree_dSv_event_c__isEventBit(static_cast<void*>(dComIfGs_getEvent()), 0x102) != 0;
}

/* 023463E0 */
u32 ftree_heapCallback(Act_c* actor) { return actor->create_heap(); }

/* 02346D30 */
s32 ftree_eventCallback(Act_c* actor, s32 event) { return actor->XyEventCallBack(event); }

/* 02349C2C */
u32 ftree_createMethod(Act_c* actor) { return actor->_create(); }

/* 02349C30 */
u32 ftree_deleteMethod(Act_c* actor) { return actor->_delete(); }

/* 02349C34 */
u32 ftree_executeMethod(Act_c* actor) { return actor->_execute(); }

/* 02349C38 */
u32 ftree_drawMethod(Act_c* actor) { return actor->_draw(); }

/* 02349CD0 */
void ftree_angleHelperDtor(void* object, u32 flags) {
    if (object && (flags & 1))
        ftree_dl(object);
}

/* 02349CE4 */
void ftree_vectorHelperDtor(void* object, u32 flags) {
    if (object && (flags & 1))
        ftree_dl(object);
}

/* 02349D14 */
void ftree_callbackHelperDtor(void* object, u32 flags) {
    if (object && (flags & 1))
        ftree_dl(object);
}

/* 02349D28 */
void ftree_otherHelperDtor(void* object, u32 flags) {
    if (object && (flags & 1))
        ftree_dl(object);
}

/* 02349CF8 */
void ftree_emptyCallback() {}

/* 02349DF0 */
void ftree_emptyStatic() {}

/* 02349DF4 */
u32 ftree_isDelete(Act_c* actor) { return 1; }
/* 02349CFC */
void ftree_terminateBuffer(void* object) {
    u32 a = gabi::ea(object), base = static_cast<u32>(gabi::object_at<FtreeColorNameBuffer>(a).string),
        count = static_cast<u32>(gabi::object_at<FtreeColorNameBuffer>(a).size);
    gabi::object_at<be<u8>>(base + count - 1) = 0;
}
/* 02349D3C */
void ftree_destroy(Act_c* actor, u32 flags) {
    if (!actor)
        return;
    ftree_dBgS_Acch___dBgS_Acch(static_cast<void*>(&actor->m508), 2);
    ftree_destroyCollisionStatus(static_cast<void*>(&actor->m4CC), 2);
    ftree_dBgS_Acch___dBgS_Acch(static_cast<void*>(&actor->m398), 2);
    ftree_destroyCollisionStatus(static_cast<void*>(&actor->m35C), 2);
    actor->mGndChk.cBgS_PolyInfo::__vtbl = ftree_tu::destroyPolyInfo;
    actor->mGndChk.dBgS_PolyPassChk::__vtbl = ftree_tu::destroyPolyPassCheck;
    actor->mGndChk.dBgS_GrpPassChk::__vtbl = ftree_tu::destroyGroupPassCheck;
    ftree_cBgS_Chk___cBgS_Chk(static_cast<void*>(&actor->mGndChk), 0);
    ftree_fopAc_ac_c___fopAc_ac_c(actor, 0);
    if (flags & 1)
        ftree_dl_2(actor);
}

/* 02349C3C */
void ftree_staticInit() {
    ftree_hioWord10469BD0() = 0;
    ftree_hioWord10469BC8() = 0;
    ftree_hioWord10469BD4() = 0;
    ftree_hioWord10469BCC() = 0;
    ftree_register_global_object(ftree_hioDestructorRecordA());
    f32 minimum = static_cast<f32>(ftree_hioMinimum()), maximum = static_cast<f32>(ftree_hioMaximum());
    ftree_hioFloat10469BBC() = minimum;
    ftree_hioFloat10469BC0() = maximum;
    ftree_initCommonByteA(ftree_hioByteA());
    ftree_register_global_object(ftree_hioDestructorRecordB());
    ftree_initCommonByteB(ftree_hioByteB());
    ftree_register_global_object(ftree_hioDestructorRecordC());
}
/* 02345C40 */
u32 ftree_nodeEffectCallback(void* node, s32 phase) {
    if (phase != 0)
        return 1;
    u32 actor = static_cast<u32>(ftree_model(static_cast<u32>(ftree_j3dSystem())).userArea);
    return ftree_actor(actor).NodeCallBack_Effect(node, 0);
}

// HD: load the entire matrix before the GHS permutation of stores.
/* 02345C68 */
u32 ftree_nodeMediumCallback(void* node, s32 phase) {
    if (phase != 0)
        return 1;
    u32 joint = ftree_getJoint(node);
    u32 system = static_cast<u32>(ftree_j3dSystem()), actor = static_cast<u32>(ftree_model(system).userArea);
    u16 index = static_cast<u16>(gabi::object_at<FtreeJointView>(joint).index);
    if (!actor)
        return 1;
    gabi::Local<csXyz> angles;
    gabi::Local<be<u32>[8]> anglesLinkage;
    ftree_csXyz__csXyz(angles.get(), static_cast<s16>(ftree_actor(actor).m294),
                       static_cast<s16>(ftree_actor(actor).m296), 0);
    u32 model = static_cast<u32>(ftree_model(system).matrixBuffer),
        matrices = static_cast<u32>(ftree_matrixBuffer(model).mpAnmMtx);
    u16 flags = static_cast<u16>(ftree_matrixBuffer(model).mFlags);
    ftree_matrixBuffer(model).mFlags = flags | 0x10;
    ftree_PSMTXCopy(static_cast<void*>(&gabi::object_at<Mtx34>(matrices + index * sizeof(Mtx34))),
                    static_cast<void*>(&ftree_workMatrix()));
    ftree_mDoMtx_ZXYrotM(static_cast<void*>(&ftree_workMatrix()), s16(angles->x), s16(angles->y),
                         s16(angles->z));
    model = static_cast<u32>(ftree_model(system).matrixBuffer);
    flags = static_cast<u16>(ftree_matrixBuffer(model).mFlags);
    ftree_matrixBuffer(model).mFlags = flags | 0x10;
    f32 m[12];
    for (unsigned i = 0; i < 12; ++i)
        m[i] = static_cast<f32>(ftree_workMatrix().m[i / 4][i % 4]);
    u32 destination = static_cast<u32>(ftree_matrixBuffer(model).mpAnmMtx) + index * 48;
    const unsigned order[] = {3, 4, 2, 0, 5, 11, 8, 1, 6, 9, 7, 10};
    for (unsigned i : order)
        gabi::object_at<Mtx34>(destination).m[i / 4][i % 4] = m[i];
    ftree_PSMTXCopy(static_cast<void*>(&ftree_workMatrix()), static_cast<void*>(&ftree_jointWorkMatrix()));
    return 1;
}

// HD: large-tree joints use their own tilt pair and the same deferred matrix stores.
/* 02345DA0 */
u32 ftree_nodeLargeCallback(void* node, s32 phase) {
    if (phase != 0)
        return 1;
    u32 joint = ftree_getJoint(node);
    u32 system = static_cast<u32>(ftree_j3dSystem()), actor = static_cast<u32>(ftree_model(system).userArea);
    u16 index = static_cast<u16>(gabi::object_at<FtreeJointView>(joint).index);
    if (!actor)
        return 1;
    gabi::Local<csXyz> angles;
    gabi::Local<be<u32>[8]> anglesLinkage;
    ftree_csXyz__csXyz_2(angles.get(), 0, static_cast<s16>(ftree_actor(actor).m298),
                         static_cast<s16>(ftree_actor(actor).m29A));
    u32 model = static_cast<u32>(ftree_model(system).matrixBuffer),
        matrices = static_cast<u32>(ftree_matrixBuffer(model).mpAnmMtx);
    u16 flags = static_cast<u16>(ftree_matrixBuffer(model).mFlags);
    ftree_matrixBuffer(model).mFlags = flags | 0x10;
    ftree_PSMTXCopy(static_cast<void*>(&gabi::object_at<Mtx34>(matrices + index * sizeof(Mtx34))),
                    static_cast<void*>(&ftree_workMatrix()));
    ftree_mDoMtx_ZXYrotM(static_cast<void*>(&ftree_workMatrix()), s16(angles->x), s16(angles->y),
                         s16(angles->z));
    model = static_cast<u32>(ftree_model(system).matrixBuffer);
    flags = static_cast<u16>(ftree_matrixBuffer(model).mFlags);
    ftree_matrixBuffer(model).mFlags = flags | 0x10;
    f32 m[12];
    for (unsigned i = 0; i < 12; ++i)
        m[i] = static_cast<f32>(ftree_workMatrix().m[i / 4][i % 4]);
    u32 destination = static_cast<u32>(ftree_matrixBuffer(model).mpAnmMtx) + index * 48;
    const unsigned order[] = {3, 4, 2, 0, 5, 11, 8, 1, 6, 9, 7, 10};
    for (unsigned i : order)
        gabi::object_at<Mtx34>(destination).m[i / 4][i % 4] = m[i];
    ftree_PSMTXCopy(static_cast<void*>(&ftree_workMatrix()), static_cast<void*>(&ftree_jointWorkMatrix()));
    return 1;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data ftree_M_cyl_srcW 100291A4
data ftree_M_cyl_src 10029160
data M_action_init 10029260
data M_action_main 1002938C
str 10029550 in 0234726C
str 100292E4 in 023467DC
str 100292F8 in 023467DC
str 10029454 in 023485D4
str 10029468 in 023485D4
str 10029480 in 02348884
str 10029494 in 02348884
str 10029428 in 023480C4
str 1002943C in 023480C4
str 100294AC in 02348CF8
str 100294C0 in 02348CF8
str 100294D8 in 02348F20
str 100294EC in 02348F20
str 10029500 in 0234915C
str 10029514 in 0234915C
str 100291E8 in 02345ED8
str 100291F4 in 02346064
str 10029208 in 02346064
str 1002921C in 02346064
str 1002922C in 02346064
str 1002923C in 02346064
str 10029250 in 02346064
str 10029324 in 02346D34
str 10029338 in 02346D34
str 1002934C in 02346D34
str 10029360 in 02346D34
str 10029374 in 02346D34
str 10029550 in 02346D34
ext 025200D4 unsigned int ftree_getPlay()
ext 025B8BB0 int ftree_dSv_event_c__getEventReg(void *, int)
ext 025B8B94 int ftree_dSv_event_c__isEventBit(void *, int)
ext 023464B4 unsigned int ftree_daObjFtree__is_broughtID(unsigned int)
ext 023464B4 int ftree_daObjFtree__is_broughtID_2(unsigned int)
ext 025B8BB0 unsigned int ftree_dSv_event_c__getEventReg_2(void *, int)
ext 025B8AF4 void ftree_dSv_event_c__setEventReg(void *, int, unsigned int)
ext 025B8AF4 void ftree_dSv_event_c__setEventReg_2(void *, int, unsigned char)
ext 025B8B68 void ftree_dSv_event_c__onEventBit(void *, int)
ext 025204C8 void ftree_dComIfG_resDelete(void *, const char *)
ext 025E535C unsigned int ftree_mDoExt_McaMorf__play(void *, int, int, int)
ext 025E742C unsigned int ftree_mDoExt_baseAnm__play(void *)
ext 0273AF40 void ftree_dl(void *)
ext 02515A70 void ftree_dBgS_Acch___dBgS_Acch(void *, int)
ext 02515860 void ftree_destroyCollisionStatus(void *, int)
ext 02008DAC void ftree_cBgS_Chk___cBgS_Chk(void *, int)
ext 025D50BC void ftree_fopAc_ac_c___fopAc_ac_c(daObjFtree::Act_c *, int)
ext 0273AF40 void ftree_dl_2(daObjFtree::Act_c *)
ext 020198D8 float ftree_cM_rndF(float)
ext 025E19CC void ftree_playTreeSound(int, void *)
ext 028F026C void ftree_register_global_object(void *)
ext 028ED6F8 void ftree_initCommonByteA(void *)
ext 028EAB2C void ftree_initCommonByteB(void *)
ext 0256019C int ftree_dKy_rain_check()
ext 026066C4 unsigned int ftree_dRes_control_c__getRes(void *, SafeString *, int)
ext 025E4A98 void ftree_mDoExt_McaMorf__setAnm(void *, void *, int, float, float, float, float, int)
ext 027F7878 unsigned int ftree_getJoint(void *)
ext 028E90D4 void ftree_PSMTXCopy(void *, void *)
ext 028E90D4 void ftree_PSMTXCopy_2(void *, Mtx34 *)
ext 028E8F64 void ftree_PSMTXMultVec(Mtx34 *, cXyz *, cXyz *)
ext 025A847C unsigned int ftree_dPa_control_c__set(void *, int, int, cXyz *, int, int, int, int, int, int, int, int)
ext 0201A478 void ftree_csXyz__csXyz(csXyz *, short, short, int)
ext 025F1B48 void ftree_mDoMtx_ZXYrotM(void *, short, short, short)
ext 0201A478 void ftree_csXyz__csXyz_2(csXyz *, int, short, short)
ext 025D69FC void ftree_fopAcM_rollPlayerCrash(daObjFtree::Act_c *, int, float)
ext 0200F428 void ftree_cLib_addCalcAngleS2(void *, short, int, int)
ext 025200D4 void ftree_getPlay_2()
ext 020195B0 int ftree_cM_atan2s(float, float)
ext 028E93CC void ftree_PSMTXTrans(void *, float, float, float)
ext 025F1C28 void ftree_mDoMtx_YrotM(void *, short)
ext 028E8DE8 float ftree_PSVECSquareDistance(cXyz *, void *)
ext 025440C8 int ftree_dEvent_manager_c__endCheck(void *, short)
ext 025B7D90 int ftree_dSv_player_collect_c__isSymbol(void *, int)
ext 0273AA24 void ftree_JUTAssertion__getSDevice(const char *, int, const char *)
ext 027F4D5C void ftree_J3DModel__calc(void *)
ext 025E8154 unsigned int ftree_initBrkAnimation(void *, void *, void *, int, int, int, int, int, float, int)
ext 02516464 unsigned int ftree_dCcD_GObjInf__ChkCoHit(void *)
ext 025B61C8 unsigned int ftree_dSv_player_item_record_c__getTimer(void *)
ext 025D8AB0 unsigned int ftree_fopAcM_fastCreateItem(cXyz *, int, signed char, csXyz *, cXyz *, int, int, float, float, float)
ext 025F7DB0 unsigned int ftree_fopMsgM_messageSet(void *, unsigned int, void *)
ext 025F795C unsigned int ftree_fopMsgM_SearchByID(void *)
ext 025F74D0 void ftree_setMessageStatus(void *, int)
ext 028E8DE8 float ftree_PSVECSquareDistance_2(void *, void *)
ext 0201ADE0 void ftree_cXyz____mi(void *, cXyz *, void *)
ext 02555D0C unsigned int ftree_dScnKy_env_light_c__setLightTevColorType()
ext 025626A4 void ftree_dScnKy_env_light_c__settingTevStruct(void *, int, void *, void *)
ext 02562F5C void ftree_setLightTevColor(void *, void *, void *)
ext 025E83FC void ftree_mDoExt_brkAnm__entry(void *, void *, float)
ext 025E54D8 void ftree_mDoExt_McaMorf__updateDL(void *)
ext 025E2DE0 void ftree_mDoExt_modelUpdateDL(void *, int)
ext 02759C28 void ftree_formatColorName(FtreeColorNameBuffer *, const char *, unsigned int)
ext 027DF9B0 int ftree_findMaterialParameter(void *, void *)
ext 0274D458 void ftree_convertMaterialRgb(FtreeFloatColor *, FtreeFloatColor *, float)
ext 027F9F0C unsigned int ftree_getMaterialColorStorage(void *, unsigned int)
ext 025162A4 int ftree_dCcD_GObjInf__ChkTgHit(void *)
ext 023129C4 void ftree_daObj__HitSeStart(void *, signed char, void *, int)
ext 02312C8C void ftree_daObj__HitEff_kikuzu(daObjFtree::Act_c *, void *)
ext 0255F458 void ftree_dKy_Sound_set(cXyz *, int, unsigned int, int)
ext 0251621C void ftree_dCcD_GObjInf__ClrTgHit(void *)
ext 020184DC void ftree_cM3dGCyl__SetR(void *, float)
ext 02018428 void ftree_cM3dGCyl__SetH(void *, float)
ext 020182E0 void ftree_cM3dGCyl__SetC(void *, cXyz *)
ext 0200E240 void ftree_cCcS__Set(void *, void *)
ext 020182E0 void ftree_cM3dGCyl__SetC_2(void *, void *)
ext 025B8B94 unsigned int ftree_dSv_event_c__isEventBit_2(void *, int)
ext 0201A478 void ftree_csXyz__csXyz_3(csXyz *, int, int, int)
ext 0200ECD4 void ftree_cLib_addCalc(void *, float, float, float, float)
ext 025E4F64 unsigned int ftree_mDoExt_McaMorf__mDoExt_McaMorf(int, void *, int, int, void *, int, int, int, float, int, int, int, int)
ext 025E38E0 unsigned int ftree_mDoExt_J3DModel__create(void *, int, int)
ext 025D4ED0 void ftree_fopAc_ac_c__fopAc_ac_c(daObjFtree::Act_c *)
ext 025E80D0 void ftree_mDoExt_brkAnm__init(void *)
ext 02008E0C void ftree_cBgS_GndChk__cBgS_GndChk(void *)
ext 0200BD2C void ftree_constructCollisionStatus(void *)
ext 02515DA0 void ftree_dCcD_GStts__dCcD_GStts(void *)
ext 02515FB8 void ftree_dCcD_GObjInf__dCcD_GObjInf(void *)
ext 02018590 void ftree_constructCylinder(void *)
ext 02520460 unsigned int ftree_dComIfG_resLoad(void *, const char *)
ext 025D63E8 unsigned int ftree_fopAcM_entrySolidHeap(daObjFtree::Act_c *, int, int)
ext 02008974 float ftree_cBgS__GroundCross(void *, void *)
ext 025D674C void ftree_fopAcM_setCullSizeBox(daObjFtree::Act_c *, float, float, float, float, float, float)
ext 02515F14 void ftree_dCcD_Stts__Init(void *, int, int, daObjFtree::Act_c *)
ext 02516518 void ftree_dCcD_Cyl__Set(void *, void *)
ext 02543F10 int ftree_dEvent_manager_c__getEventIdx(void *, const char *, int)
*/
// clang-format on
