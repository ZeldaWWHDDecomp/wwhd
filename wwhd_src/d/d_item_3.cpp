/* d_item part 3: the collect-map helper and item_func_collectmap61..01 (02550310..0255055B).
 * WWHD. See d_item.cpp for the unit's range.
 *
 * GHS merged the 61 identical GameCube bodies (dComIfGs_onGetCollectMap(no);
 * dComIfGs_offOpenCollectMap(no); dComIfGs_offCompleteCollectMap(no)) into one function
 * (02550310, not in the function tables); each item_func_collectmapNN tail-calls it with NN. */
#include "bindings.h"

namespace d_item_3_cpp {
#include "d_item_local.h"

/* 02550310 (new: GHS-merged body; the map bits are numbered from 0) */
void item_func_collectmap_common(s32 no) {
    WWHD_FUNC(0x02550310, void, no);
    dSv_map_onGetMap_l(svbase() + SV_MAP, no - 1);      /* dComIfGs_onGetCollectMap(no) */
    dSv_map_offOpenMap_l(svbase() + SV_MAP, no - 1);    /* dComIfGs_offOpenCollectMap(no) */
    dSv_map_offCompleteMap_l(svbase() + SV_MAP, no - 1); /* dComIfGs_offCompleteCollectMap(no) */
}
VERIFY(0x02550310, item_func_collectmap_common);

/* 02550374 */
void item_func_collectmap61() {
    WWHD_FUNC(0x02550374, void);
    item_func_collectmap_common(61);
}
VERIFY(0x02550374, item_func_collectmap61);

/* 0255037C */
void item_func_collectmap60() {
    WWHD_FUNC(0x0255037C, void);
    item_func_collectmap_common(60);
}
VERIFY(0x0255037C, item_func_collectmap60);

/* 02550384 */
void item_func_collectmap59() {
    WWHD_FUNC(0x02550384, void);
    item_func_collectmap_common(59);
}
VERIFY(0x02550384, item_func_collectmap59);

/* 0255038C */
void item_func_collectmap58() {
    WWHD_FUNC(0x0255038C, void);
    item_func_collectmap_common(58);
}
VERIFY(0x0255038C, item_func_collectmap58);

/* 02550394 */
void item_func_collectmap57() {
    WWHD_FUNC(0x02550394, void);
    item_func_collectmap_common(57);
}
VERIFY(0x02550394, item_func_collectmap57);

/* 0255039C */
void item_func_collectmap56() {
    WWHD_FUNC(0x0255039C, void);
    item_func_collectmap_common(56);
}
VERIFY(0x0255039C, item_func_collectmap56);

/* 025503A4 */
void item_func_collectmap55() {
    WWHD_FUNC(0x025503A4, void);
    item_func_collectmap_common(55);
}
VERIFY(0x025503A4, item_func_collectmap55);

/* 025503AC */
void item_func_collectmap54() {
    WWHD_FUNC(0x025503AC, void);
    item_func_collectmap_common(54);
}
VERIFY(0x025503AC, item_func_collectmap54);

/* 025503B4 */
void item_func_collectmap53() {
    WWHD_FUNC(0x025503B4, void);
    item_func_collectmap_common(53);
}
VERIFY(0x025503B4, item_func_collectmap53);

/* 025503BC */
void item_func_collectmap52() {
    WWHD_FUNC(0x025503BC, void);
    item_func_collectmap_common(52);
}
VERIFY(0x025503BC, item_func_collectmap52);

/* 025503C4 */
void item_func_collectmap51() {
    WWHD_FUNC(0x025503C4, void);
    item_func_collectmap_common(51);
}
VERIFY(0x025503C4, item_func_collectmap51);

/* 025503CC */
void item_func_collectmap50() {
    WWHD_FUNC(0x025503CC, void);
    item_func_collectmap_common(50);
}
VERIFY(0x025503CC, item_func_collectmap50);

/* 025503D4 */
void item_func_collectmap49() {
    WWHD_FUNC(0x025503D4, void);
    item_func_collectmap_common(49);
}
VERIFY(0x025503D4, item_func_collectmap49);

/* 025503DC */
void item_func_collectmap48() {
    WWHD_FUNC(0x025503DC, void);
    item_func_collectmap_common(48);
}
VERIFY(0x025503DC, item_func_collectmap48);

/* 025503E4 */
void item_func_collectmap47() {
    WWHD_FUNC(0x025503E4, void);
    item_func_collectmap_common(47);
}
VERIFY(0x025503E4, item_func_collectmap47);

/* 025503EC */
void item_func_collectmap46() {
    WWHD_FUNC(0x025503EC, void);
    item_func_collectmap_common(46);
}
VERIFY(0x025503EC, item_func_collectmap46);

/* 025503F4 */
void item_func_collectmap45() {
    WWHD_FUNC(0x025503F4, void);
    item_func_collectmap_common(45);
}
VERIFY(0x025503F4, item_func_collectmap45);

/* 025503FC */
void item_func_collectmap44() {
    WWHD_FUNC(0x025503FC, void);
    item_func_collectmap_common(44);
}
VERIFY(0x025503FC, item_func_collectmap44);

/* 02550404 */
void item_func_collectmap43() {
    WWHD_FUNC(0x02550404, void);
    item_func_collectmap_common(43);
}
VERIFY(0x02550404, item_func_collectmap43);

/* 0255040C */
void item_func_collectmap42() {
    WWHD_FUNC(0x0255040C, void);
    item_func_collectmap_common(42);
}
VERIFY(0x0255040C, item_func_collectmap42);

/* 02550414 */
void item_func_collectmap41() {
    WWHD_FUNC(0x02550414, void);
    item_func_collectmap_common(41);
}
VERIFY(0x02550414, item_func_collectmap41);

/* 0255041C */
void item_func_collectmap40() {
    WWHD_FUNC(0x0255041C, void);
    item_func_collectmap_common(40);
}
VERIFY(0x0255041C, item_func_collectmap40);

/* 02550424 */
void item_func_collectmap39() {
    WWHD_FUNC(0x02550424, void);
    item_func_collectmap_common(39);
}
VERIFY(0x02550424, item_func_collectmap39);

/* 0255042C */
void item_func_collectmap38() {
    WWHD_FUNC(0x0255042C, void);
    item_func_collectmap_common(38);
}
VERIFY(0x0255042C, item_func_collectmap38);

/* 02550434 */
void item_func_collectmap37() {
    WWHD_FUNC(0x02550434, void);
    item_func_collectmap_common(37);
}
VERIFY(0x02550434, item_func_collectmap37);

/* 0255043C */
void item_func_collectmap36() {
    WWHD_FUNC(0x0255043C, void);
    item_func_collectmap_common(36);
}
VERIFY(0x0255043C, item_func_collectmap36);

/* 02550444 */
void item_func_collectmap35() {
    WWHD_FUNC(0x02550444, void);
    item_func_collectmap_common(35);
}
VERIFY(0x02550444, item_func_collectmap35);

/* 0255044C */
void item_func_collectmap34() {
    WWHD_FUNC(0x0255044C, void);
    item_func_collectmap_common(34);
}
VERIFY(0x0255044C, item_func_collectmap34);

/* 02550454 */
void item_func_collectmap33() {
    WWHD_FUNC(0x02550454, void);
    item_func_collectmap_common(33);
}
VERIFY(0x02550454, item_func_collectmap33);

/* 0255045C */
void item_func_collectmap32() {
    WWHD_FUNC(0x0255045C, void);
    item_func_collectmap_common(32);
}
VERIFY(0x0255045C, item_func_collectmap32);

/* 02550464 */
void item_func_collectmap31() {
    WWHD_FUNC(0x02550464, void);
    item_func_collectmap_common(31);
}
VERIFY(0x02550464, item_func_collectmap31);

/* 0255046C */
void item_func_collectmap30() {
    WWHD_FUNC(0x0255046C, void);
    item_func_collectmap_common(30);
}
VERIFY(0x0255046C, item_func_collectmap30);

/* 02550474 */
void item_func_collectmap29() {
    WWHD_FUNC(0x02550474, void);
    item_func_collectmap_common(29);
}
VERIFY(0x02550474, item_func_collectmap29);

/* 0255047C */
void item_func_collectmap28() {
    WWHD_FUNC(0x0255047C, void);
    item_func_collectmap_common(28);
}
VERIFY(0x0255047C, item_func_collectmap28);

/* 02550484 */
void item_func_collectmap27() {
    WWHD_FUNC(0x02550484, void);
    item_func_collectmap_common(27);
}
VERIFY(0x02550484, item_func_collectmap27);

/* 0255048C */
void item_func_collectmap26() {
    WWHD_FUNC(0x0255048C, void);
    item_func_collectmap_common(26);
}
VERIFY(0x0255048C, item_func_collectmap26);

/* 02550494 */
void item_func_collectmap25() {
    WWHD_FUNC(0x02550494, void);
    item_func_collectmap_common(25);
}
VERIFY(0x02550494, item_func_collectmap25);

/* 0255049C */
void item_func_collectmap24() {
    WWHD_FUNC(0x0255049C, void);
    item_func_collectmap_common(24);
}
VERIFY(0x0255049C, item_func_collectmap24);

/* 025504A4 */
void item_func_collectmap23() {
    WWHD_FUNC(0x025504A4, void);
    item_func_collectmap_common(23);
}
VERIFY(0x025504A4, item_func_collectmap23);

/* 025504AC */
void item_func_collectmap22() {
    WWHD_FUNC(0x025504AC, void);
    item_func_collectmap_common(22);
}
VERIFY(0x025504AC, item_func_collectmap22);

/* 025504B4 */
void item_func_collectmap21() {
    WWHD_FUNC(0x025504B4, void);
    item_func_collectmap_common(21);
}
VERIFY(0x025504B4, item_func_collectmap21);

/* 025504BC */
void item_func_collectmap20() {
    WWHD_FUNC(0x025504BC, void);
    item_func_collectmap_common(20);
}
VERIFY(0x025504BC, item_func_collectmap20);

/* 025504C4 */
void item_func_collectmap19() {
    WWHD_FUNC(0x025504C4, void);
    item_func_collectmap_common(19);
}
VERIFY(0x025504C4, item_func_collectmap19);

/* 025504CC */
void item_func_collectmap18() {
    WWHD_FUNC(0x025504CC, void);
    item_func_collectmap_common(18);
}
VERIFY(0x025504CC, item_func_collectmap18);

/* 025504D4 */
void item_func_collectmap17() {
    WWHD_FUNC(0x025504D4, void);
    item_func_collectmap_common(17);
}
VERIFY(0x025504D4, item_func_collectmap17);

/* 025504DC */
void item_func_collectmap16() {
    WWHD_FUNC(0x025504DC, void);
    item_func_collectmap_common(16);
}
VERIFY(0x025504DC, item_func_collectmap16);

/* 025504E4 */
void item_func_collectmap15() {
    WWHD_FUNC(0x025504E4, void);
    item_func_collectmap_common(15);
}
VERIFY(0x025504E4, item_func_collectmap15);

/* 025504EC */
void item_func_collectmap14() {
    WWHD_FUNC(0x025504EC, void);
    item_func_collectmap_common(14);
}
VERIFY(0x025504EC, item_func_collectmap14);

/* 025504F4 */
void item_func_collectmap13() {
    WWHD_FUNC(0x025504F4, void);
    item_func_collectmap_common(13);
}
VERIFY(0x025504F4, item_func_collectmap13);

/* 025504FC */
void item_func_collectmap12() {
    WWHD_FUNC(0x025504FC, void);
    item_func_collectmap_common(12);
}
VERIFY(0x025504FC, item_func_collectmap12);

/* 02550504 */
void item_func_collectmap11() {
    WWHD_FUNC(0x02550504, void);
    item_func_collectmap_common(11);
}
VERIFY(0x02550504, item_func_collectmap11);

/* 0255050C */
void item_func_collectmap10() {
    WWHD_FUNC(0x0255050C, void);
    item_func_collectmap_common(10);
}
VERIFY(0x0255050C, item_func_collectmap10);

/* 02550514 */
void item_func_collectmap09() {
    WWHD_FUNC(0x02550514, void);
    item_func_collectmap_common(9);
}
VERIFY(0x02550514, item_func_collectmap09);

/* 0255051C */
void item_func_collectmap08() {
    WWHD_FUNC(0x0255051C, void);
    item_func_collectmap_common(8);
}
VERIFY(0x0255051C, item_func_collectmap08);

/* 02550524 */
void item_func_collectmap07() {
    WWHD_FUNC(0x02550524, void);
    item_func_collectmap_common(7);
}
VERIFY(0x02550524, item_func_collectmap07);

/* 0255052C */
void item_func_collectmap06() {
    WWHD_FUNC(0x0255052C, void);
    item_func_collectmap_common(6);
}
VERIFY(0x0255052C, item_func_collectmap06);

/* 02550534 */
void item_func_collectmap05() {
    WWHD_FUNC(0x02550534, void);
    item_func_collectmap_common(5);
}
VERIFY(0x02550534, item_func_collectmap05);

/* 0255053C */
void item_func_collectmap04() {
    WWHD_FUNC(0x0255053C, void);
    item_func_collectmap_common(4);
}
VERIFY(0x0255053C, item_func_collectmap04);

/* 02550544 */
void item_func_collectmap03() {
    WWHD_FUNC(0x02550544, void);
    item_func_collectmap_common(3);
}
VERIFY(0x02550544, item_func_collectmap03);

/* 0255054C */
void item_func_collectmap02() {
    WWHD_FUNC(0x0255054C, void);
    item_func_collectmap_common(2);
}
VERIFY(0x0255054C, item_func_collectmap02);

/* 02550554 */
void item_func_collectmap01() {
    WWHD_FUNC(0x02550554, void);
    item_func_collectmap_common(1);
}
VERIFY(0x02550554, item_func_collectmap01);

}  // namespace d_item_3_cpp
