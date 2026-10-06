/**
 * d_a_itembase_static.cpp (WWHD)
 * Item - Base Item Class (static part, linked into the main executable on GameCube)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_a_itembase_static.cpp) to the WWHD layout and code, verified against
 * cking.rpx. HD: the functions follow d_a_itembase.cpp (021841C8..021843F0), in a different
 * order (getItemNo, CheckItemCreateHeap, CheckFieldItemCreateHeap, getHeight, getR, hide, show,
 * chkDraw, changeDraw, dead, chkDead, setLoadError, __sinit).
 */
#include "d/actor/d_a_itembase.h"

/* 021841C8 */
u8 daItemBase_c::getItemNo() {
    WWHD_FUNC(0x021841C8, u8, this);
    return m_itemNo;
}
VERIFY(0x021841C8, &daItemBase_c::getItemNo);

/* 02184288 */
u8 daItemBase_c::getHeight() {
    WWHD_FUNC(0x02184288, u8, this);
    return dItem_data::getH(m_itemNo);
}
VERIFY(0x02184288, &daItemBase_c::getHeight);

/* 021842A0 */
u8 daItemBase_c::getR() {
    WWHD_FUNC(0x021842A0, u8, this);
    return dItem_data::getR(m_itemNo);
}
VERIFY(0x021842A0, &daItemBase_c::getR);

/* 021842B8 */
void daItemBase_c::hide() {
    WWHD_FUNC(0x021842B8, void, this);
    mDrawFlags = mDrawFlags & (u8)~0x01;
}
VERIFY(0x021842B8, &daItemBase_c::hide);

/* 021842C8 */
void daItemBase_c::show() {
    WWHD_FUNC(0x021842C8, void, this);
    mDrawFlags = mDrawFlags | 0x01;
}
VERIFY(0x021842C8, &daItemBase_c::show);

/* 021842E4 */
void daItemBase_c::changeDraw() {
    WWHD_FUNC(0x021842E4, void, this);
    if (chkDraw())
        hide();
    else
        show();
}
VERIFY(0x021842E4, &daItemBase_c::changeDraw);

/* 021842D8 */
bool daItemBase_c::chkDraw() {
    WWHD_FUNC(0x021842D8, bool, this);
    return (mDrawFlags & 0x01) != 0;
}
VERIFY(0x021842D8, &daItemBase_c::chkDraw);

/* 0218432C */
void daItemBase_c::dead() {
    WWHD_FUNC(0x0218432C, void, this);
    mDrawFlags = mDrawFlags | 0x02;
}
VERIFY(0x0218432C, &daItemBase_c::dead);

/* 0218433C (not named by the matcher) */
bool daItemBase_c::chkDead() {
    WWHD_FUNC(0x0218433C, bool, this);
    return (mDrawFlags & 0x02) != 0;
}
VERIFY(0x0218433C, &daItemBase_c::chkDead);

/* 02184350 */
void daItemBase_c::setLoadError() {
    WWHD_FUNC(0x02184350, void, this);
    mDrawFlags = mDrawFlags | 0x04;
}
VERIFY(0x02184350, &daItemBase_c::setLoadError);

/* 021841D0 (not named by the matcher) */
BOOL CheckItemCreateHeap(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x021841D0, BOOL, i_ac);
    daItemBase_c* i_this = (daItemBase_c*)i_ac;
    u8 itemNo = i_this->getItemNo();
    u32 res = dItem_data::item_resource(itemNo);
    return i_this->CreateItemHeap(
        gabi::at<const char>(gabi::load<u32>(res + 0x0)), /* getArcname */
        gabi::load<s16>(res + 0x8),                       /* getBmdIdx */
        gabi::load<s16>(res + 0xA),                       /* getSrtIdx */
        gabi::load<s16>(res + 0xC),                       /* getSrtIdx2 */
        gabi::load<s16>(res + 0xE),                       /* getTevIdx */
        gabi::load<s16>(res + 0x10),                      /* getTevIdx2 */
        gabi::load<s16>(res + 0x12),                      /* getBckIdx */
        -1
    );
}
VERIFY(0x021841D0, CheckItemCreateHeap);

/* 0218422C (not named by the matcher) */
BOOL CheckFieldItemCreateHeap(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x0218422C, BOOL, i_ac);
    daItemBase_c* i_this = (daItemBase_c*)i_ac;
    u8 itemNo = i_this->getItemNo();
    u32 res = dItem_data::field_item_res(itemNo);
    return i_this->CreateItemHeap(
        gabi::at<const char>(gabi::load<u32>(res + 0x0)), /* getFieldArc */
        gabi::load<s16>(res + 0x4),                       /* getFieldBmdIdx */
        gabi::load<s16>(res + 0x6),                       /* getFieldSrtIdx */
        gabi::load<s16>(res + 0x8),                       /* getFieldSrtIdx2 */
        gabi::load<s16>(res + 0xA),                       /* getFieldTevIdx */
        gabi::load<s16>(res + 0xC),                       /* getFieldTevIdx2 */
        gabi::load<s16>(res + 0xE),                       /* getFieldBckIdx */
        -1
    );
}
VERIFY(0x0218422C, CheckFieldItemCreateHeap);

/* 02184360 */
static void __sinit_d_a_itembase_static_cpp() {
    WWHD_FUNC(0x02184360, void, (u32)0);
    sinit_header_statics(0x1046484C, 0x101B7AB4);
}
VERIFY(0x02184360, __sinit_d_a_itembase_static_cpp);
