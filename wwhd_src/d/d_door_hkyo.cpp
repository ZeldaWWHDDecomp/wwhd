/** Door messages and Triforce crest effects. GameCube-style source verified as d_door_06. */
#include "d/d_door_hkyo.h"

const gc_tu_data gc_tu = { .safeString = 0x1004C524 };
static dDoor_messageManagerGlobal_c sMessageManager = {};

/* 0252BEC4 */
void dDoor_msg_c::init(s32 msg_id) {
    mMsgId = s16(msg_id);
    mMsgPId = -1;
    mState = 0;
    // HD: mReady is retained across init.
}

/* 0252BEDC */
BOOL dDoor_msg_c::proc(cXyz* pos) {
    // HD: messages use the global manager and numeric status instead of a msg_class pointer.
    void* manager = sMessageManager.mpManager;
    switch (mState) {
    case 0:
        mMsgPId = dDoor_messageSet(manager, mMsgId, pos);
        if (mMsgPId != -1) mState = u8(mState + 1);
        break;
    case 1:
        mReady = 1;
        mState = 2;
        if (dDoor_messageStatus(manager) != BOX_CLOSED) return FALSE;
        dDoor_messageSetStatus(manager, MSG_DESTROYED);
        return TRUE;
    case 2:
        if (dDoor_messageStatus(manager) == MSG_TYPING) {
            s32 id = mMsgId;
            if (id == 0x6A8 || id == 0x1BBD) {
                // HD: obtain the game object before building the shock vector.
                dVibration_c* vibration = dComIfGp_getVibration();
                cXyz direction(0.0f, 1.0f, 0.0f);
                vibration->StartShock(7, -33, &direction);
            }
            mState = u8(mState + 1);
        }
        break;
    case 3:
        if (dDoor_messageStatus(manager) == MSG_DISPLAYED) {
            s32 id = mMsgId;
            if (id == 0x1BBD || (u32(id) >= 0x1BC0 && u32(id) <= 0x1BC2)) {
                mMsgId = s16(id + 1);
                dDoor_messageSetStatus(manager, MSG_CONTINUES);
                dDoor_messageSet(manager, mMsgId, nullptr);
            } else {
                mState = u8(mState + 1);
                dDoor_messageSetStatus(manager, MSG_ENDS);
            }
        }
        break;
    }
    if (!mReady || dDoor_messageStatus(manager) != BOX_CLOSED) return FALSE;
    dDoor_messageSetStatus(manager, MSG_DESTROYED);
    return TRUE;
}

/* 0252C0BC */
static dDoor_hkyo_c* dDoor_hkyo_construct(dDoor_hkyo_c* i_this) {
    // HD: GHS allocates on a null placement argument, then constructs the phase request.
    if (!i_this) {
        i_this = (dDoor_hkyo_c*)operator_new(sizeof(dDoor_hkyo_c));
        if (!i_this) return nullptr;
    }
    dDoor_phaseConstruct(&i_this->mPhs, sizeof(i_this->mPhs));
    i_this->m11 = 0;
    i_this->mpModel = nullptr;
    i_this->mAnmIdx = 0;
    i_this->mpBrkAnm = nullptr;
    return i_this;
}

/* 0252C11C */
s32 dDoor_hkyo_c::resLoad() {
    if (!m11) return cPhs_COMPLEATE_e;
    return dComIfG_resLoad(&mPhs, "Hkyo");
}

/* 0252C13C */
void dDoor_hkyo_c::resDelete() {
    if (m11) dComIfG_resDelete(&mPhs, "Hkyo");
}

/* 0252C154 */
BOOL dDoor_hkyo_c::create() {
    if (!m11) return TRUE;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("Hkyo", dRes_INDEX_HKYO_BDL_HKYO1_e);
    if (!modelData) dDoor_assert("d_door.cpp", 0x4A7, "modelData != (0)");
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000202);
    if (!mpModel) return FALSE;
    mpBrkAnm = dDoor_brkConstruct(nullptr);
    if (!mpBrkAnm) return FALSE;
    void* brk = dComIfG_getObjectRes("Hkyo", dRes_INDEX_HKYO_BRK_HKYO1B_e);
    return dDoor_brkInit(mpBrkAnm, modelData, brk, 1, J3DFrameCtrl::EMode_LOOP,
                        0, -1, 0, 0, 1.0f) != 0;
}

/* 0252C274 */
void dDoor_hkyo_c::init() { mAnmIdx = NONE; }

/* 0252C280 */
void dDoor_hkyo_c::calcMtx(fopAc_ac_c* door, f32 offsY) {
    if (!m11 || !mpModel) return;
    mDoMtx_stack_c::transS(door->current.pos.x, door->current.pos.y, door->current.pos.z);
    mDoMtx_stack_c::YrotM(door->current.angle.y);
    mDoMtx_stack_c::transM(0.0f, offsY, 0.0f);
    // HD: snapshot all twelve values before re-reading mpModel (matrix can alias it).
    f32 values[12];
    for (int i = 0; i < 12; ++i) values[i] = mDoMtx_stack_c::get()->m[i / 4][i % 4];
    J3DModel* model = mpModel;
    for (int i = 0; i < 12; ++i) model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
}

/* 0252C390 */
BOOL dDoor_hkyo_c::chkFirst() {
    if (m11 != 1 || mAnmIdx != FIRST) return FALSE;
    return !dComIfGs_isEventBit(0x2602);
}

/* 0252C3F8 */
void dDoor_hkyo_c::draw(fopAc_ac_c* door) {
    if (!m11 || !mAnmIdx || !mpModel || chkFirst()) return;
    J3DModelData* modelData = mpModel->getModelData();
    dScnKy_env_light_c* light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel, &door->tevStr);
    mDoExt_brkAnm* anm = mpBrkAnm;
    f32 frame = ((dDoor_brkFrame_c*)anm)->mFrameCtrl.getFrame();
    mDoExt_brkAnm_entry(anm, modelData, frame);
    mDoExt_modelUpdateDL(mpModel);
}

/* 0252C49C */
void dDoor_hkyo_c::setAnm(u32 idx) {
    if (mAnmIdx == idx) return;
    mAnmIdx = u8(idx);
    if (!idx) return;
    J3DModelData* modelData = mpModel->getModelData();
    s32 fileIndex = idx == FIRST ? dRes_INDEX_HKYO_BRK_HKYO1A_e : idx == SECOND ? dRes_INDEX_HKYO_BRK_HKYO1B_e : dRes_INDEX_HKYO_BRK_HKYO1C_e;
    void* brk = dComIfG_getObjectRes("Hkyo", fileIndex);
    dDoor_brkInit(mpBrkAnm, modelData, brk, 1, J3DFrameCtrl::EMode_LOOP,
                 0, -1, 1, 0, 1.0f);
}

/* 0252C574 */
void dDoor_hkyo_c::proc(fopAc_ac_c* door) {
    u8 mode = m11;
    if (!mode || !mpModel) return;
    u32 idx = NONE;
    switch (mode) {
    case 1:
        if (dComIfGs_isTmpBit(0x0108)) idx = FIRST;
        else if (dComIfGs_isTmpBit(0x0110)) idx = SECOND;
        break;
    case 4:
        if (!dComIfGs_isEventBit(0x1710)) idx = THIRD;
        break;
    case 3:
        if (dDoor_isTact(&dComIfGs_info()->mSavedata.mPlayer.mCollect, 2) &&
            !dComIfGs_isEventBit(0x1704)) idx = THIRD;
        break;
    case 2:
        if (dComIfGs_isEventBit(0x1704) && !dComIfGs_isEventBit(0x1B01)) idx = THIRD;
        break;
    }
    setAnm(idx);
    if (!mAnmIdx) return;
    mDoExt_baseAnm_play(mpBrkAnm);
    // HD: retain the null check on the wrapped eye-position address.
    if (door && gabi::ea(door) + (u32)offsetof(fopAc_ac_c, eyePos)) {
        s32 reverb = dComIfGp_getReverb(door->current.roomNo);
        mDoAud_seStart(0x61D4, &door->eyePos, 0, reverb);
    }
}

/* 0252C748 */
void dDoor_hkyo_c::onFirst() {
    u8 idx = mAnmIdx;
    if (idx == FIRST || idx == SECOND) dComIfGs_onEventBit(idx == FIRST ? 0x2602 : 0x2601);
}

/* 0252C788 */
static u32 dDoor_hkyo_chkStart() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    return (((dDoor_playerFlags_c*)player)->mGrabUpEnd >> 5) & 1;
}

/* ---------------- HD: compiler-generated and template code ---------------- */

/* 0252C7B4 */
static void __sinit_d_door_cpp() {
    sinit_header_statics_rodata(0x10475704, 0x101D60E8, 0x10475710, 0x1004C898);
}

/* 0252C848 */
static void dDoor_hkyo_destroy(dDoor_hkyo_c* i_this, u32 flags) {
    if (i_this && (flags & 1)) operator_delete(i_this);
}

/* 0252C85C */
static void dDoor_info_destroy(fopAc_ac_c* i_this, u32 flags) {
    if (!i_this) return;
    dDoor_actorDestroy(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}

/* 0252C8B0 */
static void dDoor_safeString_assureTermination(dDoor_safeString_c* i_this) {}

/* 0252C8B4 */
static dDoor_archiveStream_c* dDoor_archiveOpen(void* archive, dDoor_safeString_c* name) {
    dDoor_archiveStream_c* stream = (dDoor_archiveStream_c*)operator_new(sizeof(dDoor_archiveStream_c));
    if (!stream) return nullptr;
    stream->mBase[1] = 0;
    stream->mBase[0] = 0;
    // HD: unsigned guest addition can wrap the embedded-buffer address to null.
    u32 bufferAddress = gabi::ea(stream) + offsetof(dDoor_archiveStream_c, mName);
    dDoor_bufferString_c* buffer = gabi::at<dDoor_bufferString_c>(bufferAddress);
    if (!buffer) buffer = (dDoor_bufferString_c*)operator_new(sizeof(dDoor_bufferString_c));
    if (buffer) {
        buffer->mStringTop = buffer->mBuffer;
        buffer->__vtbl = 0x1004C8B8;
        buffer->mBufferSize = 16;
        buffer->mBuffer[15] = 0;
        be<u8>* data = buffer->mStringTop;
        buffer->__vtbl = 0x1004C948;
        *data = 0;
        buffer->__vtbl = 0x1004C960;
    }
    dDoor_streamConstruct(stream);
    u32 assure = name->__vtbl->assureTermination;
    be<u8>* dest = stream->mName.mStringTop;
    gabi::call_ptr<void>(assure, name);
    be<u8>* data = name->mStringTop;
    s32 count = 0;
    if (*data) {
        do {
            ++count;
            ++data;
            // HD: safety limit in the verified archive-name copy.
            if (count > 0x40000) { count = 0; break; }
        } while (*data);
    }
    s32 capacity = stream->mName.mBufferSize;
    assure = name->__vtbl->assureTermination;
    if (count >= capacity) count = capacity - 1;
    gabi::call_ptr<void>(assure, name);
    dDoor_copyBytes(dest, name->mStringTop, count, 0);
    // HD: signed count is added with 32-bit address wrap, including -1.
    *gabi::at<be<u8>>(gabi::ea(dest) + u32(count)) = 0;
    if (dDoor_streamOpen(stream, archive)) {
        dDoor_streamDestroy(stream, 3);
        return nullptr;
    }
    return stream;
}

/* WWHD glue
data sMessageManager 101F4B5C nocheck
str 1004C858 "Hkyo" in 0252C11C
str 1004C860 "Hkyo" in 0252C13C
str 1004C868 "Hkyo" in 0252C154
str 1004C890 "Hkyo" in 0252C49C
str 1004C870 "d_door.cpp"
str 1004C87C "modelData != (0)"
ext 025F7DB0 s32 dDoor_messageSet(void* manager, s32 id, cXyz* pos)
ext 025F795C u32 dDoor_messageStatus(void* manager)
ext 025F74D0 void dDoor_messageSetStatus(void* manager, u32 status)
ext 028F521C void dDoor_phaseConstruct(request_of_phase_process_class* phase, u32 size)
ext 025E80D0 mDoExt_brkAnm* dDoor_brkConstruct(mDoExt_brkAnm* anm)
ext 025E8154 u32 dDoor_brkInit(mDoExt_brkAnm* anm, J3DModelData* data, void* key, u32 play, s32 mode, s32 start, s32 end, u32 enable, s32 extra, f32 speed)
ext 025B7B10 BOOL dDoor_isTact(dSv_player_collect_c* collect, u32 tact)
ext 025D50BC void dDoor_actorDestroy(fopAc_ac_c* actor, u32 flags)
ext 027F0A9C void dDoor_streamConstruct(dDoor_archiveStream_c* stream)
ext 027F0B04 u32 dDoor_streamOpen(dDoor_archiveStream_c* stream, void* archive)
ext 027F0A0C void dDoor_streamDestroy(dDoor_archiveStream_c* stream, u32 flags)
ext C0009988 void dDoor_copyBytes(be<u8>* dest, be<u8>* source, s32 count, u32 flags)
ext 0273AA24 void dDoor_assert(const char* file, s32 line, const char* expression)
*/
