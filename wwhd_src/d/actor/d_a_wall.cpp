/**
 * Object - Bombable walls, using zeldaret/tww names and WWHD layouts.
 * Verified against the original by d_a_wall. Private game-derived source.
 */
#include "d/actor/d_a_wall.h"

const gc_tu_data gc_tu = {.safeString = 0x10041F64};
static WallResourceTable m_heapsize;
static WallResourceTable m_bmdname;
static WallResourceTable m_dzbname;
static WallArchiveTable m_arcname;
static WallVertexTable m_tri_vtx;
static WallCullTable m_cull_size;
static GXColor m_smoke_color;
static WallSourceTriangle l_tri_src;
static WallModeTable mode_proc = {{&daWall_c::mode_wait, &daWall_c::mode_break}};
static WallHeaderStatics l_headerStatics;
static WallDestructorRecords l_destructorRecords;
static f32 l_headerMin;
static f32 l_headerMax;
// HD: this TU's actor/AABB vtables and the shared collision-shape vtables.
static constexpr u32 actorVtable = 0x10041F8C;
static constexpr u32 aabVtable = 0x10041F7C;
static constexpr u32 shapeBaseVtable = 0x100015A8;
static constexpr u32 triangleHitVtable = 0x1004B010;
static constexpr u32 triangleGeometryVtable = 0x1004B058;
static constexpr u32 triangleShapeVtable = 0x1004B068;

static WallTriangle* dCcD_Tri_ct(WallTriangle*);

/* 024D4258 */
s32 daWall_c::_delete() {
    mSmokeCb.remove();
    if (heap != NULL && !mState)
        dComIfG_Bgsp()->Release(mpBgW);
    dComIfG_resDelete(&mPhs, m_arcname.names[mType]);
    return TRUE;
}

/* 024D3D6C */
static s32 CheckCreateHeap(daWall_c* i_this) { return i_this->CreateHeap(); }

/* 024D3C44 */
s32 daWall_c::CreateHeap() {
    gabi::AutoFrame frame{gabi::current_function()};
    WallStackPayload<SafeString> modelName, backgroundName;
    const char* name = m_arcname.names[mType];
    s32 index = m_bmdname.values[mType];
    modelName.get()->mStringTop = gabi::guest_addr(name);
    modelName.get()->__vtbl = gc_tu.safeString;
    J3DModelData* modelData = (J3DModelData*)Wall_getRes(dComIfG_resControl(), modelName.get(), index);
    if (modelData == NULL)
        JUT_ASSERT_fail("d_a_wall.cpp", 0x184, "modelData != (0)");
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    mpModel = model;
    if (model == NULL)
        return FALSE;
    setMoveBGMtx();
    dBgW* background = new_dBgW();
    mpBgW = background;
    if (background == NULL)
        return FALSE;
    name = m_arcname.names[mType];
    index = m_dzbname.values[mType];
    backgroundName.get()->mStringTop = gabi::guest_addr(name);
    backgroundName.get()->__vtbl = gc_tu.safeString;
    cBgD_t* data = (cBgD_t*)Wall_getRes(dComIfG_resControl(), backgroundName.get(), index);
    // HD: this call site tests the full signed return, rather than a truncated bool.
    return WallBgW_Set(mpBgW, data, cBgW_MOVE_BG_e, &mMtx) == 0;
}

/* 024D3FE0 */
void daWall_c::CreateInit() {
    J3DModel* model = mpModel;
    u32 type = mType;
    fopAcM_SetMtx(this, model ? &model->mBaseTransformMtx : NULL);
    const cXyz* bounds = m_cull_size.values[type];
    fopAcM_setCullSizeBox(this, bounds[0].x, bounds[0].y, bounds[0].z, bounds[1].x, bounds[1].y, bounds[1].z);
    fopAcM_setCullSizeFar(this, 1.0f);
    mStts.Init(0xFF, 0xFF, this);
    set_tri();
    dComIfG_Bgsp()->Regist(mpBgW, this);
    set_mtx();
    mpBgW->Move();
    u8 switchNo = ((WallParameters*)&mParameters)->switchNo;
    mState = WALL_WAIT;
    mSwitchNo = switchNo;
}

/* 024D40AC */
s32 daWall_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        // HD: nullable placement construction and the original array callback ABI.
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = actorVtable;
            dCcD_Stts_ct(&mStts);
            WallTriangleArray_ct(mTri, 2, sizeof(WallTriangle), gabi::guest_fn((const void*)dCcD_Tri_ct));
            WallSmokeCallback_ct(&mSmokeCb, &m_smoke_color, NULL, 0);
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    u32 type = ((u32)mParameters >> 8) & 0xFF;
    u8 switchNo = ((WallParameters*)&mParameters)->switchNo;
    // HD: invalid variants select the third wall.
    mType = type < 3 ? type : 2;
    mSwitchNo = switchNo;
    if (dComIfGs_isSwitch(switchNo, home.roomNo))
        return cPhs_ERROR_e;
    if (mSwitchNo == 0xFF)
        return cPhs_ERROR_e;
    s32 phase_state = dComIfG_resLoad(&mPhs, m_arcname.names[mType]);
    if (phase_state == cPhs_COMPLEATE_e) {
        s32 heapSize = m_heapsize.values[mType];
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)CheckCreateHeap), heapSize))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase_state;
}

/* 024D3F08 */
void daWall_c::set_mtx() {
    // HD: load the complete scale before loading the model, and all matrix values before stores.
    f32 x = scale.x, y = scale.y, z = scale.z;
    J3DModel* model = mpModel;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    f32 values[3][4];
    for (u32 i = 0; i < 3; i++)
        for (u32 j = 0; j < 4; j++)
            values[i][j] = mDoMtx_stack_c::get()->m[i][j];
    model = mpModel;
    for (u32 i = 0; i < 3; i++)
        for (u32 j = 0; j < 4; j++)
            model->mBaseTransformMtx.m[i][j] = values[i][j];
}

/* 024D3BD4 */
void daWall_c::setMoveBGMtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}

/* 024D433C */
s32 daWall_c::_execute() {
    (this->*mode_proc.entries[mState])();
    return TRUE;
}

/* 024D46D4 */
void daWall_c::mode_wait() {
    for (u32 i = 0; i < 2; i++) {
        if (mTri[i].ChkTgHit()) {
            dCcD_GObjInf* at = (dCcD_GObjInf*)mTri[i].GetTgHitObj();
            if (at != NULL && at->ChkAtType(WallCollision::AT_TYPE_BOMB)) {
                set_effect();
                set_se();
                break;
            }
            mTri[i].ClrTgHit();
        }
    }
    for (u32 i = 0; i < 2; i++)
        dComIfG_Ccsp_Set(&mTri[i]);
}

/* 024D4618 */
void daWall_c::mode_break() {
    u8 counter = mBreakCounter;
    if (counter != 0) {
        u8 type = mType;
        counter = (u8)(counter + 1);
        mBreakCounter = counter;
        if (type <= 2) {
            if (counter > 10 && cLib_chaseF(&mDst, 0.0f, 2.222222328186035f))
                fopAcM_delete(this);
            WallEmitter* pEmitter = (WallEmitter*)mSmokeCb.getEmitter();
            if (pEmitter != NULL)
                pEmitter->mGlobalAlpha = (u8)gabi::ftoi(mDst);
        } else {
            fopAcM_delete(this);
        }
    }
    fopAcM_offDraw(this);
}

/* inlined into daWall_c::set_tri */
static void copyVertex(cXyz* dst, const cXyz* src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

/* 024D3D70 */
void daWall_c::set_tri() {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 i = 0; i < 2; i++) {
        mTri[i].Set((const dCcD_SrcTri*)&l_tri_src);
        mTri[i].SetStts(&mStts);
    }
    WallStackPayload<WallVertices> vertices;
    cXyz* vertex = vertices.get()->values;
    u32 type = mType;
    for (u32 j = 0; j < 3; j++)
        copyVertex(&vertex[j], &m_tri_vtx.values[type][j]);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    for (u32 j = 0; j < 3; j++)
        mDoMtx_stack_c::multVec(&vertex[j], &vertex[j]);
    mTri[0].mTri.setPos(&vertex[0], &vertex[1], &vertex[2]);
    type = mType;
    for (u32 j = 0; j < 3; j++) {
        u32 index = j == 0 ? 0 : j + 1;
        copyVertex(&vertex[j], &m_tri_vtx.values[type][index]);
    }
    for (u32 j = 0; j < 3; j++)
        mDoMtx_stack_c::multVec(&vertex[j], &vertex[j]);
    mTri[1].mTri.setPos(&vertex[0], &vertex[1], &vertex[2]);
}

/* 024D43B4 */
void daWall_c::set_effect() {
    gabi::AutoFrame frame{gabi::current_function()};
    WallStackPayload<csXyz> reversedAngle;
    WallStackPayload<WallParticleIDs> debrisIds, smokeIds;
    const u16 projection_id[3] = {WallParticleName::ID_IT_ST_SHIREN_WL1_HAHEN00,
        WallParticleName::ID_IT_ST_SHIREN_WL2_HAHEN00, WallParticleName::ID_IT_ST_SHIREN_WL3_HAHEN00};
    const u16 particle_id[3] = {WallParticleName::ID_IT_ST_SHIREN_WL1_SMOKE00,
        WallParticleName::ID_IT_ST_SHIREN_WL2_SMOKE00, WallParticleName::ID_IT_ST_SHIREN_WL3_SMOKE00};
    for (u32 i = 0; i < 3; i++) {
        debrisIds.get()->values[i] = projection_id[i];
        smokeIds.get()->values[i] = particle_id[i];
    }
    mDst = 200.0f;
    reversedAngle.get()->x = current.angle.x;
    reversedAngle.get()->y = (s16)((s16)current.angle.y - 0x8000);
    reversedAngle.get()->z = current.angle.z;
    u32 type = mType;
    if (type <= 2) {
        s32 room = current.roomNo;
        u32 id = debrisIds.get()->values[type];
        dComIfGp_getParticle()->set(4, id, &current.pos, &current.angle, NULL, 0xFF, NULL,
                                  room, &tevStr.mColorK0, &tevStr.mColorK0, NULL);
        type = mType;
        room = current.roomNo;
        id = debrisIds.get()->values[type];
        dComIfGp_getParticle()->set(4, id, &current.pos, reversedAngle.get(), NULL, 0xFF, NULL,
                                  room, &tevStr.mColorK0, &tevStr.mColorK0, NULL);
        type = mType;
        room = current.roomNo;
        id = smokeIds.get()->values[type];
        u8 alpha = (u8)gabi::ftoi(mDst);
        WallEmitter* emitter = (WallEmitter*)dComIfGp_getParticle()->set(0, id, &current.pos,
            &current.angle, NULL, alpha, (dPa_levelEcallBack*)&mSmokeCb, room, NULL, NULL, NULL);
        mpEmitter = emitter;
        if (emitter != NULL)
            emitter->mFlags |= WallEmitter::STATUS_IMMORTAL_EMITTER;
    } else {
        mpEmitter = NULL;
    }
    mBreakCounter = 1;
    mState = WALL_BREAK;
    dComIfG_Bgsp()->Release(mpBgW);
    s32 switchNo = mSwitchNo;
    if (switchNo != 0xFF)
        dComIfGs_onSwitch(switchNo, home.roomNo);
}

/* 024D45D0 */
void daWall_c::set_se() {
    // HD: the sound uses eyePos, with the original signed 32-bit reverb result.
    s32 reverb = Wall_getReverb(current.roomNo);
    mDoAud_seStart(JA_SE_OBJ_BOMB_WALL_BRK, &eyePos, 0, reverb);
}

/* 024D42DC */
s32 daWall_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}

/* 024D4254 */
static s32 daWall_Create(daWall_c* i_this) { return i_this->_create(); }
/* 024D42D8 */
static s32 daWall_Delete(daWall_c* i_this) { return i_this->_delete(); }
/* 024D4338 */
static s32 daWall_Draw(daWall_c* i_this) { return i_this->_draw(); }
/* 024D43B0 */
static s32 daWall_Execute(daWall_c* i_this) { return i_this->_execute(); }
/* 024D48CC */
static s32 daWall_IsDelete(daWall_c*) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 024D4798 */
static void __sinit_d_a_wall_cpp() {
    for (u32 i = 0; i < 4; i++)
        l_headerStatics.zero[i] = 0;
    __register_global_object(gabi::ea(l_destructorRecords.entries[0]));
    l_headerStatics.minimum = l_headerMin;
    l_headerStatics.maximum = l_headerMax;
    WallFirstHeaderObject_ct(&l_headerStatics.first);
    __register_global_object(gabi::ea(l_destructorRecords.entries[1]));
    WallSecondHeaderObject_ct(&l_headerStatics.second);
    __register_global_object(gabi::ea(l_destructorRecords.entries[2]));
}
/* 024D482C */
static void SafeString_dtor(SafeString* object, u32 deleting) {
    if (object != NULL && (deleting & 1))
        operator_delete(object);
}
/* 024D4840 */
static WallTriangle* dCcD_Tri_ct(WallTriangle* triangle) {
    if (triangle == NULL) {
        triangle = (WallTriangle*)operator_new(sizeof(WallTriangle));
        if (triangle == NULL)
            return NULL;
    }
    WallGObjInf_ct(triangle);
    triangle->mShapeAttr.shapeVtable = shapeBaseVtable;
    triangle->mShapeAttr.aabVtable = aabVtable;
    WallTriangleGeometry_ct(&triangle->mTri);
    triangle->__vtbl_hitinf = triangleHitVtable;
    triangle->mTri.__vtbl = triangleGeometryVtable;
    triangle->mShapeAttr.shapeVtable = triangleShapeVtable;
    return triangle;
}
/* 024D48D4 */
static void daWall_c_dtor(daWall_c* i_this, u32 deleting) {
    if (i_this != NULL) {
        WallTriangleArray_dt(i_this->mTri, 2, sizeof(WallTriangle), 0x025159F8, 0, 0);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt((fopAc_ac_c*)i_this);
        if (deleting & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue: translation-unit data and other-unit calls.
str 10041F9C "d_a_wall.cpp"
str 10041FAC "modelData != (0)"
data m_heapsize 10041FE8
data m_bmdname 10041FF0
data m_dzbname 10041FF8
data m_arcname 101D2D20
data m_tri_vtx 10042000
data m_cull_size 10042090
data m_smoke_color 101D2CEC
data l_tri_src 101D2C74
data mode_proc 10041FC4
data l_headerStatics 1046EAD8
data l_destructorRecords 101D2CC8
data l_headerMin 10041FE0
data l_headerMax 10041FE4
ext 026066C4 void* Wall_getRes(dRes_control_c*, SafeString*, s32)
ext 0200A030 s32 WallBgW_Set(dBgW*, cBgD_t*, u32, Mtx34*)
ext 0251650C void WallTriangle::Set(const dCcD_SrcTri*)
ext 0201924C void WallTriangleGeometry::setPos(cXyz*, cXyz*, cXyz*)
ext 028EFFD0 void WallTriangleArray_ct(WallTriangle*, u32, u32, u32)
ext 028F0164 void WallTriangleArray_dt(WallTriangle*, u32, u32, u32, u32, u32)
ext 025A5CEC void WallSmokeCallback_ct(dPa_smokeEcallBack*, const GXColor*, void*, u32)
ext 02515FB8 void WallGObjInf_ct(WallTriangle*)
ext 02019040 void WallTriangleGeometry_ct(WallTriangleGeometry*)
ext 02520540 s32 Wall_getReverb(s32)
ext 028ED6F8 void WallFirstHeaderObject_ct(u8*)
ext 028EAB2C void WallSecondHeaderObject_ct(u8*)
*/
