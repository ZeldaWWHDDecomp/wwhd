/* d_save — Maps, player configuration, event flags and player initialization.
 * GameCube names with verified WWHD layouts and behavior. Unit: d_save. */
#include "bindings.h"
#include "d/d_save_readable.h"

namespace d_save_cpp {

#include "d/d_save_local.h"

static inline void setMapBit(u32 a, u32 no, const char* file, s32 line, const char* msg) {
    if (no >= 0x80) saveAssert(file, line, msg);
    be<u32>& flags = indexed(view<be<u32>>(a), (u32)((s32)no >> 5));
    flags = (u32)flags | (1u << (no & 31));
}
static inline void clearMapBit(u32 a, u32 no, const char* file, s32 line, const char* msg) {
    if (no >= 0x80) saveAssert(file, line, msg);
    be<u32>& flags = indexed(view<be<u32>>(a), (u32)((s32)no >> 5));
    flags = (u32)flags & ~(1u << (no & 31));
}
static inline BOOL testMapBit(u32 a, u32 no, const char* file, s32 line, const char* msg) {
    if (no >= 0x80) saveAssert(file, line, msg);
    be<u32>& flags = indexed(view<be<u32>>(a), (u32)((s32)no >> 5));
    return ((u32)flags & (1u << (no & 31))) ? TRUE : FALSE;
}

// HD: starts with Triforce charts 1, 3, 5, 6 and 7 already owned.
/* 025B7E74 */
void d_save_cpp::dSv_player_map_c::init() {
    for (u32 i = 0; i < 4; i++) {
        indexed(field_0x0[0], i) = 0;
        indexed(field_0x0[1], i) = 0;
        indexed(field_0x0[2], i) = 0;
        indexed(field_0x0[3], i) = 0;
    }
    for (u32 i = 0; i < 0x31; i++) indexed(mFmapBits, i) = 0;
    field_0x81 = 0xEA;
    indexed(mFmapBits, 0xA) = 3;
    indexed(mFmapBits, 0) = 3;
    indexed(mFmapBits, 0x2B) = 3;
    for (u32 i = 0; i < 0x10; i++) indexed(field_0x71, i) = 0;
}

/* 025B7EF4 */
void d_save_cpp::dSv_player_map_c::onGetMap(u32 no) {
    setMapBit(gabi::ea(field_0x0[1]), no, "d_save.cpp", 0x6C9, "0 <= i_no && i_no < 128");
}

/* 025B7F68 */
BOOL d_save_cpp::dSv_player_map_c::isGetMap(u32 no) {
    return testMapBit(gabi::ea(field_0x0[1]), no, "d_save.cpp", 0x6E5, "0 <= i_no && i_no < 128");
}

/* 025B7FE0 */
void d_save_cpp::dSv_player_map_c::onOpenMap(u32 no) {
    setMapBit(gabi::ea(field_0x0[2]), no, "d_save.cpp", 0x706, "0 <= i_no && i_no < 128");
}

/* 025B8054 */
void d_save_cpp::dSv_player_map_c::offOpenMap(u32 no) {
    clearMapBit(gabi::ea(field_0x0[2]), no, "d_save.cpp", 0x714, "0 <= i_no && i_no < 128");
}

/* 025B80C8 */
BOOL d_save_cpp::dSv_player_map_c::isOpenMap(u32 no) {
    return testMapBit(gabi::ea(field_0x0[2]), no, "d_save.cpp", 0x722, "0 <= i_no && i_no < 128");
}

/* 025B8140 */
void d_save_cpp::dSv_player_map_c::onCompleteMap(u32 no) {
    setMapBit(gabi::ea(field_0x0[3]), no, "d_save.cpp", 0x743, "0 <= i_no && i_no < 128");
}

/* 025B81B4 */
void d_save_cpp::dSv_player_map_c::offCompleteMap(u32 no) {
    clearMapBit(gabi::ea(field_0x0[3]), no, "d_save.cpp", 0x751, "0 <= i_no && i_no < 128");
}

/* 025B8228 */
BOOL d_save_cpp::dSv_player_map_c::isCompleteMap(u32 no) {
    return testMapBit(gabi::ea(field_0x0[3]), no, "d_save.cpp", 0x75F, "0 <= i_no && i_no < 128");
}

/* 025B82A0 */
void d_save_cpp::dSv_player_map_c::onTriforce(u32 no) {
    setByteBit(gabi::ea(&field_0x81), no, 8, "d_save.cpp", 0x781, "0 <= i_no && i_no < 8");
}

/* 025B8308 */
BOOL d_save_cpp::dSv_player_map_c::isTriforce(u32 no) {
    return testByteBit(gabi::ea(&field_0x81), no, 8, "d_save.cpp", 0x79D, "0 <= i_no && i_no < 8");
}

/* 025B8378 */
s32 d_save_cpp::dSv_player_map_c::getCollectMapNum() {
    s32 num = 0;
    for (s32 i = 1; i <= 61; i++) {
        if (i != 35 && i != 36 && i < 52 && isGetMap((u32)(i - 1))) num++;
    }
    return num;
}

/* 025B840C */
void d_save_cpp::dSv_player_map_c::onFmapBit(u32 idx, u32 no) {
    setByteBit(gabi::ea(&indexed(mFmapBits, idx)), no, 8, "d_save.cpp", 0x7D4, "0 <= i_no && i_no < 8");
}

/* 025B8484 */
void d_save_cpp::dSv_player_map_c::onSaveArriveGrid(u32 no) {
    if (no >= 0x31) saveAssert("d_save.cpp", 0x7FB, "(no >= 0) && (no < 49)");
    onFmapBit(no, 0);
}

/* 025B84E8 */
BOOL d_save_cpp::dSv_player_map_c::isFmapBit(u32 idx, u32 no) {
    return testByteBit(gabi::ea(&indexed(mFmapBits, idx)), no, 8, "d_save.cpp", 0x7F2, "0 <= i_no && i_no < 8");
}

/* 025B8568 */
BOOL d_save_cpp::dSv_player_map_c::isSaveArriveGrid(u32 no) {
    if (no >= 0x31) saveAssert("d_save.cpp", 0x809, "(no >= 0) && (no < 49)");
    return isFmapBit(no, 0);
}

/* 025B85CC */
void d_save_cpp::dSv_player_map_c::onSaveArriveGridForAgb(u32 no) {
    if (no >= 0x31) saveAssert("d_save.cpp", 0x810, "(no >= 0) && (no < 49)");
    onFmapBit(no, 1);
}

/* strncpy(dst, src, 17) (inline) */
static inline void copyPlayerName(u32 dst, u32 src) {
    u32 i = 0;
    for (; i < 0x11; i++) {
        u8 c = (u8)indexed(view<be<u8>>(src), i);
        indexed(view<be<u8>>(dst), i) = c;
        if (c == 0) break;
    }
    for (; i < 0x11; i++) indexed(view<be<u8>>(dst), i) = 0;
}

/* 025B8630 */
void d_save_cpp::dSv_player_info_c::init() {

    copyPlayerName(gabi::ea(mPlayerName), gabi::ea(l_defaultName));
    field_0x10 = 0;
    copyPlayerName(gabi::ea(field_0x25), gabi::ea(l_defaultName));
    copyPlayerName(gabi::ea(field_0x36), gabi::ea(l_defaultName));
    mDeathCount = 0;
    mClearCount = 0;
    for (u32 i = 0; i < 0x11; i++) indexed(mPuzzleData, i) = 0;
    u8 pt = (u8)gabi::ftoi(saveRandom(3.0f));
    if (pt >= 3) pt = 2;
    mRandomSalvagePoint = pt;
}

// HD: selects sound mode 2 without consulting OSGetSoundMode.
/* 025B8800 */
void d_save_cpp::dSv_player_config_c::init() {
    mRuby = 1;
    mSoundMode = 2;
    saveAudioOutput(2);
    mVibration = 1;
    mAttentionType = 0;
}

// HD: returns the play setting without the GameCube rumble availability check.
/* 025B8850 */
u8 d_save_cpp::dSv_player_config_c::checkVibration() {
    return (u8)view<HdVibrationView>(play())->mSetting;
}

/* 025B8874 */
void d_save_cpp::dSv_player_priest_c::init() {
    field_0xf = 0;
}

/* 025B8880 */
void d_save_cpp::dSv_player_priest_c::set(u32 a, const cXyz* pos, u32 angle, u32 roomNo) {
    field_0xf = (u8)a;
    wordPosition(&field_0x0).x = wordPosition(pos).x;
    wordPosition(&field_0x0).y = wordPosition(pos).y;
    u32 z = wordPosition(pos).z;
    field_0xe = (s8)roomNo;
    wordPosition(&field_0x0).z = z;
    field_0xc = (s16)angle;
}

// HD: initializes all seven components of the compact player-state copy.
/* 025B88A8 */
void d_save_cpp::dSv_player_status_c_c::init() {
    view<dSv_player_status_a_c>(gabi::ea(&mRecollectStatusA))->init();
    view<dSv_player_item_c>(gabi::ea(&mRecollectItem))->init();
    view<dSv_player_item_record2_c>(gabi::ea(&mRecollectItemRecord))->init();
    view<dSv_player_item_max2_c>(gabi::ea(&mRecollectItemMax))->init();
    view<dSv_player_bag_item_c>(gabi::ea(&mRecollectBagItem))->init();
    view<dSv_player_bag_item_record_c>(gabi::ea(&mRecollectBagItemRecord))->init();
    view<dSv_player_collect_c>(gabi::ea(&mRecollectCollect))->init();
}

/* 025B8904 */
void d_save_cpp::dSv_player_c::init() {
    view<dSv_player_status_a_c>(gabi::ea(&mPlayerStatusA))->init();
    view<dSv_player_status_b_c>(gabi::ea(&mPlayerStatusB))->init();
    view<dSv_player_return_place_c>(gabi::ea(&mReturnPlace))->init();
    view<dSv_player_item_c>(gabi::ea(&mPlayerItem))->init();
    view<dSv_player_get_item_c>(gabi::ea(&mGetItem))->init();
    view<dSv_player_item_record_c>(gabi::ea(&mItemRecord))->init();
    view<dSv_player_item_max_c>(gabi::ea(&mItemMax))->init();
    view<dSv_player_bag_item_c>(gabi::ea(&mBagItem))->init();
    view<dSv_player_get_bag_item_c>(gabi::ea(&mGetBagItem))->init();
    view<dSv_player_bag_item_record_c>(gabi::ea(&mBagItemRecord))->init();
    view<dSv_player_collect_c>(gabi::ea(&mCollect))->init();
    view<dSv_player_map_c>(gabi::ea(&mMap))->init();   /* mMap */
    view<dSv_player_info_c>(gabi::ea(&mInfo))->init(); /* mInfo */
    view<dSv_player_config_c>(gabi::ea(&mConfig))->init(); /* mConfig */
}

/* 025B8998 */
void d_save_cpp::dSv_ocean_c::init() {
    for (u32 i = 0; i < 50; i++) indexed(field_0x0, i) = 0;
}

/* 025B89B4 */
void d_save_cpp::dSv_ocean_c::onOceanSvBit(u32 grid, u32 bit) {
    if (grid > 0x31) saveAssert("d_save.cpp", 0xA96, "(0 <= i_grid) && (i_grid <= 0x31)");
    if (bit >= 0x10) saveAssert("d_save.cpp", 0xA97, "(0 <= i_bit) && (i_bit < 16)");
    be<u16>& flags = indexed(field_0x0, grid);
    flags = (u16)((u16)flags | slw(1, bit));
}

/* 025B8A50 */
BOOL d_save_cpp::dSv_ocean_c::isOceanSvBit(u32 grid, u32 bit) {
    if (grid > 0x31) saveAssert("d_save.cpp", 0xAB6, "(0 <= i_grid) && (i_grid <= 0x31)");
    if (bit >= 0x10) saveAssert("d_save.cpp", 0xAB7, "(0 <= i_bit) && (i_bit < 16)");
    return ((u16)indexed(field_0x0, grid) & (u16)slw(1, bit)) ? TRUE : FALSE;
}

static inline u32 eventByteIndex(u32 no) { return (u32)((s32)no >> 8); }

/* 025B8AF4 */
void d_save_cpp::dSv_event_c::setEventReg(u32 reg, u32 no) {
    be<u8>& flags = indexed(mFlags, eventByteIndex(reg));
    flags = (u8)(((u8)flags & ~(reg & 0xFF)) | no);
}

/* 025B8B10 */
void d_save_cpp::dSv_event_c::init() {
    for (u32 i = 0; i < 0x100; i++) indexed(mFlags, i) = 0;
    setEventReg(0xBEFF, (u8)l_kg1HighScore);
    setEventReg(0x7EFF, 0xE);
}

/* 025B8B68 */
void d_save_cpp::dSv_event_c::onEventBit(u32 no) {
    be<u8>& flags = indexed(mFlags, eventByteIndex(no));
    flags = (u8)((u8)flags | no);
}

/* 025B8B7C */
void d_save_cpp::dSv_event_c::offEventBit(u32 no) {
    be<u8>& flags = indexed(mFlags, eventByteIndex(no));
    flags = (u8)((u8)flags & ~(no & 0xFF));
}

/* 025B8B94 */
BOOL d_save_cpp::dSv_event_c::isEventBit(u32 no) {
    return ((u8)indexed(mFlags, eventByteIndex(no)) & (no & 0xFF)) ? TRUE : FALSE;
}

/* 025B8BB0 */
u8 d_save_cpp::dSv_event_c::getEventReg(u32 reg) {
    return (u8)((u8)indexed(mFlags, eventByteIndex(reg)) & reg);
}

}  // namespace d_save_cpp

/* WWHD glue
str 10053944 "d_save.cpp" in 025B7EF4
str 1005392C "0 <= i_no && i_no < 128" in 025B7EF4
str 1005396C "d_save.cpp" in 025B7F68
str 10053954 "0 <= i_no && i_no < 128" in 025B7F68
str 10053994 "d_save.cpp" in 025B7FE0
str 1005397C "0 <= i_no && i_no < 128" in 025B7FE0
str 100539B8 "d_save.cpp" in 025B8054
str 100539A0 "0 <= i_no && i_no < 128" in 025B8054
str 100539DC "d_save.cpp" in 025B80C8
str 100539C4 "0 <= i_no && i_no < 128" in 025B80C8
str 10053A04 "d_save.cpp" in 025B8140
str 100539EC "0 <= i_no && i_no < 128" in 025B8140
str 10053A28 "d_save.cpp" in 025B81B4
str 10053A10 "0 <= i_no && i_no < 128" in 025B81B4
str 10053A4C "d_save.cpp" in 025B8228
str 10053A34 "0 <= i_no && i_no < 128" in 025B8228
str 10053A74 "d_save.cpp" in 025B82A0
str 10053A5C "0 <= i_no && i_no < 8" in 025B82A0
str 10053A9C "d_save.cpp" in 025B8308
str 10053A84 "0 <= i_no && i_no < 8" in 025B8308
str 10053AC4 "d_save.cpp" in 025B840C
str 10053AAC "0 <= i_no && i_no < 8" in 025B840C
str 10053AD0 "d_save.cpp" in 025B8484
str 10053ADC "(no >= 0) && (no < 49)" in 025B8484
str 10053B14 "d_save.cpp" in 025B84E8
str 10053AFC "0 <= i_no && i_no < 8" in 025B84E8
str 10053B20 "d_save.cpp" in 025B8568
str 10053B2C "(no >= 0) && (no < 49)" in 025B8568
str 10053B44 "d_save.cpp" in 025B85CC
str 10053B50 "(no >= 0) && (no < 49)" in 025B85CC
str 10053B98 "d_save.cpp" in 025B89B4
str 10053B74 "(0 <= i_grid) && (i_grid <= 0x31)" in 025B89B4
str 10053BA4 "(0 <= i_bit) && (i_bit < 16)" in 025B89B4
str 10053BF0 "d_save.cpp" in 025B8A50
str 10053BCC "(0 <= i_grid) && (i_grid <= 0x31)" in 025B8A50
str 10053BFC "(0 <= i_bit) && (i_bit < 16)" in 025B8A50
*/
