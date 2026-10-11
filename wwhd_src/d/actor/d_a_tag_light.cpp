#include "d/actor/d_a_tag_light.h"
using namespace daTagLight;

const gc_tu_data gc_tu = {.safeString = 0x1003F724};
static const char *M_arcname = "Mspot";
static s16 M_cc_r[] = {20, 40, 60, 4};
static f32 M_alpha_spd[] = {0.06666667014360428f, 0.03333333507180214f, 0.01666666753590107f, 1.0f};
static TagLightSphereSourceHD M_sph_src;
static TagLightHeaderHD l_header;
static TagLightRegistrationTableHD l_registration;
static TagLightSinCosHD l_sinCos;
static cXyz l_baseY;
using TagLightFloatHD = be<f32>;
static TagLightFloatHD l_projectionAdjust;

s32 TagLight_parameter(Act_c *, u32, u32);
u32 TagLight_isSwitch(dSv_info_c *, s32, s32);
u32 TagLight_schedule();
void TagLight_assert(const char *, s32, const char *);
void TagLight_resDelete(request_of_phase_process_class *, const char *);
u32 TagLight_detectLight(void *, cXyz *);
void TagLight_onSwitch(dSv_info_c *, s32, s32);
u32 TagLight_checkSwitch(Act_c *);
u32 TagLight_checkSchedule(Act_c *);
s32 TagLight_chaseAlpha(be<f32> *, f32, f32);
void TagLight_playAnimation(mDoExt_btkAnm *);
void TagLight_destroySphere(dCcD_Sph *, s32);
void TagLight_destroyStatus(dCcD_Stts *, s32);
void TagLight_destroyActor(fopAc_ac_c *, s32);
mDoExt_btkAnm *TagLight_constructBtk(mDoExt_btkAnm *);
u32 TagLight_initBtk(mDoExt_btkAnm *, J3DModelData *, J3DAnmTextureSRTKey *, s32, s32, s32, f32, s32, s32,
                     s32);
s32 TagLight_createHeapResult(Act_c *);
void TagLight_startOffset(Act_c *, cXyz *);
void TagLight_multVec(const be<f32> *, const cXyz *, cXyz *);
void TagLight_projectionInfo(Act_c *, cXyz *, cXyz *, cXyz *, const cXyz *, const cXyz *);
void TagLight_projectionMatrix(Act_c *, const cXyz *, const cXyz *, const cXyz *, const cXyz *);
void TagLight_setSpotMatrix(Act_c *);
void TagLight_setSpot(Act_c *);
void TagLight_initSpot(Act_c *);
void TagLight_color(be<f32> *, be<u8> *);
void TagLight_linearColor(be<f32> *, be<f32> *, f32);
be<f32> *TagLight_materialColor(TagLightColorBlockHD *, s32);
dScnKy_env_light_c *TagLight_env();
void TagLight_settingTev(dScnKy_env_light_c *, s32, cXyz *, dKy_tevstr_c *);
void TagLight_setLight(dScnKy_env_light_c *, J3DModel *, dKy_tevstr_c *);
void TagLight_entryBtk(mDoExt_btkAnm *, J3DModelData *, f32);
void TagLight_setMaterial(TagLightMaterialHD *, u32);
void TagLight_updateModel(J3DModel *, u32);
void TagLight_registerGlobal(TagLightRegistrationHD *);
void TagLight_initHeaderByte(be<u8> *);
void TagLight_initHeaderFlag(be<u8> *);
void TagLight_constructActor(fopAc_ac_c *);
void TagLight_constructSphere(dCcD_Sph *);
void TagLight_transS(be<f32> *, f32, f32, f32);
void TagLight_ZXYrotM(be<f32> *, s16, s16, s16);
void TagLight_scaleM(f32, f32, f32);
void TagLight_copyMatrix(const be<f32> *, be<f32> *);
s32 TagLight_inverseMatrix(const be<f32> *, be<f32> *);
s32 TagLight_resLoad(request_of_phase_process_class *, const char *);
void TagLight_cullBox(fopAc_ac_c *, f32, f32, f32, f32, f32, f32);
f32 TagLight_collisionRadius(Act_c *);
s32 TagLight_createResult(Act_c *);
s32 TagLight_deleteResult(Act_c *);
s32 TagLight_executeResult(Act_c *);
s32 TagLight_drawResult(Act_c *);
void TagLight_ZXYrotS(be<f32> *, s16, s16, s16);
s16 TagLight_atan2s(f32, f32);
f32 TagLight_sqrt(f32);
void TagLight_YrotM(be<f32> *, s16);
void TagLight_constructLineCheck(TagLightLineCheckHD *);
void TagLight_setLineCheck(TagLightLineCheckHD *, const cXyz *, const cXyz *, Act_c *);
u32 TagLight_lineCrossNonMoveBG(dBgS *, TagLightLineCheckHD *);
cM3dGPla *TagLight_getTriPlane(dBgS *, u32, u32);
void TagLight_destroyLineCheck(TagLightLineCheckHD *, s32);
// HD: material colors are represented as normalized float RGBA.
/* 024ACEA4 */
void tagLightColor(be<f32> *out, be<u8> *in) {
    f32 r = f32(u8(in[0])) / 255.0f, g = f32(u8(in[1])) / 255.0f, b = f32(u8(in[2])) / 255.0f,
        a = f32(u8(in[3])) / 255.0f;
    out[0] = r;
    out[1] = g;
    out[2] = b;
    out[3] = a;
}
/* 024AD0F4 */
void Act_c::get_base_model_light_start_offset(cXyz *out) {
    f32 ratio = f32(TagLight_parameter(this, PRM_CONERATIO_W, PRM_CONERATIO_S)) * 0.06666667014360428f;
    out->x = 0.0f;
    out->y = ratio < 0.5f ? 135.0f : 100.0f;
    out->z = 0.0f;
}
/* 024AD93C */
u8 Act_c::chk_sw() {
    s32 on = TagLight_parameter(this, PRM_SWSAVE_W, PRM_SWSAVE_S),
        off = TagLight_parameter(this, PRM_SWSAVE2_W, PRM_SWSAVE2_S);
    bool active = true;
    if (on != 255) {
        dSv_info_c *save = dComIfGs_info();
        s32 room = home.roomNo;
        active = TagLight_isSwitch(save, on, room) != 0;
    }
    if (off != 255) {
        dSv_info_c *save = dComIfGs_info();
        s32 room = home.roomNo;
        if (TagLight_isSwitch(save, off, room))
            return 0;
    }
    return active;
}
/* 024ADA2C */
u8 Act_c::chk_sch() {
    u8 mask = TagLight_parameter(this, PRM_SCH_W, PRM_SCH_S);
    u32 schedule = TagLight_schedule();
    return !mask || (mask & schedule) != 0;
}
/* 024ADA80 */
f32 Act_c::get_cc_r() {
    u32 index = TagLight_parameter(this, PRM_CCR_W, PRM_CCR_S);
    if (index >= 4)
        TagLight_assert("d_a_tag_light.cpp", 426, "pnt >= 0 && pnt < CcR_Max");
    return f32(s16(M_cc_r[index]));
}
/* 024AE044 */
u8 Act_c::_delete() {
    if (m2A0 == Type_Cone)
        TagLight_resDelete(&mPhase, M_arcname);
    return 1;
}
/* 024AE080 */
u32 Act_c::chk_light() {
    if (mSph.ChkTgHit()) {
        mSph.ClrTgHit();
        return 1;
    }
    void *detect = TagLight_detect();
    return TagLight_detectLight(detect, &current.pos);
}
/* 024AE0E8 */
u8 Act_c::_execute() {
    if (m2A0 == Type_Detector) {
        bool hit = chk_light();
        s16 count = s16(u16(m306) + (hit ? 1u : 0xFFFFu));
        if (count < 0)
            count = 0;
        else if (count > 20)
            count = 20;
        m306 = count;
        if (count > 10) {
            s32 index = TagLight_parameter(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
            dSv_info_c *save = dComIfGs_info();
            s32 room = home.roomNo;
            TagLight_onSwitch(save, index, room);
        }
        dComIfG_Ccsp_Set(&mSph);
    } else {
        u8 enabled = (TagLight_checkSwitch(this) != 0) && (TagLight_checkSchedule(this) != 0);
        m304 = enabled;
        f32 target = enabled ? 1.0f : 0.0f;
        s32 fade = TagLight_parameter(this, PRM_FADETYPE_W, PRM_FADETYPE_S);
        f32 speed = f32(M_alpha_spd[u32(fade)]);
        TagLight_chaseAlpha(&m308, target, speed);
        mDoExt_btkAnm *btk = mpBtk;
        if (btk)
            TagLight_playAnimation(btk);
    }
    return 1;
}
/* 024AE5F8 */
void TagLight_baseDestructor(Act_c *a) {}
/* 024AE668 */
u32 Mthd_IsDelete(Act_c *a) {
    return 1;
}
/* 024AE670 */
s32 PrmAbstract(Act_c *a, u32 width, u32 shift) {
    u32 mask = (width & 32) ? 0u : (1u << (width & 31));
    u32 value = (shift & 32) ? 0u : (u32(a->mParameters) >> (shift & 31));
    return value & (mask - 1);
}
/* 024AE5E4 */
void TagLight_trivialDestructor(void *a, u32 flags) {
    if (a && (flags & 1))
        operator_delete(a);
}
/* 024AE5FC */
void TagLight_actorDestructor(Act_c *a, u32 flags) {
    if (a) {
        TagLight_destroySphere(&a->mSph, 2);
        TagLight_destroyStatus(&a->mStts, 2);
        TagLight_destroyActor(a, 0);
        if (flags & 1)
            operator_delete(a);
    }
}
// HD: preserve a 16-byte guest linkage cushion, including the callee SP+4 LR word.
template <class T> struct LightLocal {
    struct Slot {
        u8 linkage[16];
        T value;
    };
    gabi::Local<Slot> storage;
    T *get() {
        return &storage->value;
    }
};
/* 024ACF58 */
u8 Act_c::create_heap() {
    f32 ratio = f32(TagLight_parameter(this, PRM_CONERATIO_W, PRM_CONERATIO_S)) * 0.06666667014360428f;
    s32 modelIndex = ratio < 0.5666667222976685f ? 4 : 5,
        animationIndex = ratio < 0.5666667222976685f ? 8 : 9;
    J3DModelData *modelData = static_cast<J3DModelData *>(dComIfG_getObjectRes(M_arcname, modelIndex));
    if (!modelData)
        TagLight_assert("d_a_tag_light.cpp", 467, "modelData != 0");
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000u, 0x11000222u);
    J3DAnmTextureSRTKey *animation =
        static_cast<J3DAnmTextureSRTKey *>(dComIfG_getObjectRes(M_arcname, animationIndex));
    if (!animation)
        TagLight_assert("d_a_tag_light.cpp", 479, "btk_data != 0");
    mDoExt_btkAnm *btk = static_cast<mDoExt_btkAnm *>(operator_new(0x74));
    if (btk)
        btk = TagLight_constructBtk(btk);
    mpBtk = btk;
    bool initialized = false;
    if (btk)
        initialized = TagLight_initBtk(btk, modelData, animation, 1, 2, 0, 1.0f, -1, 0, 0) != 0;
    return mpModel.get() != nullptr && initialized;
}
/* 024AD0F0 */
// HD: callbacks and method wrappers retain full return registers.
s32 solidHeapCB(Act_c *a) {
    return TagLight_createHeapResult(a);
}
/* 024AD894 */
void Act_c::set_spot_mtx() {
    struct Vectors {
        cXyz bottom, end, center, offset, start, point, normal;
    };
    LightLocal<Vectors> locals;
    auto *v = locals.get();
    TagLight_startOffset(this, &v->offset);
    v->bottom.x = 0;
    v->bottom.y = -50;
    v->bottom.z = 0;
    TagLight_multVec(m2A4, &v->offset, &v->start);
    TagLight_multVec(m2A4, &v->bottom, &v->end);
    TagLight_projectionInfo(this, &v->point, &v->normal, &v->center, &v->start, &v->end);
    TagLight_projectionMatrix(this, &v->point, &v->normal, &v->center, &v->start);
}
/* 024AD934 */
void Act_c::set_spot() {
    TagLight_setSpotMatrix(this);
}
/* 024AD938 */
void Act_c::init_spot() {
    TagLight_setSpot(this);
}
/* 024AE290 */
void set_material(TagLightMaterialHD *material, u32 alpha) {
    if (!alpha) {
        while (material) {
            material->mpShape->mVisible = 0;
            material = material->mpNext;
        }
        return;
    }
    // HD: update the TEV color and linear float material block through their native accessors.
    struct Colors {
        be<f32> linear[4], rgba[4];
    };
    LightLocal<Colors> colors;
    while (material) {
        material->mpShape->mVisible = 1;
        TagLightTevBlockHD *tev = material->mpTev;
        be<u8> *color = TagLight_getKColor(tev, 3);
        color[3] = u8(alpha);
        tev = material->mpTev;
        color = TagLight_getKColor(tev, 3);
        tev = material->mpTev;
        TagLight_setKColor(tev, 3, color);
        TagLight_color(colors.get()->rgba, color);
        TagLight_linearColor(colors.get()->linear, colors.get()->rgba, 1.0f);
        material->mColorBlock.mFlags = (u32)material->mColorBlock.mFlags | 0x400;
        be<f32> *output = TagLight_materialColor(&material->mColorBlock, 10);
        f32 opacity = f32(u8(color[3])) / 255.0f;
        f32 red = colors.get()->linear[0], green = colors.get()->linear[1], blue = colors.get()->linear[2];
        auto *rgba = output;
        rgba[0] = red;
        rgba[1] = green;
        rgba[2] = blue;
        rgba[3] = opacity;
        material = material->mpNext;
    }
}
/* 024AE430 */
u8 Act_c::_draw() {
    if (m2A0 == Type_Cone && f32(m308) > 0.0f) {
        dScnKy_env_light_c *env = TagLight_env();
        TagLight_settingTev(env, 1, &current.pos, &tevStr);
        env = TagLight_env();
        J3DModel *model = mpModel;
        TagLight_setLight(env, model, &tevStr);
        mDoExt_btkAnm *btk = mpBtk;
        if (btk) {
            model = mpModel;
            f32 frame = btk->getFrame();
            J3DModelData *data = model->getModelData();
            TagLight_entryBtk(btk, data, frame);
        }
        u8 alpha = u8(gabi::ftoi(f32(m308) * 255.5f));
        J3DModelData *data = mpModel->getModelData();
        TagLight_setMaterial(TagLight_jointMesh(data, 1), u32(alpha));
        TagLight_setMaterial(TagLight_jointMesh(data, 2), u32(alpha));
        model = mpModel;
        TagLight_updateModel(model, 0);
    }
    return 1;
}
/* 024AE550 */
void __sinit_d_a_tag_light_cpp() {
    // HD: retain zero-word stores and global registration order.
    auto *v = l_header.mZero;
    v[2] = 0;
    v[0] = 0;
    v[3] = 0;
    v[1] = 0;
    TagLight_registerGlobal(&l_registration.mRecords[0]);
    l_header.mPiMin = -3.1415927410125732f;
    l_header.mPiMax = 3.1415927410125732f;
    TagLight_initHeaderByte(&l_header.mAngleByte);
    TagLight_registerGlobal(&l_registration.mRecords[1]);
    TagLight_initHeaderFlag(&l_header.mAngleFlag);
    TagLight_registerGlobal(&l_registration.mRecords[2]);
}
/* 024ADB04 */
s32 Act_c::_create() {
    if (!(u32(actor_condition) & 8)) {
        if (this) {
            TagLight_constructActor(this);
            __vtbl = 0x1003F7DC;
            dCcD_Stts_ct(&mStts);
            TagLight_constructSphere(&mSph);
        }
        actor_condition = u32(actor_condition) | 8;
    }
    s32 type = TagLight_parameter(this, PRM_TYPE_W, PRM_TYPE_S);
    f32 z = scale.z;
    m2A0 = type;
    f32 y = scale.y, x = scale.x;
    f32 scaledZ = z;
    if (type == Type_Cone)
        y = y + y;
    else if (type == Type_Box) {
        x = x + x;
        y = y + y;
        scaledZ = z + z;
    }
    m4A4.x = x;
    m4A4.z = scaledZ;
    m4A4.y = y;
    be<f32> *stack = &mDoMtx_stack_c::get()->m[0][0];
    f32 py = current.pos.y, px = current.pos.x, pz = current.pos.z;
    TagLight_transS(stack, px, py, pz);
    s16 rz = shape_angle.z, rx = shape_angle.x, ry = shape_angle.y;
    TagLight_ZXYrotM(stack, rx, ry, rz);
    TagLight_scaleM(x, y, scaledZ);
    TagLight_copyMatrix(stack, m2A4);
    TagLight_inverseMatrix(m2A4, m2D4);
    s32 phase = cPhs_COMPLEATE_e;
    if (m2A0 == Type_Cone) {
        phase = TagLight_resLoad(&mPhase, M_arcname);
        if (phase != cPhs_COMPLEATE_e)
            return phase;
        if (!fopAcM_entrySolidHeap(this, (BOOL (*)(fopAc_ac_c *))solidHeapCB, 0xAC0))
            return cPhs_ERROR_e;
        pz = current.pos.z;
        px = current.pos.x;
        py = current.pos.y;
        TagLight_transS(stack, px, py, pz);
        rz = shape_angle.z;
        rx = shape_angle.x;
        ry = shape_angle.y;
        TagLight_ZXYrotM(stack, rx, ry, rz);
        f32 values[12];
        for (int i = 0; i < 12; i++)
            values[i] = f32(stack[i]);
        auto *m = &mpModel->mBaseTransformMtx.m[0][0];
        for (int i = 0; i < 12; i++)
            m[i] = values[i];
        // HD: base scale is stored z, x, then y after the matrix copy.
        m = &mpModel->mBaseScale.x;
        m[2] = scaledZ;
        m[0] = x;
        m[1] = y;
        TagLight_initSpot(this);
    }
    cullMtx = gabi::ea(m2A4);
    TagLight_cullBox(this, -51.0f, -1.0f, -51.0f, 51.0f, 101.0f, 51.0f);
    bool active = false;
    if (m2A0 != Type_Detector)
        active = TagLight_checkSwitch(this) && TagLight_checkSchedule(this);
    s32 currentType = m2A0;
    m304 = active;
    m306 = 0;
    m308 = active ? 1.0f : 0.0f;
    if (currentType == Type_Detector) {
        mStts.Init(255, 255, this);
        mSph.Set(reinterpret_cast<dCcD_SrcSph *>(&M_sph_src));
        mSph.SetStts(&mStts);
        mSph.SetC(&current.pos);
        f32 radius = TagLight_collisionRadius(this);
        mSph.SetR(radius);
    }
    return phase;
}
/* 024AE540 */
s32 Mthd_Create(Act_c *a) {
    return TagLight_createResult(a);
}
/* 024AE544 */
s32 Mthd_Delete(Act_c *a) {
    return TagLight_deleteResult(a);
}
/* 024AE548 */
s32 Mthd_Execute(Act_c *a) {
    return TagLight_executeResult(a);
}
/* 024AE54C */
s32 Mthd_Draw(Act_c *a) {
    return TagLight_drawResult(a);
}
/* 024AD594 */
void Act_c::make_projection_mtx(const cXyz *point, const cXyz *normal, const cXyz *center,
                                const cXyz *start) {
    bool narrow =
        f32(TagLight_parameter(this, PRM_CONERATIO_W, PRM_CONERATIO_S)) * 0.06666667014360428f < 0.5f;
    struct Data {
        cXyz normal, rotated;
        be<f32> matrix[12];
    };
    LightLocal<Data> local;
    auto *d = local.get();
    d->normal.x = normal->x;
    d->normal.y = normal->y;
    d->normal.z = normal->z;
    be<f32> *stack = &mDoMtx_stack_c::get()->m[0][0];
    s16 x = shape_angle.x, z = shape_angle.z, y = shape_angle.y;
    TagLight_ZXYrotS(stack, x, y, z);
    TagLight_copyMatrix(stack, d->matrix);
    TagLight_multVec(d->matrix, &d->normal, &d->rotated);
    f32 xx = d->rotated.x, zz = d->rotated.z;
    s16 heading = TagLight_atan2s(zz, xx);
    zz = d->rotated.z;
    xx = d->rotated.x;
    f32 length = TagLight_sqrt(gabi::fmadds(xx, xx, zz * zz));
    f32 yy = d->rotated.y;
    s16 inclination = TagLight_atan2s(length, std::fabs(yy));
    be<f32> *cosineValue = &l_sinCos.mEntries[u32(u16(inclination)) >> 3].mCosine;
    f32 cosine = f32(*cosineValue);
    if (std::fabs(cosine) < 0.0010000000474974513f) {
        m305 = 0;
        return;
    }
    m305 = 1;
    f32 halfX = f32(m4A4.x) / f32(*cosineValue) * 0.5f, halfZ = f32(m4A4.z) * 0.5f;
    f32 py = point->y, px, pz, factor;
    if (!narrow) {
        px = point->x;
        pz = point->z;
        factor = 1.034999966621399f;
    } else {
        f32 sy = start->y, ay = current.pos.y, ratio = (py - sy) / (ay - sy);
        f32 adjust = f32(l_projectionAdjust);
        py = center->y;
        px = center->x;
        pz = center->z;
        halfZ = halfZ * ratio;
        halfX = halfX * ratio;
        factor = adjust + 0.9700000286102295f;
    }
    TagLight_transS(stack, px, py, pz);
    y = shape_angle.y;
    TagLight_YrotM(stack, s16(u16(y) - u16(heading)));
    y = shape_angle.y;
    TagLight_YrotM(stack, s16(-s32(y)));
    f32 middle = (halfX + halfZ) * 0.5f;
    TagLight_scaleM(halfX * factor, middle * factor, halfZ * factor);
    y = shape_angle.y;
    TagLight_YrotM(stack, y);
    TagLight_copyMatrix(stack, m474);
}
/* 024AD198 */
void Act_c::get_projection_info(cXyz *point, cXyz *normal, cXyz *center, const cXyz *start, const cXyz *end) {
    center->copy(current.pos);
    bool narrow =
        f32(TagLight_parameter(this, PRM_CONERATIO_W, PRM_CONERATIO_S)) * 0.06666667014360428f < 0.5f;
    // HD: keep this aggregate in the unit header so its query remains one guest stack object.
    LightLocal<TagLightProjectionInfoLocalsHD> local;
    auto *d = local.get();
    TagLightLineCheckHD *check = &d->check;
    TagLight_constructLineCheck(check);
    check->mpPolyPass = &check->mPolyVtable;
    check->mpGroupPass = &check->mGroupVtable;
    point->copy(current.pos);
    check->mCheckVtable = 0x1003F79C;
    check->mGroupVtable = 0x1003F7BC;
    normal->copy(l_baseY);
    for (int i = 1; i < 7; i++)
        check->mPass[i] = 0;
    check->mPass[0] = 1;
    check->mGroup = 1;
    check->mPolyInfoVtable = 0x1003F7AC;
    check->mPolyVtable = 0x1003F7CC;
    TagLight_setLineCheck(check, start, end, this);
    dBgS *background = dComIfG_Bgsp();
    if (TagLight_lineCrossNonMoveBG(background, check)) {
        point->x = f32(check->mEnd.x);
        point->y = f32(check->mEnd.y);
        point->z = f32(check->mEnd.z);
        background = dComIfG_Bgsp();
        u16 triangle = check->mBgIndex, mesh = check->mPolyIndex;
        cM3dGPla *plane = TagLight_getTriPlane(background, u32(triangle), u32(mesh));
        if (plane) {
            normal->x = f32(plane->mNormal.x);
            normal->y = f32(plane->mNormal.y);
            normal->z = f32(plane->mNormal.z);
            if (narrow) {
                f32 width = f32(m4A4.x) * 12.0f, depth = f32(m4A4.z) * 12.0f;
                for (int i = 0; i < 4; i++) {
                    d->cross[i].copy(*point);
                    d->offsets[0].x = width;
                    d->offsets[0].y = 0;
                    d->offsets[0].z = 0;
                    d->offsets[1].x = -width;
                    d->offsets[1].y = 0;
                    d->offsets[1].z = 0;
                    d->offsets[2].x = 0;
                    d->offsets[2].y = 0;
                    d->offsets[2].z = depth;
                    d->offsets[3].x = 0;
                    d->offsets[3].y = 0;
                    d->offsets[3].z = -depth;
                    TagLight_multVec(m2A4, &d->offsets[i], &d->transformed);
                    f32 ny = f32(plane->mNormal.y), ty = d->transformed.y, sy = start->y,
                        tx = d->transformed.x, sx = start->x, sz = start->z;
                    f32 dx = tx - sx, dy = ty - sy, tz = d->transformed.z, dz = tz - sz,
                        nx = f32(plane->mNormal.x), nz = f32(plane->mNormal.z);
                    // HD: fused operations and plane/component loads follow native order.
                    f32 denominator = gabi::fmadds(nz, dz, gabi::fmadds(nx, dx, ny * dy)),
                        distance = f32(plane->mD);
                    if (denominator != 0) {
                        f32 numerator = gabi::fmadds(nz, sz, gabi::fmadds(nx, sx, ny * sy)) + distance,
                            t = -numerator / denominator;
                        d->cross[i].x = gabi::fmadds(dx, t, sx);
                        d->cross[i].y = gabi::fmadds(dy, t, sy);
                        d->cross[i].z = gabi::fmadds(dz, t, sz);
                    }
                }
                f32 cx =
                    ((f32(d->cross[0].x) + f32(d->cross[1].x)) + f32(d->cross[2].x)) + f32(d->cross[3].x);
                f32 cy =
                    ((f32(d->cross[0].y) + f32(d->cross[1].y)) + f32(d->cross[2].y)) + f32(d->cross[3].y);
                f32 cz =
                    ((f32(d->cross[0].z) + f32(d->cross[1].z)) + f32(d->cross[2].z)) + f32(d->cross[3].z);
                center->x = cx * 0.25f;
                center->y = cy * 0.25f;
                center->z = cz * 0.25f;
            }
        }
    }
    check->mPolyVtable = 0x1003F78C;
    check->mGroupVtable = 0x1003F74C;
    check->mPolyInfoVtable = 0x1003F73C;
    TagLight_destroyLineCheck(check, 0);
}

/* WWHD glue
data M_cc_r 1003F8A0
data M_alpha_spd 1003F8F0
data M_sph_src 1003F8B0
data l_header 1046E57C
data l_registration 101D1CF0
data l_sinCos 104A44F8
data l_baseY 101FFBC0
data l_projectionAdjust 1047C150
str 1003F84C in 024ADA80 # "d_a_tag_light.cpp"
str 1003F860 # "pnt >= 0 && pnt < CcR_Max"
str 1003F7F0 in 024ACF58 # "d_a_tag_light.cpp"
str 1003F814 # "modelData != 0"
str 1003F804 # "btk_data != 0"
str 1003F8A8 # "Mspot"
ext 024AE670 s32 TagLight_parameter(Act_c*, u32, u32)
ext 025BA0C0 u32 TagLight_isSwitch(dSv_info_c*, s32, s32)
ext 025602A8 u32 TagLight_schedule()
ext 0273AA24 void TagLight_assert(const char*, s32, const char*)
ext 025204C8 void TagLight_resDelete(request_of_phase_process_class*, const char*)
ext 0252A038 u32 TagLight_detectLight(void*, cXyz*)
ext 025B9E38 void TagLight_onSwitch(dSv_info_c*, s32, s32)
ext 024AD93C u32 TagLight_checkSwitch(Act_c*)
ext 024ADA2C u32 TagLight_checkSchedule(Act_c*)
ext 0200F5C8 s32 TagLight_chaseAlpha(be<f32>*, f32, f32)
ext 025E742C void TagLight_playAnimation(mDoExt_btkAnm*)
ext 02515AE8 void TagLight_destroySphere(dCcD_Sph*, s32)
ext 02515860 void TagLight_destroyStatus(dCcD_Stts*, s32)
ext 025D50BC void TagLight_destroyActor(fopAc_ac_c*, s32)
ext 025E7C6C mDoExt_btkAnm* TagLight_constructBtk(mDoExt_btkAnm*)
ext 025E7CE0 u32 TagLight_initBtk(mDoExt_btkAnm*, J3DModelData*, J3DAnmTextureSRTKey*, s32, s32, s32, f32, s32, s32, s32)
ext 024ACF58 s32 TagLight_createHeapResult(Act_c*)
ext 024AD0F4 void TagLight_startOffset(Act_c*, cXyz*)
ext 028E8F64 void TagLight_multVec(const be<f32>*, const cXyz*, cXyz*)
ext 024AD198 void TagLight_projectionInfo(Act_c*, cXyz*, cXyz*, cXyz*, const cXyz*, const cXyz*)
ext 024AD594 void TagLight_projectionMatrix(Act_c*, const cXyz*, const cXyz*, const cXyz*, const cXyz*)
ext 024AD894 void TagLight_setSpotMatrix(Act_c*)
ext 024AD934 void TagLight_setSpot(Act_c*)
ext 024AD938 void TagLight_initSpot(Act_c*)
ext 024ACEA4 void TagLight_color(be<f32>*, be<u8>*)
ext 0274D458 void TagLight_linearColor(be<f32>*, be<f32>*, f32)
ext 027F9F0C be<f32>* TagLight_materialColor(TagLightColorBlockHD*, s32)
ext 02555D0C dScnKy_env_light_c* TagLight_env()
ext 025626A4 void TagLight_settingTev(dScnKy_env_light_c*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void TagLight_setLight(dScnKy_env_light_c*, J3DModel*, dKy_tevstr_c*)
ext 025E7FC4 void TagLight_entryBtk(mDoExt_btkAnm*, J3DModelData*, f32)
ext 024AE290 void TagLight_setMaterial(TagLightMaterialHD*, u32)
ext 025E2DE0 void TagLight_updateModel(J3DModel*, u32)
ext 028F026C void TagLight_registerGlobal(TagLightRegistrationHD*)
ext 028ED6F8 void TagLight_initHeaderByte(be<u8>*)
ext 028EAB2C void TagLight_initHeaderFlag(be<u8>*)
ext 025D4ED0 void TagLight_constructActor(fopAc_ac_c*)
ext 025166F0 void TagLight_constructSphere(dCcD_Sph*)
ext 028E93CC void TagLight_transS(be<f32>*, f32, f32, f32)
ext 025F1B48 void TagLight_ZXYrotM(be<f32>*, s16, s16, s16)
ext 025F2518 void TagLight_scaleM(f32, f32, f32)
ext 028E90D4 void TagLight_copyMatrix(const be<f32>*, be<f32>*)
ext 028E91EC s32 TagLight_inverseMatrix(const be<f32>*, be<f32>*)
ext 02520460 s32 TagLight_resLoad(request_of_phase_process_class*, const char*)
ext 025D674C void TagLight_cullBox(fopAc_ac_c*, f32, f32, f32, f32, f32, f32)
ext 024ADA80 f32 TagLight_collisionRadius(Act_c*)
ext 024ADB04 s32 TagLight_createResult(Act_c*)
ext 024AE044 s32 TagLight_deleteResult(Act_c*)
ext 024AE0E8 s32 TagLight_executeResult(Act_c*)
ext 024AE430 s32 TagLight_drawResult(Act_c*)
ext 025F1AA4 void TagLight_ZXYrotS(be<f32>*, s16, s16, s16)
ext 020195B0 s16 TagLight_atan2s(f32, f32)
ext 028F4384 f32 TagLight_sqrt(f32)
ext 025F1C28 void TagLight_YrotM(be<f32>*, s16)
ext 02008FEC void TagLight_constructLineCheck(TagLightLineCheckHD*)
ext 024F1AFC void TagLight_setLineCheck(TagLightLineCheckHD*, const cXyz*, const cXyz*, Act_c*)
ext 024EF4A8 u32 TagLight_lineCrossNonMoveBG(dBgS*, TagLightLineCheckHD*)
ext 020084C8 cM3dGPla* TagLight_getTriPlane(dBgS*, u32, u32)
ext 02008B4C void TagLight_destroyLineCheck(TagLightLineCheckHD*, s32)
*/
