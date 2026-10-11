/* d_item part 3: the collect-map helper and item_func_collectmap61..01 (02550310..0255055B).
 * WWHD. See d_item.cpp for the unit's range.
 *
 * GHS merged the 61 HD map-handler bodies (dComIfGs_onGetCollectMap(no);
 * dComIfGs_offOpenCollectMap(no); dComIfGs_offCompleteCollectMap(no)) into one function
 * (02550310, not in the function tables); each item_func_collectmapNN tail-calls it with NN. */
#include "d/d_item_3.h"

// HD: GHS merged the 61 identical handlers; map bits are numbered from zero.
/* 02550310 */
void item_func_collectmap_common(s32 no) {
    itemCollectMap()->onGetMap((u32)no - 1);
    itemCollectMap()->offOpenMap((u32)no - 1);
    itemCollectMap()->offCompleteMap((u32)no - 1);
}

/* 02550374 */
void item_func_collectmap61() {
    item_func_collectmap_common(61);
}

/* 0255037C */
void item_func_collectmap60() {
    item_func_collectmap_common(60);
}

/* 02550384 */
void item_func_collectmap59() {
    item_func_collectmap_common(59);
}

/* 0255038C */
void item_func_collectmap58() {
    item_func_collectmap_common(58);
}

/* 02550394 */
void item_func_collectmap57() {
    item_func_collectmap_common(57);
}

/* 0255039C */
void item_func_collectmap56() {
    item_func_collectmap_common(56);
}

/* 025503A4 */
void item_func_collectmap55() {
    item_func_collectmap_common(55);
}

/* 025503AC */
void item_func_collectmap54() {
    item_func_collectmap_common(54);
}

/* 025503B4 */
void item_func_collectmap53() {
    item_func_collectmap_common(53);
}

/* 025503BC */
void item_func_collectmap52() {
    item_func_collectmap_common(52);
}

/* 025503C4 */
void item_func_collectmap51() {
    item_func_collectmap_common(51);
}

/* 025503CC */
void item_func_collectmap50() {
    item_func_collectmap_common(50);
}

/* 025503D4 */
void item_func_collectmap49() {
    item_func_collectmap_common(49);
}

/* 025503DC */
void item_func_collectmap48() {
    item_func_collectmap_common(48);
}

/* 025503E4 */
void item_func_collectmap47() {
    item_func_collectmap_common(47);
}

/* 025503EC */
void item_func_collectmap46() {
    item_func_collectmap_common(46);
}

/* 025503F4 */
void item_func_collectmap45() {
    item_func_collectmap_common(45);
}

/* 025503FC */
void item_func_collectmap44() {
    item_func_collectmap_common(44);
}

/* 02550404 */
void item_func_collectmap43() {
    item_func_collectmap_common(43);
}

/* 0255040C */
void item_func_collectmap42() {
    item_func_collectmap_common(42);
}

/* 02550414 */
void item_func_collectmap41() {
    item_func_collectmap_common(41);
}

/* 0255041C */
void item_func_collectmap40() {
    item_func_collectmap_common(40);
}

/* 02550424 */
void item_func_collectmap39() {
    item_func_collectmap_common(39);
}

/* 0255042C */
void item_func_collectmap38() {
    item_func_collectmap_common(38);
}

/* 02550434 */
void item_func_collectmap37() {
    item_func_collectmap_common(37);
}

/* 0255043C */
void item_func_collectmap36() {
    item_func_collectmap_common(36);
}

/* 02550444 */
void item_func_collectmap35() {
    item_func_collectmap_common(35);
}

/* 0255044C */
void item_func_collectmap34() {
    item_func_collectmap_common(34);
}

/* 02550454 */
void item_func_collectmap33() {
    item_func_collectmap_common(33);
}

/* 0255045C */
void item_func_collectmap32() {
    item_func_collectmap_common(32);
}

/* 02550464 */
void item_func_collectmap31() {
    item_func_collectmap_common(31);
}

/* 0255046C */
void item_func_collectmap30() {
    item_func_collectmap_common(30);
}

/* 02550474 */
void item_func_collectmap29() {
    item_func_collectmap_common(29);
}

/* 0255047C */
void item_func_collectmap28() {
    item_func_collectmap_common(28);
}

/* 02550484 */
void item_func_collectmap27() {
    item_func_collectmap_common(27);
}

/* 0255048C */
void item_func_collectmap26() {
    item_func_collectmap_common(26);
}

/* 02550494 */
void item_func_collectmap25() {
    item_func_collectmap_common(25);
}

/* 0255049C */
void item_func_collectmap24() {
    item_func_collectmap_common(24);
}

/* 025504A4 */
void item_func_collectmap23() {
    item_func_collectmap_common(23);
}

/* 025504AC */
void item_func_collectmap22() {
    item_func_collectmap_common(22);
}

/* 025504B4 */
void item_func_collectmap21() {
    item_func_collectmap_common(21);
}

/* 025504BC */
void item_func_collectmap20() {
    item_func_collectmap_common(20);
}

/* 025504C4 */
void item_func_collectmap19() {
    item_func_collectmap_common(19);
}

/* 025504CC */
void item_func_collectmap18() {
    item_func_collectmap_common(18);
}

/* 025504D4 */
void item_func_collectmap17() {
    item_func_collectmap_common(17);
}

/* 025504DC */
void item_func_collectmap16() {
    item_func_collectmap_common(16);
}

/* 025504E4 */
void item_func_collectmap15() {
    item_func_collectmap_common(15);
}

/* 025504EC */
void item_func_collectmap14() {
    item_func_collectmap_common(14);
}

/* 025504F4 */
void item_func_collectmap13() {
    item_func_collectmap_common(13);
}

/* 025504FC */
void item_func_collectmap12() {
    item_func_collectmap_common(12);
}

/* 02550504 */
void item_func_collectmap11() {
    item_func_collectmap_common(11);
}

/* 0255050C */
void item_func_collectmap10() {
    item_func_collectmap_common(10);
}

/* 02550514 */
void item_func_collectmap09() {
    item_func_collectmap_common(9);
}

// HD: map 08 also uses the common body; GC additionally clears its Triforce-map flag.
/* 0255051C */
void item_func_collectmap08() {
    item_func_collectmap_common(8);
}

/* 02550524 */
void item_func_collectmap07() {
    item_func_collectmap_common(7);
}

/* 0255052C */
void item_func_collectmap06() {
    item_func_collectmap_common(6);
}

/* 02550534 */
void item_func_collectmap05() {
    item_func_collectmap_common(5);
}

/* 0255053C */
void item_func_collectmap04() {
    item_func_collectmap_common(4);
}

/* 02550544 */
void item_func_collectmap03() {
    item_func_collectmap_common(3);
}

/* 0255054C */
void item_func_collectmap02() {
    item_func_collectmap_common(2);
}

/* 02550554 */
void item_func_collectmap01() {
    item_func_collectmap_common(1);
}
/* WWHD glue
ext 025B7EF4 void itemMap_onGetMap(dSv_player_map_c* map, s32 no)
ext 025B8054 void itemMap_offOpenMap(dSv_player_map_c* map, s32 no)
ext 025B81B4 void itemMap_offCompleteMap(dSv_player_map_c* map, s32 no)
*/
