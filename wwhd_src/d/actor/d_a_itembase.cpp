/**
 * d_a_itembase.cpp (WWHD)
 * Item - Base Item Class
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_itembase.cpp) to the WWHD layout
 * and code, verified against cking.rpx. "HD:" marks where WWHD's code differs
 * from the GameCube source.
 */
#include "d/actor/d_a_itembase_private.h"
using namespace item_base;
static WordHD itemBaseSafeStringVtable;
static ByteHD itemBaseMonotone;
static WordHD itemBaseOpaList;
static WordHD itemBaseXluList;
static WordHD itemBaseResourceManager;
static WordHD itemBaseBckVtable;
static WordHD itemBaseBckBaseVtable;
static WordHD itemBaseBckHelperVtable;
static WordHD itemBaseSinitState;
static WordHD itemBaseSinitData;
static ItemResourcesHD itemBaseResources;

#define SAFESTRING_VTBL gabi::ea(&itemBaseSafeStringVtable)
#define FILE_NAME "d_a_itembase.cpp"
// HD: preprocessed assertion messages spell NULL as (0).

// HD: the specular map takes the model rather than its model data.
// HD: the bomb animation also updates a callback-driven frame helper.
static void daPy_bombBrk_setFrame0(PlayerBombHD *player) {
  be<f32> *anmFrame = player->anmFrame;
  player->frame = 0.0f;
  *anmFrame = 0.0f;
  FrameHelperHD *helper = player->helper;
  u32 fn = helper->normalize;
  f32 end = helper->end;
  f32 start = helper->start;
  f32 frame = gabi::call_ptr<f32>(fn, (u32)helper->argument, 0.0f, start, end);
  helper->frame = frame;
  itemBaseNormalizeFrame(&player->helper);
}
/* 02183788 */
BOOL daItemBase_c::DeleteBase(const char *resName) {
  dComIfG_resDelete(&mPhs, resName);
  return TRUE;
}

/* 021841BC */
BOOL daItemBase_c::clothCreate() { return TRUE; }

// Inlined resource and animation helpers.
// GHS uses PowerPC shifts: bit five clears the result rather than masking the
// shift.
static inline u32 ppc_slw(u32 x, u32 n) {
  return (n & 0x20) ? 0 : x << (n & 0x1F);
}
static inline u32 ppc_srw(u32 x, u32 n) {
  return (n & 0x20) ? 0 : x >> (n & 0x1F);
}
static void safestring_set(SafeStringHD *slot, u32 string) {
  slot->vtable = view<SafeStringVtableHD>(SAFESTRING_VTBL);
  slot->string = view<const be<u8>>(string);
}
static void safestring_assure(SafeStringHD *slot) {
  gabi::call_ptr(slot->vtable->slots[5], slot);
}
static bool safestring_isEqual(SafeStringHD *a, SafeStringHD *b) {
  itemBaseAssureTermination(gabi::ea(a));
  safestring_assure(a);
  const be<u8> *sa = a->string;
  safestring_assure(b);
  const be<u8> *sb = b->string;
  if (sa == sb)
    return true;
  for (u32 k = 0; k < 0x40001; k++) {
    u8 ca = sa[k];
    if (ca != sb[k])
      return false;
    if (ca == 0)
      return true;
  }
  return false;
}
// HD: the constructor initializes the BFRES frame helper and this TU's vtable.
static void bckAnm_ct(BckAnmHD *b) {
  itemBaseFrameInit(gabi::ea(b), 0);
  b->vtable = gabi::ea(&itemBaseBckBaseVtable);
  itemBaseFrameHelperInit(b->frameHelper);
  b->resource = 0;
  b->helperState = 0;
  b->helperVtable = gabi::ea(&itemBaseBckHelperVtable);
  b->model = 0;
  b->vtable = gabi::ea(&itemBaseBckVtable);
  b->state = 0;
  b->flags = 0;
}
static BOOL btkAnm_init(u32 a, J3DModelData *d, void *key, s32 play) {
  return itemBaseBtkInit(a, d, key, play, (s32)J3DFrameCtrl::EMode_LOOP, 1.0f,
                         (s16)0, (s16)-1, false, (s32)0);
}
static BOOL brkAnm_init(u32 a, J3DModelData *d, void *key, s32 play) {
  return itemBaseBrkInit(a, d, key, play, (s32)J3DFrameCtrl::EMode_LOOP, 1.0f,
                         (s16)0, (s16)-1, false, (s32)0);
}

// HD: Vho textures are replaced through BFRES, and rupees alter material-packet
// flags.
/* 021838D0 */
BOOL daItemBase_c::CreateItemHeap(const char *resName, s16 resIdx, s16 btkAnm1,
                                  s16 btkAnm2, s16 brkAnm1, s16 brkAnm2,
                                  s16 bckAnm, s16 param_8) {
  // GHS uses this exact frame, including untouched texture words and relative
  // addresses.
  auto frame = GuestFrame(sizeof(HeapFrameHD));
  HeapFrameHD *work = view<HeapFrameHD>(frame.sp);

  J3DModelData *modelData =
      (J3DModelData *)dComIfG_getObjectRes(resName, resIdx, SAFESTRING_VTBL);
  if (modelData == nullptr)
    JUT_ASSERT_fail(FILE_NAME, 0x61, "modelData != (0)");

  mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
  if (!mpModel) {
    return FALSE;
  }

  switch (m_itemNo) {
  case dItemNo_ARROW_30_e:
    mpModelArrow[0] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
    if (!mpModelArrow[0]) {
      return FALSE;
    }
  case dItemNo_ARROW_20_e:
    mpModelArrow[1] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
    if (!mpModelArrow[1]) {
      return FALSE;
    }
    break;
  default:
    mpModelArrow[0] = NULL;
    mpModelArrow[1] = NULL;
  }

  if (m_itemNo == dItemNo_VHO_HD_e) {
    ModelDataHD *md = view<ModelDataHD>(J3DModel_getModelData(mpModel));
    TextureTableHD *texTable = md->textureTable;
    if (texTable == 0)
      return FALSE;
    u32 nameTab =
        view<ModelDataHD>(J3DModel_getModelData(mpModel))->textureNames;
    if (nameTab == 0)
      return FALSE;
    TextureEntryHD *entryTmp = &work->texture;
    for (u16 i = 0; i < (u16)texTable->count; i++) {
      u32 name = itemBaseTextureName(nameTab, (u32)i); /* JUTNameTab::getName */
      if (name == 0)
        JUT_ASSERT_fail(FILE_NAME, 0xA0, "name != (0)");
      bool match = false;
      if (name != 0) {
        safestring_set(&work->textureName, gabi::guest_addr("Vho")); /* "Vho" */
        safestring_set(&work->candidate, name);
        match = safestring_isEqual(&work->textureName, &work->candidate);
      }
      if (!match)
        continue;
      safestring_set(&work->resourceName, gabi::ea(resName));
      safestring_set(&work->fileName,
                     gabi::guest_addr("Vho.bfres")); /* "Vho.bfres" */
      u32 file = itemBaseResourceFile((u32)itemBaseResourceManager,
                                      &work->resourceName, &work->fileName, 0);
      u32 tex = itemBaseTexture(
          relativeAddress(&view<ResourceFileHD>(file)->texturesOffset),
          "Vho2"); /* "Vho2" */
      if (tex == 0)
        JUT_ASSERT_fail(FILE_NAME, 0xA7, "res_tex");
      entryTmp->texture = tex;
      entryTmp->height = (u16)view<TextureResourceHD>(tex)->height;
      entryTmp->format = 0;
      entryTmp->width = (u16)view<TextureResourceHD>(tex)->width;
      TextureEntryHD *entry = texTable->entries + i;
      // GHS copies each word in order, including the uninitialized frame bytes.
      for (int k = 0; k < 9; k++)
        entry->words[k] = entryTmp->words[k];
      entry = texTable->entries + i;
      entry->paletteOffset =
          entry->paletteOffset + gabi::ea(entryTmp) - gabi::ea(entry);
      entry = texTable->entries + i;
      entry->imageOffset =
          entry->imageOffset + gabi::ea(entryTmp) - gabi::ea(entry);
      entry = texTable->entries + i;
      entry->texture = entryTmp->texture;
      // HD: the packed 128-bit texture mask uses PowerPC shifts.
      u32 hi = (u32)texTable->oneHigh;
      u32 lo = (u32)texTable->oneLow;
      if (i < 0x40) {
        u32 n = i;
        u32 shi =
            ppc_slw(lo, n + 0x20) | (ppc_slw(hi, n) | ppc_srw(lo, 0x20 - n));
        u32 slo = ppc_slw(lo, n);
        texTable->maskHigh = texTable->maskHigh | shi;
        texTable->maskLow = texTable->maskLow | slo;
      } else {
        u32 n = i - 0x40;
        u32 shi =
            ppc_slw(lo, n + 0x20) | (ppc_slw(hi, n) | ppc_srw(lo, 0x20 - n));
        u32 slo = ppc_slw(lo, n);
        texTable->maskUpperLow = texTable->maskUpperLow | slo;
        texTable->maskUpperHigh = texTable->maskUpperHigh | shi;
      }
    }
  } else if (isRupee(m_itemNo)) {
    ModelDataHD *md = view<ModelDataHD>(J3DModel_getModelData(mpModel));
    safestring_set(&work->materialName,
                   gabi::guest_addr("SC_lupy_outside")); /* "SC_lupy_outside" */
    MaterialTableHD *matTable = view<MaterialTableHD>((u32)md->mpResHeader);
    itemBaseAssureTermination(
        gabi::ea(&work->materialName)); /* cstr(): assureTerminationImpl_ */
    s32 idx = itemBaseMaterialIndex(
        relativeAddress(&matTable->namesOffset),
        gabi::ea((const be<u8> *)
                     work->materialName.string)); /* JUTNameTab::getIndex */
    MaterialHD *mat;
    if (idx < 0) {
      mat = nullptr;
    } else {
      mat = view<MaterialHD>((u32)md->mMaterials);
      if ((u32)idx < (u32)md->mMaterialNum)
        mat = element(mat, idx);
    }
    if (mat != 0) {
      u16 matIdx = mat->descriptor->index;
      ModelPacketsHD *model = view<ModelPacketsHD>((J3DModel *)mpModel);
      MaterialPacketHD *matPacket = nullptr;
      if (matIdx < model->count)
        matPacket = model->packets + matIdx;
      WordHD *packetFlags = packetFlagsSlot(matPacket);
      *packetFlags = *packetFlags & ~1u;
    }
    ModelFlagsHD *model = view<ModelFlagsHD>((J3DModel *)mpModel);
    u32 flags = model->flags & ~2u;
    model->flags = flags;
    itemBasePacketFlags(gabi::ea(model), flags);
  }

  J3DAnmTextureSRTKey *pbtk;
  mpBtkAnm1 = NULL;
  if (btkAnm1 != -1) {
    pbtk = (J3DAnmTextureSRTKey *)dComIfG_getObjectRes(resName, btkAnm1,
                                                       SAFESTRING_VTBL);
    if (pbtk == nullptr)
      JUT_ASSERT_fail(FILE_NAME, 0xC4, "pbtk != (0)");
    mpBtkAnm1 = itemBaseNewBtk(0); /* new mDoExt_btkAnm() */
    if (!mpBtkAnm1 ||
        !btkAnm_init(gabi::ea(mpBtkAnm1), modelData, pbtk, TRUE)) {
      return FALSE;
    }
  }

  mpBtkAnm2 = NULL;
  if (btkAnm2 != -1) {
    pbtk = (J3DAnmTextureSRTKey *)dComIfG_getObjectRes(resName, btkAnm2,
                                                       SAFESTRING_VTBL);
    if (pbtk == nullptr)
      JUT_ASSERT_fail(FILE_NAME, 0xD4, "pbtk != (0)");
    mpBtkAnm2 = itemBaseNewBtk(0);
    if (!mpBtkAnm2 ||
        !btkAnm_init(gabi::ea(mpBtkAnm2), modelData, pbtk, TRUE)) {
      return FALSE;
    }
  }

  J3DAnmTevRegKey *pbrk;
  mpBrkAnm1 = NULL;
  if (brkAnm1 != -1) {
    pbrk = (J3DAnmTevRegKey *)dComIfG_getObjectRes(resName, brkAnm1,
                                                   SAFESTRING_VTBL);
    if (pbrk == nullptr)
      JUT_ASSERT_fail(FILE_NAME, 0xE5, "pbrk != (0)");
    s8 tevFrm = (s8)itemBaseResources.items[m_itemNo].mTevFrm;
    BOOL shouldAnimate = TRUE;
    if (tevFrm != -1) {
      shouldAnimate = FALSE;
    }
    mpBrkAnm1 = itemBaseNewBrk(0); /* new mDoExt_brkAnm() */
    if (!mpBrkAnm1 ||
        !brkAnm_init(gabi::ea(mpBrkAnm1), modelData, pbrk, shouldAnimate)) {
      return FALSE;
    }
  }

  mpBrkAnm2 = NULL;
  if (brkAnm2 != -1) {
    pbrk = (J3DAnmTevRegKey *)dComIfG_getObjectRes(resName, brkAnm2,
                                                   SAFESTRING_VTBL);
    if (pbrk == nullptr)
      JUT_ASSERT_fail(FILE_NAME, 0xFE, "pbrk != (0)");
    mpBrkAnm2 = itemBaseNewBrk(0);
    if (!mpBrkAnm2 ||
        !brkAnm_init(gabi::ea(mpBrkAnm2), modelData, pbrk, TRUE)) {
      return FALSE;
    }
  }

  J3DAnmTransform *pbck;
  mpBckAnm = NULL;
  if (bckAnm != -1) {
    pbck = (J3DAnmTransform *)dComIfG_getObjectRes(resName, bckAnm,
                                                   SAFESTRING_VTBL);
    if (pbck == nullptr)
      JUT_ASSERT_fail(FILE_NAME, 0x10D, "pbck != (0)");
    u32 p = gabi::ea(operator_new(sizeof(BckAnmHD))); /* new mDoExt_bckAnm() */
    if (p != 0)
      bckAnm_ct(view<BckAnmHD>(p));
    mpBckAnm = view<mDoExt_bckAnm>(p);
    if (!mpBckAnm ||
        !mpBckAnm->init(modelData, pbck, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f,
                        0, -1, false)) {
      return FALSE;
    }
  }

  if (!gabi::call_ptr<BOOL>(vfn(daItemBase_VT_CLOTHCREATE), this)) {
    return FALSE;
  }

  return TRUE;
}

// HD: shadow rendering is handled outside the base item draw.
/* 021833D4 */
BOOL daItemBase_c::DrawBase() {
  gabi::call_ptr(vfn(daItemBase_VT_SETTEVSTR), this);
  gabi::call_ptr(vfn(daItemBase_VT_ANIMENTRY), this);
  gabi::call_ptr(vfn(daItemBase_VT_SETLISTSTART), this);
  gabi::call_ptr(vfn(daItemBase_VT_SETTINGBEFOREDRAW), this);

  mDoExt_modelUpdateDL(mpModel);

  if (mpModelArrow[0]) {
    mDoExt_modelUpdateDL(mpModelArrow[0]);
  }
  if (mpModelArrow[1]) {
    mDoExt_modelUpdateDL(mpModelArrow[1]);
  }

  setListEnd();
  return TRUE;
}

/* 02183488 */
void daItemBase_c::setListStart() {
  // HD: rupees have their own translucent draw list.
  if (isRupee(m_itemNo)) {
    itemBaseXluList = play()->xluRupee;
    return;
  }
  bool monotone = itemBaseMonotone != 0;
  PlayDrawListsHD *lists = play();
  if (!monotone) {
    itemBaseOpaList = lists->opaMaskOff;
    itemBaseXluList = play()->xluMaskOff;
  } else {
    itemBaseOpaList = lists->opaP1;
    itemBaseXluList = play()->xluP1;
  }
}

/* 02183390 */
void daItemBase_c::setListEnd() { dComIfGd_setList(); }

/* 02183520 */
void daItemBase_c::settingBeforeDraw() {
  if (isBomb(m_itemNo)) {
    PlayerBombHD *player = play()->player;
    daPy_bombBrk_setFrame0(player);
    J3DModelData *modelData = J3DModel_getModelData(mpModel);
    view<JointHD>((u32)modelData->mJointNodes)->mMtxCalc = 0;
  }

  if (m_itemNo == dItemNo_BOMB_BAG_e || m_itemNo == dItemNo_SKULL_HAMMER_e ||
      m_itemNo == dItemNo_SMALL_KEY_e) {
    itemBaseSpecular(&eyePos, &tevStr, mpModel, 1.0f);
  }
}

/* 021835F4 */
void daItemBase_c::setTevStr() {
  settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
  dScnKy_env_light_c *light = dKy_getEnvlight();
  setLightTevColorType(light, mpModel, &tevStr);

  for (int i = 0; i < 2; i++) {
    if (!mpModelArrow[i]) {
      continue;
    }
    light = dKy_getEnvlight();
    setLightTevColorType(light, mpModelArrow[i], &tevStr);
  }
}

/* 0218368C */
void daItemBase_c::animEntry() {
  if (mpBrkAnm1) {
    int constantFrame = (s8)itemBaseResources.items[m_itemNo].mTevFrm;
    J3DModelData *modelData = J3DModel_getModelData(mpModel);
    if (constantFrame != -1) {
      mDoExt_brkAnm_entry(mpBrkAnm1, modelData, (f32)constantFrame);
    } else {
      mDoExt_brkAnm_entry(mpBrkAnm1, modelData,
                          anm_frameCtrl(mpBrkAnm1)->getFrame());
    }
  }
  if (mpBtkAnm1) {
    mDoExt_btkAnm_entry(mpBtkAnm1, J3DModel_getModelData(mpModel),
                        anm_frameCtrl(mpBtkAnm1)->getFrame());
  }
  if (mpBrkAnm2) {
    mDoExt_brkAnm_entry(mpBrkAnm2, J3DModel_getModelData(mpModel),
                        anm_frameCtrl(mpBrkAnm2)->getFrame());
  }
  if (mpBtkAnm2) {
    mDoExt_btkAnm_entry(mpBtkAnm2, J3DModel_getModelData(mpModel),
                        anm_frameCtrl(mpBtkAnm2)->getFrame());
  }
  if (mpBckAnm) {
    mpBckAnm->entry(J3DModel_getModelData(mpModel),
                    anm_frameCtrl(mpBckAnm)->getFrame());
  }
}

/* 021837B0 */
void daItemBase_c::animPlay(f32 brk1Speed, f32 brk2Speed, f32 btk1Speed,
                            f32 btk2Speed, f32 bckSpeed) {
  if (mpBrkAnm1 && (s8)itemBaseResources.items[m_itemNo].mTevFrm == -1) {
    anm_frameCtrl(mpBrkAnm1)->setRate(brk1Speed);
    mDoExt_baseAnm_play(mpBrkAnm1);
  }

  if (mpBtkAnm1) {
    anm_frameCtrl(mpBtkAnm1)->setRate(btk1Speed);
    mDoExt_baseAnm_play(mpBtkAnm1);
  }

  if (mpBrkAnm2) {
    anm_frameCtrl(mpBrkAnm2)->setRate(brk2Speed);
    mDoExt_baseAnm_play(mpBrkAnm2);
  }

  if (mpBtkAnm2) {
    anm_frameCtrl(mpBtkAnm2)->setRate(btk2Speed);
    mDoExt_baseAnm_play(mpBtkAnm2);
  }

  if (mpBckAnm) {
    anm_frameCtrl(mpBckAnm)->setRate(bckSpeed);
    mDoExt_baseAnm_play(mpBckAnm);
  }
}

// HD: compiler-generated and template code of this translation unit.
/* 02184114 */
static void __sinit_d_a_itembase_cpp() {
  sinit_header_statics(gabi::ea(&itemBaseSinitState),
                       gabi::ea(&itemBaseSinitData));
}

/* 021841A8 */
static void SafeString_dt(void *p, s32 flags) {
  if (p != nullptr && (flags & 1))
    operator_delete(p);
}

/* 021841C4 */
static void SafeString_assureTerminationImpl(void *p) {}
/* WWHD glue (tools/verify/autoglue.py)
data itemBaseSafeStringVtable 10012054
data itemBaseMonotone 101F4829
data itemBaseOpaList 104B4634
data itemBaseXluList 104B4638
data itemBaseResourceManager 101F4F7C
data itemBaseBckVtable 1001206C
data itemBaseBckBaseVtable 1016E54C
data itemBaseBckHelperVtable 1016D820
data itemBaseSinitState 10464830
data itemBaseSinitData 101B7A90
data itemBaseResources 101E4674
ext 0252CA4C void itemBaseSpecular(cXyz* eye, dKy_tevstr_c* tev, J3DModel* model, f32 scale)
ext 027DF40C void itemBaseNormalizeFrame(gptr<item_base::FrameHelperHD>* helper)
ext 021841C4 void itemBaseAssureTermination(u32 string)
ext 027ED1F0 u32 itemBaseTextureName(u32 names, u32 index)
ext 026124B0 u32 itemBaseResourceFile(u32 manager, item_base::SafeStringHD* archive, item_base::SafeStringHD* file, int flags)
ext 027DFA24 u32 itemBaseTexture(u32 textures, const char* name)
ext 027DF9B0 s32 itemBaseMaterialIndex(u32 names, u32 name)
ext 027F596C void itemBasePacketFlags(u32 model, u32 flags)
ext 025E7C6C mDoExt_btkAnm* itemBaseNewBtk(int self)
ext 025E80D0 mDoExt_brkAnm* itemBaseNewBrk(int self)
ext 025E7CE0 BOOL itemBaseBtkInit(u32 anm, J3DModelData* data, void* key, s32 play, s32 mode, f32 speed, s16 start, s16 end, bool modify, s32 flags)
ext 025E8154 BOOL itemBaseBrkInit(u32 anm, J3DModelData* data, void* key, s32 play, s32 mode, f32 speed, s16 start, s16 end, bool modify, s32 flags)
ext 027F2BC0 void itemBaseFrameInit(u32 self, int frame)
ext 027DA984 void itemBaseFrameHelperInit(u8* helper)
str 10012128
str 1001213C
str 100120EC
str 100120C8
str 100120F8
str 10012104
str 10012110
str 100120D0
str 1001211C
str 100120DC
str 100120D4
*/
