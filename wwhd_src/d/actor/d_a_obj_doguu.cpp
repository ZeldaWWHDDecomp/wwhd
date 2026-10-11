/* Goddess Statues (Triangle Islands), in the zeldaret/tww layout and names.
 * WWHD behavior verified against cking.rpx. */
#include "d/actor/d_a_obj_doguu.h"

const gc_tu_data gc_tu = {.safeString = 0x100272EC};

static DoguuHD::ResourceIndices daObjDoguu_idx_table = {26, 27, 28, 23, 24, 25, 7,  8,  9,
                                                        31, 32, 33, 14, 15, 16, 18, 19, 20};
// HD: the light color table is brighter than the GameCube table.
static DoguuHD::LightColors light_color = {400, 100, 100, 100, 400, 100, 100, 100, 500};
static DoguuHD::Actions act_table;
static dCcD_SrcCyl l_cyl_src;
static u8 l_quakePattern;
static u32 l_demoManager;
static u32 l_adjGuard;
static Mtx34 l_adjMtx;
static f32 l_adjInit[12] = {0.5f, 0.0f, 0.0f, 0.5f, 0.0f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f};
static cXyz l_vecZero;
static cXyz l_vecBaseY;
static DoguuHD::HeaderStatics l_headerStatics;
static DoguuHD::DestructorRecords l_destructorRecords;

static void* saveEvents() { return &dComIfGs_info()->mSavedata.mEvent; }

/* inlined into daObjDoguu_c::CreateHeap */
static void* doguuResource(s32 index) {
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::guest_addr("Doguu");
    name->__vtbl = gc_tu.safeString;
    dRes_control_c* controller = dComIfG_resControl();
    return reinterpret_cast<void*>((uintptr_t)doguuObjectResource(controller, name.get(), index));
}
/* inlined into daObjDoguu_c::CreateHeap */
static void assertResource(void* resource, s32 line, const char* message) {
    if (!resource)
        JUT_ASSERT_fail("d_a_obj_doguu.cpp", line, message);
}

// HD: materials keep per-model texture-matrix dirty bits and a relative resource table.
/* inlined into daObjDoguu_c::_draw */
static void updateTexMatrices(J3DModel* model) {
    auto* renderModel = reinterpret_cast<DoguuHD::Model*>(model);
    u16 count = renderModel->mMaterialCount;
    for (u16 i = 0; i < count; ++i) {
        DoguuHD::MaterialPacket* packets = renderModel->mpMaterialPackets;
        auto* material = &packets[i];
        for (s32 slot = 0; slot < 8; ++slot) {
            gabi::Local<be<s32>> index;
            *index = -1;
            auto* info =
                reinterpret_cast<be<u32>*>((uintptr_t)doguuTexMatrixInfo(material, slot, index.get()));
            if (!info || *info != 10)
                continue;
            DoguuHD::MaterialData* data = material->mpData;
            u32 relative = data->mMatrixTableOffset;
            s32 matrix = *index;
            // Relative offsets and matrix indices retain HD's wrapping guest-address arithmetic.
            u32 table = relative ? gabi::ea(&data->mMatrixTableOffset) + relative : 0;
            auto* entry = reinterpret_cast<DoguuHD::TextureMatrixEntry*>(
                (uintptr_t)(table + (u32)matrix * sizeof(DoguuHD::TextureMatrixEntry)));
            auto dirty = [&](s32 which) {
                u16 flags = material->mFlags;
                be<u32>* bits = material->mpDirtyBits;
                material->mFlags = flags | 4;
                be<u32>* word = &bits[which >> 5];
                *word = (u32)*word | (1u << (which & 31));
            };
            if (entry->mId >= 0) {
                dirty(matrix);
                data = material->mpData;
            }
            relative = data->mMatrixTableOffset;
            u16 parent = entry->mParent;
            table = relative ? gabi::ea(&data->mMatrixTableOffset) + relative : 0;
            auto* parentEntry = reinterpret_cast<DoguuHD::TextureMatrixEntry*>(
                (uintptr_t)(table + parent * sizeof(DoguuHD::TextureMatrixEntry)));
            if (parentEntry->mId >= 0)
                dirty(parent);
            auto* effect = reinterpret_cast<gptr<Mtx34>*>((uintptr_t)doguuTexEffectMatrix(material, slot));
            if (effect)
                *effect = &renderModel->mEffectMtx;
        }
    }
}

/* 0233516C */
void daObjDoguu_c::setPointLight() {
    s32 variant = field_0x894;
    const be<u32>* source = reinterpret_cast<const be<u32>*>(&mPos);
    u32 px = source[0], py = source[1], pz = source[2];
    be<u32>* destination = reinterpret_cast<be<u32>*>(&mLightInfluence.mPos);
    destination[0] = px;
    destination[1] = py;
    destination[2] = pz;
    s16 red = light_color.colors[3u * (u32)variant + 0];
    f32 power = field_0x8FC;
    mLightInfluence.mColorR = (s16)gabi::ftoi((f32)red * power);
    s16 green = light_color.colors[3u * (u32)variant + 1];
    mLightInfluence.mColorG = (s16)gabi::ftoi((f32)green * power);
    s16 blue = light_color.colors[3u * (u32)variant + 2];
    mLightInfluence.mColorB = (s16)gabi::ftoi((f32)blue * power);
    // HD: color channels and radius are scaled by the current light power.
    mLightInfluence.mPower = (f32)(s16)gabi::ftoi(300.0f * power);
    mLightInfluence.mFluctuation = 100.0f;
}

/* 02334FD4 */
BOOL CheckCreateHeap(daObjDoguu_c* actor) { return actor->CreateHeap(); }

/* 02334BE8 */
BOOL daObjDoguu_c::CreateHeap() {
    // HD: each variant directly supplies model data; material-table copies and skin deformers are absent.
    void* data = doguuResource(daObjDoguu_idx_table.bmt_vgsm[(u32)field_0x894]);
    assertResource(data, 0x162, "modelData != (0)");
    auto* main = doguuModelCreate(data, 0x80000, 0x15220202);
    field_0x6CC = main;
    if (!main)
        return FALSE;
    void* animation = doguuResource(daObjDoguu_idx_table.brk[(u32)field_0x894]);
    assertResource(animation, 0x178, "brk_doguu != 0");
    f32 rate = 1.0f;
    if (!doguuBrkInit(&mBrk, data, animation, 1, 2, 0, -1, 0, rate, 0))
        return FALSE;
    data = doguuResource(daObjDoguu_idx_table.bdl_vgsh[(u32)field_0x894]);
    assertResource(data, 0x17F, "modelData != (0)");
    auto* head = doguuModelCreate(data, 0, 0x11020203);
    field_0x6D0 = head;
    if (!head)
        return FALSE;
    animation = doguuResource(daObjDoguu_idx_table.bck[(u32)field_0x894]);
    assertResource(animation, 0x185, "bck_head != 0");
    if (!doguuBckInit(&mBckHead, data, animation, 1, 0, 0, -1, 0, rate))
        return FALSE;
    data = doguuResource(daObjDoguu_idx_table.bmt_vgsb[(u32)field_0x894]);
    assertResource(data, 0x19A, "modelData != (0)");
    auto* body = doguuModelCreate(data, 0x80000, 0x15220202);
    field_0x6D4 = body;
    if (!body)
        return FALSE;
    J3DModelData* modelData = body->mModelData;
    auto* names = reinterpret_cast<DoguuHD::JointNames*>((uintptr_t)doguuJointNames(modelData));
    u32 relative = names->mTableOffset;
    s32 joint = doguuJointIndex(
        reinterpret_cast<void*>((uintptr_t)(relative ? gabi::ea(&names->mTableOffset) + relative : 0)),
        "head1");
    if (joint >= 0)
        mHead1JntNo = joint;
    animation = doguuResource(dRes_INDEX_DOGUU_BCK_VGSBA_e);
    assertResource(animation, 0x1C5, "bck_body != 0");
    if (!doguuBckInit(&mBckBody, data, animation, 1, 0, 0, -1, 0, rate))
        return FALSE;
    data = doguuResource(daObjDoguu_idx_table.bdl_vgsp[(u32)field_0x894]);
    assertResource(data, 0x1D9, "modelData != (0)");
    auto* crystal = doguuModelCreate(data, 0, 0x11020203);
    field_0x6D8 = crystal;
    if (!crystal)
        return FALSE;
    animation = doguuResource(dRes_INDEX_DOGUU_BCK_VGSPA_e);
    assertResource(animation, 0x1DF, "bck_crystal != 0");
    if (!doguuBckInit(&mBckCrystal, data, animation, 1, 0, 0, -1, 0, rate))
        return FALSE;
    field_0x89C = 1;
    return TRUE;
}

/* 0233528C */
void daObjDoguu_c::set_mtx() {
    gptr<J3DModel>* const models[] = {&field_0x6CC, &field_0x6D4, &field_0x6D8, &field_0x6D0};
    for (auto* member : models) {
        f32 x = scale.x, y = scale.y, z = scale.z;
        J3DModel* model = *member;
        model->mBaseScale.x = x;
        model->mBaseScale.y = y;
        model->mBaseScale.z = z;
    }
    for (u32 i = 0; i < 3; ++i) {
        auto* matrix = mDoMtx_stack_c::get();
        f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
        PSMTXTrans(matrix, x, y, z);
        mDoMtx_YrotM(matrix, (s16)current.angle.y);
        f32 values[12];
        for (u32 j = 0; j < 12; ++j)
            values[j] = matrix->m[j / 4][j % 4];
        J3DModel* model = *models[i];
        for (u32 j = 0; j < 12; ++j)
            model->mBaseTransformMtx.m[j / 4][j % 4] = values[j];
    }
    J3DModel* body = field_0x6D4;
    u32 joint = mHead1JntNo;
    J3DMtxBuffer_hd* block = body->mpMtxBuffer;
    u16 flags = block->mFlags;
    u32 matrices = block->mpAnmMtx;
    J3DModel* head = field_0x6D0;
    block->mFlags = flags | 0x10;
    Mtx34* matrix = reinterpret_cast<Mtx34*>((uintptr_t)(matrices + joint * sizeof(Mtx34)));
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = matrix->m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i)
        head->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
}

/* 02335548 */
void daObjDoguu_c::CreateInit() {
    u32 model = gabi::ea((J3DModel*)field_0x6CC);
    cullMtx = model ? gabi::ea(&reinterpret_cast<J3DModel*>((uintptr_t)model)->mBaseTransformMtx) : 0;
    fopAcM_setCullSizeBox(this, -600.0f, -0.0f, -600.0f, 600.0f, 400.0f, 600.0f);
    s32 variant = field_0x894;
    cullSizeFar = 1.0f;
    mItemNo = variant == 0   ? DoguuHD::ITEM_DIN_PEARL
              : variant == 1 ? DoguuHD::ITEM_FARORE_PEARL
                             : DoguuHD::ITEM_NAYRU_PEARL;
    doguuStatusInit(&mStts, 255, 255, this);
    doguuCollisionAssign(&mCyl, &l_cyl_src);
    if (!doguuCheckItem((u8)mItemNo, 1))
        doguuCylinderSetR(&mCyl.mCyl, 50.0f);
    mCyl.mStts = &mStts;
    field_0x8FC = 0;
    setPointLight();
    dKy_plight_set(&mLightInfluence);
    if (argument > 0) {
        f32 head = (f32)mBckHead.mFrameCtrl.mStart, body = (f32)mBckBody.mFrameCtrl.mStart,
            crystal = (f32)mBckCrystal.mFrameCtrl.mStart;
        field_0x8A0 = 1;
        mBckHead.mFrameCtrl.mFrame = head;
        mBckBody.mFrameCtrl.mFrame = body;
        field_0x8A1 = 0;
        field_0x8A2 = 1;
        mBckCrystal.mFrameCtrl.mFrame = crystal;
        field_0x8AC = STATE_DONE_e;
    } else if (doguuIsEvent(saveEvents(), DoguuHD::FINISH_DIN_EVENT) &&
               doguuIsEvent(saveEvents(), DoguuHD::FINISH_FARORE_EVENT) &&
               doguuIsEvent(saveEvents(), DoguuHD::FINISH_NAYRU_EVENT)) {
        f32 head = (f32)mBckHead.mFrameCtrl.mEnd, body = (f32)mBckBody.mFrameCtrl.mEnd,
            crystal = (f32)mBckCrystal.mFrameCtrl.mEnd;
        field_0x8A0 = 1;
        field_0x8A1 = 1;
        mBckHead.mFrameCtrl.mFrame = head;
        field_0x8AC = STATE_DONE_e;
        field_0x8A2 = 0;
        mBckBody.mFrameCtrl.mFrame = body;
        mBckCrystal.mFrameCtrl.mFrame = crystal;
    } else {
        variant = field_0x894;
        field_0x8A1 = 0;
        bool placed = false;
        if (variant == 0) {
            placed = doguuIsEvent(saveEvents(), DoguuHD::FINISH_DIN_EVENT) != 0;
            if (!placed)
                variant = field_0x894;
        }
        if (!placed && variant == 1) {
            placed = doguuIsEvent(saveEvents(), DoguuHD::FINISH_FARORE_EVENT) != 0;
            if (!placed)
                variant = field_0x894;
        }
        if (!placed && variant == 2)
            placed = doguuIsEvent(saveEvents(), DoguuHD::FINISH_NAYRU_EVENT) != 0;
        field_0x8A2 = placed;
        field_0x8A0 = placed;
        field_0x8AC = placed ? STATE_WAIT_e : STATE_WAIT_PEARL_e;
    }
    eyePos.y = (f32)eyePos.y + 125.0f;
    set_mtx();
    // HD: calculate the body skeleton and refresh the head attachment during initialization.
    ((J3DModel*)field_0x6D4)->calc();
    set_mtx();
    auto event = []() { return dComIfGp_getPEvtManager(); };
    mDoguuDemo1EventIdx = doguuGetEventIndex(event(), "DOGUU_DEMO1", 255);
    mDoguuDemo2EventIdx = doguuGetEventIndex(event(), "DOGUU_DEMO2", 255);
    mDoguuDemo3EventIdx = doguuGetEventIndex(event(), "DOGUU_DEMO3", 255);
    s16 megami = doguuGetEventIndex(event(), "MEGAMI_DEMO", 255);
    bool pearl = field_0x8A0 != 0;
    mMegamiDemoEventIdx = megami;
    field_0x8A3 = pearl ? 2 : 0; // HD: this light selector has three states instead of a bool.
    mAllColRatio = 1;
}

/* 02335B6C */
s32 daObjDoguu_c::_create() {
    u32 status = actor_condition;
    if (!(status & 8)) {
        if (gabi::ea(this)) {
            daObjDoguu_c_construct(this);
            status = actor_condition;
        }
        actor_condition = status | 8;
    }
    s8 argument = this->argument;
    field_0x894 = argument > 0 ? argument - 1 : reinterpret_cast<const be<u8>*>(&mParameters)[3];
    s32 phase = doguuResourceLoad(&mPhs, "Doguu");
    if (phase == cPhs_COMPLEATE_e) {
        if (!doguuEntryHeap(this, CheckCreateHeap, 0))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}

/* 02335C60 */
BOOL daObjDoguu_c::_delete() {
    dKy_plight_cut(&mLightInfluence);
    doguuResourceDelete(&mPhs, "Doguu");
    return TRUE;
}

/* 02335CA4 */
BOOL daObjDoguu_c::_draw() {
    u32 light = doguuEnvironment();
    doguuSettingTev(reinterpret_cast<void*>((uintptr_t)light), 0, &current.pos, &tevStr);
    gptr<J3DModel>* const models[] = {&field_0x6CC, &field_0x6D0, &field_0x6D4, &field_0x6D8};
    for (auto* member : models) {
        light = doguuEnvironment();
        J3DModel* model = *member;
        doguuLightTev(reinterpret_cast<void*>((uintptr_t)light), model, &tevStr);
    }
    if (!l_adjGuard) {
        l_adjGuard = 1;
        memcpy_g(&l_adjMtx, l_adjInit, sizeof(Mtx34));
    }
    auto* camera = reinterpret_cast<DoguuHD::Camera*>((uintptr_t)doguuGetCamera());
    gabi::Local<cXyz> look, lightDirection, reflection;
    cXyz_mi(&eyePos, look.get(), &camera->mEye);
    doguuLightDirection(&tevStr.mLightPosWorld, &eyePos, lightDirection.get());
    doguuHalfAngle(look.get(), lightDirection.get(), reflection.get());
    gabi::Local<Mtx34> reflectionMatrix;
    doguuMatrixLookAt(reflectionMatrix.get(), &l_vecZero, &l_vecBaseY, reflection.get());
    auto* matrix = mDoMtx_stack_c::get();
    doguuMatrixScale(matrix, 2.0f, 2.0f, 1.0f);
    doguuMatrixConcat(matrix, &l_adjMtx, matrix);
    doguuMatrixConcat(matrix, reflectionMatrix.get(), matrix);
    mDoMtx_stack_c::get()->m[0][3] = 0;
    mDoMtx_stack_c::get()->m[1][3] = 0;
    mDoMtx_stack_c::get()->m[2][3] = 0;
    J3DModel* head = field_0x6D0;
    doguuMatrixCopy(matrix, &reinterpret_cast<DoguuHD::Model*>(head)->mEffectMtx);
    J3DModel* body = field_0x6D4;
    doguuMatrixCopy(matrix, &reinterpret_cast<DoguuHD::Model*>(body)->mEffectMtx);
    updateTexMatrices(field_0x6D0);
    updateTexMatrices(field_0x6D4);
    auto entryBck = [&](gptr<J3DModel>* member, mDoExt_bckAnm* animation) {
        J3DModel* model = *member;
        f32 value = animation->mFrameCtrl.mFrame;
        J3DModelData* data = model->mModelData;
        doguuBckEntry(animation, data, value);
        doguuModelUpdate((J3DModel*)*member, 0);
        model = *member;
        data = model->mModelData;
        auto* joint = reinterpret_cast<DoguuHD::JointTree*>((uintptr_t)(u32)data->mJointNodes);
        joint->mBck = 0;
    };
    if (field_0x8A1) {
        entryBck(&field_0x6D0, &mBckHead);
        entryBck(&field_0x6D4, &mBckBody);
    } else {
        J3DModel* model = field_0x6CC;
        f32 frame = mBrk.mFrameCtrl.mFrame;
        J3DModelData* data = model->mModelData;
        doguuBrkEntry(&mBrk, data, frame);
        doguuModelUpdate((J3DModel*)field_0x6CC, 0);
        model = field_0x6CC;
        data = model->mModelData;
        reinterpret_cast<DoguuHD::MaterialAnimation*>(data)->mBrk = 0;
    }
    if (field_0x8A0)
        entryBck(&field_0x6D8, &mBckCrystal);
    return TRUE;
}

/* 02336194 */
void daObjDoguu_c::setGoal(s32 staff) {
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    auto* value =
        reinterpret_cast<cXyz*>((uintptr_t)doguuGetSubstance(&play->play.mEvtManager, staff, "Posion", 1));
    f32 y = current.pos.y, localY = value->y;
    f32 x = current.pos.x, localZ = value->z, z = current.pos.z, localX = value->x;
    auto* matrix = mDoMtx_stack_c::get();
    PSMTXTrans(matrix, x, y, z);
    mDoMtx_YrotM(matrix, (s16)current.angle.y);
    mDoMtx_stack_c::transM(localX, localY, localZ);
    f32 gx = mDoMtx_stack_c::get()->m[0][3], gy = mDoMtx_stack_c::get()->m[1][3],
        gz = mDoMtx_stack_c::get()->m[2][3];
    field_0x8B4.y = gy;
    field_0x8B4.x = gx;
    field_0x8B4.z = gz;
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    doguuSetGoal(&play->play.mEvtManager, &field_0x8B4);
}

/* 02336294 */
void daObjDoguu_c::setPlayerAngle(s32 staff) {
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    auto* value = reinterpret_cast<const be<s16>*>(
        (uintptr_t)doguuGetSubstance(&play->play.mEvtManager, staff, "angle", 3));
    s16 offset = value ? value[1] : 0;
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    s16 angle = current.angle.y;
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    u32 vt = player->__vtbl;
    u32 fn = reinterpret_cast<DoguuHD::PlayerVtable*>((uintptr_t)vt)->mSetAngle;
    gabi::call(fn, player, &player->current.pos, (s16)(angle + offset));
}

/* 0233631C */
void daObjDoguu_c::setQuake(s32 staff) {
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    gabi::Local<cXyz> direction;
    direction->x = 0;
    direction->y = 1;
    direction->z = 0;
    doguuStartQuake(&play->play.mVibration, &l_quakePattern, 0, 63, direction.get());
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    auto* value = reinterpret_cast<const be<s32>*>(
        (uintptr_t)doguuGetSubstance(&play->play.mEvtManager, staff, "Timer", 3));
    if (!value) {
        JUT_ASSERT_fail("d_a_obj_doguu.cpp", 0x2F6, "pTimer != (0)");
        mTimer = 1;
    } else
        mTimer = *value;
}

/* 023363DC */
void daObjDoguu_c::setJDemo(s32 staff) {
    // HD: the stage helper adds wipe and transition flags.
    s16 room = reinterpret_cast<const be<s16>*>(&field_0x894)[1];
    doguuSetNextStage("ADMumi", room, 0, 8, 0, 1, 0, 0.0f);
}

/* 02336408 */
void daObjDoguu_c::privateCut() {
    auto event = []() { return dComIfGp_getPEvtManager(); };
    s32 staff = doguuGetStaff(event(), "Doguu", 0, 0);
    if (staff == -1)
        return;
    s8 action = (s8)doguuGetAct(event(), staff, &act_table, 9, 1, 0);
    mActIdx = action;
    if (action == -1) {
        doguuCutEnd(event(), staff);
        return;
    }
    bool end = false;
    if (doguuIsAdvance(event(), staff)) {
        action = mActIdx;
        switch (action) {
        case ACT_SETGOAL_e:
            setGoal(staff);
            break;
        case ACT_SETBALL_e: {
            s8 room = current.roomNo;
            field_0x8A0 = 1;
            s32 reverb = doguuReverb(room);
            mDoAud_seStart(0x288C, &current.pos, 0, reverb);
            field_0x8A3 = 1;
            break;
        }
        case ACT_SETANGLE_e:
            setPlayerAngle(staff);
            break;
        case ACT_EYEFLASH_e:
            field_0x8A2 = 1;
            break;
        case ACT_MESSAGE_OITA_e:
            doguuSubBgm(0x8000004Fu);
            break;
        case ACT_QUAKE_e:
            setQuake(staff);
            break;
        case ACT_JDEMO_e:
            setJDemo(staff);
            break;
        case ACT_SETLIGHT_e:
            field_0x8C4 = 0;
            break;
        }
    }
    action = mActIdx;
    switch (action) {
    case ACT_SETGOAL_e:
    case ACT_SETBALL_e:
    case ACT_SETANGLE_e:
    case ACT_EYEFLASH_e:
    case ACT_JDEMO_e:
        end = true;
        break;
    case ACT_MESSAGE_KOKOE_e:
    case ACT_MESSAGE_OITA_e:
        end = doguuTalk(this, 1) == 18;
        break;
    case ACT_QUAKE_e: {
        s32 timer = (u32)(s32)mTimer - 1;
        mTimer = timer;
        if (timer == 0) {
            doguuStopQuake(dComIfGp_getVibration(), 63);
            end = true;
        }
        break;
    }
    case ACT_SETLIGHT_e: {
        s32 timer = field_0x8C4;
        if (timer >= 200) {
            mAllColRatio = 1.0f;
            end = true;
        } else {
            timer = (u32)timer + 1;
            field_0x8C4 = timer;
        }
        timer = field_0x8C4;
        cLib_addCalc2(&mAllColRatio, timer < 160 ? 0.1f : 1.0f, 0.1f, 0.025f);
        timer = field_0x8C4;
        if (timer == 40) {
            mPos.y = 0;
            mPos.x = 0;
            mPos.z = 0;
            J3DModel* crystal = field_0x6D8;
            u32 joint = field_0x89C;
            J3DMtxBuffer_hd* block = crystal->mpMtxBuffer;
            u32 matrices = block->mpAnmMtx;
            block->mFlags |= 0x10;
            doguuMatrixCopy(reinterpret_cast<Mtx34*>((uintptr_t)(matrices + joint * sizeof(Mtx34))),
                            mDoMtx_stack_c::get());
            mPos.x = mDoMtx_stack_c::get()->m[0][3];
            mPos.y = mDoMtx_stack_c::get()->m[1][3];
            s32 variant = field_0x894;
            mPos.z = mDoMtx_stack_c::get()->m[2][3];
            auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            dPa_control_c* particles = play->play.mParticle;
            u32 id = variant == 0 ? 0x8434 : variant == 1 ? 0x8436 : 0x8435;
            doguuParticle(particles, 0, id, &mPos, &current.angle, 0, 255, 0, -1, 0, 0, 0);
        } else if (timer == 80) // HD: the completed light begins earlier than the GameCube frame 125.

            field_0x8A3 = 2;
        break;
    }
    default:
        end = true;
        break;
    }
    if (end)
        doguuCutEnd(event(), staff);
}

/* 02336824 */
s32 daObjDoguu_c::getFinishEventCount() {
    s32 count = 0;
    if (doguuIsEvent(saveEvents(), DoguuHD::FINISH_DIN_EVENT))
        count = 1;
    if (doguuIsEvent(saveEvents(), DoguuHD::FINISH_FARORE_EVENT))
        ++count;
    if (doguuIsEvent(saveEvents(), DoguuHD::FINISH_NAYRU_EVENT))
        ++count;
    return count;
}

/* 023368B0 */
void daObjDoguu_c::setFinishMyEvent() {
    s32 variant = field_0x894;
    s32 event = variant == 0   ? DoguuHD::FINISH_DIN_EVENT
                : variant == 1 ? DoguuHD::FINISH_FARORE_EVENT
                               : DoguuHD::FINISH_NAYRU_EVENT;
    doguuOnEvent(saveEvents(), event);
}

/* 023368F0 */
BOOL daObjDoguu_c::_execute() {
    if (field_0x8A3) {
        mPos.y = 0;
        mPos.x = 0;
        mPos.z = 0;
        J3DModel* crystal = field_0x6D8;
        u32 joint = field_0x89C;
        J3DMtxBuffer_hd* block = crystal->mpMtxBuffer;
        u32 matrices = block->mpAnmMtx;
        block->mFlags |= 0x10;
        doguuMatrixCopy(reinterpret_cast<Mtx34*>((uintptr_t)(matrices + joint * sizeof(Mtx34))),
                        mDoMtx_stack_c::get());
        mPos.x = mDoMtx_stack_c::get()->m[0][3];
        f32 y = mDoMtx_stack_c::get()->m[1][3];
        u8 active = field_0x8A3;
        mPos.y = y;
        mPos.z = mDoMtx_stack_c::get()->m[2][3];
        // HD: light jitter and smoothing are updated here instead of in setPointLight.
    f32 random = cM_rndFX(active == 1 ? 0.08f : 0.2f);
        cLib_addCalc2(&field_0x8FC, random + (active == 1 ? 0.15f : 0.8f), 0.25f, active == 1 ? 0.1f : 0.25f);
        setPointLight();
    }
    doguuCylinderSetC(&mCyl.mCyl, &current.pos);
    doguuCollisionSet(dComIfG_Ccsp(), &mCyl);
    f32 ratio = mAllColRatio;
    if (ratio != 1.0f)
        doguuSetColorRatio(ratio);
    auto event = []() { return dComIfGp_getPEvtManager(); };
    auto orderOther = [&](s16 id) { doguuOrderOther(this, id, 255, 65535, 0, 1); };
    auto orderChange = [&](s16 id) { doguuOrderChange(this, id, 0, 65535); };
    auto accepted = [&]() { return eventInfo.mCommand == dEvtCmd_INDEMO_e; };
    switch ((s32)field_0x8AC) {
    case STATE_WAIT_PEARL_e:
        if (doguuCheckItem((u8)mItemNo, 1) && doguuCheckCoHit(&mCyl)) {
            auto* object = reinterpret_cast<dCcD_GObjInf*>((uintptr_t)doguuCoHitObject(&mCyl));
            if (object) {
                dCcD_Stts* status = object->mStts;
                if (status) {
                    fopAc_ac_c* other = status->mp_actor;
                    if (other && other->mProcName == DoguuHD::fpcNm_PLAYER_e) {
                        doguuCylinderSetR(&mCyl.mCyl, 50.0f);
                        orderOther(mDoguuDemo1EventIdx);
                        field_0x8AC = STATE_ORDER_DEMO1_e;
                    }
                }
            }
        }
        break;
    case STATE_ORDER_DEMO1_e:
        if (accepted())
            field_0x8AC = STATE_DEMO1_e;
        else
            orderOther(mDoguuDemo1EventIdx);
        break;
    case STATE_DEMO1_e: {
        privateCut();
        s16 id = mDoguuDemo1EventIdx;
        if (doguuCheckEventEnd(event(), id))
            field_0x8AC = STATE_SELECT_DEMO2_e;
        break;
    }
    case STATE_SELECT_DEMO2_e:
        if (getFinishEventCount() == 0) {
            orderChange(mDoguuDemo2EventIdx);
            field_0x8AC = STATE_ORDER_DEMO2_e;
        } else {
            orderChange(mDoguuDemo3EventIdx);
            field_0x8AC = STATE_ORDER_DEMO3_e;
        }
        break;
    case STATE_ORDER_DEMO2_e:
        if (accepted())
            field_0x8AC = STATE_DEMO2_e;
        break;
    case STATE_DEMO2_e: {
        privateCut();
        s16 id = mDoguuDemo2EventIdx;
        if (doguuCheckEventEnd(event(), id))
            field_0x8AC = STATE_START_DEMO3_e;
        break;
    }
    case STATE_START_DEMO3_e:
        orderChange(mDoguuDemo3EventIdx);
        field_0x8AC = STATE_ORDER_DEMO3_e;
        break;
    case STATE_ORDER_DEMO3_e:
        if (accepted())
            field_0x8AC = STATE_DEMO3_e;
        break;
    case STATE_DEMO3_e: {
        privateCut();
        s16 id = mDoguuDemo3EventIdx;
        if (doguuCheckEventEnd(event(), id))
            field_0x8AC = STATE_FINISH_DEMO3_e;
        break;
    }
    case STATE_FINISH_DEMO3_e:
        setFinishMyEvent();
        if (getFinishEventCount() >= 3) {
            doguuOnEvent(saveEvents(), DoguuHD::TOWER_OPEN_EVENT);
            orderChange(mMegamiDemoEventIdx);
            field_0x8AC = STATE_ORDER_MEGAMI_e;
        } else {
            field_0x8AC = STATE_WAIT_e;
            auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            play->play.mEvtCtrl.mEventFlag |= 8;
        }
        break;
    case STATE_WAIT_e:
    case STATE_DONE_e:
        doguuCylinderSetR(&mCyl.mCyl, 30.0f);
        break;
    case STATE_ORDER_MEGAMI_e:
        if (accepted())
            field_0x8AC = STATE_MEGAMI_e;
        break;
    case STATE_MEGAMI_e: {
        privateCut();
        s16 id = mMegamiDemoEventIdx;
        if (doguuCheckEventEnd(event(), id))
            field_0x8AC = STATE_MEGAMI_DONE_e;
        break;
    }
    }
    if (!field_0x8A1 && field_0x8A2) {
        f32 frame = mBrk.mFrameCtrl.mFrame;
        if (frame < 1.0f) {
            s32 reverb = doguuReverb((s8)current.roomNo);
            mDoAud_seStart(0x6A31, &current.pos, 0, reverb);
        }
        mBrk.play();
    }
    u8 demo = demoActorID;
    if (demo) {
        if (demo <= 32) {
            u32 object = l_demoManager;
            if (!object) {
                JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
                object = l_demoManager;
            }
            auto* playback = reinterpret_cast<DoguuHD::DemoActor*>(
                (uintptr_t)doguuGetDemoActor(reinterpret_cast<void*>((uintptr_t)object), demo));
            if (playback) {
                if (playback->mEnable & DoguuHD::DemoActor::ENABLE_SHAPE_e)
                    mShape = playback->mShape;
                if (playback->mEnable & DoguuHD::DemoActor::ENABLE_ANGLE_e) {
                    current.angle.x = playback->mAngleX;
                    current.angle.y = playback->mAngleY;
                    current.angle.z = playback->mAngleZ;
                }
                if (playback->mEnable & DoguuHD::DemoActor::ENABLE_POSITION_e) {
                    current.pos.x = playback->mPos.x;
                    current.pos.y = playback->mPos.y;
                    current.pos.z = playback->mPos.z;
                }
                if (playback->mEnable & DoguuHD::DemoActor::ENABLE_FRAME_e) {
                    f32 frame = playback->mFrame;
                    mBckHead.mFrameCtrl.mFrame = frame;
                    mBckCrystal.mFrameCtrl.mFrame = frame;
                    mBckBody.mFrameCtrl.mFrame = frame;
                }
            }
        }
        field_0x8A1 = mShape != 0;
    }
    set_mtx();
    return TRUE;
}

/* 02336F84 */
u32 daObjDoguu_c::getMsg() {
    if (mActIdx == ACT_MESSAGE_KOKOE_e)
        return 3821;
    s32 variant = field_0x894;
    return variant == 0 ? 3822 : variant == 1 ? 3823 : 3824;
}

/* 02336FC4 */
u16 daObjDoguu_c::next_msgStatus(be<u32>* message) {
    u32 id = *message;
    if (id >= 3822 && id <= 3824) {
        if (getFinishEventCount() == 0) {
            *message = 3825;
            return 15;
        }
        if (getFinishEventCount() == 1) {
            *message = 3826;
            return 15;
        }
    }
    return 16;
}

/* 02337104 */
BOOL daObjDoguu_IsDelete(daObjDoguu_c* actor) { return TRUE; }

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 02334FD8 */
daObjDoguu_c* daObjDoguu_c_construct(daObjDoguu_c* actor) {
    if (!actor)
        actor = doguuAllocate(sizeof(daObjDoguu_c));
    if (!actor)
        return nullptr;
    doguuNpcCtor(actor);
    actor->__vtbl = DoguuHD::ActorVtable;
    doguuBrkCtor(&actor->mBrk);
    mDoExt_bckAnm* const animations[] = {&actor->mBckHead, &actor->mBckBody, &actor->mBckCrystal};
    for (auto* animation : animations) {
        auto* construction = reinterpret_cast<DoguuHD::BckConstruction*>(animation);
        doguuFrameCtrlInit(&construction->mFrameCtrl, 0);
        construction->mVtable = DoguuHD::FrameCtrlVtable;
        doguuAnimationCacheCtor(construction->mTransformCache);
        construction->mData = 0;
        construction->mCacheData = 0;
        construction->mResource = 0;
        construction->mCacheCount = 0;
        construction->mCacheVtable = DoguuHD::TransformCacheVtable;
        construction->mVtable = DoguuHD::BckVtable;
        construction->mFlags = 0;
    }
    doguuStatusBaseCtor(&actor->mStts);
    doguuGlobalStatusCtor(&actor->mStts.__vtbl_gstts);
    actor->mStts.__vtbl = DoguuHD::StatusVtable;
    actor->mStts.__vtbl_gstts = DoguuHD::GlobalStatusVtable;
    auto* cylinder = reinterpret_cast<DoguuHD::CylinderConstruction*>(&actor->mCyl);
    doguuCollisionCtor(cylinder);
    cylinder->mShapeVtable = DoguuHD::ShapeVtable;
    cylinder->mAabVtable = DoguuHD::AabVtable;
    doguuCylinderCtor(&cylinder->mCyl);
    actor->mLightInfluence.mHD20 = 1.0f;
    cylinder->mCyl.__vtbl = DoguuHD::CylinderGeometryVtable;
    cylinder->__vtbl_hitinf = DoguuHD::CylinderHitVtable;
    cylinder->mShapeVtable = DoguuHD::CylinderShapeVtable;
    return actor;
}

/* 0233705C */
void __sinit_d_a_obj_doguu_cpp() {
    l_headerStatics.mZeros[2] = 0;
    l_headerStatics.mZeros[0] = 0;
    l_headerStatics.mZeros[3] = 0;
    l_headerStatics.mZeros[1] = 0;
    doguuRegisterDtor(l_destructorRecords.records[0]);
    l_headerStatics.mMinAngle = -3.1415927410125732f;
    l_headerStatics.mMaxAngle = 3.1415927410125732f;
    doguuHeaderInitA(&l_headerStatics.mInitA);
    doguuRegisterDtor(l_destructorRecords.records[1]);
    doguuHeaderInitB(&l_headerStatics.mInitB);
    doguuRegisterDtor(l_destructorRecords.records[2]);
}

/* 023370F0 */
void SafeString_dtor(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 0233710C */
void daObjDoguu_c_dtor(daObjDoguu_c* actor, u32 flags) {
    if (!actor)
        return;
    doguuCollisionDtor(&actor->mCyl, 2);
    doguuStatusDtor(&actor->mStts, 2);
    doguuAnimationDtor(&reinterpret_cast<DoguuHD::BckConstruction*>(&actor->mBckCrystal)->mVtable, 0);
    doguuAnimationDtor(&reinterpret_cast<DoguuHD::BckConstruction*>(&actor->mBckBody)->mVtable, 0);
    doguuAnimationDtor(&reinterpret_cast<DoguuHD::BckConstruction*>(&actor->mBckHead)->mVtable, 0);
    doguuCollisionDtor(&actor->fopNpc_npc_c::mCyl, 2);
    doguuStatusDtor(&actor->fopNpc_npc_c::mStts, 2);
    doguuCircleDtor(&actor->mAcchCir.m_cir, 2);
    auto* acch = reinterpret_cast<DoguuHD::AcchConstruction*>(&actor->mObjAcch);
    acch->mVtable = DoguuHD::AcchVtable;
    acch->mSecondaryVtable = DoguuHD::AcchSecondaryVtable;
    doguuAcchDtor(&actor->mObjAcch, 0);
    doguuActorDtor(actor, 0);
    if (flags & 1)
        operator_delete(actor);
}

/* 023371E4 */
void SafeString_assureTerminationImpl() {}

/* WWHD glue
str 100272E4 in 02335B6C
str 1002735C in 023368F0
str 1002736C in 023368F0
str 100273C4 in 02334BE8
str 100273CC in 02334BE8
str 100273D4 in 02334BE8
str 100273E8 in 02334BE8
str 100273F8 in 02334BE8
str 10027408 in 02334BE8
str 1002741C in 02334BE8
str 1002742C in 02334BE8
str 10027480 in 02335548
str 1002748C in 02335548
str 10027498 in 02335548
str 100274A4 in 02335548
str 100274B0 in 02335C60
str 100274BC in 02336194
str 100274C4 in 02336294
str 100274D4 in 0233631C
str 100274DC in 0233631C
str 100274F0 in 0233631C
str 10027500 in 023363DC
str 10027510 in 02336408
data daObjDoguu_idx_table 10027378
data light_color 10027450
data act_table 101C8C3C
data l_cyl_src 101C8BF8
data l_quakePattern 100274CC
data l_demoManager 101D5FFC
data l_adjGuard 101FDB9C
data l_adjMtx 101FDBA0
data l_adjInit 101C8BA8
data l_vecZero 101FFBA8
data l_vecBaseY 101FFBC0
data l_headerStatics 10469640
data l_destructorRecords 101C8C60
ext 0200BD2C void doguuStatusBaseCtor(void*)
ext 0200E240 void doguuCollisionSet(void*, void*)
ext 02018034 void doguuCircleDtor(void*, s32)
ext 020182E0 void doguuCylinderSetC(void*, cXyz*)
ext 020184DC void doguuCylinderSetR(void*, f32)
ext 02018590 void doguuCylinderCtor(void*)
ext 024EFD9C void doguuAcchDtor(void*, s32)
ext 024F8020 u32 doguuGetCamera()
ext 02515860 void doguuStatusDtor(void*, s32)
ext 02515A70 void doguuCollisionDtor(void*, s32)
ext 02515DA0 void doguuGlobalStatusCtor(void*)
ext 02515F14 void doguuStatusInit(void*, s32, s32, fopAc_ac_c*)
ext 02515FB8 void doguuCollisionCtor(void*)
ext 025163BC u32 doguuCoHitObject(void*)
ext 02516464 s32 doguuCheckCoHit(void*)
ext 02516518 void doguuCollisionAssign(void*, void*)
ext 0252012C void doguuSetNextStage(const char*, s32, s32, s32, s32, s32, s32, f32)
ext 02520460 s32 doguuResourceLoad(request_of_phase_process_class*, const char*)
ext 025204C8 void doguuResourceDelete(request_of_phase_process_class*, const char*)
ext 02520540 s32 doguuReverb(s32)
ext 02526E70 u32 doguuGetDemoActor(void*, u32)
ext 02542D88 s32 doguuGetStaff(void*, const char*, s32, s32)
ext 02542EDC s32 doguuGetAct(void*, s32, void*, s32, s32, s32)
ext 02543280 void doguuCutEnd(void*, s32)
ext 02543714 void doguuSetGoal(void*, cXyz*)
ext 02543F10 s32 doguuGetEventIndex(void*, const char*, s32)
ext 025440C8 s32 doguuCheckEventEnd(void*, s32)
ext 025447C8 s32 doguuIsAdvance(void*, s32)
ext 0254487C u32 doguuGetSubstance(void*, s32, const char*, s32)
ext 0254DA50 s32 doguuCheckItem(s32, s32)
ext 02555D0C u32 doguuEnvironment()
ext 02560444 void doguuSetColorRatio(f32)
ext 025626A4 void doguuSettingTev(void*, s32, cXyz*, void*)
ext 02562F5C void doguuLightTev(void*, void*, void*)
ext 02563F64 void doguuLightDirection(void*, cXyz*, cXyz*)
ext 025A11EC s32 doguuTalk(void*, s32)
ext 025A1458 void doguuNpcCtor(void*)
ext 025A847C void doguuParticle(void*, s32, u32, cXyz*, csXyz*, s32, s32, s32, s32, s32, s32, s32)
ext 025B8B68 void doguuOnEvent(void*, s32)
ext 025B8B94 s32 doguuIsEvent(void*, s32)
ext 025CB4E0 void doguuStartQuake(void*, void*, s32, s32, cXyz*)
ext 025CB610 void doguuStopQuake(void*, s32)
ext 025D50BC void doguuActorDtor(void*, s32)
ext 025D63E8 s32 doguuEntryHeap(void*, DoguuHeapCallback, s32)
ext 025D7874 void doguuOrderChange(void*, s32, s32, s32)
ext 025D7A58 void doguuOrderOther(void*, s32, s32, s32, s32, s32)
ext 025E1918 void doguuSubBgm(u32)
ext 025E2DE0 void doguuModelUpdate(void*, s32)
ext 025E38E0 J3DModel* doguuModelCreate(void*, u32, u32)
ext 025E80D0 void doguuBrkCtor(void*)
ext 025E8154 s32 doguuBrkInit(void*, void*, void*, s32, s32, s32, s32, s32, f32, s32)
ext 025E83FC void doguuBrkEntry(void*, void*, f32)
ext 025E8508 s32 doguuBckInit(void*, void*, void*, s32, s32, s32, s32, s32, f32)
ext 025E86B8 void doguuBckEntry(void*, void*, f32)
ext 026066C4 u32 doguuObjectResource(void*, SafeString*, s32)
ext 0273AD10 daObjDoguu_c* doguuAllocate(u32)
ext 027DA984 void doguuAnimationCacheCtor(void*)
ext 027DF9B0 s32 doguuJointIndex(void*, const char*)
ext 027F2BC0 void doguuFrameCtrlInit(void*, s32)
ext 027F3628 void doguuAnimationDtor(void*, s32)
ext 027F68FC u32 doguuJointNames(void*)
ext 027FA678 u32 doguuTexEffectMatrix(void*, s32)
ext 027FA974 u32 doguuTexMatrixInfo(void*, s32, be<s32>*)
ext 028E90D4 void doguuMatrixCopy(void*, void*)
ext 028E9108 void doguuMatrixConcat(Mtx34*, Mtx34*, Mtx34*)
ext 028E945C void doguuMatrixScale(Mtx34*, f32, f32, f32)
ext 028E9684 void doguuMatrixLookAt(Mtx34*, cXyz*, cXyz*, cXyz*)
ext 028E9E88 void doguuHalfAngle(cXyz*, cXyz*, cXyz*)
ext 028EAB2C void doguuHeaderInitB(void*)
ext 028ED6F8 void doguuHeaderInitA(void*)
ext 028F026C void doguuRegisterDtor(void*)
*/
