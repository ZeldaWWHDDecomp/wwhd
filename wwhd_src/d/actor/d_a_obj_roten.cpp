/* Zunari's outdoor shop posts and curtains. GameCube names with verified WWHD
 * behavior. */
#include "d/actor/d_a_obj_roten.h"

const gc_tu_data gc_tu = {.safeString = 0x1002EDE4};
static const char* M_arcname = "Roten";
static daObj_Roten_HIO_c l_HIO;
// HD: the background callback is stored as a guest entry address.
static const u32 dBgS_MoveBGProc_Typical = 0x024EE658;

// HD: placement construction stores the actor vtable.
daObj_Roten_c::daObj_Roten_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1002EE0C;
}

/* 02388A1C */
void daObj_Roten_c::set_mtx() {
    // HD: retain all scale loads before model stores, then copy matrix floats.
    f32 x = scale.x, y = scale.y;
    J3DModel* model = mpModel;
    f32 z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    f32 matrix[12];
    for (u32 i = 0; i < 12; i++)
        matrix[i] = mDoMtx_stack_c::get()->m[i / 4][i % 4];
    model = mpModel;
    for (u32 i = 0; i < 12; i++)
        model->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
}

/* 02388A18 */
BOOL CheckCreateHeap(fopAc_ac_c* i_this) { return static_cast<daObj_Roten_c*>(i_this)->CreateHeap(); }

/* 02388908 */
BOOL daObj_Roten_c::CreateHeap() {
    // HD: numeric Roten02/03/04 indices keep autoglue image checks explicit.
    static u32 bdl_arc_idx[] = {
        4,
        5,
        6,
    };
    static u32 dzb_arc_idx[] = {
        9,
        10,
        11,
    };
    u32 idx = mType;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, bdl_arc_idx[idx]);
    JUT_ASSERT(0xB0, modelData != NULL);
    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    RotenBgW* bg = static_cast<RotenBgW*>(new_dBgW());
    mpBgW = bg;
    // HD: collision setup is skipped if allocation fails.
    if (bg) {
        idx = mType;
        cBgD_t* bgData = (cBgD_t*)dComIfG_getObjectRes(M_arcname, dzb_arc_idx[idx]);
        J3DModel* model = mpModel;
        bg = mpBgW;
        bg->Set(bgData, dBgW::MOVE_BG_e, model->getBaseTRMtx());
    }
    return mpModel != nullptr && mpBgW != nullptr;
}

/* 02388AE8 */
s32 daObj_Roten_c::CreateInit() {
    set_mtx();
    J3DModel* model = mpModel;
    RotenBgW* bg = mpBgW;
    fopAcM_SetMtx(this, model->getBaseTRMtx());
    bg->SetCrrFunc(dBgS_MoveBGProc_Typical);
    dBgS* bgs = dComIfG_Bgsp();
    RotenBgW* w = mpBgW;
    bgs->Regist(w, this);
    return cPhs_COMPLEATE_e;
}

static BOOL dComIfGs_isGetItemReserve(u32 index) {
    return dSv_player_get_bag_item_isReserve(&dComIfGs_info()->mSavedata.mPlayer.mGetBagItem, index);
}

/* 02388B50 */
s32 daObj_RotenCreate(daObj_Roten_c* self) {
    fopAcM_ct(self, daObj_Roten_c);
    u32 type = fopAcM_GetParam(self) >> 24;
    if (type > 2)
        type = 2;
    self->mType = type;
    s32 count = 0;
    for (u32 i = 0; i < 12; i++) {
        if (dComIfGs_isGetItemReserve(i) || i == 0)
            count++;
    }
    s32 needed = type == 0 ? 3 : type == 1 ? 6 : 9;
    if (count <= needed) {
        self->field_0x2D1 = 1;
        return cPhs_ERROR_e;
    }
    self->field_0x2D1 = 0;
    s32 phase = dComIfG_resLoad(&self->mPhs, M_arcname);
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    // HD: reload the selected type after resource loading.
    const u32 heap_size[3] = {0x2BA0, 0x2A60, 0x2C60};
    u32 slot = self->mType;
    if (!fopAcM_entrySolidHeap(self, CheckCreateHeap, heap_size[slot]))
        return cPhs_ERROR_e;
    return self->CreateInit();
}

/* 02388CC4 */
BOOL daObj_RotenDelete(daObj_Roten_c* self) {
    if (!self->field_0x2D1) {
        dBgS* bgs = dComIfG_Bgsp();
        RotenBgW* w = self->mpBgW;
        bgs->Release(w);
        dComIfG_resDelete(&self->mPhs, M_arcname);
    }
    return TRUE;
}

/* 02388D1C */
BOOL daObj_RotenExecute(daObj_Roten_c* self) {
    self->set_mtx();
    self->mpBgW->Move();
    return FALSE;
}

/* 02388D54 */
BOOL daObj_RotenDraw(daObj_Roten_c* self) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &self->current.pos, &self->tevStr);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    J3DModel* model = self->mpModel;
    setLightTevColorType(env, model, &self->tevStr);
    model = self->mpModel;
    mDoExt_modelUpdateDL(model);
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 02388DB0 */
daObj_Roten_HIO_c* hioConstructor(daObj_Roten_HIO_c* self) {
    if (!self)
        self = (daObj_Roten_HIO_c*)operator_new(sizeof(daObj_Roten_HIO_c));
    if (self) {
        self->mNo = -1;
        self->field_0x08 = 0.0f;
        self->field_0x0C = 0;
        self->mVtable = 0x1002EDFC;
    }
    return self;
}

/* 02388E0C */
void __sinit_d_a_obj_roten_cpp() {
    sinit_header_statics_z(0x1046BD78, 0x101CC964, 0x1046BD94);
    hioConstructor(&l_HIO);
}

/* 02388EAC */
void hioDestructor(daObj_Roten_HIO_c* self, s32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}

/* 02388EC0 */
BOOL daObj_RotenIsDelete(daObj_Roten_c* self) { return TRUE; }

/* 02388EC8 */
void actorDestructor(daObj_Roten_c* self, s32 flags) {
    if (self) {
        __dt(static_cast<fopAc_ac_c*>(self));
        if (flags & 1)
            operator_delete(self);
    }
}

/* 02388F1C */
void emptyVirtual(daObj_Roten_c* self) {}

/* WWHD glue (tools/verify/autoglue.py)
str 1002EE60
str 1002EE20
str 1002EE34 "modelData != NULL" renamed # HD: assertion spells zero instead of NULL.
data bdl_arc_idx 101CC94C
data dzb_arc_idx 101CC958
data l_HIO 1046BD84
ext 025B7840 BOOL dSv_player_get_bag_item_isReserve(dSv_player_get_bag_item_c*, u32)
*/
