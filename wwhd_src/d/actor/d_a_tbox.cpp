// WWHD treasure chest. Verified-source decompilation.
#include "d/actor/d_a_tbox.h"
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo.h"
static u32 daTbox_HIO_ct(u32 self);
static void daTbox_HIO_dt(u32 self, u32 flags);
static BOOL daTbox_IsDelete(daTbox_c* self);
static void emptyVirtual(u32 self);
static BOOL CheckCreateHeap(daTbox_c* self);
static BOOL daTbox_Execute(daTbox_c* self);
static BOOL daTbox_Delete(daTbox_c* self);
static void sinit_d_a_tbox();
static void daTbox_dtor(daTbox_c* self, u32 flags);
static s32 daTbox_Create(daTbox_c* self);
static daTbox_c* daTbox_ct(daTbox_c* self);
static BOOL daTbox_Draw(daTbox_c* self);

static daTbox_HIO_c l_HIO;
static TboxModelInfoTable l_modelInfo;
static TboxRoomControl l_roomControl;
static TboxResourceControl l_resourceControl;
static TboxDrawBuffers l_drawBuffers;
static TboxHeaderStatics l_headerStatics;
static TboxHeaderZeros l_headerZeros;
static TboxPiPair l_pi;

static u32 addr(const void* p) { return gabi::ea(p); }
static u32 play() { return dComIfGp_ea(); }
static u32 save() { return dComIfGs_ea() + 0x20; }

// inlined into the chest opening and appearance demos
static u32 particle(daTbox_c* self, u32 id, u32 angle, u32 type = 0, u32 cb = 0, u32 alpha = 255) {
    u32 p = play(), manager = addr((dPa_control_c*)tboxView<dComIfG_inf_c>(p)->play.mParticle);
    return tboxSetParticle(manager, type, id, &self->current.pos, angle, 0, alpha, cb, -1, 0, 0, 0);
}

// HD: preserve the eight-byte SafeString temporary and this TU's vtable.
// inlined into commonShapeSet, effectShapeSet and bgCheckSet
static u32 objectRes(u32 name, s32 index) {
    u32 control = l_resourceControl.mControl;
    gabi::Local<TboxSafeString> str;
    str->text = name;
    str->vtable = 0x1003FDC4;
    return tboxGetResource(control, str.get(), index);
}
// HD: GHS loads all twelve floats before storing any; this also preserves NaN quieting.
// inlined into commonShapeSet and execute
static void copyMtx(u32 src, u32 dst) {
    f32 v[12];
    for (int i = 0; i < 12; i++)
        v[i] = tboxView<Mtx34>(src)->m[i / 4][i % 4];
    for (int i = 0; i < 12; i++)
        tboxView<Mtx34>(dst)->m[i / 4][i % 4] = v[i];
}

static void cutEnd(daTbox_c* self) {
    s32 staff = self->mStaffId;
    u32 p = play();
    tboxCutEnd(p + 0x52C4, staff);
}

// HD: shader resources store self-relative offsets, with zero denoting null.
static u32 relativePtr(u32 object, u32 offset) {
    u32 p = object + offset, v = (u32)*tboxView<be<u32>>(p);
    return v ? p + v : 0;
}
static void markShaderParameter(u32 material, u32 descriptor, s32 index) {
    if (tboxView<TboxShaderParameter>(descriptor)->mIndex < 0)
        return;
    u16 flags = tboxView<TboxMaterial>(material)->mFlags;
    u32 mask = tboxView<TboxMaterial>(material)->mDirtyMask;
    tboxView<TboxMaterial>(material)->mFlags = flags | 4;
    u32 word = mask + (u32)(index >> 5) * 4;
    *tboxView<be<u32>>(word) = (u32)*tboxView<be<u32>>(word) | (1U << ((u32)index & 31));
}
static void updateInvisibleMaterial(u32 material, u32 name, f32 value, bool scale) {
    u32 shader = tboxView<TboxMaterial>(material)->mShader;
    u32 names = relativePtr(shader, 0x38);
    s32 index = tboxFindShaderParameter(names, name);
    if (index < 0)
        return;
    shader = tboxView<TboxMaterial>(material)->mShader;
    u32 table = relativePtr(shader, 0x34), desc = table + (u32)index * 20;
    markShaderParameter(material, desc, index);
    shader = tboxView<TboxMaterial>(material)->mShader;
    table = relativePtr(shader, 0x34);
    u16 parent = tboxView<TboxShaderParameter>(desc)->mParent;
    markShaderParameter(material, table + parent * 20, parent);
    u32 parameters = tboxView<TboxMaterial>(material)->mParameters +
                     tboxView<TboxShaderParameter>(desc)->mOffset;
    if (scale) {
        tboxView<TboxShaderValues>(parameters)->mScaleY = value;
        tboxView<TboxShaderValues>(parameters)->mScale = value;
    } else
        tboxView<TboxShaderValues>(parameters)->mScroll = value;
}
// HD: retain a separate play accessor before each draw-buffer load.
static void drawList(u32 source) {
    u32 p = play();
    l_drawBuffers.mOpa = (u32)*tboxView<be<u32>>(p + source);
    p = play();
    l_drawBuffers.mXlu = (u32)*tboxView<be<u32>>(p + source + sizeof(be<u32>));
}

/* 024B4368 */
s32 daTbox_c::commonShapeSet() {
    u32 info = getModelInfo();
    u32 data = objectRes(gabi::guest_addr("Dalways"), tboxView<modelInfo>(info)->modelId);
    if (!data)
        tboxAssert(gabi::guest_addr("d_a_tbox.cpp"), 0xA4, gabi::guest_addr("modelData != (0)"));
    u32 anim = objectRes(gabi::guest_addr("Dalways"), tboxView<modelInfo>(info)->openBckId);
    if (!mOpenAnm.init(tboxView<J3DModelData>(data), tboxView<J3DAnmTransform>(anim), 1, 0, 1.0f, 0,
                       -1, 0))
        return cPhs_ERROR_e;
    u32 flags = 0x11000022;
    if (tboxView<modelInfo>(info)->btkId > 0) {
        u32 p = tboxConstructBtk(0);
        mpAppearTexAnm = (p ? tboxView<mDoExt_btkAnm>(p) : nullptr);
        if (!p)
            return cPhs_ERROR_e;
        anim = objectRes(gabi::guest_addr("Dalways"), tboxView<modelInfo>(info)->btkId);
        if (!tboxView<mDoExt_btkAnm>(addr((void*)mpAppearTexAnm))
                 ->init(tboxView<J3DModelData>(data), tboxView<J3DAnmTextureSRTKey>(anim), 1, 2,
                        1.0f, 0, -1, 0, 0))
            return cPhs_ERROR_e;
        flags = 0x11001222;
    }
    if (tboxView<modelInfo>(info)->brkId > 0) {
        u32 p = tboxConstructBrk(0);
        mpAppearRegAnm = (p ? tboxView<mDoExt_brkAnm>(p) : nullptr);
        if (!p)
            return cPhs_ERROR_e;
        anim = objectRes(gabi::guest_addr("Dalways"), tboxView<modelInfo>(info)->brkId);
        if (!tboxView<mDoExt_brkAnm>(addr((void*)mpAppearRegAnm))
                 ->init(tboxView<J3DModelData>(data), tboxView<J3DAnmTevRegKey>(anim), 1, 0, 1.0f,
                        0, -1, 0, 0))
            return cPhs_ERROR_e;
        flags |= 0x0F020000;
    }
    u32 model = addr(mDoExt_J3DModel__create(tboxView<J3DModelData>(data), 0x80000, flags));
    mpChestMdl = (model ? tboxView<J3DModel>(model) : nullptr);
    if (!model)
        return cPhs_ERROR_e;
    if (getFuncType() == FUNC_TYPE_TACT) {
        data = objectRes(gabi::guest_addr("Dalways"), 23);
        if (!data)
            tboxAssert(gabi::guest_addr("d_a_tbox.cpp"), 0xE9, gabi::guest_addr("modelData"));
        u32 p = addr(mDoExt_J3DModel__create(tboxView<J3DModelData>(data), 0x80000, 0x11000000));
        mpTactPlatformMdl = (p ? tboxView<J3DModel>(p) : nullptr);
        if (!p)
            return cPhs_ERROR_e;
        anim = objectRes(gabi::guest_addr("Dalways"), 31);
        if (!mTactPlatformBrk.init(tboxView<J3DModelData>(data), tboxView<J3DAnmTevRegKey>(anim), 1,
                                   0, 1.0f, 0, -1, 0, 0))
            return cPhs_ERROR_e;
        model = addr((void*)mpChestMdl);
    }
    f32 x = scale.x, y = scale.y, z = scale.z;
    tboxView<J3DModel>(model)->mBaseScale.x = x;
    tboxView<J3DModel>(model)->mBaseScale.y = y;
    tboxView<J3DModel>(model)->mBaseScale.z = z;
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)current.angle.y);
    copyMtx(0x1048D0CC, addr((void*)mpChestMdl) + 0xC8);
    if (getFuncType() == FUNC_TYPE_TACT) {
        mDoMtx_stack_c::transM(0.0f, 1.0f, 0.0f);
        copyMtx(0x1048D0CC, addr((void*)mpTactPlatformMdl) + 0xC8);
    }
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    return cPhs_COMPLEATE_e;
}

/* 024B4B20 */
s32 daTbox_c::effectShapeSet() {
    u32 data = objectRes(gabi::guest_addr("Dalways"), 22);
    if (!data)
        tboxAssert(gabi::guest_addr("d_a_tbox.cpp"), 0x11E,
                   gabi::guest_addr("flashModelData != (0)"));
    u32 model = addr(mDoExt_J3DModel__create(tboxView<J3DModelData>(data), 0x80000, 0x1000200));
    mpFlashMdl = (model ? tboxView<J3DModel>(model) : nullptr);
    if (!model)
        return cPhs_ERROR_e;
    u32 anim = objectRes(gabi::guest_addr("Dalways"), 11);
    if (!mFlashAnm.init(tboxView<J3DModelData>(data), tboxView<J3DAnmTransform>(anim), 1, 0, 1.0f,
                        0, -1, 0))
        return cPhs_ERROR_e;
    anim = objectRes(gabi::guest_addr("Dalways"), 37);
    if (!mFlashTexAnm.init(tboxView<J3DModelData>(data), tboxView<J3DAnmTextureSRTKey>(anim), 1, 0,
                           1.0f, 0, -1, 0, 0))
        return cPhs_ERROR_e;
    anim = objectRes(gabi::guest_addr("Dalways"), 30);
    return mFlashRegAnm.init(tboxView<J3DModelData>(data), tboxView<J3DAnmTevRegKey>(anim), 1, 0,
                             1.0f, 0, -1, 0, 0)
               ? 4
               : 5;
}

/* 024B4CDC */
s32 daTbox_c::bgCheckSet() {
    u32 info = getModelInfo();
    u32 data = objectRes(gabi::guest_addr("Dalways"), tboxView<modelInfo>(info)->closedColId);
    if (!data)
        tboxAssert(gabi::guest_addr("d_a_tbox.cpp"), 0x1AA, gabi::guest_addr("bgd != (0)"));
    u32 w = tboxConstructBgW(0);
    mpBgWClosed = (w ? tboxView<dBgW>(w) : nullptr);
    if (!w || tboxView<dBgW>(w)->Set(tboxView<cBgD_t>(data), 1, &mMtx))
        return cPhs_ERROR_e;
    data = objectRes(gabi::guest_addr("Dalways"), tboxView<modelInfo>(info)->openColId);
    if (!data)
        tboxAssert(gabi::guest_addr("d_a_tbox.cpp"), 0x1BB, gabi::guest_addr("bgd != (0)"));
    w = tboxConstructBgW(0);
    mpBgWOpen = (w ? tboxView<dBgW>(w) : nullptr);
    if (!w || tboxView<dBgW>(w)->Set(tboxView<cBgD_t>(data), 1, &mMtx))
        return cPhs_ERROR_e;
    if (getFuncType() == FUNC_TYPE_SWITCH_VISIBLE) {
        data = objectRes(gabi::guest_addr("Dalways"), 46);
        if (!data)
            tboxAssert(gabi::guest_addr("d_a_tbox.cpp"), 0x1CE, gabi::guest_addr("bgd != (0)"));
        w = tboxConstructBgW(0);
        mpBgWVines = (w ? tboxView<dBgW>(w) : nullptr);
        if (!w || tboxView<dBgW>(w)->Set(tboxView<cBgD_t>(data), 1, &mMtx))
            return cPhs_ERROR_e;
    }
    tboxView<dBgW>(addr((void*)mpBgWClosed))->Move();
    tboxView<dBgW>(addr((void*)mpBgWOpen))->Move();
    w = addr((void*)mpBgWVines);
    if (w)
        tboxView<dBgW>(w)->Move();
    mpBgWCurrent = nullptr;
    return cPhs_COMPLEATE_e;
}

/* 024B5CBC */
void daTbox_c::searchRoomNo() {
    s32 room = mRoomNo;
    u16 flags = mFlags;
    if (room == -1) {
        room = (s16)home.angle.x & 63;
        mRoomNo = room;
    }
    if (flags & (daTboxFlg_UNK_02))
        clrDzb();
    else if (room != -1 && !(void*)mpBgWCurrent)
        setDzb();
}

/* 024B5A64 */
void daTbox_c::lightReady() {
    f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
    mPLight.mPos.x = x;
    mPLight.mPos.z = z;
    mEfLight.mColor.b = 100;
    mAllColRatio = 0;
    mPLight.mPos.y = y + 55;
    mPLight.mFluctuation = 0;
    mEfLight.mColor.g = 255;
    mPLight.mColor.g = 255;
    mEfLight.mPos.y = y + 50;
    mEfLight.mColor.r = 255;
    mPLight.mColor.b = 255;
    mPLight.mColor.r = 255;
    mPLight.mPower = 0;
    mEfLight.mPos.z = z;
    mEfLight.mPower = 0;
    mEfLight.mFluctuation = 0;
    mEfLight.mPos.x = x;
}

/* 024B481C */
BOOL daTbox_c::checkEnv() {
    s32 n = getShapeType();
    return l_modelInfo.entries[n].brkId > 0;
}

/* 024B4AB4 */
BOOL daTbox_c::checkOpen() {
    s32 type = getFuncType();
    u32 number = (u32)mParameters >> 7 & 31;
    if (type == FUNC_TYPE_EXTRA_SAVE_INFO || type == FUNC_TYPE_EXTRA_SAVE_INFO_SPAWN)
        return tboxIsStageBox(1, number);
    return tboxIsBox(dComIfGs_ea() + 0x798, number);
}

/* 024B432C */
u32 daTbox_c::getModelInfo() { return addr(&l_modelInfo.entries[getShapeType()]); }

/* 024B5B50 */
void daTbox_c::clrDzb() {
    if ((void*)mpBgWCurrent) {
        u32 p = play();
        tboxView<dBgS>(p + 0x12A0)->Release(tboxView<dBgW>(addr((void*)mpBgWCurrent)));
        mpBgWCurrent = nullptr;
        mColCyl.mObjCo.mSPrm = (u32)mColCyl.mObjCo.mSPrm & ~1U;
    }
}

/* 024B5BA8 */
void daTbox_c::setDzb() {
    clrDzb();
    if (checkOpen())
        mpBgWCurrent = (dBgW*)mpBgWOpen;
    else if (getFuncType() == FUNC_TYPE_SWITCH_VISIBLE &&
             !tboxView<dSv_info_c>(save())->isSwitch((u32)mParameters >> 12 & 255, (s32)mRoomNo))
        mpBgWCurrent = (dBgW*)mpBgWVines;
    else
        mpBgWCurrent = (dBgW*)mpBgWClosed;
    u32 p = play();
    s32 result =
        tboxView<dBgS>(p + 0x12A0)->Regist(tboxView<dBgW>(addr((void*)mpBgWCurrent)), this);
    if (result)
        tboxAssert(gabi::guest_addr("d_a_tbox.cpp"), 0x249, gabi::guest_addr("!rt"));
    u32 w = addr((void*)mpBgWCurrent);
    *tboxView<be<u16>>(w + 0xB8) = (u16)mRoomNo;
    mColCyl.mObjCo.mSPrm = (u32)mColCyl.mObjCo.mSPrm | 1;
}

/* 024B6C10 */
void daTbox_c::surfaceProc() {
    if (!(void*)mpBgWCurrent || !((u16)mFlags & (daTboxFlg_APPEARING_e)))
        return;
    f32 offset = mAppearingYOffset;
    if (offset < -1) {
        offset += 1;
        f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
        mAppearingYOffset = offset;
        PSMTXTrans(mDoMtx_stack_c::get(), x, y + offset, z);
    } else {
        mFlags = (u16)mFlags & (u16) ~(daTboxFlg_APPEARING_e);
        f32 y = current.pos.y;
        f32 x = current.pos.x, z = current.pos.z;
        mAppearingYOffset = 0;
        PSMTXTrans(mDoMtx_stack_c::get(), x, y + 0.0f, z);
    }
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)current.angle.y);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    tboxView<dBgW>(addr((void*)mpBgWCurrent))->Move();
}

/* 024B4F34 */
BOOL daTbox_c::checkRoomDisp(s32 room) {
    play();
    u32 r = addr(&l_roomControl.mStatus[0]) + (u32)room * sizeof(TboxRoomStatus);
    if (tboxView<TboxRoomStatus>(r)->mFlags & 8)
        return FALSE;
    play();
    return (tboxView<TboxRoomStatus>(r)->mFlags & 16) != 0;
}

/* 024B4314 */
s32 daTbox_c::getShapeType() {
    s32 n = (u32)mParameters >> 20 & 15;
    return n < 4 ? n : 0;
}

/* 024B435C */
s32 daTbox_c::getFuncType() { return (u32)mParameters & 127; }

/* 024B59CC */
BOOL daTbox_c::checkNormal() {
    s32 type = getFuncType();
    if (type == FUNC_TYPE_NORMAL || type == FUNC_TYPE_SWITCH_VISIBLE || type == FUNC_TYPE_GRAVITY ||
        type == FUNC_TYPE_EXTRA_SAVE_INFO)
        return TRUE;
    s32 room = mRoomNo;
    if (room == -1 || room == 63)
        return FALSE;
    s32 sw = (u32)mParameters >> 12 & 255;
    if (sw >= 192)
        return FALSE;
    return tboxView<dSv_info_c>(save())->isSwitch(sw, room) != 0;
}

/* 024B4F30 */
static BOOL CheckCreateHeap(daTbox_c* self) { return self->CreateHeap(); }

/* 024B4E90 */
BOOL daTbox_c::CreateHeap() {
    if (commonShapeSet() != cPhs_COMPLEATE_e)
        return FALSE;
    if (checkEnv() && tboxEnvShapeSet(this) != cPhs_COMPLEATE_e)
        return FALSE;
    if (!checkOpen() && effectShapeSet() != cPhs_COMPLEATE_e)
        return FALSE;
    return bgCheckSet() == cPhs_COMPLEATE_e;
}

/* 024B5CFC */
void daTbox_c::CreateInit() {
    s32 type = getFuncType();
    u32 a = addr(this), model = addr((void*)mpChestMdl);
    mSmokeCB.mWindOff = 1;
    mFlags = 0;
    mOpenAnm.mFrameCtrl.mRate = 0;
    tboxSetModelInvisible(model, 0);
    if (checkOpen()) {
        f32 end = mOpenAnm.mFrameCtrl.mEnd;
        setAction(ACTION_WAIT);
        mOpenAnm.mFrameCtrl.mFrame = end;
        if (checkEnv()) {
            u32 anim = addr((void*)mpAppearRegAnm);
            mInvisibleScrollVal = 2;
            tboxView<mDoExt_baseAnm>(anim)->mFrameCtrl.mFrame =
                (f32)tboxView<mDoExt_baseAnm>(anim)->mFrameCtrl.mEnd;
        }
    } else if (!checkEnv())
        setAction(ACTION_OPEN_WAIT);
    else if (checkNormal()) {
        BOOL vines =
            type == FUNC_TYPE_SWITCH_VISIBLE &&
            !tboxView<dSv_info_c>(save())->isSwitch((u32)mParameters >> 12 & 255, (s32)mRoomNo);
        mInvisibleScrollVal = 2;
        u32 anim = addr((void*)mpAppearRegAnm);
        setAction(vines ? ACTION_SWITCH_WAIT2 : ACTION_OPEN_WAIT);
        tboxView<mDoExt_baseAnm>(anim)->mFrameCtrl.mFrame =
            (f32)tboxView<mDoExt_baseAnm>(anim)->mFrameCtrl.mEnd;
    } else {
        mFlags = (u16)mFlags | (daTboxFlg_UNK_04);
        tboxSetModelInvisible(addr((void*)mpChestMdl), 1);
        switch (type) {
        case FUNC_TYPE_ENEMIES:
            setAction(ACTION_GENOCIDE);
            mGenocideDelayTimer = 65;
            mFlags = (u16)mFlags | (daTboxFlg_UNK_01 | daTboxFlg_UNK_02);
            mAppearTimer = 120;
            break;
        case FUNC_TYPE_SWITCH:
        case FUNC_TYPE_EXTRA_SAVE_INFO_SPAWN:
            mFlags = (u16)mFlags | (daTboxFlg_UNK_01 | daTboxFlg_UNK_02);
            mAppearTimer = 120;
            setAction(ACTION_SWITCH_WAIT);
            break;
        case FUNC_TYPE_TACT:
            setAction(ACTION_SWITCH_WAIT);
            mFlags = (u16)mFlags | (daTboxFlg_UNK_01 | daTboxFlg_UNK_02);
            mAppearTimer = l_HIO.m08;
            break;
        case FUNC_TYPE_SWITCH_TRANSPARENT:
            mFlags = (u16)mFlags | (daTboxFlg_UNK_02);
            setAction(ACTION_SWITCH_WAIT);
            mAppearTimer = 90;
            tboxView<mDoExt_brkAnm>(addr((void*)mpAppearRegAnm))->setFrame(30);
            break;
        default:
            tboxAssert(gabi::guest_addr("d_a_tbox.cpp"), 0x340, gabi::guest_addr("0"));
            break;
        }
        mInvisibleScrollVal = -2;
    }
    lightReady();
    mAllColRatio = 1;
    if (l_HIO.mNo < 0) {
        u8 child = tboxCreateHioChild(0x1003FF94, addr(&l_HIO));
        l_HIO.mNo = child;
    }
    shape_angle.z = 0;
    shape_angle.x = 0;
    current.angle.z = 0;
    current.angle.x = 0;
    mColStatus.Init(255, 255, this);
    mColCyl.Set(tboxView<dCcD_SrcCyl>(0x101EA190));
    mColCyl.mStts = tboxView<dCcD_Stts>(a + 0x998);
    setCollision();
    mColCyl.mObjCo.mSPrm = mColCyl.mObjCo.mSPrm & ~1U;
    searchRoomNo();
    if (type == FUNC_TYPE_GRAVITY) {
        mAcchCir.SetWall(30.0f, 0.0f);
        mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
        gravity = -2.5f;
    }
    mTboxNo = (u32)mParameters >> 7 & 31;
}

/* 024B72C8 */
BOOL daTbox_c::boxCheck() {
    u32 p = play(),
        player = addr((fopAc_ac_c*)tboxView<dComIfG_inf_c>(p)->play.mPlayerInfo[0].mpPlayer);
    gabi::Local<cXyz> difference, flat;
    tboxSubtractVector(player + 0x314, difference.get(), &home.pos);
    flat->x = difference->x;
    flat->y = 0;
    flat->z = difference->z;
    if (!(PSVECSquareMag(flat.get()) < 10000))
        return FALSE;
    p = play();
    u32 a = addr((fopAc_ac_c*)tboxView<dComIfG_inf_c>(p)->play.mPlayerInfo[0].mpPlayer);
    if (tboxSeenActorAngle(addr(this), a) >= 0x2000)
        return FALSE;
    return tboxSeenActorAngle(player, addr(this)) < 0x2000;
}

/* 024B6D0C */
void daTbox_c::lightUpProc() {
    f32 p = mPLight.mPower;
    if (p < 130)
        mPLight.mPower = p + 13;
    f32 e = mEfLight.mPower;
    if (e < 120)
        mEfLight.mPower = e + 12;
}

/* 024B6D58 */
void daTbox_c::lightDownProc() {
    f32 p = mPLight.mPower;
    if (p > 5.2f) {
        f32 e = mEfLight.mPower;
        mPLight.mPower = p - 5.2f;
        mEfLight.mPower = e > 4.8f ? e - 4.8f : 0;
    } else {
        f32 e = mEfLight.mPower;
        mPLight.mPower = 0;
        mEfLight.mPower = e > 4.8f ? e - 4.8f : 0;
    }
}

/* 024B6DBC */
void daTbox_c::darkProc() {
    u16 time = mOpenTimer;
    if (time > 150)
        mAllColRatio = 1;
    else if (time > 120)
        mAllColRatio = gabi::fmadds((f32)(time - 120) / 30.0f, 0.6f, 0.4f);
}

/* 024B6E38 */
void daTbox_c::volmProc() {
    u16 time = mOpenTimer;
    if (time == 36)
        tboxView<TboxEmitter>(addr((void*)mSmokeEmitter))->mGlobalPrmColor[3] = 255;
    else if (time >= 181) {
        dKy_plight_cut(reinterpret_cast<LIGHT_INFLUENCE*>(&mPLight));
        dKy_efplight_cut(reinterpret_cast<LIGHT_INFLUENCE*>(&mEfLight));
        u32 e = addr((void*)mSmokeEmitter);
        tboxView<TboxEmitter>(e)->mGlobalPrmColor[3] = 0;
        e = addr((void*)mSmokeEmitter);
        u32 flags = tboxView<TboxEmitter>(e)->mFlags;
        tboxView<TboxEmitter>(e)->mMaxFrame = 0xFFFFFFFF;
        tboxView<TboxEmitter>(e)->mFlags = flags | 1;
        mSmokeEmitter = nullptr;
    } else if (time > 156)
        tboxView<TboxEmitter>(addr((void*)mSmokeEmitter))->mGlobalPrmColor[3] = 1810 - time * 10;
}

/* 024B6EF8 */
void daTbox_c::demoProcOpen() {
    u16 time = mOpenTimer;
    if (time < 1000) {
        time++;
        mOpenTimer = time;
        if (time < 156)
            lightUpProc();
        else
            lightDownProc();
    } else
        lightDownProc();
    if ((u16)mOpenTimer == 36) {
        u16 flags = mFlags;
        mFlashAnm.mFrameCtrl.mRate = 1;
        mAllColRatio = 0.4f;
        mFlashTexAnm.mFrameCtrl.mRate = 1;
        mOpenAnm.mFrameCtrl.mRate = 1;
        mFlashRegAnm.mFrameCtrl.mRate = 1;
        mFlags = flags | (daTboxFlg_UNK_08);
    }
    darkProc();
    if ((void*)mSmokeEmitter)
        volmProc();
}

/* 024B6664 */
void daTbox_c::demoInitAppear_Tact() {
    gabi::Local<csXyz> rot;
    rot->x = current.angle.x;
    rot->y = current.angle.y;
    rot->z = current.angle.z;
    particle(this, 0x82F1, addr(rot.get()));
    particle(this, 0x82F0, addr(rot.get()));
    rot->y = (s16)rot->y + 0x5555;
    particle(this, 0x82F0, addr(rot.get()));
    rot->y = (s16)rot->y + 0x5555;
    particle(this, 0x82F0, addr(rot.get()));
    s32 reverb = dComIfGp_getReverb((s8)current.roomNo);
    tboxStartSound(0x6A15, &eyePos, 0, reverb);
}

/* 024B67E4 */
void daTbox_c::demoInitAppear() {
    s32 reverb = dComIfGp_getReverb((s8)current.roomNo);
    tboxStartSound(0x692F, &eyePos, 0, reverb);
    particle(this, 0x3EB, 0);
    particle(this, 0x3EC, 0);
}

/* 024B68B0 */
void daTbox_c::demoProcAppear_Tact() {
    s32 total = l_HIO.m08, reveal = l_HIO.m06;
    u16 timer = mAppearTimer;
    if ((u32)timer == (u32)(total - reveal)) {
        u16 flags = mFlags;
        u32 anim = addr((void*)mpAppearRegAnm);
        mFlags = flags & (u16) ~(daTboxFlg_UNK_01);
        mInvisibleScrollVal = 2;
        tboxView<mDoExt_baseAnm>(anim)->mFrameCtrl.mFrame =
            (f32)tboxView<mDoExt_baseAnm>(anim)->mFrameCtrl.mEnd;
        tboxView<mDoExt_brkAnm>(addr((void*)mpAppearRegAnm))->play();
        mFlags = (u16)mFlags & (u16) ~(daTboxFlg_UNK_04);
        tboxSetModelInvisible(addr((void*)mpChestMdl), 0);
    }
    mTactPlatformBrk.play();
    timer = mAppearTimer;
    if (timer) {
        timer--;
        mAppearTimer = timer;
        s32 down = l_HIO.m0A;
        total = l_HIO.m08;
        f32 ratio = 0.4f;
        if ((s32)timer > total - down)
            ratio = gabi::fmadds((f32)((s32)timer + down - total), 0.6f / (f32)down, ratio);
        else {
            s32 up = l_HIO.m0C;
            if ((s32)timer < up)
                ratio = gabi::fmadds((f32)(up - timer), 0.6f / (f32)up, ratio);
        }
        tboxSetAllColorRatio(ratio);
    } else {
        tboxSetAllColorRatio(1.0f);
        s32 staff = mStaffId;
        u32 p = play();
        tboxCutEnd(p + 0x52C4, staff);
    }
}

/* 024B6AB8 */
void daTbox_c::demoProcAppear() {
    u16 timer = mAppearTimer;
    if ((u32)timer - 1 < 120) {
        cLib_chaseF(&mInvisibleScrollVal, 2.0f, 0.033333335f);
        timer = mAppearTimer;
    }
    if (timer == 60) {
        tboxView<mDoExt_brkAnm>(addr((void*)mpAppearRegAnm))->setFrame(150);
        timer = mAppearTimer;
    }
    if (timer == 5) {
        u32 e = particle(this, 0x2022, 0, 2, addr(this) + 0x75C, 185);
        if (e) {
            tboxView<TboxEmitter>(e)->mInitialVelDir = 25;
            tboxView<TboxEmitter>(e)->mSpread = 1;
            tboxView<TboxEmitter>(e)->mRate = 100;
        }
        timer = mAppearTimer;
    }
    if (timer == 4) {
        if (!(JPABaseEmitter*)mSmokeCB.mpEmitter) {
            mAppearTimer = timer - 1;
        } else {
            mSmokeCB.end();
            timer = mAppearTimer;
            if (timer)
                mAppearTimer = timer - 1;
        }
    } else if (timer)
        mAppearTimer = timer - 1;
    if (tboxView<mDoExt_brkAnm>(addr((void*)mpAppearRegAnm))->play()) {
        s32 staff = mStaffId;
        u32 p = play();
        tboxCutEnd(p + 0x52C4, staff);
        u16 flags = mFlags;
        mFlags = flags & (u16) ~(daTboxFlg_UNK_04);
        tboxSetModelInvisible(addr((void*)mpChestMdl), 0);
    }
}

/* 024B6F9C */
s32 daTbox_c::demoProc() {
    s32 staff = mStaffId;
    u32 p = play();
    u32 action = tboxGetActionIndex(p + 0x52C4, staff, 0x101D2370, 4, 0, 0);
    staff = mStaffId;
    p = play();
    if (tboxEventIsAdvance(p + 0x52C4, staff)) {
        mHasOpenAnmFinished = 0;
        if (action == 1) {
            OpenInit();
            lightReady();
            mPLight.mPower = 0;
            mEfLight.mPower = 0;
        } else if (action == 2) {
            mFlags = (u16)mFlags | (daTboxFlg_APPEARING_e);
            mAppearingYOffset = -130;
            setDzb();
            s32 type = getFuncType();
            u16 flags = mFlags;
            mFlags =
                flags & (type == FUNC_TYPE_TACT ? (u16) ~(daTboxFlg_UNK_02)
                                                : (u16) ~(daTboxFlg_UNK_01 | daTboxFlg_UNK_02));
            if (type == FUNC_TYPE_TACT)
                demoInitAppear_Tact();
            else
                demoInitAppear();
        } else if (action == 3)
            OpenInit_com();
    }
    if (action == 1 || action == 3) {
        if ((u8)mHasOpenAnmFinished)
            cutEnd(this);
        else if (mOpenAnm.play()) {
            staff = mStaffId;
            mHasOpenAnmFinished = 1;
            p = play();
            tboxCutEnd(p + 0x52C4, staff);
            s32 reverb = dComIfGp_getReverb((s8)current.roomNo);
            tboxStartSound(0x690D, &eyePos, 0, reverb);
        }
    } else if (action == 2) {
        if (getFuncType() == FUNC_TYPE_TACT)
            demoProcAppear_Tact();
        else
            demoProcAppear();
        surfaceProc();
    } else
        cutEnd(this);
    u16 flags = mFlags;
    if (flags & (daTboxFlg_OPENING_e)) {
        demoProcOpen();
        flags = mFlags;
    }
    if (flags & (daTboxFlg_UNK_08))
        tboxSetAllColorRatio((f32)mAllColRatio);
    return 0;
}

/* 024B63A8 */
void daTbox_c::OpenInit_com() {
    mOpenAnm.mFrameCtrl.mRate = 1;
    s32 type = getFuncType();
    if (type == FUNC_TYPE_EXTRA_SAVE_INFO || type == FUNC_TYPE_EXTRA_SAVE_INFO_SPAWN)
        tboxOnStageBox(1, (u32)mParameters >> 7 & 31);
    else
        tboxOnBox(dComIfGs_ea() + 0x798, (u32)mParameters >> 7 & 31);
    u8 sw = static_cast<u8>(home.angle.z);
    if (sw != 255)
        tboxView<dSv_info_c>(save())->onSwitch(sw, (s32)mRoomNo);
    setDzb();
    s32 reverb = dComIfGp_getReverb((s8)current.roomNo);
    tboxStartSound(0x690C, &eyePos, 0, reverb);
    reverb = dComIfGp_getReverb((s8)current.roomNo);
    tboxStartSound(0x690B, &eyePos, 0, reverb);
}

/* 024B6494 */
void daTbox_c::OpenInit() {
    OpenInit_com();
    u16 flags = mFlags;
    mIsFlashPlaying = 1;
    mFlashAnm.mFrameCtrl.mRate = 1;
    mOpenTimer = 0;
    mFlashTexAnm.mFrameCtrl.mRate = 1;
    mFlags = flags | (daTboxFlg_OPENING_e);
    mFlashRegAnm.mFrameCtrl.mRate = 1;
    particle(this, 0x1F1, addr(&current.angle));
    particle(this, 0x1F2, addr(&current.angle));
    particle(this, 0x1F3, addr(&current.angle));
    particle(this, 0x1F4, addr(&current.angle));
    particle(this, 0x1F6, addr(&current.angle));
    u32 e = particle(this, 0x1F5, addr(&current.angle));
    mSmokeEmitter = (e ? tboxView<JPABaseEmitter>(e) : nullptr);
    if (e)
        tboxView<TboxEmitter>(e)->mGlobalPrmColor[3] = 0;
}

/* 024B5AE0 */
void daTbox_c::setCollision() {
    u32 a = addr(this);
    mColCyl.mCyl.SetC(&current.pos);
    mColCyl.mCyl.SetR(40.0f);
    mColCyl.mCyl.SetH(110.0f);
    u32 p = play();
    tboxRegisterCylinder(p + 0x26A4, a + 0x9D4);
}

/* 024B79AC */
BOOL daTbox_c::actionWait() { return TRUE; }

/* 024B7708 */
BOOL daTbox_c::actionDemo() {
    s16 event = eventInfo.mEventId;
    u32 p = play();
    if (!tboxEventEndCheck(p + 0x52C4, event)) {
        demoProc();
        return TRUE;
    }
    setAction(ACTION_WAIT);
    p = play();
    tboxView<dComIfG_inf_c>(p)->play.mEvtCtrl.onEventFlag(8);
    tboxSetAllColorRatio(1.0f);
    mFlags = (u16)mFlags & (u16) ~(daTboxFlg_UNK_08 | daTboxFlg_OPENING_e);
    u32 control = play() + 0x51D0;
    u32 id = tboxGetEventPid(control, 0);
    tboxView<dEvt_control_c>(control)->mPtItem = id;
    if ((void*)mSmokeEmitter) {
        dKy_plight_cut(reinterpret_cast<LIGHT_INFLUENCE*>(&mPLight));
        dKy_efplight_cut(reinterpret_cast<LIGHT_INFLUENCE*>(&mEfLight));
        u32 e = addr((void*)mSmokeEmitter), flags = tboxView<TboxEmitter>(e)->mFlags;
        tboxView<TboxEmitter>(e)->mMaxFrame = 0xFFFFFFFF;
        tboxView<TboxEmitter>(e)->mFlags = flags | 1;
        mSmokeEmitter = nullptr;
    }
    return TRUE;
}

/* 024B7804 */
BOOL daTbox_c::actionDemo2() {
    u32 p = play();
    if (tboxEventEndCheckOld(p + 0x52C4, gabi::guest_addr("DEFAULT_TREASURE_APPEAR"))) {
        setAction(ACTION_OPEN_WAIT);
        p = play();
        tboxView<dComIfG_inf_c>(p)->play.mEvtCtrl.onEventFlag(8);
    } else
        demoProc();
    return TRUE;
}

/* 024B7394 */
BOOL daTbox_c::actionOpenWait() {
    u32 a = addr(this);
    if (eventInfo.mCommand == dEvtCmd_INDOOR_e) {
        u32 p = play();
        tboxView<dComIfG_inf_c>(p)->play.mEvtCtrl.onEventFlag(4);
        u32 item = ((s16)home.angle.z >> 8) & 255;
        u32 id = tboxCreateDemoItem(&current.pos, item, -1, -1, 0, 0);
        if (id != 0xFFFFFFFF) {
            p = play();
            tboxView<dComIfG_inf_c>(p)->play.mEvtCtrl.mPtItem = id;
        }
        if (getShapeType()) {
            tboxStartStream(0x80000009);
            mAllColRatio = 0.4f;
            mFlags = (u16)mFlags | (daTboxFlg_UNK_08);
            tboxSetAllColorRatio(0.4f);
            lightReady();
            mPLight.mPower = 0;
            mEfLight.mPower = 0;
            tboxSetPriorityLight(a + 0x714);
            dKy_efplight_set(reinterpret_cast<LIGHT_INFLUENCE*>(&mEfLight));
        }
        setAction(ACTION_DEMO);
        p = play();
        mStaffId = tboxGetStaffId(p + 0x52C4, gabi::guest_addr("TREASURE"), 0, 0);
        demoProc();
    } else if (boxCheck()) {
        eventInfo.mCondition = eventInfo.mCondition | 4;
        s32 shape = getShapeType();
        tboxSetEventName(a + 0xF8, shape ? gabi::guest_addr("DEFAULT_TREASURE")
                                         : gabi::guest_addr("DEFAULT_TREASURE_A"));
    }
    return TRUE;
}

/* 024B7514 */
BOOL daTbox_c::actionSwOnWait() {
    if (eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        setAction(ACTION_DEMO2);
        u32 p = play();
        mStaffId = tboxGetStaffId(p + 0x52C4, gabi::guest_addr("TREASURE"), 0, 0);
        demoProc();
    } else if (tboxView<dSv_info_c>(save())->isSwitch((u32)mParameters >> 12 & 255, (s32)mRoomNo)) {
        tboxOrderOtherEvent(this, gabi::guest_addr("DEFAULT_TREASURE_APPEAR"), 1, 65535);
        eventInfo.mCondition = eventInfo.mCondition | 2;
    }
    return TRUE;
}

/* 024B6334 */
BOOL daTbox_c::actionSwOnWait2() {
    if (tboxView<dSv_info_c>(save())->isSwitch((u32)mParameters >> 12 & 255, (s32)mRoomNo)) {
        setAction(ACTION_OPEN_WAIT);
        setDzb();
    }
    return TRUE;
}

/* 024B75EC */
BOOL daTbox_c::actionGenocide() {
    if (eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        setAction(ACTION_DEMO2);
        u32 p = play();
        mStaffId = tboxGetStaffId(p + 0x52C4, gabi::guest_addr("TREASURE"), 0, 0);
        demoProc();
        return TRUE;
    }
    s32 room = mRoomNo;
    if (room == -1 || room != l_roomControl.mStayNo)
        return TRUE;
    if (tboxSearchRoomEnemy((s8)room))
        return TRUE;
    u8 delay = mGenocideDelayTimer;
    if (delay) {
        mGenocideDelayTimer = delay - 1;
        return TRUE;
    }
    tboxOrderOtherEvent(this, gabi::guest_addr("DEFAULT_TREASURE_APPEAR"), 1, 65535);
    u16 flags = eventInfo.mCondition;
    u32 prm = mParameters;
    eventInfo.mCondition = flags | 2;
    tboxView<dSv_info_c>(save())->onSwitch(prm >> 12 & 255, (s32)mRoomNo);
    return TRUE;
}

/* 024B54F8 */
BOOL daTbox_c::execute() {
    s32 room = mRoomNo;
    if (room == -1 || checkRoomDisp(room) != 1)
        return TRUE;
    (this->*mActionFunc)();
    u32 anim = addr((void*)mpAppearTexAnm);
    if (anim)
        tboxView<mDoExt_baseAnm>(anim)->play();
    if ((u8)mIsFlashPlaying) {
        mFlashAnm.play();
        mFlashTexAnm.play();
        if (mFlashRegAnm.play())
            mIsFlashPlaying = 0;
        u32 model = addr((void*)mpFlashMdl);
        tboxView<J3DModel>(model)->mBaseScale.x = 1.42857146f;
        tboxView<J3DModel>(model)->mBaseScale.z = 1;
        tboxView<J3DModel>(model)->mBaseScale.y = 1;
        f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
        PSMTXTrans(mDoMtx_stack_c::get(), x, y + 50, z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)((s16)current.angle.y + 32767));
        copyMtx(0x1048D0CC, addr((void*)mpFlashMdl) + 0xC8);
    }
    if (getFuncType() == FUNC_TYPE_GRAVITY) {
        fopAcM_posMoveF(this, nullptr);
        u32 p = play();
        mObjAcch.CrrPos(tboxView<dBgS>(p + 0x12A0));
        tboxView<TboxWordVector>(addr(&attention_info.position))->x =
            tboxView<TboxWordVector>(addr(&current.pos))->x;
        f32 y = current.pos.y, z = current.pos.z;
        tboxView<TboxWordVector>(addr(&attention_info.position))->z =
            tboxView<TboxWordVector>(addr(&current.pos))->z;
        u32 ybits = tboxView<TboxWordVector>(addr(&current.pos))->y;
        f32 x = current.pos.x;
        tboxView<TboxWordVector>(addr(&attention_info.position))->y = ybits;
        PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)home.angle.y);
        copyMtx(0x1048D0CC, addr((void*)mpChestMdl) + 0xC8);
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
        u32 w = addr((void*)mpBgWCurrent);
        if (w)
            tboxView<dBgW>(w)->Move();
    }
    return TRUE;
}

/* 024B4FA8 */
static BOOL daTbox_Draw(daTbox_c* self) {
    s32 room = self->mRoomNo;
    if (room != -1 && !self->checkRoomDisp(room))
        return TRUE;
    u16 flags = self->mFlags;
    u8 number;
    if ((flags & 1) || (self->checkEnv() && (flags & 4)))
        number = self->mTboxNo;
    else
        number = 255;
    if (!self->checkOpen()) {
        s8 mapRoom = (s8)self->mRoomNo;
        u8 gba = self->gbaName;
        f32 x = self->current.pos.x, y = self->current.pos.y, z = self->current.pos.z;
        tboxDrawMapPoint(5, mapRoom, -32768, number, gba, 0, x, y, z);
    }
    self->tevStr.mRoomNo = (s32)self->mRoomNo;
    u32 env = tboxGetEnvLight();
    tboxSettingTevStruct(env, 0, &self->current.pos, &self->tevStr);
    if (self->getFuncType() == daTbox_c::FUNC_TYPE_TACT) {
        u32 data = addr(tboxView<J3DModel>(addr((void*)self->mpTactPlatformMdl))->getModelData());
        env = tboxGetEnvLight();
        tboxSetLightTevColor(env, (void*)self->mpTactPlatformMdl, &self->tevStr);
        self->mTactPlatformBrk.entry(tboxView<J3DModelData>(data),
                                     (f32)self->mTactPlatformBrk.mFrameCtrl.mFrame);
        mDoExt_modelUpdateDL(tboxView<J3DModel>(addr((void*)self->mpTactPlatformMdl)), 0);
    }
    if ((u16)self->mFlags & (daTbox_c::daTboxFlg_UNK_01))
        return TRUE;
    env = tboxGetEnvLight();
    tboxSetLightTevColor(env, (void*)self->mpChestMdl, &self->tevStr);
    u32 data = addr(tboxView<J3DModel>(addr((void*)self->mpChestMdl))->getModelData());
    self->mOpenAnm.entry(tboxView<J3DModelData>(data), (f32)self->mOpenAnm.mFrameCtrl.mFrame);
    u32 anim = addr((void*)self->mpAppearTexAnm);
    if (anim)
        tboxView<mDoExt_btkAnm>(anim)->entry(tboxView<J3DModelData>(data),
                                             (f32)tboxView<mDoExt_btkAnm>(anim)->mFrameCtrl.mFrame);
    anim = addr((void*)self->mpAppearRegAnm);
    if (anim)
        tboxView<mDoExt_brkAnm>(anim)->entry(tboxView<J3DModelData>(data),
                                             (f32)tboxView<mDoExt_brkAnm>(anim)->mFrameCtrl.mFrame);
    bool invisible = self->checkEnv() && ((u16)self->mFlags & (daTbox_c::daTboxFlg_UNK_04));
    if (invisible) {
        f32 scroll = (f32)self->mInvisibleScrollVal - (-2.0f), whole = (s8)gabi::ftoi(scroll),
            scale = gabi::fmadds(scroll - whole, 0.5f, 0.5f);
        u8 i = 0;
        u32 md = tboxGetMaterialHeader(data);
        while (i < tboxView<TboxMaterialHeader>(md)->mMaterialCount) {
            u32 model = addr((void*)self->mpChestMdl),
                materials = tboxView<TboxModelRenderView>(model)->mMaterials,
                material = materials + i * 60;
            updateInvisibleMaterial(material, gabi::guest_addr("indirect_mtx0a"), scale, true);
            updateInvisibleMaterial(material, gabi::guest_addr("indirect_mtx0b"), whole, false);
            i++;
            md = tboxGetMaterialHeader(data);
        }
        if ((u16)self->mFlags & (daTbox_c::daTboxFlg_UNK_04)) {
            drawList(0x5D8C);
            mDoExt_modelUpdateDL(tboxView<J3DModel>(addr((void*)self->mpChestMdl)), 0);
            drawList(0x5D78);
        } else
            mDoExt_modelUpdateDL(tboxView<J3DModel>(addr((void*)self->mpChestMdl)), 0);
    } else
        mDoExt_modelUpdateDL(tboxView<J3DModel>(addr((void*)self->mpChestMdl)), 0);
    if ((u8)self->mIsFlashPlaying && (u16)self->mOpenTimer >= 36) {
        data = addr(tboxView<J3DModel>(addr((void*)self->mpFlashMdl))->getModelData());
        self->mFlashAnm.entry(tboxView<J3DModelData>(data), (f32)self->mFlashAnm.mFrameCtrl.mFrame);
        self->mFlashRegAnm.entry(tboxView<J3DModelData>(data),
                                 (f32)self->mFlashRegAnm.mFrameCtrl.mFrame);
        self->mFlashTexAnm.entry(tboxView<J3DModelData>(data),
                                 (f32)self->mFlashTexAnm.mFrameCtrl.mFrame);
        drawList(0x5D84);
        mDoExt_modelUpdateDL(tboxView<J3DModel>(addr((void*)self->mpFlashMdl)), 0);
        drawList(0x5D78);
    }
    return TRUE;
}

/* 024B577C */
static BOOL daTbox_Execute(daTbox_c* self) { return self->execute(); }

/* 024B79A4 */
static BOOL daTbox_IsDelete(daTbox_c* self) { return TRUE; }

/* 024B5780 */
static BOOL daTbox_Delete(daTbox_c* self) {
    if ((void*)self->mpBgWCurrent) {
        u32 p = play();
        tboxView<dBgS>(p + 0x12A0)->Release(tboxView<dBgW>(addr((void*)self->mpBgWCurrent)));
    }
    self->mSmokeCB.remove();
    tboxDeleteResource(&self->mPhase, gabi::guest_addr("Dalways"));
    s8 h = l_HIO.mNo;
    if (h >= 0) {
        tboxDeleteHioChild((s32)h);
        l_HIO.mNo = -1;
    }
    return TRUE;
}

/* 024B6268 */
static s32 daTbox_Create(daTbox_c* self) {
    u32 condition = self->actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (self) {
            daTbox_ct(self);
            condition = self->actor_condition;
        }
        self->actor_condition = condition | fopAcCnd_INIT_e;
    }
    s32 phase = tboxLoadResource(&self->mPhase, gabi::guest_addr("Dalways"));
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    self->mRoomNo = (s16)self->home.angle.x & 63;
    if (!tboxEntrySolidHeap(self, gabi::guest_fn((const void*)CheckCreateHeap), 0))
        return cPhs_ERROR_e;
    self->CreateInit();
    self->attention_info.flags = 64;
    return phase;
}

// HD: compiler-generated construction, destruction and header statics.

/* 024B7888 */
static u32 daTbox_HIO_ct(u32 self) {
    if (!self) {
        self = tboxAllocate(16);
        if (!self)
            return 0;
    }
    tboxView<daTbox_HIO_c>(self)->m08 = 180;
    tboxView<daTbox_HIO_c>(self)->mNo = -1;
    tboxView<daTbox_HIO_c>(self)->__vtbl = 0x1003FE54;
    tboxView<daTbox_HIO_c>(self)->m0C = 30;
    tboxView<daTbox_HIO_c>(self)->m06 = 130;
    tboxView<daTbox_HIO_c>(self)->m0A = 48;
    return self;
}

/* 024B7990 */
static void daTbox_HIO_dt(u32 self, u32 flags) {
    if (self && (flags & 1))
        operator_delete(tboxView<void>(self));
}

/* 024B7A68 */
static void emptyVirtual(u32 self) {}

/* 024B78F0 */
static void sinit_d_a_tbox() {
    l_headerZeros.mWords[2] = 0;
    l_headerZeros.mWords[0] = 0;
    l_headerZeros.mWords[3] = 0;
    l_headerZeros.mWords[1] = 0;
    tboxRegisterGlobal(0x101D2380);
    l_headerStatics.mAngleMin = (f32)l_pi.mMin;
    l_headerStatics.mAngleMax = (f32)l_pi.mMax;
    tboxInitMathHeader(addr(&l_headerStatics.mMathHeader));
    tboxRegisterGlobal(0x101D238C);
    tboxInitMatrixHeader(addr(&l_headerStatics.mMatrixHeader));
    tboxRegisterGlobal(0x101D2398);
    daTbox_HIO_ct(addr(&l_HIO));
}

/* 024B79B4 */
static void daTbox_dtor(daTbox_c* self, u32 flags) {
    if (!self)
        return;
    u32 a = addr(self);
    dCcD_Cyl_dt(&self->mColCyl, 2);
    dCcD_Stts_dt(&self->mColStatus, 2);
    tboxDestroyCircle(a + 0x96C, 2);
    self->mObjAcch.dBgS_GrpPassChk::__vtbl = 0x1003FE24;
    self->mObjAcch.dBgS_PolyPassChk::__vtbl = 0x1003FE34;
    tboxDestroyAcch(a + 0x794, 0);
    tboxDestroyMtxCalc(a + 0x474, 0);
    tboxDestroyMtxCalc(a + 0x3CC, 0);
    tboxDestroyActor(self, 0);
    if (flags & 1)
        operator_delete((void*)self);
}

/* 024B5808 */
static daTbox_c* daTbox_ct(daTbox_c* self) {
    // HD: null-this construction allocates; offset views must not replace this null conversion.
    if (!self) {
        self = gabi::at<daTbox_c>(tboxAllocate(0xB08));
        if (!self)
            return nullptr;
    }
    u32 a = addr(self);
    tboxConstructActor(self);
    self->__vtbl = 0x1003FE44;
    tboxInitFrameCtrl(a + 0x3BC, 0);
    reinterpret_cast<TboxBckView*>(&self->mOpenAnm)->mMtxCalcVtable = 0x1016E54C;
    tboxConstructAnmCalc(a + 0x3D0);
    reinterpret_cast<TboxBckView*>(&self->mOpenAnm)->mAnmData = 0;
    reinterpret_cast<TboxBckView*>(&self->mOpenAnm)->mAnmTarget = 0;
    reinterpret_cast<TboxBckView*>(&self->mOpenAnm)->mAnmIndex = 0;
    reinterpret_cast<TboxBckView*>(&self->mOpenAnm)->mBlendFlags = 0;
    reinterpret_cast<TboxBckView*>(&self->mOpenAnm)->mMtxCalcVtable = 0x1003FDEC;
    reinterpret_cast<TboxBckView*>(&self->mOpenAnm)->mAnmFlags = 0;
    reinterpret_cast<TboxBckView*>(&self->mOpenAnm)->mBlendVtable = 0x1016D820;
    tboxInitFrameCtrl(a + 0x464, 0);
    reinterpret_cast<TboxBckView*>(&self->mFlashAnm)->mMtxCalcVtable = 0x1016E54C;
    tboxConstructAnmCalc(a + 0x478);
    reinterpret_cast<TboxBckView*>(&self->mFlashAnm)->mAnmData = 0;
    reinterpret_cast<TboxBckView*>(&self->mFlashAnm)->mAnmIndex = 0;
    reinterpret_cast<TboxBckView*>(&self->mFlashAnm)->mBlendFlags = 0;
    reinterpret_cast<TboxBckView*>(&self->mFlashAnm)->mBlendVtable = 0x1016D820;
    reinterpret_cast<TboxBckView*>(&self->mFlashAnm)->mAnmTarget = 0;
    reinterpret_cast<TboxBckView*>(&self->mFlashAnm)->mMtxCalcVtable = 0x1003FDEC;
    reinterpret_cast<TboxBckView*>(&self->mFlashAnm)->mAnmFlags = 0;
    tboxConstructBtk(a + 0x4F0);
    tboxConstructBrk(a + 0x564);
    tboxConstructBrk(a + 0x5E0);
    tboxConstructBrk(a + 0x65C);
    self->mPLight.mHD20 = 1;
    self->mEfLight.mHD20 = 1;
    tboxConstructSmokeCallback(a + 0x75C, 1);
    tboxConstructAcch(a + 0x794);
    self->mObjAcch.cBgS_Chk::__vtbl = 0x1003FE14;
    self->mObjAcch.dBgS_PolyPassChk::__vtbl = 0x1003FE34;
    self->mObjAcch.mbObjThrough = 1;
    self->mObjAcch.dBgS_GrpPassChk::__vtbl = 0x1003FE24;
    dBgS_AcchCir_ct(&self->mAcchCir);
    dCcD_Stts_ct(&self->mColStatus);
    dCcD_Cyl_ct(&self->mColCyl, 0x1003FDDC);
    return self;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): Treasure chest
data l_headerStatics 1046E6D0
data l_headerZeros 1046E6EC
data l_pi 100400A8
data l_roomControl 1047E6C8
data l_resourceControl 101F4F28
data l_drawBuffers 104B4634
str 1003FE64 "indirect_mtx0a"
str 1003FE74 "indirect_mtx0b"
str 1003FE8C "Dalways" in 024B4368
str 1003FE94 "modelData"
str 1003FEA0 "d_a_tbox.cpp" in 024B4368
str 1003FEB0 "modelData != (0)"
str 1003FEF0 "Dalways" in 024B4B20
str 1003FEF8 "flashModelData != (0)"
str 1003FF10 "d_a_tbox.cpp" in 024B4B20
str 1003FF20 "Dalways" in 024B4CDC
str 1003FF28 "d_a_tbox.cpp" in 024B4CDC
str 1003FF38 "bgd != (0)"
str 1003FF5C "Dalways" in 024B5780
str 1003FF70 "!rt"
str 1003FF74 "d_a_tbox.cpp" in 024B5BA8
str 1003FF90 "0"
str 1003FF9C "d_a_tbox.cpp" in 024B5CFC
str 1003FFAC "Dalways" in 024B6268
str 10040014 "TREASURE" in 024B7394
str 10040020 "DEFAULT_TREASURE_A"
str 10040034 "DEFAULT_TREASURE"
str 10040048 "DEFAULT_TREASURE_APPEAR" in 024B7514
str 10040060 "TREASURE" in 024B7514
str 1004006C "DEFAULT_TREASURE_APPEAR" in 024B75EC
str 10040084 "TREASURE" in 024B75EC
str 10040090 "DEFAULT_TREASURE_APPEAR" in 024B7804
ext 0200E240 void tboxRegisterCylinder(u32 collision, u32 cylinder)
ext 025204C8 void tboxDeleteResource(request_of_phase_process_class* phase, u32 name)
ext 02520460 s32 tboxLoadResource(request_of_phase_process_class* phase, u32 name)
ext 0201ADE0 void tboxSubtractVector(u32 left, cXyz* result, const cXyz* right)
ext 026066C4 u32 tboxGetResource(u32 control, TboxSafeString* name, s32 index)
ext 025A847C u32 tboxSetParticle(u32 manager, u32 group, u32 id, cXyz* pos, u32 angle, u32 scale, u32 alpha, u32 callback, s32 setup, u32 prmColor, u32 envColor, u32 scale2D)
ext 028F026C void tboxRegisterGlobal(u32 record)
ext 028ED6F8 void tboxInitMathHeader(u32 object)
ext 028EAB2C void tboxInitMatrixHeader(u32 object)
ext 02018034 void tboxDestroyCircle(u32 object, u32 flags)
ext 024EFD9C void tboxDestroyAcch(u32 object, u32 flags)
ext 027F3628 void tboxDestroyMtxCalc(u32 object, u32 flags)
ext 025D50BC void tboxDestroyActor(daTbox_c* actor, u32 flags)
ext 025E7C6C u32 tboxConstructBtk(u32 object)
ext 025E80D0 u32 tboxConstructBrk(u32 object)
ext 024F23F4 u32 tboxConstructBgW(u32 object)
ext 025D4ED0 void tboxConstructActor(daTbox_c* actor)
ext 027F2BC0 void tboxInitFrameCtrl(u32 object, s32 end)
ext 027DA984 void tboxConstructAnmCalc(u32 object)
ext 025A5B18 void tboxConstructSmokeCallback(u32 object, u32 windOff)
ext 024F0474 void tboxConstructAcch(u32 object)
ext 025D7E88 u32 tboxCreateDemoItem(cXyz* pos, u32 item, s32 room, s32 flag, u32 angle, u32 scale)
ext 025D63E8 BOOL tboxEntrySolidHeap(daTbox_c* actor, u32 callback, u32 size)
ext 02560444 void tboxSetAllColorRatio(f32 ratio)
ext 025E1918 void tboxStartStream(u32 id)
ext 025E1A40 void tboxStartSound(u32 id, const cXyz* pos, u32 flags, s32 reverb)
ext 0273AD10 u32 tboxAllocate(u32 size)
ext 0273AA24 void tboxAssert(u32 file, u32 line, u32 expression)
ext 0255A2B8 void tboxSetPriorityLight(u32 light)
ext 025F0A18 void tboxDeleteHioChild(s32 child)
ext 025F0A10 u8 tboxCreateHioChild(u32 name, u32 hio)
ext 025207D8 void tboxOnStageBox(u32 stage, u32 box)
ext 02520864 BOOL tboxIsStageBox(u32 stage, u32 box)
ext 025B8C0C void tboxOnBox(u32 save, u32 box)
ext 025B8C74 BOOL tboxIsBox(u32 save, u32 box)
ext 024B4858 s32 tboxEnvShapeSet(daTbox_c* actor)
ext 02542D88 s32 tboxGetStaffId(u32 manager, u32 name, u32 actor, u32 id)
ext 02543280 void tboxCutEnd(u32 manager, s32 staff)
ext 025440C8 BOOL tboxEventEndCheck(u32 manager, s32 event)
ext 0254457C BOOL tboxEventEndCheckOld(u32 manager, u32 name)
ext 025447C8 BOOL tboxEventIsAdvance(u32 manager, s32 staff)
ext 02542EDC u32 tboxGetActionIndex(u32 manager, s32 staff, u32 actions, u32 count, u32 force, u32 parameter)
ext 0253F124 u32 tboxGetEventPid(u32 control, u32 index)
ext 0253E9B0 void tboxSetEventName(u32 info, u32 name)
ext 025D77DC void tboxOrderOtherEvent(daTbox_c* actor, u32 name, u32 priority, u32 flags)
ext 025D98E8 u32 tboxSearchRoomEnemy(s8 room)
ext 025D68A0 s32 tboxSeenActorAngle(u32 actor, u32 target)
ext 027DF9B0 s32 tboxFindShaderParameter(u32 names, u32 name)
ext 027F58E0 void tboxSetModelInvisible(u32 model, u32 invisible)
ext 027F3F8C u32 tboxGetMaterialHeader(u32 modelData)
ext 02590488 void tboxDrawMapPoint(u32 type, s8 room, s32 angle, u8 number, u8 gbaName, u32 flags, f32 x, f32 y, f32 z)
ext 02555D0C u32 tboxGetEnvLight()
ext 025626A4 void tboxSettingTevStruct(u32 env, s32 type, cXyz* pos, dKy_tevstr_c* tev)
ext 02562F5C void tboxSetLightTevColor(u32 env, void* model, dKy_tevstr_c* tev)
 data l_HIO 1046E6DC
 data l_modelInfo 101D2320
*/
// clang-format on
