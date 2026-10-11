// Beamos, actual WWHD layouts and behavior.
#include "d/actor/d_a_obj_bemos.h"
using gabi::at;
using gabi::call;
using gabi::load;
const gc_tu_data gc_tu = {.safeString = 0x10026254};

// HD: source descriptors and guarded tables retain their original guest storage.
static BemosSrcSph l_sph_src = {0, 0, 0, {}, 0, 0x1C4020,        0xF, 0x79, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 2,  0, {0.f, 0.f, 0.f}, 65.f};
static BemosSrcCyl l_cyl_src = {0, 0, 0, {}, 0, 0xFF1DFEFF,      0xF,  0x79, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 1,  0, {0.f, 0.f, 0.f}, 70.f, 210.f};
static BemosSrcCyl l_cylGuard_src = {
    0, 0, 0, {}, 0, 0, 0, 0x19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, {0.f, 0.f, 0.f}, 200.f, 400.f};
static u32 l_heap_init[3] = {0x1740, 0x1e00, 0xba0};
static u32 SHeapSize[3] = {0x0, 0x0, 0x0};
static u32 l_breakColors[4] = {0x0, 0x0, 0x0, 0x0};
static u32 l_breakColorInit[4] = {0x1023c000, 0x1436c300, 0xc0102300, 0xc3143600};
static u32 l_breakColorGuard[4] = {0x0, 0x0, 0x0, 0x0};
static u32 l_heapGuard;

// HD: load every component before storing, including when the model matrix aliases the stack.
void bemos_copy_matrix(Mtx34* dst) {
    f32 m[12];
    for (int i = 0; i < 12; i++)
        m[i] = f32(mDoMtx_stack_c::get()->m[i / 4][i % 4]);
    for (int i = 0; i < 12; i++)
        dst->m[i / 4][i % 4] = m[i];
}

/* 023252C8 */
void daBemos_c::set_mtx() {
    auto* i_this = this;
    f32 x = static_cast<f32>(i_this->scale.x), y = static_cast<f32>(i_this->scale.y),
        z = static_cast<f32>(i_this->scale.z);
    u32 model = static_cast<u32>(i_this->m2B8.v);
    at<J3DModel>(model)->mBaseScale.x = x;
    at<J3DModel>(model)->mBaseScale.y = y;
    at<J3DModel>(model)->mBaseScale.z = z;
    x = static_cast<f32>(i_this->current.pos.x);
    y = static_cast<f32>(i_this->current.pos.y);
    z = static_cast<f32>(i_this->current.pos.z);
    BemosHD_PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    BemosHD_mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), static_cast<s16>(i_this->shape_angle.x),
                           static_cast<s16>(i_this->shape_angle.y), static_cast<s16>(i_this->shape_angle.z));
    bemos_copy_matrix(&at<J3DModel>(i_this->m2B8.v)->mBaseTransformMtx);
    if (static_cast<u8>(i_this->m6B8) == 2)
        return;
    gabi::Local<cXyz> local;
    local->z = 60.f;
    local->x = 0;
    local->y = 0;
    x = static_cast<f32>(i_this->m360.x);
    y = static_cast<f32>(i_this->m360.y);
    z = static_cast<f32>(i_this->m360.z);
    model = static_cast<u32>(i_this->m2BC.v);
    at<J3DModel>(model)->mBaseScale.x = x;
    at<J3DModel>(model)->mBaseScale.y = y;
    at<J3DModel>(model)->mBaseScale.z = z;
    x = static_cast<f32>(i_this->m2A4.x);
    y = static_cast<f32>(i_this->m2A4.y);
    z = static_cast<f32>(i_this->m2A4.z);
    BemosHD_PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    s16 sum = (s16)(static_cast<s16>(i_this->m29C.y) + static_cast<s16>(i_this->current.angle.y));
    BemosHD_mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), static_cast<s16>(i_this->m29C.x), sum,
                           static_cast<s16>(i_this->m29C.z));
    BemosHD_PSMTXMultVec(mDoMtx_stack_c::get(), local.get(), &i_this->m354);
    bemos_copy_matrix(&at<J3DModel>(i_this->m2BC.v)->mBaseTransformMtx);
    if (static_cast<u8>(i_this->m6B8) != 1)
        return;
    x = static_cast<f32>(i_this->current.pos.x);
    y = static_cast<f32>(i_this->current.pos.y);
    z = static_cast<f32>(i_this->current.pos.z);
    BemosHD_PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    sum = (s16)(static_cast<s16>(i_this->m29C.y) + static_cast<s16>(i_this->current.angle.y));
    BemosHD_mDoMtx_YrotM(mDoMtx_stack_c::get(), sum);
    BemosHD_mDoMtx_stack_c_transM(0.f, 0.f, 0.f);
    bemos_copy_matrix(&at<J3DModel>(i_this->m2C0.v)->mBaseTransformMtx);
}

/* 02324424 */
s32 CheckCreateHeap(void* actor) { return static_cast<daBemos_c*>(actor)->CreateHeap(); }

/* 0232409C */
s32 daBemos_c::CreateHeap1() {
    auto* i_this = this;
    gabi::Local<SafeString> name0;
    name0->mStringTop = gabi::guest_addr("Hmos");
    name0->__vtbl = gc_tu.safeString;
    u32 data0 = BemosHD_dRes_control_c_getRes(dComIfG_resControl(), name0.get(), dRes_INDEX_HMOS_BDL_HMOS1_e);
    if (!data0)
        BemosHD_JUTAssertion_showAssert("d_a_obj_bemos.cpp", 422, "modelData != (0)");
    u32 model0 = BemosHD_mDoExt_J3DModel__create(at<void>(data0), 0, 0x11020203);
    i_this->m2B8.v = model0;
    if (!model0)
        return 0;
    gabi::Local<SafeString> name1;
    name1->mStringTop = gabi::guest_addr("Hmos");
    name1->__vtbl = gc_tu.safeString;
    u32 data1 =
        BemosHD_dRes_control_c_getRes(dComIfG_resControl(), name1.get(), dRes_INDEX_HMOS_BDL_HMOSUP1_e);
    if (!data1)
        BemosHD_JUTAssertion_showAssert("d_a_obj_bemos.cpp", 431, "modelData != (0)");
    u32 model1 = BemosHD_mDoExt_J3DModel__create(at<void>(data1), 0, 0x11020203);
    i_this->m2BC.v = model1;
    if (!model1)
        return 0;
    return 1;
}

/* 0232419C */
s32 daBemos_c::CreateHeap2() {
    auto* i_this = this;
    gabi::Local<SafeString> name0;
    name0->mStringTop = gabi::guest_addr("Hmos");
    name0->__vtbl = gc_tu.safeString;
    u32 data0 = BemosHD_dRes_control_c_getRes(dComIfG_resControl(), name0.get(), dRes_INDEX_HMOS_BDL_HMOS2_e);
    if (!data0)
        BemosHD_JUTAssertion_showAssert("d_a_obj_bemos.cpp", 453, "modelData != (0)");
    u32 model0 = BemosHD_mDoExt_J3DModel__create(at<void>(data0), 0, 0x11020203);
    i_this->m2B8.v = model0;
    if (!model0)
        return 0;
    gabi::Local<SafeString> name1;
    name1->mStringTop = gabi::guest_addr("Hmos");
    name1->__vtbl = gc_tu.safeString;
    u32 data1 =
        BemosHD_dRes_control_c_getRes(dComIfG_resControl(), name1.get(), dRes_INDEX_HMOS_BDL_HMOSUP2_e);
    if (!data1)
        BemosHD_JUTAssertion_showAssert("d_a_obj_bemos.cpp", 462, "modelData != (0)");
    u32 model1 = BemosHD_mDoExt_J3DModel__create(at<void>(data1), 0, 0x11020203);
    i_this->m2BC.v = model1;
    if (!model1)
        return 0;
    gabi::Local<SafeString> name2;
    name2->mStringTop = gabi::guest_addr("Hmos");
    name2->__vtbl = gc_tu.safeString;
    u32 data2 =
        BemosHD_dRes_control_c_getRes(dComIfG_resControl(), name2.get(), dRes_INDEX_HMOS_BDL_HMOS2T_e);
    if (!data2)
        BemosHD_JUTAssertion_showAssert("d_a_obj_bemos.cpp", 480, "modelData != (0)");
    u32 model2 = BemosHD_mDoExt_J3DModel__create(at<void>(data2), 0, 0x11020203);
    u32 upper = static_cast<u32>(i_this->m2BC.v);
    i_this->m2C0.v = model2;
    if (!upper)
        return 0;
    gabi::Local<SafeString> name3;
    name3->mStringTop = gabi::guest_addr("Hmos");
    name3->__vtbl = gc_tu.safeString;
    u32 data3 =
        BemosHD_dRes_control_c_getRes(dComIfG_resControl(), name3.get(), dRes_INDEX_HMOS_BRK_HMOS2T_e);
    i_this->m_sch_brk.v = data3;
    if (!data3) {
        BemosHD_JUTAssertion_showAssert("d_a_obj_bemos.cpp", 488, "m_sch_brk != (0)");
        data3 = static_cast<u32>(i_this->m_sch_brk.v);
    }
    return BemosHD_mDoExt_brkAnm_init(&i_this->mBrkAnm2, at<void>(data2), at<void>(data3), 1, 2, 1.f, 0, -1,
                                      0, 0) != 0;
}

/* 0232435C */
s32 daBemos_c::CreateHeap3() {
    auto* i_this = this;
    gabi::Local<SafeString> name0;
    name0->mStringTop = gabi::guest_addr("Hmos");
    name0->__vtbl = gc_tu.safeString;
    u32 data0 = BemosHD_dRes_control_c_getRes(dComIfG_resControl(), name0.get(), dRes_INDEX_HMOS_BDL_HMOS3_e);
    if (!data0)
        BemosHD_JUTAssertion_showAssert("d_a_obj_bemos.cpp", 501, "modelData != (0)");
    u32 model0 = BemosHD_mDoExt_J3DModel__create(at<void>(data0), 0, 0x11020203);
    i_this->m2B8.v = model0;
    if (!model0)
        return 0;
    return 1;
}

/* 023243F8 */
s32 daBemos_c::CreateHeap() {
    u8 type = this->m6B8;
    if (type == 0)
        return CreateHeap1();
    if (type == 1)
        return CreateHeap2();
    if (type == 2)
        return CreateHeap3();
    return 0;
}

/* 02324628 */
s32 daBemos_c::CreateInit1() {
    auto* i_this = this;
    u8 amount = u8(u32(mParameters));
    if (amount == 255)
        amount = 0;
    i_this->m320 = gabi::fmadds((f32)amount, 100.f, 500.f);
    BemosHD_dCcD_Stts_Init(&i_this->mSttsSph, 255, 255, this);
    BemosHD_dCcD_Sph_Set(&i_this->mSph, &l_sph_src);
    u32 flag = static_cast<u32>(i_this->mSph.mGObjTg.mSPrm);
    i_this->mSph.mStts.v = gabi::ea(&i_this->mSttsSph);
    i_this->mSph.mGObjTg.mSPrm = flag | 2;
    BemosHD_daBemos_c_blue_eye_wait_init(this);
    f32 y = static_cast<f32>(i_this->current.pos.y), x = static_cast<f32>(i_this->current.pos.x),
        z = static_cast<f32>(i_this->current.pos.z);
    i_this->m2A4.y = y;
    i_this->m2A4.z = z;
    i_this->m2A4.x = x;
    f32 height = f32(bemos_heightReg()) + 260.f;
    f32 eyeY = y + height, groundY = y + 50.f;
    i_this->m314.x = 0;
    i_this->m360.x = 0;
    i_this->m314.y = 0;
    i_this->mGndChk.m_pos.x = x;
    i_this->m314.z = 0;
    i_this->eyePos.x = x;
    i_this->m310 = 255;
    i_this->mGndChk.mActorPid = static_cast<u32>(i_this->mBsPcId);
    i_this->m2A4.y = eyeY;
    i_this->attention_info.position.x = x;
    i_this->attention_info.position.z = z;
    i_this->mGndChk.m_pos.y = groundY;
    i_this->attention_info.position.y = eyeY + 60.f;
    i_this->m360.y = 0.75f;
    i_this->mGndChk.m_pos.z = z;
    i_this->eyePos.y = eyeY;
    i_this->eyePos.z = z;
    i_this->m6A4 = 0;
    i_this->m360.z = 1.f;
    u32 play = BemosHD_dComIfGp_get();
    f32 ground = BemosHD_cBgS_GroundCross(&bemos_play(play).mBgS, &i_this->mGndChk);
    i_this->m6A0 = ground;
    gabi::Local<cXyz> scale;
    scale->x = 2.f;
    scale->y = 2.f;
    scale->z = 10.f;
    u32 id = static_cast<u32>(i_this->mBsPcId);
    s8 room = static_cast<s8>(i_this->tevStr.mRoomNo);
    u32 child = BemosHD_fopAcM_createChild(fpcNm_Beam_e, id, 16777216, &i_this->m2A4, room,
                                           &i_this->current.angle, scale.get(), -1, 0);
    s16 angle = static_cast<s16>(i_this->shape_angle.z);
    i_this->parentActorID = child;
    i_this->shape_angle.z = 0;
    i_this->current.angle.z = 0;
    i_this->m6BF = (u16)angle & 63;
    i_this->m6C0 = ((u16)angle >> 6) & 127;
    return 1;
}

/* 02324A2C */
s32 daBemos_c::CreateInit2() {
    auto* i_this = this;
    u32 param = static_cast<u32>(i_this->mParameters);
    u8 amount = param;
    s8 direction = (s8)(param >> 8);
    if (amount == 255)
        amount = 0;
    i_this->m320 = gabi::fmadds((f32)amount, 100.f, 500.f);
    i_this->m6B2 = direction * 250 + (direction >= 0 ? 750 : -500);
    BemosHD_dCcD_Stts_Init(&i_this->mSttsSph, 255, 255, this);
    BemosHD_dCcD_Sph_Set(&i_this->mSph, &l_sph_src);
    u32 flag = static_cast<u32>(i_this->mSph.mGObjTg.mSPrm);
    i_this->mSph.mStts.v = gabi::ea(&i_this->mSttsSph);
    i_this->mSph.mGObjTg.mSPrm = flag | 2;
    BemosHD_daBemos_c_red_eye_wait_init(this);
    f32 y = static_cast<f32>(i_this->current.pos.y), x = static_cast<f32>(i_this->current.pos.x),
        z = static_cast<f32>(i_this->current.pos.z);
    i_this->m2A4.y = y;
    i_this->m2A4.z = z;
    i_this->m2A4.x = x;
    f32 height = f32(bemos_heightReg()) + 260.f;
    f32 eyeY = y + height, groundY = y + 50.f;
    i_this->m314.x = 0;
    i_this->m360.x = 0;
    i_this->m314.y = 0;
    i_this->mGndChk.m_pos.x = x;
    i_this->m314.z = 0;
    i_this->eyePos.x = x;
    i_this->m310 = 255;
    i_this->mGndChk.mActorPid = static_cast<u32>(i_this->mBsPcId);
    i_this->m2A4.y = eyeY;
    i_this->attention_info.position.x = x;
    i_this->attention_info.position.z = z;
    i_this->mGndChk.m_pos.y = groundY;
    i_this->attention_info.position.y = eyeY + 60.f;
    i_this->m360.y = 0.75f;
    i_this->mGndChk.m_pos.z = z;
    i_this->eyePos.y = eyeY;
    i_this->eyePos.z = z;
    i_this->m6A4 = 0;
    i_this->m360.z = 1.f;
    u32 play = BemosHD_dComIfGp_get();
    f32 ground = BemosHD_cBgS_GroundCross(&bemos_play(play).mBgS, &i_this->mGndChk);
    i_this->m6A0 = ground;
    gabi::Local<cXyz> scale;
    scale->x = 2.f;
    scale->y = 2.f;
    scale->z = 10.f;
    u32 id = static_cast<u32>(i_this->mBsPcId);
    s8 room = static_cast<s8>(i_this->tevStr.mRoomNo);
    u32 child = BemosHD_fopAcM_createChild(fpcNm_Beam_e, id, 0, &i_this->m2A4, room, &i_this->current.angle,
                                           scale.get(), -1, 0);
    s16 angle = static_cast<s16>(i_this->shape_angle.z);
    i_this->parentActorID = child;
    i_this->shape_angle.z = 0;
    i_this->current.angle.z = 0;
    i_this->m6BF = (u16)angle & 63;
    i_this->m6C0 = ((u16)angle >> 6) & 127;
    return 1;
}

/* 02324DC8 */
s32 daBemos_c::CreateInit3() {
    auto* i_this = this;
    u32 param = static_cast<u32>(i_this->mParameters);
    i_this->m6B9 = param >> 8;
    i_this->m6A8 = param & 255;
    u32 path = BemosHD_dPath_GetRoomPath((param >> 16) & 255, s32(static_cast<s8>(i_this->current.roomNo)));
    i_this->m320 = 1000;
    if (static_cast<u8>(i_this->m6B9) == 255)
        i_this->m6B9 = 0;
    f32 length = 10, offset = 0;
    gabi::Local<cXyz> target, diff, flat;
    bool has = false;
    if (path) {
        u32 points = at<path_layout::dPath>(path)->m_points.v;
        f32 x = f32(at<path_layout::dPnt>(points)->m_position.x),
            y = f32(at<path_layout::dPnt>(points)->m_position.y),
            z = f32(at<path_layout::dPnt>(points)->m_position.z);
        i_this->current.pos.x = x;
        i_this->current.pos.y = y;
        i_this->current.pos.z = z;
        i_this->home.pos.x = x;
        i_this->home.pos.y = y;
        i_this->home.pos.z = z;
        for (u32 i = 0; i < 3; i++) {
            u16 n = u16(bemos_anglePart(bemos_zeroAngle(), i));
            bemos_anglePart(i_this->shape_angle, i) = n;
            bemos_anglePart(i_this->current.angle, i) = n;
        }
        points = at<path_layout::dPath>(path)->m_points.v;
        target->x = f32(at<path_layout::dPnt>(points)[1].m_position.x);
        target->y = f32(at<path_layout::dPnt>(points)[1].m_position.y);
        target->z = f32(at<path_layout::dPnt>(points)[1].m_position.z);
        BemosHD_fopAcM_createChild(
            fpcNm_Bemos_e, static_cast<u32>(i_this->mBsPcId),
            static_cast<u32>(i_this->m6A8) | 0x2fff0000 | (u32(static_cast<u8>(i_this->m6B9)) << 8),
            target.get(), s32(static_cast<s8>(i_this->tevStr.mRoomNo)), nullptr, nullptr, 2, 0);
        has = true;
    } else if (static_cast<u32>(i_this->parentActorID) != 0xffffffff) {
        gabi::Local<be<u32>> id;
        *id = static_cast<u32>(i_this->parentActorID);
        u32 parent = BemosHD_fopAcIt_Judge(
            at<void>(gabi::guest_fn((const void*)&BemosCallback_fpcSch_JudgeByID)), id.get());
        for (u32 i = 0; i < 3; i++) {
            u16 n = u16(bemos_anglePart(bemos_zeroAngle(), i));
            bemos_anglePart(i_this->shape_angle, i) = n;
            bemos_anglePart(i_this->current.angle, i) = n;
        }
        target->x = static_cast<f32>(at<fopAc_ac_c>(parent)->current.pos.x);
        target->y = static_cast<f32>(at<fopAc_ac_c>(parent)->current.pos.y);
        target->z = static_cast<f32>(at<fopAc_ac_c>(parent)->current.pos.z);
        has = true;
    }
    if (has) {
        s32 y = BemosHD_cLib_targetAngleY(&i_this->current.pos, target.get());
        i_this->m29C.y = y;
        s32 x = BemosHD_cLib_targetAngleX(&i_this->current.pos, target.get());
        s16 z = static_cast<s16>(i_this->current.angle.z);
        i_this->m29C.x = x;
        i_this->current.angle.x = x;
        i_this->shape_angle.x = x;
        s16 yy = static_cast<s16>(i_this->m29C.y);
        i_this->current.angle.y = yy;
        i_this->shape_angle.y = yy;
        i_this->shape_angle.z = z;
        BemosHD_cXyz_subtractInto(target.get(), diff.get(), &i_this->current.pos);
        flat->x = diff->x;
        flat->y = 0;
        flat->z = diff->z;
        f32 sq = BemosHD_PSVECSquareMag(flat.get());
        i_this->m320 = BemosHD_sqrtf(sq);
        sq = BemosHD_PSVECSquareMag(diff.get());
        f32 d = BemosHD_sqrtf(sq);
        length = 0.01f * (d - 180.f);
        offset = path ? 20.f : -20.f;
        i_this->m6BD = path ? 1 : 0;
    }
    BemosHD_cM3dGCyl_SetH(&i_this->mCyl.mCyl, 60.f);
    bemos_bits(i_this->m2A4.z) = static_cast<u32>(bemos_bits(i_this->current.pos.z));
    u32 ypos = static_cast<u32>(bemos_bits(i_this->current.pos.y));
    s32 ax = static_cast<s16>(i_this->current.angle.x), ay = static_cast<s16>(i_this->current.angle.y),
        az = static_cast<s16>(i_this->current.angle.z);
    u32 xpos = static_cast<u32>(bemos_bits(i_this->current.pos.x));
    bemos_bits(i_this->m2A4.y) = ypos;
    bemos_bits(i_this->m2A4.x) = xpos;
    gabi::Local<cXyz> v, out;
    v->x = 0;
    v->y = 0;
    v->z = 80;
    BemosHD_mDoMtx_ZXYrotS(mDoMtx_stack_c::get(), ax, ay, az);
    BemosHD_PSMTXMultVec(mDoMtx_stack_c::get(), v.get(), out.get());
    BemosHD_PSVECAdd(&i_this->m2A4, out.get(), &i_this->m2A4);
    i_this->m6A4 = 0;
    BemosHD_daBemos_c_yellow_eye_wait_init(this);
    gabi::Local<cXyz> scale, pos;
    scale->x = 2;
    scale->y = 2;
    scale->z = length;
    v->x = 0;
    v->y = offset;
    v->z = 40;
    BemosHD_PSMTXTrans(mDoMtx_stack_c::get(), static_cast<f32>(i_this->m2A4.x),
                       static_cast<f32>(i_this->m2A4.y), static_cast<f32>(i_this->m2A4.z));
    BemosHD_mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), s32(static_cast<s16>(i_this->m29C.x)),
                           s32(static_cast<s16>(i_this->m29C.y)), s32(static_cast<s16>(i_this->m29C.z)));
    BemosHD_PSMTXMultVec(mDoMtx_stack_c::get(), v.get(), pos.get());
    u32 flags = static_cast<u8>(i_this->m6B9) == 2 ? 0x11000000 : 0x41000000;
    u32 id = BemosHD_fopAcM_createChild(fpcNm_Beam_e, static_cast<u32>(i_this->mBsPcId), flags, pos.get(),
                                        s32(static_cast<s8>(i_this->tevStr.mRoomNo)), &i_this->m29C,
                                        scale.get(), -1, 0);
    i_this->parentActorID = id;
    i_this->m6BB = 0;
    i_this->current.angle.y = static_cast<s16>(i_this->m29C.y);
    u32 sw = static_cast<u32>(i_this->m6A8);
    if (sw != 255)
        i_this->m6BA =
            BemosHD_dSv_info_c_isSwitch(bemos_save(), sw, s32(static_cast<s8>(i_this->home.roomNo)));
    if (static_cast<u8>(i_this->m6BD) && static_cast<u8>(i_this->m6B9) != 2) {
        BemosHD_dCcD_Stts_Init(&i_this->mSttsCyl, 255, 255, this);
        BemosHD_dCcD_Cyl_Set(&i_this->mCyl, &l_cylGuard_src);
        i_this->mCyl.mStts.v = gabi::ea(&i_this->mSttsCyl);
    }
    i_this->actor_status = static_cast<u32>(i_this->actor_status) & 0xffffffc0;
    return 1;
}

/* 02325558 */
s32 daBemos_c::CreateInit() {
    auto* i_this = this;
    if (static_cast<u8>(i_this->m6B8) != 2) {
        BemosHD_dCcD_Stts_Init(&i_this->mSttsCyl, 255, 255, this);
        BemosHD_dCcD_Cyl_Set(&i_this->mCyl, &l_cyl_src);
        u32 flags = static_cast<u32>(i_this->mCyl.mGObjTg.mSPrm);
        i_this->mCyl.mStts.v = gabi::ea(&i_this->mSttsCyl);
        i_this->mCyl.mGObjTg.mSPrm = flags | 2;
    }
    i_this->m29C.x = u16(bemos_zeroAngle().x);
    i_this->m29C.y = u16(bemos_zeroAngle().y);
    u8 type = static_cast<u8>(i_this->m6B8);
    u16 z = u16(bemos_zeroAngle().z);
    i_this->m2FC.v = 0;
    i_this->m29C.z = z;
    i_this->m30C.v = 0;
    i_this->m300.v = 0;
    i_this->m308.v = 0;
    i_this->m6BC = 1;
    i_this->m304.v = 0;
    if (type == 0)
        BemosHD_daBemos_c_CreateInit1(this);
    else if (type == 1)
        BemosHD_daBemos_c_CreateInit2(this);
    else if (type == 2)
        BemosHD_daBemos_c_CreateInit3(this);
    BemosHD_daBemos_c_set_mtx(this);
    u32 model = static_cast<u32>(i_this->m2B8.v);
    if (model)
        model = gabi::ea(&at<J3DModel>(model)->mBaseTransformMtx);
    i_this->cullMtx = model;
    return cPhs_COMPLEATE_e;
}

/* 02325698 */
s32 daBemos_c::_create() {
    auto* i_this = this;
    u32 a = gabi::ea(this);
    if (!u32(l_heapGuard)) {
        l_heapGuard = 1;
        BemosHD_memcpy(SHeapSize, l_heap_init, 12);
    }
    u32 flags = static_cast<u32>(i_this->actor_condition);
    if (!(flags & 8)) {
        if (a) {
            BemosHD_fopAc_ac_c_fopAc_ac_c(this);
            i_this->__vtbl = BemosVtableHD::mActor;
            BemosHD_mDoExt_brkAnm_init(&i_this->mBrkAnm1);
            BemosHD_mDoExt_brkAnm_init(&i_this->mBrkAnm2);
            BemosHD_cCcD_Stts_construct(&i_this->mSttsCyl);
            BemosHD_dCcD_GStts_dCcD_GStts(&i_this->mSttsCyl.__vtbl_gstts);
            i_this->mSttsCyl.__vtbl = BemosVtableHD::mCcStts;
            i_this->mSttsCyl.__vtbl_gstts = BemosVtableHD::mGCcStts;
            BemosHD_dCcD_GObjInf_dCcD_GObjInf(&i_this->mCyl);
            bemos_shape(i_this->mCyl).mShapeVtable = BemosVtableHD::mShapeAttr;
            bemos_shape(i_this->mCyl).mAabVtable = BemosVtableHD::mAab;
            BemosHD_cM3dGCyl_construct(&i_this->mCyl.mCyl);
            i_this->mCyl.__vtbl_hitinf = BemosVtableHD::mGObjInf;
            bemos_shape(i_this->mCyl).mShapeVtable = BemosVtableHD::mCcCyl;
            i_this->mCyl.mCyl.__vtbl = BemosVtableHD::mCyl;
            BemosHD_cCcD_Stts_construct(&i_this->mSttsSph);
            BemosHD_dCcD_GStts_dCcD_GStts(&i_this->mSttsSph.__vtbl_gstts);
            i_this->mSttsSph.__vtbl = BemosVtableHD::mCcStts;
            i_this->mSttsSph.__vtbl_gstts = BemosVtableHD::mGCcStts;
            BemosHD_dCcD_Sph_construct(&i_this->mSph);
            BemosHD_cBgS_GndChk_cBgS_GndChk(&i_this->mGndChk);
            i_this->mGndChk.mbArrowThrough = 0;
            i_this->mGndChk.mbBombThrough = 0;
            i_this->mGndChk.mbObjThrough = 1;
            i_this->mGndChk.mbCamThrough = 0;
            i_this->mGndChk.mGrp = 1;
            i_this->mGndChk.cBgS_PolyInfo::__vtbl = BemosVtableHD::mGroundPoly;
            i_this->mGndChk.mbRopeThrough = 0;
            i_this->mGndChk.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(&i_this->mGndChk));
            i_this->mGndChk.cBgS_Chk::__vtbl = BemosVtableHD::mGroundChk;
            i_this->mGndChk.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(&i_this->mGndChk));
            i_this->mGndChk.mbLinkThrough = 0;
            i_this->mGndChk.dBgS_GrpPassChk::__vtbl = BemosVtableHD::mGroupPass;
            i_this->mGndChk.mbBoomerangThrough = 0;
            flags = static_cast<u32>(i_this->actor_condition);
            i_this->mGndChk.dBgS_PolyPassChk::__vtbl = BemosVtableHD::mPolyPass;
        }
        i_this->actor_condition = flags | 8;
    }
    s32 phase = BemosHD_dComIfG_resLoad(&i_this->mPhase, "Hmos");
    u32 type = static_cast<u32>(i_this->mParameters) >> 28;
    i_this->m6B8 = type;
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    u32 hint = u32(SHeapSize[type]);
    if (!BemosHD_fopAcM_entrySolidHeap(this, at<void>(gabi::guest_fn((const void*)&CheckCreateHeap)), hint))
        return cPhs_ERROR_e;
    return BemosHD_daBemos_c_CreateInit(this);
}

/* 02326274 */
s32 daBemos_c::blue_range_check(s32 active, csXyz* angle) {
    auto* i_this = this;
    u32 play = BemosHD_dComIfGp_get(), player = u32(bemos_play(play).mPlayerInfo[0].mpPlayer.v),
        vt = static_cast<u32>(at<fopAc_ac_c>(player)->__vtbl), f = at<BemosPlayerVtableHD>(vt)->getGroundY;
    f32 x = static_cast<f32>(at<fopAc_ac_c>(player)->current.pos.x), y = call<f32>(f, at<void>(player)),
        z = static_cast<f32>(at<fopAc_ac_c>(player)->current.pos.z);
    gabi::Local<cXyz> pos, diff, v0, v1;
    pos->x = x;
    pos->y = y;
    pos->z = z;
    BemosHD_cXyz_subtractInto(pos.get(), diff.get(), &i_this->current.pos);
    u16 yaw = static_cast<u16>(i_this->current.angle.y);
    f32 range = static_cast<f32>(i_this->m320), width = static_cast<f32>(bemos_hio().m08);
    u32 trig = yaw;
    f32 sine = f32(bemos_sinCos(trig).sin), cosine = f32(bemos_sinCos(trig).cos);
    v0->x = sine;
    v0->y = 0;
    v0->z = cosine;
    v1->x = cosine;
    v1->y = 0;
    v1->z = -sine;
    f32 forward = BemosHD_PSVECDotProduct(v0.get(), diff.get()),
        side = BemosHD_PSVECDotProduct(v1.get(), diff.get());
    f32 mag = BemosHD_sqrtf(gabi::fmadds(forward, forward, side * side)), dy = diff->y;
    if (active) {
        width += static_cast<f32>(bemos_hio().m30);
        range += static_cast<f32>(bemos_hio().m34);
    }
    if (forward < range && forward > 0 && __builtin_fabsf(side) < width && __builtin_fabsf(dy) < 250.f) {
        f32 base = static_cast<f32>(i_this->m2A4.y) - static_cast<f32>(i_this->current.pos.y);
        s32 pitch = BemosHD_cM_atan2s(dy + base, mag);
        angle->x = pitch;
        i_this->m6A4 = 1;
        return 1;
    }
    i_this->m6A4 = 0;
    return 0;
}

/* 02326460 */
s32 daBemos_c::red_range_check(s32 active, csXyz* angle) {
    auto* i_this = this;
    u32 play = BemosHD_dComIfGp_get(), player = u32(bemos_play(play).mPlayerInfo[0].mpPlayer.v),
        vt = static_cast<u32>(at<fopAc_ac_c>(player)->__vtbl), f = at<BemosPlayerVtableHD>(vt)->getGroundY;
    f32 x = static_cast<f32>(at<fopAc_ac_c>(player)->current.pos.x), y = call<f32>(f, at<void>(player)),
        z = static_cast<f32>(at<fopAc_ac_c>(player)->current.pos.z);
    gabi::Local<cXyz> pos, diff, flat, move;
    pos->x = x;
    pos->y = y;
    pos->z = z;
    BemosHD_cXyz_subtractInto(pos.get(), diff.get(), &i_this->current.pos);
    flat->x = diff->x;
    flat->y = 0;
    flat->z = diff->z;
    f32 range = static_cast<f32>(i_this->m320), sq = BemosHD_PSVECSquareMag(flat.get()),
        dist = BemosHD_sqrtf(sq);
    s32 direction = BemosHD_cM_atan2s(f32(diff->x), f32(diff->z));
    s16 relative = s16(direction - static_cast<s16>(i_this->current.angle.y)),
        delta = s16(relative - static_cast<s16>(i_this->m29C.y));
    s32 absdelta = delta < 0 ? -s32(delta) : s32(delta);
    bool eligible = true;
    if (!static_cast<u8>(i_this->m6BC)) {
        BemosHD_cXyz_subtractInto(&at<fopAc_ac_c>(player)->current.pos, move.get(), &i_this->m36C);
        sq = BemosHD_PSVECSquareMag(move.get());
        f32 moved = BemosHD_sqrtf(sq);
        if (moved > 5.f)
            i_this->m6BC = 1;
        else if (absdelta > 0x1000)
            i_this->m6BC = 1;
        else
            eligible = false;
    }
    if (eligible) {
        if (active)
            range += static_cast<f32>(bemos_hio().m2C);
        if ((absdelta < 0x1000 || active == 1) && dist < range) {
            f32 base = static_cast<f32>(i_this->m2A4.y) - static_cast<f32>(i_this->current.pos.y);
            s32 pitch = BemosHD_cM_atan2s(base - f32(diff->y), dist);
            angle->x = pitch;
            angle->y = relative;
            i_this->m6A4 = 1;
            return 1;
        }
    }
    i_this->m6A4 = 0;
    return 0;
}

/* 0232665C */
s32 daBemos_c::yellow_range_check(s32 range, void* angle) {
    this->m6A4 = 1;
    return 1;
}

/* 02324428 */
s32 daBemos_c::blue_eye_wait_init() {
    auto* i_this = this;
    if (!static_cast<u32>(i_this->m2FC.v)) {
        gabi::Local<cXyz> pos;
        f32 x = static_cast<f32>(i_this->current.pos.x), y = static_cast<f32>(i_this->current.pos.y),
            z = static_cast<f32>(i_this->current.pos.z);
        pos->x = x;
        pos->z = z;
        pos->y = y + 210.f;
        u32 play = BemosHD_dComIfGp_get();
        u32 emitter = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0,
                                                dPa_name::ID_AK_SN_BMOSWAIT00, pos.get(), nullptr, 0, 0xff, 0,
                                                -1, nullptr, 0, 0);
        i_this->m2FC.v = emitter;
        if (emitter) {
            at<BemosEmitterHD>(emitter)->mGlobalPrmColor.r = 160;
            at<BemosEmitterHD>(emitter)->mGlobalPrmColor.g = 180;
            at<BemosEmitterHD>(emitter)->mGlobalPrmColor.b = 255;
            u32 e = static_cast<u32>(i_this->m2FC.v);
            at<BemosEmitterHD>(e)->mGlobalEnvColor.g = 155;
            at<BemosEmitterHD>(e)->mGlobalEnvColor.r = 24;
            at<BemosEmitterHD>(e)->mGlobalEnvColor.b = 212;
        }
    }
    if (!static_cast<u32>(i_this->m300.v)) {
        u32 angle = static_cast<u16>(i_this->current.angle.y), trig = angle;
        f32 y = static_cast<f32>(i_this->m2A4.y), x = static_cast<f32>(i_this->m2A4.x);
        f32 sine = f32(bemos_sinCos(trig).sin);
        gabi::Local<cXyz> pos;
        pos->y = y;
        x = gabi::fmadds(60.f, sine, x);
        f32 z = static_cast<f32>(i_this->m2A4.z), cosine = f32(bemos_sinCos(trig).cos);
        z = gabi::fmadds(60.f, cosine, z);
        pos->x = x;
        pos->z = z;
        u32 play = BemosHD_dComIfGp_get();
        u32 e = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0,
                                          dPa_name::ID_AK_SN_BMOSEYE00, pos.get(), nullptr, 0, 0xff, 0, -1,
                                          nullptr, 0, 0);
        i_this->m300.v = e;
        if (e) {
            at<BemosEmitterHD>(e)->mGlobalPrmColor.r = 20;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.g = 54;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.b = 195;
        }
    }
    u32 old = static_cast<u32>(i_this->m304.v);
    if (old) {
        u32 flags = static_cast<u32>(at<BemosEmitterHD>(old)->mFlags);
        at<BemosEmitterHD>(old)->mMaxFrame = 0xffffffff;
        at<BemosEmitterHD>(old)->mFlags = flags | 1;
        i_this->m304.v = 0;
    }
    i_this->m290.d = 0;
    i_this->m360.y = 0.75f;
    i_this->m360.z = 1.f;
    i_this->m290.i = 0xffff;
    i_this->m360.x = 0.f;
    i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_blue_eye_wait);
    return 1;
}

/* 023267CC */
s32 daBemos_c::blue_eye_wait() {
    auto* i_this = this;
    u32 a = gabi::ea(this);
    if (BemosHD_daBemos_c_getBeamActor(this)) {
        gabi::Local<csXyz> angle;
        if (BemosHD_daBemos_c_blue_range_check(this, static_cast<u32>(i_this->m6A4), angle.get())) {
            BemosHD_daBemos_c_blue_eye_charge_init(this);
            return 1;
        }
        BemosHD_cLib_chaseUC(&i_this->m310, 255, 15);
        u8 alpha = static_cast<u8>(i_this->m310);
        u32 e = static_cast<u32>(i_this->m300.v);
        if (alpha == 255)
            i_this->attention_info.flags = 0;
        if (e) {
            f32 y = static_cast<f32>(i_this->m354.y), x = static_cast<f32>(i_this->m354.x),
                z = static_cast<f32>(i_this->m354.z);
            u8 kind = static_cast<u8>(at<BemosEmitterHD>(e)->mCoordinateSystem);
            at<BemosEmitterHD>(e)->mGlobalTranslation.x = x;
            at<BemosEmitterHD>(e)->mGlobalTranslation.y = y;
            at<BemosEmitterHD>(e)->mGlobalTranslation.z = z;
            if (kind >= 7)
                at<BemosEmitterHD>(e)->mGlobalTranslation.y =
                    -static_cast<f32>(at<BemosEmitterHD>(e)->mGlobalTranslation.y);
            at<BemosEmitterHD>(i_this->m300.v)->mGlobalPrmColor.a = i_this->m310;
        }
    }
    return 0;
}

/* 0232666C */
s32 daBemos_c::blue_eye_charge_init() {
    auto* i_this = this;
    u32 existing = static_cast<u32>(i_this->m304.v);
    i_this->attention_info.flags = 1;
    if (!existing) {
        u32 p = BemosHD_dComIfGp_get();
        u32 e = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(p).mParticle.v)), 0,
                                          dPa_name::ID_AK_SN_BMOSBALL00, &i_this->m2A4, nullptr, 0, 0xff, 0,
                                          -1, nullptr, 0, 0);
        i_this->m304.v = e;
        if (e) {
            at<BemosEmitterHD>(e)->mGlobalPrmColor.r = 16;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.g = 35;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.b = 192;
        }
    }
    u32 old = static_cast<u32>(i_this->m2FC.v);
    if (old) {
        u32 flags = static_cast<u32>(at<BemosEmitterHD>(old)->mFlags);
        at<BemosEmitterHD>(old)->mMaxFrame = 0xffffffff;
        at<BemosEmitterHD>(old)->mFlags = flags | 1;
        i_this->m2FC.v = 0;
    }
    s16 index = static_cast<s16>(i_this->m290.i);
    i_this->m6AC = 0;
    bool waiting = false;
    if (index == -1) {
        if (index == 0)
            waiting = true;
        else if (static_cast<s16>(i_this->m290.d) == 0 &&
                 static_cast<u32>(i_this->m290.f) ==
                     gabi::guest_fn((const void*)&BemosCallback_blue_eye_wait))
            waiting = true;
    }
    if (waiting) {
        i_this->m6AE = 0;
        s16 timer = static_cast<s16>(bemos_hio().m1C);
        i_this->m290.d = 0;
        i_this->m6B0 = timer;
        i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_blue_eye_charge);
        i_this->m290.i = 0xffff;
    } else {
        s16 timer = static_cast<s16>(bemos_hio().m1E);
        i_this->m6AE = timer;
        timer = static_cast<s16>(bemos_hio().m1E);
        i_this->m290.d = 0;
        i_this->m6B0 = timer;
        i_this->m290.i = 0xffff;
        i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_blue_eye_charge);
    }
    return 1;
}

/* 023268D0 */
s32 daBemos_c::blue_eye_charge() {
    auto* i_this = this;
    u32 a = gabi::ea(this);
    gabi::Local<csXyz> angle;
    bool on = BemosHD_daBemos_c_blue_range_check(this, static_cast<u32>(i_this->m6A4), angle.get()) != 0;
    if (on) {
        BemosHD_cLib_chaseF(&i_this->m360.x, 1.f, static_cast<f32>(bemos_hio().m14));
        BemosHD_cLib_chaseF(&i_this->m360.y, 1.f, 0.25f * static_cast<f32>(bemos_hio().m14));
        s16 count = static_cast<s16>(i_this->m6AE), limit = static_cast<s16>(i_this->m6B0);
        if (count > limit) {
            if (!static_cast<u32>(i_this->m30C.v)) {
                u32 play = BemosHD_dComIfGp_get();
                u32 e = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0,
                                                  dPa_name::ID_AK_SN_LASERROOT00, &i_this->m354, nullptr, 0,
                                                  255, 0, -1, nullptr, 0, 0);
                i_this->m30C.v = e;
            }
            if (static_cast<f32>(i_this->m360.x) == 1.f) {
                s16 timer = s16(static_cast<s16>(i_this->m6AC) + 1);
                i_this->m6AC = timer;
                if (timer >= 5) {
                    u32 e = static_cast<u32>(i_this->m30C.v);
                    if (e) {
                        u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
                        at<BemosEmitterHD>(e)->mMaxFrame = -1;
                        at<BemosEmitterHD>(e)->mFlags = flags | 1;
                        i_this->m30C.v = 0;
                    }
                    BemosHD_daBemos_blue_eye_search_init(this);
                }
            }
        } else
            i_this->m6AE = count + 1;
        BemosHD_cLib_chaseUC(&i_this->m310, 0, 30);
        BemosHD_cLib_addCalcAngleS2(&i_this->m29C, s32(s16(angle->x)), 4, 0x400);
    } else {
        s16 count = static_cast<s16>(i_this->m6AE);
        if (count <= 0) {
            BemosHD_cLib_chaseF(&i_this->m360.x, 0.f, static_cast<f32>(bemos_hio().m18));
            BemosHD_cLib_chaseF(&i_this->m360.y, 0.75f, 0.25f * static_cast<f32>(bemos_hio().m18));
            if (static_cast<f32>(i_this->m360.x) == 0) {
                u32 e = static_cast<u32>(i_this->m30C.v);
                if (e) {
                    u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
                    at<BemosEmitterHD>(e)->mMaxFrame = -1;
                    at<BemosEmitterHD>(e)->mFlags = flags | 1;
                    i_this->m30C.v = 0;
                }
                BemosHD_daBemos_c_blue_eye_wait_init(this);
            }
            BemosHD_cLib_chaseUC(&i_this->m310, 255, 15);
            BemosHD_cLib_addCalcAngleS2(&i_this->m29C, 0, 4, 0x400);
            s16 timer = static_cast<s16>(i_this->m6AC);
            if (timer > 0)
                i_this->m6AC = timer - 1;
        } else
            i_this->m6AE = count - 1;
    }
    f32 t = f32(static_cast<s16>(i_this->m6AC)) - 2.5f, scale = gabi::fnmsubs(t, t, 6.25f) * 1.6f;
    i_this->m314.x = scale;
    i_this->m314.y = scale;
    i_this->m314.z = scale;
    BemosHD_cM3dGSph_SetC(&i_this->mSph.mSph, &i_this->m2A4);
    u32 play = BemosHD_dComIfGp_get();
    BemosHD_cCcS_Set(&bemos_play(play).mCcS, &i_this->mSph);
    u32 e = static_cast<u32>(i_this->m300.v);
    if (e) {
        f32 y = static_cast<f32>(i_this->m354.y), z = static_cast<f32>(i_this->m354.z),
            x = static_cast<f32>(i_this->m354.x);
        u8 kind = static_cast<u8>(at<BemosEmitterHD>(e)->mCoordinateSystem);
        at<BemosEmitterHD>(e)->mGlobalTranslation.x = x;
        at<BemosEmitterHD>(e)->mGlobalTranslation.y = y;
        at<BemosEmitterHD>(e)->mGlobalTranslation.z = z;
        if (kind >= 7)
            at<BemosEmitterHD>(e)->mGlobalTranslation.y =
                -static_cast<f32>(at<BemosEmitterHD>(e)->mGlobalTranslation.y);
        at<BemosEmitterHD>(i_this->m300.v)->mGlobalPrmColor.a = i_this->m310;
    }
    e = static_cast<u32>(i_this->m30C.v);
    if (e) {
        f32 x = static_cast<f32>(i_this->m354.x), z = static_cast<f32>(i_this->m354.z),
            y = static_cast<f32>(i_this->m354.y);
        s32 rz = static_cast<s16>(i_this->m29C.z), rx = static_cast<s16>(i_this->m29C.x),
            ry = s16(static_cast<s16>(i_this->m29C.y) + static_cast<s16>(i_this->current.angle.y));
        u8 kind = static_cast<u8>(at<BemosEmitterHD>(e)->mCoordinateSystem);
        at<BemosEmitterHD>(e)->mGlobalTranslation.x = x;
        at<BemosEmitterHD>(e)->mGlobalTranslation.y = y;
        at<BemosEmitterHD>(e)->mGlobalTranslation.z = z;
        if (kind >= 7)
            at<BemosEmitterHD>(e)->mGlobalTranslation.y =
                -static_cast<f32>(at<BemosEmitterHD>(e)->mGlobalTranslation.y);
        BemosHD_JPAGetXYZRotateMtx(rx, ry, rz, &at<BemosEmitterHD>(i_this->m30C.v)->mGlobalRotation);
        x = static_cast<f32>(i_this->m314.x);
        y = static_cast<f32>(i_this->m314.y);
        e = static_cast<u32>(i_this->m30C.v);
        z = static_cast<f32>(i_this->m314.z);
        at<BemosEmitterHD>(e)->mGlobalScale.x = x;
        at<BemosEmitterHD>(e)->mGlobalScale.y = y;
        at<BemosEmitterHD>(e)->mGlobalScale.z = z;
        at<BemosEmitterHD>(e)->mGlobalPrmScale.x = x;
        at<BemosEmitterHD>(e)->mGlobalPrmScale.y = y;
        at<BemosEmitterHD>(e)->mGlobalPrmScale.z = z;
    }
    return 0;
}

/* 023268A8 */
s32 daBemos_c::blue_eye_search_init() {
    auto* i_this = this;
    i_this->m290.d = 0;
    i_this->m290.i = 0xffff;
    i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_blue_eye_search);
    i_this->m6AC = 0;
    return 1;
}

/* 02326E4C */
s32 daBemos_c::blue_eye_search() {
    auto* i_this = this;
    u32 beam = BemosHD_daBemos_c_getBeamActor(this);
    if (!beam)
        return 0;
    gabi::Local<csXyz> angle, beamangle;
    bool on = BemosHD_daBemos_c_blue_range_check(this, static_cast<u32>(i_this->m6A4), angle.get()) != 0;
    if (on) {
        BemosHD_cLib_addCalcAngleS2(&i_this->m29C, s32(s16(angle->x)), 4, 0x400);
        s16 z = static_cast<s16>(i_this->m29C.z),
            yaw = s16(static_cast<s16>(i_this->m29C.y) + static_cast<s16>(i_this->current.angle.y));
        beamangle->x = static_cast<s16>(i_this->m29C.x);
        beamangle->z = z;
        if (!static_cast<u32>(at<daBeam_c>(beam)->m690.v)) {
            u32 play = BemosHD_dComIfGp_get();
            u32 e = BemosHD_dPa_control_c_set(
                at<void>(u32(bemos_play(play).mParticle.v)), 0, dPa_name::ID_AK_SN_LASERROOT00,
                &at<daBeam_c>(beam)->current.pos, nullptr, 0, 255, 0, -1, nullptr, 0, 0);
            at<daBeam_c>(beam)->m690.v = e;
        }
        s16 state = static_cast<s16>(at<daBeam_c>(beam)->m5F4);
        at<daBeam_c>(beam)->m5A8 = 0;
        f32 width = 5;
        if (!state) {
            f32 old = static_cast<f32>(at<daBeam_c>(beam)->m588);
            if (old < 5.f)
                width = old + 1.f;
            else {
                at<daBeam_c>(beam)->m5A8 = 0;
                at<daBeam_c>(beam)->m5F4 = 1;
            }
        }
        at<daBeam_c>(beam)->m588 = width;
        for (u32 i = 0; i < 3; i++)
            bemos_positionWord(at<daBeam_c>(beam)->current.pos, i) = u32(bemos_positionWord(i_this->m354, i));
        at<daBeam_c>(beam)->current.angle.y = yaw;
        at<daBeam_c>(beam)->current.angle.x = beamangle->x;
        at<daBeam_c>(beam)->current.angle.z = beamangle->z;
    } else {
        u32 e = static_cast<u32>(at<daBeam_c>(beam)->m690.v);
        if (e) {
            u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
            at<BemosEmitterHD>(e)->mMaxFrame = -1;
            at<BemosEmitterHD>(e)->mFlags = flags | 1;
            at<daBeam_c>(beam)->m690.v = 0;
        }
        if (static_cast<s16>(at<daBeam_c>(beam)->m5F4) == 1) {
            f32 old = static_cast<f32>(at<daBeam_c>(beam)->m588);
            if (old < 5.f)
                at<daBeam_c>(beam)->m588 = old + 1.f;
            f32 height = static_cast<f32>(at<daBeam_c>(beam)->m5A8);
            if (height < 4.f)
                at<daBeam_c>(beam)->m5A8 = height + 1.f;
            else {
                at<daBeam_c>(beam)->m5A8 = 0;
                at<daBeam_c>(beam)->m588 = 0;
                at<daBeam_c>(beam)->m5F4 = 0;
                BemosHD_daBemos_c_blue_eye_charge_init(this);
            }
        } else {
            at<daBeam_c>(beam)->m588 = 0;
            at<daBeam_c>(beam)->m5A8 = 0;
            BemosHD_daBemos_c_blue_eye_charge_init(this);
        }
    }
    BemosHD_cM3dGSph_SetC(&i_this->mSph.mSph, &i_this->m2A4);
    u32 play = BemosHD_dComIfGp_get();
    BemosHD_cCcS_Set(&bemos_play(play).mCcS, &i_this->mSph);
    return 0;
}

/* 02324830 */
s32 daBemos_c::red_eye_wait_init() {
    auto* i_this = this;
    if (!static_cast<u32>(i_this->m2FC.v)) {
        gabi::Local<cXyz> pos;
        f32 x = static_cast<f32>(i_this->current.pos.x), y = static_cast<f32>(i_this->current.pos.y),
            z = static_cast<f32>(i_this->current.pos.z);
        pos->x = x;
        pos->z = z;
        pos->y = y + 210.f;
        u32 play = BemosHD_dComIfGp_get();
        u32 emitter = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0,
                                                dPa_name::ID_AK_SN_BMOSWAIT00, pos.get(), nullptr, 0, 0xff, 0,
                                                -1, nullptr, 0, 0);
        i_this->m2FC.v = emitter;
        if (emitter) {
            at<BemosEmitterHD>(emitter)->mGlobalPrmColor.r = 255;
            at<BemosEmitterHD>(emitter)->mGlobalPrmColor.g = 128;
            at<BemosEmitterHD>(emitter)->mGlobalPrmColor.b = 200;
            u32 e = static_cast<u32>(i_this->m2FC.v);
            at<BemosEmitterHD>(e)->mGlobalEnvColor.r = 212;
            at<BemosEmitterHD>(e)->mGlobalEnvColor.b = 255;
            at<BemosEmitterHD>(e)->mGlobalEnvColor.g = 24;
        }
    }
    if (!static_cast<u32>(i_this->m300.v)) {
        u32 angle = static_cast<u16>(i_this->current.angle.y), trig = angle;
        f32 y = static_cast<f32>(i_this->m2A4.y), x = static_cast<f32>(i_this->m2A4.x);
        f32 sine = f32(bemos_sinCos(trig).sin);
        gabi::Local<cXyz> pos;
        pos->y = y;
        x = gabi::fmadds(60.f, sine, x);
        f32 z = static_cast<f32>(i_this->m2A4.z), cosine = f32(bemos_sinCos(trig).cos);
        z = gabi::fmadds(60.f, cosine, z);
        pos->x = x;
        pos->z = z;
        u32 play = BemosHD_dComIfGp_get();
        u32 e = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0,
                                          dPa_name::ID_AK_SN_BMOSEYE00, pos.get(), nullptr, 0, 0xff, 0, -1,
                                          nullptr, 0, 0);
        i_this->m300.v = e;
        if (e) {
            at<BemosEmitterHD>(e)->mGlobalPrmColor.r = 195;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.g = 20;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.b = 54;
        }
    }
    u32 old = static_cast<u32>(i_this->m304.v);
    if (old) {
        u32 flags = static_cast<u32>(at<BemosEmitterHD>(old)->mFlags);
        at<BemosEmitterHD>(old)->mMaxFrame = 0xffffffff;
        at<BemosEmitterHD>(old)->mFlags = flags | 1;
        i_this->m304.v = 0;
    }
    i_this->m290.d = 0;
    i_this->m360.y = 0.75f;
    i_this->m360.z = 1.f;
    i_this->m290.i = 0xffff;
    i_this->m360.x = 0.f;
    i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_red_eye_wait);
    return 1;
}

/* 0232728C */
s32 daBemos_c::red_eye_wait() {
    auto* i_this = this;
    u32 a = gabi::ea(this), beam = BemosHD_daBemos_c_getBeamActor(this);
    BemosHD_mDoExt_baseAnm_play(&i_this->mBrkAnm2);
    if (!beam)
        return 0;
    gabi::Local<csXyz> angle;
    if (BemosHD_daBemos_c_red_range_check(this, static_cast<u32>(i_this->m6A4), angle.get())) {
        BemosHD_daBemos_c_red_eye_charge_init(this);
        return 1;
    }
    BemosHD_cLib_addCalcAngleS2(&i_this->m29C, 0, 4, 0x400);
    s16 delta = static_cast<s16>(i_this->m6B2), old = static_cast<s16>(i_this->m29C.y);
    i_this->m29C.y = old + delta;
    BemosHD_cLib_chaseUC(&i_this->m310, 255, 15);
    u8 alpha = static_cast<u8>(i_this->m310);
    u32 e = static_cast<u32>(i_this->m300.v);
    if (alpha == 255)
        i_this->attention_info.flags = 0;
    if (e) {
        f32 y = static_cast<f32>(i_this->m354.y), x = static_cast<f32>(i_this->m354.x),
            z = static_cast<f32>(i_this->m354.z);
        u8 kind = static_cast<u8>(at<BemosEmitterHD>(e)->mCoordinateSystem);
        at<BemosEmitterHD>(e)->mGlobalTranslation.x = x;
        at<BemosEmitterHD>(e)->mGlobalTranslation.y = y;
        at<BemosEmitterHD>(e)->mGlobalTranslation.z = z;
        if (kind >= 7)
            at<BemosEmitterHD>(e)->mGlobalTranslation.y =
                -static_cast<f32>(at<BemosEmitterHD>(e)->mGlobalTranslation.y);
        at<BemosEmitterHD>(i_this->m300.v)->mGlobalPrmColor.a = i_this->m310;
    }
    return 1;
}

/* 0232712C */
s32 daBemos_c::red_eye_charge_init() {
    auto* i_this = this;
    u32 existing = static_cast<u32>(i_this->m304.v);
    i_this->attention_info.flags = 1;
    if (!existing) {
        u32 p = BemosHD_dComIfGp_get();
        u32 e = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(p).mParticle.v)), 0,
                                          dPa_name::ID_AK_SN_BMOSBALL00, &i_this->m2A4, nullptr, 0, 0xff, 0,
                                          -1, nullptr, 0, 0);
        i_this->m304.v = e;
        if (e) {
            at<BemosEmitterHD>(e)->mGlobalPrmColor.r = 192;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.g = 16;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.b = 35;
        }
    }
    u32 old = static_cast<u32>(i_this->m2FC.v);
    if (old) {
        u32 flags = static_cast<u32>(at<BemosEmitterHD>(old)->mFlags);
        at<BemosEmitterHD>(old)->mMaxFrame = 0xffffffff;
        at<BemosEmitterHD>(old)->mFlags = flags | 1;
        i_this->m2FC.v = 0;
    }
    s16 index = static_cast<s16>(i_this->m290.i);
    i_this->m6AC = 0;
    bool waiting = false;
    if (index == -1) {
        if (index == 0)
            waiting = true;
        else if (static_cast<s16>(i_this->m290.d) == 0 &&
                 static_cast<u32>(i_this->m290.f) == gabi::guest_fn((const void*)&BemosCallback_red_eye_wait))
            waiting = true;
    }
    if (waiting) {
        i_this->m6AE = 0;
        s16 timer = static_cast<s16>(bemos_hio().m28);
        i_this->m290.d = 0;
        i_this->m6B0 = timer;
        i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_red_eye_charge);
        i_this->m290.i = 0xffff;
    } else {
        s16 timer = static_cast<s16>(bemos_hio().m2A);
        i_this->m6AE = timer;
        timer = static_cast<s16>(bemos_hio().m2A);
        i_this->m290.d = 0;
        i_this->m6B0 = timer;
        i_this->m290.i = 0xffff;
        i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_red_eye_charge);
    }
    return 1;
}

/* 023273F0 */
s32 daBemos_c::red_eye_charge() {
    auto* i_this = this;
    u32 a = gabi::ea(this);
    BemosHD_mDoExt_baseAnm_play(&i_this->mBrkAnm2);
    gabi::Local<csXyz> angle;
    bool on = BemosHD_daBemos_c_red_range_check(this, static_cast<u32>(i_this->m6A4), angle.get()) != 0;
    if (on) {
        BemosHD_cLib_chaseF(&i_this->m360.x, 1.f, static_cast<f32>(bemos_hio().m20));
        BemosHD_cLib_chaseF(&i_this->m360.y, 1.f, 0.25f * static_cast<f32>(bemos_hio().m20));
        s16 count = static_cast<s16>(i_this->m6AE), limit = static_cast<s16>(bemos_hio().m28);
        if (count > limit) {
            BemosHD_cLib_addCalcAngleS(&i_this->m29C, s32(s16(angle->x)), 2, 0x400, 0);
            if (!static_cast<u32>(i_this->m30C.v)) {
                u32 play = BemosHD_dComIfGp_get();
                u32 e = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0,
                                                  dPa_name::ID_AK_SN_LASERROOT00, &i_this->m354, nullptr, 0,
                                                  255, 0, -1, nullptr, 0, 0);
                i_this->m30C.v = e;
            }
            if (static_cast<f32>(i_this->m360.x) == 1.f) {
                s16 timer = s16(static_cast<s16>(i_this->m6AC) + 1);
                i_this->m6AC = timer;
                if (timer >= 5) {
                    u32 e = static_cast<u32>(i_this->m30C.v);
                    if (e) {
                        u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
                        at<BemosEmitterHD>(e)->mMaxFrame = -1;
                        at<BemosEmitterHD>(e)->mFlags = flags | 1;
                        i_this->m30C.v = 0;
                    }
                    BemosHD_daBemos_red_eye_search_init(this);
                }
            }
        } else
            i_this->m6AE = count + 1;
        BemosHD_cLib_chaseUC(&i_this->m310, 0, 30);

    } else {
        s16 count = static_cast<s16>(i_this->m6AE);
        if (count <= 0) {
            BemosHD_cLib_chaseF(&i_this->m360.x, 0.f, static_cast<f32>(bemos_hio().m24));
            BemosHD_cLib_chaseF(&i_this->m360.y, 0.75f, 0.25f * static_cast<f32>(bemos_hio().m24));
            if (static_cast<f32>(i_this->m360.x) == 0) {
                u32 e = static_cast<u32>(i_this->m30C.v);
                if (e) {
                    u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
                    at<BemosEmitterHD>(e)->mMaxFrame = -1;
                    at<BemosEmitterHD>(e)->mFlags = flags | 1;
                    i_this->m30C.v = 0;
                }
                BemosHD_daBemos_c_red_eye_wait_init(this);
            }
            BemosHD_cLib_chaseUC(&i_this->m310, 255, 15);
            BemosHD_cLib_addCalcAngleS2(&i_this->m29C, 0, 4, 0x400);
            s16 timer = static_cast<s16>(i_this->m6AC);
            if (timer > 0)
                i_this->m6AC = timer - 1;
        } else
            i_this->m6AE = count - 1;
    }
    BemosHD_cM3dGSph_SetC(&i_this->mSph.mSph, &i_this->m2A4);
    u32 play = BemosHD_dComIfGp_get();
    BemosHD_cCcS_Set(&bemos_play(play).mCcS, &i_this->mSph);
    f32 t = f32(static_cast<s16>(i_this->m6AC)) - 2.5f, scale = gabi::fnmsubs(t, t, 6.25f) * 1.6f;
    i_this->m314.x = scale;
    i_this->m314.y = scale;
    i_this->m314.z = scale;
    u32 e = static_cast<u32>(i_this->m300.v);
    if (e) {
        f32 y = static_cast<f32>(i_this->m354.y), z = static_cast<f32>(i_this->m354.z),
            x = static_cast<f32>(i_this->m354.x);
        u8 kind = static_cast<u8>(at<BemosEmitterHD>(e)->mCoordinateSystem);
        at<BemosEmitterHD>(e)->mGlobalTranslation.x = x;
        at<BemosEmitterHD>(e)->mGlobalTranslation.y = y;
        at<BemosEmitterHD>(e)->mGlobalTranslation.z = z;
        if (kind >= 7)
            at<BemosEmitterHD>(e)->mGlobalTranslation.y =
                -static_cast<f32>(at<BemosEmitterHD>(e)->mGlobalTranslation.y);
        at<BemosEmitterHD>(i_this->m300.v)->mGlobalPrmColor.a = i_this->m310;
    }
    e = static_cast<u32>(i_this->m30C.v);
    if (e) {
        f32 x = static_cast<f32>(i_this->m354.x), z = static_cast<f32>(i_this->m354.z),
            y = static_cast<f32>(i_this->m354.y);
        s32 rz = static_cast<s16>(i_this->m29C.z), rx = static_cast<s16>(i_this->m29C.x),
            ry = s16(static_cast<s16>(i_this->m29C.y) + static_cast<s16>(i_this->current.angle.y));
        u8 kind = static_cast<u8>(at<BemosEmitterHD>(e)->mCoordinateSystem);
        at<BemosEmitterHD>(e)->mGlobalTranslation.x = x;
        at<BemosEmitterHD>(e)->mGlobalTranslation.y = y;
        at<BemosEmitterHD>(e)->mGlobalTranslation.z = z;
        if (kind >= 7)
            at<BemosEmitterHD>(e)->mGlobalTranslation.y =
                -static_cast<f32>(at<BemosEmitterHD>(e)->mGlobalTranslation.y);
        BemosHD_JPAGetXYZRotateMtx(rx, ry, rz, &at<BemosEmitterHD>(i_this->m30C.v)->mGlobalRotation);
        x = static_cast<f32>(i_this->m314.x);
        y = static_cast<f32>(i_this->m314.y);
        e = static_cast<u32>(i_this->m30C.v);
        z = static_cast<f32>(i_this->m314.z);
        at<BemosEmitterHD>(e)->mGlobalScale.x = x;
        at<BemosEmitterHD>(e)->mGlobalScale.y = y;
        at<BemosEmitterHD>(e)->mGlobalScale.z = z;
        at<BemosEmitterHD>(e)->mGlobalPrmScale.x = x;
        at<BemosEmitterHD>(e)->mGlobalPrmScale.y = y;
        at<BemosEmitterHD>(e)->mGlobalPrmScale.z = z;
    }
    return 1;
}

/* 023273C0 */
s32 daBemos_c::red_eye_search_init() {
    auto* i_this = this;
    i_this->m290.d = 0;
    i_this->m290.i = 0xffff;
    i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_red_eye_search);
    i_this->m6B6 = static_cast<s16>(bemos_hio().m42);
    return 1;
}

/* 02327968 */
s32 daBemos_c::red_eye_search() {
    auto* i_this = this;
    BemosHD_mDoExt_baseAnm_play(&i_this->mBrkAnm2);
    u32 beam = BemosHD_daBemos_c_getBeamActor(this);
    if (!beam)
        return 0;
    gabi::Local<csXyz> angle, beamangle;
    bool on = BemosHD_daBemos_c_red_range_check(this, static_cast<u32>(i_this->m6A4), angle.get()) != 0;
    if (on) {
        BemosHD_cLib_addCalcAngleS2(&i_this->m29C, s32(s16(angle->x)), 4, 0x400);
        BemosHD_cLib_addCalcAngleS2(&i_this->m29C.y, s32(s16(angle->y)), 1,
                                    s32(static_cast<s16>(bemos_hio().m40)));
        s16 z = static_cast<s16>(i_this->m29C.z),
            yaw = s16(static_cast<s16>(i_this->m29C.y) + static_cast<s16>(i_this->current.angle.y));
        beamangle->x = static_cast<s16>(i_this->m29C.x);
        beamangle->z = z;
        if (!static_cast<u32>(at<daBeam_c>(beam)->m690.v)) {
            u32 play = BemosHD_dComIfGp_get();
            u32 e = BemosHD_dPa_control_c_set(
                at<void>(u32(bemos_play(play).mParticle.v)), 0, dPa_name::ID_AK_SN_LASERROOT00,
                &at<daBeam_c>(beam)->current.pos, nullptr, 0, 255, 0, -1, nullptr, 0, 0);
            at<daBeam_c>(beam)->m690.v = e;
        }
        s16 state = static_cast<s16>(at<daBeam_c>(beam)->m5F4);
        at<daBeam_c>(beam)->m5A8 = 0;
        f32 width = 5;
        if (!state) {
            f32 old = static_cast<f32>(at<daBeam_c>(beam)->m588);
            if (old < 5.f)
                width = old + 1.f;
            else {
                at<daBeam_c>(beam)->m5A8 = 0;
                at<daBeam_c>(beam)->m5F4 = 1;
            }
        }
        at<daBeam_c>(beam)->m588 = width;
        for (u32 i = 0; i < 3; i++)
            bemos_positionWord(at<daBeam_c>(beam)->current.pos, i) = u32(bemos_positionWord(i_this->m354, i));
        at<daBeam_c>(beam)->current.angle.y = yaw;
        at<daBeam_c>(beam)->current.angle.x = beamangle->x;
        at<daBeam_c>(beam)->current.angle.z = beamangle->z;
        s16 timer = static_cast<s16>(i_this->m6B6);
        if (timer <= 0) {
            u32 play = BemosHD_dComIfGp_get(), player = u32(bemos_play(play).mPlayerInfo[0].mpPlayer.v);
            bemos_bits(i_this->m36C.x) = static_cast<u32>(bemos_bits(at<fopAc_ac_c>(player)->current.pos.x));
            bemos_bits(i_this->m36C.y) = static_cast<u32>(bemos_bits(at<fopAc_ac_c>(player)->current.pos.y));
            u32 z = static_cast<u32>(bemos_bits(at<fopAc_ac_c>(player)->current.pos.z));
            i_this->m6BC = 0;
            bemos_bits(i_this->m36C.z) = z;
        } else
            i_this->m6B6 = timer - 1;
    } else {
        u32 e = static_cast<u32>(at<daBeam_c>(beam)->m690.v);
        if (e) {
            u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
            at<BemosEmitterHD>(e)->mMaxFrame = -1;
            at<BemosEmitterHD>(e)->mFlags = flags | 1;
            at<daBeam_c>(beam)->m690.v = 0;
        }
        if (static_cast<s16>(at<daBeam_c>(beam)->m5F4) == 1) {
            f32 old = static_cast<f32>(at<daBeam_c>(beam)->m588);
            if (old < 5.f)
                at<daBeam_c>(beam)->m588 = old + 1.f;
            f32 height = static_cast<f32>(at<daBeam_c>(beam)->m5A8);
            if (height < 4.f)
                at<daBeam_c>(beam)->m5A8 = height + 1.f;
            else {
                at<daBeam_c>(beam)->m5A8 = 0;
                at<daBeam_c>(beam)->m588 = 0;
                at<daBeam_c>(beam)->m5F4 = 0;
                BemosHD_daBemos_c_red_eye_charge_init(this);
            }
        } else {
            at<daBeam_c>(beam)->m588 = 0;
            at<daBeam_c>(beam)->m5A8 = 0;
            BemosHD_daBemos_c_red_eye_charge_init(this);
        }
    }
    BemosHD_cM3dGSph_SetC(&i_this->mSph.mSph, &i_this->m2A4);
    u32 play = BemosHD_dComIfGp_get();
    BemosHD_cCcS_Set(&bemos_play(play).mCcS, &i_this->mSph);
    return 0;
}

/* 0232599C */
s32 daBemos_c::eye_break_init() {
    auto* i_this = this;
    bool blue = false;
    if (!static_cast<u32>(i_this->m304.v)) {
        u32 play = BemosHD_dComIfGp_get();
        u32 e = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0,
                                          dPa_name::ID_AK_SN_BMOSBALL00, &i_this->m2A4, nullptr, 0, 255, 0,
                                          -1, nullptr, 0, 0);
        i_this->m304.v = e;
        if (e) {
            blue = static_cast<u8>(i_this->m6B8) == 0;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.r = blue ? 16 : 192;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.g = blue ? 35 : 16;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.b = blue ? 192 : 35;
        }
    }
    u32 play = BemosHD_dComIfGp_get();
    BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0, dPa_name::ID_AK_JN_OK,
                              &i_this->m2A4, nullptr, 0, 255, 0, -1, nullptr, 0, 0);
    u32 flags = static_cast<u32>(i_this->actor_status);
    i_this->attention_info.flags = 0;
    i_this->m6AC = 0;
    i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_eye_break);
    i_this->actor_status = flags & 0xffffffc0;
    i_this->m290.d = 0;
    i_this->m290.i = -1;
    return 1;
}

/* 02328120 */
s32 daBemos_c::eye_break() {
    auto* i_this = this;
    u32 beam = BemosHD_daBemos_c_getBeamActor(this);
    if (beam) {
        u32 e = static_cast<u32>(at<daBeam_c>(beam)->m690.v);
        if (e) {
            u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
            at<BemosEmitterHD>(e)->mMaxFrame = -1;
            at<BemosEmitterHD>(e)->mFlags = flags | 1;
            at<daBeam_c>(beam)->m690.v = 0;
        }
        if (static_cast<s16>(at<daBeam_c>(beam)->m5F4) == 1) {
            f32 width = static_cast<f32>(at<daBeam_c>(beam)->m588);
            if (width < 5.f)
                at<daBeam_c>(beam)->m588 = width + 1.f;
            f32 height = static_cast<f32>(at<daBeam_c>(beam)->m5A8);
            if (height < 4.f)
                at<daBeam_c>(beam)->m5A8 = height + 1.f;
            else {
                at<daBeam_c>(beam)->m5F4 = 0;
                at<daBeam_c>(beam)->m5A8 = 0;
                at<daBeam_c>(beam)->m588 = 0;
            }
        } else {
            at<daBeam_c>(beam)->m5A8 = 0;
            at<daBeam_c>(beam)->m588 = 0;
        }
    }
    BemosHD_cLib_addCalcAngleS2(&i_this->m29C, 0, 2, 0x800);
    BemosHD_cLib_chaseF(&i_this->m360.x, 1.f, 0.2f);
    BemosHD_cLib_chaseF(&i_this->m360.y, 1.f, 0.05f);
    s16 timer = s16(static_cast<s16>(i_this->m6AC) + 1);
    i_this->m6AC = timer;
    if (timer <= 0)
        return 1;
    for (u32 i = 0; i < 5; i++) {
        u32 e = bemos_emitter(this, i).v;
        if (e) {
            u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
            at<BemosEmitterHD>(e)->mMaxFrame = -1;
            at<BemosEmitterHD>(e)->mFlags = flags | 1;
        }
    }
    i_this->m308.v = 0;
    i_this->m2FC.v = 0;
    i_this->m300.v = 0;
    i_this->m304.v = 0;
    i_this->m30C.v = 0;
    bemos_bits(i_this->m360.x) = u32(bemos_positionWord(bemos_zero(), 0));
    u32 second = u32(bemos_positionWord(bemos_zero(), 1));
    gabi::Local<csXyz> angle;
    s16 rz = static_cast<s16>(i_this->m29C.z), rx = static_cast<s16>(i_this->m29C.x),
        yaw = static_cast<s16>(i_this->current.angle.y), ry = static_cast<s16>(i_this->m29C.y);
    angle->x = rx;
    angle->z = rz;
    u8 type = static_cast<u8>(i_this->m6B8);
    bemos_bits(i_this->m360.y) = second;
    bemos_bits(i_this->m360.z) = u32(bemos_positionWord(bemos_zero(), 2));
    angle->y = s16(ry + yaw);
    u32 colorIndex = type ? 2 : 0;
    gabi::Local<be<u32>> color0, color1;
    // HD: GHS gives each color its own guard and copies the original word through memcpy.
    if (!u32(l_breakColorGuard[colorIndex])) {
        l_breakColorGuard[colorIndex] = 1;
        BemosHD_memcpy(&l_breakColors[colorIndex], &l_breakColorInit[colorIndex], 4);
    }
    *color0 = u32(l_breakColors[colorIndex]);
    if (!u32(l_breakColorGuard[colorIndex + 1])) {
        l_breakColorGuard[colorIndex + 1] = 1;
        BemosHD_memcpy(&l_breakColors[colorIndex + 1], &l_breakColorInit[colorIndex + 1], 4);
    }
    *color1 = u32(l_breakColors[colorIndex + 1]);
    s32 room = static_cast<s8>(i_this->current.roomNo);
    u32 play = BemosHD_dComIfGp_get();
    BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0, dPa_name::ID_AK_SN_BMOSBREAK00,
                              &i_this->m2A4, angle.get(), 0, 255, 0, room, color0.get(), 0, 0);
    room = static_cast<s8>(i_this->current.roomNo);
    play = BemosHD_dComIfGp_get();
    BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0, dPa_name::ID_AK_SN_BMOSBREAK01,
                              &i_this->m2A4, angle.get(), 0, 255, 0, room, color1.get(), 0, 0);
    if (beam)
        BemosHD_fopAcM_delete(at<void>(beam));
    s32 reverb = BemosHD_dComIfGp_getReverb(s32(static_cast<s8>(i_this->current.roomNo)));
    // HD: emit positional audio at eyePos with the room reverb.
    BemosHD_mDoAud_seStart(0x6807, &i_this->eyePos, 0, reverb);
    u32 item = static_cast<u8>(i_this->m6BF);
    f32 x = static_cast<f32>(i_this->m2A4.x), y = static_cast<f32>(i_this->m2A4.y),
        z = static_cast<f32>(i_this->m2A4.z);
    u16 ang = angle->y;
    u32 trig = ang;
    f32 sine = f32(bemos_sinCos(trig).sin), cosine = f32(bemos_sinCos(trig).cos);
    gabi::Local<cXyz> pos;
    pos->x = gabi::fmadds(30.f, sine, x);
    pos->y = y;
    pos->z = gabi::fmadds(30.f, cosine, z);
    BemosHD_fopAcM_createItemFromTable(pos.get(), item, u32(static_cast<u8>(i_this->m6C0)),
                                       s32(static_cast<s8>(i_this->current.roomNo)), 0, angle.get(), 1, 0);
    i_this->m290.d = 0;
    i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_eye_dummy);
    i_this->m290.i = -1;
    return 1;
}

/* 023286F8 */
s32 daBemos_c::eye_dummy() {
    auto* i_this = this;
    if (static_cast<u8>(i_this->m6B8) == 1)
        i_this->mBrkAnm2.mFrameCtrl.mFrame = 10.f;
    return 0;
}

/* 02324C5C */
s32 daBemos_c::yellow_eye_wait_init() {
    auto* i_this = this;
    if (!static_cast<u32>(i_this->m2FC.v)) {
        gabi::Local<cXyz> pos, delta, out;
        pos->x = static_cast<f32>(i_this->current.pos.x);
        pos->y = static_cast<f32>(i_this->current.pos.y);
        pos->z = static_cast<f32>(i_this->current.pos.z);
        delta->x = 0;
        delta->y = 0;
        delta->z = 20;
        BemosHD_mDoMtx_ZXYrotS(
            mDoMtx_stack_c::get(), s32(s16(static_cast<s16>(i_this->current.angle.x) + 0x4000)),
            s32(static_cast<s16>(i_this->current.angle.y)), s32(static_cast<s16>(i_this->current.angle.z)));
        BemosHD_PSMTXMultVec(mDoMtx_stack_c::get(), delta.get(), out.get());
        BemosHD_PSVECAdd(pos.get(), out.get(), pos.get());
        u32 play = BemosHD_dComIfGp_get();
        u32 e = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0,
                                          dPa_name::ID_AK_SN_BMOSWAIT00, pos.get(), &i_this->current.angle, 0,
                                          0xff, 0, -1, nullptr, 0, 0);
        i_this->m2FC.v = e;
        if (e) {
            at<BemosEmitterHD>(e)->mGlobalPrmColor.r = 180;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.b = 120;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.g = 180;
            u32 p = static_cast<u32>(i_this->m2FC.v);
            at<BemosEmitterHD>(p)->mGlobalEnvColor.r = 128;
            at<BemosEmitterHD>(p)->mGlobalEnvColor.b = 20;
            at<BemosEmitterHD>(p)->mGlobalEnvColor.g = 128;
        }
    }
    u32 e = static_cast<u32>(i_this->m304.v);
    if (e) {
        u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
        at<BemosEmitterHD>(e)->mMaxFrame = -1;
        at<BemosEmitterHD>(e)->mFlags = flags | 1;
        i_this->m304.v = 0;
    }
    i_this->m290.d = 0;
    i_this->m290.i = -1;
    i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_yellow_eye_wait);
    return 1;
}

/* 02327D98 */
s32 daBemos_c::yellow_eye_wait() {
    auto* i_this = this;
    u32 beam = BemosHD_daBemos_c_getBeamActor(this);
    if (!beam)
        return 0;
    u32 sw = static_cast<u32>(i_this->m6A8);
    bool off = false;
    if (sw != 255)
        off = BemosHD_dSv_info_c_isSwitch(bemos_save(), sw, s32(static_cast<s8>(i_this->home.roomNo))) != 0;
    if (!off && static_cast<u8>(i_this->m6B9) == 1)
        off = bemos_stayRoom() != static_cast<s8>(i_this->current.roomNo);
    if (off) {
        s16 state = static_cast<s16>(at<daBeam_c>(beam)->m5F4);
        if (!state)
            return 0;
        u32 e = static_cast<u32>(at<daBeam_c>(beam)->m690.v);
        if (e) {
            u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
            at<BemosEmitterHD>(e)->mMaxFrame = -1;
            at<BemosEmitterHD>(e)->mFlags = flags | 1;
            state = static_cast<s16>(at<daBeam_c>(beam)->m5F4);
            at<daBeam_c>(beam)->m690.v = 0;
        }
        if (state == 1) {
            f32 x = static_cast<f32>(at<daBeam_c>(beam)->m588);
            if (x < 5.f)
                at<daBeam_c>(beam)->m588 = x + 1.f;
            f32 z = static_cast<f32>(at<daBeam_c>(beam)->m5A8);
            if (z < 4.f) {
                at<daBeam_c>(beam)->m5A8 = z + 1.f;
                return 0;
            }
            at<daBeam_c>(beam)->m588 = 0;
            at<daBeam_c>(beam)->m5A8 = 0;
            at<daBeam_c>(beam)->m5F4 = 0;
        } else {
            at<daBeam_c>(beam)->m588 = 0;
            at<daBeam_c>(beam)->m5A8 = 0;
        }
        return 0;
    }
    if (BemosHD_daBemos_c_yellow_range_check(this, static_cast<u32>(i_this->m6A4), 0))
        BemosHD_daBemos_c_yellow_eye_search_init(this);
    return 0;
}

/* 02327CB8 */
s32 daBemos_c::yellow_eye_search_init() {
    auto* i_this = this;
    if (!static_cast<u32>(i_this->m304.v)) {
        u32 play = BemosHD_dComIfGp_get();
        u32 e = BemosHD_dPa_control_c_set(at<void>(u32(bemos_play(play).mParticle.v)), 0,
                                          dPa_name::ID_AK_SN_BMOSBALL00, &i_this->m2A4, nullptr, 0, 255, 0,
                                          -1, nullptr, 0, 0);
        i_this->m304.v = e;
        if (e) {
            at<BemosEmitterHD>(e)->mGlobalPrmColor.r = 160;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.g = 192;
            at<BemosEmitterHD>(e)->mGlobalPrmColor.b = 20;
        }
    }
    u32 e = static_cast<u32>(i_this->m2FC.v);
    if (e) {
        u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
        at<BemosEmitterHD>(e)->mMaxFrame = -1;
        at<BemosEmitterHD>(e)->mFlags = flags | 1;
        i_this->m2FC.v = 0;
    }
    i_this->m6AC = 0;
    i_this->m290.d = 0;
    i_this->m290.i = -1;
    i_this->m290.f = gabi::guest_fn((const void*)&BemosCallback_yellow_eye_search);
    return 1;
}

/* 02327F08 */
s32 daBemos_c::yellow_eye_search() {
    auto* i_this = this;
    u32 beam = BemosHD_daBemos_c_getBeamActor(this);
    if (!beam)
        return 0;
    bool on = BemosHD_daBemos_c_yellow_range_check(this, static_cast<u32>(i_this->m6A4), 0) != 0;
    if (on) {
        u32 sw = static_cast<u32>(i_this->m6A8);
        if (sw != 255)
            on =
                BemosHD_dSv_info_c_isSwitch(bemos_save(), sw, s32(static_cast<s8>(i_this->home.roomNo))) == 0;
    }
    if (on) {
        if (!static_cast<u32>(at<daBeam_c>(beam)->m690.v)) {
            u32 play = BemosHD_dComIfGp_get();
            u32 e = BemosHD_dPa_control_c_set(
                at<void>(u32(bemos_play(play).mParticle.v)), 0, dPa_name::ID_AK_SN_LASERROOT00,
                &at<daBeam_c>(beam)->current.pos, nullptr, 0, 255, 0, -1, nullptr, 0, 0);
            at<daBeam_c>(beam)->m690.v = e;
        }
        s16 state = static_cast<s16>(at<daBeam_c>(beam)->m5F4);
        at<daBeam_c>(beam)->m5A8 = 0;
        if (!state) {
            f32 x = static_cast<f32>(at<daBeam_c>(beam)->m588);
            if (x < 5.f) {
                at<daBeam_c>(beam)->m588 = x + 1.f;
                return 0;
            }
            at<daBeam_c>(beam)->m5A8 = 0;
            at<daBeam_c>(beam)->m588 = 5;
            at<daBeam_c>(beam)->m5F4 = 1;
        } else
            at<daBeam_c>(beam)->m588 = 5;
        return 0;
    }
    u32 e = static_cast<u32>(at<daBeam_c>(beam)->m690.v);
    if (e) {
        u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
        at<BemosEmitterHD>(e)->mMaxFrame = -1;
        at<BemosEmitterHD>(e)->mFlags = flags | 1;
        at<daBeam_c>(beam)->m690.v = 0;
    }
    if (static_cast<s16>(at<daBeam_c>(beam)->m5F4) == 1) {
        f32 x = static_cast<f32>(at<daBeam_c>(beam)->m588);
        if (x < 5.f)
            at<daBeam_c>(beam)->m588 = x + 1.f;
        f32 z = static_cast<f32>(at<daBeam_c>(beam)->m5A8);
        if (z < 4.f) {
            at<daBeam_c>(beam)->m5A8 = z + 1.f;
            return 0;
        }
        at<daBeam_c>(beam)->m5A8 = 0;
        at<daBeam_c>(beam)->m588 = 0;
        at<daBeam_c>(beam)->m5F4 = 0;
        BemosHD_daBemos_c_yellow_eye_wait_init(this);
    } else {
        at<daBeam_c>(beam)->m588 = 0;
        at<daBeam_c>(beam)->m5A8 = 0;
        BemosHD_daBemos_c_yellow_eye_wait_init(this);
    }
    return 0;
}

/* 02325B44 */
void daBemos_c::event_move() {
    auto* i_this = this;
    if (static_cast<u8>(i_this->m6B8) != 2)
        return;
    if (static_cast<u8>(i_this->m6BD)) {
        u32 sw = static_cast<u32>(i_this->m6A8);
        if (sw == 255)
            return;
        s8 room = static_cast<s8>(i_this->home.roomNo);
        u32 save = gabi::ea(bemos_save());
        u8 old = static_cast<u8>(i_this->m6BA);
        u8 now = BemosHD_dSv_info_c_isSwitch(at<dSv_info_c>(save), sw, room);
        i_this->m6BA = now;
        bool start = false;
        if (now != old && now == 1) {
            save = gabi::ea(bemos_save());
            if (!BemosHD_dSv_event_c_isEventBit(&at<dSv_info_c>(save)->mSavedata.mEvent, 0x1010)) {
                save = gabi::ea(bemos_save());
                BemosHD_dSv_event_c_onEventBit(&at<dSv_info_c>(save)->mSavedata.mEvent, 0x1010);
                i_this->m6BB = 1;
                start = true;
            }
        }
        u8 event = static_cast<u8>(i_this->m6BB);
        if (start || event == 1) {
            if (static_cast<u16>(i_this->eventInfo.mCommand) == 2) {
                i_this->m6BB = 2;
            } else {
                BemosHD_fopAcM_orderOtherEvent2(this, "hmos3cam", 1, 65535);
                i_this->eventInfo.mCondition = static_cast<u16>(i_this->eventInfo.mCondition) | 2;
            }
        } else if (event == 2) {
            u32 play = BemosHD_dComIfGp_get();
            s32 staff = BemosHD_dEvent_manager_c_getMyStaffId(&bemos_play(play).mEvtManager, "Hmos3", 0, 0);
            play = BemosHD_dComIfGp_get();
            u32 ended = BemosHD_dEvent_manager_c_endCheckOld(&bemos_play(play).mEvtManager, "hmos3cam");
            play = BemosHD_dComIfGp_get();
            if (ended) {
                bemos_play(play).mEvtCtrl.mEventFlag |= 8;
                i_this->m6BB = 0;
            } else
                BemosHD_dEvent_manager_c_cutEnd(&bemos_play(play).mEvtManager, staff);
        }
        if (static_cast<u8>(i_this->m6B8) != 2)
            return;
    }
    u32 sw = static_cast<u32>(i_this->m6A8);
    if (sw == 255)
        return;
    u32 save = gabi::ea(bemos_save());
    s8 room = static_cast<s8>(i_this->home.roomNo);
    if (!BemosHD_dSv_info_c_isSwitch(at<dSv_info_c>(save), sw, room))
        return;
    u32 e = static_cast<u32>(i_this->m2FC.v);
    if (e) {
        u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
        at<BemosEmitterHD>(e)->mMaxFrame = 0xffffffff;
        at<BemosEmitterHD>(e)->mFlags = flags | 1;
        i_this->m2FC.v = 0;
    }
}

/* 02325DB8 */
void daBemos_c::guard_proc() {
    auto* i_this = this;
    if (static_cast<u8>(i_this->m6B8) != 2 || static_cast<u8>(i_this->m6BD) != 1)
        return;
    u32 beam = BemosHD_daBemos_c_getBeamActor(this);
    if (!beam || !static_cast<s16>(at<daBeam_c>(beam)->m5F4))
        return;
    u32 play = BemosHD_dComIfGp_get();
    gabi::Local<cXyz> diff, pos, v0, v1, out0, out1;
    BemosHD_cXyz_subtractInto(&at<fopAc_ac_c>(bemos_play(play).mpPlayerPtr[0].v)->current.pos, diff.get(),
                              &i_this->current.pos);
    v0->x = f32(bemos_baseZ().x);
    v0->z = f32(bemos_baseZ().z);
    v0->y = f32(bemos_baseZ().y);
    v1->x = f32(bemos_baseX().x);
    v1->y = f32(bemos_baseX().y);
    v1->z = f32(bemos_baseX().z);
    pos->x = static_cast<f32>(i_this->current.pos.x);
    pos->y = static_cast<f32>(i_this->current.pos.y);
    pos->z = static_cast<f32>(i_this->current.pos.z);
    BemosHD_mDoMtx_ZXYrotS(mDoMtx_stack_c::get(), s32(static_cast<s16>(i_this->current.angle.x)),
                           s32(static_cast<s16>(i_this->current.angle.y)),
                           s32(static_cast<s16>(i_this->current.angle.z)));
    BemosHD_PSMTXMultVec(mDoMtx_stack_c::get(), v0.get(), out0.get());
    BemosHD_PSMTXMultVec(mDoMtx_stack_c::get(), v1.get(), out1.get());
    f32 d0 = BemosHD_PSVECDotProduct(diff.get(), out0.get()),
        d1 = BemosHD_PSVECDotProduct(diff.get(), out1.get());
    BemosHD_PSVECScale(out0.get(), out0.get(), d0);
    f32 scale = static_cast<f32>(bemos_hio().m44) - 20.f;
    if (d1 > 0)
        scale = -scale;
    BemosHD_PSVECScale(out1.get(), out1.get(), scale);
    BemosHD_PSVECAdd(pos.get(), out0.get(), pos.get());
    BemosHD_PSVECAdd(pos.get(), out1.get(), pos.get());
    play = BemosHD_dComIfGp_get();
    bool low = (u32(bemos_play(play).mPlayerStatus[0][1]) & 32) != 0;
    if (low)
        pos->y = f32(pos->y) - 50.f;
    else
        pos->y = gabi::fnmsubs(static_cast<f32>(bemos_hio().m48), 0.5f, pos->y);
    BemosHD_cM3dGCyl_SetC(&i_this->mCyl.mCyl, pos.get());
    BemosHD_cM3dGCyl_SetR(&i_this->mCyl.mCyl, static_cast<f32>(bemos_hio().m44));
    BemosHD_cM3dGCyl_SetH(&i_this->mCyl.mCyl, low ? 100.f : static_cast<f32>(bemos_hio().m48));
    play = BemosHD_dComIfGp_get();
    BemosHD_cCcS_Set(&bemos_play(play).mCcS, &i_this->mCyl);
}

/* 02325D3C */
void* daBemos_c::getBeamActor() {
    u32 id = parentActorID;
    gabi::Local<be<u32>> local;
    *local = id;
    u32 found = 0;
    if (id != 0xffffffff)
        found = BemosHD_fopAcIt_Judge(at<void>(gabi::guest_fn((const void*)&BemosCallback_fpcSch_JudgeByID)),
                                      local.get());
    if (!found)
        return nullptr;
    if (!BemosHD_fopAc_IsActor(at<void>(found)))
        return nullptr;
    if (static_cast<s16>(at<fopAc_ac_c>(found)->mProfName) != fpcNm_Beam_e)
        return nullptr;
    return at<void>(found);
}

/* 023258A8 */
s32 daBemos_c::_delete() {
    auto* i_this = this;
    BemosHD_dComIfG_resDelete(&i_this->mPhase, "Hmos");
    {
        u32 e = static_cast<u32>(i_this->m2FC.v);
        if (e) {
            u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
            at<BemosEmitterHD>(e)->mMaxFrame = 0xffffffff;
            at<BemosEmitterHD>(e)->mFlags = flags | 1;
        }
    }
    {
        u32 e = static_cast<u32>(i_this->m300.v);
        if (e) {
            u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
            at<BemosEmitterHD>(e)->mMaxFrame = 0xffffffff;
            at<BemosEmitterHD>(e)->mFlags = flags | 1;
        }
    }
    {
        u32 e = static_cast<u32>(i_this->m304.v);
        if (e) {
            u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
            at<BemosEmitterHD>(e)->mMaxFrame = 0xffffffff;
            at<BemosEmitterHD>(e)->mFlags = flags | 1;
        }
    }
    {
        u32 e = static_cast<u32>(i_this->m308.v);
        if (e) {
            u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
            at<BemosEmitterHD>(e)->mMaxFrame = 0xffffffff;
            at<BemosEmitterHD>(e)->mFlags = flags | 1;
        }
    }
    {
        u32 e = static_cast<u32>(i_this->m30C.v);
        if (e) {
            u32 flags = static_cast<u32>(at<BemosEmitterHD>(e)->mFlags);
            at<BemosEmitterHD>(e)->mMaxFrame = 0xffffffff;
            at<BemosEmitterHD>(e)->mFlags = flags | 1;
        }
    }
    i_this->m300.v = 0;
    i_this->m304.v = 0;
    i_this->m308.v = 0;
    i_this->m30C.v = 0;
    i_this->m2FC.v = 0;
    return 1;
}

/* 02326020 */
s32 daBemos_c::_execute() {
    auto* i_this = this;
    if (static_cast<u8>(i_this->m6B8) != 2 && BemosHD_dCcD_GObjInf_ChkTgHit(&i_this->mSph)) {
        BemosHD_daBemos_c_eye_break_init(this);
        BemosHD_dCcD_GObjInf_ClrTgHit(&i_this->mSph);
    }
    (this->*m290)();
    BemosHD_daBemos_c_event_move(this);
    BemosHD_daBemos_c_set_mtx(this);
    if (static_cast<u8>(i_this->m6B8) != 2) {
        BemosHD_dCcD_GStts_Move(&i_this->mSttsCyl.__vtbl_gstts);
        BemosHD_dCcD_GStts_Move(&i_this->mSttsSph.__vtbl_gstts);
        if (BemosHD_dCcD_GObjInf_ChkTgHit(&i_this->mCyl)) {
            BemosHD_daObj_HitEff_hibana(this, &i_this->mCyl);
            BemosHD_daObj_HitSeStart(&i_this->current.pos, static_cast<s8>(i_this->current.roomNo),
                                     &i_this->mCyl, 13);
        }
        BemosHD_cM3dGCyl_SetC(&i_this->mCyl.mCyl, &i_this->current.pos);
        u32 play = BemosHD_dComIfGp_get();
        BemosHD_cCcS_Set(&bemos_play(play).mCcS, &i_this->mCyl);
    } else if (static_cast<u8>(i_this->m6B9) != 2)
        BemosHD_daBemos_c_guard_proc(this);
    BemosHD_fopAcM_setGbaName(this, 39, 35, 44);
    return 0;
}

/* 023261A0 */
s32 daBemos_c::_draw() {
    auto* i_this = this;
    u32 env = BemosHD_dKy_getEnvlight();
    BemosHD_dScnKy_env_light_c_settingTevStruct(at<void>(env), 0, &i_this->current.pos, &i_this->tevStr);
    env = BemosHD_dKy_getEnvlight();
    BemosHD_dScnKy_env_light_setLightTevColorType(at<void>(env), at<void>(static_cast<u32>(i_this->m2B8.v)),
                                                  &i_this->tevStr);
    BemosHD_mDoExt_modelUpdateDL(at<void>(static_cast<u32>(i_this->m2B8.v)), 0);
    if (static_cast<u8>(i_this->m6B8) == 2)
        return 1;
    if (static_cast<f32>(i_this->m360.x) > 0.f) {
        env = BemosHD_dKy_getEnvlight();
        BemosHD_dScnKy_env_light_setLightTevColorType(
            at<void>(env), at<void>(static_cast<u32>(i_this->m2BC.v)), &i_this->tevStr);
        BemosHD_mDoExt_modelUpdateDL(at<void>(static_cast<u32>(i_this->m2BC.v)), 0);
    }
    if (static_cast<u8>(i_this->m6B8) != 1)
        return 1;
    env = BemosHD_dKy_getEnvlight();
    BemosHD_dScnKy_env_light_setLightTevColorType(at<void>(env), at<void>(static_cast<u32>(i_this->m2C0.v)),
                                                  &i_this->tevStr);
    u32 m = static_cast<u32>(i_this->m2C0.v);
    f32 frame = static_cast<f32>(i_this->mBrkAnm2.mFrameCtrl.mFrame);
    BemosHD_mDoExt_brkAnm_entry(&i_this->mBrkAnm2, at<void>(at<J3DModel>(m)->mModelData.v), frame);
    BemosHD_mDoExt_modelUpdateDL(at<void>(static_cast<u32>(i_this->m2C0.v)), 0);
    return 1;
}

/* 023288BC */
s32 daBemosIsDelete(void* actor) { return 1; }

// HD: compiler-generated and template code of this translation unit.

/* 02328718 */
void* daBemos_HIO_c__ct(void* obj) {
    u32 a = gabi::ea(obj);
    if (!a) {
        a = BemosHD_operator_new(0x50);
        if (!a)
            return nullptr;
    }
    auto* i_hio = at<daBemos_HIO_c>(a);
    i_hio->m20 = 0.1f;
    i_hio->m14 = 0.3f;
    i_hio->m08 = 400.f;
    i_hio->mNo = 0xff;
    i_hio->m30 = 0;
    i_hio->m34 = 0;
    i_hio->m3C = 0;
    i_hio->m2A = 0;
    i_hio->m1E = 7;
    i_hio->m1C = 3;
    i_hio->m0C = 900.f;
    i_hio->m18 = 0.3f;
    i_hio->m42 = 30;
    i_hio->m40 = 400;
    i_hio->m38 = 0;
    i_hio->m48 = 100.f;
    i_hio->m10 = 0;
    i_hio->m44 = 200.f;
    i_hio->m2C = 0;
    i_hio->m4C = 0;
    i_hio->__vtbl = BemosVtableHD::mHio;
    i_hio->m24 = 0.07f;
    i_hio->m28 = 0;
    return at<void>(a);
}

/* 02328808 */
void __sinit_d_a_obj_bemos_cpp() {
    bemos_headerStatics().mZero[2] = 0;
    bemos_headerStatics().mZero[0] = 0;
    bemos_headerStatics().mZero[3] = 0;
    bemos_headerStatics().mZero[1] = 0;
    BemosHD___register_global_object(bemos_dtorRecord(0));
    bemos_headerStatics().mMinAngle = -3.1415927410125732f;
    bemos_headerStatics().mMaxAngle = 3.1415927410125732f;
    BemosHD_headerFirst_construct(&bemos_headerStatics().mFirst);
    BemosHD___register_global_object(bemos_dtorRecord(1));
    BemosHD_headerSecond_construct(&bemos_headerStatics().mSecond);
    BemosHD___register_global_object(bemos_dtorRecord(2));
    BemosHD_daBemos_HIO_c_daBemos_HIO_c(&bemos_hio());
}

/* 023288A8 */
void daBemos_HIO_c__dt(void* obj, u32 flags) {
    if (obj && (flags & 1))
        BemosHD_operator_delete(obj);
}

/* 023288C4 */
void daBemos_c__dt(void* obj, u32 flags) {
    auto* i_this = static_cast<daBemos_c*>(obj);
    u32 a = gabi::ea(obj);
    if (a) {
        i_this->mGndChk.cBgS_PolyInfo::__vtbl = BemosVtableHD::mGroundPolyDtor;
        i_this->mGndChk.dBgS_PolyPassChk::__vtbl = BemosVtableHD::mPolyPassDtor;
        i_this->mGndChk.dBgS_GrpPassChk::__vtbl = BemosVtableHD::mGroupPassDtor;
        BemosHD_cBgS_Chk_destruct_cBgS_Chk(&i_this->mGndChk, 0);
        BemosHD_dCcD_Sph_destruct(&i_this->mSph, 2);
        BemosHD_dCcD_Stts_destruct(&i_this->mSttsSph, 2);
        BemosHD_dCcD_Cyl_destruct(&i_this->mCyl, 2);
        BemosHD_dCcD_Stts_destruct(&i_this->mSttsCyl, 2);
        BemosHD_fopAc_ac_c_destruct_fopAc_ac_c(obj, 0);
        if (flags & 1)
            BemosHD_operator_delete(obj);
    }
}

/* 02328978 */
void daBemos_safeStringAssureTermination(void* actor) {}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_sph_src 101C7FF8
data l_cyl_src 101C8064
data l_cylGuard_src 101C80A8
data l_heap_init 101C8058
data SHeapSize 101FDB6C nocheck
data l_breakColors 101FEC04 nocheck
data l_breakColorInit 10026240
data l_breakColorGuard 101FDB78 nocheck
data l_heapGuard 101FDB68
ext 023267CC void BemosCallback_blue_eye_wait(void*) callback
ext 0232728C void BemosCallback_red_eye_wait(void*) callback
ext 02326E4C void BemosCallback_blue_eye_search(void*) callback
ext 02327968 void BemosCallback_red_eye_search(void*) callback
ext 023268D0 void BemosCallback_blue_eye_charge(void*) callback
ext 023273F0 void BemosCallback_red_eye_charge(void*) callback
ext 02327D98 void BemosCallback_yellow_eye_wait(void*) callback
ext 02327F08 void BemosCallback_yellow_eye_search(void*) callback
ext 02328120 void BemosCallback_eye_break(void*) callback
ext 023286F8 void BemosCallback_eye_dummy(void*) callback
ext 025E1234 void BemosCallback_fpcSch_JudgeByID(void*) callback
str 1002632C in 0232409C
str 10026340 in 0232409C
str 10026358 in 0232419C
str 1002636C in 0232419C
str 10026380 in 0232419C
str 10026394 in 0232435C
str 100263A8 in 0232435C
str 10026478
str 10026414
str 1002640C
ext 026066C4 u32 BemosHD_dRes_control_c_getRes(void*, void*, s32)
ext 0273AA24 void BemosHD_JUTAssertion_showAssert(const char*, s32, const char*)
ext 025E38E0 u32 BemosHD_mDoExt_J3DModel__create(void*, s32, s32)
ext 025E8154 u32 BemosHD_mDoExt_brkAnm_init(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32)
ext 025200D4 u32 BemosHD_dComIfGp_get()
ext 025A847C u32 BemosHD_dPa_control_c_set(void*, s32, s32, void*, void*, s32, s32, s32, s32, void*, s32, s32)
ext 0273AF40 void BemosHD_operator_delete(void*)
ext 02008DAC void BemosHD_cBgS_Chk_destruct_cBgS_Chk(void*, s32)
ext 02515AE8 void BemosHD_dCcD_Sph_destruct(void*, s32)
ext 02515860 void BemosHD_dCcD_Stts_destruct(void*, s32)
ext 02515A70 void BemosHD_dCcD_Cyl_destruct(void*, s32)
ext 025D50BC void BemosHD_fopAc_ac_c_destruct_fopAc_ac_c(void*, s32)
ext 025D5218 u32 BemosHD_fopAcIt_Judge(void*, void*)
ext 025D4604 u32 BemosHD_fopAc_IsActor(void*)
ext 0273AD10 u32 BemosHD_operator_new(s32)
ext 028F026C void BemosHD___register_global_object(void*)
ext 028ED6F8 void BemosHD_headerFirst_construct(void*)
ext 028EAB2C void BemosHD_headerSecond_construct(void*)
ext 02328718 void BemosHD_daBemos_HIO_c_daBemos_HIO_c(void*)
ext 028E93CC void BemosHD_PSMTXTrans(void*, f32, f32, f32)
ext 025F1B48 void BemosHD_mDoMtx_ZXYrotM(void*, s32, s32, s32)
ext 028E8F64 void BemosHD_PSMTXMultVec(void*, void*, void*)
ext 025F1C28 void BemosHD_mDoMtx_YrotM(void*, s32)
ext 025F24E0 void BemosHD_mDoMtx_stack_c_transM(f32, f32, f32)
ext 02515F14 void BemosHD_dCcD_Stts_Init(void*, s32, s32, void*)
ext 0251677C void BemosHD_dCcD_Sph_Set(void*, void*)
ext 02324428 void BemosHD_daBemos_c_blue_eye_wait_init(void*)
ext 02008974 f32 BemosHD_cBgS_GroundCross(void*, void*)
ext 025D5A20 u32 BemosHD_fopAcM_createChild(s32, u32, u32, void*, s32, void*, void*, s32, s32)
ext 02324830 void BemosHD_daBemos_c_red_eye_wait_init(void*)
ext 02516518 void BemosHD_dCcD_Cyl_Set(void*, void*)
ext 02324628 void BemosHD_daBemos_c_CreateInit1(void*)
ext 02324A2C void BemosHD_daBemos_c_CreateInit2(void*)
ext 02324DC8 void BemosHD_daBemos_c_CreateInit3(void*)
ext 023252C8 void BemosHD_daBemos_c_set_mtx(void*)
ext 025204C8 void BemosHD_dComIfG_resDelete(void*, const char*)
ext C000A848 void BemosHD_memcpy(void*, void*, s32)
ext 025D4ED0 void BemosHD_fopAc_ac_c_fopAc_ac_c(void*)
ext 025E80D0 void BemosHD_mDoExt_brkAnm_init(void*)
ext 0200BD2C void BemosHD_cCcD_Stts_construct(void*)
ext 02515DA0 void BemosHD_dCcD_GStts_dCcD_GStts(void*)
ext 02515FB8 void BemosHD_dCcD_GObjInf_dCcD_GObjInf(void*)
ext 02018590 void BemosHD_cM3dGCyl_construct(void*)
ext 025166F0 void BemosHD_dCcD_Sph_construct(void*)
ext 02008E0C void BemosHD_cBgS_GndChk_cBgS_GndChk(void*)
ext 02520460 s32 BemosHD_dComIfG_resLoad(void*, const char*)
ext 025D63E8 u32 BemosHD_fopAcM_entrySolidHeap(void*, void*, u32)
ext 02325558 s32 BemosHD_daBemos_c_CreateInit(void*)
ext 025162A4 u32 BemosHD_dCcD_GObjInf_ChkTgHit(void*)
ext 0232599C void BemosHD_daBemos_c_eye_break_init(void*)
ext 0251621C void BemosHD_dCcD_GObjInf_ClrTgHit(void*)
ext 02325B44 void BemosHD_daBemos_c_event_move(void*)
ext 02515E50 void BemosHD_dCcD_GStts_Move(void*)
ext 02312E54 void BemosHD_daObj_HitEff_hibana(void*, void*)
ext 023129C4 void BemosHD_daObj_HitSeStart(void*, s32, void*, s32)
ext 020182E0 void BemosHD_cM3dGCyl_SetC(void*, void*)
ext 0200E240 void BemosHD_cCcS_Set(void*, void*)
ext 02325DB8 void BemosHD_daBemos_c_guard_proc(void*)
ext 025DA088 void BemosHD_fopAcM_setGbaName(void*, s32, s32, s32)
ext 02555D0C u32 BemosHD_dKy_getEnvlight()
ext 025626A4 void BemosHD_dScnKy_env_light_c_settingTevStruct(void*, s32, void*, void*)
ext 02562F5C void BemosHD_dScnKy_env_light_setLightTevColorType(void*, void*, void*)
ext 025E2DE0 void BemosHD_mDoExt_modelUpdateDL(void*, s32)
ext 025E83FC void BemosHD_mDoExt_brkAnm_entry(void*, void*, f32)
ext 025BA0C0 u32 BemosHD_dSv_info_c_isSwitch(void*, u32, s32)
ext 025B8B94 u32 BemosHD_dSv_event_c_isEventBit(void*, s32)
ext 025B8B68 void BemosHD_dSv_event_c_onEventBit(void*, s32)
ext 025D77DC void BemosHD_fopAcM_orderOtherEvent2(void*, const char*, s32, s32)
ext 02542D88 s32 BemosHD_dEvent_manager_c_getMyStaffId(void*, const char*, s32, s32)
ext 0254457C u32 BemosHD_dEvent_manager_c_endCheckOld(void*, const char*)
ext 02543280 void BemosHD_dEvent_manager_c_cutEnd(void*, s32)
ext 025F1AA4 void BemosHD_mDoMtx_ZXYrotS(void*, s32, s32, s32)
ext 028E8D88 void BemosHD_PSVECAdd(void*, void*, void*)
ext 025AAF88 u32 BemosHD_dPath_GetRoomPath(u32, s32)
ext 0200F93C s32 BemosHD_cLib_targetAngleY(void*, void*)
ext 0200F974 s32 BemosHD_cLib_targetAngleX(void*, void*)
ext 0201ADE0 void BemosHD_cXyz_subtractInto(void*, void*, void*)
ext 028E8DD0 f32 BemosHD_PSVECSquareMag(void*)
ext 028F4384 f32 BemosHD_sqrtf(f32)
ext 02018428 void BemosHD_cM3dGCyl_SetH(void*, f32)
ext 02324C5C void BemosHD_daBemos_c_yellow_eye_wait_init(void*)
ext 02325D3C u32 BemosHD_daBemos_c_getBeamActor(void*)
ext 02326274 u32 BemosHD_daBemos_c_blue_range_check(void*, u32, void*)
ext 0232666C void BemosHD_daBemos_c_blue_eye_charge_init(void*)
ext 0200F4FC void BemosHD_cLib_chaseUC(void*, s32, s32)
ext 025E742C void BemosHD_mDoExt_baseAnm_play(void*)
ext 02326460 u32 BemosHD_daBemos_c_red_range_check(void*, u32, void*)
ext 0232712C void BemosHD_daBemos_c_red_eye_charge_init(void*)
ext 0200F428 void BemosHD_cLib_addCalcAngleS2(void*, s32, s32, s32)
ext 028E8F44 f32 BemosHD_PSVECDotProduct(void*, void*)
ext 028E8E64 void BemosHD_PSVECScale(void*, void*, f32)
ext 020184DC void BemosHD_cM3dGCyl_SetR(void*, f32)
ext 020195B0 s32 BemosHD_cM_atan2s(f32, f32)
ext 0232665C u32 BemosHD_daBemos_c_yellow_range_check(void*, u32, s32)
ext 02327CB8 void BemosHD_daBemos_c_yellow_eye_search_init(void*)
ext 02018D40 void BemosHD_cM3dGSph_SetC(void*, void*)
ext 0200F5C8 void BemosHD_cLib_chaseF(void*, f32, f32)
ext 023268A8 void BemosHD_daBemos_blue_eye_search_init(void*)
ext 028245AC void BemosHD_JPAGetXYZRotateMtx(s32, s32, s32, void*)
ext 0200F378 void BemosHD_cLib_addCalcAngleS(void*, s32, s32, s32, s32)
ext 023273C0 void BemosHD_daBemos_red_eye_search_init(void*)
ext 025D57E0 void BemosHD_fopAcM_delete(void*)
ext 02520540 s32 BemosHD_dComIfGp_getReverb(s32)
ext 025E1A40 void BemosHD_mDoAud_seStart(s32, void*, s32, s32)
ext 025D8120 void BemosHD_fopAcM_createItemFromTable(void*, u32, u32, s32, s32, void*, s32, s32)
*/
// clang-format on
