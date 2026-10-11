/* Base item heap callbacks, collision dimensions and draw flags.
 * GameCube d_a_itembase_static.cpp on the HD daItemBase_c layout.
 * Verification retains each function and its native signature. */
#include "d/actor/d_a_itembase_private.h"
using namespace item_base;
static ItemResourcesHD itemBaseStaticResources;
static FieldItemResourcesHD itemBaseFieldResources;
static ItemInfosHD itemBaseInfos;
static WordHD itemBaseStaticSinitState;
static WordHD itemBaseStaticSinitData;
enum { DRAW_VISIBLE = 1, DRAW_DEAD = 2, DRAW_LOAD_ERROR = 4 };

/* 021841C8 */
u8 daItemBase_c::getItemNo() { return m_itemNo; }

/* 02184288 */
u8 daItemBase_c::getHeight() {
  return itemBaseInfos.items[m_itemNo].mCollisionH;
}

/* 021842A0 */
u8 daItemBase_c::getR() { return itemBaseInfos.items[m_itemNo].mCollisionR; }

/* 021842B8 */
void daItemBase_c::hide() { mDrawFlags = mDrawFlags & (u8)~DRAW_VISIBLE; }

/* 021842C8 */
void daItemBase_c::show() { mDrawFlags = mDrawFlags | DRAW_VISIBLE; }

/* 021842E4 */
void daItemBase_c::changeDraw() {
  if (chkDraw())
    hide();
  else
    show();
}

/* 021842D8 */
bool daItemBase_c::chkDraw() { return (mDrawFlags & DRAW_VISIBLE) != 0; }

/* 0218432C */
void daItemBase_c::dead() { mDrawFlags = mDrawFlags | DRAW_DEAD; }

/* 0218433C */
bool daItemBase_c::chkDead() { return (mDrawFlags & DRAW_DEAD) != 0; }

/* 02184350 */
void daItemBase_c::setLoadError() { mDrawFlags = mDrawFlags | DRAW_LOAD_ERROR; }

/* 021841D0 */
BOOL CheckItemCreateHeap(fopAc_ac_c *i_ac) {
  daItemBase_c *i_this = (daItemBase_c *)i_ac;
  u8 itemNo = i_this->getItemNo();
  dItem_data_item_resource *res = &itemBaseStaticResources.items[itemNo];
  return i_this->CreateItemHeap(res->mArcname, res->mBmdIdx, res->mSrtIdx,
                                res->mSrtIdx2, res->mTevIdx, res->mTevIdx2,
                                res->mBckIdx, -1);
}

/* 0218422C */
BOOL CheckFieldItemCreateHeap(fopAc_ac_c *i_ac) {
  daItemBase_c *i_this = (daItemBase_c *)i_ac;
  u8 itemNo = i_this->getItemNo();
  dItem_data_field_item_res *res = &itemBaseFieldResources.items[itemNo];
  return i_this->CreateItemHeap(res->mArc, res->mBmdIdx, res->mSrtIdx,
                                res->mSrtIdx2, res->mTevIdx, res->mTevIdx2,
                                res->mBckIdx, -1);
}

/* 02184360 */
static void __sinit_d_a_itembase_static_cpp() {
  sinit_header_statics(gabi::ea(&itemBaseStaticSinitState),
                       gabi::ea(&itemBaseStaticSinitData));
}
/* WWHD glue (tools/verify/autoglue.py)
data itemBaseStaticResources 101E4674
data itemBaseFieldResources 101E6A74
data itemBaseInfos 101E8674
data itemBaseStaticSinitState 1046484C
data itemBaseStaticSinitData 101B7AB4
*/
